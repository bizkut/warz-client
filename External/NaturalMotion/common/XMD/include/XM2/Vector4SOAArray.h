//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector4SOA.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// Forward Declarations
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XVector3;
class XM2EXPORT XVector4;
class XM2EXPORT XMatrix;
class XM2EXPORT XVector3Array;
class XM2EXPORT XVector4Array;
class XM2EXPORT XVector3SOAArray;
class XM2EXPORT XVector4SOAArray;
class XM2EXPORT XMatrix;
class XM2EXPORT XVector3PackedArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XVector4SOAArray
/// \brief   defines an aligned vector4 SOA array (std::vector) type
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XVector4SOAArray 
  : public pod_vector< XVector4SOA > 
{
public:

  /// \name  streaming / swizzling

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector3PackedArray& input);

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector3Array& input);

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector4Array& input);

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector3SOAArray& input);

  /// \brief  streams data in from the input array, swizzles it, and stores it in the 
  ///         SOA array.
  /// \param  input - the array to stream in
  XM2_INLINE void streamIn(const XVector4SOAArray& input);

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector3PackedArray& output) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector3Array& output) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector4Array& output) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector3SOAArray& output) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector4SOAArray& output) const;

  /// \name  math

  /// \brief  adds an XVector4SOAArray to an array of XVector4SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOAArray& a, const XVector4SOAArray& b);

  /// \brief  adds an XVector4SOA to an XVector4SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOAArray& a, const XVector4SOA& b);

  /// \brief  adds an XVector4 to an XVector4SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOAArray& a, const XVector4& b);

  /// \brief  adds an XVector3 to an XVector4SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOAArray& a, XReal b);

  /// \brief  subtracts an XVector4SOAArray array from an array of XVector4SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOAArray& a, const XVector4SOAArray& b);

  /// \brief  subtracts an XVector4SOA from an array of XVector4SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOAArray& a, const XVector4SOA& b);

  /// \brief  subtracts an XVector4 from an array of XVector4SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOAArray& a, const XVector4& b);

  /// \brief  subtracts a float from an array of XVector4SOAArray. i.e.
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOAArray& a, XReal b);

  /// \brief  multiplies two XVector4SOAArrays. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOAArray& a, const XVector4SOAArray& b);

  /// \brief  multiplies an XVector4SOA with an XVector4SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOAArray& a, const XVector4SOA& b);

  /// \brief  multiplies an XVector4 with an XVector4SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOAArray& a, const XVector4& b);

  /// \brief  multiplies an XVector4SOAArray with a float. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOAArray& a, XReal b);

  /// \brief  divides an XVector4SOAArray by another XVector4SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOAArray& a, const XVector4SOAArray& b);

  /// \brief  divides an XVector4SOAArray by an XVector4SOA. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOAArray& a, const XVector4SOA& b);

  /// \brief  divides an XVector4SOAArray by an XVector4. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOAArray& a, const XVector4& b);

  /// \brief  divides an XVector4SOAArray by a float. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOAArray& a, XReal b);

  /// \brief  adds an XVector4SOAArray to this XVector4SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOAArray& b);

  /// \brief  adds an XVector4SOA to this XVector4SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4SOA& b);

  /// \brief  adds an XVector4 to this XVector4SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector4& b);

  /// \brief  adds a float to this XVector4SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(XReal b);

  /// \brief  subtracts an XVector4SOAArray from this XVector4SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOAArray& b);

  /// \brief  subtracts an XVector3SOA from this XVector4SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4SOA& b);

  /// \brief  subtracts an XVector4 from this XVector4SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector4& b);

  /// \brief  subtracts a float from this XVector4SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(XReal b);

  /// \brief  multiplies this XVector4SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOAArray& b);

  /// \brief  multiplies this XVector4SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4SOA& b);

  /// \brief  multiplies this XVector4SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector4& b);

  /// \brief  multiplies this XVector4SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(XReal b);

  /// \brief  divides this XVector4SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOAArray& b);

  /// \brief  divides this XVector4SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4SOA& b);

  /// \brief  divides this XVector4SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector4& b);

  /// \brief  divides this XVector4SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(XReal b);

  /// \brief  normalises all the XVector4SOAs in the array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = normalise( this[i] );
  ///         }
  /// \endcode
  XM2_INLINE void normalise();

  /// \brief  normalises all the XVector4SOAs in the array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = normalise( v[i] );
  ///         }
  /// \endcode
  /// \param  v - the array to normalise
  XM2_INLINE void normalise(const XVector4SOAArray& v);


  //------------------------------------------------------------------------------------
  // initialise
  //------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  sets all elements in the array to zero
  XM2_INLINE void zero();

  //------------------------------------------------------------------------------------
  // dot & cross products
  //------------------------------------------------------------------------------------

  /// \name  dot / cross

  /// \brief  computes the dot products of 2 XVector3Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XVector4Array& product, const XVector4& b) const;

  /// \brief  computes the dot products of 2 XVector3Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XVector4Array& product, const XVector4SOA& b) const;

  /// \brief  computes the dot products of 2 XVector3Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XVector4Array& product, const XVector4SOAArray& b) const;

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector4SOAArray& a, const XVector4& b);

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector4SOAArray& a, const XVector4SOA& b);

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector4SOAArray& a, const XVector4SOAArray& b);

  //------------------------------------------------------------------------------------
  // XVector3 Lerps and Slerps
  //------------------------------------------------------------------------------------

  /// \name  lerp /slerp

  /// \brief  linearly interpolates (LERPS) between XVector3 a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector4SOAArray& a, const XVector4SOAArray& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector4SOAArray& a,
                         const XVector4SOAArray& b,
                         const XVector4SOAArray& c,
                         const XVector4SOAArray& d,
                         XReal t);

  //------------------------------------------------------------------------------------
  // XVector3 Negation
  //------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates this XVector3Array 
  XM2_INLINE void negate();

  /// \brief  negates an XVector3Array 
  /// \param  v - the XVector3Array to negate
  XM2_INLINE void negate(const XVector4SOAArray& v);

  //------------------------------------------------------------------------------------
  // XVector4SOAArray operators
  //------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE const XVector4SOAArray& operator=(const XVector4SOAArray& rhs);
  XM2_INLINE const XVector4SOAArray& operator*=(const XVector4SOAArray& rhs);
  XM2_INLINE const XVector4SOAArray& operator/=(const XVector4SOAArray& rhs);
  XM2_INLINE const XVector4SOAArray& operator+=(const XVector4SOAArray& rhs);
  XM2_INLINE const XVector4SOAArray& operator-=(const XVector4SOAArray& rhs);
  XM2_INLINE const XVector4SOAArray& operator*=(const XVector4SOA& rhs);
  XM2_INLINE const XVector4SOAArray& operator/=(const XVector4SOA& rhs);
  XM2_INLINE const XVector4SOAArray& operator+=(const XVector4SOA& rhs);
  XM2_INLINE const XVector4SOAArray& operator-=(const XVector4SOA& rhs);
  XM2_INLINE const XVector4SOAArray& operator*=(const XVector4& rhs);
  XM2_INLINE const XVector4SOAArray& operator/=(const XVector4& rhs);
  XM2_INLINE const XVector4SOAArray& operator+=(const XVector4& rhs);
  XM2_INLINE const XVector4SOAArray& operator-=(const XVector4& rhs);
  XM2_INLINE const XVector4SOAArray& operator*=(XReal rhs);
  XM2_INLINE const XVector4SOAArray& operator/=(XReal rhs);
  XM2_INLINE const XVector4SOAArray& operator+=(XReal rhs);
  XM2_INLINE const XVector4SOAArray& operator-=(XReal rhs);
  XM2_INLINE const XVector4SOAArray& operator*=(const XMatrix& rhs);

  //------------------------------------------------------------------------------------
  // XVector4SOAArray stdio
  //------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XVector4SOAArray the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector4SOAArray to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector4SOAArray from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector4SOAArray from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector4SOAArray to stdout
  void print32() const;

  /// \brief  prints this XVector4SOAArray to stdout
  void print64() const;

  /// \brief  writes this XVector4SOAArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes=false) const;

  /// \brief  writes this XVector4SOAArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes=false) const;

  /// \brief  reads a XVector4SOAArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes=false);

  /// \brief  reads a XVector4SOAArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes=false); 

  #endif
};

// streaming ops
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XVector4SOAArray& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XVector4SOAArray& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector4.h"
#include "XM2/Matrix34.h"
#include "XM2/Vector4Array.h"
#include "XM2/inl/Vector4SOAArray.inl"
//----------------------------------------------------------------------------------------------------------------------
