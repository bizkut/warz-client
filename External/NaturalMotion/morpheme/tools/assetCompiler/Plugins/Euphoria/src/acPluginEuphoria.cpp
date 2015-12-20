// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "ACPluginEuphoria.h"
#include "BodyDefBuilder.h"
#include "InteractionProxyDefBuilder.h"
#include "assetProcessor/AnimationPoseBuilder.h"
#include "physics/mrPhysicsRigDef.h"
#include "NodeBehaviourBuilder.h"
#include "NodePerformanceBehaviourBuilder.h"
#include "NodeBehaviourGrouperBuilder.h"
#include "euphoria/erAttribData.h"
#include "NodeLimbInfoBuilder.h"
#include "NodeOperatorHitBuilder.h"
#include "NodeOperatorContactReporterBuilder.h"
#include "NodeOperatorRollDownStairsBuilder.h"
#include "NodeOperatorPhysicalConstraintBuilder.h"
#include "NodeOperatorOrientationInFreeFallBuilder.h"
#include "NodeOperatorFallOverWallBuilder.h"
#include "MessageHitBuilder.h"
#include "NodeTrajectoryOverrideBuilder.h"
#include "euphoria/Nodes/erNodes.h"
#include "euphoria/erAttribData.h"
#include "assetProcessor/AnimSource/animSourcePreprocessor.h"
#include "NMPlatform/NMBuffer.h"

//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
void initBuilders(AssetProcessor& assetProc)
{
  NodeDefBuilder* nodeBuilder;

  nodeBuilder = new NodeBehaviourGrouperBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_BEHAVIOUR_GROUPER, nodeBuilder);

  nodeBuilder = new NodeBehaviourBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_BEHAVIOUR, nodeBuilder);

  nodeBuilder = new NodePerformanceBehaviourBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_PERFORMANCE_BEHAVIOUR, nodeBuilder);

  nodeBuilder = new NodeOperatorHitBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_HIT, nodeBuilder);

  nodeBuilder = new NodeOperatorContactReporterBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_CONTACTREPORTER, nodeBuilder);

  nodeBuilder = new NodeOperatorRollDownStairsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_ROLLDOWNSTAIRS, nodeBuilder);

  nodeBuilder = new NodeOperatorPhysicalConstraintBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_PHYSICALCONSTRAINT, nodeBuilder);

  nodeBuilder = new NodeOperatorOrientationInFreeFallBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_ORIENTATIONINFREEFALL, nodeBuilder);

  nodeBuilder = new NodeOperatorFallOverWallBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_FALLOVERWALL, nodeBuilder);

  nodeBuilder = new NodeLimbInfoBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_LIMB_INFO, nodeBuilder);

  nodeBuilder = new NodeTrajectoryOverrideBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_TRAJECTORY_OVERRIDE, nodeBuilder);
}

//----------------------------------------------------------------------------------------------------------------------
void initMessages(AssetProcessor& assetProc)
{
  MessageBuilder* messageBuilder;

  messageBuilder = new MessageHitBuilder();
  assetProc.registerMessageBuilder(MESSAGE_TYPE_HIT, messageBuilder);
}

//----------------------------------------------------------------------------------------------------------------------
ACPluginEuphoria::ACPluginEuphoria(AP::AssetProcessor* assetProc, MR::Manager* manager) :
  AssetCompilerPlugin(assetProc, manager)
{
}

//----------------------------------------------------------------------------------------------------------------------
ACPluginEuphoria::~ACPluginEuphoria()
{
}

