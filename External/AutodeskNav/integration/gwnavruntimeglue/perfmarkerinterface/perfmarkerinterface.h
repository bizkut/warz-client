/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef KyRuntimeGlue_PerfMarkerInterface_H
#define KyRuntimeGlue_PerfMarkerInterface_H

#include "gwnavruntime/basesystem/iperfmarkerinterface.h"

/// Implementation of IPerfMarkerInterface that uses snTuner and PIX.
class PerfMarkerInterface : public Kaim::IPerfMarkerInterface
{
public:
	virtual void Begin(const char* name);
	virtual void End();
};

#endif