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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_ATTRIBDATA
#define MR_ATTRIBDATA
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMFlags.h"
#include "NMPlatform/NMBuffer.h"
#include "NMPlatform/NMVectorContainer.h"
#include "NMPlatform/NMRNG.h"
#include "NMPlatform/NMAtomic.h"

#include "morpheme/mrAttribAddress.h"
#include "morpheme/mrEventTrackCurve.h"
#include "morpheme/mrEventTrackDuration.h"
#include "morpheme/mrEventTrackDiscrete.h"
#include "morpheme/mrNetworkLogger.h"
#include "morpheme/mrSyncEventPos.h"
#include "morpheme/mrFootCyclePrediction.h"
#include "morpheme/mrCompressedDataBufferVector3.h"
#include "morpheme/mrCompressedDataBufferQuat.h"
//----------------------------------------------------------------------------------------------------------------------

// nonstandard extension used : nameless struct/union
#ifdef NM_COMPILER_MSVC
#pragma warning (push)
#pragma warning (disable : 4201)
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup AttribData Network Attribute Data.
/// \ingroup NetworkModule
///
/// An attribute instance is a self contained Network data unit.
/// As such it forms the basic component of data that can be processed within a Network.
/// Attribute data can be serialized by the asset compiler into constant Network Definition data or Dynamically created
/// during network execution; either prior to or during the dispatch phase (task execution).
//----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------
/// \def MR_ATTRIB_DATA_ALIGNMENT
/// \brief Set the required MR_ATTRIB_DATA_ALIGNMENT dependent on our target platforms requirements.
/// \ingroup AttribData
#define MR_ATTRIB_DATA_ALIGNMENT NMP_VECTOR_ALIGNMENT


//-----------------------------------------
/// \def MR_EMITTED_REQUEST_MAP_ATTRIB_ALIGNMENT
/// \brief Set the required MR_EMITTED_REQUEST_MAP_ATTRIB_ALIGNMENT dependent on the largest alignment requirement of its
///  components (target platform dependent).
/// \ingroup AttribData
#if (NMP_VECTOR_ALIGNMENT > MR_ATTRIB_DATA_ALIGNMENT)
  #define MR_EMITTED_REQUEST_MAP_ATTRIB_ALIGNMENT NMP_VECTOR_ALIGNMENT
#else
  #define MR_EMITTED_REQUEST_MAP_ATTRIB_ALIGNMENT MR_ATTRIB_DATA_ALIGNMENT
#endif

//-----------------------------------------
/// \def MR_TRANSFORM_ATTRIB_ALIGNMENT
/// \brief Set the required MR_TRANSFORM_ATTRIB_ALIGNMENT dependent on the largest alignment requirement of its
///  components (target platform dependent).
/// \ingroup AttribData
#if (NMP_VECTOR_ALIGNMENT > MR_ATTRIB_DATA_ALIGNMENT)
  #define MR_TRANSFORM_ATTRIB_ALIGNMENT NMP_VECTOR_ALIGNMENT
#else
  #define MR_TRANSFORM_ATTRIB_ALIGNMENT MR_ATTRIB_DATA_ALIGNMENT
#endif

// Used by AttribDataSampledEvents
const uint32_t MAX_NUM_DISCRETE_EVENTS = 64;
const uint32_t MAX_NUM_CURVE_EVENTS = 64;

// Used by AttribDataDurationEventTrackSet
const uint32_t MAX_NUM_DURATION_EVENTS = 128;
const uint32_t MAX_NUM_DURATION_TRACKS = 12;

//-----------------------------------------
// Forward declarations
namespace NMRU
{
namespace JointLimits
{
struct Params;
}
namespace PointIK
{
struct Params;
}
}

namespace MR
{

class NodeInitDataArrayDef;
class NodeDef;

//----------------------------------------------------------------------------------------------------------------------
const uint16_t     IS_DEF_ATTRIB_DATA = 0xFFFF;      ///< Assigned to the m_refCount member of and AttribData that is
                                                     ///<  created as definition data. Lets us know that this data is
                                                     ///<  persistent def data that does not need ref counting.
const uint32_t     LIFESPAN_FOREVER = 0xFFFF;        ///< Used to set an AttribAddress lifespan to be infinite.
                                                     ///<  Useful for preventing data from ever being deleted.
const AnimSetIndex CURRENT_ANIM_SET_INDEX = 0xFFFF;  ///< Used when searching for AttribData to indicate that the current
                                                     ///<  animation set's data should be returned

//----------------------------------------------------------------------------------------------------------------------
enum AttribSemanticSense
{
  ATTRIB_SENSE_NONE,                 ///< The AttribData is not passed between nodes
  ATTRIB_SENSE_FROM_PARENT_TO_CHILD, ///< The AttribData is generated by a parent and passed to it's children
  ATTRIB_SENSE_FROM_CHILD_TO_PARENT, ///< The AttribData is generated by a child and passed to it's parent
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribData
/// \brief The base attribute data class, all other attribute data types inherit from this class.
/// \ingroup AttribData
///
/// All AttribData types that are to be created and serialised as constant definition data within the AssetCompiler,
/// must have a locate and a dislocate function. This class is 8 bytes in 32 bit builds and 16 bytes in 64 byte builds
//----------------------------------------------------------------------------------------------------------------------
class AttribData
{
public:
  AttribData()
  {
    m_allocator = NULL;
  }

  NM_FORCEINLINE void setType(AttribDataType type) { m_typeRefCount = (m_typeRefCount & 0xffff) | ((uint32_t)type << 16); }

  NM_FORCEINLINE AttribDataType getType() const { return (AttribDataType)(m_typeRefCount >> 16); }

  NM_FORCEINLINE AttribDataType getTypeUnlocated() const { uint32_t locatedTypeRefCount = m_typeRefCount; NMP::endianSwap(locatedTypeRefCount); return (AttribDataType)(locatedTypeRefCount >> 16); }

  /// \brief Refixes an instance of this class after being dislocated and moved to another location in memory or after
  ///  loading from disk.
  /// \see MR::AttribData::dislocate
  ///
  /// This function is mostly used to reinitialise attribute data after loading from disk.
  /// Locate includes:
  ///   + Turning relative memory offset values that were generated in the dislocate back into pointers.
  ///   + Turns asset ObjectID s that are registered with the MR::Manager into asset pointers using
  ///     MR::getObjectPtrFromObjectID.
  static void locate(AttribData* target);

  /// \brief Unfixes an instance of this class either for copying to another location in memory or for storage to disk.
  /// \see MR::AttribData::locate
  ///
  /// Generally this function is only used in the AssetCompiler to prepare this attribute for storage.
  /// Therefore all AttribData types that are to be created and serialised as constant definition data within the
  /// AssetCompiler, must have a locate and a dislocate function.
  /// Dislocate includes:
  ///   + Endian swapping of data in preparation for the endian nature of the target platform.
  ///   + Pointers to memory blocks that are allocated with this attribute are turned to relative memory offset values.
  ///   + Turns pointers to assets registered with the MR::Manager into asset ObjectID s using
  ///     Manager::getObjectPtrFromObjectID.
  static void dislocate(AttribData* target);

  NM_FORCEINLINE uint16_t getRefCount() const { return (uint16_t)(m_typeRefCount & 0xffff); }
  NM_FORCEINLINE void setRefCount(uint16_t refCount) { m_typeRefCount = (m_typeRefCount & 0xffff0000) | (uint32_t)refCount; }
  NM_FORCEINLINE uint16_t refCountIncrease()
  {
    if (getRefCount() != IS_DEF_ATTRIB_DATA)
      NMP::atomicIncrement32(m_typeRefCount);
    return getRefCount();
  }

  NM_FORCEINLINE uint16_t refCountDecrease()
  {
    NMP_ASSERT(getRefCount() > 0);
    if (getRefCount() != IS_DEF_ATTRIB_DATA)
      NMP::atomicDecrement32(m_typeRefCount);
    return getRefCount();
  }

  NM_FORCEINLINE uint16_t refCountIncrease(uint16_t increment)
  {
    if (getRefCount() != IS_DEF_ATTRIB_DATA)
      NMP::atomicAdd32(m_typeRefCount, (uint32_t)increment);
    return getRefCount();
  }

  NM_FORCEINLINE uint16_t refCountDecrease(uint16_t decrement)
  {
    NMP_ASSERT(getRefCount() >= decrement);
    if (getRefCount() != IS_DEF_ATTRIB_DATA)
      NMP::atomicSub32(m_typeRefCount, (uint32_t)decrement);
    return getRefCount();
  }

  AttribData& operator = (const AttribData& NMP_USED_FOR_ASSERTS(s)) { NMP_ASSERT(getType() == s.getType()); return *this; }

  NMP::MemoryAllocator* m_allocator;    ///< The NMP::MemoryAllocator that was used to allocate this object (if there is one).
private:
  uint32_t              m_typeRefCount; ///< Multiple Network attribute entries can reference 1 AttribData instance.
                                        ///<  This indicates how many Network attribute entries are referencing this piece
                                        ///<  of attribute data. Prevents deletion of AttribData, before all references
                                        ///<  have been deleted.
#ifdef NM_HOST_64_BIT
  // This padding ensures we get consistent packing across all 64-bit compilers.
  uint32_t  m_padding;
#endif
private:
  void * operator new (size_t, void*); ///< Avoid placement new.
};

#ifdef NM_HOST_64_BIT
NM_ASSERT_COMPILE_TIME(sizeof(AttribData) == 16);
#else
NM_ASSERT_COMPILE_TIME(sizeof(AttribData) == 8);
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::AttribDataHandle
/// \brief Provides meta data about the AttribData it points to, allowing it to be DMA'd about (for example).
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
struct AttribDataHandle
{
  NM_FORCEINLINE void set(AttribData* attribData, const NMP::Memory::Format& format) { m_attribData = attribData; m_format = format; }
  NM_INLINE void endianSwap();

