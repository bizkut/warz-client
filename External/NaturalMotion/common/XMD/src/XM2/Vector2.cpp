
#include "XM2/Vector2.h"

namespace XM2
{
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XVector2::fprint32(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  ::fprintf(fp,"%f %f\n",(float)x,(float)y);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::fprint64(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  ::fprintf(fp,"%lf %lf\n",(double)x,(double)y);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::fscan32(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%f%f",&x,&y);
  # else
  ::fscanf(fp,"%f%f",&x,&y);
  # endif
  #else
  float x_,y_;
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%f%f",&x_,&y_);
  # else
  ::fscanf(fp,"%f%f",&x_,&y_);
  # endif
  x = (XReal)x_;
  y = (XReal)y_;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::fscan64(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%lf%lf",&x,&y);
  # else
  ::fscanf(fp,"%lf%lf",&x,&y);
  # endif
  #else
  double x_,y_;
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%lf%lf",&x_,&y_);
  # else
  ::fscanf(fp,"%lf%lf",&x_,&y_);
  # endif
  x = (XReal)x_;
  y = (XReal)y_;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::fwrite32(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  if(flip_bytes)
  {
    float temp[2];
    temp[0] = (float)x;
    temp[1] = (float)y;
    XM2swap(temp[0]);
    XM2swap(temp[1]);
    ::fwrite(temp,sizeof(float),2,fp);
  }
  else
    ::fwrite(this,sizeof(XVector2),1,fp);
  #else
  float temp[2];
  temp[0] = (float)x;
  temp[1] = (float)y;
  if(flip_bytes)
  {
    XM2swap(temp[0]);
    XM2swap(temp[1]);
  }
  ::fwrite(this,sizeof(float),2,fp);
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::fwrite64(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  if(flip_bytes)
  {
    double temp[2];
    temp[0] = (double)x;
    temp[1] = (double)y;
    XM2swap(temp[0]);
    XM2swap(temp[1]);
    ::fwrite(temp,sizeof(double),2,fp);
  }
  else
    ::fwrite(this,sizeof(XVector2),1,fp);
  #else
  double temp[2];
  temp[0] = (double)x;
  temp[1] = (double)y;
  if(flip_bytes)
  {
    XM2swap(temp[0]);
    XM2swap(temp[1]);
  }
  ::fwrite(this,sizeof(double),2,fp);
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::fread32(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  ::fread(this,sizeof(XVector2),1,fp);
  if(flip_bytes)
  {
    XM2swap(x);
    XM2swap(y);
  }
  #else
  float temp[2];
  ::fread(temp,sizeof(float),2,fp);
  if(flip_bytes)
  {
    XM2swap(temp[0]);
    XM2swap(temp[1]);
  }
  x = (XReal)temp[0];
  y = (XReal)temp[1];
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::fread64(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  ::fread(this,sizeof(XVector2),1,fp);
  if(flip_bytes)
  {
    XM2swap(x);
    XM2swap(y);
  }
  #else
  double temp[2];
  ::fread(temp,sizeof(double),2,fp);
  if(flip_bytes)
  {
    XM2swap(temp[0]);
    XM2swap(temp[1]);
  }
  x = (XReal)temp[0];
  y = (XReal)temp[1];
  #endif
}
#endif

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& os,const XVector2& data)
{
  return os << data.x << " " << data.y;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& is,XVector2& data)
{
  return is >> data.x >> data.y;
}
#endif
}
