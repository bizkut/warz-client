// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMCommandLineProcessor.h"
#include "NMPlatform/NMFile.h"
//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#if defined NM_HOST_WIN32 || defined NM_HOST_WIN64
#define SUPPORTS_OPTIONS_FILES
#endif

#ifdef SUPPORTS_OPTIONS_FILES
#include <sys/stat.h>
#endif
//----------------------------------------------------------------------------------------------------------------------

#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
#define SSCANF(SSCANF_SRC, SSCANF_FMT, ...) sscanf_s((SSCANF_SRC), (SSCANF_FMT), __VA_ARGS__)
#elif defined(NM_COMPILER_LLVM) || defined (NM_COMPILER_GCC) || defined (NM_COMPILER_SNC) || defined (NM_COMPILER_GHS)
#define SSCANF(SSCANF_SRC, SSCANF_FMT, ...) sscanf((SSCANF_SRC), (SSCANF_FMT), __VA_ARGS__)
#else
#error Secure scanf undefined for this compiler / platform
#endif

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// CommandLineProcessor
//----------------------------------------------------------------------------------------------------------------------
CommandLineProcessor::CommandLineProcessor()
{
  m_options = OptionVector::create(kMaxNumOptions, &m_allocator);

  m_parseErrorString[0] = '\0';
  m_executablePath = 0;
  m_executableName = 0;
}

