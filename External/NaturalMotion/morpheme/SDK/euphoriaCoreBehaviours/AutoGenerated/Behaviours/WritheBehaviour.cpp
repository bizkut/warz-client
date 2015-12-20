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

#include "Behaviours/WritheBehaviour.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erAttribData.h"


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

const ER::BehaviourDef::ModuleToEnable* WritheBehaviourDef::getModulesToEnable(uint32_t& numToEnable)
{
  static const ER::BehaviourDef::ModuleToEnable mteDef[] =
  {
    { NetworkManifest::writheBehaviourInterface, false },
    { NetworkManifest::arms_0_writhe, false },
    { NetworkManifest::arms_1_writhe, false },
    { NetworkManifest::legs_0_writhe, false },
    { NetworkManifest::legs_1_writhe, false },
    { NetworkManifest::spines_0_writhe, false },
    { NetworkManifest::randomLook, false },
    { NetworkManifest::heads_0_point, false },
    { NetworkManifest::heads_0_eyes, false },
  };
  numToEnable = sizeof(mteDef) / sizeof(mteDef[0]);
  return mteDef;
}

//----------------------------------------------------------------------------------------------------------------------
void WritheBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* data)
{
  m_params.updateFromControlParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
void WritheBehaviour::interpretMessageData(const ER::AttribDataBehaviourParameters* data)
{
  m_params.updateFromMessageParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
bool WritheBehaviour::isAnimationRequired(int32_t animationID) const
{
  switch (animationID)
  {
    case DefaultPose_ID:  /* 0 */
      return m_poseRequirements.enablePoseInput_DefaultPose;

  default:
    NMP_DEBUG_MSG("Unexpected animation ID passed to Writhe");
    NMP_ASSERT_FAIL();
    break;
  }
  return false;
}

void WritheBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body)
{
  switch (animationData->m_animationID)
  {
    case DefaultPose_ID:  /* 0 */
    {
      int iNetworkLimb = 0; // Based on network indexes.
      int iRigLimb = 0; // Based on rig indexes.
      for (int iLimb = 0 ; iLimb < body->getNumArms() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_DefaultPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms;
      for (int iLimb = 0 ; iLimb < body->getNumHeads() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_DefaultPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads;
      for (int iLimb = 0; iLimb < body->getNumLegs() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_DefaultPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      iNetworkLimb = NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumHeads + NetworkConstants::networkMaxNumLegs;
      for (int iLimb = 0; iLimb < body->getNumSpines() ; ++iLimb, ++iRigLimb, ++iNetworkLimb)
      {
        m_params.pd_DefaultPose.m_poseEndRelativeToRoots[iNetworkLimb] = body->m_definition->m_limbDefs[iRigLimb]->getRootToEndTransform(
            animationData->m_TMs);
      }
      m_params.pd_DefaultPose.m_poseWeight = animationData->m_isSet ? 1.0f : 0.0f;
    }
    break;

  default:
    NMP_DEBUG_MSG("Unexpected animation ID passed to Writhe");
    NMP_ASSERT_FAIL();
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool WritheBehaviour::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(m_params);
  savedState.addValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool WritheBehaviour::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  m_params = savedState.getValue(m_params);
  m_poseRequirements = savedState.getValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void WritheBehaviour::handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  m_params.updateOutputControlParams(outputControlParams, numOutputControlParams);
}

//----------------------------------------------------------------------------------------------------------------------
void WritheBehaviour::handleEmittedMessages(uint32_t& messages)
{
  m_params.updateAndClearEmittedMessages(messages);
}

//----------------------------------------------------------------------------------------------------------------------
void WritheBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam*, size_t )
{
}

//----------------------------------------------------------------------------------------------------------------------
void WritheBehaviourData::updateAndClearEmittedMessages(uint32_t&)
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

