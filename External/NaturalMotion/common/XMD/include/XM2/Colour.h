//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/MathCommon.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XColour
  /// \brief   An RGBA colour type
  /// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XColour
{
public:
  /// \name  constructors

  /// \brief  ctor
  XM2_INLINE XColour();

  /// \brief  copy ctor
  /// \param  c - the colour to copy
  XM2_INLINE XColour(const XColour& c);

  /// \brief  ctor
  /// \param  r - the red colour component
  /// \param  g - the green colour component
  /// \param  b - the blue colour component
  /// \param  a - the alpha colour component
  XM2_INLINE XColour(XReal r, XReal g, XReal b, XReal a = 1.0f);
  
  /// \brief  ctor
  /// \param  v - the value to assign to all colour components
  XM2_INLINE XColour(XReal v);
  
  /// \brief  ctor
  /// \param  col - the un-aligned memory to initialise this colour from
  XM2_INLINE XColour(const XReal* col);

  //--------------------------------------------------------------------------------------------------------------------
  // XColour initialisation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  sets all to 0 (black)
  XM2_INLINE void zero();
  
  /// \brief  sets all to 1 (white)
  XM2_INLINE void white();

  /// \brief  sets the colour components
  /// \param  r - red colour component
  /// \param  g - green colour component
  /// \param  b - blue colour component
  /// \param  a - alpha colour component
  XM2_INLINE void set(XReal r, XReal g, XReal b, XReal a = 1.0f);

  //--------------------------------------------------------------------------------------------------------------------
  // XColour Logical Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second colour to test
  /// \param  eps - comparison epsilon
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XColour& b, XReal eps = XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second colour to test
  /// \param  eps - comparison epsilon
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XColour& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XColour operations
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math operations

  /// \brief  multiplies a and b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void mul(const XColour& a,const XColour& b);

  /// \brief  divides a by b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void div(const XColour& a,const XColour& b);

  /// \brief  subtracts b from a, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void sub(const XColour& a,const XColour& b);

  /// \brief  adds a and b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void add(const XColour& a,const XColour& b);

  /// \brief  multiplies a and b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void mul(const XColour& a, XReal b);

  /// \brief  divides a by b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void div(const XColour& a, XReal b);

  /// \brief  subtracts b from a, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void sub(const XColour& a, XReal b);

  /// \brief  adds a and b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void add(const XColour& a, XReal b);
  
  /// \brief  multiplies a and b, stores in this
  /// \param  b - input 2
  XM2_INLINE void mul(const XColour& b) { mul(*this,b); }

  /// \brief  divides a by b, stores in this
  /// \param  b - input 2
  XM2_INLINE void div(const XColour& b) { div(*this,b); }

  /// \brief  subtracts b from a, stores in this
  /// \param  b - input 2
  XM2_INLINE void sub(const XColour& b) { sub(*this,b); }

  /// \brief  adds a and b, stores in this
  /// \param  b - input 2
  XM2_INLINE void add(const XColour& b) { add(*this,b); }

  /// \brief  multiplies a and b, stores in this
  /// \param  b - input 2
  XM2_INLINE void mul(XReal b) { mul(*this,b); }

  /// \brief  divides a by b, stores in this
  /// \param  b - input 2
  XM2_INLINE void div(XReal b) { div(*this,b); }

  /// \brief  subtracts b from a, stores in this
  /// \param  b - input 2
  XM2_INLINE void sub(XReal b) { sub(*this,b); }

  /// \brief  adds a and b, stores in this
  /// \param  b - input 2
  XM2_INLINE void add(XReal b) { add(*this,b); }

  /// \brief  inverts a and stores in this (i.e. 1-r,1-g,1-b,1-a)
  /// \param  a - the colour to invert
  XM2_INLINE void invert(const XColour& a);

  /// \brief  inverts this colour (i.e. 1-r,1-g,1-b,1-a
  XM2_INLINE void invert();

  /// \brief  negates a and stores in this (i.e. -r,-g,-b,-a)
  /// \param  a - the colour to negate
  XM2_INLINE void negate(const XColour& a);

  /// \brief  negates this colour (i.e. -r,-g,-b,-a)
  XM2_INLINE void negate();


  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp/slerp

  /// \brief  linearly interpolates (LERPS) between XVector3 a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XColour& a, const XColour& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XColour& a, const XColour& b, const XColour& c, const XColour& d, XReal t);
  
  #ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT

  /// \brief  linearly interpolates (LERPS) between XColour a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XColour& a, const XColour& b, const __m128 t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XColour& a, const XColour& b, const XColour& c, const XColour& d, __m128 t);

  #else
  
  /// \brief  linearly interpolates (LERPS) between XColour a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XColour& a, const XColour& b, const __m128d t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XColour& a, const XColour& b, const XColour& c, const XColour& d, __m128d t);

  #endif
  #endif
  //--------------------------------------------------------------------------------------------------------------------
  // XColour stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XColour the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XColour to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XColour from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XColour from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XColour to stdout
  void print32() const;

  /// \brief  prints this XColour to stdout
  void print64() const;

  /// \brief  writes this XColour to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XColour to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XColour from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XColour from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);    

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // C++ operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  /// \brief  comparison operator
  /// \param  c - the colour to compare to this
  /// \return true if c is the same as this colour
  XM2_INLINE bool operator == (const XColour& c) const;

  /// \brief  comparison operator
  /// \param  c - the colour to compare to this
  /// \return true if c is not the same as this colour
  XM2_INLINE bool operator != (const XColour& c) const;

  /// \brief  assignment operator
  /// \param  c - the colour to copy
  /// \return *this
  XM2_INLINE const XColour& operator = (const XColour& c);

  /// \brief  *this += c
  /// \param  c - the colour to add to this colour
  /// \return *this
  XM2_INLINE const XColour& operator += (const XColour& c);

  /// \brief  *this -= c
  /// \param  c - the colour to subtract this colour
  /// \return *this
  XM2_INLINE const XColour& operator -= (const XColour& c);

  /// \brief  *this /= c
  /// \param  c - the colour to divide this colour by
  /// \return *this
  XM2_INLINE const XColour& operator /= (const XColour& c);

  /// \brief  *this *= c
  /// \param  c - the colour to multiply with this colour
  /// \return *this
  XM2_INLINE const XColour& operator *= (const XColour& c);

  /// \brief  adds a float to each component of this colour
  /// \param  c - the float to add to this colour
  /// \return *this
  XM2_INLINE const XColour& operator += (XReal c);

  /// \brief  subtracts a float value from each component of this colour
  /// \param  c - the float to subtract from this colour
  /// \return *this
  XM2_INLINE const XColour& operator -= (XReal c);

  /// \brief  divides each component of this colour by c
  /// \param  c - the float to divide this colour by
  /// \return *this
  XM2_INLINE const XColour& operator /= (XReal c);

  /// \brief  multiples a float with each component of this colour
  /// \param  c - the float to multiply with this colour
  /// \return *this
  XM2_INLINE const XColour& operator *= (XReal c);

  /// \brief  adds the colour c to this colour and returns the result
  /// \param  c - the colour to add to this
  /// \return this + c
  XM2_INLINE XColour operator + (const XColour& c) const;

  /// \brief  subtracts the colour c from this colour and returns the result
  /// \param  c - the colour to subtract from this
  /// \return this - c
  XM2_INLINE XColour operator - (const XColour& c) const;

  /// \brief  divides this colour by c and returns the result
  /// \param  c - the colour to divide this
  /// \return this / c
  XM2_INLINE XColour operator / (const XColour& c) const;

  /// \brief  multiplies the colour c with this colour and returns the result
  /// \param  c - the colour to multiply with this
  /// \return this * c
  XM2_INLINE XColour operator * (const XColour& c) const;

  /// \brief  adds the float c to this colour and returns the result
  /// \param  c - the float to add to this
  /// \return this + c
  XM2_INLINE XColour operator + (XReal c) const;

  /// \brief  subtracts the float c from this colour and returns the result
  /// \param  c - the float to subtract from this
  /// \return this - c
  XM2_INLINE XColour operator - (XReal c) const;

  /// \brief  divides this colour by the float c and returns the result
  /// \param  c - the float to divide this with
  /// \return this / c
  XM2_INLINE XColour operator / (XReal c) const;

  /// \brief  multiplies the float c with this colour and returns the result
  /// \param  c - the float to multiply with this
  /// \return this * c
  XM2_INLINE XColour operator * (XReal c) const;

  /// \brief  multiplies the float f with the colour p and returns the result
  /// \param  f - the float
  /// \param  p - the colour
  /// \return f * p
  friend XM2_INLINE XColour operator * (XReal f, const XColour& p);

  //--------------------------------------------------------------------------------------------------------------------
  // XColour array functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  static array methods

  /// \brief  multiplies each element of a with each element of b and stores the result 
  ///         in the output array.
  /// \param  output - the output colour array
  /// \param  a - the input colour array
  /// \param  b - the input colour array
  /// \param  num - number of colours in the arrays
  static bool arrayEqual(const XColour* b, const XColour* a, int num, XReal eps = XM2_FLOAT_ZERO);

  /// \brief  multiplies each element of a with b and stores the result 
  ///         in the output array.
  /// \param  output - the output colour array
  /// \param  a - the input colour array
  /// \param  b - the input colour
  /// \param  num - number of colours in the arrays
  static bool arrayNotEqual(const XColour* b, const XColour* a, int num, XReal eps = XM2_FLOAT_ZERO);

  /// \brief  multiplies each element of a with each element of b and stores the result 
  ///         in the output array.
  /// \param  output - the output colour array
  /// \param  a - the input colour array
  /// \param  b - the input colour array
  /// \param  num - number of colours in the arrays
  static void arrayMul(XColour* output, const XColour* a, const XColour* b, int num);

  /// \brief  multiplies each element of a with b and stores the result 
  ///         in the output array.
  /// \param  output - the output colour array
  /// \param  a - the input colour array
  /// \param  b - the input colour
  /// \param  num - number of colours in the arrays
  static void arrayMul(XColour* output, const XColour* a, const XColour& b, int num);

  /// \brief  multiplies each element of a with each element of b and stores the result 
  ///         in the output array.
  /// \param  output - the output colour array
  /// \param  a - the input colour array
  /// \param  b - the input float array
  /// \param  num - number of colours in the arrays
  static void arrayMul(XColour* output, const XColour* a, const XReal* b, int num);

  /// \brief  multiplies each element of a with the scalar b and stores the result 
  ///         in the output array.
  /// \param  output - the output colour array
  /// \param  a - the input colour array
  /// \param  b - the input scalar
  /// \param  num - number of colours in the arrays
  static void arrayMul(XColour* output, const XColour* a, XReal b, int num);

  /// \brief  multiplies each element of the output array with each element of b
  /// \param  output - the output colour array
  /// \param  b - the input colour array
  /// \param  num - number of colours in the arrays
  static XM2_INLINE void arrayMul(XColour* output, const XColour* b, int num);

  /// \brief  multiplies each element of the output array with b
  /// \param  output - the output colour array
  /// \param  b - the input colour
  /// \param  num - number of colours in the arrays
  static XM2_INLINE void arrayMul(XColour* output, const XColour& b, int num);

  /// \brief  multiplies each element of the output array with each element of b
  /// \param  output - the output colour array
  /// \param  b - the input scalar array
  /// \param  num - number of colours in the arrays
  static XM2_INLINE void arrayMul(XColour* output,const XReal* b, int num);

  /// \brief  multiplies each element of the output array with b
  /// \param  output - the output colour array
  /// \param  b - the input scalar
  /// \param  num - number of colours in the arrays
  static XM2_INLINE void arrayMul(XColour* output, XReal b, int num);

  /// \brief  divides each element of a by each element of b and stores the result 
  ///         in the output array.
  /// \param  output - the output colour array
  /// \param  a - the input colour array
  /// \param  b - the input colour array
  /// \param  num - number of colours in the arrays
  static void arrayDiv(XColour* output, const XColour* a, const XColour* b, int num);

  /// \brief  divides each element of a by b and stores the result 
  ///         in the output array.
  /// \param  output - the output colour array
  /// \param  a - the input colour array
  /// \param  b - the input colour
  /// \param  num - number of colours in the arrays
  static void arrayDiv(XColour* output, const XColour* a, const XColour& b, int num);

  /// \brief  divides each element of a by each element of b and stores the result 
  ///         in the output array.
  /// \param  output - the output colour array
  /// \param  a - the input colour array
  /// \param  b - the input float array
  /// \param  num - number of colours in the arrays
  static void arrayDiv(XColour* output, const XColour* a, const XReal* b, int num);

  /// \brief  divides each element of a by b and stores the result 
  ///         in the output array.
  /// \param  output - the output colour array
  /// \param  a - the input colour array
  /// \param  b - the input float
  /// \param  num - number of colours in the arrays
  static void arrayDiv(XColour* output, const XColour* a, XReal b, int num);

  /// \brief  divides each element of the output array by each element of b
  /// \param  output - the output colour array
  /// \param  b - the input colour array
  /// \param  num - number of colours in the arrays
  static XM2_INLINE void arrayDiv(XColour* output, const XColour* b, int num);

  /// \brief  divides each element of the output array by b
  /// \param  output - the output colour array
  /// \param  b - the input colour
  /// \param  num - number of colours in the arrays
  static XM2_INLINE void arrayDiv(XColour* output, const XColour& b, int num);

  /// \brief  divides each element of the output array by each element of b
  /// \param  output - the output colour array
  /// \param  b - the input float array
  /// \param  num - number of colours in the arrays
  static XM2_INLINE void arrayDiv(XColour* output, const XReal* b, int num);

  /// \brief  divides each element of the output array by b
  /// \param  output - the output colour array
  /// \param  b - the input float
  /// \param  num - number of colours in the arrays
  /// 
  static XM2_INLINE void arrayDiv(XColour* output, XReal b, int num);

  /// \brief  adds each element of b to each element of a, and stores the result
  ///         in the output array
  /// \param  output - the output array
  /// \param  a - the input array of colours
  /// \param  b - the array of colour values to add to the array
  /// \param  num - number of elements in the array
  static void arrayAdd(XColour* output, const XColour* a, const XColour* b, int num);

  /// \brief  adds b to each element of a, and stores the result
  ///         in the output array
  /// \param  output - the output array
  /// \param  a - the input array of colours
  /// \param  b - the colour value to add to the array
  /// \param  num - number of elements in the array
  static void arrayAdd(XColour* output, const XColour* a, const XColour& b, int num);

  /// \brief  adds each element of b to each element of a, and stores the result
  ///         in the output array
  /// \param  output - the output array
  /// \param  a - the input array of colours
  /// \param  b - the array of float values to add to the array
  /// \param  num - number of elements in the array
  static void arrayAdd(XColour* output, const XColour* a, const XReal* b, int num);

  /// \brief  adds b to each element of a, and stores the result
  ///         in the output array
  /// \param  output - the output array
  /// \param  a - the input array of colours
  /// \param  b - the float value to add to the array
  /// \param  num - number of elements in the array
  static void arrayAdd(XColour* output, const XColour* a, XReal b, int num);

  /// \brief  adds each element of b to each element of the output array
  /// \param  output - the output array
  /// \param  b - the array of colour values to add to the array
  /// \param  num - number of elements in the array
  static XM2_INLINE void arrayAdd(XColour* output, const XColour* b, int num);

  /// \brief  adds b to each element of the output array
  /// \param  output - the output array
  /// \param  b - the colour value to add to the array
  /// \param  num - number of elements in the array
  static XM2_INLINE void arrayAdd(XColour* output, const XColour& b, int num);

  /// \brief  adds each element of b to each element of the output array
  /// \param  output - the output array
  /// \param  b - the array of float values to add to the array
  /// \param  num - number of elements in the array
  static XM2_INLINE void arrayAdd(XColour* output, const XReal* b, int num);

  /// \brief  adds b to each element of the output array
  /// \param  output - the output array
  /// \param  b - the float value to add to the array
  /// \param  num - number of elements in the array
  static XM2_INLINE void arrayAdd(XColour* output, XReal b, int num);

  /// \brief  subtracts each element of b from each element of a, and stores the result
  ///         in the output array
  /// \param  output - the output array
  /// \param  a - the input array of colours
  /// \param  b - the array of colour values to subtract from the array
  /// \param  num - number of elements in the array
  static void arraySub(XColour* output, const XColour* a, const XColour* b, int num);

  /// \brief  subtracts b from each element of a, and stores the result
  ///         in the output array
  /// \param  output - the output array
  /// \param  a - the input array of colours
  /// \param  b - the colour value to subtract from the array
  /// \param  num - number of elements in the array
  static void arraySub(XColour* output, const XColour* a, const XColour& b, int num);

  /// \brief  subtracts each element of b from each element of a, and stores the result
  ///         in the output array
  /// \param  output - the output array
  /// \param  a - the input array of colours
  /// \param  b - the array of float values to subtract from the array
  /// \param  num - number of elements in the array
  static void arraySub(XColour* output, const XColour* a,const XReal* b, int num);

  /// \brief  subtracts b from each element of a, and stores the result
  ///         in the output array
  /// \param  output - the output array
  /// \param  a - the input array of colours
  /// \param  b - the float value to subtract from the array
  /// \param  num - number of elements in the array
  static void arraySub(XColour* output, const XColour* a, XReal b, int num);

  /// \brief  subtracts each element of b from each element in the output array
  /// \param  output - the array to modify
  /// \param  b - the array of colour values to subtract from the array
  /// \param  num - number of elements in the array
  static XM2_INLINE void arraySub(XColour* output, const XColour* b, int num);

  /// \brief  subtracts b from each element in the array
  /// \param  output - the array to modify
  /// \param  b - the colour value to subtract from the array
  /// \param  num - number of elements in the array
  static XM2_INLINE void arraySub(XColour* output, const XColour& b, int num);

  /// \brief  subtracts each element of b from each element in the output array
  /// \param  output - the array to modify
  /// \param  b - the array of float values to subtract from the array
  /// \param  num - number of elements in the array
  static XM2_INLINE void arraySub(XColour* output, const XReal* b, int num);
  
  /// \brief  subtracts b from each element in the array
  /// \param  output - the array to modify
  /// \param  b - the float value to subtract from the array
  /// \param  num - number of elements in the array
  static XM2_INLINE void arraySub(XColour* output, XReal b, int num);

  /// \brief  inverts an array of colours
  /// \param  input - the array to invert
  /// \param  output - the returned values
  /// \param  num - the number of colours in the array
  static void arrayInvert(XColour* output, const XColour* input, int num);

  /// \brief  negates an array of colours
  /// \param  input - the array to negate
  /// \param  output - the returned values
  /// \param  num - the number of colours in the array
  static void arrayNegate(XColour* output, const XColour* input, int num);

  /// \brief  inverts an array of colours
  /// \param  output - the array to invert
  /// \param  num - the number of colours in the array
  static XM2_INLINE void arrayInvert(XColour* output, int num);

  /// \brief  negates an array of colours
  /// \param  output - the array to negate
  /// \param  num - the number of colours in the array
  static XM2_INLINE void arrayNegate(XColour* output, int num);

  /// \brief  sets an array of colours to black
  /// \param  output - the array to modify
  /// \param  num - the number of colours in the array
  static void arrayZero(XColour* output, int num);

  /// \brief  sets an array of colours to white
  /// \param  output - the array to modify
  /// \param  num - the number of colours in the array
  static void arrayWhite(XColour* output, int num);

  /// \brief  multiples every element of the array a by the weight, and sums the result
  ///         into the output array.
  /// \param  output - the output array
  /// \param  a - the input colour array
  /// \param  weight - a weight value
  /// \param  num - number of elements in the output and a arrays 
  static void arraySum(XColour* output, const XColour* a, const XReal& weight, int num);

  /// \brief  adds (a*weight) to each element of the output array
  /// \param  output - the output array
  /// \param  a - the input colour
  /// \param  weight - a weight value
  /// \param  num - number of elements in the output array
  static void arraySum(XColour* output, const XColour& a, const XReal& weight, int num);
  

  //--------------------------------------------------------------------------------------------------------------------
  // XColour Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates (LERPS) between the XColour arrays a and b and 
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
  static void arrayLerp(XColour* output,
                        const XColour* a, const XColour* b,
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
  static void arrayBezier(XColour* output,
                          const XColour* a, const XColour* b,
                          const XColour* c, const XColour* d,
                          XReal t, int num);

  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  /// \name  data

  //--------------------------------------------------------------------------------------------------------------------
  // Member variables
  //--------------------------------------------------------------------------------------------------------------------
  #ifdef DOXYGEN
    /// red
    XReal r; 
    /// green
    XReal g;
    /// blue
    XReal b;
    /// alpha
    XReal a;
    /// the colour as an array of floats
    XReal data[4];
  #else
  union 
  {
    struct 
    {
      XReal r; 
      XReal g;
      XReal b;
      XReal a;
    };
    XReal data[4];

    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        __m128 sse;
      #else
        struct 
        {
          __m128d rg;
          __m128d ba;
        };
      #endif
    #endif
  };
  #endif
} 
XM2_ALIGN_SUFFIX(16);

#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& os, const XColour& data);
XM2EXPORT std::istream& operator>>(std::istream& is, XColour& data);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/inl/Colour.inl"
#if XM2_SSE
# include "XM2/inl/sse/Colour.inl"
#elif XM2_SSE2
# include "XM2/inl/sse2/Colour.inl"
#elif XM2_FPU
# include "XM2/inl/vanilla/Colour.inl"
#else
# error Unknown CPU architecture
#endif
