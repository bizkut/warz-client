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


#ifndef PX_GJK_H
#define PX_GJK_H


#include "GuConvexSupportTable.h"
#include "GuGJKWrapper.h"
#include "GuGJKSimplex.h"
#include "GuGJKSimplexTesselation.h"
#include "GuGJKFallBack.h"




namespace physx
{
namespace Gu
{

	class ConvexV;



#ifndef	__SPU__

	template<class ConvexA, class ConvexB>
	PxGJKStatus gjk(const ConvexA& a, const ConvexB& b, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist)
	{
		using namespace Ps::aos;
		Vec3V Q[4];
		Vec3V A[4];
		Vec3V B[4];

		const Vec3V zeroV = V3Zero();
		const FloatV zero = FZero();
		const BoolV bTrue = BTTTT();
		//const BoolV bFalse = BFFFF();
		PxU32 size=0;

		const Vec3V _initialSearchDir = V3Sub(a.getCenter(), b.getCenter());
		Vec3V v = V3Sel(FIsGrtr(V3Dot(_initialSearchDir, _initialSearchDir), zero), _initialSearchDir, V3UnitX());

	/*	const FloatV marginA = a.getMargin();
		const FloatV marginB = b.getMargin();*/
		const FloatV minMargin = FMin(a.getMinMargin(), b.getMinMargin());
		const FloatV eps2 = FMul(minMargin, FloatV_From_F32(0.001f));
		const FloatV epsRel = FMul(eps2, eps2);

		Vec3V closA(zeroV), closB(zeroV);
		FloatV sDist = FMax();
		FloatV minDist = sDist;
		Vec3V closAA;   
		Vec3V closBB;

		
		BoolV bNotTerminated = bTrue;
		BoolV bCon = bTrue;
		
		do
		{
			minDist = sDist;
			closAA = closA;
			closBB = closB;

			const Vec3V supportA=a.support(V3Neg(v));
			const Vec3V supportB=b.support(v);
			
			//calculate the support point
			const Vec3V support = V3Sub(supportA, supportB);
			const FloatV signDist = V3Dot(v, support);
			const FloatV tmp0 = FSub(sDist, signDist);

			PX_ASSERT(size < 4);
			A[size]=supportA;
			B[size]=supportB;
			Q[size++]=support;
	
			if(FAllGrtr(FMul(epsRel, sDist), tmp0))
			{
				const Vec3V n = V3Normalize(V3Sub(closB, closA));
			/*	closestA = V3Sel(aQuadratic, V3ScaleAdd(n, marginA, closA), closA);
				closestB = V3Sel(bQuadratic, V3NegScaleSub(n, marginB, closB), closB);*/
				closestA = closA;
				closestB = closB;
				sqDist = sDist;
				normal = n;
				return GJK_NON_INTERSECT;
			}

			//calculate the closest point between two convex hull
			const Vec3V tempV = GJKCPairDoSimplex(Q, A, B, support, supportA, supportB, size, closA, closB);
			v = tempV;
			sDist = V3Dot(v, v);
			bCon = FIsGrtr(minDist, sDist);
			bNotTerminated = BAnd(FIsGrtr(sDist, eps2), bCon);
		}while(BAllEq(bNotTerminated, bTrue));		
	
		closA = V3Sel(bCon, closA, closAA);
		closB = V3Sel(bCon, closB, closBB);

		closestA = closA;
		closestB = closB;
		normal = V3Normalize(V3Sub(closB, closA));
		sqDist = FSel(bCon, sDist, minDist);
		return BAllEq(bCon, bTrue) == 1 ? GJK_CONTACT : GJK_DEGENERATE;
	}

