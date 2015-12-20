/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: NOBODY
#ifndef Navigation_DynamicNavMeshQueryDisplayListBuilder_H
#define Navigation_DynamicNavMeshQueryDisplayListBuilder_H

#include "gwnavruntime/visualsystem/idisplaylistbuilder.h"
#include "gwnavruntime/visualsystem/visualcolor.h"


namespace Kaim
{

enum DynamicNavMeshQueryDisplayListBuilderEnum
{
	DynamicNavMeshQueryDisplay_Wireframe,
	DynamicNavMeshQueryDisplay_Transparency,
	DynamicNavMeshQueryDisplay_Offset
};

class DynamicNavMeshQueryDisplayListBuilder: public IDisplayListBuilder
{
public:
	DynamicNavMeshQueryDisplayListBuilder()
	{}

	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 deepBlobSize = 0);
	virtual void Init();
};

} // namespace Kaim

#endif // Navigation_DynamicNavMeshQueryDisplayListBuilder_H
