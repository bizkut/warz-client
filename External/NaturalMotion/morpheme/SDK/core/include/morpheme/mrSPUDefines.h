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
#ifndef MR_SPU_DEFINES_H
#define MR_SPU_DEFINES_H

#if defined(NM_HOST_CELL_PPU) || defined(NM_HOST_CELL_SPU)
  #include <cell/spurs.h>
  #include <cell/sync/queue.h>
#else
  #error mrSPUDefines.h should only be included on PS3!
#endif

#include "NMPlatform/NMRingBuffer.h"
#include "NMPlatform/NMVectorContainer.h"
#include "NMPlatform/ps3/NMSPUDefines.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

#define TASK_RESERVE_SIZE 512

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::PreAllocatedTask
/// \brief Stores a pre-allocated task along with additional dependency chunks.  This is used to store the next task
///  so that it doesn't need to be copied from the PPU multiple times.  A task can only be stored in PreAllocated task
///  if its size is <= TASK_RESERVE_SIZE and it its m_numExtraDependencyChunks is <= MAX_ADDITIONAL_DEPENDENT_CHUNKS.
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  SPU_RESULT_OK = 0x0001,
  SPU_RESULT_VERSION = 0x0002,
  SPU_RESULT_OUT_OF_MEMORY_STARTING_UP = 0x0004,
  SPU_RESULT_OUT_OF_MEMORY_DURING_TASK = 0x0008,
  SPU_RESULT_OUT_OF_OUTPUT_SLOTS = 0x0010,
  SPU_RESULT_UNREGISTERED_FUNCTION_CALLED = 0x0020,
  SPU_RESULT_OUT_OF_MEMORY_DURING_DEP_UPDATE = 0x0040,
  SPU_RESULT_EXTERNAL_DEPENDANCY = 0x0080,
} SPUExecuteResult;

//----------------------------------------------------------------------------------------------------------------------
typedef struct SPUMorphemeArguments
{
  // This ensures that this DMA structure is a multiple of 16 bytes
  PPU_POINTER(Task*) taskList __attribute__ ((aligned(16)));
  uint32_t numTasksInList;
  PPU_POINTER(uint32_t) numUnprocessedTasksAddr;

  // For semantic registration finalization in the SPU Manager
  uint32_t highestRegisteredAttribSemantic;

  // These 4 words contain the version number of the PPU executable - this helps us detect problems with non-matching
  //  PPU and SPU executables
  uint32_t versionNumber[4];

  uint32_t spuIndex;

  // 128 bytes = 32 words.  There's 19 words above leaving us with a 13 word pad
  uint32_t pad[3];
} SPUMorphemeArguments;

//----------------------------------------------------------------------------------------------------------------------
NM_ASSERT_COMPILE_TIME(sizeof(SPUMorphemeArguments) == 48);

//----------------------------------------------------------------------------------------------------------------------
typedef union SPUExeArgument
{
  struct
  {
    // This ensures that this DMA structure is a multiple of 16 bytes
    uint32_t morphemePackedArg __attribute__ ((aligned(16)));
    uint32_t SPUTaskFnMapEA;
    uint16_t initvalue;
  };
#ifdef NM_HOST_CELL_PPU
  CellSpursTaskArgument spursArgument;
#elif defined(NM_HOST_CELL_SPU)
  vec_uint4 uiQWord;
#endif

  void setMorphemeArgs(MR::SPUMorphemeArguments* morphemeArgs)
  {
    uint32_t _morphemeArgs = (uint32_t)morphemeArgs;
    NMP_ASSERT((_morphemeArgs & 1) == 0);
    morphemePackedArg = _morphemeArgs;  // Not specifying an ArgQueue
  };
  void setMorphemeArgQueue(CellSyncQueue* morphemeArgQueue)
  {
    uint32_t _morphemeArgQueue = (uint32_t)morphemeArgQueue;
    NMP_ASSERT((_morphemeArgQueue & 1) == 0);
    morphemePackedArg = _morphemeArgQueue | 1; // Specifying an ArgQueue with lowest bit
  };
  PPU_POINTER(MR::SPUMorphemeArguments) getMorphemeArgs()
  {
    NMP_ASSERT(!isMorphemeArgNull());
    NMP_ASSERT(isMorphemeArgSingleTask());
    return (PPU_POINTER(MR::SPUMorphemeArguments))morphemePackedArg;
  }
  PPU_POINTER(CellSyncQueue) getMorphemeArgQueue()
  {
    NMP_ASSERT(!isMorphemeArgNull());
    NMP_ASSERT(!isMorphemeArgSingleTask());
    return (PPU_POINTER(CellSyncQueue)) (morphemePackedArg & ~1);
  }
  void setMorphemeArgNull()      { morphemePackedArg = 0; };
  bool isMorphemeArgNull()       { return  morphemePackedArg == 0; };
  bool isMorphemeArgSingleTask() { return (morphemePackedArg & 1) == 0; };

} SPUArgument;

