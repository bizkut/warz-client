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
#include <sys/stat.h>
#include <fstream>
#include <shlwapi.h>
#include "ACOptions.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
bool parseConfigForPlugins(std::string& pluginListFromConfig, const char* pluginPath)
{
  // Search for the config file
  struct stat pluginFileInfo;
  int result = stat(pluginPath, &pluginFileInfo);
  if (result != 0 || ((pluginFileInfo.st_mode & S_IFREG) == 0))
  {
    return false;
  }

  std::ifstream configFile(pluginPath);
  if (configFile.is_open())
  {
    std::string plugin;
    while (configFile.peek() != std::ifstream::traits_type::eof())
    {
      configFile >> plugin;
      pluginListFromConfig += plugin; 
      if (configFile.peek() != std::ifstream::traits_type::eof())
      {
        pluginListFromConfig += ",";
      }
    }
    pluginListFromConfig += '\0';
    configFile.close();
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
ACOptions::ACOptions(int argc, const char* const* argv)
{
  init(argc, argv);
}

//----------------------------------------------------------------------------------------------------------------------
void ACOptions::init(int argc, const char* const* argv)
{
  m_proc = new NMP::CommandLineProcessor();

  // Setup default values
  m_info = false;
  m_infoLogger = 0;

  m_successCode = 0;
  m_failureCode = 1;
  m_messageLogger = NULL;
  m_errorLogger = NULL;

  m_baseDir = NULL;
  m_outputDir = NULL;
  m_cacheDir = NULL;
  m_outputFilename = NULL;

  m_animInfoToUpdate = NULL;
  m_doCleanBuild = false;
  m_doPreviewAnimation = false;
  m_runtimeAssetScaleFactor = 1.0f;
  m_maxAnimSubSectionSize = 0;

  m_assetList = NULL;
  m_pluginNameListCount = 0;

  m_proc->registerStringOption("-info");

  m_proc->registerStringOption("-logFile");                 // Log file to write to.  If none given, information is written to stdout.
  m_proc->registerStringOption("-errFile");
  m_proc->registerUInt32Option("-successCode");             // The value returned when we have completed successfully - default = 0.
  m_proc->registerUInt32Option("-failureCode");             // The value returned when we have not completed successfully - default = 1.

  m_proc->registerStringOption("-baseDir");                 // Where to load assets from.
  m_proc->registerStringOption("-cacheDir");                // Where cached files and information are stored (mostly animation files).
  m_proc->registerStringOption("-outputDir");               // Where to store the runtime binary output files.
  m_proc->registerStringOption("-outputFileName");          // Name of a bundled output file which the processed assets will be written to.
                                                            //  If not specified the name of the first input asset will be used.
  m_proc->registerStringOption("-precomputeNode");
  m_proc->registerStringOption("-precomputeFile");

  m_proc->registerStringListOption("-asset");               // Name of source asset to compile.
  m_proc->registerFlagOption("-clean");                     // True or false. Destroy any existing compiled asset files relating to
                                                            //  the indicated command line assets.
  m_proc->registerFlagOption("-previewAnimation");          // True or false. Do we need to copy compiled animation data to the anim browser file.
  m_proc->registerStringOption("-updateAnimInfo");          // Specifies a target directory or file that should be checked for animation updates.
  m_proc->registerFloatOption("-runtimeAssetScaleFactor");  // The factor required to scale morpheme internal units to meters.
  m_proc->registerUInt32Option("-maxAnimSubSectionSize");
  
  m_proc->registerStringListOption("-plugin");
  m_proc->registerStringOption("-pluginDir");               // The directory a plugin is loaded from.

  bool goodCmdLine = m_proc->parseCommandLine(argc, argv);
  
  // Set up the logging
  {
    const char *logFileName;
    if (m_proc->getOptionValue("-logFile", &logFileName))
      m_messageLogger = new NMP::FileLogger(logFileName);
    else
      m_messageLogger = new NMP::BasicLogger();

    if (m_proc->getOptionValue("-errFile", &logFileName))
      m_errorLogger = new NMP::FileLogger(logFileName);
    else
      m_errorLogger = new NMP::BasicLogger();
  }

  // Check command line after error log set up
  if (!goodCmdLine)
  {
    // Report the error.
    NMP_VERIFY_FAIL(m_proc->getParseErrorString());
  }

  const char* mInfo;
  if (m_proc->getOptionValue("-info", &mInfo))
  {
    m_info = true;
    m_infoLogger = new NMP::FileLogger(mInfo);
    return;
  }

#ifdef NM_DEBUG
  // Report the parsed command line params
  {
    char report[2048];
    m_proc->reportParsedOptions(report, 2048);
    m_messageLogger->output(report);
  }
#endif

  // If the option isn't passed on the command line, the current value will be used.
  m_proc->getOptionValue("-successCode", &m_successCode);
  m_proc->getOptionValue("-failureCode", &m_failureCode);

  m_proc->getOptionValue("-baseDir", &m_baseDir);
  m_proc->getOptionValue("-outputDir", &m_outputDir);
  m_proc->getOptionValue("-cacheDir", &m_cacheDir);
  m_proc->getOptionValue("-outputFileName", &m_outputFilename);
  m_proc->getOptionValue("-updateAnimInfo", &m_animInfoToUpdate);

  m_proc->getOptionValue("-asset", &m_assetList);
  m_proc->getOptionValue("-clean", &m_doCleanBuild);
  m_proc->getOptionValue("-previewAnimation", &m_doPreviewAnimation);
  m_proc->getOptionValue("-runtimeAssetScaleFactor", &m_runtimeAssetScaleFactor);
  m_proc->getOptionValue("-maxAnimSubSectionSize", &m_maxAnimSubSectionSize);

  initPluginNameList();

  // Process the plugin directory args. This only needs to be done if there is a plugin list.
  if (m_pluginNameListCount > 0)
  {
    const char* pluginDir = "plugins\\assetCompiler";
    m_proc->getOptionValue("-pluginDir", &pluginDir);
    // The input path may contain quotes so that a path with spaces can be used, these need to be removed.
    NMP::NMFile::removeQuotesFromPath(pluginDir, MAX_PATH, m_pluginDir, MAX_PATH);

    // construct the path to the plugins

    // Check if the directory exists.
    struct stat directoryInfo;
    int result = stat(m_pluginDir, &directoryInfo);
    if (result != 0 || ((directoryInfo.st_mode & (S_IFDIR | S_IFMT)) == 0))
    {
      // If the directory didn't exist it may have been a relative path to the asset compiler exe.
      // Use that path if it exists.
      char buffer[MAX_PATH];
      const char* exePath = m_proc->getExecutablePath();
      NMP_ASSERT(exePath);
      NMP_STRNCPY_S(buffer, MAX_PATH, exePath);
      NMP_STRNCAT_S(buffer, MAX_PATH, m_pluginDir);

      result = stat(buffer, &directoryInfo);
      if (result != 0)
      {
        NMP_VERIFY_FAIL("Failed to find plugin path \"%s\".", buffer);
        m_pluginDir[0] = '\0';
      }
      else if ((directoryInfo.st_mode & (S_IFDIR | S_IFMT)) == 0)
      {
        NMP_VERIFY_FAIL("Plugin path \"%s\" is not a file or directory.", buffer);
        m_pluginDir[0] = '\0';
      }
      else
      {
        NMP_STRNCPY_S(m_pluginDir, MAX_PATH, buffer);
      }
    }

  }

  validate();
}

//----------------------------------------------------------------------------------------------------------------------
ACOptions::~ACOptions()
{
  const NMP::CommandLineProcessor::StringList *temp;
  if (!m_proc->getOptionValue("-plugin", &temp))
  {
    // We don't have a stringList in the command line parser, so we must be responsible for the memory allocated for m_pluginNameList
    for (uint32_t i = 0; i<m_pluginNameListCount; i++)
    {
      free((void*)m_pluginNameList[i]);
    }
  }

  delete m_infoLogger;
  delete m_messageLogger;
  delete m_errorLogger;
  delete m_proc;
}

//----------------------------------------------------------------------------------------------------------------------
bool ACOptions::validate() const
{
  // Validation of command line args
  if (!getAnimInfoToUpdate() && !getDoCleanBuild())
  {
    // These parameters are required if not just updating anim info
    NMP_VERIFY_MSG(getBaseDir(), "Missing command line argument: %s", "-baseDir");
    NMP_VERIFY_MSG(getOutputDir(), "Missing command line argument: %s", "-outputDir");
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ACOptions::initPluginNameList()
{
  // Check to see if we have plugins passed in with '-plugin' arguments
  const NMP::CommandLineProcessor::StringList *strList;
  if (m_proc->getOptionValue("-plugin", &strList))
  {
    NMP_VERIFY(strList->size() <= MAX_NUM_PLUGINS);

    m_pluginNameListCount = 0;
    NMP::CommandLineProcessor::StringList::iterator it = strList->begin();
    for (; it != strList->end(); ++it)
    {
      m_pluginNameList[m_pluginNameListCount] = *it;
      ++m_pluginNameListCount;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
