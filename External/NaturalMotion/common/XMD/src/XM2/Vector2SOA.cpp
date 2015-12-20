/*---------------------------------------------------------------------------------------*/
/*
 *  \file   common/Vector2SOA.cpp
 *  \date   19-05-2008
 *  \brief  all functions that are common to the SSE and vanilla versions.
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Vector2SOA.h"
namespace XM2
{
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::fprint32(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  for(int i=0;i<4;++i)
    ::fprintf(fp,"%f %f\n",(float)x[i],(float)y[i]);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::fprint64(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  for(int i=0;i<4;++i)
    ::fprintf(fp,"%lf %lf\n",(double)x[i],(double)y[i]);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::fscan32(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  for(int i=0;i<4;++i)
  {
    #if XM2_USE_FLOAT
    # if XM2_USE_SECURE_SCL
    ::fscanf_s(fp,"%f%f",&x[i],&y[i]);
    # else
    ::fscanf(fp,"%f%f",&x[i],&y[i]);
    # endif
    #else
    float x_,y_;
    # if XM2_USE_SECURE_SCL
    ::fscanf_s(fp,"%f%f",&x_,&y_);
    # else
    ::fscanf(fp,"%f%f",&x_,&y_);
    # endif
    x[i] = (XReal)x_;
    y[i] = (XReal)y_;
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::fscan64(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  for(int i=0;i<4;++i)
  {
    #if !XM2_USE_FLOAT
    # if XM2_USE_SECURE_SCL
    ::fscanf_s(fp,"%lf%lf",&x[i],&y[i]);
    # else
    ::fscanf(fp,"%lf%lf",&x[i],&y[i]);
    # endif
    #else
    double x_,y_;
    # if XM2_USE_SECURE_SCL
    ::fscanf_s(fp,"%lf%lf",&x_,&y_);
    # else
    ::fscanf(fp,"%lf%lf",&x_,&y_);
    # endif
    x[i] = (XReal)x_;
    y[i] = (XReal)y_;
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::fwrite32(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  if(flip_bytes)
  {
    for(int i=0;i<8;++i)
    {
      float temp = (float)data[i];
      XM2swap(temp);
      ::fwrite(&temp,sizeof(float),1,fp);
    }
  }
  else
  {
    ::fwrite(this,sizeof(XVector2SOA),1,fp);
  }
  #else
  for(int i=0;i<8;++i)
  {
    float temp = (float)data[i];
    ::fwrite(&temp,sizeof(float),1,fp);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::fwrite64(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  if(flip_bytes)
  {
    for(int i=0;i<8;++i)
    {
      double temp = (double)data[i];
      XM2swap(temp);
      ::fwrite(&temp,sizeof(double),1,fp);
    }
  }
  else
  {
    ::fwrite(this,sizeof(XVector2SOA),1,fp);
  }
  #else
  if(flip_bytes)
  {
    for(int i=0;i<8;++i)
    {
      double temp = (double)data[i];
      XM2swap(temp);
      ::fwrite(&temp,sizeof(double),1,fp);
    }
  }
  else
  {
    for(int i=0;i<8;++i)
    {
      double temp = (double)data[i];
      ::fwrite(&temp,sizeof(double),1,fp);
    }
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::fread32(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  ::fread(this,sizeof(XVector2SOA),1,fp);
  if(flip_bytes)
  {
    XM2swap32(this,8);
  }
  #else
  for(int i=0;i<8;++i)
  {
    float temp;
    ::fread(&temp,sizeof(float),1,fp);
    data[i] = (XReal)temp;
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::fread64(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  ::fread(this,sizeof(XVector2SOA),1,fp);
  if(flip_bytes)
  {
    XM2swap64(this,8);
  }
  #else
  for(int i=0;i<8;++i)
  {
    double temp;
    ::fread(&temp,sizeof(double),1,fp);
    if(flip_bytes) XM2swap(temp);
    data[i] = (XReal)temp;
  }
  #endif
}
#endif

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XVector2SOA& c)
{
  for(int i=0;i<4;++i)
    ofs << c.x[i] << " " << c.y[i] << std::endl;
  return ofs;
}
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ifs,XVector2SOA& c)
{
  for(int i=0;i<4;++i)
    ifs >> c.x[i] >> c.y[i];
  return ifs;
}
#endif

}