//----------------------------------------------------------------------------------------------------------------------
const ProcessedAsset* ACPluginEuphoria::processAsset(
  ME::AssetExport* const sourceAsset,
  MR::Manager::AssetType NMP_UNUSED(assetTypeToGenerate))
{
  MR::Manager::AssetType  assetType = MR::Manager::kAsset_Invalid;
  const char*             assetName = 0;
  ME::GUID                guid = 0;
  NMP::Memory::Resource   assetMemory = { 0, NMP::Memory::Format(0, 0) };
  ProcessedAsset*         processedAsset = 0;

  if (dynamic_cast<BodyDefExport*>(sourceAsset) != 0)
  {
    // Source asset is a EuphoriaBodyDef.
    BodyDefExport* bodySource = dynamic_cast<BodyDefExport*>(sourceAsset);
    ME::GUID physicsRigGUID = bodySource->getPhysicsRigGUID();
    ME::GUID rigGUID = bodySource->getRigGUID();

    const ProcessedAsset* physicsRigAsset = m_assetProcessor->findProcessedAsset(physicsRigGUID);
    const ProcessedAsset* rigAsset = m_assetProcessor->findProcessedAsset(rigGUID);
    if (!physicsRigAsset || !rigAsset)
    {
      // To build a BodyDef, the corresponding PhysicsRigDef and Rig need to be loaded.
      // This is just to prevent the case in which the BodyDef asset is loaded
      // before the network is. It shouldn't actually happen because usually the first asset
      // to be loaded is the network.
      return NULL;
    }

    // Get the rig and the physics rig def asset from their guids.
    MR::PhysicsRigDef* physicsRigDef = (MR::PhysicsRigDef*) physicsRigAsset->assetMemory.ptr;
    MR::AnimRigDef* rig = (MR::AnimRigDef*) rigAsset->assetMemory.ptr;

    m_assetProcessor->beginNewAssetProcessing();

    assetType = MR::Manager::kAsset_BodyDef;
    assetName = bodySource->getName();
    guid = bodySource->getGUID();
    assetMemory = createBodyDef(bodySource, rig->getNumBones());

    // Add the asset to the processed asset list
    processedAsset = m_assetProcessor->addAsset(assetMemory, assetType, assetName, guid);
    bodySetRigAndPhysicsRig(processedAsset, rig, physicsRigDef);
  }
  else if (dynamic_cast<InteractionProxyExport*>(sourceAsset) != 0)
  {
    m_assetProcessor->beginNewAssetProcessing();
    // Source asset is a EuphoriaInteractionProxy.
    InteractionProxyExport* proxySource = dynamic_cast<InteractionProxyExport*>(sourceAsset);
    assetType = MR::Manager::kAsset_InteractionProxyDef;
    assetName = proxySource->getName();
    guid = proxySource->getGUID();
    assetMemory = createInteractionProxy(proxySource);
    // Add the asset to the processed asset list
    processedAsset = m_assetProcessor->addAsset(assetMemory, assetType, assetName, guid);
  }
  else
  {
    // Unrecognised asset type
    m_assetProcessor->cancelNewAssetProcessing();
    return NULL;
  }

  // Return the processed asset
  return processedAsset;
}

