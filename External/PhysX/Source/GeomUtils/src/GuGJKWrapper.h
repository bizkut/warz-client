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


#ifndef PX_GJKWRAPPER_H
#define PX_GJKWRAPPER_H

#include "PxPhysXCommon.h"  // for PX_PHYSX_COMMON_API
#include "CmPhysXCommon.h"
#include "PsVecMath.h"
#include "PsVecTransform.h"
#include "GuGJKUtil.h"


/*
	This file is used to avoid the inner loop cross DLL calls
*/
namespace physx
{
namespace Gu
{
	
	class TriangleV;
	class CapsuleV;
	class BoxV;
	class ConvexHullV;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//													gjk
	//			
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*PX_PHYSX_COMMON_API PxGJKStatus GJK(const CapsuleV& a, const ConvexHullV& b, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJK(const BoxV& a, const ConvexHullV& b, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJK(const ConvexHullV& a, const ConvexHullV& b, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);

	PX_PHYSX_COMMON_API PxGJKStatus GJK(const TriangleV& a, const BoxV& b, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJK(const TriangleV& a, const ConvexHullV& b, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);*/

  
		
	PX_PHYSX_COMMON_API PxGJKStatus GJKRelative(const CapsuleV& a, const ConvexHullV& b, const Ps::aos::PsMatTransformV& aToB, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJKRelative(const CapsuleV& a, const BoxV& b, const Ps::aos::PsMatTransformV& aToB,  Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJKRelative(const BoxV& a,	const BoxV& b, const Ps::aos::PsMatTransformV& aToB,  Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJKRelative(const BoxV& a, const ConvexHullV& b, const Ps::aos::PsMatTransformV& aToB,  Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJKRelative(const ConvexHullV& a, const ConvexHullV& b, const Ps::aos::PsMatTransformV& aToB,  Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);

	PX_PHYSX_COMMON_API PxGJKStatus GJKRelative(const TriangleV& a, const BoxV& b, const Ps::aos::PsMatTransformV& aToB, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJKRelative(const TriangleV& a, const ConvexHullV& b, const Ps::aos::PsMatTransformV& aToB,  Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);


	//a is in the local space of b
	PX_PHYSX_COMMON_API PxGJKStatus GJKLocal(const CapsuleV& a, const ConvexHullV& b,  Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJKLocal(const TriangleV& a, const BoxV& b,  Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJKLocal(const TriangleV& a, const ConvexHullV& b,  Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);

	//GJK Tesselation, which will increase the accuracy of gjk code
	PX_PHYSX_COMMON_API PxGJKStatus GJKRelativeTesselation(const BoxV& a, const ConvexHullV& b, const Ps::aos::PsMatTransformV& aToB,  const Ps::aos::FloatVArg sqTolerance, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	PX_PHYSX_COMMON_API PxGJKStatus GJKRelativeTesselation(const ConvexHullV& a, const ConvexHullV& b, const Ps::aos::PsMatTransformV& aToB,  const Ps::aos::FloatVArg sqTolerance, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);

  
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//													gjk raycast
	//			
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//PX_PHYSX_COMMON_API bool GJKRayCast(TriangleV& a, BoxV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA);
	//PX_PHYSX_COMMON_API bool GJKRayCast(TriangleV& a, CapsuleV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA);
	//PX_PHYSX_COMMON_API bool GJKRayCast(TriangleV& a, ConvexHullV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA);
	//

	//PX_PHYSX_COMMON_API bool GJKRayCast(CapsuleV& a, CapsuleV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA);
	//PX_PHYSX_COMMON_API bool GJKRayCast(CapsuleV& a, BoxV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA);
	//PX_PHYSX_COMMON_API bool GJKRayCast(CapsuleV& a, ConvexHullV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA);

	//PX_PHYSX_COMMON_API bool GJKRayCast(BoxV& a, BoxV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA);
	//PX_PHYSX_COMMON_API bool GJKRayCast(BoxV& a, ConvexHullV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA);
	//PX_PHYSX_COMMON_API bool GJKRayCast(ConvexHullV& a, ConvexHullV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA);


	PX_PHYSX_COMMON_API bool GJKLocalRayCast(TriangleV& a, BoxV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal inflation = 0.f, const bool initialOverlap = true);
	PX_PHYSX_COMMON_API bool GJKLocalRayCast(TriangleV& a, ConvexHullV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal inflation = 0.f, const bool initialOverlap = true);
	PX_PHYSX_COMMON_API bool GJKLocalRayCast(CapsuleV& a, BoxV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal inflation = 0.f, const bool initialOverlap = true);
	PX_PHYSX_COMMON_API bool GJKLocalRayCast(CapsuleV& a, ConvexHullV& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal inflation = 0.f, const bool initialOverlap = true);
	  
	PX_PHYSX_COMMON_API bool GJKRelativeRayCast(TriangleV& a, BoxV& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal inflation = 0.f, const bool initialOverlap = true);
	PX_PHYSX_COMMON_API bool GJKRelativeRayCast(TriangleV& a, CapsuleV& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA,  const PxReal inflation = 0.f, const bool initialOverlap = true);
	PX_PHYSX_COMMON_API bool GJKRelativeRayCast(TriangleV& a, ConvexHullV& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA,  const PxReal inflation = 0.f, const bool initialOverlap = true);
	

	PX_PHYSX_COMMON_API bool GJKRelativeRayCast(CapsuleV& a, CapsuleV& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA,  const PxReal inflation = 0.f, const bool initialOverlap = true);
	PX_PHYSX_COMMON_API bool GJKRelativeRayCast(CapsuleV& a, BoxV& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA,  const PxReal inflation = 0.f, const bool initialOverlap = true);
	PX_PHYSX_COMMON_API bool GJKRelativeRayCast(CapsuleV& a, ConvexHullV& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA,  const PxReal inflation = 0.f, const bool initialOverlap = true);

	PX_PHYSX_COMMON_API bool GJKRelativeRayCast(BoxV& a, BoxV& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA,  const PxReal inflation = 0.f, const bool initialOverlap = true);
	PX_PHYSX_COMMON_API bool GJKRelativeRayCast(BoxV& a, ConvexHullV& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA,  const PxReal inflation = 0.f, const bool initialOverlap = true);
	PX_PHYSX_COMMON_API bool GJKRelativeRayCast(ConvexHullV& a, ConvexHullV& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA,  const PxReal inflation = 0.f, const bool initialOverlap = true);

}
}

#endif
