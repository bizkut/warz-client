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
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
template <typename ScalarType>
std::ostream& operator<<(std::ostream& os, const Matrix<ScalarType>& mat)
{
  os << "dim : " << mat.numRows() << " " << mat.numColumns() << " " << std::endl;
  for (uint32_t i = 0; i < mat.numRows(); i++)
  {
    for (uint32_t j = 0; j < mat.numColumns(); j++)
    {
      os.width(10);
      os << mat.element(i, j) << "\t";
    }
    os << std::endl;
  }
  return os;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename ScalarType>
std::ostream& operator<<(std::ostream& os, const Vector<ScalarType>& vec)
{
  os << "dim : " << vec.numElements() << " " << std::endl;
  for (uint32_t i = 0; i < vec.numElements(); i++)
  {
    os.width(10);
    os << vec.element(i) << "\t";
  }

  os << std::endl;

  return os;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
