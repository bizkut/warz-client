/*---------------------------------------------------------------------------------------*/
/*
 *  \date   22-05-2008
 *  \brief  C++ wrapper for BoundingBox
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Sphere.h"
#include "XM2/BoundingBox.h"
#include "XM2/Vector3Array.h"
#include "XM2/Vector4Array.h"
#include "XM2/Vector3SOAArray.h"
#include "XM2/Vector4SOAArray.h"
#include "XM2/Vector3PackedArray.h"

namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
bool XSphere::intersects(const XRay& ray,XReal& intersect) const
{ 
  XVector3 new_origin;
  XReal a,b,c,disc,distSqrt,q,t0,t1;

  XM2_ALIGN_ASSERT(&ray);
  XM2_ALIGN_ASSERT(this);

  // get ray origin in XSphere::'s space. Since XSphere::s are still XSphere::s after rotation,
  // we just need to subtract the points
  new_origin.sub(ray.origin,center);

  // compute A, B and C coefficients
  a = ray.direction.dot(ray.direction);
  b = 2.0f * ray.direction.dot(new_origin);
  c = new_origin.dot(new_origin) - getRadiusSquared();

  // find discriminant
  disc = b * b - 4.0f * a * c;

  // if discriminant is negative there are no real roots, so return 
  // false as ray misses XSphere::
  if (disc < 0)
    return 0;

  // compute q as described above
  distSqrt = XMsqrt(disc);
  if (b < 0)
    q = (-b - distSqrt)*0.5f;
  else
    q = (-b + distSqrt)*0.5f;

  // compute t0 and t1
  t0 = q / a;
  t1 = c / q;

  // make sure t0 is smaller than t1
  if (t0 > t1)
  {
    // if t0 is bigger than t1 swap them around
    XReal temp = t0;
    t0 = t1;
    t1 = temp;
  }

  // if t1 is less than zero, the object is in the ray's negative direction
  // and consequently the ray misses the XSphere::
  if (t1 < 0)
    return false;

  // if t0 is less than zero, the intersection point is at t1
  if (t0 < 0)
  {
    intersect = t1;
    return true;
  }

  // else the intersection point is at t0
  intersect = t0;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------  
void XSphere::expand(const XVector3Array& p)
{
  XM2_ALIGN_ASSERT(this);
  /// \todo can be improved
  for (int i=0;i<(int)p.size();++i)
  {
    expand(p[i]);
  }
}
//----------------------------------------------------------------------------------------------------------------------  
void XSphere::expand(const XVector3PackedArray& p)
{
  XM2_ALIGN_ASSERT(this);
  /// \todo can be improved
  for (int i=0;i<(int)p.size();++i)
  {
    expand(p[i]);
  }
}
//----------------------------------------------------------------------------------------------------------------------  
void XSphere::expand(const XVector4Array& p)
{
  XM2_ALIGN_ASSERT(this);
  /// \todo can be improved
  for (int i=0;i<(int)p.size();++i)
  {
    expand(p[i]);
  }
}
//----------------------------------------------------------------------------------------------------------------------  
void XSphere::expand(const XVector3SOAArray& p)
{
  XM2_ALIGN_ASSERT(this);
  /// \todo can be improved
  for (int i=0;i<(int)p.size();++i)
  {
    expand(p[i]);
  }
}
//----------------------------------------------------------------------------------------------------------------------  
void XSphere::expand(const XVector4SOAArray& p)
{
  XM2_ALIGN_ASSERT(this);
  /// \todo can be improved
  for (int i=0;i<(int)p.size();++i)
  {
    expand(p[i]);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XSphere::expand(const XBoundingBox& box)
{
  XM2_ALIGN_ASSERT(&box);
  XM2_ALIGN_ASSERT(this);

  /* init 8 corner points */
  XVector3 verts[8];
  verts[0].set(box.maximum.x, box.maximum.y, box.maximum.z );
  verts[1].set(box.minimum.x, box.maximum.y, box.maximum.z );
  verts[2].set(box.maximum.x, box.minimum.y, box.maximum.z );
  verts[3].set(box.maximum.x, box.maximum.y, box.minimum.z );
  verts[4].set(box.minimum.x, box.minimum.y, box.maximum.z );
  verts[5].set(box.minimum.x, box.maximum.y, box.minimum.z );
  verts[6].set(box.maximum.x, box.minimum.y, box.minimum.z );
  verts[7].set(box.minimum.x, box.minimum.y, box.minimum.z );

  /* expand the XSphere:: to hold each point */
  expand(verts[0]);
  expand(verts[1]);
  expand(verts[2]);
  expand(verts[3]);
  expand(verts[4]);
  expand(verts[5]);
  expand(verts[6]);
  expand(verts[7]);
}

