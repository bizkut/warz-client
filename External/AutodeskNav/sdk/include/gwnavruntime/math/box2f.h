/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Box2f_H
#define Navigation_Box2f_H

#include "gwnavruntime/math/box3f.h"

namespace Kaim
{

/// This class represents a two-dimensional axis-aligned bounding box whose dimensions are stored using
/// floating-point numbers.
class Box2f
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Constructors ----------------------------------

	/// Creates a new Box2f with invalid extents: you must call Set() before using it.
	Box2f() { Clear(); }

	/// Creates a new Box2f with the provided extents.
	/// \param _min			The minima of the bounding box.
	/// \param _max			The maxima of the bounding box. 
	Box2f(const Vec2f& _min, const Vec2f& _max) :
		m_min(_min), m_max(_max) {}

	/// Creates a new Box2f with the provided extents.
	/// \param min_x		The minimum position of the bounding box on the X axis.
	/// \param min_y		The minimum position of the bounding box on the Y axis.
	/// \param max_x		The maximum position of the bounding box on the X axis.
	/// \param max_y		The maximum position of the bounding box on the Y axis. 
	Box2f(KyFloat32 min_x, KyFloat32 min_y, KyFloat32 max_x, KyFloat32 max_y) :
		m_min(min_x, min_y), m_max(max_x, max_y) {}


	// ---------------------------------- Main API Functions ----------------------------------

	/// Returns true if both the minima and maxima of other are the same as the minima and maxima of this object. 
	bool operator==(const Box2f& other) const { return m_min == other.m_min && m_max == other.m_max; }

	/// Returns true if either the minima and maxima of other is different from the minima or maxima of this object. 
	bool operator!=(const Box2f& other) const { return !operator==(other); }

	/// Clears all information maintained by this object. This method sets the extents of the box to invalid values; 
	/// you must follow this method with a call to Set() before you can use the box. 
	KY_INLINE void Clear()
	{
		m_min.Set(KyFloat32MAXVAL , KyFloat32MAXVAL);
		m_max.Set(-KyFloat32MAXVAL, -KyFloat32MAXVAL);
	}

	/// Return true if m_min.x <= m_max.x && m_min.y <= m_max.y
	KY_INLINE bool IsValid()
	{
		return m_min.x <= m_max.x && m_min.y <= m_max.y;
	}

	/// Sets the extents of the bounding box to the specified values. 
	/// \param min_			The minima of the bounding box.
	/// \param max_			The maxima of the bounding box. 
	KY_INLINE void Set(const Vec2f& min_, const Vec2f& max_)
	{
		m_min = min_;
		m_max = max_;
	}

	KY_INLINE void Set(KyFloat32 min_x, KyFloat32 min_y, KyFloat32 max_x, KyFloat32 max_y)
	{
		m_min.x = min_x;
		m_min.y = min_y;
		m_max.x = max_x;
		m_max.y = max_y;
	}

	/// Retrieves the extents of the box along the X axis. 
	KY_INLINE KyFloat32 SizeX() { return m_max.x - m_min.x; }
	
	/// Retrieves the extents of the box along the Y axis. 
	KY_INLINE KyFloat32 SizeY() { return m_max.y - m_min.y; }

	/// Enlarges the extents of the bounding box to include the specified two-dimensional point. 
	/// If the point is already contained within the bounding box, the box is not modified. 
	void ExpandByVec2(const Vec2f& pos)
	{
		const KyFloat32 posx = pos.x;
		const KyFloat32 posy = pos.y;
		m_min.x = Kaim::Min(m_min.x, posx);
		m_min.y = Kaim::Min(m_min.y, posy);
		m_max.x = Kaim::Max(m_max.x, posx);
		m_max.y = Kaim::Max(m_max.y, posy);
	}

