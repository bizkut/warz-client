//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/MathCommon.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------------------------------------------------

class XM2EXPORT XVector3;
class XM2EXPORT XRay;
class XM2EXPORT XSphere;

//----------------------------------------------------------------------------------------------------------------------
/// \class   XM2::XPlane
/// \brief   A 3d plane primitive
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XPlane
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XPlane ctors
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XPlane();

  /// \brief  copy ctor
  /// \param  p - the plane to copy
  XM2_INLINE XPlane(const XPlane& p);

  /// \brief  ctor - constructs plane from 3 points on the plane
  /// \param  p0 - 1st of 3 points
  /// \param  p1 - 2nd of 3 points
  /// \param  p2 - 3rd of 3 points
  XM2_INLINE XPlane(const XVector3& p0, const XVector3& p1, const XVector3& p2);

  /// \brief  ctor - construct the plane from a point and normal
  /// \param  point - point on plane
  /// \param  norm - the normal vector
  XM2_INLINE XPlane(const XVector3& point, const XVector3& norm);

  /// \brief  ctor - constructs from 4 components
  /// \param  _a - a plane component
  /// \param  _b - b plane component
  /// \param  _c - c plane component
  /// \param  _d - d plane component
  XM2_INLINE XPlane(XReal _a, XReal _b, XReal _c, XReal _d);
    
  //--------------------------------------------------------------------------------------------------------------------
  // XPlane Initialisation funcs
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  initialisation

  /// \brief  initialises the Plane a to 0,0,0
  XM2_INLINE void zero();

  /// \brief  sets the values in the specified Plane
  /// \param  a - the x normal component
  /// \param  b - the y normal component
  /// \param  c - the z normal component
  /// \param  d - the distance component
  XM2_INLINE void set(XReal a, XReal b, XReal c, XReal d);

  /// \brief  constructs a plane from 3 points
  /// \param  p0 - first point
  /// \param  p1 - second point
  /// \param  p2 - third point
  XM2_INLINE void constructFromPoints(const XVector3& p0, const XVector3& p1, const XVector3& p2);
  
  /// \brief  constructs a plane from a point and a normal
  /// \param  point - a point on the plane
  /// \param  norm - the normal vector of the plane
  XM2_INLINE void constructFromPointAndNormal(const XVector3& point, const XVector3& norm);

  /// \brief  sets the values in the specified Plane
  /// \param  ptr - the data to read from
  XM2_INLINE void set(const XReal* ptr);
  
  /// \brief  gets the normal vector from the specified Plane
  /// \param  norm - the output normal
  XM2_INLINE void normal(XVector3& norm);
  
  /// \brief  computes the distance from point to the plane
  /// \param  point - the point to test
  /// \return the distance between the point and the plane
  XM2_INLINE XReal distance(const XVector3& point);
  
  /// \brief  intersects a ray with the plane
  /// \param  ray - the ray to intersect with this plane
  /// \param  intersect - the intersect value
  /// \return true if the ray intersects the plane
  XM2_INLINE bool lineIntersect(const XRay& ray, XReal& intersect);

  /// \brief  intersects a ray with the plane
  /// \param  ray - the ray to intersect with this plane
  /// \param  outpoint - the returned intersection point
  /// \return true if the ray intersects the plane
  XM2_INLINE bool lineIntersectPoint(const XRay& ray, XVector3& outpoint);

  //--------------------------------------------------------------------------------------------------------------------
  // XPlane comparison
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  equality

  /// \brief  this == b
  /// \param  b - second plane to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XPlane& b, XReal eps = XM2_FLOAT_ZERO) const;
  
  /// \brief  this != b
  /// \param  b - second plane to test
  /// \param  eps - floating point error tolerance
  /// \return true if this and b are not the same, false otherwise
  XM2_INLINE bool notEqual(const XPlane& b, XReal eps = XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XPlane stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this XPlane the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this XPlane to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a XPlane from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a XPlane from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this XPlane to stdout
  void print32() const;

  /// \brief  prints this XPlane to stdout
  void print64() const;

  /// \brief  writes this XPlane to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes = false) const;

  /// \brief  writes this XPlane to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes = false) const;

  /// \brief  reads a XPlane from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes = false);

  /// \brief  reads a XPlane from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes = false);  

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XPlane stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  /// \brief  assignment operator
  /// \param  p - the plane to copy
  /// \return *this
  XM2_INLINE const XPlane& operator = (const XPlane& p);

  /// \brief  equality operator
  /// \param  p - the plane to compare against
  /// \return true if the same
  XM2_INLINE bool operator == (const XPlane& p) const;

  /// \brief  in-equality operator
  /// \param  p - the plane to compare against
  /// \return true if not the same
  XM2_INLINE bool operator != (const XPlane& p) const;
  
  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  /// \name  data

  //--------------------------------------------------------------------------------------------------------------------
  // Member variables
  //--------------------------------------------------------------------------------------------------------------------
  #ifdef DOXYGEN
    /// x component of normal
    XReal a; 
    /// y component of normal 
    XReal b;
    /// z component of normal
    XReal c;
    /// distance from origin
    XReal d;
    /// data stored as an array
    XReal data[4];
  #else
  union 
  {
    struct 
    {
      XReal a; 
      XReal b;
      XReal c;
      XReal d;
    };
    XReal data[4];
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        __m128 sse;
      #else
        struct 
        {
          __m128d ab;
          __m128d cd;
        };
      #endif
    #endif
  };
  #endif
}
XM2_ALIGN_SUFFIX(16);

#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& ofs, const XPlane& c);
XM2EXPORT std::istream& operator>>(std::istream& ofs, XPlane& c);
#endif
};
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Ray.h"
#include "XM2/inl/Plane.inl"
#ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
    #include "XM2/inl/sse/Plane.inl"
  #else
    #include "XM2/inl/sse2/Plane.inl"
  #endif
#else
  #include "XM2/inl/vanilla/Plane.inl"
#endif
//----------------------------------------------------------------------------------------------------------------------
