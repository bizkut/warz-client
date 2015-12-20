//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/MathCommon.h"
#include "XM2/pod_vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XRealArray 
/// \brief   defines an aligned float array. The array data is aligned to 16bytes, and processed using SSE
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XRealArray 
  : public pod_vector< XReal > 
{
public:

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XRealArray() : pod_vector<XReal>()
  {
  }

  /// \brief  copy ctor
  XM2_INLINE XRealArray(const XRealArray& rhs) 
    : pod_vector<XReal>()
  {
    resize(rhs.size());
    memcpy(buffer(),rhs.buffer(),sizeof(XReal)*size());
  }

  /// \brief  sets all elements to zero
  void zero();

  //--------------------------------------------------------------------------------------------------------------------
  // XRealArray comparison
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  return (this == b)
  /// \param  b - second array to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  bool equal(const XRealArray& b, XReal eps = XM2_FLOAT_ZERO) const;

  ///
  /// \brief  return (this != b)
  /// \param  b - second array to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are NOT the same, false otherwise
  bool notEqual(const XRealArray& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XRealArray operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE const XRealArray& operator*=(const XRealArray& rhs);
  XM2_INLINE const XRealArray& operator/=(const XRealArray& rhs);
  XM2_INLINE const XRealArray& operator+=(const XRealArray& rhs);
  XM2_INLINE const XRealArray& operator-=(const XRealArray& rhs);
  XM2_INLINE const XRealArray& operator*=(XReal rhs);
  XM2_INLINE const XRealArray& operator/=(XReal rhs);
  XM2_INLINE const XRealArray& operator+=(XReal rhs);
  XM2_INLINE const XRealArray& operator-=(XReal rhs);
  XM2_INLINE bool operator==(const XRealArray& other) const;
  XM2_INLINE bool operator!=(const XRealArray& other) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XRealArray / XRealArray arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  arithmetic

  /// \brief  multiplies all elements of a by all elements of b and stores result in this array
  /// \param  a - 
  /// \param  b - 
  XM2_INLINE void mul(const XRealArray& a, const XRealArray& b);

  /// \brief  divides all elements of a by all elements of b and stores result in this array
  /// \param  a - 
  /// \param  b - 
  XM2_INLINE void div(const XRealArray& a, const XRealArray& b);

  /// \brief  subtracts all elements of a by all elements of b and stores result in this array
  /// \param  a - 
  /// \param  b - 
  XM2_INLINE void sub(const XRealArray& a, const XRealArray& b);

  /// \brief  adds all elements of a by all elements of b and stores result in this array
  /// \param  a - 
  /// \param  b - 
  XM2_INLINE void add(const XRealArray& a, const XRealArray& b);

  /// \brief  multiplies all elements of this array, by all elements of b, and stores 
  ///         the result in this array
  /// \param  a - 
  XM2_INLINE void mul(const XRealArray& a);

  /// \brief  divides all elements of this array, by all elements of b, and stores 
  ///         the result in this array
  /// \param  a - 
  XM2_INLINE void div(const XRealArray& a);

  /// \brief  subtracts all elements of this array, by all elements of b, and stores 
  ///         the result in this array
  /// \param  a - 
  XM2_INLINE void sub(const XRealArray& a);

  /// \brief  adds all elements of this array, by all elements of b, and stores 
  ///         the result in this array
  /// \param  a - 
  XM2_INLINE void add(const XRealArray& a);

  //--------------------------------------------------------------------------------------------------------------------
  // XRealArray / XReal arithmetic
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  multiplies all elements of a by b, and stores the result in this array
  /// \param  a -
  /// \param  b -
  XM2_INLINE void mul(const XRealArray& a, XReal b);

  /// \brief  divides  all elements of a by b, and stores the result in this array
  /// \param  a - 
  /// \param  b - 
  XM2_INLINE void div(const XRealArray& a, XReal b);

  /// \brief  subtracts  all elements of a by b, and stores the result in this array
  /// \param  a - 
  /// \param  b - 
  XM2_INLINE void sub(const XRealArray& a, XReal b);

  /// \brief  adds  all elements of a by b, and stores the result in this array
  /// \param  a - 
  /// \param  b - 
  XM2_INLINE void add(const XRealArray& a, XReal b);

  /// \brief  multiplies every element in this array by a
  /// \param  a - 
  XM2_INLINE void mul(const XReal& a);

  /// \brief  divides every element of this array by a
  /// \param  a - 
  XM2_INLINE void div(const XReal& a);

  /// \brief  subtracts a from every element in this array 
  /// \param  a - 
  XM2_INLINE void sub(const XReal& a);

  /// \brief  adds a to every element in this array 
  /// \brief  adds  all elements of a by b, and stores the result in this array
  /// \param  a - 
  XM2_INLINE void add(const XReal& a);

  //--------------------------------------------------------------------------------------------------------------------
  // XRealArray stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XRealArray the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XRealArray to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XRealArray from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XRealArray from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XRealArray to stdout
  void print32() const;

  /// \brief  prints this XRealArray to stdout
  void print64() const;

  /// \brief  writes this XRealArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XRealArray to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XRealArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XRealArray from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif

  //------------------------------------------------------------------------------------
  // raw array methods
  //------------------------------------------------------------------------------------

  /// \name  static array methods

  /// \brief  multiplies a and b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static void arrayMul(XReal* output, const XReal* a, const XReal* b, int num);

  /// \brief  divides a by b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static void arrayDiv(XReal* output, const XReal* a, const XReal* b, int num);

  /// \brief  subtracts b from a, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static void arrayAdd(XReal* output, const XReal* a, const XReal* b, int num);

  /// \brief  adds a and b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static void arraySub(XReal* output, const XReal* a, const XReal* b, int num);

  /// \brief  multiplies a and b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static void arrayMul(XReal* output, const XReal* a, XReal b, int num);

  /// \brief  divides a by b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static void arrayDiv(XReal* output, const XReal* a, XReal b, int num);

  /// \brief  subtracts b from a, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static void arrayAdd(XReal* output, const XReal* a, XReal b, int num);

  /// \brief  adds a and b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static void arraySub(XReal* output, const XReal* a, XReal b, int num);

  /// \brief  multiplies a and b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static XM2_INLINE void arrayMul(XReal* output, const XReal* b, int num);

  /// \brief  divides a by b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static XM2_INLINE void arrayDiv(XReal* output, const XReal* b, int num);

  /// \brief  subtracts b from a, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static XM2_INLINE void arrayAdd(XReal* output, const XReal* b, int num);

  /// \brief  adds a and b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static XM2_INLINE void arraySub(XReal* output, const XReal* b, int num);

  /// \brief  multiplies a and b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static XM2_INLINE void arrayMul(XReal* output, XReal b, int num);

  /// \brief  divides a by b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static XM2_INLINE void arrayDiv(XReal* output, XReal b, int num);

  /// \brief  subtracts b from a, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static XM2_INLINE void arrayAdd(XReal* output, XReal b, int num);

  /// \brief  adds a and b, stores in this
  /// \param  output - the output array
  /// \param  a - input 1
  /// \param  b - input 2
  /// \param  num - number of elements in the array
  static XM2_INLINE void arraySub(XReal* output, XReal b, int num);
};
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/inl/RealArray.inl"
//----------------------------------------------------------------------------------------------------------------------
