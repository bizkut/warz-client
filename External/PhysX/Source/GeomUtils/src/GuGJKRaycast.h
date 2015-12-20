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


#ifndef PX_GJKRAYCAST_H
#define PX_GJKRAYCAST_H

//#include "GuGJKPenetrationWrapper.h"
#include "GuGJKSimplex.h"
#include "GuConvexSupportTable.h"
#include "GuGJKPenetration.h"
//#include "GuGJKPenetrationWrapper.h"


namespace physx
{


namespace Gu
{

#ifndef	__SPU__

	//template<class ConvexA, class ConvexB>
	//bool gjkRayCast(ConvexA& a, ConvexB& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA)
	//{
	//	using namespace Ps::aos;
	//	const Vec3V zeroV = V3Zero();
	//	const FloatV zero = FZero();
	//	const FloatV one = FOne();
	//	const BoolV bTrue = BTTTT();

	//	const FloatV maxDist = FloatV_From_F32(PX_MAX_REAL);
	//
	//	FloatV _lambda = zero;//initialLambda;
	//	Vec3V x = V3ScaleAdd(r, _lambda, s);
	//	PxU32 size=1;

	//	const Vec3V bOriginalCenter = b.getCenter(); 
	//	b.setCenter(x);
	//	const Vec3V _initialSearchDir(V3Sub(a.getCenter(), b.getCenter()));
	//	const Vec3V initialSearchDir = V3Sel(FIsGrtr(V3Dot(_initialSearchDir, _initialSearchDir), zero), _initialSearchDir, V3UnitX());

	//	const Vec3V initialSupportA(a.supportSweep(V3Neg(initialSearchDir)));
	//	const Vec3V initialSupportB(b.supportSweep(initialSearchDir));
	//	 
	//	Vec3V Q[4] = {V3Sub(initialSupportA, initialSupportB), zeroV, zeroV, zeroV}; //simplex set
	//	Vec3V A[4] = {initialSupportA, zeroV, zeroV, zeroV}; //ConvexHull a simplex set
	//	Vec3V B[4] = {initialSupportB, zeroV, zeroV, zeroV}; //ConvexHull b simplex set
	//	 

	//	Vec3V v = V3Neg(Q[0]);
	//	Vec3V supportA = initialSupportA;
	//	Vec3V supportB = initialSupportB;
	//	Vec3V support = Q[0];
	//	const FloatV eps1 = FloatV_From_F32(0.0001f);
	//	const FloatV eps2 = FMul(eps1, eps1);
	//	Vec3V closA(initialSupportA), closB(initialSupportB);
	//	FloatV sDist = V3Dot(v, v);
	//	FloatV minDist = sDist;
	//	Vec3V closAA = initialSupportA;
	//	Vec3V closBB = initialSupportB;
	//	
	//	BoolV bNotTerminated = FIsGrtr(sDist, eps2);
	//	BoolV bCon = bTrue;

	//	Vec3V nor = v;
	//	
	//	while(BAllEq(bNotTerminated, bTrue))
	//	{
	//		
	//		minDist = sDist;
	//		closAA = closA;
	//		closBB = closB;

	//		supportA=a.supportSweep(v);
	//		supportB=b.supportSweep(V3Neg(v));
	//		
	//		//calculate the support point
	//		support = V3Sub(supportA, supportB);
	//		const Vec3V w = V3Neg(support);
	//		const FloatV vw = V3Dot(v, w);
	//		const FloatV vr = V3Dot(v, r);
	//		if(FAllGrtr(vw, zero))
	//		{
	//
	//			if(FAllGrtrOrEq(vr, zero))
	//			{
	//				b.setCenter(bOriginalCenter);
	//				return false;
	//			}
	//			else
	//			{
	//				const FloatV _oldLambda = _lambda;
	//				_lambda = FSub(_lambda, FDiv(vw, vr));
	//				if(FAllGrtr(_lambda, _oldLambda))
	//				{
	//					if(FAllGrtr(_lambda, one))
	//					{
	//						b.setCenter(bOriginalCenter);
	//						return false;
	//					}
	//					const Vec3V bPreCenter = b.getCenter();
	//					x = V3ScaleAdd(r, _lambda, s);
	//					b.setCenter(x);
	//					const Vec3V offSet = V3Sub(x, bPreCenter);
	//					const Vec3V b0 = V3Add(B[0], offSet);
	//					const Vec3V b1 = V3Add(B[1], offSet);
	//					const Vec3V b2 = V3Add(B[2], offSet);

