
#include "XM2/Colour.h"

namespace XM2
{
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XColour::fprint32(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  ::fprintf(fp,"%f %f %f %f\n",(float)r,(float)g,(float)b,(float)a);
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::fprint64(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  ::fprintf(fp,"%lf %lf %lf %lf\n",(double)r,(double)g,(double)b,(double)a);
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::fscan32(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%f%f%f%f",&r,&g,&b,&a);
  # else
  ::fscanf(fp,"%f%f%f%f",&r,&g,&b,&a);
  # endif
  #else
  float r_,g_,b_,a_;
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%f%f%f%f",&r_,&g_,&b_,&a_);
  # else
  ::fscanf(fp,"%f%f%f%f",&r_,&g_,&b_,&a_);
  # endif
  r = (XReal)r_;
  g = (XReal)g_;
  b = (XReal)b_;
  a = (XReal)a_;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::fscan64(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%lf%lf%lf%lf",&r,&g,&b,&a);
  # else
  ::fscanf(fp,"%lf%lf%lf%lf",&r,&g,&b,&a);
  # endif
  #else
  double r_,g_,b_,a_;
  # if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%lf%lf%lf%lf",&r_,&g_,&b_,&a_);
  # else
  ::fscanf(fp,"%lf%lf%lf%lf",&r_,&g_,&b_,&a_);
  # endif
  r = (XReal)r_;
  g = (XReal)g_;
  b = (XReal)b_;
  a = (XReal)a_;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::fwrite32(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
    if(flip_bytes)
    {
      XColour temp=*this;
      XM2swap(temp.r);
      XM2swap(temp.g);
      XM2swap(temp.b);
      XM2swap(temp.a);
      ::fwrite(&temp,sizeof(XColour),1,fp);
    }
    else
      ::fwrite(this,sizeof(XColour),1,fp);
  #else
  float temp[4];
  temp[0] = (float)r;
  temp[1] = (float)g;
  temp[2] = (float)b;
  temp[3] = (float)a;
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
void XColour::fwrite64(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
    if(flip_bytes)
    {
      XColour temp=*this;
      XM2swap(temp.r);
      XM2swap(temp.g);
      XM2swap(temp.b);
      XM2swap(temp.a);
      ::fwrite(&temp,sizeof(XColour),1,fp);
    }
    else
      ::fwrite(this,sizeof(XColour),1,fp);
  #else
  double temp[4];
  temp[0] = (double)r;
  temp[1] = (double)g;
  temp[2] = (double)b;
  temp[3] = (double)a;
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
void XColour::fread32(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
    ::fread(this,sizeof(XColour),1,fp);
    if(flip_bytes)
    {
      XM2swap(r);
      XM2swap(g);
      XM2swap(b);
      XM2swap(a);
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
  r = temp[0];
  g = temp[1];
  b = temp[2];
  a = temp[3];
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::fread64(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
    ::fread(this,sizeof(XColour),1,fp);
    if(flip_bytes)
    {
      XM2swap(r);
      XM2swap(g);
      XM2swap(b);
      XM2swap(a);
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
  r = (XReal)temp[0];
  g = (XReal)temp[1];
  b = (XReal)temp[2];
  a = (XReal)temp[3];
  #endif
}
#endif


#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& os,const XColour& data)
{
  return os << data.r << " " << data.g << " " << data.b << " " << data.a;
}
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& is,XColour& data)
{
  return is >> data.r >> data.g >> data.b >> data.a;
}
#endif
}
