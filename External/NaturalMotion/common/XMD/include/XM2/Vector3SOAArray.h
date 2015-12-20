//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector3SOA.h"
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
class XM2EXPORT XVector3PackedArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XVector3SOAArray
/// \brief   defines an aligned array of 3D vectors stored in structure or array format
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XVector3SOAArray 
  : public pod_vector< XVector3SOA > 
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
  XM2_INLINE void streamOut(XVector3PackedArray& input) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector3Array& input) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector4Array& input) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector3SOAArray& input) const;

  /// \brief  streams data from this class into the output array, and converts the xyz data
  ///         back into XVector3s. 
  /// \param  output - the array to stream data into
  XM2_INLINE void streamOut(XVector4SOAArray& input) const;

  /// \name  math

  /// \brief  adds an XVector3SOAArray to an array of XVector3SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOAArray& a, const XVector3SOAArray& b);

  /// \brief  adds an XVector3SOA to an XVector3SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOAArray& a, const XVector3SOA& b);

  /// \brief  adds an XVector3 to an XVector3SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOAArray& a, const XVector3& b);

  /// \brief  adds an XVector3 to an XVector3SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOAArray& a, XReal b);

  /// \brief  subtracts an XVector3SOAArray array from an array of XVector3SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOAArray& a, const XVector3SOAArray& b);

  /// \brief  subtracts an XVector3SOA from an array of XVector3SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOAArray& a, const XVector3SOA& b);

  /// \brief  subtracts an XVector3 from an array of XVector3SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOAArray& a, const XVector3& b);

  /// \brief  subtracts a float from an array of XVector3SOAArray. i.e.
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOAArray& a, XReal b);

  /// \brief  multiplies two XVector3SOAArrays. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOAArray& a, const XVector3SOAArray& b);

  /// \brief  multiplies an XVector3SOA with an XVector3SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOAArray& a, const XVector3SOA& b);

  /// \brief  multiplies an XVector3 with an XVector3SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOAArray& a, const XVector3& b);

  /// \brief  multiplies an XVector3SOAArray with a float. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOAArray& a, XReal b);

  /// \brief  divides an XVector3SOAArray by another XVector3SOAArray. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOAArray& a, const XVector3SOAArray& b);

  /// \brief  divides an XVector3SOAArray by an XVector3SOA. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOAArray& a, const XVector3SOA& b);

  /// \brief  divides an XVector3SOAArray by an XVector3. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOAArray& a, const XVector3& b);

  /// \brief  divides an XVector3SOAArray by a float. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOAArray& a, XReal b);

  /// \brief  adds an XVector3SOAArray to this XVector3SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOAArray& b);

  /// \brief  adds an XVector3SOA to this XVector3SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3SOA& b);

  /// \brief  adds an XVector3 to this XVector3SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3& b);

  /// \brief  adds a float to this XVector3SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(XReal b);

  /// \brief  subtracts an XVector3SOAArray from this XVector3SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOAArray& b);

  /// \brief  subtracts an XVector3SOA from this XVector3SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3SOA& b);

  /// \brief  subtracts an XVector3 from this XVector3SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3& b);

  /// \brief  subtracts a float from this XVector3SOAArray
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(XReal b);

  /// \brief  multiplies this XVector3SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOAArray& b);

  /// \brief  multiplies this XVector3SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3SOA& b);

  /// \brief  multiplies this XVector3SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3& b);

  /// \brief  multiplies this XVector3SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(XReal b);

  /// \brief  divides this XVector3SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOAArray& b);

  /// \brief  divides this XVector3SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3SOA& b);

  /// \brief  divides this XVector3SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3& b);

  /// \brief  divides this XVector3SOAArray with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(XReal b);

  /// \brief  normalises all the XVector3s in the array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = normalise( this[i] );
  ///         }
  /// \endcode
  XM2_INLINE void normalise();

  /// \brief  normalises all the XVector3s in the array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = normalise( v[i] );
  ///         }
  /// \endcode
  /// \param  v - the array to normalise
  XM2_INLINE void normalise(const XVector3SOAArray& v);


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
  XM2_INLINE void dot(XVector4Array& product, const XVector3& b) const;

  /// \brief  computes the dot products of 2 XVector3Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XVector4Array& product, const XVector3SOA& b) const;

  /// \brief  computes the dot products of 2 XVector3Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XVector4Array& product, const XVector3SOAArray& b) const;

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector3SOAArray& a, const XVector3& b);

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector3SOAArray& a, const XVector3SOA& b);

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector3SOAArray& a, const XVector3SOAArray& b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp /slerp

  /// \brief  linearly interpolates (LERPS) between XVector3 a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector3SOAArray& a, const XVector3SOAArray& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector3SOAArray& a,
                         const XVector3SOAArray& b,
                         const XVector3SOAArray& c,
                         const XVector3SOAArray& d,
                         XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates this XVector3SOAArray 
  XM2_INLINE void negate();

  /// \brief  negates an XVector3Array 
  /// \param  v - the XVector3SOAArray to negate
  XM2_INLINE void negate(const XVector3SOAArray& v);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOAArray operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE const XVector3SOAArray& operator=(const XVector3SOAArray& rhs);
  XM2_INLINE const XVector3SOAArray& operator=(const XVector4SOAArray& rhs);

  XM2_INLINE const XVector3SOAArray& operator*=(const XVector3SOAArray& rhs);
  XM2_INLINE const XVector3SOAArray& operator/=(const XVector3SOAArray& rhs);
  XM2_INLINE const XVector3SOAArray& operator+=(const XVector3SOAArray& rhs);
  XM2_INLINE const XVector3SOAArray& operator-=(const XVector3SOAArray& rhs);

  XM2_INLINE const XVector3SOAArray& operator*=(const XVector4SOAArray& rhs);
  XM2_INLINE const XVector3SOAArray& operator/=(const XVector4SOAArray& rhs);
  XM2_INLINE const XVector3SOAArray& operator+=(const XVector4SOAArray& rhs);
  XM2_INLINE const XVector3SOAArray& operator-=(const XVector4SOAArray& rhs);

  XM2_INLINE const XVector3SOAArray& operator*=(const XVector3SOA& rhs);
  XM2_INLINE const XVector3SOAArray& operator/=(const XVector3SOA& rhs);
  XM2_INLINE const XVector3SOAArray& operator+=(const XVector3SOA& rhs);
  XM2_INLINE const XVector3SOAArray& operator-=(const XVector3SOA& rhs);
  
  XM2_INLINE const XVector3SOAArray& operator*=(const XVector4SOA& rhs);
  XM2_INLINE const XVector3SOAArray& operator/=(const XVector4SOA& rhs);
  XM2_INLINE const XVector3SOAArray& operator+=(const XVector4SOA& rhs);
  XM2_INLINE const XVector3SOAArray& operator-=(const XVector4SOA& rhs);

  XM2_INLINE const XVector3SOAArray& operator*=(const XVector3& rhs);
  XM2_INLINE const XVector3SOAArray& operator/=(const XVector3& rhs);
  XM2_INLINE const XVector3SOAArray& operator+=(const XVector3& rhs);
  XM2_INLINE const XVector3SOAArray& operator-=(const XVector3& rhs);
  
  XM2_INLINE const XVector3SOAArray& operator*=(XReal rhs);
  XM2_INLINE const XVector3SOAArray& operator/=(XReal rhs);
  XM2_INLINE const XVector3SOAArray& operator+=(XReal rhs);
  XM2_INLINE const XVector3SOAArray& operator-=(XReal rhs);
  
  XM2_INLINE const XVector3SOAArray& operator*=(const XMatrix& rhs);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3SOAArray stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XVector3SOAArray  the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector3SOAArray  to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector3SOAArray  from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector3SOAArray  from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector3SOAArray  to stdout
  void print32() const;

  /// \brief  prints this XVector3SOAArray  to stdout
  void print64() const;

  /// \brief  writes this XVector3SOAArray  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XVector3SOAArray  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XVector3SOAArray  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XVector3SOAArray  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false); 

  #endif
};

// streaming ops
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XVector3SOAArray& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XVector3SOAArray& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector3SOAArray.h"
#include "XM2/Vector4.h"
#include "XM2/Vector4Array.h"
#include "XM2/Matrix34.h"
#include "XM2/inl/Vector3SOAArray.inl"
//----------------------------------------------------------------------------------------------------------------------
