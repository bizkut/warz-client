/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: LASI (NavCellCleaner_BlobBuilder)
#ifndef GwNavGen_DynamicNavCellBlobBuilder_H
#define GwNavGen_DynamicNavCellBlobBuilder_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavgeneration/generator/generatornavdatafilter.h"

namespace Kaim
{

class NavCellBlob;

// The purpose of this "special" BlobBuilder is to build a new NavCellBlob using an original NavCellBlob
// and dynamicVersionOf NavFloorBlob
class NavCellDynamic_BlobBuilder : public BaseBlobBuilder<NavCellBlob>
{
public:
	NavCellDynamic_BlobBuilder(const NavCellBlob& originalNavCell, KyArray<Ptr<BaseBlobHandler> >& blobHandlerOfDynamicNavFloors)
	{
		m_originalNavCell = &originalNavCell; 
		m_blobHandlerOfDynamicNavFloors = &blobHandlerOfDynamicNavFloors; 
	}

private:
	virtual void DoBuild();

	const NavCellBlob* m_originalNavCell;
	KyArray<Ptr<BaseBlobHandler> >* m_blobHandlerOfDynamicNavFloors;
};

} // namespace Kaim

#endif // GwNavGen_DynamicNavCellBlobBuilder_H
