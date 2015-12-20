//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector2.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
class XM2EXPORT XRealArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XVector2Array 
/// \brief   a class to hold an array of  XVector2s
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XVector2Array 
  : public pod_vector<XVector2> 
{
public:

  /// \name  construction 

  /// \brief  ctor
  XVector2Array() : pod_vector<XVector2>() {}

  /// \brief  copy ctor
  /// \param  v - the array to copy 
  XVector2Array(const XVector2Array& v) : pod_vector<XVector2>(v) {}

  //--------------------------------------------------------------------------------------------------------------------
  // initialise
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  sets all elements in the array to zero
  XM2_INLINE void zero();

  //--------------------------------------------------------------------------------------------------------------------
  // dot & cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  math operations

  /// \brief  computes the dot products of 2 XVector2Arrays and returns the result as
  ///         a real array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           product[i] = dot( this[i], b[i] );
  ///         }
  /// \endcode
  XM2_INLINE void dot(XRealArray& product, const XVector2Array& b) const;

  //--------------------------------------------------------------------------------------------------------------------
  // dot & cross products
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  normalises all the XVector2s in the array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = normalise( this[i] );
  ///         }
  /// \endcode
  XM2_INLINE void normalise();

  /// \brief  normalises all the XVector2s in the input array. i.e.
  /// \code   
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = normalise( v[i] );
  ///         }
  /// \endcode
  /// \param  v - the XVector3 array to normalise
  XM2_INLINE void normalise(const XVector2Array& v);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2 maths
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  adds 2 vec2 arrays together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2Array& a, const XVector2Array& b);

  /// \brief  adds a vec to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2Array& a, const XVector2& b);

  /// \brief  adds a float to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] + b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2Array& a, XReal b);

  /// \brief  adds 2 vec2 arrays together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2Array& a, const XVector2Array& b);

  /// \brief  adds a vec to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2Array& a, const XVector2& b);

  /// \brief  adds a float to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] - b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2Array& a, XReal b);

  /// \brief  adds 2 vec2 arrays together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2Array& a, const XVector2Array& b);

  /// \brief  adds a vec to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2Array& a, const XVector2& b);

  /// \brief  adds a float to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] * b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2Array& a, XReal b);

  /// \brief  adds 2 vec2 arrays together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b[i];
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2Array& a, const XVector2Array& b);

  /// \brief  adds a vec to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           output[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2Array& a, const XVector2& b);

  /// \brief  adds a float to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] = a[i] / b;
  ///         }
  /// \endcode
  /// \param  a - input arg 0
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2Array& a, XReal b);

  /// \brief  adds 2 vec2 arrays together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2Array& b);

  /// \brief  adds a vec to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(const XVector2& b);

  /// \brief  adds a float to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           output[i] += b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void add(XReal b);

  /// \brief  adds 2 vec2 arrays together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2Array& b);

  /// \brief  adds a vec to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(const XVector2& b);

  /// \brief  adds a float to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] -= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void sub(XReal b);

  /// \brief  adds 2 vec2 arrays together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2Array& b);

  /// \brief  adds a vec to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(const XVector2& b);

  /// \brief  adds a float to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] *= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void mul(XReal b);

  /// \brief  adds 2 vec2 arrays together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b[i];
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2Array& b);

  /// \brief  adds a vec to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(const XVector2& b);

  /// \brief  adds a float to a vec2 array together
  /// \code
  ///         for(i=0;i<size();++i)
  ///         {
  ///           this[i] /= b;
  ///         }
  /// \endcode
  /// \param  b - input arg 1
  XM2_INLINE void div(XReal b);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2Array Lerps and Slerps
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  lerp / slerp

  /// \brief  linearly interpolates (LERPS) between XVector3 a and b and stores in this
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void lerp(const XVector2Array& a, const XVector2Array& b, XReal t);

  /// \brief  interpolates using a bezier function between a,b,c and d. 
  /// \param  a - the first value
  /// \param  b - the 2nd value
  /// \param  c - the 3rd value
  /// \param  d - the 4th value
  /// \param  t - the 0 to 1 interpolation control value
  XM2_INLINE void bezier(const XVector2Array& a,
                         const XVector2Array& b,
                         const XVector2Array& c,
                         const XVector2Array& d,
                         const XReal t);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2Array Negation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  negation

  /// \brief  negates this XVector3Array 
  XM2_INLINE void negate();

  /// \brief  negates an XVector3Array 
  /// \param  v - the XVector3Array to negate
  XM2_INLINE void negate(const XVector2Array& v);

  //--------------------------------------------------------------------------------------------------------------------
  // XVector2Array operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE const XVector2Array& operator*=(const XVector2Array& rhs);
  XM2_INLINE const XVector2Array& operator/=(const XVector2Array& rhs);
  XM2_INLINE const XVector2Array& operator+=(const XVector2Array& rhs);
  XM2_INLINE const XVector2Array& operator-=(const XVector2Array& rhs);

  XM2_INLINE const XVector2Array& operator*=(const XVector2& rhs);
  XM2_INLINE const XVector2Array& operator/=(const XVector2& rhs);
  XM2_INLINE const XVector2Array& operator+=(const XVector2& rhs);
  XM2_INLINE const XVector2Array& operator-=(const XVector2& rhs);

  XM2_INLINE const XVector2Array& operator*=(XReal rhs);
  XM2_INLINE const XVector2Array& operator/=(XReal rhs);
  XM2_INLINE const XVector2Array& operator+=(XReal rhs);
  XM2_INLINE const XVector2Array& operator-=(XReal rhs);
  
  //--------------------------------------------------------------------------------------------------------------------
  // XVector2Array stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XVector2Array the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XVector2Array to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XVector2Array from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XVector2Array from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XVector2Array to stdout
  void print32() const;

  /// \brief  prints this XVector2Array to stdout
  void print64() const;

  /// \brief  writes this XVector2Array to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XVector2Array to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XVector2Array from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XVector2Array from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif
};
// streaming ops
#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XVector2Array& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XVector2Array& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/RealArray.h"
#include "XM2/inl/Vector2Array.inl"
//----------------------------------------------------------------------------------------------------------------------
