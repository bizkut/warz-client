/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE void TriangleFromPosQuery::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

KY_INLINE void TriangleFromPosQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}
KY_INLINE void TriangleFromPosQuery::SetInputIntegerPos(const WorldIntegerPos& integerPos) { m_inputIntegerPos = integerPos; }
KY_INLINE void TriangleFromPosQuery::SetQueryType(TriangleFromPosQueryType queryType)      { m_queryType       = queryType;  }
KY_INLINE void TriangleFromPosQuery::SetResult(TriangleFromPosQueryResult result)          { m_result          = result;     }

KY_INLINE TriangleFromPosQueryResult TriangleFromPosQuery::GetResult()                         const { return m_result;             }
KY_INLINE TriangleFromPosQueryType   TriangleFromPosQuery::GetQueryType()                      const { return m_queryType;          }
KY_INLINE const WorldIntegerPos&     TriangleFromPosQuery::GetInputIntegerPos()                const { return m_inputIntegerPos;    }
KY_INLINE const Vec3f&               TriangleFromPosQuery::GetInputPos()                       const { return m_inputPos3f;         }
KY_INLINE const NavTrianglePtr&      TriangleFromPosQuery::GetResultTrianglePtr()              const { return m_resultTrianglePtr;  }
KY_INLINE KyFloat32                  TriangleFromPosQuery::GetAltitudeOfProjectionInTriangle() const { return m_altitudeInTriangle; }

KY_INLINE const PositionSpatializationRange& TriangleFromPosQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
