// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "NMPlatform/NMProfiler.h"

#include "euphoria/erCharacter.h"
#include "euphoria/erCharacterDef.h"
#include "euphoria/erBody.h"
#include "euphoria/erModule.h"
#include "euphoria/erEuphoriaUserData.h"
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erNetworkInterface.h"

#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsScene.h"
#include "physics/mrPhysicsSerialisationBuffer.h"

#include <stdio.h>

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
Character::Character()
: m_debugInterface(0)
, m_behaviours(32)
{}

//----------------------------------------------------------------------------------------------------------------------
Character::~Character()
{
  EUPHORIA_LOG_ENTER_FUNC();
}

//----------------------------------------------------------------------------------------------------------------------
void Character::create(CharacterDef* definition, ER::RootModule* network)
{
  EUPHORIA_LOG_ENTER_FUNC();
  m_definition = definition;
  NMP_ASSERT(m_behaviours.getNumUsedSlots() == 0);

  uint32_t numBehaviours = m_definition->m_behaviourDefs->getNumBehaviourDefs();
  for (uint32_t i = 0; i < numBehaviours; ++i)
  {
    BehaviourDef* behaviourDef = m_definition->m_behaviourDefs->getBehaviourDef(i);
    Behaviour* behaviour = behaviourDef->createInstance();
    behaviour->create(behaviourDef);
    m_behaviours.insert(behaviour->getBehaviourID(), behaviour);
  }

  network->create(0);
  m_euphoriaRootModule = network;
}

//----------------------------------------------------------------------------------------------------------------------
void Character::setDebugInterface(MR::InstanceDebugInterface* debugInterface)
{
  m_debugInterface = debugInterface;
  m_euphoriaRootModule->setDebugInterface(debugInterface);
}

//----------------------------------------------------------------------------------------------------------------------
void Character::initialise(uint32_t collisionIgnoreGroups, class Body* body, MR::InstanceDebugInterface* debugInterface)
{
  m_collisionIgnoreGroups = collisionIgnoreGroups;
  m_body = body;
  m_disableBehaviourUpdates = false;
  m_behavioursActiveLastUpdate = false;

  Behaviours::const_value_walker it = m_behaviours.constWalker();
  while (it.next())
  {
    Behaviour* behaviour = it();
    behaviour->initialise(this);
  }

  m_euphoriaRootModule->initialise();
  m_euphoriaRootModule->setCharacter(this);
  m_euphoriaRootModule->enable(0);

  setDebugInterface(debugInterface);
}

//----------------------------------------------------------------------------------------------------------------------
void Character::destroy()
{
  // There's no need to destroy the modules themselves. However, the memory for them will be
  // deallocated later.
  Behaviours::const_value_walker it = m_behaviours.constWalker();
  while (it.next())
  {
    Behaviour* behaviour = it();
    behaviour->destroy();
    NMP::Memory::memFree(behaviour);
  }
  m_behaviours.clear();
}

