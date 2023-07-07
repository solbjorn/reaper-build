static const half2 poisson_disc12[6] =
{
	half2(-0.326212f , -0.405810f),
	half2(-0.695914f ,  0.457137f),
	half2( 0.962340f , -0.194983f),
	half2( 0.519456f ,  0.767022f),
	half2( 0.507431f ,  0.064425f),
	half2(-0.321940f , -0.932615f),
};

//	Screen space ambient occlusion
//	P	screen space position of the original point
//	N	screen space normal of the original point
//	tc	G-buffer coordinates of the original point
half	calc_ssao( half3 P, half3 N, half2 tc)
{
	half point_depth = P.z;
	if (point_depth<0.01) point_depth = 100000.0h;	//	filter for the sky
	half2 	scale 	= half2	(.5f / 1024.h, .5f / 768.h)*150/max(point_depth,1.3);

	// sample
	half 	occ	= 0.1h;
	half num_dir	= 0.1h;

for (int a=1; a<3; ++a)
{
	half2	scale_tmp = scale*a;
	for (int i=0; i<6; i++)
	{
		half3 	dir 	= tex2D	(s_position,tc + poisson_disc12[i]*scale_tmp)-P.xyz;
		half 	occ_factor = saturate(length(dir));
		half 	infl 	= saturate( dot( normalize( dir ), N.xyz ) );

		{
			occ += (infl+0.01)*lerp( 1, occ_factor, infl)/(occ_factor+0.1);
			num_dir += (infl+0.01)/(occ_factor+0.1);
		}
	}
}
	occ /= num_dir;

	return occ;
}
