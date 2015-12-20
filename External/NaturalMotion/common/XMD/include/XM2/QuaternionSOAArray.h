//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/QuaternionSOA.h"
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

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XQuaternionSOAArray
/// \brief   defines an array of quaternions stored in structure of array format
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XQuaternionSOAArray 
  : public pod_vector< XQuaternionSOA > 
{
public:

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XQuaternionSOAArray();

  /// \brief  copy ctor
  XM2_INLINE XQuaternionSOAArray(const XMatrixArray& rhs);

  /// \brief  copy ctor
  XM2_INLINE XQuaternionSOAArray(const XQuaternionArray& rhs);

  /// \brief  copy ctor
  XM2_INLINE XQuaternionSOAArray(const XQuaternionSOAArray& rhs);

  /// \name  streaming / swizzling

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XMatrixArray& input);

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XQuaternionArray& input);

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XMatrixArray& input) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XQuaternionArray& input) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOAArray operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  /// \brief  multiples the quaternions by the matrix
  /// \param  rhs - the matrix to multiply the array with
  /// \return *this
  XM2_INLINE const XQuaternionSOAArray& operator*=(const XMatrix& rhs);

  /// \brief  multiples the quaternions by the transform
  /// \param  rhs - the transform to multiply the array with
  /// \return *this 
  XM2_INLINE const XQuaternionSOAArray& operator*=(const XTransform& rhs);

  /// \brief  multiples the quaternions by the specified quaternion
  /// \param  rhs - the transform to multiply the array with
  /// \return *this
  XM2_INLINE const XQuaternionSOAArray& operator*=(const XQuaternion& rhs);

  /// \brief  adds the quaternion to every quaternion in this array
  /// \param  rhs - the quaternion to add to this array 
  /// \return *this
  XM2_INLINE const XQuaternionSOAArray& operator+=(const XQuaternion& rhs);

  /// \brief  multiples the quaternion array by a scalar
  /// \param  rhs - the scalar
  /// \return *this
  XM2_INLINE const XQuaternionSOAArray& operator*=(const XReal rhs);

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOAArray interpolation methods
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates q0 and q1 with respect to t, and stored in this
  /// \param  q0 - first arg
  /// \param  q1 - second arg
  /// \param  t - the interpolation amount
  void lerp(const XQuaternionSOAArray& q0, const XQuaternionSOAArray& q1, XReal t);

  /// \brief  spherically interpolates q0 and q1 with respect to t
  /// \param  q0 - first arg
  /// \param  q1 - second arg
  /// \param  t - the interpolation amount
  void slerp(const XQuaternionSOAArray& q0, const XQuaternionSOAArray& q1, XReal t);

  /// \brief  performs this += (q*w)
  /// \param  q - the quaternion array to sum into this
  /// \param  w - the weight
  void sum(const XQuaternionSOAArray& q, XReal w);

  /// \name  math operations

  /// \brief  zeros all quaternions in this array
  void zero();

  /// \brief  normalises all quaternions in this array
  void normalise();
  
  /// \brief  sets all quaternions to the identity quaternion
  void identity();

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOAArray / XQuaternionSOAArray arithmetic
  //--------------------------------------------------------------------------------------------------------------------
  
  /// \brief  multiplies two arrays of quaternions together. 
  /// \param  a - first quaternion array
  /// \param  b - second quaternion array
  XM2_INLINE void mul(const XQuaternionSOAArray& a, const XQuaternionSOAArray& b);

  /// \brief  multiplies two arrays of quaternions together. 
  /// \param  a - first quaternion array
  XM2_INLINE void mul(const XQuaternionSOAArray& a);

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOAArray / XReal arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  multiplies the quaternion array by the scalar b
  /// \param  a - quaternion data
  /// \param  b - the scalar value
  XM2_INLINE void mul(const XQuaternionSOAArray& a, XReal b);

  /// \brief  multiplies the quaternion array by the scalar b
  /// \param  b - the scalar value
  XM2_INLINE void mul(XReal b);

  /// \brief  inverts all of the quaternions in the array
  XM2_INLINE void invert();

  /// \brief  inverts an array of quaternions 
  /// \param  q - the array of quaternions to invert
  XM2_INLINE void invert(const XQuaternionSOAArray& q);

  /// \brief  negates a quaternion 
  XM2_INLINE void negate();

  /// \brief  negates an array of quaternions 
  /// \param  q - the array of quaternions to negate
  XM2_INLINE void negate(const XQuaternionSOAArray& q);
  
  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOAArray stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XQuaternionSOAArray the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XQuaternionSOAArray to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XQuaternionSOAArray from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XQuaternionSOAArray from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XQuaternionSOAArray to stdout
  void print32() const;

  /// \brief  prints this XQuaternionSOAArray to stdout
  void print64() const;

  /// \brief  writes this XQuaternionSOAArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XQuaternionSOAArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XQuaternionSOAArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XQuaternionSOAArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif
};

// streaming ops
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XQuaternionSOAArray& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XQuaternionSOAArray& c);
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/inl/QuaternionArray.inl"
//----------------------------------------------------------------------------------------------------------------------
