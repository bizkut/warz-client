#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_SPHERE__INL__
#define XM2_SSE_SPHERE__INL__
#if (!XM2_USE_FLOAT)
# error SSE2 float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
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
  radius = _mm_max_ps(radius,
             _mm_max_ps(
                _mm_max_ps( _mm_shuffle_ps(dist.sse,dist.sse,_MM_SHUFFLE(0,0,0,0)),
                            _mm_shuffle_ps(dist.sse,dist.sse,_MM_SHUFFLE(1,1,1,1)) ),
                _mm_max_ps( _mm_shuffle_ps(dist.sse,dist.sse,_MM_SHUFFLE(2,2,2,2)),
                            _mm_shuffle_ps(dist.sse,dist.sse,_MM_SHUFFLE(3,3,3,3)) )
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
  diff._w = _mm_setzero_ps();
  XVector4 dist;
  diff.lengthSquared(dist);
  radius = _mm_max_ps(radius,
             _mm_max_ps(
                _mm_max_ps( _mm_shuffle_ps(dist.sse,dist.sse,_MM_SHUFFLE(0,0,0,0)),
                            _mm_shuffle_ps(dist.sse,dist.sse,_MM_SHUFFLE(1,1,1,1)) ),
                _mm_max_ps( _mm_shuffle_ps(dist.sse,dist.sse,_MM_SHUFFLE(2,2,2,2)),
                            _mm_shuffle_ps(dist.sse,dist.sse,_MM_SHUFFLE(3,3,3,3)) )
             )
           );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XSphere::setRadiusSquared(const XReal _radius)
{
  XM2_ALIGN_ASSERT(this);
  radius = _mm_set_ps1(_radius);
}
}
#endif
