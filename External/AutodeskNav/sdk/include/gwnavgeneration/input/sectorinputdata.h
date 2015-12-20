/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_SectorInputData_H
#define GwNavGen_SectorInputData_H


#include "gwnavgeneration/input/inputcellblob.h"
#include "gwnavgeneration/input/inputtileblob.h"


namespace Kaim
{

class GeneratorSystem;
class ClientInputConsumer;
class GeneratorSectorBuilder;


/// Relevant if m_doEnableLimitedMemoryMode is true, maintains the per InputTile information that will allows to load the
/// InputCellBlob from file only when it's necessary.
class InputTileImprint : public RefCountBase<InputTileImprint, MemStat_NavDataGen>
{
public:
	InputTileImprint() {} // To make KyArray happy

	InputTileImprint(GeneratorSectorBuilder* sectorBuilder, const TilePos& tilePos)
		: m_sectorBuilder(sectorBuilder), m_tilePos(tilePos) {}

public:
	GeneratorSectorBuilder* m_sectorBuilder;
	TilePos m_tilePos;
	KyArray<CellPos> m_cellPositions;
};


/// Maintains the InputCellBlobs spatialized per CellPos for 1 SectorInput.
/// If m_doEnableLimitedMemoryMode is true, SectorInputData maintains InputTileImprint instead of m_inputCellHandlers.
/// SectorInputData is the final result of the Input production/consumption 
class SectorInputData
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
	KY_CLASS_WITHOUT_COPY(SectorInputData)

public:
	SectorInputData() : m_sys(KY_NULL), m_sectorBuilder(KY_NULL) {}

	// called at the end GeneratorSectorBuilder::ProduceInputs() via inputConsumer.FinalizeSectorInput(m_sectorInput);
	KyResult Init(GeneratorSectorBuilder* sectorBuilder, ClientInputConsumer& inputConsumer);

private:
	KyResult Init_NoInputTiling(ClientInputConsumer& inputConsumer);
	KyResult Init_WithInputTiling(ClientInputConsumer& inputConsumer);
	void RegisterCellPos(const CellPos& cellPos);

public:
	GeneratorSystem* m_sys;
	GeneratorSectorBuilder* m_sectorBuilder;

	// No InputTiling => InputCellBlob are aggregated directly in SectorInput
	KyArray<Ptr<BlobHandler<InputCellBlob> > > m_inputCellHandlers;

	// InputTiling activated => InputCellBlob are saved to files
	// we only keep in memory the cellPos imprints of each inputTile
	KyArray<Ptr<InputTileImprint> > m_inputTileImprints;
};



}


#endif

