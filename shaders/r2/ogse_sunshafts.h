/*  Volumetric Sunlights aka GodRays aka Sunshafts
	made by K.D. (OGSE team)
	used papers:
	1) GPU Gems 3. Volumetric Light Scattering as a Post-Process (Kenny Mitchell)
	2) ���������� ���������� �������� �������������. ����� 2 - �������� ����-��������. (�������� ������� �Nikola Tesla�)
	3) Crysis shaders :)*/

#ifndef	OGSE_SUNSHAFTS_H
	#define OGSE_SUNSHAFTS_H
	#include "common.h"
	#ifndef SS_QUALITY
		#define	SS_MAX_SAMPLES int(1)
	#else
		#if SS_QUALITY==1
			#define	SS_MAX_SAMPLES int(20)
		#else
			#if SS_QUALITY==2
				#define	SS_MAX_SAMPLES int(50)
			#else
				#if SS_QUALITY==3
					#define	SS_MAX_SAMPLES int(100)
				#endif
			#endif
		#endif
	#endif
	#define SS_FARPLANE float(180.0)
uniform float4 common_params2;
uniform sampler2D s_jitter_0;	// ����������� ���, 64x64
uniform sampler2D s_jitter_1;	// ����������� ���, 64x64
uniform sampler2D s_jitter_5;	// ��� �������� ����������, 1024x1024
	float normalize_depth(float depth)
	{
		return (saturate(depth/SS_FARPLANE));
	}
	 float3 blend_soft_light(float3 a, float3 b)
	{
		float3 c = 2 * a * b + a * a * (1 - 2 * b);
		float3 d = sqrt(a) * (2 * b - 1) + 2 * a * (1 - b);
		return ( b < 0.5 )? c : d;
	}
	float3 sunshafts(float2 tc, float3 init_color)
	{  
		// dist to the sun
		float sun_dist = SS_FARPLANE / (sqrt(1 - L_sun_dir_w.y * L_sun_dir_w.y));
		// sun pos
		float3 sun_pos_world = sun_dist*L_sun_dir_w + eye_position;
		float4 sun_pos_projected = mul(m_VP, float4(sun_pos_world, 1));
		float4 sun_pos_screen = convert_to_screen_space(sun_pos_projected)/sun_pos_projected.w;
		// sun-pixel vector
		float2 sun_vec_screen = sun_pos_screen.xy - tc;
		// calculate filtering effect. sunshafts must appear only when looking in the sun dir
		float angle_cos = dot(-eye_direction, normalize(L_sun_dir_w));
		float ray_fade = saturate(saturate(angle_cos)*(1 - saturate(dot(sun_vec_screen, sun_vec_screen)))*saturate(length(sun_vec_screen)));
		float2 init_tc = tc;
		
		float3 pos = tex2D(s_position, tc);
		float dep = pos.z;
		float depth = normalize_depth(dep);
		float mask = (saturate(angle_cos*0.1 + 0.9) + depth - 0.99999) * SS_BLEND_FACTOR;
		// adjust sampling
		sun_vec_screen *= angle_cos * SS_DENSITY / SS_MAX_SAMPLES;

		// sampling image along ray
		float3 accum = 0;
		float depth_accum = 0;
		for (int i = 0; i < SS_MAX_SAMPLES; i++)
		{ 
			tc += sun_vec_screen;
			depth = normalize_depth(tex2D(s_position, tc).z);
			accum += depth * (1 - i / SS_MAX_SAMPLES);
			depth_accum += saturate(1 - depth*1000);
		}
		accum *= saturate(depth_accum/SS_MAX_SAMPLES*5);
	#ifdef SS_DUST
		float dust_size = 8/SS_DUST_SIZE;
		float3 jit;
		float2 jtc = init_tc;
		float2 sun_dir_e = L_sun_dir_e.xy;
		sun_dir_e /= sin(common_params2.x);
		sun_dir_e *= common_params2.y;
		jtc.x += sun_dir_e.x;
		jtc.y -= sun_dir_e.y;
		jtc.x = (jtc.x > 1.0)?(jtc.x-1.0):jtc.x;
		jtc.y = (jtc.y < 0.0)?(1.0-jtc.y):jtc.y;
		jit.x = tex2D(s_jitter_0, float2(jtc.x, jtc.y + timers.x*0.01*SS_DUST_SPEED)*dust_size).x;
		jit.y = tex2D(s_jitter_1, float2(jtc.x + timers.x*0.01*SS_DUST_SPEED, jtc.y)*dust_size).y;
		jit.z = tex2D(s_jitter_5, jtc).x;
		jit.z = saturate(jit.z + SS_DUST_DENSITY - 1);
		float3 dust = saturate(float3(jit.x, jit.x, jit.x)*float3(jit.y, jit.y, jit.y)*float3(jit.z, jit.z, jit.z));
		float len = length(dust);
		dust *= SS_DUST_INTENSITY;
		dust = lerp(0, dust, (1 - saturate(dep * 0.2)) * (1 - saturate((0.001 - dep)*10000.0)));
		dust += float3(1,1,1);
	#else
		float3 dust = float3(1,1,1);
	#endif
		
		// watch some magic: if sun is occluded by terrain or some buildings, sunshafts must be eliminated. Otherwise image is terrible.
		accum *= 2 * ray_fade / SS_MAX_SAMPLES * dust;// * (1 - mat);
		// blending
		float3 out_color = init_color + accum * L_sun_color * (1.0 - init_color);
		out_color = blend_soft_light(out_color, L_sun_color * mask * 0.5 + 0.5) - init_color;
		return out_color;
	}  
	float3 calc_sunshafts(float2 tc, float3 init_color) {
		#ifndef	SS_QUALITY
			return 0;
		#else
			return sunshafts(tc, init_color);
		#endif	
	}
#endif