	//					B[0] = b0;
	//					B[1] = b1;
	//					B[2] = b2;

	//					Q[0]=V3Sub(A[0], b0);
	//					Q[1]=V3Sub(A[1], b1);
	//					Q[2]=V3Sub(A[2], b2);

	//					supportB = b.supportSweep(V3Neg(v));
	//					support = V3Sub(supportA, supportB);
	//					minDist = maxDist;
	//					nor = v;
	//					//size=0;
	//				}
	//			}
	//		}

	//		PX_ASSERT(size < 4);
	//		A[size]=supportA;
	//		B[size]=supportB;
	//		Q[size++]=support;
	//
	//		//calculate the closest point between two convex hull
	//		const Vec3V tempV = GJKCPairDoSimplex(Q, A, B, support, supportA, supportB, size, closA, closB);
	//		v = V3Neg(tempV);
	//		sDist = V3Dot(tempV, tempV);
	//		bCon = FIsGrtr(minDist, sDist);
	//		bNotTerminated = BAnd(FIsGrtr(sDist, eps2), bCon);
	//	}

	//	lambda = _lambda;
	//	if(FAllEq(_lambda, zero))
	//	{
	//		//time of impact is zero, the sweep shape is intesect, use epa to get the normal and contact point
	//		b.setCenter(bOriginalCenter);
	//		const FloatV contactDist = getContactEps(a.getMargin(), b.getMargin());

	//		closestA = closAA;
	//		normal= V3Normalize(V3Sub(closAA, closBB));
	//		//hitPoint = x;

	//		if(GJKPenetration(a, b, contactDist, closA, closB, normal, sDist))
	//		{
	//			closestA = closA;
	//		}
	//	}
	//	else
	//	{
	//		//const FloatV stepBackRatio = FDiv(offset, V3Length(r));
	//		//lambda = FMax(FSub(lambda, stepBackRatio), zero);
	//		b.setCenter(bOriginalCenter);
	//		closA = V3Sel(bCon, closA, closAA);
	//		closestA = closA;
	//		normal = V3Neg(V3Normalize(nor));
	//	}
	//	return true;
	//}

	//template<class ConvexA, class ConvexB>
	//bool gjkRelativeRayCast(ConvexA& a, ConvexB& b, const Ps::aos::PsMatTransformV& aToBRel, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA)
	//{
	//	using namespace Ps::aos;
	//	const Vec3V zeroV = V3Zero();
	//	const FloatV zero = FZero();
	//	const FloatV one = FOne();
	//	const BoolV bTrue = BTTTT();

	//	const FloatV maxDist = FloatV_From_F32(PX_MAX_REAL);
	//
	//	FloatV _lambda = zero;//initialLambda;
	//	Vec3V x = V3ScaleAdd(r, _lambda, s);
	//	PxU32 size=1;

	//	Ps::aos::PsMatTransformV aToB(aToBRel);

	///*	const Vec3V bOriginalCenter = b.getCenter(); 
	//	b.setCenter(x);*/

	///*	const Vec3V bOriginalCenter = V3Zero(); 
	//	b.setCenter(x);*/
	//	const Vec3V _initialSearchDir(aToB.p);//(V3Sub(a.getCenter(), b.getCenter()));
	//	const Vec3V initialSearchDir = V3Sel(FIsGrtr(V3Dot(_initialSearchDir, _initialSearchDir), zero), _initialSearchDir, V3UnitX());

	//	const Vec3V initialSupportA(a.supportSweepRelative(V3Neg(initialSearchDir), aToB));
	//	const Vec3V initialSupportB(b.supportSweepLocal(initialSearchDir));
	//	 
	//	Vec3V Q[4] = {V3Sub(initialSupportA, initialSupportB), zeroV, zeroV, zeroV}; //simplex set
	//	Vec3V A[4] = {initialSupportA, zeroV, zeroV, zeroV}; //ConvexHull a simplex set
	//	Vec3V B[4] = {initialSupportB, zeroV, zeroV, zeroV}; //ConvexHull b simplex set
	//	 

