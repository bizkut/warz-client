/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE void BaseSpatializedPointCollectorInAABBQuery::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)    { m_startIntegerPos    = startIntegerPos;    }
KY_INLINE void BaseSpatializedPointCollectorInAABBQuery::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)   { m_startTrianglePtr   = startTrianglePtr;   }
KY_INLINE void BaseSpatializedPointCollectorInAABBQuery::SetResult(SpatializedPointCollectorInAABBQueryResult result)  { m_result             = result;             }
KY_INLINE void BaseSpatializedPointCollectorInAABBQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput) { m_queryDynamicOutput = queryDynamicOutput; }

KY_INLINE void BaseSpatializedPointCollectorInAABBQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE SpatializedPointCollectorInAABBQueryResult BaseSpatializedPointCollectorInAABBQuery::GetResult() const { return m_result; }

KY_INLINE const Vec3f&           BaseSpatializedPointCollectorInAABBQuery::GetStartPos()           const { return m_startPos3f;         }
KY_INLINE const Box3f&           BaseSpatializedPointCollectorInAABBQuery::GetExtentBox()          const { return m_extentBox;          }
KY_INLINE const NavTrianglePtr&  BaseSpatializedPointCollectorInAABBQuery::GetStartTrianglePtr()   const { return m_startTrianglePtr;   }
KY_INLINE QueryDynamicOutput*    BaseSpatializedPointCollectorInAABBQuery::GetQueryDynamicOutput() const { return m_queryDynamicOutput; }
KY_INLINE const WorldIntegerPos& BaseSpatializedPointCollectorInAABBQuery::GetStartIntegerPos()    const { return m_startIntegerPos;    }

KY_INLINE const PositionSpatializationRange& BaseSpatializedPointCollectorInAABBQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

KY_INLINE Box3f BaseSpatializedPointCollectorInAABBQuery::ComputeAABB() const { return Box3f(m_startPos3f - m_extentBox.m_min, m_startPos3f + m_extentBox.m_max); }

}
