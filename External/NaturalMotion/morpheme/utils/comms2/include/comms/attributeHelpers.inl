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
#ifndef MC_ATTRIBUTE_HELPERS_INL
#define MC_ATTRIBUTE_HELPERS_INL

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// getDataType
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline Attribute::DataType getDataType()
{
  return Attribute::DATATYPE_INVALID;
}

//----------------------------------------------------------------------------------------------------------------------
/// getDataType template specialisations
//----------------------------------------------------------------------------------------------------------------------
template<>
inline Attribute::DataType getDataType<float>()
{
  return Attribute::DATATYPE_FLOAT;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
inline Attribute::DataType getDataType<int32_t>()
{
  return Attribute::DATATYPE_INT;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
inline Attribute::DataType getDataType<Attribute::PhysicsShapeType>()
{
  return Attribute::DATATYPE_INT;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
inline Attribute::DataType getDataType<MCOMMS::Attribute::ControllerShapeType>()
{
  return Attribute::DATATYPE_INT;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
inline Attribute::DataType getDataType<uint32_t>()
{
  return Attribute::DATATYPE_UINT;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
inline Attribute::DataType getDataType<char>()
{
  return Attribute::DATATYPE_CHAR;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
inline Attribute::DataType getDataType<NMP::Matrix34>()
{
  return Attribute::DATATYPE_MATRIX;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
inline Attribute::DataType getDataType<NMP::Vector3>()
{
  return Attribute::DATATYPE_VECTOR;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
inline Attribute::DataType getDataType<NMP::Colour>()
{
  return Attribute::DATATYPE_COLOUR;
}

//----------------------------------------------------------------------------------------------------------------------
/// getAttributeData
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool getAttributeData(const MCOMMS::Attribute* attribute, T* value)
{
  if (attribute->getDataType() != getDataType<T>())
  {
    return false;
  }

  if (attribute->getDataCount() != 1)
  {
    return false;
  }

  NMP_ASSERT(attribute->getDataSize() == sizeof(T));
  *value = *reinterpret_cast<const T*>(attribute->getData());

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// getAttributeArrayData
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool getAttributeArrayData(const MCOMMS::Attribute* attribute, const T** values, size_t* count)
{
  if (attribute->getDataType() != getDataType<T>())
  {
    *values = 0;
    *count = 0;
    return false;
  }

  size_t dataCount = attribute->getDataCount();

  NMP_ASSERT(attribute->getDataSize() == sizeof(T) * dataCount);
  *values = reinterpret_cast<const T*>(attribute->getData());
  *count = dataCount;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// setAttributeData
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool setAttributeData(MCOMMS::Attribute* attribute, const T& value)
{
  NMP_ASSERT(attribute != 0);

  if (attribute->getDataType() != getDataType<T>())
  {
    return false;
  }

  if (attribute->getDataCount() != 1)
  {
    return false;
  }

  NMP_ASSERT(attribute->getDataSize() == sizeof(T));
  T* data = reinterpret_cast<T*>(attribute->getData());
  *data = value;

  return true;
}

} // MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MC_ATTRIBUTE_HELPERS_INL
//----------------------------------------------------------------------------------------------------------------------
