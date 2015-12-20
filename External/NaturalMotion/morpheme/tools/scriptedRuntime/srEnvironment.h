#pragma once
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

namespace sqp
{
  class SquirrelVM;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class Environment
/// \brief This class holds settings from the command line arguments of the app and exposes them for use in Squirrel.
class Environment
{
public:
  Environment(int argc, const char *argv[]);
  ~Environment();

  void registerForScripting(sqp::SquirrelVM& vm);

  const char* getScenarioName() const { return m_scenarioName; }
  int getSimulationRate() const { return m_simulationRate; }
  bool isLogOutputEnabled() const { return m_logOutputEnabled; }

  const char* getScriptDir() const { return m_scriptDir; }
  const char* getNetworksDir() const { return m_networksDir; }

  const char* getDefaultNetwork() const { return m_defaultNetwork; }

protected:

  Environment* getInstForScript() { return this; }

  enum { eBufLength = 260 };

  char      m_scenarioName[eBufLength];
  char      m_scriptDir[eBufLength];
  char      m_networksDir[eBufLength];
  char      m_defaultNetwork[eBufLength];
  int       m_simulationRate;
  bool      m_logOutputEnabled;
};

DECLARE_INSTANCE_TYPE_NAME(Environment, env);
