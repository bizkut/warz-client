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

#include "Behaviours/ReachForBodyBehaviour.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erAttribData.h"


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

const ER::BehaviourDef::ModuleToEnable* ReachForBodyBehaviourDef::getModulesToEnable(uint32_t& numToEnable)
{
  static const ER::BehaviourDef::ModuleToEnable mteDef[] =
  {
    { NetworkManifest::reachForBodyBehaviourInterface, false },
    { NetworkManifest::bodyFrame, false },
    { NetworkManifest::bodyFrame_reachForBody, true },
    { NetworkManifest::upperBody, true },
    { NetworkManifest::lowerBody, true },
    { NetworkManifest::spines_0_support, true },
    { NetworkManifest::spines_0_control, true },
    { NetworkManifest::spines_0_reachReaction, true },
    { NetworkManifest::legs_0, false },
    { NetworkManifest::legs_1, false },
    { NetworkManifest::legs_0_reachReaction, true },
    { NetworkManifest::legs_1_reachReaction, true },
    { NetworkManifest::arms_0_reachForBodyPart, true },
    { NetworkManifest::arms_1_reachForBodyPart, true },
    { NetworkManifest::arms_0_reachReaction, true },
    { NetworkManifest::arms_1_reachReaction, true },
  };
  numToEnable = sizeof(mteDef) / sizeof(mteDef[0]);
  return mteDef;
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBodyBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* data)
{
  m_params.updateFromControlParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBodyBehaviour::interpretMessageData(const ER::AttribDataBehaviourParameters* data)
{
  m_params.updateFromMessageParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
bool ReachForBodyBehaviour::isAnimationRequired(int32_t animationID) const
{
  switch (animationID)
  {
    case ArmReachForBodyPose_ID:  /* 0 */
      return m_poseRequirements.enablePoseInput_ArmReachForBodyPose;

  default:
    NMP_DEBUG_MSG("Unexpected animation ID passed to ReachForBody");
    NMP_ASSERT_FAIL();
    break;
  }
  return false;
}

void ReachForBodyBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body)
{
  switch (animationData->m_animationID)
  {
    case ArmReachForBodyPose_ID:  /* 0 */
    {
      int iNetworkLimb = 0; // Based on network indexes.
      int iRigLimb = 0; // Based on rig indexes.
      for (int iLimb = 0 ; iLimb < body->getNumArms() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ArmReachForBodyPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms;
      for (int iLimb = 0 ; iLimb < body->getNumHeads() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ArmReachForBodyPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads;
      for (int iLimb = 0; iLimb < body->getNumLegs() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ArmReachForBodyPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads + NetworkConstants::networkMaxNumLegs;
      for (int iLimb = 0; iLimb < body->getNumSpines() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_ArmReachForBodyPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      m_params.pd_ArmReachForBodyPose.m_poseWeight = animationData->m_isSet ? 1.0f : 0.0f;
    }
    break;

  default:
    NMP_DEBUG_MSG("Unexpected animation ID passed to ReachForBody");
    NMP_ASSERT_FAIL();
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool ReachForBodyBehaviour::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(m_params);
  savedState.addValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ReachForBodyBehaviour::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  m_params = savedState.getValue(m_params);
  m_poseRequirements = savedState.getValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBodyBehaviour::handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  m_params.updateOutputControlParams(outputControlParams, numOutputControlParams);
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBodyBehaviour::handleEmittedMessages(uint32_t& messages)
{
  m_params.updateAndClearEmittedMessages(messages);
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBodyBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam*, size_t )
{
}

//----------------------------------------------------------------------------------------------------------------------
void ReachForBodyBehaviourData::updateAndClearEmittedMessages(uint32_t&)
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

