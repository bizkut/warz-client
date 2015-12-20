// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/GameAnimModule.h"
#include "GameManagement/GameCharacterManager.h"

#include <algorithm>

// PIX CAPTURE
#define NM_RTT_PIX_CAPTUREx
#if defined(NM_HOST_X360) && defined(PROFILE) && defined(NM_RTT_PIX_CAPTURE)
  #include "Tracerecording.h"
  const uint32_t PIX_CAPTURE_FRAME_COUNT = 300;
  static bool s_pixCapture = false;
#endif // NM_HOST_X360
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterManager::CharacterManager()
{
  m_characterDefList.clear();
  m_characterList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterManager::~CharacterManager()
{
  NMP_ASSERT((m_characterDefList.size() == 0) && (m_characterList.size() == 0));
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManager::init()
{
  // We must not have already been initialised.
  NMP_ASSERT((m_characterDefList.size() == 0) && (m_characterList.size() == 0));
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManager::term()
{
  // Destroy all Characters.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    (*iter)->destroy();
    (*iter) = NULL;
  }
  m_characterList.clear();

  // Destroy all CharacterDefs; unloading their animations and deleting them.
  for (CharacterDefList::iterator iter = m_characterDefList.begin(); iter != m_characterDefList.end(); ++iter)
  {
    CharacterDef::destroy(*iter);
    (*iter) = NULL;
  }
  m_characterDefList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterManager::createCharacterDef(
  const char*          filename,
  ValidatePluginListFn validatePluginList)
{
  Game::CharacterDef* gameCharacterDef = Game::CharacterDef::create(filename, validatePluginList);
  if (!gameCharacterDef || !gameCharacterDef->isLoaded())
  {
    NMP_STDOUT("gameCharacterDef %s was not loaded successfully.", filename);
    NMP_ASSERT_FAIL();
    return NULL;
  }
  registerCharacterDef(gameCharacterDef); // The management of this gameCharacterDefs memory is this CharacterManagers responsibility.
  return gameCharacterDef;
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterManager::createCharacterDef(
  void*                bundle,
  size_t               bundleSize,
  ValidatePluginListFn validatePluginList)
{
  Game::CharacterDef* gameCharacterDef = Game::CharacterDef::create(bundle, bundleSize, validatePluginList);
  if (!gameCharacterDef || !gameCharacterDef->isLoaded())
  {
    NMP_STDOUT("gameCharacterDef was not loaded successfully from a memory location.");
    NMP_ASSERT_FAIL();
    return NULL;
  }
  registerCharacterDef(gameCharacterDef); // The management of this gameCharacterDefs memory is this CharacterManagers responsibility.
  return gameCharacterDef;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManager::destroyCharacterDef(CharacterDef* characterDef)
{
  characterDef->decRefCount();

  if(characterDef->getRefCount() == 0)
  {
    // Remove it from the registered list.
    unregisterCharacterDef(characterDef);

    // Destroy the CharacterDef itself.
    CharacterDef::destroy(characterDef); 
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManager::destroyCharacter(Character* character)
{
  NMP_ASSERT(character);
  // Remove it from the registered list.
  unregisterCharacter(character);
  
  // Destroy the Character itself.
  character->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManager::registerCharacterDef(CharacterDef* characterDef)
{
  NMP_ASSERT(characterDef);
  // Make sure the CharacterDef is not already registered.
  CharacterDefList::iterator iter = std::find(m_characterDefList.begin(), m_characterDefList.end(), characterDef);
  NMP_ASSERT(iter == m_characterDefList.end());

  // Add to the list.
  m_characterDefList.push_back(characterDef);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManager::unregisterCharacterDef(CharacterDef* characterDef)
{
  NMP_ASSERT(characterDef);
  // Find the CharacterDef in the list.
  CharacterDefList::iterator iter = std::find(m_characterDefList.begin(), m_characterDefList.end(), characterDef);
  NMP_ASSERT((iter != m_characterDefList.end()) && ((*iter) == characterDef)); // Make sure the CharacterDef is in the registered list.
 
  // Remove the entry from the list; we do not destroy the CharacterDef itself.
  m_characterDefList.erase(iter);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManager::registerCharacter(Character* character)
{
  NMP_ASSERT(character);
  // Make sure the Character is not already registered.
  CharacterList::iterator iter = std::find(m_characterList.begin(), m_characterList.end(), character);
  NMP_ASSERT(iter == m_characterList.end());

  // Add to the list.
  m_characterList.push_back(character);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManager::unregisterCharacter(Character* character)
{
  NMP_ASSERT(character);
  // Find the Character in the list.
  CharacterList::iterator iter = std::find(m_characterList.begin(), m_characterList.end(), character);
  NMP_ASSERT((iter != m_characterList.end()) && ((*iter) == character)); // Make sure the CharacterDef is in the registered list.

  // Remove the entry from the list; we do not destroy the Character itself.
  m_characterList.erase(iter);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterManager::findCharacterDefByGUID(const uint8_t guid[16])
{
  // Step through our list of CharacterDefs.
  for (CharacterDefList::iterator iter = m_characterDefList.begin(); iter != m_characterDefList.end(); ++iter)
  {
    CharacterDef* currentDef = *iter;
    const uint8_t* currentGUID = currentDef->getNetworkDefGuid();

    // Compare the 16 byte guid values.
    bool found = true;
    for (uint32_t i = 0; i < 16; ++i)
    {
      if (guid[i] != currentGUID[i])
      {
        found = false;
        break;
      }
    }
    if (found)
    {
      // GUIDs are the same.
      return currentDef;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterManager::isCharacterRegistered(Character* character) const
{
  NMP_ASSERT(character);
  CharacterList::const_iterator iter = std::find(m_characterList.begin(), m_characterList.end(), character);
  return ((iter != m_characterList.end()) && ((*iter) == character));
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterManager::isCharacterDefRegistered(CharacterDef* characterDef) const
{
  NMP_ASSERT(characterDef);
  CharacterDefList::const_iterator iter = std::find(m_characterDefList.begin(), m_characterDefList.end(), characterDef);
  return ((iter != m_characterDefList.end()) && ((*iter) == characterDef));
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterManager::getCharacterDef(uint32_t index)
{
  if (m_characterDefList.size() > index)
  {
    CharacterDefList::iterator iter = m_characterDefList.begin();
    for (uint32_t i = 0; i < index; ++i)
    {
      ++iter;
    }
    return (*iter);
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
Character* CharacterManager::getCharacter(uint32_t index)
{
  if (m_characterList.size() > index)
  {
    CharacterList::iterator iter = m_characterList.begin();
    for (uint32_t i = 0; i < index; ++i)
    {
      ++iter;
    }
    return (*iter);
  }
  return NULL;
}

namespace
{
#ifdef NM_PROFILING
  // -----------------------------------------------------------------------------
  void recursiveTraverseRecords(NMP::Profiler* profiler, const NMP::Profiler::ProfilerRecord* record, size_t depth, void (*processRecord)(NMP::Profiler*, const NMP::Profiler::ProfilerRecord*, size_t))
  {
    (*processRecord)(profiler, record, depth);
    const NMP::Profiler::ProfilerRecord* end = profiler->getRecordChildrenEnd(record);
    for (const NMP::Profiler::ProfilerRecord* child = profiler->getRecordChildrenBegin(record); child != end; child = profiler->getRecordNextSibling(child))
    {
      recursiveTraverseRecords(profiler, child, depth + 1, processRecord);
    }
  }

  // -----------------------------------------------------------------------------
  void printRecord(NMP::Profiler* profiler, const NMP::Profiler::ProfilerRecord* record, size_t depth)
  {
    const NMP::Profiler::ProfilerBlock* block = profiler->getRecordBlock(record);
    for (uint32_t tab = 1; tab < depth; ++tab)
    {
      printf(" | ");
    }
    if (depth)
    {
      printf(" |_");
    }
    printf("%6.3f us - %s\n", profiler->getRecordTime(record) * 1.0e3f, block->getTag());
  }

  // -----------------------------------------------------------------------------
  void gatherProfilingData(NMP::Profiler* profiler)
  {
    const size_t threadCount = profiler->getNumThreads();
    for (size_t threadIndex = 0; threadIndex != threadCount; ++threadIndex)
    {
      printf("=================================================\n");
      printf("=================================================\n");

      const NMP::Profiler::ProfilerRecord* rootRecord = profiler->getRootRecord(threadIndex);

      if (rootRecord)
      {
        recursiveTraverseRecords(profiler, rootRecord, 0, &printRecord);
      }
      printf("=================================================\n\n");
    }
  }
#endif // NM_PROFILING
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void CharacterManager::startProfiling()
{
  // Record a PIX capture at this point at a particular frame, for profiling.
#if defined(NM_HOST_X360) && defined(PROFILE) && defined(NM_RTT_PIX_CAPTURE)
  static int pixFrameCount = 0;
  s_pixCapture = false;
  if (m_characterInstancesListHead) // If there are any characters.
  {
    ++pixFrameCount;
    if (pixFrameCount == PIX_CAPTURE_FRAME_COUNT)
    {
      s_pixCapture = true;
      XTraceStartRecording("game:\\morphemeCapture.pix2");
    }
  }
#endif // NM_HOST_X360
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManager::endProfiling()
{
  // End PIX trace.
#if defined(NM_HOST_X360) && defined(PROFILE) && defined(NM_RTT_PIX_CAPTURE)
  if (s_pixCapture)
  {
    XTraceStopRecording();
    XTraceSaveBuffer("game:\\morphemeCapture.pix2");
  }
#endif // NM_HOST_X360

#ifdef NM_PROFILING
  NMP::Profiler::getProfiler()->endProfilingFrame();
  gatherProfilingData(NMP::Profiler::getProfiler());
  NMP::Profiler::getProfiler()->reset();
#endif // NM_PROFILING
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
