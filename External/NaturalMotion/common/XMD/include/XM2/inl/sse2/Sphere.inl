#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_SPHERE__INL__
#define XM2_SSE2_SPHERE__INL__
#if XM2_USE_FLOAT
# error SSE2 double precision routines included when using float
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::expand(const XVector3SOA& p)
{
  XM2_ALIGN_ASSERT(&p);
  XM2_ALIGN_ASSERT(this);
  XVector3SOA diff(center);
  diff.sub(p,diff);
  XVector4 dist;
  diff.lengthSquared(dist);
  radius = _mm_max_pd(radius,
             _mm_max_pd(
                _mm_max_pd( _mm_shuffle_pd(dist.xy,dist.xy,_MM_SHUFFLE2(0,0)),
                            _mm_shuffle_pd(dist.xy,dist.xy,_MM_SHUFFLE2(1,1)) ),
                _mm_max_pd( _mm_shuffle_pd(dist.zw,dist.zw,_MM_SHUFFLE2(0,0)),
                            _mm_shuffle_pd(dist.zw,dist.zw,_MM_SHUFFLE2(1,1)) )
             )
           );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::expand(const XVector4SOA& p)
{
  XM2_ALIGN_ASSERT(&p);
  XM2_ALIGN_ASSERT(this);
  XVector4SOA diff(center);
  diff.sub(p,diff);
  diff._w0 = diff._w1 = _mm_setzero_pd();
  XVector4 dist;
  diff.lengthSquared(dist);
  radius = _mm_max_pd(radius,
             _mm_max_pd(
                _mm_max_pd( _mm_shuffle_pd(dist.xy,dist.xy,_MM_SHUFFLE2(0,0)),
                            _mm_shuffle_pd(dist.xy,dist.xy,_MM_SHUFFLE2(1,1)) ),
                _mm_max_pd( _mm_shuffle_pd(dist.zw,dist.zw,_MM_SHUFFLE2(0,0)),
                            _mm_shuffle_pd(dist.zw,dist.zw,_MM_SHUFFLE2(1,1)) )
             )
           );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::setRadiusSquared(const XReal _radius)
{
  XM2_ALIGN_ASSERT(this);
  radius = _mm_set1_pd(_radius);
}
}
#endif
