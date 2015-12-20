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

#ifndef PX_PHYSICS_GEOMUTILS_VEC_CONVEXHULL_H
#define PX_PHYSICS_GEOMUTILS_VEC_CONVEXHULL_H

#include "GuVecConvex.h"
#include "GuConvexMeshData.h"
#include "GuBigConvexData.h"
#include "GuConvexSupportTable.h"
#include "GuCubeIndex.h"
#include "PsFPU.h"

#if defined(PX_X360) || defined(PX_PS3) || defined(__SPU__)
#define PX_USE_BRANCHLESS_SUPPORT
#endif

#ifdef __SPU__
#define PX_SUPPORT_INLINE __attribute__((noinline))
#define PX_SUPPORT_FORCE_INLINE PX_INLINE
#else
#define PX_SUPPORT_INLINE PX_FORCE_INLINE
#define PX_SUPPORT_FORCE_INLINE PX_FORCE_INLINE
#endif

namespace physx
{
namespace Gu
{



	PX_SUPPORT_FORCE_INLINE Ps::aos::FloatV CalculatePCMConvexMargin(const Gu::ConvexHullData* hullData, const Ps::aos::Vec3VArg scale)
	{
		using namespace Ps::aos;
		const Vec3V minimum = Vec3V_From_PxVec3(hullData->mAABB.minimum);
		const Vec3V maximum = Vec3V_From_PxVec3(hullData->mAABB.maximum);
		
		const Vec3V extents = V3Mul(V3Scale(V3Sub(maximum, minimum), FHalf()), scale);
		const FloatV perc = FloatV_From_F32(0.075f);
		//const FloatV perc = FloatV_From_F32(0.07f);

		const FloatV min = V3ExtractMin(extents);//FMin(V3GetX(extents), FMin(V3GetY(extents), V3GetZ(extents)));
		return FMul(min, perc);

		/*using namespace Ps::aos;
		const FloatV _extents = FloatV_From_F32(hullData->mInternal.mRadius);
		const FloatV extents = FMul(_extents, scale);
		const FloatV perc = FloatV_From_F32(0.15f);
		return FMul(extents, perc);*/

		
	}

	
	PX_SUPPORT_FORCE_INLINE Ps::aos::FloatV CalculateConvexTolerance(const Gu::ConvexHullData* hullData, const Ps::aos::Vec3VArg scale)
	{

		using namespace Ps::aos;
		const Vec3V _extents =  Vec3V_From_F32Array(hullData->mInternal.mExtents);
		const Vec3V extents = V3Mul(_extents, scale);
		const FloatV ratio1 = Ps::aos::FloatV_From_F32(0.01f);
		const FloatV min = V3ExtractMin(extents);
		return FMul(min, ratio1);
	}


	PX_SUPPORT_FORCE_INLINE Ps::aos::FloatV CalculateConvexMargin(const Gu::ConvexHullData* hullData, const Ps::aos::Vec3VArg scale)
	{

		using namespace Ps::aos;
		const Vec3V _extents =  Vec3V_From_F32Array(hullData->mInternal.mExtents);
		const FloatV ratio1 = Ps::aos::FloatV_From_F32(0.01f);
		const FloatV ratio2 = Ps::aos::FloatV_From_F32(0.2f);

		const Vec3V extents = V3Mul(_extents, scale);
		
		const FloatV min = V3ExtractMin(extents);
		const FloatV max = V3ExtractMax(extents);
		const FloatV _min = FMul(min, ratio2);
		const FloatV _max = FMul(max, ratio1);

		return FMin(min, FMax(_min, _max));

		//const Vec3V _extents = Vec3V_From_PxVec3((PxVec3&)hullData->mInternal.mRadius);
		//const Vec3V extents = V3Mul(_extents, scale);
		//const FloatV perc = FloatV_From_F32(0.2f);
		//return FMul(extents, perc);

	}



	PX_SUPPORT_FORCE_INLINE void CalculateConvexMargin(const Gu::ConvexHullData* hullData, Ps::aos::FloatV& margin, Ps::aos::FloatV& minMargin, const Ps::aos::Vec3VArg scale)
	{

		using namespace Ps::aos;
		const Vec3V _extents = Vec3V_From_F32Array(hullData->mInternal.mExtents);
		const FloatV ratio1 = Ps::aos::FloatV_From_F32(0.05f);
		const FloatV ratio2 = Ps::aos::FloatV_From_F32(0.2f);
		const FloatV ratio = Ps::aos::FloatV_From_F32(0.1f);

		const Vec3V extents = V3Mul(_extents, scale);
		
		const FloatV min = V3ExtractMin(extents);
		const FloatV max = V3ExtractMax(extents);
		const FloatV _min = FMul(min, ratio2);
		const FloatV _max = FMul(max, ratio1);

		margin = FMin(min, FMax(_min, _max));
		minMargin = FMul(min, ratio);
	}



