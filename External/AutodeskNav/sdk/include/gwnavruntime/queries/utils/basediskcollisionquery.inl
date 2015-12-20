/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{

KY_INLINE void BaseDiskCollisionQuery::SetCenterTrianglePtr(const NavTrianglePtr& centerTrianglePtr) { m_centerTrianglePtr = centerTrianglePtr;   }
KY_INLINE void BaseDiskCollisionQuery::SetCenterIntegerPos(const WorldIntegerPos& centerIntegerPos)  { m_centerIntegerPos  = centerIntegerPos;    }
KY_INLINE void BaseDiskCollisionQuery::SetResult(DiskCollisionQueryResult result)                    { m_result            = result;              }
KY_INLINE void BaseDiskCollisionQuery::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode)     { m_dynamicOutputMode = dynamicOutputMode;   }
KY_INLINE void BaseDiskCollisionQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput) { m_queryDynamicOutput = queryDynamicOutput; }
KY_INLINE void BaseDiskCollisionQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE DiskCollisionQueryResult BaseDiskCollisionQuery::GetResult()             const { return m_result;             }
KY_INLINE DynamicOutputMode        BaseDiskCollisionQuery::GetDynamicOutputMode()  const { return m_dynamicOutputMode;  }
KY_INLINE const Vec3f&             BaseDiskCollisionQuery::GetCenterPos()          const { return m_centerPos3f;        }
KY_INLINE const NavTrianglePtr&    BaseDiskCollisionQuery::GetCenterTrianglePtr()  const { return m_centerTrianglePtr;  }
KY_INLINE KyFloat32                BaseDiskCollisionQuery::GetRadius()             const { return m_radius;             }
KY_INLINE QueryDynamicOutput*      BaseDiskCollisionQuery::GetQueryDynamicOutput() const { return m_queryDynamicOutput; }
KY_INLINE const WorldIntegerPos&   BaseDiskCollisionQuery::GetCenterIntegerPos()   const { return m_centerIntegerPos;   }

KY_INLINE const PositionSpatializationRange& BaseDiskCollisionQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
