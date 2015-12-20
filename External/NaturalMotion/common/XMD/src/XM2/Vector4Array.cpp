
#include "XM2/Vector4Array.h"
#include "XM2/Vector3Array.h"
namespace XM2
{
NMTL_POD_VECTOR_EXPORT(XVector4,XM2EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayZero(XVector4* ptr,const int num)
{
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    ptr[i].zero();
  }
}
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XVector4Array::fprint32(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4Array::fprint64(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4Array::fscan32(FILE* fp)
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
void XVector4Array::fscan64(FILE* fp)
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
void XVector4Array::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4Array::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4Array::fwrite32(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XVector4),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4Array::fwrite64(FILE* fp,bool flip_bytes) const
{
  unsigned n = (unsigned)size();
  ::fwrite(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  if(flip_bytes)
  {
    for (const_iterator it = begin();it != end();++it)
    {
      it->fwrite32(fp,flip_bytes);
    }
  }
  else
    ::fwrite(buffer(),sizeof(XVector4),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite64(fp,flip_bytes);
  }
  #endif  
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4Array::fread32(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector4),n,fp);
  if(flip_bytes) 
    XM2swap32(buffer(),n*(sizeof(XVector4)/sizeof(float)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4Array::fread64(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector4),n,fp);
  if(flip_bytes) 
    XM2swap64(buffer(),n*(sizeof(XVector4)/sizeof(double)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread64(fp,flip_bytes);
  }
  #endif  
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayDot(XReal* product,const XVector4* a,const XVector4* b,const int num)
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
void XVector4::arrayDot(XReal* product,const XVector4* a,const XVector4& b,const int num)
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
void XVector4::arrayCross(XVector4* output,const XVector4* a,const XVector4* b,const int num)
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
    output[i].cross(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayCross(XVector4* output,const XVector4* a,const XVector4& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].cross(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayAdd(XVector4* output,const XVector4* a,const XVector4* b,const int num)
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
void XVector4::arrayAdd(XVector4* output,const XVector4* a,const XVector4& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].add(a[i],b);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayAdd(XVector4* output,const XVector4* a,const XVector3* b,const int num)
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
void XVector4::arrayAdd(XVector4* output,const XVector4* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].add(a[i],b);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayAdd(XVector4* output,const XVector4* a,const XReal b,const int num)
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
void XVector4::arraySub(XVector4* output,const XVector4* a,const XVector4* b,const int num)
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
void XVector4::arraySub(XVector4* output,const XVector4* a,const XVector4& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].sub(a[i],b);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XVector4::arraySub(XVector4* output,const XVector4* a,const XVector3* b,const int num)
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
void XVector4::arraySub(XVector4* output,const XVector4* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].sub(a[i],b);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XVector4::arraySub(XVector4* output,const XVector4* a,const XReal b,const int num)
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
void XVector4::arrayMul(XVector4* output,const XVector4* a,const XVector4* b,const int num)
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
void XVector4::arrayMul(XVector4* output,const XVector4* a,const XVector4& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].mul(a[i],b);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayMul(XVector4* output,const XVector4* a,const XVector3* b,const int num)
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
void XVector4::arrayMul(XVector4* output,const XVector4* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].mul(a[i],b);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayMul(XVector4* output,const XVector4* a,const XReal b,const int num)
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
void XVector4::arrayDiv(XVector4* output,const XVector4* a,const XVector4* b,const int num)
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
void XVector4::arrayDiv(XVector4* output,const XVector4* a,const XVector4& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].div(a[i],b);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayDiv(XVector4* output,const XVector4* a,const XVector3* b,const int num)
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
void XVector4::arrayDiv(XVector4* output,const XVector4* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].div(a[i],b);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayDiv(XVector4* output,const XVector4* a,const XReal b,const int num)
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
void XVector4::arrayNormalise(XVector4* output,const XVector4* input,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(input);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(input);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].normalise(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayNegate(XVector4* output,const XVector4* input,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(input);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(input);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].negate(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayLerp(XVector4* output,
                         const XVector4* a,
                         const XVector4* b,
                         const XReal t,const int num)
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
    output[i].lerp(a[i],b[i],t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4::arrayBezier(XVector4* output,
                           const XVector4* a,const XVector4* b,
                           const XVector4* c,const XVector4* d,
                           const XReal t,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(c);
  XM2_ASSERT(d);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(c);
  XM2_ALIGN_ASSERT(d);

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
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XVector4Array& data)
{
  ofs << (unsigned)data.size() << std::endl;
  for (XM2::XVector4Array::const_iterator it = data.begin();it != data.end();++it)
  {
    ofs << *it << std::endl;
  }
  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& is,XVector4Array& data)
{
  size_t sz;
  is >> sz;
  data.resize(sz);
  for (XM2::XVector4Array::iterator it = data.begin();it != data.end();++it)
  {
    is >> *it;
  }
  return is;
}
#endif

}