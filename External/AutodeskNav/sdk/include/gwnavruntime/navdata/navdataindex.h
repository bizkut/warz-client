/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: GUAL
#ifndef Navigation_NavDataIndex_H
#define Navigation_NavDataIndex_H

#include "gwnavruntime/navdata/navdatablobcategory.h"
#include "gwnavruntime/base/kyguid.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/math/box2f.h"
#include "gwnavruntime/base/fileopener.h"
#include "gwnavruntime/blob/blobaggregate.h"
#include "gwnavruntime/basesystem/logger.h"

namespace Kaim
{

class NavDataDescriptor
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	KY_CLASS_WITHOUT_COPY(NavDataDescriptor)
public:
	NavDataDescriptor() {}
	const char* GetFileName() const { return m_fileName.GetValues(); }

	BlobArray<char> m_fileName;
	KyGuid m_guid;
	Box2f m_boundingBox;
};
inline void SwapEndianness(Endianness::Target e, NavDataDescriptor& self)
{
	SwapEndianness(e, self.m_fileName);
	SwapEndianness(e, self.m_guid);
	SwapEndianness(e, self.m_boundingBox);
}


class NavDataIndex
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	KY_ROOT_BLOB_CLASS(NavData, NavDataIndex, 0)
	KY_CLASS_WITHOUT_COPY(NavDataIndex)
public:
	NavDataIndex() {}
	KyUInt32 GetDescriptorsCount()                         const { return m_descriptors.GetCount(); }
	const NavDataDescriptor& GetDescriptor(KyUInt32 index) const { return m_descriptors.GetValues()[index]; }
	BlobArray<NavDataDescriptor> m_descriptors;
};

inline void SwapEndianness(Endianness::Target e, NavDataIndex& self)
{
	SwapEndianness(e, self.m_descriptors);
}

class NavDataIndexLoader
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	static Ptr< BlobHandler<NavDataIndex> > LoadNavDataIndex(const char* filename, FileOpenerBase* fileOpener = KY_NULL)
	{
		DefaultFileOpener defaultFileOpener;
		FileOpenerBase* opener = fileOpener ? fileOpener : &defaultFileOpener;
		Ptr<File> file = opener->OpenFile(filename, OpenMode_Read);
		if (file == KY_NULL)
		{
			KY_LOG_ERROR(("Could not open file [%s]", filename));
			return KY_NULL;
		}

		BlobAggregate blobAggregate;

		if (blobAggregate.Load(file, MemStat_Blob, KY_NULL, BLOB_AGGREGATE_IGNORE_UNKNOWN_BLOBTYPES) == KY_ERROR)
			return KY_NULL;

		BlobAggregate::Collection<NavDataIndex> collection = blobAggregate.GetCollection<NavDataIndex>();
		
		if (collection.GetCount() == 0)
		{
			KY_LOG_ERROR(("File [%s] does not contain any NavDataIndex", filename));
			return KY_NULL;
		}

		if (collection.GetCount() > 1)
		{
			KY_LOG_ERROR(("ERROR: File [%s] does contains more than 1 NavDataIndex", filename));
			return KY_NULL;
		}

		return collection.GetHandler(0);
	}
};


} // namespace Kaim

#endif
