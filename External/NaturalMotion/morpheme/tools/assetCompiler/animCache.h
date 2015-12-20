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
#ifndef ANIM_CACHE_H
#define ANIM_CACHE_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "export/mcExportXml.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
class AnimSourceBase;
class RigToAnimMap;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class Anim cache
/// \brief
/// \ingroup AssetCompilerModule
class AnimCache
{
public:
  AnimCache(const char* cacheDir) : m_cacheDir(cacheDir), m_lastCacheIndex(0) {}

  /// \class Key
  /// \brief This class acts as a key for a given entry in the cache.
  /// \ingroup AssetCompilerModule
  class Key
  {
  public:
    typedef std::pair<std::string, std::string> NameAndGUID;
    typedef std::vector<NameAndGUID> NameAndGUIDVector;

    Key(
      const std::string&       sourceFilepath,
      const std::string&       sourceTakename,
      const std::string&       animFormat,
      const std::string&       animOptions,
      const std::string&       animScale,
      const NameAndGUIDVector& rigVector,
      const std::string&       maxSectionSize);

    // This function is required for built-in STL list functions like sort.
    bool operator<(const Key& rhs) const
    {
      return (strcmp(this->m_comparisonString.c_str(), rhs.m_comparisonString.c_str()) < 0);
    }

  protected:
    // Make this a friend of the anim cache.
    friend AnimCache;

    // Only allow specific setting of the platform internally (i.e. when loading a cache).
    Key(
      const std::string&       sourceFilepath,
      const std::string&       sourceTakename,
      const std::string&       animFormat,
      const std::string&       animOptions,
      const std::string&       animScale,
      const std::string&       platformFmt,
      const NameAndGUIDVector& rigVector,
      const std::string&       maxSectionSize);

    // Allow an internal default constructor.
    Key() {}

    std::string getComparisonString() const;

    std::string m_sourceFilepath;
    std::string m_sourceTakename;
    std::string m_animFormat;
    std::string m_animOptions;
    std::string m_animScale;
    std::string m_platformFmt;
    std::string m_maxSectionSize;

    NameAndGUIDVector m_rigVector;
    std::string       m_comparisonString;
  };

  /// \class Entry
  /// \brief This structure holds the cached information about a previously-processed animation in the processed animation cache.
  /// \ingroup AssetCompilerModule
  class Entry
  {
  public:
    Entry(
      const std::string& filenameInCache,
      const std::string& sourceFilepath,
      const std::string& sourceTakename,
      float              compression,
      int64_t            modifyTime,
      uint32_t           fileCRC) :
        m_filenameInCache(filenameInCache),
        m_sourceFilepath(sourceFilepath),
        m_sourceTakename(sourceTakename),
        m_compression(compression),
        m_modifyTime(modifyTime),
        m_animFileCRC(fileCRC)
    {
    }

    bool isUpToDate() const;
    NM_INLINE uint32_t getFileCRC() const { return m_animFileCRC; }

  protected:
    // Make this a friend of the anim cache
    friend AnimCache;

    std::string    m_sourceFilepath;    ///< Name of source file
    std::string    m_sourceTakename;    ///< Take in source file for this entry
    std::string    m_filenameInCache;   ///< Name of compiled anim in cache.
    int64_t        m_modifyTime;        ///< The last modified time for cache file.
    uint32_t       m_animFileCRC;       ///< The cyclic redundancy check identifier of the binary file
    float          m_compression;       ///< The number of bytes per bone per second of animation.
  };

  bool create();
  bool load();
  bool save();

  void addEntry(
    const Key&                            k,
    const std::vector<MR::RigToAnimMap*>& rigToAnimMaps,
    float                                 compression,
    uint32_t                              fileCRC,
    const NMP::Memory::Resource&          resource);

  bool removeEntry(const Key& k);
  void clean();
  void cleanAndRemoveFiles();

  const Entry* getEntryForKey(const Key& k) const;

  NMP::Memory::Resource getAnimation(const Key& k) const;
  std::vector<MR::RigToAnimMap*> getRigToAnimMaps(const Key& k) const;
  float getCompression(const Key& k) const;

protected:

  Entry* getEntryForKey(const Key& k);
  bool removePartiallyMatchingEntry(const Key& k);

  std::string m_cacheDir;
  std::map<Key, Entry*> m_cache;
  uint32_t m_lastCacheIndex;

private:
};

int64_t getFileModificationTime(const char* fileName);

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_FLAGS_H
//----------------------------------------------------------------------------------------------------------------------