	PX_SUPPORT_FORCE_INLINE Ps::aos::Mat33V ConstructSkewMatrix(const Ps::aos::Vec3VArg scale, const Ps::aos::QuatVArg rotation) 
	{
		using namespace Ps::aos;
		const Mat33V rot = QuatGetMat33V(rotation);
		Mat33V trans = M33Trnsps(rot);
		trans.col0 = V3Scale(trans.col0, V3GetX(scale));
		trans.col1 = V3Scale(trans.col1, V3GetY(scale));
		trans.col2 = V3Scale(trans.col2, V3GetZ(scale));
		return M33MulM33(trans, rot);
	}

	PX_SUPPORT_FORCE_INLINE void ConstructSkewMatrix(const Ps::aos::Vec3VArg scale, const Ps::aos::QuatVArg rotation, Ps::aos::Mat33V& vertex2Shape) 
	{
		using namespace Ps::aos;
		const Mat33V rot = QuatGetMat33V(rotation);
		const Mat33V trans = M33Trnsps(rot);
		/*
			vertex2shape
			skewMat = Inv(R)*Diagonal(scale)*R;
		*/

		const Mat33V temp(V3Scale(trans.col0, V3GetX(scale)), V3Scale(trans.col1, V3GetY(scale)), V3Scale(trans.col2, V3GetZ(scale)));
		vertex2Shape = M33MulM33(temp, rot);

		//don't need it in the support function
		///*
		//	shape2Vertex
		//	invSkewMat =(invSkewMat)= Inv(R)*Diagonal(1/scale)*R;
		//*/
		//const Vec3V invScale = V3Recip(scale);
		//shape2Vertex.col0 = V3Scale(trans.col0, V3GetX(invScale));
		//shape2Vertex.col1 = V3Scale(trans.col1, V3GetY(invScale));
		//shape2Vertex.col2 = V3Scale(trans.col2, V3GetZ(invScale));
		//shape2Vertex = M33MulM33(shape2Vertex, rot);

	}


	//PX_FORCE_INLINE PxVec3 intersectPlanes(const PxPlane& p1, const PxPlane& p2, const PxPlane& p3)
	//{
	////	const PxF32 eps = (1e-6f);
	//	const PxVec3 u = p2.n.cross(p3.n);
	//	const PxF32 denom = p1.n.dot(u);
	//	PX_ASSERT(PxAbs(denom) > (1e-6f));
	//	//if(abs(denom) < eps)return 0;
	//	const PxVec3 temp = (p3.n * p2.d) - (p2.n * p3.d); 
	//	return (p1.n.cross(temp) - (u*p1.d))/denom;
	//}


	/*PX_FORCE_INLINE Ps::aos::Vec3V intersectPlanes(const GuVecPlane& p1, const GuVecPlane& p2, const GuVecPlane& p3)
	{
		using namespace Ps::aos;
		const Vec3V u = V3Cross(p2.n, p3.n);
		const FloatV denom = V3Dot(p1.n, u);
		const Vec3V temp = V3Sub(V3Scale(p3.n, p2.d), V3Scale(p2.n, p3.d));
		const Vec3V p = V3Sub(V3Cross(p1.n, temp), V3Scale(u, p1.d));
		return V3ScaleInv(p, denom);
	}*/

	PX_SUPPORT_FORCE_INLINE Ps::aos::Vec3V intersectPlanes(const Ps::aos::Vec3VArg n1, const Ps::aos::FloatVArg d1,
															const Ps::aos::Vec3VArg n2, const Ps::aos::FloatVArg d2,
															const Ps::aos::Vec3VArg n3, const Ps::aos::FloatVArg d3)
	{
		using namespace Ps::aos;
		const Vec3V u = V3Cross(n2, n3);
		const FloatV denom = V3Dot(n1, u);
		const Vec3V temp = V3NegScaleSub(n2, d3, V3Scale(n3, d2));
		const Vec3V p = V3NegScaleSub(u, d1, V3Cross(n1, temp));
		return V3ScaleInv(p, denom);
	}

	


	class ConvexHullV : public ConvexV
	{

