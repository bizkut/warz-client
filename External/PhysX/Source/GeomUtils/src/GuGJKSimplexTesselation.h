// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  


#ifndef PX_GJKSIMPLEX_TESSELATION_H
#define PX_GJKSIMPLEX_TESSELATION_H

#include "GuGJKSimplex.h"

namespace physx
{
namespace Gu
{

	PX_NOALIAS Ps::aos::Vec3V closestPtPointSegmentTesselation(const Ps::aos::Vec3VArg Q0, const Ps::aos::Vec3VArg Q1, const Ps::aos::Vec3VArg A0, const Ps::aos::Vec3VArg A1,
		const Ps::aos::Vec3VArg B0, const Ps::aos::Vec3VArg B1, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB);
	PX_NOALIAS Ps::aos::Vec3V closestPtPointTetrahedronTesselation(Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB);
	PX_NOALIAS Ps::aos::Vec3V closestPtPointTriangleTesselation(Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, PxU32* PX_RESTRICT indices, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB);
	
	PX_NOALIAS PX_FORCE_INLINE Ps::aos::Vec3V GJKCPairDoSimplexTesselation(Ps::aos::Vec3V* PX_RESTRICT Q, Ps::aos::Vec3V* PX_RESTRICT A, Ps::aos::Vec3V* PX_RESTRICT B, 
		const Ps::aos::Vec3VArg support, const Ps::aos::Vec3VArg supportA, const Ps::aos::Vec3VArg supportB, PxU32& size, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB)
	{
		using namespace Ps::aos;

		//const PxU32 tempSize = size;
		//calculate a closest from origin to the simplex
		switch(size)
		{
		case 1:
			{
				closestA = supportA;
				closestB = supportB;
				return support;
			}
		case 2:
			{
				return closestPtPointSegmentTesselation(Q[0], support, A[0], supportA, B[0], supportB, size, closestA, closestB);
			}
		case 3:
			{
		
				PxU32 tempIndices[3] ={0, 1, 2};
				return closestPtPointTriangleTesselation(Q, A, B, tempIndices, size, closestA, closestB);
			}
		case 4:
			{
				return closestPtPointTetrahedronTesselation(Q, A, B, size, closestA, closestB);
			}
		default:
			PX_ASSERT(0);
		}
		return support;
	}

}

}

#endif