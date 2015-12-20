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


#ifndef PX_PHYSICS_GEOMUTILS_VEC_TRIANGLE_H
#define PX_PHYSICS_GEOMUTILS_VEC_TRIANGLE_H
/** \addtogroup geomutils
  @{
*/

#include "GuVecConvex.h"
#include "GuConvexSupportTable.h"
#include "GuDistancePointTriangle.h"

namespace physx
{
namespace Gu
{

	
	class TriangleV : public ConvexV
	{
		public:
		/**
		\brief Constructor
		*/
		PX_FORCE_INLINE			TriangleV() : ConvexV(E_TRIANGLE)
		{
			using namespace Ps::aos;
			margin = FZero();
			minMargin = FMax();
#ifdef __SPU__
			bMarginIsRadius = true;
#endif
		}
		/**
		\brief Constructor

		\param[in] p0 Point 0
		\param[in] p1 Point 1
		\param[in] p2 Point 2
		*/

		PX_FORCE_INLINE	TriangleV(const Ps::aos::Vec3VArg p0, const Ps::aos::Vec3VArg p1, const Ps::aos::Vec3VArg p2): ConvexV(E_TRIANGLE)									
		{
			using namespace Ps::aos;
			//const FloatV zero = FZero();
			const FloatV num = FloatV_From_F32(0.333333f);
			const Vec3V center = V3Scale(V3Add(V3Add(p0, p1), p2), num);
			setCenter(center);
			verts[0] = p0;
			verts[1] = p1;
			verts[2] = p2;
			margin = FZero();
			minMargin = FMax();
#ifdef __SPU__
			bMarginIsRadius = true;
#endif
		}

		/**
		\brief Copy constructor

		\param[in] triangle Tri to copy
		*/
		PX_FORCE_INLINE			TriangleV(const Gu::TriangleV& triangle) : ConvexV(E_TRIANGLE, triangle.center, triangle.margin, triangle.minMargin)
		{
			using namespace Ps::aos;
			verts[0] = triangle.verts[0];
			verts[1] = triangle.verts[1];
			verts[2] = triangle.verts[2];
#ifdef __SPU__
			bMarginIsRadius = true;
#endif
		}
		/**
		\brief Destructor
		*/
		PX_FORCE_INLINE			~TriangleV()
		{
		}

		PX_FORCE_INLINE Ps::aos::FloatV getSweepMargin() const
		{
			return Ps::aos::FZero();
		}

		void setCenter(const Ps::aos::Vec3VArg _center)
		{
			using namespace Ps::aos;
			Vec3V offset = V3Sub(_center, center);
			center = _center;
			verts[0] = V3Add(verts[0], offset);
			verts[1] = V3Add(verts[1], offset);
			verts[2] = V3Add(verts[2], offset);
		}

		/**
		\brief Compute the normal of the Triangle.

		\param[out] _normal Triangle normal.
		*/
		PX_FORCE_INLINE	void	normal(Ps::aos::Vec3V& _normal) const
		{
			using namespace Ps::aos;
			const Vec3V ab = V3Sub(verts[1], verts[0]);
			const Vec3V ac = V3Sub(verts[2], verts[0]);
			const Vec3V n = V3Cross(ab, ac);
			_normal = V3Normalize(n);
		}

		/**
		\brief Compute the unnormalized normal of the Triangle.

		\param[out] _normal Triangle normal (not normalized).
		*/
		PX_FORCE_INLINE	void	denormalizedNormal(Ps::aos::Vec3V& _normal) const
		{
			using namespace Ps::aos;
			const Vec3V ab = V3Sub(verts[1], verts[0]);
			const Vec3V ac = V3Sub(verts[2], verts[0]);
			_normal = V3Cross(ab, ac);
		}

		PX_FORCE_INLINE	Ps::aos::FloatV	area() const
		{
			using namespace Ps::aos;
			const FloatV half = FloatV_From_F32(0.5f);
			const Vec3V ba = V3Sub(verts[0], verts[1]);
			const Vec3V ca = V3Sub(verts[0], verts[2]);
			const Vec3V v = V3Cross(ba, ca);
			return FMul(V3Length(v), half);
		}


		

		

