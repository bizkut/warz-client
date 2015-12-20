/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_InputCellInfoGrid_H
#define GwNavGen_InputCellInfoGrid_H


#include "gwnavgeneration/common/densegrid.h"
#include "gwnavgeneration/input/inputcellblob.h"
#include "gwnavgeneration/input/inputcellinfo.h"


namespace Kaim
{

class GeneratorSystem;


class InputCellInfoGrid
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)

public:
	InputCellInfoGrid() {}

	void Add(GeneratorSectorBuilder* sectorBuilder, const CellPos& cellPos, Ptr<BlobHandler<InputCellBlob> > inputCellHandler = KY_NULL)
	{
		InputCellInfoArrayAtPos* inputCellInfoArrayAtPos = m_grid.GetOrCreate(cellPos.x, cellPos.y);

		InputCellInfo inputCellInfo(sectorBuilder, cellPos, inputCellHandler);

		KY_ASSERT(inputCellInfoArrayAtPos->DoesContain(inputCellInfo) == false);
		inputCellInfoArrayAtPos->PushBack(inputCellInfo);
	}

	InputCellInfoArrayAtPos* GetArrayAtPos(const CellPos& pos)       { return m_grid.Get(pos.x, pos.y); }
	InputCellInfoArrayAtPos* GetArrayAtPos(CellCoord x, CellCoord y) { return m_grid.Get(x, y); }

	Ptr<InputCellInfoArrayAtPos>* GetArrayOfArray() { return m_grid.GetElements(); }
	KyUInt32 GetArrayOfArrayCount()                 { return m_grid.GetElementsCount(); }

public:
	DenseGridAndArrayOfRefPtr<InputCellInfoArrayAtPos> m_grid;
};


}


#endif

