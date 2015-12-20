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
#include "assetProcessor/AnimationPoseBuilder.h"
#include "assetProcessor/AnimSource/AnimSourcePreprocessor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimationPoseBuilder::getMemoryRequirements(
  const ME::AnimationSetExport* animSetExport,
  const ME::AnimationPoseDefExport* pose)
{
  const ME::RigExport* rigExport = animSetExport->getRig();
  uint32_t numChannels = rigExport->getNumJoints();

  NMP::Memory::Format result(0, NMP_NATURAL_TYPE_ALIGNMENT);

  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;

  if(pose)
  { 
    MR::AttribDataTransformBuffer::getPosQuatMemoryRequirements(numChannels, buffMemReqs, internalBuffMemReqs);
    result += buffMemReqs;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimationPoseBuilder::constructPose(
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

} // namespace AP