		////dir is in world space
		//Ps::aos::Vec3V support(const Ps::aos::Vec3VArg dir)const
		//{
		//	using namespace Ps::aos;
		//	//verts are in world space
		//	const Vec3V v0 = verts[0];
		//	const Vec3V v1 = verts[1];
		//	const Vec3V v2 = verts[2];
		//	const FloatV d0 = V3Dot(v0, dir);
		//	const FloatV d1 = V3Dot(v1, dir);
		//	const FloatV d2 = V3Dot(v2, dir);

		//	const BoolV con0 = BAnd(FIsGrtr(d0, d1), FIsGrtr(d0, d2));
		//	const BoolV con1 = FIsGrtr(d1, d2);
		//	return V3Sel(con0, v0, V3Sel(con1, v1, v2));
		//}

		//dir is in local space, verts in the local space
		Ps::aos::Vec3V supportLocal(const Ps::aos::Vec3VArg dir) const
		{
			using namespace Ps::aos;
			const Vec3V v0 = verts[0];
			const Vec3V v1 = verts[1];
			const Vec3V v2 = verts[2];
			const FloatV d0 = V3Dot(v0, dir);
			const FloatV d1 = V3Dot(v1, dir);
			const FloatV d2 = V3Dot(v2, dir);

			const BoolV con0 = BAnd(FIsGrtr(d0, d1), FIsGrtr(d0, d2));
			const BoolV con1 = FIsGrtr(d1, d2);
			return V3Sel(con0, v0, V3Sel(con1, v1, v2));
		}

		//dir is in b space
		Ps::aos::Vec3V supportRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aToB)const
		{
			using namespace Ps::aos;
			//verts are in local space
			const Vec3V _dir = aToB.rotateInv(dir); //transform dir back to a space
			const Vec3V maxPoint = supportLocal(_dir);
			return aToB.transform(maxPoint);//transform maxPoint to the b space
		}

		Ps::aos::Vec3V supportLocal(const Ps::aos::Vec3VArg dir, PxI32& index) const
		{
		
			using namespace Ps::aos;
			const VecI32V vZero = VecI32V_Zero();
			const VecI32V vOne = VecI32V_One();
			const VecI32V vTwo = VecI32V_Two();
			const Vec3V v0 = verts[0];
			const Vec3V v1 = verts[1];
			const Vec3V v2 = verts[2];
			const FloatV d0 = V3Dot(v0, dir);
			const FloatV d1 = V3Dot(v1, dir);
			const FloatV d2 = V3Dot(v2, dir);
			const BoolV con0 = BAnd(FIsGrtr(d0, d1), FIsGrtr(d0, d2));
			const BoolV con1 = FIsGrtr(d1, d2);
			const VecI32V vIndex = VecI32V_Sel(con0, vZero, VecI32V_Sel(con1, vOne, vTwo));
			PxI32_From_VecI32V(vIndex, &index);
			return V3Sel(con0, v0, V3Sel(con1, v1, v2));
		}

