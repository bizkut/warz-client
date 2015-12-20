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
#include "comms/simpleBundleUtils.h"
#include "simpleBundle/simpleBundle.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
MR::NetworkDef* readNetworkFromSimpleBundle(
  void*                      fileBuffer,
  size_t                     fileSize,
  MCOMMS::GUID&              guid,    // OUT - this contains the GUID from the file.
  IPluginValidator*          validateCompilerPlugins,
  MR::NetworkPredictionDef*& netPredictionDef)
{
  MR::NetworkDef*                  netDef = NULL;
  std::vector<MR::RuntimeAssetID>  assetIDs;
  MR::Manager::AssetType           assetType;
  void*                            asset;
  NMP::Memory::Format              assetMemReqs;
  MR::RuntimeAssetID               assetID;
  uint8_t*                         fileGuid = 0;

  netPredictionDef = NULL;

  // Step through each asset in the file and register it.
  MR::UTILS::SimpleBundleReader reader(fileBuffer, fileSize);

  while (reader.readNextAsset(assetType, assetID, fileGuid, asset, assetMemReqs))
  {
    if (assetType == MR::Manager::kAsset_PluginList)
    {
      NMP::OrderedStringTable* pluginList = (NMP::OrderedStringTable*)asset;
      pluginList->locate();
      if (validateCompilerPlugins)
      {
        if (!validateCompilerPlugins->validatePluginList(*pluginList))
        {
          NMP_VERIFY_FAIL("Plug-in verification failed.  Make sure the plugins used when compiling the bundle match the"
            " order the libraries are registered in the runtime.")
          return NULL;
        }
      }
      continue;
    }

    if (assetType < MR::Manager::kAsset_NumAssetTypes)
    {
      // Does the asset already exist ?
      if (MR::Manager::getInstance().getObjectPtrFromObjectID(assetID) != 0)
      {
        // Yes.  No need to re-register.
        NMP_DEBUG_MSG(
          " Asset with id %x, type 0x%x, size 0x%x already exists in mr::Manager.\n",
          assetID,
          assetType,
          assetMemReqs.size);

        if (assetType == MR::Manager::kAsset_NetworkDef)
        {
          netDef = (MR::NetworkDef*) MR::Manager::getInstance().getObjectPtrFromObjectID(assetID);
        }

        continue;
      }

      // No. Register
      printf(" Registering asset with id %x\n", assetID);

      // Make a copy of the asset.
      void* assetCopy = NMPMemoryAllocAligned(assetMemReqs.size, assetMemReqs.alignment);
      NMP_ASSERT(assetCopy);
      memcpy(assetCopy, asset, assetMemReqs.size);

      // Locate the copy
      MR::AssetLocateFn locateFn = MR::Manager::getInstance().getAssetLocateFn(assetType);
      bool locateResult = false;
      if (locateFn)
      {
        locateResult = locateFn(assetType, assetCopy);
      }

      if (!locateResult)
      {
        NMP_DEBUG_MSG("   Error.  Unknown asset type: %s\n", MR::Manager::getInstance().getAssetTypeName(assetType));

        // We shouldn't register assets that failed to locate.  We continue to try to deal with the other assets though.
        NMP::Memory::memFree(assetCopy);
        continue;
      }


      // Take special note of certain asset types.
      switch (assetType)
      {
        case MR::Manager::kAsset_NetworkDef:
          NMP_ASSERT(!netDef);            // We only expect one network definition per bundle.
          netDef = (MR::NetworkDef*)assetCopy;
          // copy the guid, overwriting our local version
          for (uint32_t i = 0; i < 16; ++i)
            guid.value[i] = fileGuid[i];
          break;
        case MR::Manager::kAsset_NetworkPredictionDef:
          NMP_ASSERT(!netPredictionDef);  // We only expect one network prediction manager per bundle.
          netPredictionDef = (MR::NetworkPredictionDef*)assetCopy;
          break;

        default: // Do nothing for anything else
          break;
      }

      if (assetType == MR::Manager::kAsset_NetworkDef)
      {
        netDef = (MR::NetworkDef*)assetCopy;

        // copy the guid, overwriting our local version
        for (uint32_t i = 0; i < 16; ++i)
          guid.value[i] = fileGuid[i];
      }

      // Register it
      if (!MR::Manager::getInstance().registerObject(assetCopy, assetType, assetID))
      {
        // Registration failed for some reason
        NMP_DEBUG_MSG("   !! Failed to register object at %p, with assetID %d.\n", assetCopy, assetID);

        // Unregister everything and free the memory
        for (int32_t i = (int32_t)assetIDs.size() - 1 ; i >= 0 ; --i)
        {
          const void* assetMem = MR::Manager::getInstance().getObjectPtrFromObjectID(assetIDs[i]);
          MR::Manager::getInstance().unregisterObject(assetIDs[i]);
          NMP::Memory::memFree((void*)assetMem);
        }

        return NULL;
      }

      // Record the ID in case something goes wrong
      assetIDs.push_back(assetID);
    }
  }

  return netDef;
}

//----------------------------------------------------------------------------------------------------------------------
MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* readAnimIDToFilenameFromSimpleBundle(
  void*  fileBuffer,
  size_t fileSize)
{
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* netDefSpecificAnimIDtoFilename = 0;
  MR::Manager::AssetType           assetType;
  void*                            asset;
  NMP::Memory::Format              assetMemReqs;
  MR::RuntimeAssetID               assetID;
  uint8_t*                         fileGuid = 0;

  // Step through each asset in the file and register it.
  MR::UTILS::SimpleBundleReader reader(fileBuffer, fileSize);

  while (reader.readNextAsset(assetType, assetID, fileGuid, asset, assetMemReqs))
  {
    if (assetType == (MR::Manager::AssetType)MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup::kAsset_SimpleAnimRuntimeIDtoFilenameLookup)
    {
      // Make a copy of the asset.
      void* assetCopy = NMPMemoryAllocAligned(assetMemReqs.size, assetMemReqs.alignment);
      NMP_ASSERT(assetCopy);
      memcpy(assetCopy, asset, assetMemReqs.size);

      ((MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup*)assetCopy)->locate();
      netDefSpecificAnimIDtoFilename = (MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup*)assetCopy;
    }
  }

  return netDefSpecificAnimIDtoFilename;
}

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
