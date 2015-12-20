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

#include "mrPhysX2Configure.h"
#include "NMPlatform/NMPlatform.h"

// Turn off warnings from PhysX headers
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4100) // unreferenced formal parameter
  #pragma warning (disable : 4512) // assignment operator could not be generated
  #pragma warning (disable : 4245) // signed/unsigned  mismatch
  #pragma warning (disable : 4244) // possible loss of data
  #pragma warning (disable : 4668) // warns about using #if <identifier> if <identifier> has not been explicitly set
  // nx.h has some errors about this (__APPLE__)
#endif

#include "NxPhysics.h"
#include "NxPhysicsSDK.h"
#include "NxUserContactReport.h"
#include "NxScene.h"
#include "NxActor.h"  
#include "NxController.h"
#include "NxControllerManager.h"
#include "NxCapsuleController.h"
#include "NxBoxController.h"
#include "NxCooking.h"
#include "NxMaterial.h"
#include "NxJoint.h"

#if defined(NM_HOST_CELL_PPU)
  #include "NxCellConfiguration.h"
#endif

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif

#endif // NM_PHYSXINCLUDES_H

