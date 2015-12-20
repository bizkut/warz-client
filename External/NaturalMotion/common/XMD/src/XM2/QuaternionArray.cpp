
#include "XM2/QuaternionArray.h"

namespace XM2
{
NMTL_POD_VECTOR_EXPORT(XQuaternion,XM2EXPORT,XM2);
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XQuaternionArray::fprint32(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternionArray::fprint64(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternionArray::fscan32(FILE* fp)
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
void XQuaternionArray::fscan64(FILE* fp)
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
void XQuaternionArray::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternionArray::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternionArray::fwrite32(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XQuaternion),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternionArray::fwrite64(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XQuaternion),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite64(fp,flip_bytes);
  }
  #endif  
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternionArray::fread32(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XQuaternion),n,fp);
  if(flip_bytes) 
    XM2swap32(buffer(),n*4);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread32(fp);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternionArray::fread64(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XQuaternion),n,fp);
  if(flip_bytes) 
    XM2swap64(buffer(),n*4);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread64(fp,flip_bytes);
  }
  #endif  
}
#endif


//----------------------------------------------------------------------------------------------------------------------
bool XQuaternion::arrayEqual(const XQuaternion* a,const XQuaternion* b,const int num,const XReal eps)
{
  for (int i =0;i!=num;++i)
  {
    if(!a[i].equal(b[i],eps))
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XQuaternion::arrayNotEqual(const XQuaternion* a,const XQuaternion* b,const int num,const XReal eps)
{
  for (int i =0;i!=num;++i)
  {
    if(!a[i].notEqual(b[i],eps))
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayFromMatrixArray(XQuaternion* output,const XMatrix* input,const int num)
{
  XMatrix::arrayToQuaternionArray(output,input,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayToMatrixArray(XMatrix* output,const XQuaternion* input,const int num)
{
  XMatrix::arrayFromQuaternionArray(output,input,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayFromTransformArray(XQuaternion* output,const XTransform* input,const int num)
{
  XM2_ASSERT(input);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(input);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].copy(input[i].rotate);
    //_mm_stream_ps(output[i].data, _mm_load_ps(input[i].rotate.data));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayToTransformArray(XTransform* output,const XQuaternion* input,const int num)
{
  XM2_ASSERT(input);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(input);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].rotate.copy(input[i]);
    //_mm_stream_ps(output[i].rotate.data, _mm_load_ps(input[i].data));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayDot(XReal* output,const XQuaternion* a,const XQuaternion* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i] = a[i].dot(b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayZero(XQuaternion* q,const int num)
{
  XM2_ASSERT(q);
  XM2_ALIGN_ASSERT(q);

  memset(q,0,sizeof(XQuaternion)*num);
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayIdentity(XQuaternion* q,const int num)
{
  XM2_ALIGN_ASSERT(q);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    q[i].identity();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arraySum(XQuaternion* output,const XQuaternion* a,const XReal t,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].sum(a[i],t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayAdd(XQuaternion* output,const XQuaternion* a,const XQuaternion* b,const int num)
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
  for(int i=0;i<num;++i)
  {
    output[i].add(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayAdd(XQuaternion* output,const XQuaternion* a,const XQuaternion& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].add(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayMul(XQuaternion* output,const XQuaternion* a,const XQuaternion* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  // must not be the same data!
  XM2_ASSERT(a != output);
  XM2_ASSERT(b != output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].mul(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayMul(XQuaternion* output,const XQuaternion* a,const XQuaternion& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);

  // must not be the same data!
  XM2_ASSERT(a != output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].mul(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayMul(XQuaternion* output,const XQuaternion* b,const int num)
{
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].mul(b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayMul(XQuaternion* output,const XQuaternion& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].mul(b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayMul(XQuaternion* output,const XQuaternion* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].mul(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayMul(XQuaternion* output,const XQuaternion* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      __m128 _b = _mm_set_ps1(b);
    #else
      __m128d _b = _mm_set1_pd(b);
    #endif
  #endif

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        output[i].sse = _mm_mul_ps(a[i].sse,_b);
      #else
        output[i].xy = _mm_mul_pd(a[i].xy,_b);
        output[i].zw = _mm_mul_pd(a[i].zw,_b);
      #endif
    #else
      output[i].mul(a[i],b);
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arraySlerp(XQuaternion* output,const XQuaternion* a,const XQuaternion* b,const XReal t,const int num)
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
  for(int i=0;i<num;++i)
  {
    output[i].slerp(a[i],b[i],t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayLerp(XQuaternion* output,const XQuaternion* a,const XQuaternion* b,const XReal t,const int num)
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
  for(int i=0;i<num;++i)
  {
    output[i].lerp(a[i],b[i],t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayNegate(XQuaternion* q,const XQuaternion* input,const int num)
{
  XM2_ASSERT(q);
  XM2_ALIGN_ASSERT(q);
  XM2_ASSERT(input);
  XM2_ALIGN_ASSERT(input);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    q[i].negate(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayInvert(XQuaternion* q,const XQuaternion* input,const int num)
{
  XM2_ASSERT(q);
  XM2_ALIGN_ASSERT(q);
  XM2_ASSERT(input);
  XM2_ALIGN_ASSERT(input);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    q[i].invert(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XQuaternion::arrayNormalise(XQuaternion* q,const XQuaternion* input,const int num)
{
  XM2_ASSERT(q);
  XM2_ALIGN_ASSERT(q);
  XM2_ASSERT(input);
  XM2_ALIGN_ASSERT(input);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    q[i].normalise(input[i]);
  }
}

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XQuaternionArray& c)
{
  ofs << (unsigned)c.size() << std::endl;
  for (XM2::XQuaternionArray::const_iterator it = c.begin();it != c.end();++it)
  {
    ofs << *it;
  }
  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ifs,XQuaternionArray& c)
{
  unsigned sz;
  ifs >> sz;
  c.resize(sz);
  for (XM2::XQuaternionArray::iterator it = c.begin();it != c.end();++it)
  {
    ifs >> *it;
  }
  return ifs;
}
#endif
}
