// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "euphoria/erModule.h"
#include "euphoria/erEuphoriaLogger.h"

#include <ctime>

#define ENABLE_ER_MODULE_LOGGING 0

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
Module::~Module()
{
  EUPHORIA_LOG_ENTER_FUNC();
}

//----------------------------------------------------------------------------------------------------------------------
void Module::initialise()
{
  clearAllData();
  int32_t number = getNumChildren();
  for (int32_t i = 0; i < number; i++)
    getChild(i)->initialise();
}

//----------------------------------------------------------------------------------------------------------------------
bool Module::storeStateChildren(MR::PhysicsSerialisationBuffer& savedState)
{
  int32_t number = getNumChildren();
  for (int32_t i = number - 1; i >= 0; i--)
  {
    Module* module = getChild(i);
    module->storeState(savedState);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Module::restoreStateChildren(MR::PhysicsSerialisationBuffer& savedState)
{
  int32_t number = getNumChildren();
  for (int32_t i = number - 1; i >= 0; i--)
  {
    Module* module = getChild(i);
    module->restoreState(savedState);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Module::getModuleNames(char* names, char* parentNames, int32_t nameLength, int32_t& numModules)
{
  for (int32_t i = 0; i < numModules; i++)
  {
    if (!strcmp(getClassName(), &names[nameLength*i])) // already considered this module type
      return;
  }

  NMP_STRNCPY_S(&names[nameLength*(numModules)], nameLength, getClassName());
  const char* parentName = getOwner() ? getOwner()->getClassName() : "";
  NMP_STRNCPY_S(&parentNames[nameLength*(numModules)], nameLength, parentName);
  numModules++;

  int32_t numChildren = getNumChildren();
  for (int32_t i = 0; i < numChildren; ++i)
  {
    Module* module = getChild(i);
    module->getModuleNames(names, parentNames, nameLength, numModules);
  }
}

//----------------------------------------------------------------------------------------------------------------------
RootModule::RootModule() : 
  Module(0, 0),
  m_character(0),
  m_allModules(0),
  m_numModules(0),
  m_moduleEnableStates(0)
{
  time_t t = time(0);
  m_networkSeed = (uint32_t) t;
}

//----------------------------------------------------------------------------------------------------------------------
const RootModule* Module::getRootModule() const
{
  const Module* result = this;
  while (result->getOwner())
  {
    result = result->getOwner();
  }
  return (const RootModule*) result;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t RootModule::getAllDebugEnabledModuleNames(char* names, int nameLength) const
{
  int32_t numModules = 0;
#if defined(MR_OUTPUT_DEBUGGING)
  getDebugEnabledModuleNames(names, nameLength, numModules);
#else
  (void) names;
  (void) nameLength;
#endif
  return numModules;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t RootModule::getAllEnabledModules(Module** modules)
{
  int32_t numModules = 0;
  getEnabledModules(modules, numModules);
  return numModules;
}

//----------------------------------------------------------------------------------------------------------------------
bool RootModule::isModuleEnabled(const char* name)
{
  // Search through all the enabled modules.
  // TODO this function should be cleaned up.
  const int32_t maxModules = 128;
  Module* enabledModules[maxModules];
  int32_t numEnabledModules = getAllEnabledModules(enabledModules);
  NMP_ASSERT(numEnabledModules < maxModules);
  for (int32_t i = 0; i < numEnabledModules; ++i)
  {
    Module* module = enabledModules[i];
    if (strcmp(module->getClassName(), name) == 0)
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void RootModule::setCharacter(ER::Character* myCharacter)
{
  m_character = myCharacter;
  initLimbIndices();
}

//----------------------------------------------------------------------------------------------------------------------
void RootModule::enable(uint32_t moduleIndex)
{
  NMP_ASSERT(moduleIndex < m_numModules);
#if ENABLE_ER_MODULE_LOGGING
  EUPHORIA_LOG_MESSAGE("Enabling module %s (count %d)\n", getClassName(), m_moduleEnableStates[moduleIndex].m_enabled);
#endif
  ModuleEnableState& mes = m_moduleEnableStates[moduleIndex];

  if (!mes.m_enabled) // each module should recursively enable its children and its owner
  {
    m_allModules[moduleIndex]->clearAllData();
    m_allModules[moduleIndex]->entry();
  }
  mes.m_enabled++;
}

//----------------------------------------------------------------------------------------------------------------------
void RootModule::enableOwners(uint32_t moduleIndex)
{
  NMP_ASSERT(moduleIndex < m_numModules);
#if ENABLE_ER_MODULE_LOGGING
  EUPHORIA_LOG_MESSAGE("Enabling module %s owners\n", getClassName());
#endif
  Module* owner = (Module*)m_allModules[moduleIndex]->getOwner();  // we have to force it to be non-const here
  if (owner)
  {
    enableOwners(owner->getManifestIndex());
    enable(owner->getManifestIndex());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RootModule::enableModuleAndChildren(uint32_t moduleIndex)
{
  NMP_ASSERT(moduleIndex < m_numModules);
#if ENABLE_ER_MODULE_LOGGING
  EUPHORIA_LOG_MESSAGE("Enabling module %s and children\n", getClassName());
#endif

  enable(moduleIndex);

  Module* md = m_allModules[moduleIndex];

  int32_t number = md->getNumChildren();
  for (int32_t i = 0; i < number; i++)
  {
    enableModuleAndChildren(md->getChild(i)->getManifestIndex());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RootModule::disable(uint32_t moduleIndex)
{
  NMP_ASSERT(moduleIndex < m_numModules);
#if ENABLE_ER_MODULE_LOGGING
  EUPHORIA_LOG_MESSAGE("Disabling module %s (count %d)\n", getClassName(), m_moduleEnableStates[moduleIndex].m_enabled);
#endif
  ModuleEnableState& mes = m_moduleEnableStates[moduleIndex];

  NMP_ASSERT(mes.m_enabled > 0);
  mes.m_enabled --;
}

//----------------------------------------------------------------------------------------------------------------------
void RootModule::disableOwners(uint32_t moduleIndex)
{
  NMP_ASSERT(moduleIndex < m_numModules);
#if ENABLE_ER_MODULE_LOGGING
  EUPHORIA_LOG_MESSAGE("Disabling module %s owners\n", getClassName());
#endif
  Module* owner = (Module*)m_allModules[moduleIndex]->getOwner();
  if (owner)
  {
    disable(owner->getManifestIndex());
    disableOwners(owner->getManifestIndex());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RootModule::disableModuleAndChildren(uint32_t moduleIndex)
{
  NMP_ASSERT(moduleIndex < m_numModules);
#if ENABLE_ER_MODULE_LOGGING
  EUPHORIA_LOG_MESSAGE("Disabling module %s and children\n", getClassName());
#endif
  Module* md = m_allModules[moduleIndex];

  int32_t number = md->getNumChildren();
  for (int32_t i = 0; i < number; i++)
  {
    disableModuleAndChildren(md->getChild(i)->getManifestIndex());
  }

  disable(moduleIndex);
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
void RootModule::getDebugEnabledModuleNames(char* names, int nameLength, int& numModules) const
{
  for (uint32_t i = 0; i < m_numModules; i++)
  {
    const char* modClass = m_allModules[i]->getClassName();

    ModuleEnableState& mes = m_moduleEnableStates[i];

    if (mes.m_enabled)
    {
      // horrendous O(n-sq) D:
      bool alreadyEncountered = false;
      for (int c = 0; c < numModules; c++)
      {
        if (!strcmp(modClass, &names[nameLength*c])) // already considered this module type
        {
          alreadyEncountered = true;
          break;
        }
      }

      if (!alreadyEncountered)
      {
        // cache the name of the module and its status (debugEnable'd or not)
        NMP_STRNCPY_S(&names[nameLength*(numModules)], nameLength, modClass);
        numModules++;
      }
    }
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void RootModule::getEnabledModules(Module** modules, int& numModules)
{
  for (uint32_t i = 0; i < m_numModules; i++)
  {
    if (isEnabled(i))
    {
      modules[numModules] = m_allModules[i];
      numModules++;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RootModule::setDebugInterface(MR::InstanceDebugInterface* debugInterface)
{
  m_debugInterface = debugInterface;
}

}