		class TinyBitMap
		{
		public:
			PxU32 m[8];
			PX_FORCE_INLINE TinyBitMap()			{ m[0] = m[1] = m[2] = m[3] = m[4] = m[5] = m[6] = m[7] = 0;	}
			PX_FORCE_INLINE void set(PxU8 v)		{ m[v>>5] |= 1<<(v&31);											}
			PX_FORCE_INLINE bool get(PxU8 v) const	{ return (m[v>>5] & 1<<(v&31)) != 0;							}
		};


		public:
		/**
		\brief Constructor
		*/
		PX_SUPPORT_INLINE ConvexHullV(): ConvexV(E_CONVEXHULL)
		{
		}

		PX_SUPPORT_INLINE ConvexHullV(const Gu::ConvexHullData* _hullData, const Ps::aos::Vec3VArg _center, const Ps::aos::Vec3VArg scale, const Ps::aos::QuatVArg scaleRot):
													ConvexV(E_CONVEXHULL, _center)
		{
			using namespace Ps::aos;

			hullData = _hullData;	
			const PxVec3* __restrict tempVerts = _hullData->getHullVertices();
			//const PxU8* __restrict polyInds = _hullData->getFacesByVertices8();
			//const HullPolygonData* __restrict polygons = _hullData->mPolygons;
			verts = tempVerts;
			numVerts = _hullData->mNbHullVertices;
			CalculateConvexMargin( _hullData, margin, minMargin, scale);
			ConstructSkewMatrix(scale, scaleRot, skew);

		//	searchIndex = 0;
			data = _hullData->mBigConvexRawData;

			PxU8* startAddress = (PxU8*)_hullData->mPolygons;
			PxI32 totalPrefetchBytes = (_hullData->getFacesByVertices8() + _hullData->mNbHullVertices * 3) - startAddress;

			//Prefetch core data
			
			while(totalPrefetchBytes > 0)
			{
				totalPrefetchBytes -= 128;
				Ps::prefetch128(startAddress);
				startAddress += 128;
			}

			if(data)
			{
				PxI32 totalSize = data->mNbSamples + data->mNbVerts * sizeof(Gu::Valency) + data->mNbAdjVerts;
				startAddress = data->mSamples;
				while(totalSize > 0)
				{
					totalSize -= 128;
					Ps::prefetch128(startAddress);
					startAddress += 128;
				}
			}
			
		}


		PX_SUPPORT_INLINE ConvexHullV(const Gu::ConvexHullData* _hullData, const Ps::aos::Vec3VArg _center, const Ps::aos::FloatVArg _margin, const Ps::aos::FloatVArg _minMargin, const Ps::aos::Vec3VArg scale, const Ps::aos::QuatVArg scaleRot) : 
													ConvexV(E_CONVEXHULL, _center)
		{
			using namespace Ps::aos;

			hullData = _hullData;
			margin = _margin;
			minMargin = _minMargin;
	
			const PxVec3* tempVerts = _hullData->getHullVertices();
			const PxU8* __restrict polyInds = _hullData->getFacesByVertices8();
			const HullPolygonData* __restrict polygons = _hullData->mPolygons;
			verts = tempVerts;
			numVerts = _hullData->mNbHullVertices;

			ConstructSkewMatrix(scale, scaleRot, skew);

			Ps::prefetch128(tempVerts);
			Ps::prefetch128(tempVerts,128);
			Ps::prefetch128(tempVerts,256);

			Ps::prefetch128(polyInds);
			Ps::prefetch128(polyInds,128);

			Ps::prefetch128(polygons);
			Ps::prefetch128(polygons, 128);
			Ps::prefetch128(polygons, 256);
		}


		PX_SUPPORT_INLINE void initialize(const Gu::ConvexHullData* _hullData, const Ps::aos::Vec3VArg _center, /*const Ps::aos::Mat33V& _rot,*/ const Ps::aos::Vec3VArg scale, const Ps::aos::QuatVArg scaleRot)
		{	
			using namespace Ps::aos;
			
			const PxVec3* tempVerts = _hullData->getHullVertices();
			CalculateConvexMargin(_hullData, margin, minMargin, scale);
			ConstructSkewMatrix(scale, scaleRot, skew);

			verts = tempVerts;
			numVerts = _hullData->mNbHullVertices;
			//rot = _rot;	
			
			center = _center;

		//	searchIndex = 0;
			data = _hullData->mBigConvexRawData;

			hullData = _hullData;
			if(_hullData->mBigConvexRawData)
			{
				Ps::prefetch128(hullData->mBigConvexRawData->mValencies);
				Ps::prefetch128(hullData->mBigConvexRawData->mValencies,128);
				Ps::prefetch128(hullData->mBigConvexRawData->mAdjacentVerts);
			}
		}



