/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// Primary contact: LASI - secondary contact: NOBODY
#ifndef GwNavGen_DynamicNavCellBuilder_H
#define GwNavGen_DynamicNavCellBuilder_H

#include "gwnavruntime/navmesh/celldesc.h"
#include "gwnavgeneration/containers/tlsarray.h"

namespace Kaim
{

class GeneratorSystem;
class BoundaryGraph;
class BoundaryPolygon;
class DynamicNavCell;
class DynamicNavCellHeightField;
class NavTag;

class DynamicNavCellBuilder
{

public:
	DynamicNavCellBuilder(GeneratorSystem* sys, const CellDesc& celldesc) : m_sys(sys), m_cellDesc(celldesc) {}

	KyResult BuildNavCellFromBoundaryPolygons(
		const BoundaryGraph& inputGraph, 
		DynamicNavCell& cell, 
		const KyArrayTLS_POD<KyUInt32>&connexIdxToNavTagIdx,
		const KyArrayTLS_POD<const NavTag*>& navtagArray, 
		DynamicNavCellHeightField* heightField);

public:
	GeneratorSystem* m_sys;
	CellDesc m_cellDesc;
};

}


#endif //GwNavGen_DynamicNavCellBuilder_H

