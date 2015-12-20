
#include "XM2/Vector2Array.h"
namespace XM2
{
NMTL_POD_VECTOR_EXPORT(XVector2,XM2EXPORT,XM2);
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XVector2Array::fprint32(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2Array::fprint64(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2Array::fscan32(FILE* fp)
{
  int n;
  #if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%d",&n);
  #else
  ::fscanf(fp,"%d",&n);
  #endif
  resize((size_t)n);
  for (iterator it = begin();it != end(); ++it)
  {
    it->fscan32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2Array::fscan64(FILE* fp)
{
  int n;
  #if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%d",&n);
  #else
  ::fscanf(fp,"%d",&n);
  #endif
  resize((size_t)n);
  for (iterator it = begin();it != end(); ++it)
  {
    it->fscan64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2Array::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2Array::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2Array::fwrite32(FILE* fp,bool flip_bytes) const
{
  unsigned n = (unsigned)size();
  ::fwrite(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  if(flip_bytes)
  {
    for (const_iterator it = begin();it != end();++it)
    {
      it->fwrite32(fp,flip_bytes);
    }
  }
  else
    ::fwrite(buffer(),sizeof(XVector2),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2Array::fwrite64(FILE* fp,bool flip_bytes) const
{
  unsigned n = (unsigned)size();
  ::fwrite(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  if(flip_bytes)
  {
    for (const_iterator it = begin();it != end();++it)
    {
      it->fwrite64(fp,flip_bytes);
    }
  }
  else
    ::fwrite(buffer(),sizeof(XVector2),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite64(fp,flip_bytes);
  }
  #endif  
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2Array::fread32(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector2),n,fp);
  if(flip_bytes) 
    XM2swap32(buffer(),n*(sizeof(XVector2)/sizeof(float)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2Array::fread64(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector2),n,fp);
  if(flip_bytes) 
    XM2swap64(buffer(),n*(sizeof(XVector2)/sizeof(double)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread64(fp,flip_bytes);
  }
  #endif  
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayAdd(XVector2* output,const XVector2* a,const XVector2* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].add(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayAdd(XVector2* output,const XVector2* a,const XVector2& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].add(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayAdd(XVector2* output,const XVector2* a,const XReal b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].add(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arraySub(XVector2* output,const XVector2* a,const XVector2* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].sub(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arraySub(XVector2* output,const XVector2* a,const XVector2& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].sub(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arraySub(XVector2* output,const XVector2* a,const XReal b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].sub(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayMul(XVector2* output,const XVector2* a,const XVector2* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].mul(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayMul(XVector2* output,const XVector2* a,const XVector2& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].mul(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayMul(XVector2* output,const XVector2* a,const XReal b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].mul(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayDiv(XVector2* output,const XVector2* a,const XVector2* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].div(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayDiv(XVector2* output,const XVector2* a,const XVector2& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].div(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayDiv(XVector2* output,const XVector2* a,const XReal b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].div(a[i],b);
  }
}


//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayDot(XReal* product,const XVector2* a,const XVector2* b,const int num)
{
  XM2_ASSERT(product);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    product[i] = a[i].dot(b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayDot(XReal* product,const XVector2* a,const XVector2& b,const int num)
{
  XM2_ASSERT(product);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    product[i] = a[i].dot(b);
  }
}


//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayNormalise(XVector2* output,const XVector2* input,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(input);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].normalise(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayNegate(XVector2* output,const XVector2* input,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(input);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].negate(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayLerp(XVector2* output,
                         const XVector2* a,
                         const XVector2* b,
                         const XReal t,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].lerp(a[i],b[i],t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2::arrayBezier(XVector2* output,
                           const XVector2* a,const XVector2* b,
                           const XVector2* c,const XVector2* d,
                           const XReal t,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(c);
  XM2_ASSERT(d);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].bezier(a[i],b[i],c[i],d[i],t);
  }
}


#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XVector2Array& data)
{
  ofs << (unsigned)data.size() << std::endl;
  for (XM2::XVector2Array::const_iterator it = data.begin();it != data.end();++it)
  {
    ofs << *it << std::endl;
  }
  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& is,XVector2Array& data)
{
  size_t sz;
  is >> sz;
  data.resize(sz);
  for (XM2::XVector2Array::iterator it = data.begin();it != data.end();++it)
  {
    is >> *it;
  }
  return is;
}
#endif
}
