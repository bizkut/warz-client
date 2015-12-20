/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{

KY_INLINE void BaseNearestBorderHalfEdgeFromPosQuery::SetHorizontalTolerance(KyFloat32 horizontalTolerance)      { m_horizontalTolerance = fabs(horizontalTolerance); }
KY_INLINE void BaseNearestBorderHalfEdgeFromPosQuery::SetInputIntegerPos(const WorldIntegerPos& inputIntegerPos) { m_inputIntegerPos     = inputIntegerPos;           }
KY_INLINE void BaseNearestBorderHalfEdgeFromPosQuery::SetResult(NearestBorderHalfEdgeFromPosQueryResult result)  { m_result              = result;                    }

KY_INLINE void BaseNearestBorderHalfEdgeFromPosQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE NearestBorderHalfEdgeFromPosQueryResult BaseNearestBorderHalfEdgeFromPosQuery::GetResult()     const { return m_result; }

KY_INLINE KyFloat32              BaseNearestBorderHalfEdgeFromPosQuery::GetHorizontalTolerance()         const { return m_horizontalTolerance;              }
KY_INLINE KyFloat32              BaseNearestBorderHalfEdgeFromPosQuery::GetSquareDistFromNearestBorder() const { return m_squareDistFromHalfEdge;           }
KY_INLINE const Vec3f&           BaseNearestBorderHalfEdgeFromPosQuery::GetInputPos()                    const { return m_inputPos3f;                       }
KY_INLINE const Vec3f&           BaseNearestBorderHalfEdgeFromPosQuery::GetNearestPosOnHalfEdge()        const { return m_nearestPosOnHalfEdge;             }
KY_INLINE const NavHalfEdgePtr&  BaseNearestBorderHalfEdgeFromPosQuery::GetNearestHalfEdgePtrOnBorder()  const { return m_nearestHalfEdgeOnBorder;          }
KY_INLINE const NavHalfEdgePtr&  BaseNearestBorderHalfEdgeFromPosQuery::GetNextHalfEdgePtrAlongBorder()  const { return m_nextHalfEdgeOnBorder;             }
KY_INLINE const NavHalfEdgePtr&  BaseNearestBorderHalfEdgeFromPosQuery::GetPrevHalfEdgePtrAlongBorder()  const { return m_prevHalfEdgeOnBorder;             }
KY_INLINE const WorldIntegerPos& BaseNearestBorderHalfEdgeFromPosQuery::GetInputIntegerPos()             const { return m_inputIntegerPos;                  }
KY_INLINE const WorldIntegerPos& BaseNearestBorderHalfEdgeFromPosQuery::GetNearestIntegerPosOnHalfEdge() const { return m_nearestPointOnHalfEdgeIntegerPos; }

KY_INLINE const PositionSpatializationRange& BaseNearestBorderHalfEdgeFromPosQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
