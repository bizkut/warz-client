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
#ifndef NM_VECTOR_H
#define NM_VECTOR_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"

#include <string.h>
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

template <typename T>
class Matrix;

/// \ingroup NaturalMotionPlatformMath
/// \brief A generic Vector class that can have arbitrary dimension.  This is NOT
/// intended to be used for fast vector manipulation.  Vectors are never resized
/// and must always be initialized with the correct dimensions.
template <typename T>
class Vector
{
public:
  /// \brief Constructor / Destructor functions
  ~Vector();
  Vector(); // Empty vector
  Vector(uint32_t elements);
  Vector(uint32_t elements, T value);
  Vector(uint32_t elements, const T* data);
  Vector(const Vector& copy);

  /// \brief Output vector in a file (for debug purpose mainly)
  void fwritef(const char* filename) const;

  /// \brief Constructors and functions for all fixed-size types
  Vector(const Vector3& v);
  Vector3 getVector3() const;

  /// \brief Allows a Vector of one type to be constructed from another
  template <typename Q>
  Vector(const Vector<Q>& other);

  /// \brief Vector size and element access functions
  T& element(uint32_t index);
  const T& element(uint32_t index) const;
  T& end(uint32_t index); // v[(n-1) - index]
  const T& end(uint32_t index) const;
  T& operator [] (uint32_t index);
  const T& operator [] (uint32_t index) const;

  T* data();
  const T* data() const;

  uint32_t numElements() const;
  bool isSize(uint32_t n) const;
  bool isEmpty() const;

  /// \brief Basic operations (allocates memory)
  Vector operator + (const Vector& v) const;
  Vector operator - (const Vector& v) const;
  Vector operator * (T s) const;
  Vector operator / (T s) const;

  /// Negate all vector elements
  Vector operator - (void) const;

  /// \brief Basic operations (no memory allocated)
  const Vector& operator = (const Vector& copy);
  bool operator == (const Vector& v) const;
  bool operator != (const Vector& v) const;

  void operator += (const Vector& v);
  void operator -= (const Vector& v);
  void operator += (T s);
  void operator -= (T s);
  void operator *= (T s);
  void operator /= (T s);

  /// \brief Whole vector data functions

  /// \brief Swaps contents with vector v.
  void swap(Vector& v);

  void get(T* data) const;
  void set(const T* data);
  void set(T val);

  template <typename Q>
  void get(Q* data) const;

  template <typename Q>
  void set(const Q* data);

  Vector<T>& setFromMultiplication(const Matrix<T>& m, const Vector& v);
  Vector<T>& setFromMultiplication(T coeff, const Vector& v);
  Vector<T>& setFromMultiplication(T coeff);
  Vector<T>& setFromAdd(const Vector& v1, const Vector& v2);
  Vector<T>& setFromDiff(const Vector& v1, const Vector& v2);
  void reverse();
  void normalise();
  void cumulativeSum();
  void linear(T start, T end);

  Vector diff() const;
  void diff(Vector& v) const;

  void sort();

  /// \brief Sub-vector functions
  void getSubVector(uint32_t indx, Vector& v) const;
  void getSubVector(uint32_t indx, uint32_t n, T* data) const;

  void setSubVector(uint32_t indx, const Vector& v);
  void setSubVector(uint32_t indx, uint32_t n, const T* data);
  void setSubVector(uint32_t indx, uint32_t n, T s);
  void setSubVector(uint32_t indx, T s); // [indx:end] = s

  /// \brief Per element vector functions

  /// \brief Absolute values.
  void absolute();

  /// \brief Zero small absolute values below threshold.
  void zeroSmall(T v);

  /// \brief Element-wise multiply.
  void multiply(const Vector& v);

  /// \brief Square elements.
  void square();

  /// \brief Square root elements.
  void squareroot();

  /// \brief Vector property functions
  T maximum(uint32_t& indx) const;
  T minimum(uint32_t& indx) const;
  T maximum() const;
  T minimum() const;

  T sum() const;
  T mean() const;
  T median() const;
  T var() const;
  T sd() const;

  T magnitudeSquared() const;
  T magnitude() const;
  T dot(const Vector& other) const;

  // Index and position functions

  /// \brief Binary search to find the interval: v_{ i } <= x < v_{ i + 1 } that the value x lies within.
  ///
  /// Elements within the vector are assumed to be monotonically increasing.
  ///
  /// \param  x The value for which we wish to find the interval.
  /// \param  i Resultant interval index.
  /// \param  incEnd Include the end element in the search, i.e.: v_{ end - 1 } <= x <= v_{ end }
  /// \return -1 if x lies before the first element, 0 if within an interval, or 1 if x lies beyond the last element.
  int32_t interval(T x, uint32_t& i, bool incEnd = true) const; 

  /// \brief Comparison function with tolerance.
  bool isZero(T threshold) const;
  bool isEqual(const Vector& v, T threshold) const;

protected:

  uint32_t  m_numElements;
  T*       m_elements;

};

template <typename T>
Vector<T> operator * (T s, const Vector<T>& v);

template <typename T> T
Dot(const Vector<T>& v1, const Vector<T>& v2);

} // namespace NMP

#include "NMPlatform/NMVector.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_VECTOR_H
//----------------------------------------------------------------------------------------------------------------------
