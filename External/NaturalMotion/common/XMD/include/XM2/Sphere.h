//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/MathCommon.h"
#include "XM2/Vector3.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// Forward Declarations
//----------------------------------------------------------------------------------------------------------------------

class XM2EXPORT XRay;
class XM2EXPORT XMatrix;
class XM2EXPORT XBoundingBox;
class XM2EXPORT XVector3;
class XM2EXPORT XVector4;
class XM2EXPORT XVector3SOA;
class XM2EXPORT XVector4SOA;
class XM2EXPORT XVector3Array;
class XM2EXPORT XVector4Array;
class XM2EXPORT XVector3SOAArray;
class XM2EXPORT XVector4SOAArray;
class XM2EXPORT XVector3Packed;
class XM2EXPORT XVector3PackedArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XSphere
/// \brief   a 3D sphere 
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XSphere
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XSphere construction / destruction
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  XM2_INLINE XSphere();
  XM2_INLINE XSphere(const XVector3& c, XReal r);  
  XM2_INLINE explicit XSphere(const XSphere& src);

  //--------------------------------------------------------------------------------------------------------------------
  // XSphere comparison
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second Sphere to test
  /// \param  eps - the floating point tolerance 
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XSphere& b, XReal eps = XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second Sphere to test
  /// \param  eps - the floating point tolerance
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XSphere& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XSphere Initialisation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  clears all previous data from the Sphere, the Sphere will be set to +/- infinity
  XM2_INLINE void zero();

  /// \brief  sets the values of the sphere
  /// \param  _center - the new center position
  /// \param  _radius - the new radius
  XM2_INLINE void set(const XVector3& _center, XReal _radius);
  ///
  /// \brief  copies src into this
  /// \param  src - the sphere to cop
  XM2_INLINE void copy(const XSphere& src);

  //--------------------------------------------------------------------------------------------------------------------
  // XSphere expand / transform
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  modification

  /// \brief  transforms the bounding box by the specified matrix
  /// \param  mat - the matrix to transform it by
  XM2_INLINE void transform(const XMatrix& mat);

  /// \brief  expands the bounding box to hold the specified point
  /// \param  p - the point to include within the bounding box
  XM2_INLINE void expand(const XVector3& p);

  /// \brief  expands the bounding box to hold the specified point
  /// \param  p - the point to include within the bounding box
  XM2_INLINE void expand(const XVector4& p);

  /// \brief  expands the bounding box to hold the specified point
  /// \param  p - the point to include within the bounding box
  XM2_INLINE void expand(const XVector3SOA& p);

  /// \brief  expands the bounding box to hold the specified point
  /// \param  p - the point to include within the bounding box
  XM2_INLINE void expand(const XVector4SOA& p);

  /// \brief  expands the bounding box to hold the specified point
  /// \param  p - the point to include within the bounding box
  XM2_INLINE void expand(const XVector3Packed& p);

  /// \brief  expands the bounding box to contain the array of points specified
  /// \param  p - the point to include within the bounding box
  void expand(const XVector3Array& p);

  /// \brief  expands the bounding box to contain the array of points specified
  /// \param  p - the point to include within the bounding box
  void expand(const XVector4Array& p);

  /// \brief  expands the bounding box to contain the array of points specified
  /// \param  p - the point to include within the bounding box
  void expand(const XVector3SOAArray& p);

  /// \brief  expands the bounding box to contain the array of points specified
  /// \param  p - the point to include within the bounding box
  void expand(const XVector4SOAArray& p);

  /// \brief  expands the bounding box to contain the array of points specified
  /// \param  p - the point to include within the bounding box
  void expand(const XVector3PackedArray& p);

  /// \brief  expands Sphere so that it contains box
  /// \param  box - the Sphere that Sphere should contain
  XM2_INLINE void expand(const XSphere& box);

  /// \brief  expands Sphere so that it contains box
  /// \param  box - the bounding box that Sphere should contain
  void expand(const XBoundingBox& box);

  //--------------------------------------------------------------------------------------------------------------------
  // XSphere Test functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  intersection

  /// \brief  a query function to see if Sphere contains the specified point
  /// \param  point - the point to test
  XM2_INLINE bool contains(const XVector3& point) const;

  /// \brief  a query function to see if this bounding box contains the specified point
  /// \param  point - the point to test
  /// \return true if point is contained within this box
  XM2_INLINE bool contains(const XVector4& point) const;

  /// \brief  a query function to see if this bounding box contains the specified point
  /// \param  point - the point to test
  /// \return true if point is contained within this box
  XM2_INLINE bool contains(const XVector3Packed& point) const;

  /// \brief  returns true if the two Sphere intersect
  /// \param  b - the sphere to test
  /// \return true if they intersect
  XM2_INLINE bool intersects(const XSphere& b) const;

  /// \brief  returns true if the box and this Sphere intersect
  /// \param  b - the bounding box to test
  /// \param  collision_normal - the returned collision normal
  /// \param  collision_depth - the returned collision depth
  /// \return true if they intersect
  XM2_INLINE bool intersects(const XBoundingBox& b, XVector3& collision_normal, XReal& collision_depth) const;

  /// \brief  tests to see if the specified ray intersects with this sphere. If it does,
  ///         then it sticks the intersect point into the returned intersect_value
  /// \param  b - the ray to test
  /// \param  intersect_value - the returned intersect value
  /// \return true if they intersect
  XM2_INLINE bool intersects(const XRay& b, XReal& intersect_value) const;
  
  /// \brief  tests to see if the specified ray intersects with this sphere. If it does,
  ///         then it sticks the intersect point into the returned intersect_value
  /// \param  b - the ray to test
  /// \param  intersect_point - the returned intersect point
  /// \return true if they intersect
  XM2_INLINE bool intersects(const XRay& b, XVector3& intersect_point) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XSphere query functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  getters

  /// \brief  returns the radius of the sphere
  /// \return the box width
  XM2_INLINE XReal getRadius() const;

  /// \brief  returns the radius of the sphere
  /// \return the box width
  XM2_INLINE XReal getRadiusSquared() const;

  /// \brief  returns the radius of the sphere
  /// \return the box width
  XM2_INLINE XReal getCircumference() const;

  /// \brief  returns the radius of the sphere
  /// \return the box width
  XM2_INLINE void setRadius(XReal r);

  /// \brief  returns the radius of the sphere
  /// \return the box width
  XM2_INLINE void setRadiusSquared(XReal r);

  //--------------------------------------------------------------------------------------------------------------------
  // XSphere stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XSphere the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XSphere to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XSphere from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XSphere from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XSphere to stdout
  void print32() const;

  /// \brief  prints this XSphere to stdout
  void print64() const;

  /// \brief  writes this XSphere to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XSphere to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XSphere from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XSphere from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XSphere operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE XSphere& operator=(const XSphere& other);
  XM2_INLINE bool operator==(const XSphere& other) const;
  XM2_INLINE bool operator!=(const XSphere& other) const;
  
  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS
    
  //--------------------------------------------------------------------------------------------------------------------
  // member variables
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  data

  /// center point of the sphere
  XVector3 center;

private:
  
  // hiding the radius attr since a fairly big optimization is to make the radius an 
  // SSE intrinsic type...
  //
  #if defined(XM2_USE_SSE) && !defined(DOXYGEN)
    #if XM2_USE_FLOAT
      __m128 radius;
    #else
      __m128d radius;
    #endif
  #else
    XReal radius;
  #endif
}
XM2_ALIGN_SUFFIX(16);

#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XSphere& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XSphere& c);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Matrix34.h"
#include "XM2/Ray.h"
#include "XM2/inl/Sphere.inl"
#if XM2_SSE
# include "XM2/inl/sse/Sphere.inl"
#elif XM2_SSE2
# include "XM2/inl/sse2/Sphere.inl"
#elif XM2_FPU
# include "XM2/inl/vanilla/Sphere.inl"
#else
# error Unknown CPU architecture
#endif
//----------------------------------------------------------------------------------------------------------------------
