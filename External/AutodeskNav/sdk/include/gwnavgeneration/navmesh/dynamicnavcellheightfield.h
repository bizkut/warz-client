/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LASI - secondary contact: NONE
#ifndef GwNavGen_DynamicHeightFieldCell_H
#define GwNavGen_DynamicHeightFieldCell_H

#include "gwnavgeneration/navmesh/dynamicnavfloorheightfield.h"
#include "gwnavgeneration/common/generatormemory.h"

namespace Kaim
{

class NavRasterCell;
class GeneratorSystem;
class DynamicNavRasterCell;
class DynamicNavCell;

class DynamicPointGraphCell;

// -----------------------------------
// ----- DynamicHeightFieldCell -----
// -----------------------------------
class DynamicNavCellHeightField
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	DynamicNavCellHeightField(GeneratorSystem* sys) : m_sys(sys) {}
	~DynamicNavCellHeightField();

	KyResult ExtractHeightFieldFromDynamicNavRasterCell(const DynamicNavRasterCell* navRasterCell, KyUInt32 stride);
	// Can return KY_NULL if navFloorBlob was too small to be sampled and has no heightFieldAssociated
	DynamicNavFloorHeightField* GetNavFloorHeightFieldFromColor(KyUInt32 color)
	{
		if (color >= m_heightfieldFloors.GetCount() || m_heightfieldFloors[color] == KY_NULL)
		{
			return KY_NULL;
		}
		return m_heightfieldFloors[color];
	}

public: 

	GeneratorSystem* m_sys;
	KyArrayTLS_POD<DynamicNavFloorHeightField*> m_heightfieldFloors;
};

}


#endif //GwNavGen_DynamicNavFloor_H

