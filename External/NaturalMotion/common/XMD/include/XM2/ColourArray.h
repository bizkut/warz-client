//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Colour.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XColourArray
/// \brief   defines an aligned colour array (std::vector) type
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XColourArray 
  : public pod_vector< XColour > 
{
public:
  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XColourArray() : pod_vector<XColour>()
  {
  }
  
  /// \brief  copy ctor
  /// \param  rhs - the array to copy
  XM2_INLINE XColourArray(const XColourArray& rhs) 
    : pod_vector<XColour>()
  {
    resize(rhs.size());
    memcpy(buffer(),rhs.buffer(),sizeof(XColour)*size());
  }

  /// \name  initialisation

  /// \brief  sets all to 0 (black)
  XM2_INLINE void zero();

  /// \brief  sets all to 1 (white)
  XM2_INLINE void white();

  //--------------------------------------------------------------------------------------------------------------------
  // XColourArray Logical Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second vector to test
  /// \param  eps - floating point error tolerance
  /// \return true if a and b are the same, false otherwise
  XM2_INLINE bool equal(const XColourArray& b, XReal eps = XM2_FLOAT_ZERO) const;

  /// \brief  this != b
  /// \param  b - second vector to test
  /// \param  eps - floating point error tolerance
  /// \return true if a and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XColourArray& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XColourArray operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  /// \todo docs
  XM2_INLINE bool operator==(const XColourArray& ca) const;
  XM2_INLINE bool operator!=(const XColourArray& ca) const;
  XM2_INLINE const XColourArray& operator*=(const XColourArray& rhs);
  XM2_INLINE const XColourArray& operator/=(const XColourArray& rhs);
  XM2_INLINE const XColourArray& operator+=(const XColourArray& rhs);
  XM2_INLINE const XColourArray& operator-=(const XColourArray& rhs);
  XM2_INLINE const XColourArray& operator*=(const XColour& rhs);
  XM2_INLINE const XColourArray& operator/=(const XColour& rhs);
  XM2_INLINE const XColourArray& operator+=(const XColour& rhs);
  XM2_INLINE const XColourArray& operator-=(const XColour& rhs);
  XM2_INLINE const XColourArray& operator*=(XReal rhs);
  XM2_INLINE const XColourArray& operator/=(XReal rhs);
  XM2_INLINE const XColourArray& operator+=(XReal rhs);
  XM2_INLINE const XColourArray& operator-=(XReal rhs);

  //--------------------------------------------------------------------------------------------------------------------
  // XColourArray operations
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  colour operations

  /// \brief  multiplies a and b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void mul(const XColourArray& a, const XColourArray& b);

  /// \brief  divides a by b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void div(const XColourArray& a, const XColourArray& b);

  /// \brief  subtracts b from a, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void sub(const XColourArray& a, const XColourArray& b);

  /// \brief  adds a and b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void add(const XColourArray& a, const XColourArray& b);

  /// \brief  multiplies a and b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void mul(const XColourArray& a, const XColour& b);

  /// \brief  divides a by b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void div(const XColourArray& a, const XColour& b);

  /// \brief  subtracts b from a, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void sub(const XColourArray& a, const XColour& b);

  /// \brief  adds a and b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void add(const XColourArray& a, const XColour& b);

  /// \brief  multiplies a and b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void mul(const XColourArray& a, const XReal b);

  /// \brief  divides a by b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void div(const XColourArray& a, const XReal b);

  /// \brief  subtracts b from a, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void sub(const XColourArray& a, const XReal b);

  /// \brief  adds a and b, stores in this
  /// \param  a - input 1
  /// \param  b - input 2
  XM2_INLINE void add(const XColourArray& a, const XReal b);

  /// \brief  multiplies the input array with this
  /// \param  b - the input colour array
  XM2_INLINE void mul(const XColourArray& b);

  /// \brief  divides this by the input array 
  /// \param  b - the input colour array
  XM2_INLINE void div(const XColourArray& b);

  /// \brief  subtracts the input array from this
  /// \param  b - the input colour array
  XM2_INLINE void sub(const XColourArray& b);

  /// \brief  adds the input array to this
  /// \param  b - the input colour array
  XM2_INLINE void add(const XColourArray& b);

  /// \brief  multiplies this array by b
  /// \param  b - input colour
  XM2_INLINE void mul(const XColour& b);

  /// \brief  divides this array with b
  /// \param  b - input colour
  XM2_INLINE void div(const XColour& b);

  /// \brief  subtracts b from this array 
  /// \param  b - input colour
  XM2_INLINE void sub(const XColour& b);

  /// \brief  adds this array to b
  /// \param  b - input colour
  XM2_INLINE void add(const XColour& b);

  /// \brief  multiplies the colour array by a scalar
  /// \param  b - input 2
  XM2_INLINE void mul(XReal b);

  /// \brief  divides this array by a scalar
  /// \param  b - the scalar
  XM2_INLINE void div(XReal b);

  /// \brief  subtracts b from this array 
  /// \param  b - the scalar
  XM2_INLINE void sub(XReal b);

  /// \brief  adds a scalar to this array
  /// \param  b - the scalar
  XM2_INLINE void add(XReal b);

  /// \brief  inverts the colour array
  /// \param  a - the colour to invert
  XM2_INLINE void invert(const XColourArray& a);

  /// \brief  inverts this colour array
  XM2_INLINE void invert();

  /// \brief  negates the colour array a and stores in this
  /// \param  a - the colour array to negate
  XM2_INLINE void negate(const XColourArray& a);

  /// \brief  negates this colour array
  XM2_INLINE void negate();

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp /slerp

  /// \brief  linearly interpolates (LERPS) between XVector3 a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XColourArray& a, const XColourArray& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XColourArray& a,
                         const XColourArray& b,
                         const XColourArray& c,
                         const XColourArray& d,
                         XReal t);

  /// \brief  multiplies the input array a with the weight, and sums into this array
  /// \param  a - the value to multiply with the weight and add to this colour array
  /// \param  weight - the 0 to 1 interpolation control value
  XM2_INLINE void sum(const XColourArray& a, XReal weight);

  /// \brief  multiplies the input array a with the weight, and sums into this array
  /// \param  a - the value to multiply with the weight and add to this colour array
  /// \param  weight - the 0 to 1 interpolation control value
  XM2_INLINE void sum(const XColour& a, XReal weight);
  
  //--------------------------------------------------------------------------------------------------------------------
  // XColourArray stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XColourArray the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XColourArray to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XColourArray from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XColourArray from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XColourArray to stdout
  void print32() const;

  /// \brief  prints this XColourArray to stdout
  void print64() const;

  /// \brief  writes this XColourArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XColourArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XColourArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XColourArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);    

  #endif
};

// streaming ops
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XColourArray& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XColourArray& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/inl/ColourArray.inl"
//----------------------------------------------------------------------------------------------------------------------
