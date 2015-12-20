// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/GameAnimModule.h"
#include "morpheme/mrDispatcher.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

bool AnimSystemModule::m_initialised = false;

//----------------------------------------------------------------------------------------------------------------------
void AnimSystemModule::term()
{
  // Only shut down if we have actually been initialised.
  if (m_initialised)
  {
    // Shut down morpheme dispatchers.
    MR::DispatcherBasic::term();
#ifdef NM_HOST_CELL_PPU
    MR::DispatcherPS3::term();
    NMP::SPUManager::term();
#endif // NM_HOST_CELL_PPU

    // Shut down the morpheme library.
    MR::Manager::termMorphemeLib();

    // Tidy the NaturalMotion memory system.
    NMP::Memory::shutdown();

    m_initialised = false;
  }
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
