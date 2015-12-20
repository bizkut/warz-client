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
#ifndef AP_PLUGINMANAGER_H
#define AP_PLUGINMANAGER_H
//-----------------------------------//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVectorContainer.h"
#include "assetProcessor/AssetProcessor.h"
#include "acCore.h"

//----------------------------------------------------------------------------------------------------------------------
namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Asset compiler plug-in interface
//
// Each asset compiler plug-in must contain the following interface.  If an interface function is not found, the plug-in
// will fail to load.  An exception to this is loadPlugin and loadPluginWithLogging. loadPlugin is used if 
// NMP_MEMORY_LOGGING is not defined, LoadPluginWithLogging is used if it is defined.  Only one will be retrieved when
// the plug-in is loaded.  All interface function are case sensitive.  
//
// loadPlugin(MR::Manager* manager, const NMP::Memory::Config* config);
// loadPluginWithLogging(MR::Manager* manager, const NMP::Memory::Config* config NMP_MEMORY_LOGGING_DECL);
// finaliseInitialisePlugin(AP::AssetProcessor* assetProc, MR::Manager* manager, uint32_t numDispatchers, MR::Dispatcher** dispatchers);
// registerPluginWithProcessor();
// destroyPluginData();
// unloadPlugin();

#if defined(_MSC_VER)
// disable warning C4251: 'AP::PluginManager::m_plugins' : class 'std::vector<_Ty>' needs to have dll-interface to be
// used by clients of class 'AP::PluginManager'
# pragma warning(push)
# pragma warning( disable: 4251 )
#endif

//----------------------------------------------------------------------------------------------------------------------
class PluginInstance;

class AC_CORE_EXPORT PluginManager
{
public:

  ~PluginManager();

  /// \brief load a comma separated list of plug-ins from the directory provided.  This will initialise the core 
  /// morpheme library and allocate the registry.
  bool loadPluginsFromList(
    const char* directory,
    const char* const* pluginList,
    uint32_t pluginListLength,
    const NMP::Memory::Config* config
    NMP_MEMORY_LOGGING_DECL);

  /// \brief initialise all plug-ins held by the manager
  void registerPluginsWithProcessor(
    AP::AssetProcessor* assetProcessor,
    uint32_t numDispatchers,
    MR::Dispatcher** dispatchers);

  /// \brief de-initialise all plugins held by the manager
  void destroyPluginData();

  void unloadAll();

  uint32_t getPluginCount() const;
  const char* getPluginName(uint32_t i) const;

  MR::Manager* getManager();

private:
  std::vector<PluginInstance*> m_plugins;
};

#if defined(_MSC_VER)
# pragma warning(pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_PLUGINMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
