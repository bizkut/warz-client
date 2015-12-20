/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_EndOfSynchronizationBlob_H
#define Navigation_EndOfSynchronizationBlob_H


#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"

namespace Kaim
{

// This blob is sent by the Lab when the syncing step is completed.
class EndOfSynchronizationBlob
{
public:
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, EndOfSynchronizationBlob, 0)
public:
};

inline void SwapEndianness(Endianness::Target /*e*/, EndOfSynchronizationBlob& /*self*/)
{
}

}

#endif // Navigation_NewFrameBlob_H
