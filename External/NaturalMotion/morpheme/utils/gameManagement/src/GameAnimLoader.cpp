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
#include "GameManagement/GameAnimLoader.h"
#include "GameManagement/GameAnimModule.h"
#include <sstream>
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

// Enable output of animation loading and unloading info to console.
#define OUTPUT_DEBUG_ANIM_LOADING_INFOz
#ifdef OUTPUT_DEBUG_ANIM_LOADING_INFO
  #define ANIM_INFO_DEBUG_MSG NMP_DEBUG_MSG
  #define USED_FOR_ANIM_INFO NMP_USED_FOR_ASSERTS 
#else  // NMP_ENABLE_ASSERTS
  #define ANIM_INFO_DEBUG_MSG(...) {}
  #define USED_FOR_ANIM_INFO NMP_UNUSED
#endif // NMP_ENABLE_ASSERTS

MR::UTILS::SimpleAnimRegistry AnimLoader::sm_animRegistry;

//----------------------------------------------------------------------------------------------------------------------
AnimLoader::~AnimLoader()
{
  // Make sure no animations are still cached, which would imply that some NetworkDefs have not been properly shut down.
  NMP_ASSERT(sm_animRegistry.getNumEntries() == 0);
}

//----------------------------------------------------------------------------------------------------------------------
MR::AnimSourceBase* AnimLoader::requestAnim(
  const MR::RuntimeAnimAssetID animAssetID,
  void*                        userdata_)
{
  // The user data is provided when calling MR::NetworkDef::loadAnimations() (see GameAnimNetworkDef::loadAnimations())
  NMP_ASSERT(userdata_);
  const UserData& userData = *(UserData*)userdata_;
  NMP_ASSERT(userData.m_animFileLookup);
  NMP_ASSERT(userData.m_animRegistry);

  //----------------------------
  // Check the animation format.
  // This is to make sure the animation format has been registered (see GameAnimModule::registerAnimationFormats()).
  const char* const format = userData.m_animFileLookup->getFormatType(animAssetID);
  const MR::Manager::AnimationFormatRegistryEntry* const animFormatEntry =
                                    MR::Manager::getInstance().findAnimationFormatRegistryEntry(format);
  if (!animFormatEntry)
  {
    NMP_DEBUG_MSG("error: Unregistered animation format!");
    NMP_ASSERT_FAIL();
    return NULL;
  }

  void* fileBuffer = NULL;
  int64_t bytesRead;
  const char* filename = NULL;
  std::string filenameHelper;

  if (!userData.m_connection)
  {
    //----------------------------
    // Add the file extension to the start of the filename. By default this is hard coded to the ProjectData folder.
    std::stringstream filenameBuilder;
    filenameBuilder << userData.m_animLocation;
    filenameBuilder << "\\";
    filenameBuilder << userData.m_animFileLookup->getFilename(animAssetID);
    filenameHelper = filenameBuilder.str();

    filename = filenameHelper.c_str();
    NMP_DEBUG_MSG("info: Loading animation <%s> (id=%u)", filename, animAssetID);
    
    //----------------------------
    // Is this animation already in the registry.
    const MR::UTILS::SimpleAnimRegistry::Entry* animRegistryEntry = userData.m_animRegistry->getEntry(filename);
    if (animRegistryEntry)
    {
      // Animation already in the registry, increase its ref count and return a pointer to it.
      userData.m_animRegistry->addReferenceToEntry(const_cast<MR::UTILS::SimpleAnimRegistry::Entry*>(animRegistryEntry));
      return animRegistryEntry->getAnim();
    }
    // Animation not already in the registry, proceed with loading it.

    //----------------------------
    // Load animation from disk.
    //
    // The memory allocated for the animation is freed in releaseAnim(). We allocate the animation to the maximum
    // alignment required by any platform to make sure there will no be any alignment issues later.
    int64_t length = 0;

    bytesRead = NMP::NMFile::allocAndLoad(filename, &fileBuffer, &length, NMP_VECTOR_ALIGNMENT);
    if (bytesRead == -1)
    {
      NMP::Memory::memFree(fileBuffer);
      NMP_DEBUG_MSG("error: Failed to load animation file <%s>", filename);
      NMP_ASSERT_FAIL();
      return NULL;
    }
  }
  else
  {
    std::stringstream filenameBuilder;
    filenameBuilder << "preview\\runtimeBinary";
    filenameBuilder << "\\";
    filenameBuilder << userData.m_animFileLookup->getFilename(animAssetID);
    filenameHelper = filenameBuilder.str();

    //----------------------------
    // Get the animation file name.
    filename = filenameHelper.c_str();
    NMP_DEBUG_MSG("info: Loading animation <%s> (id=%u)", filename, animAssetID);

    //----------------------------
    // Is this animation already in the registry.
    const MR::UTILS::SimpleAnimRegistry::Entry* animRegistryEntry = userData.m_animRegistry->getEntry(filename);
    if (animRegistryEntry)
    {
      // Animation already in the registry, increase its ref count and return a pointer to it.
      userData.m_animRegistry->addReferenceToEntry(const_cast<MR::UTILS::SimpleAnimRegistry::Entry*>(animRegistryEntry));
      return animRegistryEntry->getAnim();
    }
    // Animation not already in the registry, proceed with loading it.

    //----------------------------
    // Determine file size.
    //
    // We are loading an animation over a COMMS connection so we ask the connection for the size of the file.
    const uint32_t fileSize = userData.m_connection->getFileSize(filename);
    if (!fileSize)
    {
      NMP_DEBUG_MSG("error: Failed to load animation file <%s> over connection!", filename);
      NMP_ASSERT_FAIL();
      return NULL;
    }

    //----------------------------
    // Allocate buffer and load file.
    //
    // The memory allocated for the animation is freed in releaseAnim(). We allocate the animation to the maximum
    // alignment required by any platform to make sure there will no be any alignment issues later.
    fileBuffer = NMPMemoryAllocAligned(fileSize, NMP_VECTOR_ALIGNMENT);
    if (!fileBuffer)
    {
      NMP_DEBUG_MSG("error: Failed to allocate memory for animation <%s>!", filename);
      NMP_ASSERT_FAIL();
      return NULL;
    }

    const bool loaded = userData.m_connection->getFile(fileBuffer, fileSize, filename);
    if (!loaded)
    {
      NMP::Memory::memFree(fileBuffer);
      NMP_DEBUG_MSG("error: Failed to load animation file <%s> over connection!", filename);
      NMP_ASSERT_FAIL();
      return NULL;
    }

    bytesRead = fileSize;
  }

  NMP_ASSERT(fileBuffer && (bytesRead > 0));

  //----------------------------
  // Register the animation - the registry will now manage this memory.
  userData.m_animRegistry->addEntry(filename, (MR::AnimSourceBase*)fileBuffer);

  //----------------------------
  // Return the animation source.
  //
  // The animation source still needs to be located before use (much like what we do with the asset loaded from bundles)
  // but MR::NetworkDef::loadAnimations() will take care of that for us.
  return (MR::AnimSourceBase*)fileBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimLoader::releaseAnim(
  const MR::RuntimeAnimAssetID NMP_USED_FOR_ASSERTS(animAssetID),
  MR::AnimSourceBase*          loadedAnimation,
  void*                        userdata_)
{
  // The user data is provided when calling MR::NetworkDef::loadAnimations (see Game::CharacterDef::loadAnimationsFromDisk).
  NMP_ASSERT(userdata_);
  const UserData& userData = *(UserData*)userdata_;
  NMP_ASSERT(userData.m_animFileLookup);
  NMP_ASSERT(userData.m_animRegistry);

  if (loadedAnimation)
  {
    // Do not unload an animation whos registry ref count is not zero.
    const MR::UTILS::SimpleAnimRegistry::Entry* animRegistryEntry = userData.m_animRegistry->getEntry(loadedAnimation);
    NMP_ASSERT_MSG(
        animRegistryEntry,
        "  Error - Attempting to unload an anim that does not exist in the anim registry. RuntimeAnimAssetID: '%i'\n",
        animAssetID);
    NMP_ASSERT(loadedAnimation == animRegistryEntry->getAnim());

    if (userData.m_animRegistry->removeReferenceFromEntry(const_cast<MR::UTILS::SimpleAnimRegistry::Entry*>(animRegistryEntry)))
    {
      // Free the animation itself.
      NMP_DEBUG_MSG("Freeing animation asset ID %d", animAssetID);
      NMP::Memory::memFree(loadedAnimation);
    }
  }
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
