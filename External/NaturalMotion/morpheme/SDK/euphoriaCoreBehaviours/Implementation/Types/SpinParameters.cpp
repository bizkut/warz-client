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

#include "Types/SpinParameters.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SpinParameters::SpinParameters()
{
  maxRadius = 0.0f;
  maxAngSpeed = 0.0f;
  spinWeightLateral = 0.0f;
  spinWeightUp = 0.0f;
  spinWeightForward = 0.0f;
  spinCentreLateral = 0.0f;
  spinCentreUp = 0.0f;
  spinCentreForward = 0.0f;
  spinOutwardsDistanceWhenBehind = 0.0f;
  spinArmControlCompensationScale = 0.0f;
  strengthReductionTowardsEnd = 0.0f;
  synchronised = false;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

