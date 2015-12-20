/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_Funnel_H
#define Navigation_Funnel_H

#include "gwnavruntime/channel/diagonalstrip.h"


namespace Kaim
{

enum FunnelDirection
{
	ForwardFunnel,
	BackwardFunnel
};

enum FunnelIntegrateCornerResult
{
	FunnelIntegrateCornerFailure,
	CornerIgnored,
	FunnelUnchanged,
	FunnelTightened,
	FunnelClosedOverRightCorner,
	FunnelClosedOverLeftCorner
};


/// Basic straight line Funnel to be used in standard line-of-view based browsing of diagonal strips.
class Funnel
{
public:
	Funnel(const DiagonalStrip* diagonalstrip)
		: m_diagonalstrip(diagonalstrip)
		, m_funnelDirection(ForwardFunnel)
		, m_previousResult(FunnelUnchanged)
	{}

	void Initialize(KyUInt32 apexDiagonalStripCornerIdx, FunnelDirection funnelDirection);

	/// Initialize the funnel to its current closure corner and updates cornerIdx to the next
	/// corner to be added to the funnel.
	void InitializeToNextApex(KyUInt32& cornerIdx);

	FunnelIntegrateCornerResult UpdateAccordinglyToDiagonalStripCorner(KyUInt32 cornerIdx);

	const DiagonalStrip*        GetDiagonalStrip()   const { return m_diagonalstrip;   }
	FunnelDirection             GetFunnelDirection() const { return m_funnelDirection; }
	KyUInt32                    GetApexIdx()         const { return m_apexIdx;         }
	FunnelIntegrateCornerResult GetPreviousResult()  const { return m_previousResult;  }

	static bool IsClosureCase(FunnelIntegrateCornerResult result) { return ((result == FunnelClosedOverRightCorner) || (result == FunnelClosedOverLeftCorner)); }

private:
	bool IsStrictlyOutsideOnLeft(const Vec2f& axis2d)  const { return ((CrossProduct(axis2d, m_vec2dToLeft) < 0.0f) && (DotProduct(axis2d, m_vec2dToLeft) > 0.0f)); }
	bool IsStrictlyOutsideOnRight(const Vec2f& axis2d) const { return ((CrossProduct(axis2d, m_vec2dToRight) > 0.0f) && (DotProduct(axis2d, m_vec2dToRight) > 0.0f)); }


	const DiagonalStrip* m_diagonalstrip;
	FunnelDirection m_funnelDirection;

	KyUInt32 m_apexIdx;
	KyUInt32 m_leftCornerIdx;
	KyUInt32 m_rightCornerIdx;

	Vec2f m_vec2dToLeft;
	Vec2f m_vec2dToRight;

	FunnelIntegrateCornerResult m_previousResult;
};

} // namespace Kaim

#endif // Navigation_Funnel_H
