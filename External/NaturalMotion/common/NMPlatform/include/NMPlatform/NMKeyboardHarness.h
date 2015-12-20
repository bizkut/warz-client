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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_KEYBOARD_HARNESS_H
#define NM_KEYBOARD_HARNESS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
class KeyboardHarness
{
public:
  typedef bool (KeyDownCB)(unsigned int key);

  static bool isKeyDown(unsigned int whichPad);
  static void setKeyDownCB(KeyDownCB* keyDownCB) { sm_isKeyDownCB = keyDownCB; }
protected:
  static KeyDownCB* sm_isKeyDownCB;
};
} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_KEYBOARD_HARNESS_H
//----------------------------------------------------------------------------------------------------------------------