	template<class ConvexA, class ConvexB>
	PxGJKStatus gjkRelative(const ConvexA& a, const ConvexB& b, const Ps::aos::PsMatTransformV& aToB, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist)
	{
		using namespace Ps::aos;
		Vec3V Q[4];
		Vec3V A[4];
		Vec3V B[4];

		const Vec3V zeroV = V3Zero();
		const FloatV zero = FZero();
		const BoolV bTrue = BTTTT();
		//const BoolV bFalse = BFFFF();
		PxU32 size=0;

		const Vec3V _initialSearchDir = aToB.p;
		Vec3V v = V3Sel(FIsGrtr(V3Dot(_initialSearchDir, _initialSearchDir), zero), _initialSearchDir, V3UnitX());

		const FloatV tenPerc = FloatV_From_F32(0.1f);
		const FloatV minMargin = FMin(a.getMinMargin(), b.getMinMargin());
		const FloatV _eps2 = FMul(minMargin, tenPerc);
		const FloatV eps2 = FMul(_eps2, _eps2);
		//const FloatV eps2 = FMul(minMargin, minMargin);
		const FloatV epsRel = FloatV_From_F32(0.000225f);//1.5%.
	//	const FloatV epsRel = FloatV_From_F32(0.0001f);

	//	const FloatV epsRel = FloatV_From_F32(0.005f);//1.5%.

	//	const FloatV epsRel = FloatV_From_F32(0.2f);//1.5%.

		Vec3V closA(zeroV), closB(zeroV);
		FloatV sDist = FMax();
		FloatV minDist = sDist;
		Vec3V closAA;   
		Vec3V closBB;

		
		BoolV bNotTerminated = bTrue;
		BoolV bCon = bTrue;
		
		do
		{
			minDist = sDist;
			closAA = closA;
			closBB = closB;

			const Vec3V supportA=a.supportRelative(V3Neg(v), aToB);
			const Vec3V supportB=b.supportLocal(v);
			
			//calculate the support point
			const Vec3V support = V3Sub(supportA, supportB);
			const FloatV signDist = V3Dot(v, support);
			const FloatV tmp0 = FSub(sDist, signDist);

			PX_ASSERT(size < 4);
			A[size]=supportA;
			B[size]=supportB;
			Q[size++]=support;   
	
			if(FAllGrtr(FMul(epsRel, sDist), tmp0))
			{
				const Vec3V n = V3Normalize(V3Sub(closB, closA));
			/*	closestA = V3Sel(aQuadratic, V3ScaleAdd(n, marginA, closA), closA);
				closestB = V3Sel(bQuadratic, V3NegScaleSub(n, marginB, closB), closB);*/
				closestA = closA;
				closestB = closB;
				sqDist = sDist;
				normal = n;
				return GJK_NON_INTERSECT;
			}  

			//calculate the closest point between two convex hull
			const Vec3V tempV = GJKCPairDoSimplex(Q, A, B, support, supportA, supportB, size, closA, closB);
			v = tempV;
			sDist = V3Dot(v, v);
			bCon = FIsGrtr(minDist, sDist);
			bNotTerminated = BAnd(FIsGrtr(sDist, eps2), bCon);
		}while(BAllEq(bNotTerminated, bTrue));
		
		closA = V3Sel(bCon, closA, closAA);
		closB = V3Sel(bCon, closB, closBB);
		closestA = closA;
		closestB = closB;
		normal = V3Normalize(V3Sub(closB, closA));
		sqDist = FSel(bCon, sDist, minDist);
		
		return BAllEq(bCon, bTrue) == 1 ? GJK_CONTACT : GJK_DEGENERATE;
		
	}

	template<class ConvexA, class ConvexB>
	PxGJKStatus gjkLocal(const ConvexA& a, const ConvexB& b, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist)
	{
		using namespace Ps::aos;
		Vec3V Q[4];
		Vec3V A[4];
		Vec3V B[4];

		const Vec3V zeroV = V3Zero();
		const FloatV zero = FZero();
		const BoolV bTrue = BTTTT();
		//const BoolV bFalse = BFFFF();
		PxU32 size=0;

		const Vec3V _initialSearchDir = V3Sub(a.getCenter(), b.getCenter());
		Vec3V v = V3Sel(FIsGrtr(V3Dot(_initialSearchDir, _initialSearchDir), zero), _initialSearchDir, V3UnitX());

	/*	const BoolV aQuadratic = a.isMarginEqRadiusV();
		const BoolV bQuadratic = b.isMarginEqRadiusV();*/
		/*const FloatV marginA = a.getMargin();
		const FloatV marginB = b.getMargin();*/

		const FloatV tenPerc = FloatV_From_F32(0.1f);
		const FloatV minMargin = FMin(a.getMinMargin(), b.getMinMargin());
		const FloatV _eps2 = FMul(minMargin, tenPerc);
		const FloatV eps2 = FMul(_eps2, _eps2);
		//const FloatV epsRel = FMul(onePerc, onePerc);>of 
		const FloatV epsRel = FloatV_From_F32(0.000225f);//1.5%.
		
		Vec3V closA(zeroV), closB(zeroV);
		FloatV sDist = FMax();
		FloatV minDist = sDist;
		Vec3V closAA;   
		Vec3V closBB;

		
		BoolV bNotTerminated = bTrue;
		BoolV bCon = bTrue;
		
		do
		{
			minDist = sDist;
			closAA = closA;
			closBB = closB;

			const Vec3V supportA=a.supportLocal(V3Neg(v));
			const Vec3V supportB=b.supportLocal(v);
			
			//calculate the support point
			const Vec3V support = V3Sub(supportA, supportB);
			const FloatV signDist = V3Dot(v, support);
			const FloatV tmp0 = FSub(sDist, signDist);

			PX_ASSERT(size < 4);
			A[size]=supportA;
			B[size]=supportB;
			Q[size++]=support;
	
			if(FAllGrtr(FMul(epsRel, sDist), tmp0))
			{
				const Vec3V n = V3Normalize(V3Sub(closB, closA));
				closestA = closA;
				closestB = closB;
				sqDist = sDist;
				normal = n;
				return GJK_NON_INTERSECT;
			}

			//calculate the closest point between two convex hull
			const Vec3V tempV = GJKCPairDoSimplex(Q, A, B, support, supportA, supportB, size, closA, closB);
			v = tempV;
			sDist = V3Dot(v, v);
			bCon = FIsGrtr(minDist, sDist);
			bNotTerminated = BAnd(FIsGrtr(sDist, eps2), bCon);
		}while(BAllEq(bNotTerminated, bTrue));

		closA = V3Sel(bCon, closA, closAA);
		closB = V3Sel(bCon, closB, closBB);

		closestA = closA;
		closestB = closB;
		normal = V3Normalize(V3Sub(closB, closA));
		sqDist = FSel(bCon, sDist, minDist);

		return BAllEq(bCon, bTrue) == 1 ? GJK_CONTACT : GJK_DEGENERATE;
	}


