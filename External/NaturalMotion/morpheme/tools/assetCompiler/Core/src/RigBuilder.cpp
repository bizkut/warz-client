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
#include "RigBuilder.h"
#include "assetProcessor/AssetProcessorUtils.h"
#include "assetProcessor/AnimSource/animSourcePreprocessor.h"
#include "morpheme/mrAttribData.h"
#include "XMD/Model.h"
#include "XMD/AnimCycle.h"
#include "XMD/PoseStream.h"
#include <sstream>
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimRigDefBuilder::getMemoryRequirements(
  AP::AssetProcessor*       NMP_UNUSED(processor),
  const ME::RigExport*      rigExport,
  std::vector<const char*>& boneNames, // Out: entries are allocated in this function. Caller must deallocate these names.
  uint32_t&                 tableSize) // Required size of string table.
{
  NMP::Memory::Format result(sizeof(AnimRigDef), MR_TRANSFORM_ATTRIB_ALIGNMENT);

  uint32_t numEntries = rigExport->getNumJoints();

  // m_hierarchy.
  result += NMP::Hierarchy::getMemoryRequirements(numEntries);

  // m_boneNameMap, bone name string table.
  int32_t boneNameIndex;
  tableSize = 0;
  char duplicateBoneRename[256];
  uint32_t duplicateCount;
  const char* boneName;
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    boneName = rigExport->getJoint(i)->getName();

    // Deal with any duplicate bone names.
    boneNameIndex = findIndexOfStringInVector(boneName, boneNames);
    duplicateCount = 0;
    while (boneNameIndex != -1)
    {
      sprintf_s(duplicateBoneRename, "%s_%d", boneName, duplicateCount);
      boneNameIndex = findIndexOfStringInVector(duplicateBoneRename, boneNames);
      if (boneNameIndex == -1)
        boneName = duplicateBoneRename;
      ++duplicateCount;
    }

    // Add to bone names.
    uint32_t nameLen = (uint32_t)(strlen(boneName) + 1);
    char* newBoneName = (char*)NMPMemoryAlloc(nameLen);
    NMP_ASSERT(newBoneName);
    strcpy_s(newBoneName, nameLen, boneName);

    boneNames.push_back(newBoneName);
    tableSize += nameLen;
  }
  result += NMP::OrderedStringTable::getMemoryRequirements(numEntries, tableSize);

  // m_bindPose.
  result += getBindPoseMemoryRequirements(numEntries);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_TRANSFORM_ATTRIB_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimRigDefBuilder::getBindPoseMemoryRequirements(uint32_t numChannels)
{
  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;
  MR::AttribDataTransformBuffer::getPosQuatMemoryRequirements(numChannels, buffMemReqs, internalBuffMemReqs);
  return buffMemReqs;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimRigDefBuilder::initBindPose(
  NMP::Memory::Resource& memRes,
  AP::AssetProcessor*    processor,
  const ME::RigExport*   rigExport,
  AnimRigDefBuilder*     animRigDef)
{
  uint32_t numJoints = rigExport->getNumJoints();
  NMP_VERIFY(animRigDef->getNumBones() == numJoints);
  animRigDef->m_bindPose = NULL;

  //----------------------
  // Create the take.
  XMD::XModel xmdAnim;
  XMD::XAnimationTake* xmdTake = xmdAnim.CreateNode(XMD::XFn::AnimationTake)->HasFn<XMD::XAnimationTake>();
  NMP_VERIFY(xmdTake);

  // Set up some take information.
  xmdTake->SetStartTime(0.0f);
  xmdTake->SetEndTime(0.0f);
  xmdTake->SetNumFrames(1);
  xmdTake->SetFramesPerSecond(30.0f);
  xmdTake->SetName("BindPoseTake");
  
  // Bones common between the pose source anim and anim rig
  std::vector<XMD::XId> animBoneIDs;
  // Rig bone name ID list.
  std::vector<uint32_t> rigBoneNamesIDsVector;

  // calculate the value needed to scale the bind pose into game world units.
  const float rigScaleFactor = rigExport->getRigScaleFactor();
  const float runtimeAssetScaleFactor = processor->getRuntimeAssetScaleFactor();
  const float bindPoseScaleFactor = rigScaleFactor / runtimeAssetScaleFactor;

  bool shouldScale = true;
  const float errorTolerance = 1.0e-4f;
  if ((bindPoseScaleFactor + errorTolerance) > 1.0f &&
      (bindPoseScaleFactor - errorTolerance) < 1.0f)
  {
    shouldScale = false;
  }

  float x, y, z, w;
  // For each joint create an animated xmd bone.
  for (uint32_t i = 0; i < numJoints; ++i)
  {
    // Create an xmd node (XBone) for each joint.
    XMD::XBone* xmdBone = xmdAnim.CreateNode(XMD::XFn::Bone)->HasFn<XMD::XBone>();
    xmdBone->SetTranslation(XMD::XVector3(0.0f,0,0));
    xmdBone->SetRotation(XMD::XQuaternion(0.0f,1.0f,0.0f,0.0f));
    xmdBone->SetScale(XMD::XVector3(1.0f,1.0f,1.0f));
    const char* currentBoneName = rigExport->getJoint(i)->getName();
    xmdBone->SetName(currentBoneName);

    
    // Check to see if this anim bone name is present in the rig.
    const uint32_t numRigBones = animRigDef->getNumBones();
    for (uint32_t rigBoneIndex = 0; rigBoneIndex < numRigBones; ++rigBoneIndex)
    {
      const char* currentRigBoneName = animRigDef->getBoneName(rigBoneIndex);
      if (strcmp(currentRigBoneName, currentBoneName) == 0)
      {
        // There are some sampled key frames against this bone name in the source animation.
        rigBoneNamesIDsVector.push_back(rigBoneIndex);
        animBoneIDs.push_back(xmdBone->GetID());
      }
    }


    // Create a container which will hold the animation data for the node.
    XMD::XAnimatedNode* xmdAnimNode = xmdTake->CreateNodeAnimation(xmdBone);
    // Create an XAnimatedAttribute for each attribute we want to animation.
    XMD::XAnimatedAttribute* rotAnimAttr = xmdAnimNode->CreateAnimatedAttribute(XMD::XBone::kRotation); // XQuaternion
    XMD::XAnimatedAttribute* transAnimAttr = xmdAnimNode->CreateAnimatedAttribute(XMD::XBone::kTranslation); // XVector3

    // Create pose streams for each animated attribute (pose is used because it is a single unchanging frame of animation). 
    XMD::XPoseStream* rotAnimAttrPose = rotAnimAttr->CreatePoseStream();
    XMD::XPoseStream* transAnimAttrPose = transAnimAttr->CreatePoseStream();

    // Get the rotation value from the rig bind pose.
    const ME::JointExport* joint = rigExport->getJoint(i);
    joint->getRotation(x, y, z, w);
    // Set the rotation value of the pose.
    rotAnimAttrPose->Set(XMD::XQuaternion(x, y, z, w));

    // Get the translation value from the rig bind pose.
    joint->getTranslation(x, y, z);
    // Scale the pose
    if (shouldScale)
    {
      x = x * bindPoseScaleFactor;
      y = y * bindPoseScaleFactor;
      z = z * bindPoseScaleFactor;
    }
    // Set the translation value of the pose.
    transAnimAttrPose->Set(XMD::XVector3(x, y, z));
  }
  
  //----------------------
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
  
  // Initialise an anim info structure required by the anim processor.
  acAnimInfo animInfo;
  animInfo.m_StartTime = 0.0f;        ///< start time in seconds.
  animInfo.m_EndTime = 0.0f;          ///< end time in seconds.
  animInfo.m_FramesPerSecond = 30.0f; ///< number of frames per second in the cycle.
  animInfo.m_NumFrames = 1;           ///< number of frames in the cycle.
  //  std::string m_TakeName;
  animInfo.m_TakeIndex = 0;  

  //----------------------
  // Process the pose source XMD into an uncompressed anim source
  AnimSourceUncompressedOptions animSourceOptions; // Unchanging channel default tolerances (not needed)
  AnimSourceUncompressed* uncompressedSourceAnim = AnimSourceUncompressedBuilderXMD::init(
                                                                      xmdAnim,
                                                                      animInfo,
                                                                      animBoneIDs,
                                                                      animRigDef,
                                                                      rigToAnimEntryMap,
                                                                      1.0f, // Correct scaling is already done by connect so it should not be necessary here.
                                                                      animSourceOptions);
   
  //----------------------
  // Allocate some space to get the uncompressed anim keys into 
  NMP_VERIFY(uncompressedSourceAnim->getChannelSets()->getNumChannelSets() == numJoints);
  NMP::Vector3* sourcePositions = (NMP::Vector3*)NMPMemoryAlloc(sizeof(NMP::Vector3) * numJoints);
  NMP_ASSERT(sourcePositions);
  NMP::Quat*    sourceOrientations = (NMP::Quat*)NMPMemoryAlloc(sizeof(NMP::Quat) * numJoints);
  NMP_ASSERT(sourceOrientations);

  uncompressedSourceAnim->getChannelSets()->getPosKeysAtFrame(0, sourcePositions);
  uncompressedSourceAnim->getChannelSets()->getQuatKeysAtFrame(0, sourceOrientations);

  // The uncompressed source channels with be in anim-channel order, so we need to rearrange them to rig-channel order
  NMP::Vector3* sortedPositions = (NMP::Vector3*)NMPMemoryAlloc(sizeof(NMP::Vector3) * numJoints);
  NMP_ASSERT(sortedPositions);
  NMP::Quat*    sortedOrientations = (NMP::Quat*)NMPMemoryAlloc(sizeof(NMP::Quat) * numJoints);
  NMP_ASSERT(sortedOrientations);

  // Perform the sort into rig order
  for(uint16_t i = 0; i < numJoints; i++)
  {
    uint16_t rigIndex;
    rigToAnimEntryMap->getRigIndexForAnimIndex(i, rigIndex);
    NMP_VERIFY_MSG(rigIndex < numJoints, "An out-of-range rig index was specified for the pose RTAM");
    sortedPositions[rigIndex] = sourcePositions[i];
    sortedOrientations[rigIndex] = sourceOrientations[i];

    // The w component of the position has an undefined value at this point.  It doesn't affect any calculations but to 
    // ensure the rig builder produces identical binaries for the same rig we force the value to 0.
    sortedPositions[rigIndex].w = 0.0f;
  }

  //----------------------
  // Create the bind pose on the provides resource and store a pointer to it from the rig.
  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;
  MR::AttribDataTransformBuffer::getPosQuatMemoryRequirements(numJoints, buffMemReqs, internalBuffMemReqs);
  animRigDef->m_bindPose = MR::AttribDataTransformBuffer::initPosQuat(memRes, internalBuffMemReqs, numJoints, MR::IS_DEF_ATTRIB_DATA);

  // Store the bind pose transform for each channel.
  for (uint32_t i = 0; i < numJoints; ++i)
  {
    animRigDef->m_bindPose->m_transformBuffer->setChannelQuat(i, sortedOrientations[i]);
    animRigDef->m_bindPose->m_transformBuffer->setChannelPos(i, sortedPositions[i]);
    animRigDef->m_bindPose->m_transformBuffer->setChannelUsed(i);
  }
  animRigDef->m_bindPose->m_transformBuffer->calculateFullFlag();


  //----------------------
  // Free the temporary buffers we have created.
  NMP::Memory::memFree(sourcePositions);
  NMP::Memory::memFree(sourceOrientations);
  NMP::Memory::memFree(sortedPositions);
  NMP::Memory::memFree(sortedOrientations);
  NMP::Memory::memFree(rigToAnimEntryMap);
  NMP::Memory::memFree(uncompressedSourceAnim);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource AnimRigDefBuilder::init(
  AP::AssetProcessor*  processor,
  const ME::RigExport* rigExport)
{
  std::vector<const char*> boneNames;
  uint32_t tableSize = 0;
  NMP::Memory::Format format = AnimRigDefBuilder::getMemoryRequirements(processor, rigExport, boneNames, tableSize);
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(format);
  NMP::Memory::Resource result = resource;
  AnimRigDefBuilder* rig = (AnimRigDefBuilder*)resource.ptr;
  format.set(sizeof(AnimRigDef), MR_TRANSFORM_ATTRIB_ALIGNMENT);
  resource.increment(format);

  //--------------------
  // Set trajectory and root bone indexes.
  uint32_t numEntries = rigExport->getNumJoints();
  rig->m_trajectoryBoneIndex = rigExport->getTrajectoryIndex();
  rig->m_characterRootBoneIndex = rigExport->getHipIndex();
  NMP_VERIFY(numEntries > rig->m_trajectoryBoneIndex);
  NMP_VERIFY(numEntries > rig->m_characterRootBoneIndex);

  //--------------------
  // Blend frame transform.
  float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
  rigExport->getBlendFrameOrientationQuat(x, y, z, w);
  rig->m_blendFrameOrientation.setWXYZ(w, x, y, z);
  rigExport->getBlendFramePositionVec(x, y, z);
  rig->m_blendFrameTranslation.set(x, y, z);

  //--------------------
  // Hierarchy.
  // Allocate a temporary buffer of parent indices to construct the hierarchy from.
  int32_t* array = (int32_t*)NMPMemoryAlloc(sizeof(int32_t) * numEntries);
  NMP_ASSERT(array);
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    array[i] = rigExport->getJoint(i)->getParentIndex();
  }

  format = NMP::Hierarchy::getMemoryRequirements(numEntries);
  resource.align(format);
  rig->m_hierarchy = NMP::Hierarchy::init(resource, numEntries, array);
  NMP::Memory::memFree(array);

  //--------------------
  // Construct bone name string table
  uint32_t numStrings = numEntries;

  uint32_t* stringOffset = (uint32_t*)NMPMemoryAlloc(sizeof(uint32_t) * numStrings);
  NMP_ASSERT(stringOffset);

  // Now create the table input data
  char* debugStrings = (char*)NMPMemoryAlloc(tableSize);
  NMP_ASSERT(debugStrings);
  uint32_t currentOffset = 0;
  char* currentPtr = debugStrings;
  for (uint32_t i = 0; i < numStrings; ++i)
  {
    const char* boneName = boneNames[i];

    stringOffset[i] = currentOffset;
    memcpy(currentPtr, boneName, strlen(boneName) + 1);
    currentOffset += (uint32_t)(strlen(boneName) + 1);
    currentPtr += (uint32_t)(strlen(boneName) + 1);
  }

  // Create a memory desc to place this table into.
  format = NMP::OrderedStringTable::getMemoryRequirements(numStrings, tableSize);

  // initialise the table from the stream.
  resource.align(format);
  rig->m_boneNameMap = NMP::OrderedStringTable::init(resource, numStrings, stringOffset, debugStrings, tableSize);

  NMP::Memory::memFree(stringOffset);
  NMP::Memory::memFree(debugStrings);

  //--------------------
  // Bind pose.
  initBindPose(resource, processor, rigExport, rig);
  
  //--------------------
  // Wipe down names array.
  for (uint32_t i = 0; i < boneNames.size(); ++i)
  {
    NMP::Memory::memFree((void*)boneNames[i]);
  }
  boneNames.clear();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimRigDefBuilder::getMemoryRequirements(
  const std::vector<const char*>& boneNames,
  uint32_t&                       tableSize)
{
  NMP::Memory::Format result(sizeof(AnimRigDef), MR_TRANSFORM_ATTRIB_ALIGNMENT);

  uint32_t numEntries = (uint32_t)boneNames.size();

  // m_hierarchy.
  result += NMP::Hierarchy::getMemoryRequirements(numEntries);

  // m_boneNameMap, bone name string table.
  tableSize = 0;
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    uint32_t nameLen = (uint32_t)(strlen(boneNames[i]) + 1);
    tableSize += nameLen;
  }
  result += NMP::OrderedStringTable::getMemoryRequirements(numEntries, tableSize);

  // m_bindPose.
  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;
  MR::AttribDataTransformBuffer::getMemoryRequirements(
    2,
    NMP::DataBuffer::sm_PosQuatDescriptor,
    numEntries,
    buffMemReqs,
    internalBuffMemReqs);
  result += buffMemReqs;

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_TRANSFORM_ATTRIB_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource AnimRigDefBuilder::createRigDef(
  const std::vector<const char*>&           rigBoneNames,
  const std::vector<const MR::AnimRigDef*>& rigs)
{
  uint32_t numEntries = (uint32_t)rigBoneNames.size();
  NMP_VERIFY(numEntries > 0);
  NMP_VERIFY(rigs.size() > 0);

  //--------------------
  uint32_t tableSize = 0;
  NMP::Memory::Format format = AnimRigDefBuilder::getMemoryRequirements(rigBoneNames, tableSize);
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(format);
  NMP::Memory::Resource result = resource;
  AnimRigDefBuilder* rig = (AnimRigDefBuilder*)resource.ptr;
  format.set(sizeof(AnimRigDef), MR_TRANSFORM_ATTRIB_ALIGNMENT);
  resource.increment(format);

  //--------------------
  // Set trajectory and root bone indexes.
  const char* boneName;
  const MR::AnimRigDef* curRig = rigs[0];
  uint32_t rigBoneIndex = curRig->getTrajectoryBoneIndex();
  boneName = curRig->getBoneName(rigBoneIndex);
  rig->m_trajectoryBoneIndex = findIndexOfStringInVector(boneName, rigBoneNames);

  rigBoneIndex = curRig->getCharacterRootBoneIndex();
  boneName = curRig->getBoneName(rigBoneIndex);
  rig->m_characterRootBoneIndex = findIndexOfStringInVector(boneName, rigBoneNames);

  // Verify common trajectory and hips channels in every rig
  for (uint32_t rigIndex = 1; rigIndex < rigs.size(); ++rigIndex)
  {
    curRig = rigs[rigIndex];
    rigBoneIndex = curRig->getTrajectoryBoneIndex();
    boneName = curRig->getBoneName(rigBoneIndex);
    int32_t boneNameIndex = findIndexOfStringInVector(boneName, rigBoneNames);
    NMP_VERIFY((uint32_t)boneNameIndex == rig->m_trajectoryBoneIndex);

    rigBoneIndex = curRig->getCharacterRootBoneIndex();
    boneName = curRig->getBoneName(rigBoneIndex);
    boneNameIndex = findIndexOfStringInVector(boneName, rigBoneNames);
    NMP_VERIFY((uint32_t)boneNameIndex == rig->m_characterRootBoneIndex);
  }

  //--------------------
  // Blend frame transform.
  curRig = rigs[0];
  rig->m_blendFrameOrientation = *curRig->getBlendFrameOrientation();
  rig->m_blendFrameTranslation = *curRig->getBlendFrameTranslation();

  // Verify common blend frame transform in every rig
  for (uint32_t rigIndex = 1; rigIndex < rigs.size(); ++rigIndex)
  {
    NMP_VERIFY_MSG(
      haveIdenticalBlendFrameTransforms(curRig, rigs[rigIndex]),
      "The Blend frame transforms for the rig set are different.\n");
  }

  //--------------------
  // Hierarchy.
  // Allocate a temporary buffer of parent indices to construct the hierarchy from.
  int32_t* array = (int32_t*)NMPMemoryAlloc(sizeof(int32_t) * numEntries);
  NMP_ASSERT(array);
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    int32_t boneNameIndex = -1;
    for (uint32_t rigIndex = 0; rigIndex < rigs.size(); ++rigIndex)
    {
      curRig = rigs[rigIndex];
      rigBoneIndex = curRig->getBoneIndexFromName(rigBoneNames[i]);
      if (rigBoneIndex != MR::AnimRigDef::BONE_NAME_NOT_FOUND)
      {
        int32_t parentBoneIndex = curRig->getParentBoneIndex(rigBoneIndex);
        if (parentBoneIndex != -1)
        {
          const char* parentBoneName = curRig->getBoneName(parentBoneIndex);
          boneNameIndex = findIndexOfStringInVector(parentBoneName, rigBoneNames);
          NMP_VERIFY(boneNameIndex != -1);
        }
        break;
      }
    }
    array[i] = boneNameIndex;
  }

  format = NMP::Hierarchy::getMemoryRequirements(numEntries);
  resource.align(format);
  rig->m_hierarchy = NMP::Hierarchy::init(resource, numEntries, array);
  NMP::Memory::memFree(array);

  //--------------------
  // Construct bone name string table
  uint32_t numStrings = numEntries;

  uint32_t* stringOffset = (uint32_t*)NMPMemoryAlloc(sizeof(uint32_t) * numStrings);
  NMP_ASSERT(stringOffset);

  // Now create the table input data
  char* debugStrings = (char*)NMPMemoryAlloc(tableSize);
  NMP_ASSERT(debugStrings);
  uint32_t currentOffset = 0;
  char* currentPtr = debugStrings;
  for (uint32_t i = 0; i < numStrings; ++i)
  {
    boneName = rigBoneNames[i];

    stringOffset[i] = currentOffset;
    memcpy(currentPtr, boneName, strlen(boneName) + 1);
    currentOffset += (uint32_t)(strlen(boneName) + 1);
    currentPtr += (uint32_t)(strlen(boneName) + 1);
  }

  // Create a memory desc to place this table into.
  format = NMP::OrderedStringTable::getMemoryRequirements(numStrings, tableSize);

  // initialise the table from the stream.
  resource.align(format);
  rig->m_boneNameMap = NMP::OrderedStringTable::init(resource, numStrings, stringOffset, debugStrings, tableSize);

  NMP::Memory::memFree(stringOffset);
  NMP::Memory::memFree(debugStrings);

  //--------------------
  // Bind pose.
  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;
  MR::AttribDataTransformBuffer::getMemoryRequirements(
    2,
    NMP::DataBuffer::sm_PosQuatDescriptor,
    numEntries,
    buffMemReqs,
    internalBuffMemReqs);
  resource.align(buffMemReqs);
  rig->m_bindPose = MR::AttribDataTransformBuffer::init(
                      resource,
                      internalBuffMemReqs,
                      2,
                      NMP::DataBuffer::sm_PosQuatDescriptor,
                      numEntries);

  // Store the bind pose transform for each channel.
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    NMP::Quat bindPoseQuat(NMP::Quat::kIdentity);
    NMP::Vector3 bindPosePos(NMP::Vector3::InitZero);
    for (uint32_t rigIndex = 0; rigIndex < rigs.size(); ++rigIndex)
    {
      curRig = rigs[rigIndex];
      rigBoneIndex = curRig->getBoneIndexFromName(rigBoneNames[i]);
      if (rigBoneIndex != MR::AnimRigDef::BONE_NAME_NOT_FOUND)
      {
        bindPoseQuat = *curRig->getBindPoseBoneQuat(rigBoneIndex);
        bindPosePos = *curRig->getBindPoseBonePos(rigBoneIndex);
        break;
      }
    }
    rig->m_bindPose->m_transformBuffer->setChannelQuat(i, bindPoseQuat);
    rig->m_bindPose->m_transformBuffer->setChannelPos(i, bindPosePos);
  }
  rig->m_bindPose->m_transformBuffer->calculateFullFlag();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Note: The bone indexes do not need to be identical, its the hierarchy that we are checking.
bool AnimRigDefBuilder::haveIdenticalParentHierarchiesFromStartingBoneIndexes(
  const AnimRigDef* rig0,
  uint32_t          rig0StartBoneIndex,
  const AnimRigDef* rig1,
  uint32_t          rig1StartBoneIndex)
{
  uint32_t    currentBoneIndex0;
  uint32_t    currentBoneIndex1;
  const char* bone0Name;
  const char* bone1Name;

  NMP_VERIFY(rig0 && rig1);

  currentBoneIndex0 = rig0StartBoneIndex;
  currentBoneIndex1 = rig1StartBoneIndex;

  while ((currentBoneIndex0 != NO_PARENT_BONE) && (currentBoneIndex1 != NO_PARENT_BONE))
  {
    bone0Name = rig0->getBoneName(currentBoneIndex0);
    bone1Name = rig1->getBoneName(currentBoneIndex1);
    if (strcmp(bone0Name, bone1Name) != 0)
      return false; // Hierarchies are different.

    currentBoneIndex0 = rig0->getParentBoneIndex(currentBoneIndex0);
    currentBoneIndex1 = rig1->getParentBoneIndex(currentBoneIndex1);
  }

  if (currentBoneIndex0 != currentBoneIndex1)
    return false; // Hierarchies are different (Do not reach the root at the same time).

  return true; // Hierarchies are the same.
}

//----------------------------------------------------------------------------------------------------------------------
// Note: The bone indexes do not need to be identical, its the hierarchy that we are checking.
bool AnimRigDefBuilder::haveIdenticalTrajectoryComponents(const AnimRigDef* rig0, const AnimRigDef* rig1)
{
  bool result;
  NMP_VERIFY(rig0 && rig1);

  /// Trajectory bone parental hierarchy.
  result = haveIdenticalParentHierarchiesFromStartingBoneIndexes(
             rig0,
             rig0->getTrajectoryBoneIndex(),
             rig1,
             rig1->getTrajectoryBoneIndex());

  /// Character root bone parental hierarchy.
  result = result && haveIdenticalParentHierarchiesFromStartingBoneIndexes(
             rig0,
             rig0->getCharacterRootBoneIndex(),
             rig1,
             rig1->getCharacterRootBoneIndex());

  // Blend frame transform
  result = result && haveIdenticalBlendFrameTransforms(rig0, rig1);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimRigDefBuilder::searchBoneIsInParentHierarchyOfStartingBone(
  const AnimRigDef*  rigToSearch,
  uint32_t           rigStartBoneIndex,
  const char*        boneNameToSearchFor)
{
  uint32_t    currentBoneIndex;
  const char*  currentBoneName;

  NMP_VERIFY(rigToSearch && boneNameToSearchFor);

  currentBoneIndex = rigStartBoneIndex;
  while (currentBoneIndex != NO_PARENT_BONE)
  {
    currentBoneName = rigToSearch->getBoneName(currentBoneIndex);
    if (strcmp(currentBoneName, boneNameToSearchFor) == 0)
      return currentBoneIndex; // Bone is part of the starting bones parent hierarchy.
    currentBoneIndex = rigToSearch->getParentBoneIndex(currentBoneIndex);
  }

  return INVALID_BONE_INDEX;  // Bone is not part of the starting bones parent hierarchy.
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimRigDefBuilder::haveIdenticalBlendFrameTransforms(
  const AnimRigDef* rig0,
  const AnimRigDef* rig1)
{
  NMP_VERIFY(rig0 && rig1);

  NMP::Quat blendFrameQuat0 = *rig0->getBlendFrameOrientation();
  NMP::Vector3 blendFramePos0 = *rig0->getBlendFrameTranslation();
  NMP::Quat blendFrameQuat1 = *rig1->getBlendFrameOrientation();
  NMP::Vector3 blendFramePos1 = *rig1->getBlendFrameTranslation();
  const float tol = 1e-4f;

  // Blend frame Quat
  if (fabs(blendFrameQuat0.x - blendFrameQuat1.x) > tol ||
      fabs(blendFrameQuat0.y - blendFrameQuat1.y) > tol ||
      fabs(blendFrameQuat0.z - blendFrameQuat1.z) > tol ||
      fabs(blendFrameQuat0.w - blendFrameQuat1.w) > tol)
  {
    return false;
  }

  // Blend frame pos
  if (fabs(blendFramePos0.x - blendFramePos1.x) > tol ||
      fabs(blendFramePos0.y - blendFramePos1.y) > tol ||
      fabs(blendFramePos0.z - blendFramePos1.z) > tol)
  {
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimRigDefBuilder::isOneOfTheTrajectoryComponents(const AnimRigDef* rigToSearch, const char* boneNameToSearchFor)
{
  uint32_t foundBoneIndex;

  NMP_VERIFY(rigToSearch && boneNameToSearchFor);

  /// Trajectory bone parental hierarchy.
  foundBoneIndex = searchBoneIsInParentHierarchyOfStartingBone(
                     rigToSearch,
                     rigToSearch->getTrajectoryBoneIndex(),
                     boneNameToSearchFor);
  if (foundBoneIndex != INVALID_BONE_INDEX)
    return foundBoneIndex;

  /// Character root bone parental hierarchy.
  foundBoneIndex = searchBoneIsInParentHierarchyOfStartingBone(
                     rigToSearch,
                     rigToSearch->getCharacterRootBoneIndex(),
                     boneNameToSearchFor);
  if (foundBoneIndex != INVALID_BONE_INDEX)
    return foundBoneIndex;

  return INVALID_BONE_INDEX;  // Bone is not part of the starting bones parent hierarchy.
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
