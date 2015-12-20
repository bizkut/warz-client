/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_LoadedAbstractGraphNodeIdx_H
#define Navigation_LoadedAbstractGraphNodeIdx_H

#include "gwnavruntime/abstractgraph/abstractgraphtypes.h"

namespace Kaim
{


class LoadedAbstractGraphNodeIdx
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	LoadedAbstractGraphNodeIdx() { Invalidate(); }
	LoadedAbstractGraphNodeIdx(AbstractGraphIdx abstractGraphInGridIdx, AbstractGraphNodeIdx nodeIdx) { Set(abstractGraphInGridIdx, nodeIdx); }

	bool IsValid() const { return m_abstractGraphInGridIdx < AbstractGraphIdx_Invalid && m_nodeIdx < AbstractGraphNodeIdx_Invalid; }
	void Invalidate()
	{
		m_abstractGraphInGridIdx = AbstractGraphIdx_Invalid;
		m_nodeIdx = AbstractGraphNodeIdx_Invalid;
	}

	void Set(AbstractGraphIdx abstractGraphInGridIdx, AbstractGraphNodeIdx nodeIdx)
	{
		m_abstractGraphInGridIdx = abstractGraphInGridIdx;
		m_nodeIdx = nodeIdx;
	}

public:
	AbstractGraphIdx m_abstractGraphInGridIdx;
	AbstractGraphNodeIdx m_nodeIdx;
};

}

#endif