#if defined(NM_HOST_CELL_SPU)

//----------------------------------------------------------------------------------------------------------------------
class SPUBookMark
{
public:
  static void Insert(uint32_t bookmark)
  {
    __asm__ volatile ("wrch $69, %0" :: "r" (bookmark));
    __asm__ volatile ("nop; nop; nop; nop; nop; nop; nop; nop");
    __asm__ volatile ("nop; nop; nop; nop; nop; nop; nop; nop");
  }
  enum Event
  {
    // rgb-555 colours
    START        = 0x7fff,
    OVERLAY_DMA  = 0x7C00,
    OVERLAY_WAIT = 0x03ff,
    OVERLAY_END  = 0x03e0,
    INPUT_DMA    = 0x7C01,
    INPUT_WAIT   = 0x03fe,
    INPUT_END    = 0x03e1,
    RELOCATE     = 0x7C02,
    PREPARE      = 0x03e2,
    TASK_START   = 0x001f,
    TASK_END     = 0x03e3,
    DISLOCATE    = 0x03fd,
    OUTPUT_DMA   = 0x7C03,
    OUTPUT_WAIT  = 0x03fc,
    OUTPUT_END   = 0x03e4,
    END          = 0x7ffe

    // SNTuner Mappings.ini
    // [SpuBookmarks]
    // Find what:    :b*{ :i }:b*=:b*{ 0x[0123456789abcdef]+ }\, *:b*
    // Replace with: \2=\1

  };
};

  #if defined(NM_USE_SNTUNER)

    #define MR_SPUBOOKMARK(_id)       MR::SPUBookMark::Insert(MR::SPUBookMark::_id)
    #define MR_SPUBOOKMARKUINT(_uint) MR::SPUBookMark::Insert(_uint)
    #define MR_SNTUNEROVERLAYLOAD(_unused)   // No implementation yet
    #define MR_SNTUNEROVERLAYUNLOAD(_unused) // No implementation yet
    #define MR_SNTUNERTASKSTART(_uint) \
  SPUPutTaskStartPacket(OVERLAY_DMA_TAG, MR_SPUTASKOVERLAYBEGIN, 4, _uint + 0x30303030 /* == "0000" */); \
  MR_SPUBOOKMARKUINT(_uint)
    #define MR_SNTUNERTASKEND(_unused) \
  MR_SPUBOOKMARK(TASK_END); \
  SPUPutTaskStopPacket(OVERLAY_DMA_TAG, MR_SPUTASKOVERLAYBEGIN)

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SPUPutTaskStartPacket(unsigned DMATag, uint32_t elfBaseAddress, uint16_t level, uint32_t module)
{
  // static as cellSpursModulePutTrace() does not wait for DMA completion
  static CellSpursTracePacket packet;

  packet.header.tag = CELL_SPURS_TRACE_TAG_START;
  packet.data.start.level = level;
  packet.data.start.ls = elfBaseAddress >> 2;
  *((uint32_t*)packet.data.start.module) = module;

  cellSpursModulePutTrace(&packet, DMATag);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SPUPutTaskStopPacket(unsigned DMATag, uint32_t elfBaseAddress)
{
  // static as cellSpursModulePutTrace() does not wait for DMA completion
  static CellSpursTracePacket packet;

  packet.header.tag = CELL_SPURS_TRACE_TAG_STOP;
  cellSpursGetSpuGuid((qword*)elfBaseAddress, &packet.data.guid);

  cellSpursModulePutTrace(&packet, DMATag);
}

  #else

    #define MR_SPUBOOKMARK(_id)
    #define MR_SPUBOOKMARKUINT(_id)
    #define MR_SNTUNEROVERLAYLOAD(_unused)
    #define MR_SNTUNEROVERLAYUNLOAD(_unused)
    #define MR_SNTUNERTASKSTART(_uint)
    #define MR_SNTUNERTASKEND(_unused)

  #endif

#endif // defined(NM_HOST_CELL_SPU)

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif //MR_SPU_DEFINES_H
//----------------------------------------------------------------------------------------------------------------------

