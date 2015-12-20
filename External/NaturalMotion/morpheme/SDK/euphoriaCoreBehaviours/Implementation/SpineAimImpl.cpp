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
#include "SpineAim.h"
#include "SpineAimPackaging.h"
#include "Helpers/Aim.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
template<> inline void aimEndTargetCorrection<SpineAimUpdatePackage>(
  SpineAimUpdatePackage& pkg,
  NMP::Matrix34& targetTM,
  MR::InstanceDebugInterface* NMP_UNUSED(pDebugDrawInst))
{
  const SpineLimbSharedState& limbState = pkg.owner->owner->data->spineLimbSharedStates[pkg.owner->data->childIndex];

  matchWindingDirection(
    pkg.in->getWindingDirection(),
    limbState.m_endTM,
    pkg.in->getAimInfo().poseEndRelativeToRoot * limbState.m_rootTM,
    targetTM);
}

//----------------------------------------------------------------------------------------------------------------------
void SpineAimUpdatePackage::update(
  float NMP_UNUSED(timeStep),
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  const SpineLimbSharedState& spineLimbState = owner->owner->data->spineLimbSharedStates[owner->data->childIndex];
  const NMP::Matrix34 targetTM = aimUpdate(*this, spineLimbState, pDebugDrawInst);

  out->setTargetEndTM(targetTM, in->getAimInfoImportance());
}

//----------------------------------------------------------------------------------------------------------------------
void SpineAim::entry()
{
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE