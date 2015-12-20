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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMFastHeapAllocator.h"
#include "NMPlatform/NMFile.h"
#include "NMPlatform/NMHash.h"
#include "NMPlatform/NMString.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrManager.h"
#include "morpheme/Nodes/mrNodes.h"
#include "assetProcessor/AssetProcessor.h"
#include "XMD/AnimCycle.h"
#include "XMD/Model.h"
#include "XMD/AnimationTake.h"

#include <rpc.h> // Required for GUID generation
#pragma comment(lib, "Rpcrt4.lib")

//----------------------------------------------------------------------------------------------------------------------
// Get rid of the warning about deprecated strcmpi name.
#pragma warning(disable: 4996)

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
AssetCompilerPlugin::AssetCompilerPlugin(
  AssetProcessor* assetProc,
  MR::Manager* manager) :
  m_assetProcessor(assetProc)
{
  NMP_VERIFY(assetProc);
  assetProc->registerPlugin(this);

  // Set the instance of the manager. This allow to bring the manager across the dll boundaries.
  MR::Manager::setInstance(manager);
}

//----------------------------------------------------------------------------------------------------------------------
AssetCompilerPlugin::~AssetCompilerPlugin()
{
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_DEBUG
void printDataBlock(const DataBlockExport* dataBlock)
{
  uint32_t bufferLength = dataBlock->getDataBlockSize();
  void* buffer = NMPMemoryAlloc(bufferLength);
  NMP_VERIFY(buffer);
  dataBlock->writeDataBlock(buffer);

  printf("  DATABLOCK : ");
  for (uint32_t i = 0 ; i < bufferLength ; ++i)
  {
    printf("%02x ", ((unsigned char*)buffer)[i]);
  }
  printf("\n");
  NMP::Memory::memFree(buffer);
}
#endif // NM_DEBUG

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::registerPlugin(AssetCompilerPlugin* plugin)
{
  m_plugins.push_back(plugin);
}

//----------------------------------------------------------------------------------------------------------------------
// AssetProcessor
//----------------------------------------------------------------------------------------------------------------------
AssetProcessor::AssetProcessor(
  GenUIDFunc                genUIDFunc,
  HandleProcessedAssetFunc  handleProcessedAssetFunc,
  ProcessAnimFunc           processAnimFunc,
  FinalizeProcessAnimsFunc  finalizeAnimsFunc,
  InitNetworkInstanceFunc   initNetworkInstanceFunc,
  UpdateNetworkInstanceFunc updateNetworkInstanceFunc,
  ReleaseNetworkInstanceFunc releaseNetworkInstanceFunc,
  ChangeNetworkAnimSetFunc  changeNetworkAnimSetFunc,
  DeMacroisePathFunc        deMacroisePathFunc,
  ExportFactory*            NMP_UNUSED(exportFactory),
  const char*               rootDirectory,
  NMP::BasicLogger*         messageLogger,
  NMP::BasicLogger*         errorLogger) :
  m_genUIDFunc(genUIDFunc),
  m_handleProcessedAssetFunc(handleProcessedAssetFunc),
  m_processAnimFunc(processAnimFunc),
  m_finalizeAnimsFunc(finalizeAnimsFunc),
  m_initNetworkInstanceFunc(initNetworkInstanceFunc),
  m_updateNetworkInstanceFunc(updateNetworkInstanceFunc),
  m_releaseNetworkInstanceFunc(releaseNetworkInstanceFunc),
  m_changeNetworkAnimSetFunc(changeNetworkAnimSetFunc),
  m_deMacroisePathFunc(deMacroisePathFunc),
  m_networkDefBuilder(NULL),
  m_numRegisteredNodeBuilders(0),
  m_numRegisteredTransitConditionDefBuilders(0),
  m_numRegisteredTransitDeadBlendDefBuilders(0),
  m_numRegisteredMessageBuilders(0),
  m_numRegisteredPredictionBuilders(0),
  m_currentAssetID(1),
  m_network(NULL),
  m_precomputeDataExport(NULL),
  m_processingFailFlag(false)
{
  m_analysisProcessors.clear();
  m_analysisConditionBuilders.clear();
  m_analysisPropertyBuilders.clear();

  m_processedAssets.clear();

  if (rootDirectory)
    m_rootDirectory = rootDirectory;

  if (messageLogger)
    m_messageLogger = messageLogger;
  else
    m_messageLogger = &(NMP::NullLogger::sm_nullLogger);

  if (errorLogger)
    m_errorLogger = errorLogger;
  else
    m_errorLogger = &(NMP::NullLogger::sm_nullLogger);

  m_exceptionOccured = false;
}

//----------------------------------------------------------------------------------------------------------------------
AssetProcessor::~AssetProcessor()
{
  freeProcessedAssets();

  if (m_network && m_releaseNetworkInstanceFunc)
  {
    m_releaseNetworkInstanceFunc(this);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::makeGUID(std::string& guid)
{
#ifdef WIN32
  UUID uuid;
  RPC_STATUS status = ::UuidCreate(&uuid);
  if (status == RPC_S_OK)
  {
    unsigned char* str = 0;
    if (::UuidToStringA(&uuid, &str) == RPC_S_OK)
    {
      guid = (char*)str;
      ::RpcStringFreeA(&str);
      return true;
    }
  }
  return false;
#else
  #warning MakeGUID() not implemented on this platform.
#endif
}

//----------------------------------------------------------------------------------------------------------------------
ProcessedAsset* AssetProcessor::addAsset(
  NMP::Memory::Resource    assetMemory,
  MR::Manager::AssetType   assetType,
  const char*              name,
  const char*              guid,
  int32_t*                 runtimeID)
{
  // Make sure we have a valid guid, otherwise default
  static const uint32_t defaultGuidLength = 36;
  NMP_VERIFY_MSG(NMP_STRLEN(guid) == defaultGuidLength, "Invalid length GUID detected!");

  // Obtain runtimeID from app callback
  int32_t id;
  if (runtimeID)
  {
    id = *runtimeID;
  }
  else
  {
    id = m_genUIDFunc(name, guid);
  }

  // Validate that this ID does not already exist in the vector of processed assets.
  if (findProcessedAsset(id))
  {
    m_errorLogger->output(
      "Attempting to add an asset with an ID that already exists in the processed assets array. Asset name %s, Asset GUID %s\n",
      name,
      guid);
    return NULL;  // Asset with this ID already exists so we cant add this asset.
  }

  // Create ProcessedAsset and add it to the table;
  ProcessedAsset* processedAsset = new ProcessedAsset();
  processedAsset->assetType = assetType;
  processedAsset->uniqueRuntimeID = id;
  processedAsset->assetMemory = assetMemory;
  
  // Make a copy of the guid.
  NMP_ASSERT(strlen(guid) > 0);
  processedAsset->guid = (char*) NMPMemoryAlloc(strlen(guid) + 1);
  NMP_STRNCPY_S(((char*) processedAsset->guid), strlen(guid) + 1, guid);

  processedAsset->id = m_dependencyStack.top();
  m_dependencyStack.pop();
  if (!m_dependencyStack.empty())
  {
    processedAsset->parendIDs.insert(m_dependencyStack.top());
  }

  m_processedAssets.push_back(processedAsset);

  //---------------------------------
  // Insert in to fast search tables.

  NMP_VERIFY_MSG(!findProcessedAsset(processedAsset->guid, processedAsset->assetType), "Asset collision: guid already occupied: %s!", processedAsset->guid);
  uint32_t guidHash = NMP::hashStringCRC32(processedAsset->guid);
  m_procAssetsByGUID.insert(std::make_pair(guidHash, processedAsset));
  NMP_VERIFY_MSG(m_procAssetsByRuntimeID.find(processedAsset->uniqueRuntimeID) == m_procAssetsByRuntimeID.end(), "Asset collision: runtime Id already occupied: %u!", processedAsset->uniqueRuntimeID);
  m_procAssetsByRuntimeID.insert(std::make_pair(processedAsset->uniqueRuntimeID, processedAsset));
  NMP_VERIFY_MSG(m_procAssetsByMemory.find(processedAsset->assetMemory.ptr) == m_procAssetsByMemory.end(), "Asset collision: memory already occupied: %x!", processedAsset->assetMemory.ptr);
  m_procAssetsByMemory.insert(std::make_pair(processedAsset->assetMemory.ptr, processedAsset));

  //-----------------------------------------------
  // Register the new binary asset with the runtime.
  if (!MR::Manager::getInstance().objectIsRegistered(processedAsset->uniqueRuntimeID))
  {
    if (!MR::Manager::getInstance().registerObject(
          processedAsset->assetMemory.ptr,   // Compiled asset structure.
          processedAsset->assetType,         //
          processedAsset->uniqueRuntimeID))  // Unique runtime id.
    {
      // Registration failed for some reason
      NMP_DEBUG_MSG("   !! Registration Failed !!.\n");
    }
    else
    {
      // Increment the reference counter for this object.
      MR::Manager::incObjectRefCount(processedAsset->uniqueRuntimeID);
    }
  }

  return processedAsset;
}

//----------------------------------------------------------------------------------------------------------------------
const ProcessedAsset* AssetProcessor::findProcessedAsset(MR::RuntimeAssetID assetID)const
{
  quickAssetsRunID::const_iterator itAsset = m_procAssetsByRuntimeID.find(assetID);
  if (itAsset != m_procAssetsByRuntimeID.end())
    return itAsset->second;

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const ProcessedAsset* AssetProcessor::findProcessedAsset(
  ME::GUID               guid,    
  MR::Manager::AssetType assetTypeToFind) const
{
  uint32_t guidHash = NMP::hashStringCRC32(guid);

  if (assetTypeToFind == MR::Manager::kAsset_Invalid)
  {
    quickAssetsGUID::const_iterator itAsset = m_procAssetsByGUID.find(guidHash);
    if (itAsset != m_procAssetsByGUID.end())
    {
      // Make sure that there isn't more than 1 asset entry with the same GUID.
      //  If this is the case the caller should be specifying an AssetType to refine the search.
      NMP_ASSERT(m_procAssetsByGUID.count(guidHash) == 1);
      return itAsset->second;
    }
  }
  else
  {
    quickAssetsGUID::const_iterator lb = m_procAssetsByGUID.lower_bound(guidHash);
    quickAssetsGUID::const_iterator ub = m_procAssetsByGUID.upper_bound(guidHash);

    quickAssetsGUID::const_iterator itAsset;
    for (itAsset = lb; itAsset != ub; itAsset++)
    {
      if ((*itAsset).second->assetType == assetTypeToFind)
        return (*itAsset).second;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const ProcessedAsset* AssetProcessor::findProcessedAsset(const void* memory) const
{
  quickAssetsMemory::const_iterator itAsset = m_procAssetsByMemory.find(memory);
  if (itAsset != m_procAssetsByMemory.end())
    return itAsset->second;
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
// Recursively called asset processing.
const ProcessedAsset* AssetProcessor::doProcessAsset(
  ME::AssetExport* const sourceAsset,
  MR::Manager::AssetType assetTypeToGenerate)
{
  //------------------------------------------
  // Has this asset already been processed (Identified by a pointer to the source asset structure).
  const ProcessedAsset* processedAsset = findProcessedAsset(sourceAsset->getGUID());
  if (processedAsset != 0)
  {
    // add additional dependency if needed
    uint32_t lastNewAsset = getLastNewAssetStackValue();
    if (lastNewAsset)
      processedAsset->parendIDs.insert(lastNewAsset);
    return processedAsset;
  }

  // Try to process the asset on the registered plugins.
  for (uint32_t i = 0; i < m_plugins.size(); ++i)
  {
    AssetCompilerPlugin* plugin = m_plugins[i];
    processedAsset = plugin->processAsset(sourceAsset, assetTypeToGenerate);
    if (processedAsset)
    {
      break;
    }
  }

  return processedAsset;
}

//----------------------------------------------------------------------------------------------------------------------
// Check to see if a binary-equivalent version of a mapping already exists in the processed asset list
ProcessedAsset* AssetProcessor::findMatchingMapping(const MR::RigToAnimMap* mapping) const
{
  for (uint32_t i = 0; i < (int32_t) m_processedAssets.size(); ++i)
  {
    ProcessedAsset* asset = m_processedAssets[i];
    if (asset->assetType == MR::Manager::kAsset_RigToAnimMap)
    {
      if (mapping->isEqual((MR::RigToAnimMap*)(asset->assetMemory.ptr)))
      {
        // We found a matching RigToAnimMap
        return asset;
      }
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AssetProcessor::getNumProcessedAssetsOfType(MR::Manager::AssetType assetType)
{
  uint32_t result = 0;
  for (uint32_t i = 0; i < (int32_t) m_processedAssets.size(); ++i)
  {
    ProcessedAsset* asset = m_processedAssets[i];
    if (asset->assetType == assetType)
    {
      result++;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::getProcessedAssetsOfType(MR::Manager::AssetType assetType, ProcessedAsset** processedAssets)
{
  NMP_ASSERT(processedAssets);
  uint32_t indx = 0;
  for (uint32_t i = 0; i < (int32_t) m_processedAssets.size(); ++i)
  {
    ProcessedAsset* asset = m_processedAssets[i];
    if (asset->assetType == assetType)
    {
      processedAssets[indx] = asset;
      indx++;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* AssetProcessor::getRootDirectory() const
{
  return m_rootDirectory.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::setRootDirectory(const char* rootDirectory)
{
  m_rootDirectory = rootDirectory;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::dislocateAsset(ProcessedAsset* asset)
{
  for (uint32_t i = 0; i < m_plugins.size(); ++i)
  {
    if (m_plugins[i]->dislocateAsset(asset))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::processAsset(ME::AssetExport* sourceAsset)
{
  doProcessAsset(sourceAsset);
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::decrementProcessedAssetRefCounts()
{
  for (int32_t j = (int32_t)m_processedAssets.size() - 1; j >= 0 ; --j)
  {
    ProcessedAsset* asset = m_processedAssets[j];
    MR::Manager::decObjectRefCount(asset->uniqueRuntimeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::serializeProcessedAssets()
{
  bool success = true;

  //-----------------------------------------------
  // Dislocate all objects in preparation for writing to disc
  for (int32_t j = (int32_t)m_processedAssets.size() - 1; j >= 0 ; --j)
  {
    ProcessedAsset* asset = m_processedAssets[j];
    MR::Manager::decObjectRefCount(asset->uniqueRuntimeID);

    if (!dislocateAsset(asset))
    {
      // Dislocation failed for some reason
      NMP_DEBUG_MSG("   !! Dislocation Failed !!.\n");
      success = false;
    }
  }

  //-----------------------------------------------
  if (m_handleProcessedAssetFunc)
  {
    // Callback to app to handle the serialized block
    for (int32_t j = 0; j < (int32_t) m_processedAssets.size(); ++j)
    {
      ProcessedAsset* asset = m_processedAssets[j];
      m_handleProcessedAssetFunc(
        asset->uniqueRuntimeID,
        asset->guid,
        asset->assetMemory,
        asset->assetType);
    }
  }

  return success;
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::processAssets(std::vector<ME::AssetExport*>& sourceAssets)
{
  for (uint32_t assetId = 0; assetId < sourceAssets.size(); ++assetId)
  {
    processAsset(sourceAssets[assetId]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::registerNetworkDefBuilder(NetworkDefBuilderBase* networkDefBuilder)
{
  NMP_VERIFY(networkDefBuilder);
  NMP_VERIFY_MSG(
    m_networkDefBuilder == NULL,
    "Attempting to register a network builder which already has a builder registerd against it!\n");

  m_networkDefBuilder = networkDefBuilder;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDefBuilderBase* AssetProcessor::getNetworkDefBuilder()
{
  return m_networkDefBuilder;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::registerNodeBuilder(MR::NodeType nodeType, NodeDefBuilder* nodeBuilder)
{
  NMP_VERIFY(nodeBuilder);
  NMP_VERIFY_MSG(
    getNodeBuilder(nodeType) == NULL,
    "Attempting to register a builder for node type %i, which already has a builder registered against it!\n",
    nodeType);

  m_nodeBuilders[m_numRegisteredNodeBuilders].typeID = nodeType;
  m_nodeBuilders[m_numRegisteredNodeBuilders].nodeBuilder = nodeBuilder;
  m_numRegisteredNodeBuilders++;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::overrideNodeBuilder(MR::NodeType nodeType, NodeDefBuilder* nodeBuilder)
{
  // Try to find an existing builder
  for (uint32_t i = 0; i < m_numRegisteredNodeBuilders; i++)
  {
    if (m_nodeBuilders[i].typeID == nodeType)
    {
      // We found one, so override it with the specified builder
      m_nodeBuilders[i].nodeBuilder = nodeBuilder;
      return true;
    }
  }

  // If we get here, the typeID wasn't already used.  Simply register it.
  return registerNodeBuilder(nodeType, nodeBuilder);
}

//----------------------------------------------------------------------------------------------------------------------
NodeDefBuilder* AssetProcessor::getNodeBuilder(MR::NodeType nodeType)
{
  NodeDefBuilder* result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredNodeBuilders; i++)
  {
    if (m_nodeBuilders[i].typeID == nodeType)
    {
      result = m_nodeBuilders[i].nodeBuilder;
      break;
    }

  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::registerTransitConditionDefBuilder(
  MR::TransitConditType       transitConditType,
  TransitConditionDefBuilder* conditionBuilder)
{
  m_transitConditionDefs[m_numRegisteredTransitConditionDefBuilders].typeID = transitConditType;
  m_transitConditionDefs[m_numRegisteredTransitConditionDefBuilders].transitConditionBuilder = conditionBuilder;
  m_numRegisteredTransitConditionDefBuilders++;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitConditionDefBuilder* AssetProcessor::getTransitConditionDefBuilder(MR::TransitConditType transitConditType)
{
  TransitConditionDefBuilder* result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredTransitConditionDefBuilders; ++i)
  {
    if (m_transitConditionDefs[i].typeID == transitConditType)
    {
      result = m_transitConditionDefs[i].transitConditionBuilder;
      break;
    }

  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::registerTransitDeadBlendDefBuilder(
  MR::TransitDeadBlendType    transitDeadBlendType,
  TransitDeadBlendDefBuilder* deadBlendBuilder)
{
  m_transitDeadBlendDefs[m_numRegisteredTransitDeadBlendDefBuilders].typeID = transitDeadBlendType;
  m_transitDeadBlendDefs[m_numRegisteredTransitDeadBlendDefBuilders].transitDeadBlendBuilder = deadBlendBuilder;
  m_numRegisteredTransitDeadBlendDefBuilders++;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitDeadBlendDefBuilder* AssetProcessor::getTransitDeadBlendDefBuilder(MR::TransitDeadBlendType transitDeadBlendType)
{
  TransitDeadBlendDefBuilder* result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredTransitDeadBlendDefBuilders; ++i)
  {
    if (m_transitDeadBlendDefs[i].typeID == transitDeadBlendType)
    {
      result = m_transitDeadBlendDefs[i].transitDeadBlendBuilder;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::registerMessageBuilder(MR::MessageType messageType, MessageBuilder* messageBuilder)
{
  m_messageBuilders[m_numRegisteredMessageBuilders].typeID = messageType;
  m_messageBuilders[m_numRegisteredMessageBuilders].messageBuilder = messageBuilder;
  ++m_numRegisteredMessageBuilders;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MessageBuilder* AssetProcessor::getMessageBuilder(MR::MessageType messageType) const
{
  MessageBuilder* builder = NULL;

  for (uint32_t i = 0; i < m_numRegisteredMessageBuilders; ++i)
  {
    if (m_messageBuilders[i].typeID == messageType)
    {
      builder = m_messageBuilders[i].messageBuilder;
      break;
    }
  }

  return builder;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::registerAnalysisProcessor(const char* taskType, AnalysisProcessor* statisticAnalyser)
{
  m_analysisProcessors[taskType] = statisticAnalyser;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::unregisterAnalysisProcessor(const char* taskType)
{
  m_analysisProcessors[taskType] = NULL;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisProcessor* AssetProcessor::getAnalysisProcessorForType(const char* taskType) const
{
  AnalysisProcessors::const_iterator analyserIterator = m_analysisProcessors.find(taskType);

  if(analyserIterator != m_analysisProcessors.end())
  {
    return analyserIterator->second;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::registerAnalysisConditionBuilder(const char* conditionType, AnalysisConditionBuilder* condition)
{
  m_analysisConditionBuilders[conditionType] = condition;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::unregisterAnalysisConditionBuilder(const char* conditionType)
{
  m_analysisConditionBuilders[conditionType] = NULL;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisConditionBuilder* AssetProcessor::getAnalysisConditionBuilderForType(const char* conditionType) const
{
  AnalysisConditionBuilders::const_iterator it = m_analysisConditionBuilders.find(conditionType);

  if(it != m_analysisConditionBuilders.end())
  {
    return it->second;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::registerAnalysisPropertyBuilder(const char* propertyType, AnalysisPropertyBuilder* property)
{
  m_analysisPropertyBuilders[propertyType] = property;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::unregisterAnalysisPropertyBuilder(const char* propertyType)
{
  m_analysisPropertyBuilders[propertyType] = NULL;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisPropertyBuilder* AssetProcessor::getAnalysisPropertyBuilderForType(const char* propertyType) const
{
  AnalysisPropertyBuilders::const_iterator it = m_analysisPropertyBuilders.find(propertyType);

  if(it != m_analysisPropertyBuilders.end())
  {
    return it->second;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::registerPredictionBuilder(
  MR::PredictionType         predictionType,
  const char*                typeName,
  PredictionModelBuilder*    predictionBuilder)
{
  NMP_VERIFY(predictionBuilder && typeName);
  NMP_VERIFY_MSG(
    !getPredictionBuilder(predictionType) && !getPredictionBuilder(typeName),
    "Attempting to register a builder for prediction model type: %i name: %s, which already has a builder registered against it!\n",
    predictionType,
    typeName);
  NMP_VERIFY(m_numRegisteredPredictionBuilders < MR::MAX_NUM_PREDICTION_MODEL_TYPES);
  PredictionRegistryEntry* predictionRegEntry = &(m_predictionBuilders[m_numRegisteredPredictionBuilders]);
  predictionRegEntry->typeID = predictionType;
  predictionRegEntry->predictionBuilder = predictionBuilder;
  predictionRegEntry->typeName = typeName;
  m_numRegisteredPredictionBuilders++;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
PredictionModelBuilder* AssetProcessor::getPredictionBuilder(MR::PredictionType predictionType)
{
  PredictionModelBuilder* result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredPredictionBuilders; i++)
  {
    if (m_predictionBuilders[i].typeID == predictionType)
    {
      result = m_predictionBuilders[i].predictionBuilder;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PredictionModelBuilder* AssetProcessor::getPredictionBuilder(const char* typeName)
{
  PredictionModelBuilder* result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredPredictionBuilders; i++)
  {
    if (NMP_STRCMP(m_predictionBuilders[i].typeName, typeName) == 0)
    {
      result = m_predictionBuilders[i].predictionBuilder;
      break;
    }

  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::freeProcessedAssets()
{
  // Free all the processed assets
  for (int32_t i = 0; i < (int32_t) m_processedAssets.size(); ++i)
  {
    ProcessedAsset* asset = m_processedAssets[i];

    // We check if the manager exists here so that the AC will not crash on exit when handling exceptions.
    if (&MR::Manager::getInstance())
    {
      MR::Manager::getInstance().unregisterObject(asset->uniqueRuntimeID);
    }
    NMP::Memory::memFree(asset->assetMemory.ptr);
    NMP::Memory::memFree((void *)asset->guid);
  }
  m_processedAssets.clear();
  m_procAssetsByGUID.clear();
  m_procAssetsByRuntimeID.clear();
  m_procAssetsByMemory.clear();
}

//----------------------------------------------------------------------------------------------------------------------
AssetCompilerPlugin::AnimationMetadata* AssetProcessor::prepareAnimationMetaData(const std::vector<ME::AssetExport*>& networks)
{
  AssetCompilerPlugin::AnimationMetadata* metaData = NULL;

  // Try to process the asset on the registered plugins.
  for (uint32_t i = 0; i < m_plugins.size(); ++i)
  {
    AssetCompilerPlugin* plugin = m_plugins[i];
    if (plugin->prepareAnimationMetaData(networks, &metaData))
    {
      break;
    }
  }

  return metaData;
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::beginNewAssetProcessing()
{
  m_dependencyStack.push(m_currentAssetID++);
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::cancelNewAssetProcessing()
{
  m_dependencyStack.pop();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AssetProcessor::getLastNewAssetStackValue()
{
  return m_dependencyStack.top();
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::outputNodeError(MR::NodeType nodeType, unsigned int nodeID, const char* errorMessage)
{
  m_messageLogger->output("[NodeError %i %i] %s\n", nodeType, nodeID, errorMessage);
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::outputConditionError(MR::TransitConditType conditionType, const char* errorMessage)
{
  m_messageLogger->output("[TransitConditionError %i] %s\n", conditionType, errorMessage);
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::outputDeadBlendError(MR::TransitDeadBlendType deadBlendType, const char* errorMessage)
{
  m_messageLogger->output("[TransitDeadBlendError %i] %s\n", deadBlendType, errorMessage);
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::outputMessageError(MR::MessageType messageType, const char* errorMessage)
{
  m_messageLogger->output("[MessageError %i] %s\n", messageType, errorMessage);
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetProcessor::findTakeInXMDAnimData(
  const char*  takename,         // IN: Take to find.
  const XMD::XModel& xmdAnims,   // IN: XMD anim structure to try and find the take in.
  acAnimInfo&  animInfo          // Information about the anim and take that we are interested in.
  )
{
  const XMD::XAnimCycle* cycle;
  uint32_t        i;

  NMP_VERIFY(takename);

  for (i = 0; i != xmdAnims.NumAnimCycles(); ++i)
  {
    cycle = xmdAnims.GetAnimCycle(i);
    if (cycle && (strcmpi(takename, cycle->GetName()) == 0))
    {
      // Found the take we are interested in.
      XMD::XReal firstFrame = cycle->GetStartTime();
      XMD::XReal lastFrame = cycle->GetEndTime();

      animInfo.m_StartTime = firstFrame / cycle->GetFramesPerSecond();
      animInfo.m_EndTime   = lastFrame / cycle->GetFramesPerSecond();
      animInfo.m_FramesPerSecond = (float)cycle->GetFramesPerSecond();
      animInfo.m_NumFrames  = cycle->GetNumFrames();
      animInfo.m_TakeName   = cycle->GetName();

      // Cache the source XMD cycle index so we can find it again quickly later.
      animInfo.m_TakeIndex =  i;
      return true;
    }
  }

  XMD::XList takes;
  xmdAnims.List(takes, XMD::XFn::AnimationTake);
  for (XMD::XList::iterator it = takes.begin(); it != takes.end(); ++it, ++i)
  {
    XMD::XAnimationTake* take = (*it)->HasFn<XMD::XAnimationTake>();
    if (take && (strcmpi(takename, take->GetName()) == 0))
    {
      // Found the take we are interested in.
      XMD::XReal firstFrame = take->GetStartTime();
      XMD::XReal lastFrame = take->GetEndTime();

      animInfo.m_StartTime = firstFrame / take->GetFramesPerSecond();
      animInfo.m_EndTime   = lastFrame / take->GetFramesPerSecond();
      animInfo.m_FramesPerSecond = (float)take->GetFramesPerSecond();
      animInfo.m_NumFrames  = take->GetNumFrames();
      animInfo.m_TakeName   = take->GetName();

      // Cache the source XMD cycle index so we can find it again quickly later.
      animInfo.m_TakeIndex =  i;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void AssetProcessor::listTakesInXMDAnimData(
  const XMD::XModel& xmdAnims, // IN: XMD anim structure to try and find the take in.
  acAnimInfos*       animInfos // Information about the animation takes that we find
  )
{
  NMP_VERIFY(animInfos);

  XMD::XList takes;
  xmdAnims.List(takes, XMD::XFn::AnimationTake);

  animInfos->reserve(animInfos->size() + xmdAnims.NumAnimCycles() + takes.size());

  uint32_t        i;
  for (i = 0; i != xmdAnims.NumAnimCycles(); ++i)
  {
    const XMD::XAnimCycle* cycle = xmdAnims.GetAnimCycle(i);
    if (cycle)
    {
      // Found the take we are interested in.
      XMD::XReal startTime = cycle->GetStartTime();
      XMD::XReal endTime = cycle->GetEndTime();

      acAnimInfo animInfo;
      animInfo.m_StartTime = startTime / cycle->GetFramesPerSecond();
      animInfo.m_EndTime   = endTime / cycle->GetFramesPerSecond();
      animInfo.m_FramesPerSecond = (float)cycle->GetFramesPerSecond();
      animInfo.m_NumFrames  = cycle->GetNumFrames();
      animInfo.m_TakeName   = cycle->GetName();

      // Cache the source XMD cycle index so we can find it again quickly later.
      animInfo.m_TakeIndex =  i;
      animInfos->push_back(animInfo);
    }
  }

  for (XMD::XList::iterator it = takes.begin(); it != takes.end(); ++it, ++i)
  {
    XMD::XAnimationTake* take = (*it)->HasFn<XMD::XAnimationTake>();
    if (take)
    {
      // Found the take we are interested in.
      XMD::XReal startTime = take->GetStartTime();
      XMD::XReal endTime = take->GetEndTime();

      acAnimInfo animInfo;
      animInfo.m_StartTime = startTime / take->GetFramesPerSecond();
      animInfo.m_EndTime   = endTime / take->GetFramesPerSecond();
      animInfo.m_FramesPerSecond = (float)take->GetFramesPerSecond();
      animInfo.m_NumFrames  = take->GetNumFrames();
      animInfo.m_TakeName   = take->GetName();

      // Cache the source XMD cycle index so we can find it again quickly later.
      animInfo.m_TakeIndex = i;
      animInfos->push_back(animInfo);
    }
  }

  // If there are no takes then one is implied by the bind pose
  size_t totalTakes = xmdAnims.NumAnimCycles() + takes.size();
  if (totalTakes == 0)
  {
    acAnimInfo animInfo;
    animInfo.m_StartTime  = 0;
    animInfo.m_EndTime    = 1.0f / (float)acAnimInfo::kBindPoseFPS;
    animInfo.m_FramesPerSecond = (float)acAnimInfo::kBindPoseFPS;
    animInfo.m_NumFrames  = 2;
    animInfo.m_TakeName   = "bind_pose";
    animInfo.m_TakeIndex  = (uint32_t)acAnimInfo::kBindPoseTakeIndex;

    animInfos->push_back(animInfo);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Network instance options
//----------------------------------------------------------------------------------------------------------------------
AssetProcessor::InitNetworkInstanceArgs::InitNetworkInstanceArgs()
{
  m_characterStartPosition.setToZero();
  m_characterStartRotation.identity();
  m_queueTrajectory = true;
  m_queueTransforms = true;
  m_queueSampledEvents = true;
  m_queueSyncEventTrack = true;
  m_startingAnimSetIndex = 0;
  m_startStateNodeID = MR::INVALID_NODE_ID;
}

//----------------------------------------------------------------------------------------------------------------------
AssetProcessor::UpdateNetworkInstanceArgs::UpdateNetworkInstanceArgs()
{
  m_updateTime = 1.0f / 60.0f;
  m_absTime = false;
  m_queueTrajectory = true;
  m_queueTransforms = true;
  m_queueSampledEvents = true;
  m_queueSyncEventTrack = true;
}

//----------------------------------------------------------------------------------------------------------------------
// AssetCompilerPlugin::AnimationFileMetadata
//----------------------------------------------------------------------------------------------------------------------
void AssetCompilerPlugin::AnimationFileMetadata::updateIdentifier()
{
  NMP_VERIFY(m_animFileName);
  NMP_VERIFY(m_animTakeName);
  NMP_VERIFY(m_animFormatType);
  NMP_VERIFY(m_animOptions);

  m_FileNameHash = NMP::hashStringCRC32(m_animFileName);
  m_TakeNameHash = NMP::hashStringCRC32(m_animTakeName);
  m_FormatTypeHash = NMP::hashStringCRC32(m_animFormatType);
  m_OptionsHash = NMP::hashStringCRC32(m_animOptions);

  m_identifier =
    m_FileNameHash ^ m_TakeNameHash ^ m_FormatTypeHash ^ m_OptionsHash;
}

//----------------------------------------------------------------------------------------------------------------------
void AssetCompilerPlugin::AnimationFileRigData::alignTo(const AnimationFileRigData& other)
{
  for (uint32_t iRigData = 0; iRigData < other.m_rigNames.size(); ++iRigData)
  {
    if (strcmp(m_rigNames[iRigData], other.m_rigNames[iRigData]) != 0)
    {
      for (uint32_t iRigSwap = iRigData + 1; iRigSwap < m_rigNames.size(); ++iRigSwap)
      {
        if (strcmp(m_rigNames[iRigSwap], other.m_rigNames[iRigData]) == 0)
        {
          // thanks to the global nature of the contained data, we can simply swap the pointers
          m_rigNames[iRigSwap] = m_rigNames[iRigData];
          m_rigNames[iRigData] = other.m_rigNames[iRigData];
          m_rigs[iRigSwap] = m_rigs[iRigData];
          m_rigs[iRigData] = other.m_rigs[iRigData];

          break;
        }
      }
      NMP_VERIFY_MSG(strcmp(m_rigNames[iRigData], other.m_rigNames[iRigData]) == 0, "Rigdata does not match!");
    }
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
