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
#include "NMPlatform/NMProfiler.h"
#include "NMPlatform/NMAtomic.h"
#include "NMPlatform/NMMathUtils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//----------------------------------------------------------------------------------------------------------------------

// Disable deprecated string function warnings under Win32.
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4996)
#endif

namespace NMP
{

struct Profiler::ProfilerTree
{
  ProfilerTree*   m_next;
  ProfilerRecord* m_currentRecord;
  ThreadId        m_threadId;
  ProfilerRecord* m_newRecord;
  ProfilerRecord* m_records;
  size_t          m_currentNumRecords;
  size_t          m_maxRecords;

  ProfilerTree(ThreadId threadId, size_t initialSize);
  ~ProfilerTree();
  ProfilerRecord* beginRecording(Timer& timer, ProfilerBlock* block);
  ProfilerRecord* endRecording(Timer& timer);
  void endRecordingFrame();
  bool isFinishedRecording();
  void reset();
  void growRecords(size_t numRecords);
};

struct Profiler::ProfilerRecord
{
private:
  NM_INLINE const ProfilerRecord* childrenBegin() const
  {
    return this + 1;
  }

  NM_INLINE const ProfilerRecord* childrenEnd() const
  {
    return m_subtreeEnd;
  }

  NM_INLINE const ProfilerRecord* nextSibling() const
  {
    return m_subtreeEnd;
  }

  uint64_t       m_timeElapsed;
  ProfilerBlock* m_block;

  // Implementation comments:
  // We just need a pointer to the end of the whole subtree on each node to rebuild the call tree.
  // Imagine the following call tree:
  // A
  // {
  //   B
  //   {
  //     C
  //     {
  //       D
  //       {
  //       }
  //     E
  //     {
  //       F
  //       {
  //       }
  //     }
  //   G
  //   {
  //   }
  // }
  // This will result in the following structure (using indices for clarity instead of pointers):
  // --------------------------------------------------
  // | Function name | Node index | End subtree index |
  // --------------------------------------------------
  // |             A |          0 |                 7 |
  // |             B |          1 |                 6 |
  // |             C |          2 |                 4 |
  // |             D |          3 |                 4 |
  // |             E |          4 |                 6 |
  // |             F |          5 |                 6 |
  // |             G |          6 |                 7 |
  // --------------------------------------------------
  // As the blocks are inserted in call order (depth first traversal), the first child of
  // a node is always the one following it.
  // The next sibling of a node, if there is one, is the one after its subtree.
  // The way to iterate the tree is shown in the public section of the class.
  // It is possible to know the parent of a node, iterating from the top node.
  // We don't need that, and it's not constant time, so the function is not added.
  ProfilerRecord* m_subtreeEnd;

