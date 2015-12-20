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

#include "Behaviours/LegsPedalBehaviour.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erAttribData.h"


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

const ER::BehaviourDef::ModuleToEnable* LegsPedalBehaviourDef::getModulesToEnable(uint32_t& numToEnable)
{
  static const ER::BehaviourDef::ModuleToEnable mteDef[] =
  {
    { NetworkManifest::legsPedalBehaviourInterface, false },
    { NetworkManifest::lowerBody, false },
    { NetworkManifest::legs_0_spin, false },
    { NetworkManifest::legs_1_spin, false },
  };
  numToEnable = sizeof(mteDef) / sizeof(mteDef[0]);
  return mteDef;
}

//----------------------------------------------------------------------------------------------------------------------
void LegsPedalBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* data)
{
  m_params.updateFromControlParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
void LegsPedalBehaviour::interpretMessageData(const ER::AttribDataBehaviourParameters* data)
{
  m_params.updateFromMessageParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
bool LegsPedalBehaviour::isAnimationRequired(int32_t NMP_UNUSED(animationID)) const
{
  return false;
}

void LegsPedalBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData*, ER::Body*)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool LegsPedalBehaviour::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(m_params);
  savedState.addValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LegsPedalBehaviour::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  m_params = savedState.getValue(m_params);
  m_poseRequirements = savedState.getValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void LegsPedalBehaviour::handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  m_params.updateOutputControlParams(outputControlParams, numOutputControlParams);
}

//----------------------------------------------------------------------------------------------------------------------
void LegsPedalBehaviour::handleEmittedMessages(uint32_t& messages)
{
  m_params.updateAndClearEmittedMessages(messages);
}

//----------------------------------------------------------------------------------------------------------------------
void LegsPedalBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam*, size_t )
{
}

//----------------------------------------------------------------------------------------------------------------------
void LegsPedalBehaviourData::updateAndClearEmittedMessages(uint32_t&)
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