		PX_FORCE_INLINE Ps::aos::Vec3V supportMarginPoint(const PxI32 index)const
		{
			return planeShift(index, margin);
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportPoint(const PxI32 index)const
		{
			using namespace Ps::aos;
			//return planeShift(index, margin);
			return M33MulV3(skew, Vec3V_From_PxVec3(verts[index]));
		}

		//get the support point in vertex space
		PX_SUPPORT_INLINE Ps::aos::Vec3V planeShift(const PxU32 index, const Ps::aos::FloatVArg margin)const
		{
			using namespace Ps::aos;

			//calculate the support point for the core(shrunk) shape
			const PxU8* __restrict polyInds = hullData->getFacesByVertices8();

			//transfrom the vertex from vertex space to shape space
			const Vec3V p = M33MulV3(skew, Vec3V_From_PxVec3(verts[index]));

			const PxU32 ind = index*3;

			const PxPlane& data1 = hullData->mPolygons[polyInds[ind]].mPlane;
			const PxPlane& data2 = hullData->mPolygons[polyInds[ind+1]].mPlane;
			const PxPlane& data3 = hullData->mPolygons[polyInds[ind+2]].mPlane;

			//transform the normal from vertex space to shape space
			//This is only required if the scale is not uniform
			const Vec3V n1 = V3Normalize(M33MulV3(skew, Vec3V_From_PxVec3(data1.n)));
			const Vec3V n2 = V3Normalize(M33MulV3(skew, Vec3V_From_PxVec3(data2.n)));
			const Vec3V n3 = V3Normalize(M33MulV3(skew, Vec3V_From_PxVec3(data3.n)));

			//This is only required if the scale is not 1
			const FloatV d1 = FSub(margin, V3Dot(p, n1));
			const FloatV d2 = FSub(margin, V3Dot(p, n2));
			const FloatV d3 = FSub(margin, V3Dot(p, n3));


			//This is unavoidable unless we pre-calc the core shape
			return intersectPlanes(n1, d1, n2, d2, n3, d3);
		}
	

#ifndef PX_USE_BRANCHLESS_SUPPORT

		//points are in vertex space
		PX_SUPPORT_INLINE PxU32 supportVertexIndex(const Ps::aos::Vec3VArg _dir)const
		{
			using namespace Ps::aos;
			if(data)
			{
				const Gu::Valency* valency = data->mValencies;
				const PxU8* adjacentVerts = data->mAdjacentVerts;
				
				//NotSoTinyBitMap visited;
				PxU32 smallBitMap[8] = {0,0,0,0,0,0,0,0};

			//	PxU32 index = searchIndex;
				PxU32 index = 0;

				{
					PxVec3 vertexSpaceDirection;
					PxVec3_From_Vec3V(_dir, vertexSpaceDirection);
					const PxU32 offset = ComputeCubemapNearestOffset(vertexSpaceDirection, data->mSubdiv);
					//const PxU32 offset = ComputeCubemapOffset(vertexSpaceDirection, data->mSubdiv);
					index = data->mSamples[offset];
				}

				Vec3V maxPoint = Vec3V_From_PxVec3(verts[index]);
				FloatV max = V3Dot(maxPoint, _dir);
		
				PxU32 initialIndex = index;

				
				do
				{
					initialIndex = index;
					const PxU32 numNeighbours = valency[index].mCount;
					const PxU32 offset = valency[index].mOffset;

					for(PxU32 a = 0; a < numNeighbours; ++a)
					{
						const PxU32 neighbourIndex = adjacentVerts[offset + a];

						const Vec3V vertex = Vec3V_From_PxVec3(verts[neighbourIndex]);
						const FloatV dist = V3Dot(vertex, _dir);
						if(FAllGrtr(dist, max))
						{
							const PxU32 ind = neighbourIndex>>5;
							const PxU32 mask = 1 << (neighbourIndex & 31);
							if((smallBitMap[ind] & mask) == 0)
							{
								smallBitMap[ind] |= mask;
								max = dist;
								index = neighbourIndex;
							}
						}
					}
				}while(index != initialIndex);

			//	searchIndex = index;

				return index;
			}
			else
			{
				//brute force
				//get the support point from the orignal margin
				FloatV max = V3Dot(Vec3V_From_PxVec3(verts[0]), _dir);
				PxU32 maxIndex=0;

				for(PxU32 i = 1; i < numVerts; ++i)
				{
					const Vec3V vertex = Vec3V_From_PxVec3(verts[i]);
					const FloatV dist = V3Dot(vertex, _dir);
					if(FAllGrtr(dist, max))
					{
						max = dist;
						maxIndex = i;
					}
				}

				return maxIndex;
			}
		}

		PX_SUPPORT_INLINE void supportVertexIndex(const Ps::aos::Vec3VArg _dir, PxI32& maxIndex)const
		{
			using namespace Ps::aos;
			if(data)
			{
				const Gu::Valency* valency = data->mValencies;
				const PxU8* adjacentVerts = data->mAdjacentVerts;
				
				//NotSoTinyBitMap visited;
				PxU32 smallBitMap[8] = {0,0,0,0,0,0,0,0};

			//	PxU32 index = searchIndex;
				PxU32 index = 0;

				{
					PxVec3 vertexSpaceDirection;
					PxVec3_From_Vec3V(_dir, vertexSpaceDirection);
					const PxU32 offset = ComputeCubemapNearestOffset(vertexSpaceDirection, data->mSubdiv);
					//const PxU32 offset = ComputeCubemapOffset(vertexSpaceDirection, data->mSubdiv);
					index = data->mSamples[offset];
				}

				Vec3V maxPoint = Vec3V_From_PxVec3(verts[index]);
				FloatV max = V3Dot(maxPoint, _dir);
		
				PxU32 initialIndex = index;

				
				do
				{
					initialIndex = index;
					const PxU32 numNeighbours = valency[index].mCount;
					const PxU32 offset = valency[index].mOffset;

					for(PxU32 a = 0; a < numNeighbours; ++a)
					{
						const PxU32 neighbourIndex = adjacentVerts[offset + a];

						const Vec3V vertex = Vec3V_From_PxVec3(verts[neighbourIndex]);
						const FloatV dist = V3Dot(vertex, _dir);
						if(FAllGrtr(dist, max))
						{
							const PxU32 ind = neighbourIndex>>5;
							const PxU32 mask = 1 << (neighbourIndex & 31);
							if((smallBitMap[ind] & mask) == 0)
							{
								smallBitMap[ind] |= mask;
								max = dist;
								index = neighbourIndex;
							}
						}
					}
				}while(index != initialIndex);

			//	searchIndex = index;

				maxIndex = index;
			}
			else
			{
				//brute force
				//get the support point from the orignal margin
				FloatV max = V3Dot(Vec3V_From_PxVec3(verts[0]), _dir);
				PxU32 besIndex=0;

				for(PxU32 i = 1; i < numVerts; ++i)
				{
					const Vec3V vertex = Vec3V_From_PxVec3(verts[i]);
					const FloatV dist = V3Dot(vertex, _dir);
					if(FAllGrtr(dist, max))
					{
						max = dist;
						besIndex = i;
					}
				}

				maxIndex = besIndex;
				//return maxIndex;
			}
		}

#else

		//points are in vertex space
		PX_SUPPORT_INLINE PxU32 supportVertexIndex(const Ps::aos::Vec3VArg _dir)const
		{
			using namespace Ps::aos;

			if(data)
			{
				const Gu::Valency* valency = data->mValencies;
				const PxU8* adjacentVerts = data->mAdjacentVerts;
				
				//NotSoTinyBitMap visited;
				PxU32 smallBitMap[8] = {0,0,0,0,0,0,0,0};

	//			PxU32 index = searchIndex;

				PxU32 index = 0;

				{
					PxVec3 vertexSpaceDirection;
					PxVec3_From_Vec3V(_dir, vertexSpaceDirection);
					const PxU32 offset = ComputeCubemapNearestOffset(vertexSpaceDirection, data->mSubdiv);
					//const PxU32 offset = ComputeCubemapOffset(vertexSpaceDirection, data->mSubdiv);
					index = data->mSamples[offset];
				}

				Vec3V maxPoint = Vec3V_From_PxVec3(verts[index]);
				FloatV max = V3Dot(maxPoint, _dir);
		
				PxU32 initialIndex = index;

				
				do
				{
					initialIndex = index;
					const PxU32 numNeighbours = valency[index].mCount;
					const PxU32 offset = valency[index].mOffset;

					for(PxU32 a = 0; a < numNeighbours; ++a)
					{
						const PxU32 neighbourIndex = adjacentVerts[offset + a];

						const Vec3V vertex = Vec3V_From_PxVec3(verts[neighbourIndex]);
						const FloatV dist = V3Dot(vertex, _dir);
						if(FAllGrtr(dist, max))
						{
							const PxU32 ind = neighbourIndex>>5;
							const PxU32 mask = 1 << (neighbourIndex & 31);
							if((smallBitMap[ind] & mask) == 0)
							{
								smallBitMap[ind] |= mask;
								max = dist;
								index = neighbourIndex;
							}
						}
					}
				}while(index != initialIndex);

			//	searchIndex = index;

				return index;
			}
			else
			{

				FloatV max = V3Dot(Vec3V_From_PxVec3(verts[0]), _dir);

				
				const VecI32V vIOne = VecI32V_One();
				const VecI32V vZero = VecI32V_Zero();
				VecI32V index = vZero;
				VecI32V bestInd = vZero;

				const PxU32 numIterations = (numVerts-1)/4;
				
				PxU32 i = 1;
				for(PxU32 a = 0; a < numIterations; ++a)
				{
					Ps::prefetch128(&verts[i+4]);
					const Vec3V p0 = Vec3V_From_PxVec3(verts[i++]);
					const Vec3V p1 = Vec3V_From_PxVec3(verts[i++]);
					const Vec3V p2 = Vec3V_From_PxVec3(verts[i++]);
					const Vec3V p3 = Vec3V_From_PxVec3(verts[i++]);

					const FloatV d0 = V3Dot(p0, _dir);
					const FloatV d1 = V3Dot(p1, _dir);
					const FloatV d2 = V3Dot(p2, _dir);
					const FloatV d3 = V3Dot(p3, _dir);

					const BoolV con0 = FIsGrtr(d0, max);
					index = VecI32V_Add(index, vIOne);
					max = FSel(con0, d0, max);
					bestInd = VecI32V_Sel(con0, index, bestInd);

					const BoolV con1 = FIsGrtr(d1, max);
					index = VecI32V_Add(index, vIOne);
					max = FSel(con1, d1, max);
					bestInd = VecI32V_Sel(con1, index, bestInd);

					const BoolV con2 = FIsGrtr(d2, max);
					index = VecI32V_Add(index, vIOne);
					max = FSel(con2, d2, max);
					bestInd = VecI32V_Sel(con2, index, bestInd);

					const BoolV con3 = FIsGrtr(d3, max);
					index = VecI32V_Add(index, vIOne);
					max = FSel(con3, d3, max);
					bestInd = VecI32V_Sel(con3, index, bestInd);
				}
				for(; i < numVerts; ++i)
				{
					index = VecI32V_Add(index, vIOne);
					const Vec3V vertex = Vec3V_From_PxVec3(verts[i]);
					const FloatV dist = V3Dot(vertex, _dir);

					const BoolV isGrtr = FIsGrtr(dist, max);
					max = FSel(isGrtr, dist, max);
					bestInd = VecI32V_Sel(isGrtr, index, bestInd);
				}
		
				PxI32 maxIndex;
				PxI32_From_VecI32V(bestInd, &maxIndex);
				return maxIndex;
			}
		}

				//points are in vertex space
		PX_SUPPORT_INLINE void supportVertexIndex(const Ps::aos::Vec3VArg _dir, PxI32& maxIndex)const
		{
			using namespace Ps::aos;

			if(data)
			{
				const Gu::Valency* valency = data->mValencies;
				const PxU8* adjacentVerts = data->mAdjacentVerts;
				
				//NotSoTinyBitMap visited;
				PxU32 smallBitMap[8] = {0,0,0,0,0,0,0,0};

	//			PxU32 index = searchIndex;

				PxU32 index;

				{
					PxVec3 vertexSpaceDirection;
					PxVec3_From_Vec3V(_dir, vertexSpaceDirection);
					const PxU32 offset = ComputeCubemapNearestOffset(vertexSpaceDirection, data->mSubdiv);
					//const PxU32 offset = ComputeCubemapOffset(vertexSpaceDirection, data->mSubdiv);
					index = data->mSamples[offset];
				}

				Vec3V maxPoint = Vec3V_From_PxVec3(verts[index]);
				FloatV max = V3Dot(maxPoint, _dir);
		
				PxU32 initialIndex = index;

				
				do
				{
					initialIndex = index;
					const PxU32 numNeighbours = valency[index].mCount;
					const PxU32 offset = valency[index].mOffset;

					for(PxU32 a = 0; a < numNeighbours; ++a)
					{
						const PxU32 neighbourIndex = adjacentVerts[offset + a];

						const Vec3V vertex = Vec3V_From_PxVec3(verts[neighbourIndex]);
						const FloatV dist = V3Dot(vertex, _dir);
						if(FAllGrtr(dist, max))
						{
							const PxU32 ind = neighbourIndex>>5;
							const PxU32 mask = 1 << (neighbourIndex & 31);
							if((smallBitMap[ind] & mask) == 0)
							{
								smallBitMap[ind] |= mask;
								max = dist;
								index = neighbourIndex;
							}
						}
					}
				}while(index != initialIndex);

				maxIndex = index;
			}
			else
			{

				FloatV max = V3Dot(Vec3V_From_PxVec3(verts[0]), _dir);

				
				const VecI32V vIOne = VecI32V_One();
				const VecI32V vZero = VecI32V_Zero();
				VecI32V index = vZero;
				VecI32V bestInd = vZero;

				const PxU32 numIterations = (numVerts-1)/4;
				
				PxU32 i = 1;
				for(PxU32 a = 0; a < numIterations; ++a)
				{
					Ps::prefetch128(&verts[i+4]);
					const Vec3V p0 = Vec3V_From_PxVec3(verts[i++]);
					const Vec3V p1 = Vec3V_From_PxVec3(verts[i++]);
					const Vec3V p2 = Vec3V_From_PxVec3(verts[i++]);
					const Vec3V p3 = Vec3V_From_PxVec3(verts[i++]);

					const FloatV d0 = V3Dot(p0, _dir);
					const FloatV d1 = V3Dot(p1, _dir);
					const FloatV d2 = V3Dot(p2, _dir);
					const FloatV d3 = V3Dot(p3, _dir);

					const BoolV con0 = FIsGrtr(d0, max);
					index = VecI32V_Add(index, vIOne);
					max = FSel(con0, d0, max);
					bestInd = VecI32V_Sel(con0, index, bestInd);

					const BoolV con1 = FIsGrtr(d1, max);
					index = VecI32V_Add(index, vIOne);
					max = FSel(con1, d1, max);
					bestInd = VecI32V_Sel(con1, index, bestInd);

					const BoolV con2 = FIsGrtr(d2, max);
					index = VecI32V_Add(index, vIOne);
					max = FSel(con2, d2, max);
					bestInd = VecI32V_Sel(con2, index, bestInd);

					const BoolV con3 = FIsGrtr(d3, max);
					index = VecI32V_Add(index, vIOne);
					max = FSel(con3, d3, max);
					bestInd = VecI32V_Sel(con3, index, bestInd);
				}
				for(; i < numVerts; ++i)
				{
					index = VecI32V_Add(index, vIOne);
					const Vec3V vertex = Vec3V_From_PxVec3(verts[i]);
					const FloatV dist = V3Dot(vertex, _dir);

					const BoolV isGrtr = FIsGrtr(dist, max);
					max = FSel(isGrtr, dist, max);
					bestInd = VecI32V_Sel(isGrtr, index, bestInd);
				}
	
				PxI32_From_VecI32V(bestInd, &maxIndex);
			}
		}
#endif
  
		void printVerts() const
		{
			using namespace Ps::aos;
			
		/*	for(PxU32 i =0; i<numVerts; ++i)
			{
				PX_PRINTF("ints[%i] = %u;\n", i*3,   PX_IR(verts[i].x));
				PX_PRINTF("ints[%i] = %u;\n", i*3+1, PX_IR(verts[i].y));
				PX_PRINTF("ints[%i] = %u;\n", i*3+2, PX_IR(verts[i].z));
			}*/

		}
		//dir is in the shape space
		PX_SUPPORT_INLINE Ps::aos::Vec3V supportLocal(const Ps::aos::Vec3VArg dir)const
		{
			using namespace Ps::aos;
			//scale dir and put it in the vertex space
			const Vec3V _dir = M33MulV3(skew, dir);
			//const Vec3V maxPoint = supportVertex(_dir);
			const PxU32 maxIndex = supportVertexIndex(_dir);
			return M33MulV3(skew, Vec3V_From_PxVec3(verts[maxIndex]));
		}




		PX_SUPPORT_INLINE Ps::aos::Vec3V supportRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aTob)const
		{
			using namespace Ps::aos;
		
			//transform dir into the shape space
			const Vec3V _dir = aTob.rotateInv(dir);//relTra.rotateInv(dir);
			const Vec3V maxPoint =supportLocal(_dir);
			//translate maxPoint from shape space of a back to the b space
			return aTob.transform(maxPoint);//relTra.transform(maxPoint);
		}

