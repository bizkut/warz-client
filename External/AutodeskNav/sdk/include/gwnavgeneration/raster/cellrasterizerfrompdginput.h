/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_CellRasterizerFromPdgInput_H
#define GwNavGen_CellRasterizerFromPdgInput_H

#include "gwnavgeneration/raster/cellrasterizer.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/navmesh/celldesc.h"
#include "gwnavruntime/containers/kyarray.h"

namespace Kaim
{

class GeneratorSystem;
class InputCellBlob;

class CellRasterizerFromPdgInput : public CellRasterizer
{
public:
	CellRasterizerFromPdgInput(GeneratorSystem* sys, const KyArrayPOD<const InputCellBlob*>* inputCells, const CellDesc& cellDesc, DynamicRasterCell* dynamicRaster)
		: CellRasterizer(dynamicRaster)
	{
		m_sys = sys;
		m_inputCells = inputCells;
		m_pixelBox = cellDesc.m_enlargedPixelBox;
		m_hasMultipleNavTags = false;
	}

public: 
	bool HasMultipleNavTags() const { return m_hasMultipleNavTags; }

protected:
	virtual KyResult DoRasterize();

protected:
	GeneratorSystem* m_sys;
	const KyArrayPOD<const InputCellBlob*>* m_inputCells;
	PixelBox m_pixelBox; // = cellDesc.m_enlargedPixelBox
	bool m_hasMultipleNavTags; // useful information to speedup painting
};


}


#endif