//----------------------------------------------------------------------------------------------------------------------
CommandLineProcessor::~CommandLineProcessor()
{
  while (!m_options->empty())
  {
    Option& option = m_options->back();

    m_allocator.memFree(option.m_name);

    if (option.m_Type == kStringOptionType)
    {
      if (option.m_value.m_string)
      {
        m_allocator.memFree(option.m_value.m_string);
      }
    }
    if (option.m_Type == kStringListOptionType)
    {
      // free the strings in the list
      while (!option.m_value.m_stringList->empty())
      {
        m_allocator.memFree((void*)(option.m_value.m_stringList->back()));
        option.m_value.m_stringList->pop_back();
      }

      if (option.m_value.m_stringList)
      {
        m_allocator.memFree(option.m_value.m_stringList);
      }
    }

    m_options->pop_back();
  }

  m_allocator.memFree(m_options);

  if (m_executablePath)
    m_allocator.memFree(m_executablePath);
  if (m_executableName)
    m_allocator.memFree(m_executableName);
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::registerFlagOption(const char* optionName)
{
  return addOption(optionName, kFlagOptionType);
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::registerUInt32Option(const char* optionName)
{
  return addOption(optionName, kUInt32OptionType);
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::registerFloatOption(const char* optionName)
{
  return addOption(optionName, kFloatOptionType);
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::registerStringOption(const char* optionName)
{
  return addOption(optionName, kStringOptionType);
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::registerStringListOption(const char* optionName)
{
  return addOption(optionName, kStringListOptionType);
}
/*
//----------------------------------------------------------------------------------------------------------------------
#error convert to cleanPath (in, out) and make all callers use that. ACOptions should call clean path and then move rest of code there.
bool CommandLineProcessor::makeAbsolutePath(char *outPath, uint32_t outLen, const char* inPath, const char* basePath)
{
#ifdef SUPPORTS_OPTIONS_FILES

  NMP_ASSERT(basePath);

  // The input path may contain quotes so that a path with spaces can be used, these need to be removed.
  if (inPath[0] == '\'' || inPath[0] == '\"')
  {
    NMP_STRNCPY_S(outPath, outLen, inPath + 1);
  }
  else
  {
    NMP_STRNCPY_S(outPath, outLen, inPath);
  }

  // Remove trailing quotes
  const size_t lastCharacterIndex = NMP_STRLEN(outPath) - 1;
  if (outPath[lastCharacterIndex] == '\'' || outPath[lastCharacterIndex] == '\"')
  {
    outPath[lastCharacterIndex] = '\0';
  }
#error this should be in ACOptions - see the patch
  // Check if the directory exists.
  struct stat directoryInfo;
  int result = stat(outPath, &directoryInfo);
  if (result != 0 || ((directoryInfo.st_mode & (S_IFDIR | S_IFMT)) == 0))
  {
    // If the directory didn't exist it may have been a relative path to the asset compiler exe. Use that path if it
    // exists.
    static const uint32_t bufferLen = 2048;
    char buffer[bufferLen];
    NMP_ASSERT(bufferLen >= outLen);

    NMP_STRNCPY_S(buffer, bufferLen, basePath);
    NMP_STRNCAT_S(buffer, bufferLen, outPath);

    result = stat(buffer, &directoryInfo);
    if (result != 0)
    {
      NMP_VERIFY_FAIL("Failed to find path \"%s\".", buffer);
      return false;
    }
    if ((directoryInfo.st_mode & (S_IFDIR | S_IFMT)) == 0)
    {
      NMP_VERIFY_FAIL("Path \"%s\" is not a file or directory.", buffer);
      return false;
    }
    NMP_STRNCPY_S(outPath, outLen, buffer);
  }

  return true;

#else // SUPPORTS_OPTIONS_FILES

  (void)outPath;
  (void)outLen;
  (void)inPath;
  (void)basePath;
  NMP_VERIFY_FAIL("Option files are not implemented on this platform");
  return false;

#endif // SUPPORTS_OPTIONS_FILES
}
*/
//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::parseString(char* string)
{
  // tokenise the string into a string array
  char* strArray[kMaxNumOptions];
  const char seps[] = " \t\r\n"; // note the initial space character
  char *next_token = NULL;
  char *token = NMP_STRTOK(string, seps, &next_token);
  uint32_t i = 0;
  while (token != NULL)
  {
    strArray[i] = token;
    ++i;
    token = NMP_STRTOK(NULL, seps, &next_token);
  }

  return parseStringArray(i, strArray);
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::parseFile(const char* filename)
{
  // The input path may contain quotes so that a path with spaces can be used, these need to be removed.
  char cleanFilename[MAX_PATH];
  NMP::NMFile::removeQuotesFromPath(filename, MAX_PATH, cleanFilename, MAX_PATH);

  // open the config file that contains (space, tab and new-line separated) command line parameters
  if (!NMP::NMFile::getExists(cleanFilename))
  {
    NMP_SPRINTF(m_parseErrorString, kMaxErrorStringLength, "Unable to open config file '%s'.", cleanFilename);
    return false;
  }

  uint64_t length = NMP::NMFile::getSize(cleanFilename);
  char *buffer = (char*)NMPAllocatorMemAlloc(&m_allocator, (size_t)length+1, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP_ASSERT(buffer);
  NMP::NMFile::load(cleanFilename, buffer, length);
  // Null terminate the string
  buffer[length] = '\0';

  bool success = parseString(buffer);
  m_allocator.memFree(buffer);
  if (!success)
  {
    // parseConfigString will have filled in the error message
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::parseCommandLine(int argc, const char* const* argv)
{
  // reset the error string
  //
  m_parseErrorString[0] = '\0';

#if !(defined NM_HOST_NO_EXE_NAME_CLA)
  // On most platforms, the path and filename are passed as the first argument.
  {
    // Store the path and filename.  PathRemoveFileSpec can't be used because it doesn't handle forward slashes.
    //

    // Find the last slash
    size_t slashLoc = NMP_STRLEN(argv[0]);
    if (argv[0][slashLoc] != '\\' && argv[0][slashLoc] != '/')
    {
      while (slashLoc-- && argv[0][slashLoc] != '\\' && argv[0][slashLoc] != '/')
      {
      }

      // slashLoc can be -1 at this moment.

      size_t length;

      length = NMP_STRLEN(argv[0]) + 1;
      m_executablePath = (char*)NMPAllocatorMemAlloc(&m_allocator, length, NMP_NATURAL_TYPE_ALIGNMENT);
      NMP_ASSERT(m_executablePath);
      m_executableName = (char*)NMPAllocatorMemAlloc(&m_allocator, length, NMP_NATURAL_TYPE_ALIGNMENT);
      NMP_ASSERT(m_executableName);
      NMP_STRNCPY_S(m_executablePath, length, argv[0]);
      m_executablePath[slashLoc + 1] = '\0';
      NMP_STRNCPY_S(m_executableName, length - slashLoc - 1, &argv[0][slashLoc + 1]);
    }
  }

  // Advance past the first argument
  argc--;
  argv++;
#endif // NM_HOST_NO_EXE_NAME_CLA

  // Now parse the other arguments
  if (!parseStringArray(argc, &argv[0]))
  {
    // Failure - the error string will have been filled in
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::parseStringArray(int numArgs, const char* const* argArray)
{
  for (int i = 0; i < numArgs; ++i)
  {
    // Special case: check for a config file.
    if (strcmp("-optionsFile", argArray[i]) == 0)
    {
      if (i+1 >= numArgs)
      {
        NMP_SPRINTF(
          m_parseErrorString,
          kMaxErrorStringLength,
          "command line switch '%s' requires string argument but got none.",
          argArray[i]);
        return false;
      }

      // this is a config file that contains (new-line seperated) command line parameters
      if (!parseFile(argArray[i+1]))
      {
        // Error string will have been filled in by parseConfigFile
        return false;
      }

      // Don't want to do normal processing on this now, skip to the next arg.
      ++i;
      continue;
    }

    // Loop over all the registered arg, finding the one with the correct name
    Option *option = NULL;
    const OptionVector::iterator end = m_options->end();
    for (OptionVector::iterator it = m_options->begin(); it != end; ++it)
    {
      if (strcmp((*it).m_name, argArray[i]) == 0)
      {
        option = &(*it);
        break;
      }
    }

    if (!option)
    { 
      // Couldn't find an arg with the correct name
      NMP_SPRINTF(m_parseErrorString, kMaxErrorStringLength, "unknown command line switch '%s'.", argArray[i]);
    }
    else
    {
        switch (option->m_Type)
        {
        case kFlagOptionType:
          // a flag being specified means it is enabled
          //
          option->m_value.m_flag = true;
          option->m_hasValue = true;
          break;
        case kUInt32OptionType:
          if (i + 1 < numArgs)
          {
            // sscanf returns the number of successfully filled variables which should be 1.
            //
            if (SSCANF(argArray[i + 1], "%u", &option->m_value.m_uint32) != 1)
            {
              NMP_SPRINTF(
                m_parseErrorString,
                kMaxErrorStringLength,
                "command line switch '%s' expected integer argument but got '%s'.",
                argArray[i],
                argArray[i + 1]);
              return false;
            }
            option->m_hasValue = true;
    
            ++i;
          }
          else
          {
            NMP_SPRINTF(
              m_parseErrorString,
              kMaxErrorStringLength,
              "command line switch '%s' requires integer argument but got none.",
              argArray[i]);
            return false;
          }
          break;
        case kFloatOptionType:
          if (i + 1 < numArgs)
          {
            // sscanf returns the number of successfully filled variables which should be 1.
            //
            if (SSCANF(argArray[i + 1], "%f", &option->m_value.m_float) != 1)
            {
              NMP_SPRINTF(
                m_parseErrorString,
                kMaxErrorStringLength,
                "command line switch '%s' expected float argument but got '%s'.",
                argArray[i],
                argArray[i + 1]);
              return false;
            }
            option->m_hasValue = true;
    
            ++i;
          }
          else
          {
            NMP_SPRINTF(
              m_parseErrorString,
              kMaxErrorStringLength,
              "command line switch '%s' requires float argument but got none.",
              argArray[i]);
            return false;
          }
          break;
        case kStringOptionType:
          if (i + 1 < numArgs)
          {
            if (option->m_value.m_string)
            {
              m_allocator.memFree(option->m_value.m_string);
            }
    
            // copy the argument
            //
            size_t length = NMP_STRLEN(argArray[i + 1]) + 1;
            option->m_value.m_string = (char*)NMPAllocatorMemAlloc(&m_allocator, length, NMP_NATURAL_TYPE_ALIGNMENT);
            NMP_ASSERT(option->m_value.m_string);
            NMP_STRNCPY_S(option->m_value.m_string, length, argArray[i + 1]);
            option->m_hasValue = true;
    
            ++i;
          }
          else
          {
            NMP_SPRINTF(
              m_parseErrorString,
              kMaxErrorStringLength,
              "command line switch '%s' requires string argument but got none.",
              argArray[i]);
            return false;
          }
          break;
        case kStringListOptionType:
          if (i + 1 < numArgs)
          {
            // The container for the string list will have been allocated when it was added
            // copy the argument
            //
            size_t length = NMP_STRLEN(argArray[i + 1]) + 1;
            char *str = (char*)NMPAllocatorMemAlloc(&m_allocator, length, NMP_NATURAL_TYPE_ALIGNMENT);
            NMP_ASSERT(str);
            NMP_STRNCPY_S(str, length, argArray[i + 1]);
            option->m_value.m_stringList->push_back(str);
            option->m_hasValue = true;
    
            ++i;
          }
          else
          {
            NMP_SPRINTF(
              m_parseErrorString,
              kMaxErrorStringLength,
              "command line switch '%s' requires string argument but got none.",
              argArray[i]);
            return false;
          }
          break;
        }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* CommandLineProcessor::getParseErrorString() const
{
  return &m_parseErrorString[0];
}

//----------------------------------------------------------------------------------------------------------------------
char* CommandLineProcessor::reportParsedOptions(char *buffer, uint32_t len) const
{
  buffer[0] = (char)NULL;
  for (OptionVector::iterator it = m_options->begin(), end = m_options->end(); it != end; ++it)
  {
    Option& option = *it;

    if (option.hasValue())
    {
      // There is some data for this parameter
      NMP_STRNCAT_S(buffer, len, option.m_name);
      NMP_STRNCAT_S(buffer, len, "\t");

      NMP_STRNCAT_S(buffer, len, option.getTypeString());
      NMP_STRNCAT_S(buffer, len, "\t");

      char tempBuff[2048];
      option.getValueString(tempBuff, 2048);
      NMP_STRNCAT_S(buffer, len, tempBuff);
      NMP_STRNCAT_S(buffer, len, "\t");

      NMP_STRNCAT_S(buffer, len, "\n");
    }
  }
  return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
const char* CommandLineProcessor::getExecutableName() const
{
  return m_executableName;
}

//----------------------------------------------------------------------------------------------------------------------
const char* CommandLineProcessor::getExecutablePath() const
{
  return m_executablePath;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::getOptionValue(const char* optionName, bool* value) const
{
  NMP_ASSERT(value);

  const OptionValue* optionValue = getOptionValue(optionName, kFlagOptionType);
  if (optionValue)
  {
    *value = optionValue->m_flag;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::getOptionValue(const char* optionName, uint32_t* value) const
{
  NMP_ASSERT(value);

  const OptionValue* optionValue = getOptionValue(optionName, kUInt32OptionType);
  if (optionValue)
  {
    *value = optionValue->m_uint32;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::getOptionValue(const char* optionName, float* value) const
{
  NMP_ASSERT(value);

  const OptionValue* optionValue = getOptionValue(optionName, kFloatOptionType);
  if (optionValue)
  {
    *value = optionValue->m_float;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::getOptionValue(const char* optionName, const char** value) const
{
  NMP_ASSERT(value);

  const OptionValue* optionValue = getOptionValue(optionName, kStringOptionType);
  if (optionValue)
  {
    *value = optionValue->m_string;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::getOptionValue(const char* optionName, const StringList** value) const
{
  NMP_ASSERT(value);

  const OptionValue* optionValue = getOptionValue(optionName, kStringListOptionType);
  if (optionValue)
  {
    *value = optionValue->m_stringList;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::addOption(const char* optionName, OptionTypes optionType)
{
  if (m_options->size() >= kMaxNumOptions)
  {
    return false;
  }

  if (!optionName)
  {
    return false;
  }

  size_t length = NMP_STRLEN(optionName) + 1;
  if (length == 1)
  {
    return false;
  }

  Option option;

  // allocate and copy the option name
  //
  option.m_name = (char*)NMPAllocatorMemAlloc(&m_allocator, length, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP_ASSERT(option.m_name);
  NMP_STRNCPY_S(option.m_name, length, optionName);

  option.m_Type = optionType;
  option.m_hasValue = false;
  // Initialise to 0 - not initialise as a ptr to make sure 64bit platforms work.
  option.m_value.m_string = 0;

  if (optionType == kStringListOptionType)
  {
    Memory::Format reqs = StringList::getMemoryRequirements(kMaxNumStringListEntries);
    Memory::Resource res = NMPAllocatorAllocateFromFormat(&m_allocator, reqs);
    option.m_value.m_stringList = StringList::init(res, kMaxNumStringListEntries);
  }

  bool result = m_options->push_back(option);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const CommandLineProcessor::OptionValue* CommandLineProcessor::getOptionValue(
  const char* optionName,
  OptionTypes expectedArgumentType) const
{
  for (OptionVector::iterator it = m_options->begin(), end = m_options->end(); it != end; ++it)
  {
    Option& option = *it;

    if (strcmp(option.m_name, optionName) == 0)
    {
      if (option.m_Type == expectedArgumentType)
      {
        if (option.m_hasValue)
        {
          return &option.m_value;
        }
        else
        {
          // The option's value wasn't parsed
          return 0;
        }
      }
      else
      {
        NMP_VERIFY_FAIL("Command line option '%s' is not of expected type", optionName);
        return 0;
      }
    }
  }

  NMP_VERIFY_FAIL("Unknown command line option: %s", optionName);
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool CommandLineProcessor::Option::hasValue() const
{
  return m_hasValue;
}

//----------------------------------------------------------------------------------------------------------------------
const char* CommandLineProcessor::Option::getTypeString() const
{
  switch (m_Type)
  {
  case kFlagOptionType:
    return "Flag";
  case kUInt32OptionType:
    return "uint32";
  case kFloatOptionType:
    return "float";
  case kStringOptionType:
    return "string";
  case kStringListOptionType:
    return "StrList";
  default:
    NMP_VERIFY_FAIL("Unknown Argument Type");
  }
  return "ERROR";
}

//----------------------------------------------------------------------------------------------------------------------
const char* CommandLineProcessor::Option::getValueString(char *buffer, uint32_t len) const
{
  // Initialise the buffer
  buffer[0] = (char)NULL;
  static const uint32_t bufferLen = 255;
  char tempBuff[bufferLen];

  switch (m_Type)
  {
  case kFlagOptionType:
    if (m_value.m_flag)
    {
      NMP_STRNCAT_S(buffer, len, "true");
    }
    else
    {
      NMP_STRNCAT_S(buffer, len, "false");
    }
    break;
  case kUInt32OptionType:
    NMP_SPRINTF(tempBuff, bufferLen, "%i", m_value.m_uint32);
    NMP_STRNCAT_S(buffer, len, tempBuff);
    break;
  case kFloatOptionType:
    NMP_SPRINTF(tempBuff, bufferLen, "%f", m_value.m_float);
    NMP_STRNCAT_S(buffer, len, tempBuff);
    break;
  case kStringOptionType:
    NMP_STRNCAT_S(buffer, len, m_value.m_string);
    break;
  case kStringListOptionType:
    {
      NMP_STRNCAT_S(buffer, len, "{ ");
      NMP::CommandLineProcessor::StringList::iterator it = m_value.m_stringList->begin();
      for (; it != m_value.m_stringList->end(); ++it)
      {
        NMP_STRNCAT_S(buffer, len, *it);
        NMP_STRNCAT_S(buffer, len, " ");
      }
      NMP_STRNCAT_S(buffer, len, "}");
    }
    break;
  }
  return buffer;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
