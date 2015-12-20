/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: NOBODY
#ifndef Navigation_MessageBlob_H
#define Navigation_MessageBlob_H

#include "gwnavruntime/visualdebug/amp/Amp_Message.h"
#include "gwnavruntime/visualdebug/amp/Amp_Interfaces.h"
#include "gwnavruntime/blob/baseblobhandler.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/kernel/SF_Stats.h"
#include "gwnavruntime/kernel/SF_Threads.h"
#include "gwnavruntime/base/endianness.h"


namespace Kaim
{

// Message that includes a blob field
class MessageBlob : public Net::AMP::Message
{
public:
	static const char*  GetTypeName()              { return "MessageBlob"; }
	static UInt32 GetLatestVersion()               { return Net::AMP::Message::GetLatestVersion(); }

	MessageBlob() : Message()
		, m_blobTypeId(0)
		, m_shallowBlobSize(0)
		, m_blobHandler(KY_NULL)
	{
		Version = GetLatestVersion();
	}

	virtual ~MessageBlob() {}

	virtual void Read(File& str);
	virtual void Write(File& str) const;

	virtual String ToString() const;

	virtual const char* GetMessageTypeName() const { return MessageBlob::GetTypeName(); }

	UInt32 GetBlobTypeId()          const { return m_blobTypeId; }
	UInt32 GetShallowBlobSize()     const { return m_shallowBlobSize; }
	UInt32 GetDeepBlobSize()		const { return (m_blobHandler ? m_blobHandler->GetDeepBlobSize() : (UInt32)m_blob.Data.GetSize()); }
	const void* GetBlob()           const { return (m_blobHandler ? m_blobHandler->GetMutableBlob() : m_blob.Data.GetDataPtr()); }

	void Init(KyUInt32 blobtypeId, KyUInt32 shallowBlobSize, KyUInt32 deepBlobSize, char* blob);
	void Init(Ptr<BaseBlobHandler> blobHandler);

protected:
	void SwapBlobEndianness(Endianness::Target target) const; // not really const since it modifies m_blob.Data

	UInt32                     m_blobTypeId;
	UInt32                     m_shallowBlobSize;
	
	// m_blobHandler or m_blob are used when sending
	// m_blob is always the one used at reception
	Ptr<BaseBlobHandler>  m_blobHandler;
	mutable Net::AMP::RawData m_blob; // mutable so that endianness of m_blob.Data can be swapped in Write, in the socket thread. 
};


// Message that includes a buffer that contains several blobs
class MessageAggregatedBlob : public Net::AMP::Message
{
public:
	static const char*  GetTypeName()              { return "MessageAggregatedBlob"; }
	static UInt32 GetLatestVersion()         { return Net::AMP::Message::GetLatestVersion(); }

	MessageAggregatedBlob() : Message()
		, m_currentIndex(0)
	{
		Version = GetLatestVersion();
	}

	virtual ~MessageAggregatedBlob() {}

	virtual void Read(File& str);
	virtual void Write(File& str) const;

	virtual String ToString() const;

	virtual const char* GetMessageTypeName() const { return MessageAggregatedBlob::GetTypeName(); }

	bool GoFirst();
	bool GoNext();

	KyUInt32 GetCurrentBlobTypeId();
	KyUInt32 GetCurrentShallowBlobSize();
	KyUInt32 GetCurrentDeepBlobSize();
	const void* GetCurrentBlob();


	void Init(KyUInt32 bufferSize);
	KyResult Add(KyUInt32 blobTypeId, KyUInt32 shallowBlobSize, KyUInt32 deepBlobSize, char* blob);
	KyResult Add(Ptr<BaseBlobHandler> blobHandler)
	{ return Add(blobHandler->GetBlobTypeId(), blobHandler->GetShallowBlobSize(), blobHandler->GetDeepBlobSize(), (char*)blobHandler->GetMutableBlob()); }

	UInt32 GetAggregateSize() { return (UInt32) m_data.Data.GetSize(); }

protected:
	void SwapBlobEndianness(Endianness::Target target, void* blob, KyUInt32 blobTypeId) const; // not really const since it modifies m_blob.Data
	mutable Net::AMP::RawData m_data; // mutable so that endianness of can be swapped in Write, in the socket thread. 

	UInt32 m_currentIndex;
};


} // namespace Kaim

#endif // Navigation_MessageBlob_H
