#include "XM2/Vector3PackedArray.h"
#include "XM2/Vector3SOA.h"
namespace XM2
{
NMTL_POD_VECTOR_EXPORT(XVector3Packed,XM2EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayZero(XVector3Packed* data,const int num)
{
  memset(data,0,num*sizeof(XVector3Packed));
}
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDot(XReal* product,const XVector3Packed* a,const XVector3Packed* b,const int num)
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
void XVector3Packed::arrayDot(XReal* product,const XVector3Packed* a,const XVector3Packed& b,const int num)
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
void XVector3Packed::arrayCross(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(b);
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
void XVector3Packed::arrayCross(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed& b,const int num)
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
    output[i].cross(a[i],b);
  }
}
#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayLerp(XVector3Packed* output,
                               const XVector3Packed* a,
                               const XVector3Packed* b,
                               const XReal t,
                               const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  int n3 = (num*3);
  int n4 = n3/4;
  int nstart = (n4*4);

  const __m128 _t = _mm_set_ps1(t);
  __m128* _o = (__m128*)((void*)output);

  const __m128* _a = (const __m128*)((const void*)a);
  const __m128* _b = (const __m128*)((const void*)b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_add_ps(_a[i],_mm_mul_ps(_mm_sub_ps(_b[i],_a[i]),_t));
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] + t * (_fb[i]-_fa[i]);
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayLerp(XVector3Packed* output,
                               const XVector3Packed* a,
                               const XVector3Packed* b,
                               const XReal t,
                               const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  int n3 = (num*3);
  int n4 = n3/2;
  int nstart = (n4*2);

  const __m128d _t = _mm_set1_pd(t);
  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _a = (const __m128d*)((const void*)a);
  const __m128d* _b = (const __m128d*)((const void*)b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_add_pd(_a[i],_mm_mul_pd(_mm_sub_pd(_b[i],_a[i]),_t));
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] + t * (_fb[i]-_fa[i]);
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayLerp(XVector3Packed* output,
                               const XVector3Packed* a,
                               const XVector3Packed* b,
                               const XReal t,
                               const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].lerp(a[i],b[i],t);
  }
}
#endif  

#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayBezier(XVector3Packed* output,
                                 const XVector3Packed* a,
                                 const XVector3Packed* b,
                                 const XVector3Packed* c,
                                 const XVector3Packed* d,
                                 const XReal t,
                                 const int num)
{
  XReal B0,B1,B2,B3;
  __m128 _B0;
  __m128 _B1;
  __m128 _B2;
  __m128 _B3;
  {
    XReal invt = (1.0f-t);
    B0 = invt*invt*invt;
    B1 = 3.0f*t*invt*invt;
    B2 = 3.0f*t*t*invt;
    B3 = t*t*t;

    _B0 = _mm_set_ps1(B0);
    _B1 = _mm_set_ps1(B1);
    _B2 = _mm_set_ps1(B2);
    _B3 = _mm_set_ps1(B3);
  }

  int n3 = (num*3);
  int n4 = n3/4;
  int nstart = (n4*4);

  __m128* _o = (__m128*)((void*)output);

  const __m128* _a = (const __m128*)((const void*)a);
  const __m128* _b = (const __m128*)((const void*)b);
  const __m128* _c = (const __m128*)((const void*)c);
  const __m128* _d = (const __m128*)((const void*)d);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_a[i],_B0),_mm_mul_ps(_b[i],_B1)),
                       _mm_add_ps(_mm_mul_ps(_c[i],_B2),_mm_mul_ps(_d[i],_B3)));
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));
  const XReal* _fc = (const XReal*)((const void*)(_c+n4));
  const XReal* _fd = (const XReal*)((const void*)(_d+n4));

  for (int i=nstart;i<n3;++i)
  {
    output[i].x = B0*_fa[i] + B1*_fb[i] + B2*_fc[i] + B3*_fd[i];
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayBezier(XVector3Packed* output,
                                 const XVector3Packed* a,
                                 const XVector3Packed* b,
                                 const XVector3Packed* c,
                                 const XVector3Packed* d,
                                 const XReal t,
                                 const int num)
{
  XReal B0,B1,B2,B3;
  __m128d _B0;
  __m128d _B1;
  __m128d _B2;
  __m128d _B3;
  {
    XReal invt = (1.0f-t);
    B0 = invt*invt*invt;
    B1 = 3.0f*t*invt*invt;
    B2 = 3.0f*t*t*invt;
    B3 = t*t*t;

    _B0 = _mm_set1_pd(B0);
    _B1 = _mm_set1_pd(B1);
    _B2 = _mm_set1_pd(B2);
    _B3 = _mm_set1_pd(B3);
  }

  int n3 = (num*3);
  int n4 = n3/2;
  int nstart = (n4*2);

  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _a = (const __m128d*)((const void*)a);
  const __m128d* _b = (const __m128d*)((const void*)b);
  const __m128d* _c = (const __m128d*)((const void*)c);
  const __m128d* _d = (const __m128d*)((const void*)d);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_add_pd(_mm_add_pd(_mm_mul_pd(_a[i],_B0),_mm_mul_pd(_b[i],_B1)),
                       _mm_add_pd(_mm_mul_pd(_c[i],_B2),_mm_mul_pd(_d[i],_B3)));
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));
  const XReal* _fc = (const XReal*)((const void*)(_c+n4));
  const XReal* _fd = (const XReal*)((const void*)(_d+n4));

  for (int i=nstart;i<n3;++i)
  {
    output[i].x = B0*_fa[i] + B1*_fb[i] + B2*_fc[i] + B3*_fd[i];
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayBezier(XVector3Packed* output,
                                 const XVector3Packed* a,
                                 const XVector3Packed* b,
                                 const XVector3Packed* c,
                                 const XVector3Packed* d,
                                 const XReal t,
                                 const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].bezier(a[i],b[i],c[i],d[i],t);
  }
}
#endif

