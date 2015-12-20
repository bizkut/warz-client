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
#include "acPhysicsCore.h"
#include "PhysicsRigDefBuilder.h"
#include "physics/mrPhysicsRigDef.h"

#pragma warning(disable: 4996)
#include "NodePhysicsBuilder.h"
#include "NodePhysicsGrouperBuilder.h"
#include "NodeSetNonPhysicsTransformsBuilder.h"
#include "NodeApplyPhysicsJointLimitsBuilder.h"
#include "NodeOperatorPhysicsInfoBuilder.h"
#include "NodeExpandLimitsBuilder.h"
#include "NodeControlParamPhysicsObjectPointerBuilder.h"
#include "NodeOperatorApplyImpulseBuilder.h"

#include "NodePhysicsTransitBuilder.h"
#include "NodePhysicsTransitSyncEventsBuilder.h"

#include "TransitConditionDefBuilderGroundContact.h"
#include "TransitConditionDefBuilderPhysicsAvailable.h"
#include "TransitConditionDefBuilderPhysicsInUse.h"
#include "TransitConditionDefBuilderPhysicsMoving.h"
#include "TransitConditionDefBuilderSKDeviation.h"

#include "physics/Nodes/mrPhysicsNodes.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
void initBuilders(AssetProcessor& assetProc)
{
  NodeDefBuilder* nodeBuilder;

  //---------------------
  // Node builders.
  nodeBuilder = new NodePhysicsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_PHYSICS, nodeBuilder);

  nodeBuilder = new NodePhysicsGrouperBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_PHYSICS_GROUPER, nodeBuilder);

  nodeBuilder = new NodeSetNonPhysicsTransformsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_SET_NON_PHYSICS_TRANSFORMS, nodeBuilder);

  nodeBuilder = new NodeApplyPhysicsJointLimitsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_APPLY_PHYSICS_JOINT_LIMITS, nodeBuilder);

  nodeBuilder = new NodeOperatorPhysicsInfoBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_PHYSICS_INFO, nodeBuilder);

  // We override the AC Core's transit builders and replace them with our own, which have greater capabilities.
  nodeBuilder = new NodePhysicsTransitBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_TRANSIT_PHYSICS, nodeBuilder);

  nodeBuilder = new NodePhysicsTransitSyncEventsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS, nodeBuilder);

  nodeBuilder = new NodeExpandLimitsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_EXPAND_LIMITS, nodeBuilder);

  nodeBuilder = new NodeOperatorApplyImpulseBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_APPLYIMPULSE, nodeBuilder);

  nodeBuilder = new NodeControlParamPhysicsObjectPointerBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_PHYSICS_OBJECT_POINTER, nodeBuilder);

  //---------------------
  // TransitCondition builders.

  TransitConditionDefBuilder* conditionBuilder = new TransitConditionDefBuilderGroundContact();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_GROUND_CONTACT_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderPhysicsAvailable();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_PHYSICS_AVAILABLE_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderPhysicsInUse();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_PHYSICS_IN_USE_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderPhysicsMoving();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_PHYSICS_MOVING_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderSKDeviation();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_SK_DEVIATION_ID, conditionBuilder);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsNetworkBuilderPlugin::populateNetDefSemanticLookupTable(
  const ME::NetworkDefExport*      netDefExport,
  MR::SemanticLookupTable*         netDefSemanticTable)
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  for (uint32_t i = 0; i < animLibraryExport->getNumAnimationSets(); ++i)
  {
    // Add space for the anim to physics maps.
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(i);

    const ME::PhysicsRigDefExport* physicsRigExport = animSetExport->getPhysicsRigDef();
    if (physicsRigExport)
    {
      netDefSemanticTable->addLookupIndex(MR::ATTRIB_SEMANTIC_PHYSICS_RIG_DEF);
      netDefSemanticTable->addLookupIndex(MR::ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP);
      return;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysicsNetworkBuilderPlugin::getPluginMemoryRequirements(const ME::NetworkDefExport* netDefExport)
{
  NMP::Memory::Format result(0, NMP_NATURAL_TYPE_ALIGNMENT);

  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //-----------------
  // Stuff per animation set.
  for (uint32_t i = 0; i < animLibraryExport->getNumAnimationSets(); ++i)
  {
    // Add space for the anim to physics maps.
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(i);
    const ME::RigExport* rigExport = animSetExport->getRig();
    uint32_t numChannels = rigExport->getNumJoints();
    const ME::PhysicsRigDefExport* physicsRigExport = animSetExport->getPhysicsRigDef();
    uint32_t numPhysicsChannels = 0;
    if (physicsRigExport)
    {
      numPhysicsChannels = physicsRigExport->getNumParts();

      // Add space for the physics rig def attrib data.
      result += MR::AttribDataPhysicsRigDef::getMemoryRequirements();
      result += MR::AttribDataAnimToPhysicsMap::getMemoryRequirements(numChannels, numPhysicsChannels);
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsNetworkBuilderPlugin::buildPhysicsRigAttribData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  AP::AssetProcessor*              processor,
  const ME::AnimationSetExport*    animSetExport,
  MR::AnimSetIndex                 animSetIndex)
{
  // Find the physics rig, if there is one.
  const ME::PhysicsRigDefExport* physicsRigDefExport = animSetExport->getPhysicsRigDef();
  MR::PhysicsRigDef* physicsRigDef = NULL;
  if (physicsRigDefExport)
  {
    const ProcessedAsset* physicsRigDefAsset = processor->findProcessedAsset(physicsRigDefExport->getGUID());
    if (physicsRigDefAsset)
      physicsRigDef = (MR::PhysicsRigDef*) physicsRigDefAsset->assetMemory.ptr;

    // Create an AttribData to hold a pointer to the rig (may be a NULL pointer).
    MR::AttribDataPhysicsRigDef* physRigAttribData = MR::AttribDataPhysicsRigDef::init(memRes, physicsRigDef);

    uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_PHYSICS_RIG_DEF, animSetIndex);
    NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
    netDefAttribDataArray[lookupIndex].set(physRigAttribData, MR::AttribDataPhysicsRigDef::getMemoryRequirements());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsNetworkBuilderPlugin::buildAnimToPhysicsMapAttibData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  const ME::AnimationSetExport*    animSetExport,
  MR::AnimRigDef*                  animRigDef,
  MR::AnimSetIndex                 animSetIndex)
{
  // Construct the AttribData.
  uint32_t numPhysicsChannels = 0;
  const ME::PhysicsRigDefExport* physicsRigDefExport = animSetExport->getPhysicsRigDef();
  if (physicsRigDefExport)
  {
    numPhysicsChannels = physicsRigDefExport->getNumParts();

    MR::AttribDataAnimToPhysicsMap* animToPhysicsMapAttribData = MR::AttribDataAnimToPhysicsMap::init(
          memRes,
          animRigDef->getNumBones(),
          numPhysicsChannels,
          MR::IS_DEF_ATTRIB_DATA);

    uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP, animSetIndex);
    NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
    netDefAttribDataArray[lookupIndex].set(
          animToPhysicsMapAttribData, 
          MR::AttribDataAnimToPhysicsMap::getMemoryRequirements(animRigDef->getNumBones(), numPhysicsChannels));

    const ME::RigExport* animRigDefExport = animSetExport->getRig();

    // Populate the new mapping structure.
    populateAnimToPhysicsMap(animToPhysicsMapAttribData->m_animToPhysicsMap, animRigDefExport, physicsRigDefExport);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsNetworkBuilderPlugin::populateAnimToPhysicsMap(
  MR::AnimToPhysicsMap*          animToPhysics,
  const ME::RigExport*           animRig,
  const ME::PhysicsRigDefExport* physicsRig)
{
  for (int32_t i = 0; i < (int32_t)animRig->getNumJoints(); ++i)
  {
    animToPhysics->m_mapAnimToPhysics[i] = -1;
  }

  if (physicsRig)
  {
    for (int32_t i = 0; i < (int32_t)physicsRig->getNumJoints(); ++i)
    {
      animToPhysics->m_mapPhysicsToAnim[i] = -1;
    }

    for (int32_t i = 0; i < (int32_t)animRig->getNumJoints(); ++i)
    {
      for (int32_t j = 0; j != (int32_t)physicsRig->getNumParts(); ++j)
      {
        const JointExport* joint = animRig->getJoint(i);
        const PhysicsPartExport* part = physicsRig->getPart(j);

        const char* jointBodyPlanTag = joint->getBodyPlanTag();
        const char* partBodyPlanTag = part->getBodyPlanTag();

        if (jointBodyPlanTag && partBodyPlanTag && *jointBodyPlanTag != '\0')
        {
          if (strcmp(jointBodyPlanTag, partBodyPlanTag) == 0)
          {
            animToPhysics->makeMappingBetweenPhysicsAndAnim(j, i);
          }
        }
      }
    }

    // Physics bones not paired with animation will be given default settings.
    animToPhysics->initializeBonesFilter();
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsNetworkBuilderPlugin::initPluginData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         netDefAttribDataArraySize,
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  AP::AssetProcessor*              processor,
  const ME::NetworkDefExport*      netDefExport)
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //-----------------
  // Stuff per animation set.
  for (MR::AnimSetIndex i = 0; i < animLibraryExport->getNumAnimationSets(); ++i)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(i);

    // Find the animation rig (there must be one).
    const ME::RigExport* animRigExport = animSetExport->getRig();
    NMP_VERIFY_MSG(animRigExport != 0, "AnimSet %s does not contain a Rig", animSetExport->getName());
    const ProcessedAsset* animRigAsset = processor->findProcessedAsset(animRigExport->getGUID());
    NMP_VERIFY_MSG(animRigAsset != 0, "AnimSet %s does not contain a Rig", animSetExport->getName());
    MR::AnimRigDef* animRigDef = (MR::AnimRigDef*) animRigAsset->assetMemory.ptr;
    NMP_VERIFY_MSG(animRigDef != 0, "AnimSet %s does not contain a Rig", animSetExport->getName());

    // Create an AttribData to hold a pointer to the physics rig (may be a NULL pointer).
    buildPhysicsRigAttribData(memRes, netDefAttribDataArray, netDefAttribDataArraySize, netDefSemanticLookupTable, processor, animSetExport, i);

    // Create the animation sets map between its Animation and Physics rigs.
    buildAnimToPhysicsMapAttibData(memRes, netDefAttribDataArray, netDefAttribDataArraySize, netDefSemanticLookupTable, animSetExport, animRigDef, i);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsDriverBuilder
//----------------------------------------------------------------------------------------------------------------------
PhysicsDriverDataBuilder::~PhysicsDriverDataBuilder()
{
}

//----------------------------------------------------------------------------------------------------------------------
// ACPluginPhysics
//----------------------------------------------------------------------------------------------------------------------
PhysicsDriverDataBuilder* ACPluginPhysics::sm_physicsDriverBuilder = 0;

//----------------------------------------------------------------------------------------------------------------------
ACPluginPhysics::ACPluginPhysics(AP::AssetProcessor* assetProc, MR::Manager* manager) :
  AssetCompilerPlugin(assetProc, manager)
{
}

//----------------------------------------------------------------------------------------------------------------------
ACPluginPhysics::~ACPluginPhysics()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool ACPluginPhysics::dislocateAsset(ProcessedAsset* asset)
{
  void* object = asset->assetMemory.ptr;

  switch (asset->assetType)
  {
  case MR::Manager::kAsset_PhysicsRigDef:
    NMP_VERIFY_MSG(sm_physicsDriverBuilder, "registerPhysicsDriverBuilder must be called with a valid driver before attempting to dislocate PhysicsRigDefs.");
    sm_physicsDriverBuilder->dislocateDriverData((MR::PhysicsRigDef*)object);
    ((MR::PhysicsRigDef*)object)->dislocate();
    break;

  default:
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ACPluginPhysics::prepareAnimationMetaData(
  const std::vector<ME::AssetExport*>& NMP_UNUSED(sourceAssets),
  AnimationMetadata** NMP_UNUSED(metadata))
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const ProcessedAsset* ACPluginPhysics::processAsset(
  ME::AssetExport* const sourceAsset,
  MR::Manager::AssetType assetTypeToGenerate)   
{
  MR::Manager::AssetType  assetType = MR::Manager::kAsset_Invalid;
  const char*             assetName = 0;
  ME::GUID                guid = 0;
  NMP::Memory::Resource   assetMemory = { 0, NMP::Memory::Format(0, 0) };

  //------------------------------------------
  // Has this asset already been processed (Identified by a pointer to the source asset structure).
  const ProcessedAsset* processedAsset = m_assetProcessor->findProcessedAsset(sourceAsset->getGUID(), assetTypeToGenerate);
  if (processedAsset != 0)
  {
    // add additional dependency if needed
    uint32_t lastNewAsset = m_assetProcessor->getLastNewAssetStackValue();
    if (lastNewAsset)
      processedAsset->parendIDs.insert(lastNewAsset);

    return processedAsset;
  }

  m_assetProcessor->beginNewAssetProcessing();

  //------------------------------------------
  // Create a runtime asset from the source asset.
  // Determines the correct asset type to create by attempting to dynamically cast the source asset to one of the possible types.
  if (dynamic_cast<PhysicsRigDefExport*>(sourceAsset) != 0)
  {
    // Source asset is a PhysicsRigDef.
    PhysicsRigDefExport* rigSource = dynamic_cast<PhysicsRigDefExport*>(sourceAsset);
    assetType = MR::Manager::kAsset_PhysicsRigDef;
    assetName = rigSource->getName();
    guid = rigSource->getGUID();
    assetMemory = createPhysicsRigDef(rigSource);
  }
  else
  {
    // Unrecognised asset type
    m_assetProcessor->cancelNewAssetProcessing();
    return NULL;
  }

  // Add the asset to the processed asset list and return
  return m_assetProcessor->addAsset(assetMemory, assetType, assetName, guid);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource ACPluginPhysics::createPhysicsRigDef(ME::PhysicsRigDefExport* physicsRigDefExport)
{
  NMP::Memory::Resource objectMemory = { NULL, NMP::Memory::Format(0, 0) };

  NMP_VERIFY_MSG(sm_physicsDriverBuilder, "registerPhysicsDriverBuilder must be called with a valid driver before calling createPhysicsRigDef.");
  objectMemory = PhysicsRigDefBuilder::init(m_assetProcessor, sm_physicsDriverBuilder, physicsRigDefExport);

  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
bool ACPluginPhysics::registerPhysicsDriverBuilder(PhysicsDriverDataBuilder* plugin)
{
  if (sm_physicsDriverBuilder)
  {
    return false;
  }
  sm_physicsDriverBuilder = plugin;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
ACPluginPhysics* g_pluginPhysics = NULL;

//----------------------------------------------------------------------------------------------------------------------
bool registerPhysicsDriverBuilder(PhysicsDriverDataBuilder* plugin)
{
  bool result = ACPluginPhysics::registerPhysicsDriverBuilder(plugin);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void loadPluginPhysics(MR::Manager* manager, const NMP::Memory::Config* c NMP_MEMORY_LOGGING_DECL)
{
#ifdef NMP_MEMORY_LOGGING
  if(c)
    NMP::Memory::init(*c, *NMP_MEMORY_LOGGING_CONFIG_NAME);
  else
    NMP::Memory::init(*NMP_MEMORY_LOGGING_CONFIG_NAME);
#else//MEM_LOGGING_FUNC_ARGS
  if(c)
    NMP::Memory::init(*c);
  else
    NMP::Memory::init();
#endif//MEM_LOGGING_FUNC_ARGS

  if (manager)
  {
    MR::Manager::setInstance(manager);
  }

  MR::initMorphemePhysics(0, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
void finalisePluginInitialisationPhysics()
{
  MR::finaliseInitMorphemePhysics();
}

//----------------------------------------------------------------------------------------------------------------------
void registerPluginWithProcessorPhysics(
  AssetProcessor*  assetProc,
  MR::Manager*     manager,
  uint32_t         numDispatchers,
  MR::Dispatcher** dispatchers)
{
  NMP_VERIFY_MSG(
    g_pluginPhysics == NULL,
    "The asset compiler plugin has already been initialised.");

  g_pluginPhysics = new ACPluginPhysics(assetProc, manager);

  initBuilders(*assetProc);

  // Register the task functions with the dispatchers
  for (uint32_t i = 0; i < numDispatchers; ++i)
    MR::CoreTaskIDs::registerNMPhysicsTasks(dispatchers[i]);

  registerBuilderPlugin(new PhysicsNetworkBuilderPlugin());
}

//----------------------------------------------------------------------------------------------------------------------
void deinitialisePluginPhysics()
{
  if (g_pluginPhysics)
  {
    delete g_pluginPhysics;
    g_pluginPhysics = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void unloadPluginPhysics()
{
  NMP::Memory::shutdown();
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
