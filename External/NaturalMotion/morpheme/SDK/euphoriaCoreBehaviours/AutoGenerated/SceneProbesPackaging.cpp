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
#include "SceneProbes.h"
#include "SceneProbesPackaging.h"
#include "Head.h"
#include "HeadEyes.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SceneProbes_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SceneProbes*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SceneProbes_Con::~SceneProbes_Con()
{
  junc_in_focalCentre->destroy();
  junc_in_focalRadius->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SceneProbes_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SceneProbes_Con), 16);

  result += ER::Junction::getMemoryRequirements(1);
  result += ER::Junction::getMemoryRequirements(1);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SceneProbes_Con::buildConnections(SceneProbes* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SceneProbes_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_focalRadius = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->out->getFocalRadius()), (float*)&(module->owner->heads[0]->eyes->out->getFocalRadiusImportanceRef()) );

  junc_in_focalCentre = activeJnc = ER::Junction::init(resource, 1, ER::Junction::directInput);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->out->getFocalCentre()), (float*)&(module->owner->heads[0]->eyes->out->getFocalCentreImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void SceneProbes_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SceneProbes_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_focalRadius = ER::Junction::relocate(resource);
  // junc_in_focalRadius->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->out->getFocalRadius()), (float*)&(module->owner->heads[0]->eyes->out->getFocalRadiusImportanceRef()) );

  junc_in_focalCentre = ER::Junction::relocate(resource);
  // junc_in_focalCentre->getEdges()[0].reinit( (char*)&(module->owner->heads[0]->eyes->out->getFocalCentre()), (float*)&(module->owner->heads[0]->eyes->out->getFocalCentreImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SceneProbes_Con::combineInputs(ER::Module* modulePtr)
{
  SceneProbes* module = (SceneProbes*)modulePtr;
  SceneProbesInputs& in = *module->in;

  // Junction assignments.
  in.m_focalRadiusImportance = junc_in_focalRadius->combineDirectInput(&in.focalRadius);
  in.m_focalCentreImportance = junc_in_focalCentre->combineDirectInput(&in.focalCentre);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


