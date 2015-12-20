// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_DEBUGDRAW_H
#define NM_DEBUGDRAW_H

#include "NMPlatform/NMPlatform.h"
#include "sharedDefines/mEuphoriaDebugInterface.h"
#include "morpheme/mrDebugMacros.h"

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
enum DebugControl
{
  aimControl,
  animateControl,
  avoidControl,
  bufferControl,
  dodgeControl,
  freefallControl,
  grabControl,
  holdingSupportControl,
  interceptControl,
  objectSeenControl,
  objectSweepSucceededControl,
  placementControl,
  pointControl,
  poseControl,
  reachControl,
  sitControl,
  sittingSupportControl,
  spinControl,
  standingSupportControl,
  stepControl,
  supportControl,
  swingControl,
  writheControl,
  debugControlMax
};

NM_ASSERT_COMPILE_TIME(debugControlMax <= LimbControlAmounts::MAX_NUM_DEBUG_CONTROLS);

//----------------------------------------------------------------------------------------------------------------------
/// Gets the default colour for the specified control type - note that in connect etc it may get
/// coloured differently
NMP::Vector3 getDefaultColourForControl(DebugControl control);

/// Converts to a colour using all the control amounts and the default colours
NMP::Vector3 convertControlAmountsToDefaultColour(const LimbControlAmounts& controlAmounts);

/// Returns the display name for the control type
const char* getDebugControlTypeName(uint32_t control);

/// This function will catch uninitialised data over 75% of the time... don't rely on it 100% Note
/// that 0xcdcdcdcd is the default value for memory on the stack. However, in debug Vector3 will
/// initialise to NaN which (for a float) is 0xcccccccc. Note that 0xcdcdcdcd is actually a valid
/// float, and 0xcccccccc is of course a valid non-float, but we hope that they don't come up for
/// real.
/// This is disabled on PPU as there seems to be padding in the vtable that doesn't get initialised
/// by the constructor.
#if defined(_DEBUG) && !defined(NM_HOST_CELL_PPU) && !defined(NM_HOST_64_BIT)
template <class T>
void assertIsInitialised(T* obj)
{
  uint32_t cls_size = (uint32_t)sizeof(T);
  NMP_ASSERT((cls_size % 4) == 0); // Cannot tell on this class as it isn't composed of 4-byte elements
  uint32_t* as_obj = (uint32_t*)obj;
  for (uint32_t i = 0; i < cls_size / 4; i += 4)
  {
    // assert below mean parts of structure are uninitialised
    NMP_ASSERT(as_obj[i] != 0xcdcdcdcd && as_obj[i] != 0xcccccccc);
    if (as_obj[i+3] != 0xcdcdcdcd && as_obj[i+3] != 0xcccccccc)
      NMP_ASSERT(as_obj[i+1] != 0xcdcdcdcd && as_obj[i+2] != 0xcdcdcdcd && as_obj[i+1] != 0xcccccccc && as_obj[i+2] != 0xcccccccc);
    if (as_obj[i+2] != 0xcdcdcdcd && as_obj[i+2] != 0xcccccccc)
      NMP_ASSERT(as_obj[i+1] != 0xcdcdcdcd && as_obj[i+1] != 0xcdcdcdcd);
  }
}
#else
template <class T>
void assertIsInitialised(T* NMP_UNUSED(obj)) {}
#endif

} // namespace ER

#endif // NM_DEBUGDRAW_H
