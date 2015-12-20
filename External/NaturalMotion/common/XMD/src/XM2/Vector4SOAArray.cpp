#include "XM2/Vector4SOA.h"
#include "XM2/Vector4SOAArray.h"

namespace XM2
{
// export STL base classes into DLL
NMTL_POD_VECTOR_EXPORT(XVector4SOA,XM2EXPORT,XM2);
#ifdef XM2_USE_SSE
#if XM2_USE_FLOAT
//----------------------------------------------------------------------------------------------------------------------  
const XVector4SOAArray& XVector4SOAArray::operator=(const XVector4SOAArray& rhs)
{
  const int _size = (int)rhs.size();
  resize(_size);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<_size;++i)
  {
    _mm_stream_ps(buffer()[i].x,rhs[i]._x);
    _mm_stream_ps(buffer()[i].y,rhs[i]._y);
    _mm_stream_ps(buffer()[i].z,rhs[i]._z);
    _mm_stream_ps(buffer()[i].w,rhs[i]._w);
  }
  return *this;
}
#else
//----------------------------------------------------------------------------------------------------------------------  
const XVector4SOAArray& XVector4SOAArray::operator=(const XVector4SOAArray& rhs)
{
  const int _size = (int)rhs.size();
  resize(_size);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<_size;++i)
  {
    _mm_stream_pd(buffer()[i].x,rhs[i]._x0);
    _mm_stream_pd(buffer()[i].x+2,rhs[i]._x1);
    _mm_stream_pd(buffer()[i].y,rhs[i]._y0);
    _mm_stream_pd(buffer()[i].y+2,rhs[i]._y1);
    _mm_stream_pd(buffer()[i].z,rhs[i]._z0);
    _mm_stream_pd(buffer()[i].z+2,rhs[i]._z1);
    _mm_stream_pd(buffer()[i].w,rhs[i]._w0);
    _mm_stream_pd(buffer()[i].w+2,rhs[i]._w1);
  }
  return *this;
}
#endif
#else
//----------------------------------------------------------------------------------------------------------------------  
const XVector4SOAArray& XVector4SOAArray::operator=(const XVector4SOAArray& rhs)
{
  const int _size = (int)rhs.size();
  resize(_size);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<_size;++i)
  {
    for(int j=0;j!=4;++j)
    {
      buffer()[i].x[j] = rhs[i].x[j];
      buffer()[i].y[j] = rhs[i].y[j];
      buffer()[i].z[j] = rhs[i].z[j];
      buffer()[i].w[j] = rhs[i].w[j];
    }
  }
  return *this;
}
#endif
//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayZero(XVector4SOA* ptr,const int num)
{
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    ptr[i].zero();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayAdd(XVector4SOA* output,const XVector4SOA* a,const XVector4SOA* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].add(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayAdd(XVector4SOA* output,const XVector4SOA* a,const XVector4SOA& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].add(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayAdd(XVector4SOA* output,const XVector4SOA* a,const XVector4& b,const int num)
{
  XVector4SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    temp._z = _mm_set_ps1(b.z);
    temp._w = _mm_set_ps1(b.w);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    temp._z0 = _mm_set1_pd(b.z);
    temp._z1 = _mm_set1_pd(b.z);
    temp._w0 = _mm_set1_pd(b.w);
    temp._w1 = _mm_set1_pd(b.w);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
    temp.z[i] = b.z;
    temp.w[i] = b.w;
  }
  #endif
  arrayAdd(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayAdd(XVector4SOA* output,const XVector4SOA* a,const XReal b,const int num)
{
  XVector4SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = 
    temp._y = 
    temp._z = 
    temp._w = _mm_set_ps1(b);
    #else
    temp._x0 = 
    temp._x1 = 
    temp._y0 = 
    temp._y1 = 
    temp._z0 = 
    temp._z1 = 
    temp._w0 = 
    temp._w1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
    temp.z[i] = b;
    temp.w[i] = b;
  }
  #endif
  arrayAdd(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arraySub(XVector4SOA* output,const XVector4SOA* a,const XVector4SOA* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].sub(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arraySub(XVector4SOA* output,const XVector4SOA* a,const XVector4SOA& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].sub(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arraySub(XVector4SOA* output,const XVector4SOA* a,const XVector4& b,const int num)
{
  XVector4SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    temp._z = _mm_set_ps1(b.z);
    temp._w = _mm_set_ps1(b.w);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    temp._z0 = _mm_set1_pd(b.z);
    temp._z1 = _mm_set1_pd(b.z);
    temp._w0 = _mm_set1_pd(b.w);
    temp._w1 = _mm_set1_pd(b.w);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
    temp.z[i] = b.z;
    temp.w[i] = b.w;
  }
  #endif
  arraySub(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arraySub(XVector4SOA* output,const XVector4SOA* a,const XReal b,const int num)
{
  XVector4SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = 
    temp._y = 
    temp._z = 
    temp._w = _mm_set_ps1(b);
    #else
    temp._x0 = 
    temp._x1 = 
    temp._y0 = 
    temp._y1 = 
    temp._z0 = 
    temp._z1 = 
    temp._w0 = 
    temp._w1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
    temp.z[i] = b;
    temp.w[i] = b;
  }
  #endif
  arraySub(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayMul(XVector4SOA* output,const XVector4SOA* a,const XVector4SOA* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].mul(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayMul(XVector4SOA* output,const XVector4SOA* a,const XVector4SOA& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].mul(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayMul(XVector4SOA* output,const XVector4SOA* a,const XVector4& b,const int num)
{
  XVector4SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    temp._z = _mm_set_ps1(b.z);
    temp._w = _mm_set_ps1(b.w);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    temp._z0 = _mm_set1_pd(b.z);
    temp._z1 = _mm_set1_pd(b.z);
    temp._w0 = _mm_set1_pd(b.w);
    temp._w1 = _mm_set1_pd(b.w);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
    temp.z[i] = b.z;
    temp.w[i] = b.w;
  }
  #endif
  arrayMul(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayMul(XVector4SOA* output,const XVector4SOA* a,const XReal b,const int num)
{
  XVector4SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = 
    temp._y = 
    temp._z = 
    temp._w = _mm_set_ps1(b);
    #else
    temp._x0 = 
    temp._x1 = 
    temp._y0 = 
    temp._y1 = 
    temp._z0 = 
    temp._z1 = 
    temp._w0 = 
    temp._w1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
    temp.z[i] = b;
    temp.w[i] = b;
  }
  #endif
  arrayMul(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayDiv(XVector4SOA* output,const XVector4SOA* a,const XVector4SOA* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].div(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayDiv(XVector4SOA* output,const XVector4SOA* a,const XVector4SOA& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].div(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayDiv(XVector4SOA* output,const XVector4SOA* a,const XVector4& b,const int num)
{
  XVector4SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    temp._z = _mm_set_ps1(b.z);
    temp._w = _mm_set_ps1(b.w);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    temp._z0 = _mm_set1_pd(b.z);
    temp._z1 = _mm_set1_pd(b.z);
    temp._w0 = _mm_set1_pd(b.w);
    temp._w1 = _mm_set1_pd(b.w);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
    temp.z[i] = b.z;
    temp.w[i] = b.w;
  }
  #endif
  arrayDiv(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayDiv(XVector4SOA* output,const XVector4SOA* a,const XReal b,const int num)
{
  XVector4SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = 
    temp._y = 
    temp._z = 
    temp._w = _mm_set_ps1(b);
    #else
    temp._x0 = 
    temp._x1 = 
    temp._y0 = 
    temp._y1 = 
    temp._z0 = 
    temp._z1 = 
    temp._w0 = 
    temp._w1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
    temp.z[i] = b;
    temp.w[i] = b;
  }
  #endif
  arrayDiv(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayDot(XVector4* output,const XVector4SOA* a,const XVector4SOA* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    a[i].dot(output[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayDot(XVector4* output,const XVector4SOA* a,const XVector4SOA& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    a[i].dot(output[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayDot(XVector4* output,const XVector4SOA* a,const XVector4& b,const int num)
{
  XM2_ALIGN_ASSERT(&b);
  XVector4SOA _b;
  _b.set0(b);
  _b.set1(b);
  _b.set2(b);
  _b.set3(b);
  arrayDot(output,a,_b,num);
}


//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayCross(XVector4SOA* output,const XVector4SOA* a,const XVector4SOA* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].cross(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayCross(XVector4SOA* output,const XVector4SOA* a,const XVector4SOA& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].cross(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayCross(XVector4SOA* output,const XVector4SOA* a,const XVector4& b,const int num)
{
  XM2_ALIGN_ASSERT(&b);
  XVector4SOA _b;
  _b.set0(b);
  _b.set1(b);
  _b.set2(b);
  _b.set3(b);
  arrayCross(output,a,_b,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayNegate(XVector4SOA* output,const XVector4SOA* input,const int num)
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
void XVector4SOA::arrayNormalise(XVector4SOA* output,const XVector4SOA* input,const int num)
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
void XVector4SOA::arrayLerp(XVector4SOA* output,
                            const XVector4SOA* a,const XVector4SOA* b,
                            const XReal t,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].lerp(a[i],b[i],t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOA::arrayBezier(XVector4SOA* output,
                              const XVector4SOA* a,const XVector4SOA* b,
                              const XVector4SOA* c,const XVector4SOA* d,
                              const XReal t,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);
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
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XVector4SOAArray& c)
{
  ofs << (unsigned) c.size() << std::endl;
  for (XVector4SOAArray::const_iterator it = c.begin();it != c.end();++it)
  {
    ofs << *it << std::endl;
  }
  return ofs;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2EXPORT std::istream& operator>>(std::istream& ifs,XVector4SOAArray& c)
{
  unsigned sz;
  ifs >> sz;
  c.resize(sz);
  for (XVector4SOAArray::iterator it = c.begin();it != c.end();++it)
  {
    ifs >> *it;
  }
  return ifs;
}
#endif

#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XVector4SOAArray::fprint32(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOAArray::fprint64(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOAArray::fscan32(FILE* fp)
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
void XVector4SOAArray::fscan64(FILE* fp)
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
void XVector4SOAArray::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOAArray::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOAArray::fwrite32(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XVector4SOA),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOAArray::fwrite64(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XVector4SOA),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite64(fp,flip_bytes);
  }
  #endif  
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOAArray::fread32(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector4SOA),n,fp);
  if(flip_bytes) 
    XM2swap32(buffer(),n*(sizeof(XVector4SOA)/sizeof(float)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector4SOAArray::fread64(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector4SOA),n,fp);
  if(flip_bytes) 
    XM2swap64(buffer(),n*(sizeof(XVector4SOA)/sizeof(double)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread64(fp,flip_bytes);
  }
  #endif  
}
#endif
}
