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
#include "BodySection.h"
#include "BraceChooser.h"
#include "BraceChooserPackaging.h"
#include "BodySectionPackaging.h"
#include "MyNetwork.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void BraceChooser_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((BraceChooser*)module);
}

//----------------------------------------------------------------------------------------------------------------------
BraceChooser_Con::~BraceChooser_Con()
{
  junc_feedIn_braceStates_1->destroy();
  junc_feedIn_braceStates_0->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format BraceChooser_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(BraceChooser_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void BraceChooser_Con::buildConnections(BraceChooser* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BraceChooser_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_feedIn_braceStates_0 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getBraceStates(0)), (float*)&(module->owner->feedIn->getBraceStatesImportanceRef(0)) );
  junc_feedIn_braceStates_1 = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getBraceStates(1)), (float*)&(module->owner->feedIn->getBraceStatesImportanceRef(1)) );

}

//----------------------------------------------------------------------------------------------------------------------
void BraceChooser_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(BraceChooser_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_feedIn_braceStates_0 = ER::Junction::relocate(resource);
  // junc_feedIn_braceStates_0->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getBraceStates(0)), (float*)&(module->owner->feedIn->getBraceStatesImportanceRef(0)) );
  junc_feedIn_braceStates_1 = ER::Junction::relocate(resource);
  // junc_feedIn_braceStates_1->getEdges()[0].reinit( (char*)&(module->owner->feedIn->getBraceStates(1)), (float*)&(module->owner->feedIn->getBraceStatesImportanceRef(1)) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void BraceChooser_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  BraceChooser* module = (BraceChooser*)modulePtr;
  BraceChooserFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_braceStatesImportance[0] = junc_feedIn_braceStates_0->combineDirectInput(&feedIn.braceStates[0]);
  feedIn.m_braceStatesImportance[1] = junc_feedIn_braceStates_1->combineDirectInput(&feedIn.braceStates[1]);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