  friend class Profiler;
  friend struct ProfilerTree;
};

//----------------------------------------------------------------------------------------------------------------------
namespace
{
Profiler* g_profiler = Profiler::getProfiler();

Profiler::ProfilerBlock* getRootBlock();
Profiler::ProfilerBlock* g_rootBlock = getRootBlock();

Profiler::ProfilerBlock* getRootBlock()
{
  static Profiler::ProfilerBlock rootBlock("ROOT", 0);
  g_rootBlock = &rootBlock;
  return g_rootBlock;
}
}

//----------------------------------------------------------------------------------------------------------------------
Profiler* Profiler::getProfiler()
{
  // This is called during static c++ runtime initialization.
  // If no one calls getProfiler manually, it will get called initializing g_profiler above.
  // That's single threaded code, so no need to worry about threading issues here.
  // We need to use g_profiler above, otherwise the linker won't include neither the
  // variable nor the call to getProfiler, which is what we are interested on.
  // We can, either test !g_profiler, or write it every time.
  // Have to test which option is faster, particularly on consoles!!!
  static const size_t NUM_RECORDS = 16384;
  static Profiler profiler(NUM_RECORDS);
  g_profiler = &profiler;
  return &profiler;
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::Profiler(size_t initialSize) :
  m_firstTree(0),
  m_initialSize(initialSize)
{
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::~Profiler()
{
  ProfilerTree* tree = getFirstTree();

  while (tree)
  {
    ProfilerTree* deleteTree = tree;
    tree = getNextTree(tree);
    delete deleteTree;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Profiler::beginProfiling(ProfilerBlock* block)
{
  ThreadId threadId = getCurrentThreadId();
  ProfilerTree* tree = findTree(threadId);
  if (!tree)
  {
    tree = createTree(threadId);
  }
  tree->beginRecording(m_timer, block);
}

//----------------------------------------------------------------------------------------------------------------------
void Profiler::endProfiling()
{
  ThreadId threadId = getCurrentThreadId();
  ProfilerTree* tree = findTree(threadId);
  if (!tree)
  {
    tree = createTree(threadId);
  }
  tree->endRecording(m_timer);
}

//----------------------------------------------------------------------------------------------------------------------
void Profiler::endProfilingFrame()
{
  ProfilerTree* tree = getFirstTree();

  while (tree)
  {
    tree->endRecordingFrame();
    tree = getNextTree(tree);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Profiler::reset()
{
  ProfilerTree* tree = getFirstTree();

  while (tree)
  {
    tree->reset();
    tree = getNextTree(tree);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerTree* Profiler::findTree(ThreadId threadId) const
{
  ProfilerTree* tree = m_firstTree;
  while (tree && tree->m_threadId != threadId)
  {
    tree = tree->m_next;
  }

  return tree;
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerTree* Profiler::createTree(ThreadId threadId)
{
  // We need this to be thread safe.
  // We have a linked list of ProfilerTrees, starting with m_firstTree,
  // then following the m_next pointers, until we reach a NULL pointer.
  // There are several restrictions that make a lock free implementation easy.
  // First, no two threads will try to insert the same object (the same being the one the
  // other thread is searching for), so we don't have to block findTree, and we can
  // just create the object we will be inserting and don't block around that either!
  // Second, we never remove from the list.
  // So, basically, all we need to make sure is that a node of the list is never lost.
  // There's a race condition if we do a simple assignment to enlist the new las item.
  // If we just do lastTreePtr = newTree, two threads executing at the same time
  // can cause a node to be missing from the list.
  // We can use an interlocked compare exchange primitive to work around this.

  ProfilerTree* newTree = new ProfilerTree(threadId, m_initialSize);

  ProfilerTree** lastTreePtr = &m_firstTree;
  do
  {
    while (*lastTreePtr)
    {
      lastTreePtr = &(*lastTreePtr)->m_next;
    }

  } while (atomicCompareAndSwapPtr((void *&)(*lastTreePtr), 0, newTree));

  return newTree;
}

//----------------------------------------------------------------------------------------------------------------------
size_t Profiler::getNumThreads() const
{
  size_t numThreads = 0;
  ProfilerTree* tree = getFirstTree();
  while (tree)
  {
    ++numThreads;
    tree = getNextTree(tree);
  }

  return numThreads;
}

//----------------------------------------------------------------------------------------------------------------------
ThreadId Profiler::getThreadId(size_t threadIndex) const
{
  return getTree(threadIndex)->m_threadId;
}

//----------------------------------------------------------------------------------------------------------------------
size_t Profiler::getNumRecords(size_t threadIndex) const
{
  ProfilerTree* tree = getTree(threadIndex);

  if (!tree)
  {
    return 0;
  }

  return tree->m_newRecord - tree->m_records;
}

//----------------------------------------------------------------------------------------------------------------------
bool Profiler::isOverflowed(size_t threadIndex) const
{
  ProfilerTree* tree = getTree(threadIndex);

  if (!tree)
  {
    return 0;
  }

  return (uintptr_t)(tree->m_newRecord - tree->m_records) > tree->m_currentNumRecords;
}

//----------------------------------------------------------------------------------------------------------------------
const Profiler::ProfilerRecord* Profiler::getRootRecord(size_t threadIndex) const
{
  ProfilerTree* tree = getTree(threadIndex);

  if (!tree || !tree->isFinishedRecording())
  {
    return 0;
  }

  return &tree->m_records[0];
}

//----------------------------------------------------------------------------------------------------------------------
float Profiler::getRecordTime(const ProfilerRecord* record) const
{
  return m_timer.getTime(record->m_timeElapsed);
}

//----------------------------------------------------------------------------------------------------------------------
const Profiler::ProfilerBlock* Profiler::getRecordBlock(const ProfilerRecord* record) const
{
  return record->m_block;
}

//----------------------------------------------------------------------------------------------------------------------
const Profiler::ProfilerRecord* Profiler::getRecordChildrenBegin(const ProfilerRecord* record) const
{
  return record + 1;
}

//----------------------------------------------------------------------------------------------------------------------
const Profiler::ProfilerRecord* Profiler::getRecordChildrenEnd(const ProfilerRecord* record) const
{
  return record->m_subtreeEnd;
}

//----------------------------------------------------------------------------------------------------------------------
const Profiler::ProfilerRecord* Profiler::getRecordNextSibling(const ProfilerRecord* record) const
{
  return record->m_subtreeEnd;
}

//----------------------------------------------------------------------------------------------------------------------
size_t Profiler::getPoolSize(size_t threadIndex) const
{
  ProfilerTree* tree = getTree(threadIndex);

  if (!tree)
  {
    return 0;
  }

  return tree->m_currentNumRecords;
}

//----------------------------------------------------------------------------------------------------------------------
size_t Profiler::getPoolSizeBytes(size_t threadIndex) const
{
  return getPoolSize(threadIndex) * sizeof(ProfilerRecord);
}

//----------------------------------------------------------------------------------------------------------------------
float Profiler::aggregateBlockTime(const ProfilerBlock* block, size_t threadIndex) const
{
  float time = 0.0f;

  const ProfilerRecord* rootRecord = getRootRecord(threadIndex);
  if (rootRecord)
  {
    const ProfilerRecord* end = rootRecord->childrenEnd();
    for (const ProfilerRecord* record = rootRecord; record != end; ++record)
    {
      if (record->m_block == block)
      {
        time += getRecordTime(record);
      }
    }
  }

  return time;
}

//----------------------------------------------------------------------------------------------------------------------
float Profiler::aggregateBlockTime(const ProfilerBlock* block) const
{
  const size_t threadCount = getNumThreads();

  float time = 0.0f;
  for (size_t threadIndex = 0; threadIndex != threadCount; ++threadIndex)
  {
    time += aggregateBlockTime(block, threadIndex);
  }

  return time;
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerTree* Profiler::getFirstTree() const
{
  return m_firstTree;
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerTree* Profiler::getNextTree(ProfilerTree* tree) const
{
  return tree->m_next;
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerTree* Profiler::getTree(size_t treeIndex) const
{
  size_t numThreads = 0;
  ProfilerTree* tree = getFirstTree();
  while (numThreads != treeIndex)
  {
    ++numThreads;
    tree = getNextTree(tree);
  }

  return tree;
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerTree::ProfilerTree(ThreadId threadId, size_t initialSize) :
  m_next(0),
  m_currentRecord(0),
  m_threadId(threadId),
  m_newRecord(0),
  m_records(0),
  m_currentNumRecords(0),
  m_maxRecords(0)
{
  growRecords(initialSize);
  m_currentRecord = m_newRecord = m_records + 1;
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerTree::~ProfilerTree()
{
  delete[] m_records;
}

//----------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerTree::growRecords(size_t numRecords)
{
  delete[] m_records;
  m_records = new Profiler::ProfilerRecord[numRecords];
  m_currentNumRecords = numRecords;
}

static Profiler::ProfilerRecord dummy;

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerRecord* Profiler::ProfilerTree::beginRecording(Timer& timer, ProfilerBlock* block)
{
  // A dummy node is used to avoid branches, so that we have always something to fill.
  intptr_t condition = m_newRecord - m_records - m_currentNumRecords;
  ProfilerRecord* newRecord = (ProfilerRecord*)intPtrSelect(condition, (intptr_t)&dummy, (intptr_t)(m_newRecord++));
  timer.currentTime(newRecord->m_timeElapsed);
  newRecord->m_block = block;
  newRecord->m_subtreeEnd = m_currentRecord;

  m_currentRecord = (ProfilerRecord*)intPtrSelect(condition, (intptr_t)m_currentRecord, (intptr_t)newRecord);

  ++m_maxRecords;

  return newRecord;
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerRecord* Profiler::ProfilerTree::endRecording(Timer& timer)
{
  // Take overflow into account. If the profiler tree is over capacity, we use the dummy node to
  // avoid branches.
  intptr_t condition = m_records + m_currentNumRecords - m_newRecord;
  ProfilerRecord* currentRecord = (ProfilerRecord*)intPtrSelect(condition, (intptr_t)m_currentRecord, (intptr_t)&dummy);
  m_currentRecord = currentRecord->m_subtreeEnd;

  uint64_t currentTime;
  timer.currentTime(currentTime);
  currentRecord->m_timeElapsed = currentTime - currentRecord->m_timeElapsed;
  currentRecord->m_subtreeEnd = (ProfilerRecord*)intPtrSelect(condition, (intptr_t)m_newRecord, (intptr_t)m_currentRecord);

  m_newRecord = (ProfilerRecord*)intPtrSelect(condition, (intptr_t)m_newRecord, (intptr_t)(m_newRecord - 1));

  return currentRecord;
}

//----------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerTree::endRecordingFrame()
{
  NMP_ASSERT_MSG(m_currentRecord == m_records + 1, "There are recordings in progress!");
  ProfilerRecord* currentRecord = m_records;
  currentRecord->m_block = getRootBlock();
  currentRecord->m_subtreeEnd = m_newRecord;
  currentRecord->m_timeElapsed = 0;
  for (const ProfilerRecord* child = currentRecord->childrenBegin(); child != currentRecord->childrenEnd(); child = child->nextSibling())
  {
    currentRecord->m_timeElapsed += child->m_timeElapsed;
  }
  m_currentRecord = m_records;
}

//----------------------------------------------------------------------------------------------------------------------
bool Profiler::ProfilerTree::isFinishedRecording()
{
  return m_currentRecord == m_records;
}

//----------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerTree::reset()
{
  NMP_ASSERT_MSG(isFinishedRecording(), "There are recordings in progress!");
  if (m_maxRecords > m_currentNumRecords)
  {
    growRecords(m_maxRecords);
  }
  m_currentRecord = m_newRecord = m_records + 1;
  m_maxRecords = 0;
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerBlock* Profiler::ProfilerBlock::sm_firstBlock = 0;

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerBlock::ProfilerBlock(const char* tag, const void* userData) :
  m_tag(tag),
  m_userData(userData),
  m_next(0)
{
  registerBlock(this);
}

//----------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerBlock::registerBlock(ProfilerBlock* block)
{

  // We need this to be thread safe.
  // We have a linked list of ProfilerBlock, starting with m_firstTree,
  // then following the m_next pointers, until we reach a NULL pointer.
  // There are several restrictions that make a lock free implementation easy.
  // First, if two threads try to create the same object, it doesn't really matter
  // if we initialize the object twice, as it will contain the same data.
  // Second, the destructor does nothing, so we don't have to care about double destruction either.
  // Third, we never remove from the list.
  // So, basically, all we need to make sure is that a node of the list is never lost.
  // There's a race condition if we do a simple assignment to enlist the new las item.
  // If we just do lastTreePtr = newTree, two threads executing at the same time
  // can cause a node to be missing from the list.
  // We can use an interlocked compare exchange primitive to work around this.

  ProfilerBlock** lastBlockPtr = &sm_firstBlock;
  do
  {
    while (*lastBlockPtr)
    {
      if (*lastBlockPtr == block)
      {
        // Already registered from another thread. We had a race!
        return;
      }
      lastBlockPtr = &(*lastBlockPtr)->m_next;
    }

  } while (atomicCompareAndSwapPtr((void *&)(*lastBlockPtr), 0, block));
}

//----------------------------------------------------------------------------------------------------------------------
Profiler::ProfilerBlock* Profiler::ProfilerBlock::findBlock(const char* tag)
{
  ProfilerBlock* block = sm_firstBlock;
  while (block)
  {
    if (!strncmp(block->m_tag, tag, NM_PROF_MAXNAMETAG))
    {
      return block;
    }

    block = block->m_next;
  }

  return 0;
}

}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
