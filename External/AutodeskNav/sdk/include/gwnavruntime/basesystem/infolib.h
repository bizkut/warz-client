/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef INFOLIB_H
#define INFOLIB_H

#include "gwnavruntime/base/types.h"

namespace Kaim
{
/// Outputs the build mode, target platform, library name, version, and build date of this library.
/// The text is logged using debug messages.
/// \return true if the text was successfully logged, or false if the BaseSystem has not yet been
/// initialized.
bool ShowInfoAboutLib();

/// Retrieves a string that contains the version number of this library.
const char* GetNavigationVersion();

}

#endif // INFOLIB_H

