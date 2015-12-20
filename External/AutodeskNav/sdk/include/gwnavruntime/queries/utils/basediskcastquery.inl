/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE void BaseDiskCastQuery::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)   { m_startTrianglePtr   = startTrianglePtr;   }
KY_INLINE void BaseDiskCastQuery::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)    { m_startIntegerPos    = startIntegerPos;    }
KY_INLINE void BaseDiskCastQuery::SetSafetyDist(KyFloat32 safetyDist)                           { m_safetyDist         = safetyDist;         }
KY_INLINE void BaseDiskCastQuery::SetQueryType(DiskCastQueryType queryType)                     { m_queryType          = queryType;          }
KY_INLINE void BaseDiskCastQuery::SetResult(DiskCastQueryResult result)                         { m_result             = result;             }
KY_INLINE void BaseDiskCastQuery::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode)     { m_dynamicOutputMode  = dynamicOutputMode;  }
KY_INLINE void BaseDiskCastQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput) { m_queryDynamicOutput = queryDynamicOutput; }
KY_INLINE void BaseDiskCastQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE DiskCastQueryResult    BaseDiskCastQuery::GetResult()                  const { return m_result;               }
KY_INLINE DynamicOutputMode      BaseDiskCastQuery::GetDynamicOutputMode()       const { return m_dynamicOutputMode;    }
KY_INLINE const Vec3f&           BaseDiskCastQuery::GetStartPos()                const { return m_startPos3f;           }
KY_INLINE const Vec3f&           BaseDiskCastQuery::GetCollisionPos()            const { return m_collisionPos3f;       }
KY_INLINE const Vec3f&           BaseDiskCastQuery::GetArrivalPos()              const { return m_arrivalPos3f;         }
KY_INLINE const Vec2f&           BaseDiskCastQuery::GetNormalizedDir2d()         const { return m_normalizedDir2d;      }
KY_INLINE const NavTrianglePtr&  BaseDiskCastQuery::GetStartTrianglePtr()        const { return m_startTrianglePtr;     }
KY_INLINE const NavTrianglePtr&  BaseDiskCastQuery::GetArrivalTrianglePtr()      const { return m_arrivalTrianglePtr;   }
KY_INLINE const NavHalfEdgePtr&  BaseDiskCastQuery::GetCollisionNavHalfEdgePtr() const { return m_collisionHalfEdgePtr; }
KY_INLINE KyFloat32              BaseDiskCastQuery::GetMaxDist()                 const { return m_maxDist;              }
KY_INLINE KyFloat32              BaseDiskCastQuery::GetSafetyDist()              const { return m_safetyDist;           }
KY_INLINE KyFloat32              BaseDiskCastQuery::GetRadius()                  const { return m_radius;               }
KY_INLINE DiskCastQueryType      BaseDiskCastQuery::GetQueryType()               const { return m_queryType;            }
KY_INLINE QueryDynamicOutput*    BaseDiskCastQuery::GetQueryDynamicOutput()      const { return m_queryDynamicOutput;   }
KY_INLINE const WorldIntegerPos& BaseDiskCastQuery::GetStartIntegerPos()         const { return m_startIntegerPos;      }
KY_INLINE const WorldIntegerPos& BaseDiskCastQuery::GetArrivalIntegerPos()       const { return m_arrivalIntegerPos;    }

KY_INLINE const PositionSpatializationRange& BaseDiskCastQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
