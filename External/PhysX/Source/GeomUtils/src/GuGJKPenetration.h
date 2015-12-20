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


#ifndef PX_GJK_PENETRATION_H
#define PX_GJK_PENETRATION_H


#include "GuConvexSupportTable.h"
#include "GuVecBox.h"
#include "GuEPA.h"
#include "GuGJKSimplex.h"

namespace physx
{
namespace Gu
{

	class ConvexV;


#ifndef	__SPU__

	//world space
	template<class ConvexA, class ConvexB>
	PxGJKStatus gjkPenetration(const ConvexA& a, const ConvexB& b, const Ps::aos::FloatVArg contactDist, Ps::aos::Vec3V& contactA, Ps::aos::Vec3V& contactB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& penetrationDepth)
	{
		//PIX_PROFILE_ZONE(GJKPenetration);
		using namespace Ps::aos;

		Vec3V A[4]; 
		Vec3V B[4];
		Vec3V Q[4];
		//Vec3V D[4]; //store the direction
		PxI32 aInd[4];
		PxI32 bInd[4];
	
		const FloatV zero = FZero();

		const BoolV aQuadratic = a.isMarginEqRadiusV();
		const BoolV bQuadratic = b.isMarginEqRadiusV();

		//const BoolV bHasMarginEqRadius = BOr(aQuadratic, bQuadratic);

		//const Vec3V centerAToCenterB =  V3Sub(b.getCenter(), a.getCenter());
		const Vec3V _initialSearchDir = V3Sub(a.getCenter(), b.getCenter());//V3Neg(centerAToCenterB);
		Vec3V v = V3Sel(FIsGrtr(V3Dot(_initialSearchDir, _initialSearchDir), zero), _initialSearchDir, V3UnitX());

		const FloatV marginA = a.getMargin();
		const FloatV marginB = b.getMargin();
		
		const FloatV minMargin = FMin(a.getMinMargin(), b.getMinMargin());

		//const FloatV eps1 = FloatV_From_F32(0.0001f);
		const FloatV eps2 = FMul(minMargin, FloatV_From_F32(0.001f));
		//const FloatV ratio = FloatV_From_F32(0.05f);
		const Vec3V zeroV = V3Zero();
		const BoolV bTrue = BTTTT();
		const BoolV bFalse = BFFFF();
		PxU32 size=0;

		//const FloatV tenthMargin = FMul(minMargin, ratio);
	
		//const FloatV sumMargin = FAdd(FAdd(marginA, marginB), tenthMargin);
		const FloatV sumMargin0 = FAdd(marginA, marginB);
		const FloatV sumMargin = FAdd(sumMargin0, contactDist);
		//const FloatV sumMargin = FAdd(marginA, marginB);
		const FloatV sqMargin = FMul(sumMargin, sumMargin);

		Vec3V closA = zeroV;
		Vec3V closB = zeroV;
		FloatV sDist = FMax();
		FloatV minDist;
		Vec3V tempClosA;
		Vec3V tempClosB;

		BoolV bNotTerminated = bTrue;
		BoolV bCon = bTrue;

		do
		{
			minDist = sDist;
			tempClosA = closA;
			tempClosB = closB;

			const Vec3V nv = V3Neg(v);

			//D[size] = v;
			const Vec3V supportA=a.supportMargin(nv, A[size], aInd[size]);
			const Vec3V supportB=b.supportMargin(v, B[size], bInd[size]);
			
			//calculate the support point
			const Vec3V support = V3Sub(supportA, supportB);
			Q[size++]=support;

		

			PX_ASSERT(size <= 4);

			const FloatV tmp = FMul(sDist, sqMargin);//FMulAdd(sDist, sqMargin, eps3);
			const FloatV vw = V3Dot(v, support);
			const FloatV sqVW = FMul(vw, vw);

			
			const BoolV bTmp1 = FIsGrtr(vw, zero);
			const BoolV bTmp2 = FIsGrtr(sqVW, tmp);
			BoolV con = BAnd(bTmp1, bTmp2);//this is the non intersect condition


			const FloatV tmp1 = FSub(sDist, vw);
			const FloatV tmp2 = FMul(eps2, sDist);
			const BoolV conGrtr = FIsGrtrOrEq(tmp2, tmp1);//this is the margin intersect condition

			const BoolV conOrconGrtr(BOr(con, conGrtr));

			if(BAllEq(conOrconGrtr, bTrue))
			{
				//size--; if you want to get the correct size, this line need to be on
				if(BAllEq(con, bFalse)) //must be true otherwise we wouldn't be in here...
				{
					const FloatV recipDist = FRsqrt(sDist);
					const FloatV dist = FRecip(recipDist);//FSqrt(sDist);
					PX_ASSERT(FAllGrtr(dist, FEps()));
					const Vec3V n = V3Scale(v, recipDist);//normalise
					/*contactA = V3Sub(closA, V3Scale(n, marginA));
					contactB = V3Add(closB, V3Scale(n, marginB));*/
					contactA = V3NegScaleSub(n, marginA, closA);
					contactB = V3ScaleAdd(n, marginB, closB);
				/*	contactA = closA;
					contactB = closB;*/
					penetrationDepth = FSub(dist, sumMargin0);
					normal = n; 
					return GJK_CONTACT;
					
				}
				else
				{
					return GJK_NON_INTERSECT;
				}
			}

			//calculate the closest point between two convex hull

			//v = GJKCPairDoSimplex(Q, A, B, size, closA, closB);
			v = GJKCPairDoSimplex(Q, A, B, aInd, bInd, support, supportA, supportB, size, closA, closB);

			sDist = V3Dot(v, v);

			bCon = FIsGrtr(minDist, sDist);
			bNotTerminated = BAnd(FIsGrtr(sDist, eps2), bCon);
		}
		while(BAllEq(bNotTerminated, bTrue));

		
		if(BAllEq(bCon, bFalse))
		{
			if(FAllGrtrOrEq(sqMargin, sDist))
			{
				//Reset back to older closest point
				closA = tempClosA;
				closB = tempClosB;
				sDist = minDist;
				v = V3Sub(closA, closB);
				//v = V3Sub(closB, closA);

				const FloatV recipDist = FRsqrt(sDist);
				const FloatV dist = FRecip(recipDist);//FSqrt(sDist);
				PX_ASSERT(FAllGrtr(dist, FEps()));
				const Vec3V n = V3Scale(v, recipDist);//normalise
				contactA = V3NegScaleSub(n, marginA, closA);
				contactB = V3ScaleAdd(n, marginB, closB);
				/*contactA = closA;
				contactB = closB;*/
				penetrationDepth = FSub(dist, sumMargin0);
				normal = n;
				return GJK_CONTACT;  
			}
			return GJK_DEGENERATE;  
		}
		else 
		{

			//return RecalculateSimplex(a, b, a.getSupportMapping(), b.getSupportMapping(),  a.getSupportPointMapping(), b.getSupportPointMapping(), aInd, bInd, size, contactA, contactB, normal, penetrationDepth, aQuadratic, bQuadratic);
			return false;
		}
	}

