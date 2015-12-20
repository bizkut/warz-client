/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_DynamicQueryOutput_DisplayListBuilder_H
#define Navigation_DynamicQueryOutput_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/idisplaylistbuilder.h"

namespace Kaim
{

class ScopedDisplayList;
class QueryDynamicOutputDisplayListBuilder : public IDisplayListBuilder
{
public:
	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 /*deepBlobSize*/ = 0);
};

}

#endif