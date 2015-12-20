/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: none
#ifndef Navigation_SpatializedCylinderBlob_H
#define Navigation_SpatializedCylinderBlob_H

#include "gwnavruntime/spatialization/spatializedcylinder.h"
#include "gwnavruntime/spatialization/spatializedpoint.h"
#include "gwnavruntime/spatialization/spatializedpointblob.h"
#include "gwnavruntime/world/runtimeblobcategory.h"


namespace Kaim
{

//////////////////////////////////////////////////////////////////////////
// SpatializedCylinderBlob
class SpatializedCylinderBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Runtime, SpatializedCylinderBlob, 0)

public:
	KyUInt32 m_visualDebugId;
	Vec3f m_position;
	Vec3f m_velocity;
};

inline void SwapEndianness(Endianness::Target e, SpatializedCylinderBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_position);
	SwapEndianness(e, self.m_velocity);
}

// SpatializedCylinderBlobBuilder
class SpatializedCylinderBlobBuilder : public BaseBlobBuilder<SpatializedCylinderBlob>
{
public:
	SpatializedCylinderBlobBuilder(KyUInt32 visualDebugId, const SpatializedCylinder* spatializedCylinder)
		: m_visualDebugId(visualDebugId), m_spatializedCylinder(spatializedCylinder)
	{}

private:
	virtual void DoBuild();

	KyUInt32 m_visualDebugId;
	const SpatializedCylinder* m_spatializedCylinder;
};



//////////////////////////////////////////////////////////////////////////
// SpatializedCylinderConfigBlob
class SpatializedCylinderConfigBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Runtime, SpatializedCylinderConfigBlob, 0)

public:
	KyUInt32 m_visualDebugId;
	KyFloat32 m_height;
	KyFloat32 m_radius;
};

inline void SwapEndianness(Endianness::Target e, SpatializedCylinderConfigBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_radius);
	SwapEndianness(e, self.m_height);
}

// SpatializedCylinderConfigBlobBuilder
class SpatializedCylinderConfigBlobBuilder : public BaseBlobBuilder<SpatializedCylinderConfigBlob>
{
public:
	SpatializedCylinderConfigBlobBuilder(KyUInt32 visualDebugId, const SpatializedCylinder* spatializedCylinder)
		: m_visualDebugId(visualDebugId), m_spatializedCylinder(spatializedCylinder)
	{}

private:
	virtual void DoBuild();

	KyUInt32 m_visualDebugId;
	const SpatializedCylinder* m_spatializedCylinder;
};

} // namespace Kaim

#endif // Navigation_SpatializedCylinderBlob_H
