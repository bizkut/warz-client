/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorNavDataFilter_H
#define GwNavGen_GeneratorNavDataFilter_H


#include "gwnavruntime/navmesh/blobs/navmeshelementblob.h"
#include "gwnavgeneration/generator/generatorguidcompound.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_Types.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/containers/bitfield.h"


namespace Kaim
{

class GeneratorNavDataElement;
class NavMeshElementManager;
class NavMeshElement;
class Database;
class DynamicNavFloor;
class NavFloor;
// GeneratorNavDataFilter is where we detect connected components and we discard parts of the NavMesh (aka NavDataElement) if...
// - part is not reachable from a seedpoint with PerformFilteringFromSeed()
// - part surface is less than a threshold  with PerformFilteringFromSurfaces()
class GeneratorNavDataFilter
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	GeneratorNavDataFilter(GeneratorNavDataElement& navDataElement);

	KyResult PerformFilteringFromSeed(KyArray<Vec3f>& seedpoints);

	KyResult PerformFilteringFromSurfaces(const KyFloat32 minSurfaceOfConnectedComponent);

private:
	friend class GeneratorConnectComponentTraversal;
	friend class NavCellCleaner_BlobBuilder;

	enum ConnectedComponentStatus
	{
		ConnectedComponentStatus_NOSEED,
		ConnectedComponentStatus_SEED
	};

	struct ConnectedComponentData
	{
		KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	public:
		ConnectedComponentData() : m_status(ConnectedComponentStatus_NOSEED), m_surface(0.0f) {}
		explicit ConnectedComponentData(KyFloat32 surfaceInit) : m_status(ConnectedComponentStatus_NOSEED), m_surface(surfaceInit) {}

		ConnectedComponentStatus m_status;
		KyFloat32 m_surface;
	};

	KyUInt32 ComputeConnectedComponent(NavMeshElement& navMeshElement);

	void FilterConnectedComponentFromSeedPoints(const KyArray<Vec3f>& seedpoints);

	void FilterConnectedComponentFromOtherNavMeshInvolvedInOverlap(NavMeshElement& navMeshElement);
	void AddAllInvolvedNavMeshInOverlapToNavMeshElementManager(KyArrayPOD<NavMeshElement*>& addedNavMeshs);
	void RemoveAllInvolvedNavMeshInOverlapFromNavMeshElementManager(KyArrayPOD<NavMeshElement*>& addedNavMeshs);

	void ComputeConnectedComponentSurface(NavMeshElement& navMeshElement); 

	KyResult CleanNavMeshElement(NavMeshElement& navMeshElement);
	void BuildNewNavCell(NavMeshElement& navMeshElement, KyUInt32 navCellIdx, KyArray<BitField>& connexFilterStatus);
	void BuildNewFloor(DynamicNavFloor& floor, NavFloor* navFloor, BitField& connexStatus);

	void ResizeConnectedComponentDataTableWithDefaultValue(KyUInt32 size, KyFloat32 defaultValue);

public:
	GeneratorNavDataElement* m_navDataElement;
	Ptr<World> m_world;
	Database* m_database;
	NavMeshElementManager* m_navMeshElementManager;
	KyArray<ConnectedComponentData> m_connectedComponents;
};


}


#endif

