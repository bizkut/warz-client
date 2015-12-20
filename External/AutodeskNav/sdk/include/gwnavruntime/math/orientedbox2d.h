/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_OrientedBox2d_H
#define Navigation_OrientedBox2d_H


#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/kernel/SF_Debug.h"

namespace Kaim
{

/*
     ^       +--------------------+
     |       |                    |
     |width  |                    |
     |       |                    |
     |       |                    |        orientation
     v      A+--------------------+------------->

             <-------------------->
                   length
*/

/// This class represents a three-dimensional bounding box with a free orientation on the (X,Y) plane.
/// It is defined by:
/// -	The position of a single reference corner (#m_a).
/// -	An orientation vector that determines the facing direction of the box from the reference corner (#m_normalizedOrientation).
/// -	A length value, which determines the extents of the box along its orientation vector (#m_length).
/// -	A width value, which determines the extents of the box perpendicular to the orientation vector (#m_width).
/// 	The width of the box always extends to the left when facing the orientation.
/// -	A thickness value, which determines the extents of the box along the vertical Z axis (#m_thickness).
/// For example:
/// \code
///  ^       +--------------------+
///  |       |                    |
///  |width  |                    |
///  |       |                    |
///  |       |                    |        orientation
///  v      A+--------------------+------------->
/// 
///          <-------------------->
///                 length
/// \endcode
class OrientedBox2d
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	// ---------------------------------- Constructors ----------------------------------

	/// Creates a new OrientedBox2d with 0.0f length, width and thickness.
	/// If you use this constructor, you must call Set() to set the dimensions of the rectangle
	/// before it can be used.
	OrientedBox2d();

	/// Creates a new OrientedBox2d and initialize it accordingly to provided information.
	/// \param a				The position of the reference corner.
	/// \param orientation		The facing orientation of the box.
	/// \param length			The extents of the box along its orientation vector.
	/// \param width			The extents of the box perpendicular to its orientation vector on the (X,Y) plane.
	/// \param thickness		The extents of the box along the vertical Z axis. 
	OrientedBox2d(const Vec3f& a, const Vec2f& orientation, KyFloat32 length, KyFloat32 width, KyFloat32 thickness);

	// ---------------------------------- Main API Functions ----------------------------------

	bool IsValid();
	void Clear();

	/// Sets the dimensions of the box.
	/// \param a				The position of the reference corner.
	/// \param orientation		The facing orientation of the box.
	/// \param length			The extents of the box along its orientation vector.
	/// \param width			The extents of the box perpendicular to its orientation vector on the (X,Y) plane.
	/// \param thickness		The extents of the box along the vertical Z axis. 
	void Set(const Vec3f& a, const Vec2f& orientation, KyFloat32 length, KyFloat32 width, KyFloat32 thickness);

	/// Creates a new OrientedBox2d that will be an inflate of the segment going from start to dest of radius
	/// Object created using this constructor
	void InitAs2dInflatedSegment(const Vec3f& start, const Vec3f& dest, KyFloat32 radius);

	bool IsInside2d(const Vec3f& pos);

	// ---------------------------------- Public Data Members ----------------------------------

	Vec3f m_a; ///< The position of the reference corner. 
	Vec2f m_normalizedOrientation; ///< The facing orientation of the box (unit vector). 
	KyFloat32 m_length; ///< The extents of the box along its orientation vector. 
	KyFloat32 m_width; ///< The extents of the box perpendicular to its orientation vector on the (X,Y) plane. 
	KyFloat32 m_thickness; ///< The extents of the box along the vertical Z axis. 
};


KY_INLINE OrientedBox2d::OrientedBox2d() : m_length(0.f), m_width(0.f), m_thickness(0.f) {}

KY_INLINE OrientedBox2d::OrientedBox2d(const Vec3f& a, const Vec2f& orientation, KyFloat32 length, KyFloat32 width, KyFloat32 thickness) :
m_a(a), m_length(length), m_width(width), m_thickness(thickness)
{
	orientation.GetNormalized(m_normalizedOrientation);
}

KY_INLINE bool OrientedBox2d::IsValid() { return m_normalizedOrientation != Vec2f::Zero(); }
KY_INLINE void OrientedBox2d::Clear() { Set(Vec3f::Zero(), Vec2f::Zero(), 0.f, 0.f, 0.f);  }

KY_INLINE void OrientedBox2d::Set(const Vec3f& a, const Vec2f& orientation, KyFloat32 length, KyFloat32 width, KyFloat32 thickness)
{
	KY_DEBUG_ASSERTN(orientation.GetLength() == Vec2f(orientation).Normalize(), ("Wrong BoxObtacle parameter, the orientation should be normalized"));

	m_a.Set(a.x, a.y, a.z);
	m_normalizedOrientation.Set(orientation.x, orientation.y);
	m_length = length;
	m_width = width;
	m_thickness = thickness;
}

inline void OrientedBox2d::InitAs2dInflatedSegment(const Vec3f& start, const Vec3f& dest, KyFloat32 radius)
{
	Vec2f orientation = dest.Get2d() - start.Get2d();
	KyFloat32 length = orientation.GetNormalized(m_normalizedOrientation);

	if (length == 0.f)
		m_normalizedOrientation = Vec2f::UnitX();

	m_length = length + 2.f*radius;
	m_width = 2.f*radius;
	// a = start -  m_normalizedOrientation * radius - m_normalizedOrientation.PerpCCW() * radius
	m_a.x = start.x - m_normalizedOrientation.x * radius + m_normalizedOrientation.y * radius;
	m_a.y = start.y - m_normalizedOrientation.y * radius - m_normalizedOrientation.x * radius;
	m_a.z = -KyFloat32MAXVAL;
	m_thickness = KyFloat32MAXVAL;
}

inline bool OrientedBox2d::IsInside2d(const Vec3f& pos)
{
	const Vec2f AtoP = pos.Get2d() - m_a.Get2d();
	KyFloat32 dotProd1 = DotProduct(AtoP, m_normalizedOrientation);
	KyFloat32 dotProd2 = DotProduct(AtoP, m_normalizedOrientation.PerpCCW());
	if (dotProd1 < 0.f || dotProd1 > m_length ||
		dotProd2 < 0.f || dotProd2 > m_width )
		return false;

	return true;
}

}

#endif

