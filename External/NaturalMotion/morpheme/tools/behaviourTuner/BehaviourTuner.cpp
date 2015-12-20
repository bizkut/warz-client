// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS

#include "Configuration.h"
#include "GA.h"

#include <windows.h>
#include <ctime>
#include <io.h>

//----------------------------------------------------------------------------------------------------------------------
typedef std::vector<PROCESS_INFORMATION> Processes;
typedef std::vector<DWORD> ExitCodes;
typedef std::vector<const float*> Genomes;

//----------------------------------------------------------------------------------------------------------------------
const char* scriptPre = 
  "print(\"Opening project \" .. projectFile .. \"...\")\n"
  "project.open(projectFile)\n"
  "print(\"Loading \" .. mcnFile .. \"...\")\n"
  "assert(mcn.open(mcnFile))\n"
  "\n"
  "print(\"Modifying parameters...\")\n";

//----------------------------------------------------------------------------------------------------------------------
const char* scriptModify = 
  "local animSets = listAnimSets()\n"
  "for nodeType, nodeTypeAttributes in pairs(tuneableAttributes) do\n"
  "  local nodes = ls(nodeType)\n"
  "  for _, node in pairs(nodes) do\n"
  "    for _, attr in pairs(nodeTypeAttributes) do\n"
  "      if attr.name then\n"
  "        local value = attr.value\n"
  "        for _, set in pairs(animSets) do\n"
  "          local attrPath = string.format(\"%s.%s\", node, attr.name)\n"
  "          local _, info = getAttributeType(attrPath)\n"
  "          if (info.isSet) then\n"
  "            if hasAnimSetData(attrPath, set) then\n"
  "              setAttribute(attrPath, value, set)\n"
  "            end\n"
  "          else\n"
  "            setAttribute(attrPath, value)\n"
  "          end\n"
  "        end\n"
  "      end\n"
  "    end\n"
  "  end\n"
  "end\n"
  "\n"
  "for _, attr in pairs(tuneableCPs) do\n"
  "  local value = attr.value\n"
  "  local attrPath = string.format(\"ControlParameters|%s\", attr.name)\n"
  "  setDefaultValue(attrPath, value)\n"
  "end\n";

//----------------------------------------------------------------------------------------------------------------------
const char* scriptPost = 
  "\n"
  "print(\"Exporting to \" .. exportFile .. \"...\")\n"
  "local result = mcn.export(exportFile)\n"
  "assert(result)\n"
  "if (result) then\n"
  "  print(\"Done!\")\n"
  "else\n"
  "  print(\"There was an error\")\n"
  "end\n";

//----------------------------------------------------------------------------------------------------------------------
std::string getQuoted(const std::string& orig)
{
  return std::string("\"") + orig + "\"";
}

//----------------------------------------------------------------------------------------------------------------------
inline float getScaledValue(float min, float max, float f)
{
  return min + (max - min) * f;
}

