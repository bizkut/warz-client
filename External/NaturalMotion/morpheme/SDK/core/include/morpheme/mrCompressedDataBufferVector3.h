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
#ifndef NMP_COMPRESSED_DATA_BUFFER_VECTOR3_H
#define NMP_COMPRESSED_DATA_BUFFER_VECTOR3_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMVector3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
/// \class MR::CompressedDataBufferVector3
/// \brief Compressed array of vector3 elements
/// \ingroup CoreModule
///
/// Compressed in the following fashion:
///   - 10 bits,  (0 -  9 = Unsigned z component).
///   - 11 bits,  (10 - 20 = Unsigned y component).
///   - 11 bits,  (21 - 31 = Unsigned x component).
//----------------------------------------------------------------------------------------------------------------------
class CompressedDataBufferVector3
{
public:
  void locate();
  void dislocate();
  
  /// \brief Calculate the memory requirements of an instance, given the parameters.
  static NMP::Memory::Format getMemoryRequirements(uint32_t length);
  
  /// \brief Initialise an instance into the provided memory region.
  static CompressedDataBufferVector3* init(
    NMP::Memory::Resource&   resource,
    uint32_t                 length);
  
  /// \brief Encodes a data buffer
  void encode(const NMP::Vector3* dataBuffer);

  /// \brief Decodes a data buffer
  void decode(NMP::Vector3* dataBuffer) const;
  
  /// \brief Multiplexed decode. The data buffer is decoded in blocks of four
  /// as 4-vectors of X, Y, Z and W components
  void decodeMP(NMP::Vector3* dataBuffer) const;

protected:
  CompressedDataBufferVector3();
  ~CompressedDataBufferVector3();

  NMP::Vector3    m_scales;           ///< The quantisation scales
  NMP::Vector3    m_offsets;          ///< The quantisation offsets
  uint32_t        m_length;           ///< Number of entries in this buffer.
  uint32_t*       m_sampledKeys;      ///< The compressed sample data
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_COMPRESSED_DATA_BUFFER_VECTOR3_H
//----------------------------------------------------------------------------------------------------------------------
