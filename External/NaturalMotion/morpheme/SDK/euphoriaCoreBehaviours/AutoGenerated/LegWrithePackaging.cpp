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
#include "Leg.h"
#include "LegWrithe.h"
#include "LegWrithePackaging.h"
#include "MyNetwork.h"
#include "WritheBehaviourInterface.h"
#include "LegPackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void LegWrithe_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((LegWrithe*)module);
}

//----------------------------------------------------------------------------------------------------------------------
LegWrithe_Con::~LegWrithe_Con()
{
  junc_in_writheParams->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format LegWrithe_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(LegWrithe_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void LegWrithe_Con::buildConnections(LegWrithe* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegWrithe_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_writheParams = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->writheBehaviourInterface->out->getLegsParams(module->owner->getChildIndex())), (float*)&(module->owner->owner->writheBehaviourInterface->out->getLegsParamsImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
void LegWrithe_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(LegWrithe_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_writheParams = ER::Junction::relocate(resource);
  // junc_in_writheParams->getEdges()[0].reinit( (char*)&(module->owner->owner->writheBehaviourInterface->out->getLegsParams(module->owner->getChildIndex())), (float*)&(module->owner->owner->writheBehaviourInterface->out->getLegsParamsImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void LegWrithe_Con::combineInputs(ER::Module* modulePtr)
{
  LegWrithe* module = (LegWrithe*)modulePtr;
  LegWritheInputs& in = *module->in;

  // Junction assignments.
  in.m_writheParamsImportance = junc_in_writheParams->combineDirectInput(&in.writheParams);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


