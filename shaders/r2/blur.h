#ifndef BLUR
#define BLUR

static const float2 blurcfg [12] = /* x, y in pixels */
{
	float2( -0.326212f, -0.405810f ),
	float2( -0.840144f, -0.073580f ),
	float2( -0.695914f,  0.457137f ),
	float2( -0.203345f,  0.620716f ),
	float2(  0.962340f, -0.194983f ),
	float2(  0.473434f, -0.480026f ),
	float2(  0.519456f,  0.767022f ),
	float2(  0.185461f, -0.893124f ),
	float2(  0.507431f,  0.064425f ),
	float2(  0.896420f,  0.412458f ),
	float2( -0.321940f, -0.932615f ),
	float2( -0.791559f, -0.597710f ),
};

static const half2 res = half2( 1.f / 1024.h, 1.f / 768.h );

float3 h_blur( float2 center, float blur )
{
	float d = blur;

	int mul = 1;
	int spls = 0;

	half3 sum = -tex2D( s_image, center );

	for ( int e = 1; e <= mul; e++ )
	{
		for ( int i = 0; i < 12; i++ )
		{
			sum += tex2D( s_image, center + ( blurcfg[i].xy * res * e * ( d/mul ) ) );
			spls++;
		}
	}

	return half3( sum / spls );
}

#endif // BLUR