	//	Vec3V v = V3Neg(Q[0]);
	//	Vec3V supportA = initialSupportA;
	//	Vec3V supportB = initialSupportB;
	//	Vec3V support = Q[0];
	//	const FloatV eps1 = FloatV_From_F32(0.0001f);
	//	const FloatV eps2 = FMul(eps1, eps1);
	//	Vec3V closA(initialSupportA), closB(initialSupportB);
	//	FloatV sDist = V3Dot(v, v);
	//	FloatV minDist = sDist;
	//	Vec3V closAA = initialSupportA;
	//	Vec3V closBB = initialSupportB;
	//	
	//	BoolV bNotTerminated = FIsGrtr(sDist, eps2);
	//	BoolV bCon = bTrue;

	//	Vec3V nor = v;
	//	
	//	while(BAllEq(bNotTerminated, bTrue))
	//	{
	//		
	//		minDist = sDist;
	//		closAA = closA;
	//		closBB = closB;

	//		supportA=a.supportSweepRelative(v, aToB);
	//		supportB=b.supportSweepLocal(V3Neg(v));
	//		
	//		//calculate the support point
	//		support = V3Sub(supportA, supportB);
	//		const Vec3V w = V3Neg(support);
	//		const FloatV vw = V3Dot(v, w);
	//		const FloatV vr = V3Dot(v, r);
	//		if(FAllGrtr(vw, zero))
	//		{
	//
	//			if(FAllGrtrOrEq(vr, zero))
	//			{
	//				//b.setCenter(bOriginalCenter);
	//				return false;
	//			}
	//			else
	//			{
	//				const FloatV _oldLambda = _lambda;
	//				_lambda = FSub(_lambda, FDiv(vw, vr));
	//				if(FAllGrtr(_lambda, _oldLambda))
	//				{
	//					if(FAllGrtr(_lambda, one))
	//					{
	//					//	b.setCenter(bOriginalCenter);
	//						return false;
	//					}
	//					const Vec3V bPreCenter = aToB.p;
	//					x = V3ScaleAdd(r, _lambda, s);
	//					//b.setCenter(x);
	//					aToB.p = V3Neg(x);
	//					const Vec3V offSet = V3Sub(x, bPreCenter);
	//					const Vec3V a0 = V3Sub(A[0], offSet);
	//					const Vec3V a1 = V3Sub(A[1], offSet);
	//					const Vec3V a2 = V3Sub(A[2], offSet);
	//				
	//					A[0] = a0;
	//					A[1] = a1;
	//					A[2] = a2;

	//					Q[0]=V3Sub(a0, B[0]);
	//					Q[1]=V3Sub(a1, B[1]);
	//					Q[2]=V3Sub(a2, B[2]);

	//					supportA = a.supportSweepRelative(v, aToB);
	//					support = V3Sub(supportA, supportB);
	//					minDist = maxDist;
	//					nor = v;
	//					//size=0;
	//				}
	//			}
	//		}

	//		PX_ASSERT(size < 4);
	//		A[size]=supportA;
	//		B[size]=supportB;
	//		Q[size++]=support;
	//
	//		//calculate the closest point between two convex hull
	//		const Vec3V tempV = GJKCPairDoSimplex(Q, A, B, support, supportA, supportB, size, closA, closB);
	//		v = V3Neg(tempV);
	//		sDist = V3Dot(tempV, tempV);
	//		bCon = FIsGrtr(minDist, sDist);
	//		bNotTerminated = BAnd(FIsGrtr(sDist, eps2), bCon);
	//	}

	//	lambda = _lambda;
	//	if(FAllEq(_lambda, zero))
	//	{
	//		//time of impact is zero, the sweep shape is intesect, use epa to get the normal and contact point
	//		//b.setCenter(bOriginalCenter);
	//		const FloatV contactDist = getContactEps(a.getMargin(), b.getMargin());

	//		closestA = closAA;
	//		normal= V3Normalize(V3Sub(closAA, closBB));
	//		//hitPoint = x;

