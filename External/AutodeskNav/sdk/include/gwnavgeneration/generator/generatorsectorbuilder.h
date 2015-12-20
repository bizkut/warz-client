/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorSectorGenerator_H
#define GwNavGen_GeneratorSectorGenerator_H


#include "gwnavgeneration/generator/generatorsystem.h"
#include "gwnavgeneration/generator/generatorsector.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavruntime/navmesh/blobs/navmeshelementblob.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/blob/blobaggregate.h"
#include "gwnavgeneration/input/clientinputconsumer.h"
#include "gwnavgeneration/generator/generatorguidcompound.h"
#include "gwnavgeneration/generator/navdataelementglobalmap.h"
#include "gwnavruntime/navgraph/blobs/navgrapharray.h"
#include "gwnavruntime/navdata/navdataindex.h"
#include "gwnavruntime/navdata/navdata.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "generatorsectorreport.h"

namespace Kaim
{

class NavCellGenerator;
class GeneratorSector;
class GeneratorNavDataElement;
class ClientInput;
class NavDataIndex;
class InputTileBlob;
class InputTileCache;
class SectorInputData;

/// Maintains generation data for each sector
class GeneratorSectorBuilder : public Kaim::RefCountBaseNTS<GeneratorSectorBuilder, MemStat_NavDataGen>
{
public: 
	// determines whether or not the sector is overlapping another one
	enum SectorOverlapStatus { SECTOR_IS_OVERLAPPING_ANOTHER_SECTOR = 0, SECTOR_IS_ISOLATED };

public:
	GeneratorSectorBuilder(GeneratorSystem* sys, Ptr<GeneratorSector> sector);

	~GeneratorSectorBuilder();

public:
	String GetTileDirName();
	String GetTileFileName(KyInt32 x, KyInt32 y);

	void ClearSectorInput();
	void ClearNavDataElements();

	KyResult ProduceSectorInput(GeneratorInputProducer* producer);

	GeneratorNavDataElement* GetOrCreateNavDataElement(const GeneratorGuidCompound& guidCompound);

	KyResult GenerateRawNavCells(InputTileCache& inputTileCache);

	KyResult FilterCells();

	// Compute m_sector->m_navData from NavDataElements and m_sector->m_colData
	KyResult MakeOutputBlobAggregates();

	const GeneratorNavDataElementMap& GetNavDataElements() const { return m_navDataElements; }

	const KyGuid& GetGuid() const { return m_sector->GetGuid(); }

	const char* GetName() const { return m_sector->GetName(); }

	void ComputeSectorBBox(Box2f& box);

	template<class OSTREAM> void LogConfiguration(OSTREAM& os);

	void ComputeStatistics();

private:
	KyResult BuildNavMeshElementBlobs();
	KyResult MakeNavData();
	KyResult MakeColData();
	void Init();
	void ComputeGenerationFlags();
	KyResult SaveNavData(const String& navDataName, BlobAggregate* navData);
	KyResult SaveColData(const String& colDataName, BlobAggregate* colData);
	void ClearNavCellHandlers();
	void ComputeSectorCellBox(Box2i& cellBox);

public:
	GeneratorSystem* m_sys;
	Ptr<GeneratorSector> m_sector;

	// result of the input production
	SectorInputData*             m_sectorInput; // Triangles spatialized by cellPos
	bool                         m_sectorInputWasProduced;
	KyArray<CellPos>             m_currentPatchCellPosList;
	CellBox                      m_currentInputCellBox;
	KyArray<Vec3f>               m_currentSeedPoints;
	KyArray<TagVolumeInitConfig> m_currentTagVolumeInitConfigs;

	IndexedMeshBlobBuilder     m_inputMeshBuilder;
	KyArray<Ptr<IndexedMesh> > m_indexedMeshes;
	Ptr<HeightField>           m_heightfield;
	ClientInputConsumerStatistics m_clientInputConsumerStatistics;

	GeneratorNavDataElementMap m_navDataElements;

	bool m_doGenerateNavData;
	bool m_doGenerateColData;

	GeneratorSectorReport m_sectorStatistics;
};

template<class OSTREAM>
inline void GeneratorSectorBuilder::LogConfiguration(OSTREAM& os)
{
	os << "sectorName         : " << m_sector->GetName()                  << Endl;
	os << "sectorGuidString   : " << m_sector->GetGuidString().ToCStr()   << Endl;

	switch (m_sector->GetSectorChange())
	{
	case GenFlags::SECTOR_CHANGED  : os << "SectorChange       : " << "SECTOR_CHANGED"   << Endl; break;
	case GenFlags::SECTOR_NOCHANGE: os << "SectorChange       : " << "SECTOR_NOCHANGE" << Endl; break;
	case GenFlags::SECTOR_REMOVED  : os << "SectorChange       : " << "SECTOR_REMOVED"   << Endl; break;
	}

	os << "doGenerateNavData      : " << m_doGenerateNavData        << Endl;
	os << "doGenerateColData     : " << m_doGenerateColData     << Endl;
}

} // namespace Kaim

#endif

