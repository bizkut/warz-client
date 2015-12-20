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

#include "Behaviours/ProtectBehaviour.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erAttribData.h"


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

const ER::BehaviourDef::ModuleToEnable* ProtectBehaviourDef::getModulesToEnable(uint32_t& numToEnable)
{
  static const ER::BehaviourDef::ModuleToEnable mteDef[] =
  {
    { NetworkManifest::protectBehaviourInterface, false },
    { NetworkManifest::heads_0_eyes, false },
    { NetworkManifest::environmentAwareness, true },
    { NetworkManifest::bodyFrame, false },
    { NetworkManifest::bodyFrame_supportPolygon, true },
    { NetworkManifest::upperBody, true },
    { NetworkManifest::lowerBody, true },
    { NetworkManifest::hazardManagement_hazardResponse, false },
    { NetworkManifest::hazardManagement_chestImpactPredictor, false },
    { NetworkManifest::sceneProbes, true },
    { NetworkManifest::heads_0_point, false },
    { NetworkManifest::heads_0_avoid, false },
    { NetworkManifest::heads_0_dodge, false },
    { NetworkManifest::heads_0_support, false },
    { NetworkManifest::spines_0_support, true },
    { NetworkManifest::spines_0_control, true },
    { NetworkManifest::arms_0_swing, false },
    { NetworkManifest::arms_1_swing, false },
    { NetworkManifest::arms_0_spin, false },
    { NetworkManifest::arms_1_spin, false },
    { NetworkManifest::legs_0_swing, false },
    { NetworkManifest::legs_1_swing, false },
    { NetworkManifest::legs_0_spin, false },
    { NetworkManifest::legs_1_spin, false },
    { NetworkManifest::arms_0_brace, false },
    { NetworkManifest::arms_1_brace, false },
    { NetworkManifest::legs_0_brace, false },
    { NetworkManifest::legs_1_brace, false },
    { NetworkManifest::arms_0_placement, false },
    { NetworkManifest::arms_1_placement, false },
  };
  numToEnable = sizeof(mteDef) / sizeof(mteDef[0]);
  return mteDef;
}

//----------------------------------------------------------------------------------------------------------------------
void ProtectBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* data)
{
  m_params.updateFromControlParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
void ProtectBehaviour::interpretMessageData(const ER::AttribDataBehaviourParameters* data)
{
  m_params.updateFromMessageParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
bool ProtectBehaviour::isAnimationRequired(int32_t NMP_UNUSED(animationID)) const
{
  return false;
}

void ProtectBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData*, ER::Body*)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool ProtectBehaviour::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(m_params);
  savedState.addValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ProtectBehaviour::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  m_params = savedState.getValue(m_params);
  m_poseRequirements = savedState.getValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ProtectBehaviour::handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  m_params.updateOutputControlParams(outputControlParams, numOutputControlParams);
}

//----------------------------------------------------------------------------------------------------------------------
void ProtectBehaviour::handleEmittedMessages(uint32_t& messages)
{
  m_params.updateAndClearEmittedMessages(messages);
}

//----------------------------------------------------------------------------------------------------------------------
void ProtectBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam*, size_t )
{
}

//----------------------------------------------------------------------------------------------------------------------
void ProtectBehaviourData::updateAndClearEmittedMessages(uint32_t&)
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

