/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorNavDataElement_H
#define GwNavGen_GeneratorNavDataElement_H

#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavgeneration/common/densegrid.h"
#include "gwnavgeneration/input/inputcellinfo.h"
#include "gwnavruntime/navmesh/blobs/navmeshelementblob.h"
#include "gwnavgeneration/generator/generatorguidcompound.h"
#include "gwnavruntime/navmesh/blobs/navcellblob.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavgeneration/input/inputtilecontributionatpos.h"


namespace Kaim
{

class GeneratorSystem;
class CellGenerator;
class NavMeshElementBlob;
class GeneratorSectorBuilder;



// GeneratorNavDataElement corresponds either to a "pure" NavMesh or to an "overlap" NavMesh
// GeneratorSector (1 GUID)                                                   <->    NavData
// +- 1 mandatory pure    (GuidCompound =   1 GUID) GeneratorNavDataElement   <->    +- 1 mandatory pure    (GuidCompound =   1 GUID) NavMeshElementBlob
// +- N optional  overlap (GuidCompound = X>1 GUID) GeneratorNavDataElement   <->    +- N optional  overlap (GuidCompound = X>1 GUID) NavMeshElementBlob
class GeneratorNavDataElement
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)

public:
	GeneratorNavDataElement(GeneratorSystem* sys)
		: m_sys(sys)
		, m_mainSectorBuilder(KY_NULL)
		, m_emptyNavCellMode(KEEP_EMPTY_NAVCELL)
	{}
	
	// if InputTiling is enabled this function will delete inputCellInfoArrayAtPos adn set it to NULL
	void SetContributingInputCellInfoArrayAtPos(const Ptr<InputCellInfoArrayAtPos>& inputCellInfoArrayAtPos);

	KyResult GenerateRawNavCells(InputTileCache& inputTileCache);

	// Seed or Small Surface Filtering
	KyResult FilterNavCells();

	// Replace some NavCellBlobs in m_navCellHandlers
	// Uses m_navMeshElementHandler temporarily
	KyResult IntegrateTagVolumes();

	// KEEP_EMPTY_NAVCELL is useful to keep 1to1 mapping with m_outputNavCells
	// DISCARD_EMPTY_NAVCELL is useful when building the final NavData blobs
	enum EmptyNavCellMode { KEEP_EMPTY_NAVCELL, DISCARD_EMPTY_NAVCELL};

	// Build m_navMeshElementHandler from m_navCellHandlers
	KyResult BuildNavMeshElementBlob(EmptyNavCellMode emptyNavCellMode);

	// Helper getter
	NavMeshElementBlob* GetNavMeshElementBlob() { return m_navMeshElementHandler ? m_navMeshElementHandler->Blob() : KY_NULL; }

	// Used in IntegrateTagVolumes and FilterNavCells
	void ReplaceNavCell(KyUInt32 navCellIndex, Ptr<BlobHandler<NavCellBlob> > navCellHandler);

	// Clear m_navCellHandlers
	// Called when the final (filtered) m_navMeshElementHandler has been initialized
	void ClearNavCellHandlers();

	// Used in GeneratorNavDataElementMap (that uses a binary tree internally)
	// Compares m_guidCompound
	bool operator<(const GeneratorNavDataElement& other);

private:
	KyResult ProcessCellGenerators(KyArray<CellGenerator*>& cellGenerators);
	KyResult GenerateRawNavCells_NoTiling();
	KyResult GenerateRawNavCells_WithTiling(InputTileCache& inputTileCache);

public:
	GeneratorSystem* m_sys;
	GeneratorGuidCompound m_guidCompound;
	GeneratorSectorBuilder* m_mainSectorBuilder;

	// -------------- inputs fed by AddInputCellInfosAtPos --------------
	KyArray<Ptr<InputCellInfoArrayAtPos> > m_inputCellInfoArrayAtPosArray;  // if InputTiling is disabled
	DenseGridAndArrayOfRawPtr<InputTileContributionAtPos> m_inputTileGrid;  // if InputTiling is enabled

	// ----------------------- intermediate outputs -----------------------
	// m_navCellHandlers.GetCount() == m_navMeshElementHandler->GetCount() as long as
	// BuildNavMeshElementBlob(KEEP_EMPTY_NAVCELL) was called
	// Even if NavCell is empty, we garantee that all m_navCellHandlers[i].m_navCellHandler != KY_NULL && m_navCellHandlers[i].m_navCellHandler->Blob() != KY_NULL
	KyArray<Ptr<BlobHandler<NavCellBlob> > > m_navCellHandlers;
	EmptyNavCellMode m_emptyNavCellMode;

	// ---- final outputs: NavCellBlobs aggregated in NavMeshElementBlobs -----
	// m_navMeshElementHandler is also used temporarily in FilterNavCells() and IntegrateTagVolumes()
	Ptr<BlobHandler<NavMeshElementBlob> > m_navMeshElementHandler;
};


}


#endif

