/*
* Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
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
#include "CharacteristicsBehaviourInterface.h"
#include "MyNetwork.h"
#include "Behaviours/CharacteristicsBehaviour.h"
#include "NetworkDescriptor.h"
#include "euphoria/erDebugDraw.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void CharacteristicsBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  const CharacteristicsBehaviourData& params = ((CharacteristicsBehaviour*)
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Characteristics)))->getParams();

  out->setWholeBodyStrengthReduction(1.0f - NMP::maximum(0.0f, params.getWholeBodyStrengthScaleCP()));
  for (uint32_t i = 0 ; i < params.maxArmStrengthScaleCP ; ++i)
    out->setArmStrengthReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getArmStrengthScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxHeadStrengthScaleCP ; ++i)
    out->setHeadStrengthReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getHeadStrengthScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxLegStrengthScaleCP ; ++i)
    out->setLegStrengthReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getLegStrengthScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxSpineStrengthScaleCP ; ++i)
    out->setSpineStrengthReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getSpineStrengthScaleCP(i)));

  out->setWholeBodyControlCompensationReduction(1.0f - NMP::maximum(0.0f, params.getWholeBodyControlCompensationScaleCP()));
  for (uint32_t i = 0 ; i < params.maxArmControlCompensationScaleCP ; ++i)
    out->setArmControlCompensationReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getArmControlCompensationScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxHeadControlCompensationScaleCP ; ++i)
    out->setHeadControlCompensationReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getHeadControlCompensationScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxLegControlCompensationScaleCP ; ++i)
    out->setLegControlCompensationReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getLegControlCompensationScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxSpineControlCompensationScaleCP ; ++i)
    out->setSpineControlCompensationReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getSpineControlCompensationScaleCP(i)));

  out->setWholeBodyExternalComplianceReduction(1.0f - NMP::maximum(0.0f, params.getWholeBodyExternalComplianceScaleCP()));
  for (uint32_t i = 0 ; i < params.maxArmExternalComplianceScaleCP ; ++i)
    out->setArmExternalComplianceReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getArmExternalComplianceScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxHeadExternalComplianceScaleCP ; ++i)
    out->setHeadExternalComplianceReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getHeadExternalComplianceScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxLegExternalComplianceScaleCP ; ++i)
    out->setLegExternalComplianceReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getLegExternalComplianceScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxSpineExternalComplianceScaleCP ; ++i)
    out->setSpineExternalComplianceReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getSpineExternalComplianceScaleCP(i)));

  out->setWholeBodyDampingRatioReduction(1.0f - NMP::maximum(0.0f, params.getWholeBodyDampingRatioScaleCP()));
  for (uint32_t i = 0 ; i < params.maxArmDampingRatioScaleCP ; ++i)
    out->setArmDampingRatioReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getArmDampingRatioScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxHeadDampingRatioScaleCP ; ++i)
    out->setHeadDampingRatioReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getHeadDampingRatioScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxLegDampingRatioScaleCP ; ++i)
    out->setLegDampingRatioReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getLegDampingRatioScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxSpineDampingRatioScaleCP ; ++i)
    out->setSpineDampingRatioReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getSpineDampingRatioScaleCP(i)));

  out->setWholeBodySoftLimitStiffnessReduction(1.0f - NMP::maximum(0.0f, params.getWholeBodySoftLimitStiffnessScaleCP()));
  for (uint32_t i = 0 ; i < params.maxArmSoftLimitStiffnessScaleCP ; ++i)
    out->setArmSoftLimitStiffnessReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getArmSoftLimitStiffnessScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxHeadSoftLimitStiffnessScaleCP ; ++i)
    out->setHeadSoftLimitStiffnessReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getHeadSoftLimitStiffnessScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxLegSoftLimitStiffnessScaleCP ; ++i)
    out->setLegSoftLimitStiffnessReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getLegSoftLimitStiffnessScaleCP(i)));
  for (uint32_t i = 0 ; i < params.maxSpineSoftLimitStiffnessScaleCP ; ++i)
    out->setSpineSoftLimitStiffnessReductionAt(i, 1.0f - NMP::maximum(0.0f, params.getSpineSoftLimitStiffnessScaleCP(i)));
}

//----------------------------------------------------------------------------------------------------------------------
void CharacteristicsBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

