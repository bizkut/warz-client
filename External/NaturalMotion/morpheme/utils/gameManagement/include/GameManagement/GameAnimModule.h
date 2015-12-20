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
#ifndef GAME_ANIM_MODULE_H
#define GAME_ANIM_MODULE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"

#if defined(NM_HOST_CELL_PPU)
  #define NM_USE_SPU_DISPATCHER_ON_PS3    ///< ON PS3 disable this define to test execution with a BasicDispatcher
                                          ///<  rather than a PS3 specific one.
  #include "NMPlatform/ps3/NMSPUManager.h"
  #include "morpheme/mrDispatcherPS3.h"
#endif
//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_HOST_WIN64
 #define PLATFORMDIR "x64"
#elif defined(NM_HOST_WIN32)
 #define PLATFORMDIR "win32"
#elif defined(NM_HOST_X360)
 #define PLATFORMDIR "x360"
#elif defined(NM_HOST_CELL_PPU)
 #define PLATFORMDIR "ps3"
#else
 #define PLATFORMDIR "UnknownHost"
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AnimSystemModule
/// \brief Base class responsible for initialising and terminating the morpheme animation system module.
///
/// Inherited classes implement complete functionality for different skus (physics and no physics).
//----------------------------------------------------------------------------------------------------------------------
class AnimSystemModule
{
public:
  
  /// \brief Terminates morpheme core libraries.
  static void term();

  /// \brief Has the morpheme animation system been initialised.
  static bool isInitialised() { return m_initialised; }

protected:

  /// This is a static class.
  AnimSystemModule() {}

  /// Is the morpheme animation system initialised.
  static bool m_initialised;
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_ANIM_MODULE_H
//----------------------------------------------------------------------------------------------------------------------
