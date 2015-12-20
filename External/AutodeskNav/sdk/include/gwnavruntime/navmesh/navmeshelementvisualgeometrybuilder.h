/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: MAMU
#ifndef Navigation_NavMeshElementVisualGeometryBuilder_H
#define Navigation_NavMeshElementVisualGeometryBuilder_H

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"

namespace Kaim
{

class NavMeshElementBlob;

/* This class of VisualRepresentation defines the way the data in a NavMeshElementBlob
	should be rendered using triangles.
	A NavCellVisualRepresentation is used to build the geometry for each NavCellBlob
	in the NavMeshElementBlob, and the geometries of all NavCells are combined to build the geometry for the
	NavMeshElementBlob. */
class NavMeshElementVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	NavMeshElementVisualGeometryBuilder(const NavMeshElementBlob* Blob) : m_navMeshElementBlob(Blob) {}

	virtual void DoBuild();

public:
	const NavMeshElementBlob* m_navMeshElementBlob;
};


}


#endif

