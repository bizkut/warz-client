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
#include "BodyDefBuilder.h"
#include "export/mcExportBody.h"
#include "export/mcExportPhysics.h"
#include "euphoria/erBodyDef.h"
#include "euphoria/erLimbDef.h"
#include "physics/mrPhysicsRigDef.h"
#include "physics/mrPhysicsRig.h"
#include "NMGeomUtils/NMJointLimits.h"
#include "assetProcessor/AnimSource/animSourcePreprocessor.h"
#include "NMPlatform/NMBuffer.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool dislocateBodyDef(void* object)
{
  ((ER::BodyDef*)object)->dislocate();
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool bodySetRigAndPhysicsRig(
  AP::ProcessedAsset* asset,
  MR::AnimRigDef* rig,
  MR::PhysicsRigDef* physicsRigDef)
{
  NMP_VERIFY(asset && rig && physicsRigDef);
  ER::BodyDef* bodyDef = (ER::BodyDef*) asset->assetMemory.ptr;
  bodyDef->setPhysicsRigDef(physicsRigDef);
  bodyDef->setRig(rig);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
ER::LimbDef::LimbType limbTypeFromLimbTypeName(const char* typeName)
{
  if (typeName == 0)
    return ER::LimbTypeEnum::L_unknown;

  std::string sTName(typeName);
  if (sTName == "Leg")
    return ER::LimbTypeEnum::L_leg;
  else if (sTName == "Arm")
    return ER::LimbTypeEnum::L_arm;
  else if (sTName == "Head")
    return ER::LimbTypeEnum::L_head;
  else if (sTName == "Spine")
    return ER::LimbTypeEnum::L_spine;

  return ER::LimbTypeEnum::L_unknown;
}

//----------------------------------------------------------------------------------------------------------------------
/// Return the number of valid parts from a set.
int32_t validateParts(const char* partNames[], int32_t numParts, MR::PhysicsRigDef* physicsRigDef)
{
  int32_t numPartsAct = 0;

  for (int32_t i = 0; i < numParts; ++i)
  {
    int32_t extraPartId = physicsRigDef->getPartIndexFromName(partNames[i], false);
    if (extraPartId != -1)
    {
      ++numPartsAct;
    }
  }

  return numPartsAct;
}

//----------------------------------------------------------------------------------------------------------------------
class LimbDefBuilder
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    AP::AssetProcessor*      processor,
    const ME::BodyDefExport* bodyDefExport,
    const ME::LimbDefExport* limbDefExport);

  static void init(
    NMP::Memory::Resource& resource,
    AP::AssetProcessor*      processor,
    const ME::BodyDefExport* bodyDefExport,
    const ME::LimbDefExport* limbDefExport);
};

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format LimbDefBuilder::getMemoryRequirements(
  AP::AssetProcessor*      processor,
  const ME::BodyDefExport* bodyDefExport,
  const ME::LimbDefExport* limbDefExport)
{
  // start with natural type alignment
  // Setup space for limb def shell
  NMP::Memory::Format limbDefFormat(sizeof(ER::LimbDef), NMP_VECTOR_ALIGNMENT);
  
  ME::GUID physicsRigGUID = bodyDefExport->getPhysicsRigGUID();
  const ProcessedAsset* physicsRigAsset = processor->findProcessedAsset(physicsRigGUID);

  // Hitting this assert means that the physics rig is not loaded yet. This is prevented elsewhere.
  NMP_VERIFY(physicsRigAsset);
  MR::PhysicsRigDef* physicsRigDef = (MR::PhysicsRigDef*) physicsRigAsset->assetMemory.ptr;

  // Setup space for limb def name
  limbDefFormat += NMP::Memory::Format(1 + strlen(limbDefExport->getName()), NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for the chain.
  int32_t endIndex = physicsRigDef->getPartIndexFromName(limbDefExport->getEndPartName(), false);
  NMP_VERIFY_MSG(endIndex != -1, "LimbDefBuilder: %s - Invalid end part: %s", limbDefExport->getName(), limbDefExport->getEndPartName());
  int32_t rootIndex = physicsRigDef->getPartIndexFromName(limbDefExport->getRootPartName(), false);
  NMP_VERIFY_MSG(rootIndex != -1, "LimbDefBuilder: %s - Invalid root part: %s", limbDefExport->getName(), limbDefExport->getEndPartName());

  int32_t numJointsInChain = physicsRigDef->calculateNumJointsInChain(endIndex, rootIndex);
  NMP_VERIFY_MSG(numJointsInChain >= 0, "LimbDefBuilder: %s - Invalid joint chains", limbDefExport->getName());

  // Setup space for joint indexes
  limbDefFormat += NMP::Memory::Format(sizeof(uint32_t) * numJointsInChain, NMP_NATURAL_TYPE_ALIGNMENT);

  int32_t totalNumParts = numJointsInChain + 1;
  // Add extra parts.
  int32_t numExtraParts = (uint32_t)limbDefExport->getNumExtraParts();
  const int32_t MAX_NUM_EXTRA_PARTS = 16;
  NMP_VERIFY_MSG(numExtraParts < MAX_NUM_EXTRA_PARTS, "LimbDefBuilder: %s - Invalid number of extra parts", limbDefExport->getName());
  const char* extraPartNames[MAX_NUM_EXTRA_PARTS];
  limbDefExport->getExtraParts(extraPartNames, (uint32_t)numExtraParts);

  numExtraParts = validateParts(extraPartNames, numExtraParts, physicsRigDef);
  NMP_VERIFY_MSG(numExtraParts >= 0, "LimbDefBuilder: %s - Invalid number of extra parts", limbDefExport->getName());

  totalNumParts += numExtraParts;
  // Setup space for part indexes
  limbDefFormat += NMP::Memory::Format(sizeof(uint32_t) * totalNumParts, NMP_NATURAL_TYPE_ALIGNMENT);

  const int32_t numPartsInChain = numJointsInChain + 1;

  // Setup space for:
  // Guide pose active joints
  limbDefFormat += NMP::Memory::Format(sizeof(bool) * numPartsInChain, NMP_NATURAL_TYPE_ALIGNMENT);
  // IK orientations weights  
  limbDefFormat += NMP::Memory::Format(sizeof(float) * numPartsInChain, NMP_NATURAL_TYPE_ALIGNMENT);
  // IK position weights
  limbDefFormat += NMP::Memory::Format(sizeof(float) * numPartsInChain, NMP_NATURAL_TYPE_ALIGNMENT);
  
  // Setup space for swivel quats
  NMP::Memory::Format quatArrayFormat;
  quatArrayFormat = NMP::Memory::Format(sizeof(NMP::Quat), NMP_VECTOR_ALIGNMENT);
  quatArrayFormat *= numJointsInChain;
  // zero swivel quats
  limbDefFormat += quatArrayFormat;
  // positive swivel quats
  limbDefFormat += quatArrayFormat;
  // negative swivel quats
  limbDefFormat += quatArrayFormat;

  // Hamstring bend scales
  const ME::HamstringExport* hamstring = limbDefExport->getHamstring();
  if (hamstring->getEnabled())
  {
    // there is a weight for all objects in the limb
    uint32_t bendScaleWeightCount = numJointsInChain + 1;
    // Setup space for bend scale weights
    NMP::Memory::Format vecArrayFormat;
    vecArrayFormat = NMP::Memory::Format(sizeof(NMP::Vector3), NMP_VECTOR_ALIGNMENT);
    vecArrayFormat *= bendScaleWeightCount;
    limbDefFormat += vecArrayFormat;
  }

  return limbDefFormat;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbDefBuilder::init(
  NMP::Memory::Resource& resource,
  AP::AssetProcessor* processor,
  const ME::BodyDefExport* bodyDefExport,
  const ME::LimbDefExport* limbDefExport)
{
  NMP::Memory::Format limbDefShellFormat(NMP::Memory::Format(sizeof(ER::LimbDef), NMP_VECTOR_ALIGNMENT));
  resource.align(limbDefShellFormat);
  ER::LimbDef* limbDef = (ER::LimbDef*)resource.ptr;
  resource.increment(limbDefShellFormat);

  // The idea is to get the physics rig def asset from its guid.
  ME::GUID physicsRigGUID = bodyDefExport->getPhysicsRigGUID();
  const ProcessedAsset* physicsRigAsset = processor->findProcessedAsset(physicsRigGUID);

  // Hitting this assert means that the physics rig is not loaded yet.
  NMP_VERIFY(physicsRigAsset);
  MR::PhysicsRigDef* physicsRigDef = (MR::PhysicsRigDef*) physicsRigAsset->assetMemory.ptr;

  // Setup limb def properties.
  limbDef->m_type = limbTypeFromLimbTypeName(limbDefExport->getType());
  limbDef->m_isRootLimb = false;

  limbDef->m_reachLimit.m_angle = limbDefExport->getReachConeAngle();
  limbDef->m_reachLimit.m_transform = limbDefExport->getReachOffset();
  limbDef->m_reachLimit.m_distance = limbDefExport->getReachDistance();

  limbDef->m_endOffset  = limbDefExport->getEndOffset();
  limbDef->m_rootOffset = limbDefExport->getRootOffset();
  NMP_VERIFY(limbDef->m_endOffset.isValidTM(0.001f));
  NMP_VERIFY(limbDef->m_rootOffset.isValidTM(0.001f));

  int32_t endIndex = physicsRigDef->getPartIndexFromName(limbDefExport->getEndPartName(), false);
  NMP_VERIFY_MSG(
    endIndex != -1,
    "LimbDefBuilder: %s - Invalid end part: %s",
    limbDefExport->getName(),
    limbDefExport->getEndPartName());
  int32_t rootIndex = physicsRigDef->getPartIndexFromName(limbDefExport->getRootPartName(), false);
  NMP_VERIFY_MSG(rootIndex != -1,
    "LimbDefBuilder: %s - Invalid root part: %s",
    limbDefExport->getName(),
    limbDefExport->getRootPartName());
  int32_t baseIndex = physicsRigDef->getPartIndexFromName(limbDefExport->getBasePartName(), false);
  NMP_VERIFY_MSG(baseIndex != -1,
    "LimbDefBuilder: %s - Invalid base part: %s",
    limbDefExport->getName(),
    limbDefExport->getBasePartName());

  // mid index is only valid for arms and legs otherwise must be invalid
  int32_t midIndex = physicsRigDef->getPartIndexFromName(limbDefExport->getMidPartName(), false);
  bool isArmOrLeg = (limbDef->m_type == ER::LimbTypeEnum::L_arm || limbDef->m_type == ER::LimbTypeEnum::L_leg);
  NMP_VERIFY_MSG( 
    (isArmOrLeg && midIndex != -1) || (!isArmOrLeg && midIndex == -1),
    "LimbDefBuilder: %s - Invalid mid part: %s",
    limbDefExport->getName(),
    limbDefExport->getMidPartName());

  limbDef->m_endIndex  = endIndex;
  limbDef->m_rootIndex = rootIndex;
  limbDef->m_baseIndex = baseIndex;
  limbDef->m_midIndex = midIndex;

  limbDef->m_numJointsInChain = physicsRigDef->calculateNumJointsInChain(limbDef->m_endIndex, limbDef->m_rootIndex);
  NMP_VERIFY_MSG(limbDef->m_numJointsInChain > 0, "LimbDefBuilder: %s - root part: %s and end part: %s do not form a chain", limbDefExport->getName(), limbDefExport->getRootPartName(), limbDefExport->getEndPartName());
  limbDef->m_numPartsInChain = limbDef->m_numJointsInChain + 1;

  int32_t numExtraParts = (uint32_t)limbDefExport->getNumExtraParts();
  const int MAX_NUM_EXTRA_PARTS = 16;
  NMP_VERIFY_MSG(numExtraParts < MAX_NUM_EXTRA_PARTS, "LimbDefBuilder: %s - Invalid number of extra parts", limbDefExport->getName());
  const char* extraPartNames[MAX_NUM_EXTRA_PARTS];
  limbDefExport->getExtraParts(extraPartNames, (uint32_t)numExtraParts);
  numExtraParts = validateParts(extraPartNames, numExtraParts, physicsRigDef);
  NMP_VERIFY_MSG(numExtraParts >= 0, "LimbDefBuilder: %s - Invalid number of extra parts", limbDefExport->getName());

  limbDef->m_totalNumParts = limbDef->m_numPartsInChain + numExtraParts;

  // Set up the name.
  size_t length = strlen(limbDefExport->getName());
  NMP::Memory::Format nameFormat(NMP::Memory::Format(sizeof(char) * (length + 1), NMP_NATURAL_TYPE_ALIGNMENT));
  resource.align(nameFormat);
  limbDef->m_name = (char*) resource.ptr;
  if (length != 0)
  {
    NMP::Memory::memcpy(
      limbDef->m_name,
      limbDefExport->getName(),
      sizeof(char) * length);
  }  
  limbDef->m_name[length] = '\0';
  resource.increment(nameFormat);

  // Setup rig joint indices
  NMP::Memory::Format rigJointIndicesFormat(sizeof(int32_t) * limbDef->m_numJointsInChain, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(rigJointIndicesFormat);
  limbDef->m_physicsRigJointIndices = (int32_t*) resource.ptr;
  resource.increment(rigJointIndicesFormat);

  // Store the joint indices.
  int32_t boneIndex = limbDef->m_endIndex;
  for (int32_t i = limbDef->m_numJointsInChain; i-- != 0; )
  {
    // The joint_index is always [the_part_index - 1], as it is a tree
    limbDef->m_physicsRigJointIndices[i] = boneIndex - 1;
    NMP_VERIFY_MSG(
      limbDef->m_physicsRigJointIndices[i] >= 0 && limbDef->m_physicsRigJointIndices[i] < physicsRigDef->m_numJoints,
      "LimbDefBuilder: %s - Invalid joint index in the chain.", limbDefExport->getName());

    boneIndex = physicsRigDef->m_joints[limbDef->m_physicsRigJointIndices[i]]->m_parentPartIndex;
  }

  // Set up the rig part indices
  NMP::Memory::Format rigPartIndicesFormat(sizeof(int32_t) * limbDef->m_totalNumParts, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(rigPartIndicesFormat);
  limbDef->m_physicsRigPartIndices = (int32_t*) resource.ptr;
  resource.increment(rigPartIndicesFormat);

  limbDef->m_numPartsBeforeBase = 0;
  // Store the part indices.
  for (uint32_t i = 0; i < limbDef->m_numPartsInChain; ++i)
  {
    // Note that this uses the fact that the joint index is one less than the part index. This is
    // because every part (except the root part) has a parent joint.
    if (i == 0)
    {
      // Get parent bone of joint 0
      NMP_VERIFY_MSG(
        limbDef->m_physicsRigJointIndices[0] >= 0 && limbDef->m_physicsRigJointIndices[0] < physicsRigDef->m_numJoints,
        "LimbDefBuilder: %s - Invalid joint index in the chain.", limbDefExport->getName());

      limbDef->m_physicsRigPartIndices[i] = physicsRigDef->m_joints[limbDef->m_physicsRigJointIndices[0]]->m_parentPartIndex;
    }
    else
    {
      // Get child body of joint partIndex
      NMP_VERIFY_MSG(
        limbDef->m_physicsRigJointIndices[i-1] >= 0 && limbDef->m_physicsRigJointIndices[i-1] < physicsRigDef->m_numJoints,
        "LimbDefBuilder: %s - Invalid joint index in the chain.", limbDefExport->getName());

      limbDef->m_physicsRigPartIndices[i] = physicsRigDef->m_joints[limbDef->m_physicsRigJointIndices[i-1]]->m_childPartIndex;
    }
    if (limbDef->m_physicsRigPartIndices[i] == limbDef->m_baseIndex)
      limbDef->m_numPartsBeforeBase = i;
  }

  for (int32_t i = 0; i < numExtraParts; ++i)
  {
    int32_t extraPartId = physicsRigDef->getPartIndexFromName(extraPartNames[i], false);
    NMP_VERIFY_MSG(extraPartId != -1, "LimbDefBuilder: %s - Invalid extra part.", limbDefExport->getName());
    limbDef->m_physicsRigPartIndices[i + limbDef->m_numPartsInChain] = extraPartId;
  }

  limbDef->m_guidePoseWeight = limbDefExport->getGuidePoseWeight();
  limbDef->m_neutralPoseWeight = limbDefExport->getNeutralPoseWeight();

  bool returnStatus = true;

  // Setup guide pose flags
  NMP::Memory::Format guidePoseJointFlagsFormat(sizeof(bool) * limbDef->m_numPartsInChain, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(guidePoseJointFlagsFormat);
  limbDef->m_activeGuidePoseJoints = (bool*)resource.ptr;
  resource.increment(guidePoseJointFlagsFormat);

  returnStatus = limbDefExport->getGuidePoseJoints(limbDef->m_activeGuidePoseJoints, limbDef->m_numPartsInChain);
  NMP_VERIFY_MSG(returnStatus, "Failed to find limb guide pose joints.");

  // Setup ik pos and ori weights
  NMP::Memory::Format ikWeightsFormat(sizeof(float) * limbDef->m_numPartsInChain, NMP_NATURAL_TYPE_ALIGNMENT);
  // pos
  resource.align(ikWeightsFormat);
  limbDef->m_positionWeights = (float*)resource.ptr;
  resource.increment(ikWeightsFormat);  
  returnStatus = limbDefExport->getPositionWeights(limbDef->m_positionWeights, limbDef->m_numPartsInChain);
  NMP_VERIFY_MSG(returnStatus, "Failed to find limb position weights.");
  // ori
  resource.align(ikWeightsFormat);
  limbDef->m_orientationWeights = (float*)resource.ptr;
  resource.increment(ikWeightsFormat);
  returnStatus = limbDefExport->getOrientationWeights(limbDef->m_orientationWeights, limbDef->m_numPartsInChain);
  NMP_VERIFY_MSG(returnStatus, "Failed to find limb orientation weights.");

  // Setup swivel quat arrays
  NMP::Memory::Format swivelQuatsFormat(sizeof(NMP::Quat), NMP_VECTOR_ALIGNMENT);
  swivelQuatsFormat *= limbDef->m_numJointsInChain;
  resource.align(swivelQuatsFormat);
  limbDef->m_zeroSwivelPoseQuats = (NMP::Quat*)resource.ptr;
  resource.increment(swivelQuatsFormat);
  resource.align(swivelQuatsFormat);
  limbDef->m_negativeSwivelPoseQuats = (NMP::Quat*)resource.ptr;
  resource.increment(swivelQuatsFormat);
  resource.align(swivelQuatsFormat);
  limbDef->m_positiveSwivelPoseQuats = (NMP::Quat*)resource.ptr;
  resource.increment(swivelQuatsFormat);

  const ME::HamstringExport* hamstring = limbDefExport->getHamstring();
  limbDef->m_hamstring.m_enabled = hamstring->getEnabled();
  if (limbDef->m_hamstring.m_enabled)
  {
    uint32_t bendScaleWeightCount = limbDef->m_numJointsInChain + 1; // there is a weight for all objects in the limb
    float* bendScaleBuffer = (float*) NMPMemoryAlloc(sizeof(float) * bendScaleWeightCount );
    NMP_ASSERT(bendScaleBuffer);

    limbDef->m_hamstring.m_stiffness = hamstring->getStiffness();
    limbDef->m_hamstring.m_thresholdAngle = hamstring->getThreshold();

    NMP::Memory::Format bendScaleWeightsFormat(sizeof(NMP::Vector3), NMP_VECTOR_ALIGNMENT);
    bendScaleWeightsFormat *= bendScaleWeightCount;
    resource.align(bendScaleWeightsFormat);
    limbDef->m_hamstring.m_bendScaleWeights = (NMP::Vector3*)resource.ptr;
    resource.increment(bendScaleWeightsFormat);

    // The export library outputs three float arrays for the twist, swing1, and swing2.  Format this into the vector
    // array the hamstring code expects
    returnStatus = hamstring->getTwistBendScaleWeights(bendScaleBuffer,  bendScaleWeightCount);
    NMP_VERIFY_MSG(returnStatus, "Failed to get hamstring twist bend scale weights.");

    for (uint32_t i = 0; i < bendScaleWeightCount; ++i)
    {
      limbDef->m_hamstring.m_bendScaleWeights[i].x = bendScaleBuffer[i];
    }
    returnStatus = hamstring->getSwing1BendScaleWeights(bendScaleBuffer, bendScaleWeightCount);
    NMP_VERIFY_MSG(returnStatus, "Failed to get hamstring swing1 bend scale weights.");

    for (uint32_t i = 0; i < bendScaleWeightCount; ++i)
    {
      limbDef->m_hamstring.m_bendScaleWeights[i].y = bendScaleBuffer[i];
    }

    returnStatus = hamstring->getSwing2BendScaleWeights(bendScaleBuffer, bendScaleWeightCount);
    NMP_VERIFY_MSG(returnStatus, "Failed to get hamstring swing2 bend scale weights.");
    for (uint32_t i = 0; i < bendScaleWeightCount; ++i)
    {
      limbDef->m_hamstring.m_bendScaleWeights[i].z = bendScaleBuffer[i];
    }

    NMP::Memory::memFree(bendScaleBuffer);
  }
  else
  {
    limbDef->m_hamstring.m_stiffness = 0;
    limbDef->m_hamstring.m_thresholdAngle = 0;
    limbDef->m_hamstring.m_bendScaleWeights = 0;
  }

  // check that the resource pointer has been advanced by the expected number of bytes
  // 
  NMP_VERIFY(
    (char*)resource.ptr == 
    (char*)limbDef + LimbDefBuilder::getMemoryRequirements(processor, bodyDefExport, limbDefExport).size
    );
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BodyDefBuilder::getMemoryRequirements(
  AP::AssetProcessor*      processor,
  const ME::BodyDefExport* bodyDefExport,
  uint32_t numAnimBones)
{
  NMP::Memory::Format bodyDefFormat(sizeof(ER::BodyDef), NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for default pose data buffer
  NMP::Memory::Format animBufferFormat = NMP::DataBuffer::getPosQuatMemoryRequirements(numAnimBones);
  bodyDefFormat += animBufferFormat;

  // Add space for limb definitions.
  NMP::Memory::Format limbDefPointersFormat(sizeof(ER::LimbDef*), NMP_NATURAL_TYPE_ALIGNMENT);
  limbDefPointersFormat *= bodyDefExport->getNumLimbs();
  bodyDefFormat += limbDefPointersFormat;
  for (uint32_t iLimb = 0; iLimb < bodyDefExport->getNumLimbs(); ++iLimb)
  {
    const ME::LimbDefExport* limbDefExport = bodyDefExport->getLimb(iLimb);
    NMP::Memory::Format limbFormat = LimbDefBuilder::getMemoryRequirements(processor, bodyDefExport, limbDefExport);
    bodyDefFormat += limbFormat;
  }
  return bodyDefFormat;
}

//----------------------------------------------------------------------------------------------------------------------
// Aux for BodyDefBuilder::buildBodyDefAnimPoseData(), initialise the anim pose data.
//
static void initGuidePoseData(
  ER::BodyDef* bodyDef,
  MR::AnimSetIndex                  animSetIndex,
  AP::AssetProcessor*               processor,
  ME::AnimationSetExport*     animSetExport);

static void initDefaultPoseData(
  ER::BodyDef* bodyDef,
  MR::AnimSetIndex                  animSetIndex,
  AP::AssetProcessor*               processor,
  ME::AnimationSetExport*     animSetExport);

//----------------------------------------------------------------------------------------------------------------------
void  AP::BodyDefBuilder::buildBodyDefAnimPoseData(
  ER::BodyDef*            bodyDef,
  MR::AnimSetIndex        animSetIndex,
  AP::AssetProcessor*     processor,
  ME::AnimationSetExport* animSetExport)
{
  initDefaultPoseData(bodyDef, animSetIndex, processor, animSetExport);
  initGuidePoseData(bodyDef, animSetIndex, processor, animSetExport);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource BodyDefBuilder::init(AP::AssetProcessor* processor, 
                                           const ME::BodyDefExport* bodyDefExport, 
                                           uint32_t numAnimBones)
{
  NMP::Memory::Format memFmt = getMemoryRequirements(processor, bodyDefExport, numAnimBones);
  NMP::Memory::Resource result = NMPMemoryAllocateFromFormat(memFmt);

  // cast to a builder since then we have access to the protected data
  NMP::Memory::Resource resource = result;

  NMP::Memory::Format bodyDefShellFormat(sizeof(ER::BodyDef), NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(bodyDefShellFormat); // probably redundant
  ER::BodyDef* bodyDef = (ER::BodyDef*)resource.ptr;
  resource.increment(bodyDefShellFormat);

  bodyDef->m_numLimbs = bodyDefExport->getNumLimbs();
  bodyDef->m_numArmLimbs = bodyDefExport->getNumArmLimbs();
  bodyDef->m_numHeadLimbs = bodyDefExport->getNumHeadLimbs();
  bodyDef->m_numSpineLimbs = bodyDefExport->getNumSpineLimbs();
  bodyDef->m_numLegLimbs = bodyDefExport->getNumLegLimbs();

  // self avoidance
  bodyDefExport->getSelfAvoidance(bodyDef->m_selfAvoidance.radius);

  // default pose animation data
  NMP::Memory::Format dataBufferMemFmt = NMP::DataBuffer::getPosQuatMemoryRequirements(numAnimBones);
  resource.align(dataBufferMemFmt);
  bodyDef->m_defaultPoseAnimData = NMP::DataBuffer::initPosQuat(resource, dataBufferMemFmt, numAnimBones);

  // Block of limb def pointers
  NMP::Memory::Format limbDefPointersMemFmt(sizeof(ER::LimbDef*), NMP_NATURAL_TYPE_ALIGNMENT);
  limbDefPointersMemFmt *= bodyDef->m_numLimbs;
  resource.align(limbDefPointersMemFmt);
  bodyDef->m_limbDefs = (ER::LimbDef**)resource.ptr;
  resource.increment(limbDefPointersMemFmt);

  // Limb def data for each pointer
  for (uint32_t iLimb = 0; iLimb < bodyDef->m_numLimbs; ++iLimb)
  {
    const ME::LimbDefExport* limbDefExport = bodyDefExport->getLimb(iLimb);
    NMP::Memory::Format limbDefFormat = LimbDefBuilder::getMemoryRequirements(processor, bodyDefExport, limbDefExport);
    resource.align(limbDefFormat);
    bodyDef->m_limbDefs[iLimb] = (ER::LimbDef*)resource.ptr;
    LimbDefBuilder::init(resource, processor, bodyDefExport, limbDefExport);

    bodyDef->m_limbDefs[iLimb]->m_bodyDef = bodyDef;
  }

  // Set up the root limb.
  bodyDef->m_rootLimbIndex = (uint32_t) - 1;
  for (uint32_t i = 0; i < bodyDef->m_numLimbs; ++i)
  {
    if (std::string(bodyDef->m_limbDefs[i]->m_name) == bodyDefExport->getRootLimbName())
    {
      bodyDef->m_rootLimbIndex = i;
      bodyDef->m_limbDefs[i]->m_isRootLimb = true;
      break;
    }
  }

  NMP_VERIFY_MSG(bodyDef->m_rootLimbIndex != (uint32_t) - 1, "BodyDefBuilder: Invalid root limb.")

  NMP_VERIFY((char*)resource.ptr == (char*)result.ptr + result.format.size);
  return result;
}


//----------------------------------------------------------------------------------------------------------------------
static void populateAnimToPhysicsMap(
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
        const ME::JointExport* joint = animRig->getJoint(i);
        const ME::PhysicsPartExport* part = physicsRig->getPart(j);

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
// Aux function for setting up NMRU::JointLimits from physX 6DOF data
//----------------------------------------------------------------------------------------------------------------------
void setNMRUJointLimitsFromPhysXSixDOF(
  const MR::PhysicsSixDOFJointDef& jointDef,
  NMRU::JointLimits::Params& jointLimits)
{
  jointLimits.setTwistLeanLimits(
    -jointDef.m_hardLimits.getTwistLimitHigh(),
    -jointDef.m_hardLimits.getTwistLimitLow(),
    jointDef.m_hardLimits.getSwing2Limit(),
    jointDef.m_hardLimits.getSwing1Limit());
}
//----------------------------------------------------------------------------------------------------------------------
// Calculate the extremal swivel poses using the zeroSwivelQuats as a starting/midpoint point.
// Extremal poses are calculated by rotation of the chain from end to base joint.
//
//----------------------------------------------------------------------------------------------------------------------
void calcExtremalSwivelQuats(
  MR::PhysicsRigDef* physicsRigDef,
  ER::LimbDef* limbDef,
  const NMP::Quat* zeroSwivelQuats,
  float swivelSense)
{
  const uint32_t numJointsInChain = limbDef->m_numJointsInChain;

  // Base joint index is one less than the number of parts before the base
  // except for limbs with base part index of zero.
  int32_t baseJointIndex = limbDef->m_numPartsBeforeBase -1;
  if (baseJointIndex < 0)
  {
    baseJointIndex = 0;
  }

  NMP::Quat* posSwivelQuats = limbDef->m_positiveSwivelPoseQuats;
  NMP::Quat* negSwivelQuats = limbDef->m_negativeSwivelPoseQuats;

  // make a joint limits object for clamping and such
  NMRU::JointLimits::Params jointLimits;

  // for now the wrist joint is just what's in the zero swivel quats, rest are calc'd in loop
  posSwivelQuats[numJointsInChain - 1] = negSwivelQuats[numJointsInChain - 1] = zeroSwivelQuats[numJointsInChain - 1];

  for (uint32_t i = 0, N = numJointsInChain - 1; i < N; ++i)
  {
    // just a straight copy
    posSwivelQuats[i] = negSwivelQuats[i] = zeroSwivelQuats[i];
  }

  // Add reach vector swivel to the extremally swivelled poses.
  // (Note: we iterate backwards (for efficiency) from penultimate joint (no point swivelling round
  // the end joint unless we include the end effector which we aren't at the moment) to the base.
  //

  // Coords of the end frame wrt to the next fixed frame
  NMP::Matrix34 jjEndTMjjnext;
  // on first itr jjnext and jjend are the same frame
  jjEndTMjjnext.identity();
  for (int32_t i = numJointsInChain - 2; i >= baseJointIndex; --i)
  {
    // Grab the relevant sixdofs (this joint's moving frame and next joint's fixed frame)
    MR::PhysicsJointDef* joint_i = physicsRigDef->m_joints[limbDef->getPhysicsRigJointIndex(i)];
    MR::PhysicsJointDef* joint_iplusone = physicsRigDef->m_joints[limbDef->getPhysicsRigJointIndex(i + 1)];
    NMP_ASSERT(joint_i->m_jointType == MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF);
    NMP_ASSERT(joint_iplusone->m_jointType == MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF);
    MR::PhysicsSixDOFJointDef* sixDOF_i = (MR::PhysicsSixDOFJointDef*)joint_i;
    MR::PhysicsSixDOFJointDef* sixDOF_iplusone = (MR::PhysicsSixDOFJointDef*)joint_iplusone;

    // We want the coords of the last fixed frame in the chain jjEnd in coords of the ith fixed frame jj_i: jjEndjj_i
    // Say this joint's moving frame is j_i and next joint's fixed frame is jjnext, we want
    // the TM of jjnext wrt j_i, ie: jjnextTMj_i = jjnextTMb . bTMj_i)
    NMP::Matrix34 jjnextTMj_i, jjnextTMb, bTMj_i;
    bTMj_i.invertFast(sixDOF_i->m_childPartFrame);
    jjnextTMb = sixDOF_iplusone->m_parentPartFrame;
    jjnextTMj_i.multiply(jjnextTMb, bTMj_i);

    // Joint transform j_i coords in jj_i frame (i.e the "joint state")
    NMP::Matrix34 j_iTMjj_i;
    j_iTMjj_i.fromQuat(posSwivelQuats[i]);
    j_iTMjj_i.translation().setToZero();

    // jjnext in frame of jj_i: jjnextTMjj_i
    NMP::Matrix34 jjnextTMjj_i;
    jjnextTMjj_i.multiply(jjnextTMj_i, j_iTMjj_i);

    // jjEnd in frame of previous jj (which btw since we are going backwards towards root is jjnext frame in this
    // iteration)
    NMP::Matrix34 jjEndTMjj_i;
    jjEndTMjj_i.multiply(jjEndTMjjnext, jjnextTMjj_i);

    // Also want jjEndTMj_i = jjEndTMjjnext . jjnextTMj_i
    NMP::Matrix34 jjEndTMj_i;
    jjEndTMj_i.multiply(jjEndTMjjnext, jjnextTMj_i);

    // Reach axis is the position component of the end joint in jj_i coords
    // we'll rotate the current joint a quarter circle about that
    NMP::Vector3 axis = jjEndTMjj_i.translation();
    axis.normalise();

    // Clamped rotation
    setNMRUJointLimitsFromPhysXSixDOF(*sixDOF_i, jointLimits);
    NMRU::JointLimits::toBoundarySimple(NMRU::JointLimits::Params(jointLimits, 0.999f), posSwivelQuats[i],
      axis * swivelSense, posSwivelQuats[i]);
    NMP_ASSERT(NMRU::JointLimits::isViolatedSimple(jointLimits, posSwivelQuats[i]) == 0);

    // and the end TM
    j_iTMjj_i.fromQuat(posSwivelQuats[i]);
    j_iTMjj_i.translation().setToZero();
    jjEndTMjj_i.multiply(jjEndTMj_i, j_iTMjj_i);

    // for next itr
    jjEndTMjjnext = jjEndTMjj_i;
  }

  // Same again but for the other extreme.
  jjEndTMjjnext.identity();
  for (int32_t i = numJointsInChain - 2; i >= baseJointIndex; --i)
  {
    // Grab the relevant sixdofs (this joint's moving frame and next joint's fixed frame)
    MR::PhysicsJointDef* joint_i = physicsRigDef->m_joints[limbDef->getPhysicsRigJointIndex(i)];
    MR::PhysicsJointDef* joint_iplusone = physicsRigDef->m_joints[limbDef->getPhysicsRigJointIndex(i + 1)];
    NMP_ASSERT(joint_i->m_jointType == MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF);
    NMP_ASSERT(joint_iplusone->m_jointType == MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF);
    MR::PhysicsSixDOFJointDef* sixDOF_i = (MR::PhysicsSixDOFJointDef*)joint_i;
    MR::PhysicsSixDOFJointDef* sixDOF_iplusone = (MR::PhysicsSixDOFJointDef*)joint_iplusone;

    // We want the coords of the last fixed frame in the chain jjEnd in coords of the ith fixed frame jj_i: jjEndjj_i
    // Say this joint's moving frame is j_i and next joint's fixed frame is jjnext, we want
    // the TM of jjnext wrt j_i, ie: jjnextTMj_i = jjnextTMb . bTMj_i)
    NMP::Matrix34 jjnextTMj_i, jjnextTMb, bTMj_i;
    bTMj_i.invertFast(sixDOF_i->m_childPartFrame);
    jjnextTMb = sixDOF_iplusone->m_parentPartFrame;
    jjnextTMj_i.multiply(jjnextTMb, bTMj_i);

    // Joint transform j_i coords in jj_i frame (i.e the "joint state")
    NMP::Matrix34 j_iTMjj_i;
    j_iTMjj_i.fromQuat(negSwivelQuats[i]);
    j_iTMjj_i.translation().setToZero();

    // jjnext in frame of jj_i: jjnextTMjj_i
    NMP::Matrix34 jjnextTMjj_i;
    jjnextTMjj_i.multiply(jjnextTMj_i, j_iTMjj_i);

    // jjEnd in frame of previous jj (which btw since we are going backwards towards root is jjnext frame in this
    // iteration)
    NMP::Matrix34 jjEndTMjj_i;
    jjEndTMjj_i.multiply(jjEndTMjjnext, jjnextTMjj_i);

    // Also want jjEndTMj_i = jjEndTMjjnext . jjnextTMj_i
    NMP::Matrix34 jjEndTMj_i;
    jjEndTMj_i.multiply(jjEndTMjjnext, jjnextTMj_i);

    // Reach axis is the position component of the end joint in jj_i coords
    // we'll rotate the current joint a quarter circle about that
    NMP::Vector3 axis = jjEndTMjj_i.translation();
    axis.normalise();

    // Clamped rotation
    setNMRUJointLimitsFromPhysXSixDOF(*sixDOF_i, jointLimits);
    NMRU::JointLimits::toBoundarySimple(NMRU::JointLimits::Params(jointLimits, 0.999f), negSwivelQuats[i],
      -axis * swivelSense, negSwivelQuats[i]);
    NMP_ASSERT(NMRU::JointLimits::isViolatedSimple(jointLimits, negSwivelQuats[i]) == 0);

    // and the end TM
    j_iTMjj_i.fromQuat(negSwivelQuats[i]);
    j_iTMjj_i.translation().setToZero();
    jjEndTMjj_i.multiply(jjEndTMj_i, j_iTMjj_i);

    // for next itr
    jjEndTMjjnext = jjEndTMjj_i;
  }
}

//----------------------------------------------------------------------------------------------------------------------
static NMP::DataBuffer* constructAnimationPose(
  MR::AnimSetIndex                  animSetIndex,
  AP::AssetProcessor*               processor,
  const ME::AnimationSetExport*     animSetExport,
  const ME::AnimationPoseDefExport* sourcePose)
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

    std::vector<const char*> addedAnimChannels;

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

    std::vector<const char*> addedAnimChannels;

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

  // put the sorted anim data into a data buffer
  //
  NMP::Memory::Format animPoseMemReqs = NMP::DataBuffer::getPosQuatMemoryRequirements(numChannelsInRig);
  NMP::Memory::Resource memResAnimPose = NMPMemoryAllocateFromFormat(animPoseMemReqs);
  NMP_VERIFY(memResAnimPose.ptr);
  NMP::DataBuffer* animPoseDataBuffer = NMP::DataBuffer::initPosQuat(memResAnimPose, animPoseMemReqs, numChannelsInRig);

  for (uint32_t i = 0; i < numChannelsInRig; ++i)
  {
    animPoseDataBuffer->setChannelPos(i, sortedPositions[i]);
    animPoseDataBuffer->setChannelQuat(i, sortedOrientations[i]);
  }

  // Free the temporary buffers we have created.
  NMP::Memory::memFree(sourcePositions);
  NMP::Memory::memFree(sourceOrientations);
  NMP::Memory::memFree(sortedPositions);
  NMP::Memory::memFree(sortedOrientations);
  NMP::Memory::memFree(rigToAnimEntryMap);
  NMP::Memory::memFree(uncompressedSourceAnim);

  return animPoseDataBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AnimToPhysicsMap* constructAnimToPhysicsMap(const ME::AnimationSetExport* animSetExport)
{
  const ME::PhysicsRigDefExport* physicsRigDefExport = animSetExport->getPhysicsRigDef();
  const ME::RigExport *animRigExport = animSetExport->getRig();

  // Make a physics to anim map (for correspondence between anim rig and physics rig)
  //
  uint32_t numRigChannels = animRigExport->getNumJoints();
  uint32_t numPhysicsParts = physicsRigDefExport->getNumParts();
  NMP::Memory::Format memReqsAnimToPhysicsMap = MR::AnimToPhysicsMap::getMemoryRequirements(numRigChannels, numPhysicsParts);
  NMP::Memory::Resource memResAnimToPhysicsMap = NMPMemoryAllocateFromFormat(memReqsAnimToPhysicsMap);
  NMP_VERIFY(memResAnimToPhysicsMap.ptr);
  MR::AnimToPhysicsMap* animToPhysicsMap = MR::AnimToPhysicsMap::init(memResAnimToPhysicsMap, numRigChannels, numPhysicsParts);
  populateAnimToPhysicsMap(animToPhysicsMap, animRigExport, physicsRigDefExport);
  return animToPhysicsMap;
}

//----------------------------------------------------------------------------------------------------------------------
static NMP::Quat getLimitedJointQuat(
  int animBoneIndex, 
  const NMP::Quat &inputQuat, 
  float limitFrac,
  const MR::AnimToPhysicsMap* animToPhysicsMap,
  const MR::AnimRigDef* animRigDef,
  const MR::PhysicsRigDef* physicsRigDef)
{
  // Result will be a (possibly clamped) copy of the input quat
  NMP::Quat qResult = inputQuat;

  // Get the index of the physics part that corresponds to the given anim bone index
  // If it exists this will be the part attached to the moving frame of the joint (the "child part")
  // 
  int32_t physicsChildPartIndex = animToPhysicsMap->getPhysicsIndexFromAnimIndex(animBoneIndex);
  if(physicsChildPartIndex != -1)
  {
    // Get the index of the parent animation bone
    int32_t animParentBoneIndex = animRigDef->getParentBoneIndex(animBoneIndex);
    if(animParentBoneIndex != -1)
    {
      // If the animation parent bone exists and has a corresponding physics part, this is the part the joint 
      // moves with, (i.e. the joint's "parent part")
      int32_t physicsParentPartIndex = animToPhysicsMap->getPhysicsIndexFromAnimIndex(animParentBoneIndex);
      for(uint32_t physicsJointIndex = 0; physicsJointIndex < physicsRigDef->getNumJoints(); ++physicsJointIndex)
      {
        // We have found the joint if it's parent and child part indexes match the ones we have just found
        const MR::PhysicsJointDef* joint = physicsRigDef->m_joints[physicsJointIndex];
        if (joint->m_parentPartIndex == physicsParentPartIndex && joint->m_childPartIndex == physicsChildPartIndex)
        {
          switch (joint->m_jointType)
          {
          case MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF:
            {
              const MR::PhysicsSixDOFJointDef* sixDOFJoint =
                static_cast<const MR::PhysicsSixDOFJointDef*>(joint);
              sixDOFJoint->m_hardLimits.getModifiableLimits().clampToLimits(qResult, limitFrac, *joint);
              break;
            }
          default:
            NMP_ASSERT_FAIL_MSG(
              "unhandled joint type %d encountered when calling getLimitedJointQuat() when building the Euphoria BodyDef.",
              joint->m_jointType);
            break;
          }
        }
      }
    }
  }
  return qResult;
}

//----------------------------------------------------------------------------------------------------------------------
static void calculateWorldSpacePartTM(
  NMP::Matrix34&                TMout,
  int32_t                       physicsPartIndex,
  const NMP::DataBuffer*        inputBuffer,
  const MR::AnimToPhysicsMap*   animToPhysicsMap,
  const MR::AnimRigDef*         animRigDef,
  const MR::PhysicsRigDef*      physicsRigDef, 
  bool                          enforceLimits)
{
  NMP_ASSERT(animToPhysicsMap);
  int32_t animPartIndex = animToPhysicsMap->getAnimIndexFromPhysicsIndex(physicsPartIndex);
  NMP_ASSERT(animPartIndex >= 0);  

  if(enforceLimits)
  {
    TMout.initialise(
      getLimitedJointQuat(
      animPartIndex, *(inputBuffer->getChannelQuat(animPartIndex)), 1.0f,
      animToPhysicsMap, animRigDef, physicsRigDef
      ), 
      *(inputBuffer->getChannelPos(animPartIndex)) );

    while ((animPartIndex = (int32_t)animRigDef->getParentBoneIndex(animPartIndex)) > 0)
    {
      // The reference transforms are kept between physics nodes. Just animate physics joints!
      int32_t k = animToPhysicsMap->getPhysicsIndexFromAnimIndex(animPartIndex);
      if (k != -1)
      {
        NMP::Matrix34 T(
          getLimitedJointQuat(animPartIndex, *(inputBuffer->getChannelQuat(animPartIndex)), 1.0f,
          animToPhysicsMap, animRigDef, physicsRigDef
          ),
          *(inputBuffer->getChannelPos(animPartIndex)) );
        TMout.multiply(T);
      }
      else
      {
        NMP::Matrix34 T(*animRigDef->getBindPoseBoneQuat(animPartIndex), *animRigDef->getBindPoseBonePos(animPartIndex));
        TMout.multiply(T);
      }
    }
  }
  else
  {
    TMout.initialise(*inputBuffer->getChannelQuat(animPartIndex), *inputBuffer->getChannelPos(animPartIndex));

    while ((animPartIndex = (int32_t)animRigDef->getParentBoneIndex(animPartIndex)) > 0)
    {
      // The reference transforms are kept between physics nodes. Just animate physics joints!
      int32_t k = animToPhysicsMap->getPhysicsIndexFromAnimIndex(animPartIndex);
      if (k != -1)
      {
        NMP::Matrix34 T(*inputBuffer->getChannelQuat(animPartIndex), *inputBuffer->getChannelPos(animPartIndex));
        TMout.multiply(T);
      }
      else
      {
        NMP::Matrix34 T(*animRigDef->getBindPoseBoneQuat(animPartIndex), *animRigDef->getBindPoseBonePos(animPartIndex));
        TMout.multiply(T);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void setDefaultPose(
  ER::LimbDef* limbDef,
  const NMP::DataBuffer* defaultPose, 
  const MR::AnimToPhysicsMap*   animToPhysicsMap,
  const MR::AnimRigDef* animRigDef,
  const MR::PhysicsRigDef* physicsRigDef)
{
  NMP::Matrix34 rootBindTM, baseBindTM, endBindTM;

  NMP::Matrix34 worldRoot(NMP::Matrix34::kIdentity);
  calculateWorldSpacePartTM(rootBindTM, limbDef->m_rootIndex, defaultPose,
    animToPhysicsMap,
    animRigDef,
    physicsRigDef,
    false);
  calculateWorldSpacePartTM(baseBindTM, limbDef->m_baseIndex, defaultPose,
    animToPhysicsMap,
    animRigDef,
    physicsRigDef,
    false);
  calculateWorldSpacePartTM(endBindTM, limbDef->m_endIndex, defaultPose,
    animToPhysicsMap,
    animRigDef,
    physicsRigDef,
    false);

  // The root and end parts in world space after the offset
  NMP::Matrix34 r, e;
  r.multiply(limbDef->m_rootOffset, rootBindTM);
  e.multiply(limbDef->m_endOffset,  endBindTM);
  // want e in r's space
  r.invert();

  limbDef->m_defaultPoseEndRelativeToRoot = e * r;
  limbDef->m_defaultPoseEndRelativeToRoot.orthonormalise();
  // store in format that is more commonly used, so requires less conversions
  limbDef->m_defaultPoseEndQuatRelativeToRoot = limbDef->m_defaultPoseEndRelativeToRoot.toQuat();
  limbDef->m_defaultPoseEndQuatRelativeToRoot.normalise();
  limbDef->m_defaultPoseLimbLength = NMP::vDistanceBetween(e.translation(), baseBindTM.translation());

  // and the zero pose
  // Calculate the zero pose end offset from the root
  NMP::Matrix34 rootZeroTM = physicsRigDef->getZeroPoseTMRelRoot(limbDef->m_rootIndex);
  NMP::Matrix34 endZeroTM = physicsRigDef->getZeroPoseTMRelRoot(limbDef->m_endIndex);

  // The root and end parts in world space after the offset
  r.multiply(limbDef->m_rootOffset, rootZeroTM);
  e.multiply(limbDef->m_endOffset,  endZeroTM);
  // want e in r's space
  r.invert();
  limbDef->m_zeroPoseEndRelativeToRoot = e * r;
  limbDef->m_zeroPoseEndRelativeToRoot.orthonormalise();
}

//----------------------------------------------------------------------------------------------------------------------
static void initDefaultPoseData(
  ER::BodyDef*            bodyDef,
  MR::AnimSetIndex        animSetIndex,
  AP::AssetProcessor*     processor,
  ME::AnimationSetExport* animSetExport)
{
  // Extract the DefaultPose animation data into a DataBuffer
  const ME::AnimationPoseDefExport* defaultPose = animSetExport->getBodyDef()->getAnimationPose("DefaultPose");
  NMP::DataBuffer *animDataBuffer = NULL;
  if(defaultPose)
  {
    animDataBuffer = constructAnimationPose(animSetIndex, processor, animSetExport, defaultPose);
  }
  // We need to have successfully got this buffer to be able to continue
  if(!animDataBuffer) 
    return;

  // Make a physics to anim map (for correspondence between anim rig data and physics rig)
  MR::AnimToPhysicsMap* animToPhysicsMap = constructAnimToPhysicsMap(animSetExport);

  // Get the physics rig def (for info regarding the joints i.e. local transforms wrt parts and limits)
  const ME::PhysicsRigDefExport* physicsRigDefExport = animSetExport->getPhysicsRigDef();
  MR::PhysicsRigDef* physicsRigDef = NULL;
  if (physicsRigDefExport)
  {
    const ProcessedAsset* physicsRigDefAsset = processor->findProcessedAsset(physicsRigDefExport->getGUID());
    if (physicsRigDefAsset)
      physicsRigDef = (MR::PhysicsRigDef*) physicsRigDefAsset->assetMemory.ptr;
  }

  const ME::RigExport *animRigExport = animSetExport->getRig();
  const ProcessedAsset* animRigAsset = processor->findProcessedAsset(animRigExport->getGUID());
  NMP_VERIFY_MSG(animRigAsset, "Could not find anim rig for anim set %i", animSetIndex);
  MR::AnimRigDef* animRigDef = (MR::AnimRigDef*) animRigAsset->assetMemory.ptr;

  // Distribute results into each limb def
  //
  if(bodyDef)
  {
    // Write the anim data buffer to the place in the body def
    animDataBuffer->copyTo(bodyDef->m_defaultPoseAnimData);

    uint32_t numLimbs = bodyDef->getNumLimbs();

    for(uint32_t iLimb = 0; iLimb < numLimbs; ++iLimb)
    {
      ER::LimbDef* limbDef = bodyDef->m_limbDefs[iLimb];
      NMP_ASSERT(limbDef);
      if(limbDef)
      {
        //setDefaultPose(limbDef, bodyDef->m_defaultPoseAnimData, animToPhysicsMap, animRigDef, physicsRigDef);
        setDefaultPose(limbDef, animDataBuffer, animToPhysicsMap, animRigDef, physicsRigDef);
      }
    }
  }
  NMP::Memory::memFree(animDataBuffer);
  NMP::Memory::memFree(animToPhysicsMap);
}

//----------------------------------------------------------------------------------------------------------------------
static float getSwivelSense(const ER::BodyDef *bodyDef, const ER::LimbDef* limbDef, int limbIndex)
{
  // Heads (no swivel)
  if (
    bodyDef->m_numHeadLimbs > 0 && 
    NMP::valueInRange(limbIndex, bodyDef->getFirstHeadLimbIndex(), bodyDef->getLastHeadLimbIndex())
    )
  {
    return 0;
  }
  // Spines (no swivel)
  else if (
    bodyDef->m_numSpineLimbs > 0 &&
    NMP::valueInRange(limbIndex, bodyDef->getFirstSpineLimbIndex(), bodyDef->getLastSpineLimbIndex())
    )
  {
    return 0;
  }
  // Arms  (left/right swivel sense)
  else if (
    bodyDef->m_numArmLimbs > 0 &&
    NMP::valueInRange(limbIndex, bodyDef->getFirstArmLimbIndex(), bodyDef->getLastArmLimbIndex())
    )
  {
    NMP::Vector3 defaultEndPosWRTRoot = limbDef->m_defaultPoseEndRelativeToRoot.translation();
    return (defaultEndPosWRTRoot.z < 0.0f) ? 1.0f : -1.0f;
  }
  // Legs  (left/right swivel sense)
  else if (
    bodyDef->m_numLegLimbs > 0 && 
    NMP::valueInRange(limbIndex, bodyDef->getFirstLegLimbIndex(), bodyDef->getLastLegLimbIndex())
    )
  {
    NMP::Vector3 defaultEndPosWRTRoot = limbDef->m_defaultPoseEndRelativeToRoot.translation();
    return (defaultEndPosWRTRoot.z < 0.0f) ? 1.0f : -1.0f;
  }

  NMP_ASSERT_FAIL_MSG("Unexpected limb index.");
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
static void initGuidePoseData(
  ER::BodyDef*            bodyDef,
  MR::AnimSetIndex        animSetIndex,
  AP::AssetProcessor*     processor,
  ME::AnimationSetExport* animSetExport)
{
  // Extract the GuidePose animation data into a DataBuffer
  //
  const ME::AnimationPoseDefExport* guidePose = animSetExport->getBodyDef()->getAnimationPose("GuidePose");
  NMP::DataBuffer *animDataBuffer = NULL;
  if(guidePose)
  {
    animDataBuffer = constructAnimationPose(animSetIndex, processor, animSetExport, guidePose);
  }
  // We need to have successfully got this buffer to be able to continue
  if(!animDataBuffer) 
    return;

  // Convert the anim data to physics rig joint angle data
  //

  // Make a physics to anim map (for correspondence between anim rig data and physics rig)
  MR::AnimToPhysicsMap* animToPhysicsMap = constructAnimToPhysicsMap(animSetExport);

  // Get the physics rig def (for info regarding the joints i.e. local transforms wrt parts and limits)
  const ME::PhysicsRigDefExport* physicsRigDefExport = animSetExport->getPhysicsRigDef();
  MR::PhysicsRigDef* physicsRigDef = NULL;
  if (physicsRigDefExport)
  {
    const ProcessedAsset* physicsRigDefAsset = processor->findProcessedAsset(physicsRigDefExport->getGUID());
    if (physicsRigDefAsset)
      physicsRigDef = (MR::PhysicsRigDef*) physicsRigDefAsset->assetMemory.ptr;
  }

  // Calculate and clamp pose joint angles for the entire rig
  //
  uint32_t numPhysicsJoints = physicsRigDefExport->getNumJoints();
  NMP::Quat* quat = (NMP::Quat*)NMPMemoryAlloc(sizeof(NMP::Quat) * numPhysicsJoints);
  NMP_ASSERT(quat);
  for(uint32_t i = 0; i < numPhysicsJoints; ++i)
  {
    const MR::PhysicsJointDef* joint = physicsRigDef->m_joints[i];
    NMP_ASSERT(joint->m_jointType == MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF);
    const MR::PhysicsSixDOFJointDef* sixdof = static_cast<const MR::PhysicsSixDOFJointDef*>(joint);
    NMP::Quat l0(joint->m_parentPartFrame.toQuat());
    NMP::Quat l1(joint->m_childPartFrame.toQuat());
    int32_t animIndexFromPhysicsIndex = animToPhysicsMap->getAnimIndexFromPhysicsIndex(joint->m_childPartIndex);
    quat[i] = ~l0 * *(animDataBuffer->getChannelQuat(animIndexFromPhysicsIndex)) * l1;
    NMRU::JointLimits::Params jointLimits;
    jointLimits.setTwistLeanLimits(
      -sixdof->m_hardLimits.getTwistLimitHigh(),
      -sixdof->m_hardLimits.getTwistLimitLow(),
      sixdof->m_hardLimits.getSwing2Limit(),
      sixdof->m_hardLimits.getSwing1Limit());
    NMRU::JointLimits::clampSimple(NMRU::JointLimits::Params(jointLimits, 0.999f), quat[i]);
  }

  // Distribute the above results into each limb def
  //
  if(bodyDef)
  {
    uint32_t numLimbs = bodyDef->getNumLimbs();

    for(uint32_t iLimb = 0; iLimb < numLimbs; ++iLimb)
    {
      ER::LimbDef* limbDef = bodyDef->m_limbDefs[iLimb];
      NMP_ASSERT(limbDef);
      if(limbDef)
      {
        // calc zero swivel quats
        uint32_t numJoints = limbDef->m_numJointsInChain;
        for(uint32_t iJoint = 0; iJoint < numJoints; ++iJoint)
        {
          uint32_t rigJointIndex = limbDef->getPhysicsRigJointIndex(iJoint);
          limbDef->m_zeroSwivelPoseQuats[iJoint] = quat[rigJointIndex];
        }
        // Calc and distribute extremal swivel quats to respective limb defs
        calcExtremalSwivelQuats(physicsRigDef,
          limbDef,
          limbDef->m_zeroSwivelPoseQuats,
          getSwivelSense(bodyDef, limbDef, iLimb)
          );
      }
    }
  }
  NMP::Memory::memFree(animDataBuffer);
  NMP::Memory::memFree(animToPhysicsMap);
  NMP::Memory::memFree(quat);
}


} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
