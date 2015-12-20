//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector3.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XM2
{
class XM2EXPORT XMatrix;
class XM2EXPORT XSphere;
class XM2EXPORT XRay;
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
/// \class   XM2::XBoundingBox
/// \brief   An axis aligned bounding box class. 
/// \ingroup XM2MathsTypes
//----------------------------------------------------------------------------------------------------------------------
XM2_ALIGN_PREFIX(16) 
class XM2EXPORT XBoundingBox
{
public:

  //--------------------------------------------------------------------------------------------------------------------
  // XBoundingBox construction / destruction
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  ctor
  XM2_INLINE XBoundingBox();
  
  /// \brief  ctor
  /// \param  _min - min point of box
  /// \param  _max - max point of box
  XM2_INLINE XBoundingBox(const XVector3& _min, const XVector3& _max);  

  /// \brief  copy ctor
  XM2_INLINE explicit XBoundingBox(const XBoundingBox& src);

  //--------------------------------------------------------------------------------------------------------------------
  // XBoundingBox comparison
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  box comparison

  /// \brief  return (this == b)
  /// \param  b - second bounding box to test
  /// \param  eps - epsilon 
  /// \return true if this and b are the same, false otherwise
  XM2_INLINE bool equal(const XBoundingBox& b, XReal eps=XM2_FLOAT_ZERO) const;
  
  /// \brief  return (this != b)
  /// \param  b - second bounding box to test
  /// \param  eps - epsilon 
  /// \return true if this and b are NOT the same, false otherwise
  XM2_INLINE bool notEqual(const XBoundingBox& b, XReal eps=XM2_FLOAT_ZERO) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XBoundingBox Initialisation
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  construction

  /// \brief  clears all previous data from the bounding box, the box will be set to +/- infinity
  XM2_INLINE void clear();

  //--------------------------------------------------------------------------------------------------------------------
  // XBoundingBox Modification functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  box modification 

  /// \brief  transforms the bounding box by the specified matrix
  /// \param  mat - the matrix to transform it by
  XM2_INLINE void transformUsing(const XMatrix& mat);

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
  XM2_INLINE void expand(const XVector3Array& p);

  /// \brief  expands the bounding box to contain the array of points specified
  /// \param  p - the point to include within the bounding box
  XM2_INLINE void expand(const XVector4Array& p);

  /// \brief  expands the bounding box to contain the array of points specified
  /// \param  p - the point to include within the bounding box
  XM2_INLINE void expand(const XVector3SOAArray& p);

  /// \brief  expands the bounding box to contain the array of points specified
  /// \param  p - the point to include within the bounding box
  XM2_INLINE void expand(const XVector4SOAArray& p);

  /// \brief  expands the bounding box to contain the array of points specified
  /// \param  p - the point to include within the bounding box
  XM2_INLINE void expand(const XVector3PackedArray& p);

  /// \brief  expands this bounding box so that it contains the specified box
  /// \param  box - the bounding box that this will contain
  XM2_INLINE void expand(const XBoundingBox& box);

  /// \brief  expands this bounding box so that it contains the specified sphere
  /// \param  sphere - the bounding sphere that this bounding box should contain
  XM2_INLINE void expand(const XSphere& sphere);

  //--------------------------------------------------------------------------------------------------------------------
  // XBoundingBox Test functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  intersection query

  /// \brief  a query function to see if this bounding box contains the specified point
  /// \param  point - the point to test
  /// \return true if point is contained within this box
  XM2_INLINE bool contains(const XVector3& point) const;

  /// \brief  a query function to see if this bounding box contains the specified point
  /// \param  point - the point to test
  /// \return true if point is contained within this box
  XM2_INLINE bool contains(const XVector4& point) const;

  /// \brief  a query function to see if this bounding box contains the specified point
  /// \param  point - the point to test
  /// \return true if point is contained within this box
  XM2_INLINE bool contains(const XVector3Packed& point) const;

  /// \brief  returns true if this bounding box intersects b
  /// \param  b - the box to test 
  /// \return true if b intersects this
  XM2_INLINE bool intersects(const XBoundingBox& b) const;

  /// \brief  returns true if this bounding box intersects b
  /// \param  b - the sphere to test 
  /// \return true if b intersects this
  XM2_INLINE bool intersects(const XSphere& b) const;

  /// \brief  tests if the ray b intersects this bounding box, and returns the 
  ///         intersection point
  /// \param  b - the ray to intersect with this
  /// \param  output - the returned intersection point
  /// \return true if b intersects this
  XM2_INLINE bool intersects(const XRay& b, XVector3& output) const;

  //--------------------------------------------------------------------------------------------------------------------
  // XBoundingBox Query functions
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  getters

  /// \brief  returns the width of this bounding box
  /// \return the box width
  XM2_INLINE XReal width() const;
  
  /// \brief  returns the height of this bounding box
  /// \return the box height
  XM2_INLINE XReal height() const;
  
  /// \brief  returns the depth of this bounding box
  /// \return the box depth
  XM2_INLINE XReal depth() const;

  /// \brief  gets the center of this bounding box
  /// \param  out_center - the returned center point
  XM2_INLINE void center(XVector3& out_center) const;
  
  #ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT

  /// \brief  returns the width of this bounding box
  /// \return the box width
  /// \note   this function is only available when SSE and floats are enabled
  XM2_INLINE __m128 widthSSE() const;
  
  /// \brief  returns the height of this bounding box
  /// \return the box height
  /// \note   this function is only available when SSE and floats are enabled
  XM2_INLINE __m128 heightSSE() const;
  
  /// \brief  returns the depth of this bounding box
  /// \return the box depth
  /// \note   this function is only available when SSE and floats are enabled
  XM2_INLINE __m128 depthSSE() const;
  #else

  /// \brief  returns the width of this bounding box
  /// \return the box width
  /// \note   this function is only available when SSE and doubles are enabled
  XM2_INLINE __m128d widthSSE() const;
  
  /// \brief  returns the height of this bounding box
  /// \return the box height
  /// \note   this function is only available when SSE and doubles are enabled
  XM2_INLINE __m128d heightSSE() const;
  
  /// \brief  returns the depth of this bounding box
  /// \return the box depth
  /// \note   this function is only available when SSE and doubles are enabled
  XM2_INLINE __m128d depthSSE() const;
  #endif
  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XBoundingBox stdio
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  file IO

  #if !XM2_NO_C_IO

  /// \brief  prints this BoundingBox to the specified text file
  /// \param  fp - the file to write to 
  void fprint32(FILE* fp) const;

  /// \brief  prints this BoundingBox to the specified text file
  /// \param  fp - the file to write to 
  void fprint64(FILE* fp) const;

  /// \brief  reads a BoundingBox from the specified text file
  /// \param  fp - the file to read from
  void fscan32(FILE* fp);

  /// \brief  reads a BoundingBox from the specified text file
  /// \param  fp - the file to read from
  void fscan64(FILE* fp);

  /// \brief  prints this BoundingBox to stdout
  void print32() const;

  /// \brief  prints this BoundingBox to stdout
  void print64() const;
  
  /// \brief  writes this BoundingBox to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite32(FILE* fp, bool flip_bytes=false) const;

  /// \brief  writes this BoundingBox to the specified binary file
  /// \param  fp - the file to write to 
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fwrite64(FILE* fp, bool flip_bytes=false) const;

  /// \brief  reads a BoundingBox from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread32(FILE* fp, bool flip_bytes=false);

  /// \brief  reads a BoundingBox from the specified binary file
  /// \param  fp - the file to read from
  /// \param  flip_bytes - flag to indicate file byte order is different to the systems
  ///         byte order, in which case the bytes being read/written need to be swapped
  void fread64(FILE* fp, bool flip_bytes=false);

  #endif

  //--------------------------------------------------------------------------------------------------------------------
  // XBoundingBox operators
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  operators

  XM2_INLINE XBoundingBox& operator=(const XBoundingBox& other);
  XM2_INLINE bool operator==(const XBoundingBox& other) const;
  XM2_INLINE bool operator!=(const XBoundingBox& other) const;
  
  //--------------------------------------------------------------------------------------------------------------------
  // 16 Byte Aligned new / delete operators
  //--------------------------------------------------------------------------------------------------------------------

  XM2_MEMORY_OPERATORS

  //--------------------------------------------------------------------------------------------------------------------
  // Member variables
  //--------------------------------------------------------------------------------------------------------------------

  /// \name  data

  XVector3 minimum;
  XVector3 maximum;
}
XM2_ALIGN_SUFFIX(16);

#if !XM2_NO_STREAM_IO
XM2EXPORT std::ostream& operator<<(std::ostream& os, const XBoundingBox& data);
XM2EXPORT std::istream& operator>>(std::istream& is, XBoundingBox& data);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector3.h"
#include "XM2/Vector4.h"
#include "XM2/Vector3Packed.h"
#include "XM2/Vector3SOA.h"
#include "XM2/Vector4SOA.h"
#include "XM2/inl/BoundingBox.inl"
#if XM2_SSE
# include "XM2/inl/sse/BoundingBox.inl"
#elif XM2_SSE2
# include "XM2/inl/sse2/BoundingBox.inl"
#elif XM2_FPU
# include "XM2/inl/vanilla/BoundingBox.inl"
#else
# error Unknown CPU architecture
#endif
//----------------------------------------------------------------------------------------------------------------------
