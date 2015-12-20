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
#ifndef NMP_DATA_BUFFER_H
#define NMP_DATA_BUFFER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBitArray.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
//----------------------------------------------------------------------------------------------------------------------

// forcing value to bool 'true' or 'false'
// used when getting if a bit is set
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4800)
#endif

namespace NMP
{

class MemoryAllocator;

struct PosQuat
{
  NMP::Vector3 m_pos;
  NMP::Quat    m_quat;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::DataBuffer
/// \brief Generic array of elements including used/unused flags.
/// \ingroup NaturalMotionPlatform
///
/// The buffer class knows only about the memory requirements (stride/alignment) of each element and the number of
/// elements in the buffer.  It knows nothing of the structure of the individual elements.  External objects can infer
/// this from the user-defined 'type' property, which is intended as a verification mechanism only.
///
/// This Buffer also maintains a list of used/unused flags.
//----------------------------------------------------------------------------------------------------------------------
class DataBuffer
{
public:

  /// \enum  ElementType
  /// \brief Identifiers for element types that can be members of a DataBuffer instance.
  /// \ingroup NaturalMotionPlatform
  enum ElementType
  {
    NMP_ELEMENT_TYPE_INT = 0,
    NMP_ELEMENT_TYPE_FLOAT,
    NMP_ELEMENT_TYPE_VEC3,
    NMP_ELEMENT_TYPE_POSVEL,
    NMP_ELEMENT_TYPE_ANGVEL,
    NMP_ELEMENT_TYPE_VEC4,
    NMP_ELEMENT_TYPE_QUAT,
    NMP_ELEMENT_TYPE_QUAT_COMP,
    NMP_ELEMENT_TYPE_POS_COMP,
    NMP_ELEMENT_TYPE_MATRIX,
    NMP_ELEMENT_TYPE_MAX,
    NMP_ELEMENT_TYPE_USER = 0xFFFF0000
  };

  struct ElementDescriptor
  {
    ElementDescriptor() :
      m_type(NMP_ELEMENT_TYPE_USER),
      m_size(0),
      m_alignment(0)
    {}
    ElementDescriptor(ElementType type, uint32_t size, uint32_t alignment) :
      m_type(type),
      m_size(size),
      m_alignment(alignment)
    {}

    NM_INLINE bool locate();
    NM_INLINE bool dislocate();

    ElementType m_type;
    uint32_t    m_size;
    uint32_t    m_alignment;
  };

  /// Some element index positions used in some commonly used buffer configurations.
  /// Allows for much faster path element access.
  static const uint32_t POS_QUAT_BUFFER_POS_ELEMENT_INDEX;             ///< Element index of the Quat in a PosQuat DataBuffer.
  static const uint32_t POS_QUAT_BUFFER_QUAT_ELEMENT_INDEX;            ///< Element index of the Quat in a PosQuat DataBuffer.
  static const uint32_t POS_VEL_ANG_VEL_BUFFER_POS_VEL_ELEMENT_INDEX;  ///< Element index of the Position Velocity in a PosVelAngVel DataBuffer.
  static const uint32_t POS_VEL_ANG_VEL_BUFFER_ANG_VEL_ELEMENT_INDEX;  ///< Element index of the Angular Velocity in a PosVelAngVel DataBuffer.

  /// Descriptors used in the construction of some commonly used buffer configurations.
  /// (PosQuat Buffers and PosVelAngVel Buffers).
  static const ElementDescriptor POS_ELEMENT_DESCRIPTOR;
  static const ElementDescriptor QUAT_ELEMENT_DESCRIPTOR;
  static const ElementDescriptor POS_VEL_ELEMENT_DESCRIPTOR;
  static const ElementDescriptor ANG_VEL_ELEMENT_DESCRIPTOR;
  static const ElementDescriptor MATRIX_ELEMENT_DESCRIPTOR;

  /// PosQuat Buffer description.
  static ElementDescriptor sm_PosQuatDescriptor[2];

  /// PosVelAngVel Buffer descriptor.
  static ElementDescriptor sm_PosVelAngVelDescriptor[2];

  //--------------------------
  // Generic format buffer functions.

  /// \brief Calculate the memory requirements of an instance, given the parameters.
  static Memory::Format getMemoryRequirements(
    uint32_t                 numElements,
    const ElementDescriptor* elements,
    uint32_t                 length);

