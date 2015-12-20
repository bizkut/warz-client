//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Transform.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
class XM2EXPORT XQuaternionArray;
class XM2EXPORT XMatrixArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XTransformArray
/// \brief   defines an array of SRT transforms
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XTransformArray 
  : public pod_vector< XTransform > 
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XTransformArray ctors
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XTransformArray();

  /// \brief  copy ctor
  /// \param  ta - the transform array to copy
  XM2_INLINE XTransformArray(const XTransformArray& ta);

  /// \brief  copy ctor
  /// \param  ta - the quaternion array to copy
  XM2_INLINE XTransformArray(const XQuaternionArray& ta);

  /// \brief  copy ctor
  /// \param  ta - the matrix array to copy 
  XM2_INLINE XTransformArray(const XMatrixArray& ta);
  
  //--------------------------------------------------------------------------------------------------------------------
  // XTransformArray operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second transform to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XTransformArray& b, XReal eps = XM2_FLOAT_ZERO) const;

  /// \brief  this != b
  /// \param  b - second transform to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XTransformArray& b, XReal eps = XM2_FLOAT_ZERO) const;

  /// \name  transform type conversion

  /// \brief  converts the XTransform data into a matrix
  /// \param  m - the output matrix
  XM2_INLINE void toMatrixArray(XMatrixArray& m) const;

  /// \brief  converts from a matrix into separate translate, rotate and scale component
  /// \param  m - the matrix to convert from
  XM2_INLINE void fromMatrixArray(const XMatrixArray& m);

  /// \brief  converts the XTransform data into a matrix
  /// \param  m - the output matrix
  XM2_INLINE void toQuaternionArray(XQuaternionArray& m) const;

  /// \brief  converts from a matrix into separate translate, rotate and scale component
  /// \param  m - the matrix to convert from
  XM2_INLINE void fromQuaternionArray(const XQuaternionArray& m);

  /// \name  lerp / slerp

  /// \brief  interpolates between a and b and stores the result in this (and will perform
  ///         a normalise on the quat values)
  /// \param  a - the first array to blend between
  /// \param  b - the second array to blend between
  /// \param  t - the interpolation amount
  XM2_INLINE void lerp(const XTransformArray& a, const XTransformArray& b, XReal t);

  /// \brief  spherically interpolates between a and b and stores the result in this
  /// \param  a - the first array to blend between
  /// \param  b - the second array to blend between
  /// \param  t - the interpolation amount
  XM2_INLINE void slerp(const XTransformArray& a, const XTransformArray& b, XReal t);

  /// \brief  multiplies a by t, and sums into this
  /// \param  a - the array to sun into this
  /// \param  t - a multiplication factor
  /// \note
  ///         this is useful for blending together N sets of transforms. Having summed the 
  ///         effect of a number of transforms, simply call normalise() and you'll have 
  ///         performed a normalised lerp of all the transform arrays. Useful for blending
  ///         multiple animations together for example...
  /// \code
  ///     this += a*t
  /// \endcode
  XM2_INLINE void sum(const XTransformArray& a, XReal t);

  /// \name  initialisation

  /// \brief  normalises the rotations on all transforms in the array
  XM2_INLINE void normalise();
  
  /// \brief  sets all transforms in the array to the identity
  XM2_INLINE void identity();

  /// \brief  zeros out all transforms in the array
  XM2_INLINE void zero();

  //--------------------------------------------------------------------------------------------------------------------
  // XTransformArray stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XTransformArray the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XTransformArray to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XTransformArray from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XTransformArray from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XTransformArray to stdout
  void print32() const;

  /// \brief  prints this XTransformArray to stdout
  void print64() const;

  /// \brief  writes this XTransformArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XTransformArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XTransformArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XTransformArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif
};
// streaming ops
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XTransformArray& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XTransformArray& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Matrix34Array.h"
#include "XM2/QuaternionArray.h"
#include "XM2/inl/TransformArray.inl"
//----------------------------------------------------------------------------------------------------------------------