//----------------------------------------------------------------------------------------------------------------------
bool XSphere::intersects(const XSphere& b) const
{
  XVector3 diff;
  XReal dist2;
  XReal test_dist2;

  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  test_dist2 = getRadius() + b.getRadius();
  test_dist2 *= test_dist2;

  diff.sub(center,b.center);
  dist2 = diff.lengthSquared();

  return test_dist2 >= dist2;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSphere::intersects(const XBoundingBox& b,XVector3& collision_normal,XReal& collision_depth) const
{
  XVector3 closest_point;
  XVector3 diff;
  XReal diff_squared;

  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&collision_normal);

  if (b.contains(center))
  {
    collision_depth = 0;
    collision_normal.zero();
    return true;
  }

  // determine closest point on box
  closest_point.x = (center.x < b.minimum.x)? b.minimum.x : (center.x > b.maximum.x)? b.maximum.x : center.x;
  closest_point.y = (center.y < b.minimum.y)? b.minimum.y : (center.y > b.maximum.y)? b.maximum.y : center.y;
  closest_point.z = (center.z < b.minimum.z)? b.minimum.z : (center.z > b.maximum.z)? b.maximum.z : center.z;
  closest_point.w = 0.0f;

  diff.sub(center,closest_point);

  diff_squared = diff.lengthSquared();
  if(diff_squared>getRadiusSquared())
  {
    // too far apart to intersect
    return false;    
  }

  diff_squared = XMsqrt(diff_squared);

  // collision depth 
  collision_depth = getRadius() - diff_squared;

  // collision normal
  collision_normal.div(diff,diff_squared);
  return 1;
}

#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XSphere::fprint32(FILE* fp) const
{
  center.fprint32(fp);
  #if XM2_USE_SECURE_SCL
  fprintf_s(fp,"%f",((float)getRadius()));
  #else
  fprintf(fp,"%f",((float)getRadius()));
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XSphere::fprint64(FILE* fp) const
{
  center.fprint64(fp);
  #if XM2_USE_SECURE_SCL
  fprintf_s(fp,"%lf",((double)getRadius()));
  #else
  fprintf(fp,"%lf",((double)getRadius()));
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XSphere::fscan32(FILE* fp)
{
  center.fscan32(fp);
  float _r;
  #if XM2_USE_SECURE_SCL
  fscanf_s(fp,"%f",&_r);
  #else
  fscanf(fp,"%f",&_r);
  #endif
  setRadius((XReal)_r);
}

//----------------------------------------------------------------------------------------------------------------------
void XSphere::fscan64(FILE* fp)
{
  center.fscan64(fp);
  double _r;
  #if XM2_USE_SECURE_SCL
  fscanf_s(fp,"%lf",&_r);
  #else
  fscanf(fp,"%lf",&_r);
  #endif
  setRadius((XReal)_r);
}

//----------------------------------------------------------------------------------------------------------------------
void XSphere::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XSphere::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XSphere::fread32(FILE* fp,bool flip_bytes)
{
  center.fread32(fp,flip_bytes);
  float _r;
  ::fread(&_r,sizeof(float),1,fp);
  if(flip_bytes)
    XM2swap(_r);
  setRadius((XReal)_r);
}

//----------------------------------------------------------------------------------------------------------------------
void XSphere::fread64(FILE* fp,bool flip_bytes)
{
  center.fread64(fp,flip_bytes);
  double _r;
  ::fread(&_r,sizeof(double),1,fp);
  if(flip_bytes)
    XM2swap(_r);
  setRadius((XReal)_r);
}

//----------------------------------------------------------------------------------------------------------------------
void XSphere::fwrite32(FILE* fp,bool flip_bytes) const
{
  center.fwrite32(fp,flip_bytes);
  float _r = (float)getRadius();
  if(flip_bytes)
    XM2swap(_r);
  ::fread(&_r,sizeof(float),1,fp);
}
//----------------------------------------------------------------------------------------------------------------------
void XSphere::fwrite64(FILE* fp,bool flip_bytes) const
{
  center.fwrite64(fp,flip_bytes);
  double _r = (double)getRadius();
  if(flip_bytes)
    XM2swap(_r);
  ::fwrite(&_r,sizeof(double),1,fp);
}
#endif

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XSphere& s)
{
  return ofs << s.center << " " << s.getRadius();
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ifs,XSphere& s)
{
  XM2::XReal r;
  ifs >> s.center >> r;
  s.setRadius(r);
  return ifs;
}
#endif
}