  /// \brief Initialise an instance into the provided memory region.
  ///
  /// Fastest method of creating a buffer if you already know its memory requirements.
  /// \return Pointer to the newly created instance. NULL on failure.
  static DataBuffer* init(
    Memory::Resource&        resource,    ///< Resource to build the buffer in to.
    const Memory::Format&    memoryReqs,  ///< The memory requirements of this data buffer instance.
    uint32_t                 numElements, ///< The number of channels that will make up this buffer.
    const ElementDescriptor* elements,    ///< A description of each of the channels making up this buffer.
    uint32_t                 length       ///< The number of entries this buffer should have.
    );

  /// \brief Allocate and initialise an instance.
  ///
  /// On initialization, all elements are marked as used, but are left uninitialized.
  /// \return Pointer to the newly created instance. NULL on failure.
  static DataBuffer* create(
    MemoryAllocator*         allocator,
    const Memory::Format&    memoryReqs,
    uint32_t                 numElements,
    const ElementDescriptor* elements,
    uint32_t                 length);

  //--------------------------
  // PosQuat format buffer functions.

  /// \brief Calculate the memory requirements of a PosQuatBuffer instance, given the parameters.
  static Memory::Format getPosQuatMemoryRequirements(uint32_t length);

  /// \brief Allocate and initialise a PosQuatBuffer instance.
  ///
  /// On initialization, all elements are marked as used, but are left uninitialized.
  /// \return Pointer to the newly created instance. NULL on failure.
  static DataBuffer* createPosQuat(
    MemoryAllocator*      allocator,
    const Memory::Format& memoryReqs,
    uint32_t              length);

  /// \brief Initialise a PosQuatBuffer instance into the provided memory region.
  ///
  /// Fastest method of creating a pos quat buffer if you already know its memory requirements.
  /// \return Pointer to the newly created instance. NULL on failure.
  static DataBuffer* initPosQuat(
    Memory::Resource&     resource,   ///< Resource to build the buffer in to.
    const Memory::Format& memoryReqs, ///< The memory requirements of this data buffer instance.
    uint32_t              length      ///< How many pos quat entries this buffer should have.
    );

  /// \brief Is this a pos quat buffer.
  NM_INLINE bool isPosQuat() const;

  //--------------------------
  // PosVelAngVel format buffer functions.

  /// \brief Calculate the memory requirements of a PosVelAngVelBuffer instance, given the parameters.
  static Memory::Format getPosVelAngVelMemoryRequirements(uint32_t length);

  /// \brief Allocate and initialise a  PosVelAngVelBuffer instance.
  ///
  /// On initialization, all elements are marked as used, but are left uninitialized.
  /// \return Pointer to the newly created instance. NULL on failure.
  static DataBuffer* createPosVelAngVel(
    MemoryAllocator*      allocator,
    const Memory::Format& memoryReqs,
    uint32_t              length);

  /// \brief Initialise a PosVelAngVelBuffer instance into the provided memory region.
  ///
  /// Fastest method of creating a PositionalVelocity AngularVelocity buffer if you already know its memory requirements.
  /// \return Pointer to the newly created instance. NULL on failure.
  static DataBuffer* initPosVelAngVel(
    Memory::Resource&     resource,   ///< Resource to build the buffer in to.
    const Memory::Format& memoryReqs, ///< The memory requirements of this data buffer instance.
    uint32_t              length      ///< How many pos velocity angle velocity entries this buffer should have.
    );

  /// \brief Is this a PositionalVelocity AngularVelocity buffer.
  NM_INLINE bool isPosVelAngVel() const;

  //--------------------------

  /// \brief Calculate the memory requirements of this buffer.
  NM_INLINE Memory::Format getInstanceMemoryRequirements() const { return m_memoryReqs; }

  /// \brief Prepare a dislocated DataBuffer for use.
  NM_INLINE bool locate();

  /// \brief Dislocate a DataBuffer ready to move to a new memory location.
  NM_INLINE bool dislocate();

  /// \brief Recalculate the pointers in this DataBuffer.  This is useful when the buffer has
  /// been moved to a new memory space.
  NM_INLINE bool relocate(void* location);
  NM_INLINE bool relocate() { return relocate(this); }

