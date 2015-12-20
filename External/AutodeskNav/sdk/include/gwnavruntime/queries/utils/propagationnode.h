/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_PropagationNode_H
#define Navigation_PropagationNode_H

#include "gwnavruntime/navmesh/identifiers/navtrianglerawptr.h"

namespace Kaim
{

class PropagationNode
{
public:
	PropagationNode() : m_triangleRawPtr(), m_cost(0.f) {}
	PropagationNode(const NavTriangleRawPtr& id) : m_triangleRawPtr(id), m_cost(0.f) {}

	KY_INLINE bool operator< (const PropagationNode& node) const { return m_cost < node.m_cost; }

public:
	NavTriangleRawPtr m_triangleRawPtr;

	KyFloat32 m_cost;
};

}


#endif //Navigation_PropagationNode_H

