/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{

// Primary contact: JUBA - secondary contact: NOBODY

KY_INLINE void BaseSegmentCanGoQuery::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)   { m_startTrianglePtr   = startTrianglePtr;   }
KY_INLINE void BaseSegmentCanGoQuery::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)    { m_startIntegerPos    = startIntegerPos;    }
KY_INLINE void BaseSegmentCanGoQuery::SetDestIntegerPos(const WorldIntegerPos& destIntegerPos)      { m_destIntegerPos     = destIntegerPos;     }
KY_INLINE void BaseSegmentCanGoQuery::SetResult(SegmentCanGoQueryResult result)                     { m_result             = result;             }
KY_INLINE void BaseSegmentCanGoQuery::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode)     { m_dynamicOutputMode  = dynamicOutputMode;  }
KY_INLINE void BaseSegmentCanGoQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput) { m_queryDynamicOutput = queryDynamicOutput; }
KY_INLINE void BaseSegmentCanGoQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE SegmentCanGoQueryResult BaseSegmentCanGoQuery::GetResult()             const { return m_result;             }
KY_INLINE DynamicOutputMode       BaseSegmentCanGoQuery::GetDynamicOutputMode()  const { return m_dynamicOutputMode;  }
KY_INLINE const Vec3f&            BaseSegmentCanGoQuery::GetStartPos()           const { return m_startPos3f;         }
KY_INLINE const Vec3f&            BaseSegmentCanGoQuery::GetDestPos()            const { return m_destPos3f;          }
KY_INLINE const NavTrianglePtr&   BaseSegmentCanGoQuery::GetStartTrianglePtr()   const { return m_startTrianglePtr;   }
KY_INLINE const NavTrianglePtr&   BaseSegmentCanGoQuery::GetDestTrianglePtr()    const { return m_destTrianglePtr;    }
KY_INLINE KyFloat32               BaseSegmentCanGoQuery::GetRadius()             const { return m_radius;             }
KY_INLINE QueryDynamicOutput*     BaseSegmentCanGoQuery::GetQueryDynamicOutput() const { return m_queryDynamicOutput; }
KY_INLINE const WorldIntegerPos&  BaseSegmentCanGoQuery::GetStartIntegerPos()    const { return m_startIntegerPos;    }
KY_INLINE const WorldIntegerPos&  BaseSegmentCanGoQuery::GetDestIntegerPos()     const { return m_destIntegerPos;     }

KY_INLINE const PositionSpatializationRange& BaseSegmentCanGoQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}

