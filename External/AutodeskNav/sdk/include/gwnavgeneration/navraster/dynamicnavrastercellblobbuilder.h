/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: GUAL
#ifndef GwNavGen_DynamicNavRasterCellBlobBuilder_H
#define GwNavGen_DynamicNavRasterCellBlobBuilder_H


#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavgeneration/navraster/navrastercell.h"


namespace Kaim
{

class DynamicNavRasterCell;


class DynamicNavRasterCellBlobBuilder : public BaseBlobBuilder<NavRasterCell>
{
public:
	DynamicNavRasterCellBlobBuilder(DynamicNavRasterCell* dynamicNavRasterCell) :
		m_dynamicNavRasterCell(dynamicNavRasterCell)
	{}

	virtual void DoBuild();

public:
	DynamicNavRasterCell* m_dynamicNavRasterCell;
};


}


#endif

