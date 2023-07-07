#ifdef OGSE_GODRAYS

/*  Volumetric Sunlights aka GodRays aka Sunshafts
	made by K.D. (OGSE team)
	used papers:
	1) GPU Gems 3. Volumetric Light Scattering as a Post-Process (Kenny Mitchell)
	2) Реализация процессора эффектов постобработки. Часть 2 - Создание пост-эффектов. (Владимир Дьячков «Nikola Tesla»)
	3) Crysis shaders :)*/

	#ifdef SS_QUALITY
		#define OGSE_GODRAYS_PRESET SS_QUALITY
	#endif

	#include "common.h"
	#if !defined( OGSE_GODRAYS_PRESET ) && !defined( OGSE_GODRAYS_SAMPLES ) // cjayho: use preset or use raw samples count
		#define	OGSE_GODRAYS_SAMPLES int(1)
	#else
		#if OGSE_GODRAYS_PRESET==1
			#define	OGSE_GODRAYS_SAMPLES int(20)
		#else
			#if OGSE_GODRAYS_PRESET==2
				#define	OGSE_GODRAYS_SAMPLES int(50)
			#else
				#if OGSE_GODRAYS_PRESET==3
					#define	OGSE_GODRAYS_SAMPLES int(100)
				#endif
			#endif
		#endif
	#endif

uniform float4 common_params2;

uniform sampler2D s_jitter_0;	// стандартный шум, 64x64
uniform sampler2D s_jitter_1;	// стандартный шум, 64x64
uniform sampler2D s_jitter_5;	// шум высокого разрешения, 1024x1024

	float normalize_depth(float depth)
	{
		return (saturate(depth/OGSE_GODRAYS_FARPLANE));
	}

	 float3 blend_soft_light(float3 a, float3 b)
	{
		float3 c = 2 * a * b + a * a * (1 - 2 * b);
		float3 d = sqrt(a) * (2 * b - 1) + 2 * a * (1 - b);
		return ( b < 0.5 )? c : d;
	}

	float4 godrays(float2 tc, float3 init_color)
	{
		// dist to the sun
		float sun_dist = OGSE_GODRAYS_FARPLANE / (sqrt(1 - L_sun_dir_w.y * L_sun_dir_w.y));
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
		float mask = (saturate(angle_cos*0.1 + 0.9) + depth - 0.99999) * OGSE_GODRAYS_BLEND_FACTOR;
		// adjust sampling
		sun_vec_screen *= angle_cos * OGSE_GODRAYS_DENSITY / OGSE_GODRAYS_SAMPLES;

		// sampling image along ray
		float3 accum = 0;
		float depth_accum = 0;
		for (int i = 0; i < OGSE_GODRAYS_SAMPLES; i++)
		{
			tc += sun_vec_screen;
			depth = normalize_depth(tex2D(s_position, tc).z);
			accum += depth * (1 - i / OGSE_GODRAYS_SAMPLES);
			depth_accum += saturate(1 - depth*10000);
		}
		accum *= saturate(depth_accum/OGSE_GODRAYS_SAMPLES*5);
	#ifdef OGSE_GODRAYS_DUST
		float dust_size = 8/OGSE_GODRAYS_DUST_SIZE;
		float3 jit;
		float2 jtc = init_tc;
		float2 sun_dir_e = L_sun_dir_e.xy;
		sun_dir_e /= sin(common_params2.x);
		sun_dir_e *= common_params2.y;
		jtc.x += sun_dir_e.x;
		jtc.y -= sun_dir_e.y;
		jtc.x = (jtc.x > 1.0)?(jtc.x-1.0):jtc.x;
		jtc.y = (jtc.y < 0.0)?(1.0-jtc.y):jtc.y;
		jit.x = tex2D(s_jitter_0, float2(jtc.x, jtc.y + timers.x*0.01*OGSE_GODRAYS_DUST_SPEED)*dust_size).x;
		jit.y = tex2D(s_jitter_1, float2(jtc.x + timers.x*0.01*OGSE_GODRAYS_DUST_SPEED, jtc.y)*dust_size).y;
		jit.z = tex2D(s_jitter_5, jtc).x;
		jit.z = saturate(jit.z + OGSE_GODRAYS_DUST_DENSITY - 1);
		float3 dust = saturate(float3(jit.x, jit.x, jit.x)*float3(jit.y, jit.y, jit.y)*float3(jit.z, jit.z, jit.z));
		float len = length(dust);
		dust *= OGSE_GODRAYS_DUST_INTENSITY;
		dust = lerp(0, dust, (1 - saturate(dep * 0.2)) * (1 - saturate((0.001 - dep)*10000.0)));
		dust += float3(1,1,1);
	#else
		float3 dust = float3(1,1,1);
	#endif

		// watch some magic: if sun is occluded by terrain or some buildings, sunshafts must be eliminated. Otherwise image is terrible.
		accum *= 2 * ray_fade / OGSE_GODRAYS_SAMPLES * dust;// * (1 - mat);
		// blending
		float3 out_color = init_color + accum * L_sun_color * (1.0 - init_color);
		out_color = blend_soft_light(out_color, L_sun_color * mask * 0.5 + 0.5) - init_color;
		return float4( out_color, 0 );
	}

#endif
