//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/MathCommon.h"
//----------------------------------------------------------------------------------------------------------------------
// XVector2 only uses SSE if the 
#ifdef XM2_USE_SSE
  #if !XM2_USE_FLOAT
    #define XM2_VEC2_SSE 
  #endif
#endif

namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
/// \class   XMD::XVector2
/// \brief   A 2D vector type
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
#ifdef XM2_VEC2_SSE
XM2_ALIGN_PREFIX(16) 
#endif
class XM2EXPORT XVector2
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Constructors
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XVector2() { set(0,0); }

  /// \brief  copy ctor
  /// \param  rhs - the XVector2 to copy
  XM2_INLINE XVector2(const XVector2& rhs) { memcpy(this,&rhs,sizeof(XVector2)); }

  /// \brief  ctor - initialises XVector2 elements
  /// \param  _x - the x XVector2 component
  /// \param  _y - the y XVector2 component
  XM2_INLINE XVector2(XReal _x, XReal _y) { set(_x,_y); }

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Initialisation funcs
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  initialises this XVector2 to 0,0,0
  XM2_INLINE void zero();

  /// \brief  sets the XVector2 values
  /// \param  x - the x component
  /// \param  y - the y component
  XM2_INLINE void set(XReal x, XReal y);
  
  /// \brief  sets the XVector2 values 
  /// \param  ptr - the data to read from
  XM2_INLINE void set(const XReal* ptr);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Logical Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second XVector2 to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XVector2& b, XReal eps=XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second XVector2 to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XVector2& b, XReal eps=XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math

  /// \brief  adds 2 XVector2's together
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2& a, const XVector2& b);

  /// \brief  subtracts 2 XVector2's 
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2& a, const XVector2& b);

  /// \brief  adds b to this
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2& b);

  /// \brief  subtracts b from this
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2& b);
  
  /// \brief  multiples 2 XVector2's together
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2& a, const XVector2& b);

  /// \brief  divides 2 XVector2's 
  /// \code
  ///         this = this / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2& a, const XVector2& b);

  /// \brief  multiplies this vector with b 
  /// \code
  ///         this = this * b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2& b);

  /// \brief  divides this vector by  b 
  /// \code
  ///         this = this / b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2 / XReal arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds 2 XVector2's together
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2& a, XReal b);

  /// \brief  subtracts 2 XVector2's 
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2& a, XReal b);
  
  /// \brief  divides 2 XVector2's 
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2& a, XReal b);

  /// \brief  subtracts 2 XVector2's 
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2& a, XReal b);

  /// \brief  adds 2 XVector2's together
  /// \code
  ///         this = this + b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(XReal b);

  /// \brief  subtracts 2 XVector2's 
  /// \code
  ///         this = this - b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(XReal b);
  
  /// \brief  divides 2 XVector2's 
  /// \code
  ///         this = this / b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(XReal b);

  /// \brief  subtracts 2 XVector2's 
  /// \code
  ///         this = this * b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(XReal b);

  //--------------------------------------------------------------------------------------------------------------------
  // Dot & Cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  performs a dot product between this and b
  /// \param  b - second vec2
  /// \return the dot product of a and b
  XM2_INLINE XReal dot(const XVector2& b) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Length & Normalisation routines
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  computes the length^2 of this XVector2
  /// \return the square length of this XVector2
  XM2_INLINE XReal lengthSquared() const;

  /// \brief  calculate the length of this XVector2 
  /// \return return the vector's length
  XM2_INLINE XReal length() const;

  /// \brief  normalises this XVector2 so it's of unit length
  XM2_INLINE void normalise();

  /// \brief  normalises this XVector2 so it's of unit length
  XM2_INLINE void normalise(const XVector2& q);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates (LERPS) between a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector2& a, const XVector2& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector2& a, const XVector2& b, const XVector2& c, const XVector2& d, XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2 Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates this XVector2 
  XM2_INLINE void negate();

  /// \brief  negates this XVector2 
  /// \param  v - the XVector2 to negate
  XM2_INLINE void negate(const XVector2& v);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2 stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  fileIO

  #if !XM2_NO_C_IO

  /// \brief  prints this XVector2 the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector2 to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector2 from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector2 from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector2 to stdout
  void print32() const;

  /// \brief  prints this XVector2 to stdout
  void print64() const;

  /// \brief  writes this XVector2 to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XVector2 to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XVector2 from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XVector2 from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // C++ operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE XVector2 operator-() const;

  XM2_INLINE bool operator == (const XVector2& c) const;
  XM2_INLINE bool operator != (const XVector2& c) const;

  XM2_INLINE const XVector2& operator = (const XVector2& c);

  XM2_INLINE const XVector2& operator *= (const XVector2& c);
  XM2_INLINE const XVector2& operator /= (const XVector2& c);
  XM2_INLINE const XVector2& operator += (const XVector2& c);
  XM2_INLINE const XVector2& operator -= (const XVector2& c);

  XM2_INLINE const XVector2& operator += (XReal c);
  XM2_INLINE const XVector2& operator -= (XReal c);
  XM2_INLINE const XVector2& operator /= (XReal c);
  XM2_INLINE const XVector2& operator *= (XReal c);

  XM2_INLINE XVector2 operator + (const XVector2& c) const;
  XM2_INLINE XVector2 operator - (const XVector2& c) const;
  XM2_INLINE XVector2 operator * (const XVector2& c) const;
  XM2_INLINE XVector2 operator / (const XVector2& c) const;

  XM2_INLINE XVector2 operator + (XReal c) const;
  XM2_INLINE XVector2 operator - (XReal c) const;
  XM2_INLINE XVector2 operator / (XReal c) const;
  XM2_INLINE XVector2 operator * (XReal c) const;

  friend XM2_INLINE XVector2 operator * (XReal f, const XVector2& p);

  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  //--------------------------------------------------------------------------------------------------------------------
  // Member variables
  //-------------------------------------------------------------------------------------------------------------------- 

  /// \name  data

  #ifdef DOXYGEN
    /// x component 
    XReal x; 
    /// y component 
    XReal y;
    /// data stored as an array
    XReal data[2];
  #else
  union 
  {
    struct 
    {
      XReal x; 
      XReal y;
    };
    XReal data[2];
    #ifdef XM2_VEC2_SSE
      __m128d sse;
    #endif
  };
  #endif

  /// \name  static array methods

  /// \brief  initialises this vector to 0,0,0
  static void arrayZero(XVector2* ptr, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2 array arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  normalises each XVector2 in the input array. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           product[i] = dot(a[i],b[i]);
  ///         }
  /// \endcode
  /// \param  product - the output array of normalised vectors
  /// \param  input - the input vector array
  /// \param  num - the number of elements in the arrays
  static void arrayNormalise(XVector2* data,const XVector2* input, int num);

  /// \brief  normalises each XVector2 in the data array. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           data[i] = normalise(data[i]);
  ///         }
  /// \endcode
  /// \param  data - the array to normalise
  /// \param  num - the number of elements in the array
  XM2_INLINE static void arrayNormalise(XVector2* data, int num);

  /// \brief  normalises each XVector2 in the input array. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           product[i] = dot(a[i],b[i]);
  ///         }
  /// \endcode
  /// \param  data - the output array of normalised vectors
  /// \param  input - the input vector array
  /// \param  num - the number of elements in the arrays
  static void arrayDot(XReal* product, const XVector2* a, const XVector2* b, int num);

  /// \brief  normalises each XVector2 in the input array. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           product[i] = dot(a[i],b);
  ///         }
  /// \endcode
  /// \param  data - the output array of normalised vectors
  /// \param  input - the input vector array
  /// \param  num - the number of elements in the arrays
  static void arrayDot(XReal* product, const XVector2* a, const XVector2& b, int num);

  /// \brief  linearly interpolates (LERPS) between two XVector2 arrays, a and b, and stores in this. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = lerp( a[i], b[i], t );
  ///         }
  /// \endcode
  /// \param  output - output XVector2 array
  /// \param  a - the first array
  /// \param  b - the 2nd array
  /// \param  t - the 0 to 1 interpolation control value
  /// \param  num - the number of elements in the array
  static void arrayLerp(XVector2* output, const XVector2* a, const XVector2* b, XReal t, int num);

  /// \brief  interpolates using a bezier function between a,b,c and d. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = bezier( a[i], b[i], c[i], d[i], t );
  ///         }
  /// \endcode
  /// \param  output - output XVector2 array
  /// \param  a - the first array
  /// \param  b - the 2nd array
  /// \param  c - the 3rd array
  /// \param  d - the 4th array
  /// \param  t - the 0 to 1 interpolation control value
  /// \param  num - the number of elements in the arrays
  static void arrayBezier(XVector2* output, const XVector2* a, const XVector2* b, const XVector2* c, const XVector2* d, XReal t, int num);

  /// \brief  negates an array of XVector2's. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = negate(output[i]);
  ///         }
  /// \endcode    
  /// \param  output - the array to negate
  /// \param  num - the number of elements in the array
  XM2_INLINE static void arrayNegate(XVector2* output, int num);

  /// \brief  negates an array of XVector2 's. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = negate(v[i]);
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  v - the array to negate
  /// \param  num - the number of elements in the array
  static void arrayNegate(XVector2* output, const XVector2* v, int num);

  /// \brief  adds 2 vec2 arrays together. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arrayAdd(XVector2* output, const XVector2* a,const XVector2* b, int num);

  /// \brief  adds a vec to a vec2 array together. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arrayAdd(XVector2* output, const XVector2* a, const XVector2& b, int num);

  /// \brief  adds a float to a vec2 array and stores the result in the output. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arrayAdd(XVector2* output, const XVector2* a, XReal b, int num);

  /// \brief  subtracts 2 XVector2 arrays and stores the result in the output. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arraySub(XVector2* output, const XVector2* a, const XVector2* b, int num);

  /// \brief  subtracts a XVector2 from an array of XVector2's, and stores the result in the output. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arraySub(XVector2* output, const XVector2* a, const XVector2& b, int num);

  /// \brief  subtracts a scalar value from each XVector2 in the array a, and stores the 
  ///         result in the output. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arraySub(XVector2* output, const XVector2* a, XReal b, int num);

  /// \brief  multiples 2 XVector2 arrays and stores the result in the output. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arrayMul(XVector2* output, const XVector2* a, const XVector2* b, int num);

  /// \brief  multiples a XVector2 and an array of XVector2's, and stores the result in the output. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arrayMul(XVector2* output, const XVector2* a, const XVector2& b, int num);

  /// \brief  multiples an XVector2 array with a scalar, and stores the result in the output. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arrayMul(XVector2* output, const XVector2* a, XReal b, int num);

  /// \brief  divides 2 XVector2 arrays and stores the result in the output. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arrayDiv(XVector2* output, const XVector2* a, const XVector2* b, int num);

  /// \brief  divides an array of XVector2's by b, and stores the result in the output. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arrayDiv(XVector2* output, const XVector2* a, const XVector2& b, int num);

  /// \brief  divides a XVector2 array by a scalar, and stores the result in the output. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of elements in the arrays
  static void arrayDiv(XVector2* output, const XVector2* a, XReal b, int num);

#ifdef XM2_VEC2_SSE
} XM2_ALIGN_SUFFIX(16);
#else
};
#endif

#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& os, const XVector2& data);
XM2EXPORT std::istream& operator>>(std::istream& is, XVector2& data);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/inl/Vector2.inl"
#ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
    #include "XM2/inl/vanilla/Vector2.inl"
  #else
    #include "XM2/inl/sse2/Vector2.inl"
  #endif
#else
  #include "XM2/inl/vanilla/Vector2.inl"
#endif
//----------------------------------------------------------------------------------------------------------------------
