/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_WorldElementSpatializer_H
#define Navigation_WorldElementSpatializer_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim
{

class World;

// Internal: this class manages the spatialization of WorldElement instances in
// the World.
class WorldElementSpatializer
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
	KY_CLASS_WITHOUT_COPY(WorldElementSpatializer)

public:
	explicit WorldElementSpatializer(World* world): m_world(world) {}
	~WorldElementSpatializer() {}

	void UpdateSpatializations(KyFloat32 simulationTimeInSeconds);

private:
	void UpdateBotSpatializations(KyFloat32 simulationTimeInSeconds);
	void UpdatePointOfInterestSpatializations();
	void UpdateCylinderObstacleSpatializations();
	void UpdateBoxObstacleSpatializations();

	World* m_world;
};

} // namespace Kaim

#endif // Navigation_WorldElementSpatializer_H
