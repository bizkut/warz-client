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
#include "Configuration.h"

//----------------------------------------------------------------------------------------------------------------------
static const char* helpText = 
  "\n"
  "Tunes a morpheme network using a script and reference network running in a\n"
  " Squirrel enabled runtime executable\n"
  "\n"
  " -tuningConfigurationFile FILE\n"
  "    Uses FILE to define the behaviour. Defaults to tuning.xml.\n"
  "\n"
  " -continueFromLastRun\n"
  "    Continues tuning by loading the state from the tuning configuration file\n"
  "    Note that the genetic algorithm population size and number of tuneables.\n"
  "    must match\n"
  "\n"
  " -verbose\n"
  "    Enable verbose logging\n"
  "\n"
  " -startTime X\n"
  "      Waits until X before tuning - e.g. 20.5 for 8:30pm\n"
  "\n"
  " -endTime X\n"
  "      Stops tuning after X - e.g. 13.75 for 1:45pm\n"
  "\n"
  " -maxNumProcesses NUM\n"
  "    Use NUM threads. Note that if this is not 1 (default) then it must be an\n"
  "    even number\n";

//----------------------------------------------------------------------------------------------------------------------
static bool readValue(NM::TiXmlElement* elem, const char* name, std::string& value)
{
  const char* v = elem->Attribute(name);
  if (v)
  {
    value = v;
    return true;
  }
  else
  {
    fprintf(stderr, "Failed to read %s\n", name);
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
static bool readValue(NM::TiXmlElement* elem, const char* name, int& value)
{
  const char* v = elem->Attribute(name, &value);
  if (v)
  {
    return true;
  }
  else
  {
    fprintf(stderr, "Failed to read %s\n", name);
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
static bool readValue(NM::TiXmlElement* elem, const char* name, float& value)
{
  double doubleValue = value;
  const char* v = elem->Attribute(name, &doubleValue);
  if (v)
  {
    value = (float) doubleValue;
    return true;
  }
  else
  {
    fprintf(stderr, "Failed to read %s\n", name);
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool Configuration::initArgs(int argc, const char* argv[])
{
  m_continueFromLastRun = false;
  m_maxNumProcesses = 1;
  m_logLevel = LOG_MINIMAL;
  m_startTime = 0.0f;
  m_endTime = 24.0f;

  for (int iArg = 1 ; iArg < argc ; ++iArg)
  {
    if (!strcmp(argv[iArg], "-continueFromLastRun"))
    {
      m_continueFromLastRun = true;
    }
    if (!strcmp(argv[iArg], "-verbose"))
    {
      m_logLevel = LOG_VERBOSE;
    }
    else if (!strcmp(argv[iArg], "-maxNumProcesses"))
    {
      m_maxNumProcesses = (int) (atof(argv[iArg+1]) + 0.5f);
      if (m_maxNumProcesses != 1 && m_maxNumProcesses & 1)
      {
        fprintf(stderr, "maxNumProcesses must be 1 or even\n");
        exit(1);
      }
      ++iArg;
    }
    else if (!strcmp(argv[iArg], "-startTime"))
    {
      m_startTime = (float) atof(argv[iArg+1]);
      ++iArg;
    }
    else if (!strcmp(argv[iArg], "-endTime"))
    {
      m_endTime = (float) atof(argv[iArg+1]);
      ++iArg;
    }
    else if (
      !strcmp(argv[iArg], "-h") ||
      !strcmp(argv[iArg], "--h") ||
      !strcmp(argv[iArg], "-help") ||
      !strcmp(argv[iArg], "--help") ||
      !strcmp(argv[iArg], "?") ||
      !strcmp(argv[iArg], "/?") ||
      !strcmp(argv[iArg], "/h") ||
      !strcmp(argv[iArg], "/help")
      )
    {
      printf(helpText);
      exit(0);
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Configuration::loadFromDoc(NM::TiXmlDocument& doc)
{
  NM::TiXmlHandle docHandle( &doc );
  NM::TiXmlElement* elem = docHandle.FirstChild( "Configuration" ).ToElement();
  if (!elem)
  {
    fprintf(stderr, "Failed to locate configuration block\n");
    return false;
  }

  if (!readValue(elem, "RTExe", m_RTExe))
    return false;
  if (!readValue(elem, "RTArgs", m_RTArgs))
    return false;
  if (!readValue(elem, "assetCompilerExe", m_assetCompilerExe))
    return false;
  if (!readValue(elem, "assetCompilerBaseDir", m_assetCompilerBaseDir))
    return false;
  if (!readValue(elem, "assetCompilerPluginDir", m_assetCompilerPluginDir))
    return false;
  if (!readValue(elem, "assetCompilerConfigFile", m_assetCompilerConfigFile))
    return false;
  if (!readValue(elem, "connectExe", m_connectExe))
    return false;
  if (!readValue(elem, "workingDir", m_workingDir))
    return false;
  if (!readValue(elem, "network", m_network))
    return false;
  if (!readValue(elem, "projectFile", m_projectFile))
    return false;
  if (!readValue(elem, "GAPopulationFile", m_GAPopulationFile))
    return false;
  if (!readValue(elem, "RTTimeout", m_RTTimeout))
    return false;
  if (!readValue(elem, "connectTimeout", m_connectTimeout))
    return false;
  if (!readValue(elem, "assetCompilerTimeout", m_assetCompilerTimeout))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool GeneticAlgorithmParameters::load(NM::TiXmlDocument& doc)
{
  NM::TiXmlHandle docHandle( &doc );
  NM::TiXmlElement* elem = docHandle.FirstChild( "GeneticAlgorithmParameters" ).ToElement();
  if (!elem)
  {
    fprintf(stderr, "Failed to locate GA parameters block\n");
    return false;
  }

  if (!readValue(elem, "populationSize", m_populationSize))
    return false;
  if (!readValue(elem, "demeWidth", m_demeWidth))
    return false;
  if (!readValue(elem, "maxMutation", m_maxMutation))
    return false;
  if (!readValue(elem, "recombinationRate", m_recombinationRate))
    return false;
  if (!readValue(elem, "initialPopulationVariance", m_initialPopulationVariance))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TuneableValue::load(NM::TiXmlElement* elem)
{
  if (!readValue(elem, "name", m_name))
    return false;
  if (!readValue(elem, "minValue", m_minValue))
    return false;
  if (!readValue(elem, "maxValue", m_maxValue))
    return false;
  if (!readValue(elem, "defaultValue", m_defaultValue))
    return false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool NodeTypeAttributes::load(NM::TiXmlHandle handle)
{
  NM::TiXmlElement* elem = handle.ToElement();
  if (!elem)
    return false;
  if (!readValue(elem, "nodeType", m_nodeType))
    return false;

  for (int iAttribute = 0 ; ; ++iAttribute)
  {
    NM::TiXmlElement* attributeElem = handle.Child("Attribute", iAttribute).ToElement();
    if (!attributeElem)
      break;
    TuneableValue tv;
    if (!tv.load(attributeElem))
    {
      fprintf(stderr, "Failed to load Tuneable attribute\n");
      return false;
    }
    m_tuneableAttributes.push_back(tv);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Tuneables::load(NM::TiXmlDocument& doc)
{
  NM::TiXmlHandle docHandle( &doc );

  for (int iNetworkModification = 0 ; ; ++iNetworkModification)
  {
    NM::TiXmlHandle handle = docHandle.FirstChild("Tuneables").Child("NodeTypeAttributes", iNetworkModification);
    if (!handle.ToElement())
      break;
    NodeTypeAttributes nta;
    if (!nta.load(handle))
    {
      fprintf(stderr, "Failed to load Tuneable value\n");
      return false;
    }
    m_tuneableAttributes.push_back(nta);
  }

  for (int iControlParameter = 0 ; ; ++iControlParameter)
  {
    NM::TiXmlElement* controlParameterElem = docHandle.FirstChild("Tuneables").FirstChild("ControlParameters").Child("ControlParameter", iControlParameter).ToElement();
    if (!controlParameterElem)
      break;
    TuneableValue tv;
    if (!tv.load(controlParameterElem))
    {
      fprintf(stderr, "Failed to load Tuneable control parameter\n");
      return false;
    }
    m_tuneableControlParamers.push_back(tv);
  }


  return true;
}

//----------------------------------------------------------------------------------------------------------------------
size_t Tuneables::getNumTuneableAttributes() const
{
  size_t result = 0;
  for (size_t i = 0 ; i != m_tuneableAttributes.size() ; ++i)
    result += m_tuneableAttributes[i].m_tuneableAttributes.size();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const TuneableValue* Tuneables::getTuneableAttribute(size_t index) const
{
  for (size_t i = 0 ; i != m_tuneableAttributes.size() ; ++i)
  {
    if (index < m_tuneableAttributes[i].m_tuneableAttributes.size())
      return &m_tuneableAttributes[i].m_tuneableAttributes[index];
    else
      index -= m_tuneableAttributes[i].m_tuneableAttributes.size();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
size_t Tuneables::getNumTuneableControlParameters() const
{
  return m_tuneableControlParamers.size();
}

//----------------------------------------------------------------------------------------------------------------------
const TuneableValue* Tuneables::getTuneableControlParameter(size_t index) const
{
  return &m_tuneableControlParamers[index];
}

//----------------------------------------------------------------------------------------------------------------------
const TuneableValue* Tuneables::getTuneableValue(size_t index) const
{
  const size_t numTuneableAttributes = getNumTuneableAttributes();
  if (index < numTuneableAttributes)
    return getTuneableAttribute(index);
  else
    return getTuneableControlParameter(index - numTuneableAttributes);
}
