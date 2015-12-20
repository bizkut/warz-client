/*---------------------------------------------------------------------------------------*/
/*!
 *  \file   XQuaternion.c
 *  \date   19-05-2008
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Quaternion.h"

namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XQuaternion::XQuaternion(const XTransform& m)
{
  XM2_ALIGN_ASSERT(&m);
  XM2_ALIGN_ASSERT(this);
  *this = m.rotate;
}

//----------------------------------------------------------------------------------------------------------------------
const XQuaternion& XQuaternion::operator = (const XTransform& t)
{
  *this = t.rotate;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::toEulerAnglesR(XReal& _x,XReal& _y,XReal& _z) const
{
  XM2_ALIGN_ASSERT(this);

  {
    XMatrix M;
    XReal Nq = length(); 
    XReal s  = (Nq > 0.0f) ? (2.0f / Nq) : 0.0f;
    XReal xs = x*s,     ys = y*s,    zs = z*s;
    XReal wx = w*xs,    wy = w*ys,   wz = w*zs;
    XReal xx = x*xs,    xy = x*ys,   xz = x*zs;
    XReal yy = y*ys,    yz = y*zs,   zz = z*zs;
    XReal cy;

    M.m00 = 1.0f - (yy + zz); M.m01 = xy - wz;          M.m02 = xz + wy;
    M.m10 = xy + wz;          M.m11 = 1.0f - (xx + zz); M.m12 = yz - wx;
    M.m20 = xz - wy;          M.m21 = yz + wx;          M.m22 = 1.0f - (xx + yy);


    cy = XMsqrt( M.m00*M.m00 + 
                 M.m10*M.m10 );

    if (cy > 16*XM2_FLOAT_ZERO)
    {
      _x = XMatan2( M.m21, M.m22 );
      _y = XMatan2(-M.m20, cy );
      _z = XMatan2( M.m10, M.m00 );
    } 
    else
    {
      _x = XMatan2(-M.m12, M.m11 );
      _y = XMatan2(-M.m20, cy );
      _z = 0;
    }
  }
}

#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::fprint32(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  ::fprintf(fp,"%f %f %f %f\n",(float)x,(float)y,(float)z,(float)w);
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::fprint64(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  ::fprintf(fp,"%lf %lf %lf %lf\n",(double)x,(double)y,(double)z,(double)w);
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::fscan32(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%f%f%f%f",&x,&y,&z,&w);
  # else
  ::fscanf(fp,"%f%f%f%f",&x,&y,&z,&w);
  # endif
  #else
  float x_,y_,z_,w_;
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%f%f%f%f",&x_,&y_,&z_,&w_);
  # else
  ::fscanf(fp,"%f%f%f%f",&x_,&y_,&z_,&w_);
  # endif
  x = (XReal)x_;
  y = (XReal)y_;
  z = (XReal)z_;
  w = (XReal)w_;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::fscan64(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%lf%lf%lf%lf",&x,&y,&z,&w);
  # else
  ::fscanf(fp,"%lf%lf%lf%lf",&x,&y,&z,&w);
  # endif
  #else
  double x_,y_,z_,w_;
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%lf%lf%lf%lf",&x_,&y_,&z_,&w_);
  # else
  ::fscanf(fp,"%lf%lf%lf%lf",&x_,&y_,&z_,&w_);
  # endif
  x = (XReal)x_;
  y = (XReal)y_;
  z = (XReal)z_;
  w = (XReal)w_;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::fwrite32(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  if(flip_bytes)
  {
    XQuaternion q = *this;
    XM2swap(q.x);
    XM2swap(q.y);
    XM2swap(q.z);
    XM2swap(q.w);
    ::fwrite(&q,sizeof(XQuaternion),1,fp);
  }
  else
    ::fwrite(this,sizeof(XQuaternion),1,fp);
  #else
  float temp[4];
  temp[0] = (float)x;
  temp[1] = (float)y;
  temp[2] = (float)z;
  temp[3] = (float)w;
  if(flip_bytes)
  {
    XM2swap(temp[0]);
    XM2swap(temp[1]);
    XM2swap(temp[2]);
    XM2swap(temp[3]);
  }
  ::fwrite(this,sizeof(float),4,fp);
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::fwrite64(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  if(flip_bytes)
  {
    XQuaternion q = *this;
    XM2swap(q.x);
    XM2swap(q.y);
    XM2swap(q.z);
    XM2swap(q.w);
    ::fwrite(&q,sizeof(XQuaternion),1,fp);
  }
  else
    ::fwrite(this,sizeof(XQuaternion),1,fp);
  #else
  double temp[4];
  temp[0] = (double)x;
  temp[1] = (double)y;
  temp[2] = (double)z;
  temp[3] = (double)w;
  if(flip_bytes)
  {
    XM2swap(temp[0]);
    XM2swap(temp[1]);
    XM2swap(temp[2]);
    XM2swap(temp[3]);
  }
  ::fwrite(this,sizeof(double),4,fp);
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::fread32(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  ::fread(this,sizeof(XQuaternion),1,fp);
  if(flip_bytes)
  {
    XM2swap(x);
    XM2swap(y);
    XM2swap(z);
    XM2swap(w);
  }
  #else
  float temp[4];
  ::fread(temp,sizeof(float),1,fp);
  if(flip_bytes)
  {
    XM2swap(temp[0]);
    XM2swap(temp[1]);
    XM2swap(temp[2]);
    XM2swap(temp[3]);
  }
  x = (XReal)temp[0];
  y = (XReal)temp[1];
  z = (XReal)temp[2];
  w = (XReal)temp[3];
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::fread64(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  ::fread(this,sizeof(XQuaternion),1,fp);
  if(flip_bytes)
  {
    XM2swap(x);
    XM2swap(y);
    XM2swap(z);
    XM2swap(w);
  }
  #else
  double temp[4];
  ::fread(temp,sizeof(double),1,fp);
  if(flip_bytes)
  {
    XM2swap(temp[0]);
    XM2swap(temp[1]);
    XM2swap(temp[2]);
    XM2swap(temp[3]);
  }
  x = (XReal)temp[0];
  y = (XReal)temp[1];
  z = (XReal)temp[2];
  w = (XReal)temp[3];
  #endif
}
#endif

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XQuaternion& q)
{
  return ofs << q.x << " " << q.y << " " << q.z << " " << q.w;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ifs,XQuaternion& q)
{
  return ifs >> q.x >> q.y >> q.z >> q.w;
}
#endif
}
