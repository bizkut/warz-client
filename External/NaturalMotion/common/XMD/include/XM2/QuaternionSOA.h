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
class XM2EXPORT XVector3SOA;
class XM2EXPORT XQuaternion;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XQuaternionSOA
/// \brief   a class that stores 4 x quaternions in a structure of array format
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XQuaternionSOA
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOA Constructors
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XQuaternionSOA();

  /// \brief  copy ctor
  /// \param  q - the quaternion to copy
  XM2_INLINE XQuaternionSOA(const XQuaternionSOA& q);

  /// \brief  sets the data from an array of 4 quaternions
  /// \param  quats - an array of 4 quaternions
  XM2_INLINE XQuaternionSOA(const XQuaternion quats[4]);
  #if XM2_SSE
  XM2_INLINE XQuaternionSOA(const __m128& x_,
                            const __m128& y_,
                            const __m128& z_,
                            const __m128& w_);
  #elif XM2_SSE2
  XM2_INLINE XQuaternionSOA(const __m128d& x0_, const __m128d& x1_,
                            const __m128d& y0_, const __m128d& y1_,
                            const __m128d& z0_, const __m128d& z1_,
                            const __m128d& w0_, const __m128d& w1_);
  #endif


  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XQuaternion* input);

  /// \brief  streams data from this class into the output array, and converts the xyzw data
  ///         back into XQuaternions. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XQuaternion* input) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOA set/get XVector3 functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  set/get XVector4

  /// \brief  sets the first quaternion
  /// \param  x_ - x component of quaternion
  /// \param  y_ - y component of quaternion
  /// \param  z_ - z component of quaternion
  /// \param  w_ - w component of quaternion
  XM2_INLINE void set0(XReal x_, XReal y_, XReal z_, XReal w_);

  /// \brief  sets the second quaternion
  /// \param  x_ - x component of quaternion
  /// \param  y_ - y component of quaternion
  /// \param  z_ - z component of quaternion
  /// \param  w_ - w component of quaternion
  XM2_INLINE void set1(XReal x_, XReal y_, XReal z_, XReal w_);

  /// \brief  sets the third quaternion
  /// \param  x_ - x component of quaternion
  /// \param  y_ - y component of quaternion
  /// \param  z_ - z component of quaternion
  /// \param  w_ - w component of quaternion
  XM2_INLINE void set2(XReal x_, XReal y_, XReal z_, XReal w_);

  /// \brief  sets the fourth quaternion
  /// \param  x_ - x component of quaternion
  /// \param  y_ - y component of quaternion
  /// \param  z_ - z component of quaternion
  /// \param  w_ - w component of quaternion
  XM2_INLINE void set3(XReal x_, XReal y_, XReal z_, XReal w_);

  /// \brief  sets the first quaternion
  /// \param  quat - the new quaternion value
  XM2_INLINE void set0(const XQuaternion& quat);

  /// \brief  sets the second quaternion
  /// \param  quat - the new quaternion value
  XM2_INLINE void set1(const XQuaternion& quat);

  /// \brief  sets the third quaternion
  /// \param  quat - the new quaternion value
  XM2_INLINE void set2(const XQuaternion& quat);

  /// \brief  sets the fourth quaternion
  /// \param  quat - the new quaternion value
  XM2_INLINE void set3(const XQuaternion& quat);

  /// \brief  gets the first quaternion
  /// \param  quat - the returned quaternion
  XM2_INLINE void get0(XQuaternion& quat) const;

  /// \brief  gets the second quaternion
  /// \param  quat - the returned quaternion
  XM2_INLINE void get1(XQuaternion& quat) const;

  /// \brief  gets the third quaternion
  /// \param  quat - the returned quaternion
  XM2_INLINE void get2(XQuaternion& quat) const;

  /// \brief  gets the fourth quaternion
  /// \param  quat - the returned quaternion
  XM2_INLINE void get3(XQuaternion& quat) const;

  /// \brief  gets the first quaternion
  /// \return the returned quaternion
  XM2_INLINE XQuaternion get0() const;

  /// \brief  gets the second quaternion
  /// \return the returned quaternion
  XM2_INLINE XQuaternion get1() const;

  /// \brief  gets the third quaternion
  /// \return the returned quaternion
  XM2_INLINE XQuaternion get2() const;

  /// \brief  gets the fourth quaternion
  /// \return the returned quaternion
  XM2_INLINE XQuaternion get3() const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternion Initialisation / Conversion
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  sets all 4 quaternions to the identity quat (0,0,0,1)
  XM2_INLINE void identity();

  /// \brief  copies the quaternion data from q
  /// \param  q - the quat data to copy
  XM2_INLINE void copy(const XQuaternionSOA& q);

  /// \name  transform type conversion

  /// \brief  creates 4 quaternions from 4 axis angle rotations. x,y,z should be set to
  ///         the axis values, and w should be set to the angle (in degrees)
  /// \param  a - the axis angle data
  XM2_INLINE void fromAxisAngleD(const XQuaternionSOA& a);

  /// \brief  creates 4 quaternions from 4 axis angle rotations. x,y,z should be set to
  ///         the axis values, and w should be set to the angle (in radians)
  /// \param  a - the axis angle data
  XM2_INLINE void fromAxisAngleR(const XQuaternionSOA& a);

  /// \brief  converts the quaternion data to axis angle rotations
  /// \param  a - the output axis angle data. x,y,z will store the axes, w will store 
  ///         the angle (in degrees)
  XM2_INLINE void toAxisAngleR(XQuaternionSOA& a) const;

  /// \brief  converts the quaternion data to axis angle rotations
  /// \param  a - the output axis angle data. x,y,z will store the axes, w will store 
  ///         the angle (in radians)
  XM2_INLINE void toAxisAngleD(XQuaternionSOA& a) const;

  /// \brief  converts he quaternions to euler angles (XYZ) 
  /// \param  eulers - the output euler angles (in degrees)
  XM2_INLINE void toEulerAnglesD(XVector3SOA& eulers) const;

  /// \brief  converts he quaternions to euler angles (XYZ) 
  /// \param  eulers - the output euler angles (in radians)
  XM2_INLINE void toEulerAnglesR(XVector3SOA& eulers) const;

  /// \brief  converts the input euler angles into a set of quaternions
  /// \param  eulers - the input euler angles (in degrees)
  XM2_INLINE void fromEulerAnglesD(const XVector3SOA& eulers);

  /// \brief  converts the input euler angles into a set of quaternions
  /// \param  eulers - the input euler angles (in radians)
  XM2_INLINE void fromEulerAnglesR(const XVector3SOA& eulers);

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOA Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second quaternion to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XQuaternionSOA& b, XReal eps = XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second quaternion to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XQuaternionSOA& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOA Dot product
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math operations

  /// \brief  performs the dot product between the 2 quaternionSOA's
  /// \param  product - the returned dot products
  /// \param  b - the quaternion data to dot with
  XM2_INLINE void dot(XVector4& product, const XQuaternionSOA& b) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOA arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds two sets of quaternions together. 
  /// \param  a - input quaternion data
  /// \param  b - second quat
  XM2_INLINE void add(const XQuaternionSOA& a, const XQuaternionSOA& b);

  /// \brief  adds the quaternion b to all 4 quaternions in a
  /// \param  a - input quaternion data
  /// \param  b - second quat
  XM2_INLINE void add(const XQuaternionSOA& a, const XQuaternion& b);

  /// \brief  adds the quaternion data to this
  /// \param  a - input quaternion data
  XM2_INLINE void add(const XQuaternionSOA& a);

  /// \brief  multiplies the two sets of quaternions together
  /// \param  a - first input quaternion data
  /// \param  b - second input quaternion data
  XM2_INLINE void mul(const XQuaternionSOA& a, const XQuaternionSOA& b);

  /// \brief  multiplies the quaternion  together
  /// \param  a - first input quaternion data
  /// \param  b - second input quaternion data
  XM2_INLINE void mul(const XQuaternionSOA& a, const XQuaternion& b);

  /// \brief  multiplies the quaternion data by a scalar
  /// \param  a - input quaternion data
  /// \param  b - the scalar
  XM2_INLINE void mul(const XQuaternionSOA& a, XReal b);
  
  #ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
  /// \brief  multiplies the quaternion data by a scalar
  /// \param  a - input quaternion data
  /// \param  b - the scalar
  XM2_INLINE void mul(const __m128 b);
  
  /// \brief  multiplies the quaternion data by a scalar
  /// \param  a - input quaternion data
  /// \param  b - the scalar
  XM2_INLINE void mul(const XQuaternionSOA& a, const __m128 b);
  #else
  /// \brief  multiplies the quaternion data by a scalar
  /// \param  a - input quaternion data
  /// \param  b - the scalar
  XM2_INLINE void mul(const __m128d b);

  /// \brief  multiplies the quaternion data by a scalar
  /// \param  a - input quaternion data
  /// \param  b - the scalar
  XM2_INLINE void mul(const XQuaternionSOA& a, const __m128d b);
  #endif
  #endif

  /// \brief  multiplies the quaternion data by the quaternion b
  /// \param  b - second quat
  XM2_INLINE void mul(const XQuaternion& b);

  /// \brief  multiplies the quaternion data by a scalar
  /// \param  b - the scalar
  XM2_INLINE void mul(XReal b);

  /// \brief  inverts the quaternion data
  XM2_INLINE void invert();

  /// \brief  inverts the quaternion data
  /// \param  q - the quaternion data to invert
  XM2_INLINE void invert(const XQuaternionSOA& q);

  /// \brief  negates the quaternion data
  XM2_INLINE void negate();

  /// \brief  negates the quaternion data
  /// \param  q - the quaternion data to negate
  XM2_INLINE void negate(const XQuaternionSOA& q);

  /// \brief  returns the length of the quaternion
  /// \param  len - the returned quaternion lengths
  XM2_INLINE void length(XVector4& len) const;
  
  /// \brief  normalizes the quaternions
  XM2_INLINE void normalise();
  
  /// \brief  normalizes the quaternions
  /// \param  q - the data to be normalised
  XM2_INLINE void normalise(const XQuaternionSOA& q);

  /// \name  slerp / lerp

  /// \brief  sums a*t into this. Useful for lerping a set of quaternions into a single result
  /// \param  a - the quaternion data to multiply with t
  /// \param  t - a scalar value to weight the quaternion data
  XM2_INLINE void sum(const XQuaternionSOA& a, XReal t);

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - input quaternion data
  /// \param  b - second quat
  /// \param  t - interpolation amount, 0 to 1
  XM2_INLINE void slerp(const XQuaternionSOA& a, const XQuaternionSOA& b, XReal t);

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - input quaternion data
  /// \param  b - second quat
  /// \param  t - interpolation amount, 0 to 1
  XM2_INLINE void lerp(const XQuaternionSOA& a, const XQuaternionSOA& b, XReal t);

  /// \brief  compares a and b to find the shortest path to rotate you to b. The result
  ///         is stored in this set of quats is either b, or the negated version. 
  /// \param  a - the first set of quats to compare
  /// \param  b - the second set of quats to compare
  /// \param  product - the returned dot product (useful for slerp)
  XM2_INLINE void shortestPath(const XQuaternionSOA &a, const XQuaternionSOA &b, XVector4& product);

  /// \brief  compares a and b to find the longest path to rotate you to b. The result
  ///         is stored in this set of quats is either b, or the negated version. 
  /// \param  a - the first set of quats to compare
  /// \param  b - the second set of quats to compare
  /// \param  product - the returned dot product (useful for slerp)
  XM2_INLINE void longestPath(const XQuaternionSOA &a, const XQuaternionSOA &b, XVector4& product);

  #ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
  /// \brief  sums a*t into this. Useful for lerping a set of quaternions into a single result
  /// \param  a - the quaternion data to multiply with t
  /// \param  t - a scalar value to weight the quaternion data
  XM2_INLINE void sum(const XQuaternionSOA& a, const __m128 t);

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - input quaternion data
  /// \param  b - second quat
  /// \param  t - interpolation amount, 0 to 1
  XM2_INLINE void slerp(const XQuaternionSOA& a, const XQuaternionSOA& b, const __m128 t);

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - input quaternion data
  /// \param  b - second quat
  /// \param  t - interpolation amount, 0 to 1
  XM2_INLINE void lerp(const XQuaternionSOA& a, const XQuaternionSOA& b, const __m128 t);
  #else
  /// \brief  sums a*t into this. Useful for lerping a set of quaternions into a single result
  /// \param  a - the quaternion data to multiply with t
  /// \param  t0 - a scalar value to weight the quaternion data
  /// \param  t1 - a scalar value to weight the quaternion data
  XM2_INLINE void sum(const XQuaternionSOA& a, const __m128d t0, const __m128d t1);

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - input quaternion data
  /// \param  b - second quat
  /// \param  t0 - interpolation amount, 0 to 1 (for quats 0 and 1)
  /// \param  t1 - interpolation amount, 0 to 1 (for quats 2 and 3)
  XM2_INLINE void slerp(const XQuaternionSOA& a, const XQuaternionSOA& b, const __m128d t0, const __m128d t1);

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - input quaternion data
  /// \param  b - second quat
  /// \param  t0 - interpolation amount, 0 to 1 (for quats 0 and 1)
  /// \param  t1 - interpolation amount, 0 to 1 (for quats 2 and 3)
  XM2_INLINE void lerp(const XQuaternionSOA& a, const XQuaternionSOA& b, const __m128d t0, const __m128d t1);

  /// \brief  sums a*t into this. Useful for lerping a set of quaternions into a single result
  /// \param  a - the quaternion data to multiply with t
  /// \param  t - weight to multiply the quats by
  XM2_INLINE void sum(const XQuaternionSOA& a, const __m128d t) {sum(a,t,t);}

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - input quaternion data
  /// \param  b - second quat
  /// \param  t - interpolation amount, 0 to 1 
  XM2_INLINE void slerp(const XQuaternionSOA& a, const XQuaternionSOA& b, const __m128d t) {slerp(a,b,t,t);}

  /// \brief  interpolates between a and b with respect to t
  /// \param  a - input quaternion data
  /// \param  b - second quat
  /// \param  t - interpolation amount, 0 to 1 
  XM2_INLINE void lerp(const XQuaternionSOA& a, const XQuaternionSOA& b, const __m128d t) {lerp(a,b,t,t);}
  #endif
  #endif
  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOA stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XQuaternionSOA the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XQuaternionSOA to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XQuaternionSOA from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XQuaternionSOA from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XQuaternionSOA to stdout
  void print32() const;

  /// \brief  prints this XQuaternionSOA to stdout
  void print64() const;

  /// \brief  writes this XQuaternionSOA to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XQuaternionSOA to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XQuaternionSOA from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XQuaternionSOA from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // C++ operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE bool operator == (const XQuaternionSOA& c) const;
  XM2_INLINE bool operator != (const XQuaternionSOA& c) const;

  XM2_INLINE const XQuaternionSOA& operator = (const XQuaternionSOA& c);

  XM2_INLINE const XQuaternionSOA& operator *= (const XQuaternionSOA& c);
  XM2_INLINE const XQuaternionSOA& operator *= (XReal c);

  XM2_INLINE const XQuaternionSOA& operator += (const XQuaternionSOA& c);
  XM2_INLINE const XQuaternionSOA& operator -= (const XQuaternionSOA& c);
  
  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  //--------------------------------------------------------------------------------------------------------------------
  // XQuaternionSOA array functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  static array methods

  /// \brief  zeros all of the quaternions in an array
  /// \param  q - the array to set to zero
  /// \param  num - the number of SOA quaternions in the array
  static void arrayZero(XQuaternionSOA* q, int num);

  /// \brief  sets all of the quaternions in an array to the identity quaternion
  /// \param  q - the array to set to the identity
  /// \param  num - the number of SOA quaternions in the array
  static void arrayIdentity(XQuaternionSOA* q, int num);

  /// \brief  sums a*t into the output array
  /// \param  output - the output quaternion array
  /// \param  a - the input quaternion data
  /// \param  t - the blend factor
  /// \param  num - the number of quaternions in the array
  static void arraySum(XQuaternionSOA* output, const XQuaternionSOA* a, XReal t, int num);

  /// \brief  adds two arrays of quaternions together and stores the result in the output array
  /// \param  output - the output quaternion array
  /// \param  a - first quaternion array
  /// \param  b - second quaternion array
  /// \param  num - the number of SOA quaternions in the arrays
  static void arrayAdd(XQuaternionSOA* output, const XQuaternionSOA* a, const XQuaternionSOA* b, int num);

  /// \brief  adds the quaternion data b to an array of SOA quaternions, and stores the 
  ///         result in the output array
  /// \param  output - the output quaternion array
  /// \param  a - first quaternion array
  /// \param  b - second quaternion array
  /// \param  num - the number of SOA quaternions in the arrays
  static void arrayAdd(XQuaternionSOA* output, const XQuaternionSOA* a, const XQuaternionSOA& b, int num);

  /// \brief  multiplies two arrays of quaternions together and stores the result in the output array
  /// \param  output - the output quaternion array
  /// \param  a - first quaternion array
  /// \param  b - second quaternion array
  /// \param  num - the number of SOA quaternions in the arrays
  static void arrayMul(XQuaternionSOA* output, const XQuaternionSOA* a, const XQuaternionSOA* b, int num);

  /// \brief  multiplies the quaternion data b with an array of SOA quaternions, and stores the 
  ///         result in the output array
  /// \param  output - the output array
  /// \param  a - first quaternion array
  /// \param  b - second quaternion array
  /// \param  num - the number of quats in the array
  static void arrayMul(XQuaternionSOA* output, const XQuaternionSOA* a, const XQuaternionSOA& b, int num);

  /// \brief  multiplies an array of quaternions by an array of scalars.
  /// \param  output - the output quaternion array
  /// \param  a - first quaternion array
  /// \param  b - array of scalar values
  /// \param  num - the number of quats in the array
  static void arrayMul(XQuaternionSOA* output, const XQuaternionSOA* a, const XReal* b, int num);

  /// \brief  multiplies an array of quaternion data with a scalar
  /// \param  output - the output quaternion data
  /// \param  a - input quaternion data
  /// \param  b - scalar value
  /// \param  num - the number of quaternions in the array
  static void arrayMul(XQuaternionSOA* output, const XQuaternionSOA* a, XReal b, int num);

  /// \brief  interpolates between a and b with respect to t
  /// \param  output - the output quaternion data
  /// \param  a - the quaternion data to slerp from
  /// \param  b - the quaternion data to slerp to
  /// \param  t - interpolation amount, 0 to 1
  /// \param  num - the number of quats in the array
  static void arraySlerp(XQuaternionSOA* output, const XQuaternionSOA* a, const XQuaternionSOA* b, XReal t, int num);

  /// \brief  interpolates between a and b with respect to t using a normalised lerp.
  /// \param  output - the output quaternion data
  /// \param  a - the quaternion data to nlerp from
  /// \param  b - the quaternion data to nlerp to
  /// \param  t - interpolation amount, 0 to 1
  /// \param  num - the number of quats in the array
  static void arrayLerp(XQuaternionSOA* output, const XQuaternionSOA* a, const XQuaternionSOA* b, XReal t, int num);

  /// \brief  negates the array of quaternions
  /// \param  q - the quaternion data to negate
  /// \param  num - the number of quats in the array
  static void arrayNegate(XQuaternionSOA* q, int num);

  /// \brief  inverts an array of quaternions
  /// \param  q - the quaternion array to invert
  /// \param  num - the number of quats in the array
  static void arrayInvert(XQuaternionSOA* q, int num);
  
  /// \brief  normalizes an array quaternion data
  /// \param  q - the quaternion array to normalise
  /// \param  num - the number of quats in the array
  static void arrayNormalise(XQuaternionSOA* q, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // Member variables
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  data

  #ifdef DOXYGEN
    /// x components of the quaternions 
    XReal x[4]; 
    /// y components of the quaternions 
    XReal y[4];
    /// z components of the quaternions
    XReal z[4];
    /// w components of the quaternions
    XReal w[4];
    /// data stored as an array
    XReal data[16];
  #else
  union 
  {
    struct 
    {
      XReal x[4]; 
      XReal y[4];
      XReal z[4];
      XReal w[4];
    };
    XReal data[16];
    #ifdef XM2_USE_SSE
      struct {
      #if XM2_USE_FLOAT
        __m128 _x;
        __m128 _y;
        __m128 _z;
        __m128 _w;
      #else
        __m128d _x0;
        __m128d _x1;
        __m128d _y0;
        __m128d _y1;
        __m128d _z0;
        __m128d _z1;
        __m128d _w0;
        __m128d _w1;
      #endif
      };
    #endif
  };
  #endif
}
XM2_ALIGN_SUFFIX(16);

XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XQuaternionSOA& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XQuaternionSOA& c);
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Quaternion.h"
#include "XM2/Vector3SOA.h"
#include "XM2/inl/QuaternionSOA.inl"
#ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
    #include "XM2/inl/sse/QuaternionSOA.inl"
  #else
    #include "XM2/inl/sse2/QuaternionSOA.inl"
  #endif
#else
  #include "XM2/inl/vanilla/QuaternionSOA.inl"
#endif
//----------------------------------------------------------------------------------------------------------------------
