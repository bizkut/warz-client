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

#include "Precompiled.h"
#include "LibMDF.h"
#include "FileUtils.h"

#include "EmitCPP.h"
#include "EmitManifest.h"
#include "EmitXMLSummary.h"
#include "NetworkStats.h"

#include <string>
#include "nmtl/vector.h"
#include "nmtl/algorithm.h"

typedef nmtl::vector<std::string> FileList;


//----------------------------------------------------------------------------------------------------------------------
// recurse through directories from the given root, finding types and modules to load; these are then stored in one
// of the given FileList arrays to be walked later and fed into the network builder 
//
void gatherInputFilesRecursively(char* inputDirectory, FileList& typeFiles, FileList& moduleFiles, FileList& behaviourFiles);


//----------------------------------------------------------------------------------------------------------------------
// feedback interface that just dumps to the console via printf - although it formats to support linking in Visual Studio,
// as this tool may well be run as a built-step from the IDE
//
class ConsoleFeedbackInterface : public MDFFeedbackInterface
{
public:

  virtual void onStageBegin(Stage) NM_OVERRIDE
  {
  }

  virtual void onProcessingFailure(
    Stage duringStage,
    const char* inFilename, 
    unsigned int atLine, 
    const char *message, 
    const char *reftext,
    void *) NM_OVERRIDE
  {
    static const char* stageNames[MDFFeedbackInterface::StageCount] = 
    {
      "Parser",
      "Execution",
      "Validation",
      "Connectivity",
    };

    printf("%s: %s - %s\n", stageNames[duringStage], message, reftext);
    // This way, VS will automatically add the double click functionality and take you directly to the source.
    printf("%s(%i) \n", inFilename, atLine);
  }

  virtual void onStageComplete(Stage) NM_OVERRIDE
  {
  }

  virtual void onLogMessage(
    MDFFeedbackInterface::MessageType mt,
    const char* msg) NM_OVERRIDE
  {
    static const char* mtNames[MDFFeedbackInterface::MessageTypeCount] = 
    {
      "",
      "Warning: ",
      "Error: ",
      "API Error: ",
    };

    printf("%s%s\n", mtNames[mt], msg);
  }
};


