//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "Matrix34.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------------------------------------------------

class XM2EXPORT XMatrix;
class XM2EXPORT XTransformArray;
class XM2EXPORT XQuaternionArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XMD::XMatrixArray
/// \brief   defines an aligned matrix array (std::vector) type
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XMatrixArray 
  : public pod_vector< XMatrix > 
{
public:

  /// \name  construction

  XM2_INLINE XMatrixArray();
  XM2_INLINE XMatrixArray(const XQuaternionArray& qa);
  XM2_INLINE XMatrixArray(const XTransformArray& qa);
  XM2_INLINE XMatrixArray(const XMatrixArray& qa);

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrixArray operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE const XMatrixArray& operator = (const XQuaternionArray& rhs);
  XM2_INLINE const XMatrixArray& operator = (const XTransformArray& rhs);

  XM2_INLINE const XMatrixArray& operator *= (const XMatrixArray& rhs);
  XM2_INLINE const XMatrixArray& operator *= (const XMatrix& rhs);
  XM2_INLINE const XMatrixArray& operator *= (XReal rhs);
  XM2_INLINE const XMatrixArray& operator /= (XReal rhs);

  /// \name  initialisation

  /// sets all matrices to the identity
  XM2_INLINE void identity();

  /// sets all matrices to the zero matrix
  XM2_INLINE void zero();

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrixArray equality functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - matrix array to test against
  /// \param  eps - tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XMatrixArray& b, XReal eps = XM2_FLOAT_ZERO) const;

  /// \brief  this != b
  /// \param  b - second matrix array to test
  /// \param  eps - tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XMatrixArray& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // Functions to extract transformations from an XMatrixArray 
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  transform type conversion

  /// \brief  Converts this XMatrixArray to a XQuaternionArray
  /// \param  output - the returned XQuaternionArray 
  XM2_INLINE void toQuaternionArray(XQuaternionArray& output) const;

  /// \brief  converts the input XQuaternionArray into a XMatrixArray
  /// \param  q - the XQuaternionArray to convert
  XM2_INLINE void fromQuaternionArray(const XQuaternionArray& q);

  /// \brief  Converts this XMatrixArray to a XTransformArray
  /// \param  output - the returned XTransformArray 
  XM2_INLINE void toTransformArray(XTransformArray& output) const;

  /// \brief  converts the input XTransformArray into a XMatrixArray
  /// \param  q - the XTransformArray to convert
  XM2_INLINE void fromTransformArray(const XTransformArray& q);

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrixArray inversion
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  matrix inversion

  /// \brief  inverts the matrix array passed to this function
  /// \param  a - the matrix array to invert
  XM2_INLINE void invert(const XMatrixArray& a);

  /// \brief  inverts this matrix array 
  XM2_INLINE void invert();

  /// \brief  transposes the matrix array passed to this function
  /// \param  a - the matrix array to transpose
  XM2_INLINE void transpose(const XMatrixArray& a);

  /// \brief  transpose this matrix array
  XM2_INLINE void transpose();

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrixArray Multiplication
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  matrix multiplication

  /// \brief  multiplies 2 matrix arrays together 
  /// \param  a - first matrix array
  /// \param  b - second matrix array
  XM2_INLINE void mul(const XMatrixArray& a, const XMatrixArray& b);

  /// \brief  multiplies 2 matrix arrays together. Slightly more efficient than mul, but
  ///         only works for 4x3 matrices.
  /// \param  a - first matrix
  /// \param  b - second matrix
  XM2_INLINE void quickMult(const XMatrixArray& a, const XMatrixArray& b);

  /// \brief  multiplies a matrix arrays with a matrix.
  /// \param  a - first matrix array
  /// \param  b - second matrix
  XM2_INLINE void mul(const XMatrixArray& a, const XMatrix& b);

  /// \brief  multiplies a matrix arrays with a matrix. Slightly more efficient than mul, but
  ///         only works for 4x3 matrices.
  /// \param  a - first matrix array
  /// \param  b - second matrix
  XM2_INLINE void quickMult(const XMatrixArray& a, const XMatrix& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrixArray stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XMatrixArray the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XMatrixArray to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XMatrixArray from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XMatrixArray from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XMatrixArray to stdout
  void print32() const;

  /// \brief  prints this XMatrixArray to stdout
  void print64() const;

  /// \brief  writes this XMatrixArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XMatrixArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XMatrixArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XMatrixArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif
};

// streaming ops
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XMatrixArray& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XMatrixArray& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/QuaternionArray.h"
#include "XM2/TransformArray.h"
#include "XM2/inl/Matrix34Array.inl"
//----------------------------------------------------------------------------------------------------------------------
