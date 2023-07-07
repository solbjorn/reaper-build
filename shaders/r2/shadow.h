#ifndef	SHADOW_H
#define SHADOW_H

#include "common.h"

uniform sampler2D jitter0;
#define JITTER_SAMPLES int(16)

/*
Modded by cj ayho (ecb team). 29.08.2010
цель модификации - реализация мягких теней.
*/

//////////////////////////////////////////////////////////////////////////////////////////
// software
//////////////////////////////////////////////////////////////////////////////////////////
half 	sample_sw	(float2 tc, float2 shift, float depth_cmp)
{
	static const float 	ts = ECB_SHADOW_KERNEL / float(SMAP_size);
	tc 		+= 		shift*ts;

	float  	texsize = SMAP_size;
	float  	offset 	= 0.5f/texsize;
	float2 	Tex00 	= tc + float2(-offset, -offset);
	float2 	Tex01 	= tc + float2(-offset,  offset);
	float2 	Tex10 	= tc + float2( offset, -offset);
	float2 	Tex11 	= tc + float2( offset,  offset);
	float4 	depth 	= float4(
		depth_cmp-tex2D	(s_smap, Tex00).x,
		depth_cmp-tex2D	(s_smap, Tex01).x,
		depth_cmp-tex2D	(s_smap, Tex10).x,
		depth_cmp-tex2D	(s_smap, Tex11).x);
	half4 	compare = step	(depth,0);
	float2 	fr 		= frac	(Tex00*texsize);
	half2 	ifr 	= half2	(1,1) - fr;
	half4 	fr4 	= half4	(ifr.x*ifr.y, ifr.x*fr.y, fr.x*ifr.y,  fr.x*fr.y);
	return	dot		(compare, fr4);
}
half 	shadow_sw	(float4 tc)	{
	float2	tc_dw	= tc.xy / tc.w;
	half4	s;
	s.x	= sample_sw	(tc_dw,float2(-1,-1),tc.z);
	s.y	= sample_sw	(tc_dw,float2(+1,-1),tc.z);
	s.z	= sample_sw	(tc_dw,float2(-1,+1),tc.z);
	s.w	= sample_sw	(tc_dw,float2(+1,+1),tc.z);
	return	dot		(s, 1.h/4.h);
}

//////////////////////////////////////////////////////////////////////////////////////////
// hardware + PCF
//////////////////////////////////////////////////////////////////////////////////////////
half  	sample_hw_pcf	(float4 tc,float4 shift){
	static const float 	ts = ECB_SHADOW_KERNEL / float(SMAP_size);

	float4 tc2 = tc / tc.w + shift * ts;
	tc2.w = 0;
	return tex2Dlod(s_smap, tc2);

}
half 	shadow_hw	( float4 tc )
{

/*
Modded by cj ayho (ecb team). 29.08.2010. цель модификации - реализация мягких теней.
version 1.1 by cj ayho (ecb team) 13.04.2011 - оптимизация

<mod>
*/

  half4 s0;

  int n = ECB_SHADOW_STEPS;

	for( int i = 0; i < n; i++ )
	{

		float3 a1 = float3( 0, +i, -i );
		float3 a2 = float3( 0, +i + 1, -i - 1 );
		float3 a3 = float3( 0, +i + 2, -i - 2 );

		s0.x += sample_hw_pcf ( tc, a1.zzxx );
		s0.y += sample_hw_pcf ( tc, a1.yzxx );
		s0.z += sample_hw_pcf ( tc, a1.zyxx );
		s0.w += sample_hw_pcf ( tc, a1.yyxx );

		s0.x += sample_hw_pcf ( tc, a1.xyxx );
		s0.y += sample_hw_pcf ( tc, a1.xzxx );
		s0.z += sample_hw_pcf ( tc, a1.yxxx );
		s0.w += sample_hw_pcf ( tc, a1.zxxx );

		s0.x += sample_hw_pcf ( tc, a2.zzxx );
		s0.y += sample_hw_pcf ( tc, a2.yzxx );
		s0.z += sample_hw_pcf ( tc, a2.zyxx );
		s0.w += sample_hw_pcf ( tc, a2.yyxx );

		s0.x += sample_hw_pcf ( tc, a2.xyxx );
		s0.y += sample_hw_pcf ( tc, a2.xzxx );
		s0.z += sample_hw_pcf ( tc, a2.yxxx );
		s0.w += sample_hw_pcf ( tc, a2.zxxx );

		s0.x += sample_hw_pcf ( tc, a3.zzxx );
		s0.y += sample_hw_pcf ( tc, a3.yzxx );
		s0.z += sample_hw_pcf ( tc, a3.zyxx );
		s0.w += sample_hw_pcf ( tc, a3.yyxx );

		s0.x += sample_hw_pcf ( tc, a3.xyxx );
		s0.y += sample_hw_pcf ( tc, a3.xzxx );
		s0.z += sample_hw_pcf ( tc, a3.yxxx );
		s0.w += sample_hw_pcf ( tc, a3.zxxx );

	}

	return dot ( s0, 1.f / ( 24 * n ) );
/*
</mod>
*/

}

