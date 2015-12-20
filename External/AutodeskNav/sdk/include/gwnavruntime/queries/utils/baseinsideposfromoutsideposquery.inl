/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{
KY_INLINE void BaseInsidePosFromOutsidePosQuery::SetHorizontalTolerance(KyFloat32 horizontalTolerance)      { m_horizontalTolerance = fabs(horizontalTolerance); }
KY_INLINE void BaseInsidePosFromOutsidePosQuery::SetDistFromObstacle(KyFloat32 distFromObstacle)            { m_distFromObstacle    = fabs(distFromObstacle);    }
KY_INLINE void BaseInsidePosFromOutsidePosQuery::SetInputIntegerPos(const WorldIntegerPos& inputIntegerPos) { m_inputIntegerPos     = inputIntegerPos;           }
KY_INLINE void BaseInsidePosFromOutsidePosQuery::SetResult(InsidePosFromOutsidePosQueryResult result)       { m_result              = result;                    }

KY_INLINE void BaseInsidePosFromOutsidePosQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE InsidePosFromOutsidePosQueryResult BaseInsidePosFromOutsidePosQuery::GetResult()   const { return m_result; }

KY_INLINE const Vec3f&           BaseInsidePosFromOutsidePosQuery::GetInputPos()             const { return m_inputPos3f;           }
KY_INLINE const Vec3f&           BaseInsidePosFromOutsidePosQuery::GetInsidePos()            const { return m_insidePos3f;          }
KY_INLINE const NavTrianglePtr&  BaseInsidePosFromOutsidePosQuery::GetInsidePosTrianglePtr() const { return m_insidePosTrianglePtr; }
KY_INLINE KyFloat32              BaseInsidePosFromOutsidePosQuery::GetHorizontalTolerance()  const { return m_horizontalTolerance;  }
KY_INLINE KyFloat32              BaseInsidePosFromOutsidePosQuery::GetDistFromObstacle()     const { return m_distFromObstacle;     }
KY_INLINE const WorldIntegerPos& BaseInsidePosFromOutsidePosQuery::GetInputIntegerPos()      const { return m_inputIntegerPos;      }
KY_INLINE const WorldIntegerPos& BaseInsidePosFromOutsidePosQuery::GetInsideIntegerPos()     const { return m_insideIntegerPos;     }

KY_INLINE const PositionSpatializationRange& BaseInsidePosFromOutsidePosQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
