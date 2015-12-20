// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include <string>
#include <algorithm>
#include <map>
#include <sys/types.h>
#include <shlwapi.h>
#include <time.h>

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMFile.h"
#include "NMPlatform/NMHierarchy.h"
#include "NMPlatform/NMTimer.h"
#include "NMPlatform/NMHash.h"

#ifdef NMP_MEMORY_LOGGING
#include "NMPlatform/NMLoggingMemoryAllocator.h"
#endif // NMP_MEMORY_LOGGING

#include "NMPlatform/NMFreelistMemoryAllocator.h"
#include "NMPlatform/NMSeh.h"

#include "morpheme/mrDispatcher.h"
#include "morpheme/mrManager.h"
#include "morpheme/Nodes/mrNodes.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/mrRig.h"

#include "XMDLogRedirector.h"
#include "simpleBundle/simpleBundle.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookupBuilder.h"
#include "animCache.h"
#include "BasicCharacterController.h"
#include "PluginManager.h"


#include "export/mcAnimInfoXml.h"
#include "export/apExportLUA.h"

#include "assetProcessor/AssetProcessor.h"
#include "assetProcessor/AssetProcessorUtils.h"

#include "XMD/AnimCycle.h"
#include "XMD/Model.h"
#include "XMD/Bone.h"

#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/AnimSource/mrAnimSourceASA.h"
#include "morpheme/AnimSource/mrAnimSourceMBA.h"
#include "morpheme/AnimSource/mrAnimSourceNSA.h"
#include "morpheme/AnimSource/mrAnimSourceQSA.h"
#include "assetProcessor/AnimSource/animSourcePreprocessor.h"
#include "assetProcessor/AnimSource/animSourceCompressorASA.h"
#include "assetProcessor/AnimSource/animSourceCompressorMBA.h"
#include "assetProcessor/AnimSource/animSourceCompressorNSA.h"
#include "assetProcessor/AnimSource/animSourceCompressorQSA.h"

#include "ACOptions.h"

#if defined(NM_ENABLE_FBX)
  #include "FBX/FBXPlugin.h"
#else
  #include "windows.h"
#endif

#include <direct.h>

template __declspec(dllimport) class XM2::vector_iterator<XMD::XU32>;
template __declspec(dllimport) class XM2::vector_const_iterator<XMD::XU32>;
template __declspec(dllimport) class XM2::pod_vector<XMD::XU32>;
#pragma warning(disable: 4996)
//----------------------------------------------------------------------------------------------------------------------
/// \defgroup AssetCompilerModule Asset Compiler.
///
/// An application to produce binary runtime data is provided in the form of the morpheme asset compiler, which is an
/// example use of the asset processor library \see AssetProcessorModule. This application implements a very simple ID
/// assignment scheme and a trivial system for packaging up binary data objects into a single file.
/// This is intended as a reference for customers creating their own asset generation tools and it is fully expected
/// that this will be heavily modified to suit individual asset management systems and pipelines.
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;
using namespace AP;

/// \def MAX_ANIMATION_FILENAME_LENGTH
///
/// Defines the maximum number of characters allows in a runtime animation file name.
/// If the file name is longer than this it will be trimmed to fit. Useful for limiting runtime asset filename lengths on consoles.
/// Must be at least 8 characters (for the identifier), includes both the extension and the preceding dot.
#define MAX_ANIMATION_FILENAME_LENGTH (42)
NM_ASSERT_COMPILE_TIME(MAX_ANIMATION_FILENAME_LENGTH >= 8);

/// For writing all of the processed assets to a bundled output file.
MR::UTILS::SimpleBundleWriter* g_bundleWriter = NULL;

/// For writing all of the processed assets to a bundled output file.
static AnimCache* g_animCache = NULL;

/// \brief this structure can be used to overwrite the input variables - like it is used for compiling to shared rigs.
static AP::AssetCompilerPlugin::AnimationMetadata* g_animMetaData = NULL;

/// \brief this structure stores the various command line parameters as globals for use within this file.
static struct
{
  NMP::BasicLogger *m_messageLogger;
  NMP::BasicLogger *m_errorLogger; 
  uint32_t m_successCode;
  uint32_t m_failureCode;
  const char *m_baseDir;
  const char *m_outputDir;
  const char *m_outputFilename;
  bool m_copyToAnimBrowserOutFile;
} g_acGlobals;

//----------------------------------------------------------------------------------------------------------------------
// Output log stuff.

/// \def ENABLE_ASSET_COMP_LOG_MSG
/// \brief Instate this define in order to output messages tracking the progress of the export.
#define ENABLE_ASSET_COMP_LOG_MSG
#if defined(ENABLE_ASSET_COMP_LOG_MSG)
  #define ASSET_COMP_LOG_MSG(fmt, ...) { g_acGlobals.m_messageLogger->output(fmt, ##__VA_ARGS__); }
#else
  #define ASSET_COMP_LOG_MSG(fmt, ...)
#endif

MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookupBuilder* g_animIDtoFilename;


//----------------------------------------------------------------------------------------------------------------------
/// Profiling information for animation processing.
/// \ingroup AssetCompilerModule
float g_macTotalEllapsedTime = 0.0f;
float g_macProcessAnimFuncElapsedTimeRigToAnimMaps = 0.0f;
float g_macProcessAnimFuncElapsedTimeLoadXMDAnims = 0.0f;
float g_macProcessAnimFuncElapsedTimeBuildUncompressedAnims = 0.0f;
float g_macProcessAnimFuncElapsedTimeBuildCompressedAnims = 0.0f;
uint32_t g_macProcessAnimFuncNumCompiledAnims = 0;

//----------------------------------------------------------------------------------------------------------------------
/// \brief Makes sure that all each byte of memory allocated is initialised to 0xcd
#define FORCE_INIT_ALL_MEMORY
#define FORCE_INIT_ALL_MEMORY_VALUE 0xcd

