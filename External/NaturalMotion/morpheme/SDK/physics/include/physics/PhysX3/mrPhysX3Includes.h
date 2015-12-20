// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_PHYSXINCLUDES_H
#define NM_PHYSXINCLUDES_H

#include "mrPhysX3Configure.h"
#include "NMPlatform/NMPlatform.h"

#ifdef NM_HOST_IOS
// Inhibit all wanings on ios that PhysX may produce
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wnewline-eof"

#endif

#include "PxPhysicsAPI.h"

// These includes are not in PxPhysicsAPI
#ifdef NM_HOST_CELL_PPU
#include "ps3/PxPS3Extension.h"
#include "ps3/PxDefaultSpuDispatcher.h"
#endif

// NM include
#include "mrPhysX3Deprecated.h"

#ifdef NM_HOST_IOS
// Restore warning state
#pragma clang diagnostic pop
#endif

#endif // NM_PHYSXINCLUDES_H