//----------------------------------------------------------------------------------------------------------------------
bool ACPluginEuphoria::dislocateAsset(ProcessedAsset* asset)
{
  void* object = asset->assetMemory.ptr;

  switch (asset->assetType)
  {
  case MR::Manager::kAsset_BodyDef:
    dislocateBodyDef(object);
    break;

  case MR::Manager::kAsset_InteractionProxyDef:
    ((ER::InteractionProxyDef*)object)->dislocate();
    break;

    // Add here other euphoria related assets dislocate.
  default:
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ACPluginEuphoria::prepareAnimationMetaData(
  const std::vector<ME::AssetExport*>& NMP_UNUSED(sourceAssets),
  AnimationMetadata** NMP_UNUSED(metadata))
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource ACPluginEuphoria::createBodyDef(ME::BodyDefExport* bodyDefExport, uint32_t numAnimBones)
{
  NMP::Memory::Resource objectMemory = { NULL, NMP::Memory::Format(0, 0) };

  objectMemory = BodyDefBuilder::init(m_assetProcessor, bodyDefExport, numAnimBones);

  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource ACPluginEuphoria::createInteractionProxy(ME::InteractionProxyExport* interactionProxyExport)
{
  NMP::Memory::Resource objectMemory = { NULL, NMP::Memory::Format(0, 0) };

  objectMemory = InteractionProxyDefBuilder::init(m_assetProcessor, interactionProxyExport);

  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
ACPluginEuphoria* g_pluginEuphoria = NULL;

#ifndef NMP_MEMORY_LOGGING
//----------------------------------------------------------------------------------------------------------------------
void loadPlugin(MR::Manager* manager, const NMP::Memory::Config* c)
{
  if (c)
    NMP::Memory::init(*c);
  else
    NMP::Memory::init();

  if (manager)
  {
    MR::Manager::setInstance(manager);
  }

  ER::initEuphoria(0, NULL);
}
#else

//----------------------------------------------------------------------------------------------------------------------
void loadPluginWithLogging(MR::Manager* manager, const NMP::Memory::Config* c NMP_MEMORY_LOGGING_DECL)
{
  if (c)
    NMP::Memory::init(*c, *NMP_MEMORY_LOGGING_CONFIG_NAME);
  else
    NMP::Memory::init(*NMP_MEMORY_LOGGING_CONFIG_NAME);

  if (manager)
  {
    MR::Manager::setInstance(manager);
  }

  ER::initEuphoria(0, NULL);
}
#endif // NMP_MEMORY_LOGGING

//----------------------------------------------------------------------------------------------------------------------
void finalisePluginInitialisation()
{
  ER::finaliseInitEuphoria();
}

//----------------------------------------------------------------------------------------------------------------------
void registerPluginWithProcessor(
  AssetProcessor*            assetProc,
  MR::Manager*               manager,
  uint32_t                   NMP_UNUSED(numDispatchers),
  MR::Dispatcher**           NMP_UNUSED(dispatchers))
{

  NMP_VERIFY_MSG(
    g_pluginEuphoria == NULL,
    "The asset compiler plugin has already been initialised.");

  g_pluginEuphoria = new ACPluginEuphoria(assetProc, manager);

 
  initBuilders(*assetProc);
  initMessages(*assetProc);

  registerBuilderPlugin(new EuphoriaNetworkBuilderPlugin());
}

//----------------------------------------------------------------------------------------------------------------------
void destroyPluginData()
{
  if (g_pluginEuphoria)
  {
    delete g_pluginEuphoria;
    g_pluginEuphoria = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void unloadPlugin()
{
  ER::destroyEuphoria();
  NMP::Memory::shutdown();
}

//----------------------------------------------------------------------------------------------------------------------
// EuphoriaNetworkBuilderPlugin
//----------------------------------------------------------------------------------------------------------------------
void EuphoriaNetworkBuilderPlugin::populateNetDefSemanticLookupTable(
  const ME::NetworkDefExport*      netDefExport,
  MR::SemanticLookupTable*         netDefSemanticLookupTable)
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint32_t numAnimationSets = animLibraryExport->getNumAnimationSets(); 

  for (uint32_t i = 0; i < numAnimationSets; ++i)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(i);
    NMP_VERIFY(animSetExport);

    const ME::InteractionProxyExport* interactionProxyExport = animSetExport->getInteractionProxy();
    if (interactionProxyExport)
    {
      netDefSemanticLookupTable->addLookupIndex(ER::ATTRIB_SEMANTIC_INTERACTION_PROXY_DEF);
      break;
    }
  }


  for (uint32_t i = 0; i < numAnimationSets; ++i)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(i);
    NMP_VERIFY(animSetExport);

    const ME::BodyDefExport* bodyDefExport = animSetExport->getBodyDef();
    if (bodyDefExport)
    {
      netDefSemanticLookupTable->addLookupIndex(ER::ATTRIB_SEMANTIC_BODY_DEF);

      // Anim poses
      const ME::AnimationPoseDefExport* defaultPose = bodyDefExport->getAnimationPose("DefaultPose");
      if (defaultPose)
      {
        netDefSemanticLookupTable->addLookupIndex(ER::ATTRIB_SEMANTIC_DEFAULT_POSE);
      }

      const ME::AnimationPoseDefExport* guidePose = bodyDefExport->getAnimationPose("GuidePose");
      if (guidePose)
      {
        netDefSemanticLookupTable->addLookupIndex(ER::ATTRIB_SEMANTIC_GUIDE_POSE);
      }

      // NB: Only one body def can be registered by this loop
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EuphoriaNetworkBuilderPlugin::getPluginMemoryRequirements(const ME::NetworkDefExport* netDefExport)
{
  NMP::Memory::Format result(0, MR_ATTRIB_DATA_ALIGNMENT);

  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //-----------------
  // Stuff per animation set.
  for (uint32_t i = 0; i < animLibraryExport->getNumAnimationSets(); ++i)
  {
    // Body def attributes
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(i);
    NMP_VERIFY(animSetExport);
    const ME::BodyDefExport* bodyDefExport = animSetExport->getBodyDef();
    if (bodyDefExport)
    {
      // Add space for the Body def attrib data.
      result += ER::AttribDataBodyDef::getMemoryRequirements();

      // Default pose
      const ME::AnimationPoseDefExport *defaultPose = bodyDefExport->getAnimationPose("DefaultPose");
      if (defaultPose)
      {
        result += AnimationPoseBuilder::getMemoryRequirements(animSetExport, defaultPose);
      }

      // Guide pose
      const ME::AnimationPoseDefExport *guidePose = bodyDefExport->getAnimationPose("GuidePose");
      if (guidePose)
      {
        result += AnimationPoseBuilder::getMemoryRequirements(animSetExport, guidePose);
      }
    }

    const ME::InteractionProxyExport* interactionProxyDefExport = animSetExport->getInteractionProxy();
    if (interactionProxyDefExport)
    {
      // Add space for the Body def attrib data.
      result += ER::AttribDataInteractionProxyDef::getMemoryRequirements();
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const ME::BodyDefExport* EuphoriaNetworkBuilderPlugin::buildBodyDefAttribData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  const ME::AnimationSetExport*    animSetExport,
  AP::AssetProcessor*              processor,
  MR::AnimSetIndex                 animSetIndex)
{
  // Find the body def, if there is one.
  const ME::BodyDefExport* bodyDefExport = animSetExport->getBodyDef();
  ER::BodyDef* bodyDef = NULL;
  if (bodyDefExport)
  {
    const ProcessedAsset* bodyDefAsset = processor->findProcessedAsset(bodyDefExport->getGUID());
    if (bodyDefAsset)
      bodyDef = (ER::BodyDef*) bodyDefAsset->assetMemory.ptr;

    // Create an AttribData to hold a pointer to the body def (may be a NULL pointer).
    ER::AttribDataBodyDef* bodyDefAttribData = ER::AttribDataBodyDef::init(memRes, bodyDef);

    // Construct body and limb def anim pose data
    AP::BodyDefBuilder::buildBodyDefAnimPoseData(
      bodyDef,
      animSetIndex,
      processor,
      // (need non-const anim set export for first access to data file lookup)
      const_cast<ME::AnimationSetExport*>(animSetExport));

    uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(ER::ATTRIB_SEMANTIC_BODY_DEF, animSetIndex);
    NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
    netDefAttribDataArray[lookupIndex].set(bodyDefAttribData, ER::AttribDataBodyDef::getMemoryRequirements());
  }

  return bodyDefExport;
}

//----------------------------------------------------------------------------------------------------------------------
const ME::InteractionProxyExport* EuphoriaNetworkBuilderPlugin::buildInteractionProxyDefAttribData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  const ME::AnimationSetExport*    animSetExport,
  AP::AssetProcessor*              processor,
  MR::AnimSetIndex                 animSetIndex)
{
  // Find the interaction proxy def, if there is one.
  const ME::InteractionProxyExport* interactionProxyDefExport = animSetExport->getInteractionProxy();
  if (interactionProxyDefExport)
  {
    const ProcessedAsset* interactionProxyDefAsset = processor->findProcessedAsset(interactionProxyDefExport->getGUID());
    ER::InteractionProxyDef* interactionProxyDef = 0;
    if (interactionProxyDefAsset)
    {
      interactionProxyDef = (ER::InteractionProxyDef*) interactionProxyDefAsset->assetMemory.ptr;
    }

    // Create an AttribData to hold a pointer to the interaction proxy def (may be a NULL pointer).
    ER::AttribDataInteractionProxyDef* interactionProxyDefAttribData = 
      ER::AttribDataInteractionProxyDef::init(memRes, interactionProxyDef);

    uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(ER::ATTRIB_SEMANTIC_INTERACTION_PROXY_DEF, animSetIndex);
    NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
    netDefAttribDataArray[lookupIndex].set(interactionProxyDefAttribData, ER::AttribDataInteractionProxyDef::getMemoryRequirements());
  }

  return interactionProxyDefExport;
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaNetworkBuilderPlugin::buildAnimPoseAttribData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         netDefAttribDataArraySize,
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  const ME::AnimationSetExport*    animSetExport,
  const ME::BodyDefExport*         bodyDefExport,
  AP::AssetProcessor*              processor,
  MR::AnimSetIndex                 animSetIndex)
{
  const ME::AnimationPoseDefExport* defaultPose = bodyDefExport->getAnimationPose("DefaultPose");
  if(defaultPose)
  {
    AnimationPoseBuilder::constructPose(
      memRes,
      netDefAttribDataArray,
      netDefAttribDataArraySize,
      netDefSemanticLookupTable,
      animSetIndex,
      processor,
      animSetExport,
      defaultPose,
      ER::ATTRIB_SEMANTIC_DEFAULT_POSE);
  }

  const ME::AnimationPoseDefExport* guidePose = bodyDefExport->getAnimationPose("GuidePose");
  if(guidePose)
  {
    AnimationPoseBuilder::constructPose(
      memRes,
      netDefAttribDataArray,
      netDefAttribDataArraySize,
      netDefSemanticLookupTable,
      animSetIndex,
      processor,
      animSetExport,
      guidePose,
      ER::ATTRIB_SEMANTIC_GUIDE_POSE);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool EuphoriaNetworkBuilderPlugin::initPluginData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         netDefAttribDataArraySize,
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  AP::AssetProcessor*              processor,
  const ME::NetworkDefExport*      netDefExport)
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();

  // Stuff per animation set.
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);

    // Create an AttribData to hold a pointer to the body (may be a NULL pointer).
    const ME::BodyDefExport* bodyDefExport = buildBodyDefAttribData(memRes, netDefAttribDataArray, netDefAttribDataArraySize, 
      netDefSemanticLookupTable, animSetExport, processor, animSetIndex);

    // Create an AttribData to hold a pointer to the interaction proxy (may be a NULL pointer).
    buildInteractionProxyDefAttribData(memRes, netDefAttribDataArray, netDefAttribDataArraySize, 
      netDefSemanticLookupTable, animSetExport, processor, animSetIndex);

    // AttribData for Euphoria Poses
    if (bodyDefExport)
    {
      buildAnimPoseAttribData(memRes, netDefAttribDataArray, netDefAttribDataArraySize, netDefSemanticLookupTable, 
        animSetExport, bodyDefExport, processor, animSetIndex);
    }
 }

  return true;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
