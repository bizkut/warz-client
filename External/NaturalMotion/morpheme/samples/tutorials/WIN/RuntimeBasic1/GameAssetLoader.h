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
  // A basic implementation of a loading function to load all assets from a NaturalMotion bundle.
  // For a more detailed implementation see AssetLoaderBasic::loadBundle in the GameManagement library.
  //
  // If a NetworkDef exists in the bundle a pointer to it and its guid are returned.
  // Common use: Loading a network definition and all associated assets from the given bundle.
  //
  // All assets in the bundle, provided they do not already exist, are loaded into the asset store and a list of asset
  // IDs for the registered assets and a list of pointers for the client assets is generated which can be used to
  // unload the assets later. The bundle memory can be freed after this method has completed.
  //
  // The network definition GUID (a 16 byte GUID) which is also returned can be used to identify network definitions to
  // a morpheme:connect instance connected via COMMS. The animation file lookup is used by GameMorphemeManager to locate
  // animation files on disk.
  // 
  // This functionality would be replaced with your own bundle loader if you do not use the simple bundle file format.
  //
  // No animation are loaded yet, this has to be done separately.
  static bool loadBundle(
    void*            bundle,
    size_t           bundleSize,
    MR::AnimRigDef** rig);

  //----------------------------
  // Unloads a set of assets.
  // Common use: Unloading a network definition and all associated assets.
  //
  // This is done by iterating over all registered assets and decreasing their reference count, and if the reference
  // counts hits zero, the asset is unregistered and its memory freed. Client assets are always freed as they are not
  // shared between network definitions.
  static void unLoadBundle(void* bundle, size_t bundleSize);

};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_ASSET_LOADER_H
//----------------------------------------------------------------------------------------------------------------------
