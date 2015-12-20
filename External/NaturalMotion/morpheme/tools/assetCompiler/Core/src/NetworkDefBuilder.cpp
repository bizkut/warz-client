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
#include "morpheme/mrDefines.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrMirroredAnimMapping.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "NetworkDefBuilder.h"
#include "RigBuilder.h"
#include "EventTrackDiscreteBuilder.h"
#include "assetProcessor/AssetProcessorUtils.h"
#include "assetProcessor/AnimSource/animSourcePreprocessor.h"
#include "MirroredAnimMappingBuilder.h"
#include "JointLimitsBuilder.h"
#include "RigRetargetMappingBuilder.h"
#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrNode.h"
#include "NodeRetargetBuilder.h"

#include "export/mcExportPhysics.h"

#include <algorithm>
#include <tchar.h>

#include "XMD/AnimCycle.h"
#include "XMD/Model.h"
#include "XMD/Bone.h"

#include <rpc.h> // Required for GUID generation
#pragma comment(lib, "Rpcrt4.lib")

//----------------------------------------------------------------------------------------------------------------------

typedef std::map<std::string, uint32_t> TrackName2UniqueIDMap;

namespace AP
{

const uint32_t MORPHEME_RIG_NOT_FOUND = 0xFFFFFFFF;

std::vector<NetworkDefBuilderPlugin*>                   NetworkDefBuilder::sm_networkPlugins;

std::vector<RigInfo*>                                   NetworkDefBuilder::sm_rigInfoVector;
std::vector<TrajectoryComponentCompatibleRigsGroup*>    NetworkDefBuilder::sm_compatibleRigGroupsVector;
std::vector<AnimCompilationInfo*>                       NetworkDefBuilder::sm_animCompilationVector;
std::vector<ConglomeratedRigBoneNames*>                 NetworkDefBuilder::sm_conglomeratedRigBoneNamesVector;

NetworkDefCompilationInfo*                              NetworkDefBuilder::sm_networkDefCompilationInfo = NULL;
MR::EmittedControlParamsInfo*                           NetworkDefBuilder::sm_emittedControlParamsInfo = NULL;
MR::NodeIDsArray*                                       NetworkDefBuilder::sm_stateMachineNodeIDs = NULL;
MR::NodeIDsArray*                                       NetworkDefBuilder::sm_messageEmitterNodeIDs = NULL;
MR::NodeIDsArray*                                       NetworkDefBuilder::sm_multiplyConnectedNodeIDs = NULL;
NMP::IDMappedStringTable*                               NetworkDefBuilder::sm_stateMachineStateIDStringTable = NULL;
NMP::IDMappedStringTable*                               NetworkDefBuilder::sm_nodeIDNamesTable = NULL;
NMP::OrderedStringTable*                                NetworkDefBuilder::sm_messageIDNamesTable = NULL;
NMP::OrderedStringTable*                                NetworkDefBuilder::sm_eventTrackIDNamesTable = NULL;
MR::NodeTagTable*                                       NetworkDefBuilder::sm_nodeTagTable = NULL;
MR::NetworkDef::NodeEventOnExitMessage*                 NetworkDefBuilder::sm_onExitMessageArray = NULL;
NMP::Memory::Format                                     NetworkDefBuilder::sm_onExitMessageArrayMemReqs;
uint32_t**                                              NetworkDefBuilder::sm_rigToUberrigMaps;
NMP::Memory::Format                                     NetworkDefBuilder::sm_rigToUberrigMapsMemReqs;
uint32_t**                                              NetworkDefBuilder::sm_uberrigToRigMaps;
NMP::Memory::Format                                     NetworkDefBuilder::sm_uberrigToRigMapsMemReqs;

//----------------------------------------------------------------------------------------------------------------------
// Allows NetworkDefBuilder access to the internal parameters of MR::NetworkDef for building
class NetworkDefBuilderEx : public MR::NetworkDef
{
  friend class NetworkDefBuilder;
};

//----------------------------------------------------------------------------------------------------------------------
bool registerBuilderPlugin(NetworkDefBuilderPlugin* plugin)
{
  NetworkDefBuilder::registerBuilderPlugin(plugin);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefBuilder::registerBuilderPlugin(NetworkDefBuilderPlugin* plugin)
{
  sm_networkPlugins.push_back(plugin);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Build the event track name to runtime id map.
NMP::OrderedStringTable* NetworkDefBuilder::buildEventTrackNameToRuntimeIDMap(
  AP::AssetProcessor* processor,
  const ME::NetworkDefExport* netDefExport)
{
  NMP_VERIFY(processor);
  NMP_VERIFY(netDefExport);
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //-----------------------
  // Build up an event track name to id map, and set the runtime ids of all the event tracks.
  TrackName2UniqueIDMap trackIDMap;
  uint32_t tableSize = 0;

  // Add the default discrete event track name.
  trackIDMap["DefaultDiscreteEventTrack"] = (uint32_t)trackIDMap.size();
  tableSize += (uint32_t)(strlen("DefaultDiscreteEventTrack") + 1);

  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  for (uint32_t animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);
    uint32_t numAnimBindings = animSetExport->getNumAnimationEntries();

    for (uint32_t animIndex = 0; animIndex < numAnimBindings; ++animIndex)
    {
      ME::AnimationEntryExport* animEntryExport = const_cast<ME::AnimationEntryExport*>(animSetExport->getAnimationEntry(animIndex));
      if (!animEntryExport)
        processor->getErrorLogger()->output("Error: The animation entry set of an animation set may not be sparsely populated.\n");
      NMP_VERIFY_MSG(animEntryExport, "Anim entry table cannot be sparse.");

      if( NetworkDefBuilder::isAnimEntryValidWithAnimSet( animEntryExport, animSetExport ) == false )
      {
        continue;
      }

      const ME::TakeExport* takeExport = animEntryExport->getTakeExportData(processor->getRootDirectory());
      if (takeExport)
      {
        uint32_t numEventTracks = takeExport->getNumEventTracks();
        for (uint32_t trackIndex = 0; trackIndex < numEventTracks; ++trackIndex)
        {
          const ME::EventTrackExport* eventTrackExport = takeExport->getEventTrack(trackIndex);
          ME::EventTrackExport::EventTrackType trackType = eventTrackExport->getEventTrackType();
          if (eventTrackExport->getNumEvents() > 0 && trackType != ME::EventTrackExport::EVENT_TRACK_TYPE_CUSTOM)
          {
            std::string trackName = eventTrackExport->getName();
            trackIDMap.insert(std::make_pair(trackName, (uint32_t)trackIDMap.size()));
            tableSize += (uint32_t)(trackName.length() + 1);
            }
          }
        }
      }
    }

  uint32_t numStrings = (uint32_t)trackIDMap.size();
  //-----------------------
  // Create the actual table.
  uint32_t* trackIDs = (uint32_t*) NMPMemoryAlloc(sizeof(uint32_t) * numStrings);
  NMP_ASSERT(trackIDs);
  uint32_t* stringOffsets = (uint32_t*) NMPMemoryAlloc(sizeof(uint32_t) * numStrings);
  NMP_ASSERT(stringOffsets);
  char* strings = (char*) NMPMemoryAlloc(tableSize);
  NMP_ASSERT(strings);
  char* currentPtr = strings;
  uint32_t currentOffset = 0;
  TrackName2UniqueIDMap::const_iterator iter = trackIDMap.begin();
  for (; iter != trackIDMap.end(); ++iter)
  {
    uint32_t ID = iter->second;
    stringOffsets[ID] = currentOffset;
    size_t len = iter->first.size() + 1;
    memcpy(currentPtr, iter->first.c_str(), len);
    currentOffset += (uint32_t)len;
    currentPtr += len;
  }

  //-----------------------
  // Initialise the table from the stream
  NMP::Memory::Format memReqs = NMP::OrderedStringTable::getMemoryRequirements(numStrings, tableSize);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  NMP::OrderedStringTable* result = NMP::OrderedStringTable::init(
                               memRes,
                               numStrings,
                               stringOffsets,
                               strings,
                               tableSize);

  NMP_VERIFY_MSG(memRes.format.size == 0, "String table did not use all its requested memory");

  //-----------------------
  // Set the source event track runtime ID of every event in the network.
  uint32_t numNodes = netDefExport->getNumNodes();
  for (uint32_t animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    for (uint32_t nodeIdx = 0; nodeIdx < numNodes; nodeIdx++)
    {
      // Check for a valid animation source node definition
      MR::NodeDef* nodeDef = sm_networkDefCompilationInfo->getNodeDef(nodeIdx);
      if (nodeDef && nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_ANIM_SOURCE)
      {
        const NodeDefDependency & nodeDefDep = sm_networkDefCompilationInfo->getNodeDefDependency(nodeIdx);
        
        if( nodeDefDep.isAnimSetValid( (MR::AnimSetIndex)animSetIndex) == false )
        {
          continue;
        }

        MR::AttribDataHandle* handle( NULL );

        //-----------------------
        // Event tracks
        handle = nodeDef->getAttribDataHandle(
                   MR::ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS,
                   (MR::AnimSetIndex)animSetIndex);
        NMP_VERIFY(handle);
        MR::AttribDataSourceEventTrackSet* sourceEventTracks = (MR::AttribDataSourceEventTrackSet*)handle->m_attribData;

        if (sourceEventTracks)
        {
          // Discrete event tracks
          uint32_t numDiscreteEventTracks = sourceEventTracks->m_numDiscreteEventTracks;
          MR::EventTrackDefDiscrete** eventSequencesDiscrete = sourceEventTracks->m_sourceDiscreteEventTracks;
          for (uint32_t i = 0; i < numDiscreteEventTracks; i++)
          {
            const char* name = eventSequencesDiscrete[i]->getTrackName();
            NMP_VERIFY_MSG(name, "Missing event track name");
            eventSequencesDiscrete[i]->setTrackID(result->getIDForString(name));
          }
          // Curve event tracks
          {
            uint32_t numCurveEventTracks = sourceEventTracks->m_numCurveEventTracks;
            MR::EventTrackDefCurve** eventSequencesFloat = sourceEventTracks->m_sourceCurveEventTracks;
            for (uint32_t i = 0; i < numCurveEventTracks; i++)
            {
              const char* name = eventSequencesFloat[i]->getTrackName();
              NMP_VERIFY_MSG(name, "Missing event track name");
              eventSequencesFloat[i]->setTrackID(result->getIDForString(name));
            }
          }

          // Duration event tracks
          {
            uint32_t numDurEventTracks = sourceEventTracks->m_numDurEventTracks;
            MR::EventTrackDefDuration** eventSequencesDuration = sourceEventTracks->m_sourceDurEventTracks;
            for (uint32_t i = 0; i < numDurEventTracks; i++)
            {
              const char* name = eventSequencesDuration[i]->getTrackName();
              NMP_VERIFY(name);
              eventSequencesDuration[i]->setTrackID(result->getIDForString(name));
            }
          }
        }
      }
    }
  }

  //-----------------------
  // Free all the temporary memory
  NMP::Memory::memFree(trackIDs);
  NMP::Memory::memFree(stringOffsets);
  NMP::Memory::memFree(strings);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefBuilder::findAnimAndRigToAnimMapAssetIDPair(
  const AP::AssetCompilerPlugin::AnimationFileMetadata& animData,
  const char*              rigFilename,
  bool                     generateDeltas,
  MR::RuntimeAnimAssetID&  animAssetID,
  MR::RuntimeAssetID&      rigToAnimMapAssetID)
{
  NMP_VERIFY(animData.m_identifier != 0 && rigFilename);

  for (uint32_t i = 0; i < sm_animCompilationVector.size(); ++i)
  {
    AnimCompilationInfo* currentAnimInfo = sm_animCompilationVector[i];

    bool generatedDeltas = (strstr(currentAnimInfo->m_data.m_animOptions, "genDeltas") != 0);
    if (currentAnimInfo->m_data == animData && generatedDeltas == generateDeltas)
    {
      uint32_t rigBoneNamesIndex = currentAnimInfo->m_conglomeratedRigBoneNamesIndex;
      NMP_VERIFY_MSG(rigBoneNamesIndex < sm_conglomeratedRigBoneNamesVector.size(), "rigBoneNamesIndex is out of range");
      ConglomeratedRigBoneNames* rigBoneNamesEntry = sm_conglomeratedRigBoneNamesVector[rigBoneNamesIndex];
      NMP_VERIFY_MSG(rigBoneNamesEntry, "Missing rig bone names entry");
      NMP_VERIFY_MSG(rigBoneNamesEntry->m_data.m_rigNames.size() <= currentAnimInfo->m_rigToAnimMapAssetIDs.size(), "Mismatched m_rigNames and m_rigToAnimMapAssetIDs counts");
      for (uint32_t k = 0; k < rigBoneNamesEntry->m_data.m_rigNames.size(); ++k)
      {
        if (strcmp(rigFilename, rigBoneNamesEntry->m_data.m_rigNames[k]) == 0)
        {
          rigToAnimMapAssetID = currentAnimInfo->m_rigToAnimMapAssetIDs[k];
          animAssetID = currentAnimInfo->m_animAssetID;
          return true;
        }
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::compileAnimationsAndRigToAnimMaps(
  AP::AssetProcessor*               processor,
  const ME::AnimationLibraryExport* animLibraryExport)
{
  // Loop through all the animations to compile.
  for (uint32_t i = 0; i < sm_animCompilationVector.size(); ++i)
  {
    uint32_t rigBoneNamesIndex = sm_animCompilationVector[i]->m_conglomeratedRigBoneNamesIndex;
    NMP_VERIFY_MSG(rigBoneNamesIndex < sm_conglomeratedRigBoneNamesVector.size(), "Error in compileAnimationsAndRigToAnimMaps");
    ConglomeratedRigBoneNames* rigBoneNamesEntry = sm_conglomeratedRigBoneNamesVector[rigBoneNamesIndex];
    NMP_VERIFY_MSG(rigBoneNamesEntry, "Error in compileAnimationsAndRigToAnimMaps");

    NMP_VERIFY_MSG(rigBoneNamesEntry->m_data.m_rigNames.size() > 0, "Error in compileAnimationsAndRigToAnimMaps");
    NMP_VERIFY_MSG(rigBoneNamesEntry->m_data.m_rigNames.size() == rigBoneNamesEntry->m_rigInfoIndexes.size(), "Error in compileAnimationsAndRigToAnimMaps");

#if NM_ENABLE_EXCEPTIONS
    try
#endif // NM_ENABLE_EXCEPTIONS
    {
#if defined(NMP_ENABLE_ASSERTS) || defined(NM_ENABLE_EXCEPTIONS)
      bool animProcessedOK = 
#endif
        // Actually do the anim and rig to anim processing.
        processor->getProcessAnimFunc()(
                               processor,                                             // IN. The asset processor
                               &(sm_animCompilationVector[i]->m_data),                // IN: AnimationFileMetadata: (filename, takename, format, options etc)
                               &(rigBoneNamesEntry->m_data),                          // IN: The AnimationFileRigData (rig names, conglomerate rig info etc.)
                               animLibraryExport->getGUID(),                          // IN: GUID used as a basis for generation of unique RigToAnimMap GUIDs.
                               sm_animCompilationVector[i]->m_animAssetID,            // OUT: Filled in by process anim function.
                               sm_animCompilationVector[i]->m_rigToAnimMapAssetIDs);  // OUT: Generated rig to anim map asset IDs, from this anim to each rig.
      NMP_VERIFY_MSG(animProcessedOK, "Error compiling animation");
    }
#if NM_ENABLE_EXCEPTIONS
    catch (const NMP::Exception& e)
    {
      processor->setExceptionOccured(true);
      processor->getErrorLogger()->output(
        "Error: %s, File: %s, Line: %d. Compiling animation: %s. Asset processing will continue.\n",
        e.getMessage(),
        e.getFile(),
        e.getLine(),
        sm_animCompilationVector[i]->m_data.m_animFileName);
      _flushall(); // Flush all streams, so that any logged error messages get written to file before proceeding.
    }
#endif // NM_ENABLE_EXCEPTIONS
  }

  //-----------------------
  // Finalize compiling the animations
  AP::AssetProcessor::FinalizeProcessAnimsFunc finalizeAnimsFunc = processor->getFinalizeAnimsFunc();
  if (finalizeAnimsFunc)
  {
    finalizeAnimsFunc(processor);
  }
}

//----------------------------------------------------------------------------------------------------------------------
MR::AnimRigDef* NetworkDefBuilder::getAnimSetExportsProcessedRig(
  AP::AssetProcessor*           processor,
  const ME::AnimationSetExport* animSetExport)
{
  NMP_VERIFY(animSetExport && processor);

  const ME::RigExport* rigExport = animSetExport->getRig();
  NMP_VERIFY(rigExport);
  const ProcessedAsset* rigAsset = processor->findProcessedAsset(rigExport->getGUID());
  NMP_VERIFY(rigAsset);
  MR::AnimRigDef* resultRig = (MR::AnimRigDef*) rigAsset->assetMemory.ptr;
  NMP_VERIFY_MSG(resultRig != 0, "AnimSet %s does not contain a Rig", animSetExport->getName());

  return resultRig;
}

//----------------------------------------------------------------------------------------------------------------------
float NetworkDefBuilder::computeMedianRigBoneLength(const MR::AnimRigDef* rig)
{
  // Get the bind pose buffer
  NMP_VERIFY(rig);
  const MR::AttribDataTransformBuffer* bindPoseTransforms = rig->getBindPose();
  NMP_VERIFY(bindPoseTransforms);
  NMP_VERIFY(bindPoseTransforms->m_transformBuffer);
  const NMP::Vector3* posKeys = bindPoseTransforms->m_transformBuffer->getChannelPos(0);
  NMP_VERIFY(posKeys);
  uint32_t numChannels = bindPoseTransforms->m_transformBuffer->getLength();
  NMP_VERIFY(numChannels > 0);  
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_VERIFY(hierarchy);
  uint32_t characterRootBoneIndex = rig->getCharacterRootBoneIndex();

  // Allocate the temporary memory for the median bone length
  NMP::Memory::Format memReqs(sizeof(float) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  float* boneLengths = (float*) memRes.ptr;

  //-----------------------
  // Compute the bone lengths
  uint32_t numEntries = 0;
  for (uint32_t iChan = 1; iChan < numChannels; ++iChan)
  {
    // Check if this bone is a child of the character root bone (i.e. hips joint)
    if (hierarchy->isParentOf(characterRootBoneIndex, iChan))
    {
      boneLengths[numEntries] = posKeys[iChan].magnitude();
      ++numEntries;
    }
  }

  //-----------------------
  float result = 0.0f;
  if (numEntries > 0)
  {
    // Sort the bone length vector
    for (uint32_t i = 0; i < numEntries - 1; ++i)
    {
      for (uint32_t j = i + 1; j < numEntries; ++j)
      {
        if (boneLengths[j] < boneLengths[i])
        {
          NMP::nmSwap(boneLengths[i], boneLengths[j]);
        }
      }
    }

    // Compute the median
    uint32_t indx = numEntries >> 1;
    if (numEntries & 0x01)
      result = boneLengths[indx]; // Odd
    else
      result = 0.5f * (boneLengths[indx-1] + boneLengths[indx]); // Even
  }

  // Tidy up
  NMP::Memory::memFree(boneLengths);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefBuilder::isMultiRig(const ME::RigExport* rigDefExport)
{
  std::string firstNamespace;
  std::string jointName;
  std::string jointNamespace;
  for (uint32_t i = 0, n = rigDefExport->getNumJoints(); i < n; ++i)
  {
    jointName = rigDefExport->getJoint(i)->getName();
    size_t sepPos = jointName.find(":");
    if (sepPos != jointName.npos)
    {
      jointNamespace = jointName.substr(0, sepPos);
      if (firstNamespace.size() == 0)
      {
        firstNamespace = jointNamespace;
      }
      else if (strcmp(firstNamespace.c_str(), jointNamespace.c_str()) != 0)
      {
        // Found a second namespace so this must be a multi rig
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::findSubRigs(const MR::AnimRigDef* rigDef, std::list<std::string>& subRigs)
{
  // Now find all the namespaces in this rig for this
  for (uint32_t i = 0; i < rigDef->getNumBones(); i++)
  {
    std::string s = std::string(rigDef->getBoneName(i));
    size_t sepPos = s.find(":");
    if (sepPos != s.npos)
    {
      subRigs.push_back(s.substr(0, sepPos));
    }
  }

  // Sort and remove dupes.
  subRigs.sort();
  subRigs.unique();
}

//----------------------------------------------------------------------------------------------------------------------
std::string NetworkDefBuilder::constructSubRigFilename(const char *rigFilename, const char *subRigName)
{
  std::string filename = std::string(rigFilename);
  filename = filename.substr(0, filename.find(".mrarig"));
  filename += "_";
  filename += subRigName;
  filename += ".mrarig";
  return filename;
}
//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::getMultiRigSubRigFilename( 
  std::string & subRigFilenameOut,
  const char * rigFilename,
  const ME::AnimationEntryExport* animEntry,
  const MR::AnimRigDef * curRig,
  AssetProcessor* processor /*=NULL*/ )// provide an AssetProcessor pointer for message logger output.
{
  // get the rig corresponding to this animation
  std::string options = std::string(animEntry->getOptions());
  char optStr[] = "-subrig ";
  size_t start = options.find(optStr);
  std::string subrig;
  if (start != options.npos)
  {
    subrig = options.substr(start + strlen(optStr));
    subrig = subrig.substr(0, subrig.find(" "));
  }
  else
  {
    if( processor )
    {
      processor->getMessageLogger()->output(
        "No sub-rig found in the anim set options. Select one from the sub-rig menu in the asset manager.");
    }
    // since we can't find one specified just use the first option 
    std::list<std::string> nameSpaces;
    findSubRigs(curRig, nameSpaces);
    if(!nameSpaces.empty())
    {
      subrig = nameSpaces.begin()->c_str(); 
    }
    else
    {
      NMP_ASSERT_FAIL_MSG("Multi rig detected without -subrig in options string.  Could not find a default option by inspecting rig.");
    }
  }

  subRigFilenameOut = constructSubRigFilename(rigFilename, subrig.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
bool makeGUID(std::string& guid)
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
bool validateMultiRig(const char* multiRigPath, const char* guidToValidateAgainst)
{
  std::ifstream inputFile;
  inputFile.open(multiRigPath);

  if (!inputFile.is_open())
  {
    return false;
  }

  std::string line;

  bool valid = false;
  
  if (inputFile.good())
  {
    getline(inputFile, line);

    size_t sourceGUIDPos = line.find("sourceGUID=\"");
    if (sourceGUIDPos != std::string::npos)
    {
      std::string sourceGUID = line.substr(sourceGUIDPos + strlen("sourceGUID=\""), 36);
      if (strcmp(sourceGUID.c_str(), guidToValidateAgainst) == 0)
      {
        valid = true;
      }
    }
  }

  inputFile.close();
  return valid;
}

//----------------------------------------------------------------------------------------------------------------------
void createMultiRig(const char* sourceRig, const char* rigNamespace, const char* outputPath, const char* sourceGUID)
{
  std::ifstream inputFile;
  inputFile.open(sourceRig);

  if (!inputFile.is_open())
  {
    NMP_VERIFY_FAIL("Failed to create multi rig. Couldn't open %s for reading.", sourceRig);
  }

  std::ofstream outputFile;
  outputFile.open(outputPath);

  if (!outputFile.is_open())
  {
    NMP_VERIFY_FAIL("Failed to create multi rig. Couldn't open %s for writin.g", outputPath);
  }

  // + 1 to include the namespace separator
  size_t rigNamespaceStrLen = NMP_STRLEN(rigNamespace) + 1;
  std::string line;

  bool firstLine = true;

  // Read the file a line at a time while removing all instances of the namespace
  while (inputFile.good())
  {
    getline(inputFile, line);

    if (firstLine)
    {
      // Adjust the GUID so that the asset processor doesn't confuse this rig with the original
      size_t guidLoc = line.find("guid") + 6;
      std::string guid;
      makeGUID(guid);
      line.replace(guidLoc, guid.size(), guid.c_str());

      // Append with the source guid
      line.replace(line.size() - 1, 1, " sourceGUID=\"");
      line.append(sourceGUID);
      line.append("\">");

      firstLine = false;
    }

    size_t namespaceLoc = line.find(rigNamespace);
    if (namespaceLoc != std::string::npos)
    {
      line.replace(namespaceLoc, rigNamespaceStrLen, "");
    }

    outputFile << line << std::endl;
  }

  inputFile.close();
  outputFile.close();
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildRigInfoVector(
  AP::AssetProcessor*                processor,
  const ME::AnimationLibraryExport*  animLibraryExport,
  std::vector<RigInfo*>&             rigInfoVector)
{
  NMP_VERIFY(rigInfoVector.size() == 0);
  for (uint32_t i = 0; i < animLibraryExport->getNumAnimationSets(); ++i)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(i);
    NMP_VERIFY(animSetExport);
    const ME::RigExport* rigExport = animSetExport->getRig();
    NMP_VERIFY(rigExport);
    MR::AnimRigDef* currentRig = getAnimSetExportsProcessedRig(processor, animSetExport);
    NMP_VERIFY(currentRig);

    std::list<std::string> nameSpaces;
    findSubRigs(currentRig, nameSpaces);

    // See if this is a multi rig
    if (nameSpaces.size() > 1)
    {
      // This is a multi rig, so load up the specific sub rig definitions
      ME::ExportFactoryXML *exportFactory = new ME::ExportFactoryXML();

      std::list<std::string>::iterator nspaceIt = nameSpaces.begin();
      std::list<std::string>::iterator nspaceEnd = nameSpaces.end();
      for (; nspaceIt != nspaceEnd; ++nspaceIt)
      {
        std::string fname = constructSubRigFilename(rigExport->getDestFilename(), nspaceIt->c_str());

        const char* rigGUID = rigExport->getGUID();

        // The multi rig may be out of date or not exist
        if (!validateMultiRig(fname.c_str(), rigGUID))
        {
          createMultiRig(rigExport->getDestFilename(), nspaceIt->c_str(), fname.c_str(), rigGUID);
        }

        ME::AssetExport* multiRigExport = exportFactory->loadAsset(fname.c_str());
        
        NMP_VERIFY_MSG(multiRigExport, "Failed to find rig %s", fname.c_str());

        // NOTE: This relies on new GUIDs being generated in the non-namespaces mrarigs - otherwise GUID will match and it won't get processed
        const AP::ProcessedAsset* rigAsset = processor->doProcessAsset(multiRigExport);
        // TODO: the runtime rig has now been added to the processor. Make sure it's not exported - perhaps remove after anim nodes are compiled?
        //processor->flagAsTemporaryAsset(rigAsset);

        MR::AnimRigDef* loadedRig = (MR::AnimRigDef*)rigAsset->assetMemory.ptr;

        addRigToRigInfoVector((ME::RigExport*)multiRigExport, loadedRig, rigInfoVector);
      }

      delete exportFactory;
    }
    else
    {
      // The standard rig still needs to be build so that the asset manager can function properly
      addRigToRigInfoVector(rigExport, currentRig, rigInfoVector);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefBuilder::addRigToRigInfoVector(
  const ME::RigExport* rigExport, 
  MR::AnimRigDef* currentRig, 
  std::vector<RigInfo*>& rigInfoVector)
{
  float rigScaleFactor = rigExport->getRigScaleFactor();
  NMP_VERIFY(rigScaleFactor > 0.0f);

  // A rig can be used in multiple animation sets within a library
  // so validate that this rig does not already exist in the rig info vector.
  if (findRigInRigInfoVector(currentRig, rigInfoVector) == MORPHEME_RIG_NOT_FOUND)
  {
    // This rig is not yet in the vector so add it.

    // We need to allocate enough memory for the rig info plus a copy of the destination filename
    size_t memSize = RigInfo::getRequiredMemory(rigExport->getDestFilename());
    RigInfo* currentRigInfo = static_cast<RigInfo*>(NMPMemoryAlloc(memSize));
    NMP_ASSERT(currentRigInfo);
    currentRigInfo->m_rig = currentRig;
    currentRigInfo->m_rigScaleFactor = rigScaleFactor;
    currentRigInfo->m_medianRigBoneLength = computeMedianRigBoneLength(currentRig);

    strcpy(currentRigInfo->m_rigFilename, rigExport->getDestFilename());

    rigInfoVector.push_back(currentRigInfo);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkDefBuilder::findRigInRigInfoVector(const char *rigFilename, const std::vector<RigInfo*>& rigInfoVector)
{
  NMP_VERIFY(rigFilename);

  for (uint32_t i = 0; i < rigInfoVector.size(); ++i)
  {
    if (strcmp(rigInfoVector[i]->m_rigFilename, rigFilename) == 0 )
      return i;
  }

  return MORPHEME_RIG_NOT_FOUND;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkDefBuilder::findRigInRigInfoVector(const MR::AnimRigDef* rigToFind, const std::vector<RigInfo*>& rigInfoVector)
{
  NMP_VERIFY(rigToFind);

  for (uint32_t i = 0; i < rigInfoVector.size(); ++i)
  {
    if (rigInfoVector[i]->m_rig == rigToFind)
      return i;
  }

  return MORPHEME_RIG_NOT_FOUND;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::tidyRigInfoVector(std::vector<RigInfo*>& rigInfoVector)
{
  for (uint32_t i = 0; i < rigInfoVector.size(); ++i)
  {
    NMP::Memory::memFree(rigInfoVector[i]);
  }
  rigInfoVector.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildCompatibleRigGroupsVector(
  std::vector<RigInfo*>&                                rigInfoVector,
  std::vector<TrajectoryComponentCompatibleRigsGroup*>& compatibleRigGroupsVector)
{
  const MR::AnimRigDef*                   currentRig;
  TrajectoryComponentCompatibleRigsGroup* rigGroup;
  uint32_t                                rigGroupIndex;

  NMP_VERIFY(compatibleRigGroupsVector.size() == 0);
  for (uint32_t i = 0; i < rigInfoVector.size(); ++i)
  {
    currentRig = rigInfoVector[i]->m_rig;
    NMP_VERIFY(currentRig);

    // Try and find a rig group that this rig is trajectory component compatible with.
    rigGroupIndex = findCompatibleRigGroup(currentRig, compatibleRigGroupsVector, rigInfoVector);
    if (rigGroupIndex == MORPHEME_RIG_NOT_FOUND)
    {
      // No existing compatible group so make a new group.
      rigGroupIndex = (uint32_t) compatibleRigGroupsVector.size();
      rigGroup = new TrajectoryComponentCompatibleRigsGroup;
      compatibleRigGroupsVector.push_back(rigGroup);
    }
    else
    {
      // There is an existing compatible group so use it.
      NMP_VERIFY(rigGroupIndex < compatibleRigGroupsVector.size());
      rigGroup = compatibleRigGroupsVector[rigGroupIndex];
    }
    rigInfoVector[i]->m_compatibleRigsGroupIndex = rigGroupIndex;
    rigGroup->m_compatibleRigsVector.push_back(i);  // Add this rig to a compatibility group.
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkDefBuilder::findCompatibleRigGroup(
    const MR::AnimRigDef*                                       rigToTest,
    const std::vector<TrajectoryComponentCompatibleRigsGroup*>& compatibleRigsVector,
    const std::vector<RigInfo*>&                                rigInfoVector)
{
  for (uint32_t i = 0; i < compatibleRigsVector.size(); ++i)
  {
    const TrajectoryComponentCompatibleRigsGroup* currentRigGroup = compatibleRigsVector[i];
    NMP_VERIFY(currentRigGroup);

    for (uint32_t k = 0; k < currentRigGroup->m_compatibleRigsVector.size(); ++k)
    {
      uint32_t currentRigInfoIndex = currentRigGroup->m_compatibleRigsVector[k];
      NMP_VERIFY(currentRigInfoIndex < rigInfoVector.size());
      const MR::AnimRigDef* currentRig = rigInfoVector[currentRigInfoIndex]->m_rig;
      NMP_VERIFY(currentRig);
      if (AnimRigDefBuilder::haveIdenticalTrajectoryComponents(rigToTest, currentRig))
      {
        return i;
      }
    }
  }

  // No compatible rig group found.
  return MORPHEME_RIG_NOT_FOUND;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::tidyCompatibleRigGroupsVector(std::vector<TrajectoryComponentCompatibleRigsGroup*>& compatibleRigGroupsVector)
{
  for (uint32_t i = 0; i < compatibleRigGroupsVector.size(); ++i)
  {
    TrajectoryComponentCompatibleRigsGroup* entry = compatibleRigGroupsVector[i];
    entry->m_compatibleRigsVector.clear();
    delete entry;
  }
  compatibleRigGroupsVector.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildAnimCompilationInfoVector(
  AP::AssetProcessor*                 processor,
  const ME::AnimationLibraryExport*   animLibraryExport,
  const std::vector<RigInfo*>&        rigInfoVector,
  std::vector<AnimCompilationInfo*> & animCompilationInfoVector)
{
  for (uint32_t i = 0; i < animLibraryExport->getNumAnimationSets(); ++i)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(i);

    // Find the anim sets rig.
    NMP_VERIFY(animSetExport);
     MR::AnimRigDef* currentRig = getAnimSetExportsProcessedRig(processor, animSetExport);
    NMP_VERIFY(currentRig);

    // Step through the animations in the anim set.
    uint32_t numAnimSetEntries = animSetExport->getNumAnimationEntries();
    for (uint32_t k = 0; k < numAnimSetEntries; ++k)
    {
      const ME::AnimationEntryExport* animEntry = animSetExport->getAnimationEntry(k);
      NMP_VERIFY_MSG(animEntry, "Anim entry table cannot be sparse."); // TODO: Need to think about LOD where we may not wish to do any transform calculation?

      // Now check the animation against its' valid animation set (written into the node dependency graph).
      if( NetworkDefBuilder::isAnimEntryValidWithAnimSet( animEntry, animSetExport ) == false )
      {
        continue;
      }

      RigInfo* currentRigInfo = NULL;

      AP::AssetCompilerPlugin::AnimationFileMetadata animData;
      animData.m_animFileName   = animEntry->getAnimationFilename();
      animData.m_animTakeName   = animEntry->getTakeName();
      animData.m_animFormatType = animEntry->getFormat();
      animData.m_animOptions    = animEntry->getOptions();
      animData.updateIdentifier();
     
      uint32_t rigInfoIndex = MORPHEME_RIG_NOT_FOUND;

      getCurrentRigInfoAndIndex( processor,
                                 animEntry,
                                 animSetExport,
                                 currentRig,
                                 rigInfoVector,
                                 rigInfoIndex,
                                 currentRigInfo );
      NMP_VERIFY(currentRigInfo);

      // Try and find an animation entry in the sm_animCompilationVector that has matching anim name, take name and rig group index
      uint32_t animCompilationIndex = findAnimationAndRigGroupInAnimCompilationInfoVector( animData, 
                                                                                           currentRigInfo->m_compatibleRigsGroupIndex,
                                                                                           animCompilationInfoVector );

      updateAnimCompilationInfoVector( animData,
                                       animCompilationInfoVector,
                                       animCompilationIndex,
                                       rigInfoIndex,
                                       currentRigInfo->m_compatibleRigsGroupIndex ); 
      
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefBuilder::isAnimEntryValidWithAnimSet( 
  const ME::AnimationEntryExport* animEntry, 
  const ME::AnimationSetExport* animSetExport )
{
  uint32_t  animEntryIndex  = animEntry->getIndex();
  uint32_t  numNodes        = sm_networkDefCompilationInfo->getNumNodes();
  bool      foundIt         = false;

  for( uint32_t ii=0; ii<numNodes; ++ii )
  {
    const NodeDefDependency & nodeDefDep = sm_networkDefCompilationInfo->getNodeDefDependency(ii);
    if( animEntryIndex == nodeDefDep.m_animationID )
    {
      foundIt = true;
      MR::AnimSetIndex animSetIndex = static_cast<MR::AnimSetIndex>( animSetExport->getIndex() );
      if( nodeDefDep.isAnimSetValid( animSetIndex ))
      {
        return true;
      }
    }
  }
  NMP_VERIFY_MSG( foundIt, "Failed to find the Animation Entry with index %d in the NodeDefDependency tree.", animEntryIndex );
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::getCurrentRigInfoAndIndex(  
  AP::AssetProcessor*             processor,
  const ME::AnimationEntryExport* animEntry,
  const ME::AnimationSetExport*   animSetExport,
  const MR::AnimRigDef*           currentRig,
  const std::vector<RigInfo*> &   rigInfoVector,
  uint32_t &                      rigInfoIndex,
  RigInfo* &                      currentRigInfo )
{
  if(isMultiRig(animSetExport->getRig()))
  {
    const MR::AnimRigDef *  curRig      = getAnimSetExportsProcessedRig(processor, animSetExport);
    const char *            rigFilename = animSetExport->getRig()->getDestFilename();
    std::string             subRigFilename;

    getMultiRigSubRigFilename( subRigFilename, rigFilename, animEntry, curRig, processor );
    rigInfoIndex = findRigInRigInfoVector(subRigFilename.c_str(), rigInfoVector);
    NMP_VERIFY(rigInfoIndex < rigInfoVector.size());
    currentRigInfo = rigInfoVector[rigInfoIndex];
  }
  else
  {
    // Not a multi rig so use the anim sets rig.
    rigInfoIndex = findRigInRigInfoVector(currentRig, rigInfoVector);
    NMP_VERIFY(rigInfoIndex < rigInfoVector.size());
    currentRigInfo = rigInfoVector[rigInfoIndex];
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Return whether the animCompilationInfoVector was added to or updated.
//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefBuilder::updateAnimCompilationInfoVector( 
  AP::AssetCompilerPlugin::AnimationFileMetadata &  animData,
  std::vector<AnimCompilationInfo*> &               animCompilationInfoVector,
  uint32_t                                          animCompilationIndex,
  uint32_t                                          rigInfoIndex,
  uint32_t                                          compatibleRigsGroupIndex )
{
  AnimCompilationInfo* animCompilationInfo( NULL );

  if (strstr(animData.m_animOptions, "") != 0)
  {
    if (animCompilationIndex == MORPHEME_RIG_NOT_FOUND)
    {
      // No existing matching animation entry, so create one.
      animCompilationInfo = new AnimCompilationInfo;
      animCompilationInfo->m_data = animData;
      animCompilationInfo->m_compatibleRigsGroupIndex = compatibleRigsGroupIndex;
      animCompilationInfo->addRigInfoIndexToCompileAgainst(rigInfoIndex);
      animCompilationInfoVector.push_back(animCompilationInfo);
      return true;
    }
    else
    {
      // Animation entry with matching animation and rig group already exists.
      NMP_VERIFY(animCompilationIndex < animCompilationInfoVector.size());
      animCompilationInfo = animCompilationInfoVector[animCompilationIndex];
      if (animCompilationInfo->findRigInfoIndexToCompileAgainst(rigInfoIndex) == MORPHEME_RIG_NOT_FOUND)
      {
        animCompilationInfo->addRigInfoIndexToCompileAgainst(rigInfoIndex);
        return true;
      }
    }
  }
  else if (strstr(animData.m_animOptions, "genDeltas") != 0)
  {
    animData.m_animOptions = "\0";
    animData.updateIdentifier();

    if (animCompilationIndex == MORPHEME_RIG_NOT_FOUND)
    {
      // No existing matching animation entry, so create one.
      animCompilationInfo = new AnimCompilationInfo;
      animCompilationInfo->m_data = animData;
      animCompilationInfo->m_compatibleRigsGroupIndex = compatibleRigsGroupIndex;
      animCompilationInfo->addRigInfoIndexToCompileAgainst(rigInfoIndex);
      animCompilationInfoVector.push_back(animCompilationInfo);
      return true;
    }
    else
    {
      // Animation entry with matching animation and rig group already exists.
      NMP_VERIFY(animCompilationIndex < animCompilationInfoVector.size());
      animCompilationInfo = animCompilationInfoVector[animCompilationIndex];
      if (animCompilationInfo->findRigInfoIndexToCompileAgainst(rigInfoIndex) == MORPHEME_RIG_NOT_FOUND)
      {
        animCompilationInfo->addRigInfoIndexToCompileAgainst(rigInfoIndex);
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkDefBuilder::findAnimationAndRigGroupInAnimCompilationInfoVector(
  const AP::AssetCompilerPlugin::AnimationFileMetadata& animData,
  uint32_t                                              searchCompatibleRigsGroupIndex,
  const std::vector<AnimCompilationInfo*>&              sourceVector)
{
  NMP_VERIFY(animData.m_identifier != 0);

  for (uint32_t i = 0; i < sourceVector.size(); ++i)
  {
    const AnimCompilationInfo* animInfo = sourceVector[i];
    if (animInfo->m_data == animData &&
        (animInfo->m_compatibleRigsGroupIndex == searchCompatibleRigsGroupIndex))
    {
      return i;
    }
  }

  return MORPHEME_RIG_NOT_FOUND;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::tidyAnimCompilationInfoVector(std::vector<AnimCompilationInfo*>& animCompilationVector)
{
  for (uint32_t i = 0; i < animCompilationVector.size(); ++i)
  {
    AnimCompilationInfo* entry = animCompilationVector[i];
    entry->m_rigInfoIndexesToCompileAgainst.clear();
    delete entry;
  }

  animCompilationVector.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/// Utility function used for sorting bone name list.
bool lexicographicallyLessThan(const char* string1, const char* string2)
{
  NMP_VERIFY(string1 && string2);

  int32_t result = strcmp(string1, string2);
  if (result < 0)
    return true;

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// local comparison object to be able to sort indices by the strings they originally represent
class cmpRigInfoNames
{
public:
  cmpRigInfoNames(const std::vector<RigInfo*>& rigInfoVector) :
    m_rigInfoVector(rigInfoVector) { }

  bool operator ()(uint32_t rigInfoIndex1, uint32_t rigInfoIndex2) const
  {
    NMP_VERIFY(rigInfoIndex1 < m_rigInfoVector.size() && rigInfoIndex2 < m_rigInfoVector.size());
    NMP_VERIFY(m_rigInfoVector[rigInfoIndex1]->m_rigFilename && m_rigInfoVector[rigInfoIndex2]->m_rigFilename);

    return strcmp(m_rigInfoVector[rigInfoIndex1]->m_rigFilename, m_rigInfoVector[rigInfoIndex2]->m_rigFilename) < 0;
  }

  cmpRigInfoNames operator=(const cmpRigInfoNames& rhs)
  {
    return cmpRigInfoNames(rhs.m_rigInfoVector);
  }

  const std::vector<RigInfo*>& m_rigInfoVector;
};

//----------------------------------------------------------------------------------------------------------------------
int32_t NetworkDefBuilder::findRigBoneIndexForSimpleHierarchy(
  const std::vector<const MR::AnimRigDef*>& rigs,
  const std::vector<const char*>& rigBoneNames,
  uint32_t rigBoneIndex)
{
  uint32_t numEntries = (uint32_t)rigBoneNames.size();
  NMP_VERIFY(rigBoneIndex < numEntries);

  int32_t result = -1;
  for (uint32_t curBoneIndex = rigBoneIndex; curBoneIndex < numEntries; ++curBoneIndex)
  {
    // Get the current bone name
    const char* boneName = rigBoneNames[curBoneIndex];

    // Get the index of its parent in the conglomerate rig bone names
    int32_t boneNameIndex = -1;
    for (uint32_t rigIndex = 0; rigIndex < rigs.size(); ++rigIndex)
    {
      const MR::AnimRigDef* curRig = rigs[rigIndex];
      uint32_t indx = curRig->getBoneIndexFromName(boneName);
      if (indx != MR::AnimRigDef::BONE_NAME_NOT_FOUND)
      {
        int32_t parentBoneIndex = curRig->getParentBoneIndex(indx);
        if (parentBoneIndex != -1)
        {
          const char* parentBoneName = curRig->getBoneName(parentBoneIndex);
          boneNameIndex = findIndexOfStringInVector(parentBoneName, rigBoneNames);
          NMP_VERIFY(boneNameIndex != -1);
        }
        break;
      }
    }
    NMP_VERIFY(boneNameIndex != (int32_t)curBoneIndex);

    // Check that the bone has a parent in a valid simple hierarchy position
    if (boneNameIndex < 0 || boneNameIndex < (int32_t)rigBoneIndex)
    {
      result = curBoneIndex;
      break;
    }
  }

  NMP_VERIFY(result != -1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::createRigBoneNamesForSimpleHierarchy(
  const std::vector<const MR::AnimRigDef*>& rigs,
  const std::vector<const char*>&           rigBoneNames,
  std::vector<const char*>&                 rigBoneNamesForSimpleHierarchy)
{
  NMP_VERIFY(rigBoneNamesForSimpleHierarchy.size() == 0);
  uint32_t numRigBoneNames = (uint32_t)rigBoneNames.size();

  //---------------------------------
  // Create the vector of bone names (pointers to static strings) and the
  // vector of bone weights (number of rigs each bone appears in).
  std::vector<uint32_t> boneWeights;
  std::vector<const char*>::const_iterator rigBoneIt;
  for (rigBoneIt = rigBoneNames.begin(); rigBoneIt != rigBoneNames.end(); ++rigBoneIt)
  {
    // Add the bone to the simple hierarchy bones list
    const char* boneName = *rigBoneIt;
    rigBoneNamesForSimpleHierarchy.push_back(boneName);
    
    // Determine how many rigs share this bone
    uint32_t weight = 0;
    for (uint32_t rigIndex = 0; rigIndex < rigs.size(); ++rigIndex)
    {
      const MR::AnimRigDef* curRig = rigs[rigIndex];
      uint32_t indx = curRig->getBoneIndexFromName(boneName);
      if (indx != MR::AnimRigDef::BONE_NAME_NOT_FOUND)
      {
        weight++;
      }
    }
    NMP_VERIFY(weight != 0);
    boneWeights.push_back(weight);
  }
  
  //---------------------------------
  // Sort the bone names according to the bone weights
  for (uint32_t i = 0; i < numRigBoneNames - 1; ++i)
  {
    // Find the index of the next bone with the highest weight
    uint32_t indx = i;
    for (uint32_t j = i + 1; j < numRigBoneNames; ++j)
    {
      if (boneWeights[j] > boneWeights[indx])
      {
        indx = j;
      }
    }
    
    // Swap the bone name entries
    if (indx != i)
    {
      NMP::nmSwap(rigBoneNamesForSimpleHierarchy[i], rigBoneNamesForSimpleHierarchy[indx]);
      NMP::nmSwap(boneWeights[i], boneWeights[indx]);
    }
  }
  
  //---------------------------------
  // Validate the simple hierarchy ordering
  for (uint32_t curBoneNameIndex = 0; curBoneNameIndex < numRigBoneNames; ++curBoneNameIndex)
  {
    // Find the index of the next bone that has a valid simple hierarchy ordering.
    int32_t nextValidBoneNameIndex = findRigBoneIndexForSimpleHierarchy(
      rigs,
      rigBoneNamesForSimpleHierarchy,
      curBoneNameIndex);
    NMP_VERIFY(nextValidBoneNameIndex >= (int32_t)curBoneNameIndex);
    
    // Swap the bone name entries to validate the hierarchy ordering
    if (nextValidBoneNameIndex != (int32_t)curBoneNameIndex)
    {
      NMP::nmSwap(
        rigBoneNamesForSimpleHierarchy[curBoneNameIndex],
        rigBoneNamesForSimpleHierarchy[nextValidBoneNameIndex]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildConglomeratedRigBoneNamesVector(
  const std::vector<AnimCompilationInfo*>& animCompilationVector,
  const std::vector<RigInfo*>&             rigInfoVector,
  std::vector<ConglomeratedRigBoneNames*>& conglomeratedRigBoneNamesVector)
{
  uint32_t  rigBonesVectorIndex;
  uint32_t  currentRigIndex;

  ConglomeratedRigBoneNames* rigBoneNamesEntry( NULL );
  const MR::AnimRigDef*      currentRig( NULL );
  const char*                currentBoneName( NULL );
  
  //----------------------------------
  // Compute the average rig bone length. This will be used later when compiling the
  // animations to detect any unchanging position channels.
  float averageRigBoneLength = 0.0f;
  uint32_t numValidEntries = 0;
  for (uint32_t i = 0; i < rigInfoVector.size(); ++i)
  {
    const RigInfo* rigInfo = rigInfoVector[i];
    NMP_VERIFY(rigInfo);
    if (rigInfo->m_medianRigBoneLength > 0.0f)
    {
      averageRigBoneLength += rigInfo->m_medianRigBoneLength;
      numValidEntries++;
    }
  }
  if (numValidEntries > 0)
    averageRigBoneLength /= (float)numValidEntries;

  //----------------------------------
  // Build up the vector of rig groups to conglomerate.
  for (uint32_t i = 0; i < animCompilationVector.size(); ++i)
  {
    AnimCompilationInfo* animCompilationInfoEntry = animCompilationVector[i];
    rigBonesVectorIndex = findRigSetInConglomeratedRigBonesVector(animCompilationInfoEntry->m_rigInfoIndexesToCompileAgainst);
    if (rigBonesVectorIndex == MORPHEME_RIG_NOT_FOUND)
    {
      // Store the conglomerated group we need to compile this anim against.
      animCompilationInfoEntry->m_conglomeratedRigBoneNamesIndex = (uint32_t) conglomeratedRigBoneNamesVector.size();

      // Add new entry to vector.
      rigBoneNamesEntry = new ConglomeratedRigBoneNames;
      rigBoneNamesEntry->m_data.m_conglomerateRig = NULL;
      rigBoneNamesEntry->m_data.m_averageRigBoneLength = averageRigBoneLength;

      rigBoneNamesEntry->addRigInfoIndexes(animCompilationInfoEntry->m_rigInfoIndexesToCompileAgainst);

      // Lexicographically sort info indexes by rig name.
      cmpRigInfoNames predicate(rigInfoVector);
      sort(
        rigBoneNamesEntry->m_rigInfoIndexes.begin(),
        rigBoneNamesEntry->m_rigInfoIndexes.end(),
        predicate);

      // Collect all of the RigInfo data into a sorted compilation vector
      // i.e. rig filenames, AnimRigDef pointers and rig scale factors
      constructRigCompilationVectors(rigBoneNamesEntry, rigInfoVector);

      // Add new entry to vector.
      conglomeratedRigBoneNamesVector.push_back(rigBoneNamesEntry);
    }
    else
    {
      // Store the conglomerated group we need to compile this anim against.
      animCompilationInfoEntry->m_conglomeratedRigBoneNamesIndex = rigBonesVectorIndex;
    }
  }

  //----------------------------------
  // Conglomerate bone names of grouped rigs.
  for (uint32_t i = 0; i < conglomeratedRigBoneNamesVector.size(); ++i)
  {
    ConglomeratedRigBoneNames* entry = conglomeratedRigBoneNamesVector[i];
    for (uint32_t k = 0; k < entry->m_rigInfoIndexes.size(); ++k)
    {
      currentRigIndex = entry->m_rigInfoIndexes[k];
      NMP_VERIFY(currentRigIndex < rigInfoVector.size());
      currentRig = rigInfoVector[currentRigIndex]->m_rig;

      for (uint32_t j = 0; j < currentRig->getNumBones(); ++j)
      {
        currentBoneName = currentRig->getBoneName(j);
        if (entry->findBoneIndex(currentBoneName) == MORPHEME_RIG_NOT_FOUND)
          entry->m_data.m_boneNames.push_back(currentBoneName);
      }
    }

    // Create the conglomerated vector of rig bone names in *simple hierarchy order*. This
    // also orders the bones in the names vector by their weighting (number of rigs the
    // bone appears in) so that animations compiled against the rig set only need to partially
    // decompress the animation channels depending on the LOD.
    std::vector<const char*> rigBoneNamesForSimpleHierarchy;
    NetworkDefBuilder::createRigBoneNamesForSimpleHierarchy(
      entry->m_data.m_rigs,
      entry->m_data.m_boneNames,
      rigBoneNamesForSimpleHierarchy);

    // Allocate rig def for super set rig used later when processing animations.
    NMP::Memory::Resource memResFinalRig = AnimRigDefBuilder::createRigDef(
        rigBoneNamesForSimpleHierarchy,
        entry->m_data.m_rigs);
    entry->m_data.m_conglomerateRig = (MR::AnimRigDef*)memResFinalRig.ptr;

    std::sort(entry->m_data.m_boneNames.begin(), entry->m_data.m_boneNames.end(), lexicographicallyLessThan);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::constructRigCompilationVectors(
  ConglomeratedRigBoneNames* rigBoneNamesEntry,
  const std::vector<RigInfo*>& rigInfoVector)
{
  uint32_t              currentRigInfoIndex;
  const char*           currentRigFilename;
  const MR::AnimRigDef* currentRig;
  float                 rigScaleFactor;

  NMP_VERIFY(rigBoneNamesEntry);

  for (uint32_t i = 0; i < rigBoneNamesEntry->m_rigInfoIndexes.size(); ++i)
  {
    currentRigInfoIndex = rigBoneNamesEntry->m_rigInfoIndexes[i];
    NMP_VERIFY(currentRigInfoIndex < rigInfoVector.size());
    currentRigFilename = rigInfoVector[currentRigInfoIndex]->m_rigFilename;
    NMP_VERIFY(currentRigFilename);
    currentRig = rigInfoVector[currentRigInfoIndex]->m_rig;
    NMP_VERIFY(currentRig);
    rigScaleFactor = rigInfoVector[currentRigInfoIndex]->m_rigScaleFactor;
    NMP_VERIFY(rigScaleFactor > 0.0f);

    rigBoneNamesEntry->m_data.m_rigNames.push_back(currentRigFilename);
    rigBoneNamesEntry->m_data.m_rigs.push_back(currentRig);
    rigBoneNamesEntry->m_data.m_rigScaleFactors.push_back(rigScaleFactor);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::tidyConglomeratedRigBoneNamesVector(std::vector<ConglomeratedRigBoneNames*>& conglomeratedRigBoneNamesVector)
{
  uint32_t i;

  for (i = 0; i < conglomeratedRigBoneNamesVector.size(); ++i)
  {
    ConglomeratedRigBoneNames* entry = conglomeratedRigBoneNamesVector[i];

    // Clear the rig compilation data vectors
    entry->m_rigInfoIndexes.clear();
    entry->m_data.m_rigNames.clear();
    entry->m_data.m_rigs.clear();
    entry->m_data.m_boneNames.clear();
    entry->m_data.m_rigScaleFactors.clear();

    // Free the memory required for the conglomerated rig
    NMP::Memory::memFree((void*)entry->m_data.m_conglomerateRig);

    delete entry;
  }
  conglomeratedRigBoneNamesVector.clear();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkDefBuilder::findRigSetInConglomeratedRigBonesVector(std::vector<uint32_t>& rigInfoIndexes)
{
  uint32_t i;

  for (i = 0; i < sm_conglomeratedRigBoneNamesVector.size(); ++i)
  {
    if (sm_conglomeratedRigBoneNamesVector[i]->hasIdenticalRigSet(rigInfoIndexes))
      return i; // Same rig index set already exists in vector.
  }

  return MORPHEME_RIG_NOT_FOUND; // Rig index set does not exist in vector.
}

//----------------------------------------------------------------------------------------------------------------------
// AnimCompilationInfo functions.
//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimCompilationInfo::findRigInfoIndexToCompileAgainst(uint32_t rigInfoIndexToFind)
{
  uint32_t i;

  for (i = 0; i < m_rigInfoIndexesToCompileAgainst.size(); ++i)
  {
    if (m_rigInfoIndexesToCompileAgainst[i] == rigInfoIndexToFind)
      return i;
  }

  return MORPHEME_RIG_NOT_FOUND;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimCompilationInfo::addRigInfoIndexToCompileAgainst(uint32_t rigInfoIndex)
{
  m_rigInfoIndexesToCompileAgainst.push_back(rigInfoIndex);
}

//----------------------------------------------------------------------------------------------------------------------
// ConglomeratedRigBoneNames functions.
//----------------------------------------------------------------------------------------------------------------------
bool ConglomeratedRigBoneNames::hasIdenticalRigSet(std::vector<uint32_t>& rigInfoIndexes)
{
  uint32_t  i;
  uint32_t  k;
  uint32_t  currentRigIndex;
  bool      found;

  if (rigInfoIndexes.size() != m_rigInfoIndexes.size())
    return false; // Rigs vectors do not match.

  // Order of appearance of rigs info indexes in each vector does not matter, just that each vector has the same content in whatever order.
  for (i = 0; i < m_rigInfoIndexes.size(); ++i)
  {
    currentRigIndex = m_rigInfoIndexes[i];
    found = false;
    for (k = 0; k < rigInfoIndexes.size(); ++k)
    {
      if (rigInfoIndexes[k] == currentRigIndex)
      {
        found = true;
        break;
      }
    }
    if (!found)
      return false; // Rigs vectors do not match.
  }

  return true; // Rigs vectors do match.
}

//----------------------------------------------------------------------------------------------------------------------
void ConglomeratedRigBoneNames::addRigInfoIndexes(std::vector<uint32_t>& rigInfoIndexes)
{
  uint32_t  i;

  for (i = 0; i < rigInfoIndexes.size(); ++i)
  {
    m_rigInfoIndexes.push_back(rigInfoIndexes[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t ConglomeratedRigBoneNames::findBoneIndex(const char* boneName)
{
  uint32_t i;
  std::vector<const char*>::const_iterator it = m_data.m_boneNames.begin();
  std::vector<const char*>::const_iterator end = m_data.m_boneNames.end();

  for (i = 0; it != end; ++it, ++i)
  {
    if (strcmp(*it, boneName) == 0)
      return i; // Bone name exists in bone names vector.
  }

  return MORPHEME_RIG_NOT_FOUND; // Bone name does not exist in bone names vector.
}

//----------------------------------------------------------------------------------------------------------------------
// NetworkDefBuilder functions.
//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::createNetworkDefCompilationInfo(
  const ME::NetworkDefExport* netDefExport,
  AP::AssetProcessor*         NMP_UNUSED(processor))
{
  NMP_VERIFY(!sm_networkDefCompilationInfo);
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  NMP_VERIFY(animLibraryExport);

  // Information
  uint32_t numNodes = netDefExport->getNumNodes();
  NMP_VERIFY(numNodes > 0);
  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);

  // Create the memory for the shared task function tables.
  MR::SharedTaskFnTables* taskQueuingFnTables =
    MR::SharedTaskFnTables::initExcludeData(numNodes);

  MR::SharedTaskFnTables* outputCPTaskFnTables =
    MR::SharedTaskFnTables::initExcludeData(numNodes);

  // Create a new networkDef compilation info.
  sm_networkDefCompilationInfo = new NetworkDefCompilationInfo(
    netDefExport,
    animLibraryExport,
    taskQueuingFnTables,
    outputCPTaskFnTables);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::tidyNetworkDefCompilationInfo()
{
  NMP_VERIFY(sm_networkDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = sm_networkDefCompilationInfo->getTaskQueuingFnTables();
  NMP_VERIFY(taskQueuingFnTables);
  MR::SharedTaskFnTables* outputCPTaskFnTables = sm_networkDefCompilationInfo->getOutputCPTaskFnTables();
  NMP_VERIFY(outputCPTaskFnTables);

  //-------------------------
  // Tidy up the shared task function tables.
  taskQueuingFnTables->freeRegisteredTaskFnTables();
  NMP::Memory::memFree(taskQueuingFnTables);

  outputCPTaskFnTables->freeRegisteredTaskFnTables();
  NMP::Memory::memFree(outputCPTaskFnTables);

  //-------------------------
  // Tidy the semantic lookup tables.
  sm_networkDefCompilationInfo->tidySemanicLookupTables();

  //-------------------------
  // Tidy the message distributors.
  sm_networkDefCompilationInfo->tidyMessageInterest();
      
  // Delete the actual network def compilation info.
  delete sm_networkDefCompilationInfo;
  sm_networkDefCompilationInfo = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::tidyNetworkNodeDefCompilationInfo()
{
  // Tidy up the event track names table.
  if (sm_eventTrackIDNamesTable)
  {
    NMP::Memory::memFree(sm_eventTrackIDNamesTable);
    sm_eventTrackIDNamesTable = NULL;
  }

  // Tidy up the message ID names table.
  if (sm_messageIDNamesTable)
  {
    NMP::Memory::memFree(sm_messageIDNamesTable);
    sm_messageIDNamesTable = NULL;
  }

  // Tidy up the state machine state id string table.
  if (sm_stateMachineStateIDStringTable)
  {
    NMP::Memory::memFree(sm_stateMachineStateIDStringTable);
    sm_stateMachineStateIDStringTable = NULL;
  }

  // Tidy up the node ID names table
  if (sm_nodeIDNamesTable)
  {
    NMP::Memory::memFree(sm_nodeIDNamesTable);
    sm_nodeIDNamesTable = NULL;
  }

  // Tidy up the message Emitter Nodes ID array.
  if (sm_messageEmitterNodeIDs)
  {
    NMP::Memory::memFree(sm_messageEmitterNodeIDs);
    sm_messageEmitterNodeIDs = NULL;
  }

  // Tidy up the node onExit Message array.
  if (sm_onExitMessageArray)
  {
    NMP::Memory::memFree(sm_onExitMessageArray);
    sm_onExitMessageArray = NULL;
  }

  // Tidy up the state machine Node IDs array.
  if (sm_stateMachineNodeIDs)
  {
    NMP::Memory::memFree(sm_stateMachineNodeIDs);
    sm_stateMachineNodeIDs = NULL;
  }

  // Tidy up the emitted control parameters information.
  if (sm_emittedControlParamsInfo)
  {
    NMP::Memory::memFree(sm_emittedControlParamsInfo);
    sm_emittedControlParamsInfo = NULL;
  }

  // Tidy up the multiply connected Node IDs array.
  if (sm_multiplyConnectedNodeIDs)
  {
    NMP::Memory::memFree(sm_multiplyConnectedNodeIDs);
    sm_multiplyConnectedNodeIDs = NULL;
  }

  // Tidy up the node tag table
  if (sm_nodeTagTable)
  {
    NMP::Memory::memFree(sm_nodeTagTable);
    sm_nodeTagTable = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDefDependency::addChildConnection(NodeDefDependency* nodeDefDep)
{
  NMP_VERIFY(nodeDefDep);

  // Add to child nodes.
  std::vector<NodeDefDependency*>::iterator cit = std::find(
        m_childNodeIDs.begin(),
        m_childNodeIDs.end(),
        nodeDefDep);
  if (cit == m_childNodeIDs.end())
  {
    m_childNodeIDs.push_back(nodeDefDep);

    // Add to parent nodes of the child
    std::vector<NodeDefDependency*>::iterator pit = std::find(
          nodeDefDep->m_parentNodeIDs.begin(),
          nodeDefDep->m_parentNodeIDs.end(),
          this);
    if (pit == nodeDefDep->m_parentNodeIDs.end())
    {
      nodeDefDep->m_parentNodeIDs.push_back(this);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeDefDependency::removeChildConnection(NodeDefDependency* nodeDefDep)
{
  // Remove from child nodes
  std::vector<NodeDefDependency*>::iterator cit = std::find(
        m_childNodeIDs.begin(),
        m_childNodeIDs.end(),
        nodeDefDep);
  if (cit != m_childNodeIDs.end())
  {
    // Remove from parent nodes of the child
    std::vector<NodeDefDependency*>::iterator pit = std::find(
          nodeDefDep->m_parentNodeIDs.begin(),
          nodeDefDep->m_parentNodeIDs.end(),
          this);
    if (pit != nodeDefDep->m_parentNodeIDs.end())
    {
      nodeDefDep->m_parentNodeIDs.erase(pit);
    }

    m_childNodeIDs.erase(cit);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeDefDependency::isAnimSetValid( MR::AnimSetIndex animSetIndex ) const
{
  for( uint32_t i=0; i<m_animSets.size(); ++i )
  {
    if( animSetIndex == m_animSets[i] )
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkDefBuilder::buildSemanticLookupTables(
  NetworkDefCompilationInfo*  networkDefCompilationInfo,
  AP::AssetProcessor*         processor,
  const ME::NetworkDefExport* netDefExport)
{
  //-----------------
  // Generate table of unique NodeType ids that have semantic lookup tables.
  NMP_VERIFY(networkDefCompilationInfo);
  uint32_t numNodes = networkDefCompilationInfo->getNumNodes();
  NMP_VERIFY(numNodes > 0);

  uint32_t semanticLookupTablesCount = 0;
  MR::NodeType* hasLookupTableNodeTypeIDs = (MR::NodeType*) NMPMemoryAlloc((sizeof(MR::NodeType) * numNodes));
  NMP_ASSERT(hasLookupTableNodeTypeIDs);
  MR::NodeType  currentNodeType;

  // Add an entry for the NetworkDef Node.
  hasLookupTableNodeTypeIDs[semanticLookupTablesCount] = NODE_TYPE_NETWORK;
  ++semanticLookupTablesCount;

  // Now scan through all the other nodes.
  for (uint32_t i = 1; i < numNodes; ++i)
  {
    NodeDefDependency& nodeDep = networkDefCompilationInfo->getNodeDefDependency(i);
    currentNodeType = nodeDep.m_nodeTypeID;
    bool foundNodeType = false;

    AP::NodeDefBuilder* nodeBuilder = processor->getNodeBuilder(currentNodeType);
    NMP_VERIFY_MSG(
      nodeBuilder,
      "Cannot find node builder for node with TypeID %i, perhaps it was not registered",
      currentNodeType);

    if (nodeBuilder->hasSemanticLookupTable())
    {
      for (uint32_t k = 0; k < semanticLookupTablesCount; ++k)
      {
        if (currentNodeType == hasLookupTableNodeTypeIDs[k])
        {
          foundNodeType = true;
        }
      }

      if (!foundNodeType)
      {
        hasLookupTableNodeTypeIDs[semanticLookupTablesCount] = currentNodeType;
        ++semanticLookupTablesCount;
      }
    }
  }

  // Allow an extra empty table for use by nodes with no data.
  ++semanticLookupTablesCount;

  //-----------------
  // Initialise the node type semantic lookup tables. 
  NMP::Memory::Format semanticLookupTablesMemReqs = MR::SemanticLookupTable::getSemanticLookupTablesMemoryRequirements(semanticLookupTablesCount);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(semanticLookupTablesMemReqs);
  MR::SemanticLookupTable** semanticLookupTables = MR::SemanticLookupTable::initSemanticLookupTables(memRes, semanticLookupTablesCount);

  // Create the empty semantic lookup table for use by nodes with no data.
  semanticLookupTables[0] = MR::SemanticLookupTable::init(memRes, MR::INVALID_NODE_TYPE_ID);

  // Create the semantic lookup table of the NODE_TYPE_NETWORK.
  // Special case because there is no NodeBuilder for this type.
  semanticLookupTables[1] = MR::SemanticLookupTable::init(memRes, NODE_TYPE_NETWORK);
  initNetworkDefNodeSemanticLookupTable(netDefExport, semanticLookupTables[1]);
    
  // Build each NodeTypes lookup table.
  for (uint32_t i = 1; i < semanticLookupTablesCount - 1; ++i)
  {
    // Call the NodeTypes own function for creating and initialising its lookup table.
    AP::NodeDefBuilder* nodeBuilder = processor->getNodeBuilder(hasLookupTableNodeTypeIDs[i]);
    NMP_VERIFY_MSG(
      nodeBuilder,
      "Cannot find node builder for node with TypeID %i, perhaps it was not registered",
      hasLookupTableNodeTypeIDs[i]);

    semanticLookupTables[i+1] = nodeBuilder->initSemanticLookupTable(memRes); 
  }

  // Store the tables that have been generated so that node builders can access them when needed.
  networkDefCompilationInfo->setSemanticLookupTables(semanticLookupTablesCount, semanticLookupTables, semanticLookupTablesMemReqs);

  //-----------------
  // Tidy up.
  NMP::Memory::memFree(hasLookupTableNodeTypeIDs);

  return semanticLookupTablesCount;
}

//----------------------------------------------------------------------------------------------------------------------
// Calculation of the valid animation sets per node. This is written into the NodeDefDependency graph.
//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::computeValidAnimSetsForAllNodes(const ME::NetworkDefExport* netDefExport)
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  NMP_VERIFY(animLibraryExport);

  const uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);

  const NodeDefDependency & netDep = sm_networkDefCompilationInfo->getNodeDefDependency(0);
  const uint32_t id                = netDefExport->getRootNodeID();
  NodeDefDependency & rootDep      = sm_networkDefCompilationInfo->getNodeDefDependency(id);

  computeValidAnimSetsForNode( netDefExport, rootDep, netDep.m_animSets );
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::computeValidAnimSetsForNode(  
  const ME::NetworkDefExport*   netDefExport,
  NodeDefDependency &           currentNodeDep, 
  std::vector<MR::AnimSetIndex> animSets)
{
  if( setAnimSets( currentNodeDep, animSets ) == false )
  {
    // No new anim sets were written. 
    // We have been here before and have no new information.
    // We can stop here.
    return;
  }

  // returns if animSets is written too (but not necessarily modified) - currently unused.
  getNodeValidAnimSets( netDefExport, currentNodeDep, animSets );

  // Node dependencies
  for (uint32_t i = 0; i < currentNodeDep.m_childNodeIDs.size(); ++i)
  {
    const uint32_t id = currentNodeDep.m_childNodeIDs[i]->m_nodeID;
    NodeDefDependency & nodeDep = sm_networkDefCompilationInfo->getNodeDefDependency(id);
    computeValidAnimSetsForNode( netDefExport, nodeDep, animSets );
  }
}

///----------------------------------------------------------------------------------------------------------------------
// Writes animSetsToSet into the nodeDeps' m_animSets vector (the valid anim sets for this node).
// Returns whether the nodeDeps' anim sets vector was written too.
//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefBuilder::setAnimSets(NodeDefDependency & nodeDep, const std::vector<MR::AnimSetIndex> & animSetsToSet)
{
  bool animSetsWrittenTo = false;

  if( nodeDep.m_animSets.size() == 0)
  {
    // Nothing written here so far.
    nodeDep.m_animSets.assign( animSetsToSet.begin(), animSetsToSet.end() );
    animSetsWrittenTo = true;
  }
  else
  {
    // We have been here before. 
    // Do we have any new information?
    std::vector<MR::AnimSetIndex>::const_iterator it( animSetsToSet.begin() );
    std::vector<MR::AnimSetIndex>::const_iterator it_end( animSetsToSet.end() );
    for(; it != it_end; ++it)
    {
      MR::AnimSetIndex animSet = *it;
      std::vector<MR::AnimSetIndex>::const_iterator find_it = std::find(  nodeDep.m_animSets.begin(),
        nodeDep.m_animSets.end(),
        animSet );

      // we didn't find this anim set already present - so it needs to be added.
      if( find_it == nodeDep.m_animSets.end() )
      {
        NMP_VERIFY( nodeDep.m_animSets.size() < nodeDep.m_animSets.capacity() );
        nodeDep.m_animSets.push_back( animSet );
        animSetsWrittenTo = true;
      }
    }
  }

  return animSetsWrittenTo;
}

//----------------------------------------------------------------------------------------------------------------------
// Gets the valid anim sets for this node. Remember this information is passed down to the children.
// Returns whether the parameter animSets vector was written (but not necessarily modified)
//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefBuilder::getNodeValidAnimSets( 
  const ME::NetworkDefExport*     netDefExport,
  const NodeDefDependency &       nodeDep, 
  std::vector<MR::AnimSetIndex> & animSets)
{
  switch( nodeDep.m_nodeTypeID )
  {
  case NODE_TYPE_RETARGET:
    {
      const ME::NodeExport*       nodeDefExport     = netDefExport->getNode(nodeDep.m_nodeID);
      NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_RETARGET, "Expecting node type RETARGET");
      const ME::DataBlockExport*  nodeDefDataBlock  = nodeDefExport->getDataBlock();
      uint32_t                    numAnimSets       = netDefExport->getAnimationLibrary()->getNumAnimationSets();
      // Get anim sets from the retarget node.
      animSets.clear();
      NodeRetargetBuilder::getAnimSetsFromDataBlockExport( nodeDefDataBlock, animSets, numAnimSets );
    }break;

  default:
    {
      return false;
    }
  }

  // Remove duplicates.
  std::sort( animSets.begin(), animSets.end() );
  animSets.erase( std::unique( animSets.begin(), animSets.end() ), animSets.end() );

  // If we have reached here - success!
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::debugWriteAnimSetsPerNode(const ME::NetworkDefExport* netDefExport)
{
  NET_LOG_MESSAGE( NMP::LOG_PRIORITY_ALWAYS, "------------------------------------------------------------------------\n" );
  NET_LOG_MESSAGE( NMP::LOG_PRIORITY_ALWAYS, "ANIM SETS PER NODE\n\n" );
  uint32_t numNodes = sm_networkDefCompilationInfo->getNumNodes();
  for (uint32_t i = 0; i < numNodes; ++i)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    const char* nodeName                = nodeDefExport->getName();
    NodeDefDependency& nodeDep          = sm_networkDefCompilationInfo->getNodeDefDependency(i);

    CHAR debugStr[256];
    uint32_t pos = sprintf( debugStr,"%s: ", nodeName);
    for (uint32_t i=0; i<nodeDep.m_animSets.size(); ++i)
    {
      pos += sprintf( debugStr+pos,"%d ", nodeDep.m_animSets[i]);
      NMP_ASSERT( pos < 256 );
    }
    NET_LOG_MESSAGE( NMP::LOG_PRIORITY_ALWAYS, "%s\n", debugStr );
  }
  NET_LOG_MESSAGE( NMP::LOG_PRIORITY_ALWAYS, "\nANIM SETS PER NODE\n" );
  NET_LOG_MESSAGE( NMP::LOG_PRIORITY_ALWAYS, "------------------------------------------------------------------------\n" );
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildNodeDefDependencyGraph(
  AP::AssetProcessor*         processor,
  const ME::NetworkDefExport* netDefExport)
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  NMP_VERIFY(animLibraryExport);
  const uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);

  NMP_VERIFY(sm_networkDefCompilationInfo);
  uint32_t numNodes = sm_networkDefCompilationInfo->getNumNodes();

  // Network dependency
  NodeDefDependency& netDep = sm_networkDefCompilationInfo->getNodeDefDependency(0);
  netDep.m_nodeID = MR::NETWORK_NODE_ID;
  netDep.m_nodeTypeID = NODE_TYPE_NETWORK;
  // The network node is always valid for all the animation sets.
  netDep.m_animSets.reserve(numAnimSets);
  for(MR::AnimSetIndex i=0; i<numAnimSets; ++i)
  {
    netDep.m_animSets.push_back( i );
  }

  // Node dependencies
  // Starts from '1' which is the root. NodeID '0' is the network node.
  for (uint32_t i = 1; i < numNodes; ++i)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    const char* nodeName = nodeDefExport->getName();
    NodeDefDependency& nodeDep = sm_networkDefCompilationInfo->getNodeDefDependency(i);

    nodeDep.m_nodeID = (MR::NodeID)i;
    nodeDep.m_nodeTypeID = nodeDefExport->getTypeID();
    // Store the animation entry index from the Library.xml. This is so we can exclude the animation
    // from being built based on the valid animation set info.
    const ME::DataBlockExport* nodeDefDataBlock  = nodeDefExport->getDataBlock();
    nodeDefDataBlock->readUInt(nodeDep.m_animationID, "AnimIndex");
    // Set the maximum capacity for the anim sets to avoid the mallocing of memory
    // as the vectors are written to.
    nodeDep.m_animSets.reserve(numAnimSets);

    // Check if the node is some type of control parameter
    if (strncmp(nodeName, "ControlParameters|", 18) == 0)
      nodeDep.m_isControlParamType = true;
    else
      nodeDep.m_isControlParamType = false;

    // Get the node builder
    AP::NodeDefBuilder* nodeBuilder = processor->getNodeBuilder(nodeDep.m_nodeTypeID);
    if (!nodeBuilder)
    {
      processor->outputNodeError(
        nodeDep.m_nodeTypeID,
        nodeDefExport->getNodeID(),
        "Cannot find node builder.");
    }
    NMP_VERIFY_MSG(
      nodeBuilder,
      "Cannot find node builder for node with TypeID %i, perhaps it was not registered",
      nodeDep.m_nodeTypeID);

    // Compute the list of the nodes child connections
    // These are written into the inputNodeIDs vector.
    std::vector<MR::NodeID> inputNodeIDs;
    nodeBuilder->getNodeDefInputConnections(
      inputNodeIDs,
      nodeDefExport,
      netDefExport,
      processor);

    // Update the node def dependencies
    std::vector<MR::NodeID>::iterator it;
    for (it = inputNodeIDs.begin(); it != inputNodeIDs.end(); ++it)
    {
      MR::NodeID inputNodeID = *it;
      NMP_VERIFY(inputNodeID < numNodes);
      nodeDep.addChildConnection(&sm_networkDefCompilationInfo->getNodeDefDependency(inputNodeID));
    }
  }

  // Write the valid anim sets for each node.
  //
  // We do it here as we call addChildConnection() for the network node below.
  // addChildConnection() writes the connections both ways parent into child and child into parent.
  // As the network node by definition is valid for all anim sets this means we are writing all
  // the anim sets as valid into any node that has the network node as its' parent.
  computeValidAnimSetsForAllNodes(netDefExport);
  // To view the valid anim sets computed uncomment the debug function below.
  // debugWriteAnimSetsPerNode(netDefExport); 

  // Update the network dependencies
  for (uint32_t i = 1; i < numNodes; ++i)
  {
    if (netDefExport->getNode(i)->getDownstreamParentID() == MR::NETWORK_NODE_ID)
    {
      netDep.addChildConnection(&sm_networkDefCompilationInfo->getNodeDefDependency(i));
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::computeNodeDefDependencies(
  NodeDefDependency*          nodeDefDep,
  bool*                       evalNodeDefDeps)
{
  NMP_VERIFY(nodeDefDep);
  NMP_VERIFY(evalNodeDefDeps);
  MR::NodeID nodeID = nodeDefDep->m_nodeID;
  if (evalNodeDefDeps[nodeID])
    return;
  evalNodeDefDeps[nodeID] = true;

  // Child node dependencies
  std::vector<NodeDefDependency*>::iterator nit;
  for (nit = nodeDefDep->m_childNodeIDs.begin(); nit != nodeDefDep->m_childNodeIDs.end(); ++nit)
  {
    NodeDefDependency* childNodeDefDep = *nit;
    computeNodeDefDependencies(
      childNodeDefDep,
      evalNodeDefDeps);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::computeNodeDefDependencies(
  MR::NodeID rootNodeID,
  std::vector<MR::NodeID>& nodeDefDeps)
{
  uint32_t numNodes = sm_networkDefCompilationInfo->getNumNodes();
  nodeDefDeps.clear();
  NMP_VERIFY(rootNodeID < (MR::NodeID)numNodes);

  // Check if we need to build the final network node definition
  if (rootNodeID == 0)
  {
    for (uint32_t i = 1; i < numNodes; ++i)
    {
      nodeDefDeps.push_back((MR::NodeID) i);
    }
  }
  else
  {
    bool* evalNodeDefDeps = new bool[numNodes];
    evalNodeDefDeps[0] = true;
    for (uint32_t i = 1; i < numNodes; ++i)
      evalNodeDefDeps[i] = false;

    // Compute the required node dependencies
    NodeDefDependency* nodeDefDep = &sm_networkDefCompilationInfo->getNodeDefDependency(rootNodeID);
    computeNodeDefDependencies(
      nodeDefDep,
      evalNodeDefDeps);

    // Add all the control parameter nodes
    for (uint32_t i = 1; i < numNodes; ++i)
    {
      NodeDefDependency& nodeDep = sm_networkDefCompilationInfo->getNodeDefDependency(i);
      if (nodeDep.m_isControlParamType)
      {
        evalNodeDefDeps[nodeDep.m_nodeID] = true;
      }
    }

    // Compute the vector of node dependencies (in node id order)
    for (uint32_t i = 1; i < numNodes; ++i)
    {
      if (evalNodeDefDeps[i])
      {
        nodeDefDeps.push_back((MR::NodeID) i);
      }
    }

    // Tidy up
    delete[] evalNodeDefDeps;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkDefBuilder::getMemoryRequirementsNetworkNodeDef(
  const ME::NetworkDefExport*  netDefExport)
{
  NMP_VERIFY(netDefExport);
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  // Reserve space for the NetworkDef class itself
  NMP::Memory::Format result(0, NMP_VECTOR_ALIGNMENT);
  result += NMP::Memory::Format(sizeof(MR::NetworkDef), NMP_NATURAL_TYPE_ALIGNMENT);

  // Compute the required number of attrib data items
  uint16_t numNetworkAttribDataItems = calcNumNetworkDefAttribDatas(netDefExport);
  
  // Reserve space for base class requirements.
  result += MR::NodeDef::getMemoryRequirementsExcludeBaseMem(1, 0, numNetworkAttribDataItems, 0);

  bool hasMirroredMapping = containsNodeOfType(netDefExport, NODE_MIRROR_TRANSFORMS_ID);
  bool hasRetargetMapping = containsNodeOfType(netDefExport, NODE_TYPE_RETARGET);

  //-----------------
  // Attrib data
  std::set<std::string> differentBoneNames;
  uint32_t numBones = 0;
  for (uint32_t i = 0; i < animLibraryExport->getNumAnimationSets(); ++i)
  {
    // Add space for the rig attrib data.
    result += MR::AttribDataRig::getMemoryRequirements();

    // Add space for the default output mask
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(i);
    const ME::RigExport* rigExport = animSetExport->getRig();
    uint32_t numChannels = rigExport->getNumJoints();
    result += MR::AttribDataBoolArray::getMemoryRequirements(numChannels);

    for (uint32_t channelIndex = 0; channelIndex < numChannels; channelIndex++)
    {
      differentBoneNames.insert(rigExport->getJoint(channelIndex)->getName());
    }
    numBones += numChannels;

    // Add space for the character controller def.
    result += MR::AttribDataCharacterControllerDef::getMemoryRequirements();

    // Add space for the mirrored anim mapping
    if (hasMirroredMapping)
    {
      result += getMirrorMappingAttribDataMemReqs(animSetExport);
    }

    // Add space for joint limits
    if (rigExport->getNumJointLimits() > 0)
    {
      result += getJointLimitsAttribDataMemReqs(rigExport);
    }

    // Add space for the retarget mapping
    if (hasRetargetMapping)
    {
      result += getRigRetargetMappingAttribDataMemReqs(animSetExport);
    }
  }

  sm_rigToUberrigMapsMemReqs = NMP::Memory::Format(sizeof(uint32_t*) * animLibraryExport->getNumAnimationSets() + sizeof(uint32_t) * numBones, NMP_NATURAL_TYPE_ALIGNMENT);
  result += sm_rigToUberrigMapsMemReqs;
  sm_uberrigToRigMapsMemReqs = NMP::Memory::Format(sizeof(uint32_t*) * animLibraryExport->getNumAnimationSets() + sizeof(uint32_t) * animLibraryExport->getNumAnimationSets() * std::distance(differentBoneNames.begin(), differentBoneNames.end()), NMP_NATURAL_TYPE_ALIGNMENT);
  result += sm_uberrigToRigMapsMemReqs;

  // Retarget solver storage stats
  if (hasRetargetMapping)
  {
    result += MR::AttribDataRetargetStorageStats::getMemoryRequirements();
  }

  // Memory required by plugins.
  std::vector<NetworkDefBuilderPlugin*>::iterator iter;
  for (iter = sm_networkPlugins.begin(); iter != sm_networkPlugins.end(); iter++)
  {
    NMP::Memory::Format pluginResult = (*iter)->getPluginMemoryRequirements(netDefExport);
    if(pluginResult.size > 0)
    {
      result += pluginResult;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildRetargetingIndexMapping(
  const NetworkDefBuilderEx* networkNodeDef,
  const ME::AnimationLibraryExport* animLibraryExport,
  std::map<std::string, int32_t>& retargetGroupMap)
{
  uint32_t maxRetargetGroup = 0;
  std::string tempMapString;
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < networkNodeDef->m_numAnimSets; ++animSetIndex)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);

    // Find the animation rig (there must be one).
    const ME::RigExport* animRigExport = animSetExport->getRig();
    NMP_VERIFY_MSG(
      animRigExport != 0,
      "AnimSet %s does not contain a Rig",
      animSetExport->getName());

    uint32_t jointCount = animRigExport->getNumJoints();
    for (uint32_t jointIndex = 0; jointIndex < jointCount; ++jointIndex)
    {
      const ME::JointExport* jointExport = animRigExport->getJoint(jointIndex);
      NMP_VERIFY(jointExport);

      const char* tag = jointExport->getRetargetingTag();

      if (tag)
      {
        tempMapString = tag;
        if (!tempMapString.empty())
        {
          std::map<std::string, int32_t>::iterator it = retargetGroupMap.find(tempMapString);
          if (it == retargetGroupMap.end())
          {
            retargetGroupMap[tempMapString] = maxRetargetGroup++;
          }
        }
      }
    }
  }
  NMP_ASSERT(retargetGroupMap.size() == maxRetargetGroup);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildNetworkNodeDef(
  AP::AssetProcessor*         processor,
  const ME::NetworkDefExport* netDefExport)
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //-----------------
  // Allocate the memory for the network definition excluding the node definition data
  NMP_VERIFY(!sm_networkDefCompilationInfo->isNodeDefCompiled(0));
  NMP::Memory::Format nodeDefMemReqs = getMemoryRequirementsNetworkNodeDef(netDefExport);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(nodeDefMemReqs);

  // Set the node def compilation information
  sm_networkDefCompilationInfo->setNodeDef(
    MR::NETWORK_NODE_ID,
    nodeDefMemReqs,
    (MR::NodeDef*)memRes.ptr);

  //-----------------
  // Header information
  NetworkDefBuilderEx* networkNodeDef = (NetworkDefBuilderEx*)memRes.ptr;
  memRes.increment(NMP::Memory::Format(sizeof(MR::NetworkDef), NMP_NATURAL_TYPE_ALIGNMENT));
  uint16_t numNetDefAttribDataItems = calcNumNetworkDefAttribDatas(netDefExport);
  MR::NodeDef::initExcludeBaseMem(
    networkNodeDef,
    memRes,
    1,                                          // numChildren
    0,                                          // numInputCPConnections
    numNetDefAttribDataItems,                   // numAttribDatas
    1,                                          // max num active child nodes
    0,                                          // numOutputCPPins
    0,                                          // numOnEnterEvents
    sm_networkDefCompilationInfo->getNodeDataPinInfo(0));

  // NodeDef
  networkNodeDef->m_nodeTypeID = NODE_TYPE_NETWORK;  // Specified in mSharedDefines.h.
  networkNodeDef->m_nodeID = MR::NETWORK_NODE_ID;
  networkNodeDef->m_parentNodeID = MR::INVALID_NODE_ID;
  networkNodeDef->m_numNodes = sm_networkDefCompilationInfo->getNumNodes();
  networkNodeDef->m_numAnimSets = (MR::AnimSetIndex)sm_networkDefCompilationInfo->getNumAnimSets();
  networkNodeDef->m_numMessageDistributors = sm_networkDefCompilationInfo->getNumMessages();
  networkNodeDef->m_owningNetworkDef = networkNodeDef;

  // NetworkDef
  networkNodeDef->m_nodes = NULL;
  networkNodeDef->m_emittedControlParamsInfo = NULL;
  networkNodeDef->m_stateMachineNodeIDs = NULL;
  networkNodeDef->m_messageEmitterNodeIDs = NULL;
  networkNodeDef->m_stateMachineStateIDStringTable = NULL;
  networkNodeDef->m_nodeIDNamesTable = NULL;
  networkNodeDef->m_messageIDNamesTable = NULL;
  networkNodeDef->m_eventTrackIDNamesTable = NULL;
  networkNodeDef->m_taskQueuingFnTables = NULL;
  networkNodeDef->m_outputCPTaskFnTables = NULL;
  networkNodeDef->m_semanticLookupTable = NULL;
  networkNodeDef->m_numSemanticLookupTables = 0;
  networkNodeDef->m_semanticLookupTables = NULL;
  networkNodeDef->m_maxBonesInAnimSets = 0;
  networkNodeDef->m_numNetworkInputControlParameters = 0;

  networkNodeDef->m_messageDistributors = NULL;
  networkNodeDef->m_tagTable = NULL;

  networkNodeDef->m_canCombineTrajectoryTransformTasks = true;

  //-----------------
  // Child node IDs
  networkNodeDef->m_childNodeIDs[0] = (MR::NodeID)netDefExport->getRootNodeID();
 
  bool hasMirroredMapping = containsNodeOfType(netDefExport, NODE_MIRROR_TRANSFORMS_ID);
  bool hasRetargetMapping = containsNodeOfType(netDefExport, NODE_TYPE_RETARGET);

  // Find the semantic lookup table for the NetworkDef node.
  MR::SemanticLookupTable* netDefSemanticLookupTable = sm_networkDefCompilationInfo->findSemanticLookupTable(NODE_TYPE_NETWORK);
  NMP_VERIFY(netDefSemanticLookupTable);

  //-----------------
  // Find unique indices for each named retarget mapping
  std::map<std::string, int32_t> retargetGroupMap;
  buildRetargetingIndexMapping(networkNodeDef, animLibraryExport, retargetGroupMap);

  //-----------------
  // Add the morpheme core attribute data to the network root node.
  // i.e. rigs, character controllers, output masks, mirror node mappings.
  std::set<std::string> differentBoneNames;
  uint32_t numBones = 0;
  MR::AttribDataHandle* attribDataArray = networkNodeDef->m_nodeAttribDataHandles;
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < networkNodeDef->m_numAnimSets; ++animSetIndex)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);

    // Find the animation rig (there must be one).
    const ME::RigExport* animRigExport = animSetExport->getRig();
    NMP_VERIFY_MSG(
      animRigExport != 0,
      "AnimSet %s does not contain a Rig",
      animSetExport->getName());

    const ProcessedAsset* animRigAsset = processor->findProcessedAsset(animRigExport->getGUID());
    NMP_VERIFY_MSG(
      animRigAsset != 0,
      "AnimSet %s does not contain a Rig",
      animSetExport->getName());

    MR::AnimRigDef* animRigDef = (MR::AnimRigDef*) animRigAsset->assetMemory.ptr;
    NMP_VERIFY_MSG(
      animRigDef != 0,
      "AnimSet %s does not contain a Rig",
      animSetExport->getName());

    uint32_t numChannels = animRigExport->getNumJoints();
    for (uint32_t channelIndex = 0; channelIndex < numChannels; channelIndex++)
    {
      differentBoneNames.insert(animRigExport->getJoint(channelIndex)->getName());
      ++numBones;
    }

    // Create an AttribData to hold a pointer to the animation rig.
    buildAnimRigAttribData(memRes, attribDataArray, numNetDefAttribDataItems, netDefSemanticLookupTable, animSetIndex, animRigDef);

    // Create an AttribData to hold a pointer to the character controller (may be a NULL pointer).
    buildCharacterControllerAttribData(memRes, attribDataArray, numNetDefAttribDataItems, netDefSemanticLookupTable, animSetIndex, animSetExport, processor);
      
    // Default output mask.
    buildDefaultOutputMaskAttribData(memRes, attribDataArray, numNetDefAttribDataItems, netDefSemanticLookupTable, animSetIndex, animRigExport);
      
    // Mirror mapping attributes.
    if (hasMirroredMapping)
    {
      buildMirrorMappingAttribData(
        memRes,
        attribDataArray,
        numNetDefAttribDataItems,
        netDefSemanticLookupTable,
        animSetIndex,
        animSetExport,
        animRigExport);
    }

    // Joint limits
    if (animRigExport->getNumJointLimits() > 0)
    {
      buildJointLimitsAttribData(memRes, attribDataArray, numNetDefAttribDataItems, netDefSemanticLookupTable, animSetIndex, animRigExport);
    }

    // Rig retarget mapping attributes
    if (hasRetargetMapping)
    {
      buildRigRetargetMappingAttribData(
        memRes,
        animSetExport,
        attribDataArray,
        numNetDefAttribDataItems,
        netDefSemanticLookupTable,
        animRigExport,
        animSetIndex,
        retargetGroupMap);
    }
  }

  sm_rigToUberrigMaps = (uint32_t**)memRes.alignAndIncrement(NMP::Memory::Format(sizeof(uint32_t*) * animLibraryExport->getNumAnimationSets(), NMP_NATURAL_TYPE_ALIGNMENT));
  for (uint32_t animSetIndex = 0; animSetIndex < animLibraryExport->getNumAnimationSets(); ++animSetIndex)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);

    // Find the animation rig (there must be one).
    const ME::RigExport* animRigExport = animSetExport->getRig();

    uint32_t numChannels = animRigExport->getNumJoints();

    sm_rigToUberrigMaps[animSetIndex] = (uint32_t*)memRes.alignAndIncrement(NMP::Memory::Format(sizeof(uint32_t) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT));
    for (uint32_t channelIndex = 0; channelIndex < numChannels; ++channelIndex)
    {
      sm_rigToUberrigMaps[animSetIndex][channelIndex] = (uint32_t)std::distance(differentBoneNames.begin(), differentBoneNames.find(animRigExport->getJoint(channelIndex)->getName()));
    }
  }

  sm_uberrigToRigMaps = (uint32_t**)memRes.alignAndIncrement(NMP::Memory::Format(sizeof(uint32_t*) * animLibraryExport->getNumAnimationSets(), NMP_NATURAL_TYPE_ALIGNMENT));
  uint32_t numUberchannels = (uint32_t)std::distance(differentBoneNames.begin(), differentBoneNames.end());
  for (uint32_t animSetIndex = 0; animSetIndex < animLibraryExport->getNumAnimationSets(); ++animSetIndex)
  {
    sm_uberrigToRigMaps[animSetIndex] = (uint32_t*)memRes.alignAndIncrement(NMP::Memory::Format(sizeof(uint32_t) * numUberchannels, NMP_NATURAL_TYPE_ALIGNMENT));

    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);

    // Find the animation rig (there must be one).
    const ME::RigExport* animRigExport = animSetExport->getRig();

    uint32_t numChannels = animRigExport->getNumJoints();

    std::set<std::string>::iterator it = differentBoneNames.begin();
    for (uint32_t uberchannelIndex = 0; uberchannelIndex < numUberchannels; ++uberchannelIndex, ++it)
    {
      sm_uberrigToRigMaps[animSetIndex][uberchannelIndex] = 0;
      for (uint32_t channelIndex = 0; channelIndex < numChannels; ++channelIndex)
      {
        if (*it == animRigExport->getJoint(channelIndex)->getName())
        {
          sm_uberrigToRigMaps[animSetIndex][uberchannelIndex] = channelIndex;
          break;
        }
      }
    }
  }

  for (uint32_t animSetIndex = 0; animSetIndex < animLibraryExport->getNumAnimationSets(); ++animSetIndex)
  {
    ((intptr_t&)sm_rigToUberrigMaps[animSetIndex]) -= (intptr_t)sm_rigToUberrigMaps;
    ((intptr_t&)sm_uberrigToRigMaps[animSetIndex]) -= (intptr_t)sm_uberrigToRigMaps;
  }

  //-----------------
  // Retarget storage stats
  // NOTE This has to go after creation of the rigs and retarget mappings, since it uses that data.
  if (hasRetargetMapping)
  {
    buildRetargetStorageStatsAttribData(
      memRes, animLibraryExport, attribDataArray, numNetDefAttribDataItems, netDefSemanticLookupTable);
  }

  //-----------------
  // Add the plug-in attribute data to the network root node.
  std::vector<NetworkDefBuilderPlugin*>::iterator iter;
  for (iter = sm_networkPlugins.begin(); iter != sm_networkPlugins.end(); iter++)
  {
    (*iter)->initPluginData(memRes, attribDataArray, numNetDefAttribDataItems, netDefSemanticLookupTable, processor, netDefExport);
  }


  //---------------------------
  // Initialise the task function tables for the nodeDef section of the network definition.
  initTaskQueuingFns(networkNodeDef, sm_networkDefCompilationInfo, processor->getMessageLogger());

  //-----------------
  // Make sure the NodeDef used all of the memory it asked for.
  if (memRes.format.size != 0)
  {
    char message[256];
    NMP_SPRINTF(
      message,
      "The network root nodeDef did not use all the memory it requested. %i bytes were left\n",
      memRes.format.size);
    processor->getErrorLogger()->output(message);
  }

  //-----------------
  // Dislocate the node def section of the network definition
  NMP_VERIFY(sm_networkDefCompilationInfo->isNodeDefCompiled(MR::NETWORK_NODE_ID));
  MR::NodeDef* nodeDef = sm_networkDefCompilationInfo->getNodeDef(MR::NETWORK_NODE_ID);
  nodeDef->dislocate();
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::copySemanticLookupTables(
  NetworkDefBuilderEx*    networkDef,
  NMP::Memory::Resource&  memRes)
{
  uint32_t numSemanticLookupTables = sm_networkDefCompilationInfo->getSemanticLookupTablesCount();
  NMP_VERIFY(numSemanticLookupTables > 0);

  // Array of tables.
  networkDef->m_numSemanticLookupTables = numSemanticLookupTables;
  networkDef->m_semanticLookupTables = MR::SemanticLookupTable::initSemanticLookupTables(
    memRes,
    numSemanticLookupTables);

  // Individual NodeType tables.
  for (uint32_t i = 0; i < numSemanticLookupTables; ++i)
  {
    networkDef->m_semanticLookupTables[i] = MR::SemanticLookupTable::initFrom(
      memRes,
      sm_networkDefCompilationInfo->getSemanticLookupTables()[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildControlParameterNodeDefs(
  AP::AssetProcessor*         processor,
  const ME::NetworkDefExport* netDefExport)
{
  NMP_VERIFY(!sm_emittedControlParamsInfo);
  uint32_t numNodes = netDefExport->getNumNodes();

  std::vector<uint32_t> emittedControlParamNodeIDs;
  
  //-------------------------
  // Build all control parameter node definitions
  for (uint32_t i = 1; i < numNodes; ++i)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    ME::DataBlockExport* dataBlock = const_cast<ME::DataBlockExport*>(nodeDefExport->getDataBlock());
    NodeDefDependency& nodeDep = sm_networkDefCompilationInfo->getNodeDefDependency(i);

    if (!sm_networkDefCompilationInfo->isNodeDefCompiled(i) &&
        nodeDep.m_isControlParamType)
    {
      // Build the node def
      buildNodeDef(i, netDefExport, processor);

      //-------------------------
      // Check if the node is an emitted control parameter
      bool isEmittedControlParameter = false;
      dataBlock->readBool(isEmittedControlParameter, "IsEmittedControlParameter");

      // Don't want to force update of an emitted control param that has no child connections.
      uint32_t inputPathsCount = 0;
      dataBlock->readUInt(inputPathsCount, "InputPathCount");

      if (isEmittedControlParameter && (inputPathsCount > 0))
      {
        emittedControlParamNodeIDs.push_back(i);
        NMP_VERIFY(sm_networkDefCompilationInfo->getNodeDef(i)->getNumOutputCPPins() == 1);
      }

      //-------------------------
      // Dislocate the node def
      MR::NodeDef* nodeDef = sm_networkDefCompilationInfo->getNodeDef(i);
      nodeDef->dislocate();
    }
  }

  //-------------------------
  // Build the emitted control parameters data
  uint32_t numEmittedControlParams = (uint32_t)emittedControlParamNodeIDs.size();
  NMP::Memory::Format memReqs = MR::EmittedControlParamsInfo::getMemoryRequirements(numEmittedControlParams);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  sm_emittedControlParamsInfo = MR::EmittedControlParamsInfo::init(memRes, numEmittedControlParams);

  for (uint32_t i = 0; i < numEmittedControlParams; ++i)
  {
    uint32_t nodeID = emittedControlParamNodeIDs[i];
    sm_emittedControlParamsInfo->setEmittedControlParamData(i, (MR::NodeID)nodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildAnimSourceNodeDefs(
  AP::AssetProcessor*         processor,
  const ME::NetworkDefExport* netDefExport,
  MR::NodeID                  rootNodeID)
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint32_t numNodes = netDefExport->getNumNodes();

  //-------------------------
  // Compile all animations and rig to anim maps for this Network Def.
  initCompileAnimations(animLibraryExport, processor);

  //-------------------------
  // Compile all animation source node definitions in the dependency graph order
  bool* evalNodeDefDeps = new bool[numNodes];
  for (uint32_t i = 0; i < numNodes; ++i)
    evalNodeDefDeps[i] = false;

  NodeDefDependency& nodeDefDep = sm_networkDefCompilationInfo->getNodeDefDependency(rootNodeID);
  buildAnimNodeDefs(
    &nodeDefDep,
    evalNodeDefDeps,
    processor,
    netDefExport);

  // Tidy up
  delete[] evalNodeDefDeps;

  //-------------------------
  // Build the event track name to runtime id map. This updates the event track runtime ids
  // within the markup data.
  NMP_VERIFY(!sm_eventTrackIDNamesTable);
  sm_eventTrackIDNamesTable = buildEventTrackNameToRuntimeIDMap(processor, netDefExport);

  //-------------------------
  // Dislocate the compiled animation source nodes
  for (uint32_t nodeIdx = 0; nodeIdx < numNodes; nodeIdx++)
  {
    MR::NodeDef* nodeDef = sm_networkDefCompilationInfo->getNodeDef(nodeIdx);
    if (nodeDef && nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_ANIM_SOURCE)
    {
      nodeDef->dislocate();
    }
  }

  //-------------------------
  // Tidy up
  tidyCompileAnimations();
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildNetworkDefInfo(
  AP::AssetProcessor* processor,
  const ME::NetworkDefExport* netDefExport)
{
  // State machine Node IDs array
  NMP_VERIFY(!sm_stateMachineNodeIDs);
  sm_stateMachineNodeIDs = buildStateMachineNodeIDsArray(netDefExport);

  // Message Emitter Nodes ID array
  NMP_VERIFY(!sm_messageEmitterNodeIDs);
  sm_messageEmitterNodeIDs = buildMessageEmitterNodeIDsArray(netDefExport);

  // Multiply connected Node IDs array
  NMP_VERIFY(!sm_multiplyConnectedNodeIDs);
  sm_multiplyConnectedNodeIDs = buildMultiplyConnectedNodeIDsArray(netDefExport);

  // Node OnExit Message array
  NMP_VERIFY(!sm_onExitMessageArray);
  sm_onExitMessageArray = buildNodeOnExitMessageArray(netDefExport, &sm_onExitMessageArrayMemReqs);

  // Map of state machine state names to state IDs.
  NMP_VERIFY(!sm_stateMachineStateIDStringTable)
  sm_stateMachineStateIDStringTable = buildStateMachineStateIDStringTable(netDefExport);

  // NodeID to Node name mapping table
  NMP_VERIFY(!sm_nodeIDNamesTable)
  sm_nodeIDNamesTable = buildNodeIDNameMappingTable(netDefExport);

  // MessageID to Message name mapping table
  NMP_VERIFY(!sm_messageIDNamesTable);
  sm_messageIDNamesTable = buildMessageIDNameMappingTable(netDefExport);

  // Build an array of semantic lookup tables, one for each NodeType in the Network. The entry for the NetworkDef node is populated later.
  buildSemanticLookupTables(sm_networkDefCompilationInfo, processor, netDefExport);

  // Build a table of all node tags
  NMP_VERIFY(!sm_nodeTagTable);
  sm_nodeTagTable = buildNodeTagsTable(netDefExport);
  
  // NodeDef header and attribute data for the network definition
  buildNetworkNodeDef(processor, netDefExport);
}

//----------------------------------------------------------------------------------------------------------------------
// Set optimisation flags to speed up runtime tests
void NetworkDefBuilder::setNodeOptimisationFlags(MR::NodeDef* nodeDef)
{
  // Set the NODE_FLAG_NO_PHYSICAL_CHILDREN flag if it's appropriate to do so.  This will speed up runtime checking for
  //  physics nodes.
  bool noPhysicalChildren = true;

  if(nodeDef->getNodeTypeID() == NODE_TYPE_STATE_MACHINE)
  {
    // This node is a state machine, so we can just check on all the steady state children for physics nodes.
    MR::AttribDataStateMachineDef* smDef = 
      nodeDef->getAttribData<MR::AttribDataStateMachineDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
    NMP_ASSERT(smDef);

    for(MR::StateID i = 0; i< smDef->getNumStates(); i++)
    {
      MR::NodeID childNodeID = smDef->getNodeIDFromStateID(i);
      NMP_VERIFY(childNodeID != MR::INVALID_NODE_ID);

      MR::NodeDef* childDef = sm_networkDefCompilationInfo->getNodeDef(childNodeID);
      NMP_VERIFY(childDef);

      if(!(childDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_NO_PHYSICAL_CHILDREN))
      {
        // One of the children has a physics node under it somewhere (or is a physics node itself).
        noPhysicalChildren = false;
        break;
      }
    }

    if(noPhysicalChildren &&
       !(nodeDef->getNodeFlags() & (MR::NodeDef::NODE_FLAG_IS_PHYSICAL | MR::NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER)))
    {
      // The state machine is not physical, and it has no nodes under it which are physical, so we can set the flag
      nodeDef->setNodeFlags(nodeDef->getNodeFlags() | MR::NodeDef::NODE_FLAG_NO_PHYSICAL_CHILDREN);

      // We can now search for any active state transitions in this state machine and set the
      //  NODE_FLAG_NO_PHYSICAL_CHILDREN on them too.
      for(uint32_t i = 0; i< nodeDef->getNumChildNodes(); i++)
      {
        MR::NodeID childNodeID = nodeDef->getChildNodeID(i);
        NMP_VERIFY(childNodeID != MR::INVALID_NODE_ID);

        MR::NodeDef* childDef = sm_networkDefCompilationInfo->getNodeDef(childNodeID);
        NMP_VERIFY(childDef);

        // We have to locate the children before we can do anything with them
        childDef->locate(NULL);
        if ((childDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_TRANSITION) &&
            (childDef->getChildNodeID(0) == MR::INVALID_NODE_ID))
        {
          childDef->setNodeFlags(childDef->getNodeFlags() | MR::NodeDef::NODE_FLAG_NO_PHYSICAL_CHILDREN);
        }
        // We have to dislocate the child once we are done with it.
        childDef->dislocate();

      }
    }
  }
  else if (nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM)
  {
    // We skip control parameters, they don't need the NODE_FLAG_NO_PHYSICAL_CHILDREN.
    return;
  }
  else if ((nodeDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_TRANSITION) &&
    (nodeDef->getChildNodeID(0) == MR::INVALID_NODE_ID))
  {
    // If it's an active state transition, we need to set the NODE_FLAG_NO_PHYSICAL_CHILDREN to be the same as the
    //  state machine (i.e. if any state in the state machine has physics, then potentially so does the active state
    //  transition). We can't copy across the flag when the transit itself is made, as we haven't built the state
    //  machine yet. The state machine will does it for us when it is constructed, so for now we just skip this node.
    return;
  }
  else
  {
    // This is a "standard" node, so we do the normal thing of checking children
    for(uint32_t i = 0; i< nodeDef->getNumChildNodes(); i++)
    {
      MR::NodeID childNodeID = nodeDef->getChildNodeID(i);
      NMP_VERIFY(childNodeID != MR::INVALID_NODE_ID);

      MR::NodeDef* childDef = sm_networkDefCompilationInfo->getNodeDef(childNodeID);
      NMP_VERIFY(childDef);

      if(!(childDef->getNodeFlags() & MR::NodeDef::NODE_FLAG_NO_PHYSICAL_CHILDREN))
      {
        // One of the children has a physics node under it somewhere (or is a physics node itself).
        noPhysicalChildren = false;
        break;
      }
    }
    
    if(noPhysicalChildren && !(nodeDef->getNodeFlags() & (MR::NodeDef::NODE_FLAG_IS_PHYSICAL | MR::NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER)))
    {
      // This node is not physical, and it has no nodes under it which are physical, so we can set the flag
      nodeDef->setNodeFlags(nodeDef->getNodeFlags() | MR::NodeDef::NODE_FLAG_NO_PHYSICAL_CHILDREN);
    }
  }

  return;
}

//----------------------------------------------------------------------------------------------------------------------
// Builds the node def but does not dislocate it
void NetworkDefBuilder::buildNodeDef(
  uint32_t                    nodeID,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY(!sm_networkDefCompilationInfo->isNodeDefCompiled(nodeID));
  const ME::NodeExport* nodeDefExport = netDefExport->getNode(nodeID);

  // Get the node builder
  MR::NodeType nodeType = nodeDefExport->getTypeID();
  AP::NodeDefBuilder* nodeBuilder = processor->getNodeBuilder(nodeType);
  if (!nodeBuilder)
  {
    processor->outputNodeError(nodeType, nodeDefExport->getNodeID(), "Cannot find node builder.");
  }
  NMP_VERIFY_MSG(
    nodeBuilder,
    "Cannot find node builder for node with TypeID %i, perhaps it was not registered",
    nodeType);

  // Pre-initialise the node definition. This is where any sub-network evaluation can be performed.
  nodeBuilder->preInit(
    sm_networkDefCompilationInfo,
    nodeDefExport,
    netDefExport,
    processor);

  // Compute the memory requirements for the node definition.
  NMP::Memory::Format memReqsNodeDef = nodeBuilder->getNodeDefMemoryRequirements(
      sm_networkDefCompilationInfo,
      nodeDefExport,
      netDefExport,
      processor);

  // Allocate the memory for the node definition.
  NMP::Memory::Resource memResNodeDef = NMPMemoryAllocateFromFormat(memReqsNodeDef);

  // Build the node definition
  MR::NodeDef* nodeDef = nodeBuilder->init(
        memResNodeDef,
        sm_networkDefCompilationInfo,
        nodeDefExport,
        netDefExport,
        processor);
  NMP_VERIFY(nodeDef);

  // Set optimisation flags to speed up runtime execution, as appropriate.
  setNodeOptimisationFlags(nodeDef);

  // The parent node ID should always be in the valid node range or the invalid ID.
  NMP_VERIFY_MSG(
    nodeDef->getParentNodeID() < sm_networkDefCompilationInfo->getNumNodes() ||
    nodeDef->getParentNodeID() == MR::INVALID_NODE_ID,
    "Invalid parent node ID for node \"%s.\"",
    netDefExport->getNode(nodeDef->getNodeID())->getName()
    );

  // Set the nodeDef compilation info
  sm_networkDefCompilationInfo->setNodeDef(nodeID, memReqsNodeDef, nodeDef);

  // Make sure the NodeDef used all of the memory it asked for.
  if (memResNodeDef.format.size != 0)
  {
    char message[256];
    NMP_SPRINTF(
      message,
      "Node Def ID %i of type %i did not use all the memory it requested. %i bytes were left\n",
      nodeID,
      nodeType,
      memResNodeDef.format.size);
    processor->getErrorLogger()->output(message);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildAnimNodeDefs(
  NodeDefDependency*          nodeDefDep,
  bool*                       evalNodeDefDeps,
  AP::AssetProcessor*         processor,
  const ME::NetworkDefExport* netDefExport)
{
  NMP_VERIFY(nodeDefDep);
  NMP_VERIFY(evalNodeDefDeps);
  MR::NodeID nodeID = nodeDefDep->m_nodeID;
  if (evalNodeDefDeps[nodeID])
    return;
  evalNodeDefDeps[nodeID] = true;

  // Build the child node dependencies
  std::vector<NodeDefDependency*>::iterator nit;
  for (nit = nodeDefDep->m_childNodeIDs.begin(); nit != nodeDefDep->m_childNodeIDs.end(); ++nit)
  {
    NodeDefDependency* childNodeDefDep = *nit;
    buildAnimNodeDefs(
      childNodeDefDep,
      evalNodeDefDeps,
      processor,
      netDefExport);
  }

  // Build the anim node def
  const ME::NodeExport* nodeDefExport = netDefExport->getNode(nodeID);
  MR::NodeType nodeType = nodeDefExport->getTypeID();
  if (nodeType == NODE_TYPE_ANIM_EVENTS && !sm_networkDefCompilationInfo->isNodeDefCompiled(nodeID))
  {
    // Build the node def, but don't dislocate it
    buildNodeDef(nodeID, netDefExport, processor);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildNodeDefs(
  NodeDefDependency*          nodeDefDep,
  bool*                       evalNodeDefDeps,
  AP::AssetProcessor*         processor,
  const ME::NetworkDefExport* netDefExport)
{
  NMP_VERIFY(nodeDefDep);
  NMP_VERIFY(evalNodeDefDeps);
  MR::NodeID nodeID = nodeDefDep->m_nodeID;
  if (evalNodeDefDeps[nodeID])
    return;
  evalNodeDefDeps[nodeID] = true;

  // If this node is an emitted control parameter, we should not continue traversing down it's connected pins, as we
  //  will have wrapped around to the root of the network, and we could hit a circular dependency.
  bool isEmittedControlParameter = false;
  ME::DataBlockExport* dataBlock = const_cast<ME::DataBlockExport*>(netDefExport->getNode(nodeID)->getDataBlock());
  NMP_VERIFY_MSG(dataBlock, "Could not extract node data block while traversing node dependency graph!");
  dataBlock->readBool(isEmittedControlParameter, "IsEmittedControlParameter");
  if(isEmittedControlParameter)
  {
    // NB: The operator nodes connected to this output control parameter node will still be built, just later on in the
    //  root buildNodeDefs function.
    return;
  }

  // Build the child node dependencies
  std::vector<NodeDefDependency*>::iterator nit;
  for (nit = nodeDefDep->m_childNodeIDs.begin(); nit != nodeDefDep->m_childNodeIDs.end(); ++nit)
  {
    NodeDefDependency* childNodeDefDep = *nit;
    buildNodeDefs(
      childNodeDefDep,
      evalNodeDefDeps,
      processor,
      netDefExport);
  }

  // Build the node def
  if (!sm_networkDefCompilationInfo->isNodeDefCompiled(nodeID))
  {
    buildNodeDef(nodeID, netDefExport, processor);

    // Dislocate the node def
    MR::NodeDef* nodeDef = sm_networkDefCompilationInfo->getNodeDef(nodeID);
    nodeDef->dislocate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildNodeDefs(
  AP::AssetProcessor*         processor,
  const ME::NetworkDefExport* netDefExport,
  MR::NodeID                  rootNodeID)
{
  //---------------------------
  // Compile all node definitions in the dependency graph order
  uint32_t numNodes = sm_networkDefCompilationInfo->getNumNodes();
  bool* evalNodeDefDeps = new bool[numNodes];
  for (uint32_t i = 0; i < numNodes; ++i)
    evalNodeDefDeps[i] = false;

  NodeDefDependency& nodeDefDep = sm_networkDefCompilationInfo->getNodeDefDependency(rootNodeID);
  buildNodeDefs(
    &nodeDefDep,
    evalNodeDefDeps,
    processor,
    netDefExport);

  // Tidy up
  delete[] evalNodeDefDeps;

  //---------------------------
  // Make a last pass to compile any remaining nodes
  if (rootNodeID == MR::NETWORK_NODE_ID)
  {
    for (uint32_t i = 1; i < numNodes; ++i)
    {
      if (!sm_networkDefCompilationInfo->isNodeDefCompiled(i))
      {
        // Build the node def
        buildNodeDef(i, netDefExport, processor);

        // Dislocate the node def
        MR::NodeDef* nodeDef = sm_networkDefCompilationInfo->getNodeDef(i);
        nodeDef->dislocate();
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool isDescendedFrom(MR::NodeDef* node, MR::NodeDef* ancestorToCheck)
{
  MR::NodeDef* ancestor = node->getParentNodeDef();
  while (ancestor)
  {
    if (ancestor == ancestorToCheck)
    {
      return true;
    }
    ancestor = ancestor->getParentNodeDef();
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// When a multiply connected pass down pin is used the pin source will be a child in all pin destinations.  A recursive
// check would end up checking the pass down pin source for every node it is connected too.  This can lead to a huge
// amount of non-unique descendants in the hierarchy making a recursive search unsuitable.  In the worst case it is
// significantly faster to search each node once by iterating over all network nodes.
bool checkChildrenForFlag(MR::NodeDef* parentNode, uint16_t flag, MR::NetworkDef* netDef)
{
  for (uint32_t i = 0, nodeCount = netDef->getNumNodeDefs(); i < nodeCount; ++i)
  {
    MR::NodeDef* node = netDef->getNodeDef((MR::NodeID)i);
    // node may be null if this function is run on a network def that has only been partially built.
    if (node && node->getNodeFlags().areSet(flag) && isDescendedFrom(node, parentNode))
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildMessageDistributors(
  AP::AssetProcessor*         processor,
  const ME::NetworkDefExport* netDefExport)
{
  const ME::MessagePresetLibraryExport* presetsLibraryExport = netDefExport->getMessagePresetLibrary();

  nmtl::pod_vector<MR::NodeID> interestedNodes;

  uint32_t numMessages = netDefExport->getNumMessages();
  for (uint32_t i = 0; i != numMessages; ++i)
  {
    const ME::MessageExport* exportMessage = netDefExport->getMessage(i);

    MR::MessageID messageID = exportMessage->getMessageID();
    MR::MessageType messageType = exportMessage->getMessageType();
    const ME::MessagePresetsListExport* messagePresets = 0;

    sm_networkDefCompilationInfo->getInterestedNodesForMessage(messageID, &interestedNodes);
    if (presetsLibraryExport)
    {
      uint32_t numPresets = presetsLibraryExport->getNumMessages();
      for (uint32_t j = 0; j != numPresets; ++j)
      {
        const ME::MessagePresetsListExport* presets = presetsLibraryExport->getMessagePresets(j);
        if (presets->getMessageType() == messageType)
        {
          messagePresets = presets;
        }
      }
    }

    //----------------------
    // Calculate the required space for the message distributor
    NMP::Memory::Format memReqsDistributor =
      MessageDistributorBuilder::getMemoryRequirements(exportMessage, messagePresets, interestedNodes, processor);

    //----------------------
    // Allocate the memory for the message distributor
    NMP::Memory::Resource memResDistributor =
      NMPMemoryAllocateFromFormat(memReqsDistributor);

    //----------------------
    // Build the message distributor
    MR::MessageDistributor* messageDistributor = MessageDistributorBuilder::init(
      memResDistributor,
      exportMessage,
      messagePresets,
      interestedNodes,
      processor);

    sm_networkDefCompilationInfo->setMessageDistributor(messageID, memReqsDistributor, messageDistributor);

    //----------------------
    // dislocate so this distributor can be safely copied
    messageDistributor->dislocate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource NetworkDefBuilder::buildSubNetworkDef(
  MR::NodeID rootNodeID,
  bool enableCombineTrajectoryTransformTasks)
{
  // Build a list of node dependencies based on the root node is.
  std::vector<MR::NodeID> nodeDefDeps;
  computeNodeDefDependencies(rootNodeID, nodeDefDeps);

  //---------------------------
  // Info.
  uint32_t numNodes = sm_networkDefCompilationInfo->getNumNodes();
  uint32_t numMessages = sm_networkDefCompilationInfo->getNumMessages();

  //---------------------------
  // Allocate the required memory for the complete network definition.
  NMP::Memory::Format memReqs = sm_networkDefCompilationInfo->getNodeDefMemReqs(MR::NETWORK_NODE_ID);

  //---------------------------
  // Add memory for the node type semantic lookup tables. 
  memReqs += sm_networkDefCompilationInfo->getSemanticLookupTablesMemReqs();

  //---------------------------
  // NodeDef pointers table.
  NMP::Memory::Format memReqsNodeDefPtrTable;
  memReqsNodeDefPtrTable = NMP::Memory::Format(sizeof(MR::NodeDef*) * numNodes, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsNodeDefPtrTable.align();
  memReqs += memReqsNodeDefPtrTable;

  //---------------------------
  // MessageDistributor pointers array.
  NMP::Memory::Format memReqsDistributorsArray;
  memReqsDistributorsArray = NMP::Memory::Format(sizeof(MR::MessageDistributor*) * numMessages, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqsDistributorsArray.align();
  memReqs += memReqsDistributorsArray;

  //---------------------------
  // Emitted control parameter Node IDs.
  NMP::Memory::Format memReqsOutputCtrlParams;
  if (sm_emittedControlParamsInfo)
  {
    memReqsOutputCtrlParams = sm_emittedControlParamsInfo->getInstanceMemoryRequirements();
    memReqs += memReqsOutputCtrlParams;
  }

  //---------------------------
  // State machine Node IDs array.
  NMP::Memory::Format memReqsStateMachineNodeIDs;
  if (sm_stateMachineNodeIDs)
  {
    memReqsStateMachineNodeIDs = sm_stateMachineNodeIDs->getInstanceMemoryRequirements();
    memReqs += memReqsStateMachineNodeIDs;
  }

  //---------------------------
  // Message Emitter Nodes ID array.
  NMP::Memory::Format memReqsMessageEmitterNodeIDs;
  if (sm_messageEmitterNodeIDs)
  {
    memReqsMessageEmitterNodeIDs = sm_messageEmitterNodeIDs->getInstanceMemoryRequirements();
    memReqs += memReqsMessageEmitterNodeIDs;
  }

  //---------------------------
  // Multiply connected Node IDs array.
  NMP::Memory::Format memReqsMultiplyConnectedNodeIDs;
  if (sm_multiplyConnectedNodeIDs)
  {
    memReqsMultiplyConnectedNodeIDs = sm_multiplyConnectedNodeIDs->getInstanceMemoryRequirements();
    memReqs += memReqsMultiplyConnectedNodeIDs;
  }

  //---------------------------
  // Node OnExit Message Array.
  NMP::Memory::Format memReqsOnExitMessageArray;
  if (sm_onExitMessageArray)
  {
    memReqsOnExitMessageArray = sm_onExitMessageArrayMemReqs;
    memReqs += memReqsOnExitMessageArray;
  }

  //---------------------------
  // State machine state to state ID.
  NMP::Memory::Format memReqsStateMachineStateStringTable;
  if (sm_stateMachineStateIDStringTable)
  {
    memReqsStateMachineStateStringTable = sm_stateMachineStateIDStringTable->getInstanceMemoryRequirements();
    memReqs += memReqsStateMachineStateStringTable;
  }
  
  //---------------------------
  // NodeID to Node name mapping table.
  NMP::Memory::Format memReqsNodeIDNamesTable;
  if (sm_nodeIDNamesTable)
  {
    memReqsNodeIDNamesTable = sm_nodeIDNamesTable->getInstanceMemoryRequirements();
    memReqs += memReqsNodeIDNamesTable;
  }

  //---------------------------
  // MessageID to Message name mapping table.
  NMP::Memory::Format memReqsMessageIDNamesTable;
  if (sm_messageIDNamesTable)
  {
    memReqsMessageIDNamesTable = sm_messageIDNamesTable->getInstanceMemoryRequirements();
    memReqs += memReqsMessageIDNamesTable;
  }

  //---------------------------
  // Mapping table between event track names and runtime IDs.
  NMP::Memory::Format memReqsEventTrackIDNamesTable;
  if (sm_eventTrackIDNamesTable)
  {
    memReqsEventTrackIDNamesTable = sm_eventTrackIDNamesTable->getInstanceMemoryRequirements();
    memReqs += memReqsEventTrackIDNamesTable;
  }

  //---------------------------
  // Shared task function tables.
  MR::SharedTaskFnTables* taskQueuingFnTables = sm_networkDefCompilationInfo->getTaskQueuingFnTables();
  NMP::Memory::Format memReqsTaskQueuingFnTables = MR::SharedTaskFnTables::getMemoryRequirements(
        taskQueuingFnTables->getNumTaskFnTables());
  memReqs += memReqsTaskQueuingFnTables;

  MR::SharedTaskFnTables* outputCPTaskFnTables = sm_networkDefCompilationInfo->getOutputCPTaskFnTables();
  NMP::Memory::Format memReqsOutputCPTaskFnTables = MR::SharedTaskFnTables::getMemoryRequirements(
        outputCPTaskFnTables->getNumTaskFnTables());
  memReqs += memReqsOutputCPTaskFnTables;

  //---------------------------
  // Node definitions.
  std::vector<MR::NodeID>::const_iterator nit;
  for (nit = nodeDefDeps.begin(); nit != nodeDefDeps.end(); ++nit)
  {
    MR::NodeID nodeID = *nit;
    memReqs += sm_networkDefCompilationInfo->getNodeDefMemReqs(nodeID);
  }

  //---------------------------
  // Message distributors.
  for (uint32_t i = 0; i != numMessages; ++i)
  {
    MR::MessageID messageID = (MR::MessageID)i;

    // take into account potentially sparse message ids.
    if (sm_networkDefCompilationInfo->isMessageDistributorCompiled(messageID))
    {
      memReqs += sm_networkDefCompilationInfo->getMessageDistributorMemReqs(messageID);
    }
  }

  //---------------------------
  // Node tags table
  NMP::Memory::Format nodeTagsTableMemReqs;
  if (sm_nodeTagTable)
  {
    nodeTagsTableMemReqs = sm_nodeTagTable->getInstanceMemoryRequirements();
    memReqs += nodeTagsTableMemReqs;
  }  

  //---------------------------
  // Mappings from and to uberrig.
  memReqs += sm_rigToUberrigMapsMemReqs;
  memReqs += sm_uberrigToRigMapsMemReqs;

  //---------------------------
  // Make sure the size is a multiple of the alignment requirement.
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  //---------------------------
  // Allocate the memory for the network definition.
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  NMP::Memory::Resource result = memRes;

  //---------------------------
  // Assemble the complete network definition from the individual node definitions.

  // Network NodeDef header and attribute data
  NetworkDefBuilderEx* networkDef = (NetworkDefBuilderEx*)memRes.alignAndIncrement(
                                      sm_networkDefCompilationInfo->getNodeDefMemReqs(MR::NETWORK_NODE_ID));
  NMP_VERIFY(networkDef);

  // Copy node def section of the network definition.
  memcpy(
    networkDef,
    sm_networkDefCompilationInfo->getNodeDef(MR::NETWORK_NODE_ID),
    sm_networkDefCompilationInfo->getNodeDefMemReqs(MR::NETWORK_NODE_ID).size);

  //-----------------
  // Initialise the node type semantic lookup tables from the network def compilation info. 
  copySemanticLookupTables(networkDef, memRes);

  //---------------------------
  // NodeDef pointers table.
  networkDef->m_nodes = (MR::NodeDef**)memRes.alignAndIncrement(memReqsNodeDefPtrTable);
  networkDef->m_nodes[0] = networkDef;
  for (uint32_t i = 1; i < numNodes; ++i)
    networkDef->m_nodes[i] = NULL; // Initialise

  //---------------------------
  networkDef->m_messageDistributors = (MR::MessageDistributor**)memRes.alignAndIncrement(memReqsDistributorsArray);
  for (uint32_t i = 0; i < numMessages; ++i)
    networkDef->m_messageDistributors[i] = NULL; // Initialise

  //---------------------------
  // Emitted Control Parameter NodeIDs.
  if (sm_emittedControlParamsInfo)
  {
    networkDef->m_emittedControlParamsInfo = (MR::EmittedControlParamsInfo*)memRes.alignAndIncrement(memReqsOutputCtrlParams);
    memcpy(
      networkDef->m_emittedControlParamsInfo,
      sm_emittedControlParamsInfo,
      memReqsOutputCtrlParams.size);
    networkDef->m_emittedControlParamsInfo->relocate();
  }

  //---------------------------
  // State machine Node IDs array.
  if (sm_stateMachineNodeIDs)
  {
    networkDef->m_stateMachineNodeIDs = (MR::NodeIDsArray*)memRes.alignAndIncrement(memReqsStateMachineNodeIDs);
    memcpy(
      networkDef->m_stateMachineNodeIDs,
      sm_stateMachineNodeIDs,
      memReqsStateMachineNodeIDs.size);
    networkDef->m_stateMachineNodeIDs->relocate();
  }

  //---------------------------
  // Message Emitter Nodes ID array.
  if (sm_messageEmitterNodeIDs)
  {
    networkDef->m_messageEmitterNodeIDs = (MR::NodeIDsArray*)memRes.alignAndIncrement(memReqsMessageEmitterNodeIDs);
    memcpy(
      networkDef->m_messageEmitterNodeIDs,
      sm_messageEmitterNodeIDs,
      memReqsMessageEmitterNodeIDs.size);
    networkDef->m_messageEmitterNodeIDs->relocate();
  }

  //---------------------------
  // Multiply connected Node IDs array.
  if (sm_multiplyConnectedNodeIDs)
  {
    networkDef->m_multiplyConnectedNodeIDs = (MR::NodeIDsArray*)memRes.alignAndIncrement(memReqsMultiplyConnectedNodeIDs);
    memcpy(
      networkDef->m_multiplyConnectedNodeIDs,
      sm_multiplyConnectedNodeIDs,
      memReqsMultiplyConnectedNodeIDs.size);
    networkDef->m_multiplyConnectedNodeIDs->relocate();
  }

  //---------------------------
  // Node OnExit Message array
  if (sm_onExitMessageArray)
  {
    networkDef->m_nodeEventOnExitMessages = (MR::NetworkDef::NodeEventOnExitMessage*)memRes.alignAndIncrement(memReqsOnExitMessageArray);
    memcpy(
      networkDef->m_nodeEventOnExitMessages,
      sm_onExitMessageArray,
      memReqsOnExitMessageArray.size);

    networkDef->m_numNodeEventOnExitMessages = (uint32_t)(memReqsOnExitMessageArray.size / sizeof(MR::NetworkDef::NodeEventOnExitMessage));
  }
  else
  {
    networkDef->m_numNodeEventOnExitMessages = 0;
    networkDef->m_nodeEventOnExitMessages = NULL;
  }

  //---------------------------
  // State machine state to state ID.
  if (sm_stateMachineStateIDStringTable)
  {
    networkDef->m_stateMachineStateIDStringTable = (NMP::IDMappedStringTable*)memRes.alignAndIncrement(memReqsStateMachineStateStringTable);
    memcpy(
      networkDef->m_stateMachineStateIDStringTable,
      sm_stateMachineStateIDStringTable,
      memReqsStateMachineStateStringTable.size);
    networkDef->m_stateMachineStateIDStringTable->relocate();
  }

  //---------------------------
  // NodeID to Node name mapping table.
  if (sm_nodeIDNamesTable)
  {
    networkDef->m_nodeIDNamesTable = (NMP::IDMappedStringTable*)memRes.alignAndIncrement(memReqsNodeIDNamesTable);
    memcpy(
      networkDef->m_nodeIDNamesTable,
      sm_nodeIDNamesTable,
      memReqsNodeIDNamesTable.size);
    networkDef->m_nodeIDNamesTable->relocate();
  }

  //---------------------------
  // MessageID to Message name mapping table.
  if (sm_messageIDNamesTable)
  {
    networkDef->m_messageIDNamesTable = (NMP::OrderedStringTable*)memRes.alignAndIncrement(memReqsMessageIDNamesTable);
    memcpy(
      networkDef->m_messageIDNamesTable,
      sm_messageIDNamesTable,
      memReqsMessageIDNamesTable.size);
    networkDef->m_messageIDNamesTable->relocate();
  }

  //---------------------------
  // Mapping table between event track names and runtime IDs.
  if (sm_eventTrackIDNamesTable)
  {
    networkDef->m_eventTrackIDNamesTable = (NMP::OrderedStringTable*)memRes.alignAndIncrement(memReqsEventTrackIDNamesTable);
    memcpy(
      networkDef->m_eventTrackIDNamesTable,
      sm_eventTrackIDNamesTable,
      memReqsEventTrackIDNamesTable.size);
    networkDef->m_eventTrackIDNamesTable->relocate();
  }

  //---------------------------
  // Shared task function tables.
  networkDef->m_taskQueuingFnTables = MR::SharedTaskFnTables::init(
                                        memRes,
                                        taskQueuingFnTables);

  networkDef->m_outputCPTaskFnTables = MR::SharedTaskFnTables::init(
                                          memRes,
                                          outputCPTaskFnTables);

  //---------------------------
  // Locate the node def section of the network def.
  MR::NodeDef* nodeDef = networkDef;
  nodeDef->locate(networkDef);

  //---------------------------
  // Node definitions.
  for (nit = nodeDefDeps.begin(); nit != nodeDefDeps.end(); ++nit)
  {
    MR::NodeID nodeID = *nit;
    NMP_VERIFY(sm_networkDefCompilationInfo->isNodeDefCompiled(nodeID));

    // Copy the nodeDef data.
    nodeDef = (MR::NodeDef*)memRes.alignAndIncrement(sm_networkDefCompilationInfo->getNodeDefMemReqs(nodeID));
    networkDef->m_nodes[nodeID] = nodeDef;

    memcpy(
      networkDef->m_nodes[nodeID],
      sm_networkDefCompilationInfo->getNodeDef(nodeID),
      sm_networkDefCompilationInfo->getNodeDefMemReqs(nodeID).size);

    // Locate the node definition.
    nodeDef->locate(networkDef);
  }

  //---------------------------
  // Message distributors.
  for (uint32_t i = 0; i != numMessages; ++i)
  {
    MR::MessageID messageID = (MR::MessageID)i;

    // take into account potentially sparse message ids.
    if (sm_networkDefCompilationInfo->isMessageDistributorCompiled(messageID))
    {
      MR::MessageDistributor* source = sm_networkDefCompilationInfo->getMessageDistributor(messageID);
      NMP::Memory::Format memReqs = sm_networkDefCompilationInfo->getMessageDistributorMemReqs(messageID);

      MR::MessageDistributor* destination = (MR::MessageDistributor*)memRes.alignAndIncrement(memReqs);
      networkDef->m_messageDistributors[messageID] = destination;

      NMP::Memory::memcpy(destination, source, memReqs.size);

      destination->locate();
    }
  }

  
  if (sm_nodeTagTable)
  {
    networkDef->m_tagTable = (MR::NodeTagTable*)memRes.alignAndIncrement(nodeTagsTableMemReqs);

    // Copy the node tag table
    memcpy(
      networkDef->m_tagTable,
      sm_nodeTagTable,
      nodeTagsTableMemReqs.size);

    networkDef->m_tagTable->relocate();
  }

  networkDef->m_rigToUberrigMaps = (uint32_t**)memRes.alignAndIncrement(sm_rigToUberrigMapsMemReqs);
  NMP::Memory::memcpy(networkDef->m_rigToUberrigMaps, sm_rigToUberrigMaps, sm_rigToUberrigMapsMemReqs.size);
  networkDef->m_uberrigToRigMaps = (uint32_t**)memRes.alignAndIncrement(sm_uberrigToRigMapsMemReqs);
  NMP::Memory::memcpy(networkDef->m_uberrigToRigMaps, sm_uberrigToRigMaps, sm_uberrigToRigMapsMemReqs.size);

  for (uint32_t animSetIndex = 0; animSetIndex < sm_networkDefCompilationInfo->getNumAnimSets(); ++animSetIndex)
  {
    ((intptr_t&)networkDef->m_rigToUberrigMaps[animSetIndex]) += (intptr_t)networkDef->m_rigToUberrigMaps;
    ((intptr_t&)networkDef->m_uberrigToRigMaps[animSetIndex]) += (intptr_t)networkDef->m_uberrigToRigMaps;
  }

  //---------------------------
  // Make sure the size is a multiple of the alignment requirement.
  memRes.align(NMP_VECTOR_ALIGNMENT);

  // Make sure the NetworkDef used all of the memory it asked for.
  NMP_VERIFY_MSG(
    memRes.format.size == 0,
    "Network Def did not use all the memory it requested. %i bytes were left",
    memRes.format.size);

  //---------------------------
  // Update the root node connection.
  if (rootNodeID != MR::NETWORK_NODE_ID)
  {
    networkDef->setChildNodeID(0, rootNodeID);
    MR::NodeDef* rootNodeDef = networkDef->getNodeDef(rootNodeID);
    rootNodeDef->setParentNodeID(MR::NETWORK_NODE_ID);
  }

  //---------------------------
  // Calculate the maximum number of bones required by any animation set
  MR::AnimSetIndex numSets = networkDef->getNumAnimSets();
  for (MR::AnimSetIndex animSet = 0; animSet < numSets; ++animSet)
  {
    MR::AnimRigDef* rigDef = networkDef->getRig(animSet);
    networkDef->m_maxBonesInAnimSets = NMP::maximum(networkDef->m_maxBonesInAnimSets, rigDef->getNumBones());
  }

  //---------------------------
  // Calculate the number of input control parameters and check for physics nodes
  networkDef->m_isPhysical = false;
  for (uint32_t i = 0; i < networkDef->getNumNodeDefs(); ++i)
  {
    MR::NodeDef* nodeDef = networkDef->getNodeDef((MR::NodeID)i);
    if (nodeDef)
    {
      MR::NodeDef::NodeFlags nodeFlags = nodeDef->getNodeFlags();
      if (nodeFlags.areSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
      {
        ++networkDef->m_numNetworkInputControlParameters;
      }
      if (nodeFlags.areSet(MR::NodeDef::NODE_FLAG_IS_PHYSICAL) ||
        nodeFlags.areSet(MR::NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER))
      {
        networkDef->m_isPhysical = true;
    }
  }
  }

  //---------------------------
  // Check to see if trajectory and transform, tasks and queuing can be combined into the single semantic
  // ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER.  A network can combine use this semantic if no nodes in the
  // network have the flag NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS.
  if (enableCombineTrajectoryTransformTasks)
  {
    MR::NodeDef* rootNodeDef = networkDef->getNodeDef(rootNodeID);
    if (rootNodeDef->getNodeFlags().areSet(MR::NodeDef::NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS))
    {
      networkDef->m_canCombineTrajectoryTransformTasks = false;
    }
    else
    {
      networkDef->m_canCombineTrajectoryTransformTasks = !checkChildrenForFlag(
        rootNodeDef,
        MR::NodeDef::NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS,
        networkDef);
    }
  }
  else
  {
    networkDef->m_canCombineTrajectoryTransformTasks = false;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::initCompileAnimations(
  const ME::AnimationLibraryExport* animLibraryExport,
  AssetProcessor* processor)
{
  // Build the vector of information about all the rigs to be used in this AnimationLibrary.
  // i.e. rig filename, pointer to compiled AnimRigDef, rig scale factor, etc.
  buildRigInfoVector(
    processor,
    animLibraryExport,
    sm_rigInfoVector);

  // Compute the vector of rig groups that are trajectory component compatible.
  buildCompatibleRigGroupsVector(
    sm_rigInfoVector,
    sm_compatibleRigGroupsVector);

  // Step through the animations in the anim sets and build the animation compilation info vector.
  // This is essentially a vector of anim entries (keyed by meta data - filename, takename, format, options)
  // that are rig group compatible (trajectory compatible). Each info entry also contains a vector of
  // RigInfo indices determining which rigs can be compiled against the animation.
  // Now the animations are checked against their valid animation sets (written into the node dependency graph)
  // only add animations that are valid for each particular anim set.
  buildAnimCompilationInfoVector(
    processor,
    animLibraryExport,
    sm_rigInfoVector,
    sm_animCompilationVector);

  // Iterate over the anim compilation entries and compute the vector of conglomerated rig bone names
  // compatible with the rig groups for these entries. Once the unique vector of conglomerated rig
  // bone names (w.r.t. rig groups) has been built, also build the super-set animRigDef for these
  // compatible rig groups containing all the conglomerated rig bones.
  buildConglomeratedRigBoneNamesVector(
    sm_animCompilationVector,
    sm_rigInfoVector,
    sm_conglomeratedRigBoneNamesVector);

  // Iterate over the anim compilation entries and compile the animations against the conglomerate
  // rig data. This also builds the corresponding rig to anim channel maps.
  compileAnimationsAndRigToAnimMaps(
    processor,
    animLibraryExport);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::tidyCompileAnimations()
{
  tidyConglomeratedRigBoneNamesVector(sm_conglomeratedRigBoneNamesVector);
  tidyAnimCompilationInfoVector(sm_animCompilationVector);
  tidyCompatibleRigGroupsVector(sm_compatibleRigGroupsVector);
  tidyRigInfoVector(sm_rigInfoVector);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource NetworkDefBuilder::init(
  const ME::NetworkDefExport* netDefExport,
  AP::AssetProcessor*         processor)
{
  NMP::Memory::Resource result;

  //-------------------------
  // Initialise the network def compilation information
  createNetworkDefCompilationInfo(netDefExport, processor);

  //-------------------------
  // Build the node compilation dependency graph
  buildNodeDefDependencyGraph(processor, netDefExport);

  //-------------------------
  // Build tables and information required for the network definition.
  buildNetworkDefInfo(processor, netDefExport);

  //-------------------------
  // Build all the control parameter and emitted control parameter node definitions
  buildControlParameterNodeDefs(processor, netDefExport);

  //-------------------------
  // Compile all source animations, rig to anim maps for this Network Def then
  // build all the anim source node definitions
  buildAnimSourceNodeDefs(processor, netDefExport, MR::NETWORK_NODE_ID);

  //-------------------------
  // Compile all node definitions in the dependency graph order
  buildNodeDefs(processor, netDefExport, MR::NETWORK_NODE_ID);

  //-------------------------
  // Compile all message distributors now all nodes have declared their message interest.
  buildMessageDistributors(processor, netDefExport);

  //-------------------------
  // Build the final network definition by assembling the individual node definitions
  // into a continuous block of memory.
  result = buildSubNetworkDef(MR::NETWORK_NODE_ID, true);

  //-------------------------
  // Tidy up the builders static data
  tidyNetworkNodeDefCompilationInfo();

  //-------------------------
  // Tidy up the network def compilation info
  tidyNetworkDefCompilationInfo();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkDefBuilder::containsNodeOfType(const ME::NetworkDefExport* netDefExport, MR::NodeType nodeTypeID)
{
  NMP_VERIFY(netDefExport);
  for (uint32_t i = 1; i < netDefExport->getNumNodes(); i++)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    if (nodeDefExport->getTypeID() == nodeTypeID)
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint16_t NetworkDefBuilder::calcNumNetworkDefAttribDatas(const ME::NetworkDefExport* netDefExport)
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint16_t numAnimationSets = (uint16_t) animLibraryExport->getNumAnimationSets();

  MR::SemanticLookupTable* netDefSemanticLookupTable = sm_networkDefCompilationInfo->findSemanticLookupTable(NODE_TYPE_NETWORK);
  NMP_VERIFY(netDefSemanticLookupTable);

  return netDefSemanticLookupTable->getNumAttribsPerAnimSet() * numAnimationSets;
}

//----------------------------------------------------------------------------------------------------------------------
uint16_t NetworkDefBuilder::initNetworkDefNodeSemanticLookupTable(
  const ME::NetworkDefExport* netDefExport,
  MR::SemanticLookupTable*    netDefSemanticLookupTable)
{  
  // Here we build up a table of semantics that are added to the NetworkDef Node, from the core and from plugins.
  //  Plugins can anonymously add AttribData to the NetworkDef.
  //  We validate that plugins do not try and add the same semantic more than once.
  NMP_VERIFY(netDefExport && netDefSemanticLookupTable);
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint32_t numAnimationSets = animLibraryExport->getNumAnimationSets(); 

  //----------
  // Compute the required number of attrib data items.
  netDefSemanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_RIG);         // The rig for each anim set.
  netDefSemanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_OUTPUT_MASK); // The default output mask for each anim set.

  //----------
  // Check to see if we need a mirror mapping.
  if (containsNodeOfType(netDefExport, NODE_MIRROR_TRANSFORMS_ID))
  {
    netDefSemanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING);
  }

  //----------
  // The character controller for each anim set.
  netDefSemanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF);

  //----------
  // Semantics for joint limit data.
  for (uint32_t i = 0; i < numAnimationSets; i++)
  {
    ME::AnimationSetExport *animSetExport = const_cast<ME::AnimationSetExport *>(animLibraryExport->getAnimationSet(i));
    ME::RigExport *rigExport = animSetExport->getRig();
    NMP_VERIFY_MSG(rigExport != 0, "AnimSet %s does not contain a Rig", animSetExport->getName());

    if (rigExport->getNumJointLimits() > 0)
    {
      netDefSemanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_JOINT_LIMITS);
      break;
    }
  }

  //----------
  // Semantics for retargeting data.
  if (containsNodeOfType(netDefExport, NODE_TYPE_RETARGET))
  {
    netDefSemanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_RETARGET_STORAGE_STATS);
    netDefSemanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING);
  }

  //----------
  // Semantics from each builder plugin.
  std::vector<NetworkDefBuilderPlugin*>::iterator iter;
  for (iter = sm_networkPlugins.begin(); iter != sm_networkPlugins.end(); iter++)
  {
    (*iter)->populateNetDefSemanticLookupTable(netDefExport, netDefSemanticLookupTable);
  }
  
  return netDefSemanticLookupTable->getNumAttribsPerAnimSet();
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::initTaskQueuingFns(
  MR::NodeDef* nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger* logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  // Set up the node specific queuing functions
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED, FN_NAME(nodeNetworkQueueUpdateCharacterController), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PHYSICS_UPDATED, FN_NAME(nodeNetworkQueueUpdatePhysics), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_ROOT_UPDATED, FN_NAME(nodeNetworkQueueUpdateRoot), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_MERGED_PHYSICS_RIG_TRANSFORM_BUFFER, FN_NAME(nodeNetworkQueueMergePhysicsRigTransformBuffer), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //-------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeNetworkDeleteInstance), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeNetworkFindGeneratingNodeForSemantic), logger);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildAnimRigAttribData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  MR::AnimSetIndex                 animSetIndex,
  MR::AnimRigDef*                  animRigDef)
{
  // Create an AttribData to hold a pointer to the rig.
  MR::AttribDataRig* rigAttribData = MR::AttribDataRig::init(memRes, animRigDef, MR::IS_DEF_ATTRIB_DATA);
  uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_RIG, animSetIndex);
  NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
  netDefAttribDataArray[lookupIndex].set(rigAttribData, MR::AttribDataRig::getMemoryRequirements());
}

//----------------------------------------------------------------------------------------------------------------------
void constructPose(
  NMP::Memory::Resource&            memRes,
  MR::AttribDataHandle*             netDefAttribDataArray,
  uint32_t                          NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*          netDefSemanticLookupTable,
  MR::AnimSetIndex                  animSetIndex,
  AP::AssetProcessor*               processor,
  const ME::AnimationSetExport*     animSetExport,
  const ME::AnimationPoseDefExport* sourcePose,
  MR::AttribDataSemantic            semantic)
{
  uint32_t numChannelsInRig = animSetExport->getRig()->getNumJoints();

  const char* poseSourceAnimFileName = sourcePose->getAnimationFile();
  const char* poseSourceAnimTakeName = sourcePose->getAnimationTake();
  int32_t animFrameIndex = sourcePose->getPoseFrameIndex();

  NMP_VERIFY_MSG(poseSourceAnimFileName && poseSourceAnimTakeName, "Pose does not contain anim file name or take name");

  // Generate the full file name
  char* fullFileName = processor->getDeMacroisePathFunc()(poseSourceAnimFileName);

  // Load the source animation file into an XMD structure.
  XMD::XModel xmdAnim;
#if defined(NMP_ENABLE_ASSERTS) || defined(NM_ENABLE_EXCEPTIONS)
  XMD::XFileError::eType fileResult = 
#endif
    xmdAnim.LoadAnim(fullFileName);
  NMP_VERIFY_MSG(fileResult == XMD::XFileError::Success, "Could not load pose file.");

  // There can be several separate animations/cycles/takes in 1 XMD file so
  // find the take that we want to process within the XMD data.
  acAnimInfo animInfo; // Will be filled in by the following call
#if defined(NMP_ENABLE_ASSERTS) || defined(NM_ENABLE_EXCEPTIONS)
  bool callSuccess = 
#endif
    processor->findTakeInXMDAnimData(poseSourceAnimTakeName, xmdAnim, animInfo);
  NMP_VERIFY_MSG(callSuccess,
                 "Failure to find animation take %s in source XMD data for anim %s",
                 poseSourceAnimTakeName,
                 fullFileName);

  // getDeMacroisePathFunc allocates the string internally, so we deallocate it here as we are done with it.
  NMP::Memory::memFree(fullFileName);

  const ME::RigExport *animRigExport = animSetExport->getRig();
  // calculate the value needed to scale the animation into game world units.
  float animationScale = animRigExport->getRigScaleFactor() / processor->getRuntimeAssetScaleFactor();

  const ProcessedAsset* animRigAsset = processor->findProcessedAsset(animRigExport->getGUID());
  NMP_VERIFY_MSG(animRigAsset, "Could not find anim rig for anim set %i", animSetIndex);
  MR::AnimRigDef* animRigDef = (MR::AnimRigDef*) animRigAsset->assetMemory.ptr;

  // Bones common between the pose source anim and anim rig
  std::vector<XMD::XId> animBoneIDs;
  // Rig bone name ID list.
  std::vector<uint32_t> rigBoneNamesIDsVector;

  XMD::XIndexList                 animatedNodes;
  XMD::XIndexList::const_iterator nodeIt;
  // Find the take that we want to process within the XMD data.
  const XMD::XAnimCycle* xmdCycle = xmdAnim.GetAnimCycle(animInfo.m_TakeIndex);
  if (xmdCycle)
  {
    //---------------------------------
    // First get a list of the bones that are present in both the rigs and the animations.
    // This defines the order of the animation set channels within the compiled animation.
    // NOTE: The sampled node id order from the XMD file may not be in hierarchy order.

    xmdCycle->GetSampledNodes(animatedNodes);

    // Loop through animation bone channels.
    for (nodeIt = animatedNodes.begin(); nodeIt != animatedNodes.end(); ++nodeIt)
    {
      // Get animation bone channel name.
      const XMD::XBase* currentAnimBoneNode = xmdAnim.FindNode(*nodeIt);
      if (currentAnimBoneNode)
      {
        const char* currentAnimBoneName = currentAnimBoneNode->GetName();

        // Check to see if this anim bone name is present in the rig.
        const uint32_t numRigBones = animRigDef->getNumBones();
        for (uint32_t rigBoneIndex = 0; rigBoneIndex < numRigBones; ++rigBoneIndex)
        {
          const char* currentRigBoneName = animRigDef->getBoneName(rigBoneIndex);
          if (strcmp(currentRigBoneName, currentAnimBoneName) == 0)
          {
            // There are some sampled key frames against this bone name in the source animation.
            rigBoneNamesIDsVector.push_back(rigBoneIndex);
            animBoneIDs.push_back(*nodeIt);
          }
        }
      }
    }
  }
  else
  {
    // if the cycle is NULL, then it's actually an XAnimationTake
    XMD::XAnimationTake* take = 0;
    XMD::XList takes;
    xmdAnim.List(takes, XMD::XFn::AnimationTake);
    take = takes[animInfo.m_TakeIndex - xmdAnim.NumAnimCycles()]->HasFn<XMD::XAnimationTake>();

    for (XMD::XU32 i = 0; i < take->GetNumberOfAnimatedNodes(); ++i)
    {
      const XMD::XAnimatedNode* nodeAnim = take->GetNodeAnimation(i);
      NMP_VERIFY(nodeAnim);
      NMP_VERIFY(nodeAnim->GetNode());

      // ignore anything that isn't a bone...
      if (nodeAnim->GetNode()->HasFn<XMD::XBone>())
      {
        const char* currentAnimBoneName = nodeAnim->GetNode()->GetName();

        // Check to see if this anim bone name is present in the rig.
        const uint32_t numRigBones = animRigDef->getNumBones();
        for (uint32_t rigBoneIndex = 0; rigBoneIndex < numRigBones; ++rigBoneIndex)
        {
          const char* currentRigBoneName = animRigDef->getBoneName(rigBoneIndex);
          if (strcmp(currentRigBoneName, currentAnimBoneName) == 0)
          {
            // There are some sampled key frames against this bone name in the source animation.
            rigBoneNamesIDsVector.push_back(rigBoneIndex);
            animBoneIDs.push_back(nodeAnim->GetNode()->GetID());
          }
        }
      }
    }
  }

  // Build a rig to anim map for this pose source animation
  uint32_t numMapEntries = (uint32_t) rigBoneNamesIDsVector.size();
  NMP_VERIFY(numMapEntries > 0);
  NMP::Memory::Format memReqsRigToAnimEntryMap = MR::RigToAnimEntryMap::getMemoryRequirements(numMapEntries);
  NMP::Memory::Resource memResRigToAnimEntryMap = NMPMemoryAllocateFromFormat(memReqsRigToAnimEntryMap);
  NMP_VERIFY(memResRigToAnimEntryMap.ptr);
  ZeroMemory(memResRigToAnimEntryMap.ptr, memReqsRigToAnimEntryMap.size);
  MR::RigToAnimEntryMap* rigToAnimEntryMap = MR::RigToAnimEntryMap::init(memResRigToAnimEntryMap, numMapEntries);

  // Fill in the RTAM
  for (uint32_t mapIndex = 0; mapIndex < numMapEntries; ++mapIndex)
  {
    uint16_t rigChannelIndex = (uint16_t) rigBoneNamesIDsVector[mapIndex];
    uint16_t animChannelIndex = (uint16_t) mapIndex;
    rigToAnimEntryMap->setEntryRigChannelIndex(mapIndex, rigChannelIndex);
    rigToAnimEntryMap->setEntryAnimChannelIndex(mapIndex, animChannelIndex);
  }

  // Sort the rig to anim map so that the rig channels are strictly increasing
  rigToAnimEntryMap->sortByRigChannels();

  // Process the pose source XMD into an uncompressed anim source
  AnimSourceUncompressedOptions animSourceOptions; // Unchanging channel default tolerances (not needed)
  AnimSourceUncompressed* uncompressedSourceAnim = AnimSourceUncompressedBuilderXMD::init(xmdAnim,
                                                                                          animInfo,
                                                                                          animBoneIDs,
                                                                                          animRigDef,
                                                                                          rigToAnimEntryMap,
                                                                                          animationScale,
                                                                                          animSourceOptions);

  // Allocate some space to get the uncompressed anim keys into 
  uint32_t numChannelsInPoseAnim = uncompressedSourceAnim->getChannelSets()->getNumChannelSets();
  NMP::Vector3* sourcePositions = (NMP::Vector3*)NMPMemoryAlloc(sizeof(NMP::Vector3) * numChannelsInPoseAnim);
  NMP_ASSERT(sourcePositions);
  NMP::Quat*    sourceOrientations = (NMP::Quat*)NMPMemoryAlloc(sizeof(NMP::Quat) * numChannelsInPoseAnim);
  NMP_ASSERT(sourceOrientations);

  uncompressedSourceAnim->getChannelSets()->getPosKeysAtFrame(animFrameIndex, sourcePositions);
  uncompressedSourceAnim->getChannelSets()->getQuatKeysAtFrame(animFrameIndex, sourceOrientations);

  // The uncompressed source channels with be in anim-channel order, so we need to rearrange them to rig-channel order
  NMP::Vector3* sortedPositions = (NMP::Vector3*)NMPMemoryAlloc(sizeof(NMP::Vector3) * numChannelsInRig);
  NMP_ASSERT(sortedPositions);
  NMP::Quat*    sortedOrientations = (NMP::Quat*)NMPMemoryAlloc(sizeof(NMP::Quat) * numChannelsInRig);
  NMP_ASSERT(sortedOrientations);

  // The first channel in the rig is always "CharacterWorldSpaceTM" so we set that to the identity
  sortedPositions[0] = NMP::Vector3(NMP::Vector3::InitZero);
  sortedOrientations[0] = NMP::QuatIdentity();

  // Perform the sort into rig order
  for (uint16_t i = 0; i < numChannelsInRig; ++i)
  {
    uint16_t animChannelIndex;
    if (rigToAnimEntryMap->getAnimIndexForRigIndex(i, animChannelIndex))
    {
      sortedPositions[i] = sourcePositions[animChannelIndex];
      sortedOrientations[i] = sourceOrientations[animChannelIndex];
    }
    else
    {
      // The rig channel doesn't exist in the animation so use the rig bind pose.
      NMP_VERIFY_MSG(i < animRigDef->getBindPose()->m_transformBuffer->getLength(),
        "Rig definition doesn't contain the bind poses required to complete pose \"%s\"", poseSourceAnimFileName);
      sortedPositions[i] = *animRigDef->getBindPoseBonePos(i);
      sortedOrientations[i] = *animRigDef->getBindPoseBoneQuat(i);
    }    
  }

  //---------------------
  // Create and initialise the AttribDataTransformBuffer to hold the pose.
  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;
  MR::AttribDataTransformBuffer::getPosQuatMemoryRequirements(numChannelsInRig, buffMemReqs, internalBuffMemReqs);
  MR::AttribDataTransformBuffer* poseAttribData = MR::AttribDataTransformBuffer::initPosQuat(
    memRes,
    internalBuffMemReqs,
    numChannelsInRig,
    MR::IS_DEF_ATTRIB_DATA);

  for (uint32_t i = 0; i < numChannelsInRig; ++i)
  {
    poseAttribData->m_transformBuffer->setChannelQuat(i, sortedOrientations[i]);
    poseAttribData->m_transformBuffer->setChannelPos(i, sortedPositions[i]);
  }
  poseAttribData->m_transformBuffer->calculateFullFlag();

  // Add the attrib data to the Attrib array.
  uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(semantic, animSetIndex);
  NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
  netDefAttribDataArray[lookupIndex].set(poseAttribData, buffMemReqs);


  // Free the temporary buffers we have created.
  NMP::Memory::memFree(sourcePositions);
  NMP::Memory::memFree(sourceOrientations);
  NMP::Memory::memFree(sortedPositions);
  NMP::Memory::memFree(sortedOrientations);
  NMP::Memory::memFree(rigToAnimEntryMap);
  NMP::Memory::memFree(uncompressedSourceAnim);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildCharacterControllerAttribData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  MR::AnimSetIndex                 animSetIndex,
  const ME::AnimationSetExport*    animSetExport,
  AP::AssetProcessor*              processor)
{
  // Find the character controller asset, if there is one.
  const ME::CharacterControllerExport* characterControllerDefExport = animSetExport->getCharacterController();
  MR::CharacterControllerDef* characterControllerDef = NULL;

  NMP_VERIFY_MSG(
    characterControllerDefExport,
    "There is no Character Controller specified for the network anim set %i",
    animSetIndex);

  const ProcessedAsset* characterControllerDefAsset = processor->findProcessedAsset(characterControllerDefExport->getGUID());
  if (characterControllerDefAsset)
    characterControllerDef = (MR::CharacterControllerDef*) characterControllerDefAsset->assetMemory.ptr;

  // Create an AttribData to hold a pointer to the character controller.
  MR::AttribDataCharacterControllerDef* charContrDefAttribData = MR::AttribDataCharacterControllerDef::init(
        memRes,
        characterControllerDef);
  uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF, animSetIndex);
  NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
  netDefAttribDataArray[lookupIndex].set(charContrDefAttribData, MR::AttribDataCharacterControllerDef::getMemoryRequirements());
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildDefaultOutputMaskAttribData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  MR::AnimSetIndex                 animSetIndex,
  const ME::RigExport*             animRigExport)
{
  uint32_t numChannels = animRigExport->getNumJoints();
  MR::AttribDataBoolArray* outputMaskAttribData = MR::AttribDataBoolArray::init(memRes, numChannels, MR::IS_DEF_ATTRIB_DATA);
  memset(outputMaskAttribData->m_values, true, numChannels);

  outputMaskAttribData->refCountIncrease(); // need to increase ref count since this mask can get referenced by other nodes.

  uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_OUTPUT_MASK, animSetIndex);
  NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
  netDefAttribDataArray[lookupIndex].set(outputMaskAttribData, MR::AttribDataBoolArray::getMemoryRequirements(numChannels));
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkDefBuilder::getMirrorMappingAttribDataMemReqs(
  const ME::AnimationSetExport* animSetExport)
{
  // Find the animation rig (there must be one).
  const ME::RigExport* animRigExport = animSetExport->getRig();
  NMP_VERIFY_MSG(
    animRigExport != 0,
    "AnimSet %s does not contain a Rig",
    animSetExport->getName());

  uint32_t numMirroredBones = (uint32_t)animRigExport->getMirrorMappingCount();
  uint32_t numMirroredEvents = animSetExport->getNumEventUserDataMirrorMappings();
  uint32_t numMirroredTracks = animSetExport->getNumEventTrackMirrorMappings();
  uint32_t numBones = animRigExport->getNumJoints();
  return MR::AttribDataMirroredAnimMapping::getMemoryRequirements(
           numMirroredBones,
           numMirroredEvents,
           numMirroredTracks,
           numBones);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkDefBuilder::getJointLimitsAttribDataMemReqs(
  const ME::RigExport* animRigExport)
{
  uint32_t numRigJoints = animRigExport->getNumJoints();
  uint32_t numLimitedJoints = animRigExport->getNumJointLimits();
  return MR::AttribDataJointLimits::getMemoryRequirements(
    numRigJoints,
    numLimitedJoints);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkDefBuilder::getRigRetargetMappingAttribDataMemReqs(
  const ME::AnimationSetExport *animSetExport)
{
  // Find the animation rig (there must be one).
  const ME::RigExport *animRigExport = animSetExport->getRig();
  NMP_VERIFY_MSG(animRigExport != 0, "AnimSet %s does not contain a Rig", animSetExport->getName());

  return MR::AttribDataRigRetargetMapping::getMemoryRequirements(animRigExport->getNumJoints());
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildMirrorMappingAttribData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  MR::AnimSetIndex                 animSetIndex,
  const ME::AnimationSetExport*    animSetExport,
  const ME::RigExport*             animRigExport)
{
  uint32_t numMirroredBones  = (uint32_t)animRigExport->getMirrorMappingCount();
  uint32_t numMirroredEvents = animSetExport->getNumEventUserDataMirrorMappings();
  uint32_t numMirroredTracks = animSetExport->getNumEventTrackMirrorMappings();
  uint32_t numBones          = animRigExport->getNumJoints();

  MR::AttribDataMirroredAnimMapping* mirroredAnimMapAttribData = MR::AttribDataMirroredAnimMapping::init(
        memRes,
        numMirroredBones,
        numMirroredEvents,
        numMirroredTracks,
        numBones,
        MR::IS_DEF_ATTRIB_DATA);
  MirroredAnimMappingBuilder::init(mirroredAnimMapAttribData, animRigExport, animSetExport);

  NMP::Memory::Format mirroredBoneMapMemReqs = MR::AttribDataMirroredAnimMapping::getMemoryRequirements(
        numMirroredBones,
        numMirroredEvents,
        numMirroredTracks,
        numBones);
  uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING, animSetIndex);
  NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
  netDefAttribDataArray[lookupIndex].set(mirroredAnimMapAttribData, mirroredBoneMapMemReqs);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildJointLimitsAttribData(
  NMP::Memory::Resource&           memRes,
  MR::AttribDataHandle*            netDefAttribDataArray,
  uint32_t                         NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*         netDefSemanticLookupTable,
  MR::AnimSetIndex                 animSetIndex,
  const ME::RigExport*             animRigExport)
{
  uint32_t numRigJoints = animRigExport->getNumJoints();
  uint32_t numLimitedJoints = animRigExport->getNumJointLimits();

  MR::AttribDataJointLimits* jointLimitsAttribData = MR::AttribDataJointLimits::init(
    memRes,
    numRigJoints,
    numLimitedJoints,
    MR::IS_DEF_ATTRIB_DATA);
  JointLimitsBuilder::init(jointLimitsAttribData, animRigExport);

  NMP::Memory::Format jointLimitsMemReqs = MR::AttribDataJointLimits::getMemoryRequirements(
    numRigJoints,
    numLimitedJoints);

  uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_JOINT_LIMITS, animSetIndex);
  NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
  netDefAttribDataArray[lookupIndex].set(jointLimitsAttribData, jointLimitsMemReqs);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildRigRetargetMappingAttribData(
  NMP::Memory::Resource&                memRes,
  const ME::AnimationSetExport*         animSetExport,
  MR::AttribDataHandle*                 netDefAttribDataArray,
  uint32_t                              NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*              netDefSemanticLookupTable,
  const ME::RigExport*                  animRigExport,
  MR::AnimSetIndex                      animSetIndex,
  const std::map<std::string, int32_t>& retargetMap)
{
  uint32_t numRigJoints = animRigExport->getNumJoints();

  // Format the provided memory resource for a rig retarget mapping attrib data.
  MR::AttribDataRigRetargetMapping* rigRetargetMapAttribData =
    MR::AttribDataRigRetargetMapping::init(memRes, numRigJoints, MR::IS_DEF_ATTRIB_DATA);

  // Initialise with values from the rig export.
  RigRetargetMappingBuilder::init(rigRetargetMapAttribData, animRigExport, animSetExport, retargetMap);

  // Initialise the entry.
  uint32_t lookupIndex = netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING, animSetIndex);
  NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
  netDefAttribDataArray[lookupIndex].set(rigRetargetMapAttribData, MR::AttribDataRigRetargetMapping::getMemoryRequirements(numRigJoints));
}

//----------------------------------------------------------------------------------------------------------------------
void calculateRetargetMetrics(
  const ME::AnimationLibraryExport* animLibraryExport,
  uint32_t& intermediateRigSize,
  uint32_t& maxRigSize,
  uint32_t& mostMappedJoints,
  uint32_t& longestSequence)
{
  // Initialise
  maxRigSize = 0;
  mostMappedJoints = 0;
  longestSequence = 1;

  // Will store a unique list of retarget tag strings
  std::list<std::string> retargetGroups;

  uint16_t numAnimationSets = (uint16_t)animLibraryExport->getNumAnimationSets();
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimationSets; ++animSetIndex)
  {
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);

    // Find the animation rig (there must be one).
    const ME::RigExport* animRigExport = animSetExport->getRig();
    NMP_VERIFY_MSG(
      animRigExport != 0,
      "AnimSet %s does not contain a Rig",
      animSetExport->getName());

    // Adjust the maximum rig size
    uint32_t numJoints = animRigExport->getNumJoints();
    maxRigSize = NMP::maximum(maxRigSize, numJoints);

    // Add retargeting tags into a local group list, and count the number of mapped joints.
    std::list<std::string> myRetargetGroups;
    uint32_t jointCount = animRigExport->getNumJoints();
    uint32_t myNumMappedJoints = 0;
    for (uint32_t jointIndex = 0; jointIndex < jointCount; ++jointIndex)
    {
      const ME::JointExport* jointExport = animRigExport->getJoint(jointIndex);
      NMP_VERIFY(jointExport);

      const char* tag = jointExport->getRetargetingTag();
      if (tag && tag[0] != 0)
      {
        ++myNumMappedJoints;
        myRetargetGroups.push_back(tag);
      }
    }

    // Adjust the largest mapping
    mostMappedJoints = NMP::maximum(mostMappedJoints, myNumMappedJoints);

    // Find the longest sequence in this rig
    uint32_t myLongestSequence = 1;
    myRetargetGroups.sort();
    std::list<std::string>::const_iterator it = myRetargetGroups.begin();
    while (it != myRetargetGroups.end())
    {
      std::string thisGroup = *it;
      uint32_t thisSequenceLength = 0;
      while (it != myRetargetGroups.end() && *it == thisGroup)
      {
        ++thisSequenceLength;
        ++it;
      }
      myLongestSequence = NMP::maximum(myLongestSequence, thisSequenceLength);
    }

    // Adjust the longest sequence in all rigs
    longestSequence = NMP::maximum(longestSequence, myLongestSequence);

    // Add this rig's retarget groups to the overall list
    retargetGroups.merge(myRetargetGroups);
  }

  // Pare the tag list down to the unique tags and count
  retargetGroups.sort();
  retargetGroups.unique();
  intermediateRigSize = (uint32_t)retargetGroups.size();
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildRetargetStorageStatsAttribData(
  NMP::Memory::Resource&                memRes,
  const ME::AnimationLibraryExport*     animLibraryExport,
  MR::AttribDataHandle*                 netDefAttribDataArray,
  uint32_t                              NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
  MR::SemanticLookupTable*              netDefSemanticLookupTable)
{
  // Format the provided memory resource for a rig retarget mapping attrib data.
  MR::AttribDataRetargetStorageStats* retargetStorageStatsAttribData =
    MR::AttribDataRetargetStorageStats::init(memRes, MR::IS_DEF_ATTRIB_DATA);

  // Initialise with values from the rig export.
  NMRU::FKRetarget::Solver::StorageStats& solverStorageStats = retargetStorageStatsAttribData->m_solverStorageStats;
  calculateRetargetMetrics(
    animLibraryExport,
    solverStorageStats.intermediateRigSize,
    solverStorageStats.largestRigSize,
    solverStorageStats.mostMappedJoints,
    solverStorageStats.longestSequence);
  solverStorageStats.mostMultiplexPackages = 0;

  // Create a retarget solver that would work for our rigs
  NMP::Memory::Format format = NMRU::FKRetarget::Solver::getMemoryRequirements(solverStorageStats);
  NMP::Memory::Resource solverResource;
  solverResource.format = format;
  solverResource.ptr = NMPMemoryAllocAligned(format.size, format.alignment);
  NMP_ASSERT(solverResource.ptr);
  NMRU::FKRetarget::Solver* solver = NMRU::FKRetarget::Solver::init(&solverResource, solverStorageStats);

  // Loop through all possible pairs of rigs, preparing the solver as if retargeting between them, and then
  // seeing what the storage stats were for that pair in order to get overall stats for the network.
  // First, loop through source rigs
  for (MR::AnimSetIndex sourceSetIndex = 0; sourceSetIndex < animLibraryExport->getNumAnimationSets();
    ++sourceSetIndex)
  {
    // Get the source rig, which must have already been created.
    uint32_t lookupIndex =
      netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_RIG, sourceSetIndex);
    const MR::AttribDataRig* sourceRigAttrib =
      static_cast<MR::AttribDataRig*>(netDefAttribDataArray[lookupIndex].m_attribData);
    _ASSERT(sourceRigAttrib);
    const MR::AnimRigDef* sourceRig = sourceRigAttrib->m_rig;

    // Get the source rig retarget mapping, which must have already been created.
    lookupIndex =
      netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING, sourceSetIndex);
    const MR::AttribDataRigRetargetMapping* sourceRigRetargetMapping =
      static_cast<MR::AttribDataRigRetargetMapping*>(netDefAttribDataArray[lookupIndex].m_attribData);
    _ASSERT(sourceRigRetargetMapping);

    // Loop through target rigs
    for (MR::AnimSetIndex targetSetIndex = 0; targetSetIndex < animLibraryExport->getNumAnimationSets();
      ++targetSetIndex)
    {
      // Get the target rig, which must have already been created.
      uint32_t lookupIndex =
        netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_RIG, targetSetIndex);
      const MR::AttribDataRig* targetRigAttrib =
        static_cast<MR::AttribDataRig*>(netDefAttribDataArray[lookupIndex].m_attribData);
      _ASSERT(targetRigAttrib);
      const MR::AnimRigDef* targetRig = targetRigAttrib->m_rig;

      // Get the source rig retarget mapping, which must have already been created.
      lookupIndex =
        netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING, targetSetIndex);
      const MR::AttribDataRigRetargetMapping* targetRigRetargetMapping =
        static_cast<MR::AttribDataRigRetargetMapping*>(netDefAttribDataArray[lookupIndex].m_attribData);
      _ASSERT(targetRigRetargetMapping);

      // Prepare the solver with this data
      solver->prepare(
        sourceRigRetargetMapping->m_retargetParams, sourceRig->getNumBones(), sourceRig->getHierarchy(),
        targetRigRetargetMapping->m_retargetParams, targetRig->getNumBones(), targetRig->getHierarchy());

      // Adjust the measure of multiplex package array length
      solverStorageStats.mostMultiplexPackages =
        NMP::maximum(solverStorageStats.mostMultiplexPackages,
        NMP::maximum(solver->getNumSourcePackages(), solver->getNumTargetPackages()));

    } // End of loop through target rigs

  } // End of loop through source rigs

  // Delete the solver
  NMP::Memory::memFree(solver);

  // Initialise the entry.
  uint32_t lookupIndex =
    netDefSemanticLookupTable->getLookupIndex(MR::ATTRIB_SEMANTIC_RETARGET_STORAGE_STATS, 0);
  NMP_ASSERT(lookupIndex < netDefAttribDataArraySize);
  netDefAttribDataArray[lookupIndex].set(
    retargetStorageStatsAttribData, MR::AttribDataRetargetStorageStats::getMemoryRequirements());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkDefBuilder::getStateMachineNodeIDsArrayMemReqs(
  const ME::NetworkDefExport* netDefExport,
  uint32_t& numStateMachineNodes)
{
  NMP_VERIFY(netDefExport);

  // Calculate the number of state machine nodes
  numStateMachineNodes = 0;
  for (uint32_t i = 1; i < netDefExport->getNumNodes(); ++i)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    MR::NodeType nodeType = nodeDefExport->getTypeID();
    if (nodeType == NODE_TYPE_STATE_MACHINE)
      ++numStateMachineNodes;
  }

  // Get the memory requirements
  return MR::NodeIDsArray::getMemoryRequirements(numStateMachineNodes);
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeIDsArray* NetworkDefBuilder::buildStateMachineNodeIDsArray(const ME::NetworkDefExport* netDefExport)
{
  uint32_t numStateMachineNodes;
  NMP::Memory::Format memReqs = getStateMachineNodeIDsArrayMemReqs(netDefExport, numStateMachineNodes);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  // Build the state machine node IDs array
  MR::NodeIDsArray* result = MR::NodeIDsArray::init(memRes, numStateMachineNodes);

  numStateMachineNodes = 0;
  for (uint32_t i = 1; i < netDefExport->getNumNodes(); ++i)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    MR::NodeType nodeType = nodeDefExport->getTypeID();
    if (nodeType == NODE_TYPE_STATE_MACHINE)
    {
      result->setEntry(numStateMachineNodes, (MR::NodeID) i);
      ++numStateMachineNodes;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkDefBuilder::getMultiplyConnectedNodeIDsArrayMemReqs(
  const ME::NetworkDefExport* netDefExport,
  uint32_t& numStateMachineNodes)
{
  NMP_VERIFY(netDefExport);

  // Calculate the number of multiply connected nodes
  numStateMachineNodes = 0;
  for (uint32_t i = 1; i < netDefExport->getNumNodes(); ++i)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    NodeDefDependency& nodeDep = sm_networkDefCompilationInfo->getNodeDefDependency(i);

    if (nodeDefExport->isDownstreamParentMultiplyConnected() && !nodeDep.m_isControlParamType)
      ++numStateMachineNodes;
  }

  // Get the memory requirements
  return MR::NodeIDsArray::getMemoryRequirements(numStateMachineNodes);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::buildMultiplyConnectedNodeIDsArray(
  NodeDefDependency*          nodeDefDep,
  bool*                       evalNodeDefDeps,
  const ME::NetworkDefExport* netDefExport,
  uint32_t&                   numMultiplyConnectedNodes,
  MR::NodeIDsArray*           result)
{
  NMP_VERIFY(nodeDefDep);
  NMP_VERIFY(evalNodeDefDeps);
  MR::NodeID nodeID = nodeDefDep->m_nodeID;
  if (evalNodeDefDeps[nodeID])
    return;
  evalNodeDefDeps[nodeID] = true;

  const ME::NodeExport* nodeDefExport = netDefExport->getNode(nodeID);
  NodeDefDependency& nodeDep = sm_networkDefCompilationInfo->getNodeDefDependency(nodeID);
  if (nodeDefExport->isDownstreamParentMultiplyConnected() && !nodeDep.m_isControlParamType)
  {
    result->setEntry(numMultiplyConnectedNodes, nodeID);
    ++numMultiplyConnectedNodes;
  }

  // Build the child node dependencies
  std::vector<NodeDefDependency*>::iterator nit;
  for (nit = nodeDefDep->m_childNodeIDs.begin(); nit != nodeDefDep->m_childNodeIDs.end(); ++nit)
  {
    NodeDefDependency* childNodeDefDep = *nit;
    buildMultiplyConnectedNodeIDsArray(
      childNodeDefDep,
      evalNodeDefDeps,
      netDefExport,
      numMultiplyConnectedNodes,
      result);
  }
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeIDsArray* NetworkDefBuilder::buildMultiplyConnectedNodeIDsArray(const ME::NetworkDefExport* netDefExport)
{
  uint32_t numMultiplyConnectedNodes;
  NMP::Memory::Format memReqs = getMultiplyConnectedNodeIDsArrayMemReqs(netDefExport, numMultiplyConnectedNodes);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  // Build the state machine node IDs array
  MR::NodeIDsArray* result = MR::NodeIDsArray::init(memRes, numMultiplyConnectedNodes);

  //---------------------------
  // Compile all node definitions in the dependency graph order
  uint32_t numNodes = netDefExport->getNumNodes();
  bool* evalNodeDefDeps = new bool[numNodes];
  for (uint32_t i = 0; i < numNodes; ++i)
    evalNodeDefDeps[i] = false;

  numMultiplyConnectedNodes = 0;

  NodeDefDependency& nodeDefDep = sm_networkDefCompilationInfo->getNodeDefDependency(MR::NETWORK_NODE_ID);
  buildMultiplyConnectedNodeIDsArray(&nodeDefDep, evalNodeDefDeps, netDefExport, numMultiplyConnectedNodes, result);

  for (uint32_t i = 1; i < numNodes; ++i)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    NodeDefDependency& nodeDep = sm_networkDefCompilationInfo->getNodeDefDependency(i);
    if (!evalNodeDefDeps[i] && nodeDefExport->isDownstreamParentMultiplyConnected() && !nodeDep.m_isControlParamType)
    {
      result->setEntry(numMultiplyConnectedNodes, (MR::NodeID) i);
      ++numMultiplyConnectedNodes;
    }
  }

  delete[] evalNodeDefDeps;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeTagTable* NetworkDefBuilder::buildNodeTagsTable(const ME::NetworkDefExport* netDefExport)
{
  const uint32_t numNodes = netDefExport->getNumNodes();
  nmtl::vector<char> nodeTags;
  nmtl::vector<nmtl::vector<uint32_t>> nodeTagArray(numNodes);
  nmtl::hashmap_dense_dynamic<uint32_t, uint32_t> existingTags(16);

  // Read all node tags into local containers
  std::string tagName;
  char tagPath[128];

  for (uint32_t nodeID = 0; nodeID < numNodes; ++nodeID)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(nodeID);
    const ME::DataBlockExport* dataBlock = nodeDefExport->getDataBlock();

    int32_t numTags;
    if (dataBlock->readInt(numTags, "NumTags"))
    {  
      for (int32_t tagIndex = 0; tagIndex < numTags; ++tagIndex)
      {
        NMP_SPRINTF(tagPath, "Tag_%i", tagIndex);
        bool readTag = dataBlock->readString(tagName, tagPath);
        NMP_VERIFY_MSG(readTag, "Failed to read \"%s\"", tagPath);
        
        const uint32_t hashedValue = nmtl::hfn::hashString(tagName.c_str());
        uint32_t tagArrayIndex = UINT_MAX;
        if (!existingTags.find(hashedValue, &tagArrayIndex))
        {
          uint32_t tagArraySize = (uint32_t)nodeTags.size();
          // Add the tag to the tag array
          nodeTags.resize(nodeTags.size() + tagName.size() + 1);
          NMP_STRNCPY_S(&nodeTags[tagArraySize], tagName.size() + 1, tagName.c_str());
          tagArrayIndex = tagArraySize;
          existingTags.insert(hashedValue, tagArrayIndex);
        }
        nodeTagArray[nodeID].push_back(tagArrayIndex);
      }
    }
  }

  NMP_ASSERT_MSG(nodeTags.size() <= _UI16_MAX, "The maximum number of node tags supported by MR::NodeTagTable is 2^16");
  NMP_ASSERT_MSG(nodeTagArray.size() <= _UI16_MAX, "The maximum number of nodes supported by MR::NodeTagTable is 2^16");

  // Convert the data into a format that the tag table can be initialised from.
  uint16_t* tagEntryLengths = (uint16_t*)NMP::Memory::memAlloc(sizeof(uint16_t) * nodeTagArray.size() NMP_MEMORY_TRACKING_ARGS);
  uint16_t** tagEntries = (uint16_t**)NMP::Memory::memAlloc(sizeof(uint16_t*) * nodeTagArray.size() NMP_MEMORY_TRACKING_ARGS);
  for (size_t i = 0; i < nodeTagArray.size(); ++i)
  {
    const uint16_t arrayLength = (uint16_t)nodeTagArray[i].size();
    tagEntryLengths[i] = arrayLength;
    if (arrayLength > 0)
    {
      tagEntries[i] = (uint16_t*)NMP::Memory::memAlloc(sizeof(uint16_t) * arrayLength NMP_MEMORY_TRACKING_ARGS);

      for (uint16_t tagIndex = 0; tagIndex < arrayLength; ++tagIndex)
      {
        tagEntries[i][tagIndex] = (uint16_t)nodeTagArray[i][tagIndex];
      }
    }
    else
    {
      tagEntries[i] = NULL;
    }
  }

  // Build the node tag table
  NMP::Memory::Format nodeTagTableMemoryRequirements = 
    MR::NodeTagTable::getMemoryRequirements((uint16_t)nodeTags.size(), tagEntryLengths, (uint16_t)nodeTagArray.size());

  NMP::Memory::Resource resource = NMP::Memory::allocateFromFormat(nodeTagTableMemoryRequirements NMP_MEMORY_TRACKING_ARGS);
  MR::NodeTagTable* nodeTagTable = MR::NodeTagTable::init(resource, nodeTags.buffer(), (uint16_t)nodeTags.size(), tagEntries, tagEntryLengths, (uint16_t)nodeTagArray.size());

  // Free all temporary memory
  if (tagEntryLengths)
  {
    NMP::Memory::memFree(tagEntryLengths);
    for (size_t i = 0; i < nodeTagArray.size(); ++i)
    {
      NMP::Memory::memFree((void*)tagEntries[i]);
    }
    NMP::Memory::memFree(tagEntries);
  }

  return nodeTagTable;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkDefBuilder::getMessageEmitterNodeIDsArrayMemReqs(
  const ME::NetworkDefExport* netDefExport,
  uint32_t& numMessageEmitterNodeIDs)
{
  NMP_VERIFY(netDefExport);

  // Calculate the number of state machine nodes
  numMessageEmitterNodeIDs = 0;
  for (uint32_t i = 1; i < netDefExport->getNumNodes(); ++i)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    const ME::DataBlockExport* dataBlock = nodeDefExport->getDataBlock();
    bool nodeEmitsMessages = false;
    dataBlock->readBool(nodeEmitsMessages, "NodeEmitsMessages");
    if (nodeEmitsMessages)
    {
      ++numMessageEmitterNodeIDs;
    }
  }

  // Get the memory requirements
  return MR::NodeIDsArray::getMemoryRequirements(numMessageEmitterNodeIDs);
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeIDsArray* NetworkDefBuilder::buildMessageEmitterNodeIDsArray(const ME::NetworkDefExport* netDefExport)
{
  uint32_t numMessageEmitterNodeIDs;
  NMP::Memory::Format memReqs = getMessageEmitterNodeIDsArrayMemReqs(netDefExport, numMessageEmitterNodeIDs);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  // Build the message Emitter Nodes ID array
  MR::NodeIDsArray* result = MR::NodeIDsArray::init(memRes, numMessageEmitterNodeIDs);

  numMessageEmitterNodeIDs = 0;
  for (uint32_t i = 1; i < netDefExport->getNumNodes(); ++i)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    const ME::DataBlockExport* dataBlock = nodeDefExport->getDataBlock();
    bool nodeEmitsMessages = false;
    dataBlock->readBool(nodeEmitsMessages, "NodeEmitsMessages");
    if (nodeEmitsMessages)
    {
      result->setEntry(numMessageEmitterNodeIDs, (MR::NodeID) i);
      ++numMessageEmitterNodeIDs;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::IDMappedStringTable* NetworkDefBuilder::buildStateMachineStateIDStringTable(const ME::NetworkDefExport* netDefExport)
{
  std::map<std::string, uint32_t> tempMap;

  // Now add the names of state machine states
  for (uint32_t nodeIdx = 0; nodeIdx < netDefExport->getNumNodes(); ++nodeIdx)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(nodeIdx);
    MR::NodeType nodeType = nodeDefExport->getTypeID();

    if (nodeType == NODE_TYPE_STATE_MACHINE)
    {
      // How many states in this state machine.
      uint32_t numStates = 0;
      ME::DataBlockExport* datablock = const_cast<ME::DataBlockExport*>(nodeDefExport->getDataBlock());
      datablock->readUInt(numStates, "ChildNodeCount");

      CHAR paramID[16];
      for (uint32_t stateIdx = 0; stateIdx < numStates; ++stateIdx)
      {
        // Need to get the nodeIDs of the states.
        uint32_t childStateNodeID;
        std::string paramName = "RuntimeChildNodeID_";
        sprintf_s(paramID, "%i", stateIdx);
        paramName += paramID;
        datablock->readUInt(childStateNodeID, paramName.c_str());

        // Get the name of this state. This is actually the name of the node inside the state machine.
        // We will strip this off to leave the state name only.
        const ME::NodeExport* childStateNodeExport = netDefExport->getNode(childStateNodeID);
        std::string childStateNodeName = childStateNodeExport->getName();

        // remove the last bit of the name (i.e. the node name so that we are left with the state name)
        // "MyStateMachine|MyState|Blend2" becomes "MyStateMachine|MyState"
        size_t pipePos = childStateNodeName.rfind('|');
        if (pipePos != childStateNodeName.npos)
        {
          childStateNodeName.resize(pipePos);
          tempMap.insert(std::make_pair(childStateNodeName, stateIdx));
        }
      }
    }
  }

  // Lets build a mapped string table.
  NMP::IDMappedStringTable* result = NULL;

  // Were there any states machine states for us to worry about?
  if( tempMap.size() > 0 )
  {
    // Collate the data and create the IDMappedStringTable.
    std::vector<const char*> stringList;
    std::vector<uint32_t> IDList;
    uint32_t tableDataSize = 0;

    for(std::map<std::string, uint32_t>::iterator it = tempMap.begin(); it != tempMap.end(); ++it)
    {
      tableDataSize += (uint32_t)it->first.size() + 1;
      stringList.push_back(it->first.c_str());
      IDList.push_back(it->second);
    }

    NMP::Memory::Format memReqs =  NMP::IDMappedStringTable::getMemoryRequirements((uint32_t)tempMap.size(), tableDataSize);
    NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

    // Initialize the table from the stream.
    result = NMP::IDMappedStringTable::init(memRes, (uint32_t)IDList.size(), &IDList[0], &stringList[0]);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkDefBuilder::getNodeIDNameMappingTableMemReqs(
  const ME::NetworkDefExport* netDefExport,
  uint32_t&                   numStrings,
  uint32_t&                   tableDataSize)
{
  NMP_VERIFY(netDefExport);
  tableDataSize = 0;
  numStrings = 0;

  // Compute the size required for all the nodes
  for (uint32_t i = 0; i < netDefExport->getNumNodes(); ++i)
  {
    //----------------------
    // Calc the size of the node names
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    const char* nodeName = nodeDefExport->getName();
    tableDataSize += (uint32_t)(strlen(nodeName) + 1);
    ++numStrings;
  }

  return NMP::IDMappedStringTable::getMemoryRequirements(numStrings, tableDataSize);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::IDMappedStringTable* NetworkDefBuilder::buildNodeIDNameMappingTable(const ME::NetworkDefExport* netDefExport)
{
  std::map<std::string, uint32_t> tempMap;

  // First put in all the names of actual nodes
  for (uint32_t i = 0; i < netDefExport->getNumNodes(); ++i)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    tempMap.insert(std::make_pair(std::string(nodeDefExport->getName()), nodeDefExport->getNodeID()));
  }

  std::vector<const char*> stringList;
  std::vector<uint32_t> IDList;
  uint32_t tableDataSize = 0;

  for(std::map<std::string, uint32_t>::iterator it = tempMap.begin(); it != tempMap.end(); ++it)
  {
    tableDataSize += (uint32_t)it->first.size() + 1;
    stringList.push_back(it->first.c_str());
    IDList.push_back(it->second);
  }

  NMP::Memory::Format memReqs =  NMP::IDMappedStringTable::getMemoryRequirements((uint32_t)tempMap.size(), tableDataSize);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  // Initialize the table from the stream.
  NMP::IDMappedStringTable* result = NMP::IDMappedStringTable::init(memRes, (uint32_t)IDList.size(), &IDList[0], &stringList[0]);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkDefBuilder::getMessageIDNameMappingTableMemReqs(
  const ME::NetworkDefExport* netDefExport,
  uint32_t&                   numStrings,
  uint32_t&                   tableDataSize)
{
  NMP_VERIFY(netDefExport);
  tableDataSize = 0;
  numStrings = netDefExport->getNumMessages();

  //----------------------
  // Compute the size required for all the messages
  for (uint32_t i = 0; i < numStrings; ++i)
  {
    const ME::MessageExport* messageExport = netDefExport->getMessage(i);

    //----------------------
    // Calc the size of the message name
    const char* messageName = messageExport->getName();
    tableDataSize += (uint32_t)(NMP_STRLEN(messageName) + 1);
  }

  return NMP::OrderedStringTable::getMemoryRequirements(numStrings, tableDataSize);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::OrderedStringTable* NetworkDefBuilder::buildMessageIDNameMappingTable(const ME::NetworkDefExport* netDefExport)
{
  uint32_t numStrings;
  uint32_t tableDataSize;
  NMP::Memory::Format memReqs = getMessageIDNameMappingTableMemReqs(netDefExport, numStrings, tableDataSize);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  // Create the arrays of indices.
  uint32_t* stringOffsets = NULL;
  char* stringsBuffer = NULL;
  if (numStrings)
  {
    stringOffsets = (uint32_t*)NMPMemoryAlloc(sizeof(uint32_t) * numStrings);
    NMP_ASSERT(stringOffsets);

    // Create the actual table.
    stringsBuffer = (char*)NMPMemoryAlloc(tableDataSize);
    NMP_ASSERT(stringsBuffer);
  }

  char* currentPtr = stringsBuffer;
  uint32_t currentOffset = 0;

  // First put in all the names of actual nodes
  for (uint32_t i = 0; i < numStrings; ++i)
  {
    const ME::MessageExport* messageExport = netDefExport->getMessage(i);
    const char* messageName = messageExport->getName();
    uint32_t messageID = messageExport->getMessageID();
    NMP_ASSERT(messageID < numStrings);

    stringOffsets[messageID] = currentOffset;
    memcpy(currentPtr, messageName, strlen(messageName) + 1);
    currentOffset += (uint32_t)(strlen(messageName) + 1);
    currentPtr += (uint32_t)(strlen(messageName) + 1);
  }

  // Initialize the table from the stream.
  NMP::OrderedStringTable* result = NMP::OrderedStringTable::init(
                               memRes,
                               numStrings,
                               stringOffsets,
                               stringsBuffer,
                               tableDataSize);
  //-----------------------
  // Free all the temporary memory
  if (stringOffsets)
    NMP::Memory::memFree(stringOffsets);
  if (stringsBuffer)
    NMP::Memory::memFree(stringsBuffer);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NetworkDef::NodeEventOnExitMessage* NetworkDefBuilder::buildNodeOnExitMessageArray(
  const ME::NetworkDefExport* netDefExport, NMP::Memory::Format* format)
{
  // Count the number of Node OnExit Messages in the netDefExport
  uint32_t numOnExitMessagesInNetwork = 0;
  uint32_t numNodes = netDefExport->getNumNodes();
  for(uint32_t i = 0; i < numNodes; i++)
  {
    const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
    const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
    uint32_t numOnExitInternalMessageIDs = 0;
    uint32_t numOnExitExternalMessageIDs = 0;
    nodeDefDataBlock->readUInt(numOnExitInternalMessageIDs, "NumOnExitInternalMessageIDs");
    nodeDefDataBlock->readUInt(numOnExitExternalMessageIDs, "NumOnExitExternalMessageIDs");
    uint32_t numOnEnterMessagesInNode = numOnExitInternalMessageIDs + numOnExitExternalMessageIDs;

    numOnExitMessagesInNetwork += numOnEnterMessagesInNode;
  }

  MR::NetworkDef::NodeEventOnExitMessage* result = NULL;

  if(numOnExitMessagesInNetwork)
  {
    NMP_VERIFY_MSG(format, "NULL format pointer passed in to buildNodeOnExitMessageArray");

    // Calculate memory requirements and fill in the passed in Format struct.
    format->size = sizeof(MR::NetworkDef::NodeEventOnExitMessage) * numOnExitMessagesInNetwork;
    format->alignment = NMP_NATURAL_TYPE_ALIGNMENT;

    // Create the array to store the OnExit messages in
    result = (MR::NetworkDef::NodeEventOnExitMessage*)NMP::Memory::memAlloc(format->size NMP_MEMORY_TRACKING_ARGS);
    uint32_t arrayIndex = 0;
    CHAR messageSlotName[256];
    uint32_t messageID;
    bool readResult;

    // Fill in the array of messages
    for(MR::NodeID i = 0; i < numNodes; i++)
    {
      const ME::NodeExport* nodeDefExport = netDefExport->getNode(i);
      const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

      // Write out the internal exit messages
      uint32_t numMessages = 0;
      nodeDefDataBlock->readUInt(numMessages, "NumOnExitInternalMessageIDs");
      for(uint32_t j = 0; j < numMessages; j++)
      {
        // Get the message ID from the XML
        sprintf_s(messageSlotName, "OnExitInternalMessageID_%d", j);
        readResult = nodeDefDataBlock->readUInt(messageID, messageSlotName);
        NMP_VERIFY_MSG(readResult, "Could not read onExit internal message %i from node ID %i", j, nodeDefExport->getNodeID());

        result[arrayIndex].m_nodeID = i;
        result[arrayIndex].m_nodeEventMessage.m_external = false;
        result[arrayIndex].m_nodeEventMessage.m_msgID = messageID;
        arrayIndex++;
      }

      numMessages = 0;
      // Write out the external exit messages
      nodeDefDataBlock->readUInt(numMessages, "NumOnExitExternalMessageIDs");
      for(uint32_t j = 0; j < numMessages; j++)
      {
        // Get the message ID from the XML
        sprintf_s(messageSlotName, "OnExitExternalMessageID_%d", j);
        readResult = nodeDefDataBlock->readUInt(messageID, messageSlotName);
        NMP_VERIFY_MSG(readResult, "Could not read onExit external message %i from node ID %i", j, nodeDefExport->getNodeID());

        result[arrayIndex].m_nodeID = i;
        result[arrayIndex].m_nodeEventMessage.m_external = true;
        result[arrayIndex].m_nodeEventMessage.m_msgID = messageID;
        arrayIndex++;
      }
    }

    // Make sure we used all the slots we allocated
    NMP_VERIFY(arrayIndex == numOnExitMessagesInNetwork);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AC_CORE_EXPORT void NetworkDefBuilder::getNodeTypesOfAllDescendants( const NodeDefDependency & nodeDep, std::vector<MR::NodeType> & nodeTypes )
{
  // A list of nodes we've already visited.
  std::vector<MR::NodeID> nodeIDs;

  for (uint32_t i = 0; i < nodeDep.m_childNodeIDs.size(); ++i)
  {
    const uint32_t id = nodeDep.m_childNodeIDs[i]->m_nodeID;
    const NodeDefDependency & childNodeDep = sm_networkDefCompilationInfo->getNodeDefDependency(id);
    getNodeTypesRecursive( childNodeDep, nodeTypes, nodeIDs );
  }

  // remove duplicates
  std::sort( nodeTypes.begin(), nodeTypes.end() );
  nodeTypes.erase( std::unique( nodeTypes.begin(), nodeTypes.end() ), nodeTypes.end() );
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkDefBuilder::getNodeTypesRecursive( const NodeDefDependency & nodeDep, std::vector<MR::NodeType> & nodeTypes, std::vector<MR::NodeID> & nodeIDs)
{
  // We only add this node to our type list if we've not already visited it.  This prevents massive duplication
  // in the case of multiply-connected pass down pins.
  if(std::find(nodeIDs.begin(), nodeIDs.end(), nodeDep.m_nodeID) != nodeIDs.end())
  {
    return;
  }

  // If we haven't already visited this node, we add it to the type list and continue recursing down it's children.
  nodeTypes.push_back( nodeDep.m_nodeTypeID );
  nodeIDs.push_back(nodeDep.m_nodeID);

  for (uint32_t i = 0; i < nodeDep.m_childNodeIDs.size(); ++i)
  {
    const uint32_t id = nodeDep.m_childNodeIDs[i]->m_nodeID;
    const NodeDefDependency & childNodeDep = sm_networkDefCompilationInfo->getNodeDefDependency(id);
    getNodeTypesRecursive( childNodeDep, nodeTypes, nodeIDs );
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
