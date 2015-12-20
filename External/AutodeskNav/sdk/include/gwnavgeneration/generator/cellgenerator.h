/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_CellGenerator_H
#define GwNavGen_CellGenerator_H


#include "gwnavruntime/navmesh/celldesc.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/navmesh/blobs/navcellblob.h"
#include "gwnavgeneration/generator/cellgeneratorprofiling.h"
#include "gwnavgeneration/generator/generatorguidcompound.h"
#include "gwnavgeneration/generator/navcellgenerator.h"
#include "gwnavgeneration/generator/generatorsystem.h"


namespace Kaim
{

class GeneratorSystem;
class GeneratorNavDataElement;
class NavCellBlob;

// Gathers inputs (array of InputCellBlob) and outputs (NavCellBlob) that are involved in the ("non-filtered" aka "Raw") NavCell Generation.
// CellGenerator are created just before calling the Generator parallel_for, and are discarded just after.
class CellGenerator
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)

public:
	CellGenerator() : m_sys(KY_NULL), m_navCellHandler(KY_NULL) {}

	void Init(GeneratorSystem* sys, const CellPos& cellPos, BlobHandler<NavCellBlob>* navCellHandler);

	void ReserveInputCellArray(KyUInt32 count) { m_inputCells.Reserve(count); }

	void AddInputCell(const InputCellBlob* inputCellBlob) { m_inputCells.PushBack(inputCellBlob); }

	// GenerateRawNavCell() is the function in m_sys->m_parallelForInterface->ParallelFor()
	KyResult GenerateRawNavCell();

	const CellDesc& Desc() const { return m_cellDesc; }
	GeneratorSystem* Sys()       { return m_sys; }
	CellGeneratorProfiling& CellProfiling() { return m_cellProfiling; }

public:
	GeneratorSystem* m_sys;
	CellDesc m_cellDesc;
	KyArrayPOD<const InputCellBlob*> m_inputCells; // input
	BlobHandler<NavCellBlob>* m_navCellHandler;    // output
	CellGeneratorProfiling m_cellProfiling;
};


}


#endif