  /// \brief Create a copy of this buffer in the memory resource provided.
  /// \return Newly created buffer.
  NM_INLINE DataBuffer* copyTo(
    const Memory::Resource& resource) const; ///< Memory resource in which to create the buffer copy.

  /// \brief Create a copy of this buffer in the destination buffer provided.
  /// \return Newly copied buffer.
  NM_INLINE DataBuffer* copyTo(
    DataBuffer* destBuffer) const;         ///< Buffer to copy to.

  /// \brief Return the maximum number of elements that this buffer can contain.
  NM_INLINE uint32_t getLength() const { return m_length; }

  /// \brief Set the maximum number of elements that this buffer can contain
  ///  Note: after setting this flag, you should restore pointers in order to ensure the buffer is
  ///   setup correctly.  Be careful not to set the length beyond the memory available.  This function
  ///   does NOT reallocate memory to accommodate a larger buffer than was originally created.
  NM_INLINE void setLength(uint32_t length) { m_length = length; };

  /// \brief Get the number of discrete elements in each entry in this buffer.
  NM_INLINE uint32_t getNumElements() const { return m_numElements; }

  /// \brief Get the full channel set status.
  NM_INLINE bool isFull() const { return m_full; }

  /// \brief Set the full cache value by checking the used flags.
  NM_INLINE void calculateFullFlag() { m_full = getUsedFlags()->calculateAreAllSet(); }

  /// \brief Set the full cache value externally - to be used with care.
  NM_INLINE void setFullFlag(bool full) { m_full = full; NMP_ASSERT(m_full == getUsedFlags()->calculateAreAllSet()); }

  /// \brief Get pointer to the array of element descriptors.
  NM_INLINE ElementDescriptor* getElementDescriptors();

  /// \brief Obtain a descriptor for one of the elements of a data buffer entry.
  NM_INLINE ElementDescriptor getElementDescriptor(uint32_t elementIdx);

  /// \brief Get the element data for a given element index
  NM_INLINE void* getElementData(uint32_t elementIdx);

  /// \brief Get the element data for a given element index
  NM_INLINE void* getElementData(uint32_t elementIdx) const;

  /// \brief Set the used flag for a given entry in the buffer. Use setChannelUsed()/setChannelUnused() where the value
  /// of 'used' is constant.
  NM_INLINE bool setChannelUsedFlag(uint32_t index, bool used);

  /// \brief Set the used flag to true for a given entry in the buffer.
  NM_INLINE void setChannelUsed(uint32_t index);

  /// \brief Set the used flag to true for a given entry in the buffer.
  NM_INLINE void setChannelUnused(uint32_t index);

  /// \brief Does this buffer have valid data for the specified channel.
  NM_INLINE bool hasChannel(uint32_t channelIndex) const;

  /// \brief Get the content of the specified channel.
  NM_INLINE Quat* getChannelQuat(uint32_t channelID) const;
  NM_INLINE Vector3* getChannelPos(uint32_t channelID) const;
  NM_INLINE Quat* getChannelQuatFast(uint32_t channelID) const;
  NM_INLINE Vector3* getChannelPosFast(uint32_t channelID) const;

  NM_INLINE Vector3* getChannelVel(uint32_t channelID) const;
  NM_INLINE Vector3* getChannelAngVel(uint32_t channelID) const;

  /// \brief Set the attitude part of a channel.
  /// NOTE: Does not flag this channel as having valid data (this needs to be done independently with setChannelUsed).
  NM_INLINE void setChannelQuat(uint32_t channelID, const Quat& quat);

  /// \brief Set the position part of a channel.
  /// NOTE: Does not flag this channel as having valid data (this needs to be done independently with setChannelUsed).
  NM_INLINE void setChannelPos(uint32_t channelID, const Vector3& pos);

  /// \brief Set the velocity part of a channel.
  /// NOTE: Does not flag this channel as having valid data (this needs to be done independently with setChannelUsed).
  NM_INLINE void setChannelVel(uint32_t channelID, const Vector3& pos);

  /// \brief Set the angular velocity part of a channel.
  /// NOTE: Does not flag this channel as having valid data (this needs to be done independently with setChannelUsed).
  NM_INLINE void setChannelAngVel(uint32_t channelID, const Vector3& pos);

