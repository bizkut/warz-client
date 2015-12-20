/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_NavGraph_DisplayListBuilder_H
#define Navigation_NavGraph_DisplayListBuilder_H

#include "gwnavruntime/visualsystem/visualcolor.h"
#include "gwnavruntime/navgraph/navgraphtypes.h"

namespace Kaim
{

class ScopedDisplayList;
class NavGraphArray;
class NavGraphBlob;

class NavGraphDisplayListBuilder
{
public:
	NavGraphDisplayListBuilder()
	{
		m_displayEdgesAsLines = false;
		m_displayEdgesAsArrows = true;
		m_displayEdgesAsCurves = false;

		m_vertexRadius = 0.05f;
		m_edgeHalfWidth = 0.1f;
		m_edgeColor = VisualColor(0, 225, 255, 128);

		m_vertexColor_whenLinkTypeIs_NoLink = VisualColor::Gray;
		m_vertexColor_whenLinkTypeIs_LinkToNavMesh = VisualColor::LightGreen;
	}

	void DisplayNavGraphArray(ScopedDisplayList* displayList, const NavGraphArray* navGraphArray);
	void DisplayNavGraph(ScopedDisplayList* displayList, const NavGraphBlob* navGraphBlob);
	void DisplayNavGraphName(ScopedDisplayList* displayList, const NavGraphBlob* navGraphBlob);

public: // Internal

	VisualColor GetVertexColorFromVertexInfo(NavGraphVertexLinkType vertexType);

public:
	bool m_displayEdgesAsLines;
	bool m_displayEdgesAsArrows;
	bool m_displayEdgesAsCurves;

	KyFloat32 m_vertexRadius;
	KyFloat32 m_edgeHalfWidth;
	VisualColor m_edgeColor;

	VisualColor m_vertexColor_whenLinkTypeIs_NoLink;
	VisualColor m_vertexColor_whenLinkTypeIs_LinkToNavMesh;
};

}

#endif

