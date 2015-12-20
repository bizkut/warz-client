/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: BRGR
#ifndef Navigation_CylinderObstacleBlob_H
#define Navigation_CylinderObstacleBlob_H

#include "gwnavruntime/world/cylinderobstacle.h"
#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/world/worldelement.h"
#include "gwnavruntime/world/databasebindingblob.h"
#include "gwnavruntime/spatialization/spatializedcylinderblob.h"


namespace Kaim
{

class CylinderObstacleBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_CylinderObstacle)
	KY_ROOT_BLOB_CLASS(Runtime, CylinderObstacleBlob, 0)

public:
	KyUInt32 m_tagVolumeIntegrationStatus;
	KyUInt32 m_visualDebugId;
	DatabaseBindingBlob m_databaseBinding;
	KyUInt32 m_triggerTagVolume;
};

inline void SwapEndianness(Endianness::Target e, CylinderObstacleBlob& self)
{
	SwapEndianness(e, self.m_tagVolumeIntegrationStatus);
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_databaseBinding);
	SwapEndianness(e, self.m_triggerTagVolume);
}

class CylinderObstacleBlobBuilder : public BaseBlobBuilder<CylinderObstacleBlob>
{
public:
	CylinderObstacleBlobBuilder(const CylinderObstacle* cylinderObstacle)
		: m_cylinderObstacle(cylinderObstacle)
	{}

private:
	virtual void DoBuild();

	const CylinderObstacle* m_cylinderObstacle;
};

} // namespace Kaim

#endif // Navigation_CylinderObstacleBlob_H
