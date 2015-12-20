// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

// ---------------------------------------------------------------------------------------------------------------------

#include "scriptedRuntimeApp.h"
#include "srEnvironment.h"

// ---------------------------------------------------------------------------------------------------------------------
Environment::Environment(int argc, const char *argv[])
{
  // invalid defaults
  m_scriptDir[0] = 0;
  m_networksDir[0] = 0;
  m_scenarioName[0] = 0;
  m_defaultNetwork[0] = 0;
  m_simulationRate = 30;
  m_logOutputEnabled = true;

  // Extract the command line arguments
  for (int i = 0; i < argc; i++)
  {
    if (_stricmp(argv[i], "--scenario") == 0)
    {
      i++;
      NMP_STRNCPY_S(m_scenarioName, eBufLength, argv[i]);
    }
    else if (_stricmp(argv[i], "--simulationRate") == 0)
    {
      i++;
      m_simulationRate = atoi(argv[i]);
    }
    else if (_stricmp(argv[i], "--enableLogOutput") == 0)
    {
      i++;
      m_logOutputEnabled = strcmp(argv[i], "true") == 0 ? true : false;
    }
    else if (_stricmp(argv[i], "--scriptDir") == 0)
    {
      i++;
      NMP_STRNCPY_S(m_scriptDir, eBufLength, argv[i]);
    }
    else if (_stricmp(argv[i], "--defaultNetwork") == 0)
    {
      i++;
      NMP_STRNCPY_S(m_defaultNetwork, eBufLength, argv[i]);
    }
    else if (_stricmp(argv[i], "--networkDir") == 0)
    {
      i++;
      NMP_STRNCPY_S(m_networksDir, eBufLength, argv[i]);
    }
  }
}

// ---------------------------------------------------------------------------------------------------------------------
Environment::~Environment()
{
}

// ---------------------------------------------------------------------------------------------------------------------
void Environment::registerForScripting(sqp::SquirrelVM& vm)
{
  sqp::ClassDefNoConstructor<Environment>(vm, "env")
    .SingletonFunc(this, &Environment::getScenarioName, "getScenarioName")
    .SingletonFunc(this, &Environment::getInstForScript, "getInst")
    ;
}
