/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphNodePtr_H
#define Navigation_AbstractGraphNodePtr_H


#include "gwnavruntime/abstractgraphtypes.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim
{

class AbstractGraph;

class AbstractGraphNodePtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	AbstractGraphNodePtr() { Invalidate(); }
	AbstractGraphNodePtr(AbstractGraph* abstractGraph, AbstractGraphNodeIdx nodeIdx) { Set(abstractGraph, nodeIdx); }

	bool IsValid() const { return m_abstractGraph != KY_NULL && m_nodeIdx < AbstractGraphNodeIdx_Invalid; }
	void Invalidate()
	{
		m_abstractGraph = KY_NULL;
		m_nodeIdx = AbstractGraphNodeIdx_Invalid;
	}

	void Set(AbstractGraph* abstractGraph, AbstractGraphNodeIdx nodeIdx)
	{
		m_abstractGraph = abstractGraph;
		m_nodeIdx = nodeIdx;
	}

public:
	Ptr<AbstractGraph> m_abstractGraph;
	AbstractGraphNodeIdx m_nodeIdx;
};

}

#endif
