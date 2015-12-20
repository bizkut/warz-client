/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Box3f_H
#define Navigation_Box3f_H

#include "gwnavruntime/math/fastmath.h"
#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{


/// This class represents a three-dimensional axis-aligned bounding box whose dimensions are stored using
/// floating-point numbers.
/// This class is typically used to identify regions of three-dimensional space. For example, this class is
/// used to store the bounding box taken up by some objects such as NavMeshes, NavFloors and NavCells
class Box3f
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Constructors ----------------------------------

	/// Creates a new Box3f with invalid extents: you must call Set() before using it.
	Box3f() { Clear(); }

	/// Creates a new Box3f with the provided extents.
	/// \param min_			The minima of the bounding box.
	/// \param max_			The maxima of the bounding box. 
	Box3f(const Vec3f& min_, const Vec3f& max_) :
		m_min(min_), m_max(max_)
	{}


	// ---------------------------------- Main API Functions ----------------------------------

	bool operator==(const Box3f& other) const { return m_min == other.m_min && m_max == other.m_max; }

	bool operator!=(const Box3f& other) const { return !operator==(other); }

	bool IsValid() { return m_min <= m_max; }

	/// Clears all information maintained by this object. This method sets the extents of the box to invalid values; 
	/// you must follow this method with a call to Set() before you can use the box. 
	KY_INLINE void Clear()
	{
		m_min.Set( KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL);
		m_max.Set(-KyFloat32MAXVAL, -KyFloat32MAXVAL, -KyFloat32MAXVAL);
	}

	/// Sets the extents of the bounding box to the specified values.
	/// \param min_				The minima of the bounding box.
	/// \param max_				The maxima of the bounding box. 
	KY_INLINE void Set(const Vec3f& min_, const Vec3f& max_)
	{
		m_min = min_;
		m_max = max_;
	}

	/// Retrieves the extents of the box along the X axis. 
	KY_INLINE KyFloat32 SizeX() const { return m_max.x - m_min.x; }
	
	/// Retrieves the extents of the box along the Y axis. 
	KY_INLINE KyFloat32 SizeY() const { return m_max.y - m_min.y; }
	
	/// Retrieves the extents of the box along the Z axis. 
	KY_INLINE KyFloat32 SizeZ() const { return m_max.z - m_min.z; }

	/// Retrieves the coordinates of the exact center of the box. 
	KY_INLINE Vec3f Center() const { return ((m_min + m_max) * 0.5f); }

	/// Retrieves the distance between the diagonally opposite corners of the box. 
	KY_INLINE KyFloat32 Radius() const
	{
		return sqrt( (SizeX()*SizeX()) + (SizeY()*SizeY()) + (SizeZ()*SizeZ()) );
	}

	/// Moves the bounding box by the specified vector. 
	KY_INLINE void Translate(const Vec3f& v)
	{
		m_min += v;
		m_max += v;
	}

	/// Enlarges the extents of the bounding box to include the specified 
	/// three-dimensional point. If the point is already contained within 
	/// the bounding box, the box is not modified. 
	void ExpandByVec3(const Vec3f& pos)
	{
		const KyFloat32 posx = pos.x;
		const KyFloat32 posy = pos.y;
		const KyFloat32 posz = pos.z;
		m_min.x = Kaim::Min(m_min.x, posx);
		m_min.y = Kaim::Min(m_min.y, posy);
		m_min.z = Kaim::Min(m_min.z, posz);
		m_max.x = Kaim::Max(m_max.x, posx);
		m_max.y = Kaim::Max(m_max.y, posy);
		m_max.z = Kaim::Max(m_max.z, posz);
	}

	/// Enlarges the extents of the bounding box to include the area covered by the specified bounding box. 
	/// If the bounding box already encompasses the area of box, the bounding box is not modified. 
	void ExpandByBox3(const Box3f& box)
	{
		m_min.x = Kaim::Min(m_min.x, box.m_min.x);
		m_min.y = Kaim::Min(m_min.y, box.m_min.y);
		m_min.z = Kaim::Min(m_min.z, box.m_min.z);
		m_max.x = Kaim::Max(m_max.x, box.m_max.x);
		m_max.y = Kaim::Max(m_max.y, box.m_max.y);
		m_max.z = Kaim::Max(m_max.z, box.m_max.z);
	}

	/// Enlarges the extents of the bounding box by the specified amount in all directions. 
	void Enlarge(KyFloat32 enlargement)
	{
		m_min.x -= enlargement;
		m_max.x += enlargement;
		m_min.y -= enlargement;
		m_max.y += enlargement;
		m_min.z -= enlargement;
		m_max.z += enlargement;
	}

	/// Returns true if the specified position is contained within the extents of the bounding box. 
	KY_INLINE bool IsPoint3DInside(const Vec3f& p) const
	{
		const KyFloat32 operand1 = Fsel(p.x - m_min.x, 1.f, 0.f);
		const KyFloat32 operand2 = Fsel(m_max.x - p.x, 1.f, 0.f);
		const KyFloat32 operand3 = Fsel(p.y - m_min.y, 1.f, 0.f);
		const KyFloat32 operand4 = Fsel(m_max.y - p.y, 1.f, 0.f);
		const KyFloat32 operand5 = Fsel(p.z - m_min.z, 1.f, 0.f);
		const KyFloat32 operand6 = Fsel(m_max.z - p.z, 1.f, 0.f);

		return operand1 * operand2 * operand3 * operand4 * operand5 * operand6 > 0.f;
	}

	/// Returns true if the (X,Y) coordinates of the specified position are contained within the 
	/// (X,Y) extents extents of the bounding box. 
	KY_INLINE bool IsPoint2DInside(const Vec2f& p) const
	{
		const KyFloat32 operand1 = Fsel(p.x - m_min.x, 1.f, 0.f);
		const KyFloat32 operand2 = Fsel(m_max.x - p.x, 1.f, 0.f);
		const KyFloat32 operand3 = Fsel(p.y - m_min.y, 1.f, 0.f);
		const KyFloat32 operand4 = Fsel(m_max.y - p.y, 1.f, 0.f);

		return operand1 * operand2 * operand3 * operand4 > 0.f;
	}


	// ---------------------------------- Public Data Members ----------------------------------

	Vec3f m_min; ///< The minima of the bounding box. 
	Vec3f m_max; ///< The maxima of the bounding box. 
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped. 
inline void SwapEndianness(Endianness::Target e, Box3f& self)
{
	SwapEndianness(e, self.m_min);
	SwapEndianness(e, self.m_max);
}

}


#endif

