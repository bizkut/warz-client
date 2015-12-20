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
#include "BodyFramePackaging.h"
#include "BraceChooserPackaging.h"
#include "BraceChooser.h"
#include "euphoria/erDebugDraw.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BraceChooserUpdatePackage::update(float NMP_UNUSED(timeStep), MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  const BraceState* states = &feedIn->getBraceStates(0);
  int numStates = feedIn->calculateNumBraceStates();
  if (numStates == 0)
    return; // early out

  // The aim of this function is to decide on which feet should brace based on their distance from target, and their
  // masses
  NMP::Vector3 bodyError(0, 0, 0); // this will be the expected motion of the body (without the limbs): we minimise this
  float limbMass = 0.0f;
  for (int i = 0; i < numStates; i++)
  {
    bodyError += states[i].error * states[i].mass;
    limbMass += states[i].mass;
  }
  float bodyErrorMagnitude = bodyError.magnitude();
  bodyErrorMagnitude /= owner->owner->data->totalMass - limbMass;
  float highestEfficiency = -1.0f;
  float efficiencies[10];
  for (int i = 0; i < numStates; i++)
  {
    // a large body motion compared to the required limb motion gives a stark advantage to that one leg conversely a
    // small body motion compared to the required limb motion favours even efficiency, meaning bracing with all legs
    efficiencies[i] = 1.0f + bodyErrorMagnitude / states[i].error.magnitude();
    if (efficiencies[i] > highestEfficiency)
      highestEfficiency = efficiencies[i];
  }
  bool shouldBrace[10];
  float singularFoot = 0.4f; // 0 to 1. 0 means always use all feet to brace, 1 means only use the closest foot
  for (int i = 0; i < numStates; i++)
  {
    if (efficiencies[i] >= highestEfficiency * singularFoot)
      shouldBrace[i] = true;
    else
      shouldBrace[i] = false;
  }

  out->setShouldBrace(numStates, shouldBrace);
}

//----------------------------------------------------------------------------------------------------------------------
void BraceChooser::entry()
{
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

