/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{

KY_INLINE void BaseDiskExpansionQuery::SetCenterTrianglePtr(const NavTrianglePtr& centerTrianglePtr) { m_centerTrianglePtr = centerTrianglePtr;   }
KY_INLINE void BaseDiskExpansionQuery::SetCenterIntegerPos(const WorldIntegerPos& centerIntegerPos)  { m_centerIntegerPos  = centerIntegerPos;    }
KY_INLINE void BaseDiskExpansionQuery::SetSafetyDist(KyFloat32 safetyDist)                           { m_safetyDist        = safetyDist;          }
KY_INLINE void BaseDiskExpansionQuery::SetResult(DiskExpansionQueryResult result)                    { m_result            = result;              }
KY_INLINE void BaseDiskExpansionQuery::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode)     { m_dynamicOutputMode = dynamicOutputMode;   }
KY_INLINE void BaseDiskExpansionQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput) { m_queryDynamicOutput = queryDynamicOutput; }
KY_INLINE void BaseDiskExpansionQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE DiskExpansionQueryResult BaseDiskExpansionQuery::GetResult()                  const { return m_result;               }
KY_INLINE DynamicOutputMode        BaseDiskExpansionQuery::GetDynamicOutputMode()       const { return m_dynamicOutputMode;    }
KY_INLINE const Vec3f&             BaseDiskExpansionQuery::GetCenterPos()               const { return m_centerPos3f;          }
KY_INLINE const NavTrianglePtr&    BaseDiskExpansionQuery::GetCenterTrianglePtr()       const { return m_centerTrianglePtr;    }
KY_INLINE KyFloat32                BaseDiskExpansionQuery::GetRadiusMax()               const { return m_radiusMax;            }
KY_INLINE KyFloat32                BaseDiskExpansionQuery::GetResultRadius()            const { return m_resultRadius;         }
KY_INLINE const Vec3f&             BaseDiskExpansionQuery::GetCollisionPos()            const { return m_collisionPos3f;       }
KY_INLINE const NavHalfEdgePtr&    BaseDiskExpansionQuery::GetCollisionNavHalfEdgePtr() const { return m_collisionHalfEdgePtr; }
KY_INLINE KyFloat32                BaseDiskExpansionQuery::GetSafetyDist()              const { return m_safetyDist;           }
KY_INLINE QueryDynamicOutput*      BaseDiskExpansionQuery::GetQueryDynamicOutput()      const { return m_queryDynamicOutput;   }
KY_INLINE const WorldIntegerPos&   BaseDiskExpansionQuery::GetCenterIntegerPos()        const { return m_centerIntegerPos;     }

KY_INLINE const PositionSpatializationRange& BaseDiskExpansionQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
