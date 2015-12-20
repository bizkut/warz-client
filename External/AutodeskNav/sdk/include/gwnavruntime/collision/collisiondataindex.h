/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JAPA - secondary contact: GUAL
#ifndef Navigation_ColDataIndex_H
#define Navigation_ColDataIndex_H

#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/base/kyguid.h"
#include "gwnavruntime/math/box2f.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/collision/collisiondatablobcategory.h"

namespace Kaim
{

class FileOpenerBase;

class ColDataDescriptor
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_CollisionData)
	KY_CLASS_WITHOUT_COPY(ColDataDescriptor)
public:
	ColDataDescriptor() {}
	const char* GetFileName() const { return m_fileName.GetValues(); }

	BlobArray<char> m_fileName;
	KyGuid m_guid;
	Box2f m_boundingBox;
};
inline void SwapEndianness(Endianness::Target e, ColDataDescriptor& self)
{
	SwapEndianness(e, self.m_fileName);
	SwapEndianness(e, self.m_guid);
	SwapEndianness(e, self.m_boundingBox);
}


class ColDataIndex
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_CollisionData)
	KY_ROOT_BLOB_CLASS(CollisionData, ColDataIndex, 0)
	KY_CLASS_WITHOUT_COPY(ColDataIndex)
public:
	ColDataIndex() {}
	KyUInt32 GetDescriptorsCount()                         const { return m_descriptors.GetCount(); }
	const ColDataDescriptor& GetDescriptor(KyUInt32 index) const { return m_descriptors.GetValues()[index]; }
	BlobArray<ColDataDescriptor> m_descriptors;
};

inline void SwapEndianness(Endianness::Target e, ColDataIndex& self)
{
	SwapEndianness(e, self.m_descriptors);
}

class ColDataIndexLoader
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_CollisionData)

public:
	static Ptr< BlobHandler<ColDataIndex> > LoadColDataIndex(const char* filename, FileOpenerBase* fileOpener = KY_NULL);
};


} // namespace Kaim

#endif
