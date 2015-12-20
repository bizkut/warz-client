/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Integration_DefaultGuidGeneratorInterface_H
#define Integration_DefaultGuidGeneratorInterface_H

#include "gwnavruntime/base/kyguid.h"


namespace KyGlue
{

/// Generates GUIDs automatically on Windows, Mac OS X and Linux platforms by calling system functions.
class DefaultGuidGeneratorInterface
{
public:
	static Kaim::KyGuid GenerateGuid();
};

} // namespace KyGlue

#endif