	//		if(GJKRelativePenetration(a, b, aToBRel, contactDist, closA, closB, normal, sDist))
	//		{
	//			// in the local space of B
	//			closestA = closA;
	//		}
	//	}
	//	else
	//	{
	//		//const FloatV stepBackRatio = FDiv(offset, V3Length(r));
	//		//lambda = FMax(FSub(lambda, stepBackRatio), zero);
	//		//b.setCenter(bOriginalCenter);
	//		closA = V3Sel(bCon, closA, closAA);
	//		closestA = closA;
	//		normal = V3Neg(V3Normalize(nor));
	//	}
	//	return true;
	//}

	
	//ConvexA is in the local space of ConvexB
	template<class ConvexA, class ConvexB>
	bool _gjkLocalRayCast(ConvexA& a, ConvexB& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal _inflation/*, const bool initialOverlap*/)
	{
		using namespace Ps::aos;

		const FloatV zero = FZero();
		FloatV _lambda = zero;//initialLambda;


		const FloatV inflation = FloatV_From_F32(_inflation);
		const Vec3V zeroV = V3Zero();
		
		const FloatV one = FOne();
		const BoolV bTrue = BTTTT();

		const FloatV maxDist = FloatV_From_F32(PX_MAX_REAL);
	
		
		Vec3V x = V3ScaleAdd(r, _lambda, s);
		PxU32 size=1;

		const Vec3V dir = V3Sub(a.getCenter(), b.getCenter());
		const Vec3V _initialSearchDir = V3Sel(FIsGrtr(V3Dot(dir, dir), FEps()), dir, V3UnitX());
		const Vec3V initialSearchDir = V3Normalize(_initialSearchDir);

		const Vec3V initialSupportA(a.supportSweepLocal(V3Neg(initialSearchDir)));
		//const Vec3V initialSupportB(V3ScaleAdd(initialSearchDir, inflation, b.supportSweepLocal(initialSearchDir)));
		const Vec3V initialSupportB(b.supportSweepLocal(initialSearchDir));
		 
		Vec3V Q[4] = {V3Sub(initialSupportA, initialSupportB), zeroV, zeroV, zeroV}; //simplex set
		Vec3V A[4] = {initialSupportA, zeroV, zeroV, zeroV}; //ConvexHull a simplex set
		Vec3V B[4] = {initialSupportB, zeroV, zeroV, zeroV}; //ConvexHull b simplex set
		 

		Vec3V v = V3Neg(Q[0]);
		Vec3V supportA = initialSupportA;
		Vec3V supportB = initialSupportB;
		Vec3V support = Q[0];
		//const FloatV onePerc = FloatV_From_F32(0.01f);
		//const FloatV minMargin = FMin(a.getMinMargin(), b.getMinMargin());

		////const FloatV eps2 = FAdd(sqInflation, FMul(minMargin, onePerc));
		//const FloatV eps2 = FMul(minMargin, onePerc);

		const FloatV eps1 = FloatV_From_F32(0.0001f);
		const FloatV eps2 = FMul(eps1, eps1);

		const FloatV inflation2 = FAdd(FMul(inflation, inflation), eps2);

		Vec3V closA(initialSupportA), closB(initialSupportB);
		FloatV sDist = V3Dot(v, v);
		FloatV minDist = sDist;
		Vec3V closAA = initialSupportA;
		//Vec3V closBB = initialSupportB;
		
		BoolV bNotTerminated = FIsGrtr(sDist, eps2);
		BoolV bCon = bTrue;

		Vec3V nor = v;
		
		while(BAllEq(bNotTerminated, bTrue))
		{
			
			minDist = sDist;
			closAA = closA;
			//closBB = closB;

			const Vec3V vNorm = V3Normalize(v);
			const Vec3V nvNorm = V3Neg(vNorm);

			supportA=a.supportSweepLocal(vNorm);
			//supportB=V3ScaleAdd(nvNorm, inflation, V3Add(x, b.supportSweepLocal(nvNorm)));
			supportB=V3Add(x, b.supportSweepLocal(nvNorm));
		
			//calculate the support point
			support = V3Sub(supportA, supportB);
			const Vec3V w = V3Neg(support);
			const FloatV vw = FSub(V3Dot(vNorm, w), inflation);
			const FloatV vr = V3Dot(vNorm, r);
			if(FAllGrtr(vw, zero))
			{
	
				if(FAllGrtrOrEq(vr, zero))
				{
					return false;
				}
				else
				{
					const FloatV _oldLambda = _lambda;
					_lambda = FSub(_lambda, FDiv(vw, vr));
					if(FAllGrtr(_lambda, _oldLambda))
					{
						if(FAllGrtr(_lambda, one))
						{
							return false;
						}
						const Vec3V bPreCenter = x;
						x = V3ScaleAdd(r, _lambda, s);
						//aToB.p = V3Sub(aToB.p, x);
					
						//aToB.p = V3Neg(x);
						const Vec3V offSet =V3Sub(x, bPreCenter);
						const Vec3V b0 = V3Add(B[0], offSet);
						const Vec3V b1 = V3Add(B[1], offSet);
						const Vec3V b2 = V3Add(B[2], offSet);
					
						B[0] = b0;
						B[1] = b1;
						B[2] = b2;

						Q[0]=V3Sub(A[0], b0);
						Q[1]=V3Sub(A[1], b1);
						Q[2]=V3Sub(A[2], b2);

						//supportA = a.supportSweepRelative(v, aToB);
						
						//supportB = V3Add(x, b.supportSweepLocal(V3Neg(v)));
						//supportB = V3ScaleAdd(nvNorm, inflation, V3Add(x, b.supportSweepLocal(nvNorm)));
						supportB = V3Add(x, b.supportSweepLocal(nvNorm));
						//supportB = b.supportSweepLocal(V3Neg(v));
						support = V3Sub(supportA, supportB);
						minDist = maxDist;
						nor = v;
						//size=0;
					}
				}
			}

			PX_ASSERT(size < 4);
			A[size]=supportA;
			B[size]=supportB;
			Q[size++]=support;
	
			//calculate the closest point between two convex hull
			const Vec3V tempV = GJKCPairDoSimplex(Q, A, B, support, supportA, supportB, size, closA, closB);
			v = V3Neg(tempV);
			sDist = V3Dot(tempV, tempV);

			bCon = FIsGrtr(minDist, sDist);
			bNotTerminated = BAnd(FIsGrtr(sDist, inflation2), bCon);
		}

		lambda = _lambda;
		
		closestA = V3Sel(bCon, closA, closAA);
		normal = V3Neg(V3Normalize(nor));
		return true;
	}

