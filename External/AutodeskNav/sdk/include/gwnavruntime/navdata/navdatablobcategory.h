/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_NavDataBlobCategory_H
#define Navigation_NavDataBlobCategory_H


#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/blob/iblobcategorybuilder.h"


namespace Kaim
{


enum NavDataCategory
{
	// DO NOT remove, comment or change order in this enum
	Blob_Navigation_NavData_NavCellBlob               = 0,
	Blob_Navigation_NavData_NavMeshElementBlob        = 1,
	Blob_Navigation_NavData_NavGraphBlob              = 2,
	Blob_Navigation_NavData_NavGraphArray             = 3,
	Blob_Navigation_NavData_NavDataIndex              = 4,
	Blob_Navigation_NavData_BlobFieldArray            = 5,
	Blob_Navigation_NavData_NavFloorBlob              = 6,
	Blob_Navigation_NavData_ActiveDataBlob            = 7,
	Blob_Navigation_NavData_NavTag                    = 8,
	Blob_Navigation_NavData_ActiveGuidsBlob           = 9,
	Blob_Navigation_NavData_TagVolumeBlob             = 10,
	Blob_Navigation_NavData_TagVolumeContextBlob      = 11,
	Blob_Navigation_NavData_NavGraphArrayLinkInfoBlob = 12,
	Blob_Navigation_NavData_KyGuid                    = 13,
	Blob_Navigation_NavData_SectorDescriptorBlob      = 14,
	Blob_Navigation_NavData_DatabaseDescriptorBlob    = 15,

	Blob_Navigation_NavData_AbstractGraphFloorBlob    = 16,
	Blob_Navigation_NavData_AbstractGraphCellBlob     = 17,
	Blob_Navigation_NavData_AbstractGraphBlob         = 18,
	Blob_Navigation_NavData_AbstractDataIndex         = 19,

	Blob_Navigation_NavData_Count
};


class NavDataCategoryBuilder : public IBlobCategoryBuilder
{
public:
	NavDataCategoryBuilder() : IBlobCategoryBuilder(Blob_Navigation, Blob_Navigation_NavData, Blob_Navigation_NavData_Count) {}
	virtual void Build(BlobCategory* category) const;
};


}


#endif // Navigation_NavDataBlobCategory_H

