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
#include "NodeAnimSyncEventsBuilder.h"
#include "EventTrackDiscreteBuilder.h"
#include "EventTrackCurveBuilder.h"
#include "EventTrackDurationBuilder.h"

#include "morpheme/mrNetworkDef.h"
#include "morpheme/Nodes/mrNodeAnimSyncEvents.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/mrAttribData.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeAnimWithEventsBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeAnimWithEventsBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_ANIM_EVENTS);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_LOOP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_SOURCE_ANIM);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  
  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
// Create the default discrete event track asset if it hasn't already been built.
const MR::EventTrackDefBase* NodeAnimWithEventsBuilder::createDefaultDiscreteEventTrack(AssetProcessor* processor, const ME::NetworkDefExport* netDefExport)
{
  MR::Manager::AssetType assetType = MR::Manager::kAsset_EventTrackDiscrete;
  const char assetName[] = "DefaultDiscreteEventTrack";
  char guid[37] = "DefaultDiscreteEventTrack:0x        ";
  uint32_t netNameHash = NMP::hashStringCRC32(netDefExport->getName());
  sprintf_s(guid + 28, sizeof(guid) - 28, "%8x", netNameHash);

  // Has this asset already been processed (Identified by a pointer to the source asset structure).
  const ProcessedAsset* processedAsset = processor->findProcessedAsset(guid);
  if (processedAsset != 0)
  {
    uint32_t lastNewAsset = processor->getLastNewAssetStackValue();
    if (lastNewAsset)
      processedAsset->parendIDs.insert(lastNewAsset);
    return static_cast<const MR::EventTrackDefBase*>(processedAsset->assetMemory.ptr);
  }

  processor->beginNewAssetProcessing();

  // Has this asset already been registered?
  // Need to calculate ID here even though it will be done later on
  int32_t id = processor->getGenUIDFunc()(assetName, guid);
  const MR::EventTrackDefBase* regEntry = static_cast<const MR::EventTrackDefBase*>(MR::Manager::getInstance().getObjectPtrFromObjectID(id));
  if (regEntry != 0)
    return regEntry;

  NMP::Memory::Format memReqs = EventTrackDefDiscreteBuilder::getMemoryRequirements(1, assetName);
  NMP::Memory::Resource assetMemory = NMPMemoryAllocateFromFormat(memReqs);

  MR::EventTrackDefDiscrete* defaultEventTrack = EventTrackDefDiscreteBuilder::initEmpty(&assetMemory, 1, assetName);
  NMP_VERIFY(defaultEventTrack);
  MR::EventDefDiscrete* eventDef = defaultEventTrack->getEvent(0);
  NMP_VERIFY(eventDef);
  eventDef->init(0.0, 1.0, 0);

  // Add the asset to the processed asset list and return
  processedAsset = processor->addAsset(assetMemory, assetType, assetName, guid, &id);
  return static_cast<const MR::EventTrackDefBase*>(processedAsset->assetMemory.ptr);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeAnimWithEventsBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    NMP_UNUSED(childNodeIDs),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
void NodeAnimWithEventsBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeAnimWithEventsBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  const ME::DataBlockExport* nodeDefDatablock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  int32_t animIndex = 0;
  nodeDefDatablock->readInt(animIndex, "AnimIndex");

  bool preComputeSyncEventTracks = false;
  nodeDefDatablock->readBool(preComputeSyncEventTracks, "PreComputeSyncEventTracks");
 
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    0,                                        // numChildren
    0,                                        // numInputCPConnections
    animLibraryExport->getNumAnimationSets(), // numAnimSetEntries
    nodeDefExport);

  // Add space for all the non-animset specific attrib datas.
  result += MR::AttribDataBool::getMemoryRequirements();

  // Find out how much space we need for each of the animation sets' event tracks.
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t i=0; i < nodeDefDep.m_animSets.size(); ++i)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[i];
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);
    ME::AnimationEntryExport* animEntry = const_cast<ME::AnimationEntryExport*>(animSetExport->getAnimationEntry(animIndex));
    NMP_VERIFY_MSG(animEntry, "Anim entry table cannot be sparse.");
    NMP_VERIFY_MSG((uint32_t)animIndex == animEntry->getIndex(), "anim index in anim entry does not match index in animSet");

    // Work out the required sizes for the EventTrack arrays.
    uint32_t numDiscreteEventTracks;
    uint32_t numCurveEventTracks;
    uint32_t numDurationEventTracks;
    uint32_t totalNumEventTracks;
    calculateEventTrackArraySizes(
      numDiscreteEventTracks,
      numDurationEventTracks,
      numCurveEventTracks,
      totalNumEventTracks,
      processor,
      animEntry);

    // Reserve space for this animation set's attrib datas.
    result += MR::AttribDataSourceAnim::getMemoryRequirements();
    result += MR::AttribDataSourceEventTrackSet::getMemoryRequirements(numDiscreteEventTracks, numCurveEventTracks, numDurationEventTracks);

    // Reserve space for sync event tracks if we have been asked to pre-compute them.
    if (preComputeSyncEventTracks)
      result += MR::AttribDataSyncEventTrack::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeAnimWithEventsBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_ANIM_EVENTS);
  const ME::DataBlockExport* nodeDefDatablock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  NetworkDefBuilderBase* networkDefBuilder = processor->getNetworkDefBuilder();
  NMP_VERIFY(networkDefBuilder);
  
  // Does this node pre-compute its sync event tracks.
  bool preComputeSyncEventTracks = false;
  nodeDefDatablock->readBool(preComputeSyncEventTracks, "PreComputeSyncEventTracks");

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,                                                   // numChildren
    0,                                                   // max num active child nodes
    0,                                                   // numInputCPConnections
    0,                                                   // numOutputCPPins
    (uint16_t) animLibraryExport->getNumAnimationSets(), // numAnimSetEntries                     
    nodeDefExport);          
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_ANIM_SOURCE);

  //---------------------------
  // Add the loop flag attrib data that's not animation set specific.
  bool loopVal = false;
  nodeDefDatablock->readBool(loopVal, "Loop");
  MR::AttribDataBool* loopable = MR::AttribDataBool::init(memRes, loopVal, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_LOOP,                     // semantic
    0,                                            // animSetIndex
    loopable,                                     // attribData 
    MR::AttribDataBool::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  bool playBackwardsVal = false; // Does this node play the animation backwards.
  nodeDefDatablock->readBool(playBackwardsVal, "PlayBackwards");
    
  bool generateDeltas = false;   // Does this node generate delta transforms.
  nodeDefDatablock->readBool(generateDeltas, "GenerateAnimationDeltas");

  int32_t animIndex;
  nodeDefDatablock->readInt(animIndex, "AnimIndex");

  //---------------------------
  // Add peer animation set specific data.
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);
    ME::AnimationEntryExport*     animEntry     = const_cast<ME::AnimationEntryExport*>(animSetExport->getAnimationEntry(animIndex));

    NMP_VERIFY_MSG(animEntry, "Anim entry table cannot be sparse.");
    const ME::RigExport* rigExport = animSetExport->getRig();
    NMP_VERIFY(rigExport);
    std::string rigFilename = std::string(rigExport->getDestFilename());

    AP::AssetCompilerPlugin::AnimationFileMetadata animData;
    animData.m_animFileName   = animEntry->getAnimationFilename();
    animData.m_animTakeName   = animEntry->getTakeName();
    animData.m_animOptions    = animEntry->getOptions();
    animData.m_animFormatType = animEntry->getFormat();

    if (NetworkDefBuilder::isMultiRig(rigExport))
    {
      const ProcessedAsset* rigAsset = processor->findProcessedAsset(rigExport->getGUID());
      NMP_VERIFY(rigAsset);
      const MR::AnimRigDef* curRig = (MR::AnimRigDef*) rigAsset->assetMemory.ptr;
      NMP_VERIFY_MSG(curRig != 0, "AnimSet %s does not contain a Rig", animSetExport->getName());

      NetworkDefBuilder::getMultiRigSubRigFilename( rigFilename, rigFilename.c_str(), animEntry, curRig, processor );
    }

    // Update based on recent changes
    animData.updateIdentifier();
    NMP_VERIFY_MSG((uint32_t)animIndex == animEntry->getIndex(), "anim index in anim entry does not match index in animSet");

    MR::RuntimeAssetID rigToAnimMapAssetID;
    uint32_t           animAssetID;

    // Find this animation rig combination in our already processed vector of animations in the AnimationLibrary.