  /// \brief Get the content of the specified channel assuming a { NMP_ELEMENT_POS, NMP_ELEMENT_QUAT } buffer. This is
  /// faster than using the generic getChannelQuat().
  NM_INLINE Quat* getPosQuatChannelQuat(uint32_t channelID) const;

  /// \brief Get the content of the specified channel assuming a { NMP_ELEMENT_POS, NMP_ELEMENT_QUAT } buffer. This is
  /// faster than using the generic getChannelPos().
  NM_INLINE Vector3* getPosQuatChannelPos(uint32_t channelID) const;

  /// \brief Get the content of the specified channel assuming a { NMP_ELEMENT_VEL, NMP_ELEMENT_ANGVEL } buffer. This is
  /// faster than using the generic getChannelVel().
  NM_INLINE Vector3* getPosVelAngVelChannelPosVel(uint32_t channelID) const;

  /// \brief Get the content of the specified channel assuming a { NMP_ELEMENT_VEL, NMP_ELEMENT_ANGVEL } buffer. This is
  /// faster than using the generic getChannelAngVel().
  NM_INLINE Vector3* getPosVelAngVelChannelAngVel(uint32_t channelID) const;

  /// \brief Set the attitude part of a channel assuming a { NMP_ELEMENT_POS, NMP_ELEMENT_QUAT } buffer.
  /// NOTE: Does not flag this channel as having valid data (this needs to be done independently with setChannelUsed).
  NM_INLINE void setPosQuatChannelQuat(uint32_t channelID, const Quat& quat);

  /// \brief Set the position part of a channel assuming a { NMP_ELEMENT_POS, NMP_ELEMENT_QUAT } buffer.
  /// NOTE: Does not flag this channel as having valid data (this needs to be done independently with setChannelUsed).
  NM_INLINE void setPosQuatChannelPos(uint32_t channelID, const Vector3& pos);

  /// \brief Set the velocity part of a channel assuming a { NMP_ELEMENT_VEL, NMP_ELEMENT_ANGVEL } buffer.
  /// NOTE: Does not flag this channel as having valid data (this needs to be done independently with setChannelUsed).
  NM_INLINE void setPosVelAngVelChannelPosVel(uint32_t channelID, const Vector3& pos);

  /// \brief Set the angular velocity part of a channel assuming a { NMP_ELEMENT_VEL, NMP_ELEMENT_ANGVEL } buffer.
  /// NOTE: Does not flag this channel as having valid data (this needs to be done independently with setChannelUsed).
  NM_INLINE void setPosVelAngVelChannelAngVel(uint32_t channelID, const Vector3& pos);

  /// \brief Set the transform of the specified channel.
  NM_INLINE void setTransform(const uint32_t channelID, const Matrix34& transform);

  /// \brief Get the transform from the specified channel.
  NM_INLINE const Matrix34* getTransform(const uint32_t channelID) const;

  /// \brief Retrieve the BitArray that specifies which entries in the buffer are used.
  NM_INLINE BitArray* getUsedFlags() { return m_usedFlags; }
  NM_INLINE const BitArray* getUsedFlags() const { return (const BitArray*) m_usedFlags; }

  /// \brief Initialise the any padding for the array to sensible values for a pos quat buffer.
  /// This is useful if the consists of Pos-Quats and vectorised operations are working on chunks of data
  NM_INLINE void padOutPosQuat();
  
  /// \brief Set the position and attitude channel transforms to the identity
  NM_INLINE void setPosQuatBufferIdentity();

protected:
  DataBuffer();
  ~DataBuffer();

  Memory::Format      m_memoryReqs;  ///< Total byte size of this buffer.
  uint32_t            m_length;      ///< Number of entries in this buffer.
  bool                m_full;        ///< Indicates a full m_usedFlags set.

