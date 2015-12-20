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
#include <string.h>
#include <stdarg.h>
#include <algorithm>
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "NMTinyXML.h"
#include "export/mcAnimInfo.h"
#include "export/mcAnimInfoXML.h"

#if defined(_MSC_VER)
  #pragma warning(disable:4996)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
#define BuildNodeList(_T, vec, xmlElement, tagName) \
{ \
  XMLElement *it = xmlElement->findChild(tagName); \
  vec.clear(); \
  while (it != 0) \
  { \
    _T *obj = new _T(it); \
    vec.push_back(obj); \
    it = xmlElement->nextChild(tagName, it); \
  } \
}

//----------------------------------------------------------------------------------------------------------------------
#define BuildNodeMap(_T, theMap, xmlElement, tagName) \
{ \
  XMLElement *it = xmlElement->findChild(tagName); \
  theMap.clear(); \
  while (it != 0) \
  { \
    _T *obj = new _T(it); \
    theMap[obj->getName()] = obj; \
    it = xmlElement->nextChild(tagName, it); \
  } \
}

//----------------------------------------------------------------------------------------------------------------------
char* createConsistentAnimName(const char* animName)
{
  if (animName)
  {
    size_t length = strlen(animName);
    char* result = new char[length + 1];

    for (size_t i = 0 ; i < length ; ++i)
    {
      result[i] = animName[i] == '/' ? '\\' : animName[i];
    }
    result[length] = 0;
    strlwr(result);

    return result;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimTakeXML
//----------------------------------------------------------------------------------------------------------------------
AnimTakeXML::AnimTakeXML(XMLElement* xmlElement) :
  m_xmlElement(xmlElement)
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimTakeXML::AnimTakeXML(
  float       duration,
  float       fps,
  const char* name) :
  m_xmlElement(0)
{
  m_xmlElement = new XMLElement("AnimTakeXML");
  m_xmlElement->setFloatAttribute("duration", duration);
  m_xmlElement->setFloatAttribute("fps", fps);
  m_xmlElement->setAttribute("name", name);
}

//----------------------------------------------------------------------------------------------------------------------
float AnimTakeXML::getDuration() const
{
  float result;
  m_xmlElement->getFloatAttribute("duration", result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimTakeXML::getName() const
{
  const char* result = m_xmlElement->getAttribute("name");
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float AnimTakeXML::getFps() const
{
  float result;
  m_xmlElement->getFloatAttribute("fps", result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnimTakeXML::~AnimTakeXML()
{
}

//----------------------------------------------------------------------------------------------------------------------
// AnimInfoXML
//----------------------------------------------------------------------------------------------------------------------
AnimInfoXML::AnimInfoXML(XMLElement* xmlElement) :
  m_xmlElement(xmlElement),
  m_cachedname(NULL)
{
  BuildNodeList(AnimTakeXML, m_takes, m_xmlElement, "AnimTakeXML");
}

//----------------------------------------------------------------------------------------------------------------------
AnimInfoXML::AnimInfoXML(const char* filename) :
  m_cachedname(NULL)
{
  m_xmlElement = new XMLElement("AnimInfoXML");
  m_xmlElement->setAttribute("animName", filename);
}

//----------------------------------------------------------------------------------------------------------------------
AnimInfoXML::~AnimInfoXML()
{
  delete [] m_cachedname;
  for (TakeNameList::iterator it = m_takes.begin(); it != m_takes.end(); ++it)
  {
    delete *it;
  }
}

//----------------------------------------------------------------------------------------------------------------------
int64_t AnimInfoXML::getValidDate() const
{
  int64_t result = 0xffffffffffffffff;
  m_xmlElement->getInt64Attribute("validDate", result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimInfoXML::getName() const
{
  if (m_cachedname == NULL)
  {
    m_cachedname = createConsistentAnimName(m_xmlElement->getAttribute("animName"));
  }

  return m_cachedname;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimInfoXML::getNumTakes() const
{
  return (unsigned int)m_takes.size();
}

//----------------------------------------------------------------------------------------------------------------------
const AnimTake* AnimInfoXML::getTake(uint32_t index) const
{
  unsigned int numTakes = getNumTakes();
  if (index >= numTakes)
  {
    return 0;
  }

  const AnimTakeXML* result = m_takes.at(index);
  return (const AnimTake*)result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimInfoXML::setValidDate(int64_t validDate) const
{
  m_xmlElement->setUInt64Attribute("validDate", validDate);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimInfoXML::clearTakes()
{
  m_takes.clear();

  m_xmlElement->Clear();

  return;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimInfoXML::addTake(const char* name, float duration, float fps)
{
  AnimTakeXML* animTake = new AnimTakeXML(duration, fps, name);

  m_takes.push_back(animTake);

  m_xmlElement->LinkEndChild(animTake->m_xmlElement);

  return;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimInfoXML::write()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimInfoListXML
//----------------------------------------------------------------------------------------------------------------------
AnimInfoListXML::AnimInfoListXML(XMLElement* xmlElement) :
  m_xmlElement(xmlElement)
{
  BuildNodeMap(AnimInfoXML, m_animInfos, m_xmlElement, "AnimInfoXML");
}

//----------------------------------------------------------------------------------------------------------------------
AnimInfoListXML::AnimInfoListXML()
{
  m_xmlElement = new XMLElement("AnimInfoList");
}

//----------------------------------------------------------------------------------------------------------------------
AnimInfoListXML::~AnimInfoListXML()
{
  for (AnimInfoXMLMap::iterator it = m_animInfos.begin(); it != m_animInfos.end(); ++it)
  {
    delete it->second;
  }
  delete m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
AnimInfoXML* AnimInfoListXML::createAnimInfo(const char* filename)
{
  NMP_ASSERT(getAnimInfo(filename) == NULL);

  AnimInfoXML* animInfo = new AnimInfoXML(filename);
  m_animInfos[animInfo->getName()] = animInfo;
  m_xmlElement->LinkEndChild(animInfo->m_xmlElement);

  // Clear the cache it will be rebuilt when needed
  m_animInfosCache.clear();

  return animInfo;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimInfoListXML::getNumAnimInfos() const
{
  unsigned int result = (uint32_t)m_animInfos.size();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnimInfo* AnimInfoListXML::getAnimInfo(unsigned int index)
{
  // if the cache is empty then rebuild it
  if (m_animInfosCache.empty())
  {
    m_animInfosCache.reserve(m_animInfos.size());
    for (AnimInfoXMLMap::const_iterator it = m_animInfos.begin(); it != m_animInfos.end(); ++it)
    {
      m_animInfosCache.push_back(it->second);
    }
  }
  unsigned int numAnimInfos = getNumAnimInfos();
  if (index >= numAnimInfos)
  {
    return 0;
  }

  return m_animInfosCache[index];
}

//----------------------------------------------------------------------------------------------------------------------
const AnimInfo* AnimInfoListXML::getAnimInfo(unsigned int index) const
{
  // if the cache is empty then rebuild it
  if (m_animInfosCache.empty())
  {
    m_animInfosCache.reserve(m_animInfos.size());
    for (AnimInfoXMLMap::const_iterator it = m_animInfos.begin(); it != m_animInfos.end(); ++it)
    {
      m_animInfosCache.push_back(it->second);
    }
  }
  unsigned int numAnimInfos = getNumAnimInfos();
  if (index >= numAnimInfos)
  {
    return 0;
  }

  return m_animInfosCache[index];
}

//----------------------------------------------------------------------------------------------------------------------
AnimInfo* AnimInfoListXML::getAnimInfo(const char* animName)
{
  const char* consistentMame = createConsistentAnimName(animName);
  AnimInfoXMLMap::iterator it = m_animInfos.find(consistentMame);
  AnimInfo* result = it == m_animInfos.end() ? NULL : it->second;
  delete [] consistentMame;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const AnimInfo* AnimInfoListXML::getAnimInfo(const char* animName) const
{
  const char* consistentMame = createConsistentAnimName(animName);
  AnimInfoXMLMap::const_iterator it = m_animInfos.find(consistentMame);
  AnimInfo* result = it == m_animInfos.end() ? NULL : it->second;
  delete [] consistentMame;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimInfoListXML::removeAnimInfo(AnimInfo* animInfo)
{
  // remove from the XML
  m_xmlElement->RemoveChild(((AnimInfoXML*)animInfo)->m_xmlElement);

  AnimInfoXMLMap::iterator it = m_animInfos.find(animInfo->getName());
  if (it != m_animInfos.end())
  {
    m_animInfos.erase(it);
  }
  m_animInfosCache.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimInfoListXML::replaceAnimInfo(const AnimInfo* animInfo)
{
  _ASSERT(animInfo);

  AnimInfo* oldInfo = getAnimInfo(animInfo->getName());
  if (oldInfo)
  {
    removeAnimInfo(oldInfo);
    delete oldInfo;
  }

  NMP_ASSERT(getAnimInfo(animInfo->getName()) == NULL);

  const AnimInfoXML* animInfoxml = static_cast<const AnimInfoXML*>(animInfo);
  NM::TiXmlNode* newChild = m_xmlElement->InsertEndChild(*animInfoxml->m_xmlElement);
  AnimInfoXML* newElement = new AnimInfoXML(static_cast<XMLElement*>(newChild));

  // insert this single element in order
  m_animInfos[newElement->getName()] = newElement;
  m_animInfosCache.clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimInfoListXML::write(const char* fileName, int* error)
{
  return m_xmlElement->saveFile(fileName, error);
}

//----------------------------------------------------------------------------------------------------------------------
// AnimInfoFactoryXML
//----------------------------------------------------------------------------------------------------------------------
AnimInfoFactoryXML::AnimInfoFactoryXML()
{

}

//----------------------------------------------------------------------------------------------------------------------
AnimInfoFactoryXML::~AnimInfoFactoryXML()
{
  std::map<std::string, XMLAnimInfoListAsset>::iterator assetIt = m_fileAssetMap.begin();

  while (assetIt != m_fileAssetMap.end())
  {
    NM::TiXmlDocument* doc = (*assetIt).second.m_document;
    doc->UnlinkChild(doc->RootElement());
    delete doc;
    delete (*assetIt).second.m_asset;

    ++assetIt;
  }

  m_fileAssetMap.clear();
}

//----------------------------------------------------------------------------------------------------------------------
AnimInfoListXML* AnimInfoFactoryXML::createAnimInfoList()
{
  AnimInfoListXML* animInfoList = new AnimInfoListXML();

  return animInfoList;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimInfoFactoryXML::destroyAnimInfoList(AnimInfoListXML* animList)
{
  delete animList;
}

//----------------------------------------------------------------------------------------------------------------------
AnimInfoList* AnimInfoFactoryXML::loadAnimInfoList(const char* srcFilename)
{
  size_t xmlFileSize = 0;

  // Load source xml
  const char* animInfoSourceXML = doLoadAnimInfoList(srcFilename, xmlFileSize);
  if (animInfoSourceXML == 0 || xmlFileSize == 0)
  {
    return 0;
  }

  // Parse the xml
  NM::TiXmlDocument* doc = new NM::TiXmlDocument();
  doc->Parse((const char*)animInfoSourceXML);

  // Free the source data
  doFreeAnimInfoList(animInfoSourceXML);

  // Check the root tag:
  const char* rootTag = "";
  XMLElement* root = (XMLElement*)doc->RootElement();
  AnimInfoList* result = 0;

  if (root != 0)
  {
    rootTag = root->Value();
  }

  if (strcmp(rootTag, "AnimInfoList") == 0)
  {
    result = new AnimInfoListXML(root);

    XMLAnimInfoListAsset xmlAsset;
    xmlAsset.m_document = doc;
    xmlAsset.m_asset = result;
    m_fileAssetMap.insert(std::pair<std::string, XMLAnimInfoListAsset>(std::string(srcFilename), xmlAsset));
  }
  else
  {
    // We got the wrong tag
    delete doc;

    return 0;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimInfoFactoryXML::unloadAnimInfoList(const char* srcFileName)
{
  std::string fileStr(srcFileName);

  // Is the info list already loaded?
  std::map<std::string, XMLAnimInfoListAsset>::iterator assetIt = m_fileAssetMap.find(fileStr);
  if (assetIt != m_fileAssetMap.end())
  {
    NM::TiXmlDocument* doc = (*assetIt).second.m_document;
    doc->UnlinkChild(doc->RootElement());
    delete doc;
    delete (*assetIt).second.m_asset;

    m_fileAssetMap.erase(assetIt);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimInfoFactoryXML::doLoadAnimInfoList(const char* srcFilename, size_t& dataSize)
{
  FILE* inFile = fopen(srcFilename, "rb");
  if (inFile == 0)
  {
    return 0;
  }

  fseek(inFile, 0, SEEK_END);

  char* textBuffer = 0;
  dataSize = (size_t)ftell(inFile);

  if (dataSize >= 0)
  {
    textBuffer = (char*)NMPMemoryAllocAligned(dataSize + 1, 4);
    if (textBuffer != 0)
    {
      fseek(inFile, 0, SEEK_SET);
#ifdef NMP_ENABLE_ASSERTS
      size_t bytes =
#endif
        fread(textBuffer, 1, (size_t)dataSize, inFile);
      NMP_ASSERT(bytes == dataSize);
      textBuffer[dataSize] = 0; // null terminate
    }
  }

  fclose(inFile);
  return textBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimInfoFactoryXML::doFreeAnimInfoList(const char* animList)
{
  NMP::Memory::memFree((void*)animList);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME
