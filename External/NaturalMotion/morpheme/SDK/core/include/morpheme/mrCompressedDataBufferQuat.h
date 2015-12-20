// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef NMP_COMPRESSED_DATA_BUFFER_QUAT_H
#define NMP_COMPRESSED_DATA_BUFFER_QUAT_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMQuat.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
/// \class MR::CompressedDataBufferQuat
/// \brief Compressed array of quat elements (quantised tan quarter angle rotation vectors)
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------
class CompressedDataBufferQuat
{
public:
  void locate();
  void dislocate();
  
  /// \brief Calculate the memory requirements of an instance, given the parameters.
  static NMP::Memory::Format getMemoryRequirements(uint32_t length);
  
  /// \brief Initialise an instance into the provided memory region.
  static CompressedDataBufferQuat* init(
    NMP::Memory::Resource&   resource,
    uint32_t                 length);
  
  /// \brief Encodes a data buffer
  void encode(const NMP::Quat* dataBuffer, NMP::Vector3* tempBuffer);

  /// \brief Decodes a data buffer
  void decode(NMP::Quat* dataBuffer) const;
  
  /// \brief Multiplexed decode. The data buffer is decoded in blocks of four
  /// as 4-vectors of X, Y, Z and W components
  void decodeMP(NMP::Quat* dataBuffer) const;

protected:
  CompressedDataBufferQuat();
  ~CompressedDataBufferQuat();
  
  NMP::Vector3    m_scales;           ///< The quantisation scales
  NMP::Vector3    m_offsets;          ///< The quantisation offsets
  uint32_t        m_length;           ///< Number of entries in this buffer.
  uint16_t*       m_sampledKeysX;     ///< The compressed sample data (X components)
  uint16_t*       m_sampledKeysY;     ///< The compressed sample data (Y components)
  uint16_t*       m_sampledKeysZ;     ///< The compressed sample data (Z components)
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_COMPRESSED_DATA_BUFFER_QUAT_H
//----------------------------------------------------------------------------------------------------------------------
