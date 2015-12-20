/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobTypes_H
#define Navigation_BlobTypes_H


#include "gwnavruntime/base/types.h"


namespace Kaim
{

// BlobNameSpaces
enum BlobNameSpaces
{
	// DO NOT remove, comment or change order in this enum
	Blob_Navigation  = 0,
	Blob_Examples = 1,
	Blob_UnitTest = 2,
	Blob_RuntimeGlue = 3,
	Blob_Client   = 16
};

// Navigation Categories
enum NavigationCategories
{
	// DO NOT remove, comment or change order in this enum
	Blob_Navigation_NavData            = 0,
	Blob_Navigation_Runtime            = 1,
	Blob_Navigation_VisualDebug        = 2,
	Blob_Navigation_VisualDebugMessage = 3,
	Blob_Navigation_Generator          = 4,
	Blob_Navigation_Aggregate          = 5,
	Blob_Navigation_Query              = 6,
	Blob_Navigation_CollisionData      = 7,

	Blob_Navigation_Count
};


class BlobTypeIdUtils
{
public:
	static KyUInt32 GetMask()              { return 0xFF; }
	static KyUInt32 GetCategoryBitShift()  { return 8;    }
	static KyUInt32 GetNameSpaceBitShift() { return 16;   }

	// --------------- GetXxxEnum functions are always on 8 bits ----------------
	static KyUInt32 GetBlobEnum(     KyUInt32 blobTypeId) { return  blobTypeId                            & GetMask(); }
	static KyUInt32 GetCategoryEnum( KyUInt32 blobTypeId) { return (blobTypeId >> GetCategoryBitShift() ) & GetMask(); }
	static KyUInt32 GetNameSpaceEnum(KyUInt32 blobTypeId) { return (blobTypeId >> GetNameSpaceBitShift()) & GetMask(); }

	// --------------- GetXxxId functions are on 32 bits ----------------
	static KyUInt32 BuildBlobTypeId(KyUInt32 nameSpaceEnum, KyUInt32 categoryEnum, KyUInt32 blobEnum)
	{
		return (nameSpaceEnum << GetNameSpaceBitShift()) | (categoryEnum << GetCategoryBitShift()) | blobEnum;
	}

	static KyUInt32 BuildCategoryId(KyUInt32 nameSpaceEnum, KyUInt32 categoryEnum)
	{
		return BuildBlobTypeId(nameSpaceEnum, categoryEnum, 0);
	}

	static KyUInt32 BuildNameSpaceId(KyUInt32 nameSpaceEnum)
	{
		return BuildBlobTypeId(nameSpaceEnum, 0, 0);
	}

	static KyUInt32 GetCategoryId(KyUInt32 blobTypeId)  { return blobTypeId & 0x00FFFF00; }
	static KyUInt32 GetNameSpaceId(KyUInt32 blobTypeId) { return blobTypeId & 0x00FF0000; }
};

#define KY_NAMESPACE_ROOT_BLOB_CLASS(blob_namespace, blob_category, blob_class_name, blob_version) \
public:                                                                    \
	static KyUInt32 GetBlobTypeId()                                        \
	{                                                                      \
		return Kaim::BlobTypeIdUtils::BuildBlobTypeId(                     \
			Kaim::Blob_##blob_namespace,                                   \
			Blob_##blob_namespace##_##blob_category,                       \
			Blob_##blob_namespace##_##blob_category##_##blob_class_name);  \
	}                                                                      \
	static const char* GetBlobTypeName() { return #blob_class_name; }      \
	static KyUInt32 GetBlobTypeVersion() { return blob_version; }          \
private:

// 
#define KY_ROOT_BLOB_CLASS(blob_category, blob_class_name, blob_version) \
KY_NAMESPACE_ROOT_BLOB_CLASS(Navigation, blob_category, blob_class_name, blob_version)

}


#endif

