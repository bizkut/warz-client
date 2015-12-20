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
#include "MyNetworkPackaging.h"
#include "BodySectionPackaging.h"
#include "BodySection.h"
#include "SupportPolygonData.h"

#define SCALING_SOURCE pkg.owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

#include "euphoria/erDebugDraw.h"

#include "Types/Environment_SupportPoly.h"

//----------------------------------------------------------------------------------------------------------------------


#undef SCALING_SOURCE
#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BodySection::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

