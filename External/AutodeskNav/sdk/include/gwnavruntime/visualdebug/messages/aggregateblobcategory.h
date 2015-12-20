/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: NOBODY
#ifndef Navigation_RawDataBlob_H
#define Navigation_RawDataBlob_H


#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/blob/iblobcategorybuilder.h"
#include "gwnavruntime/basesystem/version.h"
#include "gwnavruntime/kernel/SF_String.h"

namespace Kaim
{


enum AggregateBlobCategory
{
	// DO NOT remove, comment or change order in this enum
	Blob_Navigation_Aggregate_SdkVersionBlob      = 0,
	Blob_Navigation_Aggregate_NavDataRawBlob      = 1,
	Blob_Navigation_Aggregate_ClientInputRawBlob  = 2,
	Blob_Navigation_Aggregate_ColDataRawBlob      = 3,

	Blob_Navigation_Aggregate_Count
};
class AggregateBlobCategoryBuilder : public IBlobCategoryBuilder
{
public:
	AggregateBlobCategoryBuilder() : IBlobCategoryBuilder(Blob_Navigation, Blob_Navigation_Aggregate, Blob_Navigation_Aggregate_Count) {}
	virtual void Build(BlobCategory* category) const;
};

// SdkVersionBlob contains the full version string describing the version of Gameware Navigation.
// It is used to identify the version a BlobAggregate was constructed from, or,
// sent by the VisualDebugServer or written in a .VisualDebug file 
// allowing the NavigationLab to compare it to its own version.
class SdkVersionBlob
{
	KY_ROOT_BLOB_CLASS(Aggregate, SdkVersionBlob, 0)
public:
	bool GetSdkVersion(Version& version)
	{ return Version::GetFromFullVersionString(version, m_data.GetValues()); }

public:
	BlobArray<char> m_data;
};
inline void SwapEndianness(Kaim::Endianness::Target e, SdkVersionBlob& self)
{
	SwapEndianness(e, self.m_data);
}

// Construct a SdkVersionBlob using Kaim::Version::GetFullVersion()
class SdkVersionBlobBuilder : public BaseBlobBuilder<SdkVersionBlob>
{
public:
	SdkVersionBlobBuilder() {}

private:
	virtual void DoBuild()
	{
		const char* version = Version::GetFullVersion();
		BLOB_STRING(m_blob->m_data, version);
	}
};

// This blob is a buffer containing a NavData which is a BlobAggregate and so it supports SwapEndianness by itself.
// Typically used to send .NavData file via VisualDebug
class NavDataRawBlob
{
public:
	KY_ROOT_BLOB_CLASS(Aggregate, NavDataRawBlob, 0)
public:
	BlobArray<char> m_data;
};
inline void SwapEndianness(Kaim::Endianness::Target e, NavDataRawBlob& self)
{
	SwapEndianness(e, self.m_data);
}

// This blob is a buffer containing a ClientInput which is a BlobAggregate and so it supports SwapEndianness by itself.
// Typically used to sent .ClientInput file via VisualDebug
class ClientInputRawBlob
{
public:
	KY_ROOT_BLOB_CLASS(Aggregate, ClientInputRawBlob, 0)
public:
	BlobArray<char> m_data;
};
inline void SwapEndianness(Kaim::Endianness::Target e, ClientInputRawBlob& self)
{
	SwapEndianness(e, self.m_data);
}

// This blob is a buffer containing a NavData which is a BlobAggregate and so it supports SwapEndianness by itself.
// Typically used to send .NavData file via VisualDebug
class ColDataRawBlob
{
public:
	KY_ROOT_BLOB_CLASS(Aggregate, ColDataRawBlob, 0)
public:
	BlobArray<char> m_data;
};
inline void SwapEndianness(Kaim::Endianness::Target e, ColDataRawBlob& self)
{
	SwapEndianness(e, self.m_data);
}

// RawDataBlobBuilder is a generic BlobBuilder used to construct blobs that are simply buffers;
// It is expected to contains data that supports SwapEndiannes by itself, 
// such as BlobAggregate, for instance NavData or ClientInput.
template<class T>
class RawDataBlobBuilder : public BaseBlobBuilder<T>
{
public:
	RawDataBlobBuilder(const char* data, KyUInt32 dataSize) : m_data(data), m_dataSize(dataSize) {}

private:
	virtual void DoBuild()
	{
		BLOB_ARRAY_COPY(this->m_blob->m_data, this->m_data, this->m_dataSize);
	}

	const char* m_data;
	KyUInt32 m_dataSize;
};


}

#endif