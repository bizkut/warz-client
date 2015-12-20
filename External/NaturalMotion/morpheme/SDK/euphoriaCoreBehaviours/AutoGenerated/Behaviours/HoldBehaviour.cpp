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

#include "Behaviours/HoldBehaviour.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erAttribData.h"


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

const ER::BehaviourDef::ModuleToEnable* HoldBehaviourDef::getModulesToEnable(uint32_t& numToEnable)
{
  static const ER::BehaviourDef::ModuleToEnable mteDef[] =
  {
    { NetworkManifest::holdBehaviourInterface, false },
    { NetworkManifest::environmentAwareness, true },
    { NetworkManifest::hazardManagement_chestImpactPredictor, false },
    { NetworkManifest::hazardManagement_grab, false },
    { NetworkManifest::hazardManagement_grabDetection, false },
    { NetworkManifest::bodyFrame, false },
    { NetworkManifest::upperBody, false },
    { NetworkManifest::upperBody_rotate, false },
    { NetworkManifest::upperBody_position, false },
    { NetworkManifest::lowerBody, false },
    { NetworkManifest::lowerBody_rotate, false },
    { NetworkManifest::lowerBody_position, false },
    { NetworkManifest::sceneProbes, true },
    { NetworkManifest::spines_0_support, false },
    { NetworkManifest::spines_0_control, false },
    { NetworkManifest::arms_0_grab, false },
    { NetworkManifest::arms_1_grab, false },
    { NetworkManifest::arms_0_hold, false },
    { NetworkManifest::arms_1_hold, false },
    { NetworkManifest::arms_0_holdingSupport, false },
    { NetworkManifest::arms_1_holdingSupport, false },
    { NetworkManifest::arms_0_spin, false },
    { NetworkManifest::arms_1_spin, false },
    { NetworkManifest::arms_0_swing, false },
    { NetworkManifest::arms_1_swing, false },
    { NetworkManifest::arms_0_reachForWorld, false },
    { NetworkManifest::arms_1_reachForWorld, false },
    { NetworkManifest::legs_0_spin, false },
    { NetworkManifest::legs_1_spin, false },
    { NetworkManifest::legs_0_swing, false },
    { NetworkManifest::legs_1_swing, false },
    { NetworkManifest::heads_0_point, false },
    { NetworkManifest::heads_0_eyes, false },
  };
  numToEnable = sizeof(mteDef) / sizeof(mteDef[0]);
  return mteDef;
}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* data)
{
  m_params.updateFromControlParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviour::interpretMessageData(const ER::AttribDataBehaviourParameters* data)
{
  m_params.updateFromMessageParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
bool HoldBehaviour::isAnimationRequired(int32_t NMP_UNUSED(animationID)) const
{
  return false;
}

void HoldBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData*, ER::Body*)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool HoldBehaviour::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(m_params);
  savedState.addValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool HoldBehaviour::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  m_params = savedState.getValue(m_params);
  m_poseRequirements = savedState.getValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviour::handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  m_params.updateOutputControlParams(outputControlParams, numOutputControlParams);
}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviour::clearOutputControlParamsData()
{
  m_params.clearOutputControlParamsData();
}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviour::handleEmittedMessages(uint32_t& messages)
{
  m_params.updateAndClearEmittedMessages(messages);
}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  for (size_t iOCP = 0 ; iOCP < numOutputControlParams ; ++iOCP)
  {
    ER::AttribDataBehaviourParameters::OutputControlParam& outputControlParam = outputControlParams[iOCP];
    switch (iOCP)
    {
      case HoldBehaviour::NumConstrainedHands_OCP_ID:  /* 0 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.NumConstrainedHands;
      break;

      case HoldBehaviour::HandHolding_0_OCP_ID:  /* 1 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_BOOL);
      *((bool*) outputControlParam.data) = m_ocp.HandHolding[0];
      break;

      case HoldBehaviour::HandHolding_1_OCP_ID:  /* 2 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_BOOL);
      *((bool*) outputControlParam.data) = m_ocp.HandHolding[1];
      break;

      case HoldBehaviour::HoldAttemptImportance_OCP_ID:  /* 3 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.HoldAttemptImportance;
      break;

      case HoldBehaviour::HoldDuration_OCP_ID:  /* 4 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.HoldDuration;
      break;

      case HoldBehaviour::EdgeForwardNormal_OCP_ID:  /* 5 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_VECTOR3);
      *((NMP::Vector3*) outputControlParam.data) = m_ocp.EdgeForwardNormal;
      break;

    default:
      NMP_DEBUG_MSG("Unexpected output control param ID passed to Hold");
      NMP_ASSERT_FAIL();
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HoldBehaviourData::updateAndClearEmittedMessages(uint32_t&)
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

