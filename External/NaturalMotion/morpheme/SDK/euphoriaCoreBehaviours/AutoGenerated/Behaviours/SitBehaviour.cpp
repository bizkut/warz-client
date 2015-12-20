/*
 * Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

//----------------------------------------------------------------------------------------------------------------------
//                                  This file is auto-generated
//----------------------------------------------------------------------------------------------------------------------

#include "Behaviours/SitBehaviour.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erAttribData.h"


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

const ER::BehaviourDef::ModuleToEnable* SitBehaviourDef::getModulesToEnable(uint32_t& numToEnable)
{
  static const ER::BehaviourDef::ModuleToEnable mteDef[] =
  {
    { NetworkManifest::sitBehaviourInterface, false },
    { NetworkManifest::arms_0_standingSupport, true },
    { NetworkManifest::arms_1_standingSupport, true },
    { NetworkManifest::arms_0_sittingStep, true },
    { NetworkManifest::arms_1_sittingStep, true },
    { NetworkManifest::arms_0_placement, true },
    { NetworkManifest::arms_1_placement, true },
    { NetworkManifest::arms_0_pose, true },
    { NetworkManifest::arms_1_pose, true },
    { NetworkManifest::legs_0_sittingSupport, true },
    { NetworkManifest::legs_1_sittingSupport, true },
    { NetworkManifest::legs_0_pose, true },
    { NetworkManifest::legs_1_pose, true },
    { NetworkManifest::spines_0_support, true },
    { NetworkManifest::spines_0_control, true },
    { NetworkManifest::heads_0_support, true },
    { NetworkManifest::bodyFrame_sittingBodyBalance, false },
    { NetworkManifest::bodyFrame_sittingSupportPolygon, true },
  };
  numToEnable = sizeof(mteDef) / sizeof(mteDef[0]);
  return mteDef;
}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* NMP_UNUSED(data))
{
}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviour::interpretMessageData(const ER::AttribDataBehaviourParameters* data)
{
  m_params.updateFromMessageParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
bool SitBehaviour::isAnimationRequired(int32_t animationID) const
{
  switch (animationID)
  {
    case Pose_ID:  /* 0 */
      return m_poseRequirements.enablePoseInput_Pose;

  default:
    NMP_DEBUG_MSG("Unexpected animation ID passed to Sit");
    NMP_ASSERT_FAIL();
    break;
  }
  return false;
}

void SitBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body)
{
  switch (animationData->m_animationID)
  {
    case Pose_ID:  /* 0 */
    {
      int iNetworkLimb = 0; // Based on network indexes.
      int iRigLimb = 0; // Based on rig indexes.
      for (int iLimb = 0 ; iLimb < body->getNumArms() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_Pose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms;
      for (int iLimb = 0 ; iLimb < body->getNumHeads() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_Pose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads;
      for (int iLimb = 0; iLimb < body->getNumLegs() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_Pose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads + NetworkConstants::networkMaxNumLegs;
      for (int iLimb = 0; iLimb < body->getNumSpines() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_Pose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      m_params.pd_Pose.m_poseWeight = animationData->m_isSet ? 1.0f : 0.0f;
    }
    break;

  default:
    NMP_DEBUG_MSG("Unexpected animation ID passed to Sit");
    NMP_ASSERT_FAIL();
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SitBehaviour::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(m_params);
  savedState.addValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SitBehaviour::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  m_params = savedState.getValue(m_params);
  m_poseRequirements = savedState.getValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviour::handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  m_params.updateOutputControlParams(outputControlParams, numOutputControlParams);
}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviour::clearOutputControlParamsData()
{
  m_params.clearOutputControlParamsData();
}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviour::handleEmittedMessages(uint32_t& messages)
{
  m_params.updateAndClearEmittedMessages(messages);
}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  for (size_t iOCP = 0 ; iOCP < numOutputControlParams ; ++iOCP)
  {
    ER::AttribDataBehaviourParameters::OutputControlParam& outputControlParam = outputControlParams[iOCP];
    switch (iOCP)
    {
      case SitBehaviour::SitAmount_OCP_ID:  /* 0 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.SitAmount;
      break;

    default:
      NMP_DEBUG_MSG("Unexpected output control param ID passed to Sit");
      NMP_ASSERT_FAIL();
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SitBehaviourData::updateAndClearEmittedMessages(uint32_t&)
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

