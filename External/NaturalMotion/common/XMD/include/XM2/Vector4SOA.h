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
class XM2EXPORT XVector3;
class XM2EXPORT XVector4;
class XM2EXPORT XVector3SOA;
class XM2EXPORT XVector3SOA;
class XM2EXPORT XVector3Packed;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XVector4SOA
/// \brief   defines a class to store 4 x XVector4's in a structure of array format
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XVector4SOA
{
public:

  XM2_INLINE XVector4SOA();
  XM2_INLINE XVector4SOA(const XVector3& a);
  XM2_INLINE XVector4SOA(const XVector4& a);
  XM2_INLINE XVector4SOA(const XVector3Packed& a);
  XM2_INLINE XVector4SOA(const XVector3SOA& a);
  XM2_INLINE XVector4SOA(const XVector4SOA& a);

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
  XM2_INLINE void streamIn(const XVector3SOA& input);

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
  XM2_INLINE void streamOut(XVector3SOA& output) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector4s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector3Packed* output) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA Initialisation funcs
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisaation

  /// \brief  initialises the XVector4SOA to all zeros
  XM2_INLINE void zero();

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA set/get XVector4 funcs
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  get / set

  /// \brief  sets the first XVector4
  /// \param  x_ - the x component of the XVector4
  /// \param  y_ - the y component of the XVector4
  /// \param  z_ - the z component of the XVector4
  /// \param  w_ - the w component of the XVector4
  XM2_INLINE void set0(const XReal x_, XReal y_, XReal z_, XReal w_);

  /// \brief  sets the second XVector4
  /// \param  x_ - the x component of the XVector4
  /// \param  y_ - the y component of the XVector4
  /// \param  z_ - the z component of the XVector4
  /// \param  w_ - the w component of the XVector4
  XM2_INLINE void set1(const XReal x_, XReal y_, XReal z_, XReal w_);

  /// \brief  sets the third XVector4
  /// \param  x_ - the x component of the XVector4
  /// \param  y_ - the y component of the XVector4
  /// \param  z_ - the z component of the XVector4
  /// \param  w_ - the w component of the XVector4
  XM2_INLINE void set2(const XReal x_, XReal y_, XReal z_, XReal w_);

  /// \brief  sets the fourth XVector4
  /// \param  x_ - the x component of the XVector4
  /// \param  y_ - the y component of the XVector4
  /// \param  z_ - the z component of the XVector4
  /// \param  w_ - the w component of the XVector4
  XM2_INLINE void set3(const XReal x_, XReal y_, XReal z_, XReal w_);

  /// \brief  sets the first XVector4
  /// \param  vec - the new value for the XVector4
  XM2_INLINE void set0(const XVector4& vec);

  /// \brief  sets the second XVector4
  /// \param  vec - the new value for the XVector4
  XM2_INLINE void set1(const XVector4& vec);

  /// \brief  sets the third XVector4
  /// \param  vec - the new value for the XVector4
  XM2_INLINE void set2(const XVector4& vec);

  /// \brief  sets the fourth XVector4
  /// \param  vec - the new value for the XVector4
  XM2_INLINE void set3(const XVector4& vec);

  /// \brief  gets the first XVector4
  /// \param  vec - the returned XVector4 value
  XM2_INLINE void get0(XVector4& vec) const;

  /// \brief  gets the second XVector4
  /// \param  vec - the returned XVector4 value
  XM2_INLINE void get1(XVector4& vec) const;

  /// \brief  gets the third XVector4
  /// \param  vec - the returned XVector4 value
  XM2_INLINE void get2(XVector4& vec) const;

  /// \brief  gets the fourth XVector4
  /// \param  vec - the returned XVector4 value 
  XM2_INLINE void get3(XVector4& vec) const;

  /// \brief  gets the first XVector4
  /// \return the XVector4 value
  XM2_INLINE XVector4 get0() const;

  /// \brief  gets the second XVector4
  /// \return the XVector4 value
  XM2_INLINE XVector4 get1() const;

  /// \brief  gets the third XVector4
  /// \return the XVector4 value
  XM2_INLINE XVector4 get2() const;

  /// \brief  gets the fourth XVector4
  /// \return the XVector4 value
  XM2_INLINE XVector4 get3() const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA Logical Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second vector to test
  /// \param  eps - tfloating point error tolerance
  /// \return true if a and b are the same, false otherwise
  XM2_INLINE bool equal(const XVector4SOA& b, XReal eps=XM2_FLOAT_ZERO) const;

  /// \brief  this != b
  /// \param  b - second vector to test
  /// \param  eps - tfloating point error tolerance
  /// \return true if a and b are the same, false otherwise
  XM2_INLINE bool notEqual(const XVector4SOA& b, XReal eps=XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA / XVector4SOA Arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math

  /// \brief  adds an XVector4SOA to an XVector4SOA and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOA& a, const XVector4SOA& b);

  /// \brief  subtracts an XVector4SOA from an XVector4SOA and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode 
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOA& a, const XVector4SOA& b);

  /// \brief  multiplies an XVector4SOA with an XVector4SOA and stores the result in this.
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOA& a, const XVector4SOA& b);

  /// \brief  divides an XVector4SOA by an XVector4SOA and stores the result in this.
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOA& a, const XVector4SOA& b);

  /// \brief  adds an XVector4SOA to this.
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOA& b);

  /// \brief  subtracts an XVector4SOA from this.
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOA& b);

  /// \brief  multiplies an XVector4SOA with this.
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOA& b);

  /// \brief  divides this by an XVector4SOA.
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOA& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA / XVector4 Arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds an XVector4SOA to an XVector4 and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOA& a, const XVector4& b);

  /// \brief  subtracts an XVector4 from an XVector4SOA and stores the result in this.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOA& a, const XVector4& b);

  /// \brief  multiplies an XVector4 with an XVector4SOA and stores the result in this.
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOA& a, const XVector4& b);

  /// \brief  divides an XVector4SOA by an XVector4 and stores the result in this.
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOA& a, const XVector4& b);

  /// \brief  adds an XVector4 to this.
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4& b);

  /// \brief  subtracts an XVector4 from this.
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4& b);

  /// \brief  multiplies an XVector4 with this.
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4& b);

  /// \brief  divides this by an XVector4.
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA / Float arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds an XVector4SOA to an XReal and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOA& a, XReal b);

  /// \brief  subtracts an XReal from an XVector4SOA and stores the result in this.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOA& a, XReal b);

  /// \brief  divides an XVector4SOA by an XReal and stores the result in this.
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOA& a, XReal b);

  /// \brief  multiplies an XReal with an XVector4SOA and stores the result in this.
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOA& a, XReal b);

  /// \brief  adds an XReal to this.
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(XReal b);

  /// \brief  subtracts an XReal from this.
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(XReal b);

  /// \brief  divides this by an XReal
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(XReal b);

  /// \brief  multiplies an XReal with this.
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(XReal b);

  //--------------------------------------------------------------------------------------------------------------------
  // Dot & Cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  performs a dot product between two XVector4SOA's
  /// \param  _dot - the four returned dot product values
  /// \param  b - second XVector4SOA
  XM2_INLINE void dot(XVector4& _dot, const XVector4SOA& b) const;
  
  /// \brief  performs a cross product between two XVector4SOA's and stores the result in this
  /// \param  a - first XVector4SOA
  /// \param  b - second XVector4SOA
  XM2_INLINE void cross(const XVector4SOA& a, const XVector4SOA& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA Length & Normalisation routines
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  computes the length^2 of the XVector4SOA
  /// \param  _length_sqr - the returned length
  XM2_INLINE void lengthSquared(XVector4& _length_sqr) const;

  /// \brief  calculates the length of the XVector4SOA
  /// \param  _length - the returned length
  XM2_INLINE void length(XVector4& _length) const;

  /// \brief  normalises the XVector4SOA 
  XM2_INLINE void normalise();

  /// \brief  stores normalised versions of a in this
  /// \param  a - the XVector4SOA data to normalise
  XM2_INLINE void normalise(const XVector4SOA& a);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA interpolation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates (LERPS) between XVector4SOA a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector4SOA& a, const XVector4SOA& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector4SOA& a, const XVector4SOA& b, const XVector4SOA& c, const XVector4SOA& d, XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates the XVector4SOA's
  XM2_INLINE void negate();

  /// \brief  negates the XVector4SOA data a and stores the result in this
  /// \param  a - the XVector4SOA data to normalise
  XM2_INLINE void negate(const XVector4SOA& a);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XVector4SOA the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector4SOA to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector4SOA from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector4SOA from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector4SOA to stdout
  void print32() const;

  /// \brief  prints this XVector4SOA to stdout
  void print64() const;

  /// \brief  writes this XVector4SOA to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XVector4SOA to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XVector4SOA from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XVector4SOA from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false); 

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4SOA Operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE bool operator == (const XVector4SOA& c) const;
  XM2_INLINE bool operator != (const XVector4SOA& c) const;

  XM2_INLINE const XVector4SOA& operator = (const XVector4SOA& c);
  XM2_INLINE const XVector4SOA& operator = (const XVector4& c);
  XM2_INLINE const XVector4SOA& operator = (const XReal c);

  XM2_INLINE const XVector4SOA& operator += (const XVector4SOA& c);
  XM2_INLINE const XVector4SOA& operator -= (const XVector4SOA& c);
  XM2_INLINE const XVector4SOA& operator *= (const XVector4SOA& c);
  XM2_INLINE const XVector4SOA& operator /= (const XVector4SOA& c);

  XM2_INLINE const XVector4SOA& operator += (const XVector4& c);
  XM2_INLINE const XVector4SOA& operator -= (const XVector4& c);
  XM2_INLINE const XVector4SOA& operator *= (const XVector4& c);
  XM2_INLINE const XVector4SOA& operator /= (const XVector4& c);

  XM2_INLINE const XVector4SOA& operator += (XReal c);
  XM2_INLINE const XVector4SOA& operator -= (XReal c);
  XM2_INLINE const XVector4SOA& operator /= (XReal c);
  XM2_INLINE const XVector4SOA& operator *= (XReal c);

  XM2_INLINE XVector4SOA operator + (const XVector4SOA& c) const;
  XM2_INLINE XVector4SOA operator - (const XVector4SOA& c) const;
  XM2_INLINE XVector4SOA operator * (const XVector4SOA& c) const;
  XM2_INLINE XVector4SOA operator / (const XVector4SOA& c) const;

  XM2_INLINE XVector4SOA operator + (const XVector4& c) const;
  XM2_INLINE XVector4SOA operator - (const XVector4& c) const;
  XM2_INLINE XVector4SOA operator * (const XVector4& c) const;
  XM2_INLINE XVector4SOA operator / (const XVector4& c) const;

  XM2_INLINE XVector4SOA operator + (XReal c) const;
  XM2_INLINE XVector4SOA operator - (XReal c) const;
  XM2_INLINE XVector4SOA operator / (XReal c) const;
  XM2_INLINE XVector4SOA operator * (XReal c) const;
  
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
    /// z components of vectors 
    XReal w[4];
    /// data stored as an array
    XReal data[16];
  #else
  union 
  {
    XReal data[16];
    struct 
    {
      XReal x[4]; 
      XReal y[4];
      XReal z[4];
      XReal w[4];
    };
    struct 
    {
      XReal x0; XReal x1; XReal x2; XReal x3; 
      XReal y0; XReal y1; XReal y2; XReal y3; 
      XReal z0; XReal z1; XReal z2; XReal z3; 
      XReal w0; XReal w1; XReal w2; XReal w3; 
    };
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        struct 
        {
          __m128 _x; 
          __m128 _y;
          __m128 _z;
          __m128 _w;
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
          __m128d _w0;
          __m128d _w1;
        };  
      #endif
    #endif
  };
  #endif

  /// \name  static array methods

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 arithmetic
  //--------------------------------------------------------------------------------------------------------------------

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
  static void arrayAdd(XVector4SOA* output, const XVector4SOA* a,const XVector4SOA* b, int num);

  /// \brief  adds an XVector4SOA to an array of XVector4SOAs. i.e.
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
  static void arrayAdd(XVector4SOA* output, const XVector4SOA* a, const XVector4SOA& b, int num);

  /// \brief  adds an XVector4 to an array of XVector4SOAs. i.e.
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
  static void arrayAdd(XVector4SOA* output, const XVector4SOA* a, const XVector4& b, int num);

  /// \brief  adds an XReal to an array of XVector4SOAs. i.e.
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
  static void arrayAdd(XVector4SOA* output, const XVector4SOA* a, XReal b, int num);

  /// \brief  subtracts an array of XVector4SOAs from an array of XVector4SOAs. i.e.
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
  static void arraySub(XVector4SOA* output, const XVector4SOA* a, const XVector4SOA* b, int num);

  /// \brief  subtracts an XVector4SOA from an array of XVector4SOAs. i.e.
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
  static void arraySub(XVector4SOA* output, const XVector4SOA* a, const XVector4SOA& b, int num);

  /// \brief  subtracts an XVector4 from an array of XVector4SOAs. i.e.
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
  static void arraySub(XVector4SOA* output, const XVector4SOA* a, const XVector4& b, int num);

  /// \brief  subtracts an XReal from an array of XVector4SOAs. i.e.
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
  static void arraySub(XVector4SOA* output, const XVector4SOA* a, XReal b, int num);

  /// \brief  multiplies an array of XVector4SOAs with an array of XVector4SOAs. i.e.
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
  static void arrayMul(XVector4SOA* output, const XVector4SOA* a, const XVector4SOA* b, int num);

  /// \brief  multiplies an XVector4SOA with an array of XVector4SOAs. i.e.
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
  static void arrayMul(XVector4SOA* output, const XVector4SOA* a, const XVector4SOA& b, int num);

  /// \brief  multiplies an XVector4 with an array of XVector4SOAs. i.e.
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
  static void arrayMul(XVector4SOA* output, const XVector4SOA* a, const XVector4& b, int num);

  /// \brief  multiplies an XReal with an array of XVector4SOAs. i.e.
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
  static void arrayMul(XVector4SOA* output, const XVector4SOA* a, XReal b, int num);

  /// \brief  divides an array of XVector4SOA with an array of XVector4SOAs. i.e.
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
  static void arrayDiv(XVector4SOA* output, const XVector4SOA* a, const XVector4SOA* b, int num);

  /// \brief  divides an array of XVector4SOA with an XVector4SOAs. i.e.
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
  static void arrayDiv(XVector4SOA* output, const XVector4SOA* a, const XVector4SOA& b, int num);

  /// \brief  divides an array of XVector4SOA with an XVector4. i.e.
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
  static void arrayDiv(XVector4SOA* output, const XVector4SOA* a, const XVector4& b, int num);


  /// \brief  divides an array of XVector4SOA with an XReal. i.e.
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
  static void arrayDiv(XVector4SOA* output, const XVector4SOA* a, XReal b, int num);

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
  static XM2_INLINE void arrayAdd(XVector4SOA* output, const XVector4SOA* b, int num);

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
  static XM2_INLINE void arrayAdd(XVector4SOA* output, const XVector4SOA& b, int num);

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
  static XM2_INLINE void arrayAdd(XVector4SOA* output, const XVector4& b, int num);

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
  static XM2_INLINE void arrayAdd(XVector4SOA* output, XReal b, int num);

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
  static XM2_INLINE void arraySub(XVector4SOA* output, const XVector4SOA* b, int num);

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
  static XM2_INLINE void arraySub(XVector4SOA* output, const XVector4SOA& b, int num);

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
  static XM2_INLINE void arraySub(XVector4SOA* output, const XVector4& b, int num);

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
  static XM2_INLINE void arraySub(XVector4SOA* output, XReal b, int num);

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
  static XM2_INLINE void arrayMul(XVector4SOA* output, const XVector4SOA* b, int num);

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
  static XM2_INLINE void arrayMul(XVector4SOA* output, const XVector4SOA& b, int num);

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
  static XM2_INLINE void arrayMul(XVector4SOA* output,const XVector4& b,const int num);

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
  static XM2_INLINE void arrayMul(XVector4SOA* output, XReal b, int num);

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
  static XM2_INLINE void arrayDiv(XVector4SOA* output, const XVector4SOA* b, int num);

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
  static XM2_INLINE void arrayDiv(XVector4SOA* output, const XVector4SOA& b, int num);

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
  static XM2_INLINE void arrayDiv(XVector4SOA* output, const XVector4& b, int num);

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
  static XM2_INLINE void arrayDiv(XVector4SOA* output, XReal b, int num);

  /// \brief  normalises an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise( output[i] );
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  ///
  static XM2_INLINE void arrayNormalise(XVector4SOA* output, int num);

  /// \brief  normalises an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = normalise( input[i] );
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static void arrayNormalise(XVector4SOA* output, const XVector4SOA* input, int num);

  /// \brief  normalises an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = zero();
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static void arrayZero(XVector4SOA* output, int num);

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
  static void arrayDot(XVector4* product, const XVector4SOA* a, const XVector4SOA* b, int num);

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
  static void arrayDot(XVector4* product, const XVector4SOA* a, const XVector4SOA& b, int num);

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
  static void arrayDot(XVector4* product, const XVector4SOA* a, const XVector4& b, int num);

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
  static void arrayCross(XVector4SOA* output, const XVector4SOA* a, const XVector4SOA* b, int num);

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
  static void arrayCross(XVector4SOA* output, const XVector4SOA* a, const XVector4SOA& b, int num);

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
  static void arrayCross(XVector4SOA* output, const XVector4SOA* a, const XVector4& b, int num);

  /// \brief  linearly interpolates (LERPS) between the XVector4SOA arrays a and b and 
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
  static void arrayLerp(XVector4SOA* output ,const XVector4SOA* a, const XVector4SOA* b, XReal t, int num);

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
  static void arrayBezier(XVector4SOA* output,
                          const XVector4SOA* a, const XVector4SOA* b,
                          const XVector4SOA* c, const XVector4SOA* d,
                          XReal t, int num);

  /// \brief  normalises an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = -output[i];
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static XM2_INLINE void arrayNegate(XVector4SOA* output, int num);

  /// \brief  normalises an array of XVector4SOAs
  /// \code
  ///         for(i=0;i<num;++i)
  ///         {
  ///           output[i] = -input[i];
  ///         }
  /// \endcode
  /// \param  output - the array to normalise
  /// \param  num - the number of elements in the array
  static void arrayNegate(XVector4SOA* output, const XVector4SOA* input, int num);
}
XM2_ALIGN_SUFFIX(16);
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XVector4SOA& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XVector4SOA& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector3.h"
#include "XM2/Vector3SOA.h"
#include "XM2/Vector4.h"
#include "XM2/inl/Vector4SOA.inl"
#ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
    #include "XM2/inl/sse/Vector4SOA.inl"
  #else
    #include "XM2/inl/sse2/Vector4SOA.inl"
  #endif
#else
  #include "XM2/inl/vanilla/Vector4SOA.inl"
#endif
//----------------------------------------------------------------------------------------------------------------------
