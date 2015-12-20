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
#ifndef NM_PAD_HARNESS_H
#define NM_PAD_HARNESS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
class PadHarness
{
public:
  enum PadButtons
  {
    PAD_UP,
    PAD_DOWN,
    PAD_LEFT,
    PAD_RIGHT,
    PAD_START,
    PAD_SELECT,
    PAD_LEFT_THUMB,
    PAD_RIGHT_THUMB,
    PAD_LEFT_SHOULDER,
    PAD_RIGHT_SHOULDER,
    PAD_CROSS,          // PS3 cross == Xbox A
    PAD_CIRCLE,         // PS3 circle == Xbox B
    PAD_SQUARE,         // PS3 square == Xbox X
    PAD_TRIANGLE,       // PS3 triangle == Xbox Y

    PAD_NUM_BUTTONS
  };

  typedef bool (PadButtonDownCB)(unsigned int whichPad, unsigned int btnID);
  typedef bool (PadButtonClickedCB)(unsigned int whichPad, unsigned int btnID);

  static bool isButtonDown(unsigned int whichPad, unsigned int btnID);
  static bool isButtonClicked(unsigned int whichPad, unsigned int btnID);
  static void setPadButtonDownCB(PadButtonDownCB* padButtonDownCB) { sm_isPadButtonDownCB = padButtonDownCB; }
  static void setPadButtonClickedCB(PadButtonClickedCB* padButtonClickedCB) { sm_isPadButtonClickedCB = padButtonClickedCB; }
protected:
  static PadButtonDownCB* sm_isPadButtonDownCB;
  static PadButtonClickedCB* sm_isPadButtonClickedCB;
};
} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_PAD_HARNESS_H
//----------------------------------------------------------------------------------------------------------------------
