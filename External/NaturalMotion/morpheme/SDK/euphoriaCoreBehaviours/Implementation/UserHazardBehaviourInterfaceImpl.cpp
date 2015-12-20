/*
* Copyright (c) 2012 NaturalMotion Ltd. All rights reserved.
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
#include "UserHazardBehaviourInterface.h"
#include "NetworkDescriptor.h"
#include "Behaviours/UserHazardBehaviour.h"
#include "MyNetwork.h"
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void UserHazardBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  // Create a user defined hazard by directly controlling the data sent to HazardAwarenessHeadAndChest... this ensures
  // that all actions will follow (head avoid, headDodge, brace etc) not just arm actions.

  const UserHazardBehaviourData& params = static_cast<const UserHazardBehaviour*>(
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_UserHazard)))->getParams();

  // Copy any patches defined by client code.
  PatchSet potentialHazards = userIn->getGameHazards();

  if (params.getUserHazardCP())
  {
    // Append patch defined by input CPs to the end of the potential hazards patches array, overwriting the last game
    // defined patch if the array is at capacity.
    potentialHazards.numPatches = NMP::minimum(potentialHazards.numPatches, PatchSet::MAX_NUM_PATCHES - 1);
    potentialHazards.patches[potentialHazards.numPatches].createAsSphere(
      params.getHazardPositionCP(),
      params.getHazardRadiusCP(),
      params.getHazardVelocityCP(),
      params.getHazardMassCP(),
      0);
    ++potentialHazards.numPatches;
  }

  if (potentialHazards.numPatches > 0)
  {
    out->setPotentialHazards(potentialHazards);
  }

  out->setIgnoreNonUserHazards(params.getIgnoreOtherHazardsCP());
}

//----------------------------------------------------------------------------------------------------------------------
void UserHazardBehaviourInterface::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

