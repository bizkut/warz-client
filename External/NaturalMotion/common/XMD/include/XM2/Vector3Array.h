//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector3.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
class XM2EXPORT XRealArray;
class XM2EXPORT XVector3Packed;
class XM2EXPORT XVector3PackedArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XVector3Array 
/// \brief   defines an aligned array of XVector3s
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XVector3Array 
  : public pod_vector<XVector3> 
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
  XM2_INLINE void dot(XRealArray& product, const XVector3Array& b) const;

  /// \brief  computes the dot products of 2 XVector3Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XRealArray& product, const XVector3& b) const;

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector3Array& a, const XVector3Array& b);

  /// \brief  normalises all the XVector3s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = cross( a[i], b[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void cross(const XVector3Array& a, const XVector3& b);

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
  XM2_INLINE void normalise(const XVector3Array& v);

  //--------------------------------------------------------------------------------------------------------------------
  // dot & cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds an XVector3 to an array of XVector3s. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3Array& a, const XVector3Array& b);

  /// \brief  adds an XVector3 to an XVector3. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3Array& a, const XVector3& b);

  /// \brief  adds an XVector3 to a float. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3Array& a, XReal b);

  /// \brief  subtracts an XVector3 array from an array of XVector3s. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3Array& a, const XVector3Array& b);

  /// \brief  subtracts an XVector3 from an array of XVector3s. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3Array& a, const XVector3& b);

  /// \brief  subtracts a float from an array of XVector3s. i.e.
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3Array& a, XReal b);

  /// \brief  multiplies two XVector3 arrays. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3Array& a, const XVector3Array& b);

  /// \brief  multiplies an XVector3 with an array of XVectors. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3Array& a, const XVector3& b);

  /// \brief  multiplies an array of XVector3 with a float. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3Array& a, XReal b);

  /// \brief  divides an array of XVector3s by another array of XVector3s. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3Array& a, const XVector3Array& b);

  /// \brief  divides an array of XVector3s by an XVector3. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3Array& a, const XVector3& b);

  /// \brief  divides an array of XVector3s by a float. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3Array& a, XReal b);

  /// \brief  adds an XVector3Array to this XVector3Array
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3Array& b);

  /// \brief  adds an XVector3 to this XVector3Array
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector3& b);

  /// \brief  adds a float to this XVector3Array
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  ///
  XM2_INLINE void add(XReal b);

  /// \brief  subtracts an XVector3Array from this XVector3Array
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  ///
  XM2_INLINE void sub(const XVector3Array& b);

  /// \brief  subtracts an XVector3 from this XVector3Array
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector3& b);

  /// \brief  subtracts a float from this XVector3Array
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(XReal b);

  /// \brief  multiplies this XVector3Array with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3Array& b);

  /// \brief  multiplies this XVector3Array with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector3& b);

  /// \brief  multiplies this XVector3Array with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(XReal b);

  /// \brief  divides this XVector3Array with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3Array& b);

  /// \brief  divides this XVector3Array with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector3& b);

  /// \brief  divides this XVector3Array with b
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(XReal b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp /slerp

  /// \brief  linearly interpolates (LERPS) between XVector3 a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector3Array& a, const XVector3Array& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector3Array& a,
                         const XVector3Array& b,
                         const XVector3Array& c,
                         const XVector3Array& d,
                         XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3 Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates this XVector3Array 
  XM2_INLINE void negate();

  /// \brief  negates an XVector3Array 
  /// \param  v - the XVector3Array to negate
  XM2_INLINE void negate(const XVector3Array& v);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector3Array operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE const XVector3Array& operator*=(const XVector3Array& rhs);
  XM2_INLINE const XVector3Array& operator/=(const XVector3Array& rhs);
  XM2_INLINE const XVector3Array& operator+=(const XVector3Array& rhs);
  XM2_INLINE const XVector3Array& operator-=(const XVector3Array& rhs);

  XM2_INLINE const XVector3Array& operator*=(const XVector3& rhs);
  XM2_INLINE const XVector3Array& operator/=(const XVector3& rhs);
  XM2_INLINE const XVector3Array& operator+=(const XVector3& rhs);
  XM2_INLINE const XVector3Array& operator-=(const XVector3& rhs);

  XM2_INLINE const XVector3Array& operator*=(XReal rhs);
  XM2_INLINE const XVector3Array& operator/=(XReal rhs);
  XM2_INLINE const XVector3Array& operator+=(XReal rhs);
  XM2_INLINE const XVector3Array& operator-=(XReal rhs);
  
  //--------------------------------------------------------------------------------------------------------------------
  // XVector3Array stdio
  //--------------------------------------------------------------------------------------------------------------------

  #if !XM2_NO_C_IO

  /// \name  file IO

  /// \brief  prints this XVector3Array  the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector3Array  to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector3Array  from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector3Array  from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector3Array  to stdout
  void print32() const;

  /// \brief  prints this XVector3Array  to stdout
  void print64() const;

  /// \brief  writes this XVector3Array  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XVector3Array  to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XVector3Array  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XVector3Array  from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false); 

  #endif
};
// streaming ops
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XVector3Array& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XVector3Array& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/RealArray.h"
#include "XM2/inl/Vector3Array.inl"
//----------------------------------------------------------------------------------------------------------------------
