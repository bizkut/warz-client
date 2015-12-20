// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "euphoria/erDebugControls.h"

#include "euphoria/erCharacter.h"
#include "euphoria/erBody.h"
#include "euphoria/erContactFeedback.h"
#include "euphoria/erGravityCompensation.h"
#include "euphoria/erLimb.h"
#include "euphoria/erLimbIK.h"

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
struct DebugControlInfo
{
  DebugControlID       m_controlID;
  const char*          m_controlName;
  DebugControlFlags    m_controlFlags;
  DebugControlDataType m_controlDataType;
};

//----------------------------------------------------------------------------------------------------------------------
const DebugControlInfo g_debugControlsArray[] = {
  // Debug Controls
  { kIKSolverIterationsControlID, "IK Solver Iterations", kPerLimbControlFlag, kInt32ControlDataType },
  { kGravityCompensationControlID, "Gravity Compensation", kNoControlFlag, kBoolControlDataType },
  { kEnableJointLimitsControlID, "Hard Limits", kNoControlFlag, kBoolControlDataType },
  { kEnableSoftLimitsControlID, "Soft Limits", kNoControlFlag, kBoolControlDataType },
  { kEnableHamstringsControlID, "Hamstrings", kNoControlFlag, kBoolControlDataType },
  { kIKDrawingRelativeToRootControlID, "IK Drawing Relative To Root", kPerLimbControlFlag, kBoolControlDataType },
  { kIKUsesJointLimits, "IK Uses Joint Limits", kPerLimbControlFlag, kBoolControlDataType },

  // Debug Draw
  { kDrawGravityCompensationTorquesControlID, "Draw Gravity Compensation Torques", kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawGravityCompensationGravityControlID, "Draw Gravity Compensation Gravity", kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawHamstringsControlID, "Draw Hamstrings", kPerLimbControlFlag | kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawIKTargetsControlID, "Draw IK Targets", kPerLimbControlFlag | kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawIKFinalResultsControlID, "Draw IK Final Results", kPerLimbControlFlag | kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawContactsControlID, "Draw Contacts", kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawDetailedContactsControlID, "Draw Detailed Contacts", kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawCharacterRootControlID, "Draw Character Root", kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawIKInputPoseControlID, "Draw IK Input Pose", kPerLimbControlFlag | kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawIKGuidePoseControlID, "Draw IK Guide Pose", kPerLimbControlFlag | kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawGuidePoseRangeControlID, "Draw Guide Pose Range", kPerLimbControlFlag | kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawGuidePoseExtremesControlID, "Draw Guide Pose Extremes", kPerLimbControlFlag | kDebugDrawControlFlag, kBoolControlDataType },
  { kDrawEndContraintControlID, "Draw End Constraint", kPerLimbControlFlag | kDebugDrawControlFlag, kBoolControlDataType },
};

//----------------------------------------------------------------------------------------------------------------------
const size_t g_debugControlsCount = sizeof(g_debugControlsArray) / sizeof(DebugControlInfo);

//----------------------------------------------------------------------------------------------------------------------
uint32_t getEuphoriaDebugControlCount()
{
  return g_debugControlsCount;
}

