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

#include "Behaviours/BalanceBehaviour.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erAttribData.h"


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

const ER::BehaviourDef::ModuleToEnable* BalanceBehaviourDef::getModulesToEnable(uint32_t& numToEnable)
{
  static const ER::BehaviourDef::ModuleToEnable mteDef[] =
  {
    { NetworkManifest::balanceBehaviourInterface, false },
    { NetworkManifest::balanceBehaviourFeedback, false },
    { NetworkManifest::arms_0_standingSupport, true },
    { NetworkManifest::arms_1_standingSupport, true },
    { NetworkManifest::arms_0_step, true },
    { NetworkManifest::arms_1_step, true },
    { NetworkManifest::arms_0_swing, true },
    { NetworkManifest::arms_1_swing, true },
    { NetworkManifest::arms_0_spin, true },
    { NetworkManifest::arms_1_spin, true },
    { NetworkManifest::arms_0_pose, true },
    { NetworkManifest::arms_1_pose, true },
    { NetworkManifest::heads_0_support, true },
    { NetworkManifest::heads_0_point, true },
    { NetworkManifest::heads_0_eyes, false },
    { NetworkManifest::legs_0_standingSupport, true },
    { NetworkManifest::legs_1_standingSupport, true },
    { NetworkManifest::legs_0_step, true },
    { NetworkManifest::legs_1_step, true },
    { NetworkManifest::spines_0_support, true },
    { NetworkManifest::spines_0_control, true },
    { NetworkManifest::balanceManagement, true },
    { NetworkManifest::bodyFrame_bodyBalance, true },
    { NetworkManifest::bodyFrame_supportPolygon, true },
    { NetworkManifest::bodyFrame_balanceAssistant, true },
    { NetworkManifest::upperBody, true },
    { NetworkManifest::lowerBody, true },
  };
  numToEnable = sizeof(mteDef) / sizeof(mteDef[0]);
  return mteDef;
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* data)
{
  m_params.updateFromControlParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviour::interpretMessageData(const ER::AttribDataBehaviourParameters* data)
{
  m_params.updateFromMessageParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
bool BalanceBehaviour::isAnimationRequired(int32_t animationID) const
{
  switch (animationID)
  {
    case BalancePose_ID:  /* 0 */
      return m_poseRequirements.enablePoseInput_BalancePose;

    case ReadyPose_ID:  /* 1 */
      return m_poseRequirements.enablePoseInput_ReadyPose;

  default:
    NMP_DEBUG_MSG("Unexpected animation ID passed to Balance");
    NMP_ASSERT_FAIL();
    break;
  }
  return false;
}

void BalanceBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body)
{
  switch (animationData->m_animationID)
  {
    case BalancePose_ID:  /* 0 */
    {
      int iNetworkLimb = 0; // Based on network indexes.
      int iRigLimb = 0; // Based on rig indexes.
      for (int iLimb = 0 ; iLimb < body->getNumArms() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_BalancePose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms;
      for (int iLimb = 0 ; iLimb < body->getNumHeads() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_BalancePose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads;
      for (int iLimb = 0; iLimb < body->getNumLegs() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_BalancePose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads + NetworkConstants::networkMaxNumLegs;
      for (int iLimb = 0; iLimb < body->getNumSpines() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_BalancePose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      m_params.pd_BalancePose.m_poseWeight = animationData->m_isSet ? 1.0f : 0.0f;
    }
    break;

    case ReadyPose_ID:  /* 1 */
    {
      int iNetworkLimb = 0; // Based on network indexes.
      int iRigLimb = 0; // Based on rig indexes.
      for (int iLimb = 0 ; iLimb < body->getNumArms() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ReadyPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms;
      for (int iLimb = 0 ; iLimb < body->getNumHeads() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ReadyPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads;
      for (int iLimb = 0; iLimb < body->getNumLegs() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ReadyPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads + NetworkConstants::networkMaxNumLegs;
      for (int iLimb = 0; iLimb < body->getNumSpines() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ReadyPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      m_params.pd_ReadyPose.m_poseWeight = animationData->m_isSet ? 1.0f : 0.0f;
    }
    break;

  default:
    NMP_DEBUG_MSG("Unexpected animation ID passed to Balance");
    NMP_ASSERT_FAIL();
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool BalanceBehaviour::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(m_params);
  savedState.addValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool BalanceBehaviour::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  m_params = savedState.getValue(m_params);
  m_poseRequirements = savedState.getValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviour::handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  m_params.updateOutputControlParams(outputControlParams, numOutputControlParams);
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviour::clearOutputControlParamsData()
{
  m_params.clearOutputControlParamsData();
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviour::handleEmittedMessages(uint32_t& messages)
{
  m_params.updateAndClearEmittedMessages(messages);
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  for (size_t iOCP = 0 ; iOCP < numOutputControlParams ; ++iOCP)
  {
    ER::AttribDataBehaviourParameters::OutputControlParam& outputControlParam = outputControlParams[iOCP];
    switch (iOCP)
    {
      case BalanceBehaviour::StandingStillTime_OCP_ID:  /* 0 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.StandingStillTime;
      break;

      case BalanceBehaviour::FallenTime_OCP_ID:  /* 1 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.FallenTime;
      break;

      case BalanceBehaviour::OnGroundTime_OCP_ID:  /* 2 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.OnGroundTime;
      break;

      case BalanceBehaviour::BalanceAmount_OCP_ID:  /* 3 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.BalanceAmount;
      break;

      case BalanceBehaviour::ForwardsDirection_OCP_ID:  /* 4 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_VECTOR3);
      *((NMP::Vector3*) outputControlParam.data) = m_ocp.ForwardsDirection;
      break;

      case BalanceBehaviour::RightDirection_OCP_ID:  /* 5 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_VECTOR3);
      *((NMP::Vector3*) outputControlParam.data) = m_ocp.RightDirection;
      break;

      case BalanceBehaviour::SupportVelocity_OCP_ID:  /* 6 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_VECTOR3);
      *((NMP::Vector3*) outputControlParam.data) = m_ocp.SupportVelocity;
      break;

      case BalanceBehaviour::SteppingTime_OCP_ID:  /* 7 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.SteppingTime;
      break;

      case BalanceBehaviour::StepCount_OCP_ID:  /* 8 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_INT);
      *((int32_t*) outputControlParam.data) = m_ocp.StepCount;
      break;

      case BalanceBehaviour::LegStepFractions_0_OCP_ID:  /* 9 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.LegStepFractions[0];
      break;

      case BalanceBehaviour::LegStepFractions_1_OCP_ID:  /* 10 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.LegStepFractions[1];
      break;

    default:
      NMP_DEBUG_MSG("Unexpected output control param ID passed to Balance");
      NMP_ASSERT_FAIL();
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void BalanceBehaviourData::updateAndClearEmittedMessages(uint32_t& messages)
{
  messages = m_messages;
  m_messages = 0;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

