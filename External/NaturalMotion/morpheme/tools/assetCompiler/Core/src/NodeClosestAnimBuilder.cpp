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
#include <tchar.h>
#include "NetworkDefBuilder.h"
#include "NodeClosestAnimBuilder.h"
#include "assetProcessor/AssetProcessorUtils.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeClosestAnim.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrBlendOps.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// NodeClosestAnimPreInitData
//----------------------------------------------------------------------------------------------------------------------
class NodeClosestAnimPreInitData
{
public:
  NodeClosestAnimPreInitData();
  ~NodeClosestAnimPreInitData();

  void clear();

public:
  uint32_t                                            m_sourceNodeCount;
  
  NMP::Memory::Format                                 m_memReqsSetupData;
  MR::AttribDataClosestAnimDef*                       m_setupData;
  
  std::vector<NMP::Memory::Format>                    m_memReqsSetupDataAnimSet;
  std::vector<MR::AttribDataClosestAnimDefAnimSet*>   m_setupDataAnimSet;
  std::vector<MR::NodeID>                             m_descendantNodeIds; // All node ids under the closest anim node
};

//----------------------------------------------------------------------------------------------------------------------
NodeClosestAnimPreInitData::NodeClosestAnimPreInitData() : m_sourceNodeCount(0), m_setupData(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
NodeClosestAnimPreInitData::~NodeClosestAnimPreInitData()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
void NodeClosestAnimPreInitData::clear()
{
  // Set up data
  if (m_setupData)
  {
    NMP::Memory::memFree(m_setupData);
  }
  m_setupData = NULL;
  m_memReqsSetupData.set(0, 0);
  
  // Set up data (Anim set specific)
  uint32_t numAnimSets = (uint32_t) m_setupDataAnimSet.size();
  for (uint32_t i = 0; i < numAnimSets; ++i)
  {
    if (m_setupDataAnimSet[i])
    {
      NMP::Memory::memFree(m_setupDataAnimSet[i]);
    }
  }
  m_setupDataAnimSet.clear();
  m_memReqsSetupDataAnimSet.clear();
  m_descendantNodeIds.clear();
}


//----------------------------------------------------------------------------------------------------------------------
// NodeClosestAnimBuilder
//----------------------------------------------------------------------------------------------------------------------
bool NodeClosestAnimBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeClosestAnimBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CLOSEST_ANIM);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeClosestAnimBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  uint32_t numChildNodes = 0;
  nodeDefDataBlock->readUInt(numChildNodes, "SourceNodeCount");
  NMP_VERIFY(numChildNodes > 0);

  // Set child node IDs.
  for (uint32_t iChild = 0; iChild < numChildNodes; ++iChild)
  {
    CHAR nodeParamName[256];
    // NB: The manifest stores source node Ids starting from 0. TODO: change this for coherency?
    sprintf_s(nodeParamName, "Source%dNodeID", iChild);
    MR::NodeID sourceNodeID;
    readNodeID(nodeDefDataBlock, nodeParamName, sourceNodeID);
    childNodeIDs.push_back(sourceNodeID);
  }

  // Control parameters
  readDataPinChildNodeID(nodeDefDataBlock, "DeadBlendWeight", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void appendUniqueChildNodes(NodeClosestAnimPreInitData* userData, NetworkDefCompilationInfo* netDefCompilationInfo, MR::NodeID nodeID)
{
  std::vector<MR::NodeID>* childNodeIDs = &userData->m_descendantNodeIds;

  bool alreadyAdded = std::find(childNodeIDs->begin(), childNodeIDs->end(), nodeID) != childNodeIDs->end();
  if (!alreadyAdded)
  {
    childNodeIDs->push_back(nodeID);

    NodeDefDependency& deps = netDefCompilationInfo->getNodeDefDependency(nodeID);
    for (uint32_t i = 0; i < (uint32_t)deps.m_childNodeIDs.size(); ++i)
    {
      appendUniqueChildNodes(userData, netDefCompilationInfo, deps.m_childNodeIDs[i]->m_nodeID);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeClosestAnimBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "DeadBlendWeight", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);



  AP::AssetProcessor::InitNetworkInstanceFunc initNetworkInstanceFunc = processor->getInitNetworkInstanceFunc();
  AP::AssetProcessor::UpdateNetworkInstanceFunc updateNetworkInstanceFunc = processor->getUpdateNetworkInstanceFunc();
  AP::AssetProcessor::ReleaseNetworkInstanceFunc releaseNetworkInstanceFunc = processor->getReleaseNetworkInstanceFunc();
  NMP_VERIFY(initNetworkInstanceFunc);
  NMP_VERIFY(updateNetworkInstanceFunc);
  NMP_VERIFY(releaseNetworkInstanceFunc);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint32_t nodeID = nodeDefExport->getNodeID();
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeID);
  const std::vector<MR::AnimSetIndex> & validAnimSets = nodeDefDep.m_animSets;

  const float eps = 1e-4f;

  // Info
  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount > 0);
  if (sourceNodeCount == 0)
    return;

  bool precomputeSourcesOffline = true;
  nodeDefDataBlock->readBool(precomputeSourcesOffline, "PrecomputeSourcesOffline");
    
  bool useVelocity = false;
  nodeDefDataBlock->readBool(useVelocity, "UseVelocity");

  // Allocate the pre init data
  NodeClosestAnimPreInitData* userData = new NodeClosestAnimPreInitData;
  netDefCompilationInfo->attachUserData(nodeID, userData);
  userData->m_sourceNodeCount = sourceNodeCount;

  //---------------------------
  // Get the array of animation rigs
  std::vector<MR::AnimRigDef*> animRigs;
  for (uint32_t i = 0; i < validAnimSets.size(); ++i)
  {
    MR::AnimSetIndex animSetIndex = validAnimSets[i];
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);
    const ME::RigExport* animRigExport = animSetExport->getRig();
    NMP_VERIFY(animRigExport);
    const ProcessedAsset* animRigAsset = processor->findProcessedAsset(animRigExport->getGUID());
    NMP_VERIFY(animRigAsset);
    MR::AnimRigDef* animRigDef = (MR::AnimRigDef*) animRigAsset->assetMemory.ptr;
    NMP_VERIFY(animRigDef);
    animRigs.push_back(animRigDef);
  }

  //---------------------------
  // Get the array of joint weights
  CHAR paramNumAlphas[256];
  CHAR paramAlpha[256];
  std::vector<MR::AttribDataFloatArray*> jointWeightsArray;
  for (uint32_t i = 0; i < validAnimSets.size(); ++i)
  {
    MR::AnimSetIndex animSetIndex = validAnimSets[i];
    MR::AnimRigDef* animRigDef = animRigs[animSetIndex];
    NMP_VERIFY(animRigDef);
    
    sprintf_s(paramNumAlphas, "NumAlphaValuesSet_%d", animSetIndex + 1);
    uint32_t numAlphaValues;
    nodeDefDataBlock->readUInt(numAlphaValues, paramNumAlphas);
    NMP_VERIFY(numAlphaValues > 0);

    NMP::Memory::Format memReqsJointWeights = MR::AttribDataFloatArray::getMemoryRequirements(numAlphaValues);
    NMP::Memory::Resource memResJointWeights = NMPMemoryAllocateFromFormat(memReqsJointWeights);
    MR::AttribDataFloatArray* jointWeights = MR::AttribDataFloatArray::init(memResJointWeights, numAlphaValues, MR::IS_DEF_ATTRIB_DATA);
    jointWeightsArray.push_back(jointWeights);

    float alphaValue;
    for (uint32_t i = 0; i < numAlphaValues; ++i)
    {
      sprintf_s(paramAlpha, "Alpha_%d_Set_%d", i + 1, animSetIndex + 1);
      nodeDefDataBlock->readFloat(alphaValue, paramAlpha);
      jointWeights->m_values[i] = alphaValue;
    }
    
    // Strip out any weights for the world root and trajectory channels
    uint32_t trajIndex = animRigDef->getTrajectoryBoneIndex();
    jointWeights->m_values[0] = 0.0f;
    jointWeights->m_values[trajIndex] = 0.0f;
  }

  //---------------------------
  // Get the conglomerated vector of weighted joint names
  std::vector<const char*> conglomeratedRigBoneNames;
  std::vector<uint32_t> numEntriesArray;
  for (uint32_t i = 0; i < validAnimSets.size(); ++i)
  {
    MR::AnimSetIndex animSetIndex = validAnimSets[i];
    MR::AnimRigDef* animRigDef = animRigs[animSetIndex];
    NMP_VERIFY(animRigDef);
    const MR::AttribDataFloatArray* jointWeights = jointWeightsArray[animSetIndex];
    NMP_VERIFY(jointWeights);
    NMP_VERIFY(jointWeights->m_numValues == animRigDef->getNumBones());
    
    uint32_t numEntries = 0;
    for (uint32_t rigChanIndex = 0; rigChanIndex < jointWeights->m_numValues; ++rigChanIndex)
    {
      if (jointWeights->m_values[rigChanIndex] > eps)
      {        
        const char* boneName = animRigDef->getBoneName(rigChanIndex);
        int32_t animChanIndex = findIndexOfStringInVector(boneName, conglomeratedRigBoneNames);
        if (animChanIndex < 0)
        {
          conglomeratedRigBoneNames.push_back(boneName);
        }
        ++numEntries;
      }
    }
    numEntriesArray.push_back(numEntries);
  }
  uint32_t numAnimJoints = (uint32_t) conglomeratedRigBoneNames.size();

  //---------------------------
  // Compute the animation set rig channel and weight mappings
  for (uint32_t i = 0; i < validAnimSets.size(); ++i)
  {
    MR::AnimSetIndex animSetIndex = validAnimSets[i];
    MR::AnimRigDef* animRigDef = animRigs[animSetIndex];
    NMP_VERIFY(animRigDef);
    const MR::AttribDataFloatArray* jointWeights = jointWeightsArray[animSetIndex];
    NMP_VERIFY(jointWeights);
    uint32_t numEntries = numEntriesArray[animSetIndex];

    // Allocate the memory for the anim set specific setup data
    NMP::Memory::Format memReqsSetupData = MR::AttribDataClosestAnimDefAnimSet::getMemoryRequirements(numEntries);    
    NMP::Memory::Resource memResSetupData = NMPMemoryAllocateFromFormat(memReqsSetupData);
    MR::AttribDataClosestAnimDefAnimSet* setupDataAnimSet = 
      MR::AttribDataClosestAnimDefAnimSet::init(memResSetupData, numEntries, MR::IS_DEF_ATTRIB_DATA);

    userData->m_memReqsSetupDataAnimSet.push_back(memReqsSetupData);
    userData->m_setupDataAnimSet.push_back(setupDataAnimSet);

    // Set the rig channel and weight mappings
    uint32_t entryIndex = 0;
    for (uint32_t rigChanIndex = 0; rigChanIndex < jointWeights->m_numValues; ++rigChanIndex)
    {
      if (jointWeights->m_values[rigChanIndex] > eps)
      {
        const char* boneName = animRigDef->getBoneName(rigChanIndex);
        int32_t animChanIndex = findIndexOfStringInVector(boneName, conglomeratedRigBoneNames);
        NMP_VERIFY(animChanIndex != -1);

        setupDataAnimSet->m_weights[entryIndex] = jointWeights->m_values[rigChanIndex];
        setupDataAnimSet->m_rigChannels[entryIndex] = (uint16_t)rigChanIndex;
        setupDataAnimSet->m_animChannels[entryIndex] = (uint16_t)animChanIndex;
        ++entryIndex;
      }
    }
  }
  
  //---------------------------
  // Optimize the distribution of the animation channels. Note: we can shuffle the order of
  // the animation channels how we like. For each animation data set we store the number of
  // validly weighted animation channels that need to be compared. It's beneficial to change
  // the order of the animation channels so that there are as many non zero weighted channels
  // as possible up front.
  optimizeAnimChannelDistribution(
    conglomeratedRigBoneNames, // In/Out re-arranged rig bone names for the animation channels
    userData->m_setupDataAnimSet); // In/Out entry to anim channel maps for each anim data set

  //---------------------------
  // Count the number of nodes below the closest anim
  for (int32_t iChild = 0; iChild < sourceNodeCount; ++iChild)
  {
    CHAR nodeParamName[256];
    sprintf_s(nodeParamName, "Source%dNodeID", iChild);
    MR::NodeID sourceNodeID;
    readNodeID(nodeDefDataBlock, nodeParamName, sourceNodeID);
    appendUniqueChildNodes(userData, netDefCompilationInfo, sourceNodeID);
  }
  uint32_t numChildNodeIds = (uint32_t)userData->m_descendantNodeIds.size();
  MR::NodeID* childNodeIds = numChildNodeIds == 0 ? NULL : &userData->m_descendantNodeIds[0];

  //---------------------------
  // Allocate the memory for the non anim set specific setup data
  NMP::Memory::Format memReqsSetupData = MR::AttribDataClosestAnimDef::getMemoryRequirements(
    precomputeSourcesOffline,
    sourceNodeCount,
    numChildNodeIds,
    numAnimJoints,
    useVelocity);
  NMP::Memory::Resource memResSetupData = NMPMemoryAllocateFromFormat(memReqsSetupData);
  MR::AttribDataClosestAnimDef* setupData = MR::AttribDataClosestAnimDef::init(
    memResSetupData,
    precomputeSourcesOffline,
    sourceNodeCount,
    numChildNodeIds,
    childNodeIds,
    numAnimJoints,
    useVelocity,
    MR::IS_DEF_ATTRIB_DATA);
  userData->m_memReqsSetupData = memReqsSetupData;
  userData->m_setupData = setupData;

  //---------------------------
  // Compute the position scale factor for the influence between position and orientation
  float positionScaleFactor = computePositionScaleFactor(animRigs);
  float orientationScaleFactor = 1.0f;

  //---------------------------
  // Initialise the setup data

  // Influences
  float inflBetPosAndOri = 0.0f;
  nodeDefDataBlock->readFloat(inflBetPosAndOri, "InfluenceBetweenPositionAndOrientation");
  inflBetPosAndOri = NMP::clampValue(inflBetPosAndOri, 0.0f, 1.0f);
  setupData->m_influenceBetweenPosAndOrient = inflBetPosAndOri;
  setupData->m_positionScaleFactor = positionScaleFactor * positionScaleFactor;
  setupData->m_orientationScaleFactor = orientationScaleFactor * orientationScaleFactor;

  // Root rotation blending
  bool useRootRotationBlending = true;
  nodeDefDataBlock->readBool(useRootRotationBlending, "UseRootRotationBlending");
  setupData->m_useRootRotationBlending = useRootRotationBlending;
  float blendDurationFraction = 0.0f;
  nodeDefDataBlock->readFloat(blendDurationFraction, "BlendDuration");
  setupData->m_fractionThroughSource = NMP::clampValue(blendDurationFraction, 0.0f, 1.0f);
  float matchTolerance = NM_PI;
  nodeDefDataBlock->readFloat(matchTolerance, "MatchTolerance"); // Exported in radians
  matchTolerance = NMP::clampValue(matchTolerance, 0.0f, NM_PI);
  setupData->m_maxRootRotationAngle = cos(0.5f * matchTolerance); // w component of offset quat for fast testing

  NMP::Vector3 rootRotationAxis(0.0f, 0.0f, 0.0f);
  uint32_t upAxisIndex = 1;
  nodeDefDataBlock->readUInt(upAxisIndex, "UpAxisIndex");
  rootRotationAxis[upAxisIndex] = 1.0f;
  setupData->m_rootRotationAxis = rootRotationAxis;
  setupData->m_upAlignAtt.forRotation(rootRotationAxis, NMP::Vector3(0, 1.0f, 0));

  if (precomputeSourcesOffline && numAnimJoints > 0)
  {
    //---------------------------
    // Allocate the memory for the conglomerated channel transforms
    NMP::Memory::Format memReqsChanBuf(sizeof(NMP::Vector3) * numAnimJoints, NMP_VECTOR_ALIGNMENT);
    std::vector<NMP::Vector3*> channelPosKeys0Array, channelPosKeys1Array;
    std::vector<NMP::Quat*> channelAttKeys0Array, channelAttKeys1Array;
    
    // First frame poses
    for (int32_t iChild = 0; iChild < sourceNodeCount; ++iChild)
    {
      NMP::Memory::Resource memResPosKeys0 = NMPMemoryAllocateFromFormat(memReqsChanBuf);
      channelPosKeys0Array.push_back((NMP::Vector3*) memResPosKeys0.ptr);
      NMP::Memory::Resource memResAttKeys0 = NMPMemoryAllocateFromFormat(memReqsChanBuf);
      channelAttKeys0Array.push_back((NMP::Quat*) memResAttKeys0.ptr);
    }
    
    // Second frame poses
    if (setupData->m_useVelocity)
    {
      for (int32_t iChild = 0; iChild < sourceNodeCount; ++iChild)
      {
        NMP::Memory::Resource memResPosKeys1 = NMPMemoryAllocateFromFormat(memReqsChanBuf);
        channelPosKeys1Array.push_back((NMP::Vector3*) memResPosKeys1.ptr);
        NMP::Memory::Resource memResAttKeys1 = NMPMemoryAllocateFromFormat(memReqsChanBuf);
        channelAttKeys1Array.push_back((NMP::Quat*) memResAttKeys1.ptr);
      }
    }

    //---------------------------
    // Allocate a temporary buffer for the velocities and encoding buffer
    NMP::Memory::Resource memResVec3Buffer = NMPMemoryAllocateFromFormat(memReqsChanBuf);
    NMP::Vector3* channelVec3Buffer = (NMP::Vector3*) memResVec3Buffer.ptr;

    //---------------------------
    // Compute the transforms data
    AP::AssetProcessor::InitNetworkInstanceArgs initArgs;
    initArgs.m_queueTrajectory = true;
    initArgs.m_queueTransforms = true;
    initArgs.m_queueSampledEvents = false;
    initArgs.m_queueSyncEventTrack = false;

    AP::AssetProcessor::UpdateNetworkInstanceArgs updateArgs;
    updateArgs.m_updateTime = 1.0f / 30.0f; // 30 fps
    updateArgs.m_absTime = false;
    updateArgs.m_queueTrajectory = true;
    updateArgs.m_queueTransforms = true;
    updateArgs.m_queueSampledEvents = false;
    updateArgs.m_queueSyncEventTrack = false;

    // Iterate over the connected source nodes
    for (int32_t iChild = 0; iChild < sourceNodeCount; ++iChild)
    {
      NMP::Vector3* animChannelPosKeys0 = channelPosKeys0Array[iChild];
      NMP::Quat* animChannelAttKeys0 = channelAttKeys0Array[iChild];     
    
      // Get the connected source node id
      CHAR nodeParamName[256];
      sprintf_s(nodeParamName, "Source%dNodeID", iChild);
      MR::NodeID sourceNodeID;
      readNodeID(nodeDefDataBlock, nodeParamName, sourceNodeID);          

      // Build the sub-network def
      NetworkDefBuilderBase* networkDefBuilder = processor->getNetworkDefBuilder();
      NMP_VERIFY(networkDefBuilder);
      NMP::Memory::Resource memRes = networkDefBuilder->buildSubNetworkDef((MR::NodeID) sourceNodeID, false);
      MR::NetworkDef* networkDef = (MR::NetworkDef*)memRes.ptr;
      NMP_VERIFY(networkDef);
      MR::NodeID rootNodeID = networkDef->getRootNodeID();

      // Iterate over the animation sets
      for (uint32_t i = 0; i < validAnimSets.size(); ++i)
      {
        MR::AnimSetIndex animSetIndex = validAnimSets[i];
        MR::AttribDataClosestAnimDefAnimSet* setupDataAnimSet = userData->m_setupDataAnimSet[animSetIndex];
        NMP_VERIFY(setupDataAnimSet);
        uint32_t numEntries = setupDataAnimSet->m_numEntries;
        MR::AnimRigDef* animRigDef = animRigs[animSetIndex];
        NMP_VERIFY(animRigDef);
        uint32_t numRigJoints = animRigDef->getNumBones();
        
        //---------------------------
        // Allocate the memory for the temporary animation transforms buffer
        NMP::Memory::Format buffMemReqs;
        NMP::Memory::Format internalBuffMemReqs;
        MR::AttribDataTransformBuffer::getPosQuatMemoryRequirements(
          numRigJoints,
          buffMemReqs,
          internalBuffMemReqs);
        NMP::Memory::Resource buffMemRes = NMPMemoryAllocateFromFormat(buffMemReqs);
        MR::AttribDataTransformBuffer* tempAnimTransformsAttrib = MR::AttribDataTransformBuffer::initPosQuat(
          buffMemRes,
          internalBuffMemReqs,
          numRigJoints,
          MR::IS_DEF_ATTRIB_DATA);
        NMP::DataBuffer* tempAnimTransformsBuffer = tempAnimTransformsAttrib->m_transformBuffer;
        NMP::Vector3* channelPos = tempAnimTransformsBuffer->getPosQuatChannelPos(0);
        NMP_VERIFY(channelPos);
        NMP::Quat* channelAtt = tempAnimTransformsBuffer->getPosQuatChannelQuat(0);
        NMP_VERIFY(channelAtt);

        //---------------------------
        // Initialise a network instance
        initArgs.m_startingAnimSetIndex = animSetIndex;

        bool status = initNetworkInstanceFunc(processor, networkDef, initArgs);          
        NMP_VERIFY(status);
        (void)status;

        MR::Network* network = processor->getNetwork();
        NMP_VERIFY(network);

        // Get the network output transforms
        MR::AttribDataTransformBuffer* transformsBufferAttribNetwork = 
          (MR::AttribDataTransformBuffer*)network->getAttribData(
          MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
          rootNodeID,
          MR::INVALID_NODE_ID,
          MR::VALID_FRAME_ANY_FRAME,
          MR::CURRENT_ANIM_SET_INDEX);
        NMP_VERIFY(transformsBufferAttribNetwork);

        // Copy the network output transforms
        transformsBufferAttribNetwork->m_transformBuffer->copyTo(tempAnimTransformsBuffer);

        // Apply the bind pose to any unused channels
        MR::BlendOpsBase::applyBindPoseToUnusedChannels(
          animRigDef->getBindPose(),
          tempAnimTransformsBuffer);

        // Accumulate the transforms, including alignment rotation for the matching space.
        MR::BlendOpsBase::accumulateTransforms(
          NMP::Vector3(NMP::Vector3::InitZero),
          setupData->m_upAlignAtt,
          tempAnimTransformsBuffer,
          animRigDef,
          tempAnimTransformsBuffer);

        // Extract the required transforms (overwriting any previous values)
        for (uint32_t entryIndex = 0; entryIndex < numEntries; ++entryIndex)
        {
          uint32_t rigChannelIndex = setupDataAnimSet->m_rigChannels[entryIndex];
          uint32_t animChannelIndex = setupDataAnimSet->m_animChannels[entryIndex];
          animChannelPosKeys0[animChannelIndex] = channelPos[rigChannelIndex];
          animChannelAttKeys0[animChannelIndex] = channelAtt[rigChannelIndex];
        }
        
        //---------------------------
        // Update the network instance
        if (setupData->m_useVelocity)
        {
          NMP::Vector3* animChannelPosKeys1 = channelPosKeys1Array[iChild];
          NMP::Quat* animChannelAttKeys1 = channelAttKeys1Array[iChild];

          updateNetworkInstanceFunc(processor, updateArgs);

          transformsBufferAttribNetwork = 
            (MR::AttribDataTransformBuffer*)network->getAttribData(
            MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
            rootNodeID,
            MR::INVALID_NODE_ID,
            MR::VALID_FRAME_ANY_FRAME,
            MR::CURRENT_ANIM_SET_INDEX);
          NMP_VERIFY(transformsBufferAttribNetwork);

          // Copy the network output transforms
          transformsBufferAttribNetwork->m_transformBuffer->copyTo(tempAnimTransformsBuffer);

          // Apply the bind pose to any unused channels
          MR::BlendOpsBase::applyBindPoseToUnusedChannels(
            animRigDef->getBindPose(),
            tempAnimTransformsBuffer);

          // Accumulate the transforms, including alignment rotation for the matching space.
          MR::BlendOpsBase::accumulateTransforms(
            NMP::Vector3(NMP::Vector3::InitZero),
            setupData->m_upAlignAtt,
            tempAnimTransformsBuffer,
            animRigDef,
            tempAnimTransformsBuffer);

          // Extract the required transforms (overwriting any previous values)
          for (uint32_t entryIndex = 0; entryIndex < numEntries; ++entryIndex)
          {
            uint32_t rigChannelIndex = setupDataAnimSet->m_rigChannels[entryIndex];
            uint32_t animChannelIndex = setupDataAnimSet->m_animChannels[entryIndex];
            animChannelPosKeys1[animChannelIndex] = channelPos[rigChannelIndex];
            animChannelAttKeys1[animChannelIndex] = channelAtt[rigChannelIndex];
          }
        }

        //---------------------------
        // Release the sub-network instance
        releaseNetworkInstanceFunc(processor);

        // Tidy up the memory for the temporary animation transforms buffer
        NMP::Memory::memFree(tempAnimTransformsAttrib);
      }

      // Tidy up the sub-network def
      NMP::Memory::memFree(networkDef);
    }

    //---------------------------
    // Compress and store the extracted transforms
    for (int32_t iChild = 0; iChild < sourceNodeCount; ++iChild)
    {
      NMP::Vector3* animChannelPosKeys0 = channelPosKeys0Array[iChild];
      NMP::Quat* animChannelAttKeys0 = channelAttKeys0Array[iChild];

      MR::AttribDataClosestAnimDef::ClosestAnimSourceData* sourceData = setupData->getAnimSourceData(iChild);
      NMP_VERIFY(sourceData);
      
      // Position (in character matching space)
      sourceData->m_sourceTransformsPos->encode(animChannelPosKeys0);
      // Orientation (in character matching space)
      sourceData->m_sourceTransformsAtt->encode(animChannelAttKeys0, channelVec3Buffer);
    }
    
    if (setupData->m_useVelocity)
    {
      for (int32_t iChild = 0; iChild < sourceNodeCount; ++iChild)
      {
        NMP::Vector3* animChannelPosKeys0 = channelPosKeys0Array[iChild];
        NMP::Quat* animChannelAttKeys0 = channelAttKeys0Array[iChild];
        NMP::Vector3* animChannelPosKeys1 = channelPosKeys1Array[iChild];
        NMP::Quat* animChannelAttKeys1 = channelAttKeys1Array[iChild];
        
        MR::AttribDataClosestAnimDef::ClosestAnimSourceData* sourceData = setupData->getAnimSourceData(iChild);
        NMP_VERIFY(sourceData);
        
        // Position velocity (in character matching space)
        computePosVel(
          numAnimJoints,
          updateArgs.m_updateTime,
          animChannelPosKeys0,
          animChannelPosKeys1,
          channelVec3Buffer);
        sourceData->m_sourceTransformsPosVel->encode(channelVec3Buffer);
        
        // Angular velocity (in character matching space)
        computeAngVel(
          numAnimJoints,
          updateArgs.m_updateTime,
          animChannelAttKeys0,
          animChannelAttKeys1,
          channelVec3Buffer);
        sourceData->m_sourceTransformsAngVel->encode(channelVec3Buffer);
      }
    }

    //---------------------------
    // Tidy up
    NMP::Memory::memFree(channelVec3Buffer);

    // First frame transforms
    for (int32_t iChild = 0; iChild < sourceNodeCount; ++iChild)
    {
      NMP::Memory::memFree(channelPosKeys0Array[iChild]);
      NMP::Memory::memFree(channelAttKeys0Array[iChild]);
    }

    // Second frame transforms
    if (setupData->m_useVelocity)
    {
      for (int32_t iChild = 0; iChild < sourceNodeCount; ++iChild)
      {
        NMP::Memory::memFree(channelPosKeys1Array[iChild]);
        NMP::Memory::memFree(channelAttKeys1Array[iChild]);
      }
    }
  }

  //---------------------------
  // Tidy up the temporary anim set specific data
  for (uint32_t i = 0; i < validAnimSets.size(); ++i)
  {
    MR::AnimSetIndex animSetIndex = validAnimSets[i];
    MR::AttribDataFloatArray* jointWeights = jointWeightsArray[animSetIndex];
    NMP_VERIFY(jointWeights);
    NMP::Memory::memFree(jointWeights);
  }

  jointWeightsArray.clear();
  conglomeratedRigBoneNames.clear();
  animRigs.clear();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeClosestAnimBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{  
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  uint32_t nodeID = nodeDefExport->getNodeID();

  NodeClosestAnimPreInitData* userData = (NodeClosestAnimPreInitData*)netDefCompilationInfo->getUserData(nodeID);
  NMP_VERIFY(userData);

  uint32_t numControlParams = 1; 

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint32_t numChildNodes = userData->m_sourceNodeCount;
  
  //---------------------------
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    numChildNodes,       // numChildren
    numControlParams,    // numInputCPConnections
    numAnimSets,         // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Closest anim setup
  result += userData->m_memReqsSetupData;

  //---------------------------
  // Animation set specific attribute data
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t i=0; i < nodeDefDep.m_animSets.size(); ++i)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[i];
    result += userData->m_memReqsSetupDataAnimSet[animSetIndex];
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeClosestAnimBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{  
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CLOSEST_ANIM, "Expecting node type CLOSEST_ANIM");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint16_t numAnimSets = (uint16_t) animLibraryExport->getNumAnimationSets();

  uint32_t nodeID = nodeDefExport->getNodeID();
  NodeClosestAnimPreInitData* userData = (NodeClosestAnimPreInitData*)netDefCompilationInfo->detachUserData(nodeID);
  NMP_VERIFY(userData);
 
  //---------------------------
  // Work out how many attrib datas we actually have.
  uint32_t numChildNodes = userData->m_sourceNodeCount;
  
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    numChildNodes,   // numChildren
    1,               // max num active child nodes
    1,               // numInputCPConnections
    0,               // numOutputCPPins
    numAnimSets,     // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node IDs.
  for (uint32_t iChild = 0; iChild < numChildNodes; ++iChild)
  {
    CHAR nodeParamName[256];
    sprintf_s(nodeParamName, "Source%dNodeID", iChild);
    MR::NodeID sourceNodeID;
    readNodeID(nodeDefDataBlock, nodeParamName, sourceNodeID);
    nodeDef->setChildNodeID(iChild, sourceNodeID);
  }

  //---------------------------
  // Initialise the attrib datas.
 
  //---------------------------
  // Closest anim setup data that is not animation set dependent

  // Initialise the data in the nodeDef resource
  MR::AttribDataClosestAnimDef* setupDataAttrib = (MR::AttribDataClosestAnimDef*) memRes.alignAndIncrement(userData->m_memReqsSetupData);
  NMP_VERIFY(userData->m_setupData);
  MR::AttribDataClosestAnimDef::dislocate(userData->m_setupData);
  NMP::Memory::memcpy(setupDataAttrib, userData->m_setupData, userData->m_memReqsSetupData.size);
  MR::AttribDataClosestAnimDef::locate(setupDataAttrib);

  // Add to the node def attributes
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,  // semantic,    
    0,                                      // animSetIndex,
    setupDataAttrib,                        // attribData,  
    userData->m_memReqsSetupData);          // attribMemReqs

  //---------------------------
  // Animation set dependent attribute data
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];

    NMP::Memory::Format memReqsSetupDataAnimSet = userData->m_memReqsSetupDataAnimSet[animSetIndex];
    MR::AttribDataClosestAnimDefAnimSet* userSetupDataAnimSet = userData->m_setupDataAnimSet[animSetIndex];
    NMP_VERIFY(userSetupDataAnimSet);

    // Initialise the data in the nodeDef resource
    MR::AttribDataClosestAnimDefAnimSet* setupDataAnimSet = (MR::AttribDataClosestAnimDefAnimSet*) memRes.alignAndIncrement(memReqsSetupDataAnimSet);
    MR::AttribDataClosestAnimDefAnimSet::dislocate(userSetupDataAnimSet);
    NMP::Memory::memcpy(setupDataAnimSet, userSetupDataAnimSet, memReqsSetupDataAnimSet.size);
    MR::AttribDataClosestAnimDefAnimSet::locate(setupDataAnimSet);

    // Add to the node def attributes
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, // semantic,    
      animSetIndex,                                   // animSetIndex,
      setupDataAnimSet,                               // attribData,  
      memReqsSetupDataAnimSet);                       // attribMemReqs
  }

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    setupDataAttrib,
    processor->getMessageLogger());

  //---------------------------
  // Tidy up the nodeDef compilation info user data
  delete userData; // This frees the attached memory

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeClosestAnimBuilder::initTaskQueuingFns(
  MR::NodeDef*                          nodeDef,
  NetworkDefCompilationInfo*            netDefCompilationInfo,
  const MR::AttribDataClosestAnimDef*   setupDataAttrib,
  NMP::BasicLogger*                     logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  // The only attributes we mess with are transforms and trajectory delta.
  if (setupDataAttrib->m_useRootRotationBlending)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeClosestAnimQueueTransforms), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeClosestAnimQueueTrajectoryDeltaTransform), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeClosestAnimQueueTrajectoryDeltaAndTransforms), logger);
  }

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeClosestAnimInitInstance), logger);
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeClosestAnimDeleteInstance), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeClosestAnimUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeClosestAnimFindGeneratingNodeForSemantic), logger);
}

