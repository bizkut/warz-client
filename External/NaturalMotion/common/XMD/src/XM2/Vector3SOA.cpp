/*---------------------------------------------------------------------------------------*/
/*
 *  \file   common/Vector4SOA.c
 *  \date   19-05-2008
 *  \brief  all functions that are common to the SSE and vanilla versions.
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Vector3SOA.h"
namespace XM2
{  
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::fprint32(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  for(int i=0;i<4;++i)
    ::fprintf(fp,"%f %f %f\n",(float)x[i],(float)y[i],(float)z[i]);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::fprint64(FILE* fp) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  for(int i=0;i<4;++i)
    ::fprintf(fp,"%lf %lf %lf\n",(double)x[i],(double)y[i],(double)z[i]);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::fscan32(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  for(int i=0;i<4;++i)
  {
    #if XM2_USE_FLOAT
    # if XM2_USE_SECURE_SCL
    ::fscanf_s(fp,"%f%f%f",&x[i],&y[i],&z[i]);
    # else
    ::fscanf(fp,"%f%f%f",&x[i],&y[i],&z[i]);
    # endif
    #else
    float x_,y_,z_;
    # if XM2_USE_SECURE_SCL
    ::fscanf_s(fp,"%f%f%f",&x_,&y_,&z_);
    # else
    ::fscanf(fp,"%f%f%f",&x_,&y_,&z_);
    # endif
    x[i] = (XReal)x_;
    y[i] = (XReal)y_;
    z[i] = (XReal)z_;
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::fscan64(FILE* fp)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  for(int i=0;i<4;++i)
  {
    #if !XM2_USE_FLOAT
    # if XM2_USE_SECURE_SCL
    ::fscanf_s(fp,"%lf%lf%lf",&x[i],&y[i],&z[i]);
    # else
    ::fscanf(fp,"%lf%lf%lf",&x[i],&y[i],&z[i]);
    # endif
    #else
    double x_,y_,z_;
    # if XM2_USE_SECURE_SCL
    ::fscanf_s(fp,"%lf%lf%lf",&x_,&y_,&z_);
    # else
    ::fscanf(fp,"%lf%lf%lf",&x_,&y_,&z_);
    # endif
    x[i] = (XReal)x_;
    y[i] = (XReal)y_;
    z[i] = (XReal)z_;
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::fwrite32(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  if(flip_bytes)
  {
    for(int i=0;i<12;++i)
    {
      XReal temp = data[i];
      XM2swap(temp);
      ::fwrite(&temp,sizeof(XReal),1,fp);
    }
  }
  else
    ::fwrite(this,sizeof(XVector3SOA),1,fp);
  #else
  for(int i=0;i<12;++i)
  {
    float temp = (float)data[i];
    if(flip_bytes) XM2swap(temp);
    ::fwrite(&temp,sizeof(float),1,fp);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::fwrite64(FILE* fp,bool flip_bytes) const
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  if(flip_bytes)
  {
    for(int i=0;i<12;++i)
    {
      XReal temp = data[i];
      XM2swap(temp);
      ::fwrite(&temp,sizeof(XReal),1,fp);
    }
  }
  else
    ::fwrite(this,sizeof(XVector3SOA),1,fp);
  #else
  for(int i=0;i<12;++i)
  {
    double temp = (double)data[i];
    if(flip_bytes) XM2swap(temp);
    ::fwrite(&temp,sizeof(double),1,fp);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::fread32(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if XM2_USE_FLOAT
  ::fread(this,sizeof(XVector3SOA),1,fp);
  if(flip_bytes)
    XM2swap32(this,12);
  #else
  for(int i=0;i<12;++i)
  {
    float temp;
    ::fread(&temp,sizeof(float),1,fp);
    if(flip_bytes) XM2swap(temp);
    data[i] = (XReal)temp;
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::fread64(FILE* fp,bool flip_bytes)
{
  // ensure not NULL
  XM2_ASSERT(fp);
  XM2_ALIGN_ASSERT(this);

  #if !XM2_USE_FLOAT
  ::fread(this,sizeof(XVector3SOA),1,fp);
  if(flip_bytes)
    XM2swap64(this,12);
  #else
  for(int i=0;i<12;++i)
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
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XVector3SOA& c)
{
  for(int i=0;i<4;++i)
    ofs << c.x[i] << " " << c.y[i] << " " << c.z[i] << std::endl;
  return ofs;
}
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ifs,XVector3SOA& c)
{
  for(int i=0;i<4;++i)
    ifs >> c.x[i] >> c.y[i] >> c.z[i];
  return ifs;
}
#endif
}
