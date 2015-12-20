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

#include "euphoria/erDebugDraw.h"
#include "Types/Hazard.h"
#include "Helpers/Helpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

void Hazard::debugDraw(float MR_OUTPUT_DEBUG_ARG(radius), MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NMP::Colour colour;

  float shade = NMP::clampValue(1.0f - impactTime, 0.0f, 1.0f);
  colour.setf(shade, shade, shade);
  MR_DEBUG_DRAW_PLANE(pDebugDrawInst, impactPosition, contactNormal, radius, colour, 0.05f);
#endif // defined(MR_OUTPUT_DEBUGGING)
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE
