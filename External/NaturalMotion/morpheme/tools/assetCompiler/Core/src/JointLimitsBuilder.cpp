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
#include "JointLimitsBuilder.h"

#include "NMGeomUtils/NMJointLimits.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
void JointLimitsBuilder::init(
  MR::AttribDataJointLimits* limitsAttrib,
  const ME::RigExport* rigExport)
{
  uint32_t numRigJoints = limitsAttrib->getNumRigJoints();
  uint32_t numLimitedJoints = limitsAttrib->getNumJointLimits();
  NMP_VERIFY(numRigJoints == rigExport->getNumJoints());
  NMP_VERIFY(numLimitedJoints == rigExport->getNumJointLimits());

  // Retrieve joint limit data and store
  for (uint32_t l = 0; l < numLimitedJoints; ++l)
  {
    const ME::JointLimitExport* jointLimitExport = rigExport->getJointLimit(l);

    // Store pointer in joint limit params pointer array
    uint32_t jointIndex = jointLimitExport->getJointIndex();
    NMP_VERIFY(jointIndex < numRigJoints);

    limitsAttrib->m_jointLimits[jointIndex] = &(limitsAttrib->m_jointLimitData[l]);

    NMRU::JointLimits::Params& limitParams = limitsAttrib->m_jointLimitData[l];

    jointLimitExport->getOrientation(
      limitParams.frame.x, limitParams.frame.y, limitParams.frame.z, limitParams.frame.w);
    jointLimitExport->getOffsetOrientation(
      limitParams.offset.x, limitParams.offset.y, limitParams.offset.z, limitParams.offset.w);

    // NOTE  We need to swap the limits over to make them comply with the nomenclature as used in
    // connect.  This is different from the way joint limits are implemented in the morpheme runtime.
    if (jointLimitExport->getLimitType() == ME::JointLimitExport::kHingeLimitType)
    {
      limitParams.setTwistLeanLimits(-jointLimitExport->getTwistHigh(), -jointLimitExport->getTwistLow(), 0, 0);
    }
    else
    {
      limitParams.setTwistLeanLimits(
        -jointLimitExport->getTwistHigh(), -jointLimitExport->getTwistLow(),
        jointLimitExport->getSwing2(), jointLimitExport->getSwing1());
    }
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
