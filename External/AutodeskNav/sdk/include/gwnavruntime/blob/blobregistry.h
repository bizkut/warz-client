/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobRegistry_H
#define Navigation_BlobRegistry_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/containers/kyarray.h"


namespace Kaim
{


class IBlobCategoryBuilder;
class BlobCategory;
class IBlobTypeDescriptor;


class BlobNameSpace
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_BaseSystem)

public:
	BlobNameSpace(KyUInt32 nameSpaceEnum, KyUInt32 categoryCount);
	~BlobNameSpace();

	void AddCategory(IBlobCategoryBuilder* categoryBuilder);

	BlobCategory* GetCategory(KyUInt32 categoryEnum) { return m_categories[categoryEnum]; }

	IBlobTypeDescriptor* GetDescriptor(KyUInt32 blobTypeId, bool warnIfUnknown = true);

public:
	KyUInt32 m_nameSpaceEnum;
	KyArray<BlobCategory*, MemStat_BaseSystem> m_categories;
};


class BlobRegistry
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_BaseSystem)

public:
	static void Init();
	static void Destroy();

	static void AddCategory(IBlobCategoryBuilder* categoryBuilder);
	static IBlobTypeDescriptor* GetDescriptor(KyUInt32 blobTypeId, bool warnIfUnknown = true);

public:
	static BlobNameSpace* GetNameSpace(KyUInt32 nameSpaceEnum);
	static BlobNameSpace* GetOrCreateNameSpace(KyUInt32 nameSpaceEnum, KyUInt32 categoryCount);
	static BlobRegistry* s_instance;

	~BlobRegistry();

	KyArray<BlobNameSpace*, MemStat_BaseSystem> m_nameSpaces;
};


}


#endif
