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
// Suppress warning coming from xstring in PS3 SDK.
#ifdef NM_COMPILER_SNC
#pragma diag_push
#pragma diag_suppress=1669
#endif

#include <string.h>

#ifdef NM_COMPILER_SNC
#pragma diag_pop
#endif

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMFile.h"
#include "morpheme/mrManager.h"

#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookupBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

namespace UTILS
{

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimRuntimeIDtoFilenameLookupBuilder::SimpleAnimRuntimeIDtoFilenameLookupBuilder()
{
  clearAnimLookupAssetID();
}

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimRuntimeIDtoFilenameLookupBuilder::~SimpleAnimRuntimeIDtoFilenameLookupBuilder()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleAnimRuntimeIDtoFilenameLookupBuilder::addAnimation(
  const char* animFileName,
  const char* animTakeName,
  const char* animFormatType,
  uint32_t    runtimeID,
  uint32_t    animCRC,
  const char* outputFilename)
{
  uint8_t animFormatIndex;
  bool status = MR::Manager::getInstance().findAnimationFormatRegistryEntryIndex(animFormatType, animFormatIndex);
  NMP_ASSERT(status);
  if (!status)
    return false;

  AnimEntry* e = new AnimEntry();
  e->animID = runtimeID;
  e->animCRC = animCRC;

  e->animFilename = (char*)NMPMemoryAlloc(strlen(outputFilename) + 1);
  NMP_ASSERT(e->animFilename);
  NMP_STRNCPY_S(e->animFilename, strlen(outputFilename) + 1, outputFilename);

  e->animFormat = (char*)NMPMemoryAlloc(strlen(animFormatType) + 1);
  NMP_ASSERT(e->animFormat);
  NMP_STRNCPY_S(e->animFormat, strlen(animFormatType) + 1, animFormatType);

  e->sourceFilename = (char*)NMPMemoryAlloc(strlen(animFileName) + 1);
  NMP_ASSERT(e->sourceFilename);
  NMP_STRNCPY_S(e->sourceFilename, strlen(animFileName) + 1, animFileName);

  e->sourceTakename = (char*)NMPMemoryAlloc(strlen(animTakeName) + 1);
  NMP_ASSERT(e->sourceTakename);
  NMP_STRNCPY_S(e->sourceTakename, strlen(animTakeName) + 1, animTakeName);

  m_entries.push_back(e);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
SimpleAnimRuntimeIDtoFilenameLookup* SimpleAnimRuntimeIDtoFilenameLookupBuilder::generateAnimRuntimeIDtoFilenameLookup()
{
  // construct an array of runtime IDs and strings
  uint32_t numAnims = (uint32_t)m_entries.size();
  uint32_t* crcs = (uint32_t*)NMPMemoryAlloc(numAnims * sizeof(uint32_t));
  NMP_ASSERT(crcs);
  char** fileNames = (char**)NMPMemoryAlloc(numAnims * sizeof(char*));
  NMP_ASSERT(fileNames);
  char** animFormats = (char**)NMPMemoryAlloc(numAnims * sizeof(char*));
  NMP_ASSERT(animFormats);
  char** sourceFilenames = (char**)NMPMemoryAlloc(numAnims * sizeof(char*));
  NMP_ASSERT(sourceFilenames);
  char** sourceTakenames = (char**)NMPMemoryAlloc(numAnims * sizeof(char*));
  NMP_ASSERT(sourceTakenames);

  for (uint32_t i = 0; i < numAnims; i++)
  {
    NMP_ASSERT(m_entries[i]->animID == i);
    crcs[i]            = m_entries[i]->animCRC;
    fileNames[i]       = m_entries[i]->animFilename;
    animFormats[i]     = m_entries[i]->animFormat;
    sourceFilenames[i] = m_entries[i]->sourceFilename;
    sourceTakenames[i] = m_entries[i]->sourceTakename;
  }

  SimpleAnimRuntimeIDtoFilenameLookup* result;
  NMP::Memory::Format f = SimpleAnimRuntimeIDtoFilenameLookup::getMemoryRequirements(
                            (uint32_t)m_entries.size(),
                            fileNames,
                            animFormats,
                            sourceFilenames,
                            sourceTakenames);
  NMP::Memory::Resource r = NMPMemoryAllocateFromFormat(f);

  result = SimpleAnimRuntimeIDtoFilenameLookup::init(
             r,
             (uint32_t)m_entries.size(),
             crcs,
             fileNames,
             animFormats,
             sourceFilenames,
             sourceTakenames);

  NMP::Memory::memFree(crcs);
  NMP::Memory::memFree(fileNames);
  NMP::Memory::memFree(animFormats);
  NMP::Memory::memFree(sourceFilenames);
  NMP::Memory::memFree(sourceTakenames);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimRuntimeIDtoFilenameLookupBuilder::registerAnimLookupAssetID(
  const char* animLookupName,
  const char* animLookupGUID,
  uint32_t animLookupAssetID)
{
  NMP_STRNCPY_S(m_animLookupName, 256, animLookupName);
  NMP_STRNCPY_S(m_animLookupGUID, 256, animLookupGUID);
  m_animLookupAssetID = animLookupAssetID;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimRuntimeIDtoFilenameLookupBuilder::clearAnimLookupAssetID()
{
  NMP_STRNCPY_S(m_animLookupName, 256, "");
  NMP_STRNCPY_S(m_animLookupGUID, 256, "");
  m_animLookupAssetID = 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleAnimRuntimeIDtoFilenameLookupBuilder::clear()
{
  std::vector<AnimEntry*>::iterator it;
  for (it = m_entries.begin(); it != m_entries.end(); ++it)
  {
    NMP::Memory::memFree((*it)->animFilename);
    NMP::Memory::memFree((*it)->animFormat);
    NMP::Memory::memFree((*it)->sourceFilename);
    NMP::Memory::memFree((*it)->sourceTakename);
    delete (*it);
  }
  m_entries.clear();

  clearAnimLookupAssetID();
}

} // namespace UTILS

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
