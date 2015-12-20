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
#include "GameAssetLoader.h"
#include "GameCharacterManager.h"
#include "simpleBundle/simpleBundle.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
MR::NetworkDef* AssetLoaderBasic::loadBundle(
  void*            bundle,
  size_t           bundleSize,
  MR::AnimRigDef** rig,
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup** animFileLookUp)
{
  if (!bundle || !bundleSize)
  {
    NMP_DEBUG_MSG("error: Valid bundle data expected (%p, size=%u)!\n", bundle, bundleSize);
    return NULL;
  }

  MR::NetworkDef* networkDef = NULL;

  //----------------------------
  // Create a SimpleBundleReader to iterate over the objects
  MR::UTILS::SimpleBundleReader reader(bundle, (uint32_t)bundleSize);

  MR::Manager::AssetType  assetType;
  void*                   asset;
  NMP::Memory::Format     assetMemReqs;
  MR::RuntimeAssetID      assetID;
  uint8_t*                fileGuid = 0;

  while (reader.readNextAsset(assetType, assetID, fileGuid, asset, assetMemReqs))
  {
    //----------------------------
    // Only consider core runtime asset for registration with the manager.
    //
    // The locate process is also different for core and client assets, while core assets can be located using the
    // manager, client asset need to be located explicitly - which could also be handled outside this method.
    if (assetType < MR::Manager::kAsset_NumAssetTypes)
    {
      //----------------------------
      // Special case for plugin list.
      if (assetType == MR::Manager::kAsset_PluginList)
      {
        // Basic tutorials only use morpheme core so doesn't have any plugin restrictions
        continue;
      }

      //----------------------------
      // Grab locate function for this asset type
      const MR::AssetLocateFn locateFn = MR::Manager::getInstance().getAssetLocateFn(assetType);
      if (!locateFn)
      {
        //----------------------------
        // This may happen if you compiled your assets using an asset compiler with additional plug-ins registered
        // but are using a runtime with different modules. For more details see MR::registerCoreAssets() (called from
        // MR::Manager::initMorphemeLib()) and MR::initMorphemePhysics()
        NMP_DEBUG_MSG("error: Failed to locate core asset (type=%u, ID=%u)!\n", assetType, assetID);
        return NULL;
      }

      //----------------------------
      // If the asset is already registered just bump the reference count, if its a new ID the asset is loaded below
      void* const registeredAsset = (void*)MR::Manager::getInstance().getObjectPtrFromObjectID(assetID);
      if (registeredAsset)
      {
        asset = registeredAsset;
      }
      else
      {
        //----------------------------
        // Allocate memory to store the asset for runtime use.
        //
        // The memory is freed as the reference count goes to zero in unloadAssets()
        void* const bundleAsset = asset;
        asset = NMPMemoryAllocateFromFormat(assetMemReqs).ptr;
        NMP::Memory::memcpy(asset, bundleAsset, assetMemReqs.size);

        //----------------------------
        // Locate the asset (in-place pointer fix-up)
        if (!locateFn(assetType, asset))
        {
          NMP_DEBUG_MSG("error: Failed to locate core asset (type=%u, ID=%u)!\n", assetType, assetID);
          return NULL;
        }

        //----------------------------
        // Register the object (initialises the reference count to zero)
        if (!MR::Manager::getInstance().registerObject(asset, assetType, assetID))
        {
          NMP_DEBUG_MSG("error: Failed to register asset (type=%u, ID=%u)!\n", assetType, assetID);
          return NULL;
        } 

        //----------------------------
        // Increment reference count
        MR::Manager::incObjectRefCount(assetID);

        //----------------------------
        // In case of the animation rig we want to hold onto the pointer so we can print out bone data
        if (assetType == MR::Manager::kAsset_Rig)
        {
          NMP_STDOUT("Object found at %p with id 0x%08x:  ", asset, assetID);
          NMP_STDOUT("ANIMRIGDEF");
          *rig = (MR::AnimRigDef*)asset;
        }

        //----------------------------
        // In case of the network definition we also return the GUID for use by COMMS/LiveLink.
        if (assetType == MR::Manager::kAsset_NetworkDef)
        {
          NMP_ASSERT(!networkDef);    // We only expect one network definition per bundle
          networkDef = (MR::NetworkDef*)asset;
        }
      }  
    }
    else
    {
      //----------------------------
      // Allocate memory to store the asset for runtime use.
      void* const bundleAsset = asset;
      asset = NMPMemoryAllocateFromFormat(assetMemReqs).ptr;
      NMP::Memory::memcpy(asset, bundleAsset, assetMemReqs.size);

      //----------------------------
      // Locate the asset (in-place pointer fix-up).
      if (assetType == MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup::kAsset_SimpleAnimRuntimeIDtoFilenameLookup)
      {
        NMP_ASSERT(!(*animFileLookUp)); // We only expect one filename lookup per bundle.
        NMP_STDOUT("SIMPLEANIMRUNTIMEIDTOFILENAMELOOKUP", assetType);
        *animFileLookUp = (MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup*)asset;
        (*animFileLookUp)->locate();
      }
    }
  }

  return networkDef;
}

//----------------------------------------------------------------------------------------------------------------------
void AssetLoaderBasic::unLoadBundle(void* bundle, size_t bundleSize)
{
  MR::Manager::AssetType  assetType;
  void*                   asset;
  NMP::Memory::Format     assetMemReqs;
  MR::RuntimeAssetID      assetID;
  uint8_t*                fileGuid = 0;

  MR::UTILS::SimpleBundleReader reader(bundle, (uint32_t)bundleSize);
  while (reader.readNextAsset(assetType, assetID, fileGuid, asset, assetMemReqs))
  {
    if (assetType < MR::Manager::kAsset_NumAssetTypes)
    {
      //----------------------------
      // Special case for plugin list.
      if (assetType == MR::Manager::kAsset_PluginList)
      {
        //----------------------------
        // For basic tutorials we ignore any plugin restrictions
        continue;
      }

      //----------------------------
      // Unregister and free the asset if it's no longer referenced.
      if (MR::Manager::decObjectRefCount(assetID) == 0)
      {
        void* const asset = const_cast<void*>(MR::Manager::getInstance().getObjectPtrFromObjectID(assetID));
        MR::Manager::getInstance().unregisterObject(assetID);
        NMP::Memory::memFree(asset);
      }
    }
  }
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------