	template<class ConvexA, class ConvexB>
	bool gjkLocalRayCast(ConvexA& a, ConvexB& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal _inflation, const bool initialOverlap)
	{
		using namespace Ps::aos;
		Vec3V closA;
		Vec3V norm; 
		FloatV _lambda;
		if(_gjkLocalRayCast(a, b, initialLambda, s, r, _lambda, norm, closA, _inflation))
		{
			if(FAllEq(_lambda, FZero()) && initialOverlap)
			{

				//time of impact is zero, the sweep shape is intesect, use epa to get the normal and contact point
				const FloatV contactDist = getSweepContactEps(a.getMargin(), b.getMargin());

				Vec3V closAA;
				Vec3V closBB;
				FloatV sDist;
				PxU8 tmp =0;
				if(gjkLocalPenetration(a, b, contactDist, closAA, closBB, norm, sDist, NULL, NULL, tmp))
				{
					closA = closAA;
				}
				else
				{
					// in the local space of B
					//closestA = closA;
					closA = closAA;
					norm= V3Normalize(V3Sub(closAA, closBB));
				}
			}
			closestA = closA;
			normal = norm;
			lambda = _lambda;

			return true;
		}
		return false;
	}


	template<class ConvexA, class ConvexB>
	bool _gjkRelativeRayCast(ConvexA& a, ConvexB& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal _inflation/*, const bool initialOverlap*/)
	{
		using namespace Ps::aos;

		const FloatV inflation = FloatV_From_F32(_inflation);
		const Vec3V zeroV = V3Zero();
		const FloatV zero = FZero();
		const FloatV eps = FEps();
		const FloatV one = FOne();
		const BoolV bTrue = BTTTT();

		const FloatV maxDist = FloatV_From_F32(PX_MAX_REAL);
	
		FloatV _lambda = zero;//initialLambda;
		Vec3V x = V3ScaleAdd(r, _lambda, s);
		PxU32 size=1;
	//	PsMatTransformV aToB = aToBRel;

	/*	const Vec3V bOriginalCenter = V3Zero(); 
		b.setCenter(x);*/
		//const Vec3V _initialSearchDir(V3Normalize(aToB.p));//(V3Sub(a.getCenter(), b.getCenter()));
		const Vec3V _initialSearchDir = V3Sel(FIsGrtr(V3Dot(aToB.p, aToB.p), eps), aToB.p, V3UnitX());
		const Vec3V initialSearchDir = V3Normalize(_initialSearchDir);
		

		const Vec3V initialSupportA(a.supportSweepRelative(V3Neg(initialSearchDir), aToB));
		//const Vec3V initialSupportB(V3ScaleAdd(initialSearchDir, inflation, b.supportSweepLocal(initialSearchDir)));
		const Vec3V initialSupportB(b.supportSweepLocal(initialSearchDir));
		 
		Vec3V Q[4] = {V3Sub(initialSupportA, initialSupportB), zeroV, zeroV, zeroV}; //simplex set
		Vec3V A[4] = {initialSupportA, zeroV, zeroV, zeroV}; //ConvexHull a simplex set
		Vec3V B[4] = {initialSupportB, zeroV, zeroV, zeroV}; //ConvexHull b simplex set
		 

		Vec3V v = V3Neg(Q[0]);
		Vec3V supportA = initialSupportA;
		Vec3V supportB = initialSupportB;
		Vec3V support = Q[0];
		//const FloatV onePerc = FloatV_From_F32(0.01f);
		//const FloatV minMargin = FMin(a.getMinMargin(), b.getMinMargin());

		////const FloatV eps2 = FAdd(sqInflation, FMul(minMargin, onePerc));
		//const FloatV eps2 = FMul(minMargin, onePerc);
		const FloatV eps1 = FloatV_From_F32(0.0001f);
		const FloatV eps2 = FMul(eps1, eps1);

		const FloatV inflation2 = FAdd(FMul(inflation, inflation), eps2);

		Vec3V closA(initialSupportA), closB(initialSupportB);
		FloatV sDist = V3Dot(v, v);
		FloatV minDist = sDist;
		Vec3V closAA = initialSupportA;
		//Vec3V closBB = initialSupportB;
		
		BoolV bNotTerminated = FIsGrtr(sDist, eps2);
		BoolV bCon = bTrue;

		Vec3V nor = v;
		
		while(BAllEq(bNotTerminated, bTrue))
		{
			
			minDist = sDist;
			closAA = closA;
			//closBB = closB;

			const Vec3V vNorm = V3Normalize(v);
			const Vec3V nvNorm = V3Neg(vNorm);

			supportA=a.supportSweepRelative(vNorm, aToB);
			//supportB=V3ScaleAdd(nvNorm, inflation, V3Add(x, b.supportSweepLocal(nvNorm)));
			supportB=V3Add(x, b.supportSweepLocal(nvNorm));
		
			//calculate the support point
			support = V3Sub(supportA, supportB);
			const Vec3V w = V3Neg(support);
			const FloatV vw = FSub(V3Dot(vNorm, w), inflation);
			const FloatV vr = V3Dot(vNorm, r);
			if(FAllGrtr(vw, zero))
			{
	
				if(FAllGrtrOrEq(vr, zero))
				{
					return false;
				}
				else
				{
					const FloatV _oldLambda = _lambda;
					_lambda = FSub(_lambda, FDiv(vw, vr));
					if(FAllGrtr(_lambda, _oldLambda))
					{
						if(FAllGrtr(_lambda, one))
						{
							return false;
						}
						const Vec3V bPreCenter = x;
						x = V3ScaleAdd(r, _lambda, s);
						//aToB.p = V3Sub(aToB.p, x);
					
						//aToB.p = V3Neg(x);
						const Vec3V offSet =V3Sub(x, bPreCenter);
						const Vec3V b0 = V3Add(B[0], offSet);
						const Vec3V b1 = V3Add(B[1], offSet);
						const Vec3V b2 = V3Add(B[2], offSet);
					
						B[0] = b0;
						B[1] = b1;
						B[2] = b2;

						Q[0]=V3Sub(A[0], b0);
						Q[1]=V3Sub(A[1], b1);
						Q[2]=V3Sub(A[2], b2);

						//supportA = a.supportSweepRelative(v, aToB);
						
						//supportB = V3Add(x, b.supportSweepLocal(V3Neg(v)));
						//supportB = V3ScaleAdd(nvNorm, inflation, V3Add(x, b.supportSweepLocal(nvNorm)));
						supportB = V3Add(x, b.supportSweepLocal(nvNorm));
						//supportB = b.supportSweepLocal(V3Neg(v));
						support = V3Sub(supportA, supportB);
						minDist = maxDist;
						nor = v;
						//size=0;
					}
				}
			}

			PX_ASSERT(size < 4);
			A[size]=supportA;
			B[size]=supportB;
			Q[size++]=support;
	
			//calculate the closest point between two convex hull
			const Vec3V tempV = GJKCPairDoSimplex(Q, A, B, support, supportA, supportB, size, closA, closB);
			v = V3Neg(tempV);
			sDist = V3Dot(tempV, tempV);

			bCon = FIsGrtr(minDist, sDist);
			bNotTerminated = BAnd(FIsGrtr(sDist, inflation2), bCon);
		}

		lambda = _lambda;
		
		closestA = V3Sel(bCon, closA, closAA);
		normal = V3Neg(V3Normalize(nor));
	
		return true;
	}

	
	template<class ConvexA, class ConvexB>
	bool gjkRelativeRayCast(ConvexA& a, ConvexB& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal _inflation, const bool initialOverlap)
	{
		using namespace Ps::aos;
		Vec3V closA;
		Vec3V norm; 
		FloatV _lambda;
		if(_gjkRelativeRayCast(a, b, aToB, initialLambda, s, r, _lambda, norm, closA, _inflation))
		{
			if(FAllEq(_lambda, FZero()) && initialOverlap)
			{

				//time of impact is zero, the sweep shape is intesect, use epa to get the normal and contact point
				const FloatV contactDist = getSweepContactEps(a.getMargin(), b.getMargin());

				Vec3V closAA;
				Vec3V closBB;
				FloatV sDist;
				PxU8 tmp =0;
				if(gjkRelativePenetration(a, b, aToB, contactDist, closAA, closBB, norm, sDist, NULL, NULL, tmp))
				{
					closA = closAA;
				}
				else
				{
					// in the local space of B
					//closestA = closA;
					closA = closAA;
					norm= V3Normalize(V3Sub(closAA, closBB));
				}
			}
			closestA = closA;
			normal = norm;
			lambda = _lambda;

			return true;
		}
		return false;
	}



#else

