/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: MAMAU - secondary contact: NOBODY
#ifndef GwNavGen_ClientInputSectorDescriptor_H
#define GwNavGen_ClientInputSectorDescriptor_H

#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavruntime/blob/blobfieldarray.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/blob/blobaggregate.h"

namespace Kaim
{

/// Set of Key-Values embedded within a ClientInput BlobAggregate (serialized form).
/// These Key-Values allows typically to retrieve information about where the ClientInput comes from.
class ClientInputSectorDescriptorBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(ClientInputSectorDescriptorBlob)
	KY_ROOT_BLOB_CLASS(Generator, ClientInputSectorDescriptorBlob, 0)
public:
	ClientInputSectorDescriptorBlob() {}
	BlobFieldArray m_fields;
};
inline void SwapEndianness(Endianness::Target e, ClientInputSectorDescriptorBlob& self)
{
	SwapEndianness(e, self.m_fields);
}

/// Easy to write/read version of ClientInputSectorDescriptorBlob.
class ClientInputSectorDescriptor
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	ClientInputSectorDescriptor() : m_databaseIndex(0) {}

	KyResult ReadFromAggregate(const BlobAggregate& aggregate)
	{
		BlobAggregate::Collection<ClientInputSectorDescriptorBlob> sectorDescs = aggregate.GetCollection<ClientInputSectorDescriptorBlob>();
		if (sectorDescs.GetCount() != 1)
			return KY_ERROR; // 1 and only 1 ClientInputSectorDescriptor
		return ReadFromBlob(*sectorDescs.GetHandler(0)->Blob());
	}

	void AddMapping(BlobFieldsMapping& mapping)
	{
		mapping.AddUInt32("Database index", m_databaseIndex);
	}

	KyResult ReadFromBlob(const ClientInputSectorDescriptorBlob& blob)
	{
		BlobFieldsMapping mapping;
		AddMapping(mapping);
		return mapping.ReadFromBlobFieldArray(blob.m_fields);
	}

public:
	/// Index of the Lab's database in which the NavData will be added to.
	/// On connection, the NavData will be sent to the Game with this database index.
	KyUInt32 m_databaseIndex;
};

class ClientInputSectorDescriptorBlobBuilder : public BaseBlobBuilder<ClientInputSectorDescriptorBlob>
{
public:
	ClientInputSectorDescriptorBlobBuilder(ClientInputSectorDescriptor* sectorDescriptor)
		: m_descriptor(sectorDescriptor) {}

private:
	virtual void DoBuild()
	{
		BlobFieldsMapping mapping;
		m_descriptor->AddMapping(mapping);
		BlobFieldArrayBuilder blobFieldArrayBuilder("ClientInputSectorDescriptor", mapping);
		BLOB_BUILD(m_blob->m_fields, blobFieldArrayBuilder);
	}
	ClientInputSectorDescriptor* m_descriptor;
};



}


#endif

