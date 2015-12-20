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
#ifndef VECTOR3_QUANTISATION_BASIS_H
#define VECTOR3_QUANTISATION_BASIS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Vector3QuantisationSetBasis
//----------------------------------------------------------------------------------------------------------------------
class Vector3QuantisationSetBasis
{
public:
  /// \brief Calculate the memory required to create a Vector3QuantisationSetBasis from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numChannelSets,            ///< IN: The number of vector3 channels in the table
    uint32_t numQuantisationSets        ///< IN: The desired number of quantisation sets
    );

  /// \brief Create a new Vector3QuantisationSetBasis in the supplied memory.
  static Vector3QuantisationSetBasis* init(
    NMP::Memory::Resource&  resource,             ///< IN: Resource description of where to build the table
    uint32_t                numChannelSets,       ///< IN: The number of vector3 channels in the table
    uint32_t                numQuantisationSets   ///< IN: The desired number of quantisation sets
    );

public:
  Vector3QuantisationSetBasis() {}
  ~Vector3QuantisationSetBasis() {}

  void computeBasisVectors(
    uint32_t numEntries,
    const NMP::Vector3* const* keysTable,
    const uint32_t* keysTableCounts);
    
  uint32_t getNumChannelSets() const;
  uint32_t getNumQuantisationSets() const;

  const NMP::Vector3* getQSetMin() const;
  const NMP::Vector3* getQSetMax() const;
  const uint32_t* getChanSetToQSetMapX() const;
  const uint32_t* getChanSetToQSetMapY() const;
  const uint32_t* getChanSetToQSetMapZ() const;

protected:
  uint32_t          m_numChannelSets;
  uint32_t          m_numQuantisationSets;
  NMP::Vector3*     m_qSetMin;
  NMP::Vector3*     m_qSetMax;
  uint32_t*         m_chanSetToQSetMapX;
  uint32_t*         m_chanSetToQSetMapY;
  uint32_t*         m_chanSetToQSetMapZ;
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // VECTOR3_QUANTISATION_BASIS_H
//----------------------------------------------------------------------------------------------------------------------
