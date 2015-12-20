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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <NMTinyXML.h>

//----------------------------------------------------------------------------------------------------------------------
struct Configuration
{
  /// \brief initialise the Configuration structure using passed in command line arguments.  This function also
  ///  manages display of the help text.
  bool initArgs(int argc, const char* argv[]);

  /// \brief Load configuration properties from the provided TinyXML document.
  /// \return true on successful reading, or false if any expected properties are not found.
  bool loadFromDoc(NM::TiXmlDocument& doc);

  enum LogLevel
  {
    LOG_MINIMAL,
    LOG_VERBOSE
  };

  std::string m_RTExe;
  std::string m_RTArgs;
  std::string m_assetCompilerExe;
  std::string m_assetCompilerBaseDir;
  std::string m_assetCompilerPluginDir;
  std::string m_assetCompilerConfigFile;
  std::string m_connectExe;
  std::string m_workingDir;
  std::string m_network;
  std::string m_projectFile;
  std::string m_GAPopulationFile;

  float m_RTTimeout;
  float m_assetCompilerTimeout;
  float m_connectTimeout;

  // specified on the command line
  bool m_continueFromLastRun;
  int m_maxNumProcesses;
  // Only run between the start and end times (local time - so 20.5 is 8:30pm)
  float m_startTime;
  float m_endTime;
  LogLevel m_logLevel;
};

//----------------------------------------------------------------------------------------------------------------------
struct GeneticAlgorithmParameters
{
  bool load(NM::TiXmlDocument& doc);
  int m_populationSize;
  int m_demeWidth;
  float m_maxMutation;
  float m_recombinationRate;
  float m_initialPopulationVariance;
};

//----------------------------------------------------------------------------------------------------------------------
struct TuneableValue
{
  bool load(NM::TiXmlElement* elem);

  std::string m_name;
  float m_minValue;
  float m_maxValue;
  float m_defaultValue;
};

//----------------------------------------------------------------------------------------------------------------------
struct TuneableControlParameter
{
  bool load(NM::TiXmlElement* elem);

  std::string m_name;
  float m_minValue;
  float m_maxValue;
  float m_defaultValue;
};

//----------------------------------------------------------------------------------------------------------------------
struct NodeTypeAttributes
{
  bool load(NM::TiXmlHandle handle);
  std::string m_nodeType;

  std::vector<TuneableValue> m_tuneableAttributes;
};

//----------------------------------------------------------------------------------------------------------------------
struct Tuneables
{
  bool load(NM::TiXmlDocument& doc);

  size_t getNumTuneableValues() const {return getNumTuneableAttributes() + getNumTuneableControlParameters();}
  const TuneableValue* getTuneableValue(size_t index) const;

  size_t getNumTuneableAttributes() const;
  const TuneableValue* getTuneableAttribute(size_t index) const;

  size_t getNumTuneableControlParameters() const;
  const TuneableValue* getTuneableControlParameter(size_t index) const;

  typedef std::vector<NodeTypeAttributes> TuneableAttributes;
  typedef std::vector<TuneableValue> TuneableControlParameters;

  TuneableAttributes m_tuneableAttributes;
  TuneableControlParameters m_tuneableControlParamers;
};


#endif // CONFIGURATION_H
