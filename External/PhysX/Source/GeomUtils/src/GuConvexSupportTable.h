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

#ifndef GU_CONVEX_SUPPORT_TABLE_H
#define GU_CONVEX_SUPPORT_TABLE_H

#include "GuVecConvex.h"
#include "PsVecTransform.h"

namespace physx
{
namespace Gu
{

	class TriangleV; 
	class CapsuleV;
	class BoxV;
	class ConvexHullV;

	class SupportMap
	{
	public:
		virtual ~SupportMap(){}
		virtual void doWarmStartSupport(const PxI32 index, Ps::aos::Vec3V& point) const = 0;
		virtual Ps::aos::Vec3V doSupport(const Ps::aos::Vec3VArg dir) const = 0;
		//virtual Ps::aos::Vec3V doSupport(const Ps::aos::Vec3VArg dir, PxI32& index) = 0;
		virtual Ps::aos::Vec3V doWarmStartSupportMargin(const PxI32 index) const = 0;
		virtual Ps::aos::Vec3V doSupportMargin(const Ps::aos::Vec3VArg dir, Ps::aos::Vec3V& support, PxI32& index) const = 0;
		virtual Ps::aos::Vec3V doSupportSweep(const Ps::aos::Vec3VArg dir) const = 0;
	};

	template <typename Convex>
	class SupportMapRelativeImpl : public SupportMap
	{
		const Convex& conv;
		const Ps::aos::PsMatTransformV& aToB;
	public:
		SupportMapRelativeImpl(const Convex& _conv, const Ps::aos::PsMatTransformV& _aToB) : conv(_conv), aToB(_aToB)
		{
		}

		void doWarmStartSupport(const PxI32 index, Ps::aos::Vec3V& point) const
		{
			point = aToB.transform(conv.supportPoint(index));
		}

		Ps::aos::Vec3V doSupport(const Ps::aos::Vec3VArg dir) const
		{
			return conv.supportRelative(dir, aToB);
		}

	/*	Ps::aos::Vec3V doSupport(const Ps::aos::Vec3VArg dir, PxI32& index)
		{
			return conv.supportRelative(dir, aToB, index);
		}*/


		Ps::aos::Vec3V doWarmStartSupportMargin(const PxI32 index) const
		{
			return aToB.transform(conv.supportMarginPoint(index));
		}

		Ps::aos::Vec3V doSupportMargin(const Ps::aos::Vec3VArg dir, Ps::aos::Vec3V& support, PxI32& index) const
		{
			return conv.supportMarginRelative(dir, aToB, support, index);
		}

		Ps::aos::Vec3V doSupportSweep(const Ps::aos::Vec3VArg dir) const
		{
			return conv.supportSweepRelative(dir, aToB);
		}
	};

	template <typename Convex>
	class SupportMapLocalImpl : public SupportMap
	{
		const Convex& conv;
	public:

		SupportMapLocalImpl(const Convex& _conv) : conv(_conv)
		{
		}

		void doWarmStartSupport(const PxI32 index, Ps::aos::Vec3V& point) const
		{
			point = conv.supportPoint(index);
		}

		Ps::aos::Vec3V doSupport(const Ps::aos::Vec3VArg dir) const
		{
			return conv.supportLocal(dir);
		}

	/*	Ps::aos::Vec3V doSupport(const Ps::aos::Vec3VArg dir, PxI32& index)
		{
			return conv.supportLocal(dir, index);
		}*/

		Ps::aos::Vec3V doWarmStartSupportMargin(const PxI32 index) const
		{
			return conv.supportMarginPoint(index);
		}

		Ps::aos::Vec3V doSupportMargin(const Ps::aos::Vec3VArg dir, Ps::aos::Vec3V& support, PxI32& index) const
		{ 
			return conv.supportMarginLocal(dir, support, index);
		}

		Ps::aos::Vec3V doSupportSweep(const Ps::aos::Vec3VArg dir) const
		{
			return conv.supportSweepLocal(dir);
		}
	};


