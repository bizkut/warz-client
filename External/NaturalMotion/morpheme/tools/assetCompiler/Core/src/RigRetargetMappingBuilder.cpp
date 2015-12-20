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
#include <cstring> // Part of the temporary hard-coding of mappings - to be removed

#include "RigRetargetMappingBuilder.h"
#include "morpheme/mrAttribData.h"
#include "NMRetarget/NMFKRetarget.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
void RigRetargetMappingBuilder::init(
  MR::AttribDataRigRetargetMapping *retargetMapping,
  const ME::RigExport* rigExport,
  const ME::AnimationSetExport* NMP_UNUSED(animSetExport),
  const std::map<std::string, int32_t>& retargetMap)
{
  // Set overall rig scale
  retargetMapping->m_retargetParams->rigScale = rigExport->getRigRetargetScale();

  // Set mapping and offsets
  NMRU::FKRetarget::PerJointParams* perJointParams = retargetMapping->m_retargetParams->perJointParams;
  uint32_t numJoints = rigExport->getNumJoints();
  int32_t*& indexMap = retargetMapping->m_retargetParams->indexMap;
  //
  // Go through every joint and retrieve the offset geometry and the index mapping that maps that joint to
  // joints in other rigs (passed in).
  std::string tempMapString;
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    const ME::JointExport* jointExport = rigExport->getJoint(j);
    NMRU::FKRetarget::PerJointParams& perJoint = perJointParams[j];

    // The retargeting tag is a string that identifies the retarget mapping group to which this joint
    // belongs.  The retargetMap passed in allows me to convert this to an index for storage in the runtime.
    const char *tag = jointExport->getRetargetingTag();
    if (tag)
    {
      tempMapString = tag;
      if(!tempMapString.empty())
      {
        std::map<std::string, int32_t>::const_iterator it = retargetMap.find(tempMapString);
        if (it != retargetMap.end())
        {
          indexMap[j] = it->second;
        }
      }
    }

    // extract the offset and reference information into the params
    rigExport->getJointOffsetTransform(j, perJoint.offsetTransform.t, perJoint.offsetTransform.q);
    rigExport->getJointPostOffsetTransform(j, perJoint.referenceTransform.q);
  }

  // All the bones are rigid but this isn't the case for the hip joint and any joints above it in the
  // hierarchy, or for the trajectory joint, so mark them as non-rigid
  uint32_t nonRigid = rigExport->getHipIndex();
  while (nonRigid >= 0 && nonRigid < numJoints)
  {
    perJointParams[nonRigid].isRigid = false;

    const ME::JointExport* joint = rigExport->getJoint(nonRigid);

    NMP_VERIFY(joint);
    if(!joint)
    {
      break;
    }

    nonRigid = joint->getParentIndex();
  }
  //
  // ...same for the trajectory joint.
  uint32_t trajectoryIndex = rigExport->getTrajectoryIndex();
  NMP_VERIFY(trajectoryIndex >= 0 && trajectoryIndex < numJoints);
  if(trajectoryIndex >= 0 && trajectoryIndex < numJoints)
  {
    perJointParams[trajectoryIndex].isRigid = false;
  }

  // Joint limits
  initJointLimits(retargetMapping, rigExport);

  // Check for invalid mappings that have made it through export-size verification
  uint32_t longestSequence = 1;
  uint32_t numMappedJoints = 0;
  for (uint32_t j = 0; j < rigExport->getNumJoints(); ++j)
  {
    int32_t i = retargetMapping->m_retargetParams->indexMap[j];
    if (i >= 0) // Joint is mapped
    {
      NMP_VERIFY_MSG(i < (signed)retargetMap.size(), "Invalid retarget mapping");
      ++numMappedJoints;
      int32_t jj = rigExport->getJoint(j)->getParentIndex();
      uint32_t thisSequenceLength = 1;
      while (jj >= 0 && retargetMapping->m_retargetParams->indexMap[jj] == i)
      {
        ++thisSequenceLength;
        jj = rigExport->getJoint(jj)->getParentIndex();
      }
      if (thisSequenceLength > longestSequence)
      {
        longestSequence = thisSequenceLength;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RigRetargetMappingBuilder::initJointLimits(
  MR::AttribDataRigRetargetMapping *retargetMapping,
  const ME::RigExport *rigExport)
{
  for (uint32_t l = 0; l != rigExport->getNumJointLimits(); ++l)
  {
    const ME::JointLimitExport* jointLimit = rigExport->getJointLimit(l);
    uint32_t j = jointLimit->getJointIndex();

    NMP_VERIFY(j < rigExport->getNumJoints());

    // Get the joint limits, as well as noting that this joint is limited
    retargetMapping->m_retargetParams->perJointParams[j].isLimited = true;

    NMRU::JointLimits::Params& limitParams = retargetMapping->m_retargetParams->perJointParams[j].limits;
    jointLimit->getOrientation(limitParams.frame.x, limitParams.frame.y, limitParams.frame.z, limitParams.frame.w);
    jointLimit->getOffsetOrientation(
      limitParams.offset.x, limitParams.offset.y, limitParams.offset.z, limitParams.offset.w);

    // NOTE  We need to swap the limits over to make them comply with the nomenclature as used in
    // connect.  This is different from the way joint limits are implemented in the morpheme runtime.
    float twistLow = jointLimit->getTwistLow();
    float twistHigh = jointLimit->getTwistHigh();
    if (jointLimit->getLimitType() == ME::JointLimitExport::kHingeLimitType)
    {
      limitParams.setTwistLeanLimits(-twistHigh, -twistLow, 0, 0);
    }
    else
    {
      limitParams.setTwistLeanLimits(-twistHigh, -twistLow, jointLimit->getSwing2(), jointLimit->getSwing1());
    }
  }
}


} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
