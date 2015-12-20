/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: MAMU
#ifndef GwNavGen_ObjVisualRepresentation_H
#define GwNavGen_ObjVisualRepresentation_H

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"
#include "gwnavruntime/basesystem/coordsystem.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"

namespace Kaim
{

class ObjFileReader;

/*  VisualRepresentation of all triangles of an obj file.
	The color of the triangles is set depending on the associated material according to the given color palette.
	if materialIndex != KyUInt32MAXVAL, only the passed materialIndex is represented. */
class ObjVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	ObjVisualGeometryBuilder(
			const ObjFileReader* obj,
			const CoordSystem& coordSystem,
			PolygonWinding triangleWinding = POLYGON_IS_CCW,
			KyUInt32 representedMaterialIndex = KyUInt32MAXVAL);

	virtual void DoBuild();

private:
	const ObjFileReader* m_objFileReader;
	CoordSystem m_coordSystem;
	PolygonWinding m_triangleWinding;
	KyUInt32 m_representedMaterialIndex;
};


}


#endif

