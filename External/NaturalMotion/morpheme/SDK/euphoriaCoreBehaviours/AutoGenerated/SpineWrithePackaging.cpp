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
#include "Spine.h"
#include "SpineWrithe.h"
#include "SpineWrithePackaging.h"
#include "MyNetwork.h"
#include "WritheBehaviourInterface.h"
#include "SpinePackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SpineWrithe_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SpineWrithe*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SpineWrithe_Con::~SpineWrithe_Con()
{
  junc_in_writheParams->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SpineWrithe_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SpineWrithe_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SpineWrithe_Con::buildConnections(SpineWrithe* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SpineWrithe_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_writheParams = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->writheBehaviourInterface->out->getSpinesParams(module->owner->getChildIndex())), (float*)&(module->owner->owner->writheBehaviourInterface->out->getSpinesParamsImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
void SpineWrithe_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SpineWrithe_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_writheParams = ER::Junction::relocate(resource);
  // junc_in_writheParams->getEdges()[0].reinit( (char*)&(module->owner->owner->writheBehaviourInterface->out->getSpinesParams(module->owner->getChildIndex())), (float*)&(module->owner->owner->writheBehaviourInterface->out->getSpinesParamsImportanceRef(module->owner->getChildIndex())) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SpineWrithe_Con::combineInputs(ER::Module* modulePtr)
{
  SpineWrithe* module = (SpineWrithe*)modulePtr;
  SpineWritheInputs& in = *module->in;

  // Junction assignments.
  in.m_writheParamsImportance = junc_in_writheParams->combineDirectInput(&in.writheParams);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