//----------------------------------------------------------------------------------------------------------------------
bool getEuphoriaDebugControlInfo(
  uint32_t index,
  const char** controlName,
  DebugControlID* controlID,
  DebugControlFlags* controlFlags,
  DebugControlDataType* controlDataType)
{
  NMP_ASSERT(controlName);
  NMP_ASSERT(controlID);
  NMP_ASSERT(controlFlags);
  NMP_ASSERT(controlDataType);

  if (index < g_debugControlsCount)
  {
    *controlName = g_debugControlsArray[index].m_controlName;
    *controlID = g_debugControlsArray[index].m_controlID;
    *controlFlags = g_debugControlsArray[index].m_controlFlags;
    *controlDataType = g_debugControlsArray[index].m_controlDataType;

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const char* getEuphoriaDebugControlName(uint32_t index)
{
  return index < g_debugControlsCount ? g_debugControlsArray[index].m_controlName : NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/// DebugControl kBoolControlDataType
//----------------------------------------------------------------------------------------------------------------------
bool setEuphoriaDebugControl(
  ER::Character* character,
  DebugControlID controlID,
  uint8_t limbIndex,
  bool value)
{
  NMP_ASSERT(character);

  switch (controlID)
  {
  case kGravityCompensationControlID:
    character->getBody().setFeatureFlag(ER::Body::kFeatureGravityCompensation, value);
    return true;
  case kDrawGravityCompensationTorquesControlID:
    ER::GravityCompensation::setDebugDrawTorquesFlag(value);
    return true;
  case kDrawGravityCompensationGravityControlID:
    ER::GravityCompensation::setDebugDrawGravityFlag(value);
    return true;
  case kEnableJointLimitsControlID:
    character->getBody().setFeatureFlag(ER::Body::kFeatureJointLimits, value);
    return true;
  case kEnableSoftLimitsControlID:
    character->getBody().setFeatureFlag(ER::Body::kFeatureSoftLimits, value);
    return true;
  case kEnableHamstringsControlID:
    character->getBody().setFeatureFlag(ER::Body::kFeatureHamstrings, value);
    return true;
  case kDrawHamstringsControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      character->getBody().getLimb(limbIndex).setDebugDrawFlag(ER::Limb::kDrawHamstrings, value);
      return true;
    }
    break;
  case kDrawIKTargetsControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      limbIK.setDebugDrawFlag(ER::LimbIK::kDrawTarget, value);
      return true;
    }
    break;
  case kDrawIKFinalResultsControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      limbIK.setDebugDrawFlag(ER::LimbIK::kDrawFinalResult, value);
      return true;
    }
    break;
  case kIKDrawingRelativeToRootControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      limbIK.setDebugDrawFlag(ER::LimbIK::kDrawRelativeToRoot, value);
      return true;
    }
    break;
  case kIKUsesJointLimits:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      limbIK.setFeatureFlag(ER::LimbIK::kFeatureUseJointLimits, value);
      return true;
    }
    break;
  case kDrawContactsControlID:
    ER::ContactFeedback::setDrawContactsFlag(value);
    return true;
  case kDrawDetailedContactsControlID:
    ER::ContactFeedback::setDrawDetailedContactsFlag(value);
    return true;
  case kDrawCharacterRootControlID:
    return true;

  case kDrawIKInputPoseControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      limbIK.setDebugDrawFlag(ER::LimbIK::kDrawInputPose, value);
      return true;
    }
    break;
  case kDrawIKGuidePoseControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      limbIK.setDebugDrawFlag(ER::LimbIK::kDrawGuidePose, value);
      return true;
    }
    break;
  case kDrawGuidePoseRangeControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      ER::Limb& limb = character->getBody().getLimb(limbIndex);
      limb.setDebugDrawFlag(ER::LimbInterface::kDrawGuidePoseRange, value);
      return true;
    }
    break;
  case kDrawGuidePoseExtremesControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      ER::Limb& limb = character->getBody().getLimb(limbIndex);
      limb.setDebugDrawFlag(ER::LimbInterface::kDrawGuidePoseExtremes, value);
      return true;
    }
    break;
  case kDrawEndContraintControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      ER::Limb& limb = character->getBody().getLimb(limbIndex);
      limb.setDebugDrawFlag(ER::LimbInterface::kDrawEndConstraint, value);
      return true;
    }
    break;

  default:
    break;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool getEuphoriaDebugControl(
  const ER::Character* character,
  DebugControlID controlID,
  uint8_t limbIndex,
  bool* value)
{
  NMP_ASSERT(character);
  NMP_ASSERT(value);

  *value = false;
  switch (controlID)
  {
  case kGravityCompensationControlID:
    *value = character->getBody().getFeatureFlag(ER::Body::kFeatureGravityCompensation);
    return true;
  case kDrawGravityCompensationTorquesControlID:
    *value = ER::GravityCompensation::getDebugDrawTorquesFlag();
    return true;
  case kDrawGravityCompensationGravityControlID:
    *value = ER::GravityCompensation::getDebugDrawGravityFlag();
    return true;
  case kEnableJointLimitsControlID:
    *value = character->getBody().getFeatureFlag(ER::Body::kFeatureJointLimits);
    return true;
  case kEnableSoftLimitsControlID:
    *value = character->getBody().getFeatureFlag(ER::Body::kFeatureSoftLimits);
    return true;
  case kEnableHamstringsControlID:
    *value = character->getBody().getFeatureFlag(ER::Body::kFeatureHamstrings);
    return true;
  case kDrawHamstringsControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      *value = character->getBody().getLimb(limbIndex).getDebugDrawFlag(ER::Limb::kDrawHamstrings);
      return true;
    }
    break;
  case kDrawIKTargetsControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      const ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      *value = limbIK.getDebugDrawFlag(ER::LimbIK::kDrawTarget);
      return true;
    }
    break;
  case kDrawIKFinalResultsControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      const ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      *value = limbIK.getDebugDrawFlag(ER::LimbIK::kDrawFinalResult);
      return true;
    }
    break;
  case kIKDrawingRelativeToRootControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      const ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      *value = limbIK.getDebugDrawFlag(ER::LimbIK::kDrawRelativeToRoot);
      return true;
    }
    break;
  case kIKUsesJointLimits:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      const ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      *value = limbIK.getFeatureFlag(ER::LimbIK::kFeatureUseJointLimits);
      return true;
    }
    break;
  case kDrawContactsControlID:
    *value = ER::ContactFeedback::getDrawContactsFlag();
    return true;
  case kDrawDetailedContactsControlID:
    *value = ER::ContactFeedback::getDrawDetailedContactsFlag();
    return true;
  case kDrawCharacterRootControlID:
    return true;

  case kDrawIKInputPoseControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      const ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      *value = limbIK.getDebugDrawFlag(ER::LimbIK::kDrawInputPose);
      return true;
    }
    break;
  case kDrawIKGuidePoseControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      const ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      *value = limbIK.getDebugDrawFlag(ER::LimbIK::kDrawGuidePose);
      return true;
    }
    break;
  case kDrawGuidePoseRangeControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      const ER::Limb& limb = character->getBody().getLimb(limbIndex);
      *value = limb.getDebugDrawFlag(ER::LimbInterface::kDrawGuidePoseRange);
      return true;
    }
    break;
  case kDrawGuidePoseExtremesControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      const ER::Limb& limb = character->getBody().getLimb(limbIndex);
      *value = limb.getDebugDrawFlag(ER::LimbInterface::kDrawGuidePoseExtremes);
      return true;
    }
    break;
  case kDrawEndContraintControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      const ER::Limb& limb = character->getBody().getLimb(limbIndex);
      *value = limb.getDebugDrawFlag(ER::LimbInterface::kDrawEndConstraint);
      return true;
    }
    break;

  default:
    break;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// DebugControl kInt32ControlDataType
