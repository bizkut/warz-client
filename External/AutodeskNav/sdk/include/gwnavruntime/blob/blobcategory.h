/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobCategory_H
#define Navigation_BlobCategory_H


#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/blob/iblobcategorybuilder.h"


namespace Kaim
{


/// The BlobCategory class represents a collection of blob types. It is used to define the way individual blobs
/// of different kinds can be packaged together into a larger binary blob.
class BlobCategory
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_BaseSystem)

public:
	BlobCategory(KyUInt32 nameSpaceEnum, KyUInt32 categoryEnum)
	{
		m_categoryId = BlobTypeIdUtils::BuildCategoryId(nameSpaceEnum, categoryEnum);
	}

	template<class T> void AddBlobType()
	{
		Ptr<BlobTypeDescriptor<T> > blobTypeDescriptor = *KY_NEW_ID(MemStat_BaseSystem) BlobTypeDescriptor<T>;
		KyUInt32 blobEnum = BlobTypeIdUtils::GetBlobEnum(blobTypeDescriptor->GetBlobTypeId());
		m_descriptors[blobEnum] = blobTypeDescriptor;
	}

	void Resize(KyUInt32 count) { return m_descriptors.Resize(count); }

	void Clear() { m_descriptors.Clear(); }

	KyUInt32 GetCount() { return m_descriptors.GetCount(); }

	IBlobTypeDescriptor* GetDescriptor(KyUInt32 blobTypeId) const { return m_descriptors[BlobTypeIdUtils::GetBlobEnum(blobTypeId)]; }

public:
	KyUInt32 m_categoryId;
	KyArray<Ptr<IBlobTypeDescriptor>, MemStat_BaseSystem> m_descriptors; ///< The array of descriptors that make up this registry. Do not modify. 
};


}


#endif

