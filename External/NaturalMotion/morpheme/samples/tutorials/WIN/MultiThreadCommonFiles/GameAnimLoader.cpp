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
#include "GameAnimLoader.h"
#include "GameAnimModule.h"
//----------------------------------------------------------------------------------------------------------------------
#include <sstream>
//----------------------------------------------------------------------------------------------------------------------

#define ROOTFOLDER "./ProjectData/" PLATFORMDIR "/"

const char GameAnimLoader::ANIM_ROOT_FOLDER[] = ROOTFOLDER;

//----------------------------------------------------------------------------------------------------------------------
MR::AnimSourceBase* GameAnimLoader::requestAnim(const MR::RuntimeAnimAssetID animAssetID, void* userdata_)
{
  // The user data is provided when calling MR::NetworkDef::loadAnimations() (see GameAnimNetworkDef::loadAnimations())
  const UserData& userData = *(UserData*)userdata_;
  NMP_ASSERT(userData.m_animFileLookup);

  //--------------------------------------------------------------------------------------------------------------------
  // Check the animation format.
  //
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

  if (!userData.m_connection)
  {
    //------------------------------------------------------------------------------------------------------------------
    // Add the file extension to the start of the filename. By default this is hard coded to the ProjectData folder.
    std::string filenameHelper;
    {
      std::stringstream filenameBuilder;
      filenameBuilder << ANIM_ROOT_FOLDER;
      filenameBuilder << userData.m_animFileLookup->getFilename(animAssetID);
      filenameHelper = filenameBuilder.str();
    }

    const char* const filename = filenameHelper.c_str();
    NMP_DEBUG_MSG("info: Loading animation <%s> (id=%u)", filename, animAssetID);

    //------------------------------------------------------------------------------------------------------------------
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
    //------------------------------------------------------------------------------------------------------------------
    // Get the animation file name.
    const char* const filename = userData.m_animFileLookup->getFilename(animAssetID);
    NMP_DEBUG_MSG("info: Loading animation <%s> (id=%u)", filename, animAssetID);

    //------------------------------------------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------------------------------------------
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

  //--------------------------------------------------------------------------------------------------------------------
  // Return the animation source.
  //
  // The animation source still needs to be located before use (much like what we do with the asset loaded from bundles)
  // but MR::NetworkDef::loadAnimations() will take care of that for us.
  return (MR::AnimSourceBase*)fileBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
void GameAnimLoader::releaseAnim(
  const MR::RuntimeAnimAssetID NMP_USED_FOR_ASSERTS(animAssetID),
  MR::AnimSourceBase* loadedAnimation,
  void* NMP_UNUSED(userdata))
{
  // Dislocate and free the memory used by the animation. The memory was allocated in GameAnimLoader::networkAnimLoader()
  // calling NMP::NMFile::allocAndLoad().
  if (loadedAnimation)
  {
    NMP_DEBUG_MSG("Freeing animation asset ID %d", animAssetID);
    loadedAnimation->dislocate();
    NMP::Memory::memFree(loadedAnimation);
  }
}
