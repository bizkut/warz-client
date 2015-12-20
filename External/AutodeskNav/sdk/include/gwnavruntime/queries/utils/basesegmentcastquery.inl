/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{

KY_INLINE void BaseSegmentCastQuery::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)   { m_startTrianglePtr   = startTrianglePtr;   }
KY_INLINE void BaseSegmentCastQuery::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)    { m_startIntegerPos    = startIntegerPos;    }
KY_INLINE void BaseSegmentCastQuery::SetSafetyDist(KyFloat32 safetyDist)                           { m_safetyDist         = safetyDist;         }
KY_INLINE void BaseSegmentCastQuery::SetResult(SegmentCastQueryResult result)                      { m_result             = result;             }
KY_INLINE void BaseSegmentCastQuery::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode)     { m_dynamicOutputMode  = dynamicOutputMode;  }
KY_INLINE void BaseSegmentCastQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput) { m_queryDynamicOutput = queryDynamicOutput; }
KY_INLINE void BaseSegmentCastQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE SegmentCastQueryResult BaseSegmentCastQuery::GetResult()                  const { return m_result;               }
KY_INLINE DynamicOutputMode      BaseSegmentCastQuery::GetDynamicOutputMode()       const { return m_dynamicOutputMode;    }
KY_INLINE const Vec3f&           BaseSegmentCastQuery::GetStartPos()                const { return m_startPos3f;           }
KY_INLINE const Vec3f&           BaseSegmentCastQuery::GetCollisionPos()            const { return m_collisionPos3f;       }
KY_INLINE const Vec3f&           BaseSegmentCastQuery::GetArrivalPos()              const { return m_arrivalPos3f;         }
KY_INLINE const Vec2f&           BaseSegmentCastQuery::GetNormalizedDir2d()         const { return m_normalizedDir2d;      }
KY_INLINE const NavTrianglePtr&  BaseSegmentCastQuery::GetStartTrianglePtr()        const { return m_startTrianglePtr;     }
KY_INLINE const NavTrianglePtr&  BaseSegmentCastQuery::GetArrivalTrianglePtr()      const { return m_arrivalTrianglePtr;   }
KY_INLINE const NavHalfEdgePtr&  BaseSegmentCastQuery::GetCollisionNavHalfEdgePtr() const { return m_collisionHalfEdgePtr; }
KY_INLINE KyFloat32              BaseSegmentCastQuery::GetMaxDist()                 const { return m_maxDist;              }
KY_INLINE KyFloat32              BaseSegmentCastQuery::GetSafetyDist()              const { return m_safetyDist;           }
KY_INLINE KyFloat32              BaseSegmentCastQuery::GetRadius()                  const { return m_radius;               }
KY_INLINE QueryDynamicOutput*    BaseSegmentCastQuery::GetQueryDynamicOutput()      const { return m_queryDynamicOutput;   }
KY_INLINE const WorldIntegerPos& BaseSegmentCastQuery::GetStartIntegerPos()         const { return m_startIntegerPos;      }
KY_INLINE const WorldIntegerPos& BaseSegmentCastQuery::GetArrivalIntegerPos()       const { return m_arrivalIntegerPos;    }

KY_INLINE const PositionSpatializationRange& BaseSegmentCastQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
