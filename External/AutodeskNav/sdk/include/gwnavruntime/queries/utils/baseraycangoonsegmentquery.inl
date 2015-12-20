/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{

KY_INLINE void BaseRayCanGoOnSegmentQuery::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)               { m_startIntegerPos        = startIntegerPos;        }
KY_INLINE void BaseRayCanGoOnSegmentQuery::SetSegmentStartIntegerPos(const WorldIntegerPos& segmentStartIntegerPos) { m_segmentStartIntegerPos = segmentStartIntegerPos; }
KY_INLINE void BaseRayCanGoOnSegmentQuery::SetSegmentEndIntegerPos(const WorldIntegerPos& segmentEndIntegerPos)     { m_segmentEndIntegerPos   = segmentEndIntegerPos;   }
KY_INLINE void BaseRayCanGoOnSegmentQuery::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode)                { m_dynamicOutputMode      = dynamicOutputMode;      }
KY_INLINE void BaseRayCanGoOnSegmentQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput)            { m_queryDynamicOutput     = queryDynamicOutput;     }
KY_INLINE void BaseRayCanGoOnSegmentQuery::SetResult(RayCanGoOnSegmentQueryResult result)                           { m_result                 = result;                 }
KY_INLINE void BaseRayCanGoOnSegmentQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange) 
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE RayCanGoOnSegmentQueryResult BaseRayCanGoOnSegmentQuery::GetResult ()                const { return m_result;                 }
KY_INLINE DynamicOutputMode            BaseRayCanGoOnSegmentQuery::GetDynamicOutputMode()      const { return m_dynamicOutputMode;      }
KY_INLINE QueryDynamicOutput*          BaseRayCanGoOnSegmentQuery::GetQueryDynamicOutput()     const { return m_queryDynamicOutput;     }
KY_INLINE const Vec3f&                 BaseRayCanGoOnSegmentQuery::GetStartPos()               const { return m_startPos3f;             }
KY_INLINE const Vec3f&                 BaseRayCanGoOnSegmentQuery::GetSegmentStartPos()        const { return m_segmentStartPos3f;      }
KY_INLINE const Vec3f&                 BaseRayCanGoOnSegmentQuery::GetSegmentEndPos()          const { return m_segmentEndPos3f;        }
KY_INLINE const NavTrianglePtr&        BaseRayCanGoOnSegmentQuery::GetStartTrianglePtr()       const { return m_startTrianglePtr;       }
KY_INLINE const NavTrianglePtr&        BaseRayCanGoOnSegmentQuery::GetArrivalTrianglePtr()     const { return m_destTrianglePtr;        }
KY_INLINE const WorldIntegerPos&       BaseRayCanGoOnSegmentQuery::GetStartIntegerPos()        const { return m_startIntegerPos;        }
KY_INLINE const WorldIntegerPos&       BaseRayCanGoOnSegmentQuery::GetSegmentStartIntegerPos() const { return m_segmentStartIntegerPos; }
KY_INLINE const WorldIntegerPos&       BaseRayCanGoOnSegmentQuery::GetSegmentEndIntegerPos()   const { return m_segmentEndIntegerPos;   }

KY_INLINE const PositionSpatializationRange& BaseRayCanGoOnSegmentQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
