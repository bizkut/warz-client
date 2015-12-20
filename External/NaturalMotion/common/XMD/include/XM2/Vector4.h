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

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XVector4
/// \brief   A 4D vector type
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XVector4
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4 Constructors
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XVector4();

  /// \brief  copy ctor
  /// \param  v - the vector to copy
  XM2_INLINE XVector4(const XVector4& v);

  /// \brief  ctor
  /// \param  x_ - the x vector component
  /// \param  y_ - the y vector component
  /// \param  z_ - the z vector component
  /// \param  w_ - the w vector component
  XM2_INLINE XVector4(XReal x_, XReal y_, XReal z_, XReal w_ = 0.0f);

  #ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
  XM2_INLINE XVector4(const __m128 a) : sse(a) {}
  #endif
  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4 Initialisation funcs
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  initialises the vector a to 0,0,0
  XM2_INLINE void zero();
  
  /// \brief  sets the elements of the vector
  /// \param  x_ - the x vector component
  /// \param  y_ - the y vector component
  /// \param  z_ - the z vector component
  /// \param  w_ - the w vector component
  XM2_INLINE void set(XReal x_, XReal y_, XReal z_, XReal w_ = 0.0f);
  
  /// \brief  sets the elements of the vector
  /// \param  data - the vector data
  XM2_INLINE void set(const XReal* data);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4 Logical Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second vector to test
  /// \param  eps - floating point error tolerance
  /// \return true if a and b are the same, false otherwise
  XM2_INLINE bool equal(const XVector4& b, XReal eps = XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second vector to test
  /// \param  eps - floating point error tolerance
  /// \return true if a and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XVector4& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4 Arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math

  /// \brief  adds a XVector4 and a XVector4 together,, and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4& a, const XVector4& b);

  /// \brief  subtracts a XVector4 from a XVector4 together,, and stores the result in this.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4& a, const XVector4& b);

  /// \brief  multiplies an XVector4 with an XVector4, and stores the result in this.
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4& a, const XVector4& b);

  /// \brief  divides an XVector4 with an XVector4, and stores the result in this.
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4& a, const XVector4& b);

  /// \brief  adds a XVector4 to this.
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4& b);

  /// \brief  subtracts a XVector4 from this.
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4& b);

  /// \brief  multiplies this with an XVector4.
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4& b);

  /// \brief  divides this with an XVector4.
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4 & XVector3 Arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds a XVector4 and a XVector3 together,, and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4& a, const XVector3& b);

  /// \brief  subtracts a XVector3 from a XVector4 together,, and stores the result in this.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4& a, const XVector3& b);

  /// \brief  multiplies an XVector4 with an XVector3, and stores the result in this.
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4& a, const XVector3& b);

  /// \brief  divides an XVector4 with an XVector3, and stores the result in this.
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4& a, const XVector3& b);

  /// \brief  adds a XVector3 to this.
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3& b);

  /// \brief  multiplies this with an XVector3.
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3& b);

  /// \brief  adds 2 vec4's together
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3& b);

  /// \brief  divides this with an XVector3.
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4 & Float arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds a float and a XVector4, and stores the result in this.
  /// \code
  ///         output = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  ///
  XM2_INLINE void add(const XVector4& a, XReal b);

  /// \brief  subtracts a float from a XVector4, and stores the result in this.
  /// \code
  ///         output = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4& a, XReal b);

  /// \brief  multiplies an XVector4 with a float, and stores the result in this.
  /// \code
  ///         output = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4& a, XReal b);

  /// \brief  divides an XVector4 with a float, and stores the result in this.
  /// \code
  ///         output = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4& a, XReal b);

  /// \brief  adds a float to this.
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(XReal b);

  /// \brief  subtracts a float from this.
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(XReal b);

  /// \brief  multiplies this with a float.
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(XReal b);

  /// \brief  divides this with a float.
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(XReal b);

  //--------------------------------------------------------------------------------------------------------------------
  // Dot & Cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  performs a dot product between 2 XVector4's
  /// \param  b - second XVector4
  /// \return the dot product of a and b
  XM2_INLINE XReal dot(const XVector4& b) const;

  /// \brief  performs a cross product between 2 XVector4's
  /// \param  a - first XVector4
  /// \param  b - second XVector4
  XM2_INLINE void cross(const XVector4& a, const XVector4& b);
  
  //--------------------------------------------------------------------------------------------------------------------
  // XVector4 Length & Normalisation routines
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  computes the length^2 of the XVector4
  /// \return the square length of the XVector4
  XM2_INLINE XReal lengthSquared() const;

  /// \brief  calculate the length of the XVector4 
  /// \return return the XVector4's length
  XM2_INLINE XReal length() const;

  /// \brief  normalises this XVector4
  XM2_INLINE void normalise();

  /// \brief  normalises this XVector4
  XM2_INLINE void normalise(const XVector4& v);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4 Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates (LERPS) between XVector4s a and b and stores in output
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector4& a, const XVector4& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector4& a, const XVector4& b, const XVector4& c, const XVector4& d, XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4 Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates this XVector4
  XM2_INLINE void negate();

  /// \brief  negates the XVector4
  /// \param  a - the XVector4 to negate
  XM2_INLINE void negate(const XVector4& a);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4 stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XVector4  the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector4  to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector4  from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector4  from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector4  to stdout
  void print32() const;

  /// \brief  prints this XVector4  to stdout
  void print64() const;

  /// \brief  writes this XVector4  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes=false) const;

  /// \brief  writes this XVector4  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes=false) const;

  /// \brief  reads a XVector4  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes=false);

  /// \brief  reads a XVector4  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes=false); 

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // C++ operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE bool operator == (const XVector4& c) const;
  XM2_INLINE bool operator != (const XVector4& c) const;

  XM2_INLINE const XVector4& operator = (const XVector4& c);
  XM2_INLINE const XVector4& operator = (const XVector3& c);

  XM2_INLINE const XVector4& operator += (const XVector4& c);
  XM2_INLINE const XVector4& operator -= (const XVector4& c);
  XM2_INLINE const XVector4& operator *= (const XVector4& c);
  XM2_INLINE const XVector4& operator /= (const XVector4& c);

  XM2_INLINE const XVector4& operator += (const XVector3& c);
  XM2_INLINE const XVector4& operator -= (const XVector3& c);
  XM2_INLINE const XVector4& operator *= (const XVector3& c);
  XM2_INLINE const XVector4& operator /= (const XVector3& c);

  XM2_INLINE const XVector4& operator += (XReal c);
  XM2_INLINE const XVector4& operator -= (XReal c);
  XM2_INLINE const XVector4& operator /= (XReal c);
  XM2_INLINE const XVector4& operator *= (XReal c);

  XM2_INLINE XVector4 operator + (const XVector4& c) const;
  XM2_INLINE XVector4 operator - (const XVector4& c) const;
  XM2_INLINE XVector4 operator * (const XVector4& c) const; ///< this is not the dot product!
  XM2_INLINE XVector4 operator / (const XVector4& c) const;

  XM2_INLINE XVector4 operator + (const XVector3& c) const;
  XM2_INLINE XVector4 operator - (const XVector3& c) const;
  XM2_INLINE XVector4 operator * (const XVector3& c) const; ///< this is not the dot product!
  XM2_INLINE XVector4 operator / (const XVector3& c) const;

  XM2_INLINE XVector4 operator + (XReal c) const;
  XM2_INLINE XVector4 operator - (XReal c) const;
  XM2_INLINE XVector4 operator / (XReal c) const;
  XM2_INLINE XVector4 operator * (XReal c) const;

  friend XM2_INLINE XVector4 operator * (XReal f, const XVector4& p);

  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  /// \name  data

  //--------------------------------------------------------------------------------------------------------------------
  // Member variables
  //--------------------------------------------------------------------------------------------------------------------
  union 
  {
    struct 
    {
      /// x component 
      XReal x; 
      /// y component 
      XReal y;
      /// z component 
      XReal z;
      /// w component 
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

  /// \name  static array methods

  /// \brief  initialises an array of XVector4s to 0,0,0
  /// \param  ptr - the array of XVector4s to zero
  /// \param  num - the number of elements in the arrays
  static void arrayZero(XVector4* ptr, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds an array of XVector4s to an array of XVector4s and stores the result
  ///         in the output array.
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
  static void arrayAdd(XVector4* output, const XVector4* a, const XVector4* b, int num);

  /// \brief  adds an array of XVector4s to an XVector4 and stores the result
  ///         in the output array.
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
  static void arrayAdd(XVector4* output, const XVector4* a, const XVector4& b, int num);

  /// \brief  adds an array of XVector4s to an array of XVector3s and stores the result
  ///         in the output array.
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
  static void arrayAdd(XVector4* output, const XVector4* a, const XVector3* b, int num);

  /// \brief  adds an array of XVector4s to an XVector3 and stores the result
  ///         in the output array.
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
  static void arrayAdd(XVector4* output, const XVector4* a, const XVector3& b, int num);

  /// \brief  adds an array of XVector4s to a float and stores the result
  ///         in the output array.
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
  static void arrayAdd(XVector4* output, const XVector4* a, XReal b, int num);

  /// \brief  subtracts an array of XVector4s from an array of XVector4s and stores the result
  ///         in the output array.
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
  static void arraySub(XVector4* output, const XVector4* a, const XVector4* b, int num);

  /// \brief  subtracts an XVector4 from an array of XVector4s and stores the result
  ///         in the output array.
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
  static void arraySub(XVector4* output, const XVector4* a, const XVector4& b, int num);

  /// \brief  subtracts an array of XVector3s from an array of XVector4s and stores the result
  ///         in the output array.
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
  static void arraySub(XVector4* output, const XVector4* a, const XVector3* b, int num);

  /// \brief  subtracts an XVector3 from an array of XVector4s and stores the result
  ///         in the output array.
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
  static void arraySub(XVector4* output, const XVector4* a, const XVector3& b, int num);

  /// \brief  subtracts a float from an array of XVector4s and stores the result
  ///         in the output array.
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
  static void arraySub(XVector4* output, const XVector4* a, XReal b, int num);

  /// \brief  multiplies an array of XVector4s with an array of XVector4s and stores the result
  ///         in the output array.
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
  static void arrayMul(XVector4* output, const XVector4* a, const XVector4* b, int num);

  /// \brief  multiplies an array of XVector4s with an XVector4 and stores the result
  ///         in the output array.
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
  static void arrayMul(XVector4* output, const XVector4* a, const XVector4& b, int num);

  /// \brief  multiplies an array of XVector4s with an array of XVector3s and stores the result
  ///         in the output array.
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
  static void arrayMul(XVector4* output, const XVector4* a, const XVector3* b, int num);

  /// \brief  multiplies an array of XVector4s with an XVector3 and stores the result
  ///         in the output array.
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
  static void arrayMul(XVector4* output, const XVector4* a, const XVector3& b, int num);

  /// \brief  multiplies an array of XVector4s with a float and stores the result
  ///         in the output array.
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
  static void arrayMul(XVector4* output, const XVector4* a, XReal b, int num);

  /// \brief  divides an array of XVector4s by an array of XVector4s and stores the result
  ///         in the output array.
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
  static void arrayDiv(XVector4* output, const XVector4* a, const XVector4* b, int num);

  /// \brief  divides an array of XVector4s by an XVector4 and stores the result
  ///         in the output array.
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
  static void arrayDiv(XVector4* output, const XVector4* a, const XVector4& b, int num);

  /// \brief  divides an array of XVector4s by an array of XVector3s and stores the result
  ///         in the output array.
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
  static void arrayDiv(XVector4* output, const XVector4* a, const XVector3* b, int num);

  /// \brief  divides an array of XVector4s by an XVector3 and stores the result
  ///         in the output array.
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
  static void arrayDiv(XVector4* output, const XVector4* a, const XVector3& b, int num);

  /// \brief  divides an array of XVector4s by a float and stores the result
  ///         in the output array.
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
  static void arrayDiv(XVector4* output, const XVector4* a, XReal b, int num);
  
  //--------------------------------------------------------------------------------------------------------------------
  // dot & cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  performs the dot product on an array of XVector4
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
  static void arrayDot(XReal* product, const XVector4* a, const XVector4* b, int num);

  /// \brief  performs the dot product on an array of XVector4
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
  static void arrayDot(XReal* product, const XVector4* a, const XVector4& b, int num);

  /// \brief  performs the cross product on an array of XVector4
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
  static void arrayCross(XVector4* output, const XVector4* a, const XVector4* b, int num);

  /// \brief  performs the cross product on an array of XVector3
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
  static void arrayCross(XVector4* output, const XVector4* a, const XVector4& b, int num);

  /// \brief  normalises an array of XVector3
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise( a[i] );
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static XM2_INLINE void arrayNormalise(XVector4* output, int num);

  /// \brief  normalises an array of XVector3
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise( a[i] );
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static void arrayNormalise(XVector4* output, const XVector4* input, int num);

  /// \brief  normalises an array of XVector3
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise( a[i] );
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static XM2_INLINE void arrayNegate(XVector4* output, int num);

  /// \brief  normalises an array of XVector3
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise( a[i] );
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static void arrayNegate(XVector4* output, const XVector4* input, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOA Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates (LERPS) between the XVector3 arrays a and b and 
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
  static void arrayLerp(XVector4* output,
                        const XVector4* a, const XVector4* b,
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
  static void arrayBezier(XVector4* output,
                          const XVector4* a, const XVector4* b,
                          const XVector4* c, const XVector4* d,
                          XReal t, int num);

}
XM2_ALIGN_SUFFIX(16);
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XVector4& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XVector4& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector3.h"
#include "XM2/inl/Vector4.inl"
#ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
    #include "XM2/inl/sse/Vector4.inl"
  #else
    #include "XM2/inl/sse2/Vector4.inl"
  #endif
#else
  #include "XM2/inl/vanilla/Vector4.inl"
#endif
//----------------------------------------------------------------------------------------------------------------------
