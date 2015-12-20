/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// Primary contact: BRGR - secondary contact: NOBODY
#ifndef Navigation_AbstractGraphVisualGeometryBuilder_H
#define Navigation_AbstractGraphVisualGeometryBuilder_H


#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"


namespace Kaim
{

class AbstractGraph;

/* This class defines the way the data in a AbstractGraph should be rendered. */
class AbstractGraphVisualGeometryBuilder : public IVisualGeometryBuilder
{

public:
	explicit AbstractGraphVisualGeometryBuilder(AbstractGraph* abstractGraph) 
		: IVisualGeometryBuilder(), m_abstractGraph(abstractGraph) {}

	virtual void DoBuild();

public:
	AbstractGraph* m_abstractGraph;
};

}


#endif

