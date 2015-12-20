// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
//----------------------------------------------------------------------------------------------------------------------
#include "PluginManager.h"
#include <sys/stat.h>

namespace AP
{

/// \brief called once when a plug-in is loaded in the manager.  Used to initialise memory and register morpheme types.
/// Not called if memory logging is defined.
typedef void (*LoadPluginFn)(MR::Manager* manager, const NMP::Memory::Config* config);

/// \brief called once when a plug-in is loaded in the manager.  Used to initialise memory and register morpheme types.
/// Only called if memory logging is defined.
typedef void (*LoadPluginWithLoggingFn)(MR::Manager* manager, const NMP::Memory::Config* config NMP_MEMORY_LOGGING_DECL);

/// \brief called once after all plug-ins have been loaded but before morpheme initialisation is complete.  Used to
/// finalise semantic IDs.
typedef void (*FinaliseInitialisePluginFn)();

/// \brief may be called multiple times per run. Asset builders in the plug-in are registered with the an asset processor.
/// RegisterPluginWithProcessor will not be called subsequent times without a call to DestroyPluginData first.  
typedef void (*RegisterPluginWithProcessorFn)(
  AP::AssetProcessor*        assetProc,
  MR::Manager*               manager,
  uint32_t                   numDispatchers,
  MR::Dispatcher**           dispatchers);

/// \brief may be called multiple times per run.  Used to destroy any data created in RegisterPluginWithProcessor.
typedef void (*DestroyPluginDataFn)();

/// \brief called once before a plug-in is unloaded.  Any memory handlers initialised in the plugin should be destroyed
/// at this point
typedef uint32_t (*UnloadPluginFn)();

//----------------------------------------------------------------------------------------------------------------------
// PluginInstance
//----------------------------------------------------------------------------------------------------------------------
class PluginInstance
{
public:
  PluginInstance();
  ~PluginInstance();

  bool load(const char* pluginPath, MR::Manager& manager, const NMP::Memory::Config* c NMP_MEMORY_LOGGING_DECL);

  void registerPluginWithProcessor(
    AP::AssetProcessor* assetProcessor,
    MR::Manager*        manager,
    uint32_t            numDispatchers,
    MR::Dispatcher**    dispatchers);

  void finaliseInitialisation();

  void destroyPluginData();

  void unload();

  const char* getName() const;

private:
  void cleanMembers();

  HINSTANCE m_dllHandle;

#ifndef NMP_MEMORY_LOGGING
  LoadPluginFn m_loadFunc;
#else
  LoadPluginWithLoggingFn m_loadFunc;
#endif
  RegisterPluginWithProcessorFn m_registerFunc;
  FinaliseInitialisePluginFn m_finaliseFunc;
  DestroyPluginDataFn m_destroyFunc;
  UnloadPluginFn m_unloadFunc;
  std::string m_name;

