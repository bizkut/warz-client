/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: LAPA
#ifndef Navigation_BoxObstacleBlob_H
#define Navigation_BoxObstacleBlob_H

#include "gwnavruntime/world/boxobstacle.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/world/tagvolumeblob.h"
#include "gwnavruntime/spatialization/spatializedcylinderblob.h"
#include "gwnavruntime/world/databasebindingblob.h"


namespace Kaim
{

//////////////////////////////////////////////////////////////////////////
//
// BoxObstacleSpatializedCylindersBlob
//
//////////////////////////////////////////////////////////////////////////
class BoxObstacleSpatializedCylindersBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Runtime, BoxObstacleSpatializedCylindersBlob, 0)
public:
	BoxObstacleSpatializedCylindersBlob() {}

	KyUInt32 m_visualDebugId;
	BlobArray<SpatializedCylinderBlob> m_spatializedCylinderBlobs;
};

inline void SwapEndianness(Endianness::Target e, BoxObstacleSpatializedCylindersBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_spatializedCylinderBlobs);
}

class BoxObstacleSpatializedCylindersBlobBuilder : public BaseBlobBuilder<BoxObstacleSpatializedCylindersBlob>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoxObstacleSpatializedCylindersBlobBuilder(const BoxObstacle* boxObstacle, KyUInt32 visualDebugId)
		: m_visualDebugId(visualDebugId), m_boxObstacle(boxObstacle) {}
	virtual void DoBuild();

private:
	KyUInt32 m_visualDebugId;
	const BoxObstacle* m_boxObstacle;
};


//////////////////////////////////////////////////////////////////////////
//
// BoxObstacle spatialized cylinders configs Blob
//
//////////////////////////////////////////////////////////////////////////
class BoxObstacleSpatializedCylinderConfigsBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Runtime, BoxObstacleSpatializedCylinderConfigsBlob, 0)

public:
	BoxObstacleSpatializedCylinderConfigsBlob() {}

	KyUInt32 m_visualDebugId;
	BlobArray<SpatializedCylinderConfigBlob> m_spatializedCylinderConfigBlobs;
	DatabaseBindingBlob m_databaseBinding;
};

inline void SwapEndianness(Endianness::Target e, BoxObstacleSpatializedCylinderConfigsBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_spatializedCylinderConfigBlobs);
	SwapEndianness(e, self.m_databaseBinding);
}

class BoxObstacleSpatializedCylinderConfigsBlobBuilder : public BaseBlobBuilder<BoxObstacleSpatializedCylinderConfigsBlob>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoxObstacleSpatializedCylinderConfigsBlobBuilder(const BoxObstacle* boxObstacle, KyUInt32 visualDebugId)
		: m_visualDebugId(visualDebugId), m_boxObstacle(boxObstacle) {}
	virtual void DoBuild();

private:
	KyUInt32 m_visualDebugId;
	const BoxObstacle* m_boxObstacle;
};


//////////////////////////////////////////////////////////////////////////
//
// BoxObstacle spatialization results blob
//
//////////////////////////////////////////////////////////////////////////
class BoxObstacleSpatializationResultsBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Runtime, BoxObstacleSpatializationResultsBlob, 0)

public:
	BoxObstacleSpatializationResultsBlob() {}

	KyUInt32 GetOutsideNavmeshCount() const;
	KyUInt32 GetOutsideNavmeshCount(KyUInt32 databaseIndex) const;

public:
	KyUInt32 m_visualDebugId;
	BlobArray<SpatializationResultBlob> m_spatializationResultBlobs;
};

inline void SwapEndianness(Endianness::Target e, BoxObstacleSpatializationResultsBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_spatializationResultBlobs);
}

class BoxObstacleSpatializationResultsBlobBuilder : public BaseBlobBuilder<BoxObstacleSpatializationResultsBlob>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoxObstacleSpatializationResultsBlobBuilder(const BoxObstacle* boxObstacle, KyUInt32 visualDebugId)
		: m_visualDebugId(visualDebugId), m_boxObstacle(boxObstacle) {}
	virtual void DoBuild();

private:
	KyUInt32 m_visualDebugId;
	const BoxObstacle* m_boxObstacle;
};


//////////////////////////////////////////////////////////////////////////
//
// BoxObstacleBlob
//
//////////////////////////////////////////////////////////////////////////
class BoxObstacleBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Runtime, BoxObstacleBlob, 0)
public:
	BoxObstacleBlob() {}

	void GetWorldPosition(Vec3f& worldPosition) const { m_worldTransform.GetRootCoordinates(m_localCenter, worldPosition); }
	void GetWorldAABB(Box3f& worldAABB) const;

	Vec3f m_localCenter;
	Vec3f m_localHalfExtents;
	Transform m_worldTransform;

	Vec3f     m_linearVelocity;
	Vec3f     m_angularVelocity;
	KyUInt32  m_rotationMode;
	
	KyUInt32 m_tagVolumeIntegrationStatus;
	KyUInt32 m_visualDebugId;
};

inline void SwapEndianness(Endianness::Target e, BoxObstacleBlob& self)
{
	SwapEndianness(e, self.m_localCenter);
	SwapEndianness(e, self.m_localHalfExtents);
	SwapEndianness(e, self.m_worldTransform);
	SwapEndianness(e, self.m_linearVelocity);
	SwapEndianness(e, self.m_angularVelocity);
	SwapEndianness(e, self.m_rotationMode);
	SwapEndianness(e, self.m_tagVolumeIntegrationStatus);
	SwapEndianness(e, self.m_visualDebugId);
}

class BoxObstacleBlobBuilder : public BaseBlobBuilder<BoxObstacleBlob>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoxObstacleBlobBuilder(const BoxObstacle* boxObstacle) : m_boxObstacle(boxObstacle)
	{}

private:
	virtual void DoBuild();

	const BoxObstacle* m_boxObstacle;
};


//////////////////////////////////////////////////////////////////////////
//
// BoxObstaclesCollectionBlob and BoxObstaclesCollectionBlobBuilder
//
//////////////////////////////////////////////////////////////////////////

/* A type of blob that serializes data for several BoxObstacleBlobs that are managed through the BoxObstaclesCollectionBlobBuilder class.
		*/
class BoxObstaclesCollectionBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Runtime, BoxObstaclesCollectionBlob, 0)
public:
	BlobArray<BoxObstacleBlob> m_boxObstacles;
};
inline void SwapEndianness(Endianness::Target e, BoxObstaclesCollectionBlob& self)
{
	SwapEndianness(e, self.m_boxObstacles);
}

/// A type of blob builder that creates a BoxObstaclesCollectionBlob from a list of box BoxObstacles (Kaim::BoxObstacle).
/// You can use this class to serialize the obstacles to and from a file on disk.
class BoxObstaclesCollectionBlobBuilder : public BaseBlobBuilder<BoxObstaclesCollectionBlob>
{
public:
	/* \param boxObstacles			The List of BoxObstacles serialized by this blob builder. */
	BoxObstaclesCollectionBlobBuilder(const TrackedCollection<Ptr<BoxObstacle>, MemStat_BoxObstacle>& boxObstacles)
		: m_boxObstacles(&boxObstacles)
	{}

	virtual void DoBuild();

private:
	const TrackedCollection<Ptr<BoxObstacle>, MemStat_BoxObstacle>* m_boxObstacles;
};


} // namespace Kaim

#endif // Navigation_BoxObstacleBlob_H