#if defined(NMP_ENABLE_ASSERTS) || defined(NM_ENABLE_EXCEPTIONS)
    bool animFoundOK = 
#endif
      networkDefBuilder->findAnimAndRigToAnimMapAssetIDPair(
                         animData,
                         rigFilename.c_str(),
                         generateDeltas,
                         animAssetID, // OUT
                         rigToAnimMapAssetID); // OUT
    NMP_VERIFY_MSG(animFoundOK, "Could not find processed animation %s", animData.m_animFileName);

    // Find the registered index for the animation source format
    uint8_t registeredAnimFormatIndex;
#if defined(NMP_ENABLE_ASSERTS) || defined(NM_ENABLE_EXCEPTIONS)
    bool status = 
#endif
      MR::Manager::getInstance().findAnimationFormatRegistryEntryIndex(animData.m_animFormatType, registeredAnimFormatIndex);
    NMP_VERIFY_MSG(status, "Animation format: %s not known", animData.m_animFormatType);

    //---------------------------
    // AnimSet specific AttribDatas
    CHAR animSetCharBuf[32];
    CHAR paramName[256];
    _itoa(animSetIndex + 1, animSetCharBuf, 10); // We add one to the index as LUA arrays start at 1 and the manifest was written out using LUA array indices

    // Rig to anim map attrib.
    const ProcessedAsset* rigToAnimMapProcessedAsset =  processor->findProcessedAsset(rigToAnimMapAssetID);
    NMP_VERIFY(
      rigToAnimMapProcessedAsset &&
      (rigToAnimMapProcessedAsset->assetMemory.format.size > 0) &&
      (rigToAnimMapProcessedAsset->assetMemory.format.alignment > 0) &&
      (rigToAnimMapProcessedAsset->assetMemory.format.alignment <= NMP_VECTOR_ALIGNMENT));
    NMP::Memory::Format rigToAnimMapMemoryFormat = rigToAnimMapProcessedAsset->assetMemory.format;

    // Anim source attrib.
    MR::AttribDataSourceAnim* sourceAnim = MR::AttribDataSourceAnim::init(
      memRes,
      registeredAnimFormatIndex,
      animAssetID,
      rigToAnimMapAssetID,
      rigToAnimMapMemoryFormat,
      MR::IS_DEF_ATTRIB_DATA);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_SOURCE_ANIM,                    // semantic
      animSetIndex,                                       // animSetIndex
      sourceAnim,                                         // attribData 
      MR::AttribDataSourceAnim::getMemoryRequirements()); // attribMemReqs


    sourceAnim->m_playBackwards = playBackwardsVal;

    // Start sync event index attrib.
    sprintf_s(paramName, "StartEventIndex_%s", animSetCharBuf);
    sourceAnim->m_startSyncEventIndex = readUInt8FromUInt32(nodeDefDatablock, paramName);

    //---------------------------
    // Load the source anim so we can extract info from it.
    MR::AnimSourceBase* animationSource = MR::Manager::getInstance().requestAnimation(animAssetID);

    //---------------------------
    // Source anim clip info.
    float clipStartFraction = 0.0f;
    float clipEndFraction = 1.0f;
    int clipRangeMode = kMarkerRange;

    sprintf_s(paramName, "ClipRangeMode_%s", animSetCharBuf);
    nodeDefDatablock->readInt(clipRangeMode, paramName);
    
    switch (clipRangeMode)
    {
    case kMarkerRange:
      {
        const ME::TakeExport* take = animEntry->getTakeExportData(processor->getRootDirectory());
        if (take)
        {
          clipStartFraction = take->getClipStart();
          clipEndFraction = take->getClipEnd();
        }
      }
      break;
    case kEntireRange:
      clipStartFraction = 0.0f;
      clipEndFraction = 1.0f;
      break;
    case kCustomRange:
      sprintf_s(paramName, "ClipStartFraction_%s", animSetCharBuf);
      nodeDefDatablock->readFloat(clipStartFraction, paramName);
      NMP_VERIFY_MSG(
        clipStartFraction >= 0.0f && clipStartFraction <= 1.0f,
        "Clip start value %f is not within allowed range 0.0 to 1.0",
        clipStartFraction);
      sprintf_s(paramName, "ClipEndFraction_%s", animSetCharBuf);
      nodeDefDatablock->readFloat(clipEndFraction, paramName);
      break;
    }
    NMP_VERIFY(clipEndFraction >= 0.0f && clipEndFraction <= 1.0f);

    //---------------------------
    // Create and init the anim source nodes event track assets.
    MR::EventTrackDefDiscrete* syncTracksSourceDiscreteTrack = NULL; // The discrete event track we are computing the sync event track from.
    initEventTrackAttribs(
                  processor,
                  netDefExport,
                  animEntry,
                  nodeDef,
                  memRes,
                  loopVal,
                  clipStartFraction,
                  clipEndFraction,
                  animSetIndex,
                  sourceAnim,
                  &syncTracksSourceDiscreteTrack);

    //---------------------------
    // Source anim clip info.
    float clipDurationFraction = clipEndFraction - clipStartFraction;
    NMP_VERIFY_MSG(clipDurationFraction >= 0.0f, "Clip duration fraction is less that 0.0");

    float sourceAnimDuration = animationSource->animGetDuration();
    float clipStartTime = clipStartFraction * sourceAnimDuration;
    float clipDuration = clipDurationFraction * sourceAnimDuration;
    if (clipStartTime + clipDuration > sourceAnimDuration)  // Paranoid floating point error check.
      clipDuration = sourceAnimDuration - clipStartTime;
    NMP_VERIFY_MSG(clipDuration >= 0.0f, "Clip duration is less than 0.0");

    sourceAnim->m_clipStartFraction = clipStartFraction;
    sourceAnim->m_clipEndFraction = clipEndFraction;
    sourceAnim->m_sourceAnimDuration = sourceAnimDuration;
    sourceAnim->m_clipStartSyncEventIndex = syncTracksSourceDiscreteTrack->findEventIndexForTimeFraction(clipStartFraction);
    
    //---------------------------
    // Fail build if we are not able to build a fully valid sync event track using the source data.
    // If this assert is hit increase MAX_NUM_SYNC_EVENTS and recompile runtime target and asset compiler.
    const char* sourceDiscreteTrackName;
    NMP_VERIFY_MSG(MR::EventTrackSync::validateSourceEventCount(
                                        clipStartFraction,
                                        clipDurationFraction,
                                        sourceAnim->m_clipStartSyncEventIndex,
                                        syncTracksSourceDiscreteTrack,
                                        &sourceDiscreteTrackName),
              "Error building Node %s.\n"
              "Max num sync events (MAX_NUM_SYNC_EVENTS: %d) exceeded when generating sync track from source discrete track: %s.\n"
              "Reduce number of events in clip range of source track (or increase MAX_NUM_SYNC_EVENTS)\n",
              nodeDefExport->getName(),
              MAX_NUM_SYNC_EVENTS,
              sourceDiscreteTrackName);

    //---------------------------
    // Precompute the sync event track if we have been asked to do so.
    if (preComputeSyncEventTracks)
    {
      MR::AttribDataSyncEventTrack* syncEventTrack = MR::AttribDataSyncEventTrack::init(memRes, MR::IS_DEF_ATTRIB_DATA);
      initAttribEntry(
        nodeDef,
        MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,                   // semantic
        animSetIndex,                                           // animSetIndex
        syncEventTrack,                                         // attribData 
        MR::AttribDataSyncEventTrack::getMemoryRequirements()); // attribMemReqs

      NMP_ASSERT(syncTracksSourceDiscreteTrack);
      syncEventTrack->m_syncEventTrack.init(
                                        clipStartFraction,
                                        clipDurationFraction,
                                        sourceAnim->m_clipStartSyncEventIndex,
                                        syncTracksSourceDiscreteTrack,
                                        loopVal,
                                        sourceAnim->m_startSyncEventIndex,
                                        clipDuration,
                                        playBackwardsVal);
    }


    //---------------------------
    // Source trajectory transform info.
    NMP::Quat startAttitude, endAttitude;
    NMP::Vector3 startTranslation, endTranslation;
    const MR::TrajectorySourceBase* trajectoryData = animationSource->animGetTrajectorySourceData();
    if (trajectoryData)
    {
      trajectoryData->trajComputeTrajectoryTransformAtTime(clipStartTime, startAttitude, startTranslation);
      trajectoryData->trajComputeTrajectoryTransformAtTime(clipStartTime + clipDuration, endAttitude, endTranslation);
    }
    else
    {
      startTranslation.setToZero();
      endTranslation.setToZero();
      startAttitude.identity();
      endAttitude.identity();
    }

    sourceAnim->m_transformAtStartPos = startTranslation;
    sourceAnim->m_transformAtStartQuat = startAttitude;
    sourceAnim->m_transformAtEndPos = endTranslation;
    sourceAnim->m_transformAtEndQuat = endAttitude;

    sourceAnim->m_sourceTrajectoryChannel = 0;

    //---------------------------
    // unload the animation as we're all done with it :)
    MR::Manager::getInstance().releaseAnimation(animAssetID, animationSource);
  }

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(nodeDef, netDefCompilationInfo, processor->getMessageLogger(), preComputeSyncEventTracks);

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeAnimWithEventsBuilder::trimClipSectionWhenNotLooping(
  bool                         looping,
  float&                       clipStartFraction,
  float&                       clipEndFraction,
  const MR::EventTrackDefBase* eventTrack)
{
  if (!looping)
  {
    const MR::EventTrackDefDiscrete* sourceDescreteSyncTrack = static_cast<const MR::EventTrackDefDiscrete*>(eventTrack);

    for (uint32_t i = 0; i < sourceDescreteSyncTrack->getNumEvents(); ++i)
    {
      const MR::EventDefDiscrete* event = sourceDescreteSyncTrack->getEvent(i);
      float eventStart = event->getStartTime();
      if (clipStartFraction <= eventStart && clipEndFraction > eventStart)
      {
        clipStartFraction = eventStart;
        break;
      }
    }

    if (clipEndFraction < clipStartFraction)
    {
      clipEndFraction = clipStartFraction;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeAnimWithEventsBuilder::initEventTrackAttribs(
  AssetProcessor*                 processor,
  const ME::NetworkDefExport*     netDefExport,
  ME::AnimationEntryExport*       animEntry,
  MR::NodeDef*                    nodeDef,
  NMP::Memory::Resource&          memRes,
  bool                            loopVal,
  float&                          clipStartFraction,
  float&                          clipEndFraction,
  MR::AnimSetIndex                animSetIndex,
  MR::AttribDataSourceAnim*       sourceAnim,
  MR::EventTrackDefDiscrete**     syncTracksSourceDiscreteTrack)
{
  //---------------------------
  // Work out the required sizes for the EventTrack arrays.
  uint32_t numDiscreteEventTracks;
  uint32_t numCurveEventTracks;
  uint32_t numDurationEventTracks;
  uint32_t totalNumEventTracks;
  calculateEventTrackArraySizes(
    numDiscreteEventTracks,
    numDurationEventTracks,
    numCurveEventTracks,
    totalNumEventTracks,
    processor,
    animEntry);
  NMP_VERIFY_MSG(numDiscreteEventTracks > 0, "Zero discrete event tracks");

  //---------------------------
  // Initialise event track attributes.
  MR::AttribDataSourceEventTrackSet* sourceEventTracks = MR::AttribDataSourceEventTrackSet::init(memRes, numDiscreteEventTracks, numCurveEventTracks, numDurationEventTracks, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS, // semantic
    animSetIndex,                            // animSetIndex
    sourceEventTracks,                       // attribData 
    MR::AttribDataSourceEventTrackSet::getMemoryRequirements(numDiscreteEventTracks, numCurveEventTracks, numDurationEventTracks)); // attribMemReqs

  // Process the event tracks we need.
  const ME::EventTrackExport* eventTrackExport = NULL;
  const AP::ProcessedAsset* eventTrackAsset = NULL;
  uint32_t eventType = 0;
  const MR::EventTrackDefBase* eventTrack = NULL;

  //---------------------------
  // Initialise each EventTrack pointer.
  uint32_t discreteTrackCount = 0;
  uint32_t curveTrackCount = 0;
  uint32_t durationTrackCount = 0;
  const ME::TakeExport* take = animEntry->getTakeExportData(processor->getRootDirectory());
  const char* syncTrackName = animEntry->getSyncTrack();
  if (!syncTrackName)
    syncTrackName = "";

  bool useDefaultDiscreteTrack = true;
  *syncTracksSourceDiscreteTrack = NULL;

  for (uint32_t j = 0; j < totalNumEventTracks; ++j)
  {
    eventTrackExport = take->getEventTrack(j);
    ME::GUID eventTrackGuid = eventTrackExport->getGUID();
    ME::EventTrackExport::EventTrackType trackType = eventTrackExport->getEventTrackType() ;

    // Do not include EventTracks with no events as being valid.
    if (eventTrackExport->getNumEvents() > 0 && trackType != ME::EventTrackExport::EVENT_TRACK_TYPE_CUSTOM)
    {
      eventTrackAsset = processor->findProcessedAsset(eventTrackGuid);
      eventTrack = (MR::EventTrackDefBase*) eventTrackAsset->assetMemory.ptr;
      NMP_VERIFY(eventTrack != 0);
      eventType = eventTrack->getType();

      switch (eventType)
      {
      case MR::kEventType_Discrete:
      {
        sourceEventTracks->m_sourceDiscreteEventTracks[discreteTrackCount] =
          (MR::EventTrackDefDiscrete*) eventTrack;
        sourceEventTracks->m_sourceDiscreteEventTrackSizes[discreteTrackCount] = 
          (uint32_t)((EventTrackDefDiscreteBuilder*) eventTrack)->getInstanceMemoryRequirements().size;
        const char* trackName = eventTrackExport->getName();

        if (strcmp(trackName, syncTrackName) == 0)
        {
          // Identify the DiscreteEventTrack to use for synchronization from the syncTrackName set in Connect.
          *syncTracksSourceDiscreteTrack = (MR::EventTrackDefDiscrete*) eventTrack;
          sourceAnim->m_syncEventTrackIndex = discreteTrackCount;
          useDefaultDiscreteTrack = false;

          // Trim the clip start to the next nearest sync event if necessary.
          trimClipSectionWhenNotLooping(loopVal, clipStartFraction, clipEndFraction, eventTrack);
        }
        ++discreteTrackCount;
        break;
      }
      case MR::kEventType_Curve:
      {
        sourceEventTracks->m_sourceCurveEventTracks[curveTrackCount] =
          (MR::EventTrackDefCurve*) eventTrack;
        sourceEventTracks->m_sourceCurveEventTrackSizes[curveTrackCount] =
          (uint32_t)((EventTrackDefCurveBuilder*) eventTrack)->getInstanceMemoryRequirements().size;
        ++curveTrackCount;
        break;
      }
      case MR::kEventType_Duration:
      {
        sourceEventTracks->m_sourceDurEventTracks[durationTrackCount] =
          (MR::EventTrackDefDuration*) eventTrack;
        sourceEventTracks->m_sourceDurEventTrackSizes[durationTrackCount] =
          (uint32_t)((EventTrackDefDurationBuilder*) eventTrack)->getInstanceMemoryRequirements().size;

        ++durationTrackCount;
        break;
      }
      default:
        NMP_THROW_ERROR("Unrecognised EventTrack Type.");
      }
    }
  }

  //---------------------------
  // Use the default discrete event track for any animation source nodes that don't have a sync event track set.
  if (useDefaultDiscreteTrack)
  {
    eventTrack = NodeAnimWithEventsBuilder::createDefaultDiscreteEventTrack(processor, netDefExport);
    NMP_VERIFY_MSG(eventTrack->getType() == MR::kEventType_Discrete, "Expecting event track to be of type discrete");
    *syncTracksSourceDiscreteTrack = (MR::EventTrackDefDiscrete*) eventTrack;
    sourceEventTracks->m_sourceDiscreteEventTracks[discreteTrackCount] =
      (MR::EventTrackDefDiscrete*) eventTrack;
    sourceEventTracks->m_sourceDiscreteEventTrackSizes[discreteTrackCount] =
      (uint32_t)((EventTrackDefDiscreteBuilder*) eventTrack)->getInstanceMemoryRequirements().size;

    sourceAnim->m_syncEventTrackIndex = discreteTrackCount;
    ++discreteTrackCount;
  }

  NMP_VERIFY(discreteTrackCount == numDiscreteEventTracks);
  NMP_VERIFY(durationTrackCount == numDurationEventTracks);
  NMP_VERIFY(curveTrackCount == numCurveEventTracks);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeAnimWithEventsBuilder::calculateEventTrackArraySizes(
  uint32_t&                 numDiscreteEventTracks,
  uint32_t&                 numDurationEventTracks,
  uint32_t&                 numCurveEventTracks,
  uint32_t&                 totalNumEventTracks,
  AssetProcessor*           processor,
  ME::AnimationEntryExport* animEntry)
{
  numDiscreteEventTracks = 0;
  numDurationEventTracks = 0;
  numCurveEventTracks = 0;
  totalNumEventTracks = 0;

  // Count the number of event tracks
  const ME::TakeExport* take = animEntry->getTakeExportData(processor->getRootDirectory());
  const char* syncTrackName = animEntry->getSyncTrack();
  if (!syncTrackName)
    syncTrackName = "";

  uint32_t useDefaultDiscreteTrack = 1;
  if (take)
  {
    totalNumEventTracks = take->getNumEventTracks();
    for (uint32_t j = 0; j < totalNumEventTracks; ++j)
    {
      const ME::EventTrackExport* eventTrackExport = take->getEventTrack(j);
      ME::GUID eventTrackGuid = eventTrackExport->getGUID();
       ME::EventTrackExport::EventTrackType trackType = eventTrackExport->getEventTrackType();

      // we do not support custom event types in the runtime
      if (eventTrackExport->getNumEvents() > 0 && trackType != ME::EventTrackExport::EVENT_TRACK_TYPE_CUSTOM)
      {
        // Do not include EventTracks with no events as being valid.
        const AP::ProcessedAsset* eventTrackAsset = processor->findProcessedAsset(eventTrackGuid);
        const char* takeName = animEntry->getTakeFilename();
        (void) takeName;
        const char* trackName = eventTrackExport->getName();
        (void) trackName;
        NMP_VERIFY_MSG(
                  eventTrackAsset != 0,
                  "Error trying to locate processed event track asset: %s for anim file name: %s.",
                  eventTrackExport->getName(),
                  animEntry->getTakeFilename());
        MR::EventTrackDefBase* eventTrack = (MR::EventTrackDefBase*) eventTrackAsset->assetMemory.ptr;
        NMP_VERIFY(eventTrack);
        
        uint32_t eventType = eventTrack->getType();
        switch (eventType)
        {
          case MR::kEventType_Discrete:
          {
            const char* trackName = eventTrackExport->getName();
            if (strcmp(trackName, syncTrackName) == 0)
              useDefaultDiscreteTrack = 0;
            ++(numDiscreteEventTracks);
            break;
          }
          case MR::kEventType_Curve:
          {
            ++(numCurveEventTracks);
            break;
          }
          case MR::kEventType_Duration:
          {
            ++(numDurationEventTracks);
            break;
          }
          default:
            NMP_THROW_ERROR("Unrecognised EventTrack Type.");
        }
      }
    }
  }

  // Use the default event track if we don't have a selected sync event track.
  numDiscreteEventTracks += useDefaultDiscreteTrack;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeAnimWithEventsBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger,
  bool                        preComputeSyncEventTracks)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();
 
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeAnimSyncEventsQueueSampleTransformsFromAnimSource), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeAnimSyncEventsQueueTrajectoryDeltaTransform), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSource), logger);
  if (preComputeSyncEventTracks) 
  {
    // Set queuing functions that look for the sync track so that they look in the def data.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeAnimSyncEventsQueueTimePosPreCompSyncTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, FN_NAME(nodeAnimSyncEventsQueueTimePosPreCompSyncTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeAnimSyncEventsQueueTimePosPreCompSyncTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeAnimSyncEventsQueueTimePosPreCompSyncTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, FN_NAME(nodeAnimSyncEventsQueueTimePosPreCompSyncTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeAnimSyncEventsQueueSampledEventsBufferPreCompSyncTrack), logger);
  }
  else
  {
    // We are dynamically creating sync track info so make sure that queuing functions look for it in the instance data.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeAnimSyncEventsQueueTimePosDynamicSyncTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, FN_NAME(nodeAnimSyncEventsQueueTimePosDynamicSyncTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeAnimSyncEventsQueueTimePosDynamicSyncTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeAnimSyncEventsQueueTimePosDynamicSyncTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, FN_NAME(nodeAnimSyncEventsQueueTimePosDynamicSyncTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeAnimSyncEventsQueueSampledEventsBufferDynamicSyncTrack), logger);
  }


  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeAnimSyncEventsUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeAnimSyncEventsFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
