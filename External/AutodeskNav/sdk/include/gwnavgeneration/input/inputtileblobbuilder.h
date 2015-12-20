/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY

#ifndef GwNavGen_InputTileBlobBuilder_H
#define GwNavGen_InputTileBlobBuilder_H


#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavgeneration/input/inputtileblob.h"


namespace Kaim
{

class GeneratorSystem;
class DynamicInputCell;


class InputTileBlobBuilder_FromDynamicInputCells : public BaseBlobBuilder<InputTileBlob>
{
public:
	InputTileBlobBuilder_FromDynamicInputCells(
		GeneratorSystem* sys, const TilePos& tilePos, const KyArrayPOD<DynamicInputCell*>* cells);

private:
	virtual void DoBuild();

	GeneratorSystem* m_sys;
	TilePos m_tilePos;
	CellBox m_tileCellBox;
	const KyArrayPOD<DynamicInputCell*>* m_dynInputCells;
};

class PdgInputTileBlobBuilder_FromInputCellBlobs : public BaseBlobBuilder<InputTileBlob>
{
public:
	PdgInputTileBlobBuilder_FromInputCellBlobs(
		GeneratorSystem* sys, const TilePos& tilePos, const KyArray<Ptr<BlobHandler<InputCellBlob> > >* inputCellBlobHandlers);

private:
	virtual void DoBuild();

	GeneratorSystem* m_sys;
	TilePos m_tilePos;
	CellBox m_tileCellBox;
	const KyArray<Ptr<BlobHandler<InputCellBlob> > >* m_inputCellBlobHandlers;
};

}

#endif
