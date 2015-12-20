
#include "XM2/Matrix34Array.h"


namespace XM2
{
NMTL_POD_VECTOR_EXPORT(XMatrix,XM2EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
bool XMatrix::arrayEqual(const XMatrix* a,const XMatrix* b,const int num,const XReal eps)
{
  for (int i=0;i<num;++i)
  {
    if(!a[i].equal(b[i],eps))
      return false;
  }
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool XMatrix::arrayNotEqual(const XMatrix* a,const XMatrix* b,const int num,const XReal eps)
{
  return !arrayEqual(a,b,num,eps);
}
//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayZero(XMatrix* output,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].zero();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayIdentity(XMatrix* output,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].identity();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayToQuaternionArray(XQuaternion* output,const XMatrix* input,const int num)
{
  XM2_ASSERT(input);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(input);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].fromMatrix(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayFromQuaternionArray(XMatrix* output,const XQuaternion* input,const int num)
{
  XM2_ASSERT(input);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(input);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].fromQuaternion(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayToTransformArray(XTransform* output,const XMatrix* input,const int num)
{
  XM2_ASSERT(input);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(input);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].fromMatrix(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayFromTransformArray(XMatrix* output,const XTransform* input,const int num)
{
  XM2_ASSERT(input);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(input);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    input[i].toMatrix(output[i]);
  }
}


//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayMul(XMatrix* output,const XMatrix* a,const XMatrix* b,const int num)
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
void XMatrix::arrayMul(XMatrix* output,const XMatrix* a,const XMatrix& b,const int num)
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
void XMatrix::arrayQuickMult(XMatrix* output,const XMatrix* a,const XMatrix* b,const int num)
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
    output[i].quickMult(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayQuickMult(XMatrix* output,const XMatrix* a,const XMatrix& b,const int num)
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
    output[i].quickMult(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayMul(XMatrix* output,const XReal b,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
  __m128 _t = _mm_set_ps1(b);
  #else
  __m128d _t = _mm_set1_pd(b);
  #endif
  #endif

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        _mm_stream_ps( output[i].data +  0, _mm_mul_ps(_t,output[i]._xaxis) );
        _mm_stream_ps( output[i].data +  4, _mm_mul_ps(_t,output[i]._yaxis) );
        _mm_stream_ps( output[i].data +  8, _mm_mul_ps(_t,output[i]._zaxis) );
        _mm_stream_ps( output[i].data + 12, _mm_mul_ps(_t,output[i]._waxis) );
      #else
        _mm_stream_pd( output[i].data +  0, _mm_mul_pd(_t,output[i]._xaxis_xy) );
        _mm_stream_pd( output[i].data +  2, _mm_mul_pd(_t,output[i]._xaxis_zw) );
        _mm_stream_pd( output[i].data +  4, _mm_mul_pd(_t,output[i]._yaxis_xy) );
        _mm_stream_pd( output[i].data +  6, _mm_mul_pd(_t,output[i]._yaxis_zw) );
        _mm_stream_pd( output[i].data +  8, _mm_mul_pd(_t,output[i]._zaxis_xy) );
        _mm_stream_pd( output[i].data + 10, _mm_mul_pd(_t,output[i]._zaxis_zw) );
        _mm_stream_pd( output[i].data + 12, _mm_mul_pd(_t,output[i]._waxis_xy) );
        _mm_stream_pd( output[i].data + 14, _mm_mul_pd(_t,output[i]._waxis_zw) );
      #endif
    #else
      output[i].mul(b);
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayMul(XMatrix* output,const XMatrix* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
  #if XM2_USE_FLOAT
  __m128 _t = _mm_set_ps1(b);
  #else
  __m128d _t = _mm_set1_pd(b);
  #endif
  #endif

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        _mm_stream_ps( output[i].data +  0, _mm_mul_ps(_t,a[i]._xaxis) );
        _mm_stream_ps( output[i].data +  4, _mm_mul_ps(_t,a[i]._yaxis) );
        _mm_stream_ps( output[i].data +  8, _mm_mul_ps(_t,a[i]._zaxis) );
        _mm_stream_ps( output[i].data + 12, _mm_mul_ps(_t,a[i]._waxis) );
      #else
        _mm_stream_pd( output[i].data +  0, _mm_mul_pd(_t,a[i]._xaxis_xy) );
        _mm_stream_pd( output[i].data +  2, _mm_mul_pd(_t,a[i]._xaxis_zw) );
        _mm_stream_pd( output[i].data +  4, _mm_mul_pd(_t,a[i]._yaxis_xy) );
        _mm_stream_pd( output[i].data +  6, _mm_mul_pd(_t,a[i]._yaxis_zw) );
        _mm_stream_pd( output[i].data +  8, _mm_mul_pd(_t,a[i]._zaxis_xy) );
        _mm_stream_pd( output[i].data + 10, _mm_mul_pd(_t,a[i]._zaxis_zw) );
        _mm_stream_pd( output[i].data + 12, _mm_mul_pd(_t,a[i]._waxis_xy) );
        _mm_stream_pd( output[i].data + 14, _mm_mul_pd(_t,a[i]._waxis_zw) );
      #endif
    #else
      output[i].mul(a[i],b);
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayInvert(XMatrix* output,const XMatrix* input,const int num)
{
  XM2_ASSERT(input);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(input);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].invert(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayInvert(XMatrix* output,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].invert();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayTranspose(XMatrix* output,const XMatrix* input,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].transpose(input[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::arrayTranspose(XMatrix* output,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
    output[i].transpose(output[i]);
    #else
    output[i].transpose();
    #endif
  }
}

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XMatrixArray& c)
{
  ofs << (unsigned)c.size() << std::endl;
  for (XMatrixArray::const_iterator it = c.begin();it != c.end();++it)
  {
    ofs << *it;
  }
  return ofs;
}
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ifs,XMatrixArray& c)
{
  unsigned sz;
  ifs >> sz;
  c.resize(sz);
  for (XMatrixArray::iterator it = c.begin();it != c.end();++it)
  {
    ifs >> *it;
  }
  return ifs;
}
#endif

#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XMatrixArray::fprint32(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrixArray::fprint64(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrixArray::fscan32(FILE* fp)
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
void XMatrixArray::fscan64(FILE* fp)
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
void XMatrixArray::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrixArray::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrixArray::fwrite32(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XMatrix),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrixArray::fwrite64(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XMatrix),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite64(fp,flip_bytes);
  }
  #endif  
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrixArray::fread32(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XMatrix),n,fp);
  if(flip_bytes) 
    XM2swap32(buffer(),n*16);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrixArray::fread64(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XMatrix),n,fp);
  if(flip_bytes) 
    XM2swap64(buffer(),n*16);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread64(fp,flip_bytes);
  }
  #endif  
}
#endif
}