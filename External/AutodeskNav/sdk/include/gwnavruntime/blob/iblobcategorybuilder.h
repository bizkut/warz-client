/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_IBlobCategoryBuilder_H
#define Navigation_IBlobCategoryBuilder_H


#include "gwnavruntime/base/memory.h"


namespace Kaim
{


class BlobCategory;


class IBlobCategoryBuilder
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
public:
	IBlobCategoryBuilder(KyUInt32 nameSpaceEnum, KyUInt32 categoryEnum, KyUInt32 blobTypeCount)
		: m_nameSpaceEnum(nameSpaceEnum), m_categoryEnum(categoryEnum), m_blobTypeCount(blobTypeCount) {}

	virtual ~IBlobCategoryBuilder() {}

	KyResult BuildCategory(BlobCategory* category) const;

protected:
	virtual void Build(BlobCategory* category) const = 0;

public:
	KyUInt32 m_nameSpaceEnum;
	KyUInt32 m_categoryEnum;
	KyUInt32 m_blobTypeCount;
};


}


#endif