	//relative space, warm start
	template<class ConvexA, class ConvexB>
	PX_GJK_FORCE_INLINE PxGJKStatus gjkRelativePenetration(const ConvexA& a, const ConvexB& b,  const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg contactDist, Ps::aos::Vec3V& contactA, Ps::aos::Vec3V& contactB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& penetrationDepth,
		PxU8* __restrict aIndice, PxU8* __restrict bIndice, PxU8& _size)
	{
		//PIX_PROFILE_ZONE(GJKPenetration);
		using namespace Ps::aos;

		const FloatV zero = FZero();
		const Vec3V zeroV = V3Zero();
		const BoolV bTrue = BTTTT();
		const BoolV bFalse = BFFFF();


		PxI32 aInd[4];
		PxI32 bInd[4];

		PxU32 size = 0;//_size;

		Vec3V closA = zeroV;
		Vec3V closB = zeroV;
		FloatV sDist = FMax();

		Vec3V tempClosA = zeroV;
		Vec3V tempClosB = zeroV;
	
		
		const BoolV aQuadratic = a.isMarginEqRadiusV();
		const BoolV bQuadratic = b.isMarginEqRadiusV();

		const FloatV marginA = a.getMargin();
		const FloatV marginB = b.getMargin();
		
		const FloatV minMargin = FMin(a.getMinMargin(), b.getMinMargin());
		const FloatV eps2 = FMul(minMargin, FloatV_From_F32(0.001f));
		const FloatV eps = FEps();
		
		const FloatV sumMargin0 = FAdd(marginA, marginB);
		const FloatV sumMargin = FAdd(sumMargin0, contactDist);
		const FloatV sqMargin = FMul(sumMargin, sumMargin);

		
		FloatV minDist= sDist;
		BoolV bCon = bTrue;
		

		{
		BoolV bNotTerminated = bTrue;
		
		Vec3V v;

		Vec3V Q[4];
		Vec3V A[4];
		Vec3V B[4];
		
		
		if(_size != 0)
		{
			Vec3V supportA, supportB, support;
			
			for(PxU32 i=0; i<_size; ++i)
			{
				aInd[i] = aIndice[i];
				bInd[i] = bIndice[i];
				supportA = aToB.transform(a.supportMarginPoint(aIndice[i]));
				supportB = b.supportMarginPoint(bIndice[i]);
				support = V3Sub(supportA, supportB);

				//Get rid of the duplicate point
				BoolV match = bFalse;
				for(PxU32 na = 0; na < size; ++na)
				{
					Vec3V dif = V3Sub(Q[na], support);
					match = BOr(match, FIsGrtr(eps, V3Dot(dif, dif)));	
				}

				if(BAllEq(match, bFalse))
				{
					A[size] = supportA;
					B[size] = supportB;
					Q[size++] = support;
				}
			}

			v = GJKCPairDoSimplex(Q, A, B, aInd, bInd, support, supportA, supportB, size, closA, closB);

			sDist = V3Dot(v, v);
			minDist = sDist;
			
			bNotTerminated = FIsGrtr(sDist, eps2);
		}
		else
		{
			const Vec3V _initialSearchDir = aToB.p;//V3Sub(a.getCenter(), b.getCenter());
			v = V3Sel(FIsGrtr(V3Dot(_initialSearchDir, _initialSearchDir), zero), _initialSearchDir, V3UnitX());
		}
		
		while(BAllEq(bNotTerminated, bTrue))
		{


			
			const BoolV bA = a.supportRelativeIndex(V3Neg(v), aToB, aInd[size]);
			const BoolV bB = b.supportLocalIndex(v, bInd[size]);

			minDist = sDist;
			tempClosA = closA;
			tempClosB = closB;
			const FloatV tmp = FMul(sDist, sqMargin);//FMulAdd(sDist, sqMargin, eps3);

		//	const Vec3V nv = V3Neg(v);

		/*	const Vec3V supportA=a.supportMarginRelative(nv, aToB, A[size], aInd[size]);
			const Vec3V supportB=b.supportMarginLocal(v, B[size], bInd[size]);*/

			const Vec3V supportA = a.supportMarginRelative(A[size], aToB, aInd[size], bA);
			const Vec3V supportB = b.supportMarginLocal(B[size], bInd[size], bB);
		
			//calculate the support point
			const Vec3V support = V3Sub(supportA, supportB);
			Q[size++]=support;

			PX_ASSERT(size <= 4);

			
			const FloatV vw = V3Dot(v, support);
			const FloatV sqVW = FMul(vw, vw);

			
			const BoolV bTmp1 = FIsGrtr(vw, zero);
			const BoolV bTmp2 = FIsGrtr(sqVW, tmp);
			BoolV con = BAnd(bTmp1, bTmp2);//this is the non intersect condition


			const FloatV tmp1 = FSub(sDist, vw);
			const FloatV tmp2 = FMul(eps2, sDist);
			const BoolV conGrtr = FIsGrtrOrEq(tmp2, tmp1);//this is the margin intersect condition

			const BoolV conOrconGrtr(BOr(con, conGrtr));

			if(BAllEq(conOrconGrtr, bTrue))
			{
				if(aIndice)
				{
					PX_ASSERT(bIndice);
					const PxU32 f = size - 1;
					_size = f;
					for(PxU32 i=0; i<f; ++i)
					{
						aIndice[i] = aInd[i];
						bIndice[i] = bInd[i];
					}
				}
				//size--; if you want to get the correct size, this line need to be on
				if(BAllEq(con, bFalse)) //must be true otherwise we wouldn't be in here...
				{
					const FloatV recipDist = FRsqrt(sDist);
					const FloatV dist = FRecip(recipDist);//FSqrt(sDist);
					PX_ASSERT(FAllGrtr(dist, FEps()));
					const Vec3V n = V3Scale(v, recipDist);//normalise
					/*contactA = V3Sub(closA, V3Scale(n, marginA));
					contactB = V3Add(closB, V3Scale(n, marginB));*/
					contactA = V3NegScaleSub(n, marginA, closA);
					contactB = V3ScaleAdd(n, marginB, closB);
					penetrationDepth = FSub(dist, sumMargin0);
					normal = n; 
					return GJK_CONTACT;
					
				}
				else
				{
					return GJK_NON_INTERSECT;
				}
			}

			//calculate the closest point between two convex hull

			//v = GJKCPairDoSimplex(Q, A, B, size, closA, closB);
			v = GJKCPairDoSimplex(Q, A, B, aInd, bInd,  support, supportA, supportB, size, closA, closB);

			sDist = V3Dot(v, v);

			bCon = FIsGrtr(minDist, sDist);
			bNotTerminated = BAnd(FIsGrtr(sDist, eps2), bCon);
		}
		}

		if(aIndice)
		{
			PX_ASSERT(bIndice);
			_size = size;
			for(PxU32 i=0; i<size; ++i)
			{
				aIndice[i] = aInd[i];
				bIndice[i] = bInd[i];
			}
		}

		if(BAllEq(bCon, bFalse))
		{
			if(FAllGrtrOrEq(sqMargin, sDist))
			{
				//Reset back to older closest point
				closA = tempClosA;
				closB = tempClosB;
				sDist = minDist;
				const Vec3V dir= V3Sub(closA, closB);
				const FloatV recipDist = FRsqrt(sDist);
				const FloatV dist = FRecip(recipDist);//FSqrt(sDist);
				PX_ASSERT(FAllGrtr(dist, FEps()));
				const Vec3V n = V3Scale(dir, recipDist);//normalise
				contactA = V3NegScaleSub(n, marginA, closA);
				contactB = V3ScaleAdd(n, marginB, closB);
				penetrationDepth = FSub(dist, sumMargin0);
				normal = n;
				return GJK_CONTACT;    
			}
			return GJK_DEGENERATE; 
		}
		else 
		{
		

			//SupportMapPairRelativeImpl<ConvexA, ConvexB> supportMap(a, b, aToB);
			/*SupportMapRelativeImpl<ConvexA> map1(a, aToB);
			SupportMapLocalImpl<ConvexB> map2(b);*/

			SupportMapPairRelativeImpl<ConvexA, ConvexB> supportMap(a, b, aToB);

			return RecalculateSimplex(a, b, &supportMap, aInd, bInd, size, contactA, contactB, normal, penetrationDepth, aQuadratic, bQuadratic);
			
		}
	}


