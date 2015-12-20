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


#ifndef PX_PHYSICS_GEOMUTILS_VEC_BOX
#define PX_PHYSICS_GEOMUTILS_VEC_BOX

/** \addtogroup geomutils
@{
*/

#include "GuVecConvex.h"
#include "PsVecTransform.h"
#include "GuConvexSupportTable.h"



namespace physx
{

static Ps::aos::BoolV boxVertexTable[8] = {
											Ps::aos::BFFFF(),//---
											Ps::aos::BTFFF(),//+--
											Ps::aos::BFTFF(),//-+-
											Ps::aos::BTTFF(),//++-
											Ps::aos::BFFTF(),//--+
											Ps::aos::BTFTF(),//+-+
											Ps::aos::BFTTF(),//-++
											Ps::aos::BTTTF(),//+++
										};
namespace Gu
{
	class CapsuleV;

	//void computeOBBPoints(Ps::aos::Vec3V* PX_RESTRICT pts, const Ps::aos::Vec3VArg center, const Ps::aos::Vec3VArg extents, const Ps::aos::Vec3VArg base0, const Ps::aos::Vec3VArg base1, const Ps::aos::Vec3VArg base2);



	PX_FORCE_INLINE void CalculateBoxMargin(const Ps::aos::Vec3VArg extent, Ps::aos::FloatV& margin, Ps::aos::FloatV& minMargin)
	{
		using namespace Ps::aos;

		const FloatV ratio1 = Ps::aos::FloatV_From_F32(0.03f);
		const FloatV ratio2 = Ps::aos::FloatV_From_F32(0.15f);
		const FloatV ratio = Ps::aos::FloatV_From_F32(0.05f);
		
		const FloatV min = V3ExtractMin(extent);
		const FloatV max = V3ExtractMax(extent);
		const FloatV _min = FMul(min, ratio2);
		const FloatV _max = FMul(max, ratio1);

		margin = FMin(min, FMax(_min, _max));
		minMargin = FMul(min, ratio);
		
	}

	PX_FORCE_INLINE Ps::aos::FloatV CalculateBoxTolerance(const Ps::aos::Vec3VArg extent)
	{

		using namespace Ps::aos;
		
		const FloatV r0 = FloatV_From_F32(0.01f);
		const FloatV min = V3ExtractMin(extent);//FMin(V3GetX(extent), FMin(V3GetY(extent), V3GetZ(extent)));
		return FMul(min, r0);
	}

	PX_FORCE_INLINE Ps::aos::FloatV CalculatePCMBoxMargin(const Ps::aos::Vec3VArg extent)
	{

		using namespace Ps::aos;
		
		const FloatV r0 = FloatV_From_F32(0.15f);
		const FloatV min = V3ExtractMin(extent);//FMin(V3GetX(extent), FMin(V3GetY(extent), V3GetZ(extent)));
		return FMul(min, r0);
	
	}

	/**
	\brief Represents an oriented bounding box. 

	As a center point, extents(radii) and a rotation. i.e. the center of the box is at the center point, 
	the box is rotated around this point with the rotation and it is 2*extents in width, height and depth.
	*/

	/**
	Box geometry

	The rot member describes the world space orientation of the box.
	The center member gives the world space position of the box.
	The extents give the local space coordinates of the box corner in the positive octant.
	Dimensions of the box are: 2*extent.
	Transformation to world space is: worldPoint = rot * localPoint + center
	Transformation to local space is: localPoint = T(rot) * (worldPoint - center)
	Where T(M) denotes the transpose of M.
	*/

	class BoxV : public ConvexV
	{
	public:
		/**
		\brief Constructor
		*/
		PX_INLINE BoxV() : ConvexV(E_BOX)
		{
		}

		/**
		\brief Constructor

		\param _center Center of the OBB
		\param _extents Extents/radii of the obb.
		\param _rot rotation to apply to the obb.
		*/


		//! Construct from center, extent and rotation
		PX_FORCE_INLINE BoxV(const Ps::aos::Vec3VArg origin, const Ps::aos::Vec3VArg extent, const Ps::aos::Mat33V& base) : 
																											ConvexV(E_BOX, origin), rot(base), extents(extent)
		{
			CalculateBoxMargin(extent, margin, minMargin);
		}

