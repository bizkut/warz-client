/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphCellBlobBuilder_H
#define Navigation_AbstractGraphCellBlobBuilder_H

#include "gwnavruntime/abstractgraph/blobs/abstractgraphcellblob.h"
#include "gwnavruntime/abstractgraph/blobs/abstractgraphfloorblob.h"
#include "gwnavruntime/blob/baseblobbuilder.h"


namespace Kaim
{

class NavCell;

class AbstractGraphCellBlobBuilder : public BaseBlobBuilder<AbstractGraphCellBlob>
{
public:
	AbstractGraphCellBlobBuilder(const NavCell* navCell, const KyArray< Ptr< BlobHandler<AbstractGraphFloorBlob> >, MemStat_NavDataGen >* graphFloorBlobHandlers, KyUInt32 graphFloorCount)
		: m_navCell(navCell)
		, m_graphFloorBlobHandlers(graphFloorBlobHandlers)
		, m_graphFloorCount(graphFloorCount)
	{}

private:
	virtual void DoBuild();

private:
	const NavCell* m_navCell;
	const KyArray< Ptr< BlobHandler<AbstractGraphFloorBlob> >, MemStat_NavDataGen >* m_graphFloorBlobHandlers;
	KyUInt32 m_graphFloorCount;
};



}


#endif
