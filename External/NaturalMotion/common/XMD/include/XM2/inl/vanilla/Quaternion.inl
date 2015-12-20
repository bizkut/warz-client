#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_QUATERNION__INL__
#define XM2_VANILLA_QUATERNION__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::toMatrix(XMatrix& mat) const
{
  // sacrfice a few bytes to pre-calculate some values
  const XReal xx = x * x;
  const XReal xy = x * y;
  const XReal xz = x * z;
  const XReal xw = x * w;
  const XReal yy = y * y;
  const XReal yz = y * z;
  const XReal yw = y * w;
  const XReal zz = z * z;
  const XReal zw = z * w;

  XReal* matrix = mat.data;

  // [0] -> [3]
  *(  matrix) = 1.0f - 2.0f * (yy+zz);
  *(++matrix) =        2.0f * (xy+zw);
  *(++matrix) =        2.0f * (xz-yw);
  *(++matrix) =        0.0f;

  // [4] -> [7]
  *(++matrix) =        2.0f * (xy-zw);
  *(++matrix) = 1.0f - 2.0f * (xx+zz);
  *(++matrix) =        2.0f * (yz+xw);
  *(++matrix) =        0.0f;

  // [8] -> [11]
  *(++matrix) =        2.0f * (xz+yw);
  *(++matrix) =        2.0f * (yz-xw);
  *(++matrix) = 1.0f - 2.0f * (xx+yy);
  *(++matrix) =        0.0f;

  // [12] -> [15]
  *(++matrix) =     0.0f;
  *(++matrix) =     0.0f;
  *(++matrix) =     0.0f;
  *(++matrix) =     1.0f;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::mul(const XQuaternion& a,const XQuaternion& b)
{
  x = a.w*b.x + a.x*b.w + b.y*a.z - b.z*a.y;
  y = a.w*b.y + a.y*b.w + b.z*a.x - b.x*a.z;
  z = a.w*b.z + a.z*b.w + b.x*a.y - b.y*a.x;
  w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternion::equal(const XQuaternion& b,const XReal eps) const
{
  return float_equal(x,b.x,eps) && 
         float_equal(y,b.y,eps) && 
         float_equal(z,b.z,eps) && 
         float_equal(w,b.w,eps);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::copy(const XQuaternion& q2)
{
  x = q2.x;
  y = q2.y;
  z = q2.z;
  w = q2.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::zero()
{
  x=0;
  y=0;
  z=0;
  w=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::identity()
{
  x = 0;
  y = 0;
  z = 0;
  w = 1.0f;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::set(const XReal _x,const XReal _y,const XReal _z,const XReal _w)
{
  x = _x;
  y = _y;
  z = _z;
  w = _w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::sum(const XQuaternion& a,const XReal t)
{  
  x += a.x*t;
  y += a.y*t;
  z += a.z*t;
  w += a.w*t;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::mul(const XQuaternion& a,const XReal b)
{
  x = a.x*b;
  y = a.y*b;
  z = a.z*b;
  w = a.w*b;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::add(const XQuaternion& a,const XQuaternion& b)
{
  x = a.x+b.x;
  y = a.y+b.y;
  z = a.z+b.z;
  w = a.w+b.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XQuaternion::dot(const XQuaternion& b) const
{
  return x*b.x + y*b.y + z*b.z + w*b.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::fromMatrix(const XMatrix& mat)
{ 
#if 0
  w = XMsqrt( XMmax<XReal>( 0.0f, 1.0f + mat.m00 + mat.m11 + mat.m22 ) ) * 0.5f;
  x = XMsqrt( XMmax<XReal>( 0.0f, 1.0f + mat.m00 - mat.m11 - mat.m22 ) ) * 0.5f;
  y = XMsqrt( XMmax<XReal>( 0.0f, 1.0f - mat.m00 + mat.m11 - mat.m22 ) ) * 0.5f;
  z = XMsqrt( XMmax<XReal>( 0.0f, 1.0f - mat.m00 - mat.m11 + mat.m22 ) ) * 0.5f; 
  x = (XReal)_copysign( x, mat.m12 - mat.m12 );
  y = (XReal)_copysign( y, mat.m02 - mat.m20 );
  z = (XReal)_copysign( z, mat.m10 - mat.m01 );
#else
  const XReal *const t = (const XReal* const)mat.data;

  XReal tr, s;

  tr = 1.0f + t[0] + t[5] + t[10];

  // check the diagonals.
  if (tr > 1.0f) 
  {
    s = XMsqrt(tr) * 2.0f;

    w = s * 0.25f;

    x = (t[6] - t[9]) / s;
    y = (t[8] - t[2]) / s;
    z = (t[1] - t[4]) / s;
  } 
  else 
  {
    if ((t [0] > t [5]) && (t [0] > t [10]))  
    { // Column 0: 
      s  = XMsqrt (1.0f + t [0] - t [5] - t [10]) * 2.0f;
      x = 0.25f * s;
      y = (t [1] + t [4] ) / s;
      z = (t [8] + t [2] ) / s;
      w = (t [6] - t [9] ) / s;
    } 
    else if (t [5] > t [10] ) 
    {             // Column 1: 
      s  = XMsqrt (1.0f + t [5] - t [0] - t [10]) * 2.0f;
      x = (t [1] + t[4] ) / s;
      y = 0.25f * s;
      z = (t[6] + t[9] ) / s;
      w = (t[8] - t[2] ) / s;
    } 
    else 
    {                               // Column 2:
      s  = XMsqrt (1.0f + t [10] - t [0] - t [5]) * 2.0f;
      x = (t [8] + t [2] ) / s;
      y = (t [6] + t [9] ) / s;
      z = 0.25f * s;
      w = (t [1] - t [4] ) / s;
    }
  }
#endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::toAxisAngleR(XVector3& axis,XReal& radians) const
{
  XReal cos_a = w;
  XReal sin_a = XMsqrt( 1.0f - cos_a * cos_a );

  radians = XMacos( cos_a ) * 2.0f;
  if ( XMabs( sin_a ) > 0.0005f )
  {
    sin_a = 1.0f/sin_a;
  }
  else
    sin_a = 0.0f;
  axis.x = x*sin_a;
  axis.y = y*sin_a;
  axis.z = z*sin_a;
  axis.w = 0.0f;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::shortestPath(const XQuaternion &a,const XQuaternion &b,XReal& product)
{
  // perform dot product
  product = a.dot(b);
  *this = b;

  if(product < 0)
  {
    invert();
    product = -product;
  }

  // calculate the linear t values for the interpolation
  product = (product>1.0f) ? 0.999999f : product;
  product = (product<-1.0f) ? -0.999999f : product;
  
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::longestPath(const XQuaternion &a,const XQuaternion &b,XReal& product)
{
  // perform dot product
  product = a.dot(b);
  *this = b;

  if(product > 0)
  {
    invert();
    product = -product;
  }

  // calculate the linear t values for the interpolation
  product = (product>1.0f) ? 0.999999f : product;
  product = (product<-1.0f) ? -0.999999f : product;  
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::slerp(const XQuaternion& a,const XQuaternion& b,const XReal t)
{
  XReal product,interpolating_t=t,interpolating_invt=1.0f-t,theta,sinTheta;
  shortestPath(a,b,product);

  theta = XMacos(product);

  if(!XM2::float_equal(theta,0))
  {
    // pre-calculate the sin value
    sinTheta = XMsin(theta);
    sinTheta = 1.0f/sinTheta;

    interpolating_invt = XMsin(interpolating_invt * theta) * sinTheta;
    interpolating_t    = XMsin(interpolating_t    * theta) * sinTheta;
  }

  // perform the interpolation
  x = a.x * interpolating_invt + x * interpolating_t;
  y = a.y * interpolating_invt + y * interpolating_t;
  z = a.z * interpolating_invt + z * interpolating_t;
  w = a.w * interpolating_invt + w * interpolating_t;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::lerp(const XQuaternion& a,const XQuaternion& b,const XReal t)
{
  //
  XReal product,interpolating_t,interpolating_invt;
  shortestPath(a,b,product);

  // calculate the linear t values for the interpolation
  interpolating_t    = t;
  interpolating_invt = 1.0f-t;

  // perform the interpolation
  x = a.x + interpolating_invt * (x - a.x);
  y = a.y + interpolating_invt * (y - a.y);
  z = a.z + interpolating_invt * (z - a.z);
  w = a.w + interpolating_invt * (w - a.w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::invert(const XQuaternion& q)
{
  x = -q.x;
  y = -q.y;
  z = -q.z;
  w =  q.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::negate(const XQuaternion& q)
{
  x = -q.x;
  y = -q.y;
  z = -q.z;
  w = -q.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::normalise(const XQuaternion& q)
{
  {
    XReal l_inv = q.length();
    if(!float_equal(l_inv,0))
    {
      l_inv = 1.0f/l_inv;

      x = q.x * l_inv;
      y = q.y * l_inv;
      z = q.z * l_inv;
      w = q.w * l_inv;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternion& XQuaternion::operator += (const XQuaternion& c)
{
  x += c.x;
  y += c.y;
  z += c.z;
  w += c.w;
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternion& XQuaternion::operator -= (const XQuaternion& c)
{
  x -= c.x;
  y -= c.y;
  z -= c.z;
  w -= c.w;
  return *this;
}
}
#endif
