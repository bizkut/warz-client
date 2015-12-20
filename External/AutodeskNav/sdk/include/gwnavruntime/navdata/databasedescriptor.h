/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: 
#ifndef Navigation_DatabaseDescriptor_H
#define Navigation_DatabaseDescriptor_H

#include "gwnavruntime/blob/blobfieldarray.h"
#include "gwnavruntime/blob/blobaggregate.h"

namespace Kaim
{

/// Set of Key-Values embedded within genIO BlobAggregate (serialized form).
/// These Key-Values allows typically to retrieve information about a group of data in the same database.
class DatabaseDescriptorBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	KY_CLASS_WITHOUT_COPY(DatabaseDescriptorBlob)
	KY_ROOT_BLOB_CLASS(NavData, DatabaseDescriptorBlob, 0)

public:
	DatabaseDescriptorBlob() {}
	BlobFieldArray m_fields;
};
inline void SwapEndianness(Endianness::Target e, DatabaseDescriptorBlob& self)
{
	SwapEndianness(e, self.m_fields);
}

/// Easy to write/read version of DatabaseDescriptorBlob.
class DatabaseDescriptor
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	DatabaseDescriptor() : m_databaseIndex(0) {}

	KyResult ReadFromAggregate(const BlobAggregate& aggregate)
	{
		BlobAggregate::Collection<DatabaseDescriptorBlob> databaseDescriptor = aggregate.GetCollection<DatabaseDescriptorBlob>();
		if (databaseDescriptor.GetCount() != 1)
			return KY_ERROR; // 1 and only 1 DatabaseDescriptorBlob
		return ReadFromBlob(*databaseDescriptor.GetHandler(0)->Blob());
	}

	void AddMapping(BlobFieldsMapping& mapping)
	{
		mapping.AddUInt32("Database index", m_databaseIndex);
	}

	KyResult ReadFromBlob(const DatabaseDescriptorBlob& blob)
	{
		BlobFieldsMapping mapping;
		AddMapping(mapping);
		return mapping.ReadFromBlobFieldArray(blob.m_fields);
	}

public:
	/// Indicative index of the database. Typically used when loading a genIO file to know
	/// to which database its data should be associated to.
	KyUInt32 m_databaseIndex;
};

class DatabaseDescriptorBlobBuilder : public BaseBlobBuilder<DatabaseDescriptorBlob>
{
public:
	DatabaseDescriptorBlobBuilder(const DatabaseDescriptor* descriptor)
		: m_descriptor(descriptor) {}

private:
	virtual void DoBuild()
	{
		BlobFieldsMapping mapping;
		// AddMapping is not const, because it is used both for read and write.
		// Here we use it to 
		DatabaseDescriptor* notConstDescriptor = (DatabaseDescriptor*)m_descriptor;
		notConstDescriptor->AddMapping(mapping);
		BlobFieldArrayBuilder blobFieldArrayBuilder("DatabaseDescriptor", mapping);
		BLOB_BUILD(m_blob->m_fields, blobFieldArrayBuilder);
	}
	const DatabaseDescriptor* m_descriptor;
};

} // namespace Kaim

#endif
