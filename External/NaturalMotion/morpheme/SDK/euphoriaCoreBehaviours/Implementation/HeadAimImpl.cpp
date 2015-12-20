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
#include "MyNetworkPackaging.h"
#include "MyNetwork.h"
#include "HeadAim.h"
#include "HeadAimPackaging.h"
#include "Helpers/Aim.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
template<> inline void aimEndTargetCorrection<HeadAimUpdatePackage>(
  HeadAimUpdatePackage& pkg,
  NMP::Matrix34& targetTM,
  MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  const HeadLimbSharedState& limbState = pkg.owner->owner->data->headLimbSharedStates[pkg.owner->data->childIndex];

  matchWindingDirection(
    pkg.in->getWindingDirection(),
    limbState.m_endTM,
    pkg.in->getAimInfo().poseEndRelativeToRoot * limbState.m_rootTM,
    targetTM);
}

//----------------------------------------------------------------------------------------------------------------------
void HeadAimUpdatePackage::update(
  float NMP_UNUSED(timeStep),
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  const HeadLimbSharedState& headLimbState = owner->owner->data->headLimbSharedStates[owner->data->childIndex];
  aimUpdate(*this, headLimbState, pDebugDrawInst);
}

//----------------------------------------------------------------------------------------------------------------------
void HeadAim::entry()
{}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE