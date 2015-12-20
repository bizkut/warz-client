/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE void TagVolumesFromPosQuery::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

KY_INLINE void TagVolumesFromPosQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}
KY_INLINE void TagVolumesFromPosQuery::SetInputIntegerPos(const WorldIntegerPos& integerPos)         { m_inputIntegerPos    = integerPos;         }
KY_INLINE void TagVolumesFromPosQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput) { m_queryDynamicOutput = queryDynamicOutput; }
KY_INLINE void TagVolumesFromPosQuery::SetResult(TagVolumesFromPosQueryResult result)                { m_result             = result;             }

KY_INLINE TagVolumesFromPosQueryResult TagVolumesFromPosQuery::GetResult()             const { return m_result;             }
KY_INLINE const WorldIntegerPos&       TagVolumesFromPosQuery::GetInputIntegerPos()    const { return m_inputIntegerPos;    }
KY_INLINE const Vec3f&                 TagVolumesFromPosQuery::GetInputPos()           const { return m_inputPos3f;         }
KY_INLINE QueryDynamicOutput*          TagVolumesFromPosQuery::GetQueryDynamicOutput() const { return m_queryDynamicOutput; }

KY_INLINE const PositionSpatializationRange& TagVolumesFromPosQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }

}
