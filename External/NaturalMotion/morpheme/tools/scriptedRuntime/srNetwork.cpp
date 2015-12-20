// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

// ---------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMLoggingMemoryAllocator.h"
#include "physics/mrPhysics.h"
#include "physics/PhysX3/mrPhysicsRigPhysX3Articulation.h"
#include "euphoria/erCharacterDef.h"

#include "scriptedRuntimeApp.h"
#include "srNetwork.h"

extern void SqErrFunc(const SQChar *s, ...);

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
bool validatePluginList(const NMP::OrderedStringTable& pluginList)
{
  const char* euphoriaPlugin = "acPluginEuphoria_target_"NM_PLATFORM_FORMAT_STRING;
  const char* debugEuphoriaPlugin = "acPluginEuphoria_target_"NM_PLATFORM_FORMAT_STRING"_debug";
  const char* physX3Plugin = "acPluginPhysX3_target_"NM_PLATFORM_FORMAT_STRING;
  const char* debugPhysX3Plugin = "acPluginPhysX3_target_"NM_PLATFORM_FORMAT_STRING"_debug";

  // Assets with no plugins can always be processed
  if (pluginList.getNumEntries() == 0)
  {
    return true;
  }

  // PhysX3 assets can be processed if the first plugin is physX3
  if (pluginList.getNumEntries() > 0)
  {
    const char* pluginListEntry = pluginList.getEntryString(0);
    if ((NMP_STRCMP(pluginListEntry, physX3Plugin) != 0 && NMP_STRCMP(pluginListEntry, debugPhysX3Plugin) != 0))
    {
      return false;
    }
  }

  // Euphoria assets can be processed if the second plugin is euphoria
  if (pluginList.getNumEntries() > 1)
  {
    const char* pluginListEntry = pluginList.getEntryString(1);
    if ((NMP_STRCMP(pluginListEntry, euphoriaPlugin) != 0 && NMP_STRCMP(pluginListEntry, debugEuphoriaPlugin) != 0))
    {
      return false;
    }
  }

  return true;
}

// ---------------------------------------------------------------------------------------------------------------------
} // unnamed namespace


