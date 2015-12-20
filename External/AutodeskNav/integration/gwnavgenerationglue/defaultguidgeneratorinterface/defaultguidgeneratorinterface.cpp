/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#include <ctype.h>
#include <cassert>
#include "defaultguidgeneratorinterface.h"
#include "gwnavruntime/base/kyguid.h"

#ifdef KY_OS_WIN32

//#ifdef WIN32_LEAN_AND_MEAN
//#undef WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ObjBase.h>

typedef GUID KY_OSGUID;

#elif defined(KY_OS_LINUX) || defined(KY_OS_MAC)

#include <uuid/uuid.h> // LGPL

typedef uuid_t KY_OSGUID;
#endif

namespace KyGlue
{

Kaim::KyGuid DefaultGuidGeneratorInterface::GenerateGuid()
{
#ifdef KY_OS_WIN32
	KY_OSGUID osGuid;

	CoCreateGuid(&osGuid);
	return Kaim::KyGuid((void*)&osGuid);
#elif defined(KY_OS_LINUX) || defined(KY_OS_MAC)
	KY_OSGUID osGuid;

	uuid_generate(osGuid);
	return Kaim::KyGuid((void*)&osGuid);
#else
#error "Generation of Guid not implemented for this platform"
#endif
}

} // namespace KyGlue
