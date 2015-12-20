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

#include "Types/TranslationRequest.h"

#define SCALING_SOURCE dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
TranslationRequest::TranslationRequest()
{
#if defined(MR_OUTPUT_DEBUGGING)
  debugControlAmounts.setToZero();
#endif
  imminence = -1.0f; // ensure that this gets set (e.g. use SCALE_FREQUENCY(5.0f)) as it needs to be dimensionally scaled
  stiffnessScale = 1.0f;
  passOnAmount = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
TranslationRequest::TranslationRequest(
  const NMP::Vector3& trans,
  const ER::DimensionalScaling dimensionalScaling)
{
  translation = trans;
#if defined(MR_OUTPUT_DEBUGGING)
  debugControlAmounts.setToZero();
#endif
  imminence = SCALE_FREQUENCY(5.0f);
  stiffnessScale = 1.0f;
  passOnAmount = 1.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

