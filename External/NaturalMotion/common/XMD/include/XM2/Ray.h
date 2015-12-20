//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/MathCommon.h"
#include "XM2/Vector3.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// forward declarations 
//----------------------------------------------------------------------------------------------------------------------
class XM2EXPORT XMatrix;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XMD::XRay
/// \brief   a 3D ray 
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XRay
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XRay ctors
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XRay();
  
  /// \brief  copy ctor
  /// \param  p - the ray to copy
  XM2_INLINE XRay(const XRay& p);
  
  /// \brief  ctor
  /// \param  _origin - ray origin
  /// \param  _dir - ray direction
  XM2_INLINE XRay(const XVector3& _origin,const XVector3& _dir);
  
  /// \brief  ctor
  /// \param  ox - origin x coordinate
  /// \param  oy - origin y coordinate
  /// \param  oz - origin z coordinate
  /// \param  dx - direction x component
  /// \param  dy - direction y component
  /// \param  dz - direction z component
  XM2_INLINE XRay(XReal ox, XReal oy, XReal oz,
                  XReal dx, XReal dy, XReal dz);

  //--------------------------------------------------------------------------------------------------------------------
  // XRay initialisation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  initialises the ray a to 0
  XM2_INLINE void zero();

  /// \brief  initialises the ray
  /// \param  origin - the ray origin
  /// \param  dir - the ray direction
  XM2_INLINE void set(const XVector3& origin, const XVector3& dir);

  //--------------------------------------------------------------------------------------------------------------------
  // XRay modification
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  ray modification

  /// \brief  negates the ray direction
  XM2_INLINE void negate();

  /// \brief  transforms this ray by the specified matrix
  /// \param  m - the matrix to transform this ray by
  XM2_INLINE void transform(const XMatrix& m);
   
  //--------------------------------------------------------------------------------------------------------------------
  // XRay Comparisons
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second ray to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XRay& b, XReal eps = XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second ray to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XRay& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XRay stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XRay the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XRay to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XRay from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XRay from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XRay to stdout
  void print32() const;

  /// \brief  prints this XRay to stdout
  void print64() const;

  /// \brief  writes this XRay to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XRay to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XRay from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XRay from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  
  
  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XRay operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE const XRay& operator = (const XRay& p);
  XM2_INLINE bool operator == (const XRay& p) const;
  XM2_INLINE bool operator != (const XRay& p) const;
  
  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS
    
  /// \name  data

  /// ray origin
  XVector3 origin;

  /// ray direction
  XVector3 direction;
} 
XM2_ALIGN_SUFFIX(16);

XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XRay& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XRay& c);
};
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Matrix34.h"
#include "XM2/inl/Ray.inl"
//----------------------------------------------------------------------------------------------------------------------
