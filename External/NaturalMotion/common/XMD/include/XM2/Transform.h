//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector3.h"
#include "XM2/Quaternion.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
class XM2EXPORT XTransform;
class XM2EXPORT XMatrix;
class XM2EXPORT XVector3;
class XM2EXPORT XVector4;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XTransform
/// \brief   SRT transform type
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XTransform
{
public:

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XTransform();

  /// \brief  ctor
  XM2_INLINE XTransform(const XTransform& rhs);

  /// \brief  ctor
  XM2_INLINE XTransform(const XMatrix& rhs);

  /// \name  initialisation

  /// \brief  sets the transform to the identity
  XM2_INLINE void identity();

  /// \brief  sets all elements in the transform to zero
  XM2_INLINE void zero();

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second transform to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XTransform& b, XReal eps = XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second transform to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XTransform& b, XReal eps = XM2_FLOAT_ZERO) const;

  /// \name  transform type conversion

  /// \brief  converts the XTransform data into a matrix
  /// \param  m - the output matrix
  XM2_INLINE void toMatrix(XMatrix& m) const;

  /// \brief  converts from a matrix into separate translate, rotate and scale component
  /// \param  m - the matrix to convert from
  XM2_INLINE void fromMatrix(const XMatrix& m);

  /// \name  slerp / lerp

  /// \brief  multiplies the incoming transform by the weight, and adds the result to this
  /// \param  a - the transform to multiply with the weight
  /// \param  weight - the weight to multiply the transform by
  XM2_INLINE void sum(const XTransform& a, XReal weight);

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - the first xform
  /// \param  b - the second xform
  /// \param  t - the interpolation amount, 0 to 1 
  XM2_INLINE void lerp(const XTransform& a, const XTransform& b, XReal t);

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - the first xform
  /// \param  b - the second xform
  /// \param  t - the interpolation amount, 0 to 1 
  XM2_INLINE void slerp(const XTransform& a, const XTransform& b, XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XTransform FileIO
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XTransform the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XTransform to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XTransform from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XTransform from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XTransform to stdout
  void print32() const;

  /// \brief  prints this XTransform to stdout
  void print64() const;

  /// \brief  writes this XTransform to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes=false) const;

  /// \brief  writes this XTransform to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes=false) const;

  /// \brief  reads a XTransform from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes=false);

  /// \brief  reads a XTransform from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes=false);  

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XTransform Operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE bool operator == (const XTransform& p) const;
  XM2_INLINE bool operator != (const XTransform& p) const;

  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  /// \name  static array methods

  /// \brief  sets an array of quaternions to zero
  /// \param  q - output quaternion array
  /// \param  num - the number of quaternions in the array
  static bool arrayEqual(const XTransform* a, const XTransform* b, int num, XReal eps = XM2_FLOAT_ZERO);

  /// \brief  sets an array of quaternions to zero
  /// \param  q - output quaternion array
  /// \param  num - the number of quaternions in the array
  static bool arrayNotEqual(const XTransform* a, const XTransform* b, int num, XReal eps = XM2_FLOAT_ZERO);

  /// \brief  Converts an array of matrices to an array of quaternions
  /// \param  output - the returned quaternions
  /// \param  input - the input matrices
  /// \param  num - the number of quaternions
  static void arrayFromMatrixArray(XTransform* output, const XMatrix* input, int num);

  /// \brief  converts an array of quaternions into an array of matrices
  /// \param  output - the returned matrices
  /// \param  input - the input quaternions
  /// \param  num - the number of quaternions
  static void arrayToMatrixArray(XMatrix* output, const XTransform* input, int num);

  /// \brief  Converts an array of transforms to an array of quaternions
  /// \param  output - the returned quaternions
  /// \param  input - the input quaternions
  /// \param  num - the number of quaternions
  static void arrayFromQuaternionArray(XTransform* output, const XQuaternion* input, int num);

  /// \brief  converts an array of quaternions into an array of transforms
  /// \param  output - the returned transforms
  /// \param  input - the input quaternions
  /// \param  num - the number of quaternions
  static void arrayToQuaternionArray(XQuaternion* output, const XTransform* input, int num);

  /// \brief  converts the translate/rotate and scale into a local space matrix
  /// \param  output - output matrix
  /// \param  tr - transform to evaluate
  /// \param  num - number of elements in the arrays to process
  static void arrayEvaluate(const XTransform* tr, XMatrix* output, int num);

  /// \brief  linearly interpolates between two transforms a and b with respect to t
  /// \param  output - the output xform
  /// \param  a - transform 1
  /// \param  b - transform 2
  /// \param  t - interpolation amount
  /// \param  num - number of elements in the arrays to process
  static void arrayLerp(XTransform* output, const XTransform* a, const XTransform* b, XReal t, int num);

  /// \brief  spherically interpolates between two transforms a and b with respect to t
  /// \param  output - the output xform
  /// \param  a - transform 1
  /// \param  b - transform 2
  /// \param  t - interpolation amount
  /// \param  num - number of elements in the arrays to process
  static void arraySlerp(XTransform* output, const XTransform* a, const XTransform* b, XReal t, int num);

  /// \brief  spherically interpolates between two transforms a and b with respect to t
  /// \param  output - the output xform array
  /// \param  input - the input transform array
  /// \param  weight - interpolation amount
  /// \param  num - number of elements in the arrays to process
  static void arraySum(XTransform* output, const XTransform* input, XReal weight, int num);

  /// \brief  zeros the transform array data
  /// \param  output - the array of transforms to zero
  /// \param  num - number of elements in the arrays to process
  static void arrayZero(XTransform* output, int num);
  
  /// \brief  zeros the transform array data
  /// \param  output - the array to normalise
  /// \param  num - number of elements in the arrays to process
  static void arrayNormalise(XTransform* output, int num);
  
  /// \brief  zeros the transform array data
  /// \param  output - the array to set to the identity
  /// \param  num - number of elements in the arrays to process
  static void arrayIdentity(XTransform* output, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // Member variables
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  data

  /// rotation 
  XQuaternion rotate;

  /// translation 
  XVector3 translate;

  /// scale 
  XVector3 scale;
}
XM2_ALIGN_SUFFIX(16);

#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XTransform& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XTransform& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/inl/Transform.inl"
#ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
    #include "XM2/inl/sse/Transform.inl"
  #else
    #include "XM2/inl/sse2/Transform.inl"
  #endif
#else
  #include "XM2/inl/vanilla/Transform.inl"
#endif
//----------------------------------------------------------------------------------------------------------------------
