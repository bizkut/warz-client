/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_BubbleFunnel_H
#define Navigation_BubbleFunnel_H

#include "gwnavruntime/channel/bubblearray.h"
#include "gwnavruntime/channel/bubblefunnelextremityconstraint.h"


namespace Kaim
{

enum BubbleFunnelIntegrateBubbleResult
{
	IntegrateBubbleFailure,
	BubbleIgnored,
	BubbleFunnelUnchanged,
	BubbleFunnelTightened,
	BubbleFunnelClosedOverRightBubble,
	BubbleFunnelClosedOverLeftBubble
};


class BubbleFunnelBorder
{
public:
	BubbleFunnelBorder()
		: m_isInitialized(false)
	{}

	void Clear()
	{
		m_isInitialized = false;
	}

	void Set(KyUInt32 bubbleIdx, const Vec3f& segmentStart, const Vec3f& segmentEnd)
	{
		m_segmentStart  = segmentStart;
		m_segmentEnd    = segmentEnd;
		m_segmentVector = segmentEnd.Get2d() - segmentStart.Get2d();
		m_bubbleIdx     = bubbleIdx;
		m_isInitialized = true;
	}

	const Vec3f&   GetSegmentStart() const { return m_segmentStart;  }
	const Vec3f&   GetSegmentEnd()   const { return m_segmentEnd;    }
	const Vec2f&   GetSegmentVec2d() const { return m_segmentVector; }
	KyUInt32       GetBubbleIdx()    const { return m_bubbleIdx;     }
	bool           IsInitialized()   const { return m_isInitialized; }

private:
	Vec3f    m_segmentStart;
	Vec3f    m_segmentEnd;
	Vec2f    m_segmentVector;
	KyUInt32 m_bubbleIdx;
	bool     m_isInitialized;
};


/// Basic straight line Funnel to be used in standard line-of-view based browsing
/// of Bubble array but ignoring the Bubble radii. It is used internally in the
/// BubbleFunnel to determine straight line visibility area to be browsed to complete
/// Bubble collection once the BubbleFunnel is closed.
class BubbleFunnelIgnoreRadius
{
public:
	BubbleFunnelIgnoreRadius(const BubbleArray* bubbleArray)
		: m_bubleArray(bubbleArray)
		, m_previousResult(BubbleFunnelUnchanged)
	{}

	void Initialize(KyUInt32 apexIdx);

	/// Initialize the funnel to its current closure corner and updates cornerIdx to
	/// the next corner to be added to the funnel.
	void InitializeToNextApex(KyUInt32& cornerIdx);

	BubbleFunnelIntegrateBubbleResult UpdateAccordinglyToBubble(KyUInt32 bubbleIdx);

	const BubbleArray*                GetBubbleArray()     const { return m_bubleArray;      }
	KyUInt32                          GetApexIdx()         const { return m_apexIdx;         }
	BubbleFunnelIntegrateBubbleResult GetPreviousResult()  const { return m_previousResult;  }

private:
	bool IsStrictlyOutsideOnLeft(const Vec2f& axis2d)  const { return ((CrossProduct(axis2d, m_vec2dToLeft) < 0.0f) && (DotProduct(axis2d, m_vec2dToLeft) > 0.0f)); }
	bool IsStrictlyOutsideOnRight(const Vec2f& axis2d) const { return ((CrossProduct(axis2d, m_vec2dToRight) > 0.0f) && (DotProduct(axis2d, m_vec2dToRight) > 0.0f)); }


	const BubbleArray* m_bubleArray;

	KyUInt32 m_apexIdx;
	KyUInt32 m_leftBubbleIdx;
	KyUInt32 m_rightBubbleIdx;

	Vec2f m_vec2dToLeft;
	Vec2f m_vec2dToRight;

	BubbleFunnelIntegrateBubbleResult m_previousResult;
};


/// Adaptive radius funnel to be used typically to string pull within a Bubble array.
class BubbleFunnel
{
public:
	BubbleFunnel(const BubbleArray* bubbleArray)
		: m_startConstraint(KY_NULL)
		, m_minBiTangentLength(0.0f)
		, m_straightLineFunnel(bubbleArray)
		, m_hasQuitApexBubble(false)
	{}

	void Initialize(KyUInt32 apexIdx, KyFloat32 minBiTangentLength = 0.0f);
	void InitializeWithStartConstraint(const BubbleFunnelExtremityConstraint* startConstraint, KyFloat32 minBiTangentLength = 0.0f);

	/// Update the funnel borders accordingly to the provided Bubble. If the Bubble
	/// is not ignored, its index is pushed in integratedBubbleIndexArray.
	BubbleFunnelIntegrateBubbleResult UpdateAccordinglyToBubble(KyUInt32 bubbleIdx);

	KyUInt32                  GetApexIdx()     const { return m_straightLineFunnel.GetApexIdx(); }
	const Bubble&             GetApexBubble()  const { return m_apexBubble;                      }
	const BubbleFunnelBorder& GetLeftBorder()  const { return m_leftBorder;                      }
	const BubbleFunnelBorder& GetRightBorder() const { return m_rightBorder;                     }
	const BubbleFunnelExtremityConstraint* GetStartConstraint() const { return m_startConstraint; }

	static bool IsClosureCase(BubbleFunnelIntegrateBubbleResult result) { return ((result == BubbleFunnelClosedOverRightBubble) || (result == BubbleFunnelClosedOverLeftBubble)); }

	bool IsStraightLineFunnelClosed() const { return IsClosureCase(m_straightLineFunnel.GetPreviousResult()); }

	/// Use this once the bubbles closed the BubbleFunnel, to continue Bubble
	/// array browsing up to straight line funnel is closed too.
	BubbleFunnelIntegrateBubbleResult UpdateStraightLineFunnelOnlyAccordinglyToBubble(KyUInt32 bubbleIdx);

private:
	const BubbleArray* GetBubbleArray() const { return m_straightLineFunnel.GetBubbleArray(); }
	bool IsStrictlyOutsideOnLeft(const Vec2f& axis2d)  const;
	bool IsStrictlyOutsideOnRight(const Vec2f& axis2d) const;

	// Parameters
	const BubbleFunnelExtremityConstraint* m_startConstraint;
	KyFloat32 m_minBiTangentLength; // Ignore bubble when this would lead to create tiny edges

	// Use a straight line standard funnel to manage standard closure condition.
	BubbleFunnelIgnoreRadius m_straightLineFunnel;

	// Keep radius-related funnel information.
	Bubble m_apexBubble;
	BubbleFunnelBorder m_leftBorder;
	BubbleFunnelBorder m_rightBorder;
	bool m_hasQuitApexBubble; // A flag to check the string has quit the apex Bubble.
};

} // namespace Kaim

#endif // Navigation_BubbleFunnel_H
