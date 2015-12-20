/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_InputTileBlob_H
#define GwNavGen_InputTileBlob_H

#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavgeneration/input/inputcellblob.h"

namespace Kaim
{

typedef Vec2i TilePos;

// tilePos and array of InputCellBlob
class InputTileBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
	KY_ROOT_BLOB_CLASS(Generator, InputTileBlob, 0)
	KY_CLASS_WITHOUT_COPY(InputTileBlob)

public:
	InputTileBlob() {}

	InputCellBlob* GetInputCellBlob(const CellPos& cellPos)
	{
		KyUInt32 index = m_rowMajorMatrix.GetValues()[m_tileCellBox.GetRowMajorIndex(cellPos)];
		return &m_inputCellBlobs.GetValues()[index];
	}

public:
	TilePos m_tilePos;
	CellBox m_tileCellBox;
	BlobArray<KyUInt32> m_rowMajorMatrix;
	BlobArray<InputCellBlob> m_inputCellBlobs;
};

inline void SwapEndianness(Endianness::Target e, InputTileBlob& self)
{
	SwapEndianness(e, self.m_tilePos);
	SwapEndianness(e, self.m_tileCellBox);
	SwapEndianness(e, self.m_rowMajorMatrix);
	SwapEndianness(e, self.m_inputCellBlobs);
}

}


#endif

