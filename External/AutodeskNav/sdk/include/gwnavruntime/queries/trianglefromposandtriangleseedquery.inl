/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{
KY_INLINE void TriangleFromPosAndTriangleSeedQuery::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

KY_INLINE void TriangleFromPosAndTriangleSeedQuery::SetInputIntegerPos(const WorldIntegerPos& integerPos)  { m_inputIntegerPos = integerPos; }
KY_INLINE void TriangleFromPosAndTriangleSeedQuery::SetResult(TriangleFromPosAndTriangleSeedResult result) { m_result          = result;     }

KY_INLINE TriangleFromPosAndTriangleSeedResult TriangleFromPosAndTriangleSeedQuery::GetResult() const { return m_result; }

KY_INLINE const WorldIntegerPos& TriangleFromPosAndTriangleSeedQuery::GetInputIntegerPos()   const { return m_inputIntegerPos;   }
KY_INLINE const Vec3f&           TriangleFromPosAndTriangleSeedQuery::GetInputPos()          const { return m_inputPos3f;        }
KY_INLINE const NavTrianglePtr&  TriangleFromPosAndTriangleSeedQuery::GetSeedTrianglePtr()   const { return m_seedTrianglePtr;   }
KY_INLINE const NavTrianglePtr&  TriangleFromPosAndTriangleSeedQuery::GetResultTrianglePtr() const { return m_resultTrianglePtr; }

}
