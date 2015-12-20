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

#include "Behaviours/ObserveBehaviour.h"
#include "euphoria/erBody.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erAttribData.h"


//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

const ER::BehaviourDef::ModuleToEnable* ObserveBehaviourDef::getModulesToEnable(uint32_t& numToEnable)
{
  static const ER::BehaviourDef::ModuleToEnable mteDef[] =
  {
    { NetworkManifest::observeBehaviourInterface, false },
    { NetworkManifest::heads_0_eyes, false },
    { NetworkManifest::heads_0_point, false },
    { NetworkManifest::spines_0_support, true },
    { NetworkManifest::spines_0_control, true },
    { NetworkManifest::environmentAwareness, true },
    { NetworkManifest::sceneProbes, true },
  };
  numToEnable = sizeof(mteDef) / sizeof(mteDef[0]);
  return mteDef;
}

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviour::interpretControlParams(const ER::AttribDataBehaviourState* data)
{
  m_params.updateFromControlParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviour::interpretMessageData(const ER::AttribDataBehaviourParameters* data)
{
  m_params.updateFromMessageParams(data);
}

//----------------------------------------------------------------------------------------------------------------------
bool ObserveBehaviour::isAnimationRequired(int32_t NMP_UNUSED(animationID)) const
{
  return false;
}

void ObserveBehaviour::interpretAnimationMessage(const ER::BehaviourAnimationData*, ER::Body*)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool ObserveBehaviour::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(m_params);
  savedState.addValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ObserveBehaviour::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  m_params = savedState.getValue(m_params);
  m_poseRequirements = savedState.getValue(m_poseRequirements);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviour::handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  m_params.updateOutputControlParams(outputControlParams, numOutputControlParams);
}

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviour::clearOutputControlParamsData()
{
  m_params.clearOutputControlParamsData();
}

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviour::handleEmittedMessages(uint32_t& messages)
{
  m_params.updateAndClearEmittedMessages(messages);
}

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviourData::updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams)
{
  for (size_t iOCP = 0 ; iOCP < numOutputControlParams ; ++iOCP)
  {
    ER::AttribDataBehaviourParameters::OutputControlParam& outputControlParam = outputControlParams[iOCP];
    switch (iOCP)
    {
      case ObserveBehaviour::observedWeight_OCP_ID:  /* 0 */
      NMP_ASSERT(outputControlParam.type == MR::ATTRIB_SEMANTIC_CP_FLOAT);
      *((float*) outputControlParam.data) = m_ocp.observedWeight;
      break;

    default:
      NMP_DEBUG_MSG("Unexpected output control param ID passed to Observe");
      NMP_ASSERT_FAIL();
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ObserveBehaviourData::updateAndClearEmittedMessages(uint32_t&)
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

