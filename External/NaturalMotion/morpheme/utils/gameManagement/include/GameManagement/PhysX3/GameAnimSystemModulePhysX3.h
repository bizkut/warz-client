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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef GAME_ANIM_SYSTEM_MODULE_PHYSX3_H
#define GAME_ANIM_SYSTEM_MODULE_PHYSX3_H
//----------------------------------------------------------------------------------------------------------------------
#include "gameManagement/GameAnimModule.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AnimSystemModulePhysX3
/// \brief Base class responsible for initialising and terminating the PhysX3 sku of the morpheme animation system module.
//----------------------------------------------------------------------------------------------------------------------
class AnimSystemModulePhysX3 : public AnimSystemModule
{
public:

  /// \brief Initialises the morpheme core libraries.
  static void init(
#ifdef NM_HOST_CELL_PPU
    uint32_t           numSpus = NMP::SPUManager::MAX_SPUS,
    CellSpursTaskset2* spursTaskset = NULL,               /// Provide this data if spurs has already been initialised by your application
                                                          ///  otherwise spurs will be initialised internally.
    CellSpurs*         spursObject = NULL,                /// Provide this data if spurs has already been initialised by your application
                                                          ///  otherwise spurs will be initialised internally.
    bool               initSPUPrintf = true               /// Set to true if you have not implemented your own SPU printf (allows debug output from SPU).
#endif // NM_HOST_CELL_PPU
    );

protected:

  /// This is a static class.
  AnimSystemModulePhysX3() {}
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_ANIM_SYSTEM_MODULE_PHYSX3_H
//----------------------------------------------------------------------------------------------------------------------