	template<class ConvexA, class ConvexB>
	PX_GJK_FORCE_INLINE PxGJKStatus gjkLocalPenetration(const ConvexA& a, const ConvexB& b, const Ps::aos::FloatVArg contactDist, Ps::aos::Vec3V& contactA, Ps::aos::Vec3V& contactB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& penetrationDepth,
		PxU8* __restrict aIndice, PxU8* __restrict bIndice, PxU8& _size)
	{
		//PIX_PROFILE_ZONE(GJKPenetration);
		using namespace Ps::aos;

	
		const FloatV zero = FZero();

		const BoolV aQuadratic = a.isMarginEqRadiusV();
		const BoolV bQuadratic = b.isMarginEqRadiusV();


		const FloatV marginA = a.getMargin();
		const FloatV marginB = b.getMargin();
		
	//	const FloatV minMargin = FMin(marginA, marginB);
		const FloatV minMargin = FMin(a.getMinMargin(), b.getMinMargin());

		//const FloatV eps1 = FloatV_From_F32(0.0001f);
		const FloatV eps2 = FMul(minMargin, FloatV_From_F32(0.001f));
		const FloatV eps = FEps();
		//const FloatV ratio = FloatV_From_F32(0.05f);
		const Vec3V zeroV = V3Zero();
		const BoolV bTrue = BTTTT();
		const BoolV bFalse = BFFFF();
		//PxU32 size=0;
	
		const FloatV sumMargin0 = FAdd(marginA, marginB);
		const FloatV sumMargin = FAdd(sumMargin0, contactDist);
		const FloatV sqMargin = FMul(sumMargin, sumMargin);

		Vec3V closA = zeroV;
		Vec3V closB = zeroV;
		FloatV sDist = FMax();
		FloatV minDist= sDist;
		Vec3V tempClosA = zeroV;
		Vec3V tempClosB = zeroV;

		BoolV bNotTerminated = bTrue;
		BoolV bCon = bTrue;
		Vec3V v;

		Vec3V Q[4];
		Vec3V A[4];
		Vec3V B[4];
		PxI32 aInd[4];
		PxI32 bInd[4];
		Vec3V supportA, supportB, support;

		PxU32 size = 0;//_size;
		
		if(_size != 0)
		{
			for(PxU32 i=0; i<_size; ++i)
			{
				aInd[i] = aIndice[i];
				bInd[i] = bIndice[i];
				supportA = a.supportMarginPoint(aIndice[i]);
				supportB = b.supportMarginPoint(bIndice[i]);
				support = V3Sub(supportA, supportB);
				//Get rid of the duplicate point
				BoolV match = bFalse;
				for(PxU32 na = 0; na < size; ++na)
				{
					const Vec3V dif = V3Sub(Q[na], support);
					match = BOr(match, FIsGrtr(eps, V3Dot(dif, dif)));	
				}

				if(BAllEq(match, bFalse))
				{
					A[size] = supportA;
					B[size] = supportB;
					Q[size++] = support;
				}
			}

			v = GJKCPairDoSimplex(Q, A, B, aInd, bInd, support, supportA, supportB, size, closA, closB);

			sDist = V3Dot(v, v);
			minDist = sDist;
			
			bNotTerminated = FIsGrtr(sDist, eps2);
		}
		else
		{
			const Vec3V _initialSearchDir = V3Sub(a.getCenter(), b.getCenter());
			v = V3Sel(FIsGrtr(V3Dot(_initialSearchDir, _initialSearchDir), zero), _initialSearchDir, V3UnitX());
		}
		
		while(BAllEq(bNotTerminated, bTrue))
		{
			
			//ML: don't change the order, otherwise, we will get LHS penalty in Xbox
			const BoolV bB = b.supportLocalIndex(v, bInd[size]);
			//const Vec3V nv = V3Neg(v);
			const BoolV bA = a.supportLocalIndex(V3Neg(v), aInd[size]);

			minDist = sDist;
			tempClosA = closA;
			tempClosB = closB;
			const FloatV tmp = FMul(sDist, sqMargin);//FMulAdd(sDist, sqMargin, eps3);
				
			
			supportA=a.supportMarginLocal(A[size], aInd[size], bA);
			supportB=b.supportMarginLocal(B[size], bInd[size], bB);
			
		
			//calculate the support point
			support = V3Sub(supportA, supportB);
			Q[size++]=support;

			PX_ASSERT(size <= 4);

			const FloatV vw = V3Dot(v, support);
			const FloatV sqVW = FMul(vw, vw);

			
			const BoolV bTmp1 = FIsGrtr(vw, zero);
			const BoolV bTmp2 = FIsGrtr(sqVW, tmp);
			BoolV con = BAnd(bTmp1, bTmp2);//this is the non intersect condition


			const FloatV tmp1 = FSub(sDist, vw);
			const FloatV tmp2 = FMul(eps2, sDist);
			const BoolV conGrtr = FIsGrtrOrEq(tmp2, tmp1);//this is the margin intersect condition

			const BoolV conOrconGrtr(BOr(con, conGrtr));

			if(BAllEq(conOrconGrtr, bTrue))
			{
				if(aIndice)
				{
					PX_ASSERT(bIndice);
					const PxU32 f = size - 1;
					_size = f;
					for(PxU32 i=0; i<f; ++i)
					{
						aIndice[i] = aInd[i];
						bIndice[i] = bInd[i];
					}
				}
				//size--; if you want to get the correct size, this line need to be on
				if(BAllEq(con, bFalse)) //must be true otherwise we wouldn't be in here...
				{
					const FloatV recipDist = FRsqrt(sDist);
					const FloatV dist = FRecip(recipDist);//FSqrt(sDist);
					PX_ASSERT(FAllGrtr(dist, FEps()));
					const Vec3V n = V3Scale(v, recipDist);//normalise
					/*contactA = V3Sub(closA, V3Scale(n, marginA));
					contactB = V3Add(closB, V3Scale(n, marginB));*/
					contactA = V3NegScaleSub(n, marginA, closA);
					contactB = V3ScaleAdd(n, marginB, closB);
					penetrationDepth = FSub(dist, sumMargin0);
					normal = n; 
					return GJK_CONTACT;
					
				}
				else
				{
					return GJK_NON_INTERSECT;
				}
			}

			//calculate the closest point between two convex hull

			//v = GJKCPairDoSimplex(Q, A, B, size, closA, closB);
			v = GJKCPairDoSimplex(Q, A, B, aInd, bInd, support, supportA, supportB,  size, closA, closB);

			sDist = V3Dot(v, v);

			bCon = FIsGrtr(minDist, sDist);
			bNotTerminated = BAnd(FIsGrtr(sDist, eps2), bCon);
		}
		
		if(aIndice)
		{
			PX_ASSERT(bIndice);
			_size = size;
			for(PxU32 i=0; i<size; ++i)
			{
				aIndice[i] = aInd[i];
				bIndice[i] = bInd[i];
			}
		}

		if(BAllEq(bCon, bFalse))
		{
			if(FAllGrtrOrEq(sqMargin, sDist))
			{
				//Reset back to older closest point
				closA = tempClosA;
				closB = tempClosB;
				sDist = minDist;
				v = V3Sub(closA, closB);
				const FloatV recipDist = FRsqrt(sDist);
				const FloatV dist = FRecip(recipDist);//FSqrt(sDist);
				PX_ASSERT(FAllGrtr(dist, FEps()));
				const Vec3V n = V3Scale(v, recipDist);//normalise
				contactA = V3NegScaleSub(n, marginA, closA);
				contactB = V3ScaleAdd(n, marginB, closB);
				penetrationDepth = FSub(dist, sumMargin0);
				normal = n;
				return GJK_CONTACT;  
			}
			return GJK_DEGENERATE;  
		}
		else 
		{
			SupportMapPairLocalImpl<ConvexA, ConvexB> supportMap(a, b);

			return RecalculateSimplex(a, b, &supportMap, aInd, bInd, size, contactA, contactB, normal, penetrationDepth, aQuadratic, bQuadratic);
			
		}
	}

#else


