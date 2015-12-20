/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorTagVolumeIntegrator_H
#define GwNavGen_GeneratorTagVolumeIntegrator_H


#include "gwnavruntime/navmesh/blobs/navmeshelementblob.h"
#include "gwnavgeneration/generator/generatorguidcompound.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_Types.h"
#include "gwnavruntime/database/database.h"


namespace Kaim
{

class GeneratorNavDataElement;
class NavMeshElementManager;
class NavMeshElement;
class Database;
class TagVolume;

// GeneratorTagVolumeIntegrator is where we detect connected components and we discard parts of the NavMesh (aka NavDataElement) if...
// - part is not reachable from a seedpoint with PerformFilteringFromSeed()
// - part surface is less than a threshold  with PerformFilteringFromSurfaces()
class GeneratorTagVolumeIntegrator
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	GeneratorTagVolumeIntegrator(GeneratorNavDataElement& navDataElement);

	KyResult IntegrateTagVolumes();
private:
	void GatherAllTagVolumes(KyArray<Ptr<TagVolume> >& tagVolumes);
	KyResult ReplaceFloorBlobWithDynamicFloorBlob(NavMeshElement& navMeshElement);
public:
	GeneratorNavDataElement* m_navDataElement;
	Ptr<World> m_world;
	Database* m_database;
	NavMeshElementManager* m_navMeshElementManager;
};


}


#endif