		PX_FORCE_INLINE BoxV(const Ps::aos::Vec3VArg origin, const Ps::aos::Vec3VArg extent) : 
																				ConvexV(E_BOX, origin), extents(extent)
		{
			CalculateBoxMargin(extent, margin, minMargin);
		}

		PX_FORCE_INLINE BoxV(const Ps::aos::Vec3VArg origin, const Ps::aos::Vec3VArg extent, 
			const Ps::aos::Vec3VArg col0, const Ps::aos::Vec3VArg col1, const Ps::aos::Vec3VArg col2) : 
																									ConvexV(E_BOX, origin), extents(extent)
		{
			CalculateBoxMargin(extent, margin, minMargin);
			rot.col0 = col0;
			rot.col1 = col1;
			rot.col2 = col2;
		}

		PX_FORCE_INLINE BoxV(const Ps::aos::Vec3VArg origin, const Ps::aos::Vec3VArg extent, const Ps::aos::Mat33V& base, const Ps::aos::FloatVArg _margin) : 
																											ConvexV(E_BOX, origin, _margin), rot(base), extents(extent)
		{
		}

		PX_FORCE_INLINE BoxV(const Ps::aos::Vec3VArg origin, const Ps::aos::Vec3VArg extent, const Ps::aos::Mat33V& base, const Ps::aos::FloatVArg _margin, const Ps::aos::FloatVArg _minMargin) : 
																											ConvexV(E_BOX, origin, _margin, _minMargin), rot(base), extents(extent)
		{
		}

		PX_FORCE_INLINE BoxV(const Ps::aos::Vec3VArg origin, const Ps::aos::Vec3VArg extent,const Ps::aos::FloatVArg _margin, const Ps::aos::FloatVArg _minMargin) : 
																											ConvexV(E_BOX, origin, _margin, _minMargin), extents(extent)
		{
		}
																				
		//! construct from a matrix(center and rotation) + extent
		PX_FORCE_INLINE BoxV(const Ps::aos::Mat34V& mat, const Ps::aos::Vec3VArg extent) : ConvexV(E_BOX, mat.col3), rot(Ps::aos::Mat33V(mat.col0, mat.col1, mat.col2)), extents(extent)
		{
		}

		//! Copy constructor
		PX_FORCE_INLINE BoxV(const BoxV& other) : ConvexV(E_BOX, other.center, other.margin), rot(other.rot), extents(other.extents)
		{
		}

		
		/**
		\brief Destructor
		*/
		PX_INLINE ~BoxV()
		{
		}

		//! Assignment operator
		PX_INLINE const BoxV& operator=(const BoxV& other)
		{
			rot		= other.rot;
			center	= other.center;
			extents	= other.extents;
			margin =  other.margin;
			minMargin = other.minMargin;
			return *this;
		}

		/**
		\brief Setups an empty box.
		*/
		PX_INLINE void setEmpty()
		{
			using namespace Ps::aos;
			center = V3Zero();
			extents = Vec3V_From_FloatV(FloatV_From_F32(-PX_MAX_REAL));
			rot = M33Identity();
			margin = minMargin = FZero();

		}

		/**
		\brief Checks the box is valid.

		\return	true if the box is valid
		*/
		PX_INLINE bool isValid() const
		{
			// Consistency condition for (Center, Extents) boxes: Extents >= 0.0f
			/*if(extents.x < 0.0f)	return false;
			if(extents.y < 0.0f)	return false;
			if(extents.z < 0.0f)	return false;
		
			return true;*/

			using namespace Ps::aos;
			const Vec3V zero = V3Zero();
			return BAllEq(V3IsGrtrOrEq(extents, zero), BTTTT()) == 1;
		}

/////////////
		PX_FORCE_INLINE	void	setAxes(const Ps::aos::Vec3VArg axis0, const Ps::aos::Vec3VArg axis1, const Ps::aos::Vec3VArg axis2)
		{
			rot.col0 = axis0;
			rot.col1 = axis1;
			rot.col2 = axis2;
		}


		
		PX_INLINE	Ps::aos::Vec3V	rotate(const Ps::aos::Vec3VArg src)	const
		{
			//return rot * src;
			return Ps::aos::M33MulV3(rot, src);
		}

