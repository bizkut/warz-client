/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LASI - secondary contact: NOBODY
#ifndef GwNavGen_ClientInputChunkChunk_H
#define GwNavGen_ClientInputChunkChunk_H


#include "gwnavgeneration/input/taggedtriangle3f.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavgeneration/input/clientinputtagvolume.h"

namespace Kaim
{

class DynamicClientInputChunk
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	DynamicClientInputChunk() {}

public:
	KyArray<DynamicTaggedTriangle3f> m_dynTaggedTriangle3fs;  // in client coordinates
	KyArray<ClientInputTagVolume>    m_tagVolumes;            // in client coordinates
	KyArray<Vec3f>                   m_seedPoints;            // in client coordinates
};

class ClientInputChunk
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Generator, ClientInputChunk, 0)

public:
	ClientInputChunk() {}

public:
	BlobArray<TaggedTriangle3f>            m_taggedTriangle3fs;  // in client coordinates
	BlobArray<ClientInputTagVolumeBlob>    m_tagVolumeBlobs;     // in client coordinates
	BlobArray<Vec3f>                       m_seedPoints;         // in client coordinates
};
inline void SwapEndianness(Endianness::Target e, ClientInputChunk& self)
{
	SwapEndianness(e, self.m_taggedTriangle3fs);
	SwapEndianness(e, self.m_tagVolumeBlobs);
	SwapEndianness(e, self.m_seedPoints);
}

class ClientInputChunkBlobBuilder : public BaseBlobBuilder<ClientInputChunk>
{
public:
	ClientInputChunkBlobBuilder(const DynamicClientInputChunk& clientInputChunk) : m_dynClientInputChunk(&clientInputChunk) {}
private:
	void DoBuild()
	{
		TaggedTriangle3f* taggedTriangle3fArray = BLOB_ARRAY(m_blob->m_taggedTriangle3fs, m_dynClientInputChunk->m_dynTaggedTriangle3fs.GetCount());
		for(UPInt i = 0; i < m_dynClientInputChunk->m_dynTaggedTriangle3fs.GetCount(); ++i)
		{
			TaggedTriangle3fBlobBuilder blobBuider(m_dynClientInputChunk->m_dynTaggedTriangle3fs[i]);
			BLOB_BUILD(taggedTriangle3fArray[i], blobBuider);
		}

		ClientInputTagVolumeBlob* tagVolumeBlobArray = BLOB_ARRAY(m_blob->m_tagVolumeBlobs, m_dynClientInputChunk->m_tagVolumes.GetCount());
		for(UPInt i = 0; i < m_dynClientInputChunk->m_tagVolumes.GetCount(); ++i)
		{
			ClientInputTagVolumeBlobBuilder blobBuider(m_dynClientInputChunk->m_tagVolumes[i]);
			BLOB_BUILD(tagVolumeBlobArray[i], blobBuider);
		}

		BLOB_ARRAY_COPY(m_blob->m_seedPoints, m_dynClientInputChunk->m_seedPoints.GetDataPtr(), m_dynClientInputChunk->m_seedPoints.GetCount());
	}

	const DynamicClientInputChunk* m_dynClientInputChunk;
};


}


#endif

