/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_InputMesh_H
#define Navigation_InputMesh_H

#include "gwnavruntime/collision/indexedmeshblob.h"

namespace Kaim
{

class File;

/// The IndexedMesh class represents a 3D Triangle Mesh with Indexing.
/// You should use a IndexedMeshBlobBuilder in order to create an instance of this class.
/// Alternatively you can Initialize it from a previously serialized IndexedMeshBlob.

class IndexedMesh : public RefCountBase<IndexedMesh, MemStat_CollisionData>
{
public:
	IndexedMesh() : m_blob(KY_NULL) {}

	void InitFromBuilder(IndexedMeshBlobBuilder& builder);
	void InitFromHandler(Ptr<BlobHandler<IndexedMeshBlob> > handler);

	KyUInt32 GetTriangleCount() const { return m_blob->m_indices.GetCount() / 3; }
	KyUInt32 GetVertexCount() const   { return m_blob->m_vertices.GetCount(); }
	KyUInt32 GetIndexCount() const    { return m_blob->m_indices.GetCount(); }

	KyUInt32 GetIndexAt(const KyUInt32 i) const { return m_blob->m_indices.GetValues()[i]; }
	Vec3f GetVertexAt(const KyUInt32 i) const { return m_blob->m_vertices.GetValues()[i]; }

	const UByte* GetIndicesDataPtr()  const { return m_blob->GetIndexBase(); }
	const UByte* GetVerticesDataPtr() const { return m_blob->GetVertexBase(); }

	KyUInt32 GetIndexStride() const { return 3 * sizeof(KyUInt32); }
	KyUInt32 GetVertexStride() const { return 3 * sizeof(KyFloat32); }

	KyResult WriteToObjFile(File* objFile) const;

	BlobHandler<IndexedMeshBlob> * GetBlobHandler() { return m_blobHandler; }

private:
	Ptr<BlobHandler<IndexedMeshBlob> >  m_blobHandler;
	IndexedMeshBlob*                    m_blob;
};

}

#endif // Navigation_InputMesh_H
