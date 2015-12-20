/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobAggregate_H
#define Navigation_BlobAggregate_H

#include "gwnavruntime/visualdebug/messages/aggregateblobcategory.h"
#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/blob/blobcategory.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/blob/blobregistry.h"
#include "gwnavruntime/base/fileopener.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim
{

/* Data at the beginning a BlobAggregate file */
class BlobAggregateFileHeader
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Blob)
public:
	static const char* MagicString() { return "BlobAggFile"; }

public:
	BlobAggregateFileHeader();
	void InitForWrite(Endianness::Type endianness, KyUInt32 blobCount);
	KyResult CheckAndFixEndianness(bool& isEndiannessSwap);

public:
	char m_magicString[12]; // BlobAggFile0
	KyUInt32 m_endianness;
	KyUInt32 m_blobCount;
};


/* Data just before Blob in a BlobAggregate file */
class BlobAggregateBlobHeader
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Blob)
public:
	BlobAggregateBlobHeader();
	void Clear();
	KyResult Init(KyUInt32 blobTypeId, KyUInt32 blobTypeVersion, KyUInt32 deepBlobSize, KyUInt32 shallowBlobSize);
	KyResult Init(const BaseBlobHandler& blobHandler);
	void SwapEndianness();
	template<class T> bool Isa() const { return m_blobTypeId == T::GetBlobTypeId(); }

public:
	KyUInt32 m_blobTypeId;
	KyUInt32 m_blobTypeVersion;
	KyUInt32 m_deepBlobSize;
	KyUInt32 m_shallowBlobSize;
};


class BlobAggregateWriteContext
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Blob)
public:
	BlobAggregateWriteContext()
		: m_file(KY_NULL)
		, m_memory(KY_NULL)
		, m_saveEndianness(Endianness::BigEndian)
		, m_actuallyWriting(true)
	{}
	
	File* m_file;
	char* m_memory;
	Endianness::Type m_saveEndianness;
	bool m_actuallyWriting;
};


enum BlobAggregateReadOptions
{
	BLOB_AGGREGATE_IGNORE_UNKNOWN_BLOBTYPES, 
	BLOB_AGGREGATE_WARN_UNKNOWN_BLOBTYPES
};

class BlobAggregateReadContext
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Blob)
public:
	BlobAggregateReadContext()
		: m_file(KY_NULL)
		, m_memory(KY_NULL)
		, m_memStat(MemStat_Blob)
		, m_heap(KY_NULL), 
		m_readOptions(BLOB_AGGREGATE_WARN_UNKNOWN_BLOBTYPES)
	{}

	File* m_file;
	char* m_memory;
	KyInt32 m_memStat;
	MemoryHeap* m_heap;
	BlobAggregateReadOptions m_readOptions; 
};


class BlobAggregateBlobCollection
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Blob)
public:
	BlobAggregateBlobCollection(KyUInt32 blobTypeId) : m_blobTypeId(blobTypeId) {}
	KyUInt32 m_blobTypeId;
	KyArray<Ptr<BaseBlobHandler>, MemStat_Blob> m_blobHandlers;
};

class BlobAggregate : public RefCountBase<BlobAggregate, MemStat_Blob>
{
public:
	typedef BlobAggregateWriteContext    WriteContext;
	typedef BlobAggregateReadContext     ReadContext;
	typedef BlobAggregateBlobCollection  BlobCollection;

	template<class T> class Collection
	{
		KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Blob)
	public:
		Collection() : m_blobCollection(KY_NULL) {}

		KyUInt32 GetCount() const { return m_blobCollection ? m_blobCollection->m_blobHandlers.GetCount() : 0; }
		
		BlobHandler<T>* GetHandler(UPInt index) { return (BlobHandler<T>*)m_blobCollection->m_blobHandlers[index].GetPtr(); }
		T* GetBlob(UPInt index)                 { return              (T*)m_blobCollection->m_blobHandlers[index]->VoidBlob(); }

		BlobCollection* m_blobCollection;
	};

public:
	BlobAggregate() : m_blobsCountInCollections(0), m_writeVersionBlob(true) {}

	virtual ~BlobAggregate() { Clear(); }

	void Clear();

	void AddBlob(BaseBlobHandler* blobHandler); // does call AddRef()

	KyUInt32 ComputeByteSize();

	KyResult Save(const char* fileName, FileOpenerBase* fileOpener, Endianness::Type endianness = Endianness::BigEndian);
	KyResult Save(File* file,                                       Endianness::Type endianness = Endianness::BigEndian);
	KyResult SaveToMemory(void* memory,                             Endianness::Type endianness = Endianness::BigEndian);

	KyResult Load(const char* fileName, FileOpenerBase* fileOpener, KyInt32 memStat = MemStat_Blob, MemoryHeap* heap = KY_NULL, BlobAggregateReadOptions readOptions = BLOB_AGGREGATE_WARN_UNKNOWN_BLOBTYPES);
	KyResult Load(File* file, KyInt32 memStat = MemStat_Blob, MemoryHeap* heap = KY_NULL, BlobAggregateReadOptions readOptions = BLOB_AGGREGATE_WARN_UNKNOWN_BLOBTYPES);
	KyResult LoadFromMemory(void* memory, BlobAggregateReadOptions readOptions = BLOB_AGGREGATE_WARN_UNKNOWN_BLOBTYPES);

	KyUInt32 GetBlobsCount() const { return m_blobsCountInCollections; }

	template<class T>
	Collection<T> GetCollection() const
	{
		Collection<T> collection;
		collection.m_blobCollection = GetBlobCollection(T::GetBlobTypeId());
		return collection;
	}


private:
	KyUInt32 Write      (WriteContext& writeContext);
	KyUInt32 WriteBlob  (WriteContext& writeContext, const BaseBlobHandler& blobHandler) const;
	KyUInt32 WriteBytes (WriteContext& writeContext, void* src, KyUInt32 size) const;

	KyResult Read       (ReadContext& readContext);
	void*    ReadBytes  (ReadContext& readContext, void* memoryOnStack, KyUInt32 size);
	KyResult IgnoreBytes(ReadContext& readContext, KyUInt32 byteCount);

	BlobCollection* GetOrCreateCollection(KyUInt32 blobTypeId);
	BlobCollection* GetBlobCollection(KyUInt32 blobTypeId) const;

	void UpdateVersionBlob();

private:
	KyUInt32 m_blobsCountInCollections;
	KyArray<BlobCollection*, MemStat_Blob> m_collections;
	Ptr<BlobHandler<SdkVersionBlob> > m_versionBlobHandler; // This blob is stored out of m_collections
	bool m_writeVersionBlob;

public: //internal	
	void SetDoNotWriteVersionBlob() { m_writeVersionBlob = false; } // useful for testing binary BlobAggregate between two different versions
};

} // namespace Kaim


#endif // Navigation_BlobAggregate_H
