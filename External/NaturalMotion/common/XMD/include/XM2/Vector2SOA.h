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

class XM2EXPORT XVector2;
class XM2EXPORT XVector4;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XMD::XVector2SOA
/// \brief   a class to hold 4 x XVector2's in structure or array format
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XVector2SOA
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA Constructors
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XVector2SOA();

  /// \brief  copy ctor
  /// \param  rhs - the XVector2SOA to copy
  XM2_INLINE XVector2SOA(const XVector2SOA& rhs);

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector2* input);

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector2s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector2* input) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA Initialisation funcs
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  initialises this all four XVector2 to 0,0
  XM2_INLINE void zero();

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA set/get funcs
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  get / set

  /// \brief  gets the first XVector2 
  /// \param  vec - the returned value
  XM2_INLINE void get0(XVector2& vec) const;

  /// \brief  gets the second XVector2 
  /// \param  vec - the returned value
  XM2_INLINE void get1(XVector2& vec) const;

  /// \brief  gets the third XVector2 
  /// \param  vec - the returned value
  XM2_INLINE void get2(XVector2& vec) const;

  /// \brief  gets the fourth XVector2 
  /// \param  vec - the returned value
  XM2_INLINE void get3(XVector2& vec) const;

  /// \brief  gets the first XVector2 
  /// \return the returned value
  XM2_INLINE XVector2 get0() const;

  /// \brief  gets the second XVector2 
  /// \return the returned value
  XM2_INLINE XVector2 get1() const;

  /// \brief  gets the third XVector2 
  /// \return the returned value
  XM2_INLINE XVector2 get2() const;

  /// \brief  gets the fourth XVector2 
  /// \return the returned value
  XM2_INLINE XVector2 get3() const;

  /// \brief  sets the first XVector2 
  /// \param  x - the x component
  /// \param  y - the y component
  XM2_INLINE void set0(XReal x, XReal y);

  /// \brief  sets the first XVector2 
  /// \param  ptr - the data to read from
  XM2_INLINE void set0(const XVector2& ptr);

  /// \brief  sets the first XVector2 
  /// \param  ptr - the data to read from
  XM2_INLINE void set0(const XReal* ptr);

  /// \brief  sets the second XVector2 
  /// \param  x - the x component
  /// \param  y - the y component
  XM2_INLINE void set1(XReal x, XReal y);

  /// \brief  sets the second XVector2 
  /// \param  ptr - the data to read from
  ///
  XM2_INLINE void set1(const XVector2& ptr);

  /// \brief  sets the second XVector2 
  /// \param  ptr - the data to read from
  XM2_INLINE void set1(const XReal* ptr);

  /// \brief  sets the third XVector2 
  /// \param  x - the x component
  /// \param  y - the y component
  XM2_INLINE void set2(XReal x, XReal y);

  /// \brief  sets the third XVector2 
  /// \param  ptr - the data to read from
  XM2_INLINE void set2(const XVector2& ptr);

  /// \brief  sets the third XVector2 
  /// \param  ptr - the data to read from
  XM2_INLINE void set2(const XReal* ptr);

  /// \brief  sets the third XVector2 
  /// \param  x - the x component
  /// \param  y - the y component
  XM2_INLINE void set3(XReal x, XReal y);

  /// \brief  sets the third XVector2 
  /// \param  ptr - the data to read from
  XM2_INLINE void set3(const XVector2& ptr);

  /// \brief  sets the third XVector2 
  /// \param  ptr - the data to read from
  XM2_INLINE void set3(const XReal* ptr);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA Logical Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second vector to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XVector2SOA& b, XReal eps = XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second vector to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XVector2SOA& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA / XVector2SOA Arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math

  /// \brief  adds 2 XVector2SOA's together
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2SOA& a, const XVector2SOA& b);

  /// \brief  subtracts 2 XVector2SOA's 
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \note   does
  XM2_INLINE void sub(const XVector2SOA& a, const XVector2SOA& b);

  /// \brief  adds b to this
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2SOA& b);

  /// \brief  subtracts b from this
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2SOA& b);
  
  /// \brief  adds 2 XVector2SOA's together
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2SOA& a, const XVector2SOA& b);

  /// \brief  subtracts 2 XVector2SOA's 
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2SOA& a, const XVector2SOA& b);

  /// \brief  adds b to this
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2SOA& b);

  /// \brief  subtracts b from this
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2SOA& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA / XVector2 Arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds a XVector2 to all four XVector2's and stores the result in this
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2SOA& a, const XVector2&b);

  /// \brief  subtracts a XVector2 from all four XVector2's and stores the result in this
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \note   does
  XM2_INLINE void sub(const XVector2SOA& a, const XVector2& b);

  /// \brief  divides each XVector2 by an XVector2 and stores the result in this 
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2SOA& a, const XVector2& b);

  /// \brief  multiplies each vector2 in a with b and stores the result in this
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \note   does
  XM2_INLINE void mul(const XVector2SOA& a, const XVector2& b);

  /// \brief  adds b to this
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2& b);

  /// \brief  subtracts b from this 
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2& b);
  
  /// \brief  divides this by b
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2& b);

  /// \brief  multiplies this by b
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA / XReal Arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds a scalar to all components of a and stores the result in this
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2SOA& a, XReal b);

  /// \brief  subtracts a scalar from all components of a and stores the result in this
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2SOA& a, XReal b);

  /// \brief  divides all components of a with a scalar and stores the result in this
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2SOA& a, XReal b);

  /// \brief  multiplies a by a scalar and stores the result in this
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2SOA& a, XReal b);

  /// \brief  adds a scalar value to all components of this XVector2SOA
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input scalar
  XM2_INLINE void add(XReal b);

  /// \brief  subtracts a scalar value from all components of this XVector2SOA
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input scalar
  XM2_INLINE void sub(XReal b);

  /// \brief  divides all components of this XVector2SOA with a scalar value 
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input scalar
  XM2_INLINE void div(XReal b);

  /// \brief  multiplies all components of this XVector2SOA with a scalar value 
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input scalar
  XM2_INLINE void mul(XReal b);

  //--------------------------------------------------------------------------------------------------------------------
  // Dot & Cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  performs a dot product between this and b
  /// \param  dp - the returned dot products 
  /// \param  b - second XVector2SOA
  XM2_INLINE void dot(XVector4& dp, const XVector2SOA& b) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA Length & Normalisation routines
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  computes the length^2 of this XVector2
  /// \param  result - the square length of the 4 vectors
  XM2_INLINE void lengthSquared(XVector4& result) const;

  /// \brief  calculate the length of each XVector2
  /// \param  result - the length of the 4 vectors
  XM2_INLINE void length(XVector4& result) const;

  /// \brief  normalises all of the vectors in this class
  XM2_INLINE void normalise();

  /// \brief  normalises all of the vectors in q and stores in this
  /// \param  q - the vectors to normalise
  XM2_INLINE void normalise(const XVector2SOA& q);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA interpolation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates (LERPS) between vector a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector2SOA& a,  const XVector2SOA& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector2SOA& a, const XVector2SOA& b, const XVector2SOA& c, const XVector2SOA& d, XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates the 4 XVector2's  
  XM2_INLINE void negate();

  /// \brief  negates the 4 XVector2's and stores the result in this
  /// \param  v - the vector data to negate
  XM2_INLINE void negate(const XVector2SOA& v);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XVector2SOA  the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector2SOA  to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector2SOA  from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector2SOA  from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector2SOA  to stdout
  void print32() const;

  /// \brief  prints this XVector2SOA  to stdout
  void print64() const;

  /// \brief  writes this XVector2SOA  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XVector2SOA  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XVector2SOA  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XVector2SOA  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // C++ operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE XVector2SOA operator-() const;

  XM2_INLINE bool operator == (const XVector2SOA& c) const;
  XM2_INLINE bool operator != (const XVector2SOA& c) const;

  XM2_INLINE const XVector2SOA& operator = (const XVector2SOA& c);

  XM2_INLINE const XVector2SOA& operator *= (const XVector2SOA& c);
  XM2_INLINE const XVector2SOA& operator /= (const XVector2SOA& c);
  XM2_INLINE const XVector2SOA& operator += (const XVector2SOA& c);
  XM2_INLINE const XVector2SOA& operator -= (const XVector2SOA& c);

  XM2_INLINE const XVector2SOA& operator *= (const XVector2& c);
  XM2_INLINE const XVector2SOA& operator /= (const XVector2& c);
  XM2_INLINE const XVector2SOA& operator += (const XVector2& c);
  XM2_INLINE const XVector2SOA& operator -= (const XVector2& c);

  XM2_INLINE const XVector2SOA& operator += (XReal c);
  XM2_INLINE const XVector2SOA& operator -= (XReal c);
  XM2_INLINE const XVector2SOA& operator /= (XReal c);
  XM2_INLINE const XVector2SOA& operator *= (XReal c);


  friend XM2_INLINE XVector2 operator * (XReal f, const XVector2& p);

  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  /// \name  data

  //--------------------------------------------------------------------------------------------------------------------
  // Member variables
  //--------------------------------------------------------------------------------------------------------------------
  #ifdef DOXYGEN
    /// x components of vectors
    XReal x[4]; 
    /// y components of vectors 
    XReal y[4];
    /// data stored as an array
    XReal data[8];
  #else
  union 
  {
    struct 
    {
      XReal x[4]; 
      XReal y[4];
    };
    XReal data[8];
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
      struct 
      {
        __m128 _x; 
        __m128 _y; 
      };
      #else
      struct 
      {
        __m128d _x0; 
        __m128d _x1; 
        __m128d _y0; 
        __m128d _y1; 
      };
      #endif
    #endif
  };
  #endif

  /// \name  static array methods

  /// \brief  sets the array of vector data to zero
  /// \param  ptr - the array to zero
  /// \param  num - number of XVector2SOAs in the array
  static void arrayZero(XVector2SOA* ptr, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA array arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds 2 XVector2SOA arrays together
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayAdd(XVector2SOA* output, const XVector2SOA* a, const XVector2SOA* b, int num);

  /// \brief  adds an XVector2SOA to an XVector2SOA array 
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayAdd(XVector2SOA* output, const XVector2SOA* a, const XVector2SOA& b, int num);

  /// \brief  adds an XVector2 to an XVector2SOA array 
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayAdd(XVector2SOA* output, const XVector2SOA* a, const XVector2& b, int num);

  /// \brief  adds a float to an XVector2SOA array 
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayAdd(XVector2SOA* output, const XVector2SOA* a, XReal b, int num);

  /// \brief  subtracts an XVector2SOA array from an XVector2SOA array 
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arraySub(XVector2SOA* output, const XVector2SOA* a, const XVector2SOA* b, int num);

  /// \brief  subtracts an XVector2SOA from an XVector2SOA array 
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arraySub(XVector2SOA* output, const XVector2SOA* a, const XVector2SOA& b, int num);

  /// \brief  subtracts an XVector2 from an XVector2SOA array 
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arraySub(XVector2SOA* output, const XVector2SOA* a, const XVector2& b, int num);

  /// \brief  subtracts a float from an XVector2SOA array 
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arraySub(XVector2SOA* output, const XVector2SOA* a, XReal b, int num);

  /// \brief  multiplies an array of XVector2SOAs with an array of XVector2SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayMul(XVector2SOA* output, const XVector2SOA* a, const XVector2SOA* b, int num);

  /// \brief  multiplies an array of XVector2SOAs with an XVector2SOA. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayMul(XVector2SOA* output, const XVector2SOA* a, const XVector2SOA& b, int num);

  /// \brief  multiplies an array of XVector2SOAs with an XVector2. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayMul(XVector2SOA* output, const XVector2SOA* a, const XVector2& b, int num);

  /// \brief  multiplies an array of XVector2SOAs with a float. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayMul(XVector2SOA* output, const XVector2SOA* a,  XReal b, int num);

  /// \brief  divides an array of XVector2SOAs by an array of XVector2SOAs. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayDiv(XVector2SOA* output, const XVector2SOA* a, const XVector2SOA* b, int num);

  /// \brief  divides an array of XVector2SOAs by an XVector2SOA. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayDiv(XVector2SOA* output, const XVector2SOA* a, const XVector2SOA& b, int num);

  /// \brief  divides an array of XVector2SOAs by an XVector2SOA. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayDiv(XVector2SOA* output, const XVector2SOA* a, const XVector2& b, int num);

  /// \brief  divides an array of XVector2SOAs by a float. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  /// \param  num - number of XVector2SOAs in the array
  static void arrayDiv(XVector2SOA* output, const XVector2SOA* a, XReal b, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA array dot
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  performs a dot product
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise(output[i]);
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  num - number of XVector2SOAs in the array
  static void arrayDot(XVector4* output, const XVector2SOA* a, const XVector2SOA* b, int num);

  /// \brief  performs a dot product
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise(output[i]);
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  num - number of XVector2SOAs in the array
  static void arrayDot(XVector4* output, const XVector2SOA* a, const XVector2SOA& b, int num);

  /// \brief  performs a dot product
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise(output[i]);
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  num - number of XVector2SOAs in the array
  static void arrayDot(XVector4* output, const XVector2SOA* a, const XVector2& b, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA array normalisation
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  negates an array of XVector2SOA's. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = -output[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  num - number of XVector2SOAs in the array
  static void arrayNegate(XVector2SOA* output, int num);

  /// \brief  negates an array of XVector2SOA's. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = -input[i];
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  input - the array to negate
  /// \param  num - number of XVector2SOAs in the array
  static void arrayNegate(XVector2SOA* output, const XVector2SOA* input, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA array normalisation
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  normalises an array of XVector2SOA's. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise(output[i]);
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  num - number of XVector2SOAs in the array
  static void arrayNormalise(XVector2SOA* output, int num);

  /// \brief  normalises an array of XVector2SOA's. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise(output[i]);
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  input - the array to normalise
  /// \param  num - number of XVector2SOAs in the array
  static void arrayNormalise(XVector2SOA* output, const XVector2SOA* input, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOA array interpolation
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  linearly interpolates (LERPS) between the two XVector2SOA arrays a and b and 
  ///         stores the result in this. i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = lerp(a[i],b[i],t);
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  /// \param  num - number of XVector2SOAs in the array
  static void arrayLerp(XVector2SOA* output,
                        const XVector2SOA* a, const XVector2SOA* b,
                        XReal t, int num);

  /// \brief  interpolates using a bezier function between a,b,c and d.  i.e.
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = bezier(a[i],b[i],c[i],d[i],t);
  ///         }
  /// \endcode
  /// \param  output - the output array
  /// \param  a - the first array
  /// \param  b - the 2nd array
  /// \param  c - the 3rd array
  /// \param  d - the 4th array
  /// \param  t - the 0 to 1 interpolation control value
  /// \param  num - number of XVector2SOAs in the arrays
  static void arrayBezier(XVector2SOA* output,
                          const XVector2SOA* a, const XVector2SOA* b,
                          const XVector2SOA* c, const XVector2SOA* d,
                          XReal t, int num);

#ifdef XM2_VEC2_SSE
} XM2_ALIGN_SUFFIX(16);
#else
};
#endif
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XVector2SOA& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XVector2SOA& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector2.h"
#include "XM2/Vector4.h"
#include "XM2/inl/Vector2SOA.inl"
#ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
    #include "XM2/inl/sse/Vector2SOA.inl"
  #else
    #include "XM2/inl/sse2/Vector2SOA.inl"
  #endif
#else
  #include "XM2/inl/vanilla/Vector2SOA.inl"
#endif
//----------------------------------------------------------------------------------------------------------------------