namespace Network
{

// ---------------------------------------------------------------------------------------------------------------------
MR::NetworkDef *loadDefinition(const char *fileName, size_t& defDataFootprint)
{
  void *fileBuffer;
  static char fullPath[MAX_PATH_LENGTH];
  getFullPath(fullPath, MAX_PATH_LENGTH, fileName);
  NMP_STRNCAT_S(fullPath, MAX_PATH_LENGTH, ".nmb");

  int64_t bufferSize = 0;
  int64_t bytesRead = NMP::NMFile::allocAndLoad(fullPath, &fileBuffer, &bufferSize, NMP_VECTOR_ALIGNMENT);

  // We failed to load. The file probably doesn't exist
  if (bytesRead == -1)
  {
    SqErrFunc("Could not load definition '%s'", fullPath);
    return 0;
  }

  MR::NetworkDef *networkDef = 0;
  std::vector<ER::BodyDef *> bodyDefs;
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup *netDefSpecificAnimIDtoFilename = 0;

  // Step through each asset in the file and register it.
  MR::UTILS::SimpleBundleReader reader(fileBuffer, (uint32_t) bytesRead);

  MR::Manager::AssetType          assetType;
  void*                           asset;
  uint8_t*                        fileGuid = 0;
  NMP::Memory::Format             assetMemReqs;
  MR::RuntimeAssetID              assetID;

  // this tracks how much memory is taken up by def data
  defDataFootprint = 0;

  while(reader.readNextAsset(assetType, assetID, fileGuid, asset, assetMemReqs))
  {
    //----------------------------
    // Special case for plugin list.  If validation fails the bundle is unsuitable for the this runtime.
    if (assetType == MR::Manager::kAsset_PluginList)
    {
      NMP::OrderedStringTable* pluginList = (NMP::OrderedStringTable*)asset;
      pluginList->locate();
      if (!validatePluginList(*pluginList))
      {
        // Plugin list validation failed.  The plugin order must be acPluginPhysX3 followed
        // by acPluginEuphoria.  A sub set of these is valid. e.g. no plugins or just acPluginPhysX3."
        NMP_ASSERT_FAIL();
        return false;
      }
      continue;
    }

    if (assetType < MR::Manager::kAsset_NumAssetTypes)
    {
      // Does the asset already exist ?
      if(MR::Manager::getInstance().getObjectPtrFromObjectID(assetID) != 0)
      {
        continue;
      }

      // Locate the copy
      MR::AssetLocateFn locateFn = MR::Manager::getInstance().getAssetLocateFn(assetType);

      NMP_ASSERT(locateFn);
      locateFn(assetType, asset);

      if (assetType == MR::Manager::kAsset_NetworkDef)
      {
        networkDef = (MR::NetworkDef *)asset;
      }

      // Register it
      MR::Manager::getInstance().registerObject(asset, assetType, assetID);

      if (assetType == MR::Manager::kAsset_BodyDef)
      {
        bodyDefs.push_back((ER::BodyDef *)asset);
      }

      defDataFootprint += assetMemReqs.size;
    }
    else if (assetType == (MR::Manager::AssetType)MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup::kAsset_SimpleAnimRuntimeIDtoFilenameLookup)
    {
      ((MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup *)asset)->locate();
      netDefSpecificAnimIDtoFilename = (MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup *)asset;
    }
  }

  // If all the net def and the anim ID lookup table have been loaded then pre-load all the anims
  if (networkDef && netDefSpecificAnimIDtoFilename)
  {
    // MR::Manager::initAnimFileHandlingFunctions(networkAnimLoader, networkAnimUnLoader);
    for(MR::AnimSetIndex i = 0; i < networkDef->getNumAnimSets(); i++)
    {
      // Fixup the animations and rigToAnimMaps.
      networkDef->loadAnimations(i, netDefSpecificAnimIDtoFilename);
    }
  }

  if (!bodyDefs.empty())
  {
    const uint32_t animSetCount = networkDef->getNumAnimSets();
    for (uint16_t animSetIndex = 0; animSetIndex != animSetCount; ++animSetIndex)
    {
      std::vector<ER::BodyDef *>::iterator itEnd = bodyDefs.end();
      for (std::vector<ER::BodyDef *>::iterator it = bodyDefs.begin(); it != itEnd; ++it)
      {
        ER::BodyDef *bodyDef = *it;
        if (getPhysicsRigDef(networkDef, animSetIndex) == bodyDef->getPhysicsRigDef())
        {
          MR::AttribAddress bodyDefAddress((MR::AttribDataSemantic)ER::ATTRIB_SEMANTIC_BODY_DEF,
            MR::NETWORK_NODE_ID, // Owning node ID (i.e. the network)
            MR::INVALID_NODE_ID, // Target node ID
            MR::VALID_FOREVER, // This is valid forever
            animSetIndex); // Anim set index to associate with

          // Get the body def attribute data for this animation set. The attrib data should have been
          // allocated by the network builder plugin.
          ER::AttribDataBodyDef* bodyDefAttribData = (ER::AttribDataBodyDef*)networkDef->getAttribData(bodyDefAddress);
          NMP_ASSERT(bodyDefAttribData);

          // Fixup the body def attrib data pointer
          bodyDefAttribData->m_bodyDef = bodyDef;
        }
      }
    }
  }

  return networkDef;
}
//----------------------------------------------------------------------------------------------------------------------
// consts used here for array sizes
const uint32_t g_maxModules = 120;
const uint32_t g_maxModuleNameLength = 64;

// ---------------------------------------------------------------------------------------------------------------------
MR::Network *createInstance(
  MR::NetworkDef* networkDef,
  MR::InstanceDebugInterface* debugInterface,
  NMP::LoggingMemoryAllocator* persistantAllocator,
  MR::PhysicsScenePhysX3* physicsScene,
  const NMP::Matrix34& initialTM,
  size_t& networkInstFootprint,
  ER::RootModule** euphoriaRootModule)
{  
  MR::DispatcherBasic *dispatcher = MR::DispatcherBasic::createAndInit();
  dispatcher->setDebugInterface(debugInterface);

  static const size_t tempDataSize = 1024 * 2024;
  NMP::Memory::Format tempAllocAllocatorFormat = NMP::FastHeapAllocator::getMemoryRequirements(tempDataSize, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource tempAllocResource = NMPMemoryAllocateFromFormat(tempAllocAllocatorFormat);
  NMP_ASSERT(tempAllocResource.ptr);
  NMP::FastHeapAllocator* tempAllocator = NMP::FastHeapAllocator::init(tempAllocResource, tempDataSize, NMP_VECTOR_ALIGNMENT); 

  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(MR::CharacterControllerInterfacePhysX3), NMP_VECTOR_ALIGNMENT);
  MR::CharacterControllerInterfacePhysX3 *characterController = new(alignedMemory) MR::CharacterControllerInterfacePhysX3();

  NMP::Memory::Format networkFormat = MR::Network::getMemoryRequirements(networkDef);
  networkInstFootprint = networkFormat.size;

  MR::Network *network = MR::Network::createAndInit(networkDef, dispatcher, tempAllocator, persistantAllocator, characterController);
  MR::AnimSetIndex activeAnimSetIndex = network->getActiveAnimSetIndex();

  MR::AnimRigDef *rig = networkDef->getRig(activeAnimSetIndex);

  network->setCharacterController(characterController); 

  MR::PhysicsRigDef *physicsRigDef = (MR::PhysicsRigDef *)getPhysicsRigDef(networkDef, activeAnimSetIndex);
  MR::PhysicsRigPhysX3Articulation *physicsRig = 0;
  if (physicsRigDef)
  {
    NMP::Memory::Format format = MR::PhysicsRigPhysX3Articulation::getMemoryRequirements(physicsRigDef);
    NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(format);
    physicsRig = (MR::PhysicsRigPhysX3Articulation*)MR::PhysicsRigPhysX3Articulation::init(
      resource, 
      physicsRigDef, 
      physicsScene, 
      physx::PX_DEFAULT_CLIENT,
      15, // All bits
      rig,
      getAnimToPhysicsMap(networkDef, activeAnimSetIndex),
      1 << MR::GROUP_CHARACTER_PART,
      (1 << MR::GROUP_CHARACTER_CONTROLLER) | (1 << MR::GROUP_NON_COLLIDABLE));
    physicsRig->setRigAndAnimToPhysicsMap(network->getActiveRig(), getAnimToPhysicsMap(networkDef, activeAnimSetIndex));

    for (uint32_t i = 0; i<physicsRig->getNumParts(); i++)
    {
      physx::PxArticulationLink *link = physicsRig->getPartPhysX3Articulation(i)->getArticulationLink();
      physx::PxShape *shape;
      link->getShapes(&shape, 1);
      physx::PxFilterData data;
      data.word0 = 1<<MR::GROUP_CHARACTER_PART;
      shape->setQueryFilterData(data);
    }

    physicsRig->setKinematicPos(NMP::Vector3(1001.0f, 1001.0f, 1001.0f));
    setPhysicsRig(network, physicsRig);
  }

  network->setCharacterPropertiesWorldRootTransform(initialTM);

  bool isBehavioural = networkDef->containsNodeWithFlagsSet(MR::NodeDef::NODE_FLAG_IS_BEHAVIOURAL);
  if (isBehavioural)
  {
    NM_BEHAVIOUR_LIB_NAMESPACE::NMBehaviourLibrary behaviourLibrary;
    behaviourLibrary.init();

    ER::RootModule *module = behaviourLibrary.createNetwork();

    if (euphoriaRootModule)
      *euphoriaRootModule = module;

    ER::CharacterDef *characterDef = new ER::CharacterDef();
    characterDef->create(networkDef, behaviourLibrary.getBehaviourDefs());

    MR::InstanceDebugInterface* debugInterface = dispatcher->getDebugInterface();

    NMP_ASSERT(physicsRig);
    ER::Character* character = characterDef->createInstance(
      rig, 
      physicsRig, 
      module, 
      (1<<MR::GROUP_CHARACTER_PART | 1<<MR::GROUP_CHARACTER_CONTROLLER),
      false,
      debugInterface);
#if defined(MR_OUTPUT_DEBUGGING)
    // Initialise the module names here
    char allModuleNames[g_maxModules][g_maxModuleNameLength];
    char allModuleParentNames[g_maxModules][g_maxModuleNameLength];
    char* moduleNames[g_maxModules];
    char* moduleParentNames[g_maxModules];
    for (uint32_t i = 0; i < g_maxModules; ++i)
    {
      moduleNames[i] = (char*) & (allModuleNames[i]);
      moduleParentNames[i] = (char*) & (allModuleParentNames[i]);
    }
    int32_t numModules = 0;
    if (character->m_euphoriaRootModule)
    {
      character->m_euphoriaRootModule->getModuleNames(&allModuleNames[0][0], &allModuleParentNames[0][0], g_maxModuleNameLength, numModules);
    }
    NMP_ASSERT((uint32_t)numModules <= g_maxModules);
    debugInterface->initModuleNames(numModules, moduleNames, moduleParentNames);
#endif

    networkSetCharacter(network, character);
  }

  return network;
}

} // namespace Network