	class SupportMapPair
	{
	public:
		virtual ~SupportMapPair(){}
		virtual void doWarmStartSupport(const PxI32 indexA, const PxI32 indexB, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const = 0;
		virtual void doSupport(const Ps::aos::Vec3VArg dir, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const = 0;
		virtual void doWarmStartSupportMargin(const PxI32 indexA, const PxI32 indexB, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const = 0;
		virtual void doSupportMargin(const Ps::aos::Vec3VArg dir, PxI32& indexA, PxI32& indexB, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const = 0;
		virtual Ps::aos::Vec3V doSupportSweep(const Ps::aos::Vec3VArg dir, const Ps::aos::Vec3VArg x, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB) const = 0;
		virtual Ps::aos::Vec3V doSupportSweepOnB(const Ps::aos::Vec3VArg nvNorm, const Ps::aos::Vec3VArg x) const = 0;
		//virtual Ps::aos::Vec3V doSupportSweepOnA(const Ps::aos::Vec3VArg vNorm) const = 0;
	};


	template <typename ConvexA, typename ConvexB>
	class SupportMapPairRelativeImpl : public SupportMapPair
	{
		const ConvexA& convA;
		const ConvexB& convB;
		const Ps::aos::PsMatTransformV& aToB;

	public:

		SupportMapPairRelativeImpl(const ConvexA& _convA, const ConvexB& _convB, const Ps::aos::PsMatTransformV& _aToB) : convA(_convA), convB(_convB), aToB(_aToB)
		{
		}

		void doWarmStartSupport(const PxI32 indexA, const PxI32 indexB, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const
		{
			using namespace Ps::aos;
			const Vec3V _sa = aToB.transform(convA.supportPoint(indexA));
			const Vec3V _sb = convB.supportPoint(indexB);
			supportA = _sa;
			supportB = _sb;
			support = V3Sub(_sa, _sb);
		}

		void doSupport(const Ps::aos::Vec3VArg dir, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const
		{
			using namespace Ps::aos;
			const Vec3V _sa = convA.supportRelative(V3Neg(dir), aToB);
			const Vec3V _sb = convB.supportLocal(dir);
			supportA = _sa;
			supportB = _sb;
			support = V3Sub(_sa, _sb);
		}

		void doWarmStartSupportMargin(const PxI32 indexA, const PxI32 indexB, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const
		{
			using namespace Ps::aos;
			const Vec3V _sa = aToB.transform(convA.supportMarginPoint(indexA));
			const Vec3V _sb = convB.supportMarginPoint(indexB);
			supportA = _sa;
			supportB = _sb;
			support = V3Sub(_sa, _sb);
		}

		void doSupportMargin(const Ps::aos::Vec3VArg dir, PxI32& indexA, PxI32& indexB, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const
		{ 
			using namespace Ps::aos;
			const Vec3V _sa = convA.supportMarginRelative(V3Neg(dir), aToB, supportA, indexA);
			const Vec3V _sb = convB.supportMarginLocal(dir, supportB, indexB);
			support = V3Sub(_sa, _sb);
		}

		Ps::aos::Vec3V doSupportSweep(const Ps::aos::Vec3VArg vNorm, const Ps::aos::Vec3VArg x, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB) const
		{
			using namespace Ps::aos;
			const Vec3V nvNorm = V3Neg(vNorm);

			const Vec3V _sa = convA.supportSweepRelative(vNorm, aToB);
			const Vec3V _sb = V3Add(x, convB.supportSweepLocal(nvNorm));
			supportA = _sa;
			supportB = _sb;
			return V3Sub(_sa, _sb);
		}

		Ps::aos::Vec3V doSupportSweepOnB(const Ps::aos::Vec3VArg nvNorm, const Ps::aos::Vec3VArg x)const 
		{
			using namespace Ps::aos;
			return V3Add(x, convB.supportSweepLocal(nvNorm));
		}

	/*	Ps::aos::Vec3V doSupportSweepOnA(const Ps::aos::Vec3VArg vNorm)const 
		{
			using namespace Ps::aos;
			return convA.supportSweepRelative(vNorm, aToB);
		}
		*/
	};

	template <typename ConvexA, typename ConvexB>
	class SupportMapPairLocalImpl : public SupportMapPair
	{
		const ConvexA& convA;
		const ConvexB& convB;

	public:

		SupportMapPairLocalImpl(const ConvexA& _convA, const ConvexB& _convB) : convA(_convA), convB(_convB)
		{
		}

		void doWarmStartSupport(const PxI32 indexA, const PxI32 indexB, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const
		{
			using namespace Ps::aos;
			const Vec3V _sa = convA.supportPoint(indexA);
			const Vec3V _sb = convB.supportPoint(indexB);
			supportA = _sa;
			supportB = _sb;
			support = V3Sub(_sa, _sb);
		}

		void doSupport(const Ps::aos::Vec3VArg dir, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const
		{
			using namespace Ps::aos;
			const Vec3V _sa = convA.supportLocal(V3Neg(dir));
			const Vec3V _sb = convB.supportLocal(dir);
			supportA = _sa;
			supportB = _sb;
			support = V3Sub(_sa, _sb);
		}

		void doWarmStartSupportMargin(const PxI32 indexA, const PxI32 indexB, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const
		{
			using namespace Ps::aos;
			const Vec3V _sa = convA.supportMarginPoint(indexA);
			const Vec3V _sb = convB.supportMarginPoint(indexB);
			supportA = _sa;
			supportB = _sb;
			support = V3Sub(_sa, _sb);
		}

		void doSupportMargin(const Ps::aos::Vec3VArg dir, PxI32& indexA, PxI32& indexB, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB, Ps::aos::Vec3V& support) const
		{ 
			using namespace Ps::aos;
			const Vec3V _sa = convA.supportMarginLocal(V3Neg(dir), supportA, indexA);
			const Vec3V _sb = convB.supportMarginLocal(dir, supportB, indexB);
			support = V3Sub(_sa, _sb);
		}

		Ps::aos::Vec3V doSupportSweep(const Ps::aos::Vec3VArg vNorm, const Ps::aos::Vec3VArg x, Ps::aos::Vec3V& supportA, Ps::aos::Vec3V& supportB) const
		{
			using namespace Ps::aos;
			const Vec3V nvNorm = V3Neg(vNorm);

			const Vec3V _sa = convA.supportSweepLocal(vNorm);
			const Vec3V _sb = V3Add(x, convB.supportSweepLocal(nvNorm));
			supportA = _sa;
			supportB = _sb;
			return V3Sub(_sa, _sb);
		}

		Ps::aos::Vec3V doSupportSweepOnB(const Ps::aos::Vec3VArg nvNorm, const Ps::aos::Vec3VArg x) const
		{
			using namespace Ps::aos;
			return V3Add(x, convB.supportSweepLocal(nvNorm));
		}

	/*	Ps::aos::Vec3V doSupportSweepOnA(const Ps::aos::Vec3VArg vNorm)const 
		{
			using namespace Ps::aos;
			return convA.supportSweepLocal(vNorm);
		}
		*/
	};
}

}

#endif
