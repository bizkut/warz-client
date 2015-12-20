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
#ifndef GAME_ASSET_LOADER_H
#define GAME_ASSET_LOADER_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameCharacterDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

class AssetLoaderBasic
{
public:

  //----------------------------
  // This function iterates through the objects in a simple bundle and registers them with the morpheme runtime library.
  // It passes back a pointer to the (last) networkDefinition found in the bundle. This function is very simple and
  // simply fixes up the objects in-place, inside the bundle.
  //
  // This would be replaced with your own bundle loader and unloader if you do not use the simple bundle file format.
  static MR::NetworkDef* loadBundle(
    void*            bundle,
    size_t           bundleSize,
    MR::AnimRigDef** rig,
    MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup** animFileLookUp);

  //----------------------------
  // This unloads the objects loaded in the GameAnimModule::loadBundle. We could avoid an iteration over the bundle file
  // by reusing object count information from the loadBundle() functions but this function has been designed to be
  // self-contained.
  static void unLoadBundle(void* bundle, size_t bundleSize);

};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_ASSET_LOADER_H
//----------------------------------------------------------------------------------------------------------------------
