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

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SpineWrithe::SpineWrithe(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (SpineWritheData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(SpineWritheData), __alignof(SpineWritheData));
  in = (SpineWritheInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SpineWritheInputs), __alignof(SpineWritheInputs));
  out = (SpineWritheOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Outputs, sizeof(SpineWritheOutputs), __alignof(SpineWritheOutputs));

  m_apiBase = (SpineWritheAPIBase*)NMPMemoryAllocAligned(sizeof(SpineWritheAPIBase), 16);
  m_updatePackage = (SpineWritheUpdatePackage*)NMPMemoryAllocAligned(sizeof(SpineWritheUpdatePackage), 16);
  m_feedbackPackage = (SpineWritheFeedbackPackage*)NMPMemoryAllocAligned(sizeof(SpineWritheFeedbackPackage), 16);
}

//----------------------------------------------------------------------------------------------------------------------
SpineWrithe::~SpineWrithe()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
  NMP::Memory::memFree(m_updatePackage);
  NMP::Memory::memFree(m_feedbackPackage);
}

//----------------------------------------------------------------------------------------------------------------------
void SpineWrithe::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (Spine*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) SpineWritheAPIBase( data, in, owner->m_apiBase);
  new(m_updatePackage) SpineWritheUpdatePackage(
    data, in, out, 
    owner->m_apiBase  );
  new(m_feedbackPackage) SpineWritheFeedbackPackage(
    data, in, owner->m_apiBase  );
}

//----------------------------------------------------------------------------------------------------------------------
void SpineWrithe::clearAllData()
{
  data->clear();
  in->clear();
  out->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SpineWrithe::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*out);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SpineWrithe::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<SpineWritheData>();
  *in = savedState.getValue<SpineWritheInputs>();
  *out = savedState.getValue<SpineWritheOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SpineWrithe::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SpineWrithe::update(float timeStep)
{
  const ER::RootModule* rootModule = getRootModule();
  MR_DEBUG_MODULE_ENTER(rootModule->getDebugInterface(), getClassName());
  m_updatePackage->update(timeStep, rootModule->getDebugInterface());
#if NM_CALL_VALIDATORS
  data->validate();
#endif // NM_CALL_VALIDATORS
}

//----------------------------------------------------------------------------------------------------------------------
// No feedback() required

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


