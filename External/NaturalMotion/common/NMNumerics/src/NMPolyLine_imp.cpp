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
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// PolyLine
//----------------------------------------------------------------------------------------------------------------------
template <>
PolyLine<ScalarType>::~PolyLine()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PolyLine<ScalarType>::PolyLine()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PolyLine<ScalarType>::PolyLine(uint32_t dims) :
  BSplineCurve(2, dims)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PolyLine<ScalarType>::PolyLine(uint32_t dims, uint32_t number) :
  BSplineCurve(2, dims, number)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PolyLine<ScalarType>::PolyLine(const Matrix<ScalarType>& X) :
  BSplineCurve(2, X.numRows(), X.numColumns())
{
  knotsOpenArcLength(X);
  m_coefs = X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PolyLine<ScalarType>::PolyLine(const PolyLine& curve) :
  BSplineCurve(curve)
{
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
