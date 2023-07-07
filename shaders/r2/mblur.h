#ifndef	MBLUR_H
#define MBLUR_H

	#include "dof.h"

	#if !defined( USE_MBLUR )
		half3 	mblur	(float2 UV, half3 pos, half3 c_original)	{ return dof( UV, c_original ) ; }
	#else

		#include "common.h"

		uniform half4x4	m_current;
		uniform half4x4	m_previous;
		uniform half2 	m_blur;		// scale_x / 12, scale_y / 12

		half3 	mblur	(float2 UV, half3 pos, half3 c_original)
		{
			pos.z += 1000000.h*saturate(0.001 - pos.z);	// фикс дрожащего неба (K.D.)
			half4 	pos4		= half4	(pos,0.01h);	// фикс каймы по краям геометрии (Scavenger)
			half4 	p_current	= mul	(m_current,	pos4);
			half4 	p_previous 	= mul	(m_previous,	pos4);

			half2 	p_velocity 	= ( m_blur * ( (p_current.xy/p_current.w)-(p_previous.xy/p_previous.w) ) )/3;

			  half4 blurred 	= 	(half4) 0; //tex2D(s_image, p_velocity + UV);
					blurred		+= 	tex2D(s_image, p_velocity * 2.h  + UV);
					blurred		+= 	tex2D(s_image, p_velocity * 3.h  + UV);
					blurred		+= 	tex2D(s_image, p_velocity * 4.h  + UV);
					blurred 	+= 	tex2D(s_image, p_velocity * 5.h  + UV);
					blurred		+= 	tex2D(s_image, p_velocity * 6.h  + UV);
					blurred		+= 	tex2D(s_image, p_velocity * 7.h  + UV);
					blurred		+= 	tex2D(s_image, p_velocity * 8.h  + UV);
					blurred 	+= 	tex2D(s_image, p_velocity * 9.h  + UV);
					blurred		+= 	tex2D(s_image, p_velocity * 10.h + UV);
					blurred		+= 	tex2D(s_image, p_velocity * 11.h + UV);

				blurred /= 10;

			return 	((dof( UV, (half3) c_original ) + blurred.rgb)/2);
		}

	#endif

#endif
