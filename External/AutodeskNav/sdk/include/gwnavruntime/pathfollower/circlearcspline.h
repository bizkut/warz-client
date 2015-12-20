/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_CircleArcSpline_H
#define Navigation_CircleArcSpline_H

#include "gwnavruntime/pathfollower/circlearc.h"
#include "gwnavruntime/containers/kyarray.h"


namespace Kaim
{

/// The class representing a spline compounded of oriented circle arcs and straight
/// line segments. This is namely used to store a mid-term trajectory to drive a
/// bot within a Channel in ChannelTrajectory.
class CircleArcSpline
{
public:
	CircleArcSpline() {}

	void Clear();
	bool IsEmpty() const;
	KyUInt32 GetCircleArcCount() const;

	/// \pre First ensure CircleArcSpline is not empty before calling this function, it does not check it.
	const CircleArc& GetCircleArc(KyUInt32 index) const;

	/// \pre First ensure CircleArcSpline is not empty before calling this function, it does not check it.
	const Vec3f& GetStartPosition() const;

	/// \pre First ensure CircleArcSpline is not empty before calling this function, it does not check it.
	const Vec3f& GetEndPosition() const;

	/// \pre First ensure CircleArcSpline is not empty before calling this function, it does not check it.
	KyUInt32 GetLastCircleArcIndex() const;

	/// \pre First ensure CircleArcSpline is not empty before calling this function, it does not check it.
	const CircleArc& GetLastCircleArc() const;

	/// \pre First ensure CircleArcSpline is not empty before calling this function, it does not check it.
	Vec2f ComputeStartTangent() const;

	/// \pre First ensure CircleArcSpline is not empty before calling this function, it does not check it.
	Vec2f ComputeEndTangent() const;


	void Display(const CircleArcDisplayConfig& displayConfig, ScopedDisplayList& displayList_arc, ScopedDisplayList& displayList_extremities,
		ScopedDisplayList& displayList_sectionIdx, ScopedDisplayList& displayList_radius) const;


public: // internal

	// Cuts tail and head to keep just the spline portion between provided positions.
	KyResult KeepSplineBetween(
		const Vec3f& newStartPosition, KyUInt32 newStartArcIndex, KyUInt32 newStartSectionIdx,
		const Vec3f& newEndPosition, KyUInt32 newEndArcIndex, KyUInt32 newEndSectionIdx);

	void Append(const KyArray<CircleArc>& circleArcs);
	void Reset(const KyArray<CircleArc>& circleArcs);
	void PushBack(const CircleArc& circleArc) { m_circleArcs.PushBack(circleArc); }

	const KyArray<CircleArc>& GetCircleArcArray() const { return m_circleArcs; }


private:
	KyResult CutTailAt(const Vec3f& position, KyUInt32 arcIndex, KyUInt32 sectionIdx);
	KyResult CutHeadAt(const Vec3f& position, KyUInt32 arcIndex, KyUInt32 sectionIdx);

	KyArray<CircleArc> m_circleArcs;
};

KY_INLINE void CircleArcSpline::Clear()                                      { m_circleArcs.Clear(); };
KY_INLINE void CircleArcSpline::Reset(const KyArray<CircleArc>& circleArcs)  { m_circleArcs = circleArcs; }

KY_INLINE bool             CircleArcSpline::IsEmpty()                    const { return m_circleArcs.IsEmpty(); }
KY_INLINE KyUInt32         CircleArcSpline::GetCircleArcCount()          const { return m_circleArcs.GetCount(); }

KY_INLINE const CircleArc& CircleArcSpline::GetCircleArc(KyUInt32 index) const { KY_ASSERT(IsEmpty() == false); return m_circleArcs[index]; }
KY_INLINE const Vec3f&     CircleArcSpline::GetStartPosition()           const { KY_ASSERT(IsEmpty() == false); return m_circleArcs[0].GetStartPosition(); }
KY_INLINE const Vec3f&     CircleArcSpline::GetEndPosition()             const { KY_ASSERT(IsEmpty() == false); return m_circleArcs[GetLastCircleArcIndex()].GetEndPosition(); }
KY_INLINE KyUInt32         CircleArcSpline::GetLastCircleArcIndex()      const { KY_ASSERT(IsEmpty() == false); return (m_circleArcs.GetCount() - 1); }
KY_INLINE const CircleArc& CircleArcSpline::GetLastCircleArc()           const { KY_ASSERT(IsEmpty() == false); return m_circleArcs[GetLastCircleArcIndex()]; }
KY_INLINE Vec2f            CircleArcSpline::ComputeStartTangent()        const { KY_ASSERT(IsEmpty() == false); return m_circleArcs[0].ComputeTangent(GetStartPosition()); }
KY_INLINE Vec2f            CircleArcSpline::ComputeEndTangent()          const { KY_ASSERT(IsEmpty() == false); return m_circleArcs[GetLastCircleArcIndex()].ComputeTangent(GetEndPosition()); }

}

#endif // Navigation_CircleArcSpline_H
