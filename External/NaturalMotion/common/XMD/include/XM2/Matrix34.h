//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/MathCommon.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------------------------------------------------

class XM2EXPORT XVector3;
class XM2EXPORT XVector3SOA;
class XM2EXPORT XVector3Packed;
class XM2EXPORT XVector3SOA;
class XM2EXPORT XVector4;
class XM2EXPORT XVector4SOA;
class XM2EXPORT XVector3Array;
class XM2EXPORT XVector3SOAArray;
class XM2EXPORT XVector3PackedArray;
class XM2EXPORT XVector3SOAArray;
class XM2EXPORT XVector4Array;
class XM2EXPORT XVector4SOAArray;
class XM2EXPORT XQuaternion;
class XM2EXPORT XTransform;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XMD::XMatrix
/// \brief   a 4x3 transformation matrix
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XMatrix
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix ctor / dtor
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XMatrix();

  /// \brief  copy ctor
  XM2_INLINE XMatrix(const XMatrix& c);

  /// \brief  ctor - initialises from a quaternion
  /// \param  c - the quaternion to initialise from
  XM2_INLINE XMatrix(const XQuaternion& c);

  /// \brief  ctor - initialises from a transform
  /// \param  c - the transform to initialise from
  XM2_INLINE XMatrix(const XTransform& c);
  
  /// \brief  ctor - initialises from floats
  XM2_INLINE XMatrix(XReal _00, XReal _01, XReal _02, XReal _03,
                     XReal _10, XReal _11, XReal _12, XReal _13,
                     XReal _20, XReal _21, XReal _22, XReal _23,
                     XReal _30, XReal _31, XReal _32, XReal _33)
  {
    set(_00,_01,_02,_03,
        _10,_11,_12,_13,
        _20,_21,_22,_23,
        _30,_31,_32,_33);
  }

  /// \brief  ctor - initialises from 4 vectors
  /// \param  x_axis - the x axis of the matrix
  /// \param  y_axis - the y axis of the matrix
  /// \param  z_axis - the z axis of the matrix
  /// \param  position - the translation component
  XM2_INLINE XMatrix(const XVector3& x_axis,
                     const XVector3& y_axis,
                     const XVector3& z_axis,
                     const XVector4& position)
  {
    set(x_axis,
        y_axis,
        z_axis,
        position);
  }

  #ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
  XM2_INLINE XMatrix(const __m128& _x, const __m128& _y, const __m128& _z, const __m128& _w);
  #else
  XM2_INLINE XMatrix(const __m128d& _x0, const __m128d& _x1,
                     const __m128d& _y0, const __m128d& _y1,
                     const __m128d& _z0, const __m128d& _z1,
                     const __m128d& _w0, const __m128d& _w1);
  #endif
  #endif 

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix initialize functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  sets the elements of the matrix
  XM2_INLINE void set(XReal _00, XReal _01, XReal _02, XReal _03,
                      XReal _10, XReal _11, XReal _12, XReal _13,
                      XReal _20, XReal _21, XReal _22, XReal _23,
                      XReal _30, XReal _31, XReal _32, XReal _33);
      
  /// \brief  sets the elements of the matrix
  /// \param  x_axis - the x axis of the matrix
  /// \param  y_axis - the y axis of the matrix
  /// \param  z_axis - the z axis of the matrix
  XM2_INLINE void set(const XVector3& x_axis,
                      const XVector3& y_axis,
                      const XVector3& z_axis,
                      const XVector4& position);
  
  /// \brief  sets the elements of the matrix
  /// \param  x_axis - the x axis of the matrix
  /// \param  y_axis - the y axis of the matrix
  /// \param  z_axis - the z axis of the matrix
  /// \param  position - the translation component
  XM2_INLINE void set(const __m128& x_axis,
                      const __m128& y_axis,
                      const __m128& z_axis,
                      const __m128& position);

  /// \brief  sets all elements to 0
  XM2_INLINE void zero();
  
  /// \brief  sets this matrix to the identity
  XM2_INLINE void identity();

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix equality functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - matrix to test against
  /// \param  eps - tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XMatrix& b, XReal eps=XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second matrix to test
  /// \param  eps - tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XMatrix& b, XReal eps=XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix Transformations
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  transformations

  /// \brief  rotates the input matrix in the X axis by specified num of degrees
  /// \param  input - the initial matrix
  /// \param  degrees - the number of degrees to rotate
  XM2_INLINE void rotXD(const XMatrix& input, XReal degrees);

  /// \brief  rotates the input matrix in the X axis by specified num of radians
  /// \param  input - the initial matrix
  /// \param  radians - the number of radians to rotate
  XM2_INLINE void rotXR(const XMatrix& input, XReal radians);

  /// \brief  rotates the input matrix in the Y axis by specified num of degrees
  /// \param  input - the initial matrix
  /// \param  degrees - the number of degrees to rotate
  XM2_INLINE void rotYD(const XMatrix& input, XReal degrees);

  /// \brief  rotates the input matrix in the Y axis by specified num of radians
  /// \param  input - the initial matrix
  /// \param  radians - the number of radians to rotate
  XM2_INLINE void rotYR(const XMatrix& input, XReal radians);

  /// \brief  rotates the input matrix in the Z axis by specified num of degrees
  /// \param  input - the initial matrix
  /// \param  degrees - the number of degrees to rotate
  XM2_INLINE void rotZD(const XMatrix& input, XReal degrees);

  /// \brief  rotates the input matrix in the Z axis by specified num of radians
  /// \param  input - the initial matrix
  /// \param  radians - the number of radians to rotate
  XM2_INLINE void rotZR(const XMatrix& input, XReal radians);

  /// \brief  scales the matrix 
  /// \param  input - the initial matrix
  /// \param  scale - the scaling amount
  XM2_INLINE void scale(const XMatrix& input, const XVector3& scale);
  
  /// \brief  translates the matrix 
  /// \param  input - the initial matrix
  /// \param  translate - the translation amount
  XM2_INLINE void translate(const XMatrix& input, const XVector3& translate);

  /// \brief  rotates this matrix in the X axis by the specifies number of degrees
  /// \param  degrees - the number of degrees to rotate
  XM2_INLINE void rotXD(XReal degrees);

  /// \brief  rotates this matrix in the X axis by the specifies number of radians
  /// \param  radians - the number of radians to rotate
  XM2_INLINE void rotXR(XReal radians);

  /// \brief  rotates this matrix in the Y axis by the specifies number of degrees
  /// \param  degrees - the number of degrees to rotate
  XM2_INLINE void rotYD(XReal degrees);

  /// \brief  rotates this matrix in the Y axis by the specifies number of radians
  /// \param  radians - the number of radians to rotate
  XM2_INLINE void rotYR(XReal radians);

  /// \brief  rotates this matrix in the Z axis by the specifies number of degrees
  /// \param  degrees - the number of degrees to rotate 
  XM2_INLINE void rotZD(XReal degrees);

  /// \brief  rotates this matrix in the Z axis by the specifies number of radians
  /// \param  radians - the number of radians to rotate 
  XM2_INLINE void rotZR(XReal radians);

  /// \brief  scales the current matrix
  /// \param  _scale - the scaling to apply
  XM2_INLINE void scale(const XVector3& _scale);
  
  /// \brief  translates the current matrix
  /// \param  _translate - the amount of translation
  XM2_INLINE void translate(const XVector3& _translate);

  /// \brief  translates the current matrix
  /// \param  _translate - the amount of translation
  XM2_INLINE void rotate(const XQuaternion& _rotate);

  /// \brief  scales this matrix
  /// \param  x
  /// \param  y
  /// \param  z
  XM2_INLINE void scale(XReal x, XReal y, XReal z);
  
  /// \brief  translates this matrix
  /// \param  x
  /// \param  y
  /// \param  z
  XM2_INLINE void translate(XReal x, XReal y, XReal z);

  //--------------------------------------------------------------------------------------------------------------------
  // Functions to extract transformations from an XMatrix 
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  transform conversion

  /// \brief  Converts this matrix to a quaternion
  /// \param  output - the returned quaternion 
  XM2_INLINE void toQuaternion(XQuaternion& output) const;

  /// \brief  converts the input quaternion into a matrix
  /// \param  q - the quaternion to convert
  XM2_INLINE void fromQuaternion(const XQuaternion& q);

  /// \brief  Converts the matrix to an axis angle rotation (in degrees)
  /// \param  axis - the output axis
  /// \param  degrees - the output angle (in degrees)
  XM2_INLINE void toAxisAngleD(XVector3& axis, XReal& degrees) const;

  /// \brief  Converts the an axis angle (in degrees) into a matrix
  /// \param  axis - the axis
  /// \param  degrees - the angle (in degrees)
  XM2_INLINE void fromAxisAngleD(const XVector3& axis, XReal degrees);

  /// \brief  Converts the matrix to an axis angle (in radians)
  /// \param  axis - the output axis
  /// \param  radians - the output angle (in radians)
  XM2_INLINE void toAxisAngleR(XVector3& axis, XReal& radians) const;

  /// \brief  Converts the an axis angle (in radians) into a matrix
  /// \param  axis - the axis
  /// \param  radians - the angle (in radians)
  XM2_INLINE void fromAxisAngleR(const XVector3& axis, XReal radians);

  /// \brief  converts the rotation matrix to Euler angles
  /// \param  xd - returned x angle in degrees
  /// \param  yd - returned y angle in degrees
  /// \param  zd - returned z angle in degrees
  XM2_INLINE void toEulerAnglesD(XReal& xd, XReal &yd, XReal &zd) const;

  /// \brief  converts from euler angles into a matrix
  /// \param  xd - x angle in radians
  /// \param  yd - y angle in radians
  /// \param  zd - z angle in radians
  XM2_INLINE void fromEulerAnglesD(XReal xd, XReal yd, XReal zd);
  
  /// \brief  converts the rotation matrix to Euler angles
  /// \param  xr - returned x angle in radians
  /// \param  yr - returned y angle in radians
  /// \param  zr - returned z angle in radians
  XM2_INLINE void toEulerAnglesR(XReal& xr, XReal &yr, XReal &zr) const;

  /// \brief  converts from euler angles into a matrix
  /// \param  xr - x angle in radians
  /// \param  yr - y angle in radians
  /// \param  zr - z angle in radians
  XM2_INLINE void fromEulerAnglesR(XReal xr, XReal yr, XReal zr);

  /// \brief  extracts the amount of scale from the matrix
  /// \param  output - the returned scale amount
  /// \return false if the scale has a negative component, true if all positive
  XM2_INLINE bool getScale(XVector3& output) const;

  /// \brief  extracts the translation amount from the matrix
  /// \param  output - the returned translation amount
  XM2_INLINE void getTranslate(XVector3& output) const;

  /// \brief  decomposes this matrix into separate Translation (T), Rotation (R),
  ///         and Scale (S) values. 
  /// \param  T - the translation returned
  /// \param  R - the translation returned
  /// \param  S - the translation returned
  XM2_INLINE void decompose(XVector3& T, XQuaternion& R, XVector3& S) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix Multiplication
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  matrix multiplication

  /// \brief  multiplies 2 matrices together 
  /// \param  a - first matrix
  /// \param  b - second matrix
  XM2_INLINE void mul(const XMatrix& a, const XMatrix& b);
  
  /// \brief  multiplies 2 matrices together. 
  /// \param  a - first matrix
  /// \param  b - second matrix
  XM2_INLINE void quickMult(const XMatrix& a, const XMatrix& b);

  /// \brief  multiplies a matrix with a scalar
  /// \param  a - first matrix
  /// \param  b - the scalar value
  XM2_INLINE void mul(const XMatrix& a, XReal b);

  /// \brief  multiplies this matrix by a scalar
  /// \param  b - the scalar to multiply this by
  XM2_INLINE void mul(XReal b) { mul(*this,b); }

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix transformations of XVector3, XVector3 and XVector4
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  point/vector transformation

  /// \brief  transforms a vector4 by this matrix
  /// \param  output - the output vector
  /// \param  b - the vec4 to transform
  XM2_INLINE void transformVector(XVector4& output, const XVector4& b) const;
  
  /// \brief  transforms a vector3 by this matrix
  /// \param  output - the output vector
  /// \param  b - the vec3 to transform
  XM2_INLINE void transformPoint(XVector3& output, const XVector3& b) const;

  /// \brief  transforms a point3 by this matrix
  /// \param  output - the output vector
  /// \param  b - the point3 to transform
  XM2_INLINE void transformVector(XVector3& output, const XVector3& b) const;

  /// \brief  transforms a vector3 by this matrix
  /// \param  output - the output vector
  /// \param  b - the vec3 to transform
  XM2_INLINE void transformPoint(XVector3SOA& output, const XVector3SOA& b) const;

  /// \brief  transforms a vector3 by this matrix
  /// \param  output - the output vector
  /// \param  b - the vec3 to transform
  XM2_INLINE void transformVector(XVector3SOA& output, const XVector3SOA& b) const;

  /// \brief  transforms a vector4 by this matrix
  /// \param  output - the output vector
  /// \param  b - the vec4 to transform
  ///
  XM2_INLINE void transformVector(XVector4SOA& output, const XVector4SOA& b) const;

  /// \brief  transforms a vector4 by this matrix
  /// \param  output - the output vector
  ///
  XM2_INLINE void transformVector(XVector4& output) const;

  /// \brief  transforms a vector3 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformPoint(XVector3& output) const;

  /// \brief  transforms a point3 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformVector(XVector3& output) const;

  /// \brief  transforms a vector3 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformPoint(XVector3SOA& output) const;

  /// \brief  transforms a vector3 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformVector(XVector3SOA& output) const;

  /// \brief  transforms a vector4 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformVector(XVector4SOA& output) const;

  /// \brief  transforms an array of points by this matrix
  /// \param  output - the output array
  /// \param  b - the un-transformed points
  /// \param  num - the number of elements in the arrays
  void transformPoint(XVector3* output, const XVector3* b, int num) const;

  /// \brief  transforms an array of points by this matrix
  /// \param  output - the output array
  /// \param  b - the un-transformed points
  /// \param  num - the number of elements in the arrays
  void transformPoint(XVector3SOA* output, const XVector3SOA* b, int num) const;

  /// \brief  transforms an array of vectors by this matrix
  /// \param  output - the output array
  /// \param  b - the un-transformed vectors
  /// \param  num - the number of elements in the arrays
  void transformVector(XVector3* output, const XVector3* b, int num) const;

  /// \brief  transforms an array of vectors by this matrix
  /// \param  output - the output array
  /// \param  b - the un-transformed vectors
  /// \param  num - the number of elements in the arrays
  void transformVector(XVector3SOA* output, const XVector3SOA* b, int num) const;

  /// \brief  transforms an array of vectors by this matrix
  /// \param  output - the output array
  /// \param  b - the un-transformed vectors
  /// \param  num - the number of elements in the arrays
  void transformVector(XVector4* output, const XVector4* b, int num) const;

  /// \brief  transforms an array of vectors by this matrix
  /// \param  output - the output array
  /// \param  b - the un-transformed vectors
  /// \param  num - the number of elements in the arrays
  void transformVector(XVector4SOA* output, const XVector4SOA* b, int num) const;

  /// \brief  transforms a vector4 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformVector(XVector4Array& output, const XVector4Array& input) const;

  /// \brief  transforms a vector3 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformPoint(XVector3Array& output, const XVector3Array& input) const;

  /// \brief  transforms a point3 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformVector(XVector3Array& output, const XVector3Array& input) const;

  /// \brief  transforms a vector3 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformPoint(XVector3SOAArray& output, const XVector3SOAArray& input) const;

  /// \brief  transforms a vector3 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformVector(XVector3SOAArray& output, const XVector3SOAArray& input) const;

  /// \brief  transforms a vector4 by this matrix
  /// \param  output - the output vector
  XM2_INLINE void transformVector(XVector4SOAArray& output, const XVector4SOAArray& input) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix inversion
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  matrix inversion
  
  /// \brief  inverts this matrix
  XM2_INLINE void invert();

  /// \brief  inverts the matrix passed to this function
  /// \param  a - the matrix to invert
  void invert(const XMatrix& a);
  
  /// \brief  inverts this matrix
  XM2_INLINE void transpose();

  /// \brief  inverts this matrix
  XM2_INLINE void transpose(const XMatrix& a);

  /// \brief  calculates the determinant of the matrix
  XReal determinant() const;

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix axes access
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  axis access

  /// \brief  returns a reference to this matrices x axis
  /// \return the x axis of the matrix
  XM2_INLINE XVector3& xAxis() { return *(XVector3*)((void*)(data)); }

  /// \brief  returns a reference to this matrices y axis
  /// \return the y axis of the matrix
  XM2_INLINE XVector3& yAxis() { return *(XVector3*)((void*)(data+4)); }

  /// \brief  returns a reference to this matrices z axis
  /// \return the z axis of the matrix
  XM2_INLINE XVector3& zAxis() { return *(XVector3*)((void*)(data+8)); }

  /// \brief  returns a reference to this matrices translation vector
  /// \return the  translation vector of the matrix
  XM2_INLINE XVector4& wAxis() { return *(XVector4*)((void*)(data+12)); }


  /// \brief  returns a reference to this matrices x axis
  /// \return the x axis of the matrix
  XM2_INLINE const XVector3& xAxis() const { return *(XVector3*)((void*)(data)); }

  /// \brief  returns a reference to this matrices y axis
  /// \return the y axis of the matrix
  XM2_INLINE const XVector3& yAxis() const { return *(XVector3*)((void*)(data+4)); }

  /// \brief  returns a reference to this matrices z axis
  /// \return the z axis of the matrix
  XM2_INLINE const XVector3& zAxis() const { return *(XVector3*)((void*)(data+8)); }

  /// \brief  returns a reference to this matrices  translation vector
  /// \return the  translation vector of the matrix
  XM2_INLINE const XVector4& wAxis() const { return *(XVector4*)((void*)(data+12)); }

  //--------------------------------------------------------------------------------------------------------------------
  // XMatrix stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XMatrix the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XMatrix to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XMatrix from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XMatrix from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XMatrix to stdout
  void print32() const;

  /// \brief  prints this XMatrix to stdout
  void print64() const;

  /// \brief  writes this XMatrix to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XMatrix to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XMatrix from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XMatrix from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  
  
  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // C++ operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  /// \brief  equality operator
  /// \param  c - matrix to test against
  /// \return true if the same
  XM2_INLINE bool operator == (const XMatrix& c) const;

  /// \brief  in-equality operator
  /// \param  c - matrix to test against
  /// \return true if different
  XM2_INLINE bool operator != (const XMatrix& c) const;

  /// \brief  assignment operator
  /// \param  c - the matrix to copy
  /// \return *this
  XM2_INLINE const XMatrix& operator = (const XMatrix& c);

  /// \brief  multiplies this matrix by c
  /// \param  c - the matrix to multiply this by
  /// \return ref to this
  XM2_INLINE const XMatrix& operator *= (const XMatrix& c);

  /// \brief  multiplies this matrix by c
  /// \param  c - the matrix to multiply this by
  /// \return (this * c)
  XM2_INLINE XMatrix operator * (const XMatrix& c) const;
  
  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  /// \name  static array methods

  /// \brief  sets all elements of each matrix in the array to 0 
  /// \param  output - the array data to zero out
  /// \param  num - the number of elements in the array
  static bool arrayEqual(const XMatrix* a, const XMatrix* b, int num, XReal eps = XM2_FLOAT_ZERO);

  /// \brief  sets all elements of each matrix in the array to 0 
  /// \param  output - the array data to zero out
  /// \param  num - the number of elements in the array
  static bool arrayNotEqual(const XMatrix* a, const XMatrix* b, int num, XReal eps = XM2_FLOAT_ZERO);

  /// \brief  sets all elements of each matrix in the array to 0 
  /// \param  output - the array data to zero out
  /// \param  num - the number of elements in the array
  static void arrayZero(XMatrix* output, int num);

  /// \brief  sets all matrices in the array to the identity matrix
  /// \param  output - the matrix array
  /// \param  num - the number of matrices in the array
  static void arrayIdentity(XMatrix* output, int num);

  /// \brief  Converts an array of matrices to an array of quaternions
  /// \param  output - the returned quaternions
  /// \param  input - the input matrices
  /// \param  num - the number of matrices
  static void arrayToQuaternionArray(XQuaternion* output, const XMatrix* input, int num);

  /// \brief  converts an array of quaternions into an array of matrices
  /// \param  output - the returned matrices
  /// \param  input - the input quaternions
  /// \param  num - the number of matrices
  static void arrayFromQuaternionArray(XMatrix* output, const XQuaternion* input, int num);

  /// \brief  Converts an array of matrices to an array of quaternions
  /// \param  output - the returned quaternions
  /// \param  input - the input matrices
  /// \param  num - the number of matrices
  static void arrayToTransformArray(XTransform* output, const XMatrix* input, int num);

  /// \brief  converts an array of quaternions into an array of matrices
  /// \param  output - the returned matrices
  /// \param  input - the input quaternions
  /// \param  num - the number of matrices
  static void arrayFromTransformArray(XMatrix* output, const XTransform* input, int num);
  
  /// \brief  multiplies two arrays of matrices together
  /// \param  output - the output matrix array
  /// \param  a - first array of matrices
  /// \param  b - second array of matrices
  /// \param  num - number of matrices in the arrays
  static void arrayMul(XMatrix* output,const XMatrix* a,const XMatrix* b, int num);
  
  /// \brief  multiplies an array of matrices by a matrix
  /// \param  output - the output matrix array
  /// \param  a - array of input matrices
  /// \param  b - second matrix
  /// \param  num - the number of matrices in the array
  static void arrayMul(XMatrix* output,const XMatrix* a,const XMatrix& b, int num);

  /// \brief  multiplies two arrays of matrices together
  /// \param  output - the output matrix array
  /// \param  a - array of input matrices
  /// \param  b - second matrix
  /// \param  num - the number of matrices in the array
  static void arrayQuickMult(XMatrix* output, const XMatrix* a, const XMatrix* b, int num);

  /// \brief  multiplies an array of matrices by a matrix
  /// \param  output - the output matrix array
  /// \param  a - array of input matrices
  /// \param  b - second matrix
  /// \param  num - the number of matrices in the array
  static void arrayQuickMult(XMatrix* output, const XMatrix* a, const XMatrix& b, int num);

  /// \brief  multiplies an array of matrices by a scalar
  /// \param  output - the array of matrices
  /// \param  b - the scalar to multiply the array by
  /// \param  num - the number of matrices in the array
  static void arrayMul(XMatrix* output, XReal b, int num);

  /// \brief  multiplies an array of matrices by a scalar
  /// \param  output - the output array of matrices
  /// \param  a - the input matrix array
  /// \param  b - the scalar to multiply the array by
  /// \param  num - the number of matrices in the array
  static void arrayMul(XMatrix* output, const XMatrix* a, XReal b, int num);

  /// \brief  inverts an array of matrices
  /// \param  output - the array of matrices to invert
  /// \param  num - the number of matrices in the array
  static void arrayInvert(XMatrix* output, int num);

  /// \brief  inverts an array of matrices
  /// \param  output - the output matrix array
  /// \param  input - the input matrix array
  /// \param  num - the number of matrices in the array
  static void arrayInvert(XMatrix* output, const XMatrix* input, int num);
  
  /// \brief  transposes an array of matrices 
  /// \param  output - the array of matrices to invert
  /// \param  num - the number of matrices in the array
  static void arrayTranspose(XMatrix* output, int num);

  /// \brief  transposes an array of matrices 
  /// \param  output - the array of matrices to invert
  /// \param  num - the number of matrices in the array
  static void arrayTranspose(XMatrix* output, const XMatrix* input, int num);

  #ifdef DOXYGEN
      /// \name  data

      XReal m00; XReal m01; XReal m02; XReal m03;
      XReal m10; XReal m11; XReal m12; XReal m13;
      XReal m20; XReal m21; XReal m22; XReal m23;
      XReal m30; XReal m31; XReal m32; XReal m33;
      /// data in openGL matrix form
      XReal gl[16];
  #else
  union 
  {

    /// data as individual elements
    struct 
    {
      XReal m00; XReal m01; XReal m02; XReal m03;
      XReal m10; XReal m11; XReal m12; XReal m13;
      XReal m20; XReal m21; XReal m22; XReal m23;
      XReal m30; XReal m31; XReal m32; XReal m33;
    };

    /// data in openGL matrix form
    XReal data[16];

    /// data in 2D array form
    XReal mdata[4][4];

    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        /// data as SSE intrinsics
        struct 
        {
          /// x axis
          __m128 _xaxis; 
          
          /// y axis
          __m128 _yaxis;
          
          /// z axis
          __m128 _zaxis;

          /// position component
          __m128 _waxis;
        };
      #else
        /// data as SSE intrinsics
        struct 
        {
          /// x axis
          __m128d _xaxis_xy; 
          __m128d _xaxis_zw; 
          
          /// y axis
          __m128d _yaxis_xy;
          __m128d _yaxis_zw;
          
          /// z axis
          __m128d _zaxis_xy;
          __m128d _zaxis_zw;

          /// position component
          __m128d _waxis_xy;
          __m128d _waxis_zw;
        };
      #endif
    #endif
  };
  #endif
} 
XM2_ALIGN_SUFFIX(16);

#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XMatrix& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XMatrix& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Transform.h"
#include "XM2/Quaternion.h"
#include "XM2/Vector3.h"
#include "XM2/Vector4.h"
#include "XM2/Vector3SOA.h"
#include "XM2/Vector4SOA.h"
#include "XM2/Vector3Array.h"
#include "XM2/Vector4Array.h"
#include "XM2/Vector3SOAArray.h"
#include "XM2/Vector4SOAArray.h"
#include "XM2/inl/Matrix34.inl"
#if XM2_SSE
# include "XM2/inl/sse/Matrix34.inl"
#elif XM2_SSE2
# include "XM2/inl/sse2/Matrix34.inl"
#elif XM2_FPU
# include "XM2/inl/vanilla/Matrix34.inl"
#else
# error Unknown CPU architecture
#endif
//----------------------------------------------------------------------------------------------------------------------