//----------------------------------------------------------------------------------------------------------------------
static void* mallocAlignedInitialised(size_t size, size_t alignment, unsigned char initialisation)
{
  size = NMP::Memory::align(size, alignment);
  void* result = _aligned_malloc(size, alignment);
  for (uint32_t i = 0; i < size; i++)
  {
    ((char*)result)[i] = initialisation;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
static void* mallocWrapped(size_t size)
{
#ifdef FORCE_INIT_ALL_MEMORY
  return mallocAlignedInitialised(size, 16, FORCE_INIT_ALL_MEMORY_VALUE);
#else
  return _aligned_malloc(size, 16);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
static void* mallocAlignedWrapped(size_t size, size_t alignment)
{
#ifdef FORCE_INIT_ALL_MEMORY
  return mallocAlignedInitialised(size, alignment, FORCE_INIT_ALL_MEMORY_VALUE);
#else
  return _aligned_malloc(size, alignment);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
static void* callocWrapped(size_t size)
{
  return mallocAlignedInitialised(size, 4, 0);
}

//----------------------------------------------------------------------------------------------------------------------
static void freeWrapped(void* ptr)
{
  _aligned_free(ptr);
}

//----------------------------------------------------------------------------------------------------------------------
static void memcpyWrapped(void* dst, const void* src, size_t size)
{
  memcpy(dst, src, size);
}

//----------------------------------------------------------------------------------------------------------------------
static void memcpy128Wrapped(void* dst, const void* src, size_t size)
{
  memcpy(dst, src, size);
}

//----------------------------------------------------------------------------------------------------------------------
static size_t memSizeWrapped(void*)
{
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
char* initGuidSting(char* guid, uint32_t part1, uint32_t part2, uint32_t part3, uint32_t part4)
{
  NMP_VERIFY_MSG(guid, "NULL GUID provided to initGuidString");
  sprintf(guid, "%08x-%04x-%04x-%04x-%04x%08x",
          part1,
          part2 >> 16,
          part2 & 0xffff,
          part3 >> 16,
          part3 & 0xffff,
          part4);
  return guid;
}
//----------------------------------------------------------------------------------------------------------------------
/// \brief Outputs profiling information for animation processing in the asset compiler.
/// \ingroup AssetCompilerModule
void logProfilingInformation()
{
  g_acGlobals.m_messageLogger->output("Total asset compiler elapsed time = %f ms\n", g_macTotalEllapsedTime);
  g_acGlobals.m_messageLogger->output("Number of compiled animations = %d\n", g_macProcessAnimFuncNumCompiledAnims);
  g_acGlobals.m_messageLogger->output("Elapsed time for processing rig to anim maps = %f ms\n", g_macProcessAnimFuncElapsedTimeRigToAnimMaps);
  g_acGlobals.m_messageLogger->output("Elapsed time for loading XMD animations = %f ms\n", g_macProcessAnimFuncElapsedTimeLoadXMDAnims);
  g_acGlobals.m_messageLogger->output("Elapsed time for building uncompressed animations = %f ms\n", g_macProcessAnimFuncElapsedTimeBuildUncompressedAnims);
  g_acGlobals.m_messageLogger->output("Elapsed time for building compressed animations = %f ms\n", g_macProcessAnimFuncElapsedTimeBuildCompressedAnims);

  float macProcessAnimFuncTotal =
    g_macProcessAnimFuncElapsedTimeRigToAnimMaps +
    g_macProcessAnimFuncElapsedTimeLoadXMDAnims +
    g_macProcessAnimFuncElapsedTimeBuildUncompressedAnims +
    g_macProcessAnimFuncElapsedTimeBuildCompressedAnims;

  if (macProcessAnimFuncTotal > 0.0f)
  {
    g_acGlobals.m_messageLogger->output(
      "macProcessAnimFunc Proportions: RigToAnimMap = %f, XMD = %f, Uncomp = %f, Comp = %f\n",
      g_macProcessAnimFuncElapsedTimeRigToAnimMaps / macProcessAnimFuncTotal,
      g_macProcessAnimFuncElapsedTimeLoadXMDAnims / macProcessAnimFuncTotal,
      g_macProcessAnimFuncElapsedTimeBuildUncompressedAnims / macProcessAnimFuncTotal,
      g_macProcessAnimFuncElapsedTimeBuildCompressedAnims / macProcessAnimFuncTotal);
  }

  if (g_macTotalEllapsedTime > 0.0f)
  {
    // Compute time spent processing other things
    float elapsedTimeOther = g_macTotalEllapsedTime - macProcessAnimFuncTotal;

    // Compute proportions of total asset compiler time
    g_acGlobals.m_messageLogger->output(
      "Total Proportions: RigToAnimMap = %f, XMD = %f, Uncomp = %f, Comp = %f, Other = %f\n",
      g_macProcessAnimFuncElapsedTimeRigToAnimMaps / g_macTotalEllapsedTime,
      g_macProcessAnimFuncElapsedTimeLoadXMDAnims / g_macTotalEllapsedTime,
      g_macProcessAnimFuncElapsedTimeBuildUncompressedAnims / g_macTotalEllapsedTime,
      g_macProcessAnimFuncElapsedTimeBuildCompressedAnims / g_macTotalEllapsedTime,
      elapsedTimeOther / g_macTotalEllapsedTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief A cache map that stores animations while they're loaded/copied to reuse them whenever needed
/// \ingroup AssetCompilerModule
class AnimationSourceCache
{
public:
  //----------------------------------------------------------------------------------------------------------------------
  /// \ingroup AssetCompilerModule
  ~AnimationSourceCache()
  {
    for (TmAnimSources::const_iterator iteAnimSource = m_AnimationSources.begin();
         iteAnimSource != m_AnimationSources.end();
         ++iteAnimSource)
    {
      NMP::Memory::memFree(iteAnimSource->second);
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief This gets called during compilation - please note that the source animation gets located in here, thus you would not
  ///         like to do this before saving the source somewhere on disk!
  /// \ingroup AssetCompilerModule
  void addAnimSource(const MR::RuntimeAnimAssetID animAssetID, MR::AnimSourceBase* animSource)
  {
    const MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookupBuilder::AnimEntry* entry = g_animIDtoFilename->getEntry(animAssetID);
    NMP_VERIFY(entry);
    const char* format = entry->animFormat;
    // Get the registered animation format string
    if (!animSource->isLocated())
    {
      const MR::Manager::AnimationFormatRegistryEntry* animRegistryEntry =
        MR::Manager::getInstance().findAnimationFormatRegistryEntry(format);
      NMP_VERIFY(animRegistryEntry);
      animRegistryEntry->m_locateAnimFormatFn(animSource);
    }

    // in case that the animation got updated, replace the cache entry as well
    TmAnimSources::iterator iteAnimSource = m_AnimationSources.find(animAssetID);
    if (iteAnimSource != m_AnimationSources.end())
    {
      NMP::Memory::memFree(iteAnimSource->second);
      iteAnimSource->second = animSource;
    }
    else
    {
      m_AnimationSources.insert(std::make_pair(animAssetID, animSource));
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  /// \ingroup AssetCompilerModule
  MR::AnimSourceBase* getAnimSource(const MR::RuntimeAnimAssetID animAssetID)
  {
    TmAnimSources::const_iterator iteAnimSource = m_AnimationSources.find(animAssetID);
    if (iteAnimSource != m_AnimationSources.end())
    {
      return iteAnimSource->second;
    }
    else
    {
      // Information
      MR::AnimSourceBase*  animSource = 0;
      void*                fileBuffer = 0;
      int64_t             length = 0;

      const MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookupBuilder::AnimEntry* entry = g_animIDtoFilename->getEntry(animAssetID);
      NMP_VERIFY(entry);
      const char* filename = entry->animFilename;
      const char* format = entry->animFormat;

      // Get the registered animation format string
      const MR::Manager::AnimationFormatRegistryEntry* animRegistryEntry =
        MR::Manager::getInstance().findAnimationFormatRegistryEntry(format);
      NMP_VERIFY(animRegistryEntry);

      char filepath[MAX_PATH];
      NMP_SPRINTF(filepath, 256, "%s/%s", g_acGlobals.m_outputDir, filename);

      // Allocate and load the animation file
#if defined(NMP_ENABLE_ASSERTS) || NM_ENABLE_EXCEPTIONS
      int64_t bytesRead = 
#endif 
        NMP::NMFile::allocAndLoad(filepath, &fileBuffer, &length, NMP_VECTOR_ALIGNMENT);
      NMP_VERIFY(bytesRead != -1);

      // Locate the animation in memory
      animSource = (MR::AnimSourceBase*) fileBuffer;

      animRegistryEntry->m_locateAnimFormatFn(animSource);
      m_AnimationSources.insert(std::make_pair(animAssetID, animSource));
      return animSource;
    }
  }

private:

  typedef std::map<const MR::RuntimeAnimAssetID, MR::AnimSourceBase*> TmAnimSources;

  TmAnimSources m_AnimationSources;
};

static AnimationSourceCache* g_AnimationSourceCache = 0;

//----------------------------------------------------------------------------------------------------------------------
/// \brief Update the information about a single XMD animation file in the anim info
/// \ingroup AssetCompilerModule
bool updateAnimInfoWithXMD(AnimInfo* animInfo, const char* animFileName, int64_t animUpdateTime)
{
  XMD::XModel model;
  if (model.LoadAnim(animFileName) == XMD::XFileError::Success)
  {
    animInfo->setValidDate(animUpdateTime);
    animInfo->clearTakes();
    for (uint32_t i = 0; i != model.NumAnimCycles(); ++i)
    {
      XMD::XAnimCycle* cycle = model.GetAnimCycle(i);
      if (cycle)
      {
        const char* takename = cycle->GetName();
        float duration = (cycle->GetEndTime() - cycle->GetStartTime()) / cycle->GetFramesPerSecond();
        float fps = cycle->GetFramesPerSecond();
        animInfo->addTake(takename, duration, fps);
      }
    }

    // add new XAnimationTake data types
    XMD::XList anims;
    model.List(anims, XMD::XFn::AnimationTake);
    for (XMD::XList::iterator it = anims.begin(); it != anims.end(); ++it)
    {
      XMD::XAnimationTake* cycle = (*it)->HasFn<XMD::XAnimationTake>();
      if (cycle)
      {
        const char* takename = cycle->GetName();
        float duration = (cycle->GetEndTime() - cycle->GetStartTime()) / cycle->GetFramesPerSecond();
        float fps = cycle->GetFramesPerSecond();
        animInfo->addTake(takename, duration, fps);
      }
    }

    // if there are no takes then we "make" one from the bind pose
    size_t totalTakes = model.NumAnimCycles() + anims.size();
    if (totalTakes == 0)
    {
      animInfo->addTake("bind_pose", 1.0f / (float)acAnimInfo::kBindPoseFPS, (float)acAnimInfo::kBindPoseFPS);
    }
  }
  else
  {
    // We failed to load the animation
    g_acGlobals.m_messageLogger->output("Failed to load source animation file %s into XMD structure.\n", animFileName);
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Update the information about a single animation file in the anim info list.
/// \ingroup AssetCompilerModule
bool updateAnimInfoListWithAnim(AnimInfoListXML* animInfoList, const char* animFileName)
{
  NMP_VERIFY(animInfoList);

  // Get the stat info for the file 
  int64_t fileModificationTime = getFileModificationTime(animFileName);
  bool success = false;

  // Get the animation file type
  const char* extension = PathFindExtensionA(animFileName);
  NMP_VERIFY(extension);

  AnimInfo* animInfo = animInfoList->getAnimInfo(animFileName);
  if (animInfo)
  {
    // Check it's up to date, remaking it if not
    int64_t cachedAnimTime = animInfo->getValidDate();

    // Use a straight comparison for consistency with connect
    if (fileModificationTime != cachedAnimTime)
    {
      // The entry is out of date - update it
      if (stricmp(extension, ".xmd") == 0)
      {
        success = updateAnimInfoWithXMD(animInfo, animFileName, fileModificationTime);
      }
#if defined(NM_ENABLE_FBX)
      else if (stricmp(extension, ".fbx") == 0)
      {
        success = FBX_XMD_PLUGIN::updateAnimInfoWithFBX(animInfo, animFileName, fileModificationTime);
      }
#endif // defined(NM_ENABLE_FBX)
      else
      {
        NMP_VERIFY_FAIL("Unsupported animation file extension '%s'", extension);
      }

      // Failed so remove it from the list
      if (!success)
      {
        animInfoList->removeAnimInfo(animInfo);
      }
    }
    else
    {
      // Everything is up to date so we have succeeded
      success = true;
    }
  }
  else
  {
    // Create a new entry for this animation as it's not in the anim info list at the moment.
    animInfo = animInfoList->createAnimInfo(animFileName);

    // The entry is definitely out of date - update it
    if (stricmp(extension, ".xmd") == 0)
    {
      success = updateAnimInfoWithXMD(animInfo, animFileName, fileModificationTime);
    }
#if defined(NM_ENABLE_FBX)
    else if (stricmp(extension, ".fbx") == 0)
    {
      success = FBX_XMD_PLUGIN::updateAnimInfoWithFBX(animInfo, animFileName, fileModificationTime);
    }
#endif // defined(NM_ENABLE_FBX)
    else
    {
      NMP_VERIFY_FAIL("Unsupported animation file extension '%s'", extension);
    }

    // Failed so remove it from the list
    if (!success)
    {
      animInfoList->removeAnimInfo(animInfo);
    }
  }

  return success;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Find out if the passed in filename is a valid animation file or not.
/// \ingroup AssetCompilerModule
static bool isValidAnimationFile(const char* filename)
{
  char* extension = PathFindExtensionA(filename);

  if (!strcmpi(extension, ".xmd"))
  {
    return true;
  }

#if defined(NM_ENABLE_FBX)
  if (!strcmpi(extension, ".fbx"))
  {
    return true;
  }
#endif

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Update the information about animation files' takes and durations.
/// \ingroup AssetCompilerModule
bool macUpdateAnimInfo(const char* target)
{
  AnimInfoList* animInfoList;
  AnimInfoFactoryXML animInfoFactory;

  // Decide if the target is a file or a directory.
  BOOL processDir = PathIsDirectoryA(target);

  // Get the target anim info file
  char animInfoFile[MAX_PATH];
  strcpy(animInfoFile, target);
  // If we were passed a filename, strip the filename off the end.
  if (!processDir)
  {
    PathRemoveFileSpecA(animInfoFile);
  }
  PathAddBackslashA(animInfoFile);
  if (processDir)
  {
    strcat(animInfoFile, "animInfo.cache");
  }
  else
  {
    strcat(animInfoFile, "_animInfo.cache");
  }

  if (processDir && PathFileExistsA(animInfoFile))
  {
    // Load the cache file
    animInfoList = animInfoFactory.loadAnimInfoList(animInfoFile);
    NMP_VERIFY(animInfoList);
  }
  else
  {
    // create a new cache
    animInfoList = animInfoFactory.createAnimInfoList();
  }

  if (processDir)
  {
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char pathToSearch[MAX_PATH];
    DWORD dwError = 0;

    strcpy(pathToSearch, target);
    PathAddBackslashA(pathToSearch);
    strcat(pathToSearch, "*");

    // Find the first file in the directory.
    hFind = FindFirstFileA(pathToSearch, &ffd);

    if (INVALID_HANDLE_VALUE == hFind)
    {
      // We didn't find any files at all
      dwError = GetLastError();
      return true;
    }
    else
    {
      char animFilename[MAX_PATH];

      // Check it's a valid anim file (get the extension and check it's supported).
      if (isValidAnimationFile(ffd.cFileName))
      {
        strcpy(animFilename, target);
        PathAddBackslashA(animInfoFile);
        strcat(animFilename, ffd.cFileName);
        bool success = updateAnimInfoListWithAnim((AnimInfoListXML*)animInfoList, animFilename);
        if (success == false)
        {
          g_acGlobals.m_errorLogger->output("Failed to generate anim info for %s\n", animFilename);
        }
      }

      clock_t lastTime = clock();

      // List all the other files in the directory.
      while (FindNextFileA(hFind, &ffd) != 0)
      {
        if (isValidAnimationFile(ffd.cFileName))
        {
          strcpy(animFilename, target);
          PathAddBackslashA(animFilename);
          strcat(animFilename, ffd.cFileName);
          bool success = updateAnimInfoListWithAnim((AnimInfoListXML*)animInfoList, animFilename);
          if (success == false)
          {
            g_acGlobals.m_errorLogger->output("Failed to generate anim info for %s\n", animFilename);
          }
        }

        clock_t currTime = clock();
        if (currTime - lastTime < 10 * CLOCKS_PER_SEC)
        {
          // It's been more than 10 seconds since last we wrote out the anim info, so write it out now. This
          //  prevents having to re-run anim info processing on the whole folder if one file within it causes an error.
          int error = 0;

          bool writeSuccess = ((AnimInfoListXML*)animInfoList)->write(animInfoFile, &error);

          if (!writeSuccess)
          {
            g_acGlobals.m_errorLogger->output("File system error %i when writing file %s\n", error, animInfoFile);
            return false;
          }

          lastTime = currTime;
        }
      }

      dwError = GetLastError();
      if (dwError != ERROR_NO_MORE_FILES)
      {
        // We hit some kind of error
        g_acGlobals.m_errorLogger->output("File system error %i when trying to generate anim info for %s\n", dwError, animFilename);
        return false;
      }
    }

    FindClose(hFind);

    // Now we should check that all the animations in the info list still exist.
    // If the files are no longer there, we delete them from the info list
    for (int32_t index = animInfoList->getNumAnimInfos() - 1; index >= 0; index--)
    {
      AnimInfo* animInfo = animInfoList->getAnimInfo((uint32_t)index);
      if (!PathFileExistsA(animInfo->getName()))
      {
        animInfoList->removeAnimInfo(animInfo);
      }
    }
  }
  else
  {
    // Just check the single file asked for
    if (!updateAnimInfoListWithAnim((AnimInfoListXML*)animInfoList, target))
    {
      ((AnimInfoListXML*)animInfoList)->write(animInfoFile);

      // We hit some kind of error
      return false;
    }
  }

  // save anim info list
  int error;
  bool writeSuccess = ((AnimInfoListXML*)animInfoList)->write(animInfoFile, &error);
  if (!writeSuccess)
  {
    g_acGlobals.m_errorLogger->output("File system error %i when writing file %s\n", error, animInfoFile);
  }

  return writeSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Here, we define the functions that are required from the app to enable asset processing.
/// \ingroup AssetCompilerModule
static MR::AnimSourceBase* macCustomAnimLoader(const MR::RuntimeAnimAssetID animAssetID, void* NMP_UNUSED(userdata))
{
  return g_AnimationSourceCache->getAnimSource(animAssetID);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
static void macCustomAnimUnloader(
  const MR::RuntimeAnimAssetID  NMP_UNUSED(animAssetID),
  MR::AnimSourceBase*           NMP_UNUSED(loadedAnimation),
  void*                         NMP_UNUSED(userdata))
{
  // this does nothing - all animations are cached and be freed on exit
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Provides a 1:1 mapping from AssetUID (generated by connect) to a runtime ID that can be used to identify 1st
/// level assets.
/// \ingroup AssetCompilerModule
MR::RuntimeAssetID macGenUIDFunc(
  const char* assetName,  ///< Asset name (likely to be specified by the user within connect).
  AssetUID    connectID   ///< Generated by connect.  Must map 1:1 to the MR::RuntimeAssetID returned by]
                          ///<  this function.
)
{
  // Hash the GUID to create a runtime ID.
  MR::RuntimeAssetID id = NMP::hashStringCRC32(connectID);
  ASSET_COMP_LOG_MSG(" GUID '%s' for item '%s' generated id 0x%x.\n", connectID, assetName, id);
  return id;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Generated assets are returned to the application via this callback.
/// \ingroup AssetCompilerModule
///
/// The application can write them out immediately or store them and collect them later.
/// This will be called in reverse dependency order, i.e. if they are loaded at runtime in the order that they are
/// provided to this callback, all dependencies will be satisfied.
void macHandleProcessedAssetFunc(
  MR::RuntimeAssetID      runtimeID,
  AssetUID                connectID,
  NMP::Memory::Resource   binaryObject,
  MR::Manager::AssetType  binaryObjectType)
{
#if defined(NMP_ENABLE_ASSERTS) || NM_ENABLE_EXCEPTIONS
  bool writeResult =
#endif
    g_bundleWriter->writeAsset(
      binaryObjectType,
      runtimeID,
      (uint8_t*) connectID,
      binaryObject.ptr,
      binaryObject.format);
  NMP_VERIFY(writeResult);

  return;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
bool nameGuidLessThan(
  const AnimCache::Key::NameAndGUID& s1,
  const AnimCache::Key::NameAndGUID& s2)
{
  return (strcmp(s1.first.c_str(), s2.first.c_str()) < 0); // Lexicographical comparison.
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Loads a source animation file into an XMD animation structure.
/// \ingroup AssetCompilerModule
bool loadSourceAnim(
  XMD::XModel& xmdAnim,             // Out.
  const char*  sourceAnimFileName   // In.
)
{
  XMD::XFileError::eType fileResult;

  NMP_VERIFY(sourceAnimFileName);

  // Load the source animation file into an xmd structure. (Import it from XMD, FBX or other translator).
  fileResult = xmdAnim.LoadAnim(sourceAnimFileName);
  if (fileResult != XMD::XFileError::Success)
    return false;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Generate a animation file name with its full path specification.
/// \ingroup AssetCompilerModule
///
/// Allocates a buffer which needs to be deallocated later.
char* generateFullSourceAnimFileName(
  const char*  fileName             // In.
)
{
  char* fullSourceAnimFileName = NULL;
  const char* locationOfRootDir = strstr(fileName, "$(RootDir)");
  size_t stringLength = 0;

  NMP_VERIFY(fileName);

  if (locationOfRootDir == fileName)
  {
    // The fileName starts with '$(RootDir)' replace it with stored root directory for asset loading (kCLA_BASE_DIR).
    stringLength = strlen(g_acGlobals.m_baseDir) + strlen(fileName) + 1;
    fullSourceAnimFileName = (char*) NMPMemoryAlloc(stringLength);
    NMP_ASSERT(fullSourceAnimFileName);

    strcpy(fullSourceAnimFileName, g_acGlobals.m_baseDir);
    strcat(fullSourceAnimFileName, (fileName + 10));
  }
  else
  {
    // Assuming its already a fully qualified path and file name.
    stringLength = strlen(fileName) + 1;
    fullSourceAnimFileName = (char*) NMPMemoryAlloc(stringLength);
    NMP_ASSERT(fullSourceAnimFileName);
    strcpy(fullSourceAnimFileName, fileName);
  }

  return fullSourceAnimFileName;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Add all the rig names and guids to a combined lexicographically sorted vector of both.
/// \ingroup AssetCompilerModule
void generateRigNameGuidsVector(
  AssetProcessor* processor,
  const std::vector<const MR::AnimRigDef*>& rigs,
  const std::vector<const char*>& rigNames,
  AnimCache::Key::NameAndGUIDVector& sortedRigNameGuidsVector)
{
  size_t rigCount = rigNames.size();
  sortedRigNameGuidsVector.resize(rigCount);

  for (size_t i = 0; i != rigCount; ++i)
  {
    const MR::AnimRigDef* rig = rigs[i];

    sortedRigNameGuidsVector[i].first = rigNames[i];

    const ProcessedAsset* asset = processor->findProcessedAsset(rig);
    sortedRigNameGuidsVector[i].second = asset->guid;
  }

  sort(sortedRigNameGuidsVector.begin(), sortedRigNameGuidsVector.end(), nameGuidLessThan);
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup AssetCompilerModule
/// \brief
void parseAnimationOptionsForCompressionOptions(
  const char*  animOptions,
  std::string& compressionOptions)
{
  NMP_VERIFY(animOptions);

  compressionOptions = "";

  std::string stringOptions = animOptions;
  char* buffer = (char*)stringOptions.c_str();

  const char seps[] = " , \t\n";
  char* token;
  token = strtok(buffer, seps);
  while (token)
  {
    bool status = true;
    if (stricmp(token, "genDeltas") == 0)
      status = false;

    if (stricmp(token, "-resample") == 0)
    {
      token = strtok(0, seps); // Eat the resample option
      status = false;
    }

    if (status)
    {
      if (!compressionOptions.empty())
        compressionOptions += " ";
      compressionOptions += token;
    }

    // Get the next token
    token = strtok(0, seps);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Make a RigToAnimMap for each of the provided rigs.
void createRigToAnimEntryMaps(
  AP::AssetProcessor*                NMP_UNUSED(assetProc),
  const XMD::XModel&                 xmdAnims,                      ///< IN: XMD anim structure to try and find the take in.
  const acAnimInfo&                  animInfo,                      ///< IN: Information about the anim and take that we are interested in.
  const std::vector<const MR::AnimRigDef*>& rigsVector,             ///< IN: The compiled rigs vector.
  const MR::AnimRigDef*              conglomerateRig,               ///< IN: AnimRigDef of the conglomerate hierarchy constructed from rigs
                                                                    ///<      that this anim can be used with at runtime.
  bool                               addMissingChannels,            ///< IN: Flag determining if conglomerate channels missing from the animation
                                                                    ///<      should be added to the rig to anim maps.
  std::vector<uint32_t>&             conglomerateRigBoneNamesIDsVector, ///< OUT: The set of rigBoneName ids that are common between the source
                                                                    ///<      anim and conglomerated rig bone names list.
  std::vector<XMD::XId>&             finalAnimBoneIDsVector,        ///< OUT: The common set of XMD bone ids containing valid animation data
                                                                    ///<     for the anim channel indexes.
  std::vector<const MR::RigToAnimEntryMap*>& rigToAnimEntryMaps     ///< OUT: Rig to anim entry maps generated for each of the provided input rigs.
)
{
  NMP_VERIFY(conglomerateRigBoneNamesIDsVector.size() == 0);
  NMP_VERIFY(finalAnimBoneIDsVector.size() == 0);
  NMP_VERIFY(rigToAnimEntryMaps.size() == 0);

  XMD::XIndexList animatedNodes;
  XMD::XIndexList::const_iterator nodeIt;
  bool isBindPose = animInfo.m_TakeIndex == (uint32_t)acAnimInfo::kBindPoseTakeIndex;

  // Find the take that we want to process within the XMD data.
  const XMD::XAnimCycle* xmdCycle = xmdAnims.GetAnimCycle(animInfo.m_TakeIndex);
  if (!isBindPose && xmdCycle)
  {
    // Double check that this take is the one we want to process.
    NMP_VERIFY_MSG(
      animInfo.m_TakeName == xmdCycle->GetName(),
      "Error compiling rig to anim entry maps: take name %s does not match cycle name %s",
      animInfo.m_TakeName,
      xmdCycle->GetName());

    //---------------------------------
    // First get a list of the bones that are present in both the rigs and the animations.
    // This defines the order of the animation set channels within the compiled animation.
    // NOTE: The sampled node id order from the XMD file may not be in hierarchy order.
    xmdCycle->GetSampledNodes(animatedNodes);
    
    const uint32_t numRigBones = conglomerateRig->getNumBones();
    for (uint32_t rigBoneIndex = 1; rigBoneIndex < numRigBones; ++rigBoneIndex)
    {
      const char* currentRigBoneName = conglomerateRig->getBoneName(rigBoneIndex);
      NMP_VERIFY(currentRigBoneName);

      // Loop through animation bone channels.
      XMD::XId animBoneID = 0xFFFFFFFF;
      const char* currentAnimBoneName = NULL;
      for (nodeIt = animatedNodes.begin(); nodeIt != animatedNodes.end(); ++nodeIt)
      {
        NMP_VERIFY_MSG(xmdCycle->GetBoneKeys(*nodeIt), "No bone keys on animated node %s", xmdCycle->GetName())

        // Get animation bone channel name.
        const XMD::XBase* currentAnimBoneNode = xmdAnims.FindNode(*nodeIt);
        if (currentAnimBoneNode)
        {
          currentAnimBoneName = currentAnimBoneNode->GetName();
          NMP_VERIFY(currentAnimBoneName);

          // Check to see if this anim bone name is present in the rig.
          if (strcmp(currentRigBoneName, currentAnimBoneName) == 0)
          {
            animBoneID = *nodeIt;
            break;
          }
        }
      }

      // There are some sampled key frames against this bone name in the source animation.
      if (animBoneID != -1 || addMissingChannels)
      {
        conglomerateRigBoneNamesIDsVector.push_back(rigBoneIndex);
        finalAnimBoneIDsVector.push_back(animBoneID);
      }
    }
  }
  else if (!isBindPose)
  {
    // if the cycle is NULL, then it's actually an XAnimationTake
    XMD::XAnimationTake* take = 0;
    XMD::XList takes;
    xmdAnims.List(takes, XMD::XFn::AnimationTake);
    NMP_VERIFY((animInfo.m_TakeIndex - xmdAnims.NumAnimCycles()) < takes.size());
    take = takes[animInfo.m_TakeIndex - xmdAnims.NumAnimCycles()]->HasFn<XMD::XAnimationTake>();

    const uint32_t numRigBones = conglomerateRig->getNumBones();
    for (uint32_t rigBoneIndex = 1; rigBoneIndex < numRigBones; ++rigBoneIndex)
    {
      const char* currentRigBoneName = conglomerateRig->getBoneName(rigBoneIndex);
      NMP_VERIFY(currentRigBoneName);
      
      // Loop through animation bone channels.
      XMD::XId animBoneID = 0xFFFFFFFF;
      const char* currentAnimBoneName = NULL;
      for (XMD::XU32 i = 0; i < take->GetNumberOfAnimatedNodes(); ++i)
      {
        const XMD::XAnimatedNode* nodeAnim = take->GetNodeAnimation(i);
        NMP_VERIFY(nodeAnim);
        NMP_VERIFY(nodeAnim->GetNode());

        // ignore anything that isn't a bone...
        if (nodeAnim->GetNode()->HasFn<XMD::XBone>())
        {
          currentAnimBoneName = nodeAnim->GetNode()->GetName();
          NMP_VERIFY(currentAnimBoneName);
          
          // Check to see if this anim bone name is present in the rig.
          if (strcmp(currentRigBoneName, currentAnimBoneName) == 0)
          {
            animBoneID = nodeAnim->GetNode()->GetID();
            break;                        
          }
        }
      }
      
      // There are some sampled key frames against this bone name in the source animation.
      if (animBoneID != -1 || addMissingChannels)
      {
        conglomerateRigBoneNamesIDsVector.push_back(rigBoneIndex);
        finalAnimBoneIDsVector.push_back(animBoneID);
      }
    }
  }
  else if (isBindPose)
  {
    XMD::XBoneList boneList;
    if (xmdAnims.GetBones(boneList))
    {
      const uint32_t numRigBones = conglomerateRig->getNumBones();
      for (uint32_t rigBoneIndex = 1; rigBoneIndex < numRigBones; ++rigBoneIndex)
      {
        const char* currentRigBoneName = conglomerateRig->getBoneName(rigBoneIndex);
        NMP_VERIFY(currentRigBoneName);

        // Loop through animation bone channels.
        XMD::XId animBoneID = 0xFFFFFFFF;
        const char* currentAnimBoneName = NULL;
        for (XMD::XBoneList::const_iterator it = boneList.begin(); it != boneList.end(); ++it)
        {
          // Get animation bone channel name.
          const XMD::XBase* currentAnimBoneNode = *it;
          if (currentAnimBoneNode)
          {
            currentAnimBoneName = currentAnimBoneNode->GetName();
            NMP_VERIFY(currentAnimBoneName);

            // Check to see if this anim bone name is present in the rig.
            if (strcmp(currentRigBoneName, currentAnimBoneName) == 0)
            {
              animBoneID = (*it)->GetID();
              break;
            }
          }
        }

        // There are some sampled key frames against this bone name in the source animation.
        if (animBoneID != 0xFFFFFFFF || addMissingChannels)
        {
          conglomerateRigBoneNamesIDsVector.push_back(rigBoneIndex);
          finalAnimBoneIDsVector.push_back(animBoneID);
        }
      }    
    }
  }

  //---------------------------------
  // Check that this animation and rig share at least one common bone.
  uint32_t numAnimChannels = (uint32_t)finalAnimBoneIDsVector.size();
  if(numAnimChannels == 0)
  {
    g_acGlobals.m_errorLogger->output(
      "Error compiling rig to anim entry maps: Animation and rig do not share any bones");
  }

  //---------------------------------
  // Build a RigToAnimEntryMap for each rig in the compilation vector.
  for (uint32_t rigIndex = 0; rigIndex < rigsVector.size(); ++rigIndex)
  {
    std::vector<uint32_t> mapRigBoneIndexes;
    std::vector<uint32_t> mapAnimBoneIndexes;
    const MR::AnimRigDef* currentRig = rigsVector[rigIndex];
    NMP_VERIFY(currentRig);

    // For a strictly increasing set of rig bone indexes, find the corresponding
    // animation channel indexes that these rig bones map to.
    for (uint32_t rigBoneIndex = 1; rigBoneIndex < currentRig->getNumBones(); ++rigBoneIndex)
    {
      const char* currentRigBoneName = currentRig->getBoneName(rigBoneIndex);
      NMP_VERIFY(currentRigBoneName);
      
      // Find the corresponding animation channel index
      for (uint32_t animChannelIndex = 0; animChannelIndex < numAnimChannels; ++animChannelIndex)
      {
        uint32_t finalRigBoneIndex = conglomerateRigBoneNamesIDsVector[animChannelIndex];
        const char* finalRigBoneName = conglomerateRig->getBoneName(finalRigBoneIndex);
        NMP_VERIFY(finalRigBoneName);

        if (strcmp(currentRigBoneName, finalRigBoneName) == 0)
        {
          mapRigBoneIndexes.push_back(rigBoneIndex); // Strictly increasing
          mapAnimBoneIndexes.push_back(animChannelIndex); // Any order
          break;
        }
      }
    }

    //---------------------------------
    // Actually make the RigToAnimMap for this rig.
    uint32_t numMapEntries = (uint32_t) mapRigBoneIndexes.size();
    NMP::Memory::Format mapMemFmt = MR::RigToAnimEntryMap::getMemoryRequirements(numMapEntries);
    NMP::Memory::Resource rigToAnimMapMemResource = NMPMemoryAllocateFromFormat(mapMemFmt);
    ZeroMemory(rigToAnimMapMemResource.ptr, mapMemFmt.size);
    MR::RigToAnimEntryMap* newRigToAnimEntryMap = MR::RigToAnimEntryMap::init(rigToAnimMapMemResource, numMapEntries);

    for (uint32_t mapIndex = 0; mapIndex < numMapEntries; ++mapIndex)
    {
      uint16_t rigChannelIndex = (uint16_t) mapRigBoneIndexes[mapIndex];
      uint16_t animChannelIndex = (uint16_t) mapAnimBoneIndexes[mapIndex];
      newRigToAnimEntryMap->setEntryAnimChannelIndex(mapIndex, animChannelIndex);
      newRigToAnimEntryMap->setEntryRigChannelIndex(mapIndex, rigChannelIndex);
    }

    // Add the new RigToAnimEntryMap to the output vector.
    rigToAnimEntryMaps.push_back(newRigToAnimEntryMap);

    // Wipe down ready for the next rig
    mapRigBoneIndexes.clear();
    mapAnimBoneIndexes.clear();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void createRigToAnimEntryMap(
  const std::vector<uint32_t>& finalRigBoneNamesIDsVector,
  MR::RigToAnimEntryMap*& finalRigToAnimEntryMap)
{
  uint32_t numMapEntries = (uint32_t) finalRigBoneNamesIDsVector.size();
  
  NMP::Memory::Format mapMemFmt = MR::RigToAnimEntryMap::getMemoryRequirements(numMapEntries);
  NMP::Memory::Resource rigToAnimMapMemResource = NMPMemoryAllocateFromFormat(mapMemFmt);
  ZeroMemory(rigToAnimMapMemResource.ptr, mapMemFmt.size);
  finalRigToAnimEntryMap = MR::RigToAnimEntryMap::init(rigToAnimMapMemResource, numMapEntries);

  for (uint32_t mapIndex = 0; mapIndex < numMapEntries; ++mapIndex)
  {
    uint16_t rigChannelIndex = (uint16_t) finalRigBoneNamesIDsVector[mapIndex];
    uint16_t animChannelIndex = (uint16_t) mapIndex;
    finalRigToAnimEntryMap->setEntryAnimChannelIndex(mapIndex, animChannelIndex);
    finalRigToAnimEntryMap->setEntryRigChannelIndex(mapIndex, rigChannelIndex);
  }

  // Sort the rig to anim map so that the rig channels are strictly increasing.
  // This shouldn't be necessary since the rig bone ids vector should already be in
  // increasing order??
  finalRigToAnimEntryMap->sortByRigChannels();
}

//----------------------------------------------------------------------------------------------------------------------
bool checkCharacterWorldSpaceTMIsNotAnimated(const std::vector<const MR::RigToAnimEntryMap*>& rigToAnimEntryMaps)
{
  std::vector<const MR::RigToAnimEntryMap*>::const_iterator ram_it = rigToAnimEntryMaps.begin();
  std::vector<const MR::RigToAnimEntryMap*>::const_iterator ram_itend = rigToAnimEntryMaps.end();
  for (; ram_it != ram_itend; ++ram_it)
  {
    const MR::RigToAnimEntryMap* entryMap = *ram_it;
    NMP_VERIFY_MSG(entryMap, "Missing rig to anim entry map");

    // Make sure CharacterWorldSpaceTM rig index is not in the rig to anim map
    uint32_t entryIndex;
    if (entryMap->getNumEntries() > 0 && entryMap->findEntryIndexForRigChannelIndex(0, entryIndex))
    {
      return false;
    }
  }

  return true; // CharacterWorldSpaceTM is not animated
}

//----------------------------------------------------------------------------------------------------------------------
/// \fn
/// \brief
/// \ingroup AssetCompilerModule
NMP::Memory::Resource processAnimation(
  AP::AssetProcessor*                       assetProc,
  const acAnimInfo&                         animInfo,
  const XMD::XModel&                        xmdAnims,
  const char*                               animFileName,
  const char*                               takeName,
  const char*                               animFormatType,
  const char*                               animOptions,
  const float                               animationScale,
  const std::vector<const char*>&           NMP_UNUSED(rigNames),     // in sorted rig name order
  const std::vector<const MR::AnimRigDef*>& rigs,                     // rigs to compile the animation against (in sorted rig name order)
  const MR::AnimRigDef*                     conglomerateRig,          // conglomerate rig
  const std::vector<const char*>&           NMP_UNUSED(rigBoneNames), // unused, duplicated in the conglomerate rig
  const float                               averageRigBoneLength,     // average bone length for detecting unchanging position channels
  std::vector<MR::RigToAnimMap*>&           rigToAnimMaps,            // OUT: compiled RigToAnimMaps (in same order as input rigs)
  float&                                    resultantCompression)     // OUT: compression metric
{
  NMP::Memory::Resource result;
  std::vector<const MR::RigToAnimEntryMap*> rigToAnimEntryMaps;
  std::vector<uint32_t> conglomerateRigBoneNamesIDs;
  std::vector<XMD::XId> finalAnimBoneIDs;
  NMP::Timer            macProfilingTimer;

  macProfilingTimer.start();

  // Generate the rig bone index to anim channel index maps and return the bone IDs needed from the animation.
  // finalAnimBoneIDs contains the corresponding XMD bone ids required for the runtime animation channels.
  //
  // Note that the conglomerate rig defines the order in which the animation channels will appear in the
  // final compiled animation. The bone entries in the conglomerate rig are ordered according to the
  // bone weighting in the rig set (number of rigs the bone appears in) so that animations compiled against
  // the rig set only need to partially decompress the animation channels depending on the LOD.
  createRigToAnimEntryMaps(
    assetProc,                    // IN:  The asset processor
    xmdAnims,                     // IN:  The XMD anim structure containing the animation data.
    animInfo,                     // IN:  Information about the anim and take that we are interested in.
    rigs,                         // IN:  The set of rigs to create the Rig and anim channel entry maps for.
    conglomerateRig,              // IN:  The conglomerate rig that defines the order of the animation channels.
    false,                        // IN:  Add missing animation channels with the bind pose transforms.
    conglomerateRigBoneNamesIDs,  // OUT: Indices of the bones in the conglomerate rig that are used in the animation
                                  //      (in conglomerate rig order).
    finalAnimBoneIDs,             // OUT: IDs of the XMD data channels that are used in the animation
                                  //      (in conglomerate rig order). Channels that are missing from the animation
                                  //      are set to -1.
    rigToAnimEntryMaps            // OUT: Rig and anim channel entry maps corresponding to each of the rigs
                                  //      (in same order as input rigs).
    );

  // Check that the CharacterWorldSpaceTM rig bone is not animated.
  bool callSuccess = checkCharacterWorldSpaceTMIsNotAnimated(rigToAnimEntryMaps);
  NMP_VERIFY_MSG(
    callSuccess,
    "Animation '%s' take '%s': CharacterWorldSpaceTM rig bone should not be animated.\n",
    animFileName,
    takeName);

  // Build the RigToAnimEntryMap corresponding to the hierarchy of the conglomerated set of bones.
  // We pass this to the animation compressor so that the full mapping of rig bones to animation channels
  // is available during the compilation process.
  MR::RigToAnimEntryMap* conglomerateRigToAnimEntryMap = NULL;
  createRigToAnimEntryMap(
    conglomerateRigBoneNamesIDs,    // IN: rig channel indices in the conglomerate rig (for the animation channels)
    conglomerateRigToAnimEntryMap   // OUT: the RigToAnimEntryMap for the conglomerate rig
    );
  NMP_VERIFY(conglomerateRigToAnimEntryMap);

  g_macProcessAnimFuncElapsedTimeRigToAnimMaps += macProfilingTimer.stop();

  //---------------------
  // Pre-process the animation into the format required by morpheme runtime
  macProfilingTimer.start();

  // Compute the unchanging channel numerical tolerances
  AnimSourceUncompressedOptions animSourceOptions;
  const float posEps = NMP::maximum(0.0001f * averageRigBoneLength, 0.0001f);
  const float quatEps = 0.0001f;
  animSourceOptions.setUnchangingChannelPosEps(posEps);
  animSourceOptions.setUnchangingChannelQuatEps(quatEps);

  // Build the uncompressed animation source
  AnimSourceUncompressed* uncompressedAnim = AnimSourceUncompressedBuilderXMD::init(
    xmdAnims,
    animInfo,
    finalAnimBoneIDs,
    conglomerateRig,
    conglomerateRigToAnimEntryMap,
    animationScale,
    animSourceOptions);

  g_macProcessAnimFuncElapsedTimeBuildUncompressedAnims += macProfilingTimer.stop();

  //---------------------
  // Compress the animation
  macProfilingTimer.start();

  resultantCompression = -1.0f;
  std::string errorMessage;

  if (!strcmp("nsa", animFormatType))
  {
    // Convert the basic uncompressed animation into a NSA compressed animation
    AnimSourceCompressorNSA nsaCompressor;
    result = nsaCompressor.compressAnimation(
               uncompressedAnim,
               animFileName,
               takeName,
               &animInfo,
               animOptions,
               conglomerateRig,
               conglomerateRigToAnimEntryMap,
               &rigs,
               &rigToAnimEntryMaps,
               &rigToAnimMaps,
               assetProc->getMessageLogger(),
               assetProc->getErrorLogger());
    if (result.ptr)
    {
      resultantCompression = nsaCompressor.getCompressionRate();
      MR::AnimSourceNSA* anim = (MR::AnimSourceNSA*)result.ptr;
      anim->dislocate();
    }
    else
    {
      errorMessage = "NSA animation compression failed";
    }
  }
  else if (!strcmp("qsa", animFormatType))
  {
    // Convert the basic uncompressed animation into a QSA compressed animation
    AnimSourceCompressorQSA qsaCompressor;
    result = qsaCompressor.compressAnimation(
               uncompressedAnim,
               animFileName,
               takeName,
               &animInfo,
               animOptions,
               conglomerateRig,
               conglomerateRigToAnimEntryMap,
               &rigs,
               &rigToAnimEntryMaps,
               &rigToAnimMaps,
               assetProc->getMessageLogger(),
               assetProc->getErrorLogger());
    if (result.ptr)
    {
      resultantCompression = qsaCompressor.getCompressionRate();
      MR::AnimSourceQSA* anim = (MR::AnimSourceQSA*)result.ptr;
      anim->dislocate();
    }
    else
    {
      errorMessage = "QSA animation compression failed";
    }
  }
  else if (!strcmp("asa", animFormatType))
  {
    // Convert the basic uncompressed animation into a ASA animation
    AnimSourceCompressorASA asaCompressor;
    result = asaCompressor.compressAnimation(
               uncompressedAnim,
               animFileName,
               takeName,
               &animInfo,
               animOptions,
               conglomerateRig,
               conglomerateRigToAnimEntryMap,
               &rigs,
               &rigToAnimEntryMaps,
               &rigToAnimMaps,
               assetProc->getMessageLogger(),
               assetProc->getErrorLogger());
    if (result.ptr)
    {
      resultantCompression = asaCompressor.getCompressionRate();
      MR::AnimSourceASA* anim = (MR::AnimSourceASA*)result.ptr;
      anim->dislocate();
    }
    else
    {
      errorMessage = "ASA animation compression failed";
    }
  }
  else if (!strcmp("mba", animFormatType))
  {
    // Convert the basic uncompressed animation into a basic MBA animation
    AnimSourceCompressorMBA mbaCompressor;
    result = mbaCompressor.compressAnimation(
               uncompressedAnim,
               animFileName,
               takeName,
               &animInfo,
               animOptions,
               conglomerateRig,
               conglomerateRigToAnimEntryMap,
               &rigs,
               &rigToAnimEntryMaps,
               &rigToAnimMaps,
               assetProc->getMessageLogger(),
               assetProc->getErrorLogger());
    if (result.ptr)
    {
      resultantCompression = mbaCompressor.getCompressionRate();
      MR::AnimSourceMBA* anim = (MR::AnimSourceMBA*)result.ptr;
      anim->dislocate();
    }
    else
    {
      errorMessage = "MBA animation compression failed";
    }
  }
  else
  {
    result.ptr = NULL;
    errorMessage = "Trying to compile unknown animation format";
  }

  //---------------------
  // Tidy up the memory for the uncompressed animation
  NMP::Memory::memFree(uncompressedAnim);
  NMP::Memory::memFree(conglomerateRigToAnimEntryMap);
  for (uint32_t i = 0; i < (uint32_t)rigToAnimEntryMaps.size(); ++i)
  {
    NMP::Memory::memFree((void*) rigToAnimEntryMaps[i]);
  }

  // Check for a compilation error
  if (!result.ptr)
  {
    NMP_THROW_ERROR(errorMessage.c_str());
  }
  
  // Check that all required rig to anim maps were compiled    
  NMP_VERIFY_MSG(
    rigs.size() == rigToAnimMaps.size(),
    "Animation '%s' take '%s': Failed to compile the required number of RigToAnimMaps.\n",
    animFileName,
    takeName);

  g_macProcessAnimFuncElapsedTimeBuildCompressedAnims += macProfilingTimer.stop();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool processAnimationLookupTable()
{
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* animIDtoFilename = g_animIDtoFilename->generateAnimRuntimeIDtoFilenameLookup();

  MR::Manager::AssetType animLookupType = (MR::Manager::AssetType)MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup::kAsset_SimpleAnimRuntimeIDtoFilenameLookup;
  const char* animLookupName = "SimpleAnimRuntimeIDtoFilenameLookup";
  const char* animLookupGUID = "83010059-9745-4daa-AE2A-115094D22118";
  uint32_t animLookupAssetID = macGenUIDFunc(animLookupName, animLookupGUID);
  g_animIDtoFilename->registerAnimLookupAssetID(animLookupName, animLookupGUID, animLookupAssetID);

  if (!MR::Manager::getInstance().objectIsRegistered(animLookupAssetID))
  {
    if (!MR::Manager::getInstance().registerObject(
      animIDtoFilename,   // Compiled asset structure.
      animLookupType,     //
      animLookupAssetID)) // Unique runtime id.
    {
      // Registration failed for some reason
      NMP_DEBUG_MSG("   !! Registration Failed !!.\n");
    }
    else
    {
      // Increment the reference counter for this object.
      MR::Manager::incObjectRefCount(animLookupAssetID);
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool serializeAnimationLookupTable()
{
  uint32_t animLookupAssetID = g_animIDtoFilename->getAnimLookupAssetID();
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* animIDtoFilename =
    (MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup*)MR::Manager::getInstance().getObjectPtrFromObjectID(animLookupAssetID);
  if (animIDtoFilename)
  {
    NMP::Memory::Format animLookupMemReqs = animIDtoFilename->getInstanceMemoryRequirements();
    MR::Manager::AssetType animLookupType = MR::Manager::getInstance().getAssetTypeFromObjectPtr(animIDtoFilename);
    const char* animLookupGUID = g_animIDtoFilename->getAnimLookupGUID();

    MR::Manager::decObjectRefCount(animLookupAssetID);
    animIDtoFilename->dislocate();

    g_bundleWriter->writeAsset(
      animLookupType,
      animLookupAssetID,
      (uint8_t*)animLookupGUID,
      (void*)animIDtoFilename,
      animLookupMemReqs);

    NMP::Memory::memFree(animIDtoFilename);

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void serializePluginList(const PluginManager& pm, AssetProcessor& ap)
{
  const uint32_t pluginCount = pm.getPluginCount();

  // Create a list of plugins to initialise the string table
  NMP::Memory::Format fmt(sizeof(char*) * pluginCount, NMP_NATURAL_TYPE_ALIGNMENT);
  const char** pluginsNameList = (const char**)NMPMemoryAllocateFromFormat(fmt).ptr;

  fmt = NMP::Memory::Format(sizeof(uint32_t) * pluginCount, NMP_NATURAL_TYPE_ALIGNMENT);

  for (uint32_t i = 0; i < pluginCount; ++i)
  {
    pluginsNameList[i] = pm.getPluginName(i);
  }

  // Create the string table and dislocate
  fmt = NMP::OrderedStringTable::getMemoryRequirements(pluginCount, pluginsNameList);
  NMP::Memory::Resource stringTableResource = NMPMemoryAllocateFromFormat(fmt);
  NMP::OrderedStringTable* stringTable = NMP::OrderedStringTable::init(stringTableResource, pluginCount, pluginsNameList);
  stringTable->dislocate();

  const char* guid = "00000000-0000-0000-0000-000000000000";
  const uint32_t runtimeID = ap.getGenUIDFunc()("BundleCompilerList", "BundleCompilerList");
  g_bundleWriter->writeAsset(MR::Manager::kAsset_PluginList, runtimeID, (uint8_t*)guid, stringTable, fmt);

  // Free allocated resources
  NMP::Memory::memFree(stringTable);
  NMP::Memory::memFree(pluginsNameList);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Load an animation as a memory resource, populate the cache at the same time.
NMP::Memory::Resource loadAndCacheAnimation(
  AP::AssetProcessor*                                   assetProc,
  const AP::AssetCompilerPlugin::AnimationFileMetadata* animData, ///< IN: animation filename, takename, formattype and options
  const AP::AssetCompilerPlugin::AnimationFileRigData*  rigData,  ///< IN: rig bone names, rignames and binary animation rig definition
  AnimCache::Key::NameAndGUIDVector&                    rigNameGuidsVector,
  std::vector<MR::RigToAnimMap*>&                       rigToAnimMaps,
  float                                                 compression,
  uint32_t*                                             animResourceFileCRC)
{
  NMP::Memory::Resource animResource;
  animResource.ptr = NULL;

  XMD::XModel   xmdAnims;
  acAnimInfo*   animInfo = NULL;

  NMP_VERIFY(assetProc);
  NMP_VERIFY(animData);
  NMP_VERIFY(rigData);
  NMP_VERIFY(animResourceFileCRC);

  // verify all rig scale factors are the same for this animation
  float rigScaleFactor = rigData->m_rigScaleFactors[0];
  NMP_VERIFY_MSG(rigScaleFactor > 0.0f, "Animation referenced by rig(s) with scale factor less than or equal to 0.0f.");
  for (size_t i = 1; i < rigData->m_rigScaleFactors.size(); ++i)
  {
    NMP_VERIFY_MSG(rigScaleFactor == rigData->m_rigScaleFactors[i], "Animation referenced by rigs with different scale factors.");
  }

  // calculate the value needed to scale the animation into game world units.
  float animationScale = rigScaleFactor / assetProc->getRuntimeAssetScaleFactor();
  char animationScaleStr[MAX_PATH];
  NMP_SPRINTF(animationScaleStr, MAX_PATH, "%f", animationScale);

  // 
  char maxAnimSectionSizeStr[MAX_PATH];
  NMP_SPRINTF(maxAnimSectionSizeStr, MAX_PATH, "%d",  MR::getMaxAnimSectionSize());

  char* fullAnimFileName = generateFullSourceAnimFileName(animData->m_animFileName);

  // Load the source animation file into an XMD structure.
  NMP::Timer macProfilingTimer;
  macProfilingTimer.start();

  bool callSuccess = loadSourceAnim(xmdAnims, fullAnimFileName);
  NMP_VERIFY_MSG(
    callSuccess,
    "Failed to load source animation file %s into XMD structure.\n",
    fullAnimFileName);

  g_macProcessAnimFuncElapsedTimeLoadXMDAnims += macProfilingTimer.stop();

  acAnimInfos animInfos; // Will be filled in by the following call
  assetProc->listTakesInXMDAnimData(xmdAnims, &animInfos);

  // If we are writing a file to the browser then loop through all the takes in the animation and cache them all.
  if (g_animCache && g_acGlobals.m_copyToAnimBrowserOutFile)
  {
    std::vector<MR::RigToAnimMap*> temporaryRigToAnimMaps;

    for (acAnimInfos::iterator it = animInfos.begin(), end = animInfos.end(); it != end; ++it)
    {
      temporaryRigToAnimMaps.clear();
      acAnimInfo& info = *it;

      AnimCache::Key animCacheKey(
                      fullAnimFileName, 
                      info.m_TakeName, 
                      animData->m_animFormatType, 
                      animData->m_animOptions, 
                      animationScaleStr, 
                      rigNameGuidsVector,
                      maxAnimSectionSizeStr);

      bool isTargetEntry = strcmp(info.m_TakeName.c_str(), animData->m_animTakeName) == 0;
      std::vector<MR::RigToAnimMap*>& targetAnimMaps = isTargetEntry ? rigToAnimMaps : temporaryRigToAnimMaps;

      NMP::Memory::Resource result = processAnimation(
                                        assetProc,
                                        info,
                                        xmdAnims,
                                        animData->m_animFileName,
                                        info.m_TakeName.c_str(),
                                        animData->m_animFormatType,
                                        animData->m_animOptions,
                                        animationScale,
                                        rigData->m_rigNames,
                                        rigData->m_rigs,
                                        rigData->m_conglomerateRig,
                                        rigData->m_boneNames,
                                        rigData->m_averageRigBoneLength,
                                        targetAnimMaps,
                                        compression);
      NMP_VERIFY_MSG(result.ptr != NULL, "Animation processing failed");
      uint32_t animFileCRC = NMP::hashCRC32(result.ptr, result.format.size);
      g_animCache->addEntry(animCacheKey, targetAnimMaps, compression, animFileCRC, result);

      if (isTargetEntry)
      {
        animResource = result;
        animInfo = &info;
        *animResourceFileCRC = animFileCRC;
      }
      else
      {
        NMP::Memory::memFree(result.ptr);
      }

      // Update the number of compiled animations
      g_macProcessAnimFuncNumCompiledAnims++;
    }

    NMP_VERIFY_MSG(
                animInfo,
                "Failure to find animation take %s in source XMD data for anim %s",
                animData->m_animTakeName,
                fullAnimFileName);
  }
  else
  {
    animInfo = findTake(animData->m_animTakeName, animInfos);

    NMP_VERIFY_MSG(
                animInfo,
                "Failure to find animation take %s in source XMD data for anim %s",
                animData->m_animTakeName,
                fullAnimFileName);

    NMP::Memory::Resource result = processAnimation(
                                      assetProc,
                                      *animInfo,
                                      xmdAnims,
                                      animData->m_animFileName,
                                      animData->m_animTakeName,
                                      animData->m_animFormatType,
                                      animData->m_animOptions,
                                      animationScale,
                                      rigData->m_rigNames,
                                      rigData->m_rigs,
                                      rigData->m_conglomerateRig,
                                      rigData->m_boneNames,
                                      rigData->m_averageRigBoneLength,
                                      rigToAnimMaps,
                                      compression);
    NMP_VERIFY_MSG(result.ptr != NULL, "Animation processing failed");
    *animResourceFileCRC = NMP::hashCRC32(result.ptr, result.format.size);

    // Animation has been processed so add it to the cache. Will replace it if it exists already.
    if (g_animCache)
    {
      AnimCache::Key animCacheKey(
        fullAnimFileName, 
        animData->m_animTakeName, 
        animData->m_animFormatType, 
        animData->m_animOptions, 
        animationScaleStr, 
        rigNameGuidsVector,
        maxAnimSectionSizeStr);

      g_animCache->addEntry(animCacheKey, rigToAnimMaps, compression, *animResourceFileCRC, result);
    }

    animResource = result;

    // Update the number of compiled animations
    g_macProcessAnimFuncNumCompiledAnims++;
  }

  if (fullAnimFileName)
  {
    NMP::Memory::memFree(fullAnimFileName);
  }
  return animResource;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Convert source animation data file into runtime format and return an id.
/// \ingroup AssetCompilerModule
///
/// The id used at runtime for mr to locate animation data by calling back to the application
/// (The AnimLoaderFunction passed into mr::initMorphemeLib).
bool macProcessAnimFunc(
  AP::AssetProcessor*                assetProc,                   ///<
  const AP::AssetCompilerPlugin::AnimationFileMetadata* animData, ///< IN: animation filename, takename, formattype and options
  const AP::AssetCompilerPlugin::AnimationFileRigData*  rigData,  ///< IN: rig bone names, rignames and binary animation rig definition
  const ME::GUID                     rigToAnimMapGUIDbasis,       ///< IN: GUID used as a basis for generation of unique
                                                                  ///< RigToAnimMap GUIDs.
  MR::RuntimeAnimAssetID&            animAssetID,                 ///< OUT: Runtime identifier for the animation.
  std::vector<MR::RuntimeAssetID>&   rigToAnimMapAssetIDs         ///< OUT: Asset IDs of generated rig to anim maps from this
                                                                  ///< anim to each rig.
)
{
  // is there global meta data available? if yes, use this instead of the AssetProcessor input
  if (g_animMetaData != NULL)
  {
    for (size_t i = 0; i < g_animMetaData->animFiles.size(); ++i)
    {
      if (g_animMetaData->animFiles[i] == *animData)
      {
        animData = &g_animMetaData->animFiles[i];
        g_animMetaData->animRigdata[i].alignTo(*rigData);
        rigData = &g_animMetaData->animRigdata[i];
        break;
      }
    }
  }

  size_t stringLength;

  NMP_VERIFY(animData && rigData);
  NMP_VERIFY(animData->m_animFileName && animData->m_animTakeName && assetProc);
  NMP_VERIFY(rigData->m_rigs.size() > 0);

  // Generate lexicographically sorted vector of rig names and guids.
  AnimCache::Key::NameAndGUIDVector rigNameGuidsVector;
  generateRigNameGuidsVector(assetProc, rigData->m_rigs, rigData->m_rigNames, rigNameGuidsVector);

  // verify all rig scale factors are the same for this animation
  float rigScaleFactor = rigData->m_rigScaleFactors[0];
  NMP_VERIFY_MSG(rigScaleFactor > 0.0f, "Animation referenced by rig(s) with scale factor less than or equal to 0.0f.");
  for (size_t i = 1; i < rigData->m_rigScaleFactors.size(); ++i)
  {
    NMP_VERIFY_MSG(rigScaleFactor == rigData->m_rigScaleFactors[i], "Animation referenced by rigs with different scale factors.");
  }

  // calculate the value needed to scale the animation into game world units.
  float animationScale = rigScaleFactor / assetProc->getRuntimeAssetScaleFactor();
  char animationScaleStr[MAX_PATH];
  NMP_SPRINTF(animationScaleStr, MAX_PATH, "%f", animationScale);

  char* fullAnimFileName = generateFullSourceAnimFileName(animData->m_animFileName);
  NMP_VERIFY(NMP::NMFile::getExists(fullAnimFileName));

  char maxAnimSectionSizeStr[MAX_PATH];
  NMP_SPRINTF(maxAnimSectionSizeStr, MAX_PATH, "%d",  MR::getMaxAnimSectionSize());

  AnimCache::Key animCacheKey(fullAnimFileName, animData->m_animTakeName, animData->m_animFormatType, animData->m_animOptions, animationScaleStr, rigNameGuidsVector, maxAnimSectionSizeStr);

  // A buffer to hold a pointer to the memory for the anim.
  NMP::Memory::Resource animResource;
  std::vector<MR::RigToAnimMap*> rigToAnimMaps;
  float compression = -1.0f;
  bool cachedDataIsValid = false;
  uint32_t animFileCRC = 0;
  if (g_animCache)
  {
    const AnimCache::Entry* cacheEntry = ((const AnimCache*)g_animCache)->getEntryForKey(animCacheKey);

    if (cacheEntry)
    {
      // An entry for this animation exists in the cache.
      if (cacheEntry->isUpToDate())
      {
        animFileCRC = cacheEntry->getFileCRC();
        // more of a backwards-compatibility mode to make sure that the file CRC is available and gets written to the
        // the id map later
        if (animFileCRC == 0)
        {
          cachedDataIsValid = false;
        }
        else
        {
          // None of the associated rigs have changed (ie they all have the same guids).
          cachedDataIsValid = true;
          animResource = g_animCache->getAnimation(animCacheKey);
          rigToAnimMaps = g_animCache->getRigToAnimMaps(animCacheKey);
          compression = g_animCache->getCompression(animCacheKey);
        }

        // The rigNameGuidsVector is sorted by GUID - which is a problem. We need to adjust the rigToAnimMaps
        // now to match the input data.
        if (g_animMetaData)
        {
          for (size_t iRigData = 0; iRigData < rigData->m_rigNames.size(); ++iRigData)
          {
            if (strcmp(rigNameGuidsVector[iRigData].first.c_str(), rigData->m_rigNames[iRigData]) != 0)
            {
              for (size_t iRigSwap = iRigData + 1; iRigSwap < rigNameGuidsVector.size(); ++iRigSwap)
              {
                if (strcmp(rigNameGuidsVector[iRigSwap].first.c_str(), rigData->m_rigNames[iRigData]) == 0)
                {
                  // thanks to the global nature of the contained data, we can simply swap the pointers
                  MR::RigToAnimMap* swap = rigToAnimMaps[iRigSwap];
                  rigToAnimMaps[iRigSwap] = rigToAnimMaps[iRigData];
                  rigToAnimMaps[iRigData] = swap;

                  break;
                }
              }
            }
          }
        }
      }
    }
  }

  //--------------------
  // The cache either does not have an entry for this anim rigs combo or it is out of date.
  // Either way process the animation and rigs.
  if (!cachedDataIsValid)
  {
    animResource = loadAndCacheAnimation(
      assetProc, 
      animData,
      rigData,
      rigNameGuidsVector,
      rigToAnimMaps,
      compression,
      &animFileCRC
     );
  }

  // We now have this animation so use an incrementing ID for it.
  {
    static uint32_t assetIndex = 0;
    animAssetID = assetIndex;
    ++assetIndex;

    // The file name is designed to uniquely identify an animation - it is unique for:
    //   - Animation file (animFileName)
    //   - Animation take (takeName)
    //   - Animation options (animOptions)
    //   - Animation format (animFormatType)
    //   - The current platform (NM_PLATFORM_FORMAT_STRING)
    //   - Which rigs reference it
    //
    // The file name is used as a unique key on the runtime - so that if two different
    // files somehow share the same key (possibly across different runs of a network as it
    // is edited) then there will be problems.
    //
    // The key is generated by combining all of this information into a string and
    // generating a 32 bit hash from it. This hash is then included within the file name.

    // Calculate the length of the string needed to hold the base information excluding the rig names and guids part.
    stringLength =
      strlen(animData->m_animFileName) +
      strlen(animData->m_animTakeName) +
      strlen(animData->m_animOptions) +
      strlen(animData->m_animFormatType) +
      strlen(animationScaleStr) +
      strlen(NM_PLATFORM_FORMAT_STRING);

    // Calculate the length of the rig name and guid string for each rig in the vector
    size_t rigNameGuidsLength = 1; // include the null terminator character.
    {
      AnimCache::Key::NameAndGUIDVector::const_iterator it = rigNameGuidsVector.begin();
      AnimCache::Key::NameAndGUIDVector::const_iterator end = rigNameGuidsVector.end();
      while (it != end)
      {
        // make sure there is room for ("|" + "c:/DefaultRig.mcarig" + "|" + guid)
        rigNameGuidsLength += it->first.size() + it->second.size() + 2;
        ++it;
      }
    }

    // allocate the memory required for the whole asset name
    char* assetName = (char*) NMPMemoryAlloc(stringLength + rigNameGuidsLength);
    NMP_ASSERT(assetName);
    // fill in the first part of the asset name string
    int32_t length = NMP_SPRINTF(
                       assetName,
                       stringLength + rigNameGuidsLength,
                       "%s_%s_%s_%s_%s_%s",
                       animData->m_animFileName,
                       animData->m_animTakeName,
                       animData->m_animOptions,
                       animData->m_animFormatType,
                       animationScaleStr,
                       NM_PLATFORM_FORMAT_STRING);

    // now add the rig name + guid part of the name
    {
      // offset current to the end of the new string
      char* current = assetName + length;
      // fill the string with the first part of the string not including rig information
      const char* assetNameEnd = current + rigNameGuidsLength + 1;

      // append all the rig names + guids to the asset name
      AnimCache::Key::NameAndGUIDVector::const_iterator it = rigNameGuidsVector.begin();
      AnimCache::Key::NameAndGUIDVector::const_iterator end = rigNameGuidsVector.end();
      while (it != end)
      {
        // append "|" + name + "|" guid to the current asset name
        length = NMP_SPRINTF(current, assetNameEnd - current, "%s", it->second.c_str());
        // move current to be at the end of the new string
        current += length;
        ++it;
      }
    }

    // Create a hash from this string
    uint32_t hashID =  NMP::hashStringCRC32(assetName);

    NMP::Memory::memFree(assetName);

    // Strip the path and extension from the source animation filename
    stringLength = strlen(animData->m_animFileName) + 1;
    char* buffer = (char*) NMPMemoryAlloc(stringLength);
    NMP_ASSERT(buffer);
    NMP_STRNCPY_S(buffer, stringLength, animData->m_animFileName);
    char* sourcefile = PathFindFileNameA(buffer);
    PathRemoveExtensionA(sourcefile);

    // Construct a filename for the runtime animation
    std::string outputFileString;
    std::string humanReadable = sourcefile;
    humanReadable += "_";
    humanReadable += animData->m_animTakeName;
    char identifierBuf[9];
    NMP_SPRINTF(identifierBuf, "%x", hashID);
    std::string identifier = identifierBuf;
    std::string extension = animData->m_animFormatType;
    if (humanReadable.length() + identifier.length() + extension.length() + 2 > MAX_ANIMATION_FILENAME_LENGTH)
    {
      // trim some of the human readable part to fit.
      size_t trim = MAX_ANIMATION_FILENAME_LENGTH - (identifier.length() + extension.length() + 2);
      humanReadable.erase(humanReadable.begin() + trim, humanReadable.end());
    }
    outputFileString = humanReadable + "_" + identifier + "." + extension;
    const char* outputFileName = outputFileString.c_str();

    g_animIDtoFilename->addAnimation(animData->m_animFileName, animData->m_animTakeName,
                                     animData->m_animFormatType, animAssetID, animFileCRC, outputFileName);

    stringLength = strlen(g_acGlobals.m_outputDir) + strlen(outputFileName) + 10;
    char* outputFilePath = (char*) NMPMemoryAlloc(stringLength);
    NMP_ASSERT(outputFilePath);
    NMP_SPRINTF(outputFilePath, stringLength, "%s\\%s", g_acGlobals.m_outputDir, outputFileName);

    // the instance will be recycled later anyway, so we just allocate it here
    NMP::Memory::Resource animCopy = NMPMemoryAllocateFromFormat(animResource.format);

    // check if the output-file needs to be written
    // this looks a bit strange, but normally writing a new file is taking much longer than reading, and there is
    // enough processing power to do the hashing meanwhile.
    bool saveFile = true;
    if (NMP::NMFile::load(outputFilePath, animCopy.ptr, animResource.format.size) == (int64_t)animResource.format.size)
    {
      //file exists, check modify time!
      uint32_t checkFileCRC = NMP::hashCRC32(animCopy.ptr, animCopy.format.size);
      saveFile = (animFileCRC != checkFileCRC);
    }

    if (saveFile && (NMP::NMFile::save(outputFilePath, animResource.ptr, animResource.format.size) <= 0))
    {
      // File write failed
      NMP::Memory::memFree(animResource.ptr);
      NMP_THROW_ERROR("Saving of animation file %s failed", outputFilePath);
    }

    //create caching copy
    NMP::Memory::memcpy(animCopy.ptr, animResource.ptr, animResource.format.size);
    g_AnimationSourceCache->addAnimSource(animAssetID, (MR::AnimSourceBase*)animCopy.ptr);

    ASSET_COMP_LOG_MSG(
      "Animation file '%s' take '%s' compiled into binary '%s' @ %4.1f bytes/bone/sec \n",
      animData->m_animFileName,
      animData->m_animTakeName,
      outputFileName,
      compression);

    if (buffer)
      NMP::Memory::memFree(buffer);
    if (outputFilePath)
      NMP::Memory::memFree(outputFilePath);
  }

  //--------------------
  // Copy the animation to the animation browser preview location if we are processing this animation
  // for preview in the animation browser.
  if (g_acGlobals.m_copyToAnimBrowserOutFile)
  {
    stringLength = strlen(g_acGlobals.m_outputDir) + strlen(ANIM_BROWSER_ANIM_FILE_NAME) + strlen(animData->m_animFormatType) + 10;
    char* outputFileName = (char*) NMPMemoryAlloc(stringLength);
    NMP_ASSERT(outputFileName);
    NMP_SPRINTF(
      outputFileName,
      stringLength,
      "%s/%s.%s",
      g_acGlobals.m_outputDir,
      ANIM_BROWSER_ANIM_FILE_NAME,
      animData->m_animFormatType);

    if (NMP::NMFile::save(outputFileName, animResource.ptr, animResource.format.size) <= 0)
    {
      // File write failed
      NMP::Memory::memFree(animResource.ptr);
      NMP_THROW_ERROR("Saving of animation file %s for resource browser failed", outputFileName);
    }

    NMP::Memory::memFree(outputFileName);
  }

  // Finished with the memory for the animation now.
  NMP::Memory::memFree(animResource.ptr);

  //--------------------
  // Remove any duplicate rig to anim maps and store a list of the ones we need in the compiled anim vector.
  ProcessedAsset*       rigToAnimMapProcessedAsset;
  NMP::Memory::Resource rigToAnimMapResource;

  for (uint32_t i = 0; i < rigToAnimMaps.size(); ++i)
  {
    // Check to see if the returned cached rigToAnimMap matches any of the ones we've processed already.
    NMP_VERIFY_MSG(rigToAnimMaps[i], "Missing rigToAnimMap");
    rigToAnimMapProcessedAsset = assetProc->findMatchingMapping(rigToAnimMaps[i]);

    if (rigToAnimMapProcessedAsset)
    {
      // there is already a matching mapping available so throw away the generated one and use this instead;
      NMP::Memory::memFree(rigToAnimMaps[i]);
      // and add a dependency reference
      uint32_t lastNewAsset = assetProc->getLastNewAssetStackValue();
      if (lastNewAsset)
        rigToAnimMapProcessedAsset->parendIDs.insert(lastNewAsset);
    }
    else
    {
      // Add the RigToAnimMap to the processor's list of assets.
      // We have to make a unique "guid" here because there isn't one provided by connect atm.
      assetProc->beginNewAssetProcessing();

      // Find the rig name guid corresponding to the rig that was compiled
      // against the animation to create the rigToAnim map.
      const char* rigGuid = "00000000-0000-0000-0000-000000000000";
      for (size_t j = 0; j != rigNameGuidsVector.size(); ++j)
      {
        if (rigNameGuidsVector[j].first == rigData->m_rigNames[i])
        {
          rigGuid = rigNameGuidsVector[j].second.c_str();
        }
      }

      static char rigToAnimMapGUID[1024];

      NMP::Memory::Format memReqsRigToAnimMap = rigToAnimMaps[i]->getInstanceMemoryRequirements();

      // now create a guid string from this long string
      {
        char temp[1024];
        uint32_t part1 = NMP::hashStringCRC32(rigToAnimMapGUIDbasis);

        // Hash of the animation file name and take
        NMP_SPRINTF(temp, "%s_%s", animData->m_animFileName, animData->m_animTakeName);
        uint32_t part2 = NMP::hashStringCRC32(temp);

        // Note it is important to create a hash of the rigToAnim map data, since the compression options
        // alone will not be unique. If we have two animation sets that compile two rigs against a single
        // animation then we end up with two rigToanimMaps. If we then change the compression rate options
        // on one of these anim sets, we can no longer compile a single animation binary for use with both
        // sets. The options and anim name are the same as they were previously for the unchanged set,
        // however the rigToAnim map data will have changed since the rig must be compiled independently
        // against the animation.
        rigToAnimMaps[i]->dislocate(); // Avoid issues with the pointers
        uint32_t part3 = NMP::hashCRC32(rigToAnimMaps[i], memReqsRigToAnimMap.size);
        rigToAnimMaps[i]->locate();

        // Hash of the rig name guid for the rig that was compiled against the animation to create the rigToAnim map.
        uint32_t part4 = NMP::hashStringCRC32(rigGuid);

        initGuidSting(rigToAnimMapGUID, part1, part2, part3, part4);
      }

      rigToAnimMapResource.ptr = rigToAnimMaps[i];
      rigToAnimMapResource.format = memReqsRigToAnimMap;
      rigToAnimMapProcessedAsset = assetProc->addAsset(
                                     rigToAnimMapResource,
                                     MR::Manager::kAsset_RigToAnimMap,
                                     "rigToAnimMap",
                                     rigToAnimMapGUID);
    }
    NMP_VERIFY_MSG(rigToAnimMapProcessedAsset, "Missing processed rig to anim asset");
    rigToAnimMapAssetIDs.push_back(rigToAnimMapProcessedAsset->uniqueRuntimeID);
  }

  rigToAnimMaps.clear();

  if (fullAnimFileName)
    NMP::Memory::memFree(fullAnimFileName);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool macFinalizeAnimsFunc(AP::AssetProcessor* NMP_UNUSED(assetProc))
{
  return processAnimationLookupTable();
}

//----------------------------------------------------------------------------------------------------------------------
bool macInitNetworkInstanceFunc(
  AP::AssetProcessor*           assetProc,
  MR::NetworkDef*               networkDef,
  const AP::AssetProcessor::InitNetworkInstanceArgs& initArgs)
{
  if (!networkDef)
    return false;

  NMP_VERIFY_MSG(
    assetProc->getNetwork() == NULL,
    "A network instance already exists for compile time evaluation.")
    
  //----------------------
  // Load the animation files
  bool status = true;
  for (MR::AnimSetIndex i = 0; i < networkDef->getNumAnimSets(); ++i)
  {
    // Fixup the animations and RTAMs
    if (!networkDef->loadAnimations(i, NULL))
    {
      status = false;
    }
  }

  //----------------------
  // Create memory allocators
  NMP::FastHeapAllocator* tempMemAllocator = NULL;
  NMP::MemoryAllocator* persistentMemAllocator = NULL;

  // Set the temporary memory size to 512KB. This should be large enough to cope with most networks for now but to
  // accurately determine the temp memory size used, see FastHeapAllocator::PrintAllocations().
  uint32_t tempDataSize = (512 * 1024);
  NMP::Memory::Format memReqsTempMemAllocator = NMP::FastHeapAllocator::getMemoryRequirements(tempDataSize, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResTempMemAllocator = NMPMemoryAllocateFromFormat(memReqsTempMemAllocator);
  NMP_VERIFY(memResTempMemAllocator.ptr);
  tempMemAllocator = NMP::FastHeapAllocator::init(memResTempMemAllocator, tempDataSize, NMP_VECTOR_ALIGNMENT);

#ifdef NMP_MEMORY_LOGGING
  NMP::Memory::Format memReqsPersistentMemAllocator = NMP::LoggingMemoryAllocator::getMemoryRequirements();
  NMP::Memory::Resource memResPersistentMemAllocator = NMPMemoryAllocateFromFormat(memReqsPersistentMemAllocator);
  NMP_VERIFY(memResPersistentMemAllocator.ptr);
  persistentMemAllocator = NMP::LoggingMemoryAllocator::init(memResPersistentMemAllocator);

#else

  NMP::Memory::Format memReqsPersistentMemAllocator = NMP::FreelistMemoryAllocator::getMemoryRequirements();
  NMP::Memory::Resource memResPersistentMemAllocator = NMPMemoryAllocateFromFormat(memReqsPersistentMemAllocator);
  NMP_VERIFY(memResPersistentMemAllocator.ptr);
  persistentMemAllocator = NMP::FreelistMemoryAllocator::init(memResPersistentMemAllocator);
#endif

  //----------------------
  // Create dispatcher
  MR::DispatcherBasic* dispatcher = MR::DispatcherBasic::createAndInit();
  NMP_VERIFY(dispatcher);

  //----------------------
  // Character controller
  BasicCharacterController* characterController = BasicCharacterController::createAndInit(
    initArgs.m_characterStartPosition,
    initArgs.m_characterStartRotation);

  //----------------------
  // Create network instance
  MR::Network* network = MR::Network::createAndInit(
    networkDef,
    dispatcher,
    tempMemAllocator,
    persistentMemAllocator,
    characterController);
  NMP_VERIFY_MSG(network, "Could not create a network instance.");

  //----------------------------
  // Initialise the network
  if (network)
  {
    // Set the network instance for evaluation
    assetProc->setNetwork(network);

    // set the networks world RootTransform
    network->setCharacterPropertiesWorldRootTransform(
      NMP::Matrix34(characterController->getCharacterOrientation(), characterController->getCharacterPosition()));

    // Check if we will be queuing combined trajectory and transforms
    bool combinedTrajectoryAndTransforms = false;
    if (initArgs.m_queueTransforms && initArgs.m_queueTrajectory && networkDef->canCombineTrajectoryAndTransformSemantics())
    {
      combinedTrajectoryAndTransforms = true;
    }

    // Add the appropriate post update access to the network
    if (combinedTrajectoryAndTransforms)
    {
      // Tell the network we want to watch the delta trajectory from the root node.
      network->addPostUpdateAccessAttrib(networkDef->getRootNodeID(), MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, 1);
    }
    else
    {
      if (initArgs.m_queueTrajectory)
      {
        // Tell the network we want to watch the delta trajectory from the root node.
        network->addPostUpdateAccessAttrib(networkDef->getRootNodeID(), MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, 1);
      }

      if (initArgs.m_queueTransforms)
      {
        // Tell the network we want to watch the transforms from the root node.
        network->addPostUpdateAccessAttrib(networkDef->getRootNodeID(), MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, 1);
      }
    }

    if (initArgs.m_queueSampledEvents)
    {
      // Tell the network we want to watch the sampled events buffer output from the root node.
      network->addPostUpdateAccessAttrib(networkDef->getRootNodeID(), MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, 1);
    }

    if (initArgs.m_queueSyncEventTrack)
    {
      // Tell the network we want to watch the sampled events buffer output from the root node.
      network->addPostUpdateAccessAttrib(networkDef->getRootNodeID(), MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, 1);
    }

    //----------------------
    // Set the proper initial animation set.
    network->setActiveAnimSetIndex((MR::AnimSetIndex)initArgs.m_startingAnimSetIndex);

    //----------------------
    // Set the desired initial state machine start state
    MR::NodeID activeRootNodeID = network->getActiveChildNodeID(MR::NETWORK_NODE_ID, 0);
    const MR::NodeDef* activeRootNodeDef = networkDef->getNodeDef(activeRootNodeID);
    NMP_VERIFY(activeRootNodeDef);
    if (activeRootNodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE)
    {
      // Queue setting the requested start state
      if (initArgs.m_startStateNodeID != MR::INVALID_NODE_ID)
      {
        MR::AttribDataStateMachine* sm = network->getAttribData<MR::AttribDataStateMachine>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, activeRootNodeID);
        const MR::AttribDataStateMachineDef* smDef = sm->getDef();
        sm->queueSetStateByNodeID(initArgs.m_startStateNodeID, smDef);
      }
    }

    //----------------------
    // We don't want to use a delta time for the first update
    MR::Task* task;
    MR::ExecuteResult execResult;

    network->startUpdate(
      0.0f,
      true,
      initArgs.m_queueTransforms,
      initArgs.m_queueTrajectory,
      initArgs.m_queueSampledEvents,
      initArgs.m_queueSyncEventTrack);

    while ((execResult = network->update(task)) != MR::EXECUTE_RESULT_COMPLETE)
    {
      if (execResult != MR::EXECUTE_RESULT_IN_PROGRESS)
      {
        // should just be getting the internal physics tasks
        NMP_VERIFY(
          task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER ||
          task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS ||
          task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT);

        (void)task;
      }
    }

    // We are responsible for resetting the temp memory allocator.
    tempMemAllocator->reset();

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool macUpdateNetworkInstanceFunc(
  AP::AssetProcessor*   assetProc,
  const AP::AssetProcessor::UpdateNetworkInstanceArgs& updateArgs)
{
  NMP_VERIFY(assetProc);
  MR::Network* network = assetProc->getNetwork();
  if (!network)
    return false;

  //----------------------------
  // Update connections
  network->startUpdate(
    updateArgs.m_updateTime,
    updateArgs.m_absTime,
    updateArgs.m_queueTransforms,
    updateArgs.m_queueTrajectory,
    updateArgs.m_queueSampledEvents,
    updateArgs.m_queueSyncEventTrack);

  BasicCharacterController* characterController = (BasicCharacterController*)network->getCharacterController();
  NMP_VERIFY(characterController);

  //----------------------------
  // Pre controller
  MR::Task* task = NULL;
  MR::ExecuteResult execResult;
  do
  {
    // Update the network
    execResult = network->update(task);
  } while (execResult == MR::EXECUTE_RESULT_IN_PROGRESS);
  
  if (task)
  {
    NMP_VERIFY(task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER);
  }

  //----------------------------
  // Update the characterController
  characterController->updateController(
    network->getTranslationChange(),
    network->getOrientationChange());

  //----------------------------
  // update the networks world RootTransform to the adjusted position and orientation
  network->updateCharacterPropertiesWorldRootTransform(
    NMP::Matrix34(characterController->getCharacterOrientation(), characterController->getCharacterPosition()),
    true);

  //----------------------------
  // Pre physics
  do
  {
    execResult = network->update(task);
  } while (execResult == MR::EXECUTE_RESULT_IN_PROGRESS);
  
  if (task)
  {
    NMP_VERIFY(task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS);
  }
  
  // TODO: Add physics simulation step here
  
  //----------------------------
  // Reset the temporary allocator
  network->getTempMemoryAllocator()->reset();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool macReleaseNetworkInstanceFunc(AP::AssetProcessor* assetProc)
{
  NMP_VERIFY(assetProc);

  // Tidy up
  MR::Network* network = assetProc->getNetwork();
  if (network)
  {
    // Get the allocated data objects
    NMP::MemoryAllocator* tempMemAllocator = network->getTempMemoryAllocator();
    NMP_VERIFY(tempMemAllocator);
    NMP::MemoryAllocator* persistentMemAllocator = network->getPersistentMemoryAllocator();
    NMP_VERIFY(persistentMemAllocator);
    MR::Dispatcher* dispatcher = network->getDispatcher();
    NMP_VERIFY(dispatcher);
    BasicCharacterController* characterController = (BasicCharacterController*)network->getCharacterController();
    NMP_VERIFY(characterController);

    // Release the network attribute data
    network->releaseAndDestroy();
    assetProc->setNetwork(NULL);

    // Character controller
    NMP::Memory::memFree(characterController);

    // Release the dispatcher
    dispatcher->releaseAndDestroy();

    // Memory allocators
    tempMemAllocator->term();
    persistentMemAllocator->term();
    NMP::Memory::memFree(tempMemAllocator);
    NMP::Memory::memFree(persistentMemAllocator);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool macChangeNetworkAnimSetFunc(AP::AssetProcessor* assetProc, uint32_t animSetIndex)
{
  NMP_VERIFY(assetProc);
  MR::Network* network = assetProc->getNetwork();
  if (!network)
    return false;
    
  network->setActiveAnimSetIndex((MR::AnimSetIndex) animSetIndex);
  
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Generate an output filename for the asset bundle.
/// \ingroup AssetCompilerModule
void generateOutputBundleFileName(
  char*            outputFileName,
  uint32_t         NMP_UNUSED(maxNameLength),
  ME::AssetExport* primarySourceAsset)
{
  // The output directory first.
  strcpy(outputFileName, g_acGlobals.m_outputDir);
  if (outputFileName[strlen(outputFileName)-1] != '/')
    strcat(outputFileName, "/");

  // Followed by the file name.
  if (g_acGlobals.m_copyToAnimBrowserOutFile)
  {
    // Animation browser output generates fixed bundle filename.
    strcat(outputFileName, ANIM_BROWSER_BUNDLE_FILE_NAME);
  }
  else
  {
    const char *fname = g_acGlobals.m_outputFilename;

    if (fname)
    {
      // Use the specified output filename
      strcat(outputFileName, fname);
    }
    else
    {
      // No output filename specified
      if (primarySourceAsset)
      {
        // Generate a filename based on the name of the first asset in the command line args.
        strcat(outputFileName, primarySourceAsset->getName());
        strcat(outputFileName, ".nmb");
      }
      else
      {
        // Fallback to something reasonable
        strcat(outputFileName, "outputBundle.nmb");
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Compile the loaded source assets and store the resulting runtime files.
/// \ingroup AssetCompilerModule
/// \return 0 on success. non-zero otherwise.
int32_t compileAssets(AssetProcessor& assetProc, std::vector<ME::AssetExport*>& loadedAssets, const PluginManager& pm)
{
  char outputBundleFileName[MAX_PATH];
  // Generate an output filename.
  generateOutputBundleFileName(outputBundleFileName, MAX_PATH, loadedAssets[0]);

  // Open a stream to write out the binary data to
  FILE* file = fopen(outputBundleFileName, "wb");
  if (!file)
  {
    g_acGlobals.m_errorLogger->output("The asset processor could not open the output location %s\n", outputBundleFileName);
    return g_acGlobals.m_failureCode;
  }

  // Create globals
  g_animIDtoFilename = new MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookupBuilder();
  g_bundleWriter = new MR::UTILS::SimpleBundleWriter(file);

  //--------------------
  // Process all the assets
  assetProc.processAssets(loadedAssets);

  //--------------------
  // Serialize the processed assets
  // We could pass the bundle writer into these functions, to avoid it being global
  serializePluginList(pm, assetProc);
  assetProc.serializeProcessedAssets();
  serializeAnimationLookupTable();

  //--------------------
  // Destroy globals
  delete g_bundleWriter;
  delete g_animIDtoFilename;

  //--------------------
  // Close the stream to which the binary data was written
  fclose(file);
  assetProc.freeProcessedAssets();

  //--------------------
  if (assetProc.getExceptionOccured() || assetProc.getProcessingFailFlag())
  {
    g_acGlobals.m_errorLogger->output("Internal error in asset processor\n");
    return g_acGlobals.m_failureCode;
  }

  g_acGlobals.m_messageLogger->output("Asset processor success\n");
  return g_acGlobals.m_successCode;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Destroy any existing compiled asset files relating to the indicated command line assets.
/// \ingroup AssetCompilerModule
void cleanAssets(std::vector<ME::AssetExport*>& loadedAssets)
{
  // Destroy the animation cache (if there is one).
  if (g_animCache)
  {
    g_animCache->cleanAndRemoveFiles();
    delete(g_animCache);
    g_animCache = 0;
  }

  // Destroy the output files (if we loaded assets).
  size_t count = loadedAssets.size();
  for (size_t i = 0; i != count; ++i)
  {
    if (loadedAssets[i])
    {
      static char outputBundleFileName[MAX_PATH];

      // Generate an output filename.
      generateOutputBundleFileName(outputBundleFileName, MAX_PATH, loadedAssets[i]);

      // Destroy the asset.
      remove(outputBundleFileName);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Load all source assets and store reference to each in a vector.
/// \ingroup AssetCompilerModule
/// \param numAssetsProcessed number of assets loaded
uint32_t loadSourceAssets(
  std::vector<ME::AssetExport*>& loadedAssets,
  ExportFactory*                 factory,
  const NMP::CommandLineProcessor::StringList *assetNames,
  int32_t& numAssetsProcessed
  )
{
  numAssetsProcessed = 0;
  if (assetNames && !assetNames->empty())
  {
    NMP::CommandLineProcessor::StringList::iterator it = assetNames->begin();
    for (; it != assetNames->end(); ++it)
    {
      ME::AssetExport* loadedAsset = factory->loadAsset(*it);
      if (!loadedAsset)
      {
        // We can't process an asset that we can't load
        MR::DispatcherBasic::term();
        MR::Manager::termMorphemeLib();
        g_acGlobals.m_errorLogger->output("The asset %s was not found by the asset processor.\n", *it);
        return g_acGlobals.m_failureCode;
      }
      loadedAssets.push_back(loadedAsset);
      ++numAssetsProcessed;
    }
  }

  return g_acGlobals.m_successCode;
}

//----------------------------------------------------------------------------------------------------------------------
class RAIIMemoryWrapper
{
public:
  static NMP::Memory::Config cfg;

#ifdef NMP_MEMORY_LOGGING
  #define MAX_ALLOCS (1024 * 64)
  static void* loggedPtrs[MAX_ALLOCS];
  static size_t loggedSizes[MAX_ALLOCS];
  static NMP::Memory::NMP_MEM_STATE loggedAllocated[MAX_ALLOCS];
  static uint32_t loggedLine[MAX_ALLOCS];
  static const char* loggedFile[MAX_ALLOCS];
  static const char* loggedFunc[MAX_ALLOCS];
  static NMP::Memory::MemoryLoggingConfig memLoggingCfg;
#endif

  RAIIMemoryWrapper(){}

  void init()
  {
    // Initialise the runtime library (we need this to generate assets)
#ifdef NMP_MEMORY_LOGGING
    memLoggingCfg.loggedAllocated = loggedAllocated;
    memLoggingCfg.loggedFile = loggedFile;
    memLoggingCfg.loggedFunc = loggedFunc;
    memLoggingCfg.loggedLine = loggedLine;
    memLoggingCfg.loggedPtrs = loggedPtrs;
    memLoggingCfg.loggedSizes = loggedSizes;
    memLoggingCfg.maxAllocations = MAX_ALLOCS;
    memLoggingCfg.clearLog();
    NMP::Memory::init(cfg, memLoggingCfg);
#else
    NMP::Memory::init(cfg);
#endif
  }

  ~RAIIMemoryWrapper()
  {
#ifdef NMP_MEMORY_LOGGING
    NMP::Memory::printAllocations();
#endif
    NMP::Memory::shutdown();
  }
};

NMP::Memory::Config RAIIMemoryWrapper::cfg =
{
  mallocWrapped, mallocAlignedWrapped, callocWrapped, freeWrapped, memcpyWrapped, memcpy128Wrapped, memSizeWrapped
};

#ifdef NMP_MEMORY_LOGGING
void* RAIIMemoryWrapper::loggedPtrs[MAX_ALLOCS];
size_t RAIIMemoryWrapper::loggedSizes[MAX_ALLOCS];
NMP::Memory::NMP_MEM_STATE RAIIMemoryWrapper::loggedAllocated[MAX_ALLOCS];
uint32_t RAIIMemoryWrapper::loggedLine[MAX_ALLOCS];
const char* RAIIMemoryWrapper::loggedFile[MAX_ALLOCS];
const char* RAIIMemoryWrapper::loggedFunc[MAX_ALLOCS];

NMP::Memory::MemoryLoggingConfig RAIIMemoryWrapper::memLoggingCfg =
{
  loggedPtrs, loggedSizes, loggedAllocated, loggedLine, loggedFile, loggedFunc, MAX_ALLOCS
};
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief Application entry point.
/// \ingroup AssetCompilerModule
int32_t __cdecl main(int argc, char** argv)
{
  // Asset compiler profiling
  NMP::Timer macProfilingTimer;
  macProfilingTimer.start();

  // Initialise memory subsystem
  RAIIMemoryWrapper memWrapper;
  memWrapper.init();

  ACOptions acOptions;

#if NM_ENABLE_EXCEPTIONS

#if NM_CALL_STACK_DEBUG
  NMP::SetSETranslator Seh;
#endif

  try
#endif // NM_ENABLE_EXCEPTIONS
  {
    //-----------------------------------------

  // Need to keep this in memory as we point to the args within this.
  acOptions.init(argc, argv);

  // should the asset compiler just write info about itself and terminate
  if (acOptions.getInfo())
  {
    // write the platform format e.g. BE32
    wchar_t platformFormatString[5];
    mbstowcs(platformFormatString, NM_PLATFORM_FORMAT_STRING, 5);
    platformFormatString[4] = 0;

    acOptions.getInfoLogger()->output(L"platformFormat=%s", platformFormatString);
    return 0;
  }

  // store the global values
  {
    g_acGlobals.m_messageLogger = acOptions.getMessageLogger();
    g_acGlobals.m_errorLogger = acOptions.getErrorLogger();
    g_acGlobals.m_successCode = acOptions.getSuccessCode();
    g_acGlobals.m_failureCode = acOptions.getFailureCode();
    g_acGlobals.m_baseDir = acOptions.getBaseDir();
    g_acGlobals.m_outputDir = acOptions.getOutputDir();
    g_acGlobals.m_outputFilename = acOptions.getOutputFilename();
    g_acGlobals.m_copyToAnimBrowserOutFile = acOptions.getDoPreviewAnimation();

    uint32_t subSectionSize = acOptions.getMaxAnimSubSectionSize();
    if (subSectionSize > 0)
    {
      MR::setMaxAnimSectionSize(subSectionSize);
    }
  }

  // Uncomment this line to output to log file.
  // NET_LOG_INIT("MorphUpdate.log");

  // Uncomment this line to log everything.
  // NET_LOG_ADD_PRIORITY_RANGE(NMP::LOG_MIN_PRIORITY, NMP::LOG_MAX_PRIORITY);

  // Uncomment this line to log queued parameters.
  // NET_LOG_ADD_PRIORITY_RANGE(98, 100);

  // Uncomment this line to log queue contents when not empty at end of execution (including all task params).
  // NET_LOG_ADD_PRIORITY_RANGE(101, 107);

  // Uncomment this line to log queue contents when not empty at end of execution (Only include unevaluated task params).
  // NET_LOG_ADD_PRIORITY_RANGE(102, 103);

  // Uncomment this line to list executing tasks.
  // NET_LOG_ADD_PRIORITY_RANGE(107, 107);

  // Uncomment this line to list executing task parameters.
  // NET_LOG_ADD_PRIORITY_RANGE(108, 108);

  // Uncomment this line to list task names against their registered IDs.
  NET_LOG_ADD_PRIORITY_RANGE(109, 109);

#ifdef NMP_MEMORY_LOGGING
  // Requirements for the network instance
  NET_LOG_ADD_PRIORITY_RANGE(4, 4);
#endif

  // Logging from live link and the simple bundler.
  NM_LOG_ADD_PRIORITY_RANGE(MR::UTILS::g_SBLogger, MR::UTILS::SB_MESSAGE_PRIORITY, MR::UTILS::SB_MESSAGE_PRIORITY);

  // The PluginManager must be declared outside the try catch block so that it will not be destroyed if any exceptions
  // are thrown.  If it was then plug-ins would be unloaded and any memory referenced in the exception that belonged
  // to the plug-in would become invalid.
  PluginManager pluginManager;

//-----------------------------------------

  // Initialise XMD.
  XMDLogRedirector* g_xmdLogger;

#ifdef ENABLE_ASSET_COMP_LOG_MSG
  g_xmdLogger = new XMDLogRedirector(acOptions.getErrorLogger(), acOptions.getMessageLogger(), true);
#else
  g_xmdLogger = new XMDLogRedirector(acOptions.getErrorLogger(), acOptions.getMessageLogger(), false);
#endif

  bool XMDinitOK = XMD::XMDInit(g_xmdLogger);

  if (!XMDinitOK)
  {
    MR::DispatcherBasic::term();
    MR::Manager::termMorphemeLib();

    // Initialization failed, return gracefully
    acOptions.getErrorLogger()->output("Internal error when initializing the asset processor.\n");
    _flushall();
    return acOptions.getFailureCode();
  }

#if defined(NM_ENABLE_FBX)
  bool FBXinitOK = FBX_XMD_PLUGIN::init();

  if (!FBXinitOK)
  {
    XMD::XMDCleanup();

    MR::DispatcherBasic::term();
    MR::Manager::termMorphemeLib();

    // Initialization failed, return gracefully
    acOptions.getErrorLogger()->output("Internal error when initializing the asset processor.\n");
    _flushall();
    return acOptions.getFailureCode();
  }

  FBX_XMD_PLUGIN::FBXFileTranslator* fileTranslator = new FBX_XMD_PLUGIN::FBXFileTranslator();
  XMD::XGlobal::RegisterTranslator(fileTranslator, "fbx");
#endif

  // Load all plug-ins
  //
#ifdef NMP_MEMORY_LOGGING
  loadPluginCore(&RAIIMemoryWrapper::cfg, &RAIIMemoryWrapper::memLoggingCfg);
  bool loadResult = pluginManager.loadPluginsFromList(
    acOptions.getPluginDir(), 
    acOptions.getPluginList(), 
    acOptions.getPluginListCount(), 
    &RAIIMemoryWrapper::cfg, 
    &RAIIMemoryWrapper::memLoggingCfg);
#else
  loadPluginCore(&RAIIMemoryWrapper::cfg);
  bool loadResult = pluginManager.loadPluginsFromList(
    acOptions.getPluginDir(), 
    acOptions.getPluginList(), 
    acOptions.getPluginListCount(),
    &RAIIMemoryWrapper::cfg);
#endif// NMP_MEMORY_LOGGING

  if (!loadResult)
  {
    NMP_VERIFY_FAIL("Failed to load plugins required to process assets.");
    _flushall();
    return acOptions.getFailureCode();
  }

  // The manager initialised may belong to a different dll so bring it into this library.
  MR::Manager::setInstance(pluginManager.getManager());

  MR::Manager::getInstance().setAnimFileHandlingFunctions(macCustomAnimLoader, macCustomAnimUnloader);

  // Create full path for animation cache file.
  {
    const char *cacheDir = acOptions.getCacheDir();
    if (cacheDir)
    {
      g_animCache = new AnimCache(cacheDir);
      if (!g_animCache->load())
      {
        if (!g_animCache->create())
        {
          delete(g_animCache);
          g_animCache = 0;
        }
      }
      ::CreateDirectoryA(cacheDir, 0);
    }
  }

  g_AnimationSourceCache = new AnimationSourceCache();

  const char* animInfo = acOptions.getAnimInfoToUpdate();
  if (animInfo != NULL)
  {
    // Update the anim info for the given directory
    if (!macUpdateAnimInfo(animInfo))
    {
      delete g_AnimationSourceCache;
#if defined(NM_ENABLE_FBX)
      FBX_XMD_PLUGIN::term();
#endif
      XMD::XMDCleanup();
      MR::DispatcherBasic::term();
      MR::Manager::termMorphemeLib();
      acOptions.getErrorLogger()->output("Failed to update anim info.\n");
      _flushall();
      return acOptions.getFailureCode();
    }
  }
  else
  {
    // Initialise the ExportFactory to be used to read in network data.
    ExportFactoryXML  xmlExportFactory;
    ExportFactory*    exportFactory;
    exportFactory = dynamic_cast<ExportFactory*>(&xmlExportFactory);

    // Dispatcher for network evaluation and task registration
    MR::Dispatcher* g_dispatcher = MR::DispatcherBasic::createAndInit();
    NMP_VERIFY(g_dispatcher);
    // Register task functions
    MR::CoreTaskIDs::registerNMCoreTasks(g_dispatcher);

    // Create the output directories
    ::CreateDirectoryA(acOptions.getOutputDir(), 0);

    // Declare the default morpheme asset processor, registering the necessary application callbacks.
    AssetProcessor assetProc(
      (AssetProcessor::GenUIDFunc) macGenUIDFunc,                             // An app callback for generating a unique
                                                                              //  runtime asset id.
      (AssetProcessor::HandleProcessedAssetFunc) macHandleProcessedAssetFunc, // An app callback to be called on each processed
                                                                              //  asset for storing/bundling of runtime assets.
      (AssetProcessor::ProcessAnimFunc) macProcessAnimFunc,                   // An app callback for converting source
                                                                              //  animation data to the required runtime format.
      (AssetProcessor::FinalizeProcessAnimsFunc) macFinalizeAnimsFunc,        // An app callback when all animations have been compiled.
      (AssetProcessor::InitNetworkInstanceFunc) macInitNetworkInstanceFunc,
      (AssetProcessor::UpdateNetworkInstanceFunc) macUpdateNetworkInstanceFunc,
      (AssetProcessor::ReleaseNetworkInstanceFunc) macReleaseNetworkInstanceFunc,
      (AssetProcessor::ChangeNetworkAnimSetFunc) macChangeNetworkAnimSetFunc,
      (AssetProcessor::DeMacroisePathFunc) generateFullSourceAnimFileName,
      exportFactory,                                                          // The ExportFactory to be used to read in
                                                                              //  the source network data.
      acOptions.getBaseDir(),                                                 // The root directory parameter passed to the asset compiler
      acOptions.getMessageLogger(),                                           // Handles build process information messages.
      acOptions.getErrorLogger());                                            // Handles build process error messages.

    assetProc.setRuntimeAssetScaleFactor(acOptions.getRuntimeAssetScaleFactor());

    pluginManager.registerPluginsWithProcessor(&assetProc, 1, &g_dispatcher);

    // Load all source assets and store reference to each in a vector.
    std::vector<ME::AssetExport*> loadedAssets;
    {
      const NMP::CommandLineProcessor::StringList *assetNames = acOptions.getAssetList();

      int32_t numAssetsLoaded = 0;
      uint32_t result = loadSourceAssets(loadedAssets, exportFactory, assetNames, numAssetsLoaded);
      
      if (result != acOptions.getSuccessCode() || (numAssetsLoaded == 0 && !acOptions.getDoCleanBuild()))
      {
        if (numAssetsLoaded == 0 && !acOptions.getDoCleanBuild())
        {
          acOptions.getErrorLogger()->output("No assets to process.\n");
        }

        // Clean up and exit
        delete g_AnimationSourceCache;
        g_dispatcher->releaseAndDestroy();
#if defined(NM_ENABLE_FBX)
        FBX_XMD_PLUGIN::term();
#endif
        XMD::XMDCleanup();
        MR::DispatcherBasic::term();
        MR::Manager::termMorphemeLib();
        _flushall();
        return acOptions.getFailureCode();
      }
    }

    if (!acOptions.getDoCleanBuild())
    {
      /// Compile the loaded source assets and store the resulting runtime files.
      uint32_t result = compileAssets(assetProc, loadedAssets, pluginManager);

      if (result != acOptions.getSuccessCode())
      {
        delete g_AnimationSourceCache;
        g_dispatcher->releaseAndDestroy();
#if defined(NM_ENABLE_FBX)
        FBX_XMD_PLUGIN::term();
#endif
        XMD::XMDCleanup();
        MR::DispatcherBasic::term();
        MR::Manager::termMorphemeLib();
        acOptions.getErrorLogger()->output("Failed to compile assets\n");
        _flushall();
        return result;
      }
    }
    else
    {
      // Destroy any existing compiled asset files relating to the indicated command line assets.
      cleanAssets(loadedAssets);
    }

    assetProc.freeProcessedAssets();
    g_dispatcher->releaseAndDestroy();
  }

  delete(g_AnimationSourceCache);
#if defined(NM_ENABLE_FBX)
  FBX_XMD_PLUGIN::term();
#endif
  XMD::XMDCleanup();
  delete g_xmdLogger;

  MR::DispatcherBasic::term();
  MR::Manager::termMorphemeLib();

//-----------------------------------------
  }
#if NM_ENABLE_EXCEPTIONS
#if NM_CALL_STACK_DEBUG
  catch (NMP::SEHException& e)
  {
    // Clean up morpheme and log any messages.
    MR::DispatcherBasic::term();
    MR::Manager::termMorphemeLib();

    // Ensure things are as they should be.
    if (NMP::Exception::alreadyDealingWithAnException())
    {
      // Just log the call stack
      acOptions.getErrorLogger()->output("%s\n", e.getMessage());
      _flushall();
    }

    return acOptions.getFailureCode();
  }
#endif // NM_CALL_STACK_DEBUG
  catch (const NMP::Exception& e)
  {
    // Clean up morpheme and log any messages.
    MR::DispatcherBasic::term();
    MR::Manager::termMorphemeLib();

    // Guard against throwing across .dll boundaries...
    if (NMP::Exception::alreadyDealingWithAnException())
    {
      acOptions.getErrorLogger()->output(
        "Error in asset processor: %s\nFile: %s, Line: %d\n",
        e.getMessage(),
        e.getFile(),
        e.getLine());
      _flushall();  // Flush all streams, so that any logged error messages get written to file before proceeding.
    }

    return acOptions.getFailureCode();
  }
#endif // NM_ENABLE_EXCEPTIONS
//-----------------------------------------

  if (g_animCache)
  {
    g_animCache->save();
    delete(g_animCache);
  }

  // Asset compiler profiling
  g_macTotalEllapsedTime = macProfilingTimer.stop();
  // All done!
  acOptions.getMessageLogger()->output("Asset compiler succeeded (rc=%d)\n", acOptions.getSuccessCode());
  _flushall();
  return acOptions.getSuccessCode();
}

//----------------------------------------------------------------------------------------------------------------------