		//using in epa
		//dir is in the shape space
		PX_SUPPORT_INLINE Ps::aos::Vec3V supportLocal(const Ps::aos::Vec3VArg dir, PxI32& index)const
		{
			using namespace Ps::aos;
			//scale dir and put it in the vertex space
			const Vec3V _dir = M33MulV3(skew, dir);
			//const Vec3V maxPoint = supportVertex(_dir);
			const PxU32 maxIndex = supportVertexIndex(_dir);
			index = maxIndex;

			//transfrom the vertex from vertex space to shape space
			return M33MulV3(skew, Vec3V_From_PxVec3(verts[maxIndex]));
		}


		PX_SUPPORT_INLINE Ps::aos::Vec3V supportRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aTob, PxI32& index)const
		{
			using namespace Ps::aos;
		
			//transform dir into the shape space
			const Vec3V _dir = aTob.rotateInv(dir);//relTra.rotateInv(dir);

			const Vec3V maxPoint =supportLocal(_dir, index);

			//translate maxPoint from shape space of a back to the b space
			return aTob.transform(maxPoint);//relTra.transform(maxPoint);
		}

		PX_SUPPORT_INLINE Ps::aos::Vec3V supportSweepLocal(const Ps::aos::Vec3VArg dir)const
		{
			return supportLocal(dir);
		}

