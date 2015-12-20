// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_PHYSICS_TASKS_H
#define MR_PHYSICS_TASKS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsAttribData.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
struct PhysicsInitialisation
{
  const NMP::DataBuffer* m_transforms;
  const NMP::DataBuffer* m_prevTransforms;
  float            m_deltaTime;
  NMP::Matrix34    m_worldRoot;
  NMP::Matrix34    m_prevWorldRoot;
};

//----------------------------------------------------------------------------------------------------------------------
void initialisePhysics(
  const PhysicsInitialisation& physicsInitialisation,
  PhysicsRig*                  physicsRig,
  AttribDataPhysicsState*      physicsState,
  Dispatcher*                  dispatcher);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_TASKS_H
//----------------------------------------------------------------------------------------------------------------------
