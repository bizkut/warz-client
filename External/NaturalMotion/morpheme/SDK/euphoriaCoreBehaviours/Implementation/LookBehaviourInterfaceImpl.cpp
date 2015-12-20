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
#include "MyNetwork.h"
#include "Helpers/Helpers.h"
#include "LookBehaviourInterface.h"
#include "Behaviours/LookBehaviour.h"
#include "euphoria/erCharacter.h"
#include "NetworkDescriptor.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
static bool shouldLook(const LookBehaviourData& params, LookBehaviourInterface& module)
{
  bool result = true;

  if(params.getIgnoreDirectionWhenOutOfRange())
  {
    // Calculate pitch and yaw from pelvis position and front direction to look target
    NMP::Vector3 targetPositionInCharacterSpace = params.getLookPositionCP();
    module.owner->data->spineLimbSharedStates[0].m_rootTM.inverseTransformVector(targetPositionInCharacterSpace);
    NMP::Quat rotationFromForwardToTarget;
    rotationFromForwardToTarget.forRotation(NMP::Vector3XAxis(), targetPositionInCharacterSpace);
    NMP::Vector3 eulerXYZ = rotationFromForwardToTarget.toEulerXYZ();

    const float targetPitch = eulerXYZ.z;
    const float targetYaw   = eulerXYZ.y;

    const float yawMin   = NMP::degreesToRadians(params.getTargetYawRight());
    const float yawMax   = NMP::degreesToRadians(params.getTargetYawLeft());
    const float pitchMin = NMP::degreesToRadians(params.getTargetPitchDown());
    const float pitchMax = NMP::degreesToRadians(params.getTargetPitchUp());

    result &= NMP::valueInRange(targetYaw, yawMin, yawMax);
    result &= NMP::valueInRange(targetPitch, pitchMin, pitchMax);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void LookBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
#if defined(MR_OUTPUT_DEBUGGING)
  const ER::RootModule* rootModule = getRootModule();
#endif
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());

  const LookBehaviourData& params =
    ((LookBehaviour*)(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Look)))->getParams();

  if (shouldLook(params, *this))
  {
    NMP::Vector3 lookTarget = params.getLookPositionCP();
    float weight = params.getLookWeightCP();

    // optimal is head's stiffness / interceptUrgencyScale, see processRequest
    TargetRequest request(lookTarget, SCALING_SOURCE);
    request.passOnAmount = params.getWholeBodyLookCP();
    request.stiffnessScale = 1.0f;
    out->setTarget(request, weight);

    if (weight > 0.0f)
    {
      MR_DEBUG_DRAW_POINT(rootModule->getDebugInterface(), lookTarget, SCALE_DIST(weight), NMP::Colour::WHITE);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LookBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