		PX_INLINE	Ps::aos::Vec3V	rotateInv(const Ps::aos::Vec3VArg src)	const
		{
			//return rot.transformTranspose(src);
			return Ps::aos::M33TrnspsMulV3(rot, src);
		}

		//get the world space point from the local space
		PX_INLINE	Ps::aos::Vec3V	transformFromLocalToWorld(const Ps::aos::Vec3VArg src)	const
		{
			//return rot * src + center;
			return Ps::aos::V3Add(Ps::aos::M33MulV3(rot, src), center);
		}

		PX_INLINE	Ps::aos::Vec3V	transformFromWorldToLocal(const Ps::aos::Vec3VArg src)	const
		{
			//return Inv(rot) * (src - center);
			return Ps::aos::M33TrnspsMulV3(rot, Ps::aos::V3Sub(src, center));
		}

	/*	PX_INLINE	PxTransform getTransform()	const
		{
			using namespace Ps::aos;
			PX_ALIGN(16, PxMat33 pxMat);
			PxMat33_From_Mat33V(rot, pxMat);
			PxVec3 c;
			PxVec3_From_Vec3V(center, c);
			return PxTransform(c, PxQuat(pxMat));
		}*/


		PX_INLINE Ps::aos::Vec3V computeAABBExtent() const
		{
			using namespace Ps::aos;
			return M33TrnspsMulV3(rot, extents);
		}

		void computeOBBPoints(Ps::aos::Vec3V* PX_RESTRICT pts)
		{
			using namespace Ps::aos;
			//     7+------+6			0 = ---
			//     /|     /|			1 = +--
			//    / |    / |			2 = ++-
			//   / 4+---/--+5			3 = -+-
			// 3+------+2 /    y   z	4 = --+
			//  | /    | /     |  /		5 = +-+
			//  |/     |/      |/		6 = +++
			// 0+------+1      *---x	7 = -++

			const Vec3V nExtents = V3Neg(extents);
			const FloatV x = V3GetX(extents);
			const FloatV y = V3GetY(extents);
			const FloatV z = V3GetZ(extents);
			const FloatV nx = V3GetX(nExtents);
			const FloatV ny = V3GetY(nExtents);
			const FloatV nz = V3GetZ(nExtents);
		
			pts[0] = nExtents;
			pts[1] = V3Merge(x, ny, nz);  
			pts[2] = V3Merge(x,  y, nz);
			pts[3] = V3Merge(nx, y, nz);
			pts[4] = V3Merge(nx, ny, z);
			pts[5] = V3Merge(x,  ny, z);
			pts[6] = extents;
			pts[7] = V3Merge(nx,  y, z);

		}

		void computeOBBPoints(Ps::aos::Vec3V* PX_RESTRICT pts, const Ps::aos::Vec3VArg center, const Ps::aos::Vec3VArg extents, const Ps::aos::Vec3VArg base0, const Ps::aos::Vec3VArg base1, const Ps::aos::Vec3VArg base2) const
		{
			using namespace Ps::aos;

			PX_ASSERT(pts);

			// "Rotated extents"
			const Vec3V axis0 = V3Scale(base0,  V3GetX(extents));
			const Vec3V axis1 = V3Scale(base1,  V3GetY(extents));
			const Vec3V axis2 = V3Scale(base2,  V3GetZ(extents));

			//     7+------+6			0 = ---
			//     /|     /|			1 = +--
			//    / |    / |			2 = ++-
			//   / 4+---/--+5			3 = -+-
			// 3+------+2 /    y   z	4 = --+
			//  | /    | /     |  /		5 = +-+
			//  |/     |/      |/		6 = +++
			// 0+------+1      *---x	7 = -++


			const Vec3V a0 = V3Sub(center, axis0);
			const Vec3V a1 = V3Add(center, axis0);
			const Vec3V a2 = V3Add(axis1,  axis2);
			const Vec3V a3 = V3Sub(axis1,  axis2);

			pts[0] = V3Sub(a0, a2);
			pts[1] = V3Sub(a1, a2);

			pts[2] = V3Add(a1, a3);
			pts[3] = V3Add(a0, a3);

			pts[4] = V3Sub(a0, a3);
			pts[5] = V3Sub(a1, a3);
			
			pts[6] = V3Add(a1, a2);
			pts[7] = V3Add(a0, a2);
		}

