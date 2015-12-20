//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/MathCommon.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
class XM2EXPORT XVector3;
class XM2EXPORT XVector3;
class XM2EXPORT XVector4;
class XM2EXPORT XVector3SOA;
class XM2EXPORT XVector4SOA;
class XM2EXPORT XVector3Packed;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XVector3SOA
/// \brief   defines a class to hold 4 x XVector3s in a structure of array format
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XVector3SOA
{
public:

  XM2_INLINE XVector3SOA();
  XM2_INLINE XVector3SOA(const XVector3& a);
  XM2_INLINE XVector3SOA(const XVector4& a);
  XM2_INLINE XVector3SOA(const XVector3Packed& a);
  XM2_INLINE XVector3SOA(const XVector3SOA& a);
  XM2_INLINE XVector3SOA(const XVector4SOA& a);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA stream in/out functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  streaming / swizzling

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector3* input);

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector4* input);

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector4SOA& input);

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector3Packed* input);

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector4s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector3* output) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector4s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector4* output) const;

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamOut(XVector4SOA& output) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector4s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector3Packed* output) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA Initialisation functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  initialises all four vectors to 0,0,0
  XM2_INLINE void zero();

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA set/get XVector3 functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  get / set

  /// \brief  sets the first vector3
  /// \param  x_ - the x component of the vector
  /// \param  y_ - the y component of the vector
  /// \param  z_ - the z component of the vector
  XM2_INLINE void set0(XReal x_, XReal y_, XReal z_);

  /// \brief  sets the second vector3
  /// \param  x_ - the x component of the vector
  /// \param  y_ - the y component of the vector
  /// \param  z_ - the z component of the vector
  XM2_INLINE void set1(XReal x_, XReal y_, XReal z_);

  /// \brief  sets the third vector3
  /// \param  x_ - the x component of the vector
  /// \param  y_ - the y component of the vector
  /// \param  z_ - the z component of the vector
  XM2_INLINE void set2(XReal x_, XReal y_, XReal z_);

  /// \brief  sets the fourth vector3
  /// \param  x_ - the x component of the vector
  /// \param  y_ - the y component of the vector
  /// \param  z_ - the z component of the vector
  XM2_INLINE void set3(XReal x_, XReal y_, XReal z_);

  /// \brief  sets the first vector3
  /// \param  vec - the new vector3 value
  XM2_INLINE void set0(const XVector3& vec);

  /// \brief  sets the second vector3
  /// \param  vec - the new vector3 value
  XM2_INLINE void set1(const XVector3& vec);

  /// \brief  sets the third vector3
  /// \param  vec - the new vector3 value
  XM2_INLINE void set2(const XVector3& vec);

  /// \brief  sets the fourth vector3
  /// \param  vec - the new vector3 value
  XM2_INLINE void set3(const XVector3& vec);

  /// \brief  gets the first vector3
  /// \param  vec - the returned vector
  XM2_INLINE void get0(XVector3& vec) const;

  /// \brief  gets the second vector3
  /// \param  vec - the returned vector
  XM2_INLINE void get1(XVector3& vec) const;

  /// \brief  gets the third vector3
  /// \param  vec - the returned vector
  XM2_INLINE void get2(XVector3& vec) const;

  /// \brief  gets the fourth vector3
  /// \param  vec - the returned vector
  XM2_INLINE void get3(XVector3& vec) const;

  /// \brief  returns the first vector3
  /// \return the returned vector3
  XM2_INLINE XVector3 get0() const;

  /// \brief  returns the second vector3
  /// \return the returned vector3
  XM2_INLINE XVector3 get1() const;

  /// \brief  returns the third vector3
  /// \return the returned vector3
  XM2_INLINE XVector3 get2() const;

  /// \brief  returns the fourth vector3
  /// \return the returned vector3
  XM2_INLINE XVector3 get3() const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA comparison
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second vector to test
  /// \param  eps - the floating point error tolerance
  /// \return true if a and b are the same, false otherwise
  XM2_INLINE bool equal(const XVector3SOA& b, XReal eps = XM2_FLOAT_ZERO) const;

  /// \brief  this != b
  /// \param  b - second vector to test
  /// \param  eps - the floating point error tolerance
  /// \return true if a and b are the same, false otherwise
  XM2_INLINE bool notEqual(const XVector3SOA& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA / __m128 arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math

  #if XM2_USE_SSE2

  /// \brief  adds 2 vec3's together
  /// \code   
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOA& a, const __m128& b);

  /// \brief  subtracts 2 vec3's 
  /// \code   
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOA& a, const __m128& b);
  
  /// \brief  adds 2 vec3's together
  /// \code   
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOA& a, const __m128& b);

  /// \brief  subtracts 2 vec3's 
  /// \code   
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOA& a, const __m128& b);
  
  /// \brief  adds 2 vec3's together
  /// \code   
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const __m128& b);

  /// \brief  subtracts 2 vec3's 
  /// \code   
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const __m128& b);
  
  /// \brief  adds 2 vec3's together
  /// \code   
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  ///
  XM2_INLINE void div(const __m128& b);

  /// \brief  subtracts 2 vec3's 
  /// \code   
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const __m128& b);
  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA / XVector3 arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOA& a, const XVector3& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOA& a, const XVector3& b);
  
  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOA& a, const XVector3& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOA& a, const XVector3& b);
  
  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3& b);
  
  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA / XVector3SOA arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOA& a, const XVector3SOA& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOA& a, const XVector3SOA& b);

  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOA& a, const XVector3SOA& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOA& a, const XVector3SOA& b);

  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOA& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOA& b);

  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOA& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOA& b);
  
  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA / XVector4SOA arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOA& a, const XVector4SOA& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOA& a, const XVector4SOA& b);

  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOA& a, const XVector4SOA& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOA& a, const XVector4SOA& b);

  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOA& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOA& b);

  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOA& b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOA& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA / XReal arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOA& a, XReal b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOA& a, XReal b);
  
  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOA& a, XReal b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOA& a, XReal b);
  
  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(XReal b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(XReal b);
  
  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(XReal b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(XReal b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA / __m128 arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  #if XM2_SSE
  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOA& a, const __m128 b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOA& a, const __m128 b);
  
  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOA& a, const __m128 b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOA& a, const __m128 b);
  
  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const __m128 b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const __m128 b);
  
  /// \brief  adds 2 XVector3SOA's together
  /// \code   
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const __m128 b);

  /// \brief  subtracts 2 XVector3SOA's 
  /// \code   
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const __m128 b);
  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // dot/cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  performs a dot product between 2 vector 3's
  /// \param  _dot - the returned dot products for the four vectors
  /// \param  b - second vec3
  XM2_INLINE void dot(XVector4& _dot, const XVector3& b) const;

  /// \brief  performs a dot product between 2 vector 3's
  /// \param  _dot - the returned dot products for the four vectors
  /// \param  b - second vec3
  XM2_INLINE void dot(XVector4& _dot, const XVector3SOA& b) const;
  
  /// \brief  performs a cross product between 2 vector 3's
  /// \param  a - first vec3
  /// \param  b - second vec3
  XM2_INLINE void cross(const XVector3SOA& a, const XVector3& b);

  /// \brief  performs a cross product between 2 vector 3's
  /// \param  a - first vec3
  /// \param  b - second vec3
  XM2_INLINE void cross(const XVector3SOA& a, const XVector3SOA& b);
  
  #if XM2_SSE
  ///
  /// \brief  performs a dot product between this and b
  /// \param  b - second vec3
  /// \return the dot product of a and b
  XM2_INLINE __m128 dotSSE(const XVector3& b) const;
  ///
  /// \brief  performs a dot product between this and b
  /// \param  b - second vec3
  /// \return the dot product of a and b
  XM2_INLINE __m128 dotSSE(const XVector3SOA& b) const;
  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA length/normalisation routines
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  computes the length^2 of the vectors
  /// \param  _length_sqr - the returned length
  XM2_INLINE void lengthSquared(XVector4& _length_sqr) const;

  /// \brief  calculates the length of the vectors
  /// \param  _length - the returned length
  XM2_INLINE void length(XVector4& _length) const;

  /// \brief  normalises the vector's 
  XM2_INLINE void normalise();

  /// \brief  normalises the vector's 
  XM2_INLINE void normalise(const XVector3SOA& a);
  
  #if XM2_SSE
  /// \brief  computes the length^2 of the vectors
  /// \param  _length_sqr - the returned length
  XM2_INLINE __m128 lengthSquaredSSE() const;

  /// \brief  calculates the length of the vectors
  /// \param  _length - the returned length
  XM2_INLINE __m128 lengthSSE() const;
  #elif XM2_SSE2
  /// \brief  computes the length^2 of the vectors
  /// \param  _length_sqr - the returned length
  XM2_INLINE __m128d lengthSquaredSSE() const;

  /// \brief  calculates the length of the vectors
  /// \param  _length - the returned length
  XM2_INLINE __m128d lengthSSE() const;
  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA interpolation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates (LERPS) between vector a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector3SOA& a, const XVector3SOA& b, XReal t);
  
  /// \brief  linearly interpolates (LERPS) between vector a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector3SOA& a, const XVector3SOA& b, const __m128 t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector3SOA& a, const XVector3SOA& b,
                         const XVector3SOA& c, const XVector3SOA& d,
                         XReal t);
  
  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector3SOA& a, const XVector3SOA& b,
                         const XVector3SOA& c, const XVector3SOA& d,
                         const __m128 t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation 

  /// \brief  negates the vector3's
  XM2_INLINE void negate();

  /// \brief  negates the vector data a, and stores in this
  /// \param  a - the vector data to negate
  XM2_INLINE void negate(const XVector3SOA& a);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XVector3SOA  the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector3SOA  to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector3SOA  from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector3SOA  from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector3SOA  to stdout
  void print32() const;

  /// \brief  prints this XVector3SOA  to stdout
  void print64() const;

  /// \brief  writes this XVector3SOA  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XVector3SOA  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XVector3SOA  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XVector3SOA  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false); 

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE bool operator == (const XVector3SOA& c) const;
  XM2_INLINE bool operator != (const XVector3SOA& c) const;

  XM2_INLINE const XVector3SOA& operator = (const XReal c);
  XM2_INLINE const XVector3SOA& operator = (const XVector3& c);
  XM2_INLINE const XVector3SOA& operator = (const XVector3SOA& c);
  XM2_INLINE const XVector3SOA& operator = (const XVector4SOA& c);

  XM2_INLINE const XVector3SOA& operator += (const XVector3SOA& c);
  XM2_INLINE const XVector3SOA& operator -= (const XVector3SOA& c);
  XM2_INLINE const XVector3SOA& operator /= (const XVector3SOA& c);
  XM2_INLINE const XVector3SOA& operator *= (const XVector3SOA& c);

  XM2_INLINE const XVector3SOA& operator += (const XVector4SOA& c);
  XM2_INLINE const XVector3SOA& operator -= (const XVector4SOA& c);
  XM2_INLINE const XVector3SOA& operator /= (const XVector4SOA& c);
  XM2_INLINE const XVector3SOA& operator *= (const XVector4SOA& c);

  XM2_INLINE const XVector3SOA& operator += (const XVector3& c);
  XM2_INLINE const XVector3SOA& operator -= (const XVector3& c);
  XM2_INLINE const XVector3SOA& operator /= (const XVector3& c);
  XM2_INLINE const XVector3SOA& operator *= (const XVector3& c);

  XM2_INLINE const XVector3SOA& operator += (XReal c);
  XM2_INLINE const XVector3SOA& operator -= (XReal c);
  XM2_INLINE const XVector3SOA& operator /= (XReal c);
  XM2_INLINE const XVector3SOA& operator *= (XReal c);

  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  //--------------------------------------------------------------------------------------------------------------------
  // Member variables
  //--------------------------------------------------------------------------------------------------------------------
  #ifdef DOXYGEN
    /// \name  data

    /// x components of vectors
    XReal x[4]; 
    /// y components of vectors 
    XReal y[4];
    /// z components of vectors 
    XReal z[4];
    /// data stored as an array
    XReal data[12];
  #else
  union 
  {
    XReal data[12];
    struct 
    {
      XReal x[4]; 
      XReal y[4];
      XReal z[4];
    };
    struct 
    {
      XReal x0; XReal x1; XReal x2; XReal x3; 
      XReal y0; XReal y1; XReal y2; XReal y3; 
      XReal z0; XReal z1; XReal z2; XReal z3; 
    };
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        struct 
        {
          __m128 _x; 
          __m128 _y;
          __m128 _z;
        };
      #else
        struct 
        {
          __m128d _x0; 
          __m128d _x1; 
          __m128d _y0;
          __m128d _y1;
          __m128d _z0;
          __m128d _z1;
        };  
      #endif
    #endif
  };
  #endif

  /// \name  static array methods
  
  /// \brief  initialises an array of XVector3SOA to zero
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           ptr[i].zero();
  ///         }
  /// \endcode
  static void arrayZero(XVector3SOA* ptr, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA Arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds an array of XVectorSOAs to an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayAdd(XVector3SOA* output, const XVector3SOA* a, const XVector3SOA* b, int num);

  /// \brief  adds an array of XVectorSOAs to an XVector3SOA. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayAdd(XVector3SOA* output, const XVector3SOA* a, const XVector3SOA& b, int num);

  /// \brief  adds an XVector3 to an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayAdd(XVector3SOA* output, const XVector3SOA* a, const XVector3& b, int num);

  /// \brief  adds a float to an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayAdd(XVector3SOA* output, const XVector3SOA* a, XReal b, int num);

  /// \brief  subtracts an array of XVectorSOAs from an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arraySub(XVector3SOA* output, const XVector3SOA* a, const XVector3SOA* b, int num);

  /// \brief  subtracts an XVector3SOA from an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arraySub(XVector3SOA* output, const XVector3SOA* a, const XVector3SOA& b, int num);

  /// \brief  subtracts an XVector3 from an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arraySub(XVector3SOA* output, const XVector3SOA* a, const XVector3& b, int num);

  /// \brief  subtracts a float from an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arraySub(XVector3SOA* output, const XVector3SOA* a, XReal b, int num);

  /// \brief  multiplies an array of XVectorSOAs with an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayMul(XVector3SOA* output, const XVector3SOA* a, const XVector3SOA* b, int num);

  /// \brief  multiplies an XVector3SOA with an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayMul(XVector3SOA* output, const XVector3SOA* a, const XVector3SOA& b, int num);

  /// \brief  multiplies an XVector3 with an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayMul(XVector3SOA* output, const XVector3SOA* a, const XVector3& b, int num);

  /// \brief  multiplies an array of XVectorSOAs with a float. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  ///
  static void arrayMul(XVector3SOA* output, const XVector3SOA* a, XReal b, int num);

  /// \brief  divides an array of XVectorSOAs with an array of XVector3SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayDiv(XVector3SOA* output, const XVector3SOA* a,const XVector3SOA* b, int num);

  /// \brief  divides an array of XVectorSOAs with an XVector3SOA. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayDiv(XVector3SOA* output, const XVector3SOA* a,const XVector3SOA& b, int num);

  /// \brief  divides an array of XVectorSOAs with an aXVector3. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayDiv(XVector3SOA* output, const XVector3SOA* a,const XVector3& b, int num);

  /// \brief  divides an array of XVectorSOAs with a float. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static void arrayDiv(XVector3SOA* output, const XVector3SOA* a, XReal b, int num);

  /// \brief  adds an array of XVector4SOAs to an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayAdd(XVector3SOA* output, const XVector3SOA* b, int num);

  /// \brief  adds an XVector4SOA to an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayAdd(XVector3SOA* output, const XVector3SOA& b, int num);

  /// \brief  adds an XVector4 to an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayAdd(XVector3SOA* output, const XVector3& b, int num);

  /// \brief  adds an XReal to an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayAdd(XVector3SOA* output, XReal b, int num);

  /// \brief  subtracts an array of XVector4SOAs from an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arraySub(XVector3SOA* output, const XVector3SOA* b, int num);

  /// \brief  subtracts an XVector4SOA from an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arraySub(XVector3SOA* output, const XVector3SOA& b, int num);

  /// \brief  subtracts an XVector4 from an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arraySub(XVector3SOA* output, const XVector3& b, int num);

  /// \brief  subtracts an XReal from an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arraySub(XVector3SOA* output, XReal b, int num);

  /// \brief  multiplies an array of XVector4SOAs with an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayMul(XVector3SOA* output, const XVector3SOA* b, int num);

  /// \brief  multiplies an XVector4SOA with an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayMul(XVector3SOA* output, const XVector3SOA& b, int num);

  /// \brief  multiplies an XVector4 with an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayMul(XVector3SOA* output, const XVector3& b, int num);

  /// \brief  multiplies an XReal with an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayMul(XVector3SOA* output, XReal b, int num);

  /// \brief  divides an array of XVector4SOA with an array of XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayDiv(XVector3SOA* output, const XVector3SOA* b, int num);

  /// \brief  divides an array of XVector4SOA with an XVector4SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayDiv(XVector3SOA* output, const XVector3SOA& b, int num);

  /// \brief  divides an array of XVector4SOA with an XVector4. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayDiv(XVector3SOA* output, const XVector3& b, int num);


  /// \brief  divides an array of XVector4SOA with an XReal. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  b - input arg 1
  /// \param  num - the number of elements in the arrays
  static XM2_INLINE void arrayDiv(XVector3SOA* output, XReal b, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // dot & cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  performs the dot product on an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = dot( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  output - the dot product of a and b
  /// \param  a - first arg
  /// \param  b - second arg
  /// \param  num - the number of elements in the arrays
  static void arrayDot(XVector4* product, const XVector3SOA* a, const XVector3SOA* b, int num);

  /// \brief  performs the dot product on an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = dot( a[i], b );
  ///         }
  /// \endcode
  /// \param  output - the dot product of a and b
  /// \param  a - first arg
  /// \param  b - second arg
  /// \param  num - the number of elements in the arrays
  static void arrayDot(XVector4* product, const XVector3SOA* a, const XVector3SOA& b, int num);

  /// \brief  performs the dot product on an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = dot( a[i], b );
  ///         }
  /// \endcode
  /// \param  output - the dot product of a and b
  /// \param  a - first arg
  /// \param  b - second arg
  /// \param  num - the number of elements in the arrays
  static void arrayDot(XVector4* product, const XVector3SOA* a, const XVector3& b, int num);

  /// \brief  performs the cross product on an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  output - the cross product of a and b
  /// \param  a - first arg
  /// \param  b - second arg
  /// \param  num - the number of elements in the arrays
  static void arrayCross(XVector3SOA* output, const XVector3SOA* a, const XVector3SOA* b, int num);

  /// \brief  performs the cross product on an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = cross( a[i], b );
  ///         }
  /// \endcode
  /// \param  output - the cross product of a and b
  /// \param  a - first arg
  /// \param  b - second arg
  /// \param  num - the number of elements in the arrays
  static void arrayCross(XVector3SOA* output, const XVector3SOA* a, const XVector3SOA& b, int num);

  /// \brief  performs the cross product on an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = cross( a[i], b );
  ///         }
  /// \endcode
  /// \param  output - the cross product of a and b
  /// \param  a - first arg
  /// \param  b - second arg
  /// \param  num - the number of elements in the arrays
  static void arrayCross(XVector3SOA* output, const XVector3SOA* a, const XVector3& b, int num);

  /// \brief  normalises an array of XVector3SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise( a[i] );
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static XM2_INLINE void arrayNormalise(XVector3SOA* output, int num);

  /// \brief  normalises an array of XVector3SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise( a[i] );
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static void arrayNormalise(XVector3SOA* output, const XVector3SOA* input, int num);

  /// \brief  normalises an array of XVector3SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise( a[i] );
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static XM2_INLINE void arrayNegate(XVector3SOA* output, int num);

  /// \brief  normalises an array of XVector3SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise( a[i] );
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static void arrayNegate(XVector3SOA* output, const XVector3SOA* input, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates (LERPS) between the XVector3SOA arrays a and b and 
  ///         stores the result in the output array.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = lerp( a[i], b[i], t );
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - the first array
  /// \param  b - the 2nd array
  /// \param  t - the 0 to 1 interpolation control value
  /// \param  num - the number of elements in the arrays
  static void arrayLerp(XVector3SOA* output,
                        const XVector3SOA* a, const XVector3SOA* b,
                        XReal t, int num);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = bezier( a[i], b[i], c[i], d[i], t );
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - the first array
  /// \param  b - the 2nd array
  /// \param  c - the 3rd array
  /// \param  d - the 4th array
  /// \param  t - the 0 to 1 interpolation control value
  /// \param  num - the number of elements in the arrays
  static void arrayBezier(XVector3SOA* output,
                          const XVector3SOA* a, const XVector3SOA* b,
                          const XVector3SOA* c, const XVector3SOA* d,
                          XReal t, int num);

}
XM2_ALIGN_SUFFIX(16);

#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XVector3SOA& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XVector3SOA& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector4.h"
#include "XM2/Vector4SOA.h"
#include "XM2/inl/Vector3SOA.inl"
#ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
    #include "XM2/inl/sse/Vector3SOA.inl"
  #else
    #include "XM2/inl/sse2/Vector3SOA.inl"
  #endif
#else
  #include "XM2/inl/vanilla/Vector3SOA.inl"
#endif
//----------------------------------------------------------------------------------------------------------------------