half 	shadow_nb	( float4 tc )
{
	return sample_hw_pcf ( tc, float4( 0, 0, 0, 0 ) );
}

//////////////////////////////////////////////////////////////////////////////////////////
// hardware (ATI) + DF24/Fetch4
//////////////////////////////////////////////////////////////////////////////////////////

half  	sample_hw_f4	(float4 tc,float4 shift){
	static const float 	ts 	= 	ECB_SHADOW_KERNEL / 	float(SMAP_size);
	float4 	T4				= 	tc/tc.w		;
			T4.xy			+=	shift.xy*ts	;

	float4	D4				= 	tex2D		(s_smap, T4);
	float4	compare			= 	T4.z<D4		;

	float  	texsize 		= 	SMAP_size	;
	float2 	fr 				= 	frac		(T4.xy * texsize);
	half2 	ifr 			= 	half2		(1,1) - fr;
	half4 	fr4 			= 	half4		(ifr.x*ifr.y, ifr.x*fr.y, fr.x*ifr.y,  fr.x*fr.y);
	half4 	fr4s		 	= 	fr4.zywx	;

	return	dot	(compare, fr4s)	;
}

half 	shadow_hw_f4	(float4 tc)		{
  half  s0	= half4( sample_hw_f4	(tc,float4(-1,-1,0,0)), sample_hw_f4	(tc,float4(+1,-1,0,0)), sample_hw_f4	(tc,float4(-1,+1,0,0)), sample_hw_f4	(tc,float4(+1,+1,0,0)) );
  return	dot( s0, 1.h/4.h );
}

//////////////////////////////////////////////////////////////////////////////////////////
// select hardware or software shadowmaps
//////////////////////////////////////////////////////////////////////////////////////////
#ifdef	USE_HWSMAP_PCF
	// D24X8+PCF
	half 	shadow		(float4	tc)		{ return shadow_hw	(tc);	}
#else
	#ifdef USE_FETCH4
		// DF24+Fetch4
		half 	shadow 		(float4 tc)		{ return shadow_hw_f4(tc);	}
	#else
		// FP32
		half 	shadow		(float4 tc) 	{ return shadow_sw	(tc);	}
	#endif
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// testbed

float shadowtest(float4 tc,float4 tcJ)
{
	const float4 jitter = tex2Dproj(jitter0, tcJ) * tc.w *(ECB_SHADOW_KERNEL/float(SMAP_size));

	return (tex2Dproj(s_smap, float4(tc.xy + jitter.xy, tc.zw)).x  +
		    tex2Dproj(s_smap, float4(tc.xy + jitter.zw, tc.zw)).x  +
		    tex2Dproj(s_smap, float4(tc.xy - jitter.xy, tc.zw)).x  +
		    tex2Dproj(s_smap, float4(tc.xy - jitter.zw, tc.zw)).x) * (1/4.h);
}

float shadowtest_sun(float4 tc, float4 tcJ)
{
	const float offset = 1.h + (8 / float(JITTER_SAMPLES));
	float shadow = 0;

	for (int i = JITTER_SAMPLES / 8; i > 0; --i)
	{
		float4 jitter = tex2D(jitter0, tcJ.xy) * tc.w * (ECB_SHADOW_KERNEL/float(SMAP_size)) ;

		shadow += tex2Dproj(s_smap, float4(tc.xy + jitter.xy, tc.zw)).x +
				  tex2Dproj(s_smap, float4(tc.xy + jitter.zw, tc.zw)).x +
				  tex2Dproj(s_smap, float4(tc.xy - jitter.xy, tc.zw)).x +
				  tex2Dproj(s_smap, float4(tc.xy - jitter.zw, tc.zw)).x +
				  tex2Dproj(s_smap, float4(tc.xy + jitter.yx, tc.zw)).x +
				  tex2Dproj(s_smap, float4(tc.xy + jitter.wz, tc.zw)).x +
				  tex2Dproj(s_smap, float4(tc.xy - jitter.yx, tc.zw)).x +
				  tex2Dproj(s_smap, float4(tc.xy - jitter.wz, tc.zw)).x;

		tcJ.xy *= offset;
	}
	return shadow * (1 / float(JITTER_SAMPLES));
}

//////////////////////////////////////////////////////////////////////////////////////////
#ifdef  USE_SUNMASK
uniform float3x4	m_sunmask	;				// ortho-projection
half 	sunmask		(float4 P)	{				//
	float2 		tc	= mul	(m_sunmask, P);		//
	return 		tex2D 		(s_lmap,tc).w;		// A8

}
#else
half 	sunmask		(float4 P)	{ return 1.h; }	//
#endif

//////////////////////////////////////////////////////////////////////////////////////////
uniform float4x4	m_shadow;

#endif
