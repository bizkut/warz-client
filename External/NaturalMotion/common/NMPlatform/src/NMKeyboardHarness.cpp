// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMKeyboardHarness.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

KeyboardHarness::KeyDownCB* KeyboardHarness::sm_isKeyDownCB = 0;

//----------------------------------------------------------------------------------------------------------------------
bool KeyboardHarness::isKeyDown(unsigned int key)
{
  if (sm_isKeyDownCB)
  {
    return sm_isKeyDownCB(key);
  }

  // If there's no handling function we return false
  return false;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