	PxGJKStatus gjkPenetration(const ConvexV& a, const ConvexV& b, SupportMapPair* pair, const Ps::aos::Vec3VArg initialDir,  const Ps::aos::FloatVArg contactDist, Ps::aos::Vec3V& contactA, Ps::aos::Vec3V& contactB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& penetrationDepth,
		PxU8* __restrict  aIndice, PxU8* __restrict bIndice, PxU8& _size);
	
	template<typename ConvexA, typename ConvexB>
	PxGJKStatus gjkRelativePenetration(const ConvexA& a, const ConvexB& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg contactDist, Ps::aos::Vec3V& contactA, Ps::aos::Vec3V& contactB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& penetrationDepth)
	{
		SupportMapPairRelativeImpl<ConvexA, ConvexB> supportMap(a, b, aToB);
		return gjkPenetration(a, b,  &supportMap, aToB.p, contactDist, contactA, contactB, normal, penetrationDepth, NULL, NULL, 0);
	}


	template<class ConvexA, class ConvexB>
	PxGJKStatus gjkRelativePenetration(const ConvexA& a, const ConvexB& b, const Ps::aos::PsMatTransformV& aToB, const Ps::aos::FloatVArg contactDist, Ps::aos::Vec3V& contactA, Ps::aos::Vec3V& contactB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& penetrationDepth,
		PxU8* __restrict  aIndice, PxU8* __restrict bIndice, PxU8& _size)
	{
		SupportMapPairRelativeImpl<ConvexA, ConvexB> supportMap(a, b, aToB);
		return gjkPenetration(a, b, &supportMap, aToB.p, contactDist, contactA, contactB, normal, penetrationDepth, aIndice, bIndice, _size);
	}


	template<class ConvexA, class ConvexB>
	PxGJKStatus gjkLocalPenetration(const ConvexA& a, const ConvexB& b, const Ps::aos::FloatVArg contactDist, Ps::aos::Vec3V& contactA, Ps::aos::Vec3V& contactB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& penetrationDepth,
		PxU8* __restrict  aIndice, PxU8* __restrict bIndice, PxU8& _size)
	{
		using namespace Ps::aos;

		SupportMapPairLocalImpl<ConvexA, ConvexB> supportMap(a, b);
		const Vec3V initialDir = V3Sub(a.getCenter(), b.getCenter());
		return gjkPenetration(a, b, &supportMap, initialDir, contactDist, contactA, contactB, normal, penetrationDepth, aIndice, bIndice, _size);
	}
	

#endif

}

}

#endif
