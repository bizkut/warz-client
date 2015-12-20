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
#ifndef MR_SIMPLE_ANIM_RUNTIMEID_TO_FILENAME_LOOKUP_BUILDER_H
#define MR_SIMPLE_ANIM_RUNTIMEID_TO_FILENAME_LOOKUP_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include "simpleAnimRuntimeIDtoFilenameLookup.h"
//----------------------------------------------------------------------------------------------------------------------

// Macros for generating RuntimeAnimAssetID identifiers
//#define MR_MAKE_ANIM_ASSET_ID(animFormatIndex, animAssetIndex) ((animFormatIndex << 16) + animAssetIndex)
//#define MR_GET_ANIM_FORMAT_INDEX(animAssetID) ((animAssetID >> 16) & 0x0000ffff)
//#define MR_GET_ANIM_ASSET_INDEX(animAssetID) (animAssetID & 0x0000ffff)

namespace MR
{

namespace UTILS
{

class SimpleAnimRuntimeIDtoFilenameLookupBuilder
{
public:
  struct AnimEntry
  {
    uint32_t animID;
    uint32_t animCRC;
    char* animFilename;
    char* animFormat;
    char* sourceFilename;
    char* sourceTakename;
  };

  SimpleAnimRuntimeIDtoFilenameLookupBuilder();

  ~SimpleAnimRuntimeIDtoFilenameLookupBuilder();

  bool addAnimation(
    const char* animFilePath,
    const char* takeName,
    const char* animFormat,
    uint32_t    runtimeID,
    uint32_t    animCRC,
    const char* outputFilename);

  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* generateAnimRuntimeIDtoFilenameLookup();

  // Processed asset functions
  void registerAnimLookupAssetID(
    const char* animLookupName,
    const char* animLookupGUID,
    uint32_t animLookupAssetID);

  void clearAnimLookupAssetID();
  const char* getAnimLookupName() const { return m_animLookupName; }
  const char* getAnimLookupGUID() const { return m_animLookupGUID; }
  uint32_t getAnimLookupAssetID() const { return m_animLookupAssetID; }

  void clear();

  const AnimEntry* getEntry(uint32_t runtimeID)
  {
    std::vector<AnimEntry*>::iterator it;
    for (it = m_entries.begin(); it != m_entries.end(); ++it)
    {
      if ((*it)->animID == runtimeID)
        return (*it);
    }
    return 0;
  }

  const std::vector<AnimEntry*>& getEntries() const { return m_entries; }

protected:

  std::vector<AnimEntry*> m_entries;

  // Processed asset information
  char m_animLookupName[256];
  char m_animLookupGUID[256];
  uint32_t m_animLookupAssetID;
};

} // namespace UTILS

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_SIMPLE_ANIM_RUNTIMEID_TO_FILENAME_LOOKUP_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