		PX_FORCE_INLINE Ps::aos::BoolV supportLocalIndex(const Ps::aos::Vec3VArg dir, PxI32& index)const
		{
			using namespace Ps::aos;
			const VecI32V vZero = VecI32V_Zero();
			const VecI32V vOne = VecI32V_One();
			const VecI32V vTwo = VecI32V_Two();
			const Vec3V v0 = verts[0];
			const Vec3V v1 = verts[1];
			const Vec3V v2 = verts[2];
			const FloatV d0 = V3Dot(v0, dir);
			const FloatV d1 = V3Dot(v1, dir);
			const FloatV d2 = V3Dot(v2, dir);
			const BoolV con0 = BAnd(FIsGrtr(d0, d1), FIsGrtr(d0, d2));
			const BoolV con1 = FIsGrtr(d1, d2);
			const VecI32V vIndex = VecI32V_Sel(con0, vZero, VecI32V_Sel(con1, vOne, vTwo));
			PxI32_From_VecI32V(vIndex, &index);
			return BSetY(con0, con1);
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportMarginLocal(Ps::aos::Vec3V& support, const PxI32& index, const Ps::aos::BoolV comp)const
		{
			using namespace Ps::aos;
			const BoolV con0 = BGetX(comp);
			const BoolV con1 = BGetY(comp);
			const Vec3V p =V3Sel(con0, verts[0], V3Sel(con1, verts[1], verts[2]));
			support = p;
			return p;
	
		}

		//dir is in b space
		Ps::aos::Vec3V supportRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aToB, PxI32 index)const
		{
			using namespace Ps::aos;
			//verts are in local space
			const Vec3V _dir = aToB.rotateInv(dir); //transform dir back to a space
			const Vec3V maxPoint = supportLocal(_dir, index);
			return aToB.transform(maxPoint);//transform maxPoint to the b space
		}

		Ps::aos::Vec3V supportSweepLocal(const Ps::aos::Vec3VArg dir)const
		{
			return supportLocal(dir);
		}
  
		Ps::aos::Vec3V supportSweepRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aToB)const
		{
			return supportRelative(dir, aToB);
		}



		Ps::aos::Vec3V supportMarginLocal(const Ps::aos::Vec3VArg dir, Ps::aos::Vec3V& support, PxI32& index)const
		{
			//don't put margin in the triangle
			using namespace Ps::aos;

			const Vec3V res = supportLocal(dir, index);
			support = res;
			return res;
		}

		Ps::aos::Vec3V supportMarginRelative(const Ps::aos::Vec3VArg dir,  const Ps::aos::PsMatTransformV& aToB, Ps::aos::Vec3V& support, PxI32& index)const
		{
			//don't put margin in the triangle
			using namespace Ps::aos;
			//transfer dir into the local space of triangle
			const Vec3V _dir = aToB.rotateInv(dir);
			const Vec3V res = supportLocal(_dir, index);
			support = res;
			return aToB.transform(res);//transform maxPoint to the b space

		}

		PX_FORCE_INLINE Ps::aos::BoolV supportRelativeIndex(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aTob, PxI32& index)const
		{
			using namespace Ps::aos;
			//scale dir and put it in the vertex space
			const Vec3V _dir =aTob.rotateInv(dir);//relTra.rotateInv(dir);
			return supportLocalIndex(_dir, index);
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportMarginRelative(Ps::aos::Vec3V& support, const Ps::aos::PsMatTransformV& aTob, const PxI32& index, const Ps::aos::BoolV comp)const
		{
			using namespace Ps::aos;
			const BoolV con0 = BGetX(comp);
			const BoolV con1 = BGetY(comp);
			const Vec3V p =V3Sel(con0, verts[0], V3Sel(con1, verts[1], verts[2]));
			//transfer p into the b space
			const Vec3V ret = aTob.transform(p);//relTra.transform(p);//V3Add(center, M33MulV3(rot, p));
			support = ret;
			return ret;
		}


	
		PX_FORCE_INLINE Ps::aos::Vec3V supportPoint(const PxI32 index)const
		{
			return verts[index];
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportMarginPoint(const PxI32 index)const
		{
			return verts[index];
		}

		PX_FORCE_INLINE bool contain(const Ps::aos::Vec3VArg p)const
		{
			using namespace Ps::aos;
			FloatV u,v;
			Vec3V closestP;
			const FloatV sqDist = Gu::distancePointTriangleSquared(p, verts[0], verts[1], verts[2], u, v, closestP);
			return FAllGrtrOrEq(FloatV_From_F32(0.0000001f), sqDist) == 1;
			
		}

#ifndef __SPU__
		PX_FORCE_INLINE Ps::aos::BoolV isMarginEqRadiusV()const
		{
			return Ps::aos::BFFFF();
		}
#endif

			/**
		\brief Array of Vertices.
		*/
		Ps::aos::Vec3V		verts[3];

	};
}

}

#endif
