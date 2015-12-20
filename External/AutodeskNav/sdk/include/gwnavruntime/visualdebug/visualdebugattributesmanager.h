/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY
#ifndef GwNavRuntime_VisualDebugAttributesManager_H
#define GwNavRuntime_VisualDebugAttributesManager_H

#include "gwnavruntime/visualdebug/visualdebugattributes.h"

namespace Kaim
{

class VisualDebugServer;

class VisualDebugAttributesManager
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)

public:
	VisualDebugAttributesManager() {}
	~VisualDebugAttributesManager() { Clear(); }

	void Clear();

	VisualDebugAttributeGroup* GetAttributeGroup(const char* groupName);
	VisualDebugAttributeGroup* CreateAttributeGroup(const char* groupName);
	VisualDebugAttributeGroup* GetOrCreateAttributeGroup(const char* groupName);

	void RemoveAttributeGroup(const char* groupName, VisualDebugServer* server);

	void OnConnection(VisualDebugServer* server);
	void SendVisualDebug(VisualDebugServer* server);

public:
	KyArray<VisualDebugAttributeGroup*> m_attributeGroups;
};

}

#endif