  AttribData*          m_attribData;
  NMP::Memory::Format  m_format;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataHandle::endianSwap()
{
  NMP::endianSwap(m_attribData);
  NMP::endianSwap(m_format.size);
  NMP::endianSwap(m_format.alignment);
}

//----------------------------------------------------------------------------------------------------------------------
// Attribute data type definitions.
//----------------------------------------------------------------------------------------------------------------------
class AttribDataCreateDesc
{
public:
  NM_FORCEINLINE AttribDataCreateDesc() : m_refCount(0) {}
  NM_FORCEINLINE AttribDataCreateDesc(uint16_t refCount) : m_refCount(refCount) {}

public:
  uint16_t m_refCount;
};

//----------------------------------------------------------------------------------------------------------------------
class AttribDataArrayCreateDesc : public AttribDataCreateDesc
{
public:
  NM_FORCEINLINE AttribDataArrayCreateDesc(uint32_t numEntries) : m_numEntries(numEntries) {}
  
public:
  uint32_t m_numEntries;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBool
/// \brief Did this node loop playback on the last frame.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBool : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  
  static AttribDataBool* init(
    NMP::Memory::Resource& resource,
    bool                   value,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    bool                  value = true,
    uint16_t              refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataBool() { setType(ATTRIB_TYPE_BOOL); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BOOL; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(const AttribData* source, AttribData* dest);
  NM_INLINE static bool compare(const AttribDataBool* a, const AttribDataBool* b);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  bool  m_value;
};

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataBool::compare(const AttribDataBool* a, const AttribDataBool* b)
{
  return a->m_value == b->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataUInt
/// \brief Simple 32 bit unsigned int.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataUInt : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  
  static AttribDataUInt* init(
    NMP::Memory::Resource& resource,
    uint32_t               value,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);
    
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint32_t              value = 0,
    uint16_t              refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataUInt() { setType(ATTRIB_TYPE_UINT); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_UINT; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(const AttribData* source, AttribData* dest);
  NM_INLINE static bool compare(const AttribDataUInt* a, const AttribDataUInt* b);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  uint32_t m_value;
};

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataUInt::compare(const AttribDataUInt* a, const AttribDataUInt* b)
{
  return a->m_value == b->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataInt
/// \brief Simple signed 32 bit int.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataInt : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  
  static AttribDataInt* init(
    NMP::Memory::Resource& resource,
    int32_t                value,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc = NULL);
    
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    int32_t               value,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataInt() { setType(ATTRIB_TYPE_INT); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_INT; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(const AttribData* source, AttribData* dest);
  NM_INLINE static bool compare(const AttribDataInt* a, const AttribDataInt* b);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  int32_t m_value;
};

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataInt::compare(const AttribDataInt* a, const AttribDataInt* b)
{
  return a->m_value == b->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataFloat
/// \brief Simple float.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataFloat : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  
  static AttribDataFloat* init(
    NMP::Memory::Resource& resource,
    float                  value,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc = NULL);
    
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    float                 value,
    uint16_t              refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataFloat() { setType(ATTRIB_TYPE_FLOAT); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_FLOAT; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(const AttribData* source, AttribData* dest);
  NM_INLINE static bool compare(const AttribDataFloat* a, const AttribDataFloat* b);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  float m_value;
};

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataFloat::compare(const AttribDataFloat* a, const AttribDataFloat* b)
{
  return a->m_value == b->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataVector3
/// \brief Vector3 requires NMP_VECTOR_ALIGNMENT.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataVector3 : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataVector3* init(
    NMP::Memory::Resource& resource,
    const NMP::Vector3&    value,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc = NULL);
    
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    const NMP::Vector3&   value,
    uint16_t              refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataVector3() { setType(ATTRIB_TYPE_VECTOR3); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_VECTOR3; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(const AttribData* source, AttribData* dest);
  NM_INLINE static bool compare(const AttribDataVector3* a, const AttribDataVector3* b);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  uint32_t m_pad[2];
#endif

  NMP::Vector3 m_value;
};

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataVector3::compare(const AttribDataVector3* a, const AttribDataVector3* b)
{
  return a->m_value == b->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataVector4
/// \brief Vector4 requires NMP_VECTOR_ALIGNMENT.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataVector4 : public AttribData
{
public:  
  static NMP::Memory::Format getMemoryRequirements();
  
  static AttribDataVector4* init(
    NMP::Memory::Resource& resource,
    const NMP::Quat&       value,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc = NULL);
  
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    const NMP::Quat&      value,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataVector4() { setType(ATTRIB_TYPE_VECTOR4); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_VECTOR4; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(const AttribData* source, AttribData* dest);
  NM_INLINE static bool compare(const AttribDataVector4* a, const AttribDataVector4* b);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  uint32_t m_pad[2];
#endif

  NMP::Quat m_value;
};

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataVector4::compare(const AttribDataVector4* a, const AttribDataVector4* b)
{
  return a->m_value == b->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBoolArray
/// \brief An array of booloan values.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBoolArray : public AttribData
{
public:  
  static NMP::Memory::Format getMemoryRequirements(uint32_t numValues);

  static AttribDataBoolArray* init(
    NMP::Memory::Resource& resource,
    uint32_t               numValues,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);
    
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint32_t              numValues,
    uint16_t              refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataBoolArray() { setType(ATTRIB_TYPE_BOOL_ARRAY); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BOOL_ARRAY; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  uint32_t  m_numValues;
  bool*     m_values;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataIntArray
/// \brief An array of signed 32 bit integer values.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataIntArray : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numValues);

  static AttribDataIntArray* init(
    NMP::Memory::Resource& resource,
    uint32_t               numValues,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint32_t              numValues,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataIntArray() { setType(ATTRIB_TYPE_INT_ARRAY); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_INT_ARRAY; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  uint32_t m_numValues;
  int32_t* m_values;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataUIntArray
/// \brief An array of unsigned 32 bit integer values.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataUIntArray : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numValues);

  static AttribDataUIntArray* init(
    NMP::Memory::Resource& resource,
    uint32_t               numValues,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint32_t              numValues,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataUIntArray() { setType(ATTRIB_TYPE_UINT_ARRAY); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_UINT_ARRAY; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  uint32_t  m_numValues;
  uint32_t* m_values;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataUInt64Array
/// \brief An array of unsigned 64 bit integer values.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataUInt64Array : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numValues);

  static AttribDataUInt64Array* init(
    NMP::Memory::Resource& resource,
    uint32_t               numValues,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint32_t              numValues,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataUInt64Array() { setType(ATTRIB_TYPE_UINT64_ARRAY); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_UINT64_ARRAY; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  uint32_t  m_numValues;
  uint64_t* m_values;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataFloatArray
/// \brief An array of float values.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataFloatArray : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numValues);

  static AttribDataFloatArray* init(
    NMP::Memory::Resource& resource,
    uint32_t               numValues,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint32_t              numValues,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataFloatArray() { setType(ATTRIB_TYPE_FLOAT_ARRAY); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_FLOAT_ARRAY; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  uint32_t  m_numValues;
  float*    m_values;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataUpdatePlaybackPos
/// \brief For setting an absolute time position or updating via a delta time.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataUpdatePlaybackPos : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  
  static AttribDataUpdatePlaybackPos* init(
    NMP::Memory::Resource& resource,
    bool                   isFraction,
    bool                   isAbs,
    float                  value,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);
    
  static AttribDataHandle create(
    NMP::MemoryAllocator*  allocator,
    bool                   isFraction,
    bool                   isAbs,
    float                  value,
    uint16_t               refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataUpdatePlaybackPos() { setType(ATTRIB_TYPE_UPDATE_PLAYBACK_POS); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_UPDATE_PLAYBACK_POS; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(AttribData* source, AttribData* dest);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  /// \returns Value if already stored as a time or calculated time if stored as a fraction.
  NM_INLINE float getTime(float clipDuration);

  /// \returns Value if already stored as a fraction or calculated faction if stored as a time.
  NM_INLINE float getFraction(float clipDurationReciprocal);

  bool  m_isFraction; // This is an update via a fractional position.
  bool  m_isAbs;      // Are we setting via a delta or an absolute value.
  float m_value;      //
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float AttribDataUpdatePlaybackPos::getFraction(float clipDurationReciprocal)
{
  float result = m_value;
  NMP_ASSERT(m_value >= 0.0f);
  if (!m_isFraction)
    result = m_value * clipDurationReciprocal;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float AttribDataUpdatePlaybackPos::getTime(float clipDuration)
{
  float result = m_value;
  NMP_ASSERT(m_value >= 0.0f);
  if (m_isFraction)
    result = m_value * clipDuration;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPlaybackPos
/// \brief Current and last frame time positions.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPlaybackPos : public AttribData
{
public:  
  static NMP::Memory::Format getMemoryRequirements();
  
  static AttribDataPlaybackPos* init(
    NMP::Memory::Resource& resource,
    float                  currentPosAdj,
    float                  previousPosAdj,
    float                  currentPosReal,
    float                  previousPosReal,
    bool                   setWithAbs,
    bool                   inclusiveStart,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataPlaybackPos() { setType(ATTRIB_TYPE_PLAYBACK_POS); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PLAYBACK_POS; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(AttribData* source, AttribData* dest);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  /// \brief Set this instance as having been updated with absolute values.
  NM_INLINE void setWithAbs();

  /// \brief Set this instance as having been updated with delta values.
  NM_INLINE void setWithDelta() { m_setWithAbs = false; }

  /// \brief Indicates whether the start of the update range should be considered for event issuing purposes.
  NM_INLINE void setInclusiveStart(bool inclusiveStart) { m_inclusiveStart = inclusiveStart; }

  float m_currentPosAdj;   ///< This frames value. In owning nodes adjusted space
  float m_previousPosAdj;  ///< The last frames value. In owning nodes adjusted space
  float m_currentPosReal;  ///< This frames value. In owning nodes real space
  float m_previousPosReal; ///< The last frames value. In owning nodes real space

  float m_delta;           ///< This frames applied delta (Has been applied to m_currentPos this frame).
  bool  m_setWithAbs;      ///< Were we updated with an absolute or a delta this frame.
  bool  m_inclusiveStart;  ///< Whether the start of the interval should be considered or not.
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataPlaybackPos::setWithAbs()
{
  m_setWithAbs = true;

  // Set previous frames playback pos values to be the same as this frames values.
  m_previousPosAdj = m_currentPosAdj;
  m_previousPosReal = m_currentPosReal;

  // There can be no delta update when setting by abs.
  m_delta = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataUpdateSyncEventPlaybackPos
/// \brief For setting an absolute sync event position or updating via a delta time.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataUpdateSyncEventPlaybackPos : public AttribData
{
public: 
  static NMP::Memory::Format getMemoryRequirements();
  
  static AttribDataUpdateSyncEventPlaybackPos* init(
    NMP::Memory::Resource& resource,
    bool                   thisIsAbs,
    const SyncEventPos&    absPosAdj,
    const SyncEventPos&    absPosReal,
    const SyncEventPos&    deltaPos,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc = NULL);
    
  static AttribDataHandle create(
    NMP::MemoryAllocator*  allocator,
    bool                   thisIsAbs,
    const SyncEventPos&    absPosAdj,
    const SyncEventPos&    absPosReal,
    const SyncEventPos&    deltaPos,
    uint16_t               refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataUpdateSyncEventPlaybackPos() { setType(ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(AttribData* source, AttribData* dest);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  /// \brief Set this instance as having been updated with absolute values.
  NM_INLINE void setWithAbs();

  /// \brief Set this instance as having been updated with delta values.
  NM_INLINE void setWithDelta() { m_isAbs = false; }

  bool         m_isAbs;      ///< Are we setting via a delta or an absolute value.
  SyncEventPos m_absPosAdj;  ///< In nodes adjusted space.
                             ///<  If this is a delta = Absolute position achieved after update by the delta value below.
  SyncEventPos m_absPosReal; ///< In nodes real space.
  SyncEventPos m_deltaPos;   ///< If this is an abs not used.
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataUpdateSyncEventPlaybackPos::setWithAbs()
{
  m_isAbs = true;

  // There can be no delta update when setting by abs.
  m_deltaPos.set(0, 0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::EmittedMessageMap
/// \brief Emitted Transition Messages.
//----------------------------------------------------------------------------------------------------------------------
struct EmittedMessageMap
{
  /// These are the local message IDs that a node can emit.
  /// When writing a node task or an immediate update function that emits messages these are the IDs that will be used.
  /// The node writer can cast these to appear as something more meaningful in code
  /// e.g. #define JUMP_EVENT_REQUEST_ID REQUEST0 
  /// or 
  /// const uint32 jumpEventRequestID = REQUEST0;
  /// These IDs get mapped to global state machine message IDs.
  enum Names
  {
    REQUEST0  = (1 << 0),
    REQUEST1  = (1 << 1),
    REQUEST2  = (1 << 2),
    REQUEST3  = (1 << 3),
    REQUEST4  = (1 << 4),
    REQUEST5  = (1 << 5),
    REQUEST6  = (1 << 6),
    REQUEST7  = (1 << 7),

    REQUEST8  = (1 << 8),
    REQUEST9  = (1 << 9),
    REQUEST10 = (1 << 10),
    REQUEST11 = (1 << 11),
    REQUEST12 = (1 << 12),
    REQUEST13 = (1 << 13),
    REQUEST14 = (1 << 14),
    REQUEST15 = (1 << 15),

    REQUEST16 = (1 << 16),
    REQUEST17 = (1 << 17),
    REQUEST18 = (1 << 18),
    REQUEST19 = (1 << 19),
    REQUEST20 = (1 << 20),
    REQUEST21 = (1 << 21),
    REQUEST22 = (1 << 22),
    REQUEST23 = (1 << 23),

    REQUEST24 = (1 << 24),
    REQUEST25 = (1 << 25),
    REQUEST26 = (1 << 26),
    REQUEST27 = (1 << 27),
    REQUEST28 = (1 << 28),
    REQUEST29 = (1 << 29),
    REQUEST30 = (1 << 30),
    REQUEST31 = (1 << 31),
  };

  MessageID messageID;
  NodeID    stateMachineNodeID;
};


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataEmittedMessagesMap
/// \brief Per Node Emitted Message Mappings.
/// \ingroup AttribData
///
/// + Definition data generated in asset compiler.
/// + DataBuffer with 1 element array of type EmittedMessages (REQUEST…).
/// + 1 array entry per mapping.
/// + Each entry index corresponds to an {::Names entry.
/// + Because an emitted bit value is used to find a corresponding entry in this mapping, it cannot be sparse.
///   There do not have to be 32 entries, but if for example you want to emit 4 message types from a node there 
///   will be 4 entries corresponding to REQUEST0, REQUEST1, REQUEST2 REQUEST3. We could not emit a REQUEST9 
///   without having mapping entries from 0 through to 9.
//----------------------------------------------------------------------------------------------------------------------
class AttribDataEmittedMessagesMap : public AttribData
{
public:
  // Note: Lua manifest node export code assumes this order. If a change is made it must be made in both places.
  enum RequestType
  {
    UNUSED = 0,
    SET,
    CLEAR,
    RESET,
    EXTERNAL,
    NUM_REQUEST_TYPES
  };

  /// Memory requirements of an emitted message mapping.
  static NMP::Memory::Format getMemoryRequirements(uint32_t numEntries);

  /// Initialisation of an emitted message mapping.
  static AttribDataEmittedMessagesMap* createAndInit(NMP::MemoryAllocator* allocator, uint32_t numEntries, uint16_t refCount);
    
  /// Creation of an emitted message mapping.
  static AttribDataEmittedMessagesMap* init(NMP::Memory::Resource& resource, uint32_t numEntries, uint16_t refCount);

  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_EMITTED_MESSAGE_MAP; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target);

  NM_INLINE const EmittedMessageMap* getEmittedMessageEntry(uint32_t index) const
  {
    NMP_ASSERT(index < m_numEmittedMessage);
    return &m_emittedMessages[index];
  }

  NM_INLINE uint32_t getEmittedMessageMapLength() const
  {
    return m_numEmittedMessage;
  }

  NM_INLINE bool isEntryUsed(uint32_t index) const
  {
    return m_messageUsed[index];
  }

  void setEmittedMessage(
    uint32_t    index,
    RequestType requestType,
    NodeID      targetStateMachineID,
    MessageID   messageID);

private:
  NM_INLINE AttribDataEmittedMessagesMap(uint32_t numEmittedMessages, uint16_t refCount) :
    m_numEmittedMessage(numEmittedMessages)
  {
    setType(ATTRIB_TYPE_EMITTED_MESSAGE_MAP);
    setRefCount(refCount);
  }
  ~AttribDataEmittedMessagesMap() {}

  const AttribDataEmittedMessagesMap& operator = (const AttribDataEmittedMessagesMap&) {NMP_ASSERT_FAIL(); return *this;}
  
  uint32_t           m_numEmittedMessage;
  EmittedMessageMap* m_emittedMessages;
  bool*              m_messageUsed;
};

//----------------------------------------------------------------------------------------------------------------------
class AttribDataTransformBufferCreateDesc : public AttribDataCreateDesc
{
public:
  uint32_t m_size;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTransformBuffer
/// \brief Transform buffer.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataTransformBuffer : public AttribData
{
public:
  /// General case creation of a transform buffer. Specialised versions are faster.
  static AttribDataHandle create(
    NMP::MemoryAllocator*                     allocator,
    const NMP::Memory::Format&                memReqs,
    const NMP::Memory::Format&                internalBufferMemReqs,
    uint32_t                                  numElements,
    const NMP::DataBuffer::ElementDescriptor* elements,
    uint32_t                                  numEntries,
    uint16_t                                  refCount = 0);

  /// Specialised creation of a PosQuat transform buffer.
  static AttribDataTransformBuffer* createPosQuat(
    NMP::MemoryAllocator*      allocator,
    const NMP::Memory::Format& memReqs,
    const NMP::Memory::Format& internalBufferMemReqs,
    uint32_t                   numEntries,
    uint16_t                   refCount = 0);

  /// Specialised creation of a PosVelAngVel transform buffer.
  static AttribDataTransformBuffer* createPosVelAngVel(
    NMP::MemoryAllocator*      allocator,
    const NMP::Memory::Format& memReqs,
    const NMP::Memory::Format& internalBufferMemReqs,
    uint32_t                   numEntries,
    uint16_t                   refCount = 0);

  /// Memory requirements of a general case transform buffer. Specialised versions are faster.
  static void getMemoryRequirements(
    uint32_t                                  numElements,
    const NMP::DataBuffer::ElementDescriptor* elements,
    uint32_t                                  numEntries,
    NMP::Memory::Format&                      bufferMemReqs,
    NMP::Memory::Format&                      internalBufferMemReqs);

  /// Memory requirements of a specialised PosQuat transform buffer.
  static void getPosQuatMemoryRequirements(
    uint32_t             numEntries,
    NMP::Memory::Format& buffMemReqs,
    NMP::Memory::Format& internalBuffMemReqs);

  /// Memory requirements of a specialised PosVelAngVel transform buffer.
  static void getPosVelAngVelMemoryRequirements(
    uint32_t             numEntries,
    NMP::Memory::Format& bufferMemReqs,
    NMP::Memory::Format& internalBufferMemReqs);

  /// General case initialisation of a transform buffer. Specialised versions are faster.
  static AttribDataTransformBuffer* init(
    NMP::Memory::Resource&                    resource,
    const NMP::Memory::Format&                internalBufferMemReqs,
    uint32_t                                  numElements,
    const NMP::DataBuffer::ElementDescriptor* elements,
    uint32_t                                  numEntries,
    uint16_t                                  refCount = 0);

  /// Specialised initialisation of a PosQuat transform buffer.
  static AttribDataTransformBuffer* initPosQuat(
    NMP::Memory::Resource&     resource,
    const NMP::Memory::Format& internalBufferMemReqs,
    uint32_t                   numEntries,
    uint16_t                   refCount = 0);

  /// Specialised initialisation of a PosVelAngVel transform buffer.
  static AttribDataTransformBuffer* initPosVelAngVel(
    NMP::Memory::Resource&     resource,
    const NMP::Memory::Format& internalBufferMemReqs,
    uint32_t                   numEntries,
    uint16_t                   refCount = 0);

  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);

  static NMP::Memory::Format outputMemReqs(Network* network);

  // Find the requirements of an instanced transform buffer.
  NMP::Memory::Format getInstanceMemoryRequirements();

  NM_INLINE AttribDataTransformBuffer() { setType(ATTRIB_TYPE_TRANSFORM_BUFFER); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_TRANSFORM_BUFFER; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  NMP::DataBuffer* m_transformBuffer;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTrajectoryDeltaTransform
/// \brief The trajectory channels delta transform for this frame.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataTrajectoryDeltaTransform : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataTrajectoryDeltaTransform* init(
    NMP::Memory::Resource& resource,
    const NMP::Quat&       attitude,
    const NMP::Vector3&    position,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);

  static AttribDataHandle create(NMP::MemoryAllocator* allocator);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataTrajectoryDeltaTransform() { setType(ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  bool          m_filteredOut;  ///< This attribute does not contain valid trajectory data because the node that produced it
                                ///< does not contain the trajectory bone.
  char          m_pad[7];
#endif

  NMP::Quat     m_deltaAtt;     ///< Attitude delta.
  NMP::Vector3  m_deltaPos;     ///< Position delta.

#ifdef NM_HOST_64_BIT
  bool          m_filteredOut;  ///< This attribute does not contain valid trajectory data because the node that produced it
                                ///< does not contain the trajectory bone.
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTransform
/// \brief A single transform stored as an NMP::Quat and an NMP::Vector3.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataTransform : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  
  static AttribDataTransform* init(
    NMP::Memory::Resource& resource,
    const NMP::Quat&       attitude,
    const NMP::Vector3&    position,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);
    
  static AttribDataHandle create(
    NMP::MemoryAllocator*  allocator,
    const NMP::Quat&       attitude,
    const NMP::Vector3&    position,
    uint16_t               refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataTransform() { setType(ATTRIB_TYPE_TRANSFORM); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_TRANSFORM; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  uint32_t m_pad[2];
#endif

  NMP::Vector3 m_pos;   ///< Position.
  NMP::Quat    m_att;   ///< Attitude.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataVelocity
/// \brief Linear and angular velocity.
/// \ingroup AttribData
// MORPH-21338: Remove this and use AttribDataTransform in its place.
//----------------------------------------------------------------------------------------------------------------------
class AttribDataVelocity : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataVelocity* init(
    NMP::Memory::Resource& resource,
    const NMP::Vector3&    velocity,
    const NMP::Vector3&    angularVelocty,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc = NULL);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    const NMP::Vector3&   velocity,
    const NMP::Vector3&   angularVelocity,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataVelocity() { setType(ATTRIB_TYPE_VELOCITY); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_VELOCITY; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t bytesRemaining);
#endif // MR_OUTPUT_DEBUGGING

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  uint32_t m_pad[2];
#endif

  NMP::Vector3  m_linearVel;  ///< Linear Velocity.
  NMP::Vector3  m_angularVel; ///< Angular Velocity.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataSyncEventTrack
/// \brief Synchronisation event track.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataSyncEventTrack : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataSyncEventTrack* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);
  
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataSyncEventTrack() { setType(ATTRIB_TYPE_SYNC_EVENT_TRACK); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SYNC_EVENT_TRACK; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t bytesRemaining);
#endif // MR_OUTPUT_DEBUGGING

  EventTrackSync m_syncEventTrack;

  /// This is important in transitions from transitions so that the author can continue to specify events in the
  /// original source nodes event space.
  int32_t        m_transitionOffset;
};

//----------------------------------------------------------------------------------------------------------------------
class AttribDataSampledEventsCreateDesc : public AttribDataCreateDesc
{
public:
  NM_FORCEINLINE AttribDataSampledEventsCreateDesc(
    uint32_t numDiscreteEvents,
    uint32_t numCurveEvents) : m_numDiscreteEvents(numDiscreteEvents), m_numCurveEvents(numCurveEvents)
  {
  }

public:
  uint32_t  m_numDiscreteEvents;
  uint32_t  m_numCurveEvents;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataSampledEvents
/// \brief Buffer of sampled events.
///        Runtime only structure.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataSampledEvents : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t                numDiscreteEvents,
    uint32_t                numCurveEvents);
    
  static AttribDataSampledEvents* init(
    NMP::Memory::Resource&  resource,
    uint32_t                numDiscreteEvents,
    uint32_t                numCurveEvents,
    uint16_t                refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);
  
  static AttribDataHandle create(
    NMP::MemoryAllocator*   allocator,
    uint32_t                numDiscreteEvents,
    uint32_t                numCurveEvents,
    uint16_t                refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataSampledEvents() { setType(ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER; }

  /// For Manager registration.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t bytesRemaining);
#endif // MR_OUTPUT_DEBUGGING

  TriggeredDiscreteEventsBuffer* m_discreteBuffer;  ///< Buffer to put discrete event samples into.
  SampledCurveEventsBuffer*      m_curveBuffer;     ///< Buffer to put curve event samples into.
};

//----------------------------------------------------------------------------------------------------------------------
class AttribDataDurationEventTrackSetCreateDesc : public AttribDataCreateDesc
{
public:
  NM_FORCEINLINE AttribDataDurationEventTrackSetCreateDesc(
    uint32_t numEventTracks,
    uint32_t eventPoolSize) : m_numEventTracks(numEventTracks), m_eventPoolSize(eventPoolSize)
  {
  }

public:
  uint32_t  m_numEventTracks;
  uint32_t  m_eventPoolSize;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataDurationEventTrackSet
/// \brief A set of MR::DurationEventTrack s. Internally manages pooling of events.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataDurationEventTrackSet : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numEventTracks,
    uint32_t eventPoolSize);
    
  static AttribDataDurationEventTrackSet* init(
    NMP::Memory::Resource&  resource,
    uint32_t                numEventTracks,
    uint32_t                eventPoolSize,
    uint16_t                refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);

  static AttribDataHandle create(
    NMP::MemoryAllocator*   allocator,
    uint32_t                numEventTracks,
    uint32_t                eventPoolSize,
    uint16_t                refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);
  
  /// \brief Find the requirements of an instanced event track set.
  NMP::Memory::Format getInstanceMemoryRequirements();

  /// \brief Find the requirements of an instanced event track set, that has been stripped of all excess entries in
  ///        the pool.
  NMP::Memory::Format getPackedInstanceMemoryRequirements();

  /// \brief Copy this AttribDataDurationEventTrackSet stripped of all excess entries in the pool.
  AttribDataDurationEventTrackSet* copyAndPack(
    NMP::Memory::Resource& resource,
    NMP::MemoryAllocator*  allocator,
    uint16_t               refCount = 0);

  /// \brief Copy this AttribDataDurationEventTrackSet stripped of all excess entries in the pool.
  AttribDataDurationEventTrackSet* copyAndPack(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataDurationEventTrackSet() { setType(ATTRIB_TYPE_DURATION_EVENT_TRACK_SET); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_DURATION_EVENT_TRACK_SET; }

  /// For Manager registration.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  EventTrackDurationSet* m_durationEventTrackSet;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataSourceEventTrackSet
/// \brief A set of MR::EventTrackDefDiscrete s. Generally only source nodes like the AnimSourceNode will have this
///  kind of def data.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataSourceEventTrackSet : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numDiscreteEventTracks, 
    uint32_t numCurveEventTracks, 
    uint32_t numDurationEventTracks);

  static AttribDataSourceEventTrackSet* init(
    NMP::Memory::Resource& resource,
    uint32_t               numDiscreteEventTracks,
    uint32_t               numCurveEventTracks,
    uint32_t               numDurationEventTracks,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataSourceEventTrackSet() { setType(ATTRIB_TYPE_SOURCE_EVENT_TRACKS); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SOURCE_EVENT_TRACKS; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
  static void prepDiscreteEventTrackSetForSPU(AttribData* target, NMP::MemoryAllocator* allocator);
  static void prepCurveEventTrackSetForSPU(AttribData* target, NMP::MemoryAllocator* allocator);
  static void prepDurationEventTrackSetForSPU(AttribData* target, NMP::MemoryAllocator* allocator);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  uint32_t                m_numDiscreteEventTracks;
  EventTrackDefDiscrete** m_sourceDiscreteEventTracks;
  uint32_t*               m_sourceDiscreteEventTrackSizes;  ///< Memory requirements of each referenced event track.

  uint32_t                m_numCurveEventTracks;
  EventTrackDefCurve**    m_sourceCurveEventTracks;         ///< Pointer into shared array of resources.
  uint32_t*               m_sourceCurveEventTrackSizes;     ///< Memory requirements of each referenced event track.

  uint32_t                m_numDurEventTracks;
  EventTrackDefDuration** m_sourceDurEventTracks;           ///< Pointer into shared array of resources.
  uint32_t*               m_sourceDurEventTrackSizes;       ///< Memory requirements of each referenced event track.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTwoBoneIKSetup
/// \brief Animation set independent definition data specific to the TwoBoneIKNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataTwoBoneIKSetup : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataTwoBoneIKSetup* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataTwoBoneIKSetup() { setType(ATTRIB_TYPE_TWO_BONE_IK_SETUP); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_TWO_BONE_IK_SETUP; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

#ifndef NM_HOST_64_BIT
  // This padding ensures we get consistent packing across all 64-bit compilers.
  uint32_t      m_pad[2];
#endif

  /// Defaults used if control params not used.
  NMP::Quat     m_defaultTargetOrientation;

  bool          m_assumeSimpleHierarchy;
  bool          m_keepEndEffOrientation;
  bool          m_userControlledOrientation;
  bool          m_updateTargetByDeltas;
  /// \brief If true, the target cps are specified in world space rather than character space.
  bool          m_worldSpaceTarget;
  bool          m_useSpecifiedJointAsTarget;
  bool          m_useSpecifiedJointOrientation;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTwoBoneIKChain
/// \brief Animation set dependent definition data specific to the TwoBoneIKNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataTwoBoneIKChain : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataTwoBoneIKChain* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataTwoBoneIKChain() { setType(ATTRIB_TYPE_TWO_BONE_IK_CHAIN); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_TWO_BONE_IK_CHAIN; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);


  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  uint32_t     m_endJointIndex;
  uint32_t     m_midJointIndex;
#endif

  NMP::Vector3  m_midJointRotAxis;
  NMP::Vector3  m_midJointReferenceAxis;

#ifdef NM_HOST_64_BIT
  uint32_t     m_endJointIndex;
  uint32_t     m_midJointIndex;
#endif

  uint32_t     m_rootJointIndex;

  uint32_t     m_targetJointIndex;

  bool         m_globalReferenceAxis;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataLockFootSetup
/// \brief Animation set independent definition data specific to the LockFootNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataLockFootSetup : public AttribData
{
public:
  static AttribDataLockFootSetup* create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataLockFootSetup* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataLockFootSetup() { setType(ATTRIB_TYPE_LOCK_FOOT_SETUP); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_LOCK_FOOT_SETUP; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  uint32_t     m_upAxis; ///< Specifies up as x, y or z axis using an index.
  bool         m_lockVerticalMotion; ///< True if locking all motion not just horizontal motion.
  bool         m_assumeSimpleHierarchy; ///< All joints are sequential in the hierarchy (currently must be true).
  bool         m_trackCharacterController; ///< Use CC for world trajectory, rather than accumulating deltas.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataLockFootChain
/// \brief Animation set dependent definition data specific to the LockFootNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataLockFootChain : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataLockFootChain* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataLockFootChain() { setType(ATTRIB_TYPE_LOCK_FOOT_CHAIN); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_LOCK_FOOT_CHAIN; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  float        m_lowerHeightBound;
  float        m_toeLowerHeightBound;
#endif

  NMP::Vector3 m_kneeRotationAxis;
  NMP::Vector3 m_ballRotationAxis;
  NMP::Vector3 m_footLevelVector;

#ifdef NM_HOST_64_BIT
  float        m_lowerHeightBound;
  float        m_toeLowerHeightBound;
#endif
  
  int32_t      m_jointIndex[5];
  int32_t      m_footfallEventID;

  /// \brief An angle in cos-half-radians specifying the offset between the lock orientation and the
  /// desired orientation that will force the foot to pivot, if foot orientation fixup is being used.
  float        m_footPivotOffsetThreshold;

  float        m_catchUpSpeedFactor;
  float        m_snapToSourceDistance;
  float        m_straightestLegFactor;
  float        m_ankleLowerHeightBound;
  bool         m_fixToeGroundPenetration;
  bool         m_useBallJoint;
  bool         m_fixGroundPenetration;
  bool         m_fixFootOrientation;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataLockFootState
/// \brief Active state of a LockFootNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataLockFootState : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataLockFootState* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataLockFootState() { setType(ATTRIB_TYPE_LOCK_FOOT_STATE); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_LOCK_FOOT_STATE; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static bool copy(AttribData* source, AttribData* dest);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  AttribDataTransform m_trajectoryTransform;
  NMP::Vector3        m_previousEndEffectorPosition;
  NMP::Quat           m_previousEndJointOrientation;
  NMP::Vector3        m_lockPosition;
  NMP::Quat           m_lockOrientation;
  bool                m_tracking;
  bool                m_previouslyInFootStep;
  bool                m_firstFootStep;
  float               m_footRollTime;
  float               m_footCatchUpTime;
  float               m_footPivotTime;
  bool                m_pivotIsLocked;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataHipsIKDef
/// \brief Definition data specific to the HipsIKNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataHipsIKDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataHipsIKDef* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataHipsIKDef() { setType(ATTRIB_TYPE_HIPS_IK_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_HIPS_IK_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  float m_swivelOrientationWeight;

  bool m_localReferenceFrame;
  bool m_keepWorldFootOrientation;

  uint16_t m_padidng;
#endif

  NMP::Vector3 m_upAxis;

#ifdef NM_HOST_64_BIT
  float m_swivelOrientationWeight;

  bool m_localReferenceFrame;
  bool m_keepWorldFootOrientation;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataHipsIKAnimSetDef
/// \brief Animation set dependent definition data specific to the HipsIKNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataHipsIKAnimSetDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataHipsIKAnimSetDef* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataHipsIKAnimSetDef() { setType(ATTRIB_TYPE_HIPS_IK_ANIM_SET_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_HIPS_IK_ANIM_SET_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  int32_t      m_hipsIndex;
  bool         m_useBallJoints;
  bool         m_pad[3];
#endif

  NMP::Vector3 m_leftKneeRotationAxis;
  NMP::Vector3 m_rightKneeRotationAxis;
  int32_t      m_leftLegIndex[4];
  int32_t      m_rightLegIndex[4];

#ifdef NM_HOST_64_BIT
  int32_t      m_hipsIndex;
  bool         m_useBallJoints;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBasicUnevenTerrainSetup
/// \brief Non anim set specific attribute data setup
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBasicUnevenTerrainSetup : public AttribData
{
public:
  static AttribDataBasicUnevenTerrainSetup* create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataBasicUnevenTerrainSetup* init(
    NMP::Memory::Resource& resource,
    uint32_t               numLimbs,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataBasicUnevenTerrainSetup() { setType(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_SETUP); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_SETUP; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

public:
  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  uint32_t                          m_numLimbs;
  uint32_t                          m_pad;
#endif

  NMP::Vector3                      m_upAxis; // In the space of the character root

#ifdef NM_HOST_64_BIT
  uint32_t                          m_numLimbs;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBasicUnevenTerrainChain
/// \brief Animation set specific uneven terrain IK chain setup information
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBasicUnevenTerrainChain : public AttribData
{
public:
  class ChainData
  {
  public:
    static NMP::Memory::Format getMemoryRequirements();
    static ChainData* init(NMP::Memory::Resource& resource);

    static void locate(ChainData* target);
    static void dislocate(ChainData* target);
    static ChainData* relocate(void*& ptr);

  public:
    // Leg IK solver options
    NMP::Vector3                    m_kneeRotationAxis;    
    uint32_t                        m_channelIDs[5];
    uint32_t                        m_numFootJoints;    
  };

public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numLimbs);

  static AttribDataBasicUnevenTerrainChain* init(
    NMP::Memory::Resource& resource,
    uint32_t               numLimbs,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataBasicUnevenTerrainChain() { setType(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_CHAIN); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_CHAIN; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

public:
  // Hips channel ID for the hip height control
  uint32_t                          m_hipsChannelID;

  // Hips IK position limit clamping
  bool                              m_hipsHeightControlEnable;
  bool                              m_hipsPosVelLimitEnable;
  bool                              m_hipsPosAccelLimitEnable;
  float                             m_hipsPosVelLimit;  // Character root space
  float                             m_hipsPosAccelLimit; // Character root space

  // Leg IK solver options
  float                             m_straightestLegFactor;

  // Leg IK end joint limit clamping
  bool                              m_endJointPosVelLimitEnable;
  bool                              m_endJointPosAccelLimitEnable;
  bool                              m_endJointAngVelLimitEnable;
  bool                              m_endJointAngAccelLimitEnable;
  float                             m_endJointPosVelLimit;                ///< Limit in Character root space
  float                             m_endJointPosAccelLimit;              ///< Limit in Character root space
  float                             m_endJointAngVelLimit;                ///< Limit in Joint local space
  float                             m_endJointAngAccelLimit;              ///< Limit in Joint local space 

  // Foot alignment with the terrain surface options
  bool                              m_useGroundPenetrationFixup;          ///< Clamps the foot back onto the surface if the foot penetrates.
  bool                              m_useTrajectorySlopeAlignment;
  float                             m_footAlignToSurfaceAngleLimit;       ///< The maximum terrain surface angle limit (cosine of angle) that the foot can be aligned with.
                                                                          ///< Foot alignment is clamped to this limit.
  float                             m_footAlignToSurfaceMaxSlopeAngle;    ///< The maximum terrain slope gradient limit (cosine of angle) that is considered for foot alignment.
                                                                          ///< The foot is aligned back to the ground plane if beyond this limit.

  // Foot lifting options
  float                             m_footLiftingHeightLimit;             ///< Spatial limit on the maximum end joint pos delta between frames

  // IK chain setup data for each limb
  uint32_t                          m_numLimbs;
  ChainData*                        m_chainInfo[2];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBasicUnevenTerrainIKSetup
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBasicUnevenTerrainIKSetup : public AttribData
{
public:
  class LimbState
  {
  public:
    static NMP::Memory::Format getMemoryRequirements();
    static LimbState* init(NMP::Memory::Resource& resource);

    static LimbState* relocate(void*& ptr);

  public:
    // Leg IK information (source animation configuration)
    NMP::Matrix34                     m_rootParentWorldJointTM;
    NMP::Matrix34                     m_rootWorldJointTM;
    NMP::Matrix34                     m_midWorldJointTM;
    NMP::Matrix34                     m_endWorldJointTM;

    // Foot IK information (source animation configuration)
    NMP::Vector3                      m_localFootPivotPos; ///< Foot pivot position in the frame of the ankle joint

    // Footbase information (source animation configuration). The footbase is defined as the area hull
    // that results from projecting the foot object vertically onto the ground plane. The footbase for
    // complex terrain is approximated by a planar patch and is modeled by using a point and surface
    // normal. i.e. the footbase point results from projecting the foot pivot point onto the terrain
    // surface. Note that the footbase is not the same as the sole of the foot. In the source animation
    // configuration the footbase plane is the character root ground plane, with surface normal in the
    // direction of the world up axis vector.
    NMP::Vector3                      m_initWorldFootbasePos;     ///< Foot pivot projected onto the character root ground plane.
    float                             m_footbaseDistFromPivotPos; ///< The vertical distance of the footbase from the foot pivot.

    // Leg IK information (cont)
    float                             m_straightestLegLength;
  };

public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numLimbs);

  static AttribDataBasicUnevenTerrainIKSetup* init(
    NMP::Memory::Resource& resource,
    uint32_t               numLimbs,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataBasicUnevenTerrainIKSetup() { setType(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_SETUP); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_SETUP; }

  /// For Manager registration.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

public:
#ifndef NM_HOST_64_BIT
  float                             m_straightestLegLength;     ///< Average of straightest leg lengths
  float                             m_rootToHipsHeight;         ///< Average height from the character root to the hips (IK root)
#endif

  // Character controller world root TM
  NMP::Matrix34                     m_characterRootWorldTM;

  // Hips IK information (source animation configuration)
  NMP::Matrix34                     m_hipsParentWorldJointTM;
  NMP::Matrix34                     m_hipsWorldJointTM;

  // Foot offset rotation that aligns the inclined trajectory ground plane with the canonical ground plane
  NMP::Quat                         m_footPreAlignOffsetQuat;

  // IK state information
#ifdef NM_HOST_64_BIT
  float                             m_straightestLegLength;     ///< Average of straightest leg lengths
  float                             m_rootToHipsHeight;         ///< Average height from the character root to the hips (IK root)
#endif

  bool                              m_enableIK;                 ///< Character controller achieved its requested motion + on ground

  // Limb data
  uint32_t                          m_numLimbs;
  LimbState*                        m_limbStates[2];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBasicUnevenTerrainFootLiftingTarget
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBasicUnevenTerrainFootLiftingTarget : public AttribData
{
public:
  class LimbData
  {
  public:
    static NMP::Memory::Format getMemoryRequirements();
    static LimbData* init(NMP::Memory::Resource& resource);

    static LimbData* relocate(void*& ptr);

  public:
    NMP::Vector3                    m_targetWorldFootbaseTerrainPos;        ///< The target footbase point projected onto the surface approximation for the terrain.
                                                                            ///< Note that this should be the actual ray-cast hit position or the plane projected
                                                                            ///< approximation of it based on the last valid hit position. It will be used for ground
                                                                            ///< penetration fix-up. Is valid based on the m_targetWorldFootbaseLiftingValid flag.
    NMP::Vector3                    m_targetWorldFootbaseLiftingPos;        ///< The target footbase point for foot lifting (footbase point on foot flight curve).
                                                                            ///< Is valid based on the m_targetWorldFootbaseLiftingValid flag.
    NMP::Vector3                    m_targetWorldFootbaseLiftingNormal;     ///< The target footbase terrain normal (terrain - foot flight blended normal?)
                                                                            ///< Is valid based on the m_targetWorldFootbaseLiftingValid flag. 
    bool                            m_targetWorldFootbaseLiftingValid;      ///< Flag indicating if the terrain foot lifting target is valid.

    bool                            m_isFootInContactPhase;                 ///< Flag indicating if the foot is in a contact phase
  };

public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numLimbs);

  static AttribDataBasicUnevenTerrainFootLiftingTarget* init(
    NMP::Memory::Resource& resource,
    uint32_t               numLimbs,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataBasicUnevenTerrainFootLiftingTarget() { setType(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET; }

  /// For manager registration.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

public:
  // Limb data
  uint32_t                          m_numLimbs;
  LimbData*                         m_limbInfo[2];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBasicUnevenTerrainIKState
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBasicUnevenTerrainIKState : public AttribData
{
public:
  class LimbState
  {
  public:
    static NMP::Memory::Format getMemoryRequirements();
    static LimbState* init(NMP::Memory::Resource& resource);
    static LimbState* relocate(void*& ptr);

  public:
    // Leg foot lifting IK state (worldspace)
    NMP::Matrix34                     m_targetWorldAnkleJointTM;          ///< The target ankle transform for positioning and aligning the foot onto the terrain surface.
    NMP::Matrix34                     m_drivenWorldAnkleJointTM;          ///< The driven ankle transform after foot lifting and motion clamping.
    NMP::Vector3                      m_drivenWorldFootbaseLiftingPos;    ///< The driven footbase point for foot lifting. (i.e. should be the target terrain ray-cast hit position)
                                                                          ///< Note that this point is always aligned vertically with the foot pivot point. Consider the footbase
                                                                          ///< as being the plane a distance d below the foot pivot point. The foot is rotated about the pivot for
                                                                          ///< terrain surface alignment, however the footbase lifting pos can be seen as the intersection point
                                                                          ///< derived by projecting downwards from the pivot point onto the rotated footbase plane.
    NMP::Vector3                      m_drivenWorldFootbaseLiftingNormal; ///< The driven footbase terrain normal (i.e. should be the target terrain ray-cast hit normal).
    NMP::Vector3                      m_targetWorldFootbaseLiftingPos;    ///< The target footbase point for foot lifting (i.e. the terrain hit position).

    // End joint position clamping (character space)
    NMP::Vector3                      m_drivenEndJointPosCS;
    NMP::Vector3                      m_drivenEndJointPosVelCS;
    NMP::Vector3                      m_targetEndJointPosCS;
    NMP::Vector3                      m_targetEndJointPosVelCS;

    // End joint orientation clamping (joint local space)
    NMP::Quat                         m_drivenEndJointQuat;
    NMP::Vector3                      m_drivenEndJointAngVel;
    NMP::Quat                         m_targetEndJointQuat;
    NMP::Vector3                      m_targetEndJointAngVel;
  };

public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numLimbs);

  static AttribDataBasicUnevenTerrainIKState* init(
    NMP::Memory::Resource& resource,
    uint32_t               numLimbs,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataBasicUnevenTerrainIKState() { setType(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_STATE); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_STATE; }

  /// For manager registration.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

public:
#ifndef NM_HOST_64_BIT
  bool                              m_pad[3];
  bool                              m_isValidData;
  uint32_t                          m_numLimbs;
#endif

  // Hips joint position clamping (character space)
  NMP::Vector3                      m_drivenHipsPosCS;
  NMP::Vector3                      m_drivenHipsPosVelCS;
  NMP::Vector3                      m_targetHipsPosCS;
  NMP::Vector3                      m_targetHipsPosVelCS;

#ifdef NM_HOST_64_BIT
  bool                              m_isValidData;
  uint32_t                          m_numLimbs;
#endif

  // Limb data
  LimbState*                        m_limbStates[2];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPredictiveUnevenTerrainPredictionDef
/// \brief Animation set specific uneven terrain setup information
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPredictiveUnevenTerrainPredictionDef : public AttribData
{
public:
  class LimbData
  {
  public:
    static NMP::Memory::Format getMemoryRequirements();

    static LimbData* init(NMP::Memory::Resource& resource);

    static void locate(LimbData* target);
    static void dislocate(LimbData* target);
    static LimbData* relocate(void*& ptr);

  public:
    // Foot alignment with the terrain surface options
    uint32_t                        m_eventTrackID;
  };

public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numLimbs);

  static AttribDataPredictiveUnevenTerrainPredictionDef* init(
    NMP::Memory::Resource& resource,
    uint32_t               numLimbs,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataPredictiveUnevenTerrainPredictionDef() { setType(ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_PREDICTION_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_PREDICTION_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

public:
  // Foot lifting options dependent on the character trajectory
  float                             m_footLiftingSlopeAngleLimit;   ///< Tangent of the maximum lifting slope angle limit
  float                             m_footLiftingLateralAngleLimit; ///< Tangent of the maximum lateral support angle limit (changes in trajectory)
  float                             m_closeFootbaseTolFrac;         ///< Fraction of the average straightest leg length for predicted
                                                                    ///< mid and next footbase points to be within proximity of cur

  // Limb data
  uint32_t                          m_numLimbs;
  LimbData*                         m_limbInfo[2];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPredictiveUnevenTerrainPredictionState
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPredictiveUnevenTerrainPredictionState : public AttribData
{
public:
  // Gait phase enumerations
  enum UnevenTerrainGaitPhase
  {
    kPrevContact,
    kInitialSwing,
    kTerminalSwing,
    kNextContact,
    kNumPhaseTypes,
    kInvalidPhase = 0xFFFFFFFF
  };

public:
  class LimbState
  {
  public:
    static NMP::Memory::Format getMemoryRequirements();
    static LimbState* init(NMP::Memory::Resource& resource);

    static LimbState* relocate(void*& ptr);

    void resetFootCycleTiming();
    void resetFootCyclePrediction();

  public:
    // State initialisation information
    NMP::Vector3                      m_initNextWorldTrajectoryPos; ///< Predicted character root position at the next footplant event (source anim config)
    NMP::Vector3                      m_initMidWorldTrajectoryPos;  ///< Predicted character root position at the mid remaining time between the current and the next gait cycle.

    NMP::Vector3                      m_initNextWorldFootbasePos;   ///< Predicted foot position at the next footplant event (source anim config)
    NMP::Vector3                      m_initMidWorldFootbasePos;    ///< Predicted foot position at the mid remaining time between the current and the next gait cycle.
                                                                    ///< This is used to alternate ray casting between, current, mid and next foot cycle positions
                                                                    ///< for predictive foot lifting. This is the mid time remaining trajectory position with the
                                                                    ///< next footplant character pose. See comments in the trajectory prediction step for why this is.

    // Foot cycle prediction
    FootCyclePredictor                m_footCyclePredictor;

    // Event markup
    uint32_t                          m_prevSyncEventPos;           ///< Previous marked sync event index
    uint32_t                          m_nextSyncEventPos;           ///< Next marked sync event index
    UnevenTerrainGaitPhase            m_curLimbPhase;               ///< This frames gait phase index
    bool                              m_hasFootCycleLooped;         ///< Flag indicating a transition into a new gait cycle

    // Time to event positions relative to current sync event position
    float                             m_timeToPrevSyncEvent;        ///< Time to the previous footplant event
    float                             m_timeToPrevEnd;              ///< Time to the end of the previous contact phase
    float                             m_timeToMid;                  ///< Time to the mid point between previous and next contact phases
    float                             m_timeToNextStart;            ///< Time to the start of the next contact phase
    float                             m_timeToNextSyncEvent;        ///< Time to the next footplant event

    // Foot cycle fractions
    float                             m_timeFractionInFootCycle;    ///< Current time as a fraction through the foot cycle
    float                             m_timeFractionOfMidInFootCycle; ///< Time of Mid event as a fraction through the foot cycle

    // State initialisation information (cont.)
    bool                              m_initNextWorldTrajectoryPosValid; ///< Valid flag for the mid and next initial predicted trajectory positions
    bool                              m_initNextWorldFootbasePosValid; ///< Valid flag for the mid and next initial predicted foot plant positions
  };

public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numLimbs);

  static AttribDataPredictiveUnevenTerrainPredictionState* init(
    NMP::Memory::Resource& resource,
    uint32_t               numLimbs,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataPredictiveUnevenTerrainPredictionState() { setType(ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_IK_PREDICTION_STATE); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_IK_PREDICTION_STATE; }

  /// For Manager registration.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  void resetTrajectoryPredictionHistory();
  uint32_t getTrajectorySampleBegin() { return (m_trajSampleIndx - m_trajNumSamples) & 0x03; }
  uint32_t getTrajectorySampleEnd() { return (m_trajSampleIndx - 1) & 0x03; }
  uint32_t getTrajectorySampleNext(uint32_t indx) const { return (indx + 1) & 0x03; }

  void appendTrajectorySample(
    float deltaTime,
    const NMP::Vector3& deltaPosVel,
    const NMP::Vector3& deltaAngVel);

  void getTrajectorySample(
    uint32_t sampleIndex,
    float& deltaTime,
    NMP::Vector3& deltaPosVel,
    NMP::Vector3& deltaAngVel)
  {
    NMP_ASSERT(sampleIndex < 4);
    deltaTime = m_trajDeltaTime[sampleIndex];
    deltaPosVel = m_trajPosVel[sampleIndex];
    deltaAngVel = m_trajAngVel[sampleIndex];
  }

public:
  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifdef NM_HOST_64_BIT
  // Trajectory sample history state
  NMP::Vector3                      m_trajPosVel[4];
  NMP::Vector3                      m_trajAngVel[4];
#endif

  uint32_t                          m_trajNumSamples;           ///< Number of valid points in the history
  uint32_t                          m_trajSampleIndx;           ///< Current insertion index of the ring buffer  

#ifndef NM_HOST_64_BIT
  // Trajectory sample history state
  NMP::Vector3                      m_trajPosVel[4];
  NMP::Vector3                      m_trajAngVel[4];
#endif

  float                             m_trajDeltaTime[4];

  // Common timing information
  bool                              m_isDataValid;
  bool                              m_isAbs;
  float                             m_curSyncEventPos;          ///< Current sync event position

  // Limb data
  uint32_t                          m_numLimbs;
  LimbState*                        m_limbStates[2];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPredictiveUnevenTerrainFootLiftingState
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPredictiveUnevenTerrainFootLiftingState : public AttribData
{
public:
  enum AlternateState
  {
    kCurrentPos,
    kPredictedPos,
    kNumAlternationStates,
    kInvalidAlternationState = 0xFFFFFFFF
  };

  enum AlternatePrediction
  {
    kMidPos,
    kNextPos,
    kNumPredictionStates,
    kInvalidPredictionState = 0xFFFFFFFF
  };

public:
  class LimbData
  {
  public:
    static NMP::Memory::Format getMemoryRequirements();
    static LimbData* init(NMP::Memory::Resource& resource);

    static LimbData* relocate(void*& ptr);

    void reset();

  public:
    // Actual ray-cast hit position data
    NMP::Vector3                    m_curWorldFootbaseHitPos;
    NMP::Vector3                    m_midWorldFootbaseHitPos;
    NMP::Vector3                    m_nextWorldFootbaseHitPos;

    // Actual ray-cast hit normal data
    NMP::Vector3                    m_curWorldFootbaseHitNormal;
    NMP::Vector3                    m_midWorldFootbaseHitNormal;
    NMP::Vector3                    m_nextWorldFootbaseHitNormal;

    // Actual ray-cast hit valid flags
    bool                            m_curWorldFootbaseHitPosValid;
    bool                            m_midWorldFootbaseHitPosValid;
    bool                            m_nextWorldFootbaseHitPosValid;

    // Enable flags for foot lifting (indicates actual ray-cast entry can be used for lifting)
    bool                            m_midWorldHitDataEnabledForLifting;
    bool                            m_nextWorldHitDataEnabledForLifting;

    // Flags used to indicate the actual ray-cast hit normals are within the slope angle limit
    bool                            m_curWorldFootbaseHitNormalSlopeValid;
    bool                            m_midWorldFootbaseHitNormalSlopeValid;
    bool                            m_nextWorldFootbaseHitNormalSlopeValid;

    // Alternation state
    AlternateState                  m_alternateState;
    AlternatePrediction             m_alternatePrediction;
  };

public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numLimbs);

  static AttribDataPredictiveUnevenTerrainFootLiftingState* init(
    NMP::Memory::Resource& resource,
    uint32_t               numLimbs,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataPredictiveUnevenTerrainFootLiftingState() { setType(ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_FOOT_LIFTING_STATE); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_FOOT_LIFTING_STATE; }

  /// For manager registration.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  void reset();

public:
#ifndef NM_HOST_64_BIT
  bool                              m_pad[3];
  bool                              m_footLiftingBasisValid;
  uint32_t                          m_numLimbs;
#endif

  NMP::Matrix34                     m_footLiftingBasis;

#ifdef NM_HOST_64_BIT
  bool                              m_footLiftingBasisValid;
  uint32_t                          m_numLimbs;
#endif

  // Limb data
  LimbData*                         m_limbInfo[2];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataClosestAnimDef
/// \brief Setup definition data that determines the matching behaviour of the closest anim node.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataClosestAnimDef : public AttribData
{
public:
  class ClosestAnimSourceData
  {
  public:
    static NMP::Memory::Format getMemoryRequirements(uint32_t length, bool useVelocity);
    
    static ClosestAnimSourceData* init(
      NMP::Memory::Resource& resource,
      uint32_t               length,
      bool                   useVelocity);
      
    static void locate(ClosestAnimSourceData* target);
    static void dislocate(ClosestAnimSourceData* target);
    
  public:
    CompressedDataBufferVector3*    m_sourceTransformsPos;
    CompressedDataBufferQuat*       m_sourceTransformsAtt;
    
    CompressedDataBufferVector3*    m_sourceTransformsPosVel;
    CompressedDataBufferVector3*    m_sourceTransformsAngVel;
  };

public:
  static NMP::Memory::Format getMemoryRequirements(
    bool                    precomputeSourcesOffline,
    uint32_t                numSources,
    uint32_t                numDescendants,
    uint32_t                numAnimChannelsPerSource,
    bool                    useVelocity);

  static AttribDataClosestAnimDef* init(
    NMP::Memory::Resource&  resource,
    bool                    precomputeSourcesOffline,
    uint32_t                numSources,
    uint32_t                numDescendants,
    NodeID*                 numDescendantIDs,
    uint32_t                numAnimChannelsPerSource,
    bool                    useVelocity,
    uint16_t                refCount = 0);

  NM_INLINE AttribDataClosestAnimDef() { setType(ATTRIB_TYPE_CLOSEST_ANIM_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_CLOSEST_ANIM_DEF; }

  /// For manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  NM_INLINE uint32_t getNumSources() const { return m_numSources; }
  NM_INLINE ClosestAnimSourceData* getAnimSourceData(uint32_t animSrcIndex) { NMP_ASSERT(animSrcIndex < m_numSources); return m_sourceDataArray[animSrcIndex]; }
  NM_INLINE const ClosestAnimSourceData* getAnimSourceData(uint32_t animSrcIndex) const { NMP_ASSERT(animSrcIndex < m_numSources); return m_sourceDataArray[animSrcIndex]; }

public:
  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  float                             m_fractionThroughSource;      ///< The fraction of the matched sources duration for root rotation blending
  float                             m_maxRootRotationAngle;       ///< The maximum angle limit that a source can be rotated by.
                                                                  ///< Stored as a cos half angle for fast testing with quats.
#endif

  // Root rotation blending
  NMP::Vector3                      m_rootRotationAxis;           ///< The character space root rotation axis for rotation offset matching
  NMP::Quat                         m_upAlignAtt;                 ///< The up vector alignment rotation (for aligning up axis with Y axis)
  bool                              m_useRootRotationBlending;    ///< Flag indicating if root rotation blending is enabled
  
#ifdef NM_HOST_64_BIT
  float                             m_fractionThroughSource;      ///< The fraction of the matched sources duration for root rotation blending
  float                             m_maxRootRotationAngle;       ///< The maximum angle limit that a source can be rotated by.
                                                                  ///< Stored as a cos half angle for fast testing with quats.
#endif

  // Matching
  uint32_t                          m_numAnimJoints;              ///< The number of conglomerated weighted joints
  bool                              m_precomputeSourcesOffline;   ///< Connected source node transforms are computed offline
  bool                              m_useVelocity;                ///< Flag indicating if velocity matching is enabled
  float                             m_positionScaleFactor;
  float                             m_orientationScaleFactor;
  float                             m_influenceBetweenPosAndOrient;
  
  // Closest anim source data
  uint32_t                          m_numSources;                 ///< The number of source nodes for matching
  ClosestAnimSourceData**           m_sourceDataArray;            ///< The compressed animation source data for matching

  // Descendant info, left empty if poses are evaluated offline
  uint32_t                          m_numDescendants;             ///< The number of nodes under the closest anim node.
  NodeID*                           m_descendantIDs;              ///< Array containing the node ID of every node under
                                                                  ///< the closest anim node. Used so that last update
                                                                  ///< frame of nodes can be preserved and restored when
                                                                  ///< sub network evaluation occurs.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataClosestAnimDefAnimSet
/// \brief Setup definition data for the closest anim node that is anim set specific
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataClosestAnimDefAnimSet : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numEntries);
  static AttribDataClosestAnimDefAnimSet* init(NMP::Memory::Resource& resource, uint32_t numEntries, uint16_t refCount = 0);

  NM_INLINE AttribDataClosestAnimDefAnimSet() { setType(ATTRIB_TYPE_CLOSEST_ANIM_DEF_ANIM_SET); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_CLOSEST_ANIM_DEF_ANIM_SET; }

  /// For manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

public:
  uint32_t      m_numAnimWeights; ///< The number of unpacked anim channel weights. i.e. the max number of
                                  ///< anim channels less any zero weighted channels at the end. Used to
                                  ///< avoid computing the root rotation and error metric of completely zero
                                  ///< weighted joints.
  
  uint32_t      m_numEntries;     ///< The number of validly weighted joint channels in this animation set
  float*        m_weights;        ///< The valid non-zero weights
  uint16_t*     m_rigChannels;    ///< The rig channels corresponding to the valid weights (increasing rig bone indices)
  uint16_t*     m_animChannels;   ///< The compression buffer channels corresponding to the valid weights (not ordered)
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataClosestAnimState
/// \brief State data of the closest anim node.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataClosestAnimState : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataClosestAnimState* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataClosestAnimState() { setType(ATTRIB_TYPE_CLOSEST_ANIM_STATE); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_CLOSEST_ANIM_STATE; }

  /// For manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(AttribData* source, AttribData* dest);

public:
  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifdef NM_HOST_64_BIT
  NMP::Quat   m_rootRotationOffset;
#endif

  float       m_fractionThroughSource;
  bool        m_blendCompleteFlag;
  bool        m_isPerformingSubNetworkEval;
  bool        m_pad[2];

#ifndef NM_HOST_64_BIT
  NMP::Quat   m_rootRotationOffset;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataHeadLookSetup
/// \brief Animation set independent definition data specific to the HeadLookNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataHeadLookSetup : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataHeadLookSetup* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataHeadLookSetup() { setType(ATTRIB_TYPE_HEAD_LOOK_SETUP); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_HEAD_LOOK_SETUP; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  bool         m_updateTargetByDeltas;
  bool         m_worldSpaceTarget; ///< Target CP is expressed in world space rather than character space.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataHeadLookChain
/// \brief Animation set dependent definition data specific to the HeadLookNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataHeadLookChain : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numJoints);
  static AttribDataHeadLookChain* init(
    NMP::Memory::Resource& resource,
    uint32_t               numJoints,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataHeadLookChain() { setType(ATTRIB_TYPE_HEAD_LOOK_CHAIN); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_HEAD_LOOK_CHAIN; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifdef NM_HOST_64_BIT
  NMP::Vector3 m_endEffectorOffset;
#endif
  uint32_t     m_endJointIndex;
  uint32_t     m_rootJointIndex;

#ifndef NM_HOST_64_BIT
  NMP::Vector3 m_endEffectorOffset;
#endif

  uint32_t     m_numJoints;
  NMRU::PointIK::Params* m_ikParams; ///< The NMIK solver params object.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataGunAimSetup
/// \brief Animation set independent definition data specific to the GunAimNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataGunAimSetup : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataGunAimSetup* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataGunAimSetup() { setType(ATTRIB_TYPE_GUN_AIM_SETUP); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_GUN_AIM_SETUP; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifdef NM_HOST_64_BIT
  NMP::Vector3 m_worldUpAxis;        ///< Used to apply the 'keep upright' parameter.
  bool         m_keepUpright;        ///< Prevents tilting of the gun.
  bool         m_worldSpaceTarget;   ///< Target CP is expressed in world space rather than character space.
  bool         m_applyJointLimits;   ///< Clamps result to joint limits if they exist.
  bool         m_pad;
#else
  bool         m_keepUpright;        ///< Prevents tilting of the gun.
  bool         m_worldSpaceTarget;   ///< Target CP is expressed in world space rather than character space.
  bool         m_applyJointLimits;   ///< Clamps result to joint limits if they exist.
  bool         m_pad[5];
  NMP::Vector3 m_worldUpAxis;        ///< Used to apply the 'keep upright' parameter.
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataGunAimIKChain
/// \brief Animation set dependent definition data specific to the GunAimNode.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataGunAimIKChain : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numAimJoints);
  static AttribDataGunAimIKChain* init(
    NMP::Memory::Resource& resource,
    uint32_t               numAimJoints,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataGunAimIKChain() { setType(ATTRIB_TYPE_GUN_AIM_IK_CHAIN); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_GUN_AIM_IK_CHAIN; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifndef NM_HOST_64_BIT
  uint32_t     m_gunJointIndex;
  uint32_t     m_gunBindJointIndex;
#endif

  NMP::Vector3 m_gunPivotOffset;
  NMP::Vector3 m_gunBarrelOffset;
  NMP::Vector3 m_gunPointingVector;
  NMP::Vector3 m_secondaryArmHingeAxis;
  NMP::Vector3 m_primaryArmHingeAxis;
  uint32_t     m_spineRootJointIndex;
  uint32_t     m_secondaryWristJointIndex;
  uint32_t     m_primaryWristJointIndex;
  float        m_spineBias;
  bool         m_updateTargetByDeltas;
  bool         m_useSecondaryArm;
  uint32_t     m_numAimJoints; ///< Number of real rig joints involved in the aiming IK chain.
  NMRU::PointIK::Params* m_ikParams; ///< The NMIK solver params object.

#ifdef NM_HOST_64_BIT
  uint32_t     m_gunJointIndex;
  uint32_t     m_gunBindJointIndex;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataFunctionOperation
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataFunctionOperation : public AttribData
{
public:
  /// \enum  MR::AttribDataFunctionOperation::OperationType
  /// \brief
  enum OperationType
  {
    OPERATION_SIN           = 0,
    OPERATION_COS           = 1,
    OPERATION_TAN           = 2,
    OPERATION_EXP           = 3,
    OPERATION_LOG           = 4,
    OPERATION_SQRT          = 5,
    OPERATION_ABS           = 6,
    OPERATION_ASIN          = 7,
    OPERATION_ACOS          = 8,
    OPERATION_NUMOPS        = 9,
    OPERATION_FORCEINTSIZE  = 0xFFFFFFFF
  };

  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataFunctionOperation* init(NMP::Memory::Resource& resource, OperationType value, uint16_t refCount = 0);

  NM_INLINE AttribDataFunctionOperation() { setType(ATTRIB_TYPE_FLOAT_OPERATION); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_FLOAT_OPERATION; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  
  OperationType m_operation;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataArithmeticOperation
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataArithmeticOperation : public AttribData
{
public:
  /// \enum  MR::AttribDataArithmeticOperation::OperationType
  /// \brief
  enum OperationType
  {
    OPERATION_MULTIPLY      = 0,
    OPERATION_ADD           = 1,
    OPERATION_DIVIDE        = 2,
    OPERATION_SUBTRACT      = 3,
    OPERATION_MIN           = 4,
    OPERATION_MAX           = 5,
    OPERATION_MULTIPLYADD   = 6,
    OPERATION_NUMOPS        = 7,
    OPERATION_FORCEINTSIZE  = 0xFFFFFFFF
  };

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataArithmeticOperation* init(
    NMP::Memory::Resource& resource,
    OperationType          value,
    float                  constValue,
    float                  constX,
    float                  constY,
    float                  constZ,
    uint16_t               refCount = 0);

  static AttribDataArithmeticOperation* init(
    NMP::Memory::Resource& resource,
    OperationType          value,
    float                  constValue,
    uint16_t               refCount = 0);

  static AttribDataArithmeticOperation* init(
    NMP::Memory::Resource& resource,
    OperationType          value,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataArithmeticOperation() { setType(ATTRIB_TYPE_2_FLOAT_OPERATION); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_2_FLOAT_OPERATION; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifdef NM_HOST_64_BIT
  NMP::Vector3  m_constVector; // Vector value version of const
#endif

  OperationType m_operation;
  float         m_constValue;

#ifndef NM_HOST_64_BIT
  NMP::Vector3  m_constVector; // Vector value version of const
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBooleanOperation
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBooleanOperation : public AttribData
{
public:
  /// \enum  MR::AttribDataBooleanOperation::OperationType
  /// \brief
  enum OperationType
  {
    OPERATION_AND  = 0,
    OPERATION_OR   = 1,
    OPERATION_XOR  = 2,  ///< One and only one input is true.
    OPERATION_FORCEINTSIZE  = 0xFFFFFFFF
  };

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataBooleanOperation* init(
    NMP::Memory::Resource& resource,
    OperationType          value,        ///< The operation to perform.
    bool                   invertResult, ///< Whether we should invert the result of the operation or not.
    uint16_t               refCount = 0);
    
  NM_INLINE AttribDataBooleanOperation() { setType(ATTRIB_TYPE_BOOLEAN_OPERATION); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BOOLEAN_OPERATION; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  OperationType m_operation;
  bool          m_invertResult; ///< Whether we should invert the result of the operation or not.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataValueCompareOperation
/// \brief Can be used to describe the comparison of any type.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataValueCompareOperation : public AttribData
{
public:
  /// \enum  MR::AttribDataValueCompareOperation::OperationType
  /// \brief
  enum OperationType
  {
    OPERATION_GREATER       = 0,
    OPERATION_LESS          = 1,
    OPERATION_GREATER_EQUAL = 2,
    OPERATION_LESS_EQUAL    = 3,
    OPERATION_EQUAL         = 4,
    OPERATION_NOT_EQUAL     = 5,
    OPERATION_FORCEINTSIZE  = 0xFFFFFFFF
  };

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataValueCompareOperation* init(
    NMP::Memory::Resource& resource,
    OperationType          value,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataValueCompareOperation() { setType(ATTRIB_TYPE_VALUE_COMPARE_OPERATION); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_VALUE_COMPARE_OPERATION; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  OperationType m_operation;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataSmoothFloatOperation
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataSmoothFloatOperation : public AttribData
{
public:
  static AttribDataSmoothFloatOperation* create(
    NMP::MemoryAllocator* allocator,
    float rateInc,
    float rateDec,
    float initialValueX,
    float initialValueY,
    float initialValueZ,
    bool useInitValOnInit,
    bool smoothVel,
    uint16_t refCount = 0);
  static AttribDataSmoothFloatOperation* init(
    NMP::Memory::Resource& resource,
    float rateInc,
    float rateDec,
    float initialValueX,
    float initialValueY,
    float initialValueZ,
    bool useInitValOnInit,
    bool smoothVel,
    uint16_t refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  NM_INLINE AttribDataSmoothFloatOperation() { setType(ATTRIB_TYPE_SMOOTH_FLOAT_OPERATION); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SMOOTH_FLOAT_OPERATION; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static AttribDataHandle create(NMP::MemoryAllocator* allocator);
  static bool copy(AttribData* source, AttribData* dest);

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifdef NM_HOST_64_BIT
  NMP::Vector3 m_vectorRate; // For vector3 damping.
#endif

  float        m_floatRateIncreasing;  // For single float damping - use this value for smoothing when we are smoothing towards a larger number.
  float        m_floatRateDecreasing;  // For single float damping - use this value for smoothing when we are smoothing towards a smaller number.
  float        m_initialValueX; // The float (or for vectors, X component of) the value the operator will start at when it's initialised.
  float        m_initialValueY; // The Y component of the value the operator will start at when it's initialised.
  float        m_initialValueZ; // The Z component of the value the operator will start at when it's initialised.
  bool         m_useInitValOnInit; // Should this node use the above values on initialisation, or the incoming CP?
  bool         m_smoothVel;  // True is critical damping, false is exponential.
  bool         m_pad[2];

#ifndef NM_HOST_64_BIT
  NMP::Vector3 m_vectorRate; // For vector3 damping.
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataRateOfChangeState
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataRateOfChangeState : public AttribData
{
public:
  static AttribDataRateOfChangeState* create(NMP::MemoryAllocator* allocator, float initFloat, uint16_t refCount = 0);
  static AttribDataRateOfChangeState* init(NMP::Memory::Resource& resource, float initFloat, uint16_t refCount = 0);

  static AttribDataRateOfChangeState* create(NMP::MemoryAllocator* allocator, float x, float y, float z, uint16_t refCount = 0);
  static AttribDataRateOfChangeState* init(NMP::Memory::Resource& resource, float x, float y, float z, uint16_t refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  NM_INLINE AttribDataRateOfChangeState() { setType(ATTRIB_TYPE_RATE_OF_CHANGE_OPERATION); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_RATE_OF_CHANGE_OPERATION; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static AttribDataHandle create(NMP::MemoryAllocator* allocator);
  static bool copy(AttribData* source, AttribData* dest);

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifdef NM_HOST_64_BIT
  NMP::Vector3 m_lastVector; // Vector value on previous frame
#endif

  float        m_lastFloat;  // Float value on previous frame
  float        m_pad;

#ifndef NM_HOST_64_BIT
  NMP::Vector3 m_lastVector; // Vector value on previous frame
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataRandomFloatOperation
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataRandomFloatOperation : public AttribData
{
public:
  static AttribDataHandle create(NMP::MemoryAllocator* allocator, uint32_t seed, uint16_t refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataRandomFloatOperation* init(NMP::Memory::Resource& resource, uint32_t seed, uint16_t refCount = 0);

  NM_INLINE AttribDataRandomFloatOperation() { setType(ATTRIB_TYPE_RANDOM_FLOAT_OPERATION); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_RANDOM_FLOAT_OPERATION; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  NMP::RNG  m_generator;
  float     m_lastFrameTime;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataRandomFloatDef
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataRandomFloatDef : public AttribData
{
public:
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    float                 min,
    float                 max,
    float                 duration,
    uint32_t              seed,
    uint16_t              refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataRandomFloatDef* init(
    NMP::Memory::Resource& resource,
    float                  min,
    float                  max,
    float                  duration,
    uint32_t               seed,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataRandomFloatDef() { setType(ATTRIB_TYPE_RANDOM_FLOAT_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_RANDOM_FLOAT_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  float     m_min;
  float     m_max;
  float     m_duration;
  uint32_t  m_seed;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataNoiseGenDef
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataNoiseGenDef : public AttribData
{
public:
  static AttribDataNoiseGenDef* init(
    NMP::Memory::Resource& resource,
    uint32_t               flags,
    float                  noiseSawFrequency,
    uint16_t               refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  NM_INLINE AttribDataNoiseGenDef() { setType(ATTRIB_TYPE_NOISE_GEN_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_NOISE_GEN_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  // fill the 3 tables with random numbers
  static void fillTables(float* table1, float* table2, float* table3);

  uint32_t m_flags;
  float    m_noiseSawFrequency;
  float    m_timeOffset;
  float*   m_table1;        // MORPH-21339: These tables need to be a shared resource for all AttribDatas to access.
  float*   m_table2;
  float*   m_table3;

  static const uint32_t TABLE_1_SIZE;
  static const uint32_t TABLE_2_SIZE;
  static const uint32_t TABLE_3_SIZE;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataSwitchDef
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataSwitchDef : public AttribData
{
public:

  enum EvaluateMode
  {
    kEvalWhenActivating = 0,
    kEvalEveryFrame = 1,
    kEvalOnLoop = 2
  };

  enum InputSelectionMethod
  {
    kClosest = 0,
    kFloor,
    kCeiling,
    kWeightRampingInvalid
  };

  static AttribDataSwitchDef* init(
    NMP::Memory::Resource& resource,
    EvaluateMode           evalMode,
    InputSelectionMethod   inputSelectionMethod,
    uint16_t               refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  NM_INLINE AttribDataSwitchDef() { setType(ATTRIB_TYPE_SWITCH_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SWITCH_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  EvaluateMode m_evalMode;
  InputSelectionMethod  m_inputSelectionMethod;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataRayCastDef
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataRayCastDef : public AttribData
{
public:
  static AttribDataRayCastDef* init(
    NMP::Memory::Resource& resource,
    bool                   useLocalOrientation,
    NMP::Vector3&          rayStart,
    NMP::Vector3&          rayDelta,
    uint32_t               upAxisIndex,
    uint16_t               refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  NM_INLINE AttribDataRayCastDef() { setType(ATTRIB_TYPE_RAY_CAST_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_RAY_CAST_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  // AttribData differs in size on 64 bit builds requiring a different member layout so that vector types are aligned without padding
#ifdef NM_HOST_64_BIT
  NMP::Vector3 m_rayStart;
  NMP::Vector3 m_rayDelta;
#endif

  uint32_t     m_upAxisIndex;
  bool         m_useLocalOrientation;

#ifndef NM_HOST_64_BIT
  bool         m_pad[3];
  NMP::Vector3 m_rayStart;
  NMP::Vector3 m_rayDelta;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPhysicalEffectData
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPhysicalEffectData : public MR::AttribData
{
public:
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_PHYSICAL_EFFECT_DATA; }

  static AttribDataPhysicalEffectData* create(
    NMP::MemoryAllocator* allocator, MR::Network* network, MR::NodeDef* nodeDef, uint16_t refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataPhysicalEffectData* init(
    NMP::Memory::Resource& resource, MR::Network* network, MR::NodeDef* nodeDef, uint16_t refCount = 0);

  NM_INLINE AttribDataPhysicalEffectData() { setType(ATTRIB_TYPE_PHYSICAL_EFFECT_DATA); setRefCount(0); }

  MR::Network* m_network;
  MR::NodeDef* m_nodeDef; // Performance node definition
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPhysicsInfoDef
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPhysicsInfoDef : public AttribData
{
public:
  static AttribDataPhysicsInfoDef* init(
    NMP::Memory::Resource& resource,
    int                    partIndex,
    bool                   outputInWorldSpace,
    uint16_t               refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  NM_INLINE AttribDataPhysicsInfoDef() { setType(ATTRIB_TYPE_PHYSICS_INFO_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PHYSICS_INFO_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  int         m_partIndex;
  bool        m_outputInWorldSpace;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPhysicsObjectPointer
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPhysicsObjectPointer : public AttribData
{
public:
  static AttribDataPhysicsObjectPointer* init(
    NMP::Memory::Resource& resource,
    void*                  objectPointer,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    void*                 physicsObjectPointer,
    uint16_t              refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  NM_INLINE AttribDataPhysicsObjectPointer() { setType(ATTRIB_TYPE_PHYSICS_OBJECT_POINTER); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PHYSICS_OBJECT_POINTER; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void copy(AttribData* source, AttribData* dest);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  void*       m_value;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPhysicsSetup
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPhysicsSetup : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataPhysicsSetup* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataPhysicsSetup() { setType(ATTRIB_TYPE_PHYSICS_SETUP); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PHYSICS_SETUP; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static bool copy(AttribData* source, AttribData* dest);

  bool     m_enableCollision;
  bool     m_useActiveAnimationAsKeyframeAnchor;
  bool     m_useRootAsAnchor;
  bool     m_hasAnimationChild;
  bool     m_outputSourceAnimation;
  bool     m_preserveMomentum;
  uint32_t m_rootControlMethod; // Network::RootControlMethod

  /// \enum  MR::AttribDataPhysicsSetup::PhysicsMethod
  /// \brief
  /// \ingroup
  enum PhysicsMethod {
    METHOD_INVALID = -1,   // -1
    METHOD_RAGDOLL,        // 0
    METHOD_AA,             // 1
    METHOD_LOCAL_SK,       // 2
    METHOD_SK,             // 3
    METHOD_HK,             // 4
    METHOD_SKAA            // 5
  };
  PhysicsMethod m_method;

};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPhysicsSetupPerAnimSet
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPhysicsSetupPerAnimSet : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataPhysicsSetupPerAnimSet* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataPhysicsSetupPerAnimSet() { setType(ATTRIB_TYPE_PHYSICS_SETUP_ANIM_SET); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PHYSICS_SETUP_ANIM_SET; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static bool copy(AttribData* source, AttribData* dest);

  bool     m_disableSleeping;
  bool     m_enableJointLimits;
  bool     m_hardKeyframingEnableConstraint;
  float    m_softKeyFramingMaxAccel;
  float    m_softKeyFramingMaxAngAccel;
  float    m_activeAnimationStrengthMultiplier;
  float    m_activeAnimationDampingMultiplier;
  float    m_activeAnimationInternalCompliance;
  float    m_activeAnimationExternalCompliance;
  int      m_jointProjectionIterations;
  float    m_jointProjectionLinearTolerance;
  float    m_jointProjectionAngularTolerance;
  float    m_massMultiplier;
  float    m_gravityCompensation;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPhysicsState
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPhysicsState : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numAnimationJoints);
  
  static AttribDataPhysicsState* init(
    NMP::Memory::Resource&  resource,
    uint32_t                numAnimationJoints,
    uint16_t                refCount = 0);
    
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);
  
  static AttribDataHandle create(
    NMP::MemoryAllocator*   allocator,
    uint32_t                numAnimationJoints,
    uint16_t                refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataPhysicsState() { setType(ATTRIB_TYPE_PHYSICS_STATE); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PHYSICS_STATE; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
  static bool copy(AttribData* source, AttribData* dest, bool copyTransforms);

  // size unknown at compile time
  AttribDataTransformBuffer* m_previousChildTransforms;

  float                      m_deltaTime;         // the physics timestep for the current update.
  float                      m_previousDeltaTime; // physics timestep for the previous update.

  /// Counter incremented each transform update - will be 1 on the first updates - incremented when
  /// traj is queued.
  uint32_t                   m_updateCounter;
  bool                       m_haveAddedPhysicsRigRef;
  bool                       m_needToInitialiseRoot;
};

//----------------------------------------------------------------------------------------------------------------------
typedef AttribDataArrayCreateDesc AttribDataPhysicsStateCreateDesc;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPhysicsInitialisation
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPhysicsInitialisation : public AttribData
{
public:
  static AttribDataHandle create(
    NMP::MemoryAllocator*  allocator,
    uint32_t               numAnimationJoints,
    uint16_t               refCount = 0);

  static NMP::Memory::Format getMemoryRequirements(uint32_t numAnimationJoints);

  static AttribDataPhysicsInitialisation* init(
    NMP::Memory::Resource& resource,
    uint32_t               numAnimationJoints,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataPhysicsInitialisation() { setType(ATTRIB_TYPE_PHYSICS_INITIALISATION); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PHYSICS_INITIALISATION; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  AttribDataTransformBuffer* m_transforms;
  AttribDataTransformBuffer* m_prevTransforms;

  NMP::Matrix34              m_worldRoot;
  NMP::Matrix34              m_prevWorldRoot;
  float                      m_deltaTime;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPhysicsGrouperConfig
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPhysicsGrouperConfig : public AttribData
{
public:
  static AttribDataHandle create(
    NMP::MemoryAllocator*  allocator,
    uint16_t               refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataPhysicsGrouperConfig* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataPhysicsGrouperConfig() { setType(ATTRIB_TYPE_PHYSICS_GROUPER_CONFIG); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PHYSICS_GROUPER_CONFIG; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  /// Which children to convert
  bool m_convertToPhysics[2];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTransitDef
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataTransitDef : public AttribData
{
public:
  /// \enum  MR::AttribDataTransitDef::InitDestStartPointMethod
  /// \brief Method which this node uses to initialise the start point for the destination.
  /// \ingroup
  enum InitDestStartPointMethod
  {
    INIT_DEST_USE_START_DURATION_FRACTION = 0,                ///< Initialise destination at a fraction of its duration.
    INIT_DEST_USE_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION,     ///< Initialise destination at the specified sync event
                                                              ///<  index, but also synchronise with the current event
                                                              ///<  fraction in the source.
    INIT_DEST_USE_EVENT_INDEX_AND_USE_EVENT_FRACTION,         ///< Initialise destination at the specified sync event
                                                              ///<  index and fraction.
    INIT_DEST_SYNC_ON_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION, ///< Sync on source nodes event index and fraction.
    INIT_DEST_SYNC_ON_EVENT_INDEX_AND_USE_EVENT_FRACTION,     ///< Sync on source nodes event index but use the
                                                              ///<  specified event fraction.
    INIT_DEST_UNKNOWN = 0xFFFFFFFF
  };

  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataTransitDef* init(
    NMP::Memory::Resource&   resource,
    float                    duration,
    InitDestStartPointMethod destinationInitMethod,
    float                    destinationStartFraction,
    float                    destinationStartSyncEvent,
    bool                     reversible,
    NodeID                   reverseControlParamNodeID,
    PinIndex                 reverseControlParamPinIndex,
    bool                     slerpTrajectoryPosition,
    uint32_t                 blendMode,
    bool                     freezeSource,
    bool                     freezeDest,
    uint16_t                 refCount = 0);

  NM_INLINE AttribDataTransitDef() { setType(ATTRIB_TYPE_TRANSIT_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_TRANSIT_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  float                    m_duration;
  InitDestStartPointMethod m_destinationInitMethod;
  float                    m_destinationStartFraction;  ///< Start pos as a fraction of the duration of the destination.
  float                    m_destinationStartSyncEvent; ///< Index + Fractional parts.
                                                        ///<  Depending on mode one of these parts may be zero.

  bool                     m_slerpTrajectoryPosition;
  uint32_t                 m_blendMode;
  bool                     m_freezeSource;
  bool                     m_freezeDest;
  bool                     m_reversible;
  CPConnection             m_reverseInputCPConnection;  ///< Connection to a control param that can make the transition
                                                        ///<  reverse direction.
                                                        ///<  i.e. instead of completing in the destination state it will
                                                        ///<  complete in the source state.
  NodeInitDataArrayDef*    m_nodeInitData;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTransitSyncEventsDef
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataTransitSyncEventsDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataTransitSyncEventsDef* init(
    NMP::Memory::Resource& resource,
    float                  durationInEvents,
    uint32_t               destEventSequenceOffset,
    uint32_t               destStartEventIndex,
    bool                   usingDestStartEventIndex,
    bool                   reversible,
    NodeID                 reverseControlParamNodeID,
    PinIndex               reverseControlParamPinIndex,
    bool                   slerpTrajectoryPosition,
    uint32_t               blendMode,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataTransitSyncEventsDef() { setType(ATTRIB_TYPE_TRANSIT_SYNC_EVENTS_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_TRANSIT_SYNC_EVENTS_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  float              m_durationInEvents;
  uint32_t           m_destEventTrackOffset;
  uint32_t           m_destStartEventIndex;
  bool               m_usingDestStartEventIndex;

  bool               m_slerpTrajectoryPosition;
  uint32_t           m_blendMode;

  bool               m_reversible;
  CPConnection       m_reverseInputCPConnection;  ///< Connection to a control param that can make the transition
                                                  ///<  reverse direction.
                                                  ///<  i.e. instead of completing in the destination state it will
                                                  ///<  complete in the source state.

  NodeInitDataArrayDef* m_nodeInitData;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTransitSyncEvents
/// \brief
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataTransitSyncEvents : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  
  static AttribDataTransitSyncEvents* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);
  
  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc = NULL);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc = NULL);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataTransitSyncEvents() { setType(ATTRIB_TYPE_TRANSIT_SYNC_EVENTS); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_TRANSIT_SYNC_EVENTS; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
#if defined(MR_OUTPUT_DEBUGGING)
  static uint32_t serializeTx(Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize = 0);
#endif // MR_OUTPUT_DEBUGGING

  float m_transitionEventCount; ///< How far through transition are we.
  bool  m_completed;            ///< Transition has completed;
  bool  m_reversed;             ///< Transition has reversed direction
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataDeadBlendDef
/// \brief Specifies how a transition should behave when either the source or destination are physical.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataDeadBlendDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataDeadBlendDef* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  static AttribDataDeadBlendDef* create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataDeadBlendDef() { setType(ATTRIB_TYPE_DEAD_BLEND_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_DEAD_BLEND_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  float m_deadReckoningAngularVelocityDamping; ///< Damping of the dead reckoning angular velocity
  bool m_useDeadReckoningWhenDeadBlending;  ///< If On  - Transition source is replaced with a dead-reckoned animation source.
                                            ///< If Off - Transition source is replaced with its last output transform buffer.
  bool m_blendToDestinationPhysicsBones;    ///< If On  - Do an animation blend to the transforms that have already been affected by the physics.
                                            ///< If Off - Use the destination (physics) transforms.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataDeadBlendState
/// \brief Active status of a dead blend.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataDeadBlendState : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataDeadBlendState* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  static AttribDataDeadBlendState* create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataDeadBlendState() { setType(ATTRIB_TYPE_DEAD_BLEND_STATE); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_DEAD_BLEND_STATE; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  uint32_t m_blendMode;               ///< See BLEND_MODE_...
  float    m_duration;                ///< Extracted from the transitions duration value.

  bool     m_isBlendingToPhysics;     ///< If the destination or any of its children are physical then this will become true.
                                      ///<  This can change dynamically as the transition progresses.
  bool     m_performDeadBlend;        ///< If we need to, or are already, preforming a dead blend:
                                      ///<  1. If transitioning back to a state that is already active, the original
                                      ///<     transition out of this state must be turned in to a dead blend.
                                      ///<  2. If the source node or any of its children are physical we must perform
                                      ///<     a dead blend.
                                      ///<  3. If there is only one active child in a transition it means we have already
                                      ///<     started a dead blend.
  bool     m_slerpTrajectoryPosition; ///<  Indicates if we should be slerp blending the delta trajectory.
                                      ///<  Comes directly from the transitions settings.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBlendNxMDef
/// \brief Specifies the configuration of a BlendNxM node.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBlendNxMDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint16_t                numWeightsX,
    uint16_t                numWeightsY,
    bool                    wrapWeightsX,
    bool                    wrapWeightsY);

  static AttribDataBlendNxMDef* init(
    NMP::Memory::Resource&  resource,
    uint16_t                numWeightsX,
    uint16_t                numWeightsY,
    bool                    wrapWeightsX,
    bool                    wrapWeightsY,
    uint16_t                refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator*   allocator,
    uint16_t                numWeightsX,
    uint16_t                numWeightsY,
    bool                    wrapWeightsX,
    bool                    wrapWeightsY,
    uint16_t                refCount = 0);

  NM_INLINE AttribDataBlendNxMDef() { setType(ATTRIB_TYPE_BLEND_NXM_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BLEND_NXM_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
  static void dislocate(AttribData* target);

public:
  uint16_t    m_numWeightsX;
  uint16_t    m_numWeightsY;
  bool        m_wrapWeightsX;
  bool        m_wrapWeightsY;
  float*      m_weightsX;
  float*      m_weightsY;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataPlaybackPosInit
/// \brief Used by transitions to pass down playback initialisation information to its destination state.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataPlaybackPosInit : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataPlaybackPosInit* init(
    NMP::Memory::Resource& resource,
    bool                   initWithDurationFraction, ///< Indicates our initialisation method.
    float                  startValue,               ///< Either: Init the playback pos at a fraction of the nodes duration.
                                                     ///<     Or: Init the playback pos at the specified event position.      
    int32_t                adjustStartEventIndex,    ///< Start sync event adjustment value, used independently of m_startValue;
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator*      allocator,
    bool                       initWithDurationFraction,  ///< Indicates our initialisation method.
    float                      startValue,                ///< Either: Init the playback pos at a fraction of the nodes duration.
                                                          ///<     Or: Init the playback pos at the specified event position.      
    int32_t                    adjustStartEventIndex = 0, ///< Start sync event adjustment value, used independently of m_startValue;
    uint16_t                   refCount = 0);

  NM_INLINE AttribDataPlaybackPosInit() { setType(ATTRIB_TYPE_PLAYBACK_POS_INIT); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_PLAYBACK_POS_INIT; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static bool copy(AttribData* source, AttribData* dest);

  bool    m_initWithDurationFraction; ///< Indicates our initialisation method.
  float   m_startValue;               ///< Either: Initialise the playback pos at a fraction of the nodes duration.
                                      ///<     Or: Initialise the playback pos at the specified event position.
  int32_t m_adjustStartEventIndex;    ///< Start sync event adjustment value, used independently of m_startValue;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataJointLimits
/// \brief Stores per-anim-set kinematic joint limit data that may have been authored on the rig.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataJointLimits : public AttribData
{
public:

  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numRigJoints,
    uint32_t numLimitedJoints);
  static AttribDataJointLimits* init(
    NMP::Memory::Resource& resource,
    uint32_t               numRigJoints,
    uint32_t               numLimitedJoints,
    uint16_t               refCount = 0);

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  /// \brief Returns the number of joint limits stored.
  uint32_t getNumJointLimits() const;
  /// \brief Returns the number of joints in the rig owning these limits.
  uint32_t getNumRigJoints() const;

  /// \brief Retrieve a limit for this joint, or NULL if no limit
  const NMRU::JointLimits::Params* getLimitForJoint(uint32_t jointIndex) const;

  /// \brief The number of joints in the rig owning these limits - the size of the m_jointLimits array.
  uint32_t m_rigSize;

  /// \brief The number of joint limits - the size of the m_jointLimitData array.
  uint32_t m_numJointLimits;

  /// \brief An array of limit params pointers per rig joint, for direct access to joint limit data
  /// without having to store a limit for every joint.
  NMRU::JointLimits::Params** m_jointLimits;

  /// \brief The actual joint limit data, one per joint with a limit.
  NMRU::JointLimits::Params* m_jointLimitData;

};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataJointLimits::getNumJointLimits() const
{
  return m_numJointLimits;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribDataJointLimits::getNumRigJoints() const
{
  return m_rigSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMRU::JointLimits::Params* AttribDataJointLimits::getLimitForJoint(uint32_t jointIndex) const
{
  NMP_ASSERT(jointIndex < m_rigSize);
  return m_jointLimits[jointIndex];
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataEmitMessageOnCPValueDef
/// \brief Def date for AttribDataEmitMessageOnCPValueDef. Holds an array of comparison types and comparison values.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataEmitMessageOnCPValueDef : public AttribData
{
public:
  static AttribDataHandle create(NMP::MemoryAllocator* allocator, uint32_t numComparisons, uint16_t refCount = 0);
  static NMP::Memory::Format getMemoryRequirements(uint32_t numComparisons);
  static AttribDataEmitMessageOnCPValueDef* init(NMP::Memory::Resource& resource, uint32_t numComparisons, uint16_t refCount = 0);

  NM_INLINE AttribDataEmitMessageOnCPValueDef() { setType(ATTRIB_TYPE_EMIT_MESSAGE_ON_CP_VALUE); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_EMIT_MESSAGE_ON_CP_VALUE; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target);

  uint32_t  m_numComparisons;
  float*    m_comparisonValues;
  uint32_t* m_comparisonTypes;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBlendFlags
/// \brief Structure to describe any runtime blending optimisations.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBlendFlags : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataBlendFlags* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataBlendFlags() { setType(ATTRIB_TYPE_BLEND_FLAGS); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BLEND_FLAGS; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

public:
  bool  m_alwaysBlendTrajectoryAndTransforms;
  bool  m_alwaysCombineSampledEvents;
};

//----------------------------------------------------------------------------------------------------------------------
class AttribDataBlendWeightsCreateDesc : public AttribDataCreateDesc
{
public:
  NM_FORCEINLINE AttribDataBlendWeightsCreateDesc(
    uint16_t maxNumWeights) : m_maxNumWeights(maxNumWeights)
  {
  }

public:
  uint16_t  m_maxNumWeights;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataBlendWeights
/// \brief Generic structure for describing the interpolation weights for a blend node.
/// There are independent weights for trajectory & transforms, and events
/// so that a blend node can customise the behaviour of the blending.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataBlendWeights : public AttribData
{
public: 
  static NMP::Memory::Format getMemoryRequirements(
    uint16_t maxNumWeights);

  static AttribDataBlendWeights* init(
    NMP::Memory::Resource& resource,
    uint16_t               maxNumWeights,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::Memory::Resource& resource,
    AttribDataCreateDesc*  desc);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint16_t              maxNumWeights,
    uint16_t              refCount = 0);

  static NMP::Memory::Format outputMemReqs(Network* network);

  NM_INLINE AttribDataBlendWeights() { setType(ATTRIB_TYPE_BLEND_WEIGHTS); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_BLEND_WEIGHTS; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  // Weights
  NM_INLINE void setTrajectoryAndTransformsWeights(
    uint16_t     numWeights,
    const float* weights);

  NM_INLINE void setEventsWeights(
    uint16_t     numWeights,
    const float* weights);

  NM_INLINE void setSampledEventsWeights(
    uint16_t     numWeights,
    const float* weights);

  NM_INLINE void setAllWeights(
    float weight);

  NM_INLINE void setAllWeights(
    uint16_t     numWeights,
    const float* weights);

public:
  uint16_t      m_maxNumWeights;                      ///< The maximum number of weights in any sub catergory. NOT the total number of weights.
  uint16_t      m_trajectoryAndTransformsNumWeights;  ///<
  uint16_t      m_eventsNumWeights;                   ///< The number of weights for all non-sampled events.
  uint16_t      m_sampledEventsNumWeights;            ///< The number of weights for the sampled events only.

  // Blend node weights
  float*        m_trajectoryAndTransformsWeights;
  float*        m_eventsWeights;
  float*        m_sampledEventsWeights;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataBlendWeights::setTrajectoryAndTransformsWeights(
  uint16_t     numWeights,
  const float* weights)
{
  NMP_ASSERT(numWeights <= m_maxNumWeights);
  m_trajectoryAndTransformsNumWeights = numWeights;
  for (uint16_t i = 0; i < numWeights; ++i)
    m_trajectoryAndTransformsWeights[i] = weights[i];
}

//---------------------------------------------------------------------------------------------------------------------- 
NM_INLINE void AttribDataBlendWeights::setEventsWeights(
  uint16_t     numWeights,
  const float* weights)
{
  NMP_ASSERT(numWeights <= m_maxNumWeights);
  m_eventsNumWeights = numWeights;
  for (uint16_t i = 0; i < numWeights; ++i)
    m_eventsWeights[i] = weights[i];
}

//---------------------------------------------------------------------------------------------------------------------- 
NM_INLINE void AttribDataBlendWeights::setSampledEventsWeights(
  uint16_t     numWeights,
  const float* weights)
{
  NMP_ASSERT(numWeights <= m_maxNumWeights);
  m_sampledEventsNumWeights = numWeights;
  for (uint16_t i = 0; i < numWeights; ++i)
    m_sampledEventsWeights[i] = weights[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataBlendWeights::setAllWeights(float weight)
{
  NMP_ASSERT(m_maxNumWeights == 1);
  m_trajectoryAndTransformsNumWeights = m_eventsNumWeights = m_sampledEventsNumWeights = 1;
  m_trajectoryAndTransformsWeights[0] = m_eventsWeights[0] = m_sampledEventsWeights[0] = weight;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataBlendWeights::setAllWeights(
  uint16_t     numWeights,
  const float* weights)
{
  NMP_ASSERT(numWeights <= m_maxNumWeights);
  m_trajectoryAndTransformsNumWeights = numWeights;
  m_eventsNumWeights = numWeights;
  m_sampledEventsNumWeights = numWeights;
  for (uint16_t i = 0; i < numWeights; ++i)
  {
    m_trajectoryAndTransformsWeights[i] = weights[i];
    m_eventsWeights[i] = weights[i];
    m_sampledEventsWeights[i] = weights[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataFeatherBlend2ChannelAlphas
/// \brief An array of channel alpha values required for feather blending
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataFeatherBlend2ChannelAlphas : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint16_t numChannelAlphas);

  static AttribDataFeatherBlend2ChannelAlphas* init(
    NMP::Memory::Resource& resource,
    uint16_t               numChannelAlphas,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint16_t              numChannelAlphas,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataFeatherBlend2ChannelAlphas() { setType(ATTRIB_TYPE_FEATHER_BLEND2_CHANNEL_ALPHAS); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_FEATHER_BLEND2_CHANNEL_ALPHAS; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

public:
  uint16_t  m_trajectoryBoneIndex;        ///< The index of the alpha weight for the trajectory bone
  uint16_t  m_numChannelAlphas;           ///< The number of channel alpha values  
  float*    m_channelAlphas;              ///< The array of channel alpha weights
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataScatterBlendAnalysisDef
/// \brief Attribute data for the scatter blend node definition.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataScatterBlendAnalysisDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint16_t maxNumWeights,
    uint16_t maxNumNodeIDs);

  static AttribDataScatterBlendAnalysisDef* init(
    NMP::Memory::Resource& resource,
    uint16_t               maxNumWeights,
    uint16_t               maxNumNodeIDs,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataScatterBlendAnalysisDef() { setType(ATTRIB_TYPE_SCATTER_BLEND_ANALYSIS_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SCATTER_BLEND_ANALYSIS_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  NM_INLINE void setWeights(
    uint16_t     numWeights,
    const float* weights);

  NM_INLINE void setNodeIDs(
    uint16_t      numNodeIDs,
    const NodeID* nodeIDs);

public:
  // Reserved sizes
  uint16_t      m_maxNumWeights;
  uint16_t      m_maxNumNodeIDs;

  // Data
  uint16_t      m_numWeights;
  uint16_t      m_numNodeIDs;
  float*        m_weights;
  NodeID*       m_nodeIDs;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataScatterBlendAnalysisDef::setWeights(
  uint16_t     numWeights,
  const float* weights)
{
  NMP_ASSERT(numWeights <= m_maxNumWeights);
  m_numWeights = numWeights;
  for (uint16_t i = 0; i < numWeights; ++i)
    m_weights[i] = weights[i];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataScatterBlendAnalysisDef::setNodeIDs(
  uint16_t      numNodeIDs,
  const NodeID* nodeIDs)
{
  NMP_ASSERT(numNodeIDs <= m_maxNumNodeIDs);
  m_numNodeIDs = numNodeIDs;
  for (uint16_t i = 0; i < numNodeIDs; ++i)
    m_nodeIDs[i] = nodeIDs[i];
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataScatterBlend1DDef
/// \brief Def data for the 1D scatter blend node
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataScatterBlend1DDef : public AttribData
{
public: 
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numSubDivisions,
    uint32_t numSubSamplesPerLine,
    uint32_t numVertices,
    uint32_t numLines,
    uint32_t numSubSamples);

  static AttribDataScatterBlend1DDef* init(
    NMP::Memory::Resource& resource,
    uint32_t               numSubDivisions,
    uint32_t               numSubSamplesPerLine,
    uint32_t               numVertices,
    uint32_t               numLines,
    uint32_t               numSubSamples);

  NM_INLINE AttribDataScatterBlend1DDef() { setType(ATTRIB_TYPE_SCATTER_BLEND_1D_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SCATTER_BLEND_1D_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  NM_INLINE const uint16_t* getLineSampleIndices(
    uint32_t lineIndex) const;

public:
  // Information
  uint16_t                    m_numSubDivisions;          ///< The number of sub-divisions used for sub-sampling.
  uint16_t                    m_numSubSamplesPerLine;     ///< The number of sub-samples per line.
  uint16_t                    m_numVertices;              ///< The number of annotated vertices
  uint16_t                    m_numLines;                 ///< The number of annotated line segments
  uint16_t                    m_numSubSamples;            ///< The number of unique sub-division samples excluding the annotation vertices.

  // Unit scaling
  float                       m_unitScale;                ///< Scales the input control parameter

  // Vertex source indices
  uint16_t*                   m_vertexSourceIndices;      ///< The pin indices for the vertex sources

  // Motion parameter sample data
  float*                      m_samples;                  ///< Motion parameter sample data

  // Line annotation
  uint16_t*                   m_lineSampleIndices;        ///< Indices into the samples list for the line sub-samples.
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* AttribDataScatterBlend1DDef::getLineSampleIndices(
  uint32_t lineIndex) const
{
  NMP_ASSERT(lineIndex < NMP::nmAlignedValue4(m_numLines));
  return &m_lineSampleIndices[lineIndex * m_numSubSamplesPerLine];
}

//----------------------------------------------------------------------------------------------------------------------
enum ScatterBlend2DProjectionMode
{
  kScatterBlend2DClosestApproach = 0,
  kScatterBlend2DProjectTowardsTarget,
  kScatterBlend2DProjectInDirection,
  kScatterBlend2DProjectInAxisForFacet,

  NumScatterBlend2DProjectionModes,
  kScatterBlend2DInvalidProjectionMode = 0xFFFFFFFF
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataScatterBlend2DDef
/// \brief Def data for the 2D scatter blend node
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataScatterBlend2DDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numTriangleSubDivisions,
    uint32_t numSubSamplesPerTriangle,
    uint32_t numVertices,
    uint32_t numTriangles,
    uint32_t numSubSamples,
    uint32_t numExteriorEdgeFacets,
    uint32_t bspNumBranchNodes,
    uint32_t bspNumLeafNodes,
    uint32_t bspNumLeafNodeEntries);

  static NMP::Memory::Format getBSPMemoryRequirements(
    uint32_t numTriangleSubDivisions,
    uint32_t numTriangles,
    uint32_t bspNumBranchNodes,
    uint32_t bspNumLeafNodes,
    uint32_t bspNumLeafNodeEntries);

  static AttribDataScatterBlend2DDef* init(
    NMP::Memory::Resource& resource,
    uint32_t               numTriangleSubDivisions,
    uint32_t               numSubSamplesPerTriangle,
    uint32_t               numVertices,
    uint32_t               numTriangles,
    uint32_t               numSubSamples,
    uint32_t               numExteriorEdgeFacets,
    uint32_t               bspNumBranchNodes,
    uint32_t               bspNumLeafNodes,
    uint32_t               bspNumLeafNodeEntries);

  NM_INLINE AttribDataScatterBlend2DDef() { setType(ATTRIB_TYPE_SCATTER_BLEND_2D_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SCATTER_BLEND_2D_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }

  NM_INLINE const uint16_t* getTriangleSampleIndices(
    uint16_t triangleIndex) const;

  NM_INLINE const uint16_t* getTriangleSampleIndices(
    uint32_t triangleIndex) const;

  NM_INLINE bool hasBSPTree() const;

public:
  // Information
  uint16_t                    m_numTriangleSubDivisions;      ///< The number of triangle sub-divisions used for sub-sampling.
  uint16_t                    m_numSubSamplesPerTriangle;     ///< The number of sub-samples per triangle.
  uint16_t                    m_numVertices;                  ///< The number of annotated vertices
  uint16_t                    m_numTriangles;                 ///< The number of annotated triangles
  uint16_t                    m_numSubSamples;                ///< The number of unique sub-division samples excluding the annotation vertices.
  uint16_t                    m_numExteriorEdgeFacets;        ///< The number of exterior triangle facets for the annotation.
  uint16_t                    m_bspNumBranchNodes;            ///< The number of BSP branch nodes
  uint16_t                    m_bspNumLeafNodes;              ///< The number of BSP leaf nodes
  uint16_t                    m_bspNumLeafNodeEntries;        ///< The number of leaf node data entries
  uint16_t                    m_bspMaxNumSubTrianglesInPartition;  ///< The maximum number of sub-triangles to search in any of the partitions.
  uint16_t                    m_bspUnpackTriangleIndicesFn;   ///< The index of the triangle index unpacking function

  // Blend mode options
  ScatterBlend2DProjectionMode m_projectionMode;              ///< Controls how the input contol parameter is projected onto the annotation
                                                              ///< if it lies outside the exterior boundary.
  float                       m_projectionVector[2];          ///< The projection vector

  // Display scales
  float                       m_displayScale[2];              ///< Used in InAxisForFacet projection to scale the scatter blend
                                                              ///< data to a similar aspect ratio to the scatter blend viewport.

  // Unit scaling
  float                       m_unitScale[2];                 ///< Scales the input control parameters X, Y

  // Motion parameter sample data
  float                       m_qScale[2];                    ///< Quantisation scales
  float                       m_qOffset[2];                   ///< Quantisation offsets
  uint16_t*                   m_samples[2];                   ///< Quantised motion parameter sample data (X, Y)

  // Triangle annotation
  uint16_t*                   m_triangleSampleIndices;        ///< Indices into the samples list for the triangle sub-samples.

  // Exterior triangle facets: (triangle id, edge facet id) pairs
  uint16_t*                   m_exteriorTriangleIndices;      ///< Triangle ids
  uint8_t*                    m_exteriorEdgeFacetIndices;     ///< Triangle facet ids. Index (0-3) into m_triangleFacetIndices.

  // Binary space partition tree
  uint16_t*                   m_bspBranchNodeLeft;                ///< Index of the BSP node left of the partition.
  uint16_t*                   m_bspBranchNodeRight;               ///< Index of the BSP node right of the partition.
  uint16_t*                   m_bspBranchNodeHyperPlaneSampleIndices[2];  ///< Three triangle vertex sample indices that construct the hyper-plane
  uint16_t*                   m_bspLeafNodeEntryOffsets;          ///< Entry offsets for the leaf node data. An extra element is added to this array
                                                                  ///< so that the entry counts can be recovered by forward difference.
  void*                       m_bspLeafNodeTriangleIndices;       ///< Triangle indices corresponding to each leaf partition.
                                                                  ///< The length if the array depends on the number of triangles in the annotation.
  void*                       m_bspLeafNodeSubTriangleFlags;      ///< Sub-triangle indices corresponding to each leaf partition (Encoded as bit masks).
                                                                  ///< The length of the bit mask depends on the number of sub-divisions:
                                                                  ///< 0 - NULL, 1 - 8-bits, 2 - 64-bits
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* AttribDataScatterBlend2DDef::getTriangleSampleIndices(
  uint16_t triangleIndex) const
{
  NMP_ASSERT(triangleIndex < m_numTriangles);
  return &m_triangleSampleIndices[triangleIndex * m_numSubSamplesPerTriangle];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* AttribDataScatterBlend2DDef::getTriangleSampleIndices(
  uint32_t triangleIndex) const
{
  NMP_ASSERT(triangleIndex < m_numTriangles);
  return &m_triangleSampleIndices[triangleIndex * m_numSubSamplesPerTriangle];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool AttribDataScatterBlend2DDef::hasBSPTree() const
{
  return m_bspBranchNodeLeft != NULL;
}

//----------------------------------------------------------------------------------------------------------------------
// ScatterBlend2DParameteriserInfo
//----------------------------------------------------------------------------------------------------------------------
class ScatterBlend2DParameteriserInfo
{
public:
  NodeID    m_childNodeIDs[3];        ///< The node IDs of the active blend sources
  float     m_childNodeWeights[3];    ///< The Barycentric weights for the active sources

  float     m_desiredParameters[2];   ///< The desired motion parameters requested from the scatter blend
  float     m_achievedParameters[2];  ///< The achieved motion parameters from the scatter blend (Projection onto the annotation)
  bool      m_wasProjected;           ///< Flag indicating if the desired motion parameters were projected back back onto the annotation.
};


//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataScatterBlend2DState
/// \brief Structure for describing the interpolation weights used by a ScatterBlend2D.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataScatterBlend2DState : public AttribData
{
public: 
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataScatterBlend2DState* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    AttribDataCreateDesc* desc);

  NM_INLINE AttribDataScatterBlend2DState() { setType(ATTRIB_TYPE_SCATTER_BLEND_2D_STATE); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SCATTER_BLEND_2D_STATE; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void relocate(AttribData* target);

public:
  ScatterBlend2DParameteriserInfo m_parameteriserInfo;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ATTRIBDATA
//----------------------------------------------------------------------------------------------------------------------
