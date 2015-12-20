/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_InputTileInfoGrid_H
#define GwNavGen_InputTileInfoGrid_H

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavgeneration/input/inputtileblob.h"
#include "gwnavgeneration/common/densegrid.h"
#include "gwnavgeneration/generator/generatorguidcompound.h"


namespace Kaim
{

class GeneratorSystem;
class GeneratorSectorBuilder;
class CellGenerator;

typedef Vec2i TilePos;

class InputTileContributionAtPos
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
	friend class InputTileCache;

public:
	class InputTileInfo
	{
		KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
	public:
		InputTileInfo() : m_sectorBuilder(KY_NULL), m_inputTileBlob(KY_NULL) {} // To make KyArray happy
		InputTileInfo(GeneratorSectorBuilder* sectorBuilder) : m_sectorBuilder(sectorBuilder), m_inputTileBlob(KY_NULL) {}
		InputCellBlob* GetInputCellBlob(const CellPos& cellPos) const
		{
			KY_ASSERT(m_inputTileBlob != KY_NULL);
			return m_inputTileBlob->GetInputCellBlob(cellPos);
		}
	public:
		GeneratorSectorBuilder* m_sectorBuilder;
		InputTileBlob* m_inputTileBlob;
	};

public:
	InputTileContributionAtPos(KyInt32 x, KyInt32 y) // counter intuitive constructor to match the DynamicGrid concept
		: m_tilePos(x, y) {}

	void Init(KyUInt32 inputTileSizeInNbCells);

	void AddCellContribution(const CellPos& cellPos, GeneratorSectorBuilder* sectorBuilder);

	const TilePos& GetTilePos()     const { return m_tilePos; }
	const CellBox& GetTileCellBox() const { return m_tileCellBox; }

	// Must be called after InputTileCache::Load(inputTileContributionAtPos) was called
	void GetInputCellBlobsAtCellPos(const CellPos& cellPos, KyArrayPOD<const InputCellBlob*>& inputCellBlob);
	KyUInt32 GetInputCellBlobsCountAtCellPos(const CellPos& cellPos);

	KyUInt32 GetContributingCellCount();

	void ClearInputTileBlobs();

private:
	TilePos m_tilePos;
	CellBox m_tileCellBox;
	KyArray<InputTileInfo> m_inputTileInfos;
	KyArray<KyArray<GeneratorSectorBuilder*> > m_contributingSectorRowMajorMatrix; // Matrix in m_tileCellBox
};


class InputTileCacheEntry
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)

public:
	InputTileCacheEntry() : m_sectorBuilder(KY_NULL) {}

	bool operator==(const InputTileCacheEntry& other)
	{
		return m_sectorBuilder == other.m_sectorBuilder
			&& m_tilePos       == other.m_tilePos;
	}

public:
	GeneratorSectorBuilder* m_sectorBuilder;
	TilePos m_tilePos;
	Ptr<BlobHandler<InputTileBlob> > m_inputTileBlobHandler;
};


// InputTileCache allows to load InputTile from files.
// InputTileCache works as a least recently used cache for loaded InputTiles.
// InputTileCache is created on the stack at the beginning of GenerateSectors()
class InputTileCache
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)

public:
	InputTileCache(GeneratorSystem* sys, KyUInt32 memoryLimitInMegaBytes = 100)
		: m_sys(sys)
		, m_memoryLimitInMegaBytes(memoryLimitInMegaBytes)
		, m_currentMemoryUsageInMegaBytes(0)
	{}

	KyResult Load(InputTileContributionAtPos& inputTileContributionAtPos);

	void UnloadAll();

private:
	InputTileBlob* Load(GeneratorSectorBuilder* sectorBuilder, const TilePos& tilePos);
	
	//void UnloadLeastRecentlyUsed();
	//void Unload(InputTileInfo& inputTileInfo);

public:
	GeneratorSystem* m_sys;
	KyUInt32 m_memoryLimitInMegaBytes;
	KyUInt32 m_currentMemoryUsageInMegaBytes;

	KyArray<InputTileCacheEntry> m_entries;
};


}


#endif

