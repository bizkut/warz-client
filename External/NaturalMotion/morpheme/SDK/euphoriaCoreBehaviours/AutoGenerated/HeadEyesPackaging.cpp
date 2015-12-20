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

//----------------------------------------------------------------------------------------------------------------------
//                                  This file is auto-generated
//----------------------------------------------------------------------------------------------------------------------

// module def dependencies
#include "Head.h"
#include "HeadEyes.h"
#include "HeadEyesPackaging.h"
#include "MyNetwork.h"
#include "LookBehaviourInterface.h"
#include "RandomLook.h"
#include "BalanceManagement.h"
#include "SteppingBalance.h"
#include "ObserveBehaviourInterface.h"
#include "HazardManagement.h"
#include "ImpactPredictor.h"
#include "FreeFallManagement.h"
#include "BodyFrame.h"
#include "ReachForBody.h"
#include "ReachForWorldBehaviourInterface.h"
#include "GrabDetection.h"
#include "Grab.h"
#include "HeadPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void HeadEyes_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((HeadEyes*)module);
}

//----------------------------------------------------------------------------------------------------------------------
HeadEyes_Con::~HeadEyes_Con()
{
  junc_in_target->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format HeadEyes_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(HeadEyes_Con), 16);

  result += ER::Junction::getMemoryRequirements(10);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void HeadEyes_Con::buildConnections(HeadEyes* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadEyes_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_target = activeJnc = ER::Junction::init(resource, 10, ER::Junction::winnerTakesAll);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->lookBehaviourInterface->out->getTarget()), (float*)&(module->owner->owner->lookBehaviourInterface->out->getTargetImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->randomLook->out->getLookTarget()), (float*)&(module->owner->owner->randomLook->out->getLookTargetImportanceRef()) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->balanceManagement->steppingBalance->out->getLookTarget()), (float*)&(module->owner->owner->balanceManagement->steppingBalance->out->getLookTargetImportanceRef()) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->owner->observeBehaviourInterface->out->getFocalCentre()), (float*)&(module->owner->owner->observeBehaviourInterface->out->getFocalCentreImportanceRef()) );
  activeJnc->getEdges()[4].reinit( (char*)&(module->owner->owner->hazardManagement->chestImpactPredictor->out->getFocalCentre()), (float*)&(module->owner->owner->hazardManagement->chestImpactPredictor->out->getFocalCentreImportanceRef()) );
  activeJnc->getEdges()[5].reinit( (char*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getFocalCentre()), (float*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getFocalCentreImportanceRef()) );
  activeJnc->getEdges()[6].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getLookTarget()), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getLookTargetImportanceRef()) );
  activeJnc->getEdges()[7].reinit( (char*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getLookTarget()), (float*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getLookTargetImportanceRef()) );
  activeJnc->getEdges()[8].reinit( (char*)&(module->owner->owner->hazardManagement->grabDetection->out->getLookTarget()), (float*)&(module->owner->owner->hazardManagement->grabDetection->out->getLookTargetImportanceRef()) );
  activeJnc->getEdges()[9].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getLookTarget()), (float*)&(module->owner->owner->hazardManagement->grab->out->getLookTargetImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void HeadEyes_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(HeadEyes_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_target = ER::Junction::relocate(resource);
  // junc_in_target->getEdges()[0].reinit( (char*)&(module->owner->owner->lookBehaviourInterface->out->getTarget()), (float*)&(module->owner->owner->lookBehaviourInterface->out->getTargetImportanceRef()) );
  // junc_in_target->getEdges()[1].reinit( (char*)&(module->owner->owner->randomLook->out->getLookTarget()), (float*)&(module->owner->owner->randomLook->out->getLookTargetImportanceRef()) );
  // junc_in_target->getEdges()[2].reinit( (char*)&(module->owner->owner->balanceManagement->steppingBalance->out->getLookTarget()), (float*)&(module->owner->owner->balanceManagement->steppingBalance->out->getLookTargetImportanceRef()) );
  // junc_in_target->getEdges()[3].reinit( (char*)&(module->owner->owner->observeBehaviourInterface->out->getFocalCentre()), (float*)&(module->owner->owner->observeBehaviourInterface->out->getFocalCentreImportanceRef()) );
  // junc_in_target->getEdges()[4].reinit( (char*)&(module->owner->owner->hazardManagement->chestImpactPredictor->out->getFocalCentre()), (float*)&(module->owner->owner->hazardManagement->chestImpactPredictor->out->getFocalCentreImportanceRef()) );
  // junc_in_target->getEdges()[5].reinit( (char*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getFocalCentre()), (float*)&(module->owner->owner->hazardManagement->freeFallManagement->out->getFocalCentreImportanceRef()) );
  // junc_in_target->getEdges()[6].reinit( (char*)&(module->owner->owner->bodyFrame->reachForBody->out->getLookTarget()), (float*)&(module->owner->owner->bodyFrame->reachForBody->out->getLookTargetImportanceRef()) );
  // junc_in_target->getEdges()[7].reinit( (char*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getLookTarget()), (float*)&(module->owner->owner->reachForWorldBehaviourInterface->out->getLookTargetImportanceRef()) );
  // junc_in_target->getEdges()[8].reinit( (char*)&(module->owner->owner->hazardManagement->grabDetection->out->getLookTarget()), (float*)&(module->owner->owner->hazardManagement->grabDetection->out->getLookTargetImportanceRef()) );
  // junc_in_target->getEdges()[9].reinit( (char*)&(module->owner->owner->hazardManagement->grab->out->getLookTarget()), (float*)&(module->owner->owner->hazardManagement->grab->out->getLookTargetImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void HeadEyes_Con::combineInputs(ER::Module* modulePtr)
{
  HeadEyes* module = (HeadEyes*)modulePtr;
  HeadEyesInputs& in = *module->in;

  // Junction assignments.
  in.m_targetImportance = junc_in_target->combineWinnerTakesAll(&in.target);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


