//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector3Packed.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// Forward Declarations
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XVector3;
class XM2EXPORT XVector4;
class XM2EXPORT XRealArray;
class XM2EXPORT XVector3Array;
class XM2EXPORT XVector4Array;

//----------------------------------------------------------------------------------------------------------------------
/// \brief    This class stores an array of 3D vectors in a packed XYZ format. All 
///           maths operations are done by swizzling the data into SOA format, do the
///           action, and then swizzle the output result. For large vector3 arrays, 
///           this class may well be faster than XVector3Array
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XVector3PackedArray 
  : public pod_vector<XVector3Packed> 
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // initialise
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  sets all elements in the array to zero
  XM2_INLINE void zero();

  //--------------------------------------------------------------------------------------------------------------------
  // dot & cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  dot / cross

  /// \brief  computes the dot products of 2 XVector3Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XRealArray& product, const XVector3PackedArray& b) const;

  /// \brief  computes the dot products of 2 XVector3Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XRealArray& product, const XVector3Packed& b) const;

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector3PackedArray& a, const XVector3PackedArray& b);

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector3PackedArray& a, const XVector3Packed& b);

  //--------------------------------------------------------------------------------------------------------------------
  // dot & cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math operations

  /// \brief  normalises all the XVector4s in the array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = normalise( this[i] );
  ///         }
  /// \endcode
  XM2_INLINE void normalise();

  /// \brief  normalises all the XVector4s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = normalise( v[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void normalise(const XVector3PackedArray& v);

  //--------------------------------------------------------------------------------------------------------------------
  // artithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds a and b and stores the result in this array. i.e.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3PackedArray& a, const XVector3PackedArray& b);

  /// \brief  adds a and b and stores the result in this array. i.e.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3PackedArray& a, const XVector3Array& b);

  /// \brief  adds a and b and stores the result in this array. i.e.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3PackedArray& a, const XVector3Packed& b);

  /// \brief  adds a and b and stores the result in this array. i.e.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3PackedArray& a, const XVector3& b);

  /// \brief  adds a and b and stores the result in this array. i.e.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3PackedArray& a, XReal b);

  /// \brief  subtracts b from a and stores the result in this array. i.e.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3PackedArray& a, const XVector3PackedArray& b);

  /// \brief  subtracts b from a and stores the result in this array. i.e.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3PackedArray& a, const XVector3Array& b);

  /// \brief  subtracts b from a and stores the result in this array. i.e.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3PackedArray& a, const XVector3Packed& b);

  /// \brief  subtracts b from a and stores the result in this array. i.e.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3PackedArray& a, const XVector3& b);

  /// \brief  subtracts b from a and stores the result in this array. i.e.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3PackedArray& a, XReal b);

  /// \brief  multiplies a and b, and stores result in this array
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3PackedArray& a, const XVector3PackedArray& b);

  /// \brief  multiplies a and b, and stores result in this array
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3PackedArray& a, const XVector3Array& b);

  /// \brief  multiplies a and b, and stores result in this array
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3PackedArray& a, const XVector3Packed& b);

  /// \brief  multiplies a and b, and stores result in this array
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3PackedArray& a, const XVector3& b);

  /// \brief  multiplies a and b, and stores result in this array
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3PackedArray& a, XReal b);

  /// \brief  divides a by b, and stores result in this array
  /// \code
  ///         output = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3PackedArray& a, const XVector3PackedArray& b);

  /// \brief  divides a by b, and stores result in this array
  /// \code
  ///         output = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3PackedArray& a, const XVector3Array& b);

  /// \brief  divides a by b, and stores result in this array
  /// \code
  ///         output = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3PackedArray& a, const XVector3Packed& b);

  /// \brief  divides a by b, and stores result in this array
  /// \code
  ///         output = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3PackedArray& a, const XVector3& b);

  /// \brief  divides a by b, and stores result in this array
  /// \code
  ///         output = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3PackedArray& a, XReal b);

  /// \brief  adds b to this XVector3PackedArray
  /// \code
  ///         output += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3PackedArray& b);

  /// \brief  adds b to this XVector3PackedArray
  /// \code
  ///         output += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3Array& b);

  /// \brief  adds b to this XVector3PackedArray
  /// \code
  ///         output += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3Packed& b);

  /// \brief  adds b to this XVector3PackedArray
  /// \code
  ///         output += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3& b);

  /// \brief  adds b to this XVector3PackedArray
  /// \code
  ///         output += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(XReal b);

  /// \brief  subtracts b from this XVector3PackedArray
  /// \code
  ///         output -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3PackedArray& b);

  /// \brief  subtracts b from this XVector3PackedArray
  /// \code
  ///         output -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3Array& b);

  /// \brief  subtracts b from this XVector3PackedArray
  /// \code
  ///         output -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3Packed& b);

  /// \brief  subtracts b from this XVector3PackedArray
  /// \code
  ///         output -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3& b);

  /// \brief  subtracts b from this XVector3PackedArray
  /// \code
  ///         output -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(XReal b);

  /// \brief  multiplies this XVector3PackedArray with b
  /// \code
  ///         output *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3PackedArray& b);

  /// \brief  multiplies this XVector3PackedArray with b
  /// \code
  ///         output *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3Array& b);

  /// \brief  multiplies this XVector3PackedArray with b
  /// \code
  ///         output *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3Packed& b);

  /// \brief  multiplies this XVector3PackedArray with b
  /// \code
  ///         output *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3& b);

  /// \brief  multiplies this XVector3PackedArray with b
  /// \code
  ///         output *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(XReal b);

  /// \brief  divides this XVector3PackedArray by b
  /// \code
  ///         output /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3PackedArray& b);

  /// \brief  divides this XVector3PackedArray by b
  /// \code
  ///         output /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3Array& b);

  /// \brief  divides this XVector3PackedArray by b
  /// \code
  ///         output /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3Packed& b);

  /// \brief  divides this XVector3PackedArray by b
  /// \code
  ///         output /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3& b);

  /// \brief  divides this XVector3PackedArray by b
  /// \code
  ///         output /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(XReal b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3PackedArray Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp /slerp

  /// \brief  linearly interpolates (LERPS) between XVector3 a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector3PackedArray& a, const XVector3PackedArray& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector3PackedArray& a,
                         const XVector3PackedArray& b,
                         const XVector3PackedArray& c,
                         const XVector3PackedArray& d,
                         XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3PackedArray Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates this XVector3PackedArray 
  XM2_INLINE void negate();

  /// \brief  negates an XVector3PackedArray 
  /// \param  v - the XVector3PackedArray to negate
  XM2_INLINE void negate(const XVector3PackedArray& v);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3PackedArray operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_INLINE const XVector3PackedArray& operator*=(const XVector3PackedArray& rhs);
  XM2_INLINE const XVector3PackedArray& operator/=(const XVector3PackedArray& rhs);
  XM2_INLINE const XVector3PackedArray& operator+=(const XVector3PackedArray& rhs);
  XM2_INLINE const XVector3PackedArray& operator-=(const XVector3PackedArray& rhs);

  XM2_INLINE const XVector3PackedArray& operator*=(const XVector3Array& rhs);
  XM2_INLINE const XVector3PackedArray& operator/=(const XVector3Array& rhs);
  XM2_INLINE const XVector3PackedArray& operator+=(const XVector3Array& rhs);
  XM2_INLINE const XVector3PackedArray& operator-=(const XVector3Array& rhs);

  XM2_INLINE const XVector3PackedArray& operator*=(const XVector3Packed& rhs);
  XM2_INLINE const XVector3PackedArray& operator/=(const XVector3Packed& rhs);
  XM2_INLINE const XVector3PackedArray& operator+=(const XVector3Packed& rhs);
  XM2_INLINE const XVector3PackedArray& operator-=(const XVector3Packed& rhs);

  XM2_INLINE const XVector3PackedArray& operator*=(const XVector3& rhs);
  XM2_INLINE const XVector3PackedArray& operator/=(const XVector3& rhs);
  XM2_INLINE const XVector3PackedArray& operator+=(const XVector3& rhs);
  XM2_INLINE const XVector3PackedArray& operator-=(const XVector3& rhs);

  XM2_INLINE const XVector3PackedArray& operator*=(XReal rhs);
  XM2_INLINE const XVector3PackedArray& operator/=(XReal rhs);
  XM2_INLINE const XVector3PackedArray& operator+=(XReal rhs);
  XM2_INLINE const XVector3PackedArray& operator-=(XReal rhs);
  
  //--------------------------------------------------------------------------------------------------------------------
  // XVector3PackedArray iostream operators
  //--------------------------------------------------------------------------------------------------------------------

