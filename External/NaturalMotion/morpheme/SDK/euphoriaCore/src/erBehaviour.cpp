// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "euphoria/erBehaviour.h"
#include "euphoria/erModule.h"
#include "euphoria/erEuphoriaLogger.h"

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
Behaviour* BehaviourDef::createInstance()
{
  EUPHORIA_LOG_ENTER_FUNC();
  Behaviour* behaviour = newInstance();
  behaviour->create(this);
  return behaviour;
}

//----------------------------------------------------------------------------------------------------------------------
void Behaviour::create(BehaviourDef* behaviourDef)
{
  EUPHORIA_LOG_ENTER_FUNC();
  m_definition = behaviourDef;
}

//----------------------------------------------------------------------------------------------------------------------
void Behaviour::initialise(Character* owner)
{
  EUPHORIA_LOG_ENTER_FUNC();
  m_owner = owner;
  m_enabled = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Behaviour::destroy()
{
  EUPHORIA_LOG_ENTER_FUNC();
}

//----------------------------------------------------------------------------------------------------------------------
void Behaviour::start()
{
  EUPHORIA_LOG_MESSAGE("Starting behaviour %s (m_enabled = %d) =====================================\n",
    getBehaviourName(), m_enabled);

  m_enabled++;
  uint32_t numToEnable = 0;
  const BehaviourDef::ModuleToEnable* mte = getDefinition()->getModulesToEnable(numToEnable);

  for (uint32_t i = 0; i < numToEnable; i++)
  {
    m_owner->m_euphoriaRootModule->enableOwners(mte[i].m_moduleIndex); // make sure owners are enabled first

    if (mte[i].m_enableWithChildren)
    {
      m_owner->m_euphoriaRootModule->enableModuleAndChildren(mte[i].m_moduleIndex); // recursively enable the children
    }
    else
    {
      m_owner->m_euphoriaRootModule->enable(mte[i].m_moduleIndex); // enable single module only
    }
  }

  clearOutputControlParamsData();
}

//----------------------------------------------------------------------------------------------------------------------
void Behaviour::stop()
{
  EUPHORIA_LOG_MESSAGE("Stopping behaviour %s (m_enabled = %d) =====================================\n",
    getBehaviourName(), m_enabled);

  NMP_ASSERT(m_enabled > 0);
  if (m_enabled <= 0)
    return;
  --m_enabled;
  uint32_t numToEnable = 0;
  const BehaviourDef::ModuleToEnable* mte = getDefinition()->getModulesToEnable(numToEnable);

  for (uint32_t i = 0; i < numToEnable; i++)
  {
    if (mte[i].m_enableWithChildren)
    {
      m_owner->m_euphoriaRootModule->disableModuleAndChildren(mte[i].m_moduleIndex);
    }
    else
    {
      m_owner->m_euphoriaRootModule->disable(mte[i].m_moduleIndex);
    }

    m_owner->m_euphoriaRootModule->disableOwners(mte[i].m_moduleIndex); // then disable up the chain
  }
}

//----------------------------------------------------------------------------------------------------------------------
int32_t Behaviour::getHighestPriority()
{
  return m_priority;
}

//----------------------------------------------------------------------------------------------------------------------
void Behaviour::setHighestPriority(int32_t priority)
{
  m_priority = priority;
}

}
