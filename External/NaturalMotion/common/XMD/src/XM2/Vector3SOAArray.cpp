/*---------------------------------------------------------------------------------------*/
/*!
 *  \file   Vector3SOAArray.cpp
 *  \date   19-05-2008
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Vector3SOA.h"
#include "XM2/Vector3SOAArray.h"

namespace XM2
{
NMTL_POD_VECTOR_EXPORT(XVector3SOA,XM2EXPORT,XM2);
#ifdef XM2_USE_SSE
#if XM2_USE_FLOAT
//----------------------------------------------------------------------------------------------------------------------  
const XVector3SOAArray& XVector3SOAArray::operator=(const XVector3SOAArray& rhs)
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
  }
  return *this;
}
#else
//----------------------------------------------------------------------------------------------------------------------  
const XVector3SOAArray& XVector3SOAArray::operator=(const XVector3SOAArray& rhs)
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
  }
  return *this;
}
#endif
#else
//----------------------------------------------------------------------------------------------------------------------  
const XVector3SOAArray& XVector3SOAArray::operator=(const XVector3SOAArray& rhs)
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
    }
  }
  return *this;
}
#endif
//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayZero(XVector3SOA* ptr,const int num)
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
void XVector3SOA::arrayNegate(XVector3SOA* ptr,const XVector3SOA* input,const int num)
{
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    ptr[i].negate(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayAdd(XVector3SOA* output,const XVector3SOA* a,const XVector3SOA* b,const int num)
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
void XVector3SOA::arrayAdd(XVector3SOA* output,const XVector3SOA* a,const XVector3SOA& b,const int num)
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
void XVector3SOA::arrayAdd(XVector3SOA* output,const XVector3SOA* a,const XVector3& b,const int num)
{
  XVector3SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    temp._z = _mm_set_ps1(b.z);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    temp._z0 = _mm_set1_pd(b.z);
    temp._z1 = _mm_set1_pd(b.z);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
    temp.z[i] = b.z;
  }
  #endif
  arrayAdd(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayAdd(XVector3SOA* output,const XVector3SOA* a,const XReal b,const int num)
{
  XVector3SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b);
    temp._y = _mm_set_ps1(b);
    temp._z = _mm_set_ps1(b);
    #else
    temp._x0 = _mm_set1_pd(b);
    temp._x1 = _mm_set1_pd(b);
    temp._y0 = _mm_set1_pd(b);
    temp._y1 = _mm_set1_pd(b);
    temp._z0 = _mm_set1_pd(b);
    temp._z1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
    temp.z[i] = b;
  }
  #endif
  arrayAdd(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arraySub(XVector3SOA* output,const XVector3SOA* a,const XVector3SOA* b,const int num)
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
void XVector3SOA::arraySub(XVector3SOA* output,const XVector3SOA* a,const XVector3SOA& b,const int num)
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
void XVector3SOA::arraySub(XVector3SOA* output,const XVector3SOA* a,const XVector3& b,const int num)
{
  XVector3SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    temp._z = _mm_set_ps1(b.z);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    temp._z0 = _mm_set1_pd(b.z);
    temp._z1 = _mm_set1_pd(b.z);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
    temp.z[i] = b.z;
  }
  #endif
  arraySub(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arraySub(XVector3SOA* output,const XVector3SOA* a,const XReal b,const int num)
{
  XVector3SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b);
    temp._y = _mm_set_ps1(b);
    temp._z = _mm_set_ps1(b);
    #else
    temp._x0 = _mm_set1_pd(b);
    temp._x1 = _mm_set1_pd(b);
    temp._y0 = _mm_set1_pd(b);
    temp._y1 = _mm_set1_pd(b);
    temp._z0 = _mm_set1_pd(b);
    temp._z1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
    temp.z[i] = b;
  }
  #endif
  arraySub(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayMul(XVector3SOA* output,const XVector3SOA* a,const XVector3SOA* b,const int num)
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
void XVector3SOA::arrayMul(XVector3SOA* output,const XVector3SOA* a,const XVector3SOA& b,const int num)
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
void XVector3SOA::arrayMul(XVector3SOA* output,const XVector3SOA* a,const XVector3& b,const int num)
{
  XVector3SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    temp._z = _mm_set_ps1(b.z);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    temp._z0 = _mm_set1_pd(b.z);
    temp._z1 = _mm_set1_pd(b.z);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
    temp.z[i] = b.z;
  }
  #endif
  arrayMul(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayMul(XVector3SOA* output,const XVector3SOA* a,const XReal b,const int num)
{
  XVector3SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b);
    temp._y = _mm_set_ps1(b);
    temp._z = _mm_set_ps1(b);
    #else
    temp._x0 = _mm_set1_pd(b);
    temp._x1 = _mm_set1_pd(b);
    temp._y0 = _mm_set1_pd(b);
    temp._y1 = _mm_set1_pd(b);
    temp._z0 = _mm_set1_pd(b);
    temp._z1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
    temp.z[i] = b;
  }
  #endif
  arrayMul(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayDiv(XVector3SOA* output,const XVector3SOA* a,const XVector3SOA* b,const int num)
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
void XVector3SOA::arrayDiv(XVector3SOA* output,const XVector3SOA* a,const XVector3SOA& b,const int num)
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
void XVector3SOA::arrayDiv(XVector3SOA* output,const XVector3SOA* a,const XVector3& b,const int num)
{
  XVector3SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b.x);
    temp._y = _mm_set_ps1(b.y);
    temp._z = _mm_set_ps1(b.z);
    #else
    temp._x0 = _mm_set1_pd(b.x);
    temp._x1 = _mm_set1_pd(b.x);
    temp._y0 = _mm_set1_pd(b.y);
    temp._y1 = _mm_set1_pd(b.y);
    temp._z0 = _mm_set1_pd(b.z);
    temp._z1 = _mm_set1_pd(b.z);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b.x;
    temp.y[i] = b.y;
    temp.z[i] = b.z;
  }
  #endif
  arrayDiv(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayDiv(XVector3SOA* output,const XVector3SOA* a,const XReal b,const int num)
{
  XVector3SOA temp;
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
    temp._x = _mm_set_ps1(b);
    temp._y = _mm_set_ps1(b);
    temp._z = _mm_set_ps1(b);
    #else
    temp._x0 = _mm_set1_pd(b);
    temp._x1 = _mm_set1_pd(b);
    temp._y0 = _mm_set1_pd(b);
    temp._y1 = _mm_set1_pd(b);
    temp._z0 = _mm_set1_pd(b);
    temp._z1 = _mm_set1_pd(b);
    #endif
  #else
  for(int i=0;i!=4;++i)
  {
    temp.x[i] = b;
    temp.y[i] = b;
    temp.z[i] = b;
  }
  #endif
  arrayDiv(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayDot(XVector4* product,const XVector3SOA* a,const XVector3SOA* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(product);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(product);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    a[i].dot(product[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayDot(XVector4* product,const XVector3SOA* a,const XVector3SOA& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(product);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(product);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    a[i].dot(product[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayDot(XVector4* product,const XVector3SOA* a,const XVector3& b,const int num)
{
  XM2_ALIGN_ASSERT(&b);
  XVector3SOA _b;
  _b.set0(b);
  _b.set1(b);
  _b.set2(b);
  _b.set3(b);
  arrayDot(product,a,_b,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayCross(XVector3SOA* output,const XVector3SOA* a,const XVector3SOA* b,const int num)
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
void XVector3SOA::arrayCross(XVector3SOA* output,const XVector3SOA* a,const XVector3SOA& b,const int num)
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
void XVector3SOA::arrayCross(XVector3SOA* output,const XVector3SOA* a,const XVector3& b,const int num)
{
  XM2_ALIGN_ASSERT(&b);
  XVector3SOA _b;
  _b.set0(b);
  _b.set1(b);
  _b.set2(b);
  _b.set3(b);
  arrayCross(output,a,_b,num);
}


//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayNormalise(XVector3SOA* output,const XVector3SOA* input,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].normalise(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOA::arrayLerp(XVector3SOA* output,
                            const XVector3SOA* a,const XVector3SOA* b,
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
void XVector3SOA::arrayBezier(XVector3SOA* output,
                              const XVector3SOA* a,const XVector3SOA* b,
                              const XVector3SOA* c,const XVector3SOA* d,
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
XM2_INLINE std::ostream& operator<<(std::ostream& ofs,const XVector3SOAArray& c)
{
  ofs << (unsigned) c.size() << std::endl;
  for (XVector3SOAArray::const_iterator it = c.begin();it != c.end();++it)
  {
    ofs << *it << std::endl;
  }
  return ofs;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE std::istream& operator>>(std::istream& ifs,XVector3SOAArray& c)
{
  unsigned sz;
  ifs >> sz;
  c.resize(sz);
  for (XVector3SOAArray::iterator it = c.begin();it != c.end();++it)
  {
    ifs >> *it;
  }
  return ifs;
}
#endif

#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XVector3SOAArray::fprint32(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOAArray::fprint64(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOAArray::fscan32(FILE* fp)
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
void XVector3SOAArray::fscan64(FILE* fp)
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
void XVector3SOAArray::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOAArray::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOAArray::fwrite32(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XVector3SOA),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOAArray::fwrite64(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XVector3SOA),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite64(fp,flip_bytes);
  }
  #endif  
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOAArray::fread32(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector3SOA),n,fp);
  if(flip_bytes) 
    XM2swap32(buffer(),n*(sizeof(XVector3SOA)/sizeof(float)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3SOAArray::fread64(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector3SOA),n,fp);
  if(flip_bytes) 
    XM2swap64(buffer(),n*(sizeof(XVector3SOA)/sizeof(double)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread64(fp,flip_bytes);
  }
  #endif  
}
#endif
}
