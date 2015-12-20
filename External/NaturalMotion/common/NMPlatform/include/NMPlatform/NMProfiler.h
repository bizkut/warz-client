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
#ifndef NM_PROFILER_H
#define NM_PROFILER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMTimer.h"
#include "NMPlatform/NMSystem.h"
//----------------------------------------------------------------------------------------------------------------------

#ifndef SN_TARGET_PS3_SPU
  // turn on generic profiling
  #define NM_PROFILINGx
#else
  // turn on SPU profiling
  #define NM_SPU_PROFILINGx
#endif

#ifndef SN_TARGET_PS3_SPU
  // turn on simple profiling.
  #define NM_SIMPLE_PROFILINGx
#endif

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// A very simple profiler.
/// Generates results in milliseconds.
/// Uses statics to store average and peak times.
/// Several instances can be declared, but its best to avoid using them hierarchically because the act of
///  starting and stopping a timer is in itself costly.
//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_SIMPLE_PROFILING
  #define NM_SIMPLE_PROFILE_DECLARE(NAME)   \
    static int32_t NAME##_sampleCount = 0;  \
    static float NAME##_totalTime = 0.0f;   \
    static float NAME##_peakTime = 0.0f;    \
    NMP::Timer NAME##_timer;

  // Resets stores average and peak values.
  #define NM_SIMPLE_PROFILE_RESET(NAME)     \
    NAME##_sampleCount = 0;                 \
    NAME##_totalTime = 0.0f;                \
    NAME##_peakTime = 0.0f;

  // Starts a frames timing.
  #define NM_SIMPLE_PROFILE_BEGIN(NAME)     \
    NAME##_timer.start();

  // Ends a frames timing, calculates new averages and peak times.
  #define NM_SIMPLE_PROFILE_NUM_FRAMES_TO_AVERAGE 100     // Average times will be calculated over this number of frames. This is also the frequency of results display.
  #define NM_SIMPLE_PROFILE_END(NAME)                                                                 \
    float NAME##_thisTime = NAME##_timer.stop();                                                      \
    NAME##_totalTime += NAME##_thisTime;                                                              \
    NAME##_sampleCount++;                                                                             \
    if (NAME##_sampleCount % NM_SIMPLE_PROFILE_NUM_FRAMES_TO_AVERAGE == 0)                                                                  \
    {                                                                                                 \
      printf("%s: %f ave ms (%i samples), time: %f \n", #NAME, ((float)NAME##_totalTime) / NAME##_sampleCount, NAME##_sampleCount, NAME##_thisTime);\
      NAME##_sampleCount = 0;                                                                         \
      NAME##_totalTime = 0.0f;                                                                        \
    }                                                                                                 \
    if (NAME##_thisTime > NAME##_peakTime)                                                            \
    {                                                                                                 \
      printf("New %s peak time. Old = %fms, New = %fms\n", #NAME, NAME##_peakTime, NAME##_thisTime);  \
      NAME##_peakTime = NAME##_thisTime;                                                              \
    }
#else   // NM_SIMPLE_PROFILING
  #define NM_SIMPLE_PROFILE_DECLARE(NAME)
  
  // Resets stores average and peak values.
  #define NM_SIMPLE_PROFILE_RESET(NAME)
  
  // Starts a frames timing.
  #define NM_SIMPLE_PROFILE_BEGIN(NAME)
  
  // Ends a frames timing, calculates new averages and peak times.
  #define NM_SIMPLE_PROFILE_END(NAME)
#endif  // NM_SIMPLE_PROFILING

//----------------------------------------------------------------------------------------------------------------------
/// Maximum number of characters copied from the section.
/// Name tags need to *really* describe your sections?
/// Increase this value and note that it will be allocated with +1 characters for null termination.
#define NM_PROF_MAXNAMETAG       (63)

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Profiler
/// \brief The NM Platform hierarchical profiler.
/// \ingroup NaturalMotionPlatform
///
/// A tool to allow accurate, organised performance timing and statistic gathering of
/// compiled code
//----------------------------------------------------------------------------------------------------------------------
class Profiler
{
public:
  Profiler(size_t initialSize);
  ~Profiler();

  // Returns the default profiler, used by the profiling macros below.
  static Profiler* getProfiler();

  //----------------------------------------------------------------------------------------------------------------------
  /// \class NMP::Profiler::ProfilerBlock
  /// \brief A static profiling block in the code.
  /// \ingroup NaturalMotionPlatform
  //----------------------------------------------------------------------------------------------------------------------
  struct ProfilerBlock
  {
  public:
    ProfilerBlock(const char* tag, const void* userData);

    NM_INLINE const char* getTag() const
    {
      return m_tag;
    }

    NM_INLINE const void* getUserData() const
    {
      return m_userData;
    }

    static ProfilerBlock* findBlock(const char* tag);

  private:
    const char*    m_tag;
    const void*    m_userData;
    ProfilerBlock* m_next;

    static ProfilerBlock* sm_firstBlock;

    static void registerBlock(ProfilerBlock* block);

    friend class Profiler;
  };

  struct ProfilerTree;

  struct ProfilerRecord;

  /// \brief Begin profiling block.
  void beginProfiling(ProfilerBlock* block);

  /// \brief End profiling block.
  void endProfiling();

  /// \brief End profiling frame.
  /// \remarks Has to be called at the end of each frame, before traversing the trees.
  void endProfilingFrame();

  /// \brief Reset the collected data.
  /// \remarks Has to be called after collecting the data for one frame, and before starting the next.
  void reset();

  /// \brief Returns the number of threads for which there is profiling information.
  size_t getNumThreads() const;

  /// \brief Returns the thread ID corresponding to the thread index.
  ThreadId getThreadId(size_t threadIndex) const;

  /// \brief Returns number of profiling records of the thread index.
  size_t getNumRecords(size_t threadIndex) const;

  /// \brief Returns true if some of the thread records were discarded because they didn't fit in
  /// the allocated space.
  bool isOverflowed(size_t threadIndex) const;

  /// \brief Returns the root (outermost) record of the thread.
  const ProfilerRecord* getRootRecord(size_t threadIndex) const;

  /// \brief Returns the time in milliseconds for the record.
  float getRecordTime(const ProfilerRecord* record) const;

  /// \brief Returns the block of code the record measured.
  const ProfilerBlock* getRecordBlock(const ProfilerRecord* record) const;

  /// \brief Returns the first child of a profiler record.
  /// Record tree iteration. Usage:
  /// ProfilerRecord *current;
  /// ProfilerRecord *end = profiler->getRecordChildrenEnd(current);
  /// for (ProfilerRecord *child = profiler->getRecordChildrenBegin(current); child != end; child = profiler->getRecordNextSibling(child))
  /// {
  ///   // Do Stuff
  /// }
  const ProfilerRecord* getRecordChildrenBegin(const ProfilerRecord* record) const;

  /// \brief Returns one past the last child of a profiler record.
  const ProfilerRecord* getRecordChildrenEnd(const ProfilerRecord* record) const;

  /// \brief Returns the next sibling of a profiler record.
  const ProfilerRecord* getRecordNextSibling(const ProfilerRecord* record) const;

  /// \brief Returns the current maximum number of records for a thread.
  size_t getPoolSize(size_t threadIndex) const;

  /// \brief Returns the current size of the records buffer for a thread in bytes.
  size_t getPoolSizeBytes(size_t threadIndex) const;

  /// \brief Returns the sum of all the records for the given block and thread.
  /// \remarks Doesn't handle recursive functions well. Adds up all the times.
  float aggregateBlockTime(const ProfilerBlock* block, size_t threadIndex) const;

  /// \brief Returns the sum of all the records for the given block across all threads.
  /// \remarks Doesn't handle recursive functions well. Adds up all the times.
  float aggregateBlockTime(const ProfilerBlock* block) const;

private:
  Profiler& operator=(const Profiler&);

  friend struct ProfilerBlock;

  // Simple moving average for performance spike detection
  float m_avtime;

  /// \brief Create a new profiling block.
  ///
  /// Note: Only NM_PROF_MAXNAMETAG characters of <tt>tag</tt> will be recorded.
  /// Also, blocks are never destroyed, so no need for a destroyBlock function.
  void registerBlock(ProfilerBlock* block);

  /// \brief Returns the tree for the given thread ID, if it exists. NULL otherwise.
  ProfilerTree* findTree(ThreadId threadId) const;

  /// \brief Creates and returns a new tree for the supplied thread ID.
  ProfilerTree* createTree(ThreadId threadId);

  /// \brief Returns the first tree.
  ProfilerTree* getFirstTree() const;

  /// \brief Returns the tree following the supplied one in list order.
  ProfilerTree* getNextTree(ProfilerTree* tree) const;

  /// \brief Returns the n-th tree in list order.
  ProfilerTree* getTree(size_t treeIndex) const;

private:
  ProfilerTree*  m_firstTree;

  const size_t   m_initialSize;

  Timer          m_timer;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::ScopedProfiling
/// \brief The NM Platform scoped context profiler.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class ScopedProfiling
{
public:

  explicit ScopedProfiling(NMP::Profiler* profiler, Profiler::ProfilerBlock* block)
    : m_profiler(profiler)
  {
    m_profiler->beginProfiling(block);
  }

  ~ScopedProfiling()
  {
    m_profiler->endProfiling();
  }

private:

  NMP::Profiler* m_profiler;
};

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_PROFILING

  #define NM_INIT_PROFILING()

  #define NM_RELEASE_PROFILING()

template<int N>
void TAG_MUST_BE_LITERAL_STRING__(const char (&)[N])
{
}

  #define TAG_MUST_BE_LITERAL_STRING(tag) \
  { \
    const char TAG_MUST_BE_LITERAL_STRING_[] = tag; \
    TAG_MUST_BE_LITERAL_STRING__(TAG_MUST_BE_LITERAL_STRING_); \
  }

  #if defined(_MSC_VER)
    #define NM_BEGIN_PROFILING_USER_DATA_CUSTOM_PROFILER(tag, data, profiler) \
  do \
  { \
    TAG_MUST_BE_LITERAL_STRING(tag); \
    static NMP::Profiler::ProfilerBlock block(tag, data); \
    NMP_ASSERT_MSG(block.getUserData() == data, "The user data must be static for each profiling block!"); \
    profiler->beginProfiling(&block); \
    __pragma(warning(push)) \
    __pragma(warning(disable:4127)) \
  } while (0) \
  __pragma(warning(pop))
  #else // defined(_MSC_VER)
    #define NM_BEGIN_PROFILING_USER_DATA_CUSTOM_PROFILER(tag, data, profiler) \
  do \
  { \
    TAG_MUST_BE_LITERAL_STRING(tag); \
    static NMP::Profiler::ProfilerBlock block(tag, data); \
    NMP_ASSERT_MSG(block.getUserData() == data, "The user data must be static for each profiling block!"); \
    profiler->beginProfiling(&block); \
  } while (0)
  #endif // defined(_MSC_VER)

  #define NM_BEGIN_PROFILING_CUSTOM_PROFILER(tag, profiler) NM_BEGIN_PROFILING_USER_DATA_CUSTOM_PROFILER(tag, 0, profiler)

  #define NM_BEGIN_PROFILING_USER_DATA(tag, data) NM_BEGIN_PROFILING_USER_DATA_CUSTOM_PROFILER(tag, data, NMP::Profiler::getProfiler())

  #define NM_BEGIN_PROFILING(tag) NM_BEGIN_PROFILING_USER_DATA(tag, 0)

  #if defined(_MSC_VER)
    #define NM_END_PROFILING_CUSTOM_PROFILER(profiler) \
  do \
  { \
    profiler->endProfiling(); \
    __pragma(warning(push)) \
    __pragma(warning(disable:4127)) \
  } while (0) \
  __pragma(warning(pop))
  #else // defined(_MSC_VER)
    #define NM_END_PROFILING_CUSTOM_PROFILER(profiler) \
  do \
  { \
    profiler->endProfiling(); \
  } while (0)
  #endif // defined(_MSC_VER)

  #define NM_END_PROFILING() NM_END_PROFILING_CUSTOM_PROFILER(NMP::Profiler::getProfiler())

  #define CONCAT_AUX(a, b) a##b
  #define CONCAT(a, b) CONCAT_AUX(a, b)

// If you get an undeclared identifier error for profiler,
// make sure you're not doing something like:
// if ()
//   NM_CONTEXT_PROFILING();
  #define NM_CONTEXT_PROFILING_USER_DATA_CUSTOM_PROFILER(tag, data, profiler) \
  TAG_MUST_BE_LITERAL_STRING(tag); \
  static NMP::Profiler::ProfilerBlock CONCAT(NM_CONTEXT_PROFILING_USER_DATA_block__, __LINE__)(tag, data); \
  NMP_ASSERT_MSG(CONCAT(NM_CONTEXT_PROFILING_USER_DATA_block__, __LINE__).getUserData() == data, "The user data must be static for each profiling block!"); \
  NMP::ScopedProfiling CONCAT(NM_CONTEXT_PROFILING_scopedProfiler__, __LINE__)(profiler, &CONCAT(NM_CONTEXT_PROFILING_USER_DATA_block__, __LINE__))

  #define NM_CONTEXT_PROFILING_USER_DATA(tag, data) \
  NM_CONTEXT_PROFILING_USER_DATA_CUSTOM_PROFILER(tag, data, NMP::Profiler::getProfiler())

  #define NM_CONTEXT_PROFILING(tag) NM_CONTEXT_PROFILING_USER_DATA(tag, 0)

#else // NM_PROFILING

  #define NM_INIT_PROFILING()

  #define NM_RELEASE_PROFILING()

  #define NM_BEGIN_PROFILING_USER_DATA_CUSTOM_PROFILER(tag, data, profiler)

  #define NM_BEGIN_PROFILING_USER_DATA(tag, userData)

  #define NM_BEGIN_PROFILING(tag)

  #define NM_END_PROFILING_CUSTOM_PROFILER(profiler)

  #define NM_END_PROFILING()

  #define NM_CONTEXT_PROFILING_USER_DATA(tag, userData)

  #define NM_CONTEXT_PROFILING(tag)

#endif // NM_PROFILING

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_PROFILER_H
//----------------------------------------------------------------------------------------------------------------------
