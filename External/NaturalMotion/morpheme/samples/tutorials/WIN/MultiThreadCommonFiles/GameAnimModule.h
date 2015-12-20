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
#ifndef GAME_ANIM_MODULE_H
#define GAME_ANIM_MODULE_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef _WIN64
#define PLATFORMDIR "x64"
#else
#define PLATFORMDIR "win32"
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace MR
{
  class Dispatcher;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class GameAnimModule
/// \brief Demonstrates how to initialize morpheme.
//----------------------------------------------------------------------------------------------------------------------
class GameAnimModule
{
public:

  typedef void (*InitModuleFn)(MR::Dispatcher** const, uint32_t);
  typedef void (*TermtModuleFn)();

  /// \brief Initializes morpheme core libraries.
  ///
  /// This includes registering the core tasks with MR::DispatcherBasic and directing the animation loading functions to
  /// GameAnimLoader.
  ///
  /// Optionally, additionally an external module init function can be passed in to initialise extra modules.
  static void initMorpheme(const InitModuleFn initModule = NULL);

  /// \brief Terminates morpheme core libraries.
  static void termMorpheme(const TermtModuleFn termModule = NULL);

private:

  /// GameAnimLoader is a static class.
  GameAnimModule() {}
};

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_ANIM_MODULE_H
//----------------------------------------------------------------------------------------------------------------------