  bool m_initialised;
};

//----------------------------------------------------------------------------------------------------------------------
PluginInstance::PluginInstance()
{
  cleanMembers();
}

//----------------------------------------------------------------------------------------------------------------------
PluginInstance::~PluginInstance()
{
  NMP_ASSERT_MSG(m_dllHandle == NULL, "Plugin not unloaded yet.");
}

//----------------------------------------------------------------------------------------------------------------------
bool PluginInstance::load(const char* pluginPath, MR::Manager& manager, const NMP::Memory::Config* config NMP_MEMORY_LOGGING_DECL)
{
  // Make sure the plug-in file exists
  struct stat pluginFileInfo;
  int result = stat(pluginPath, &pluginFileInfo);
  if (result != 0 || ((pluginFileInfo.st_mode & S_IFREG) == 0))
  {
    NMP_VERIFY_FAIL("Failed to load plugin %s.  File not found.", pluginPath);
    return false;
  }

  // Load the plug-in and retrieve the interface functions
  m_dllHandle = LoadLibraryA(pluginPath);

  if (!m_dllHandle)
  {
    NMP_VERIFY_FAIL("Failed to load plug-in library %s.", pluginPath);
  }

#ifdef NMP_MEMORY_LOGGING
  m_loadFunc = (LoadPluginWithLoggingFn)GetProcAddress(m_dllHandle, "loadPluginWithLogging");
  NMP_VERIFY_MSG(m_loadFunc, "Plugin load fail! Couldn't find \"loadPluginWithLogging\" function in %s.", pluginPath);
#else
  m_loadFunc = (LoadPluginFn)GetProcAddress(m_dllHandle, "loadPlugin");
  NMP_VERIFY_MSG(m_loadFunc, "Plugin load fail! Couldn't find \"loadPlugin\" function in %s.", pluginPath);
#endif

  m_finaliseFunc = (FinaliseInitialisePluginFn)GetProcAddress(m_dllHandle, "finalisePluginInitialisation");
  m_registerFunc = (RegisterPluginWithProcessorFn)GetProcAddress(m_dllHandle, "registerPluginWithProcessor");
  m_destroyFunc = (DestroyPluginDataFn)GetProcAddress(m_dllHandle, "destroyPluginData");
  m_unloadFunc = (UnloadPluginFn)GetProcAddress(m_dllHandle, "unloadPlugin");

  NMP_VERIFY_MSG(m_finaliseFunc, "Plugin load fail! Couldn't find \"finaliseInitialisePlugin\" function in %s.", pluginPath);
  NMP_VERIFY_MSG(m_registerFunc, "Plugin load fail! Couldn't find \"registerPluginWithProcessor\" function in %s.", pluginPath);
  NMP_VERIFY_MSG(m_destroyFunc, "Plugin load fail! Couldn't find \"destroyPluginData\" function in %s.", pluginPath);
  NMP_VERIFY_MSG(m_unloadFunc, "Plugin load fail! Couldn't find \"unloadPlugin\" function in %s.", pluginPath);

  if (!m_loadFunc || !m_finaliseFunc || !m_registerFunc || !m_destroyFunc || !m_unloadFunc)
  {
    // Can't continue with a plug-in that doesn't contain the full interface
    return false;
  }

  // Extract the starting character position of the dll name from the path
  m_name = pluginPath;
  size_t lastForwardSlash = m_name.find_last_of('/');
  lastForwardSlash = lastForwardSlash == std::string::npos ? 0 : lastForwardSlash;
  size_t lastBackSlash = m_name.find_last_of('\\');
  lastBackSlash = lastBackSlash == std::string::npos ? 0 : lastBackSlash;
  size_t lastSlash = NMP::maximum(lastForwardSlash, lastBackSlash);

  // Extract the extension from the path
  size_t lastDot = m_name.find_last_of('.');
  if (lastDot == std::string::npos)
  {
    lastDot = m_name.size();
  }

  // The plugin name is the path with the directory and extension trimmed off.
  m_name = m_name.substr(lastSlash + 1, lastDot - lastSlash - 1);

#ifdef NMP_MEMORY_LOGGING
  m_loadFunc(&manager, config NMP_MEMORY_LOGGING_PASS_THROUGH);
#else
  m_loadFunc(&manager, config);
#endif

  NET_LOG_MESSAGE(109, "Loaded plugin %s", m_name);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void PluginInstance::registerPluginWithProcessor(
  AP::AssetProcessor* assetProcessor,
  MR::Manager* manager,
  uint32_t numDispatchers,
  MR::Dispatcher** dispatchers)
{
  NMP_ASSERT(!m_initialised);

  m_registerFunc(assetProcessor, manager, numDispatchers, dispatchers);
  m_initialised = true;
}

//----------------------------------------------------------------------------------------------------------------------
void PluginInstance::finaliseInitialisation()
{
  m_finaliseFunc();
}

//----------------------------------------------------------------------------------------------------------------------
void PluginInstance::destroyPluginData()
{
  if (m_initialised)
  {
    m_destroyFunc();
    m_initialised = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PluginInstance::unload()
{
  if (m_initialised)
  {
    destroyPluginData();
  }

  if (m_unloadFunc)
  {
    m_unloadFunc();
  }

  if (m_dllHandle)
  {
    FreeLibrary(m_dllHandle);
  }

  cleanMembers();
}

//----------------------------------------------------------------------------------------------------------------------
const char* PluginInstance::getName() const
{
  return m_name.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
void PluginInstance::cleanMembers()
{
  m_dllHandle = NULL;
  m_registerFunc = NULL;
  m_finaliseFunc = NULL;
  m_destroyFunc = NULL;
  m_loadFunc = NULL;

  m_initialised = false;
}

//----------------------------------------------------------------------------------------------------------------------
// PluginManager
//----------------------------------------------------------------------------------------------------------------------
PluginManager::~PluginManager()
{
  destroyPluginData();
  unloadAll();
}

//----------------------------------------------------------------------------------------------------------------------
bool PluginManager::loadPluginsFromList(
  const char* directory,
  const char* const* pluginList,
  uint32_t pluginListLength,
  const NMP::Memory::Config* config
  NMP_MEMORY_LOGGING_DECL)
{
  if (!m_plugins.empty())
  {
    NMP_VERIFY_FAIL("Plugin manager only supports loading from a plugin list once.");
    return false;
  }

  const char* targetString = NM_PLATFORM_FORMAT_STRING;

  // Initialise the runtime library (we need this to generate assets)
  MR::Manager::initMorphemeLib();

  MR::Manager& manager = MR::Manager::getInstance();

  for (uint32_t i = 0; i < pluginListLength; i++)
  {
    char buffer[1024];
#ifdef NM_DEBUG
    char postfix[] = "_debug";
#else
    char postfix[] = "";
#endif

    NMP_SPRINTF(buffer, 1024, "%s\\%s_target_%s%s.dll", directory, pluginList[i], targetString, postfix);

    // Load the plug-in and register types
    PluginInstance* plugI = new PluginInstance();
    m_plugins.push_back(plugI);
    if (!plugI->load(buffer, manager, config NMP_MEMORY_LOGGING_PASS_THROUGH))
    {
      return false;
    }
  }

  MR::Manager::getInstance().allocateRegistry();

  MR::Manager::finaliseInitMorphemeLib();

  const uint32_t pluginCount = (uint32_t)m_plugins.size();
  for (uint32_t i = 0; i < pluginCount; ++i)
  {
    m_plugins[i]->finaliseInitialisation();
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void PluginManager::registerPluginsWithProcessor(
  AP::AssetProcessor* assetProcessor,
  uint32_t            numDispatchers,
  MR::Dispatcher**    dispatchers)
{
  NMP_ASSERT(assetProcessor);
  
  MR::Manager* manager = &MR::Manager::getInstance();

  initPluginCore(assetProcessor, manager, numDispatchers, dispatchers);

  const uint32_t pluginCount = (uint32_t)m_plugins.size();

  for (uint32_t i = 0; i < pluginCount; ++i)
  {
    m_plugins[i]->registerPluginWithProcessor(assetProcessor, manager, numDispatchers, dispatchers);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PluginManager::destroyPluginData()
{
  for (uint32_t i = 0, n = (uint32_t)m_plugins.size(); i < n; ++i)
  {
    m_plugins[i]->destroyPluginData();
  }

  deinitialisePluginCore();
}

//----------------------------------------------------------------------------------------------------------------------
void PluginManager::unloadAll()
{
  for (uint32_t i = 0, n = (uint32_t)m_plugins.size(); i < n; ++i)
  {
    m_plugins[i]->unload();
  }
  m_plugins.clear();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PluginManager::getPluginCount() const
{
  return (uint32_t)m_plugins.size();
}

//----------------------------------------------------------------------------------------------------------------------
const char* PluginManager::getPluginName(uint32_t i) const
{
  NMP_ASSERT(i < (uint32_t)m_plugins.size());

  return m_plugins[i]->getName();
}

//----------------------------------------------------------------------------------------------------------------------
MR::Manager* PluginManager::getManager()
{
  return &MR::Manager::getInstance();
}

//----------------------------------------------------------------------------------------------------------------------
}// namespace AP
