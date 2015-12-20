//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector4.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// Forward Declarations
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XVector3;
class XM2EXPORT XRealArray;
class XM2EXPORT XVector3Array;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XVector4Array
/// \brief   defines an aligned vector4 array (std::vector) type
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XVector4Array 
  : public pod_vector<XVector4> 
{
public:

  /// \name  initialisation

  //--------------------------------------------------------------------------------------------------------------------
  // initialise
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  sets all elements in the array to zero
  XM2_INLINE void zero();

  //--------------------------------------------------------------------------------------------------------------------
  // dot & cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math

  /// \brief  computes the dot products of 2 XVector3Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XRealArray& product, const XVector4& b) const;

  /// \brief  computes the dot products of 2 XVector3Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XRealArray& product, const XVector4Array& b) const;

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector4Array& a, const XVector4& b);

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector4Array& a, const XVector4Array& b);

  //--------------------------------------------------------------------------------------------------------------------
  // dot & cross products
  //--------------------------------------------------------------------------------------------------------------------

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
  XM2_INLINE void normalise(const XVector4Array& v);

  //--------------------------------------------------------------------------------------------------------------------
  // dot & cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds an XVector4Array to an XVector4Array and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4Array& a, const XVector4Array& b);

  /// \brief  adds an XVector4Array to an XVector4 and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4Array& a, const XVector4& b);

  /// \brief  adds an XVector4Array to an XVector3Array and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4Array& a, const XVector3Array& b);

  /// \brief  adds an XVector4Array to an XVector3 and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  ///
  XM2_INLINE void add(const XVector4Array& a, const XVector3& b);

  /// \brief  adds an XVector4Array to a float and stores the result in this.
  /// \code
  ///         this = a + b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4Array& a, XReal b);

  /// \brief  subtracts an XVector4Array from an XVector4Array and stores the result in this.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4Array& a, const XVector4Array& b);

  /// \brief  subtracts an XVector4Array from an XVector4 and stores the result in this.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4Array& a, const XVector4& b);

  /// \brief  subtracts an XVector4Array from an XVector3Array and stores the result in this.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4Array& a, const XVector3Array& b);

  /// \brief  subtracts an XVector4Array from an XVector3 and stores the result in this.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4Array& a, const XVector3& b);

  /// \brief  subtracts an XVector4Array from an XReal and stores the result in this.
  /// \code
  ///         this = a - b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4Array& a, XReal b);

  /// \brief  multiplies an XVector4Array with an XVector4Array and stores the result in this.
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4Array& a, const XVector4Array& b);

  /// \brief  multiplies an XVector4Array with an XVector4 and stores the result in this.
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4Array& a, const XVector4& b);

  /// \brief  multiplies an XVector4Array with an XVector3Array and stores the result in this.
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4Array& a, const XVector3Array& b);

  /// \brief  multiplies an XVector4Array with an XVector3 and stores the result in this.
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4Array& a, const XVector3& b);

  /// \brief  multiplies an XVector4Array with an XReal and stores the result in this.
  /// \code
  ///         this = a * b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4Array& a, XReal b);

  /// \brief  divides an XVector4Array by an XVector4Array and stores the result in this.
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4Array& a, const XVector4Array& b);

  /// \brief  divides an XVector4Array by an XVector4 and stores the result in this.
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4Array& a, const XVector4& b);

  /// \brief  divides an XVector4Array by an XVector3Array and stores the result in this.
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4Array& a, const XVector3Array& b);

  /// \brief  divides an XVector4Array by an XVector3 and stores the result in this.
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4Array& a, const XVector3& b);

  /// \brief  divides an XVector4Array by an XReal and stores the result in this.
  /// \code
  ///         this = a / b
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4Array& a, XReal b);

  /// \brief  adds an XVector4Array to this XVector4Array
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4Array& b);

  /// \brief  adds an XVector4 to this XVector4Array
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4& b);

  /// \brief  adds an XVector3Array to this XVector4Array
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3Array& b);

  /// \brief  adds an XVector3 to this XVector4Array
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3& b);

  /// \brief  adds an XReal to this XVector4Array
  /// \code
  ///         this += b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(XReal b);

  /// \brief  subtracts an XVector4Array from this XVector4Array
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4Array& b);

  /// \brief  subtracts an XVector4 from this XVector4Array
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4& b);

  /// \brief  subtracts an XVector3Array from this XVector4Array
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3Array& b);

  /// \brief  subtracts an XVector3 from this XVector4Array
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3& b);

  /// \brief  subtracts an XReal from this XVector4Array
  /// \code
  ///         this -= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(XReal b);

  /// \brief  multiplies an XVector4Array with this XVector4Array
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4Array& b);

  /// \brief  multiplies an XVector4 with this XVector4Array
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4& b);

  /// \brief  multiplies an XVector3Array with this XVector4Array
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3Array& b);

  /// \brief  multiplies an XVector3 with this XVector4Array
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3& b);

  /// \brief  multiplies an XReal with this XVector4Array
  /// \code
  ///         this *= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(XReal b);

  /// \brief  divides this XVector4Array by an XVector4Array
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4Array& b);

  /// \brief  divides this XVector4Array by an XVector4
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4& b);

  /// \brief  divides this XVector4Array by an XVector3Array
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3Array& b);

  /// \brief  divides this XVector4Array by an XVector3
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3& b);

  /// \brief  divides this XVector4Array by an XReal
  /// \code
  ///         this /= b
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(XReal b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4Array Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates (LERPS) between XVector4Arrays a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector4Array& a, const XVector4Array& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector4Array& a,
                         const XVector4Array& b,
                         const XVector4Array& c,
                         const XVector4Array& d,
                         XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4Array Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates this XVector4Array 
  XM2_INLINE void negate();

  /// \brief  negates an XVector4Array 
  /// \param  v - the XVector4Array to negate
  XM2_INLINE void negate(const XVector4Array& v);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4Array operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE const XVector4Array& operator*=(const XVector4Array& rhs);
  XM2_INLINE const XVector4Array& operator/=(const XVector4Array& rhs);
  XM2_INLINE const XVector4Array& operator+=(const XVector4Array& rhs);
  XM2_INLINE const XVector4Array& operator-=(const XVector4Array& rhs);

  XM2_INLINE const XVector4Array& operator*=(const XVector3Array& rhs);
  XM2_INLINE const XVector4Array& operator/=(const XVector3Array& rhs);
  XM2_INLINE const XVector4Array& operator+=(const XVector3Array& rhs);
  XM2_INLINE const XVector4Array& operator-=(const XVector3Array& rhs);

  XM2_INLINE const XVector4Array& operator*=(const XVector4& rhs);
  XM2_INLINE const XVector4Array& operator/=(const XVector4& rhs);
  XM2_INLINE const XVector4Array& operator+=(const XVector4& rhs);
  XM2_INLINE const XVector4Array& operator-=(const XVector4& rhs);

  XM2_INLINE const XVector4Array& operator*=(const XVector3& rhs);
  XM2_INLINE const XVector4Array& operator/=(const XVector3& rhs);
  XM2_INLINE const XVector4Array& operator+=(const XVector3& rhs);
  XM2_INLINE const XVector4Array& operator-=(const XVector3& rhs);

  XM2_INLINE const XVector4Array& operator*=(XReal rhs);
  XM2_INLINE const XVector4Array& operator/=(XReal rhs);
  XM2_INLINE const XVector4Array& operator+=(XReal rhs);
  XM2_INLINE const XVector4Array& operator-=(XReal rhs);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector4Array stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XVector4Array  the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector4Array  to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector4Array  from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector4Array  from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector4Array  to stdout
  void print32() const;

  /// \brief  prints this XVector4Array  to stdout
  void print64() const;

  /// \brief  writes this XVector4Array  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XVector4Array  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XVector4Array  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XVector4Array  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false); 

  #endif
};

// streaming ops 
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XVector4Array& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XVector4Array& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/RealArray.h"
#include "XM2/Vector3Array.h"
#include "XM2/inl/Vector4Array.inl"
//----------------------------------------------------------------------------------------------------------------------
