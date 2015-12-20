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


#ifndef PX_PHYSICS_GEOMUTILS_VEC_CONVEX_H
#define PX_PHYSICS_GEOMUTILS_VEC_CONVEX_H

#include "CmPhysXCommon.h"
#include "PsVecMath.h"

namespace physx
{
namespace Gu
{
//to avoid LHS
#define E_CONVEXTYPE PxU32
#define E_CONVEXHULL 0
#define	E_CONVEXHULLNOSCALE	1
#define E_SPHERE 2
#define E_BOX 3
#define E_CAPSULE 4
#define E_CONE 5
#define E_CYLINDER 6
#define E_TRIANGLE 7
	/*enum E_CONVEXTYPE
	{
		E_CONVEXHULL,
		E_SPHERE,
		E_BOX,
		E_CAPSULE,
		E_CONE,
		E_CYLINDER,
		E_TRIANGLE
	};*/

	class ConvexV
	{
	public:

		PX_FORCE_INLINE ConvexV(const E_CONVEXTYPE _type) : type(_type)
		{
			using namespace Ps::aos;
			const FloatV zero = FZero();
			margin = zero;
			minMargin = zero;
#ifdef __SPU__
			bMarginIsRadius = false;
#endif
		}

		PX_FORCE_INLINE ConvexV(const E_CONVEXTYPE _type, const Ps::aos::Vec3VArg _center) 
		{
			using namespace Ps::aos;
			const FloatV zero = FZero();
			center = _center;
			type = _type;
			margin = zero;
			minMargin = zero;
#ifdef __SPU__
			bMarginIsRadius = false;
#endif
		}

		PX_FORCE_INLINE ConvexV(const E_CONVEXTYPE _type, const Ps::aos::Vec3VArg _center, const Ps::aos::FloatVArg _margin)
		{
			using namespace Ps::aos;
			const FloatV ratio = Ps::aos::FloatV_From_F32(0.25f);
			minMargin = FMul(_margin, ratio);
			center = _center;
			margin = _margin;
			type = _type;
#ifdef __SPU__
			bMarginIsRadius = false;
#endif
		}

		PX_FORCE_INLINE ConvexV(const E_CONVEXTYPE _type, const Ps::aos::Vec3VArg _center, const Ps::aos::FloatVArg _margin, const Ps::aos::FloatVArg _minMargin)
		{
			using namespace Ps::aos;
			minMargin = _minMargin;
			center = _center;
			margin = _margin;
			type = _type;
#ifdef __SPU__
			bMarginIsRadius = false;
#endif
		}

		//everytime when someone transform the object, they need to up
		void setCenter(const Ps::aos::Vec3VArg _center)
		{
			center = _center;
		}

		void setMargin(const PxF32 _margin)
		{
			margin = Ps::aos::FloatV_From_F32(_margin);
		}

		void setMargin(const Ps::aos::FloatVArg _margin)
		{
			margin = _margin;
		}


		PX_FORCE_INLINE Ps::aos::Vec3V getCenter()const 
		{
			return center;
		}

		PX_FORCE_INLINE Ps::aos::FloatV getMargin() const
		{
			return margin;
		}

		PX_FORCE_INLINE Ps::aos::FloatV getMinMargin() const
		{
			return minMargin;
		}

		PX_FORCE_INLINE Ps::aos::FloatV getSweepMargin() const
		{
			return minMargin;
		}


		void getMargin(PxF32& _marginF) const
		{
			Ps::aos::PxF32_From_FloatV(margin, &_marginF);
		}


		E_CONVEXTYPE getType() const
		{
			return type;
		}

		PX_FORCE_INLINE bool isMarginEqRadius()const
		{
#ifdef __SPU__
			return bMarginIsRadius;
#else
			return false;
#endif
		}
#ifndef __SPU__
		PX_FORCE_INLINE Ps::aos::BoolV isMarginEqRadiusV()const
		{
			return Ps::aos::BFFFF();
		}
#endif

	protected:
		Ps::aos::Vec3V center;
		Ps::aos::FloatV margin;
		Ps::aos::FloatV minMargin;
		E_CONVEXTYPE	type;
#ifdef __SPU__
		bool bMarginIsRadius;
#endif
	};

	PX_FORCE_INLINE Ps::aos::FloatV getContactEps(const Ps::aos::FloatV& _marginA, const Ps::aos::FloatV& _marginB)
	{
		using namespace Ps::aos;

		const FloatV ratio = FloatV_From_F32(0.25f);
		const FloatV minMargin = FMin(_marginA, _marginB);
		
		return FMul(minMargin, ratio);
	}

	PX_FORCE_INLINE Ps::aos::FloatV getSweepContactEps(const Ps::aos::FloatV& _marginA, const Ps::aos::FloatV& _marginB)
	{
		using namespace Ps::aos;

		const FloatV ratio = FloatV_From_F32(100.f);
		const FloatV minMargin = FAdd(_marginA, _marginB);
		
		return FMul(minMargin, ratio);
	}
}

}

#endif