/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_CircleArcSplineBubbleData_H
#define Navigation_CircleArcSplineBubbleData_H


#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{

class Channel;
class SplineConfig;
class Bubble;


// Internal: Class used to enrich Bubbles with some data useful for
// CircleArcSpline computation.
class CircleArcSplineBubbleData
{
public:
	enum BubbleType
	{
		UndefinedBubbleType,
		CornerBubble,
		StartBubble,
		StartConstraintBubble,
		EndBubble
	};

	CircleArcSplineBubbleData();
	CircleArcSplineBubbleData(const Channel* channel, KyUInt32 index);
	CircleArcSplineBubbleData(const Channel* channel, KyUInt32 firstGateIndex, KyUInt32 lastGateIndex);

	Bubble*      GetBubble()           const { return m_bubble;              }
	BubbleType   GetBubbleType()       const { return m_bubbleType;          }
	KyUInt32     GetFirstGateIndex()   const { return m_firstGateIndex;      }
	KyUInt32     GetLastGateIndex()    const { return m_lastGateIndex;       }
	KyFloat32    GetMinRadius()        const { return m_minRadius;           }
	KyFloat32    GetMaxRadius()        const { return m_maxRadius;           }
	const Vec3f& GetMidPoint()         const { return m_midPoint;            }
	const Vec2f& GetMedianTurnAxis()   const { return m_medianTurnAxis;      }
	const Vec3f& GetEntryPos()         const { return m_entryPos;            }
	const Vec3f& GetExitPos()          const { return m_exitPos;             }
	KyUInt32     GetEntrySectionIdx()  const { return m_entrySectionIdx;     }
	KyUInt32     GetExitSectionIdx()   const { return m_exitSectionIdx;      }
	bool         HasRangeBeenDefined() const { return m_hasRangeBeenDefined; }

	void SetBubble(Bubble* bubble, BubbleType bubbleType);

	void UpdateDataAsSinglePoint(const Vec3f& position, KyUInt32 sectionIdx);

	void UpdateData(const Vec3f& entryPos, KyUInt32 entrySectionIdx, const Vec3f& exitPos, KyUInt32 exitSectionIdx);

	bool IsNewRangeAdmissible(const SplineConfig& splineConfig, const Vec3f& entryPos, KyUInt32 entrySectionIdx, const Vec3f& exitPos, KyUInt32 exitSectionIdx) const;

	void Reset();

	KyFloat32 GetMinDistLeft() const;
	KyFloat32 GetMinDistRight() const;

	bool IsPositionInTurnRange(const Vec2f& position) const;

	static bool IsTurnRangeValid(const SplineConfig& splineConfig, const Bubble& bubble, const Channel* channel,
		const Vec3f& entryPos, KyUInt32 entrySectionIdx, const Vec3f& exitPos, KyUInt32 exitSectionIdx);

private:
	void ComputeRadiusRange();
	void ComputeMedianTurnAxis();


	const Channel* m_channel;
	Bubble* m_bubble;

	BubbleType m_bubbleType;

	// Both are inclusive
	KyUInt32 m_firstGateIndex;
	KyUInt32 m_lastGateIndex;

	KyFloat32 m_minRadius;
	KyFloat32 m_maxRadius;

	bool m_hasRangeBeenDefined;
	Vec2f m_medianTurnAxis; // The axis bisecting incoming border and outgoing border
	Vec3f m_midPoint; // The point at MedianTurnAxis and Bubble circle intersection. We try to keep this position on the Bubble when optimizing the spline.

	Vec3f m_entryPos;
	KyUInt32 m_entrySectionIdx;

	Vec3f m_exitPos;
	KyUInt32 m_exitSectionIdx;
};


} // namespace Kaim

#endif // Navigation_CircleArcSplineBubbleData_H