	/// Enlarges the extents of the bounding box to include the (X,Y) coordinates of the specified 
	/// three-dimensional point. If the (X,Y) coordinates of the point are already contained within 
	/// the bounding box, the box is not modified. 
	void ExpandByVec3(const Vec3f& pos)
	{
		const KyFloat32 posx = pos.x;
		const KyFloat32 posy = pos.y;
		m_min.x = Kaim::Min(m_min.x, posx);
		m_min.y = Kaim::Min(m_min.y, posy);
		m_max.x = Kaim::Max(m_max.x, posx);
		m_max.y = Kaim::Max(m_max.y, posy);
	}

	/// Enlarges the extents of the bounding box to include the area covered by the specified bounding box. 
	/// If the bounding box already encompasses the area of box, the bounding box is not modified. 
	void ExpandByBox2(const Box2f& box)
	{
		m_min.x = Kaim::Min(m_min.x, box.m_min.x);
		m_max.x = Kaim::Max(m_max.x, box.m_max.x);
		m_min.y = Kaim::Min(m_min.y, box.m_min.y);
		m_max.y = Kaim::Max(m_max.y, box.m_max.y);
	}

	/// Enlarges the extents of the bounding box to include the area covered by the specified bounding box
	/// on the plane of the (X,Y) axes. 
	/// If the bounding box already encompasses the area of box, the box is not modified. 
	void ExpandByBox3(const Box3f& box)
	{
		m_min.x = Kaim::Min(m_min.x, box.m_min.x);
		m_max.x = Kaim::Max(m_max.x, box.m_max.x);
		m_min.y = Kaim::Min(m_min.y, box.m_min.y);
		m_max.y = Kaim::Max(m_max.y, box.m_max.y);
	}

	/// Enlarges the extents of the bounding box by the specified amount in all directions. 
	void Enlarge(KyFloat32 enlargement)
	{
		m_min.x -= enlargement;
		m_max.x += enlargement;
		m_min.y -= enlargement;
		m_max.y += enlargement;
	}

	/// Indicates whether or not the specified two-dimensional point is inside the area covered by the bounding box. 
	KY_INLINE bool IsPoint2DInside(const Vec2f& p) const {
		const KyFloat32 operand1 = Fsel(p.x - m_min.x, 1.f, 0.f); // operand1 == 1.f if pos.x >= cellBox.m_min.x
		const KyFloat32 operand2 = Fsel(m_max.x - p.x, 1.f, 0.f); // operand2 == 1.f if cellBox.m_max.x >= pos.x
		const KyFloat32 operand3 = Fsel(p.y - m_min.y, 1.f, 0.f); // operand3 == 1.f if pos.y >= cellBox.m_min.y
		const KyFloat32 operand4 = Fsel(m_max.y - p.y, 1.f, 0.f); // operand4 == 1.f if cellBox.m_max.y >= pos.y
		return operand1 * operand2 * operand3 * operand4 > 0.f;
	}
	
	/// Indicates whether or not the (X,Y) coordinates of the specified three-dimensional point are inside the area 
	/// covered by the bounding box. 
	KY_INLINE bool IsPoint3DInside(const Vec3f& p) const
	{
		const KyFloat32 operand1 = Fsel(p.x - m_min.x, 1.f, 0.f); // operand1 == 1.f if pos.x >= cellBox.m_min.x
		const KyFloat32 operand2 = Fsel(m_max.x - p.x, 1.f, 0.f); // operand2 == 1.f if cellBox.m_max.x >= pos.x
		const KyFloat32 operand3 = Fsel(p.y - m_min.y, 1.f, 0.f); // operand3 == 1.f if pos.y >= cellBox.m_min.y
		const KyFloat32 operand4 = Fsel(m_max.y - p.y, 1.f, 0.f); // operand4 == 1.f if cellBox.m_max.y >= pos.y
		return operand1 * operand2 * operand3 * operand4 > 0.f;
	}

public:
	Vec2f m_min; //< Stores the minima of the box.
	Vec2f m_max; //< Stores the maxima of the box.
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.  
inline void SwapEndianness(Endianness::Target e, Box2f& self)
{
	SwapEndianness(e, self.m_min);
	SwapEndianness(e, self.m_max);
}


}


#endif

