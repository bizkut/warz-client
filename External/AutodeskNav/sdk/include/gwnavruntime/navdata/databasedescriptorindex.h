/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AggregatedDataIndex_H
#define Navigation_AggregatedDataIndex_H

#include "gwnavruntime/navdata/navdataindex.h"
#include "gwnavruntime/abstractgraph/blobs/abstractdataindex.h"
#include "gwnavruntime/collision/collisiondataindex.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/navdata/databasedescriptor.h"

namespace Kaim
{


class DatabaseDescriptorIndex : public RefCountBaseNTS<DatabaseDescriptorIndex, MemStat_Data_Other>
{
	KY_CLASS_WITHOUT_COPY(DatabaseDescriptorIndex)

public:
	enum DataIndexLoadMode
	{
		LoadDataIndex_NavData      = 1 << 0,
		LoadDataIndex_AbstractData = 1 << 1,
		LoadDataIndex_ColData      = 1 << 2,

		LoadDataIndex_All          = KyUInt32MAXVAL,
	};

public:
	DatabaseDescriptorIndex() {}
	
	void Clear()
	{
		m_navDataIndexHandler = KY_NULL;
		m_abstractDataIndexHandler = KY_NULL;
		m_colDataIndexHandler = KY_NULL;
		m_databaseDescriptor.m_databaseIndex = 0;
	}


	KyResult LoadDataIndex(const char* filename, FileOpenerBase* fileOpener = KY_NULL, DataIndexLoadMode loadMode = LoadDataIndex_All)
	{
		DefaultFileOpener defaultFileOpener;
		FileOpenerBase* opener = fileOpener ? fileOpener : &defaultFileOpener;
		Ptr<File> file = opener->OpenFile(filename, OpenMode_Read);
		if (file == KY_NULL)
		{
			KY_LOG_ERROR(("Could not open file [%s]", filename));
			return KY_ERROR;
		}

		BlobAggregate blobAggregate;

		if (blobAggregate.Load(file, MemStat_Blob, KY_NULL, BLOB_AGGREGATE_IGNORE_UNKNOWN_BLOBTYPES) == KY_ERROR)
			return KY_ERROR;

		return LoadDataIndex(blobAggregate, loadMode);
	}


	KyResult LoadDataIndex(const BlobAggregate& blobAggregate, DataIndexLoadMode loadMode = LoadDataIndex_All)
	{
		if (LoadDataIndex_NavData & loadMode)
		{
			m_navDataIndexHandler = KY_NULL;
			BlobAggregate::Collection<NavDataIndex> collection = blobAggregate.GetCollection<NavDataIndex>();
			if (collection.GetCount() == 1)
				m_navDataIndexHandler = collection.GetHandler(0);
			else if (collection.GetCount() > 1)
			{
				KY_LOG_ERROR(("ERROR: BlobAggregate contains more than 1 NavDataIndex. No NavDataIndex has been loaded"));
				Clear();
				return KY_ERROR;
			}			
		}
		
		if (LoadDataIndex_AbstractData & loadMode)
		{
			m_abstractDataIndexHandler = KY_NULL;
			BlobAggregate::Collection<AbstractDataIndex> collection = blobAggregate.GetCollection<AbstractDataIndex>();
			if (collection.GetCount() == 1)
				m_abstractDataIndexHandler = collection.GetHandler(0);
			else if (collection.GetCount() > 1)
			{
				KY_LOG_ERROR(("ERROR: BlobAggregate contains more than 1 AbstractDataIndex. No AbstractDataIndex has been loaded"));
				Clear();
				return KY_ERROR;
			}
		}	

		if (LoadDataIndex_ColData & loadMode)
		{
			m_colDataIndexHandler = KY_NULL;
			BlobAggregate::Collection<ColDataIndex> collection = blobAggregate.GetCollection<ColDataIndex>();
			if (collection.GetCount() == 1)
				m_colDataIndexHandler = collection.GetHandler(0);
			else if (collection.GetCount() > 1)
			{
				KY_LOG_ERROR(("ERROR: BlobAggregate contains more than 1 ColDataIndex. No ColDataIndex has been loaded"));
				Clear();
				return KY_ERROR;
			}
		}

		BlobAggregate::Collection<DatabaseDescriptorBlob> databaseDescriptor = blobAggregate.GetCollection<DatabaseDescriptorBlob>();
		if (databaseDescriptor.GetCount() != 0)
		{
			if (m_databaseDescriptor.ReadFromAggregate(blobAggregate) == KY_ERROR)
			{
				KY_LOG_ERROR(("ERROR: BlobAggregate contains more than 1 ColDataIndex. No ColDataIndex has been loaded"));
				Clear();
				return KY_ERROR;
			}
		}

		return KY_SUCCESS;
	}


	KyResult Save(const char* absoluteFilename, FileOpenerBase* fileOpener = KY_NULL)
	{
		DefaultFileOpener defaultFileOpener;
		FileOpenerBase* opener = fileOpener ? fileOpener : &defaultFileOpener;
		Ptr<File> file = opener->OpenFile(absoluteFilename, OpenMode_Write);
		if (file == KY_NULL)
		{
			KY_LOG_ERROR(("Could not access file [%s]", absoluteFilename));
			return KY_ERROR;
		}

		BlobAggregate blobAggregate;

		// NavDataIndex
		if (m_navDataIndexHandler != KY_NULL && m_navDataIndexHandler->Blob() != KY_NULL)
			blobAggregate.AddBlob(m_navDataIndexHandler);

		// AbstractDataIndex
		if (m_abstractDataIndexHandler != KY_NULL && m_abstractDataIndexHandler->Blob() != KY_NULL)
			blobAggregate.AddBlob(m_abstractDataIndexHandler);

		// ColDataIndex
		if (m_colDataIndexHandler != KY_NULL && m_colDataIndexHandler->Blob() != KY_NULL)
			blobAggregate.AddBlob(m_colDataIndexHandler);

		// Database descriptor
		Ptr<BlobHandler<DatabaseDescriptorBlob> > databaseDescriptorBlobHandler = *KY_NEW BlobHandler<DatabaseDescriptorBlob>;
		DatabaseDescriptorBlobBuilder descriptorBlobBuilder(&m_databaseDescriptor);
		descriptorBlobBuilder.Build(*databaseDescriptorBlobHandler);
		blobAggregate.AddBlob(databaseDescriptorBlobHandler);

		if (Result::Fail(blobAggregate.Save(file)))
		{
			KY_LOG_ERROR(("Could not save DatabaseDescriptorIndex to [%s]", absoluteFilename));
			return KY_ERROR;
		}

		return KY_SUCCESS;
	}

public:
	Ptr< BlobHandler<NavDataIndex> >      m_navDataIndexHandler;
	Ptr< BlobHandler<AbstractDataIndex> > m_abstractDataIndexHandler;
	Ptr< BlobHandler<ColDataIndex> >      m_colDataIndexHandler;

	DatabaseDescriptor m_databaseDescriptor;
};

}

#endif
