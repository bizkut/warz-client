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

// misc
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erDebugDraw.h"
#include "physics/mrPhysicsSerialisationBuffer.h"



namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
SceneProbes::SceneProbes(ER::ModuleDataAllocator* mdAllocator, ER::ModuleCon* connectionSet) : ER::Module(mdAllocator, connectionSet)
{
  data = (SceneProbesData*)mdAllocator->alloc(ER::ModuleDataAllocator::Data, sizeof(SceneProbesData), __alignof(SceneProbesData));
  in = (SceneProbesInputs*)mdAllocator->alloc(ER::ModuleDataAllocator::Inputs, sizeof(SceneProbesInputs), __alignof(SceneProbesInputs));
  feedOut = (SceneProbesFeedbackOutputs*)mdAllocator->alloc(ER::ModuleDataAllocator::FeedbackOutputs, sizeof(SceneProbesFeedbackOutputs), __alignof(SceneProbesFeedbackOutputs));

  m_apiBase = (SceneProbesAPIBase*)NMPMemoryAllocAligned(sizeof(SceneProbesAPIBase), 16);
}

//----------------------------------------------------------------------------------------------------------------------
SceneProbes::~SceneProbes()
{
  owner = 0;

  NMP::Memory::memFree(m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void SceneProbes::create(ER::Module* parent, int childIndex)
{
  ER::Module::create(parent, childIndex);
  owner = (MyNetwork*)parent;
  if (getConnections())
    getConnections()->create(this, (ER::Module*)owner);


  new(m_apiBase) SceneProbesAPIBase( data, in, owner->m_apiBase);
}

//----------------------------------------------------------------------------------------------------------------------
void SceneProbes::clearAllData()
{
  data->clear();
  in->clear();
  feedOut->clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SceneProbes::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*data);
  savedState.addValue(*in);
  savedState.addValue(*feedOut);
  storeStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SceneProbes::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  *data = savedState.getValue<SceneProbesData>();
  *in = savedState.getValue<SceneProbesInputs>();
  *feedOut = savedState.getValue<SceneProbesFeedbackOutputs>();
  restoreStateChildren(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* SceneProbes::getChildName(int32_t) const
{
  NMP_ASSERT_FAIL();
  return 0;
}


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE


