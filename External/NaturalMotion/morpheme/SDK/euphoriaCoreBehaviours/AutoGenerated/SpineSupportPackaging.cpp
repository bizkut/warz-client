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
#include "SpineSupport.h"
#include "SpineSupportPackaging.h"
#include "MyNetwork.h"
#include "BalanceManagement.h"
#include "StaticBalance.h"
#include "BalancePoser.h"
#include "BodyFrame.h"
#include "SittingBodyBalance.h"
#include "BodyBalance.h"
#include "SpinePackaging.h"
#include "MyNetworkPackaging.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
void SpineSupport_Con::create(ER::Module* module, ER::Module* NMP_UNUSED(owner))
{
  buildConnections((SpineSupport*)module);
}

//----------------------------------------------------------------------------------------------------------------------
SpineSupport_Con::~SpineSupport_Con()
{
  junc_in_targetPelvisTM->destroy();
  junc_in_desiredEndTranslationRelRoot->destroy();
  junc_in_desiredEndRotationRelRoot->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SpineSupport_Con::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(SpineSupport_Con), 16);

  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(4);
  result += ER::Junction::getMemoryRequirements(2);
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
void SpineSupport_Con::buildConnections(SpineSupport* module)
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SpineSupport_Con), 16);
  resource.increment(classFormat);

  ER::Junction* activeJnc = 0;

  // build and connect up all junctions
  junc_in_desiredEndRotationRelRoot = activeJnc = ER::Junction::init(resource, 4, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->feedOut->getDesiredEndRotationRelRoot()), (float*)&(module->owner->feedOut->getDesiredEndRotationRelRootImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->balanceManagement->staticBalance->out->getChestRelPelvisOrientation(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->staticBalance->out->getChestRelPelvisOrientationImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->balanceManagement->balancePoser->out->getChestRelPelvisOrientation(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->balancePoser->out->getChestRelPelvisOrientationImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getChestRelPelvisOrientation(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getChestRelPelvisOrientationImportanceRef(module->owner->getChildIndex())) );

  junc_in_desiredEndTranslationRelRoot = activeJnc = ER::Junction::init(resource, 4, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->feedOut->getDesiredEndTranslationRelRoot()), (float*)&(module->owner->feedOut->getDesiredEndTranslationRelRootImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->balanceManagement->staticBalance->out->getChestRelPelvisTranslation(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->staticBalance->out->getChestRelPelvisTranslationImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[2].reinit( (char*)&(module->owner->owner->balanceManagement->balancePoser->out->getChestRelPelvisTranslation(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->balancePoser->out->getChestRelPelvisTranslationImportanceRef(module->owner->getChildIndex())) );
  activeJnc->getEdges()[3].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getChestRelPelvisTranslation(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getChestRelPelvisTranslationImportanceRef(module->owner->getChildIndex())) );

  junc_in_targetPelvisTM = activeJnc = ER::Junction::init(resource, 2, ER::Junction::priority);
  activeJnc->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getTargetPelvisTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getTargetPelvisTMImportanceRef()) );
  activeJnc->getEdges()[1].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredPelvisTM()), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredPelvisTMImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
void SpineSupport_Con::relocate()
{
  // Reconstruct the previously-allocated resource to fill in the junctions
  NMP::Memory::Format format = getMemoryRequirements();
  NMP::Memory::Resource resource = {this, format};
  NMP::Memory::Format classFormat(sizeof(SpineSupport_Con), 16);
  resource.increment(classFormat);

  // Reconnect all junctions
  junc_in_desiredEndRotationRelRoot = ER::Junction::relocate(resource);
  // junc_in_desiredEndRotationRelRoot->getEdges()[0].reinit( (char*)&(module->owner->feedOut->getDesiredEndRotationRelRoot()), (float*)&(module->owner->feedOut->getDesiredEndRotationRelRootImportanceRef()) );
  // junc_in_desiredEndRotationRelRoot->getEdges()[1].reinit( (char*)&(module->owner->owner->balanceManagement->staticBalance->out->getChestRelPelvisOrientation(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->staticBalance->out->getChestRelPelvisOrientationImportanceRef(module->owner->getChildIndex())) );
  // junc_in_desiredEndRotationRelRoot->getEdges()[2].reinit( (char*)&(module->owner->owner->balanceManagement->balancePoser->out->getChestRelPelvisOrientation(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->balancePoser->out->getChestRelPelvisOrientationImportanceRef(module->owner->getChildIndex())) );
  // junc_in_desiredEndRotationRelRoot->getEdges()[3].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getChestRelPelvisOrientation(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getChestRelPelvisOrientationImportanceRef(module->owner->getChildIndex())) );

  junc_in_desiredEndTranslationRelRoot = ER::Junction::relocate(resource);
  // junc_in_desiredEndTranslationRelRoot->getEdges()[0].reinit( (char*)&(module->owner->feedOut->getDesiredEndTranslationRelRoot()), (float*)&(module->owner->feedOut->getDesiredEndTranslationRelRootImportanceRef()) );
  // junc_in_desiredEndTranslationRelRoot->getEdges()[1].reinit( (char*)&(module->owner->owner->balanceManagement->staticBalance->out->getChestRelPelvisTranslation(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->staticBalance->out->getChestRelPelvisTranslationImportanceRef(module->owner->getChildIndex())) );
  // junc_in_desiredEndTranslationRelRoot->getEdges()[2].reinit( (char*)&(module->owner->owner->balanceManagement->balancePoser->out->getChestRelPelvisTranslation(module->owner->getChildIndex())), (float*)&(module->owner->owner->balanceManagement->balancePoser->out->getChestRelPelvisTranslationImportanceRef(module->owner->getChildIndex())) );
  // junc_in_desiredEndTranslationRelRoot->getEdges()[3].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getChestRelPelvisTranslation(module->owner->getChildIndex())), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getChestRelPelvisTranslationImportanceRef(module->owner->getChildIndex())) );

  junc_in_targetPelvisTM = ER::Junction::relocate(resource);
  // junc_in_targetPelvisTM->getEdges()[0].reinit( (char*)&(module->owner->owner->bodyFrame->bodyBalance->out->getTargetPelvisTM()), (float*)&(module->owner->owner->bodyFrame->bodyBalance->out->getTargetPelvisTMImportanceRef()) );
  // junc_in_targetPelvisTM->getEdges()[1].reinit( (char*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredPelvisTM()), (float*)&(module->owner->owner->bodyFrame->sittingBodyBalance->out->getDesiredPelvisTMImportanceRef()) );

}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SpineSupport_Con::combineFeedbackInputs(ER::Module* modulePtr)
{
  SpineSupport* module = (SpineSupport*)modulePtr;
  SpineSupportFeedbackInputs& feedIn = *module->feedIn;

  // Junction assignments.
  feedIn.m_targetPelvisTMImportance = junc_in_targetPelvisTM->combinePriority(&feedIn.targetPelvisTM);

#if NM_CALL_VALIDATORS
  feedIn.validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// Combiner api.
void SpineSupport_Con::combineInputs(ER::Module* modulePtr)
{
  SpineSupport* module = (SpineSupport*)modulePtr;
  SpineSupportInputs& in = *module->in;

  // Junction assignments.
  in.m_desiredEndRotationRelRootImportance = junc_in_desiredEndRotationRelRoot->combinePriority(&in.desiredEndRotationRelRoot);
  PostCombiners::QuatNormalise(in.desiredEndRotationRelRoot);
  in.m_desiredEndTranslationRelRootImportance = junc_in_desiredEndTranslationRelRoot->combinePriority(&in.desiredEndTranslationRelRoot);

#if NM_CALL_VALIDATORS
  in.validate();
#endif // NM_CALL_VALIDATORS
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


