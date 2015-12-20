#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_BOUNDING_BOX__INL__
#define XM2_SSE2_BOUNDING_BOX__INL__
#if XM2_USE_FLOAT
# error SSE2 double precision routines included when using float
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE __m128d XBoundingBox::widthSSE() const
{
  XM2_ALIGN_ASSERT(this);
  const __m128d w = _mm_sub_pd(maximum.xy,minimum.xy);
  return _mm_shuffle_pd(w,w,_MM_SHUFFLE2(0,0));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE __m128d XBoundingBox::heightSSE() const
{
  XM2_ALIGN_ASSERT(this);
  const __m128d w = _mm_sub_pd(maximum.xy,minimum.xy);
  return _mm_shuffle_pd(w,w,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE __m128d XBoundingBox::depthSSE() const
{
  XM2_ALIGN_ASSERT(this);
  const __m128d w = _mm_sub_pd(maximum.zw,minimum.zw);
  return _mm_shuffle_pd(w,w,_MM_SHUFFLE2(0,0));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector3& p)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&p);
  minimum.xy = _mm_min_pd(minimum.xy,p.xy);
  minimum.zw = _mm_min_pd(minimum.zw,p.zw);
  maximum.xy = _mm_max_pd(maximum.xy,p.xy);
  maximum.zw = _mm_max_pd(maximum.zw,p.zw);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector4& p)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&p);
  minimum.xy = _mm_min_pd(minimum.xy,p.xy);
  minimum.zw = _mm_min_pd(minimum.zw,p.zw);
  maximum.xy = _mm_max_pd(maximum.xy,p.xy);
  maximum.zw = _mm_max_pd(maximum.zw,p.zw);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector3Packed& p)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&p);
  const __m128d __x = _mm_set_pd(p.y,p.x);
  const __m128d __y = _mm_set_pd(0.0f,p.z);
  minimum.xy = _mm_min_pd(minimum.xy,__x);
  minimum.zw = _mm_min_pd(minimum.zw,__y);
  maximum.xy = _mm_max_pd(maximum.xy,__x);
  maximum.zw = _mm_max_pd(maximum.zw,__y);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XBoundingBox& box)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&box);
  minimum.xy = _mm_min_pd(minimum.xy,box.minimum.xy);
  minimum.zw = _mm_min_pd(minimum.zw,box.minimum.zw);
  maximum.xy = _mm_max_pd(maximum.xy,box.maximum.xy);
  maximum.zw = _mm_max_pd(maximum.zw,box.maximum.zw);
}
//----------------------------------------------------------------------------------------------------------------------  
bool XBoundingBox::contains(const XVector3& point) const
{
  XM2_ALIGN_ASSERT(&point);
  XM2_ALIGN_ASSERT(this);
  const __m128d cmp_lt_max_xy = _mm_cmple_pd(point.xy,maximum.xy);
  const __m128d cmp_gt_min_xy = _mm_cmpgt_pd(point.xy,minimum.xy);
  const __m128d cmp_lt_max_zw = _mm_cmple_pd(point.zw,maximum.zw);
  const __m128d cmp_gt_min_zw = _mm_cmpgt_pd(point.zw,minimum.zw);
  const __m128d cmp_and_xy = _mm_and_pd( cmp_lt_max_xy, cmp_gt_min_xy );
  const __m128d cmp_and_zw = _mm_and_pd( cmp_lt_max_zw, cmp_gt_min_zw );
  return (_mm_movemask_pd(cmp_and_xy) == 3) &&
         (_mm_movemask_pd(cmp_and_zw) == 1);
}
//----------------------------------------------------------------------------------------------------------------------  
bool XBoundingBox::contains(const XVector4& point) const
{
  XM2_ALIGN_ASSERT(&point);
  XM2_ALIGN_ASSERT(this);
  const __m128d cmp_lt_max_xy = _mm_cmple_pd(point.xy,maximum.xy);
  const __m128d cmp_gt_min_xy = _mm_cmpgt_pd(point.xy,minimum.xy);
  const __m128d cmp_lt_max_zw = _mm_cmple_pd(point.zw,maximum.zw);
  const __m128d cmp_gt_min_zw = _mm_cmpgt_pd(point.zw,minimum.zw);
  const __m128d cmp_and_xy = _mm_and_pd( cmp_lt_max_xy, cmp_gt_min_xy );
  const __m128d cmp_and_zw = _mm_and_pd( cmp_lt_max_zw, cmp_gt_min_zw );
  return (_mm_movemask_pd(cmp_and_xy) == 3) &&
         (_mm_movemask_pd(cmp_and_zw) == 1);
}
//----------------------------------------------------------------------------------------------------------------------  
bool XBoundingBox::contains(const XVector3Packed& _point) const
{
  XM2_ALIGN_ASSERT(&point);
  XM2_ALIGN_ASSERT(this);
  XVector3 point(_point);
  const __m128d cmp_lt_max_xy = _mm_cmple_pd(point.xy,maximum.xy);
  const __m128d cmp_gt_min_xy = _mm_cmpgt_pd(point.xy,minimum.xy);
  const __m128d cmp_lt_max_zw = _mm_cmple_pd(point.zw,maximum.zw);
  const __m128d cmp_gt_min_zw = _mm_cmpgt_pd(point.zw,minimum.zw);
  const __m128d cmp_and_xy = _mm_and_pd( cmp_lt_max_xy, cmp_gt_min_xy );
  const __m128d cmp_and_zw = _mm_and_pd( cmp_lt_max_zw, cmp_gt_min_zw );
  return (_mm_movemask_pd(cmp_and_xy) == 3) &&
         (_mm_movemask_pd(cmp_and_zw) == 1);
}
}
#endif
