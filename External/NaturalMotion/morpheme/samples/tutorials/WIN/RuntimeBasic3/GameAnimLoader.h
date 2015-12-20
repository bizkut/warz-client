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
#ifndef GAME_ANIM_LOADER_H
#define GAME_ANIM_LOADER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/connection.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// Game::AnimLoader
// This is an example implementation of an animation loader that handles animation (un)loading from disk.
//
// Game::World::initMorpheme() registers this animation loader with MR::Manager for use with the tutorials. As this 
// class has little outside dependencies it should serve well as a starting point for your own implementations.
//----------------------------------------------------------------------------------------------------------------------
class AnimLoaderBasic
{
public:

  //----------------------------
  // Static animation loading function. This is past to morpheme in the Game::World::initMorpheme() so that it uses this 
  // function when it loads animations.
  static MR::AnimSourceBase* requestAnim(const MR::RuntimeAnimAssetID animAssetID, void* userdata);

  //----------------------------
  // Static function used to unload the animations. As with the animation loading function this function is passed to 
  // morpheme in Game::World::initMorpheme()
  static void releaseAnim(const MR::RuntimeAnimAssetID animAssetID, MR::AnimSourceBase* loadedAnimation, void* userdata);

protected:

  //----------------------------
  // AnimLoader is a static class.
  AnimLoaderBasic() {}

};


}

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_ANIM_LOADER_H
//----------------------------------------------------------------------------------------------------------------------