//----------------------------------------------------------------------------------------------------------------------
bool setEuphoriaDebugControl(
  ER::Character* character,
  DebugControlID controlID,
  uint8_t limbIndex,
  int32_t value)
{
  NMP_ASSERT(character);

  switch (controlID)
  {
  case kIKSolverIterationsControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      ER::Limb& limb = character->getBody().getLimb(limbIndex);
      limb.setIncrementalIKIterations(value);
      return true;
    }
    break;
  default:
    break;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool getEuphoriaDebugControl(
  const ER::Character* character,
  DebugControlID controlID,
  uint8_t limbIndex,
  int32_t* value)
{
  NMP_ASSERT(character);
  NMP_ASSERT(value);

  switch (controlID)
  {
  case kIKSolverIterationsControlID:
    if (limbIndex < character->getBody().getNumLimbs())
    {
      const ER::LimbIK& limbIK = character->getBody().getLimb(limbIndex).getIK();
      *value = limbIK.m_OP.m_maxIterations;
      return true;
    }
    break;
  default:
    break;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// DebugControl kUInt32ControlDataType
//----------------------------------------------------------------------------------------------------------------------
bool setEuphoriaDebugControl(
  ER::Character* NMP_USED_FOR_ASSERTS(character),
  DebugControlID NMP_UNUSED(controlID),
  uint8_t NMP_UNUSED(limbIndex),
  uint32_t NMP_UNUSED(value))
{
  NMP_ASSERT(character);

  // no uint32_t controls yet.

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool getEuphoriaDebugControl(
  const ER::Character* NMP_USED_FOR_ASSERTS(character),
  DebugControlID NMP_UNUSED(controlID),
  uint8_t NMP_UNUSED(limbIndex),
  uint32_t* NMP_USED_FOR_ASSERTS(value))
{
  NMP_ASSERT(character);
  NMP_ASSERT(value);

  // no uint32_t controls yet.

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// DebugControl kFloatControlDataType
//----------------------------------------------------------------------------------------------------------------------
bool setEuphoriaDebugControl(
  ER::Character* NMP_USED_FOR_ASSERTS(character),
  DebugControlID NMP_UNUSED(controlID),
  uint8_t NMP_UNUSED(limbIndex),
  float NMP_UNUSED(value))
{
  NMP_ASSERT(character);

  // no float controls yet.

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool getEuphoriaDebugControl(
  const ER::Character* NMP_USED_FOR_ASSERTS(character),
  DebugControlID NMP_UNUSED(controlID),
  uint8_t NMP_UNUSED(limbIndex),
  float* NMP_USED_FOR_ASSERTS(value))
{
  NMP_ASSERT(character);
  NMP_ASSERT(value);

  // no float controls yet.

  return false;
}

} // namespace ER