#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayNegate(XVector3Packed* output,const XVector3Packed* input,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(input);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(input);

  int n3 = (num*3);
  int n4 = n3/4;
  int nstart = (n4*4);

  __m128* _o = (__m128*)((void*)output);

  const __m128* _i = (const __m128*)((const void*)input);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_sub_ps(_mm_setzero_ps(),_i[i]);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fi = (const XReal*)((const void*)(_i+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = -_fi[i];
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayNegate(XVector3Packed* output,const XVector3Packed* input,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(input);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(input);

  int n3 = (num*3);
  int n4 = n3/2;
  int nstart = (n4*2);

  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _i = (const __m128d*)((const void*)input);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_sub_pd(_mm_setzero_pd(),_i[i]);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fi = (const XReal*)((const void*)(_i+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = -_fi[i];
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayNegate(XVector3Packed* output,const XVector3Packed* input,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(input);

  int n3 = (num*3);
  int n4 = n3/4;
  int nstart = (n4*4);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    output[i].negate(input[i]);
  }

  for (int i=nstart;i<num;++i)
  {
    output[i].negate(input[i]);
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayNormalise(XVector3Packed* output,const XVector3Packed* input,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(input);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(input);

  int n3 = num;
  int n4 = n3/4;
  int nstart = (n4*4);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    // swizzle 4 vec3's into SOA, normalise, then swizzle back
    XVector3SOA temp;
    temp.streamIn(input+(4*i));
    temp.normalise();
    temp.streamOut(output+(4*i));
  }
  for (int i=nstart;i<num;++i)
  {
    output[i].normalise(input[i]);
  }
}

#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayAdd(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  int n3 = (num*3);
  int n4 = n3/4;
  int nstart = (n4*4);

  __m128* _o = (__m128*)((void*)output);

  const __m128* _a = (const __m128*)((const void*)a);
  const __m128* _b = (const __m128*)((const void*)b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_add_ps(_a[i],_b[i]);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] + _fb[i];
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayAdd(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  int n3 = (num*3);
  int n4 = n3/2;
  int nstart = (n4*2);

  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _a = (const __m128d*)((const void*)a);
  const __m128d* _b = (const __m128d*)((const void*)b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_add_pd(_a[i],_b[i]);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] + _fb[i];
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayAdd(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].add(a[i],b[i]);
  }
}
#endif

#if (XM2_SSE || XM2_SSE2)
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayAdd(XVector3Packed* output,const XVector3Packed* a,const XVector3* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  
  int n3 = num;
  int n4 = n3/4;
  int nstart = (n4*4);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    // swizzle 4 vec3's into SOA, normalise, then swizzle back
    XVector3SOA _a;
    XVector3SOA _b;
    XVector3SOA _c;
    _a.streamIn(a+(4*i));
    _a.streamIn(b+(4*i));
    _c.add(_a,_b);
    _c.streamOut(output+(4*i));
  }
  for (int i=nstart;i<num;++i)
  {
    output[i].add(a[i],b[i]);
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayAdd(XVector3Packed* output,const XVector3Packed* a,const XVector3* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].add(a[i],b[i]);
  }
}
#endif

#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayAdd(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  // arrange B.x into the following memory layout
  // 
  //  X Y Z X
  //  Y Z X Y
  //  Z X Y Z
  // 
  // we can now jump over 4 verts at a time, adding them together
  // 
  /// \todo replace with _mm_shuffle_ps
  const __m128 _b0 = _mm_set_ps(b.x,b.z,b.y,b.x);
  const __m128 _b1 = _mm_set_ps(b.y,b.x,b.z,b.y);
  const __m128 _b2 = _mm_set_ps(b.z,b.y,b.x,b.z);
  
  int n3 = (num*3);
  int n4 = n3/12;
  int nstart = (n4*12);

  __m128* _o = (__m128*)((void*)output);

  const __m128* _a = (const __m128*)((const void*)a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    const size_t index= i * 3;
    _o[index+0] = _mm_add_ps(_a[index+0],_b0);
    _o[index+1] = _mm_add_ps(_a[index+1],_b1);
    _o[index+2] = _mm_add_ps(_a[index+2],_b2);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4*3));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4*3));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] + b.data[ i%3 ];
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayAdd(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  // arrange B.x into the following memory layout
  // 
  //  X Y 
  //  Z X
  //  Y Z 
  // 
  // we can now jump over 3 verts at a time, adding them together
  // 
  /// \todo replace with _mm_shuffle_pd
  const __m128d _b0 = _mm_set_pd(b.y,b.x);
  const __m128d _b1 = _mm_set_pd(b.x,b.z);
  const __m128d _b2 = _mm_set_pd(b.z,b.y);
  
  int n3 = (num*3);
  int n4 = n3/6;
  int nstart = (n4*6);

  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _a = (const __m128d*)((const void*)a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    const size_t index= i * 3;
    _o[index+0] = _mm_add_pd(_a[index+0],_b0);
    _o[index+1] = _mm_add_pd(_a[index+1],_b1);
    _o[index+2] = _mm_add_pd(_a[index+2],_b2);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4*3));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4*3));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] + b.data[ i%3 ];
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayAdd(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].add(a[i],b);
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayAdd(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed& b,const int num)
{
  XVector3 temp(b);
  arrayAdd(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayAdd(XVector3Packed* output,const XVector3Packed* a,const XReal b,const int num)
{
  XVector3 temp(b,b,b);
  arrayAdd(output,a,temp,num);
}



#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arraySub(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  int n3 = (num*3);
  int n4 = n3/4;
  int nstart = (n4*4);

  __m128* _o = (__m128*)((void*)output);

  const __m128* _a = (const __m128*)((const void*)a);
  const __m128* _b = (const __m128*)((const void*)b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_sub_ps(_a[i],_b[i]);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] - _fb[i];
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arraySub(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  int n3 = (num*3);
  int n4 = n3/2;
  int nstart = (n4*2);

  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _a = (const __m128d*)((const void*)a);
  const __m128d* _b = (const __m128d*)((const void*)b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_sub_pd(_a[i],_b[i]);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] - _fb[i];
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arraySub(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].sub(a[i],b[i]);
  }
}
#endif

#if (XM2_SSE || XM2_SSE2)
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arraySub(XVector3Packed* output,const XVector3Packed* a,const XVector3* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  
  int n3 = num;
  int n4 = n3/4;
  int nstart = (n4*4);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    // swizzle 4 vec3's into SOA, normalise, then swizzle back
    XVector3SOA _a;
    XVector3SOA _b;
    XVector3SOA _c;
    _a.streamIn(a+(4*i));
    _a.streamIn(b+(4*i));
    _c.sub(_a,_b);
    _c.streamOut(output+(4*i));
  }
  for (int i=nstart;i<num;++i)
  {
    output[i].sub(a[i],b[i]);
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arraySub(XVector3Packed* output,const XVector3Packed* a,const XVector3* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].sub(a[i],b[i]);
  }
}
#endif

