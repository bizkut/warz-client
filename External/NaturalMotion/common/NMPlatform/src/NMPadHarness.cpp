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
#include "NMPlatform/NMPadHarness.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

PadHarness::PadButtonDownCB* PadHarness::sm_isPadButtonDownCB = 0;
PadHarness::PadButtonClickedCB* PadHarness::sm_isPadButtonClickedCB = 0;

//----------------------------------------------------------------------------------------------------------------------
bool PadHarness::isButtonDown(unsigned int whichPad, unsigned int btnID)
{
  if (sm_isPadButtonDownCB)
  {
    return sm_isPadButtonDownCB(whichPad, btnID);
  }

  // If there's no pad handling function we return false
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool PadHarness::isButtonClicked(unsigned int whichPad, unsigned int btnID)
{
  if (sm_isPadButtonClickedCB)
  {
    return sm_isPadButtonClickedCB(whichPad, btnID);
  }

  // If there's no pad handling function we return false
  return false;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
