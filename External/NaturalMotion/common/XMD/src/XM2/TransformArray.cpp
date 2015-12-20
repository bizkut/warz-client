#include "XM2/TransformArray.h"
namespace XM2
{
NMTL_POD_VECTOR_EXPORT(XTransform,XM2EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
void XTransform::arrayEvaluate(const XTransform* tr,XMatrix* output,const int num)
{
  XM2_ASSERT(tr);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(tr);
  XM2_ALIGN_ASSERT(output);
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    tr[i].toMatrix(output[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::arrayLerp(XTransform* output,const XTransform* a,const XTransform* b,const XReal t,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      __m128 _t = _mm_set_ps1(t);
    #else
      __m128d _t = _mm_set1_pd(t);
    #endif
  #endif
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        output[i].translate.sse = _mm_add_ps(a[i].translate.sse,_mm_mul_ps(_t,_mm_sub_ps(b[i].translate.sse,a[i].translate.sse)));
        output[i].rotate.sse = _mm_add_ps(a[i].rotate.sse,_mm_mul_ps(_t,_mm_sub_ps(b[i].rotate.sse,a[i].rotate.sse)));
        output[i].scale.sse = _mm_add_ps(a[i].scale.sse,_mm_mul_ps(_t,_mm_sub_ps(b[i].scale.sse,a[i].scale.sse)));
      #else
        output[i].translate.xy = _mm_add_pd(a[i].translate.xy,_mm_mul_pd(_t,_mm_sub_pd(b[i].translate.xy,a[i].translate.xy)));
        output[i].translate.zw = _mm_add_pd(a[i].translate.zw,_mm_mul_pd(_t,_mm_sub_pd(b[i].translate.zw,a[i].translate.zw)));
        output[i].rotate.xy = _mm_add_pd(a[i].rotate.xy,_mm_mul_pd(_t,_mm_sub_pd(b[i].rotate.xy,a[i].rotate.xy)));
        output[i].rotate.zw = _mm_add_pd(a[i].rotate.zw,_mm_mul_pd(_t,_mm_sub_pd(b[i].rotate.zw,a[i].rotate.zw)));
        output[i].scale.xy = _mm_add_pd(a[i].scale.xy,_mm_mul_pd(_t,_mm_sub_pd(b[i].scale.xy,a[i].scale.xy)));
        output[i].scale.zw = _mm_add_pd(a[i].scale.zw,_mm_mul_pd(_t,_mm_sub_pd(b[i].scale.zw,a[i].scale.zw)));
      #endif
      output[i].rotate.normalise();
    #else
      output[i].lerp(a[i],b[i],t);
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::arraySlerp(XTransform* output,const XTransform* a,const XTransform* b,const XReal t,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      __m128 _t = _mm_set_ps1(t);
    #else
      __m128d _t = _mm_set1_pd(t);
    #endif
  #endif
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        output[i].scale.sse = _mm_add_ps(a[i].scale.sse,_mm_mul_ps(_t,_mm_sub_ps(b[i].scale.sse,a[i].scale.sse)));
        output[i].translate.sse = _mm_add_ps(a[i].translate.sse,_mm_mul_ps(_t,_mm_sub_ps(b[i].translate.sse,a[i].translate.sse)));
      #else
        output[i].translate.xy = _mm_add_pd(a[i].translate.xy,_mm_mul_pd(_t,_mm_sub_pd(b[i].translate.xy,a[i].translate.xy)));
        output[i].translate.zw = _mm_add_pd(a[i].translate.zw,_mm_mul_pd(_t,_mm_sub_pd(b[i].translate.zw,a[i].translate.zw)));
        output[i].scale.xy = _mm_add_pd(a[i].scale.xy,_mm_mul_pd(_t,_mm_sub_pd(b[i].scale.xy,a[i].scale.xy)));
        output[i].scale.zw = _mm_add_pd(a[i].scale.zw,_mm_mul_pd(_t,_mm_sub_pd(b[i].scale.zw,a[i].scale.zw)));
      #endif
      output[i].rotate.slerp(a[i].rotate,b[i].rotate,t);
    #else
      output[i].lerp(a[i],b[i],t);
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::arraySum(XTransform* output,const XTransform* input,const XReal weight,const int num)
{
  XM2_ASSERT(input);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(input);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      __m128 _t = _mm_set_ps1(weight);
    #else
      __m128d _t = _mm_set1_pd(weight);
    #endif
  #endif
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        output[i].translate.sse = _mm_add_ps(output[i].translate.sse,_mm_mul_ps(_t,input[i].translate.sse));
        output[i].rotate.sse = _mm_add_ps(output[i].rotate.sse,_mm_mul_ps(_t,input[i].rotate.sse));
        output[i].scale.sse = _mm_add_ps(output[i].scale.sse,_mm_mul_ps(_t,input[i].scale.sse));
      #else
        output[i].translate.xy = _mm_add_pd(output[i].translate.xy,_mm_mul_pd(_t,input[i].translate.xy));
        output[i].translate.zw = _mm_add_pd(output[i].translate.zw,_mm_mul_pd(_t,input[i].translate.zw));
        output[i].rotate.xy = _mm_add_pd(output[i].rotate.xy,_mm_mul_pd(_t,input[i].rotate.xy));
        output[i].rotate.zw = _mm_add_pd(output[i].rotate.zw,_mm_mul_pd(_t,input[i].rotate.zw));
        output[i].scale.xy = _mm_add_pd(output[i].scale.xy,_mm_mul_pd(_t,input[i].scale.xy));
        output[i].scale.zw = _mm_add_pd(output[i].scale.zw,_mm_mul_pd(_t,input[i].scale.zw));
      #endif
    #else
      output[i].translate.sum(input[i].translate,weight);
      output[i].rotate.sum(input[i].rotate,weight);
      output[i].scale.sum(input[i].scale,weight);
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XTransform::arrayEqual(const XTransform* a,const XTransform* b,const int num,const XReal eps)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  for (int i=0;i<num;++i)
  {
    if(!a[i].equal(b[i],eps))
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTransform::arrayNotEqual(const XTransform* a,const XTransform* b,const int num,const XReal eps)
{
  return !arrayEqual(a,b,num,eps);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::arrayFromMatrixArray(XTransform* output,const XMatrix* input,const int num)
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
    output[i].fromMatrix(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::arrayToMatrixArray(XMatrix* output,const XTransform* input,const int num)
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
    input[i].toMatrix(output[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::arrayFromQuaternionArray(XTransform* output,const XQuaternion* input,const int num)
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
    output[i].rotate = input[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::arrayToQuaternionArray(XQuaternion* output,const XTransform* input,const int num)
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
    output[i] = input[i].rotate;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::arrayZero(XTransform* output,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);
  memset(output,0,sizeof(XTransform)*num);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::arrayNormalise(XTransform* output,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].rotate.normalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::arrayIdentity(XTransform* output,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].translate.zero();
    output[i].rotate.identity();
    output[i].scale.set(1.0f,1.0f,1.0f);
  }
}

#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XTransformArray::fprint32(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XTransformArray::fprint64(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XTransformArray::fscan32(FILE* fp)
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
void XTransformArray::fscan64(FILE* fp)
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
void XTransformArray::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransformArray::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransformArray::fwrite32(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XTransform),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XTransformArray::fwrite64(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XTransform),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite64(fp,flip_bytes);
  }
  #endif  
}

//----------------------------------------------------------------------------------------------------------------------
void XTransformArray::fread32(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XTransform),n,fp);
  if(flip_bytes) 
    XM2swap32(buffer(),n*(sizeof(XTransform)/sizeof(float)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XTransformArray::fread64(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XTransform),n,fp);
  if(flip_bytes) 
    XM2swap64(buffer(),n*(sizeof(XTransform)/sizeof(double)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread64(fp,flip_bytes);
  }
  #endif  
}
#endif

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XTransformArray& c)
{
  ofs << (unsigned)c.size() << std::endl;
  for (XM2::XTransformArray::const_iterator it = c.begin();it != c.end();++it)
  {
    ofs << *it;
  }
  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ifs,XTransformArray& c)
{
  unsigned sz;
  ifs >> sz;
  c.resize(sz);
  for (XM2::XTransformArray::iterator it = c.begin();it != c.end();++it)
  {
    ifs >> *it;
  }
  return ifs;
}
#endif
}