#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arraySub(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  // arrange B.x into the following memory layout
  // 
  //  X Y Z X
  //  Y Z X Y
  //  Z X Y Z
  // 
  // we can now jump over 4 verts at a time, adding them together
  // 
  /// \todo replace with _mm_shuffle_ps
  const __m128 _b0 = _mm_set_ps(b.x,b.z,b.y,b.x);
  const __m128 _b1 = _mm_set_ps(b.y,b.x,b.z,b.y);
  const __m128 _b2 = _mm_set_ps(b.z,b.y,b.x,b.z);
  
  int n3 = (num*3);
  int n4 = n3/12;
  int nstart = (n4*12);

  __m128* _o = (__m128*)((void*)output);

  const __m128* _a = (const __m128*)((const void*)a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    const size_t index= i * 3;
    _o[index+0] = _mm_sub_ps(_a[index+0],_b0);
    _o[index+1] = _mm_sub_ps(_a[index+1],_b1);
    _o[index+2] = _mm_sub_ps(_a[index+2],_b2);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4*3));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4*3));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] - b.data[ i%3 ];
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arraySub(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  // arrange B.x into the following memory layout
  // 
  //  X Y 
  //  Z X
  //  Y Z 
  // 
  // we can now jump over 3 verts at a time, adding them together
  // 
  /// \todo replace with _mm_shuffle_pd
  const __m128d _b0 = _mm_set_pd(b.y,b.x);
  const __m128d _b1 = _mm_set_pd(b.x,b.z);
  const __m128d _b2 = _mm_set_pd(b.z,b.y);
  
  int n3 = (num*3);
  int n4 = n3/6;
  int nstart = (n4*6);

  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _a = (const __m128d*)((const void*)a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    const size_t index= i * 3;
    _o[index+0] = _mm_sub_pd(_a[index+0],_b0);
    _o[index+1] = _mm_sub_pd(_a[index+1],_b1);
    _o[index+2] = _mm_sub_pd(_a[index+2],_b2);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4*3));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4*3));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] - b.data[ i%3 ];
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arraySub(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].sub(a[i],b);
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arraySub(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed& b,const int num)
{
  XVector3 temp(b);
  arraySub(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arraySub(XVector3Packed* output,const XVector3Packed* a,const XReal b,const int num)
{
  XVector3 temp(b,b,b);
  arraySub(output,a,temp,num);
}




#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayMul(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  int n3 = (num*3);
  int n4 = n3/4;
  int nstart = (n4*4);

  __m128* _o = (__m128*)((void*)output);

  const __m128* _a = (const __m128*)((const void*)a);
  const __m128* _b = (const __m128*)((const void*)b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_mul_ps(_a[i],_b[i]);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] * _fb[i];
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayMul(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  int n3 = (num*3);
  int n4 = n3/2;
  int nstart = (n4*2);

  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _a = (const __m128d*)((const void*)a);
  const __m128d* _b = (const __m128d*)((const void*)b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_mul_pd(_a[i],_b[i]);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] * _fb[i];
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayMul(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].mul(a[i],b[i]);
  }
}
#endif

#if (XM2_SSE || XM2_SSE2)
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayMul(XVector3Packed* output,const XVector3Packed* a,const XVector3* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  
  int n3 = num;
  int n4 = n3/4;
  int nstart = (n4*4);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    // swizzle 4 vec3's into SOA, normalise, then swizzle back
    XVector3SOA _a;
    XVector3SOA _b;
    XVector3SOA _c;
    _a.streamIn(a+(4*i));
    _a.streamIn(b+(4*i));
    _c.mul(_a,_b);
    _c.streamOut(output+(4*i));
  }
  for (int i=nstart;i<num;++i)
  {
    output[i].mul(a[i],b[i]);
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayMul(XVector3Packed* output,const XVector3Packed* a,const XVector3* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].mul(a[i],b[i]);
  }
}
#endif