//----------------------------------------------------------------------------------------------------------------------
inline float getNormalisedValue(float min, float max, float f)
{
  if (max > min)
    return (f - min) / (max - min);
  else
    return 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
class TunerGeneConverter : public GeneConverter
{
public:
  TunerGeneConverter(const Tuneables& tuneables) : m_tuneables(&tuneables) {}

  virtual float ConvertToRealValue(float normalisedValue, int gene) const 
  {
    const TuneableValue* tuneableAttribute = m_tuneables->getTuneableValue(gene);
    assert(tuneableAttribute);
    return getScaledValue(tuneableAttribute->m_minValue, tuneableAttribute->m_maxValue, normalisedValue);
  }

  virtual float ConvertToNormalisedValue(float value, int gene) const 
  {
    const TuneableValue* tuneableAttribute = m_tuneables->getTuneableValue(gene);
    assert(tuneableAttribute);
    return getNormalisedValue(tuneableAttribute->m_minValue, tuneableAttribute->m_maxValue, value);
  }
private:
  const Tuneables* m_tuneables;
};


//----------------------------------------------------------------------------------------------------------------------
bool createProcess(const Configuration& configuration, 
                   const char* exe,
                   const char* args,
                   PROCESS_INFORMATION& process)
{
  STARTUPINFOA si;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &process, sizeof(process) );

  char cmd[1024];
  sprintf(cmd, "%s %s", exe, args);

  if (configuration.m_logLevel >= Configuration::LOG_VERBOSE)
    printf("Running %s\n", cmd);

  BOOL createResult = CreateProcessA(
    NULL,
    cmd,
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    FALSE,          // Set handle inheritance to FALSE
    0,              // No special flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory 
    &si,            // Pointer to STARTUPINFO structure
    &process );          // Pointer to PROCESS_INFORMATION structure

  if (!createResult)
  {
    process.dwProcessId = 0;
    DWORD error = GetLastError();
    fprintf(stderr, "Error creating process = %d\n", error);
    return false;
  }

  if (configuration.m_logLevel >= Configuration::LOG_VERBOSE)
    printf("Process = %p\n", process.hProcess);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool waitForProcesses(const Configuration& configuration, 
                      Processes& processes, 
                      ExitCodes& exitCodes, 
                      float timeoutSeconds,
                      char* txt)
{
  for (size_t i = 0 ; i != processes.size() ; ++i)
  {
    exitCodes[i] = 0;
    if (processes[i].dwProcessId == 0)
    {
      printf("%s Skipping waiting for process index %d as the process ID = 0\n", txt, i);
      continue;
    }
    // todo reduce this according to how long we've waited already
    int timeoutMS = (int) (timeoutSeconds * 1000);
    if (WAIT_TIMEOUT != WaitForSingleObject(processes[i].hProcess, timeoutMS))
    {
      DWORD exitCode;
      GetExitCodeProcess(processes[i].hProcess, &exitCode);
      exitCodes[i] = exitCode;
      if (configuration.m_logLevel >= Configuration::LOG_VERBOSE)
        printf("%s Process %p number %d exit code = %d\n", txt, processes[i].hProcess, i, exitCode);
    }
    else
    {
      TerminateProcess(processes[i].hProcess, 0);
      printf("%s Terminated processes %p number %d due to timeout\n", txt, processes[i].hProcess, i);
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool prepareWorkingDirectories(const Configuration& configuration)
{
  char tmpString[1024];

  for (int iProcess = 0 ; iProcess != configuration.m_maxNumProcesses ; ++iProcess)
  {
    _itoa_s(iProcess, tmpString, 10);
    std::string workingDir = configuration.m_workingDir + tmpString;
    std::string workingDirQ = getQuoted(workingDir);

    // Remove the working dir and its contents if it exists. Shouldn't be necessary, but it guarantees everything
    // is clean, and morpheme bugs like MORPH-20492 don't cause problems.
    std::string rmDirCmd("rd /s /q ");
    rmDirCmd += workingDirQ;
    system(rmDirCmd.c_str());

    // Create working dir
    CreateDirectoryA(workingDir.c_str(), NULL);

    // Copy the network to the working dir
    std::string workingNetwork = (workingDir + "\\network.mcn").c_str();
    if (!CopyFileA(configuration.m_network.c_str(), workingNetwork.c_str(), FALSE))
    {
      fprintf(stderr, "Failed to copy network file to %s\n", workingNetwork.c_str());
      return false;
    }

    // Delete the most important files if they exist. Don't care if these fail. However, it's
    // important they don't just lie around, otherwise the runtime exe may run with invalid data, or a failure
    // may not be caught.
    DeleteFileA((workingDir + "\\ExportNetwork.lua").c_str());
    DeleteFileA((workingDir + "\\network.xml").c_str());
    DeleteFileA((workingDir + "\\connectExportLog.txt").c_str());
    DeleteFileA((workingDir + "\\assetCompiler.txt").c_str());
    DeleteFileA((workingDir + "\\assetCompilerError.txt").c_str());
    DeleteFileA((workingDir + "\\runtimeBinary\\network.nmb").c_str());
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
static std::string replaceInString(const std::string& str, const std::string& oldStr, const std::string& newStr)
{
  std::string result(str);
  size_t pos = 0;
  while((pos = result.find(oldStr, pos)) != std::string::npos)
  {
    result.replace(pos, oldStr.length(), newStr);
    pos += newStr.length();
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void writeModificationScript(FILE* scriptFile, 
                             const Tuneables& tuneables,
                             const float* genome)
{
  size_t iGene = 0;

  // Attributes
  fprintf(scriptFile, "local tuneableAttributes =\n");
  fprintf(scriptFile, "{\n");

  for (size_t iNetworkModification = 0; iNetworkModification != tuneables.m_tuneableAttributes.size(); ++iNetworkModification)
  {
    const NodeTypeAttributes& nodeTypeAttributes = tuneables.m_tuneableAttributes[iNetworkModification];
    fprintf(scriptFile, "  [\"%s\"] =\n", nodeTypeAttributes.m_nodeType.c_str());
    fprintf(scriptFile, "  {\n");

    for (size_t iTuneable = 0 ; iTuneable != nodeTypeAttributes.m_tuneableAttributes.size() ; ++iTuneable)
    {
      const TuneableValue& tuneable = nodeTypeAttributes.m_tuneableAttributes[iTuneable];
      float gene = genome[iGene++];
      float newValue = getScaledValue(tuneable.m_minValue, tuneable.m_maxValue, gene);
      fprintf(scriptFile, "    {name = \"%s\", value = %f},\n", tuneable.m_name.c_str(), newValue);
    }
    fprintf(scriptFile, "  },\n");
  }
  fprintf(scriptFile, "}\n");

  // Control parameters
  fprintf(scriptFile, "local tuneableCPs =\n");
  fprintf(scriptFile, "{\n");
  for (size_t iTuneable = 0 ; iTuneable != tuneables.m_tuneableControlParamers.size() ; ++iTuneable)
  {
    const TuneableValue& tuneable = tuneables.m_tuneableControlParamers[iTuneable];
    float gene = genome[iGene++];
    float newValue = getScaledValue(tuneable.m_minValue, tuneable.m_maxValue, gene);
    fprintf(scriptFile, "  {name = \"%s\", value = %f},\n", tuneable.m_name.c_str(), newValue);
  }
  fprintf(scriptFile, "}\n");

  // Now the code
  fprintf(scriptFile, "%s", scriptModify);
}

//----------------------------------------------------------------------------------------------------------------------
bool createModificationScript(const char* scriptName, 
                              const Tuneables& tuneables,
                              const float* genome,
                              float fitness)
{
  FILE* scriptFile = fopen(scriptName, "w+");
  if (scriptFile)
  {
    fprintf(scriptFile, "-- Results in fitness = %f\n", fitness);
    writeModificationScript(scriptFile, tuneables, genome);
    fclose(scriptFile);
    return true;
  }
  else
  {
    fprintf(stderr, "Failed to open script file %s for writing\n", scriptName);
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool createExportScript(const Configuration& configuration, 
                        const Tuneables& tuneables,
                        const float* genome,
                        const std::string& workingDir)
{
  std::string scriptName = workingDir + "\\ExportNetwork.lua";
  std::string workingNetwork = workingDir + "\\network.mcn";
  std::string exportFile = workingDir + "\\network.xml";

  FILE* scriptFile = fopen(scriptName.c_str(), "w+");
  if (!scriptFile)
  {
    fprintf(stderr, "Failed to open script file for writing %s\n", scriptName.c_str());
    return false;
  }

  fprintf(scriptFile, "local mcnFile = \"%s\"\n", replaceInString(workingNetwork, "\\", "/").c_str());
  fprintf(scriptFile, "local exportFile = \"%s\"\n", replaceInString(exportFile, "\\", "/").c_str());
  fprintf(scriptFile, "local projectFile = \"%s\"\n", replaceInString(configuration.m_projectFile, "\\", "/").c_str());
  fprintf(scriptFile, "\n");

  fprintf(scriptFile, "%s", scriptPre);

  writeModificationScript(scriptFile, tuneables, genome);

  fprintf(scriptFile, "%s", scriptPost);

  fclose(scriptFile);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool modifyAndExportFromConnect(const Configuration& configuration, 
                                const Tuneables& tuneables,
                                const Genomes& genomes,
                                ExitCodes& exitCodes)
{
  Processes processes(configuration.m_maxNumProcesses);
  char tmpString[1024];

  for (size_t iProcess = 0 ; iProcess != processes.size() ; ++iProcess)
  {
    _itoa_s(iProcess, tmpString, 10);
    std::string workingDir = configuration.m_workingDir + tmpString;
    std::string script = workingDir + "\\ExportNetwork.lua";

    std::string workingDirQ = getQuoted(workingDir);
    std::string scriptQ = getQuoted(script);

    // Create the export script that will also modify the parameters
    if (!createExportScript(configuration, tuneables, genomes[iProcess], workingDir))
    {
      return false;
    }

    // Export and process the network to the working dir
    std::string args = 
      "-nogui -log " + workingDirQ + "\\connectExportLog.txt -script " + scriptQ;

    if (!createProcess(
      configuration,
      configuration.m_connectExe.c_str(),
      args.c_str(),
      processes[iProcess]))
    {
      fprintf(stderr, "Failed to run connect to export the network\n");
    }
  }

  // wait for all the exports
  waitForProcesses(configuration, processes, exitCodes, configuration.m_connectTimeout, "modifyAndExportFromConnect");

  // Check for the expected export file
  for (size_t iProcess = 0 ; iProcess != processes.size() ; ++iProcess)
  {
    _itoa_s(iProcess, tmpString, 10);
    std::string workingDir = configuration.m_workingDir + tmpString;

    std::string exportFile = workingDir + "\\network.xml";
    if (-1 == _access(exportFile.c_str(), 0))
    {
      fprintf(stderr, "Failed to find the exported file\n");
      exitCodes[iProcess] = 1;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool processExportedAssets(const Configuration& configuration, ExitCodes& exitCodes)
{
  Processes processes(configuration.m_maxNumProcesses);
  char tmpString[1024];

  // process the exported data
  for (size_t iProcess = 0 ; iProcess != processes.size() ; ++iProcess)
  {
    _itoa_s(iProcess, tmpString, 10);
    std::string workingDir = configuration.m_workingDir + tmpString;

    std::string assetQ = getQuoted(workingDir + "\\network.xml");
    std::string runtimeBinaryQ = getQuoted(workingDir + "\\runtimeBinary");
    std::string assetLogQ = getQuoted(workingDir + "\\assetCompiler.txt");
    std::string assetErrorQ = getQuoted(workingDir + "\\assetCompilerError.txt");

    std::string args = 
      " -baseDir " + getQuoted(configuration.m_assetCompilerBaseDir) +
      " -pluginDir " + getQuoted(configuration.m_assetCompilerPluginDir) + 
      " -optionsFile " + getQuoted(configuration.m_assetCompilerConfigFile) + 
      " -asset " + assetQ + 
      " -outputDir " + runtimeBinaryQ + 
      " -logFile " + assetLogQ + 
      " -errFile " + assetErrorQ;

    if (!createProcess(
      configuration,
      configuration.m_assetCompilerExe.c_str(),
      args.c_str(),
      processes[iProcess]))
    {
      fprintf(stderr, "Failed to run asset compiler\n");
    }
  }

  // wait for all the asset compilations
  waitForProcesses(configuration, processes, exitCodes, configuration.m_assetCompilerTimeout, "processExportedAssets");

  // Check for the expected binary asset
  for (size_t iProcess = 0 ; iProcess != processes.size() ; ++iProcess)
  {
    _itoa_s(iProcess, tmpString, 10);
    std::string workingDir = configuration.m_workingDir + tmpString;

    std::string binaryFile = workingDir + "\\runtimeBinary\\network.nmb";
    if (-1 == _access(binaryFile.c_str(), 0))
    {
      fprintf(stderr, "Failed to find the binary asset\n");
      exitCodes[iProcess] = 1;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool runRuntimeExe(const Configuration& configuration, ExitCodes& testResults)
{
  Processes processes(configuration.m_maxNumProcesses);
  char tmpString[1024];

  // process the exported data
  for (size_t iProcess = 0 ; iProcess != processes.size() ; ++iProcess)
  {
    _itoa_s(iProcess, tmpString, 10);
    std::string workingDir = configuration.m_workingDir + tmpString;
    std::string networkDir = workingDir + "\\runtimeBinary";

    std::string networkDirQ = getQuoted(networkDir);

    std::string args = configuration.m_RTArgs + 
      " --defaultNetwork network --scriptDir .\\ --networkDir " + networkDirQ + " --enableLogOutput " + 
      (configuration.m_logLevel == Configuration::LOG_MINIMAL ? "false " : "true ");

    if (!createProcess(
      configuration,
      configuration.m_RTExe.c_str(),
      args.c_str(),
      processes[iProcess]))
    {
      fprintf(stderr, "Failed to run runtime app!\n");
    }
  }

  // wait for all the asset compilations
  waitForProcesses(configuration, processes, testResults, configuration.m_RTTimeout, "runRuntimeExe");

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LoadConfiguration(int argc, const char* argv[], 
                       Configuration& configuration, 
                       GeneticAlgorithmParameters& geneticAlgorithmParameters,
                       Tuneables& tuneables)
{
  std::string tuningConfigurationFile = "tuning.xml";

  for (int iArg = 1 ; iArg < argc ; ++iArg)
  {
    if (!strcmp(argv[iArg], "-tuningConfigurationFile"))
    {
      tuningConfigurationFile = argv[iArg+1];
      break;
    }
  }

  if (!configuration.initArgs(argc, argv))
  {
    fprintf(stderr, "Failed to init configuration from command line arguments\n");
    return false;
  }

  NM::TiXmlDocument doc(tuningConfigurationFile.c_str());
  if (!doc.LoadFile())
  {
    fprintf(stderr, "Failed to open %s\n", tuningConfigurationFile.c_str());
    return false;
  }

  if (!configuration.loadFromDoc(doc))
  {
    fprintf(stderr, "Failed to load configuration from %s\n", tuningConfigurationFile.c_str());
    return false;
  }

  if (!geneticAlgorithmParameters.load(doc))
  {
    fprintf(stderr, "Failed to load GA parameters from %s\n", tuningConfigurationFile.c_str());
    return false;
  }

  if (!tuneables.load(doc))
  {
    fprintf(stderr, "Failed to load tuneables from %s\n", tuningConfigurationFile.c_str());
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool initialiseGA(const Configuration& configuration, 
                  const GeneticAlgorithmParameters& geneticAlgorithmParameters, 
                  const Tuneables& tuneables,
                  GA& ga)
{
  // Always initialise, and then override with the results from file if desired
  size_t numGenes = tuneables.getNumTuneableValues();
  size_t popSize = geneticAlgorithmParameters.m_populationSize;

  std::vector<float> genome(numGenes);

  for (size_t iGenome = 0 ; iGenome < popSize ; ++iGenome)
  {
    for (size_t iGene = 0 ; iGene < numGenes ; ++iGene)
    {
      const TuneableValue* tuneableAttribute = tuneables.getTuneableValue(iGene);
      assert(tuneableAttribute);

      float normalisedDefaultValue = getNormalisedValue(
        tuneableAttribute->m_minValue, 
        tuneableAttribute->m_maxValue, 
        tuneableAttribute->m_defaultValue);

      float r = -1.0f + (2.0f * rand()) / RAND_MAX; // -1 to 1
      r *= geneticAlgorithmParameters.m_initialPopulationVariance;
      normalisedDefaultValue += r;
      if (normalisedDefaultValue < 0.0f)
        normalisedDefaultValue = 0.0f;
      else if (normalisedDefaultValue > 1.0f)
        normalisedDefaultValue = 1.0f;

      genome[iGene] = normalisedDefaultValue;
    }
    ga.setGenome(iGenome, &genome[0], 0.0f);
  }

  ga.setMutationMax(geneticAlgorithmParameters.m_maxMutation);
  ga.setRecombinationRate(geneticAlgorithmParameters.m_recombinationRate);

  if (configuration.m_continueFromLastRun)
  {
    TunerGeneConverter gc(tuneables);
    if (!ga.loadPopulation(configuration.m_GAPopulationFile.c_str(), &gc))
    {
      fprintf(stderr, "Failed to load the previous GA population - check that the configuration is compatible with %s\n",
        configuration.m_GAPopulationFile.c_str());
      return false;
    }
  }


  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool checkExitCodesAreOK(const ExitCodes& exitCodes)
{
  for (size_t i = 0 ; i != exitCodes.size() ; ++i)
  {
    if (exitCodes[i] != 0)
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void waitForTime(const Configuration& configuration)
{
  float tStart = configuration.m_startTime;
  float tEnd = configuration.m_endTime;
  while (tEnd < tStart)
    tEnd += 24.0f;

  while (tEnd > tStart)
  {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    float t = timeinfo->tm_hour + (timeinfo->tm_min / 60.0f);

    while (t < tStart)
      t += 24.0f;

    if (t > tStart && t < tEnd)
      return;

    float timeToStart = tStart - t;
    while (timeToStart < 0.0f)
      timeToStart += 24.0f;
    printf("Tuning will start in %.2f hours\n", timeToStart);

    // Sleep for a maximum of one hour
    const int msSecond = 1000;
    const int msHour = msSecond * 3600;
    int ms = (int) (timeToStart * 1000 * 3600);
    if (ms > msHour)
      ms = msHour;
    if (ms < msSecond)
      ms = msSecond;
    Sleep(ms);
  }
}

//----------------------------------------------------------------------------------------------------------------------
int main(int argc, const char* argv[])
{
  Configuration configuration;
  GeneticAlgorithmParameters geneticAlgorithmParameters;
  Tuneables tuneables;

  if (!LoadConfiguration(argc, argv, configuration, geneticAlgorithmParameters, tuneables))
  {
    fprintf(stderr, "Failed to initialise configuration\n");
    return 1;
  }

  GA ga(
    geneticAlgorithmParameters.m_populationSize, 
    tuneables.getNumTuneableValues(), 
    geneticAlgorithmParameters.m_demeWidth);

  if (!initialiseGA(configuration, geneticAlgorithmParameters, tuneables, ga))
  {
    fprintf(stderr, "Failed to initialise GA\n");
    return 1;
  }

  printf("Initialisation finished - now starting the tuning loop. Use Ctrl-c to stop it.\n");

  ExitCodes exitCodes(configuration.m_maxNumProcesses);
  Genomes genomes(configuration.m_maxNumProcesses);
  std::vector<float> fitnesses(exitCodes.size());

  // The sequence is (for each process we start):
  // 1. Create working dir
  // 2. Copy the network to the working dir
  // 3. Modify the network, and export and process the network to the working dir
  // 4. Run the runtime executable
  // 5. Pick up the result.
  // 6. Update the tuning params
  // 7. Goto 2
  for (unsigned int iterations = 1 ; iterations != 0; ++iterations)
  {
    waitForTime(configuration);

    if (configuration.m_logLevel >= Configuration::LOG_VERBOSE)
      printf("============ Tuning iteration %d generation %d ==============\n", iterations, ga.getCurrentGeneration());

    if (!prepareWorkingDirectories(configuration))
    {
      fprintf(stderr, "Failed to prepare working directories\n");
      return 1;
    }

    if (configuration.m_maxNumProcesses == 1)
      genomes[0] = ga.getCurrentGenome();
    else
      genomes = ga.getNTournamentPairs(configuration.m_maxNumProcesses/2);

    if (!modifyAndExportFromConnect(configuration, tuneables, genomes, exitCodes))
    {
      fprintf(stderr, "Failed to export network from connect\n");
      return 1;
    }

    if (!checkExitCodesAreOK(exitCodes))
    {
      fprintf(stderr, "Error when exporting network from connect - attempting to re-run this iteration\n");
      --iterations;
      continue;
    }

    if (!processExportedAssets(configuration, exitCodes))
    {
      fprintf(stderr, "Failed to process exported assets\n");
      return 1;
    }

    if (!checkExitCodesAreOK(exitCodes))
    {
      fprintf(stderr, "Error when processing exported assets - attempting to re-run this iteration\n");
      --iterations;
      continue;
    }

    // Note that when we run the runtime executable, if it crashes, there's a good chance unwanted parameters are
    // causing the physics engine to be unstable. So - if it crashes just treat it as a result with fitness = 0.
    if (!runRuntimeExe(configuration, exitCodes))
    {
      fprintf(stderr, "Failed to run runtime executable\n");
      return 1;
    }
    
    for (size_t i = 0 ; i != exitCodes.size() ; ++i)
    {
      fitnesses[i] = (float) exitCodes[i];
      if (fitnesses[i] < 0.0f || fitnesses[i] > 1000000.0f) // Catch explosions which cast -ve to a uint and then float
      {
        assert(0);
        fitnesses[i] = 0.0f;
      }
      if (configuration.m_logLevel >= Configuration::LOG_VERBOSE)
        printf("fitness %d = %f\n", i, fitnesses[i]);
    }

    if (configuration.m_maxNumProcesses == 1)
      ga.setFitness(fitnesses[0]);
    else
      ga.setNFitnessPairs(fitnesses);

    // Save the current state
    TunerGeneConverter gc(tuneables);
    ga.savePopulation(configuration.m_GAPopulationFile.c_str(), &gc);

    // Write out the best results
    float bestFitness, averageFitness;
    const float* bestGenome = ga.getBestGenome(bestFitness);
    const float* averageGenome = ga.getWeightedAverageGenome(averageFitness);

    printf("Iteration %d generation %d - Best fitness = %f, weighted average fitness = %f\n", 
      iterations, ga.getCurrentGeneration(), bestFitness, averageFitness);

    createModificationScript("ModifyNetwork-Best.lua", tuneables, bestGenome, bestFitness);
    createModificationScript("ModifyNetwork-Average.lua", tuneables, averageGenome, averageFitness);
  }

  return 0;
}
