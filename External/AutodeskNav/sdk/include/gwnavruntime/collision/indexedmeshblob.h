/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_IndexedMeshBlob_H
#define Navigation_IndexedMeshBlob_H

#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/collision/collisiondatablobcategory.h"

namespace Kaim
{

class IndexedMeshBlob
{
	KY_ROOT_BLOB_CLASS(CollisionData, IndexedMeshBlob, 0)
	KY_CLASS_WITHOUT_COPY(IndexedMeshBlob)
public:
	IndexedMeshBlob() {}

	KyUInt32 GetIndexStride()     const { return 3*sizeof(KyUInt32); }
	KyUInt32 GetVertexStride()    const { return sizeof(Vec3f); }
	const UByte * GetIndexBase()  const { return (const UByte*)m_indices.GetValues(); }
	const UByte * GetVertexBase() const { return (const UByte*)m_vertices.GetValues(); }

	KyUInt32 GetTriangleCount()   const { return m_indices.GetCount() / 3; }

	BlobArray<Vec3f>    m_vertices;
	BlobArray<KyUInt32> m_indices;
};

inline void SwapEndianness(Kaim::Endianness::Target e, IndexedMeshBlob& self)
{
	SwapEndianness(e, self.m_vertices);
	SwapEndianness(e, self.m_indices);
}

class IndexedMeshBlobBuilder : public BaseBlobBuilder<IndexedMeshBlob>
{
public:
	enum IndexedState
	{
		NotIndexed = 0,
		Indexed    = 1,
	};

	IndexedMeshBlobBuilder() : m_indexedState(NotIndexed) {}

	void Clear();

	bool IsEmpty() { return m_indices.GetCount() == 0; }
	void AddTriangle(const Vec3f& a, const Vec3f& b, const Vec3f& c);

	virtual void DoBuild();

private:
	void MakeIndexed();

	KyArrayPOD<KyUInt32> m_indices;
	KyArray<Vec3f>       m_vertices;
	IndexedState         m_indexedState;
};

class IndexedMeshArrayBlob
{
	KY_ROOT_BLOB_CLASS(CollisionData, IndexedMeshArrayBlob, 0)
	KY_CLASS_WITHOUT_COPY(IndexedMeshArrayBlob)

public:
	IndexedMeshArrayBlob() {}
	//
	//KyFloat32                 m_scaling[4];
	BlobArray<IndexedMeshBlob> m_meshParts;
};

inline void SwapEndianness(Kaim::Endianness::Target e, IndexedMeshArrayBlob& self)
{
	SwapEndianness(e, self.m_meshParts);
}

}

#endif // Navigation_IndexedMeshBlob_H