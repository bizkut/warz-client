/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{

KY_INLINE void BaseRayCastQuery::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)   { m_startTrianglePtr   = startTrianglePtr;   }
KY_INLINE void BaseRayCastQuery::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)    { m_startIntegerPos    = startIntegerPos;    }
KY_INLINE void BaseRayCastQuery::SetResult(RayCastQueryResult result)                          { m_result             = result;             }
KY_INLINE void BaseRayCastQuery::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode)     { m_dynamicOutputMode  = dynamicOutputMode;  }
KY_INLINE void BaseRayCastQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput) { m_queryDynamicOutput = queryDynamicOutput; }
KY_INLINE void BaseRayCastQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}
 
KY_INLINE RayCastQueryResult     BaseRayCastQuery::GetResult()                  const { return m_result;               }
KY_INLINE DynamicOutputMode      BaseRayCastQuery::GetDynamicOutputMode()       const { return m_dynamicOutputMode;    }
KY_INLINE const Vec3f&           BaseRayCastQuery::GetStartPos()                const { return m_startPos3f;           }
KY_INLINE const Vec3f&           BaseRayCastQuery::GetCollisionPos()            const { return m_collisionPos3f;       }
KY_INLINE const Vec3f&           BaseRayCastQuery::GetArrivalPos()              const { return m_arrivalPos3f;         }
KY_INLINE const Vec2f&           BaseRayCastQuery::GetMaxMove2D()               const { return m_maxMove2D;            }
KY_INLINE const NavTrianglePtr&  BaseRayCastQuery::GetStartTrianglePtr()        const { return m_startTrianglePtr;     }
KY_INLINE const NavTrianglePtr&  BaseRayCastQuery::GetArrivalTrianglePtr()      const { return m_arrivalTrianglePtr;   }
KY_INLINE const NavHalfEdgePtr&  BaseRayCastQuery::GetCollisionNavHalfEdgePtr() const { return m_collisionHalfEdgePtr; }
KY_INLINE QueryDynamicOutput*    BaseRayCastQuery::GetQueryDynamicOutput()      const { return m_queryDynamicOutput;   }
KY_INLINE const WorldIntegerPos& BaseRayCastQuery::GetStartIntegerPos()         const { return m_startIntegerPos;      }
KY_INLINE const WorldIntegerPos& BaseRayCastQuery::GetArrivalIntegerPos()       const { return m_arrivalIntegerPos;    }

KY_INLINE const PositionSpatializationRange& BaseRayCastQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
