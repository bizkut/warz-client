/*---------------------------------------------------------------------------------------*/
/*!
 *  \file   Vector2SOAArray.cpp
 *  \date   19-05-2008
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Vector2SOA.h"
#include "XM2/Vector2SOAArray.h"

namespace XM2
{
NMTL_POD_VECTOR_EXPORT(XVector2SOA,XM2EXPORT,XM2);
#ifdef XM2_USE_SSE
#if XM2_USE_FLOAT
//----------------------------------------------------------------------------------------------------------------------  
const XVector2SOAArray& XVector2SOAArray::operator=(const XVector2SOAArray& rhs)
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
  }
  return *this;
}
#else
//----------------------------------------------------------------------------------------------------------------------  
const XVector2SOAArray& XVector2SOAArray::operator=(const XVector2SOAArray& rhs)
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
  }
  return *this;
}
#endif
#else
//----------------------------------------------------------------------------------------------------------------------  
const XVector2SOAArray& XVector2SOAArray::operator=(const XVector2SOAArray& rhs)
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
    }
  }
  return *this;
}
#endif
//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::arrayZero(XVector2SOA* ptr,const int num)
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
void XVector2SOA::arrayAdd(XVector2SOA* output,const XVector2SOA* a,const XVector2SOA* b,const int num)
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
void XVector2SOA::arrayAdd(XVector2SOA* output,const XVector2SOA* a,const XVector2SOA& b,const int num)
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
void XVector2SOA::arrayAdd(XVector2SOA* output,const XVector2SOA* a,const XVector2& b,const int num)
{
  XVector2SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
  }
  #endif
  arrayAdd(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::arrayAdd(XVector2SOA* output,const XVector2SOA* a,const XReal b,const int num)
{
  XVector2SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b);
    temp._y = _mm_set_ps1(b);
    #else
    temp._x0 = _mm_set1_pd(b);
    temp._x1 = _mm_set1_pd(b);
    temp._y0 = _mm_set1_pd(b);
    temp._y1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
  }
  #endif
  arrayAdd(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::arraySub(XVector2SOA* output,const XVector2SOA* a,const XVector2SOA* b,const int num)
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
void XVector2SOA::arraySub(XVector2SOA* output,const XVector2SOA* a,const XVector2SOA& b,const int num)
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
void XVector2SOA::arraySub(XVector2SOA* output,const XVector2SOA* a,const XVector2& b,const int num)
{
  XVector2SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
  }
  #endif
  arraySub(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::arraySub(XVector2SOA* output,const XVector2SOA* a,const XReal b,const int num)
{
  XVector2SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b);
    temp._y = _mm_set_ps1(b);
    #else
    temp._x0 = _mm_set1_pd(b);
    temp._x1 = _mm_set1_pd(b);
    temp._y0 = _mm_set1_pd(b);
    temp._y1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
  }
  #endif
  arraySub(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::arrayMul(XVector2SOA* output,const XVector2SOA* a,const XVector2SOA* b,const int num)
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
void XVector2SOA::arrayMul(XVector2SOA* output,const XVector2SOA* a,const XVector2SOA& b,const int num)
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
void XVector2SOA::arrayMul(XVector2SOA* output,const XVector2SOA* a,const XVector2& b,const int num)
{
  XVector2SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
  }
  #endif
  arrayMul(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::arrayMul(XVector2SOA* output,const XVector2SOA* a,const XReal b,const int num)
{
  XVector2SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b);
    temp._y = _mm_set_ps1(b);
    #else
    temp._x0 = _mm_set1_pd(b);
    temp._x1 = _mm_set1_pd(b);
    temp._y0 = _mm_set1_pd(b);
    temp._y1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
  }
  #endif
  arrayMul(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::arrayDiv(XVector2SOA* output,const XVector2SOA* a,const XVector2SOA* b,const int num)
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
void XVector2SOA::arrayDiv(XVector2SOA* output,const XVector2SOA* a,const XVector2SOA& b,const int num)
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
void XVector2SOA::arrayDiv(XVector2SOA* output,const XVector2SOA* a,const XVector2& b,const int num)
{
  XVector2SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
  }
  #endif
  arrayDiv(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::arrayDiv(XVector2SOA* output,const XVector2SOA* a,const XReal b,const int num)
{
  XVector2SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b);
    temp._y = _mm_set_ps1(b);
    #else
    temp._x0 = _mm_set1_pd(b);
    temp._x1 = _mm_set1_pd(b);
    temp._y0 = _mm_set1_pd(b);
    temp._y1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
  }
  #endif
  arrayDiv(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::arrayNormalise(XVector2SOA* output,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].normalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOA::arrayLerp(XVector2SOA* output,
                            const XVector2SOA* a,const XVector2SOA* b,
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
void XVector2SOA::arrayBezier(XVector2SOA* output,
                              const XVector2SOA* a,const XVector2SOA* b,
                              const XVector2SOA* c,const XVector2SOA* d,
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
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XVector2SOAArray& c)
{
  ofs << (unsigned) c.size() << std::endl;
  for (XVector2SOAArray::const_iterator it = c.begin();it != c.end();++it)
  {
    ofs << *it << std::endl;
  }
  return ofs;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2EXPORT std::istream& operator>>(std::istream& ifs,XVector2SOAArray& c)
{
  unsigned sz;
  ifs >> sz;
  c.resize(sz);
  for (XVector2SOAArray::iterator it = c.begin();it != c.end();++it)
  {
    ifs >> *it;
  }
  return ifs;
}
#endif
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XVector2SOAArray::fprint32(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOAArray::fprint64(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOAArray::fscan32(FILE* fp)
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
void XVector2SOAArray::fscan64(FILE* fp)
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
void XVector2SOAArray::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOAArray::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOAArray::fwrite32(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XVector2SOA),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOAArray::fwrite64(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XVector2SOA),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite64(fp,flip_bytes);
  }
  #endif  
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOAArray::fread32(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector2SOA),n,fp);
  if(flip_bytes) 
    XM2swap32(buffer(),n*(sizeof(XVector2SOA)/sizeof(float)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector2SOAArray::fread64(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector2SOA),n,fp);
  if(flip_bytes) 
    XM2swap64(buffer(),n*(sizeof(XVector2SOA)/sizeof(double)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread64(fp,flip_bytes);
  }
  #endif  
}
#endif
}
