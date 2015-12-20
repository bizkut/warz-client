/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// Primary contact: JUBE - secondary contact: NOBODY
#ifndef Navigation_NavGraphVisualRepresentation_H
#define Navigation_NavGraphVisualRepresentation_H


#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"

namespace Kaim
{

class NavGraph;
class NavGraphBlob;
class VisualShapeColor;
class Vec3f;

/* This class of VisualRepresentation defines the way the data in a NavGraph
	should be rendered using triangles. */
class NavGraphVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	NavGraphVisualGeometryBuilder(NavGraph* navGraph) : m_navGraph(navGraph) {}

	virtual void DoBuild();
public:
	NavGraph* m_navGraph;
};

/* This class of VisualRepresentation defines the way the data in a NavGraphBlob
	should be rendered using triangles. */
class NavGraphBlobVisualGeometryBuilder : public IVisualGeometryBuilder
{

public:
	NavGraphBlobVisualGeometryBuilder(const NavGraphBlob* navGraphBlob) : m_navGraphBlob(navGraphBlob) {}

	virtual void DoBuild();
private:
	void DrawVertex(const Vec3f& pos, KyFloat32 halfWidth, const VisualShapeColor& color);
	void DrawEdge(bool bidirectional, const Vec3f& edgeStart, const Vec3f& edgeEnd);
public:
	const NavGraphBlob* m_navGraphBlob;
};


}

#endif

