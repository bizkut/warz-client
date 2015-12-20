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
#ifndef NM_COMMANDLINEPROCESSOR_H
#define NM_COMMANDLINEPROCESSOR_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVectorContainer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

/// \brief This class will parse a set of command line arguments and report the assigned values.
/// Specifying an option of '-config \<ConfigFileName\>' will parse the contents of that file for further arguments
/// \note This class relies on NMP::Memory being initialised for the whole of its lifetime.
class CommandLineProcessor
{
public:
  enum OptionTypes
  {
    kFlagOptionType,
    kUInt32OptionType,
    kFloatOptionType,
    kStringOptionType,
    kStringListOptionType
  };

  typedef NMP::VectorContainer<const char*> StringList;

  CommandLineProcessor();
  ~CommandLineProcessor();

  /// \brief Adds a flag argument.
  /// if the flag is specified on the command line the value will be true otherwise it will be false.
  bool registerFlagOption(const char* optionName);
  /// \brief Adds a unsigned int argument.
  /// If the option is specified on the command line the value will be stored.
  bool registerUInt32Option(const char* optionName);
  /// \brief Adds a float argument.
  /// If the option is specified on the command line the value will be stored.
  bool registerFloatOption(const char* optionName);
  /// \brief Adds a string argument.
  /// If the option is specified on the command line the value will be stored.
  bool registerStringOption(const char* optionName);
  /// \brief Adds a string list argument.
  /// If the option is specified on the command line the value specified will be added to the list.
  bool registerStringListOption(const char* optionName);

  /// \brief Parses a string (separated by tabs, spaces and newlines).
  /// Any unexpected arguments will cause this function to fail.
  bool parseString(char* string);
  /// \brief Parses a file (separated by tabs, spaces and newlines).
  /// Any unexpected arguments will cause this function to fail.
  bool parseFile(const char *filename);
  /// \brief Parses a command line.
  /// Any unexpected arguments will cause this function to fail.
  /// \note This function expects the executable name to be the first selement in the array
  bool parseCommandLine(int argc, const char* const* argv);

  /// \brief If parseCommandLine failed then this will return the error message explaining why, otherwise
  /// this is a zero length string.
  const char* getParseErrorString() const;

  /// \brief Report all the options parsed by the command line parser (into the buffer provided)
  char* reportParsedOptions(char *buffer, uint32_t len) const;

  /// \brief Gets the filename of the executable (a command line was parsed)
  const char* getExecutableName() const;
  /// \brief Path of the executable (a command line was parsed)
  const char* getExecutablePath() const;

  /// \brief Reads the value of the spcified value into 'value'
  /// \return false if the option was not specified on the command line
  bool getOptionValue(const char* optionName, bool* value) const;
  /// \brief Reads the value of the spcified value into 'value'
  /// \return false if the option was not specified on the command line
  bool getOptionValue(const char* optionName, uint32_t* value) const;
  /// \brief Reads the value of the spcified value into 'value'
  /// \return false if the option was not specified on the command line
  bool getOptionValue(const char* optionName, float* value) const;
  /// \brief Reads the value of the spcified value into 'value'
  /// \return false if the option was not specified on the command line
  bool getOptionValue(const char* optionName, const char** value) const;
  /// \brief Reads the value of the spcified value into 'value'
  /// \return false if the option was not specified on the command line
  bool getOptionValue(const char* optionName, const StringList** value) const;

protected:
  enum
  {
    kMaxNumOptions = 32,
    kMaxErrorStringLength = 256,
    kMaxNumStringListEntries = 128
  };

  struct OptionValue
  {
    union
    {
      bool        m_flag;
      uint32_t    m_uint32;
      float       m_float;
      char*       m_string;
      StringList* m_stringList;
    };
  };

  struct Option
  {
    bool hasValue() const;
    const char* getTypeString() const;
    const char* getValueString(char *buffer, uint32_t len) const;

    char* m_name;
    OptionTypes m_Type;
    OptionValue m_value;
    bool m_hasValue;
  };

  typedef NMP::VectorContainer<Option> OptionVector;

  OptionVector* m_options;

  NMP::HeapAllocator m_allocator;

  char m_parseErrorString[kMaxErrorStringLength];
  char *m_executablePath;
  char *m_executableName;

  /// \brief Parses the arguments passed in the array of strings. Does NOT expect the executable name. 
  /// 
  bool parseStringArray(int numArgs, const char* const* argArray);

  /// \brief Add the option to the list of things to be expected
  /// 
  bool addOption(const char* optionName, OptionTypes optionType);

  /// \brief Gets the value of the named option.
  /// 
  const OptionValue* getOptionValue(const char* optionName, OptionTypes expectedType) const;
};

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_COMMANDLINEPROCESSOR_H
//----------------------------------------------------------------------------------------------------------------------
