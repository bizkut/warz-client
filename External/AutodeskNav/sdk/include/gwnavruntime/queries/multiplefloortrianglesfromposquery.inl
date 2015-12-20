/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE MultipleFloorTrianglesFromPosQuery::MultipleFloorTrianglesFromPosQuery() :
	m_inputPos3f(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL),
	m_queryType(MULTIPLEFLOORTRIANGLESFROMPOS_FIND_ALL),
	m_result(MULTIPLEFLOORTRIANGLESFROMPOS_NOT_INITIALIZED),
	m_queryDynamicOutput(KY_NULL) {}

KY_INLINE void MultipleFloorTrianglesFromPosQuery::BindToDatabase(Database* database)
{
	IQuery::BindToDatabase(database);

	// set parameters to their default value
	m_queryType = MULTIPLEFLOORTRIANGLESFROMPOS_FIND_ALL;

	// and clear all the inputs/outputs !
	m_inputPos3f.Set(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL);

	m_inputIntegerPos.Clear();
	m_queryDynamicOutput = KY_NULL;

	SetResult(MULTIPLEFLOORTRIANGLESFROMPOS_NOT_INITIALIZED);
}

KY_INLINE void MultipleFloorTrianglesFromPosQuery::Initialize(const Vec3f& pos)
{
	IQuery::Initialize();

	m_inputPos3f = pos;

	m_inputIntegerPos.Clear();
	m_queryDynamicOutput = KY_NULL;

	SetResult(MULTIPLEFLOORTRIANGLESFROMPOS_NOT_PROCESSED);
}

KY_INLINE void MultipleFloorTrianglesFromPosQuery::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

KY_INLINE void MultipleFloorTrianglesFromPosQuery::SetInputIntegerPos(const WorldIntegerPos& integerPos)              { m_inputIntegerPos    = integerPos;         }
KY_INLINE void MultipleFloorTrianglesFromPosQuery::SetQueryType(MultipleFloorTrianglesFromPosQueryType queryType)     { m_queryType          = queryType;          }
KY_INLINE void MultipleFloorTrianglesFromPosQuery::SetResult(MultipleFloorTrianglesFromPosQueryResult result)         { m_result             = result;             }
KY_INLINE void MultipleFloorTrianglesFromPosQuery::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput)      { m_queryDynamicOutput = queryDynamicOutput; }

KY_INLINE MultipleFloorTrianglesFromPosQueryResult MultipleFloorTrianglesFromPosQuery::GetResult()    const { return m_result;    }
KY_INLINE MultipleFloorTrianglesFromPosQueryType   MultipleFloorTrianglesFromPosQuery::GetQueryType() const { return m_queryType; }

KY_INLINE const WorldIntegerPos& MultipleFloorTrianglesFromPosQuery::GetInputIntegerPos()    const { return m_inputIntegerPos;    }
KY_INLINE const Vec3f&           MultipleFloorTrianglesFromPosQuery::GetInputPos()           const { return m_inputPos3f;         }
KY_INLINE QueryDynamicOutput*    MultipleFloorTrianglesFromPosQuery::GetQueryDynamicOutput() const { return m_queryDynamicOutput; }


}
