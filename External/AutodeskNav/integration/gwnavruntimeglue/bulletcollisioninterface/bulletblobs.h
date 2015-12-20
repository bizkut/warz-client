/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef KyRuntimeGlue_BulletBlobs_H
#define KyRuntimeGlue_BulletBlobs_H

#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/blob/iblobcategorybuilder.h"
#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/blob/blobcategory.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/collision/indexedmeshblob.h"

class btStridingMeshInterface;

enum RuntimeGlueCategories
{
	Blob_RuntimeGlue_Visibility = 0,
};

enum VisibilityCategory
{
	Blob_RuntimeGlue_Visibility_btStridingMeshInterfaceBlob  = 0,
	Blob_RuntimeGlue_Count
};

#define VISIBILITY_ROOT_BLOB_CLASS(blob_category, blob_class_name, blob_version) \
	KY_NAMESPACE_ROOT_BLOB_CLASS(RuntimeGlue, blob_category, blob_class_name, blob_version)

class btStridingMeshInterfaceBlobBuilder : public Kaim::BaseBlobBuilder<Kaim::IndexedMeshArrayBlob>
{
public:
	explicit btStridingMeshInterfaceBlobBuilder(btStridingMeshInterface* triangleMesh) : m_meshInterface(triangleMesh) {}

	void DoBuild();

	btStridingMeshInterface* m_meshInterface;
};

//class btQuantizedBvhFloatBlobBuilder : public Kaim::BaseBlobBuilder<btQuantizedBvhFloatBlob>
//{
//public:
//	btTriangleMeshShapeBlob() {}
//
//	void DoBuild()
//	{
//		m_triangleMeshShapeData->m_meshInterface;
//	}
//
//	btTriangleMeshShapeData* m_triangleMeshShapeData;
//};
//
//class btTriangleMeshShapeBlob
//{
//public:
//	btStridingMeshInterfaceBlob m_meshInterface;
//	//btQuantizedBvhFloatData		*m_quantizedFloatBvh;
//	float	m_collisionMargin;
//};

class CollisionSystemBlobCategoryBuilder : public Kaim::IBlobCategoryBuilder
{
public:
	CollisionSystemBlobCategoryBuilder() : IBlobCategoryBuilder(Kaim::Blob_RuntimeGlue, Blob_RuntimeGlue_Visibility, Blob_RuntimeGlue_Count){}

	virtual void Build(Kaim::BlobCategory* category) const
	{
		//category->AddBlobType<btStridingMeshInterfaceBlob>();
	}
};

#endif // KyRuntimeGlue_BulletBlobs_H
