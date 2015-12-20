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

#include "Behaviours/ShieldBehaviour.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erAttribData.h"


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

const ER::BehaviourDef::ModuleToEnable* ShieldBehaviourDef::getModulesToEnable(uint32_t& numToEnable)
{
  static const ER::BehaviourDef::ModuleToEnable mteDef[] =
  {
    { NetworkManifest::shieldBehaviourInterface, false },
    { NetworkManifest::heads_0_eyes, false },
    { NetworkManifest::environmentAwareness, true },
    { NetworkManifest::sceneProbes, true },
    { NetworkManifest::hazardManagement, false },
    { NetworkManifest::hazardManagement_hazardResponse, false },
    { NetworkManifest::hazardManagement_chestImpactPredictor, false },
    { NetworkManifest::hazardManagement_shieldManagement, false },
    { NetworkManifest::balanceManagement_balancePoser, false },
    { NetworkManifest::spines_0_pose, false },
    { NetworkManifest::legs_0_pose, false },
    { NetworkManifest::legs_1_pose, false },
    { NetworkManifest::arms_0_pose, false },
    { NetworkManifest::arms_1_pose, false },
    { NetworkManifest::heads_0_pose, false },
    { NetworkManifest::bodyFrame, false },
  };
  numToEnable = sizeof(mteDef) / sizeof(mteDef[0]);
  return mteDef;
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* data)
{
  m_params.updateFromControlParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviour::interpretMessageData(const ER::AttribDataBehaviourParameters* data)
{
  m_params.updateFromMessageParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
bool ShieldBehaviour::isAnimationRequired(int32_t animationID) const
{
  switch (animationID)
  {
    case ShieldPose_ID:  /* 0 */
      return m_poseRequirements.enablePoseInput_ShieldPose;

  default:
    NMP_DEBUG_MSG("Unexpected animation ID passed to Shield");
    NMP_ASSERT_FAIL();
    break;
  }
  return false;
}

void ShieldBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body)
{
  switch (animationData->m_animationID)
  {
    case ShieldPose_ID:  /* 0 */
    {
      int iNetworkLimb = 0; // Based on network indexes.
      int iRigLimb = 0; // Based on rig indexes.
      for (int iLimb = 0 ; iLimb < body->getNumArms() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ShieldPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms;
      for (int iLimb = 0 ; iLimb < body->getNumHeads() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ShieldPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads;
      for (int iLimb = 0; iLimb < body->getNumLegs() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ShieldPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads + NetworkConstants::networkMaxNumLegs;
      for (int iLimb = 0; iLimb < body->getNumSpines() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ShieldPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      m_params.pd_ShieldPose.m_poseWeight = animationData->m_isSet ? 1.0f : 0.0f;
    }
    break;

  default:
    NMP_DEBUG_MSG("Unexpected animation ID passed to Shield");
    NMP_ASSERT_FAIL();
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool ShieldBehaviour::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(m_params);
  savedState.addValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ShieldBehaviour::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  m_params = savedState.getValue(m_params);
  m_poseRequirements = savedState.getValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviour::handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  m_params.updateOutputControlParams(outputControlParams, numOutputControlParams);
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviour::clearOutputControlParamsData()
{
  m_params.clearOutputControlParamsData();
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviour::handleEmittedMessages(uint32_t& messages)
{
  m_params.updateAndClearEmittedMessages(messages);
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  for (size_t iOCP = 0 ; iOCP < numOutputControlParams ; ++iOCP)
  {
    ER::AttribDataBehaviourParameters::OutputControlParam& outputControlParam = outputControlParams[iOCP];
    switch (iOCP)
    {
      case ShieldBehaviour::isShielding_OCP_ID:  /* 0 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.isShielding;
      break;

      case ShieldBehaviour::hazardAngle_OCP_ID:  /* 1 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.hazardAngle;
      break;

    default:
      NMP_DEBUG_MSG("Unexpected output control param ID passed to Shield");
      NMP_ASSERT_FAIL();
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ShieldBehaviourData::updateAndClearEmittedMessages(uint32_t&)
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

