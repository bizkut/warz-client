/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_LoadedAbstractGraphCellIdx_H
#define Navigation_LoadedAbstractGraphCellIdx_H

#include "gwnavruntime/abstractgraph/abstractgraphtypes.h"

namespace Kaim
{

class LoadedAbstractGraphCellIdx
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	LoadedAbstractGraphCellIdx() { Invalidate(); }

	bool HasAbstractGraphNodes() const { return m_abstractGraphInGridIdx < AbstractGraphIdx_Invalid && m_cellInBlobIdx < AbstractGraphCellIdx_Invalid; }
	bool IsInAbstractGraph() const { return m_abstractGraphInGridIdx < AbstractGraphIdx_Invalid; }
	void Invalidate()
	{
		m_abstractGraphInGridIdx = AbstractGraphIdx_Invalid;
		m_cellInBlobIdx = AbstractGraphCellIdx_Invalid;
	}

	AbstractGraphIdx m_abstractGraphInGridIdx;
	AbstractGraphCellIdx m_cellInBlobIdx;
};

}

#endif
