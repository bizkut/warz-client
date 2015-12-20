#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_BOUNDING_BOX__INL__
#define XM2_SSE_BOUNDING_BOX__INL__
#if (!XM2_USE_FLOAT)
# error SSE2 float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE __m128 XBoundingBox::widthSSE() const
{
  XM2_ALIGN_ASSERT(this);
  const __m128 w = _mm_sub_ps(maximum.sse,minimum.sse);
  return _mm_shuffle_ps(w,w,_MM_SHUFFLE(0,0,0,0));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE __m128 XBoundingBox::heightSSE() const
{
  XM2_ALIGN_ASSERT(this);
  const __m128 w = _mm_sub_ps(maximum.sse,minimum.sse);
  return _mm_shuffle_ps(w,w,_MM_SHUFFLE(1,1,1,1));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE __m128 XBoundingBox::depthSSE() const
{
  XM2_ALIGN_ASSERT(this);
  const __m128 w = _mm_sub_ps(maximum.sse,minimum.sse);
  return _mm_shuffle_ps(w,w,_MM_SHUFFLE(2,2,2,2));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector3& p)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&p);
  minimum.sse = _mm_min_ps(minimum.sse,p.sse);
  maximum.sse = _mm_max_ps(maximum.sse,p.sse);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector4& p)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&p);
  minimum.sse = _mm_min_ps(minimum.sse,p.sse);
  maximum.sse = _mm_max_ps(maximum.sse,p.sse);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XVector3Packed& p)
{
  XM2_ALIGN_ASSERT(this);
  const __m128 temp = _mm_set_ps(0,p.z,p.y,p.x);
  minimum.sse = _mm_min_ps(minimum.sse,temp);
  maximum.sse = _mm_max_ps(maximum.sse,temp);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XBoundingBox::expand(const XBoundingBox& box)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&box);
  minimum.sse = _mm_min_ps(minimum.sse,box.minimum.sse);
  maximum.sse = _mm_max_ps(maximum.sse,box.maximum.sse);
}
//----------------------------------------------------------------------------------------------------------------------  
bool XBoundingBox::contains(const XVector3Packed& point) const
{
  return contains(XVector3(point));
}
//----------------------------------------------------------------------------------------------------------------------  
bool XBoundingBox::contains(const XVector3& point) const
{
  XM2_ALIGN_ASSERT(&point);
  XM2_ALIGN_ASSERT(this);
  const __m128 cmp_lt_max = _mm_cmple_ps(point.sse,maximum.sse);
  const __m128 cmp_gt_min = _mm_cmpge_ps(point.sse,minimum.sse);
  const __m128 cmp_and = _mm_and_ps( cmp_gt_min, cmp_lt_max );
  const int test = _mm_movemask_ps(cmp_and);
  return (test==7 || test==15);
}
//----------------------------------------------------------------------------------------------------------------------  
bool XBoundingBox::contains(const XVector4& point) const
{
  XM2_ALIGN_ASSERT(&point);
  XM2_ALIGN_ASSERT(this);
  const __m128 cmp_lt_max = _mm_cmple_ps(point.sse,maximum.sse);
  const __m128 cmp_gt_min = _mm_cmpge_ps(point.sse,minimum.sse);
  const __m128 cmp_and = _mm_and_ps( cmp_gt_min, cmp_lt_max );
  const int test = _mm_movemask_ps(cmp_and);
  return (test==7 || test==15);
}
}
#endif
