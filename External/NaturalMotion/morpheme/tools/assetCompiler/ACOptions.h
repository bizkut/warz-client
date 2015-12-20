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
#ifdef _MSC_VER
#pragma once
#endif
#ifndef AC_OPTIONS_H
#define AC_OPTIONS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMBasicLogger.h"
#include "NMPlatform/NMCommandLineProcessor.h"
#include "PluginManager.h"
//----------------------------------------------------------------------------------------------------------------------

#define MAX_NUM_PLUGINS (32)

class ACOptions
{
public:
  /// \brief Find root working directory, set of assets to process etc. from command line.
  ACOptions(int argc, const char* const* argv);
  ACOptions() {};
  ~ACOptions();

  /// \brief Initialize the class after empty constructor call.
  void init(int argc, const char* const* argv);

  /// \brief removes any quotes around the relPath argument and creates an absolute path. The path can be to a file or directory
  /// If the input path is relative, then it is made absolute by using the input executablePath
  bool validate() const;

  bool getInfo() { return m_info; }
  NMP::BasicLogger* getInfoLogger() { return m_infoLogger; }

  NMP::BasicLogger* getMessageLogger() { return m_messageLogger; }
  NMP::BasicLogger* getErrorLogger() { return m_errorLogger; }
  uint32_t getSuccessCode() const { return m_successCode; }
  uint32_t getFailureCode() const { return m_failureCode; }
  const char* getOutputDir() const { return m_outputDir; }
  const char* getBaseDir() const { return m_baseDir; }
  const char* getCacheDir() const { return m_cacheDir; }
  const char* getOutputFilename() const { return m_outputFilename; }
  const char* getAnimInfoToUpdate() const { return m_animInfoToUpdate; }
  const char* getPrecomputeNode() const { return m_precomputeNode; }
  const char* getPrecomputeFile() const { return m_precomputeFile; }

  bool getDoCleanBuild() const { return m_doCleanBuild; }
  bool getDoPreviewAnimation() const { return m_doPreviewAnimation; }
  float getRuntimeAssetScaleFactor() const { return m_runtimeAssetScaleFactor; }
  uint32_t getMaxAnimSubSectionSize() const { return m_maxAnimSubSectionSize; }
  const NMP::CommandLineProcessor::StringList *getAssetList() const { return m_assetList; }

  const char* getPluginDir() const { return m_pluginDir; }
  uint32_t getPluginListCount() const { return m_pluginNameListCount; }
  const char* const* getPluginList() const { return m_pluginNameList; }

protected:
  void initPluginNameList();

  NMP::CommandLineProcessor *m_proc;
  /// Handles asset compiler and asset processor build process information messages.
  NMP::BasicLogger* m_messageLogger;
  /// Handles asset compiler and asset processor build process error messages.
  NMP::BasicLogger* m_errorLogger;
  uint32_t m_successCode;
  uint32_t m_failureCode;

  /// If the "-info outputFile" option is provided then the asset compiler just writes info about itself
  ///     e.g. platformFormat=BE32
  /// into the outputFile and then terminates
  bool m_info;
  /// Logger for writing asset compiler info
  NMP::BasicLogger* m_infoLogger;

  const char *m_baseDir;
  const char *m_outputDir;
  const char *m_cacheDir;
  const char *m_outputFilename;
  const char *m_animInfoToUpdate;
  const char *m_precomputeNode;
  const char *m_precomputeFile;

  bool m_doCleanBuild;
  bool m_doPreviewAnimation;
  float m_runtimeAssetScaleFactor;
  uint32_t m_maxAnimSubSectionSize;
  const NMP::CommandLineProcessor::StringList *m_assetList;

  char m_pluginDir[MAX_PATH];
  uint32_t m_pluginNameListCount;
  const char* m_pluginNameList[MAX_NUM_PLUGINS];
};

//----------------------------------------------------------------------------------------------------------------------
#endif // AC_OPTIONS_H
//----------------------------------------------------------------------------------------------------------------------
