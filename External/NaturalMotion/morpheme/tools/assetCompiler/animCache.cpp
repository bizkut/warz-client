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
#include "animCache.h"
#include <sys/stat.h>
#include <time.h>
#include <algorithm>
#include "export/mcXML.h"
#include "NMPlatform/NMFile.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/mrRigToAnimMap.h"
//----------------------------------------------------------------------------------------------------------------------

#define CACHE_FILENAME "animationCache.xml"
#define CACHE_VERSION (6) ///< Change this value to treat previous any cache as 'empty'

//----------------------------------------------------------------------------------------------------------------------
// This is more accurate than calling 
int64_t getFileModificationTime(const char* fileName)
{
  HANDLE _file = CreateFileA(fileName,
    READ_CONTROL,
    0,
    NULL,
    OPEN_EXISTING,
    FILE_FLAG_BACKUP_SEMANTICS,
    NULL );

  if (_file)
  {
    FILETIME create;
    FILETIME access;
    FILETIME modify;

    if(GetFileTime( _file, &create, &access, &modify ))
    {
      ULARGE_INTEGER theTime;
      theTime.HighPart = modify.dwHighDateTime;
      theTime.LowPart = modify.dwLowDateTime;
      return theTime.QuadPart;
    }

    CloseHandle(_file);
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Load animation cache table data from file.
/// \ingroup AssetCompilerModule
bool AnimCache::create()
{
  // Do nothing for now - could check for read/write possibilities in the cache dir.
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Load animation cache table data from file.
/// \ingroup AssetCompilerModule
bool AnimCache::load()
{
  char filename[MAX_PATH];
  NMP_SPRINTF(&filename[0], MAX_PATH, "%s/%s", m_cacheDir.c_str(), CACHE_FILENAME);
  bool result = false;

  // Empty our global animation cache information map ready for initialising from file.
  clean();

  // Load the requested cache file.
  NM::TiXmlDocument* doc = new NM::TiXmlDocument();
  result = doc->LoadFile(filename);
  if (result == false)
    return false;

  ME::XMLElement* root = (ME::XMLElement*) doc->RootElement();

  // Check the root tag to ensure it's the correct file
  std::string rootTag(root->Value());
  if (rootTag != "AnimationCache")
  {
    delete(doc);
    return false;
  }

  // check the cache version
  uint32_t version = 0;
  root->getUIntAttribute("Version", version);
  if (version != CACHE_VERSION)
  {
    delete(doc);
    return false;
  }

  root->getUIntAttribute("OutputAnimIndex", m_lastCacheIndex);

  ME::XMLElement* animEntry = root->findChild("cachedAnimKey");

  while (animEntry)
  {
    // Get the key data
    Key key;
    {
      ME::XMLElement* keyEntry = animEntry->findChild("Key");

      NMP_VERIFY_MSG(keyEntry->getAttribute("SourceFilePath") != NULL, "Error SourceFilePath component from missing anim cache key (does the cache version need bumping?)");
      std::string sourceFilePath = keyEntry->getAttribute("SourceFilePath");

      NMP_VERIFY_MSG(keyEntry->getAttribute("SourceTakeName") != NULL, "Error SourceTakeName component from missing anim cache key (does the cache version need bumping?)");
      std::string sourceTakeName = keyEntry->getAttribute("SourceTakeName");

      NMP_VERIFY_MSG(keyEntry->getAttribute("AnimFormat") != NULL, "Error AnimFormat component from missing anim cache key (does the cache version need bumping?)");
      std::string animFormat = keyEntry->getAttribute("AnimFormat");

      NMP_VERIFY_MSG(keyEntry->getAttribute("AnimOptions") != NULL, "Error AnimOptions component from missing anim cache key (does the cache version need bumping?)");
      std::string animOptions = keyEntry->getAttribute("AnimOptions");

      NMP_VERIFY_MSG(keyEntry->getAttribute("AnimScale") != NULL, "Error AnimScale component from missing anim cache key (does the cache version need bumping?)");
      std::string animScale = keyEntry->getAttribute("AnimScale");

      NMP_VERIFY_MSG(keyEntry->getAttribute("Platform") != NULL, "Error Platform component from missing anim cache key (does the cache version need bumping?)");
      std::string platform = keyEntry->getAttribute("Platform");

      NMP_VERIFY_MSG(keyEntry->getAttribute("MaxSectionSize") != NULL, "Error MaxSectionSize component from missing anim cache key (does the cache version need bumping?)");
      std::string maxSectionSize = keyEntry->getAttribute("MaxSectionSize");

      // Get the rig info
      Key::NameAndGUIDVector rigs;
      ME::XMLElement* rigEntry = keyEntry->findChild("Rig");
      while (rigEntry)
      {
        std::string rigFilePath = rigEntry->getAttribute("FilePath");
        std::string rigGuid = rigEntry->getAttribute("GUID");
        rigs.push_back(Key::NameAndGUID(rigFilePath, rigGuid));

        rigEntry = keyEntry->nextChild("Rig", rigEntry);
      }

      key = Key(sourceFilePath, sourceTakeName, animFormat, animOptions, animScale, platform, rigs, maxSectionSize);
    }

    // get the entry data
    Entry* entry;
    {
      const char* filenameInCache = animEntry->getChildText("FilenameInCache");
      const char* sourceFilepath = animEntry->getChildText("SourceFilepath");
      const char* sourceTakename = animEntry->getChildText("SourceTakename");
      float compression;
      bool gotData = animEntry->getChildFloat("Compression", compression);
      if (!gotData)
      {
        compression = -1.0f;
      }
      int64_t modifyTime = _atoi64(animEntry->getChildText("InputFileModifyTime"));
      const char* crcStr = animEntry->getChildText("FileCRC");
      uint32_t fileCRC = (crcStr != NULL) ? (uint32_t)_atoi64(crcStr) : 0;

      entry = new Entry(filenameInCache, sourceFilepath, sourceTakename, compression, modifyTime, fileCRC);
    }

    // Create the new entry and add it to the map
    m_cache[key] = entry;

    // Go to next entry in XML
    animEntry = root->nextChild("cachedAnimKey", animEntry);
  }

  delete(doc);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Save animation cache table data to file.
/// \ingroup AssetCompilerModule
bool AnimCache::save()
{
  bool result = false;
  ME::XMLElement* xmlElement = new ME::XMLElement("AnimationCache");
  xmlElement->setUIntAttribute("Version", CACHE_VERSION);
  xmlElement->setUIntAttribute("OutputAnimIndex", m_lastCacheIndex);

  std::map<Key, Entry*>::iterator mapIter = m_cache.begin();
  std::map<Key, Entry*>::const_iterator mapEnd = m_cache.end();

  while (mapIter != mapEnd)
  {
    // Add this record to the XML element
    Entry* entry = (*mapIter).second;
    Key key = (*mapIter).first;

    ME::XMLElement* animEntry = (ME::XMLElement*) xmlElement->LinkEndChild(new ME::XMLElement("cachedAnimKey"));

    // Add the key data
    {
      ME::XMLElement* keyEntry = animEntry->addChild("Key");

      keyEntry->setAttribute("SourceFilePath", key.m_sourceFilepath);
      keyEntry->setAttribute("SourceTakeName", key.m_sourceTakename);
      keyEntry->setAttribute("AnimFormat", key.m_animFormat);
      keyEntry->setAttribute("AnimOptions", key.m_animOptions);
      keyEntry->setAttribute("AnimScale", key.m_animScale);
      keyEntry->setAttribute("Platform", key.m_platformFmt);
      keyEntry->setAttribute("MaxSectionSize", key.m_maxSectionSize);

      // Get the rig info
      Key::NameAndGUIDVector::const_iterator it = key.m_rigVector.begin();
      Key::NameAndGUIDVector::const_iterator end = key.m_rigVector.end();
      while (it != end)
      {
        ME::XMLElement* rigEntry = keyEntry->addChild("Rig");
        rigEntry->setAttribute("FilePath", (*it).first);
        rigEntry->setAttribute("GUID", (*it).second);
        ++it;
      }
    }

    // add the entry data
    {
      animEntry->addChildText("FilenameInCache", entry->m_filenameInCache);
      animEntry->addChildText("SourceFilepath", entry->m_sourceFilepath);
      animEntry->addChildText("SourceTakename", entry->m_sourceTakename);
      animEntry->addChildFloat("Compression", entry->m_compression);
      animEntry->addChildUInt("FileCRC", entry->getFileCRC());

      char buf[32];
      NMP_SPRINTF(buf, 32, "%I64d", entry->m_modifyTime);
      animEntry->addChildText("InputFileModifyTime", buf);
    }

    mapIter++;
  }

  char filename[MAX_PATH];
  NMP_SPRINTF(filename, MAX_PATH, "%s/%s", m_cacheDir.c_str(), CACHE_FILENAME);
  result = xmlElement->saveFile(filename);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
NMP::Memory::Resource AnimCache::getAnimation(const Key& k) const
{
  NMP::Memory::Resource result;
  result.ptr = 0;

  const Entry* e = getEntryForKey(k);
  if (e)
  {
    char filename[MAX_PATH];
    NMP_SPRINTF(filename, MAX_PATH, "%s/%s", m_cacheDir.c_str(), e->m_filenameInCache.c_str());

    void* buffer = NULL;
    int64_t bufferSize;
    uint32_t align = NMP_VECTOR_ALIGNMENT;
    int64_t bytesRead = NMP::NMFile::allocAndLoad(filename, &buffer, &bufferSize, align);
    if (bytesRead > 0)
    {
      result.ptr = buffer;
      result.format = NMP::Memory::Format((size_t)bufferSize, align);
      return result;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
std::vector<MR::RigToAnimMap*> AnimCache::getRigToAnimMaps(const Key& k) const
{
  std::vector<MR::RigToAnimMap*> result;

  const Entry* e = getEntryForKey(k);
  if (e)
  {
    for (uint32_t i = 0; i < k.m_rigVector.size(); ++i)
    {
      char cacheR2AMFilepath[MAX_PATH];
      NMP_SPRINTF(cacheR2AMFilepath, MAX_PATH, "%s/%s%u.R2A", m_cacheDir.c_str(), e->m_filenameInCache.c_str(), i);

      // load the RigToAnimMap file.
      void* buffer = NULL;
      int64_t bufferSize;
      uint32_t align = NMP_VECTOR_ALIGNMENT;
      int64_t bytesRead = NMP::NMFile::allocAndLoad(cacheR2AMFilepath, &buffer, &bufferSize, align);
      if (bytesRead > 0)
      {
        MR::RigToAnimMap* currentRigToAnimMap = (MR::RigToAnimMap*)buffer;
        currentRigToAnimMap->locate();
        result.push_back(currentRigToAnimMap);
      }
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
float AnimCache::getCompression(const Key& k) const
{
  const Entry* e = getEntryForKey(k);
  if (e)
  {
    return e->m_compression;
  }
  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Clears the currently loaded cache without removing any files on disk.
/// \ingroup AssetCompilerModule
void AnimCache::clean()
{
  // Use the animation map to delete all recorded output animation files.
  std::map<Key, Entry*>::iterator mapIter = m_cache.begin();
  const std::map<Key, Entry*>::iterator endIter = m_cache.end();
  while (mapIter != endIter)
  {
    // Delete the processed file
    Key key = mapIter->first;

    // delete key and entry
    delete((*mapIter).second);
    (*mapIter).second = NULL;

    ++mapIter;
  }

  m_cache.clear();
  m_lastCacheIndex = 0;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Clears the currently loaded cache also removing all cached anim files, rig to anim maps
/// and the animationCache.xml file from disk.
/// \ingroup AssetCompilerModule
void AnimCache::cleanAndRemoveFiles()
{
  char tempFilename[MAX_PATH];
  // Use the animation map to delete all recorded output animation files.
  std::map<Key, Entry*>::iterator mapIter = m_cache.begin();
  const std::map<Key, Entry*>::iterator endIter = m_cache.end();
  while (mapIter != endIter)
  {
    // Delete the processed file
    Key key = mapIter->first;
    Entry* entry = mapIter->second;

    // remove the cached anim file.
    NMP_SPRINTF(tempFilename, MAX_PATH, "%s/%s", m_cacheDir.c_str(), entry->m_filenameInCache.c_str());
    remove(tempFilename);

    // remove the associated cached R2A map files.
    size_t rigToAnimMapCount = key.m_rigVector.size();
    for (size_t i = 0; i != rigToAnimMapCount; ++i)
    {
      NMP_SPRINTF(tempFilename, MAX_PATH, "%s/%s%u.R2A", m_cacheDir.c_str(), entry->m_filenameInCache.c_str(), i);
      remove(tempFilename);
    }

    // delete key and entry
    delete((*mapIter).second);
    (*mapIter).second = NULL;

    ++mapIter;
  }

  NMP_SPRINTF(tempFilename, MAX_PATH, "%s/%s", m_cacheDir.c_str(), CACHE_FILENAME);
  remove(tempFilename);

  m_cache.clear();
  m_lastCacheIndex = 0;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
bool AnimCache::removePartiallyMatchingEntry(const AnimCache::Key& k)
{
  std::map<Key, Entry*>::const_iterator keyIt = m_cache.begin();
  std::map<Key, Entry*>::const_iterator keyEnd = m_cache.end();
  while (keyIt != keyEnd)
  {
    Key other = (*keyIt).first;
    // check to see if this key matches in all but rig GUIDs
    if (
      other.m_sourceFilepath == k.m_sourceFilepath &&
      other.m_sourceTakename == k.m_sourceTakename &&
      other.m_animFormat == k.m_animFormat &&
      other.m_animOptions == k.m_animOptions &&
      other.m_animScale == k.m_animScale &&
      other.m_platformFmt == k.m_platformFmt &&
      other.m_rigVector.size() == k.m_rigVector.size() &&
      other.m_maxSectionSize == k.m_maxSectionSize)
    {
      // Test the rignames
      size_t numRigs = other.m_rigVector.size();
      bool allRigsMatch = true;
      for (uint32_t i = 0; i < numRigs; i++)
      {
        if (other.m_rigVector[i].first != k.m_rigVector[i].first)
        {
          // This rig doesn't match, try next entry
          allRigsMatch = false;
          break;
        }
      }
      if (allRigsMatch)
      {
        delete m_cache[other];
        m_cache.erase(other);
        return true;
      }
    }
    ++keyIt;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief returns modifytime of the asset to be handled
/// \ingroup AssetCompilerModule
void AnimCache::addEntry(
  const Key&                            k,
  const std::vector<MR::RigToAnimMap*>& rigToAnimMaps,
  float                                 compression,
  uint32_t                              fileCRC,
  const NMP::Memory::Resource&          animResource)
{
  NMP_VERIFY_MSG(animResource.ptr, "Attempting to add a NULL anim to the cache.");

  // See if we have an entry that matches in all but rig GUIDs
  removePartiallyMatchingEntry(k);

  // Create an new entry
  char cacheFilename[MAX_PATH];
  NMP_SPRINTF(cacheFilename, MAX_PATH, "anim%u.%s", m_lastCacheIndex, k.m_animFormat.c_str());
  m_lastCacheIndex++;

  int64_t modifyTime = getFileModificationTime(k.m_sourceFilepath.c_str());

  Entry* e = new Entry(
                  cacheFilename,
                  k.m_sourceFilepath,
                  k.m_sourceTakename,
                  compression,
                  modifyTime,
                  fileCRC);
  m_cache[k] = e;

  // Write out the animation
  {
    char cacheFilepath[MAX_PATH];
    NMP_SPRINTF(cacheFilepath, MAX_PATH, "%s/%s", m_cacheDir.c_str(), e->m_filenameInCache.c_str());
    NMP::NMFile::save(cacheFilepath, animResource.ptr, animResource.format.size);
  }

  // Write all the RigToAnimMaps to the cache here.
  for (uint32_t i = 0; i < rigToAnimMaps.size(); ++i)
  {
    char cacheR2AMFilepath[MAX_PATH];
    NMP_SPRINTF(cacheR2AMFilepath, MAX_PATH, "%s/%s%u.R2A", m_cacheDir.c_str(), e->m_filenameInCache.c_str(), i);

    // Save the RigToAnimMap to file.
    MR::RigToAnimMap* currentRigToAnimMap = rigToAnimMaps[i];
    NMP_VERIFY_MSG(currentRigToAnimMap, "NULL rig to anim map passed in to AnimCache::addEntry");
    NMP::Memory::Format memReqsRTAMap = currentRigToAnimMap->getInstanceMemoryRequirements();

    uint64_t fileLength = memReqsRTAMap.size;
    currentRigToAnimMap->dislocate();
    NMP::NMFile::save(cacheR2AMFilepath, currentRigToAnimMap, fileLength);
    currentRigToAnimMap->locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
bool AnimCache::removeEntry(const AnimCache::Key& k)
{
  char tempFilename[MAX_PATH];

  std::map<Key, Entry*>::iterator it = m_cache.find(k);
  if (it != m_cache.end())
  {
    Entry* entry = it->second;

    // remove the cached anim file.
    NMP_SPRINTF(tempFilename, MAX_PATH, "%s/%s", m_cacheDir.c_str(), entry->m_filenameInCache.c_str());
    remove(tempFilename);

    // remove the associated cached R2A map files.
    size_t rigToAnimMapCount = k.m_rigVector.size();
    for (size_t i = 0; i != rigToAnimMapCount; ++i)
    {
      NMP_SPRINTF(tempFilename, MAX_PATH, "%s/%s%u.R2A", m_cacheDir.c_str(), entry->m_filenameInCache.c_str(), i);
      remove(tempFilename);
    }

    // delete key and entry
    delete entry;
    m_cache.erase(it);

    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief This will get associated entry for the given key.
/// \ingroup AssetCompilerModule
///
/// The key can optionally contain rig GUIDS. If these are not present then they will not be checked when matching.
const AnimCache::Entry* AnimCache::getEntryForKey(const AnimCache::Key& k) const
{
  std::map<Key, Entry*>::const_iterator it;
  it = m_cache.find(k);
  if (it != m_cache.end())
  {
    return (*it).second;
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
AnimCache::Entry* AnimCache::getEntryForKey(const AnimCache::Key& k)
{
  // TODO: Store the GUIDS as part of the key, but only use it for validity checking, not for getting entries
  std::map<Key, Entry*>::iterator it;
  it = m_cache.find(k);
  if (it != m_cache.end())
  {
    return (*it).second;
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimCache::Key
//----------------------------------------------------------------------------------------------------------------------
/// \class Key
/// \brief Can optionally contain GUIDs, if not then partial matches are possible.
/// \ingroup AssetCompilerModule
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
AnimCache::Key::Key(
  const std::string&       sourceFilepath,
  const std::string&       sourceTakename,
  const std::string&       animFormat,
  const std::string&       animOptions,
  const std::string&       animScale,
  const NameAndGUIDVector& rigNameGuidsVector,
  const std::string&       maxSectionSize) :
    m_sourceFilepath(sourceFilepath),
    m_sourceTakename(sourceTakename),
    m_animFormat(animFormat),
    m_animOptions(animOptions),
    m_animScale(animScale),
    m_platformFmt(NM_PLATFORM_FORMAT_STRING),
    m_rigVector(rigNameGuidsVector),
    m_maxSectionSize(maxSectionSize)
{
  m_comparisonString = getComparisonString();
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
AnimCache::Key::Key(
  const std::string&       sourceFilepath,
  const std::string&       sourceTakename,
  const std::string&       animFormat,
  const std::string&       animOptions,
  const std::string&       animScale,
  const std::string&       platformFmt,
  const NameAndGUIDVector& rigNameGuidsVector,
  const std::string&       maxSectionSize) :
    m_sourceFilepath(sourceFilepath),
    m_sourceTakename(sourceTakename),
    m_animFormat(animFormat),
    m_animOptions(animOptions),
    m_animScale(animScale),
    m_platformFmt(platformFmt),
    m_rigVector(rigNameGuidsVector),
    m_maxSectionSize(maxSectionSize)
{
  m_comparisonString = getComparisonString();
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// \ingroup AssetCompilerModule
std::string AnimCache::Key::getComparisonString() const
{
  std::string result = "";
  result += m_sourceFilepath + "|";
  result += m_sourceTakename + "|";
  result += m_animFormat + "|";
  result += m_animOptions + "|";
  result += m_animScale + "|";
  result += m_platformFmt + "|";
  result += m_maxSectionSize;
    
  NameAndGUIDVector::const_iterator it = m_rigVector.begin();
  NameAndGUIDVector::const_iterator end = m_rigVector.end();
  while (it != end)
  {
    result += "|";
    result += it->first;
    result += "|";
    result += it->second;
    ++it;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimCache::Entry
//----------------------------------------------------------------------------------------------------------------------
// Work out if the Entry is still valid, or if one of the source files has changed since it was generated.
// Returns true if the entry is up to date
bool AnimCache::Entry::isUpToDate() const
{
  // Get the modify date for the input XMD file
  int64_t modifyTime = getFileModificationTime(m_sourceFilepath.c_str());
  if (modifyTime == 0)
  {
    // Anim not source doesn't exist
    return false;
  }

  // Anim not up to date:- anim compile times are not the same.
  return modifyTime == m_modifyTime;
}

//----------------------------------------------------------------------------------------------------------------------
