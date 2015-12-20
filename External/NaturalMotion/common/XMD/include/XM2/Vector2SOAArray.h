//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VECTOR2SOA_ARRAY__H__
#define XM2_VECTOR2SOA_ARRAY__H__
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector2SOA.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------------------------------------------------

class XM2EXPORT XVector2;
class XM2EXPORT XVector4;
class XM2EXPORT XVector2Array;
class XM2EXPORT XVector4Array;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XMD::XVector2SOAArray 
/// \brief   defines an array of 2D vectors in structure of array format
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XVector2SOAArray 
  : public pod_vector< XVector2SOA > 
{
public:

  /// \name  streaming / swizzling

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector2Array& input);

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector2s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector2Array& input) const;

  /// \name  math

  /// \brief  adds an XVector2SOAArray to an array of XVector2SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2SOAArray& a,const XVector2SOAArray& b);

  /// \brief  adds an XVector2SOA to an XVector2SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2SOAArray& a,const XVector2SOA& b);

  /// \brief  adds an XVector2 to an XVector2SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2SOAArray& a,const XVector2& b);

  /// \brief  adds an XVector2 to an XVector2SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2SOAArray& a,const XReal b);

  /// \brief  subtracts an XVector2SOAArray array from an array of XVector2SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2SOAArray& a,const XVector2SOAArray& b);

  /// \brief  subtracts an XVector2SOA from an array of XVector2SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2SOAArray& a,const XVector2SOA& b);

  /// \brief  subtracts an XVector2 from an array of XVector2SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2SOAArray& a,const XVector2& b);

  /// \brief  subtracts a float from an array of XVector2SOAArray. i.e.
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2SOAArray& a,const XReal b);

  /// \brief  multiplies two XVector2SOAArrays. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2SOAArray& a,const XVector2SOAArray& b);

  /// \brief  multiplies an XVector2SOA with an XVector2SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2SOAArray& a,const XVector2SOA& b);

  /// \brief  multiplies an XVector2 with an XVector2SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2SOAArray& a,const XVector2& b);

  /// \brief  multiplies an XVector2SOAArray with a float. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2SOAArray& a,const XReal b);

  /// \brief  divides an XVector2SOAArray by another XVector2SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2SOAArray& a,const XVector2SOAArray& b);

  /// \brief  divides an XVector2SOAArray by an XVector2SOA. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2SOAArray& a,const XVector2SOA& b);

  /// \brief  divides an XVector2SOAArray by an XVector2. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2SOAArray& a,const XVector2& b);

  /// \brief  divides an XVector2SOAArray by a float. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2SOAArray& a,const XReal b);

  /// \brief  adds an XVector2SOAArray to this XVector2SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2SOAArray& b);

  /// \brief  adds an XVector2SOA to this XVector2SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2SOA& b);

  /// \brief  adds an XVector2 to this XVector2SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2& b);

  /// \brief  adds a float to this XVector2SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XReal b);

  /// \brief  subtracts an XVector2SOAArray from this XVector2SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2SOAArray& b);

  /// \brief  subtracts an XVector2SOA from this XVector2SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2SOA& b);

  /// \brief  subtracts an XVector2 from this XVector2SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2& b);

  /// \brief  subtracts a float from this XVector2SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XReal b);

  /// \brief  multiplies this XVector2SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2SOAArray& b);

  /// \brief  multiplies this XVector2SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2SOA& b);

  /// \brief  multiplies this XVector2SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2& b);

  /// \brief  multiplies this XVector2SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XReal b);

  /// \brief  divides this XVector2SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2SOAArray& b);

  /// \brief  divides this XVector2SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2SOA& b);

  /// \brief  divides this XVector2SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2& b);

  /// \brief  divides this XVector2SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XReal b);

  /// \brief  normalises all the XVector2SOAs in the array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = normalise( this[i] );
  ///         }
  /// \endcode
  XM2_INLINE void normalise();

  /// \brief  normalises all the XVector2SOAs in the array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = normalise( v[i] );
  ///         }
  /// \endcode
  /// \param  v - the array to normalise
  XM2_INLINE void normalise(const XVector2SOAArray& v);

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

  /// \brief  computes the dot products of 2 XVector2Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XVector4Array& product,const XVector2& b) const;

  /// \brief  computes the dot products of 2 XVector2Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XVector4Array& product,const XVector2SOA& b) const;

  /// \brief  computes the dot products of 2 XVector2Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XVector4Array& product,const XVector2SOAArray& b) const;

  /// \brief  normalises all the XVector2s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector2 array to normalise
  XM2_INLINE void cross(const XVector2SOAArray& a,const XVector2& b);

  /// \brief  normalises all the XVector2s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector2 array to normalise
  XM2_INLINE void cross(const XVector2SOAArray& a,const XVector2SOA& b);

  /// \brief  normalises all the XVector2s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector2 array to normalise
  XM2_INLINE void cross(const XVector2SOAArray& a,const XVector2SOAArray& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2 Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp /slerp

  /// \brief  linearly interpolates (LERPS) between XVector2 a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector2SOAArray& a,const XVector2SOAArray& b,const XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector2SOAArray& a,
                         const XVector2SOAArray& b,
                         const XVector2SOAArray& c,
                         const XVector2SOAArray& d,
                         const XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2 Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates this XVector2Array 
  XM2_INLINE void negate();

  /// \brief  negates an XVector2Array 
  /// \param  v - the XVector2Array to negate
  XM2_INLINE void negate(const XVector2Array& v);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOAArray operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  XM2_INLINE const XVector2SOAArray& operator=(const XVector2SOAArray& rhs);

  XM2_INLINE const XVector2SOAArray& operator*=(const XVector2SOAArray& rhs);
  XM2_INLINE const XVector2SOAArray& operator/=(const XVector2SOAArray& rhs);
  XM2_INLINE const XVector2SOAArray& operator+=(const XVector2SOAArray& rhs);
  XM2_INLINE const XVector2SOAArray& operator-=(const XVector2SOAArray& rhs);

  XM2_INLINE const XVector2SOAArray& operator*=(const XVector2SOA& rhs);
  XM2_INLINE const XVector2SOAArray& operator/=(const XVector2SOA& rhs);
  XM2_INLINE const XVector2SOAArray& operator+=(const XVector2SOA& rhs);
  XM2_INLINE const XVector2SOAArray& operator-=(const XVector2SOA& rhs);
  
  XM2_INLINE const XVector2SOAArray& operator*=(const XVector2& rhs);
  XM2_INLINE const XVector2SOAArray& operator/=(const XVector2& rhs);
  XM2_INLINE const XVector2SOAArray& operator+=(const XVector2& rhs);
  XM2_INLINE const XVector2SOAArray& operator-=(const XVector2& rhs);
  
  XM2_INLINE const XVector2SOAArray& operator*=(const XReal rhs);
  XM2_INLINE const XVector2SOAArray& operator/=(const XReal rhs);
  XM2_INLINE const XVector2SOAArray& operator+=(const XReal rhs);
  XM2_INLINE const XVector2SOAArray& operator-=(const XReal rhs);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2SOAArray stdio
  //--------------------------------------------------------------------------------------------------------------------

  #if !XM2_NO_C_IO

  /// \name  file IO

  /// \brief  prints this XVector2SOAArray  the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector2SOAArray  to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector2SOAArray  from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector2SOAArray  from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector2SOAArray  to stdout
  void print32() const;

  /// \brief  prints this XVector2SOAArray  to stdout
  void print64() const;

  /// \brief  writes this XVector2SOAArray  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp,bool flip_bytes=false) const;

  /// \brief  writes this XVector2SOAArray  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp,bool flip_bytes=false) const;

  /// \brief  reads a XVector2SOAArray  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp,bool flip_bytes=false);

  /// \brief  reads a XVector2SOAArray  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp,bool flip_bytes=false); 

  #endif
};
// streaming ops
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XVector2SOAArray& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs,XVector2SOAArray& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector2.h"
#include "XM2/inl/Vector2SOAArray.inl"
//----------------------------------------------------------------------------------------------------------------------
#endif // XM2_VECTOR2SOA_ARRAY__H__
//----------------------------------------------------------------------------------------------------------------------