#if !XM2_NO_STREAM_IO
  /// \brief  outputs the CVector3Array data to an output stream
  /// \param  os - the output stream
  /// \param  data - the data the write
  /// \return the output stream
  friend XM2EXPORT std::ostream& operator<<(std::ostream& os, const XVector3PackedArray& data);
  
  /// \brief  reads the CVector3Array data from an input stream
  /// \param  is - the input stream
  /// \param  data - the data the write into
  /// \return the input stream
  friend XM2EXPORT std::istream& operator>>(std::istream& is, XVector3PackedArray& data);
#endif
  
  //--------------------------------------------------------------------------------------------------------------------
  // XVector3PackedArray stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  prints this XVector3PackedArray  the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector3PackedArray to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector3PackedArray from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector3PackedArray from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector3PackedArray to stdout
  void print32() const;

  /// \brief  prints this XVector3PackedArray to stdout
  void print64() const;

  /// \brief  writes this XVector3PackedArray to the specified binary file
  /// \param  fp - the file to write to 
  void fwrite32(FILE* fp, bool flip_bytes) const;

  /// \brief  writes this XVector3PackedArray to the specified binary file
  /// \param  fp - the file to write to 
  void fwrite64(FILE* fp, bool flip_bytes) const;

  /// \brief  reads a XVector3PackedArray from the specified binary file
  /// \param  fp - the file to read from
  void fread32(FILE* fp, bool flip_bytes);

  /// \brief  reads a XVector3PackedArray from the specified binary file
  /// \param  fp - the file to read from
  void fread64(FILE* fp, bool flip_bytes); 
};

// streaming ops
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XVector3PackedArray& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XVector3PackedArray& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/RealArray.h"
#include "XM2/Vector3Array.h"
#include "XM2/inl/Vector3PackedArray.inl"
//----------------------------------------------------------------------------------------------------------------------