  uint32_t            m_numElements; ///< Number of element that make up an entry.
  ElementDescriptor*  m_elements;    ///< Array of descriptions of all the elements that make up an entry i the buffer.
  void**              m_data;        ///< Array of entry data. Listed by element first i.e. Block for element 1, Block for element 2 etc.
  BitArray*           m_usedFlags;   ///< Indicates which entries have initialised data.

}; // Buffer

//----------------------------------------------------------------------------------------------------------------------
// DataBuffer functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat* DataBuffer::getChannelQuat(uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    if (m_elements[i].m_type == NMP_ELEMENT_TYPE_QUAT)
    {
      // We have found a channel of quats
      return &((Quat*)m_data[i])[channelID];
    }
  }
  NMP_DEBUG_MSG("getChannelQuat has been called on a NMP::DataBuffer which does not contain quats\n");
  NMP_ASSERT_FAIL();
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3* DataBuffer::getChannelPos(uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    if (m_elements[i].m_type == NMP_ELEMENT_TYPE_VEC3)
    {
      // We have found a channel of vectors (and are assuming they are positions)
      return &((Vector3*)m_data[i])[channelID];
    }
  }
  NMP_DEBUG_MSG("getChannelPos has been called on a NMP::DataBuffer which does not contain vectors\n");
  NMP_ASSERT_FAIL();
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat* DataBuffer::getChannelQuatFast(uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);
  NMP_ASSERT(m_elements[1].m_type == NMP_ELEMENT_TYPE_QUAT);

