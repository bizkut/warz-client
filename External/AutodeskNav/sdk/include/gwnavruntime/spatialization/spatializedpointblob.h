/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: MAMU
#ifndef Navigation_SpatializedPointBlob_H
#define Navigation_SpatializedPointBlob_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/world/worldelement.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/database/positionspatializationrange.h"


namespace Kaim
{

class SpatializedPoint;
class PointDbSpatialization;

//////////////////////////////////////////////////////////////////////////
// PointDbSpatializationBlob

enum PointDbSpatializationFlag
{
	PointDbSpatializationFlag_InNavMesh = 1 << 0
};

class PointDbSpatializationBlob
{
public:
	KyUInt32 m_flags; // PointDbSpatializationFlag
	KyUInt32 m_databaseIndex;
	Vec3f m_triangleCenter;
	Vec3f m_triangleNormal;
	PositionSpatializationRange m_positionSpatializationRange;
};

inline void SwapEndianness(Endianness::Target e, PointDbSpatializationBlob& self)
{
	SwapEndianness(e, self.m_flags);
	SwapEndianness(e, self.m_databaseIndex);
	SwapEndianness(e, self.m_triangleCenter);
	SwapEndianness(e, self.m_triangleNormal);
	SwapEndianness(e, self.m_positionSpatializationRange);
}

class PointDbSpatializationBlobBuilder : public BaseBlobBuilder<PointDbSpatializationBlob>
{
	KY_CLASS_WITHOUT_COPY(PointDbSpatializationBlobBuilder)

public:
	PointDbSpatializationBlobBuilder(const PointDbSpatialization* dbProjection, const SpatializedPoint* spatializedPoint, KyUInt32 databaseIndex)
		: m_dbProjection(dbProjection), m_spatializedPoint(spatializedPoint), m_databaseIndex(databaseIndex)
	{}

	virtual void DoBuild();

private:
	const PointDbSpatialization* m_dbProjection;
	const SpatializedPoint* m_spatializedPoint;
	KyUInt32 m_databaseIndex;
};

//////////////////////////////////////////////////////////////////////////
// SpatializationResult
class SpatializationResultBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Blob)
	KY_ROOT_BLOB_CLASS(Runtime, SpatializationResultBlob, 0)

public:
	KyUInt32 GetOutsideNavmeshCount() const;
	bool IsOutsideNavMesh(KyUInt32 databaseIndex) const;

public:
	KyUInt32 m_visualDebugId;
	BlobArray<PointDbSpatializationBlob> m_pointDbSpatializations;
};

inline void SwapEndianness(Endianness::Target e, SpatializationResultBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_pointDbSpatializations);
}

class SpatializationResultBlobBuilder : public BaseBlobBuilder<SpatializationResultBlob>
{
	KY_CLASS_WITHOUT_COPY(SpatializationResultBlobBuilder)

public:
	SpatializationResultBlobBuilder(KyUInt32 visualDebugId, const SpatializedPoint* spatializedPoint)
		: m_visualDebugId(visualDebugId), m_spatializedPoint(spatializedPoint)
	{}

	virtual void DoBuild();

private:
	KyUInt32 m_visualDebugId;
	const SpatializedPoint* m_spatializedPoint;
};


//////////////////////////////////////////////////////////////////////////
// SpatializedPointBlob
class SpatializedPointBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Blob)
	KY_ROOT_BLOB_CLASS(Runtime, SpatializedPointBlob, 0)
	//KY_CLASS_WITHOUT_COPY(SpatializedPointBlob)

public:
	KyUInt32 GetOutsideNavmeshCount() const;

public:
	BlobArray<PointDbSpatializationBlob> m_pointDbSpatializations;
	Vec3f m_position;
	KyFloat32 m_height;
	KyUInt32 m_type; // stores the SpatializedPointObjectType
};

inline void SwapEndianness(Endianness::Target e, SpatializedPointBlob& self)
{
	SwapEndianness(e, self.m_pointDbSpatializations);
	SwapEndianness(e, self.m_position);
	SwapEndianness(e, self.m_height);
	SwapEndianness(e, self.m_type);
}


//////////////////////////////////////////////////////////////////////////
// SpatializedPointBlobBuilder
class SpatializedPointBlobBuilder : public BaseBlobBuilder<SpatializedPointBlob>
{
	KY_CLASS_WITHOUT_COPY(SpatializedPointBlobBuilder)

public:
	SpatializedPointBlobBuilder(const SpatializedPoint* spatializedPoint, VisualDebugLOD visualDebugLOD)
		: m_spatializedPoint(spatializedPoint)
		, m_visualDebugLOD(visualDebugLOD)
	{}

	virtual void DoBuild();

private:
	void FillPointDbSpatializationBlob(const PointDbSpatialization& dbProjection,
		PointDbSpatializationBlob& pointDbSpatialization, KyUInt32 i);

	const SpatializedPoint* m_spatializedPoint;
	VisualDebugLOD m_visualDebugLOD;
};



} // namespace Kaim

#endif // Navigation_SpatializedPointBlob_H
