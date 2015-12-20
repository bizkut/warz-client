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

class XM2EXPORT XMatrix;
class XM2EXPORT XVector3;
class XM2EXPORT XVector4;
class XM2EXPORT XVector3;
class XM2EXPORT XTransform;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XQuaternion
/// \brief   An aligned quaternion type
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XQuaternion
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternion Constructors
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XQuaternion();

  /// \brief  copy ctor
  /// \param  q - the quat to copy
  XM2_INLINE XQuaternion(const XQuaternion& q);

  /// \brief  ctor - converts from a matrix
  /// \param  m - the matrix to copy
  XM2_INLINE XQuaternion(const XMatrix& m);

  /// \brief  ctor - copies quat from the transform
  /// \param  m - the transform
             XQuaternion(const XTransform& m);

  /// \brief  ctor - converts from an axis angle 
  /// \param  a - the axis 
  /// \param  radians - the angle
  XM2_INLINE XQuaternion(const XVector3& a, XReal radians);

  /// \brief  initialises from float components
  /// \param  x - the x quaternion component
  /// \param  y - the y quaternion component
  /// \param  z - the z quaternion component
  /// \param  w - the w quaternion component
  XM2_INLINE XQuaternion(XReal x, XReal y, XReal z, XReal w);

  #ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
  XM2_INLINE XQuaternion(const __m128& q);
  #else
  XM2_INLINE XQuaternion(const __m128d& xy,const __m128d& zw);
  #endif
  #endif 

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternion Initialisation / Conversion
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initiasation

  /// \brief  sets the quaternion to the identity quaternion (0,0,0,1) 
  XM2_INLINE void zero();

  /// \brief  sets the quaternion to the identity quaternion (0,0,0,1) 
  XM2_INLINE void identity();

  /// \brief  copies q into this quaternion
  /// \param  q - the quaternion to copy
  XM2_INLINE void copy(const XQuaternion& q);
  
  /// \brief  sets the quaternion to the specified values
  /// \param  x - the x quaternion component
  /// \param  y - the y quaternion component
  /// \param  z - the z quaternion component
  /// \param  w - the w quaternion component
  XM2_INLINE void set(XReal x, XReal y, XReal z, XReal w);

  /// \name  transform type conversion

  /// \brief  creates a quaternion from an axis angle rotation
  /// \param  axis - the axis of rotation
  /// \param  degrees - the angle in degrees
  XM2_INLINE void fromAxisAngleD(const XVector3& axis, XReal degrees);

  ///
  /// \brief  creates a quaternion from an axis angle rotation
  /// \param  axis - the axis of rotation
  /// \param  radians - the angle in radians
  XM2_INLINE void fromAxisAngleR(const XVector3& axis, XReal radians);

  /// \brief  creates a quaternion from euler angles
  /// \param  x - the x rotation (in degrees)
  /// \param  y - the y rotation (in degrees)
  /// \param  z - the z rotation (in degrees)
  XM2_INLINE void fromEulerAnglesD(const XReal x, XReal y, XReal z);

  /// \brief  creates a quaternion from euler angles
  /// \param  x - the x rotation (in radians)
  /// \param  y - the y rotation (in radians)
  /// \param  z - the z rotation (in radians)
  XM2_INLINE void fromEulerAnglesR(const XReal x, XReal y, XReal z);

  /// \brief  creates quaternion from a rotation matrix
  /// \param  mat - the rotation matrix
  XM2_INLINE void fromMatrix(const XMatrix& mat);

  /// \brief  converts this quaternion to a matrix
  /// \param  mat - the output matrix
  XM2_INLINE void toMatrix(XMatrix& mat) const;

  /// \brief  converts this quaternion to euler angles.
  /// \param  x - the output x rotation (in radians)
  /// \param  y - the output y rotation (in radians)
  /// \param  z - the output z rotation (in radians)
  void toEulerAnglesR(XReal& x,XReal& y,XReal& z) const;

  /// \brief  converts this quaternion to euler angles.
  /// \param  x - the output x rotation (in degrees)
  /// \param  y - the output y rotation (in degrees)
  /// \param  z - the output z rotation (in degrees)
  XM2_INLINE void toEulerAnglesD(XReal& x,XReal& y,XReal& z) const;

  /// \brief  converts this quaternion to an axis angle rotation
  /// \param  axis - the output axis
  /// \param  angle - the output angle (in radians)
  XM2_INLINE void toAxisAngleR(XVector3& axis, XReal& angle) const;

  /// \brief  converts this quaternion to an axis angle rotation
  /// \param  axis - the output axis
  /// \param  angle - the output angle (in degrees)
  XM2_INLINE void toAxisAngleD(XVector3& axis, XReal& angle) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternion Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second quaternion to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XQuaternion& b, XReal eps = XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second quaternion to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XQuaternion& b, XReal eps = XM2_FLOAT_ZERO) const;

  /// \brief  tests to see if the two quaternions produce the same result (i.e. they are either the same quaternion
  ///         or one is the negate of the other). 
  /// \param  b - second quaternion to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equivalent(const XQuaternion& b, XReal eps = XM2_FLOAT_ZERO) const;

  /// \brief  tests to see if this is not equivalent to b
  /// \param  b - second quaternion to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEquivalent(const XQuaternion& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternion Dot product
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math operations

  /// \brief  performs the dot product between 2 quaternions
  /// \param  b - the quaternion to dot against
  /// \return the dot product
  XM2_INLINE XReal dot(const XQuaternion& b) const;

  #ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
  /// \brief  performs the dot product between 2 quaternions
  /// \param  b - the quaternion to dot against
  /// \return the dot product
  XM2_INLINE __m128 dotSSE(const XQuaternion& b) const;
  #else
  /// \brief  performs the dot product between 2 quaternions
  /// \param  b - the quaternion to dot against
  /// \return the dot product
  XM2_INLINE __m128d dotSSE(const XQuaternion& b) const;
  #endif
  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternion Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  does a component wise add of a and b, and stores the result in this
  /// \param  a - first quaternion
  /// \param  b - second quaternion
  XM2_INLINE void add(const XQuaternion& a, const XQuaternion& b);

  /// \brief  adds each component of a to this
  /// \param  a - quaternion to add to this
  XM2_INLINE void add(const XQuaternion& a);

  /// \brief  multiplies two quaternions together. 
  /// \param  a - first quaternion
  /// \param  b - second quaternion
  XM2_INLINE void mul(const XQuaternion& a, const XQuaternion& b);

  /// \brief  multiplies two quaternions together. 
  /// \param  a - first quaternion
  /// \param  b - second quaternion
  XM2_INLINE void mul(const XQuaternion& a, XReal b);

  /// \brief  multiplies two quaternions together. 
  /// \param  b - second quaternion
  XM2_INLINE void mul(const XQuaternion& b);

  /// \brief  multiplies a quaternion with a scalar
  /// \param  b - scalar value
  XM2_INLINE void mul(XReal b);

  /// \brief  inverts a quaternion 
  XM2_INLINE void invert();

  /// \brief  inverts a quaternion 
  /// \param  q - the quaternion to invert
  XM2_INLINE void invert(const XQuaternion& q);
  
  /// \brief  negates the quaternion - this produces an identical rotation
  XM2_INLINE void negate();

  /// \brief  negates a quaternion - this produces an identical rotation
  /// \param  q - the quaternion to negate
  XM2_INLINE void negate(const XQuaternion& q);

  /// \brief  returns the length of the quaternion
  /// \return the length of the quaternion
  XM2_INLINE XReal length() const;
  
  #ifdef XM2_SSE
  /// \brief  returns the length of the quaternion
  /// \return the length of the quaternion
  XM2_INLINE __m128 lengthSSE() const;
  #elif XM2_SSE2
  /// \brief  returns the length of the quaternion
  /// \return the length of the quaternion
  XM2_INLINE __m128d lengthSSE() const;
  #endif
  
  /// \brief  normalizes the quaternion
  XM2_INLINE void normalise();
  
  /// \brief  normalizes the quaternion
  /// \param  q - the quaternion to normalize
  XM2_INLINE void normalise(const XQuaternion& q);

  /// \name  slerp/lerp

  /// \brief  sums a*t into this. Useful for lerping a set of quaternions into a single outcome
  /// \param  a - the quaternion to sum
  /// \param  t - the amount of a to include.
  XM2_INLINE void sum(const XQuaternion& a, XReal t);

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - first quaternion
  /// \param  b - second quaternion
  /// \param  t - interpolation amount, 0 to 1
  XM2_INLINE void slerp(const XQuaternion& a, const XQuaternion& b, XReal t);

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - first quaternion
  /// \param  b - second quaternion
  /// \param  t - interpolation amount, 0 to 1
  XM2_INLINE void lerp(const XQuaternion& a, const XQuaternion& b, XReal t);

  /// \brief  compares a and b to find the shortest path to rotate you to b. The result
  ///         is stored in this set of quats is either b, or the negated version. 
  /// \param  a - the first set of quats to compare
  /// \param  b - the second set of quats to compare
  /// \param  product - the returned dot product (useful for slerp)
  XM2_INLINE void shortestPath(const XQuaternion &a, const XQuaternion &b, XReal& product);

  /// \brief  compares a and b to find the longest path to rotate you to b. The result
  ///         is stored in this set of quats is either b, or the negated version. 
  /// \param  a - the first set of quats to compare
  /// \param  b - the second set of quats to compare
  /// \param  product - the returned dot product (useful for slerp)
  XM2_INLINE void longestPath(const XQuaternion &a, const XQuaternion &b, XReal& product);

  #if XM2_SSE
  /// \brief  compares a and b to find the shortest path to rotate you to b. The result
  ///         is stored in this set of quats is either b, or the negated version. 
  /// \param  a - the first set of quats to compare
  /// \param  b - the second set of quats to compare
  /// \param  product - the returned dot product (useful for slerp)
  XM2_INLINE void shortestPath(const XQuaternion &a, const XQuaternion &b, __m128& product);

  /// \brief  compares a and b to find the longest path to rotate you to b. The result
  ///         is stored in this set of quats is either b, or the negated version. 
  /// \param  a - the first set of quats to compare
  /// \param  b - the second set of quats to compare
  /// \param  product - the returned dot product (useful for slerp)
  XM2_INLINE void longestPath(const XQuaternion &a, const XQuaternion &b, __m128& product);
  #elif XM2_SSE2

  /// \brief  compares a and b to find the shortest path to rotate you to b. The result
  ///         is stored in this set of quats is either b, or the negated version. 
  /// \param  a - the first set of quats to compare
  /// \param  b - the second set of quats to compare
  /// \param  product - the returned dot product (useful for slerp)
  XM2_INLINE void shortestPath(const XQuaternion &a, const XQuaternion &b, __m128d& product);

  /// \brief  compares a and b to find the longest path to rotate you to b. The result
  ///         is stored in this set of quats is either b, or the negated version. 
  /// \param  a - the first set of quats to compare
  /// \param  b - the second set of quats to compare
  /// \param  product - the returned dot product (useful for slerp)
  XM2_INLINE void longestPath(const XQuaternion &a, const XQuaternion &b, __m128d& product);
  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternion stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XQuaternion the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XQuaternion to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XQuaternion from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XQuaternion from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XQuaternion to stdout
  void print32() const;

  /// \brief  prints this XQuaternion to stdout
  void print64() const;

  /// \brief  writes this XQuaternion to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XQuaternion to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XQuaternion from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XQuaternion from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // C++ operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE bool operator == (const XQuaternion& c) const;
  XM2_INLINE bool operator != (const XQuaternion& c) const;

  XM2_INLINE const XQuaternion& operator = (const XQuaternion& c);
  XM2_INLINE const XQuaternion& operator = (const XMatrix& c);
             const XQuaternion& operator = (const XTransform& t);

  XM2_INLINE const XQuaternion& operator *= (const XQuaternion& c);
  XM2_INLINE const XQuaternion& operator *= (const XReal c);

  XM2_INLINE const XQuaternion& operator += (const XQuaternion& c);
  XM2_INLINE const XQuaternion& operator -= (const XQuaternion& c);
  
  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternion array functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  static array methods

  /// \brief  sets an array of quaternions to zero
  /// \param  q - output quaternion array
  /// \param  num - the number of quaternions in the array
  static bool arrayEqual(const XQuaternion* a, const XQuaternion* b, int num, XReal eps);

  /// \brief  sets an array of quaternions to zero
  /// \param  q - output quaternion array
  /// \param  num - the number of quaternions in the array
  static bool arrayNotEqual(const XQuaternion* a, const XQuaternion* b, int num, XReal eps);

  /// \brief  sets an array of quaternions to zero
  /// \param  q - output quaternion array
  /// \param  num - the number of quaternions in the array
  static void arrayZero(XQuaternion* q, int num);

  /// \brief  sets an array of quaternions to zero
  /// \param  q - output quaternion array
  /// \param  num - the number of quaternions in the array
  static void arrayIdentity(XQuaternion* q, int num);

  /// \brief  Converts an array of matrices to an array of quaternions
  /// \param  output - the returned quaternions
  /// \param  input - the input matrices
  /// \param  num - the number of quaternions
  static void arrayFromMatrixArray(XQuaternion* output, const XMatrix* input, int num);

  /// \brief  converts an array of quaternions into an array of matrices
  /// \param  output - the returned matrices
  /// \param  input - the input quaternions
  /// \param  num - the number of quaternions
  static void arrayToMatrixArray(XMatrix* output, const XQuaternion* input, int num);

  /// \brief  Converts an array of transforms to an array of quaternions
  /// \param  output - the returned quaternions
  /// \param  input - the input quaternions
  /// \param  num - the number of quaternions
  static void arrayFromTransformArray(XQuaternion* output, const XTransform* input, int num);

  /// \brief  converts an array of quaternions into an array of transforms
  /// \param  output - the returned transforms
  /// \param  input - the input quaternions
  /// \param  num - the number of quaternions
  static void arrayToTransformArray(XTransform* output, const XQuaternion* input, int num);

  /// \brief  performs a dot product on two arrays of quaternions
  /// \param  output - the output dot product values
  /// \param  a - input quaternion array
  /// \param  b - input quaternion array
  /// \param  num - the number of quaternions in the array
  static void arrayDot(XReal* output, const XQuaternion* a, const XQuaternion* b, int num);

  /// \brief  multiples the elements of the array a by t, and sums the result into the 
  ///         output array
  /// \param  output - the output quaternion array
  /// \param  a - input quaternion array
  /// \param  t - the weighting factor
  /// \param  num - the number of quaternions in the array
  static void arraySum(XQuaternion* output, const XQuaternion* a, XReal t, int num);

  /// \brief  adds the two input arrays together and stores the result in the output array
  /// \param  output - the output quaternion array
  /// \param  a - first quaternion array
  /// \param  b - second quaternion array
  /// \param  num - the number of quaternions in the arrays
  static void arrayAdd(XQuaternion* output, const XQuaternion* a, const XQuaternion* b, int num);

  /// \brief  adds a quaternion to each element of the quaternion array, and stores the 
  ///         result in the output array
  /// \param  output - the output quaternion array
  /// \param  a - first quaternion array
  /// \param  b - second quaternion 
  /// \param  num - the number of quaternions in the arrays
  static void arrayAdd(XQuaternion* output, const XQuaternion* a, const XQuaternion& b, int num);

  /// \brief  multiplies two quaternion arrays together. 
  /// \param  output - the output quaternion array
  /// \param  a - first quaternion array
  /// \param  b - second quaternion array
  /// \param  num - the number of quaternions in the array
  static void arrayMul(XQuaternion* output, const XQuaternion* a, const XQuaternion* b, int num);

  /// \brief  multiplies a quaternion array with a quaternion.
  /// \param  output - the output quaternion array
  /// \param  a - first quaternion array
  /// \param  b - second quaternion 
  /// \param  num - the number of quaternions in the array
  static void arrayMul(XQuaternion* output, const XQuaternion* a, const XQuaternion& b, int num);

  /// \brief  multiplies an array of quaternions with an array of scalars
  /// \param  output - the output quaternion array
  /// \param  a - input quaternion array
  /// \param  b - input float array
  /// \param  num - the number of quaternions in the array
  static void arrayMul(XQuaternion* output, const XQuaternion* a, const XReal* b, int num);

  /// \brief  multiplies an array of quaternions by a scalar
  /// \param  output - the output quaternion array
  /// \param  a - input quaternion array
  /// \param  b - scalar
  /// \param  num - the number of quaternions in the array
  static void arrayMul(XQuaternion* output, const XQuaternion* a, XReal b, int num);

  /// \brief  multiplies two quaternion arrays together. 
  /// \param  output - the output quaternion array
  /// \param  a - first quaternion array
  /// \param  b - second quaternion array
  /// \param  num - the number of quaternions in the array
  static void arrayMul(XQuaternion* output, const XQuaternion* b, int num);

  /// \brief  multiplies a quaternion array with a quaternion.
  /// \param  output - the output quaternion array
  /// \param  a - first quaternion array
  /// \param  b - second quaternion 
  /// \param  num - the number of quaternions in the array
  static void arrayMul(XQuaternion* output, const XQuaternion& b, int num);

  /// \brief  multiplies an array of quaternions with an array of scalars
  /// \param  output - the output quaternion array
  /// \param  a - input quaternion array
  /// \param  b - input float array
  /// \param  num - the number of quaternions in the array
  static XM2_INLINE void arrayMul(XQuaternion* output, const XReal* b, int num);

  /// \brief  multiplies an array of quaternions by a scalar
  /// \param  output - the output quaternion array
  /// \param  a - input quaternion array
  /// \param  b - scalar
  /// \param  num - the number of quaternions in the array
  static XM2_INLINE void arrayMul(XQuaternion* output, XReal b, int num);

  /// \brief  spherically interpolates between two input quaternion arrays with respect to t,
  ///         and stores the result in the output array
  /// \param  output - the output quaternion array
  /// \param  a - the quaternion array to slerp from
  /// \param  b - the quaternion array to slerp to
  /// \param  t - interpolation amount, 0 to 1
  /// \param  num - the number of quaternions in the array
  static void arraySlerp(XQuaternion* output, const XQuaternion* a, const XQuaternion* b, XReal t, int num);

  /// \brief  linearly interpolates between two input quaternion arrays with respect to t,
  ///         and stores the normalised result in the output array
  /// \param  output - the output quaternion array
  /// \param  a - the quaternion array to slerp from
  /// \param  b - the quaternion array to slerp to
  /// \param  t - interpolation amount, 0 to 1
  /// \param  num - the number of quaternions in the array
  static void arrayLerp(XQuaternion* output, const XQuaternion* a, const XQuaternion* b, XReal t, int num);

  /// \brief  negates an array of quaternions
  /// \param  q - the quaternion array to negate
  /// \param  num - the number of quaternions in the array
  ///
  static XM2_INLINE void arrayNegate(XQuaternion* q, int num) {arrayNegate(q,q,num);}

  /// \brief  inverts an array of quaternions
  /// \param  q - the quaternion array to invert
  /// \param  num - the number of quaternions in the array
  static XM2_INLINE void arrayInvert(XQuaternion* q, int num) {arrayInvert(q,q,num);}

  /// \brief  normalises an array of quaternions
  /// \param  q - the quaternion array to normalise
  /// \param  num - the number of quaternions in the array
  static XM2_INLINE void arrayNormalise(XQuaternion* q, int num) {arrayNormalise(q,q,num);}

  /// \brief  negates an array of quaternions
  /// \param  q - the output quaternion array 
  /// \param  input - the array to negate
  /// \param  num - the number of quaternions in the array
  static void arrayNegate(XQuaternion* q, const XQuaternion* input, int num);

  /// \brief  inverts an array of quaternions
  /// \param  q - the output quaternion array 
  /// \param  input - the array to invert
  /// \param  num - the number of quaternions in the array
  static void arrayInvert(XQuaternion* q, const XQuaternion* input, int num);

  /// \brief  normalises an array of quaternions
  /// \param  q - the output quaternion array 
  /// \param  input - the array to normalise
  /// \param  num - the number of quaternions in the array
  static void arrayNormalise(XQuaternion* q, const XQuaternion* input, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // Member variables
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  data

  #ifdef DOXYGEN
    /// x component 
    XReal x; 
    /// y component 
    XReal y;
    /// z component 
    XReal z;
    /// w component 
    XReal w;
    /// data stored as an array
    XReal data[4];
  #else
  union 
  {
    struct 
    {
      XReal x; 
      XReal y;
      XReal z;
      XReal w;
    };
    XReal data[4];
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        __m128 sse;
      #else
        struct 
        {
          __m128d xy;
          __m128d zw;
        };
      #endif
    #endif
  };
  #endif
}
XM2_ALIGN_SUFFIX(16);

XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XQuaternion& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XQuaternion& c);
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Matrix34.h"
#include "XM2/inl/Quaternion.inl"
#if XM2_SSE
# include "XM2/inl/sse/Quaternion.inl"
#elif XM2_SSE2
# include "XM2/inl/sse2/Quaternion.inl"
#elif XM2_FPU
# include "XM2/inl/vanilla/Quaternion.inl"
#else
# error Unknown CPU architecture
#endif
//----------------------------------------------------------------------------------------------------------------------
