// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "FBXPlugin.h"

#include <fbxsdk.h>

#include "NMPlatform/NMTimer.h"

#include "export/mcAnimInfo.h"

namespace FBX_XMD_PLUGIN
{
//----------------------------------------------------------------------------------------------------------------------
extern FbxManager* g_manager;
extern std::string FixupName(const char* n);

//----------------------------------------------------------------------------------------------------------------------
bool isBinaryFBX(const char* animFileName)
{
  FILE* animFile = 0;
  if (fopen_s(&animFile, animFileName, "rb") != 0)
  {
    return false;
  }

  // binary fbx files start with this string id
  const char binaryFileID[] = "Kaydara FBX Binary";
  
  const size_t binaryFileIDSize = (sizeof(binaryFileID) / sizeof(char));

  char buffer[binaryFileIDSize] = { '\0' };

  // - 1 is to not include the null character in the size
  fread_s(buffer, binaryFileIDSize, sizeof(char), binaryFileIDSize - 1, animFile);

  // if the buffer in the file and the id match then it must be a binary fbx
  bool hasBinaryID = strcmp(binaryFileID, buffer) == 0;

  fclose(animFile);

  return hasBinaryID;
}

//----------------------------------------------------------------------------------------------------------------------
bool updateAnimInfoWithFBX(ME::AnimInfo* animInfo, const char* animFileName, time_t animUpdateTime)
{
#if FBX_PLUGIN_ENABLE_PROFILING != 0
  NMP::Timer profileTimer;
#endif

  NMP_ASSERT(g_manager);

  FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
  FbxImporter* importer = FbxImporter::Create(g_manager, "");
  FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxImporter::Create");

  FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
  FbxScene* scene = FbxScene::Create(g_manager, "");
  FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxScene::Create");

  FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
  bool initialised = importer->Initialize(animFileName);
  FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxImporter::Initialize");

  bool result = false;
  if (initialised)
  {
    animInfo->setValidDate(animUpdateTime);
    animInfo->clearTakes();

    bool imported = true;

    // getting the file frame rate requires calling FbxImporter::Import which is very slow for large files
#if FBX_PLUGIN_ANIMINFO_DISABLE_IMPORT_FRAMERATE == 0
    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    FbxIOSettings* ios = FbxIOSettings::Create(g_manager, IOSROOT);
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxIOSettings::Create");

    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    ios->SetBoolProp(IMP_FBX_MATERIAL, false);
    ios->SetBoolProp(IMP_FBX_TEXTURE, false);
    ios->SetBoolProp(IMP_FBX_LINK, false);
    ios->SetBoolProp(IMP_FBX_SHAPE, false);
    ios->SetBoolProp(IMP_FBX_MODEL_COUNT, false);
    ios->SetBoolProp(IMP_FBX_GOBO, false);
    ios->SetBoolProp(IMP_FBX_ANIMATION, false);
    ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

    g_manager->SetIOSettings(ios);
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "configure IO Settings");

    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    imported = importer->Import(scene);
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxImporter::Import");
#endif // FBX_PLUGIN_ANIMINFO_DISABLE_IMPORT_FRAMERATE == 0

    if (imported)
    {
      // get the framerate
      FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
      FbxGlobalSettings& globalSettings = scene->GetGlobalSettings();
      FbxTime::EMode globalTimeMode = globalSettings.GetTimeMode();
      float framerate = (float)FbxTime::GetFrameRate(globalTimeMode);
      FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "get frame rate");

      for (int i = 0, takeCount = importer->GetAnimStackCount(); i != takeCount; ++i)
      {
        FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);

        const FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);
        const std::string takeName = FixupName(takeInfo->mName.Buffer());

        FbxTime fbxDuration = takeInfo->mLocalTimeSpan.GetDuration();
        float duration = (float)fbxDuration.GetSecondDouble();

        animInfo->addTake(takeName.c_str(), duration, framerate);

        FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "add anim info take");
      }
    }

#if FBX_PLUGIN_ANIMINFO_DISABLE_IMPORT_FRAMERATE == 0
    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    ios->Destroy();
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxIOSettings::Destroy");
#endif // FBX_PLUGIN_ANIMINFO_DISABLE_IMPORT_FRAMERATE == 0

    result = true;
  }

  FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
  scene->Destroy();
  FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxScene::Destroy");

  FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
  importer->Destroy();
  FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxImporter::Destroy");

#if FBX_PLUGIN_ENABLE_PROFILING != 0
  profileTimer.printTotalSectionTime("updateAnimInfoWithFBX");
#endif

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace FBX_XMD_PLUGIN
//----------------------------------------------------------------------------------------------------------------------