//----------------------------------------------------------------------------------------------------------------------
Behaviour* Character::getBehaviour(uint32_t behaviourID) const
{
  Behaviour* behaviour;
  if (m_behaviours.find(behaviourID, &behaviour))
  {
    return behaviour;
  }
  else
  {
    NMP_ASSERT_FAIL();
    printf("Unable to find behaviour ID %d\n", behaviourID);
    return 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
const Character::Behaviours& Character::getBehaviours() const
{
  return m_behaviours;
}

//----------------------------------------------------------------------------------------------------------------------
void Character::disableBehaviourEffects()
{
  EUPHORIA_LOG_ENTER_FUNC();
  m_body->disableBehaviourEffects();
  m_behavioursActiveLastUpdate = false;

  // Also need to reset the contacts that still get reported
  if (m_body->getPhysicsRig())
  {
    int numParts = (int)m_body->getPhysicsRig()->getNumParts();
    for (int i = 0; i < numParts; i++)
    {
      ER::EuphoriaRigPartUserData::getFromPart(m_body->getPhysicsRig()->getPart(i))->reset();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool Character::isAnimationRequiredByBehaviour(int behaviourID, int32_t animationID) const
{
  if (m_disableBehaviourUpdates)
  {
    return false;
  }

  Behaviour* behaviour = 0;
  if (m_behaviours.find(behaviourID, &behaviour))
  {
    return behaviour->isAnimationRequired(animationID);
  }
#if defined(MR_OUTPUT_DEBUGGING)
  else
  {
    printf("Invalid behaviour ID %d\n", behaviourID);
  }
#endif

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void Character::startBehaviour(int behaviourID)
{
  Behaviour* behaviour;
  if (m_behaviours.find(behaviourID, &behaviour))
  {
    behaviour->start();
  }
#if defined(MR_OUTPUT_DEBUGGING)
  else
  {
    printf("Invalid behaviour ID %d\n", behaviourID);
  }
#endif
  // force a feedback update
  m_behavioursActiveLastUpdate = false;
}

//----------------------------------------------------------------------------------------------------------------------
void Character::stopBehaviour(int behaviourID)
{
  Behaviour* behaviour;
  if (m_behaviours.find(behaviourID, &behaviour))
  {
    behaviour->stop();
  }
#if defined(MR_OUTPUT_DEBUGGING)
  else
  {
    printf("Invalid behaviour ID %d\n", behaviourID);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Character::reset()
{
  Behaviours::const_value_walker itr = m_behaviours.constWalker();
  while (itr.next())
  {
    Behaviour* behaviour = itr();
    if (behaviour->getEnabled())
    {
      behaviour->stop();
    }
  }
  m_behavioursActiveLastUpdate = false;
}

const int magicNumber = 0x12345678;

//----------------------------------------------------------------------------------------------------------------------
bool Character::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.ptr = savedState.dataStart;
  savedState.addValue(magicNumber);
  if (!m_body->storeState(savedState))
    return false;
  if (!m_euphoriaRootModule->storeState(savedState))
    return false;

  Behaviours::const_value_walker itr = m_behaviours.constWalker();
  while (itr.next())
  {
    Behaviour* behaviour = itr();
    behaviour->storeState(savedState);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Character::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.ptr = savedState.dataStart;
  if (savedState.getValue<int>() != magicNumber)
    return false; // not a valid buffer
  if (!m_body->restoreState(savedState))
    return false;
  if (!m_euphoriaRootModule->restoreState(savedState))
    return false;

  Behaviours::value_walker behaviourWalker = m_behaviours.walker();
  while (behaviourWalker.next())
  {
    Behaviour* bhv = behaviourWalker();
    bhv->restoreState(savedState);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Character::startPoseModification(MR::PhysicsSerialisationBuffer& savedState)
{
  if (m_disableBehaviourUpdates)
  {
    m_body->togglePoseFreeze();
    return true;
  }
  m_disableBehaviourUpdates = true;
  savedState.ptr = savedState.dataStart;

  m_body->startPoseModification(savedState);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Character::stopPoseModification(MR::PhysicsSerialisationBuffer& savedState)
{
  if (!m_disableBehaviourUpdates)
    return true;
  m_disableBehaviourUpdates = false;
  savedState.ptr = savedState.dataStart;

  m_body->stopPoseModification(savedState);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Character::prePhysicsStep(float timeDelta)
{
  if (m_disableBehaviourUpdates)
  {
    // Just need to reset the contacts that still get reported
    if (m_body->getPhysicsRig())
    {
      int numParts = (int)m_body->getPhysicsRig()->getNumParts();
      for (int i = 0; i < numParts; i++)
      {
        ER::EuphoriaRigPartUserData::getFromPart(
          m_body->getPhysicsRig()->getPart(i))->reset();
      }
    }

    if (m_body->getDoingPoseModification())
    {
      // apply world space damping - needs to be explicit (though it could be done with constraints)
      for (uint32_t i = 0 ; i < m_body->getPhysicsRig()->getNumParts() ; ++i)
      {
        MR::PhysicsRig::Part* part = m_body->getPhysicsRig()->getPart(i);

        float mass = part->getMass();
        NMP::Vector3 force = -(m_body->m_physicsScene->getGravity() * mass);
        // Two methods of putting him in treacle - either setting velocities to zero (which works
        // well, but weakens external constraints), or applying explicit damping.
#if 1
        part->setVel(NMP::Vector3(0.0f, 0.0f, 0.0f));
        part->setAngVel(NMP::Vector3(0.0f, 0.0f, 0.0f));
#else
        physx::PxVec3 v = link->getLinearVelocity();
        force += -mass * v / timeDelta;

        physx::PxVec3 av = link->getAngularVelocity();
        static float torqueScale = 0.001f;
        physx::PxVec3 torque = -torqueScale * mass * av / timeDelta;

        link->addTorque(torque);
#endif
        part->addForce(force);
      }
    }
    return;
  }

  // May need to prepare behaviours by running a feedback pass
  if (!m_behavioursActiveLastUpdate)
  {
    // We must clear the feedback outputs here if we're running feedback again, as they will
    // possibly be filled from last feedback, causing errors. This is a temporary solution to keep things
    // working until we get proper management of this. This was added as we are now asserting that outputs
    // are empty before we combine into them, which is not the case when switching behaviours on
    // (the m_behavioursActiveLastUpdate flag gets set back to false in this case even though some
    // modules were active last frame).  MORPH-14076
    m_euphoriaRootModule->getDataAllocator()->clear(ER::ModuleDataAllocator::FeedbackOutputs);
    m_body->postPhysicsStep(timeDelta);
    feedbackNetwork(timeDelta, false);
  }
  m_behavioursActiveLastUpdate = true;

  NM_BEGIN_PROFILING("updateNetwork");
  updateNetwork(timeDelta);           // This is the 'network update'
  NM_END_PROFILING();

  NM_BEGIN_PROFILING("Body::prePhysicsStep");
  m_body->prePhysicsStep(timeDelta, m_debugInterface);  // This is the 'behaviour tech' (minus the 'dynamic state')
  NM_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void Character::postPhysicsStep(float timeDelta)
{
  if (m_disableBehaviourUpdates)
  {
    return;
  }

  NM_BEGIN_PROFILING("Body::postPhysicsStep");
  m_body->postPhysicsStep(timeDelta);
  NM_END_PROFILING();

  NM_BEGIN_PROFILING("feedbackNetwork");
  feedbackNetwork(timeDelta, true);
  NM_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
int32_t Character::getHighestPriority(uint32_t behaviourID)
{
  Behaviour* behaviour = 0;
  if (m_behaviours.find(behaviourID, &behaviour))
  {
    return behaviour->getHighestPriority();
  }
#if defined(MR_OUTPUT_DEBUGGING)
  else
  {
    printf("Invalid behaviour ID %d\n", behaviourID);
  }
#endif
  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
void Character::setHighestPriority(uint32_t behaviourID, int32_t priority)
{
  Behaviour* behaviour = 0;
  if (m_behaviours.find(behaviourID, &behaviour))
  {
    behaviour->setHighestPriority(priority);
  }
#if defined(MR_OUTPUT_DEBUGGING)
  else
  {
    printf("Invalid behaviour ID %d\n", behaviourID);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Character::handleOutputControlParams(
  uint32_t behaviourID,
  ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams,
  size_t numOutputControlParams)
{
  if (m_disableBehaviourUpdates)
  {
    return;
  }

  Behaviour* behaviour;
  if (m_behaviours.find(behaviourID, &behaviour))
  {
    behaviour->handleOutputControlParams(outputControlParams, numOutputControlParams);
  }
#if defined(MR_OUTPUT_DEBUGGING)
  else
  {
    printf("Character::handleOutputControlParams Invalid behaviour ID %d\n", behaviourID);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Character::handleEmitMessages(const uint32_t behaviourID, uint32_t& messages)
{
  if (m_disableBehaviourUpdates)
  {
    return;
  }
  Behaviour* behaviour;
  if (m_behaviours.find(behaviourID, &behaviour))
  {
    behaviour->handleEmittedMessages(messages);
  }
#if defined(MR_OUTPUT_DEBUGGING)
  else
  {
    printf("Character::handleControlParams Invalid behaviour ID %d\n", behaviourID);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Character::handlePhysicsJointMask(
  uint32_t NMP_UNUSED(behaviourID),
  const MR::PhysicsRig::JointChooser& jointChooser)
{
  if (m_body)
  {
    m_body->handlePhysicsJointMask(jointChooser);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Character::updateNetwork(float timeStep)
{
  NMP_ASSERT(timeStep >= 0.0f);
  if (timeStep == 0.0f)
  {
    return; // Nothing needs updating.
  }

  m_euphoriaRootModule->getDataAllocator()->clear(ER::ModuleDataAllocator::Inputs);
  m_euphoriaRootModule->update(timeStep);
  m_euphoriaRootModule->getDataAllocator()->clear(ER::ModuleDataAllocator::FeedbackOutputs);
}

//----------------------------------------------------------------------------------------------------------------------
void Character::feedbackNetwork(float timeStep, bool clearUserInputs)
{
  // Note, this timeStep early-out relies on the fact that feedback control parameters are not cleared each frame, hence
  // There will still be valid feedback data when timeStep=0. If this changes then doing an early-out here may not
  // be viable
  NMP_ASSERT(timeStep >= 0.0f);
  if (timeStep == 0.0f)
  {
    return; // Nothing needs updating.
  }

  NM_BEGIN_PROFILING("feedbackNetwork");
  m_euphoriaRootModule->getDataAllocator()->clear(ER::ModuleDataAllocator::FeedbackInputs);
  m_euphoriaRootModule->feedback(timeStep);
  m_euphoriaRootModule->getDataAllocator()->clear(ER::ModuleDataAllocator::Outputs);
  // Clear user inputs here so that writing to them by game code etc doesn't get overwritten.
  if (clearUserInputs)
    m_euphoriaRootModule->getDataAllocator()->clear(ER::ModuleDataAllocator::UserInputs);
  NM_END_PROFILING();
}
}
