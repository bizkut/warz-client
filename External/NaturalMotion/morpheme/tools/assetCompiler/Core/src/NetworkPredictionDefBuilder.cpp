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
#include "morpheme/Prediction/mrNetworkPredictionDef.h"
#include "NetworkPredictionDefBuilder.h"
#include "assetProcessor/PredictionModelBuilder.h"
#include "NMPlatform/NMHash.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Classes used to allow access to protected members from the builder.
//----------------------------------------------------------------------------------------------------------------------
class NetworkPredictionDefAccessor : public MR::NetworkPredictionDef
{
  friend class NetworkPredictionDefBuilder;
};

//----------------------------------------------------------------------------------------------------------------------
class PredictionNodeModelsAccessor : public MR::PredictionNodeModels
{
  friend class NodeModelsCompilationInfo;
};

//----------------------------------------------------------------------------------------------------------------------
class PredictionModelAnimSetGroupAccessor : public MR::PredictionModelAnimSetGroup
{
  friend class PredictionModelAnimSetGroupInfo;
};

//----------------------------------------------------------------------------------------------------------------------
// NetworkPredictionDefBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource NetworkPredictionDefBuilder::init(
  AP::AssetProcessor*         processor,
  const ME::NetworkDefExport* netDefExport,
  MR::NetworkDef*             netDef)
{
  NMP::Memory::Resource objectMemory = { NULL, NMP::Memory::Format(0, 0) };

  // Compile all individual prediction models in the network.
  for (MR::NodeID nodeID = 1; nodeID < netDefExport->getNumNodes(); ++nodeID)
  {
    buildNodePredictionModelDefs(nodeID, netDefExport, processor, netDef);
  }

  // Assemble a full network prediction def from the individually compiled prediction models
  if (m_nodeModels.size() > 0)
  {
    NMP::Memory::Format memReqs = getPredictionDefMemoryRequirements();

    NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
    objectMemory = memRes;
    ZeroMemory(memRes.ptr, memRes.format.size);

    initPredictionDef(memRes, processor);

    NMP_VERIFY_MSG(
      memRes.format.size == 0,
      "Did not use all the requested memory when compiling the network prediction def: %d bytes remain",
      memRes.format.size);
  }

  // Tidy up
  release();

  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkPredictionDefBuilder::buildNodePredictionModelDefs(
  MR::NodeID                  nodeID,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor,
  MR::NetworkDef*             netDef)
{
  const ME::NodeExport* nodeDefExport = netDefExport->getNode(nodeID);
  NMP_VERIFY(nodeDefExport);
  NodeModelsCompilationInfo* nodeModelsEntry = NULL;

  // Loop through all the analysis structures stored on this node and attempt to build prediction models against them.
  uint32_t numAnalysisNodes = nodeDefExport->getNumAnalysisNodes();
  const ME::AnalysisNodeExport* analysisNode = NULL;
  const char* analysisNodeTypeName = NULL;
  for (uint32_t i = 0; i < numAnalysisNodes; ++i)
  {
    analysisNode = nodeDefExport->getAnalysisNode(i);
    analysisNodeTypeName = analysisNode->getTypeName();
    const ME::DataBlockExport* analysisNodeDataBlock = analysisNode->getDataBlock();

    //----------------------------
    // Find if there is a builder registered against this analysis node type.
    PredictionModelBuilder* predictionModelBuilder = processor->getPredictionBuilder(analysisNodeTypeName);
    if (predictionModelBuilder)
    {
      // Crete the node models container
      if (!nodeModelsEntry)
      {
        nodeModelsEntry = addNodeModelsEntry(nodeID);
      }
      NMP_VERIFY(nodeModelsEntry);

      // This analysis node is part of a set within a connect container. This set fully describes how to build a prediction model.
      // The name of the container will be used as the unique name of this prediction model in runtime.
      // NOTE: this assumes that we continue to only ever have one prediction model described within a container.
      std::string predictModelFullPathName; // Full path name of container, but stripped of the analysis nodes name.
      std::string predictModelName;         // The name of the container only (Must be unique within the list of containers attached to this node).
      extractModelNameFromAnalysisNodeName(analysisNode, predictModelFullPathName, predictModelName);

      // Create the anim set dependency map from the export
      AnalysisAnimSetDependencyMap* animSetDependencyMap = AnalysisAnimSetDependencyMap::createFromExport(
        analysisNodeDataBlock);

      // Create the anim set group prediction models table
      AnimSetGroupPredictionModels* animSetPredictionModels = AnimSetGroupPredictionModels::create(
        animSetDependencyMap->m_numUsedAnimSets);

      // Build the prediction models corresponding to the used anim set data
      for (uint32_t i = 0; i < animSetDependencyMap->m_numUsedAnimSets; ++i)
      {
        MR::AnimSetIndex animSetIndex = (MR::AnimSetIndex)animSetDependencyMap->m_usedAnimSetIndices[i];

        animSetPredictionModels->m_usedAnimSetModels[i] = predictionModelBuilder->init(
          nodeDefExport,
          netDefExport,
          processor,
          netDef,
          analysisNode,
          animSetIndex);
      }

      // Create a anim set group container. A container of this name should not exist yet.
      nodeModelsEntry->addAnimSetGroupEntry(
        predictModelName.c_str(),
        animSetDependencyMap,
        animSetPredictionModels);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkPredictionDefBuilder::extractModelNameFromAnalysisNodeName(
  const ME::AnalysisNodeExport* analysisNode,             // e.g. "StateMachine1|VaultOffRight_WholeDistance|PredictionModelNDMesh1"
  std::string&                  predictModelFullPathName, // Becomes "StateMachine1|VaultOffRight_WholeDistance"
  std::string&                  predictModelName)         // Becomes "VaultOffRight_WholeDistance"
{
  std::string analysisNodeName = analysisNode->getName();

  // Strip off the analysis node name.
  predictModelFullPathName = analysisNodeName;
  size_t pipePos = predictModelFullPathName.find_last_of('|');
  NMP_ASSERT(pipePos != std::string::npos);
  predictModelFullPathName.erase(pipePos);

  // Strip off the path name.
  predictModelName = predictModelFullPathName;
  pipePos = predictModelName.find_last_of('|');
  if (pipePos != std::string::npos)
  {
    predictModelName.erase(0, pipePos + 1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NodeModelsCompilationInfo* NetworkPredictionDefBuilder::findNodeModelsEntry(MR::NodeID nodeID)
{
  NodeModelsCompilationInfo* nodeModels = NULL;

  for(std::list<NodeModelsCompilationInfo>::iterator list_iter = m_nodeModels.begin(); 
    list_iter != m_nodeModels.end();
    ++list_iter)
  {
    if (list_iter->m_nodeID == nodeID)
    {
      nodeModels = &(*list_iter);
      break;
    }
  }

  return nodeModels;
}

//----------------------------------------------------------------------------------------------------------------------
NodeModelsCompilationInfo* NetworkPredictionDefBuilder::addNodeModelsEntry(MR::NodeID nodeID)
{
  // An entry shouldn't already exist for this node
  NMP_VERIFY_MSG(
    !findNodeModelsEntry(nodeID),
    "A node models container already exists for node %d",
    nodeID);

  // An entry for this node does not exist, so add it.
  NodeModelsCompilationInfo newEntry;
  newEntry.m_nodeID = nodeID;
  m_nodeModels.push_back(newEntry);
  NodeModelsCompilationInfo* nodeModels = &(m_nodeModels.back());

  return nodeModels;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkPredictionDefBuilder::release()
{
  // Free any working node data.
  for(std::list<NodeModelsCompilationInfo>::iterator it = m_nodeModels.begin(); 
      it != m_nodeModels.end();
      ++it)
  {
    NodeModelsCompilationInfo* nodeModels = &(*it);
    NMP_VERIFY(nodeModels);
    nodeModels->release();
  }

  // Wipe down list itself.
  m_nodeModels.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// Memory layout:
//  NetworkPredictionDef
//   m_modelAnimSetGroups array
//     m_name string                                    |
//     m_predictionModels array                         |- Repeated for each m_modelAnimSetGroups entry.
//       Prediction models (pointed to from the array)  |
//     m_associatedNodeIDs array                        |
//
//   m_nodeModelsArray
//     m_associatedAnimSetGroups array  |- Repeated for each m_associatedAnimSetGroups entry.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkPredictionDefBuilder::getPredictionDefMemoryRequirements() const
{
  // Header
  NMP::Memory::Format result(sizeof(MR::NetworkPredictionDef), MR_ATTRIB_DATA_ALIGNMENT);

  // Node models table (one node models entry per network node)
  uint32_t numNodeModels = (uint32_t)m_nodeModels.size();
  NMP::Memory::Format memReqsNodeModelsTable = NMP::Memory::Format(sizeof(MR::PredictionNodeModels) * numNodeModels, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsNodeModelsTable;

  // The node models data (each node models entry contains a table of the model groups.
  // These groups are the anim set instances of a prediction model).
  for(std::list<NodeModelsCompilationInfo>::const_iterator it = m_nodeModels.begin();
    it != m_nodeModels.end();
    ++it)
  {
    const NodeModelsCompilationInfo* nodelModelsEntryInfo = &(*it);
    NMP::Memory::Format memReqsAnimSetGrps = nodelModelsEntryInfo->getAnimSetGroupsMemoryRequirements();
    result += memReqsAnimSetGrps;
  }

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NetworkPredictionDef* NetworkPredictionDefBuilder::initPredictionDef(
  NMP::Memory::Resource& resource,
  AP::AssetProcessor* processor) const
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(MR::NetworkPredictionDef), MR_ATTRIB_DATA_ALIGNMENT);
  NetworkPredictionDefAccessor* result = (NetworkPredictionDefAccessor*)resource.alignAndIncrement(memReqsHdr);

  uint32_t numNodeModels = (uint32_t)m_nodeModels.size();
  result->m_numNodeModels = numNodeModels;

  // Node models table (one node models entry per network node)
  NMP::Memory::Format memReqsNodeModelsTable = NMP::Memory::Format(sizeof(MR::PredictionNodeModels) * numNodeModels, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_nodeModelsArray = (MR::PredictionNodeModels*)resource.alignAndIncrement(memReqsNodeModelsTable);

  // The node models data (each node models entry contains a table of the model groups.
  // These groups are the anim set instances of a prediction model).
  uint32_t entryIndex = 0;
  for(std::list<NodeModelsCompilationInfo>::const_iterator it = m_nodeModels.begin();
    it != m_nodeModels.end();
    ++it, ++entryIndex)
  {
    const NodeModelsCompilationInfo* nodelModelsEntryInfo = &(*it);
    MR::PredictionNodeModels* nodeModelsEntry = &(result->m_nodeModelsArray[entryIndex]);
    nodelModelsEntryInfo->initAnimSetGroups(resource, nodeModelsEntry, processor);
  }

  // Make sure size is a multiple of the alignment requirement.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSetGroupPredictionModels
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimSetGroupPredictionModels::getMemoryRequirements(uint32_t numUsedAnimSets)
{
  // Header
  NMP::Memory::Format result(sizeof(AnimSetGroupPredictionModels), NMP_NATURAL_TYPE_ALIGNMENT);

  // Resource table
  if (numUsedAnimSets > 0)
  {
    NMP::Memory::Format memReqsTable(sizeof(NMP::Memory::Resource) * numUsedAnimSets, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsTable;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnimSetGroupPredictionModels* AnimSetGroupPredictionModels::init(
  NMP::Memory::Resource& resource,
  uint32_t numUsedAnimSets)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AnimSetGroupPredictionModels), NMP_NATURAL_TYPE_ALIGNMENT);
  AnimSetGroupPredictionModels* result = (AnimSetGroupPredictionModels*)resource.alignAndIncrement(memReqsHdr);

  result->m_numUsedAnimSets = numUsedAnimSets;
  result->m_usedAnimSetModels = NULL;

  if (numUsedAnimSets > 0)
  {
    NMP::Memory::Format memReqsTable(sizeof(NMP::Memory::Resource) * numUsedAnimSets, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_usedAnimSetModels = (NMP::Memory::Resource*)resource.alignAndIncrement(memReqsTable);

    for (uint32_t i = 0; i < numUsedAnimSets; ++i)
    {
      result->m_usedAnimSetModels[i].format.set(0, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_usedAnimSetModels[i].ptr = NULL;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnimSetGroupPredictionModels* AnimSetGroupPredictionModels::create(uint32_t numUsedAnimSets)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(numUsedAnimSets);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  AnimSetGroupPredictionModels* result = init(memRes, numUsedAnimSets);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "Not all of the allocated memory was used when creating the anim set group prediction models: %d bytes remain",
    memRes.format.size);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSetGroupPredictionModels::releaseAndDestroy()
{
  // Free the prediction model data
  for (uint32_t i = 0; i < m_numUsedAnimSets; ++i)
  {
    if (m_usedAnimSetModels[i].ptr)
    {
      NMP_VERIFY(m_usedAnimSetModels[i].format.size > 0);
      NMP::Memory::memFree(m_usedAnimSetModels[i].ptr);
    }
  }

  // Free the table structure
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
// PredictionModelAnimSetGroupInfo
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PredictionModelAnimSetGroupInfo::getMemoryRequirements(
  const char* modelName,
  AnalysisAnimSetDependencyMap* NMP_UNUSED(animSetDependencyMap),
  AnimSetGroupPredictionModels* NMP_UNUSED(animSetPredictionModels))
{
  NMP_VERIFY(modelName);

  // Header
  NMP::Memory::Format result(sizeof(PredictionModelAnimSetGroupInfo), NMP_NATURAL_TYPE_ALIGNMENT);

  // Model name
  size_t nameLength = NMP_STRLEN(modelName);
  NMP::Memory::Format memReqsName(sizeof(char) * (nameLength + 1), NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsName;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PredictionModelAnimSetGroupInfo* PredictionModelAnimSetGroupInfo::init(
  NMP::Memory::Resource& resource,
  const char* modelName,
  AnalysisAnimSetDependencyMap* animSetDependencyMap,
  AnimSetGroupPredictionModels* animSetPredictionModels)
{
  NMP_VERIFY(modelName);
  NMP_VERIFY(animSetDependencyMap);
  NMP_VERIFY(animSetPredictionModels);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(PredictionModelAnimSetGroupInfo), NMP_NATURAL_TYPE_ALIGNMENT);
  PredictionModelAnimSetGroupInfo* result = (PredictionModelAnimSetGroupInfo*)resource.alignAndIncrement(memReqsHdr);

  // Model name
  size_t nameLength = NMP_STRLEN(modelName);
  NMP::Memory::Format memReqsName(sizeof(char) * (nameLength + 1), NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_modelName = (char*)resource.alignAndIncrement(memReqsName);
  NMP_STRNCPY_S(result->m_modelName, nameLength + 1, modelName);
  NMP_VERIFY(result->m_modelName[nameLength] == 0);

  // Model name hash
  result->m_hash = NMP::hashStringCRC32(result->m_modelName);

  // External data
  result->m_animSetDependencyMap = animSetDependencyMap;
  result->m_animSetPredictionModels = animSetPredictionModels;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PredictionModelAnimSetGroupInfo* PredictionModelAnimSetGroupInfo::create(
  const char* modelName,
  AnalysisAnimSetDependencyMap* animSetDependencyMap,
  AnimSetGroupPredictionModels* animSetPredictionModels)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(
    modelName,
    animSetDependencyMap,
    animSetPredictionModels);

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  PredictionModelAnimSetGroupInfo* result = init(
    memRes,
    modelName,
    animSetDependencyMap,
    animSetPredictionModels);

  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "Not all of the allocated memory was used when creating the anim set group container: %d bytes remain",
    memRes.format.size);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void PredictionModelAnimSetGroupInfo::releaseAndDestroy()
{
  if (m_animSetPredictionModels)
  {
    m_animSetPredictionModels->releaseAndDestroy();
  }

  if (m_animSetDependencyMap)
  {
    m_animSetDependencyMap->releaseAndDestroy();
  }

  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PredictionModelAnimSetGroupInfo::getAnimSetPredictionModelsMemoryRequirements() const
{
  // Model name
  size_t nameLength = NMP_STRLEN(m_modelName);
  NMP::Memory::Format result(sizeof(char) * (nameLength + 1), NMP_NATURAL_TYPE_ALIGNMENT);

  // Array of unique prediction models with used anim set data
  NMP::Memory::Format memReqsModelsTable(
    sizeof(MR::PredictionModelDef*) * m_animSetDependencyMap->m_numUsedAnimSets,
    NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsModelsTable;

  // Array of pointers to prediction models for each anim set entry
  NMP::Memory::Format memReqsModelsAnimSetMap(
    sizeof(MR::PredictionModelDef*) * m_animSetDependencyMap->m_numAnimSets,
    NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsModelsAnimSetMap;

  // Prediction models themselves
  for (uint32_t i = 0; i < m_animSetDependencyMap->m_numUsedAnimSets; ++i)
  {
    result += m_animSetPredictionModels->m_usedAnimSetModels[i].format;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void PredictionModelAnimSetGroupInfo::initAnimSetPredictionModels(
  NMP::Memory::Resource& resource,
  MR::PredictionModelAnimSetGroup* animSetGroup,
  AP::AssetProcessor* processor) const
{
  NMP_VERIFY(animSetGroup);
  NMP_VERIFY(processor);
  PredictionModelAnimSetGroupAccessor* target = (PredictionModelAnimSetGroupAccessor*)animSetGroup;

  // Model name
  size_t nameLength = NMP_STRLEN(m_modelName);
  NMP::Memory::Format memReqsName(sizeof(char) * (nameLength + 1), NMP_NATURAL_TYPE_ALIGNMENT);
  target->m_name = (char*)resource.alignAndIncrement(memReqsName);
  NMP_STRNCPY_S(target->m_name, nameLength + 1, m_modelName);
  NMP_VERIFY(target->m_name[nameLength] == 0);

  // Model name hash
  target->m_hash = m_hash;

  // Anim set entries
  target->m_numAnimSets = (MR::AnimSetIndex)m_animSetDependencyMap->m_numAnimSets;
  target->m_numUsedAnimSets = (MR::AnimSetIndex)m_animSetDependencyMap->m_numUsedAnimSets;

  // Array of unique prediction models with used anim set data
  NMP::Memory::Format memReqsModelsTable(
    sizeof(MR::PredictionModelDef*) * m_animSetDependencyMap->m_numUsedAnimSets,
    NMP_NATURAL_TYPE_ALIGNMENT);
  target->m_predictionModels = (MR::PredictionModelDef**)resource.alignAndIncrement(memReqsModelsTable);

  // Array of pointers to prediction models for each anim set entry
  NMP::Memory::Format memReqsModelsAnimSetMap(
    sizeof(MR::PredictionModelDef*) * m_animSetDependencyMap->m_numAnimSets,
    NMP_NATURAL_TYPE_ALIGNMENT);
  target->m_animSetPredictionModelMap = (MR::PredictionModelDef**)resource.alignAndIncrement(memReqsModelsAnimSetMap);

  // Prediction models themselves
  for (uint32_t i = 0; i < m_animSetDependencyMap->m_numUsedAnimSets; ++i)
  {
    // Get the memory for the prediction model
    MR::PredictionModelDef* predictionModel = (MR::PredictionModelDef*)resource.alignAndIncrement(
      m_animSetPredictionModels->m_usedAnimSetModels[i].format);
    target->m_predictionModels[i] = predictionModel;

    // Relocate the prediction model into the allocated memory block
    NMP::Memory::memcpy(
      predictionModel,
      m_animSetPredictionModels->m_usedAnimSetModels[i].ptr,
      m_animSetPredictionModels->m_usedAnimSetModels[i].format.size);

    MR::PredictionType type = target->m_predictionModels[i]->getType();
    PredictionModelBuilder* predictionModelBuilder = processor->getPredictionBuilder(type);
    NMP_VERIFY(predictionModelBuilder);
    predictionModelBuilder->relocate(predictionModel);

    // Set the prediction models to share the same anim group name.
    predictionModel->setName(target->m_name);
  }

  // Fix up the anim set prediction model map
  for (uint32_t i = 0; i < m_animSetDependencyMap->m_numAnimSets; ++i)
  {
    uint32_t idx = m_animSetDependencyMap->m_animSetEntryMap[i];
    NMP_VERIFY(idx < m_animSetDependencyMap->m_numUsedAnimSets);
    target->m_animSetPredictionModelMap[i] = target->m_predictionModels[idx];
  }
}

//----------------------------------------------------------------------------------------------------------------------
// NodeModelsCompilationInfo
//----------------------------------------------------------------------------------------------------------------------
PredictionModelAnimSetGroupInfo* NodeModelsCompilationInfo::findAnimSetGroupEntry(const char* modelName)
{
  NMP_VERIFY(modelName);

  // Find the model entry in the nodes list.
  int32_t nameHash = NMP::hashStringCRC32(modelName);

  for(std::list<PredictionModelAnimSetGroupInfo*>::iterator list_iter = m_modelAnimSetGroups.begin(); 
    list_iter != m_modelAnimSetGroups.end();
    ++list_iter)
  {
    PredictionModelAnimSetGroupInfo* animSetGroup = *list_iter;
    NMP_VERIFY(animSetGroup);

    if (NMP_STRCMP(animSetGroup->m_modelName, modelName) == 0)
    {
      NMP_VERIFY(animSetGroup->m_hash == nameHash); // Name string matches, but so must the hash ID.
      return animSetGroup;
    }
    else
    {
      NMP_VERIFY(animSetGroup->m_hash != nameHash); // We should not find a hash ID match without a string name match.
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
PredictionModelAnimSetGroupInfo* NodeModelsCompilationInfo::addAnimSetGroupEntry(
  const char* modelName,
  AnalysisAnimSetDependencyMap* animSetDependencyMap,
  AnimSetGroupPredictionModels* animSetPredictionModels)
{
  NMP_VERIFY(modelName);

  // Make sure this model name does not already exist on this node.
  NMP_VERIFY_MSG(
    !findAnimSetGroupEntry(modelName),
    "Trying to add model name: %s, that already exists on node ID: %d. Check for duplicates.",
    modelName,
    m_nodeID);

  // Create the anim set group
  PredictionModelAnimSetGroupInfo* modelAnimSetGroup = PredictionModelAnimSetGroupInfo::create(
    modelName,
    animSetDependencyMap,
    animSetPredictionModels);

  m_modelAnimSetGroups.push_back(modelAnimSetGroup);
  return modelAnimSetGroup;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeModelsCompilationInfo::release()
{
  // Free any working compiled prediction model memory.
  for(std::list<PredictionModelAnimSetGroupInfo*>::iterator it = m_modelAnimSetGroups.begin(); 
    it != m_modelAnimSetGroups.end();
    ++it)
  {
    PredictionModelAnimSetGroupInfo* animSetGroup = *it;
    NMP_VERIFY(animSetGroup);
    animSetGroup->releaseAndDestroy();
  }

  // Wipe down list itself.
  m_modelAnimSetGroups.clear();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeModelsCompilationInfo::getAnimSetGroupsMemoryRequirements() const
{
  // Anim set groups table
  uint32_t numAnimSetGroups = (uint32_t)m_modelAnimSetGroups.size();
  NMP_VERIFY(numAnimSetGroups > 0);
  NMP::Memory::Format result(
    sizeof(MR::PredictionModelAnimSetGroup) * numAnimSetGroups,
    NMP_NATURAL_TYPE_ALIGNMENT);

  // Prediction models in each anim set group entry
  for(std::list<PredictionModelAnimSetGroupInfo*>::const_iterator it = m_modelAnimSetGroups.begin(); 
    it != m_modelAnimSetGroups.end();
    ++it)
  {
    const PredictionModelAnimSetGroupInfo* animSetGroupInfo = *it;
    NMP_VERIFY(animSetGroupInfo);

    // Build the anim set prediction models
    NMP::Memory::Format memReqsModels = animSetGroupInfo->getAnimSetPredictionModelsMemoryRequirements();
    result += memReqsModels;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeModelsCompilationInfo::initAnimSetGroups(
  NMP::Memory::Resource& resource,
  MR::PredictionNodeModels* nodeModelsEntry,
  AP::AssetProcessor* processor) const
{
  NMP_VERIFY(nodeModelsEntry);
  PredictionNodeModelsAccessor* target = (PredictionNodeModelsAccessor*)nodeModelsEntry;

  // Fill in the node models entry
  uint32_t numAnimSetGroups = (uint32_t)m_modelAnimSetGroups.size();
  NMP_VERIFY(numAnimSetGroups > 0);
  target->m_nodeID = m_nodeID;
  target->m_numModelAnimSetGroups = numAnimSetGroups;

  // Anim set groups table
  NMP::Memory::Format memReqsAnimSetGroupsTable(
    sizeof(MR::PredictionModelAnimSetGroup) * numAnimSetGroups,
    NMP_NATURAL_TYPE_ALIGNMENT);
  target->m_modelAnimSetGroups = (MR::PredictionModelAnimSetGroup*)resource.alignAndIncrement(memReqsAnimSetGroupsTable);

  // Prediction models in each anim set group entry
  uint32_t entryIndex = 0;
  for(std::list<PredictionModelAnimSetGroupInfo*>::const_iterator it = m_modelAnimSetGroups.begin(); 
    it != m_modelAnimSetGroups.end();
    ++it, ++entryIndex)
  {
    const PredictionModelAnimSetGroupInfo* animSetGroupInfo = *it;
    NMP_VERIFY(animSetGroupInfo);
    MR::PredictionModelAnimSetGroup* animSetGroup = &target->m_modelAnimSetGroups[entryIndex];

    // Build the anim set prediction models
    animSetGroupInfo->initAnimSetPredictionModels(
      resource,
      animSetGroup,
      processor);
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