#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayMul(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  // arrange B.x into the following memory layout
  // 
  //  X Y Z X
  //  Y Z X Y
  //  Z X Y Z
  // 
  // we can now jump over 4 verts at a time, adding them together
  // 
  /// \todo replace with _mm_shuffle_ps
  const __m128 _b0 = _mm_set_ps(b.x,b.z,b.y,b.x);
  const __m128 _b1 = _mm_set_ps(b.y,b.x,b.z,b.y);
  const __m128 _b2 = _mm_set_ps(b.z,b.y,b.x,b.z);
  
  int n3 = (num*3);
  int n4 = n3/12;
  int nstart = (n4*12);

  __m128* _o = (__m128*)((void*)output);

  const __m128* _a = (const __m128*)((const void*)a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    const size_t index= i * 3;
    _o[index+0] = _mm_mul_ps(_a[index+0],_b0);
    _o[index+1] = _mm_mul_ps(_a[index+1],_b1);
    _o[index+2] = _mm_mul_ps(_a[index+2],_b2);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4*3));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4*3));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] * b.data[ i%3 ];
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayMul(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  // arrange B.x into the following memory layout
  // 
  //  X Y 
  //  Z X
  //  Y Z 
  // 
  // we can now jump over 3 verts at a time, adding them together
  // 
  /// \todo replace with _mm_shuffle_pd
  const __m128d _b0 = _mm_set_pd(b.y,b.x);
  const __m128d _b1 = _mm_set_pd(b.x,b.z);
  const __m128d _b2 = _mm_set_pd(b.z,b.y);
  
  int n3 = (num*3);
  int n4 = n3/6;
  int nstart = (n4*6);

  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _a = (const __m128d*)((const void*)a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    const size_t index= i * 3;
    _o[index+0] = _mm_mul_pd(_a[index+0],_b0);
    _o[index+1] = _mm_mul_pd(_a[index+1],_b1);
    _o[index+2] = _mm_mul_pd(_a[index+2],_b2);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4*3));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4*3));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] * b.data[ i%3 ];
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayMul(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].mul(a[i],b);
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayMul(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed& b,const int num)
{
  XVector3 temp(b);
  arrayMul(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayMul(XVector3Packed* output,const XVector3Packed* a,const XReal b,const int num)
{
  XVector3 temp(b,b,b);
  arrayMul(output,a,temp,num);
}




#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDiv(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  int n3 = (num*3);
  int n4 = n3/4;
  int nstart = (n4*4);

  __m128* _o = (__m128*)((void*)output);

  const __m128* _a = (const __m128*)((const void*)a);
  const __m128* _b = (const __m128*)((const void*)b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_div_ps(_a[i],_b[i]);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] / _fb[i];
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDiv(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);

  int n3 = (num*3);
  int n4 = n3/2;
  int nstart = (n4*2);

  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _a = (const __m128d*)((const void*)a);
  const __m128d* _b = (const __m128d*)((const void*)b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    _o[i] = _mm_div_pd(_a[i],_b[i]);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4));
  const XReal* _fb = (const XReal*)((const void*)(_b+n4));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] / _fb[i];
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDiv(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].div(a[i],b[i]);
  }
}
#endif