//----------------------------------------------------------------------------------------------------------------------
float NodeClosestAnimBuilder::computePositionScaleFactor(const std::vector<MR::AnimRigDef*>& animRigs)
{
  uint32_t numRigs = (uint32_t) animRigs.size();
  float sumMaxBound = 0.0f;

  //------------------------------------
  // Iterate over the animation rigs and compute the maximum bounds from the bind pose transforms
  for (uint32_t index = 0; index < numRigs; ++index)
  {
    const MR::AnimRigDef* rig = animRigs[index];
    NMP_VERIFY(rig);
    uint32_t numRigJoints = rig->getNumBones();

    const MR::AttribDataTransformBuffer* bindPoseTransforms = rig->getBindPose();
    NMP_VERIFY(bindPoseTransforms);

    // Allocate temporary memory for the character space bind pose transforms
    NMP::Memory::Format buffMemReqs;
    NMP::Memory::Format internalBuffMemReqs;
    MR::AttribDataTransformBuffer::getPosQuatMemoryRequirements(
      numRigJoints,
      buffMemReqs,
      internalBuffMemReqs);
    NMP::Memory::Resource buffMemRes = NMPMemoryAllocateFromFormat(buffMemReqs);
    MR::AttribDataTransformBuffer* tempAnimTransformsAttrib = MR::AttribDataTransformBuffer::initPosQuat(
      buffMemRes,
      internalBuffMemReqs,
      numRigJoints,
      MR::IS_DEF_ATTRIB_DATA);

    // Accumulate the transforms
    MR::BlendOpsBase::accumulateTransforms(
      NMP::Vector3(NMP::Vector3::InitZero),
      NMP::Quat(NMP::Quat::kIdentity),
      bindPoseTransforms->m_transformBuffer,
      rig,
      tempAnimTransformsAttrib->m_transformBuffer);

    NMP::Vector3* channelPos = tempAnimTransformsAttrib->m_transformBuffer->getPosQuatChannelPos(0);
    NMP_VERIFY(channelPos);

    //------------------------------------
    // Determine the bounding box of the character  
    NMP::Vector3 workingPos = channelPos[0];
    NMP::Vector3 boundsMin = workingPos;
    NMP::Vector3 boundsMax = workingPos;

    for (uint32_t i = 1; i < numRigJoints; ++i)
    {
      // Update the bounding box info
      workingPos = channelPos[i];
      for (uint32_t k = 0; k < 3; ++k)
      {
        boundsMin[k] = NMP::minimum(workingPos[k], boundsMin[k]);
        boundsMax[k] = NMP::maximum(workingPos[k], boundsMax[k]);
      }
    }

    // Find the largest bounding dimension
    NMP::Vector3 boundsDiff = boundsMax - boundsMin;
    float maxBoundDiff = NMP::maximum(NMP::maximum(boundsDiff.x, boundsDiff.y), boundsDiff.z);

    // Update the average
    sumMaxBound += maxBoundDiff;

    // Tidy up
    NMP::Memory::memFree(tempAnimTransformsAttrib);
  }

  float averageBound = sumMaxBound / numRigs;
  float positionScaleFactor = 1.0f;
  if (averageBound > 1e-7f)
    positionScaleFactor = 2.0f / averageBound; // Character half height

  return positionScaleFactor;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeClosestAnimBuilder::computePosVel(
  uint32_t numAnimJoints,
  float updateTime,
  NMP::Vector3* animChannelPosKeys0,
  NMP::Vector3* animChannelPosKeys1,
  NMP::Vector3* channelVec3Buffer)
{
  float recipDeltaTime = 0.0f;
  if (updateTime > 0)
    recipDeltaTime = 1.0f / updateTime;

  for (uint32_t i = 0; i < numAnimJoints; ++i)
  {
    channelVec3Buffer[i] = (animChannelPosKeys1[i] - animChannelPosKeys0[i]) * recipDeltaTime;
  }
}
    
//----------------------------------------------------------------------------------------------------------------------
void NodeClosestAnimBuilder::computeAngVel(
  uint32_t numAnimJoints,
  float updateTime,
  NMP::Quat* animChannelAttKeys0,
  NMP::Quat* animChannelAttKeys1,
  NMP::Vector3* channelVec3Buffer)
{
  float recipDeltaTimeFac = 0.0f;
  if (updateTime > 0)
    recipDeltaTimeFac = 2.0f / updateTime;

  for (uint32_t i = 0; i < numAnimJoints; ++i)
  {
    NMP::Quat deltaQuat;
    deltaQuat.multiply(animChannelAttKeys1[i], ~animChannelAttKeys0[i]);
    if (deltaQuat.w < 0.0f)
      deltaQuat *= -1.0f; // Ensure in same semi-arc
      
    channelVec3Buffer[i] = deltaQuat.fastLog() * recipDeltaTimeFac;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeClosestAnimBuilder::optimizeAnimChannelDistribution(
  std::vector<const char*>& conglomeratedRigBoneNames,
  std::vector<MR::AttribDataClosestAnimDefAnimSet*>& setupDataAnimSetArray)
{
  uint32_t numAnimChannels = (uint32_t) conglomeratedRigBoneNames.size();
  uint32_t numAnimSets = (uint32_t) setupDataAnimSetArray.size();
  if (numAnimChannels > 1 && numAnimSets > 0)
  {
    //---------------------------
    // Allocate the temporary working memory
    std::vector<int32_t*> entriesArray;
    NMP::Memory::Format memReqsChanBuffer(sizeof(int32_t) * numAnimChannels, NMP_NATURAL_TYPE_ALIGNMENT);
    for (uint32_t iSet = 0; iSet < numAnimSets; ++iSet)
    {
      NMP::Memory::Resource memResChanBuffer = NMPMemoryAllocateFromFormat(memReqsChanBuffer);
      entriesArray.push_back((int32_t*) memResChanBuffer.ptr);
    }

    //---------------------------
    // Initialise the entry index arrays
    for (uint32_t iSet = 0; iSet < numAnimSets; ++iSet)
    {
      MR::AttribDataClosestAnimDefAnimSet* setupDataAnimSet = setupDataAnimSetArray[iSet];
      NMP_VERIFY(setupDataAnimSet);

      int32_t* entries = entriesArray[iSet];
      for (uint32_t i = 0; i < numAnimChannels; ++i)
        entries[i] = -1;

      for (uint32_t indx = 0; indx < setupDataAnimSet->m_numEntries; ++indx)
      {
        uint32_t animChannelIndex = setupDataAnimSet->m_animChannels[indx];
        entries[animChannelIndex] = indx;
      }
    }

    //---------------------------
    // Perform a greedy search to determine the best improvements
    for (uint32_t i = 0; i < numAnimChannels - 1; ++i)
    {
      // Check for an improvement
      uint32_t bestImpIndex = i;
      int32_t bestImpCost = 0;
      for (uint32_t j = i + 1; j < numAnimChannels; ++j)
      {
        int32_t impCost = 0;
        for (uint32_t iSet = 0; iSet < numAnimSets; ++iSet)
        {
          // Compute the cost of the exchange
          int32_t exchCost = 0;
          int32_t* entries = entriesArray[iSet];        
          if (entries[i] == -1)
          {
            // Possible improvement
            if (entries[j] != -1)
            {
              // Find next valid entry
              uint32_t k = j - 1;
              for (; k > i; --k)
              {
                if (entries[k] != -1)
                  break;
              }
              // Cost
              exchCost = (int32_t)(k - j);
            }
          }
          else
          {
            // Possible penalty
            if (entries[j] == -1)
            {
              // Find next valid entry
              uint32_t k = j - 1;
              for (; k > i; --k)
              {
                if (entries[k] != -1)
                  break;
              }
              // Cost
              exchCost = (int32_t)(j - k);
            }
          }

          // Update improvement cost
          impCost += exchCost;
        }

        // Update best improvement
        if (impCost < bestImpCost)
        {
          bestImpCost = impCost;
          bestImpIndex = j;
        }
      }

      //---------------------------
      // Perform the exchange
      if (bestImpIndex != i)
      {
        // Exchange the rig bone names
        const char* tName = conglomeratedRigBoneNames[i];
        conglomeratedRigBoneNames[i] = conglomeratedRigBoneNames[bestImpIndex];
        conglomeratedRigBoneNames[bestImpIndex] = tName;

        // Exchange the entry indices
        for (uint32_t iSet = 0; iSet < numAnimSets; ++iSet)
        {
          int32_t* entries = entriesArray[iSet];
          int32_t tEntry = entries[i];
          entries[i] = entries[bestImpIndex];
          entries[bestImpIndex] = tEntry;
        }
      }
    }

    //---------------------------
    // Remap the anim channel indices
    for (uint32_t iSet = 0; iSet < numAnimSets; ++iSet)
    {
      MR::AttribDataClosestAnimDefAnimSet* setupDataAnimSet = setupDataAnimSetArray[iSet];
      NMP_VERIFY(setupDataAnimSet);

      int32_t* entries = entriesArray[iSet];
      for (uint32_t iChan = 0; iChan < numAnimChannels; ++iChan)
      {
        int32_t entryIndex = entries[iChan];
        if (entryIndex != -1)
        {
          setupDataAnimSet->m_animChannels[entryIndex] = (uint16_t) iChan;
        }
      }
    }

    //---------------------------
    // Tidy up
    for (uint32_t iSet = 0; iSet < numAnimSets; ++iSet)
    {
      NMP::Memory::memFree(entriesArray[iSet]);
    }
  }

  //---------------------------
  // Determine the maximum unpacked anim chanel indices
  for (uint32_t animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    MR::AttribDataClosestAnimDefAnimSet* setupDataAnimSet = setupDataAnimSetArray[animSetIndex];
    NMP_VERIFY(setupDataAnimSet);

    setupDataAnimSet->m_numAnimWeights = 0;
    for (uint32_t entryIndex = 0; entryIndex < setupDataAnimSet->m_numEntries; ++entryIndex)
    {
      setupDataAnimSet->m_numAnimWeights = NMP::maximum(
        (uint32_t)setupDataAnimSet->m_animChannels[entryIndex] + 1,
        setupDataAnimSet->m_numAnimWeights);
    }
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
