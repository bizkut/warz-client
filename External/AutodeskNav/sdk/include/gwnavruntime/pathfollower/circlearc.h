/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_CircleArc_H
#define Navigation_CircleArc_H

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/channel/bubble.h"

namespace Kaim
{

class CircleArcBlob;
class CircleArcDisplayConfig;


/// Class representing either an oriented circle arc or a straight line segment,
/// to be typically aggregated into a CircleArcSpline.
class CircleArc
{
public:
	/// Create an uninitialized CircleArc.
	CircleArc()
		: m_length2d(0.0f)
	{}

	/// Create a CircleArc as sub part of an oriented circle (a.k.a. Bubble).
	CircleArc(const Bubble& bubble, const Vec3f& startPosition, KyUInt32 startSectionIdx, const Vec3f& endPosition, KyUInt32 endSectionIdx);

	/// Create a CircleArc representing a straight line segment.
	CircleArc(const Vec3f& startPosition, KyUInt32 startSectionIdx, const Vec3f& endPosition, KyUInt32 endSectionIdx);

	void InitFromBlob(const CircleArcBlob* blob);

	void Reset();

	const Bubble&     GetBubble()            const { return m_bubble; }
	const Vec3f&      GetStartPosition()     const { return m_startPosition; }
	const Vec3f&      GetMidPoint()          const { return m_midPoint; }
	const Vec3f&      GetEndPosition()       const { return m_endPosition; }
	KyUInt32          GetStartSectionIdx()   const { return m_startSectionIdx; }
	KyUInt32          GetEndSectionIdx()     const { return m_endSectionIdx; }
	KyFloat32         GetLength2d()          const { return m_length2d; }

	const Vec3f&      GetCenter()            const { return m_bubble.GetCenter(); }
	KyFloat32         GetRadius()            const { return m_bubble.GetRadius(); }
	RotationDirection GetRotationDirection() const { return m_bubble.GetRotationDirection(); }

	bool IsInitialized() const { return (m_length2d > 0.0f); }
	bool IsASegment() const { return (GetRadius() == KyFloat32MAXVAL); }
	bool IsCurve() const { return (GetRadius() != KyFloat32MAXVAL); }

	void Display(const CircleArcDisplayConfig& displayConfig, ScopedDisplayList& displayList_arc, ScopedDisplayList& displayList_extremities,
		ScopedDisplayList& displayList_sectionIdx, ScopedDisplayList& displayList_radius) const;

	/// Sets start position related information. It then calls ComputeProperties
	/// to maintain internals up-to-date.
	void SetStartPosition(const Vec3f& startPosition, KyUInt32 startSectionIdx);

	/// Sets end position related information. It then calls ComputeProperties
	/// to maintain internals up-to-date.
	void SetEndPosition(const Vec3f& endPosition, KyUInt32 endSectionIdx);

	/// Computes the tangent direction at the given position. It does not check
	/// (nor need) the position is actually on the CircleArc.
	Vec2f ComputeTangent(const Vec3f& position) const;

	/// Move a provided position forward along the CircleArc. Note that the resulting
	/// position is always on the CircleArc, even if it was not on it when entering
	/// the function. If distance2d is larger than the distance up to circle arc end,
	/// the position is clamped on the end position.
	/// \return The 2D-distance actually moved, taken along the CircleArc. The distance actually traveled
	///         by position is slightly different when it was not on the CircleArc when entering this function.
	KyFloat32 MoveForward(Vec3f& position, KyFloat32 distance2d) const;

	/// Move a provided position backward along the CircleArc. Note that the resulting
	/// position is always on the CircleArc, even if it was not on it when entering
	/// the function. If distance2d is greater than the distance from the circle arc start,
	/// the position is clamped to the start position.
	/// \return The 2D-distance actually moved, taken along the CircleArc. The distance actually traveled
	///         by position is slightly different when it was not on the CircleArc when entering this function.
	KyFloat32 MoveBackward(Vec3f& position, KyFloat32 distance2d) const;

	/// Move the provided position to the one on CircleArc at the given distance
	/// from start position.
	/// \pre distanceFromStart must be less than m_length2d.
	void MoveTo(Vec3f& position, KyFloat32 distanceFromStart) const;

	/// Computes the angle between start position and the provided position seen
	/// from the bubble center.
	/// \pre This method asserts the CircleArc is not a segment. Take care to check
	/// this before calling this method.
	KyFloat32 ComputeAngleFromStart(const Vec3f& position) const;

	KyFloat32 ComputeDistance2dFromStart(const Vec3f& position) const;
	KyFloat32 ComputeDistance2dToEnd(const Vec3f& position) const;


private:
	void ComputeProperties();


	// Primary members, actually defining the CircleArc.
	Bubble m_bubble;
	Vec3f m_startPosition;
	Vec3f m_midPoint;
	Vec3f m_endPosition;
	KyUInt32 m_startSectionIdx;
	KyUInt32 m_endSectionIdx;

	// Secondary members, useful values resulting from the primary members.
	KyFloat32 m_length2d;

	// Internal:
	// ReferenceUnitVector is the 2D unit vector used as main reference direction
	// for this CircleArc. The meaning differs accordingly to whether this CircleArc is
	// degenerated (a line segment) or a generic case one (actually an arc of an oriented circle).
	//  - In the generic case, it is the direction from Bubble center to the start position.
	//  - In the degenerated case, it is the direction from start position to end position.
	// In both case, it is used to compute the dot products and cross products necessary to
	// locate a position relatively to this CircleArc.
	Vec2f m_referenceUnitVector;
};

}

#endif //Navigation_CircleArc_H