		///**
		//Computes the obb points.
		//\param		pts	[out] 8 box points
		//\return		true if success
		//*/
		//PX_INLINE void computeBoxPoints(Ps::aos::Vec3V* PX_RESTRICT pts) const
		//{
		//	return computeOBBPoints(pts, center, extents, rot.col0, rot.col1, rot.col2);
		//}

		/**
		\brief recomputes the OBB after an arbitrary transform by a 4x4 matrix.
		\param	mtx		[in] the transform matrix
		\param	obb		[out] the transformed OBB
		*/
		PX_INLINE	void rotate(const Ps::aos::Mat34V& mtx, BoxV& obb)	const
		{
			using namespace Ps::aos;
			// The extents remain constant
			obb.extents = extents;
			// The center gets x-formed
			obb.center =M34MulV3(mtx, obb.center);
			// Combine rotations
			const Mat33V mtxR = Mat33V(mtx.col0, mtx.col1, mtx.col2);
			obb.rot =M33MulM33(mtxR, rot);
		}

		void create(const Gu::CapsuleV& capsule);

		/**
		\brief checks the OBB is inside another OBB.
		\param		box		[in] the other OBB
		*/
		PxU32 isInside(const BoxV& box)	const;  


		PX_FORCE_INLINE Ps::aos::Vec3V supportMarginPoint(const PxI32 index)const
		{
			using namespace Ps::aos;
			const Vec3V _extents = V3Sub(extents,  V3Splat(margin));
			const BoolV con = boxVertexTable[index];
			return V3Sel(con, _extents, V3Neg(_extents));
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportPoint(const PxI32 index)const
		{
			using namespace Ps::aos;
			const BoolV con = boxVertexTable[index];
			return V3Sel(con, extents, V3Neg(extents));
		}  

		PX_FORCE_INLINE void getIndex(const Ps::aos::BoolV con, PxI32& index)const
		{
			using namespace Ps::aos;
			const VecI32V v = VecI32V_From_BoolV(con);
			const VecI32V vAllOne = VecI32V_One();
			const VecI32V vOne = VecI32V_PrepareShift(vAllOne);
			const VecI32V vTwo = VecI32V_PrepareShift(VecI32V_Two());
			const VecI32V temp = VecI32V_And(v, vAllOne);
			const VecI32V x = VecI32V_GetX(temp);
			const VecI32V y = VecI32V_GetY(temp);
			const VecI32V z = VecI32V_GetZ(temp);
			const VecI32V shiftY = VecI32V_LeftShift(y, vOne);
			const VecI32V shiftZ = VecI32V_LeftShift(z, vTwo);
			const VecI32V t = VecI32V_Or(x, VecI32V_Or(shiftY, shiftZ));
			PxI32_From_VecI32V(t, &index);
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportSweepRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aToB)const  
		{
			return supportRelative(dir, aToB);	
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportSweepLocal(const Ps::aos::Vec3VArg dir)const  
		{
			return supportLocal(dir);	
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportLocal(const Ps::aos::Vec3VArg dir)const  
		{
			using namespace Ps::aos;
			return V3Sel(V3IsGrtr(dir,  V3Zero()), extents, V3Neg(extents));
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aTob)const  
		{

			//a is the current object, b is the other object, dir is in the local space of b
			using namespace Ps::aos;
			const Vec3V _dir = aTob.rotateInv(dir);//relTra.rotateInv(dir);//from b to a
			const Vec3V p =  supportLocal(_dir);
			//transfer p into the b space
			return aTob.transform(p);//relTra.transform(p);
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportLocal(const Ps::aos::Vec3VArg dir, PxI32& index)const  
		{
			using namespace Ps::aos;
			const BoolV comp = V3IsGrtr(dir, V3Zero());
			getIndex(comp, index);
			return  V3Sel(comp, extents, V3Neg(extents));
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aTob, PxI32& index)const  
		{

			//a is the current object, b is the other object, dir is in the local space of b
			using namespace Ps::aos;
			const Vec3V _dir = aTob.rotateInv(dir);//relTra.rotateInv(dir);//from b to a
			const Vec3V p = supportLocal(_dir, index);
			//transfer p into the b space
			return aTob.transform(p);//relTra.transform(p);
		}


		//local space point
		PX_FORCE_INLINE Ps::aos::Vec3V supportMarginLocal(const Ps::aos::Vec3VArg dir,  Ps::aos::Vec3V& support, PxI32& index)const
		{
			using namespace Ps::aos;
		
			const Vec3V zero = V3Zero();
			const Vec3V _extents = V3Sub(extents,  V3Splat(margin));
			const BoolV comp = V3IsGrtr(dir, zero);
			getIndex(comp, index);
			const Vec3V p = V3Sel(comp, _extents, V3Neg(_extents));
			support = p;
			return p;
		}

		PX_FORCE_INLINE Ps::aos::BoolV supportLocalIndex(const Ps::aos::Vec3VArg dir, PxI32& index)const
		{
			using namespace Ps::aos;
			const Vec3V zero = V3Zero();
			const BoolV comp = V3IsGrtr(dir, zero);
			getIndex(comp, index);
			return comp;
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportMarginLocal(Ps::aos::Vec3V& support, const PxI32& index, const Ps::aos::BoolV comp)const
		{
			using namespace Ps::aos;
			const Vec3V _extents = V3Sub(extents,  V3Splat(margin));
			const Vec3V p = V3Sel(comp, _extents, V3Neg(_extents));
			support = p;
			return p;
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportMarginRelative(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aTob, Ps::aos::Vec3V& support, PxI32& index)const
		{
			using namespace Ps::aos;
		
			const Vec3V zero = V3Zero();
			const Vec3V _extents = V3Sub(extents,  Vec3V_From_FloatV(margin));
			//transfer dir into the local space of the box
			//const Vec3V _dir = M33TrnspsMulV3(rot, dir);
			const Vec3V _dir =aTob.rotateInv(dir);//relTra.rotateInv(dir);
			const BoolV comp = V3IsGrtr(_dir, zero);
			const Vec3V p = V3Sel(comp, _extents, V3Neg(_extents));
			getIndex(comp, index);
			//transfer p into the world space
			const Vec3V ret = aTob.transform(p);//relTra.transform(p);//V3Add(center, M33MulV3(rot, p));
			support = ret;
			return ret;
		}

		PX_FORCE_INLINE Ps::aos::BoolV supportRelativeIndex(const Ps::aos::Vec3VArg dir, const Ps::aos::PsMatTransformV& aTob, PxI32& index)const
		{
			using namespace Ps::aos;
			const Vec3V zero = V3Zero();
			//scale dir and put it in the vertex space
			const Vec3V _dir =aTob.rotateInv(dir);//relTra.rotateInv(dir);
			const BoolV comp = V3IsGrtr(_dir, zero);
			getIndex(comp, index);
			return comp;
		}

		PX_FORCE_INLINE Ps::aos::Vec3V supportMarginRelative(Ps::aos::Vec3V& support, const Ps::aos::PsMatTransformV& aTob, const PxI32& index, const Ps::aos::BoolV comp)const
		{
			using namespace Ps::aos;
		
			const Vec3V _extents = V3Sub(extents,  Vec3V_From_FloatV(margin));
			const Vec3V p = V3Sel(comp, _extents, V3Neg(_extents));
			//transfer p into the world space
			const Vec3V ret = aTob.transform(p);//relTra.transform(p);//V3Add(center, M33MulV3(rot, p));
			support = ret;
			return ret;
		}
	
		Ps::aos::Mat33V rot;
		Ps::aos::Vec3V extents;
	};
}	//PX_COMPILE_TIME_ASSERT(sizeof(Gu::BoxV) == 96);

}

/** @} */
#endif