	template<class ConvexA, class ConvexB>
	PxGJKStatus gjkRelativeTesselation(const ConvexA& a, const ConvexB& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg sqTolerance, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist)
	{
		using namespace Ps::aos;
		PxGJKStatus status = gjkRelative(a, b, aToB, closestA, closestB, normal, sqDist);
		if(status == GJK_DEGENERATE)
		{
			status = FAllGrtr(sqTolerance, sqDist)? GJK_CONTACT : GJK_NON_INTERSECT;
			if(status == GJK_NON_INTERSECT)
			{
				//PX_PRINTF("--------------------------------------------------------------------------------------\n");
				//PX_PRINTF("GJKstatus = %i, sqDist = %f\n", status, Ps::aos::PxF32_From_FloatV(sqDist));
				SupportMapRelativeImpl<ConvexA> map0(a, aToB);
				SupportMapLocalImpl<ConvexB>	map1(b);
				status = gjkRelativeFallback(a, b, &map0, &map1, aToB.p, closestA, closestB, normal, sqDist);
				if(status == GJK_DEGENERATE)
				{
					status = FAllGrtr(sqTolerance, sqDist)? GJK_CONTACT : GJK_NON_INTERSECT;
				}
			}
		}
		return status;
	}


#else
	

	PxGJKStatus gjk(const ConvexV& a, const ConvexV& b, SupportMap* map1, SupportMap* map2, const Ps::aos::Vec3VArg initialDir,  Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist);
	
	template<class ConvexA, class ConvexB>
	PxGJKStatus gjkRelative(const ConvexA& a, const ConvexB& b, const Ps::aos::PsMatTransformV& aToB, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist)
	{
		SupportMapRelativeImpl<ConvexA> map1(a, aToB);
		SupportMapLocalImpl<ConvexB> map2(b);
		return gjk(a, b, &map1, &map2, aToB.p, closestA, closestB, normal, sqDist );

	}

	template<class ConvexA, class ConvexB>
	PxGJKStatus gjkLocal(const ConvexA& a, const ConvexB& b, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist)
	{
		using namespace Ps::aos;
	
		SupportMapLocalImpl<ConvexA> map1(a);
		SupportMapLocalImpl<ConvexB> map2(b);
		const Vec3V initialDir = V3Sub(a.getCenter(), b.getCenter());
		return gjk(a, b, &map1, &map2, initialDir, closestA, closestB, normal, sqDist);
	}

	template<class ConvexA, class ConvexB>
	PxGJKStatus gjkRelativeTesselation(const ConvexA& a, const ConvexB& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg sqTolerance, Ps::aos::Vec3V& closestA, Ps::aos::Vec3V& closestB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& sqDist)
	{
		using namespace Ps::aos;
		SupportMapRelativeImpl<ConvexA> map1(a, aToB);
		SupportMapLocalImpl<ConvexB> map2(b);
		PxGJKStatus status = gjk(a, b, &map1, &map2, aToB.p, closestA, closestB, normal, sqDist);
		if(status == GJK_DEGENERATE)
		{
			status = FAllGrtr(sqTolerance, sqDist)? GJK_CONTACT : GJK_NON_INTERSECT;
			if(status == GJK_NON_INTERSECT)
			{
				//PX_PRINTF("--------------------------------------------------------------------------------------\n");
				//PX_PRINTF("GJKstatus = %i, sqDist = %f\n", status, Ps::aos::PxF32_From_FloatV(sqDist));
				status = gjkRelativeFallback(a, b, &map1, &map2, aToB.p, closestA, closestB, normal, sqDist);
				if(status == GJK_DEGENERATE)
				{
					status = FAllGrtr(sqTolerance, sqDist)? GJK_CONTACT : GJK_NON_INTERSECT;
				}
			}
		}
		return status;
	}

#endif

}

}

#endif
