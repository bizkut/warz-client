/*---------------------------------------------------------------------------------------*/
/*!
 *  \file   Plane.c
 *  \date   19-05-2008
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Vector3.h"
#include "XM2/Vector4.h"
#include "XM2/Plane.h"

namespace XM2
{
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XPlane::fprint32(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  ::fprintf(fp,"%f %f %f %f\n",(float)a,(float)b,(float)c,(float)d);
}

//----------------------------------------------------------------------------------------------------------------------
void XPlane::fprint64(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  ::fprintf(fp,"%lf %lf %lf %lf\n",(double)a,(double)b,(double)c,(double)d);
}

//----------------------------------------------------------------------------------------------------------------------
void XPlane::fscan32(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%f%f%f%f",&a,&b,&c,&d);
  # else
  ::fscanf(fp,"%f%f%f%f",&a,&b,&c,&d);
  # endif
  #else
  float a_,b_,c_,d_;
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%f%f%f%f",&a_,&b_,&c_,&d_);
  # else
  ::fscanf(fp,"%f%f%f%f",&a_,&b_,&c_,&d_);
  # endif
  a = (XReal)a_;
  b = (XReal)b_;
  c = (XReal)c_;
  d = (XReal)d_;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XPlane::fscan64(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%lf%lf%lf%lf",&a,&b,&c,&d);
  # else
  ::fscanf(fp,"%lf%lf%lf%lf",&a,&b,&c,&d);
  # endif
  #else
  double a_,b_,c_,d_;
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%lf%lf%lf%lf",&a_,&b_,&c_,&d_);
  # else
  ::fscanf(fp,"%lf%lf%lf%lf",&a_,&b_,&c_,&d_);
  # endif
  a = (XReal)a_;
  b = (XReal)b_;
  c = (XReal)c_;
  d = (XReal)d_;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XPlane::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XPlane::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XPlane::fwrite32(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  if(flip_bytes) 
  {
    XPlane p = *this;
    XM2swap(p.a);
    XM2swap(p.b);
    XM2swap(p.c);
    XM2swap(p.d);
    ::fwrite(&p,sizeof(XPlane),1,fp);
  }
  else
    ::fwrite(this,sizeof(XPlane),1,fp);
  #else
  float temp[4];
  temp[0] = (float)a;
  temp[1] = (float)b;
  temp[2] = (float)c;
  temp[3] = (float)d;
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
void XPlane::fwrite64(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  if(flip_bytes) 
  {
    XPlane p = *this;
    XM2swap(p.a);
    XM2swap(p.b);
    XM2swap(p.c);
    XM2swap(p.d);
    ::fwrite(&p,sizeof(XPlane),1,fp);
  }
  else
    ::fwrite(this,sizeof(XPlane),1,fp);
  #else
  double temp[4];
  temp[0] = (double)a;
  temp[1] = (double)b;
  temp[2] = (double)c;
  temp[3] = (double)d;
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
void XPlane::fread32(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  ::fread(this,sizeof(XPlane),1,fp);
  if(flip_bytes) 
  {
    XM2swap(a);
    XM2swap(b);
    XM2swap(c);
    XM2swap(d);
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
  a = (XReal)temp[0];
  b = (XReal)temp[1];
  c = (XReal)temp[2];
  d = (XReal)temp[3];
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XPlane::fread64(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  ::fread(this,sizeof(XPlane),1,fp);
  if(flip_bytes) 
  {
    XM2swap(a);
    XM2swap(b);
    XM2swap(c);
    XM2swap(d);
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
  a = (XReal)temp[0];
  b = (XReal)temp[1];
  c = (XReal)temp[2];
  d = (XReal)temp[3];
  #endif
}
#endif

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& os,const XPlane& data)
{
  return os << data.a << " " << data.b << " " << data.c << " " << data.d;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT  std::istream& operator>>(std::istream& is,XPlane& data)
{
  return is >> data.a >> data.b >> data.c >> data.d;
}
#endif
}
