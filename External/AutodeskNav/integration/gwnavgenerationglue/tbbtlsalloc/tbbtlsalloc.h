/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



#ifndef KyGlue_TbbTlsAlloc_H
#define KyGlue_TbbTlsAlloc_H


#include "gwnavgeneration/common/generatormemory.h"


namespace KyGlue
{


/// This implementation of Kaim::ITlsAlloc uses the scalable memory allocation functions provided with the open-source
/// Threaded Building Blocks (TBB) libraries from Intel. It is highly recommended that you use this implementation any
/// time you use TBB to parallelize your NavData generation across multiple processors.
class TbbTlsAlloc : public Kaim::ITlsAlloc
{
public:
	virtual ~TbbTlsAlloc() {}
	virtual void* Alloc(Kaim::UPInt size);
	virtual void  Free(void* ptr);
	virtual void* Realloc(void* oldPtr, Kaim::UPInt newSize);
};


}

#endif

