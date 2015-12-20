/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Box2LL_H
#define Navigation_Box2LL_H

#include "gwnavruntime/math/vec2ll.h"
#include "gwnavruntime/math/triangle3i.h"


namespace Kaim
{


/// Each instance of this class also maintains a count of the number of elements (or grid cells) contained
/// by the box along its X and Y axes, accessible through the CountX() and CountY() methods. For coordinate
/// boxes, all coordinates along the edge of the box are counted, including the corners. For grid boxes, all
/// grid cells that are contained within the box are counted.
class Box2LL
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Constructors ----------------------------------

	/// Creates a new Box2LL with invalid extents: you must call Set() before using it.
	Box2LL() { Clear(); }

	/// Creates a new Box2LL with the provided extents.
	/// \param min_			The minima of the bounding box.
	/// \param max_			The maxima of the bounding box. 
	Box2LL(const Vec2LL& min_, const Vec2LL& max_) : m_min(min_), m_max(max_) {}

	/// Creates a new Box2LL with the provided extents.
	/// \param min_x		The minimum position of the bounding box on the X axis.
	/// \param min_y		The minimum position of the bounding box on the Y axis.
	/// \param max_x		The maximum position of the bounding box on the X axis.
	/// \param max_y		The maximum position of the bounding box on the Y axis. 
	Box2LL(KyInt64 min_x, KyInt64 min_y, KyInt64 max_x, KyInt64 max_y) : m_min(min_x, min_y), m_max(max_x, max_y) {}


	// ---------------------------------- Main API Functions ----------------------------------

	/// Indicates whether or not the extents of the bounding box are valid. 
	KY_INLINE bool IsValid() { return CountX() >= 0 && CountY() >= 0; }

	/// Sets both the minima and maxima of the bounding box to (0,0). 
	KY_INLINE void MakeZero() { m_min.Set(0, 0); m_max.Set(0, 0); }

	/// Sets the extents of the bounding box to the specified values.
	/// \param min_			The minima of the bounding box.
	/// \param max_			The maxima of the bounding box. 
	KY_INLINE void Set(const Vec2LL& min_, const Vec2LL& max_) { m_min = min_; m_max = max_; }

	/// Sets the minima of the bounding box to the specified coordinates. 
	KY_INLINE void SetMin(const Vec2LL& min_) { m_min = min_; }

	/// Sets the maxima of the bounding box to the specified coordinates. 
	KY_INLINE void SetMax(const Vec2LL& max_) { m_max = max_; }

	/// Retrieves the minima of the bounding box. 
	KY_INLINE const Vec2LL& Min() const { return m_min; }

	/// Retrieves the maxima of the bounding box. 
	KY_INLINE const Vec2LL& Max() const { return m_max; }

	/// Retrieves the coordinates of the South-West corner of the bounding box. Since North is considered
	/// to be the positive direction of the Y axis and East is considered to be the positive direction of
	/// the Y axis, this corner is equivalent to the minima. 
	KY_INLINE Vec2LL CornerSouthWest() const { return m_min; }

	/// Retrieves the coordinates of the North-East corner of the bounding box. Since North is considered
	/// to be the positive direction of the Y axis and East is considered to be the positive direction of
	/// the Y axis, this corner is equivalent to the maxima. 
	KY_INLINE Vec2LL CornerNorthEast() const { return m_max; }

	/// Retrieves the coordinates of the South-East corner of the bounding box. Since North is considered
	/// to be the positive direction of the Y axis and East is considered to be the positive direction of
	/// the Y axis, this corner has the minimum value on the Y axis and the maximum value on the X axis. 
	KY_INLINE Vec2LL CornerSouthEast() const { return Vec2LL(m_max.x, m_min.y); }

	/// Retrieves the coordinates of the North-West corner of the bounding box. Since North is considered
	/// to be the positive direction of the Y axis and East is considered to be the positive direction of
	/// the Y axis, this corner has the maximum value on the Y axis and the minimum value on the X axis. 
	KY_INLINE Vec2LL CornerNorthWest() const { return Vec2LL(m_min.x, m_max.y); }

	/// Returns true if the specified position is contained within the extents of the bounding box
	/// or if the specified position is located on the edge of the bounding box. 
	KY_INLINE bool IsInside(const Vec2LL& pos) const { return pos.x >= m_min.x && pos.x <= m_max.x && pos.y >= m_min.y && pos.y <= m_max.y; }

	/// Returns true if the specified position is contained entirely within the extents of the bounding box. 
	KY_INLINE bool IsInsideStrictly(const Vec2LL& pos) const { return pos.x >  m_min.x && pos.x <  m_max.x && pos.y >  m_min.y && pos.y <  m_max.y; }

	/// Clears all information maintained by this object. This method sets the extents of the box to invalid values;
	/// you must follow this method with a call to Set() before you can use the box. 
	void Clear()
	{
		m_min.Set(KyInt64MAXVAL, KyInt64MAXVAL);
		m_max.Set(-KyInt64MAXVAL, -KyInt64MAXVAL);
	}

	/// Multiplies both the minima and maxima of the box by the specified value. 
	KY_INLINE Box2LL& operator*=(KyInt64 s) { m_min *= s; m_max *= s; return *this; }