//----------------------------------------------------------------------------------------------------------------------
// Recursively create directories to fulfill the path structure passed in
// Note: only works for relative paths, not absolute paths
//
bool createDirectoryRecursively(char* path)
{
  char folder[MAX_PATH];
  size_t bufferSize = MAX_PATH * sizeof(char);

  // loop over all occurrences of '\' creating the directory at each stage
  char* end = strchr(path, L'\\');

  while (end != NULL)
  { 
    // Split the path at the '\' (add 1 to make sure we include the trailing '\')
    ptrdiff_t numChars = end - path + 1;
    strncpy_s(folder, bufferSize, path, numChars);

    if (!CreateDirectory(folder, NULL))
    {
      DWORD err = GetLastError();

      if (err != ERROR_ALREADY_EXISTS)
      {
        // Something went wrong
        return false;
      }
    }

    // find the next '\'
    end = strchr(++end, L'\\');
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// welcome to the MDF parser!
//
int main(int argc, char *argv[])
{
  static char gInputDirectory[MAX_PATH] = {0},
              gOutputDirectory[MAX_PATH] = {0},
              gOutputManifestDirectory[MAX_PATH] = {0},
              gProjectRootDirectory[MAX_PATH] = {0};

  bool verboseMode = false,
       forceCodeGen = false,
       genStats = false,
       genXml = false;

  printf("\neuphoria3 MDF Processor\nCopyright(c) 2009-2013 NaturalMotion ltd.\n\n");

  // - Configure ------------------------------------------------------------

  // if the tool was run without arguments, print usage docs and quit
  if (argc <= 1)
  {
    printf("Usage:\n  %s -p <project root dir> -i <input dir> -o <output dir> -mo <manifest output dir>\n\n", argv[0]);
    printf("      -f              Force code generation, ignore file contents check\n");
    printf("      -stats          Emit network statistics to the console\n");
    printf("      -v              Verbose output mode\n");
    printf("      -xml            Generate XML summary instead of other formats\n");
    return 1;
  }

  // read through cmdline looking for configuration tokens
  for (int i=1; i<argc; i++)
  {
    if (strcmp(argv[i], "-i")==0)
    {
      NMP_STRNCPY_S(gInputDirectory, MAX_PATH - 1, argv[i+1]);
      PathAddBackslash(gInputDirectory);
      i++;
    }
    else if (strcmp(argv[i], "-o")==0)
    {
      NMP_STRNCPY_S(gOutputDirectory, MAX_PATH - 1, argv[i+1]);
      PathAddBackslash(gOutputDirectory);
      i++;
    }
    else if (strcmp(argv[i], "-mo")==0)
    {
      NMP_STRNCPY_S(gOutputManifestDirectory, MAX_PATH - 1, argv[i+1]);
      PathAddBackslash(gOutputManifestDirectory);
      i++;
    }
    else if (strcmp(argv[i], "-p")==0)
    {
      NMP_STRNCPY_S(gProjectRootDirectory, MAX_PATH - 1, argv[i+1]);
      PathAddBackslash(gProjectRootDirectory);
      i++;
    }
    else if (strcmp(argv[i], "-v")==0)
    {
      verboseMode = true;
    }
    else if (strcmp(argv[i], "-f")==0)
    {
      forceCodeGen = true;
    }
    else if (strcmp(argv[i], "-xml")==0)
    {
      genXml = true;
    }
    else if (strcmp(argv[i], "-stats")==0)
    {
      genStats = true;
    }
    else
    {
      printf("Unrecognized argument: %s\n", argv[i]);
    }
  }

  // - Validate Path Inputs -------------------------------------------------

  if (!PathIsDirectory(gInputDirectory))
  {
    printf("ERROR: input directory is invalid:\n\t'%s'\n", gInputDirectory);
    return 1;
  }
  if (!PathIsDirectory(gProjectRootDirectory))
  {
    printf("ERROR: project root directory is invalid:\n\t'%s'\n", gProjectRootDirectory);
    return 1;
  }
  if (!createDirectoryRecursively(gOutputDirectory) || !PathIsDirectory(gOutputDirectory))
  {
    printf("ERROR: output directory is invalid or could not be created:\n\t'%s'\n", gOutputDirectory);
    return 1;
  }
  if (!genXml)
  {
    if (!createDirectoryRecursively(gOutputManifestDirectory) || !PathIsDirectory(gOutputManifestDirectory))
    {
      printf("ERROR: manifest output directory is invalid or could not be created:\n\t'%s'\n", gOutputManifestDirectory);
      return 1;
    }
  }


  // - Gather Files ---------------------------------------------------------

#if defined(_MSC_VER)
  LARGE_INTEGER lStart, lEnd, lFreq;
  QueryPerformanceFrequency(&lFreq);
  QueryPerformanceCounter(&lStart);
#endif // USE_PERFORMANCE_TIMING

  printf("Searching For Files...\n");

  FileList moduleFiles, typeFiles, behaviourFiles;
  gatherInputFilesRecursively(gInputDirectory, typeFiles, moduleFiles, behaviourFiles);


  // - Setup the Network Builder --------------------------------------------

  printf("Initializing Parsing Engine...\n");

  ConsoleFeedbackInterface cfi;

  // MDFNetworkBuilder is the management class that provides all the API for 
  // assembling and analysing a complete MDF behaviour network
  MDFNetworkBuilder MDFNetwork;
  MDFNetwork.init(&cfi, 0xBADF000D);


  // - Type Load ------------------------------------------------------------

  printf("Parsing Types...\n");

  for (FileList::const_iterator it = typeFiles.begin(), 
                                end = typeFiles.end(); it != end; ++it)
  {
    if (!MDFNetwork.addTypes((*it).c_str()))
    {
      return 1;
    }
  }

  // connectivity & verification phases
  if (!MDFNetwork.postProcessTypes())
    return 1;


  // - Add Module Files -----------------------------------------------------

  printf("Parsing Modules...\n");

  for (FileList::const_iterator it = moduleFiles.begin(), 
                                end = moduleFiles.end(); it != end; ++it)
  {
    if (!MDFNetwork.addMDF((*it).c_str()))
    {
      return 1;
    }
  }

  // connectivity & verification phases
  if (!MDFNetwork.postProcessMDFs())
    return 1;

  // - Add Behaviour Files --------------------------------------------------

  printf("Parsing Behaviours...\n");

  for (FileList::const_iterator it = behaviourFiles.begin(), 
                                end = behaviourFiles.end(); it != end; ++it)
  {
    if (!MDFNetwork.addBehaviourDef((*it).c_str()))
    {
      return 1;
    }
  }

  // post process behaviour data
  if (!MDFNetwork.postProcessBehaviourDefs())
    return 1;


  // - Finalize -------------------------------------------------------------

  // build output artefact data for all loaded modules, behaviours, etc
  MDFNetwork.preprocessForCodeGeneration(gOutputDirectory, forceCodeGen, verboseMode);


#if defined(_MSC_VER)
  QueryPerformanceCounter(&lEnd);
  double timeTaken = (double)(lEnd.QuadPart - lStart.QuadPart) * 1000.0;
  timeTaken /= (double)lFreq.QuadPart;
  printf("Parse Time: %.1fms\n", (float)timeTaken);
  lStart = lEnd;
#endif // USE_PERFORMANCE_TIMING


  // ------------------------------------------------------------------------
  // disable parser memory system, mainly as a sanity check that any of the code gen
  // stuff isn't using PStrings, etc. If they try, asserts will fire.
  ActiveMemoryManager::set(0);


  // - Output ---------------------------------------------------------------

  printf("\n");

  const NetworkDatabase& ndb = MDFNetwork.getNDB();
  if (!validateDependencies(ndb))
    return -1;

  if (genXml)
  {
    emitXMLSummary(gOutputDirectory, ndb);
  }
  else
  {
    // generate module code
    emitCPPCode(gOutputDirectory, gProjectRootDirectory, ndb, forceCodeGen);

    // write behaviour manifest
    char manifestOutput[MAX_PATH];
    NMP_STRNCPY_S(manifestOutput, sizeof(manifestOutput), gProjectRootDirectory);
    PathAddBackslash(manifestOutput);
    NMP_STRNCAT_S(manifestOutput, sizeof(manifestOutput), gOutputManifestDirectory);
    emitManifest(manifestOutput, ndb, forceCodeGen);

    if (genStats)
      generateNetworkStatistics(ndb);
  }

#if defined(_MSC_VER)
  QueryPerformanceCounter(&lEnd);
  timeTaken = (double)(lEnd.QuadPart - lStart.QuadPart) * 1000.0;
  timeTaken /= (double)lFreq.QuadPart;
  printf("---------------------------------------------------------\nGen Time: %.1fms\n\n", (float)timeTaken);
#endif // USE_PERFORMANCE_TIMING

  return 0;
}


//----------------------------------------------------------------------------------------------------------------------
// recurse through directories from the given root, finding types and modules to load; these are then stored in one
// of the given FileList arrays to be walked later and fed into the network builder 
//
void gatherInputFilesRecursively(
  char* inputDirectory,
  FileList& typeFiles, 
  FileList& moduleFiles,
  FileList& behaviourFiles)
{
  char dir[MAX_PATH];

  NMP_STRNCPY_S(dir, sizeof(dir), inputDirectory);
  PathAddBackslash(dir);
  NMP_STRNCAT_S(dir, sizeof(dir), "*.*");

  WIN32_FIND_DATA findData;
  HANDLE findHandle = FindFirstFile(dir, &findData);
  if (findHandle == INVALID_HANDLE_VALUE)
  {
    printf("ERROR: could not examine contents of input directory (%s)\n", inputDirectory);
    exit(1);
  }

  do 
  {
    // formulate a full path spec
    PathRemoveFileSpec(dir);
    PathAddBackslash(dir);
    NMP_STRNCAT_S(dir, sizeof(dir), findData.cFileName);

    // recurse into directories
    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      // don't bother with ., .. or anything hidden/system
      if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
          !(findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
          strcmp(findData.cFileName, ".") &&
          strcmp(findData.cFileName, ".."))
      {
        gatherInputFilesRecursively(dir, typeFiles, moduleFiles, behaviourFiles);
      }
    }
    else
    {
      if (strcmp(PathFindExtension(findData.cFileName), ".types") == 0)
      {
        typeFiles.push_back(std::string(dir));
      }
      else if (strcmp(PathFindExtension(findData.cFileName), ".module") == 0)
      {
        moduleFiles.push_back(std::string(dir));
      }
      else if (strcmp(PathFindExtension(findData.cFileName), ".behaviour") == 0)
      {
        behaviourFiles.push_back(std::string(dir));
      }
    }

  } while(FindNextFile(findHandle, &findData) != 0);
  FindClose(findHandle);

  // FindFirstFile and FindNextFile don't guarantee order.
  nmtl::sort(typeFiles.begin(), typeFiles.end());
  nmtl::sort(moduleFiles.begin(), moduleFiles.end());
  nmtl::sort(behaviourFiles.begin(), behaviourFiles.end());
}
