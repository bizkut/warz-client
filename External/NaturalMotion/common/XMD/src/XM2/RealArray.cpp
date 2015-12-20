
#include "XM2/RealArray.h"

NMTL_POD_VECTOR_EXPORT(XM2::XReal,XM2EXPORT,XM2);
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::zero()
{
  memset(buffer(),0,sizeof(XReal)*size());
}
//----------------------------------------------------------------------------------------------------------------------
bool XRealArray::equal(const XRealArray& b,const XReal eps) const
{
  if(b.size()!=size())
    return false;

  for (size_t i=0;i<size();++i)
  {
    if(!float_equal(buffer()[i],b[i],eps))
      return false;
  }
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool XRealArray::notEqual(const XRealArray& b,const XReal eps) const
{
   return !equal(b,eps);
}

#if defined(XM2_USE_SSE)
#if XM2_USE_FLOAT
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayMul(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  int num_by_4 = num/4;
  size_t t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_4;++i)
  {
    _mm_stream_ps(output + t,_mm_mul_ps(_mm_load_ps(a+t),_mm_load_ps(b+t)));
    t += 4;
  }
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=num_by_4*4;i<num;++i)
  {
    output[i] = a[i] * b[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayDiv(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  int num_by_4 = num/4;
  size_t t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_4;++i)
  {
    _mm_stream_ps(output + t,_mm_div_ps(_mm_load_ps(a+t),_mm_load_ps(b+t)));
    t += 4;
  }
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=num_by_4*4;i<num;++i)
  {
    output[i] = a[i] / b[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayAdd(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  int num_by_4 = num/4;
  size_t t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_4;++i)
  {
    _mm_stream_ps(output + t,_mm_add_ps(_mm_load_ps(a+t),_mm_load_ps(b+t)));
    t += 4;
  }
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=num_by_4*4;i<num;++i)
  {
    output[i] = a[i] + b[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arraySub(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  int num_by_4 = num/4;
  size_t t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_4;++i)
  {
    _mm_stream_ps(output + t,_mm_sub_ps(_mm_load_ps(a+t),_mm_load_ps(b+t)));
    t += 4;
  }
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=num_by_4*4;i<num;++i)
  {
    output[i] = a[i] - b[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayMul(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  const __m128 _b = _mm_set_ps1(b);

  int num_by_4 = num/4;
  size_t t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_4;++i)
  {
    _mm_stream_ps(output + t,_mm_mul_ps(_mm_load_ps(a+t),_b));
    t += 4;
  }
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=num_by_4*4;i<num;++i)
  {
    output[i] = a[i] * b;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayDiv(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  const __m128 _b = _mm_set_ps1(b);
  int num_by_4 = num/4;
  size_t t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_4;++i)
  {
    _mm_stream_ps(output + t,_mm_div_ps(_mm_load_ps(a+t),_b));
    t += 4;
  }
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=num_by_4*4;i<num;++i)
  {
    output[i] = a[i] / b;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayAdd(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  const __m128 _b = _mm_set_ps1(b);
  int num_by_4 = num/4;
  size_t t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_4;++i)
  {
    _mm_stream_ps(output + t,_mm_add_ps(_mm_load_ps(a+t),_b));
    t += 4;
  }
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=num_by_4*4;i<num;++i)
  {
    output[i] = a[i] + b;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arraySub(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  const __m128 _b = _mm_set_ps1(b);
  int num_by_4 = num/4;
  size_t t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_4;++i)
  {
    _mm_stream_ps(output + t,_mm_sub_ps(_mm_load_ps(a+t),_b));
    t += 4;
  }
  
  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=num_by_4*4;i<num;++i)
  {
    output[i] = a[i] - b;
  }
}
#else

//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayMul(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  int num_by_2 = num/2;
  int t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_2;++i)
  {
    _mm_stream_pd(output + t,_mm_mul_pd(_mm_load_pd(a+t),_mm_load_pd(b+t)));
    t += 2;
  }

  if(t != num)
  {
    output[t] = a[t] * b[t];
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayDiv(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  int num_by_2 = num/2;
  int t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_2;++i)
  {
    _mm_stream_pd(output + t,_mm_div_pd(_mm_load_pd(a+t),_mm_load_pd(b+t)));
    t += 2;
  }

  if(t != num)
  {
    output[t] = a[t] / b[t];
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayAdd(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  int num_by_2 = num/2;
  int t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_2;++i)
  {
    _mm_stream_pd(output + t,_mm_add_pd(_mm_load_pd(a+t),_mm_load_pd(b+t)));
    t += 2;
  }

  if(t != num)
  {
    output[t] = a[t] + b[t];
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arraySub(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  int num_by_2 = num/2;
  int t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_2;++i)
  {
    _mm_stream_pd(output + t,_mm_sub_pd(_mm_load_pd(a+t),_mm_load_pd(b+t)));
    t += 2;
  }

  if(t != num)
  {
    output[t] = a[t] - b[t];
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayMul(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  const __m128d _b = _mm_set1_pd(b);
  int num_by_2 = num/2;
  int t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_2;++i)
  {
    _mm_stream_pd(output + t,_mm_mul_pd(_mm_load_pd(a+t),_b));
    t += 2;
  }
  
  if(t != num)
  {
    output[t] = a[t] * b;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayDiv(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  const __m128d _b = _mm_set1_pd(b);
  int num_by_2 = num/2;
  int t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_2;++i)
  {
    _mm_stream_pd(output + t,_mm_div_pd(_mm_load_pd(a+t),_b));
    t += 2;
  }
  
  if(t != num)
  {
    output[t] = a[t] / b;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayAdd(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  const __m128d _b = _mm_set1_pd(b);
  int num_by_2 = num/2;
  int t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_2;++i)
  {
    _mm_stream_pd(output + t,_mm_add_pd(_mm_load_pd(a+t),_b));
    t += 2;
  }
  
  if(t != num)
  {
    output[t] = a[t] + b;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arraySub(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  const __m128d _b = _mm_set1_pd(b);
  int num_by_2 = num/2;
  int t=0;

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num_by_2;++i)
  {
    _mm_stream_pd(output + t,_mm_sub_pd(_mm_load_pd(a+t),_b));
    t += 2;
  }
  
  if(t != num)
  {
    output[t] = a[t] - b;
  }
}
#endif
#else
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayMul(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i] = a[i] * b[i];
  }    
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayDiv(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i] = a[i] / b[i];
  }    
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayAdd(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i] = a[i] + b[i];
  }    
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arraySub(XReal* output,const XReal* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i] = a[i] - b[i];
  }    
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayMul(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i] = a[i] * b;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayDiv(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i] = a[i] / b;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arrayAdd(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i] = a[i] + b;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XRealArray::arraySub(XReal* output,const XReal* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i] = a[i] - b;
  }
}
#endif
}
