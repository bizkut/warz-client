/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavCellVisualGeometryBuilder_H
#define Navigation_NavCellVisualGeometryBuilder_H

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"

namespace Kaim
{

class NavCellBlob;

/* This class of VisualRepresentation defines the way a NavCellBlob should be rendered using triangles. */
class NavCellVisualGeometryBuilder : public IVisualGeometryBuilder
{
public :

public:
	NavCellVisualGeometryBuilder(const NavCellBlob* navCellBlob, KyInt32 cellSizeInPixel, KyFloat32 integerPrecision);

	virtual void DoBuild();

public:
	const NavCellBlob* m_navCellBlob;
	KyFloat32 m_integerPrecision;
	KyInt32 m_cellSizeInPixel;
	KyInt32 m_cellSizeInCoord;
};


}

#endif // #Navigation_NavCellVisualGeometryBuilder_H
