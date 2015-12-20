/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: NOBODY
#ifndef GwNavGen_ClientInputDescriptorBlob_H
#define GwNavGen_ClientInputDescriptorBlob_H

#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/blob/baseblobbuilder.h"


namespace Kaim
{


class ClientInputDescriptorBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(ClientInputDescriptorBlob)
	KY_ROOT_BLOB_CLASS(Generator, ClientInputDescriptorBlob, 0)

public:
	ClientInputDescriptorBlob() {}

	BlobArray<char> m_clientInputFilename;
};
inline void SwapEndianness(Endianness::Target e, ClientInputDescriptorBlob& self)
{
	SwapEndianness(e, self.m_clientInputFilename);
}


struct ClientInputDescriptor
{
public:
	ClientInputDescriptor() {} // To make KyArray happy

	ClientInputDescriptor(const char* filename)
		: m_clientInputFilename(filename) {}

	void Init(const ClientInputDescriptorBlob& blob)
	{
		m_clientInputFilename = blob.m_clientInputFilename.GetValues();
	}

public:
	String m_clientInputFilename;
};


class ClientInputDescriptorBlobBuilder : public BaseBlobBuilder<ClientInputDescriptorBlob>
{
public:
	ClientInputDescriptorBlobBuilder(const ClientInputDescriptor& information) : m_info(&information) {}

private:
	virtual void DoBuild()
	{
		BLOB_STRING(m_blob->m_clientInputFilename, m_info->m_clientInputFilename.ToCStr());
	}

	const ClientInputDescriptor* m_info;
};


} // namespace Kaim

#endif
