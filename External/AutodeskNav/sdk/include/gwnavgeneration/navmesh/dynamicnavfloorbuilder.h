/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LASI - secondary contact: NOBODY
#ifndef GwNavGen_DynamicNavFloorBuilder_H
#define GwNavGen_DynamicNavFloorBuilder_H

#include "gwnavgeneration/generator/generatorsystem.h"
#include "gwnavruntime/base/timeprofiler.h"
#include "gwnavgeneration/boundary/boundaryvertex.h"
#include "gwnavruntime/navmesh/celldesc.h"

#include "gwnavruntime/visualsystem/displaylist.h"

#include "gwnavgeneration/common/stllist.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavgeneration/containers/tlsarray.h"


namespace Kaim
{

class IndexedTriangleSoup2i;
class DynamicNavFloor;
class BoundaryPolygon;
class BoundaryGraph;
class BoundarySimplifiedPolygon;
class BoundarySimplifiedContour;
class DynamicNavFloorHeightField;
class DynamicTriangulation;
class DynamicNavCell;
class BoundaryVertex;
class BoundarySimplifiedEdge;
class NavTag;

class DynamicNavFloorBuilder
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DynamicNavFloorBuilder(GeneratorSystem* sys, const CellDesc& cellDesc) 
		: m_sys(sys), m_cellDesc(cellDesc), m_lastBuildDurationMs(0.0f), m_doProfile(false) 
	{
		m_displayLists.SetVisualDebugServer(m_sys->m_visualDebugServer);
	}

	KyResult BuildNavFloorFromBoundaryPolygons(const BoundarySimplifiedPolygon& polygon,
		DynamicNavFloorHeightField* heightField, DynamicNavFloor& floor,
		const KyArrayTLS_POD<KyUInt32>& connexIdxToNavTagIdx, const KyArrayTLS_POD<const NavTag*>& navtagArray);
	
	void ToggleProfiling(bool activate) {m_doProfile = activate;}
	KyFloat32 GetLastBuildDurationMs() { return m_lastBuildDurationMs; }

private:
	KyResult AddNavConnexToDynamicNavFloor(const DynamicTriangulation& triangulation, DynamicNavFloor& floor, KyArrayTLS_POD<BoundarySimplifiedEdge*>& edgesForEdges, KyUInt32 navTagIdx);
	KyResult MakeVerticallyAccurateTriangulation(DynamicTriangulation& triangulation, DynamicNavFloorHeightField* heightField);

	KyResult FixHalfEdgeTypeInDynamicNavFloor(DynamicNavFloor &dynaFloor, const KyArrayTLS_POD<BoundarySimplifiedEdge*>& edgesForEdges);

	void RenderDynamicTriangulationInVisualDebug(const DynamicTriangulation& triangulation);
	void RenderHeightFieldInVisualDebug(const DynamicNavFloorHeightField& heightField);
public:
	GeneratorSystem* m_sys;
	CellDesc m_cellDesc;
	KyFloat32 m_lastBuildDurationMs;
	bool m_doProfile;
	DisplayListManager m_displayLists;
};

} // namespace Kaim


#endif //GwNavGen_DynamicNavFloorBuilder_H