  return &((Quat*)m_data[1])[channelID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3* DataBuffer::getChannelPosFast(uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);
  NMP_ASSERT(m_elements[0].m_type == NMP_ELEMENT_TYPE_VEC3);

  return &((Vector3*)m_data[0])[channelID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3* DataBuffer::getChannelVel(uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    if (m_elements[i].m_type == NMP_ELEMENT_TYPE_POSVEL)
      return &((Vector3*)m_data[i])[channelID];
  }
  NMP_DEBUG_MSG("getChannelVel has been called on a NMP::DataBuffer which does not contain velocities\n");
  NMP_ASSERT_FAIL();
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3* DataBuffer::getChannelAngVel(uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    if (m_elements[i].m_type == NMP_ELEMENT_TYPE_ANGVEL)
      return &((Vector3*)m_data[i])[channelID];
  }
  NMP_DEBUG_MSG("getChannelAngVel has been called on a NMP::DataBuffer which does not contain angular velocities\n");
  NMP_ASSERT_FAIL();
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::setChannelQuat(uint32_t channelID, const Quat& quat)
{
  NMP_ASSERT(channelID < m_length);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    if (m_elements[i].m_type == NMP_ELEMENT_TYPE_QUAT)
    {
      // We have found a channel of quats
      ((Quat*)m_data[i])[channelID] = quat;
      return;
    }
  }
  NMP_DEBUG_MSG("setChannelQuat has been called on a NMP::DataBuffer which does not contain quats\n");
  NMP_ASSERT_FAIL();
  return;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::setChannelPos(uint32_t channelID, const Vector3& pos)
{
  NMP_ASSERT(channelID < m_length);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    if (m_elements[i].m_type == NMP_ELEMENT_TYPE_VEC3)
    {
      // We have found a channel of positions
      ((Vector3*)m_data[i])[channelID] = pos;
      return;
    }
  }
  NMP_DEBUG_MSG("setChannelPos has been called on a NMP::DataBuffer which does not contain positions\n");
  NMP_ASSERT_FAIL();
  return;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Quat* DataBuffer::getPosQuatChannelQuat(uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);
  NMP_ASSERT(m_elements[POS_QUAT_BUFFER_QUAT_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_QUAT);

  return &((Quat*)m_data[POS_QUAT_BUFFER_QUAT_ELEMENT_INDEX])[channelID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3* DataBuffer::getPosQuatChannelPos(uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);
  NMP_ASSERT(m_elements[POS_QUAT_BUFFER_POS_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_VEC3);

  return &((Vector3*)m_data[POS_QUAT_BUFFER_POS_ELEMENT_INDEX])[channelID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3* DataBuffer::getPosVelAngVelChannelPosVel(uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);
  NMP_ASSERT(m_elements[POS_VEL_ANG_VEL_BUFFER_POS_VEL_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_POSVEL);

  return &((Vector3*)m_data[POS_VEL_ANG_VEL_BUFFER_POS_VEL_ELEMENT_INDEX])[channelID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Vector3* DataBuffer::getPosVelAngVelChannelAngVel(uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);
  NMP_ASSERT(m_elements[POS_VEL_ANG_VEL_BUFFER_ANG_VEL_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_ANGVEL);

  return &((Vector3*)m_data[POS_VEL_ANG_VEL_BUFFER_ANG_VEL_ELEMENT_INDEX])[channelID];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::setPosQuatChannelQuat(uint32_t channelID, const Quat& quat)
{
  NMP_ASSERT(channelID < m_length);
  NMP_ASSERT(m_elements[POS_QUAT_BUFFER_QUAT_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_QUAT);

  ((Quat*)m_data[POS_QUAT_BUFFER_QUAT_ELEMENT_INDEX])[channelID] = quat;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::setPosQuatChannelPos(uint32_t channelID, const Vector3& pos)
{
  NMP_ASSERT(channelID < m_length);
  NMP_ASSERT(m_elements[POS_QUAT_BUFFER_POS_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_VEC3);

  ((Vector3*)m_data[POS_QUAT_BUFFER_POS_ELEMENT_INDEX])[channelID] = pos;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::setPosVelAngVelChannelPosVel(uint32_t channelID, const Vector3& vel)
{
  NMP_ASSERT(channelID < m_length);
  NMP_ASSERT(m_elements[POS_VEL_ANG_VEL_BUFFER_POS_VEL_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_POSVEL)

  ((Vector3*)m_data[POS_VEL_ANG_VEL_BUFFER_POS_VEL_ELEMENT_INDEX])[channelID] = vel;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::setPosVelAngVelChannelAngVel(uint32_t channelID, const Vector3& angvel)
{
  NMP_ASSERT(channelID < m_length);
  NMP_ASSERT(m_elements[POS_VEL_ANG_VEL_BUFFER_ANG_VEL_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_ANGVEL)

  ((Vector3*)m_data[POS_VEL_ANG_VEL_BUFFER_ANG_VEL_ELEMENT_INDEX])[channelID] = angvel;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool DataBuffer::isPosQuat() const
{
  if ((m_numElements == 2) &&
      (m_elements[POS_QUAT_BUFFER_QUAT_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_QUAT) &&
      (m_elements[POS_QUAT_BUFFER_POS_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_VEC3))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool DataBuffer::isPosVelAngVel() const
{
  if ((m_numElements == 2) &&
      (m_elements[POS_VEL_ANG_VEL_BUFFER_POS_VEL_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_POSVEL) &&
      (m_elements[POS_VEL_ANG_VEL_BUFFER_ANG_VEL_ELEMENT_INDEX].m_type == NMP_ELEMENT_TYPE_ANGVEL))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::setTransform(const uint32_t channelID, const Matrix34& transform)
{
  NMP_ASSERT(channelID < m_length);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    if (m_elements[i].m_type == NMP_ELEMENT_TYPE_MATRIX)
    {
      // We have found a channel of matrices
      ((Matrix34*)m_data[i])[channelID] = transform;
      return;
    }
  }
  NMP_DEBUG_MSG("setTransform has been called on a NMP::DataBuffer which does not contain matrices\n");
  NMP_ASSERT_FAIL();
  return;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const Matrix34* DataBuffer::getTransform(const uint32_t channelID) const
{
  NMP_ASSERT(channelID < m_length);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    if (m_elements[i].m_type == NMP_ELEMENT_TYPE_MATRIX)
    {
      // We have found a channel of matrices
      return &((Matrix34*)m_data[i])[channelID];
    }
  }
  NMP_DEBUG_MSG("getTransform has been called on a NMP::DataBuffer which does not contain matrices\n");
  NMP_ASSERT_FAIL();
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::setChannelVel(uint32_t channelID, const Vector3& vel)
{
  NMP_ASSERT(channelID < m_length);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    if (m_elements[i].m_type == NMP_ELEMENT_TYPE_POSVEL)
    {
      ((Vector3*)m_data[i])[channelID] = vel;
      return;
    }
  }
  NMP_DEBUG_MSG("setChannelVel has been called on a NMP::DataBuffer which does not contain velocities\n");
  NMP_ASSERT_FAIL();
  return;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::setChannelAngVel(uint32_t channelID, const Vector3& angvel)
{
  NMP_ASSERT(channelID < m_length);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    if (m_elements[i].m_type == NMP_ELEMENT_TYPE_ANGVEL)
    {
      ((Vector3*)m_data[i])[channelID] = angvel;
      return;
    }
  }
  NMP_DEBUG_MSG("setChannelAngVel has been called on a NMP::DataBuffer which does not contain angular velocities\n");
  NMP_ASSERT_FAIL();
  return;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool DataBuffer::setChannelUsedFlag(uint32_t index, bool used)
{
  NMP_ASSERT(index < m_length && m_usedFlags);

  if (used)
    m_usedFlags->setBit(index);
  else
    m_usedFlags->clearBit(index);

  return true;
};

//----------------------------------------------------------------------------------------------------------------------
// and to avoid the conditional
NM_INLINE void DataBuffer::setChannelUsed(uint32_t index)
{
  NMP_ASSERT(index < m_length && m_usedFlags);
  m_usedFlags->setBit(index);
};

//----------------------------------------------------------------------------------------------------------------------
// and to avoid the conditional
NM_INLINE void DataBuffer::setChannelUnused(uint32_t index)
{
  NMP_ASSERT(index < m_length && m_usedFlags);
  m_usedFlags->clearBit(index);
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool DataBuffer::hasChannel(uint32_t channelIndex) const
{
  NMP_ASSERT(channelIndex < m_length && m_usedFlags);
  return m_usedFlags->isBitSet(channelIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void* DataBuffer::getElementData(uint32_t elementIdx)
{
  NMP_ASSERT(elementIdx < m_numElements && m_data);
  return m_data[elementIdx];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void* DataBuffer::getElementData(uint32_t elementIdx) const
{
  NMP_ASSERT(elementIdx < m_numElements && m_data);
  return m_data[elementIdx];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DataBuffer::ElementDescriptor DataBuffer::getElementDescriptor(uint32_t elementIdx)
{
  NMP_ASSERT(elementIdx < m_numElements && m_elements);

  return m_elements[elementIdx];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DataBuffer::ElementDescriptor* DataBuffer::getElementDescriptors()
{
  return m_elements;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool DataBuffer::locate()
{
  endianSwap(m_full);
  endianSwap(m_length);
  endianSwap(m_memoryReqs.alignment);
  endianSwap(m_memoryReqs.size);

  endianSwap(m_numElements);
  REFIX_SWAP_PTR(ElementDescriptor, m_elements);
  for (uint32_t i = 0; i < m_numElements; i++)
  {
    m_elements[i].locate();
  }

  REFIX_SWAP_PTR(void*, m_data);
  for (uint32_t i = 0; i < m_numElements; i++)
  {
    REFIX_SWAP_PTR(void, m_data[i]);
    endianSwapArray(m_data[i], m_length, m_elements[i].m_size);
  }

  REFIX_SWAP_PTR(BitArray, m_usedFlags);
  m_usedFlags->locate();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool DataBuffer::dislocate()
{
  m_usedFlags->dislocate();
  UNFIX_SWAP_PTR(BitArray, m_usedFlags);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    endianSwapArray(m_data[i], m_length, m_elements[i].m_size);
    UNFIX_SWAP_PTR(void, m_data[i]);
  }
  UNFIX_SWAP_PTR(void*, m_data);

  for (uint32_t i = 0; i < m_numElements; i++)
  {
    m_elements[i].dislocate();
  }
  UNFIX_SWAP_PTR(ElementDescriptor, m_elements);
  endianSwap(m_numElements);

  endianSwap(m_memoryReqs.alignment);
  endianSwap(m_memoryReqs.size);
  endianSwap(m_length);
  endianSwap(m_full);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool DataBuffer::relocate(void* location)
{
  // We have to dig out the element descriptors first
  char* ptr =  (char*)(this + 1);
  Memory::Format usedFlagsReq = BitArray::getMemoryRequirements(m_length);
  ptr = (char*)Memory::align(ptr, usedFlagsReq.alignment);
  ptr += usedFlagsReq.size;
  ElementDescriptor* elements = (ElementDescriptor*)(Memory::align(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  Memory::Format elementsReq = Memory::Format(
    sizeof(ElementDescriptor) * m_numElements,
    NMP_NATURAL_TYPE_ALIGNMENT);
  ptr = (char*)Memory::align(ptr, elementsReq.alignment);
  ptr += elementsReq.size;
  void** data = (void**)ptr;

  Memory::Resource localResource = { location, DataBuffer::getMemoryRequirements(m_numElements, elements, m_length) };
  localResource.increment(Memory::Format(sizeof(DataBuffer), NMP_VECTOR_ALIGNMENT));

  // Used flags requirements
  localResource.align(usedFlagsReq);
  m_usedFlags = (BitArray*)localResource.ptr;
  localResource.increment(usedFlagsReq);

  // Set up elements pointer
  
  localResource.align(elementsReq);
  m_elements = (ElementDescriptor*)localResource.ptr;
  localResource.increment(elementsReq);

  // Set up data pointers
  Memory::Format pointersReq = Memory::Format(
                                 sizeof(void*) * m_numElements,
                                 NMP_NATURAL_TYPE_ALIGNMENT);
  localResource.align(pointersReq);
  m_data = (void**)localResource.ptr;
  localResource.increment(pointersReq);

  // Set up data, align to blocks of 4
  uint32_t length4 = (uint32_t)Memory::align(m_length, 4);
  for (uint32_t i = 0; i < m_numElements; i++)
  {
    Memory::Format dataReq = Memory::Format(
                               Memory::align(elements[i].m_size, elements[i].m_alignment) * length4,
                               elements[i].m_alignment);
    localResource.align(dataReq);
    data[i] = localResource.ptr;
    localResource.increment(dataReq);
  }

  return true;
}

//-----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::padOutPosQuat()
{
  // inert values
  Vector3 zv(Vector3::InitZero);
  Quat zq(0, 0, 0, 1.0f);

  // determine padding length (for x4 blending opps)
  uint32_t length4 = (uint32_t)Memory::align(m_length, 4);

  // fill padding
  for (uint32_t i = m_length; i < length4; i++)
  {
    ((Vector3*)m_data[0])[i] = zv;
    ((Quat*)m_data[1])[i] = zq;
  }
}

//-----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DataBuffer::setPosQuatBufferIdentity()
{
  // Identity values
  Vector3 vZero(Vector3::InitZero);
  Quat qIdentity(Quat::kIdentity);

  // Fill with the identity
  NMP::Vector3* pBuffer = (Vector3*)m_data[0];
  NMP::Quat* qBuffer = (Quat*)m_data[1];
  for (uint32_t i = 0; i < m_length; ++i)
  {
    pBuffer[i] = vZero;
    qBuffer[i] = qIdentity;
  }
}

//-----------------------------------------------------------------------------------------------------------------------
// Create a copy of this buffer in the memory resource provided.
NM_INLINE DataBuffer* DataBuffer::copyTo(
  const Memory::Resource& resource) const // Memory resource in which to create the buffer copy.
{
  DataBuffer* result;

  NMP_ASSERT(m_memoryReqs.alignment == resource.format.alignment);
  NMP_ASSERT(resource.format.size == m_memoryReqs.size);
  NMP_ASSERT(resource.ptr && NMP_IS_ALIGNED(resource.ptr, m_memoryReqs.alignment));

  result = (DataBuffer*) resource.ptr;

  Memory::memcpy((void*) result, this, m_memoryReqs.size);

  result->relocate();
  
  return result;
}

//-----------------------------------------------------------------------------------------------------------------------
// Create a copy of this buffer in the destination buffer provided.
NM_INLINE DataBuffer* DataBuffer::copyTo(
  DataBuffer* destBuffer) const           // Buffer to copy to.
{
  NMP_ASSERT(destBuffer);
  NMP_ASSERT(m_memoryReqs.size == destBuffer->m_memoryReqs.size);
  NMP_ASSERT(m_memoryReqs.alignment == destBuffer->m_memoryReqs.alignment);

  Memory::memcpy((void*) destBuffer, this, m_memoryReqs.size);

  destBuffer->relocate();

  return destBuffer;
}

//-----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool DataBuffer::ElementDescriptor::locate()
{
  endianSwap(m_type);
  endianSwap(m_size);
  endianSwap(m_alignment);
  return true;
}

//-----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool DataBuffer::ElementDescriptor::dislocate()
{
  endianSwap(m_type);
  endianSwap(m_size);
  endianSwap(m_alignment);
  return true;
}

} // namespace NMP

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_DATA_BUFFER_H
//----------------------------------------------------------------------------------------------------------------------