		PX_SUPPORT_INLINE Ps::aos::Vec3V supportSweepRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aToB)const
		{
			return supportRelative(dir, aToB);
		}


		//dir in the shape space
		PX_SUPPORT_INLINE Ps::aos::Vec3V supportMarginLocal(const Ps::aos::Vec3VArg dir, Ps::aos::Vec3V& support, PxI32& index)const
		{
			using namespace Ps::aos;
			//scale dir and put it in the vertex space
			const Vec3V _dir = M33MulV3(skew, dir);
			//get the extreme point index

			const PxU32 maxIndex = supportVertexIndex(_dir);
			index = maxIndex;
			//p is in the shape space
			const Vec3V p = planeShift(maxIndex, margin);
			support = p;
			return p;
		}

		



		PX_SUPPORT_INLINE Ps::aos::Vec3V supportMarginRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aTob, Ps::aos::Vec3V& support, PxI32& index)const
		{
			using namespace Ps::aos;

			//transform dir from b space to the shape space of a space
			const Vec3V _dir = aTob.rotateInv(dir);//relTra.rotateInv(dir);//M33MulV3(skewInvRot, dir);
			const Vec3V p = supportMarginLocal(_dir, support, index);
			//transfrom from a to b space
			const Vec3V ret = aTob.transform(p);
			support = ret;
			return ret;
		}

		PX_SUPPORT_INLINE Ps::aos::BoolV supportLocalIndex(const Ps::aos::Vec3VArg dir, PxI32& index)const
		{
			using namespace Ps::aos;
			//scale dir and put it in the vertex space
			const Vec3V _dir = M33MulV3(skew, dir);
			supportVertexIndex(_dir, index);
			return BTTTT();
		}

		PX_SUPPORT_INLINE Ps::aos::Vec3V supportMarginLocal(Ps::aos::Vec3V& support, const PxI32& index, const Ps::aos::BoolV comp)const
		{
			using namespace Ps::aos;
			const Vec3V p = planeShift(index, margin);
			support = p;
			return p;
		}

		PX_SUPPORT_INLINE Ps::aos::BoolV supportRelativeIndex(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aTob, PxI32& index)const
		{
			using namespace Ps::aos;
			//scale dir and put it in the vertex space
			const Vec3V _dir = aTob.rotateInv(dir);
			return supportLocalIndex(_dir, index);	
		}

		PX_SUPPORT_INLINE Ps::aos::Vec3V supportMarginRelative(Ps::aos::Vec3V& support, const Ps::aos::PsMatTransformV& aTob, const PxI32& index, const Ps::aos::BoolV comp)const
		{
			using namespace Ps::aos;
			const Vec3V p = planeShift(index, margin);
			const Vec3V ret = aTob.transform(p);
			support = ret;
			return ret;
		}

		PX_SUPPORT_INLINE	bool contain(const Ps::aos::Vec3VArg p)const
		{
			using namespace Ps::aos;
			PxVec3 point;
			PxVec3_From_Vec3V(p, point);
			PxU32 Nb = hullData->mNbPolygons;
			const Gu::HullPolygonData* Polygons = hullData->mPolygons;
			while(Nb--)
			{
				const PxPlane& pl = Polygons->mPlane;
				if(pl.distance(point) > 0.0f)	return false;
				Polygons++;
			}
			return true;

		}

		Ps::aos::Mat33V skew; //inv(R)*S*R
	//	Ps::aos::Mat33V rot;

		const Gu::ConvexHullData* hullData;
		const BigConvexRawData* data;  
		const PxVec3* verts;
		PxU8 numVerts;
	//	mutable PxU8 searchIndex;

	};


}

}

#endif	// 
