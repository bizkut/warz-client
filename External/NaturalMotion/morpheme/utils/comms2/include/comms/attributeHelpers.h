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
#ifndef MC_ATTRIBUTE_HELPERS_H
#define MC_ATTRIBUTE_HELPERS_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/attribute.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief returns the Attribute::DataType of a template type ie. getDataType<float> would return
/// Attribute::DATATYPE_FLOAT where as getDataType<NMP::Matrix34> would return Attribute::DATATYPE_MATRIX.
/// If T is an unsupported type the function will return Attribute::DATATYPE_INVALID.
template<typename T>
inline Attribute::DataType getDataType();

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool getAttributeData(const MCOMMS::Attribute* attribute, T* value);

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool getAttributeArrayData(const MCOMMS::Attribute* attribute, T** values, size_t* count);

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool setAttributeData(MCOMMS::Attribute* attribute, const T& value);

} // MCOMMS

#include "comms/attributeHelpers.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // MC_ATTRIBUTE_HELPERS_H
//----------------------------------------------------------------------------------------------------------------------