	bool gjkRayCast(ConvexV& a, ConvexV& b, SupportMapPair* pair, const Ps::aos::Vec3VArg initialDir, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal inflation, const bool initialOverlap);

	template<class ConvexA, class ConvexB>
	bool gjkRelativeRayCast(ConvexA& a, ConvexB& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal inflation, const bool initialOverlap)
	{
		SupportMapPairRelativeImpl<ConvexA, ConvexB> pair(a, b, aToB);
		return gjkRayCast(a, b, &pair, aToB.p, initialLambda, s, r, lambda, normal, closestA, inflation, initialOverlap);
	}

	template<class ConvexA, class ConvexB>
	bool gjkLocalRayCast(ConvexA& a, ConvexB& b, const Ps::aos::FloatVArg initialLambda, const Ps::aos::Vec3VArg s, const Ps::aos::Vec3VArg r, Ps::aos::FloatV& lambda, Ps::aos::Vec3V& normal, Ps::aos::Vec3V& closestA, const PxReal inflation, const bool initialOverlap)
	{
		using namespace Ps::aos;
		SupportMapPairLocalImpl<ConvexA, ConvexB> pair(a, b);
		const Vec3V initialDir = V3Sub(a.getCenter(), b.getCenter());
		return gjkRayCast(a, b, &pair, initialDir, initialLambda, s, r, lambda, normal, closestA, inflation, initialOverlap);
	}

	
#endif

}
}

#endif
