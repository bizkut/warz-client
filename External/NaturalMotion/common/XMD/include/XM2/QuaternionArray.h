//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Quaternion.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------------------------------------------------

class XM2EXPORT XMatrixArray;
class XM2EXPORT XTransformArray;
class XM2EXPORT XMatrix;
class XM2EXPORT XTransform;
class XM2EXPORT XRealArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XQuaternionArray 
/// \brief   defines an aligned quaternion array (std::vector) type
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XQuaternionArray 
  : public pod_vector< XQuaternion > 
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionArray ctors
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  XM2_INLINE XQuaternionArray();
  XM2_INLINE XQuaternionArray(const XQuaternionArray& rhs);
  XM2_INLINE XQuaternionArray(const XMatrixArray& rhs);
  XM2_INLINE XQuaternionArray(const XTransformArray& rhs);

  XM2_INLINE const XQuaternionArray& operator=(const XMatrixArray& rhs);
  XM2_INLINE const XQuaternionArray& operator=(const XTransformArray& rhs);

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionArray operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE const XQuaternionArray& operator*=(const XMatrixArray& rhs);
  XM2_INLINE const XQuaternionArray& operator*=(const XMatrix& rhs);
  XM2_INLINE const XQuaternionArray& operator*=(const XTransformArray& rhs);
  XM2_INLINE const XQuaternionArray& operator*=(const XTransform& rhs);
  XM2_INLINE const XQuaternionArray& operator*=(const XQuaternionArray& rhs);
  XM2_INLINE const XQuaternionArray& operator*=(const XQuaternion& rhs);

  XM2_INLINE const XQuaternionArray& operator+=(const XQuaternion& rhs);
  XM2_INLINE const XQuaternionArray& operator+=(const XQuaternionArray& rhs);

  XM2_INLINE const XQuaternionArray& operator*=(XReal rhs);
  XM2_INLINE const XQuaternionArray& operator*=(const XRealArray& rhs);

  XM2_INLINE bool operator == (const XQuaternionArray& c) const;
  XM2_INLINE bool operator != (const XQuaternionArray& c) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternion Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second vector to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XQuaternionArray& b, XReal eps=XM2_FLOAT_ZERO) const;

  /// \brief  this != b
  /// \param  b - second vector to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XQuaternionArray& b, XReal eps=XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionArray type conversion
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  transform type conversion

  /// \brief  creates quaternion from a rotation matrix
  /// \param  mat - the rotation matrix
  XM2_INLINE void fromMatrixArray(const XMatrixArray& mat);

  /// \brief  converts this quaternion to a matrix
  /// \param  mat - the output matrix
  XM2_INLINE void toMatrixArray(XMatrixArray& mat) const;

  /// \brief  creates quaternion from a rotation matrix
  /// \param  mat - the rotation matrix
  XM2_INLINE void fromTransformArray(const XTransformArray& mat);

  /// \brief  converts this quaternion to a matrix
  /// \param  mat - the output matrix
  XM2_INLINE void toTransformArray(XTransformArray& mat) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionArray Lerp/Slerp methods
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  slerp/lerp

  /// \brief  linearly interpolates q0 and q1 with respect to t, and stored in this
  /// \param  q0 - first arg
  /// \param  q1 - second arg
  /// \param  t - the interpolation amount
  XM2_INLINE void lerp(const XQuaternionArray& q0, const XQuaternionArray& q1, XReal t);

  /// \brief  spherically interpolates q0 and q1 with respect to t
  /// \param  q0 - first arg
  /// \param  q1 - second arg
  /// \param  t - the interpolation amount
  XM2_INLINE void slerp(const XQuaternionArray& q0, const XQuaternionArray& q1, XReal t);

  /// \brief  performs this += (q*w)
  /// \param  q - the quaternion array to sum into this
  /// \param  w - the weight
  XM2_INLINE void sum(const XQuaternionArray& q, XReal w);

  /// \brief  zeros all quaternions in this array
  XM2_INLINE void zero();

  /// \name  math operations

  /// \brief  normalises all quaternions in this array
  XM2_INLINE void normalise();
  
  /// \brief  sets all quaternions to the identity quaternion
  XM2_INLINE void identity();

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternion Dot product
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  performs the dot product between 2 quaternions
  /// \param  b - the quaternion to dot against
  /// \return the dot product
  XM2_INLINE void dot(XRealArray& product, const XQuaternionArray& b) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionArray stdio
  //--------------------------------------------------------------------------------------------------------------------
  
  /// \brief  multiplies two quaternion arrays together. 
  /// \param  a - first quaternion array
  /// \param  b - second quaternion array
  XM2_INLINE void mul(const XQuaternionArray& a, const XQuaternionArray& b);

  /// \brief  multiplies this array with another array
  /// \param  a - quaternion array to multiply this by
  XM2_INLINE void mul(const XQuaternionArray& a);

  /// \brief  multiplies two quaternion arrays together. 
  /// \param  a - first quaternion array
  /// \param  b - second quaternion array
  XM2_INLINE void mul(const XQuaternionArray& a, const XQuaternion& b);

  /// \brief  multiplies this array with another array
  /// \param  a - quaternion array to multiply this by
  XM2_INLINE void mul(const XQuaternion& a);

  /// \brief  multiplies a quaternion array with a scalar. 
  /// \param  a - input quaternion array
  /// \param  b - scalar
  XM2_INLINE void mul(const XQuaternionArray& a, const XRealArray& b);

  /// \brief  multiplies a quaternion array with a scalar. 
  /// \param  a - input quaternion array
  /// \param  b - scalar
  XM2_INLINE void mul(const XQuaternionArray& a, const XReal& b);

  /// \brief  multiplies this quaternion array by a scalar
  /// \param  b - the scalar
  XM2_INLINE void mul(const XRealArray& b);

  /// \brief  multiplies this quaternion array by a scalar
  /// \param  b - the scalar
  XM2_INLINE void mul(XReal b);

  /// \brief  inverts all quaternions in the array
  XM2_INLINE void invert();

  /// \brief  inverts the quaternions in q, and stores the result in this.
  /// \param  q - the array of quaternions to invert
  XM2_INLINE void invert(const XQuaternionArray& q);

  /// \brief  negates all quaternions in the array
  XM2_INLINE void negate();

  /// \brief  negates the quaternions in q, and stores the result in this.
  /// \param  q - the array of quaternions to negate
  XM2_INLINE void negate(const XQuaternionArray& q);
  
  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionArray stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XQuaternionArray the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XQuaternionArray to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XQuaternionArray from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XQuaternionArray from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XQuaternionArray to stdout
  void print32() const;

  /// \brief  prints this XQuaternionArray to stdout
  void print64() const;

  /// \brief  writes this XQuaternionArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XQuaternionArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XQuaternionArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XQuaternionArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif
};
// streaming ops
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XQuaternionArray& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XQuaternionArray& c);
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/RealArray.h"
#include "XM2/Matrix34Array.h"
#include "XM2/TransformArray.h"
#include "XM2/inl/QuaternionArray.inl"
//----------------------------------------------------------------------------------------------------------------------
