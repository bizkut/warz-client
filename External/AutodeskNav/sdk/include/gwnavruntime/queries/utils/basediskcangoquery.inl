/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE void BaseDiskCanGoQuery::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)   { m_startTrianglePtr   = startTrianglePtr;   }
KY_INLINE void BaseDiskCanGoQuery::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)    { m_startIntegerPos    = startIntegerPos;    }
KY_INLINE void BaseDiskCanGoQuery::SetDestIntegerPos(const WorldIntegerPos& destIntegerPos)      { m_destIntegerPos     = destIntegerPos;     }
KY_INLINE void BaseDiskCanGoQuery::SetQueryType(DiskCanGoQueryType queryType)                    { m_queryType          = queryType;          }
KY_INLINE void BaseDiskCanGoQuery::SetResult(DiskCanGoQueryResult result)                        { m_result             = result;             }
KY_INLINE void BaseDiskCanGoQuery::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode)     { m_dynamicOutputMode  = dynamicOutputMode;  }
KY_INLINE void BaseDiskCanGoQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput) { m_queryDynamicOutput = queryDynamicOutput; }
KY_INLINE void BaseDiskCanGoQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE DiskCanGoQueryResult   BaseDiskCanGoQuery::GetResult()             const { return m_result;             }
KY_INLINE DynamicOutputMode      BaseDiskCanGoQuery::GetDynamicOutputMode()  const { return m_dynamicOutputMode;  }
KY_INLINE const Vec3f&           BaseDiskCanGoQuery::GetStartPos()           const { return m_startPos3f;         }
KY_INLINE const Vec3f&           BaseDiskCanGoQuery::GetDestPos()            const { return m_destPos3f;          }
KY_INLINE const NavTrianglePtr&  BaseDiskCanGoQuery::GetStartTrianglePtr()   const { return m_startTrianglePtr;   }
KY_INLINE const NavTrianglePtr&  BaseDiskCanGoQuery::GetDestTrianglePtr()    const { return m_destTrianglePtr;    }
KY_INLINE KyFloat32              BaseDiskCanGoQuery::GetRadius()             const { return m_radius;             }
KY_INLINE DiskCanGoQueryType     BaseDiskCanGoQuery::GetQueryType()          const { return m_queryType;          }
KY_INLINE QueryDynamicOutput*    BaseDiskCanGoQuery::GetQueryDynamicOutput() const { return m_queryDynamicOutput; }
KY_INLINE const WorldIntegerPos& BaseDiskCanGoQuery::GetStartIntegerPos()    const { return m_startIntegerPos;    }
KY_INLINE const WorldIntegerPos& BaseDiskCanGoQuery::GetDestIntegerPos()     const { return m_destIntegerPos;     }

KY_INLINE const PositionSpatializationRange& BaseDiskCanGoQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
