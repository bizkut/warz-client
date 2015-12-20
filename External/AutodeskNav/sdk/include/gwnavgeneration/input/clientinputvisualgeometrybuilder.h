/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



#ifndef GwNavGen_ClientInputVisualGeometryBuilder_H
#define GwNavGen_ClientInputVisualGeometryBuilder_H

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"

namespace Kaim
{

class ClientInput;

class ClientInputVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	enum DrawSeedPointMode { DO_NOT_DRAW_SEEDPOINTS, DRAW_SEEDPOINTS };

public:
	ClientInputVisualGeometryBuilder(ClientInput* clientInput, DrawSeedPointMode drawSeedPoints = DO_NOT_DRAW_SEEDPOINTS);

	virtual void DoBuild();

public:
	ClientInput* m_clientInput;
	DrawSeedPointMode m_drawSeedPoints;
};


}

#endif // GwNavGen_ClientInputVisualGeometryBuilder_H
