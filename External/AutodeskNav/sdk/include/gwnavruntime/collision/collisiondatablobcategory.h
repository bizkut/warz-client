/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JAPA - secondary contact: NOBODY
#ifndef Navigation_ColDataBlobCategory_H
#define Navigation_ColDataBlobCategory_H

#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/blob/iblobcategorybuilder.h"

namespace Kaim
{

enum ColDataCategory
{
	// DO NOT remove, comment or change order in this enum
	Blob_Navigation_CollisionData_ColDataIndex                = 0,
	Blob_Navigation_CollisionData_HeightFieldBlob             = 1,
	Blob_Navigation_CollisionData_IndexedMeshBlob             = 2,
	Blob_Navigation_CollisionData_IndexedMeshArrayBlob        = 3,
	Blob_Navigation_CollisionData_ColDataSectorDescriptorBlob = 4,

	Blob_Navigation_ColData_Count
};


class ColDataCategoryBuilder : public IBlobCategoryBuilder
{
public:
	ColDataCategoryBuilder() : IBlobCategoryBuilder(Blob_Navigation, Blob_Navigation_CollisionData, Blob_Navigation_ColData_Count) {}
	virtual void Build(BlobCategory* category) const;
};

}

#endif // Navigation_ColDataBlobCategory_H

