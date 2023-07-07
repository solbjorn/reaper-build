#ifndef	DOF_H_INCLUDED
#define	DOF_H_INCLUDED

#ifndef	ECB_DOF

half3	dof(float2 center, half3 sum)
{
	return	sum;
}

#else	//	ECB_DOF

#ifndef ecb_rd
	uniform half4 e_barrier;
	uniform half4 e_weights;
	uniform half4 e_kernel;
#endif

#include "blur.h"

half3	dof(float2 center, half3 sum)
{
	bool reload_dof = e_weights.z > 0.1f && e_weights.z < 0.2f;
	#ifdef ECB_DDOF_AIM
	bool aim_dof = e_weights.z > 0.2f && e_weights.z < 0.3f;
	#endif
	float dep = tex2D( s_position, center ).z;
	float targetDist = reload_dof ? 0.5f : tex2D( s_position, float2( 0.500001f, 0.500001f ) ).z;
	float mycof = ECB_DOF_MAXCOF;
	float myMINDIST = ECB_DOF_MINDIST;
	float myMAXDIST = ECB_DOF_MAXDIST;
	float myMAXNEAR = ECB_DOF_MAXNEAR;
	float myMINNEAR = ECB_DOF_MINNEAR;

	#ifdef ECB_DDOF

		if (targetDist < ECB_DOF_MAXDIST && targetDist > ECB_DDOF_MINDIST)
		{
			myMINDIST = reload_dof ? targetDist : targetDist + ECB_DDOF_MINDIST_ADD;
			float maxFactor = (targetDist < ECB_DDOF_MAXDIST) ? 1 : (targetDist / ECB_DDOF_MAXDIST);
			myMAXDIST = myMINDIST * ECB_DDOF_MAXDIST_FACTOR * maxFactor * 4;
		}

	#endif

	if (targetDist <= myMAXNEAR)
	{
		myMAXNEAR = targetDist;
	}

	if (dep < myMAXNEAR && dep > myMINNEAR) // && targetDist > myMAXNEAR)
	{
		dep = ( 1 - ( dep - myMINNEAR ) / ( myMAXNEAR - myMINNEAR ) ) * myMAXDIST;
		mycof = ECB_DOF_MAXCOF_NEAR;
	}

	half 	blur		= saturate( ( dep - myMINDIST ) / ( myMAXDIST - myMINDIST ) );

	half2 	scale = mycof * blur;

	#ifdef ECB_DDOF_AIM
	if (aim_dof)
		scale = saturate( pow( ( abs( center.x - 0.5f ) + abs( center.y - 0.5f ) ), ECB_DDOF_AIM_CIRCLE ) ) * saturate(100/tex2D( s_position, center ).z);
		#undef USE_MBLUR
	#endif

	return 	h_blur( center, scale * (aim_dof ? ECB_DDOF_AIM_BLUR : 0.5f) );
}

#endif	//	ECB_DOF

#endif	//	DOF_H_INCLUDED