	/// Divides both the minima and maxima of the box by the specified value. 
	KY_INLINE Box2LL& operator/=(KyInt64 s) { m_min /= s; m_max /= s; return *this; }

	/// Adds the specified two-dimensional vector to both the minima and maxima of the box. 
	KY_INLINE Box2LL& operator+=(const Vec2LL& v) { m_min += v; m_max += v; return *this; }

	/// Subtracts the specified two-dimensional vector from both the minima and maxima of the box. 
	KY_INLINE Box2LL& operator-=(const Vec2LL& v) { m_min -= v; m_max -= v; return *this; }

	/// Check equality 
	KY_INLINE bool operator==(const Box2LL& other) const { return other.m_min == m_min && other.m_max == m_max; }

	/// Enlarges the extents of the bounding box to include the specified two-dimensional point.
	/// If the point is already contained within the bounding box, the box is not modified. 
	void ExpandByVec2(const Vec2LL& pos)
	{
		ExpandByVec2_MinMaxOnly(pos);
	}

	/// Enlarges the extents of the bounding box to include the area covered by the specified bounding box.
	/// If the bounding box already encompasses the area of box, the bounding box is not modified. 
	void ExpandByBox2(const Box2LL& box)
	{
		ExpandByVec2_MinMaxOnly(box.m_min);
		ExpandByVec2_MinMaxOnly(box.m_max);
	}

	/// Enlarges the extents of the bounding box by the specified amount in all directions. 
	void Enlarge(KyInt64 enlargement)
	{
		m_min -= enlargement;
		m_max += enlargement;
	}

	/// Enlarges the extents of the bounding box by the specified amount in all directions, and
	/// stores the new dimensions in the enlarged parameter.
	/// \param enlargement			The amount to enlarge the box in each direction.
	/// \param[out] enlarged		Stores the enlarged bounding box. 
	void GetEnlarged(KyInt64 enlargement, Box2LL& enlarged) const
	{
		enlarged = *this;
		enlarged.Enlarge(enlargement);
	}

	/// Resizes this bounding box to contain only the grid cells that are common to both it and box.
	/// This has the following results:
	/// -	If box is entirely contained within this bounding box, this bounding box is resized to match
	/// 	the extents of box.
	/// -	If the edges of this bounding box cross the edges of box, the extents of this bounding box
	/// 	are resized to include only the area common to both bounding boxes.
	/// -	If this bounding box is entirely contained within the extents of box, this bounding box is
	/// 	not altered.
	/// -	If this bounding box does not intersect box at all, the extents of this bounding box are
	/// 	set to invalid values and this method returns false.
	/// \return true if the boxes intersect, or false if they do not. If this method
	/// 		returns false, the extents of this bounding box will be set to invalid values.
	/// 		You must call Set() before you can use the box again. 
	bool IntersectWith(const Box2LL& box)
	{
		m_min.x = Kaim::Max(m_min.x, box.m_min.x);
		m_max.x = Kaim::Min(m_max.x, box.m_max.x);
		m_min.y = Kaim::Max(m_min.y, box.m_min.y);
		m_max.y = Kaim::Min(m_max.y, box.m_max.y);
		return CountX() > 0 && CountY() > 0;
	}

	/// Resizes this bounding box to contain only the grid cells that are common to both box_1 and box_2.
	/// This has the following results:
	/// -	If one box is entirely contained within the other, this bounding box is set to match
	/// 	the extents of the smaller box.
	/// -	If the edges of the two boxes cross, the extents of this bounding box are resized to include
	/// 	only the area common to both box_1 and box_2.
	/// -	Ifbox_1 and box_2 do not intersect at all, the extents of this bounding box are
	/// 	set to invalid values and this method returns false.
	/// \return true ifbox_1 and box_2 intersect, or false if they do not. If this method
	/// 		returns false, the extents of this bounding box will be set to invalid values.
	/// 		You must call Set() before you can use the box again. 
	bool SetAsIntersection(const Box2LL& box_1, const Box2LL& box_2)
	{
		m_min.x = Kaim::Max(box_1.m_min.x, box_2.m_min.x);
		m_max.x = Kaim::Min(box_1.m_max.x, box_2.m_max.x);
		m_min.y = Kaim::Max(box_1.m_min.y, box_2.m_min.y);
		m_max.y = Kaim::Min(box_1.m_max.y, box_2.m_max.y);
		return CountX() > 0 && CountY() > 0;
	}

private:
	KY_INLINE KyInt64 CountX() { return m_max.x - m_min.x + 1; }
	KY_INLINE KyInt64 CountY() { return m_max.y - m_min.y + 1; }

	void ExpandByVec2_MinMaxOnly(const Vec2LL& pos)
	{
		const KyInt64 posx = pos.x;
		const KyInt64 posy = pos.y;
		m_min.x = Kaim::Min(m_min.x, posx);
		m_max.x = Kaim::Max(m_max.x, posx);
		m_min.y = Kaim::Min(m_min.y, posy);
		m_max.y = Kaim::Max(m_max.y, posy);
	}


public:
	Vec2LL m_min;
	Vec2LL m_max;
};


template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const Kaim::Box2LL& v)
{
	os << "{" << v.Min() << ";" << v.Max() << "}";
	return os;
}


} // namespace Kaim




#endif

