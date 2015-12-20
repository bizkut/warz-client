/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_IBlobTypeDescriptor_H
#define Navigation_IBlobTypeDescriptor_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/endianness.h"


namespace Kaim
{

class BaseBlobHandler;

class IBlobTypeDescriptor : public RefCountBaseNTS<IBlobTypeDescriptor, Stat_Default_Mem>
{
	KY_CLASS_WITHOUT_COPY(IBlobTypeDescriptor)

public:
	IBlobTypeDescriptor() {}
	virtual ~IBlobTypeDescriptor() {}
	virtual KyUInt32    GetBlobTypeId()      const = 0;
	virtual const char* GetBlobTypeName()    const = 0;
	virtual KyUInt32    GetBlobTypeVersion() const = 0;
	virtual void DoSwapEndianness(Endianness::Target e, void* blob) const = 0;
	virtual IBlobTypeDescriptor* CreateBlobTypeDescriptor(int memStat = Stat_Default_Mem) const = 0;
	virtual BaseBlobHandler* CreateBlobHandler(int memStat = Stat_Default_Mem) const = 0;
};


template <class T>
bool BlobIsa(const IBlobTypeDescriptor& descriptor)
{
	return descriptor.GetBlobTypeId() == T::GetBlobTypeId();
}


}


#endif

