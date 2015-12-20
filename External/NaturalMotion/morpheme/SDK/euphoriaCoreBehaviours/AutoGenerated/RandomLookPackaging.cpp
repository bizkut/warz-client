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
#include "MyNetwork.h"
#include "RandomLook.h"
#include "RandomLookPackaging.h"
#include "IdleAwakeBehaviourInterface.h"
#include "WritheBehaviourInterface.h"
#include "Head.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void RandomLook_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((RandomLook*)module);
}

//----------------------------------------------------------------------------------------------------------------------
RandomLook_Con::~RandomLook_Con()
{
  junc_feedIn_headPosition->destroy();
  junc_pri01->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format RandomLook_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(RandomLook_Con), 16);

  result += ER::Junction::getMemoryRequirements(2);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void RandomLook_Con::buildConnections(RandomLook* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(RandomLook_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_pri01 = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->idleAwakeBehaviourInterface->out->getRandomLookParameters()), (float*)&(module->owner->idleAwakeBehaviourInterface->out->getRandomLookParametersImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->writheBehaviourInterface->out->getRandomLookParameters()), (float*)&(module->owner->writheBehaviourInterface->out->getRandomLookParametersImportanceRef()) );

  junc_feedIn_headPosition = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->feedOut->getCurrEndPartPosition()), (float*)&(module->owner->heads[0]->feedOut->getCurrEndPartPositionImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void RandomLook_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(RandomLook_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_pri01 = ER::Junction::relocate(resource);
  // junc_pri01->getEdges()[0].reinit( (char*)&(module->owner->idleAwakeBehaviourInterface->out->getRandomLookParameters()), (float*)&(module->owner->idleAwakeBehaviourInterface->out->getRandomLookParametersImportanceRef()) );
  // junc_pri01->getEdges()[1].reinit( (char*)&(module->owner->writheBehaviourInterface->out->getRandomLookParameters()), (float*)&(module->owner->writheBehaviourInterface->out->getRandomLookParametersImportanceRef()) );

  junc_feedIn_headPosition = ER::Junction::relocate(resource);
  // junc_feedIn_headPosition->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->feedOut->getCurrEndPartPosition()), (float*)&(module->owner->heads[0]->feedOut->getCurrEndPartPositionImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void RandomLook_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  RandomLook* module = (RandomLook*)modulePtr;
  RandomLookFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_headPositionImportance = junc_feedIn_headPosition->combineDirectInput(&feedIn.headPosition);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void RandomLook_Con::combineInputs(ER::Module* modulePtr)
{
  RandomLook* module = (RandomLook*)modulePtr;
  RandomLookInputs& in = *module->in;

  // Junction assignments.
  in.m_randomLookParametersImportance = junc_pri01->combinePriority(&in.randomLookParameters);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