#if (XM2_SSE || XM2_SSE2)
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDiv(XVector3Packed* output,const XVector3Packed* a,const XVector3* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  
  int n3 = num;
  int n4 = n3/4;
  int nstart = (n4*4);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    // swizzle 4 vec3's into SOA, normalise, then swizzle back
    XVector3SOA _a;
    XVector3SOA _b;
    XVector3SOA _c;
    _a.streamIn(a+(4*i));
    _a.streamIn(b+(4*i));
    _c.div(_a,_b);
    _c.streamOut(output+(4*i));
  }
  for (int i=nstart;i<num;++i)
  {
    output[i].div(a[i],b[i]);
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDiv(XVector3Packed* output,const XVector3Packed* a,const XVector3* b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ALIGN_ASSERT(b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].div(a[i],b[i]);
  }
}
#endif

#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDiv(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  // arrange B.x into the following memory layout
  // 
  //  X Y Z X
  //  Y Z X Y
  //  Z X Y Z
  // 
  // we can now jump over 4 verts at a time, adding them together
  // 
  /// \todo replace with _mm_shuffle_ps
  const __m128 _b0 = _mm_set_ps(b.x,b.z,b.y,b.x);
  const __m128 _b1 = _mm_set_ps(b.y,b.x,b.z,b.y);
  const __m128 _b2 = _mm_set_ps(b.z,b.y,b.x,b.z);
  
  int n3 = (num*3);
  int n4 = n3/12;
  int nstart = (n4*12);

  __m128* _o = (__m128*)((void*)output);

  const __m128* _a = (const __m128*)((const void*)a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    const size_t index= i * 3;
    _o[index+0] = _mm_div_ps(_a[index+0],_b0);
    _o[index+1] = _mm_div_ps(_a[index+1],_b1);
    _o[index+2] = _mm_div_ps(_a[index+2],_b2);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4*3));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4*3));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] / b.data[ i%3 ];
  }
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDiv(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  // arrange B.x into the following memory layout
  // 
  //  X Y 
  //  Z X
  //  Y Z 
  // 
  // we can now jump over 3 verts at a time, adding them together
  // 
  /// \todo replace with _mm_shuffle_pd
  const __m128d _b0 = _mm_set_pd(b.y,b.x);
  const __m128d _b1 = _mm_set_pd(b.x,b.z);
  const __m128d _b2 = _mm_set_pd(b.z,b.y);
  
  int n3 = (num*3);
  int n4 = n3/6;
  int nstart = (n4*6);

  __m128d* _o = (__m128d*)((void*)output);

  const __m128d* _a = (const __m128d*)((const void*)a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<n4;++i)
  {
    const size_t index= i * 3;
    _o[index+0] = _mm_div_pd(_a[index+0],_b0);
    _o[index+1] = _mm_div_pd(_a[index+1],_b1);
    _o[index+2] = _mm_div_pd(_a[index+2],_b2);
  }

  XReal* _fo = (XReal*)((void*)(_o+n4*3));
  const XReal* _fa = (const XReal*)((const void*)(_a+n4*3));

  for (int i=nstart;i<n3;++i)
  {
    _fo[i] = _fa[i] / b.data[ i%3 ];
  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDiv(XVector3Packed* output,const XVector3Packed* a,const XVector3& b,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].div(a[i],b);
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDiv(XVector3Packed* output,const XVector3Packed* a,const XVector3Packed& b,const int num)
{
  XVector3 temp(b);
  arrayDiv(output,a,temp,num);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3Packed::arrayDiv(XVector3Packed* output,const XVector3Packed* a,const XReal b,const int num)
{
  XVector3 temp(b,b,b);
  arrayDiv(output,a,temp,num);
}


#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XVector3PackedArray::fprint32(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3PackedArray::fprint64(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3PackedArray::fscan32(FILE* fp)
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
void XVector3PackedArray::fscan64(FILE* fp)
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
void XVector3PackedArray::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3PackedArray::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3PackedArray::fwrite32(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XVector3Packed),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3PackedArray::fwrite64(FILE* fp,bool flip_bytes) const
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
    ::fwrite(buffer(),sizeof(XVector3Packed),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite64(fp,flip_bytes);
  }
  #endif  
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3PackedArray::fread32(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector3Packed),n,fp);
  if(flip_bytes) 
    XM2swap32(buffer(),n*(sizeof(XVector3Packed)/sizeof(float)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XVector3PackedArray::fread64(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XVector3Packed),n,fp);
  if(flip_bytes) 
    XM2swap64(buffer(),n*(sizeof(XVector3Packed)/sizeof(double)));
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread64(fp,flip_bytes);
  }
  #endif  
}
#endif


}
