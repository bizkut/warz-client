#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_VECTOR2SOA__INL__
#define XM2_SSE2_VECTOR2SOA__INL__
#if (XM2_USE_FLOAT)
# error SSE2 double precision routines included when set to build using floats
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XVector2SOA::XVector2SOA(const XVector2SOA& rhs) 
{ 
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&rhs);
  _x0 = rhs._x0;
  _x1 = rhs._x1;
  _y0 = rhs._y0;
  _y1 = rhs._y1;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set0(const XReal x_,const XReal y_)
{
  XM2_ALIGN_ASSERT(this);
  const __m128d tmp = _mm_set_pd(y_,x_);
  _x0 = _mm_shuffle_pd(tmp,_x0,_MM_SHUFFLE2(1,0));
  _y0 = _mm_shuffle_pd(tmp,_y0,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set1(const XReal x_,const XReal y_)
{
  XM2_ALIGN_ASSERT(this);
  const __m128d tmp = _mm_set_pd(y_,x_);
  _x0 = _mm_shuffle_pd(_x0,tmp,_MM_SHUFFLE2(0,0));
  _y0 = _mm_shuffle_pd(_y0,tmp,_MM_SHUFFLE2(1,0));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set2(const XReal x_,const XReal y_)
{
  XM2_ALIGN_ASSERT(this);
  const __m128d tmp = _mm_set_pd(y_,x_);
  _x1 = _mm_shuffle_pd(tmp,_x1,_MM_SHUFFLE2(1,0));
  _y1 = _mm_shuffle_pd(tmp,_y1,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set3(const XReal x_,const XReal y_)
{
  XM2_ALIGN_ASSERT(this);
  const __m128d tmp = _mm_set_pd(y_,x_);
  _x1 = _mm_shuffle_pd(_x1,tmp,_MM_SHUFFLE2(0,0));
  _y1 = _mm_shuffle_pd(_y1,tmp,_MM_SHUFFLE2(1,0));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set0(const XVector2& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x0 = _mm_shuffle_pd(vec.sse,_x0,_MM_SHUFFLE2(1,0));
  _y0 = _mm_shuffle_pd(vec.sse,_y0,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set1(const XVector2& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x0 = _mm_shuffle_pd(_x0,vec.sse,_MM_SHUFFLE2(0,0));
  _y0 = _mm_shuffle_pd(_y0,vec.sse,_MM_SHUFFLE2(1,0));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set2(const XVector2& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x1 = _mm_shuffle_pd(vec.sse,_x1,_MM_SHUFFLE2(1,0));
  _y1 = _mm_shuffle_pd(vec.sse,_y1,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set3(const XVector2& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x1 = _mm_shuffle_pd(_x1,vec.sse,_MM_SHUFFLE2(0,0));
  _y1 = _mm_shuffle_pd(_y1,vec.sse,_MM_SHUFFLE2(1,0));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get0(XVector2& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _mm_store_pd(vec.data,_mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(0,0)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get1(XVector2& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _mm_store_pd(vec.data,_mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(1,1)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get2(XVector2& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _mm_store_pd(vec.data,_mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(0,0)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get3(XVector2& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _mm_store_pd(vec.data,_mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(1,1)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XVector2SOA::equal(const XVector2SOA& b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128d _eps = _mm_set1_pd(eps);
  _eps = _mm_mul_pd(_eps,_eps);
  XVector2SOA tmp;
  tmp.sub(*this,b);
  tmp._x0 = _mm_mul_pd(tmp._x0,tmp._x0);
  tmp._x1 = _mm_mul_pd(tmp._x1,tmp._x1);
  tmp._y0 = _mm_mul_pd(tmp._y0,tmp._y0);
  tmp._y1 = _mm_mul_pd(tmp._y1,tmp._y1);
  if( 3 != _mm_movemask_pd(_mm_cmplt_pd(tmp._x0,_eps)) ) return false;
  if( 3 != _mm_movemask_pd(_mm_cmplt_pd(tmp._x1,_eps)) ) return false;
  if( 3 != _mm_movemask_pd(_mm_cmplt_pd(tmp._y0,_eps)) ) return false;
  if( 3 != _mm_movemask_pd(_mm_cmplt_pd(tmp._y1,_eps)) ) return false;
  return true;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::length(XVector4& dp) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&dp);
  lengthSquared(dp);
  dp.xy = _mm_sqrt_pd(dp.xy);
  dp.zw = _mm_sqrt_pd(dp.zw);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::normalise(const XVector2SOA& q)
{
  XM2_ALIGN_ASSERT(this);
  XVector4 l;
  q.length(l);
  
  // test if any of the lengths are zero
  __m128d test = _mm_and_pd(_mm_cmpgt_pd(l.xy,_mm_sub_pd(_mm_setzero_pd(),_mm_set1_pd(XM2_FLOAT_ZERO))),
                            _mm_cmplt_pd(l.xy,_mm_set1_pd(XM2_FLOAT_ZERO)));

  // replace any zero's with 1.0f
  l.xy = _mm_or_pd(_mm_and_pd(test,_mm_set1_pd(1.0f)),
                   _mm_andnot_pd(test,l.xy));

  _x0 = _mm_div_pd(q._x0,l.xy);
  _y0 = _mm_div_pd(q._y0,l.xy);
  
  test = _mm_and_pd(_mm_cmpgt_pd(l.zw,_mm_sub_pd(_mm_setzero_pd(),_mm_set1_pd(XM2_FLOAT_ZERO))),
                    _mm_cmplt_pd(l.zw,_mm_set1_pd(XM2_FLOAT_ZERO)));

  // replace any zero's with 1.0f
  l.zw = _mm_or_pd(_mm_and_pd(test,_mm_set1_pd(1.0f)),
                   _mm_andnot_pd(test,l.zw));

  _x1 = _mm_div_pd(q._x1,l.zw);
  _y1 = _mm_div_pd(q._y1,l.zw);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::zero()
{
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_setzero_pd();
  _x1 = _mm_setzero_pd();
  _y0 = _mm_setzero_pd();
  _y1 = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2SOA& a,const XVector2SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_add_pd(a._x0,b._x0);
  _x1 = _mm_add_pd(a._x1,b._x1);
  _y0 = _mm_add_pd(a._y0,b._y0);
  _y1 = _mm_add_pd(a._y1,b._y1);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2SOA& a,const XVector2SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_sub_pd(a._x0,b._x0);
  _x1 = _mm_sub_pd(a._x1,b._x1);
  _y0 = _mm_sub_pd(a._y0,b._y0);
  _y1 = _mm_sub_pd(a._y1,b._y1);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2SOA& a,const XVector2SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_mul_pd(a._x0,b._x0);
  _x1 = _mm_mul_pd(a._x1,b._x1);
  _y0 = _mm_mul_pd(a._y0,b._y0);
  _y1 = _mm_mul_pd(a._y1,b._y1);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2SOA& a,const XVector2SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_div_pd(a._x0,b._x0);
  _x1 = _mm_div_pd(a._x1,b._x1);
  _y0 = _mm_div_pd(a._y0,b._y0);
  _y1 = _mm_div_pd(a._y1,b._y1);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2SOA& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128d _x = _mm_set1_pd(b.x);
  __m128d _y = _mm_set1_pd(b.y);
  _x0 = _mm_add_pd(a._x0,_x);
  _x1 = _mm_add_pd(a._x1,_x);
  _y0 = _mm_add_pd(a._y0,_y);
  _y1 = _mm_add_pd(a._y1,_y);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2SOA& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128d _x = _mm_set1_pd(b.x);
  __m128d _y = _mm_set1_pd(b.y);
  _x0 = _mm_sub_pd(a._x0,_x);
  _x1 = _mm_sub_pd(a._x1,_x);
  _y0 = _mm_sub_pd(a._y0,_y);
  _y1 = _mm_sub_pd(a._y1,_y);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2SOA& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128d _x = _mm_set1_pd(b.x);
  __m128d _y = _mm_set1_pd(b.y);
  _x0 = _mm_div_pd(a._x0,_x);
  _x1 = _mm_div_pd(a._x1,_x);
  _y0 = _mm_div_pd(a._y0,_y);
  _y1 = _mm_div_pd(a._y1,_y);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2SOA& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128d _x = _mm_set1_pd(b.x);
  __m128d _y = _mm_set1_pd(b.y);
  _x0 = _mm_mul_pd(a._x0,_x);
  _x1 = _mm_mul_pd(a._x1,_x);
  _y0 = _mm_mul_pd(a._y0,_y);
  _y1 = _mm_mul_pd(a._y1,_y);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128d _t = _mm_set1_pd(b);
  _x0 = _mm_add_pd(a._x0,_t);
  _x1 = _mm_add_pd(a._x1,_t);
  _y0 = _mm_add_pd(a._y0,_t);
  _y1 = _mm_add_pd(a._y1,_t);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128d _t = _mm_set1_pd(b);
  _x0 = _mm_sub_pd(a._x0,_t);
  _x1 = _mm_sub_pd(a._x1,_t);
  _y0 = _mm_sub_pd(a._y0,_t);
  _y1 = _mm_sub_pd(a._y1,_t);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128d _t = _mm_set1_pd(b);
  _x0 = _mm_div_pd(a._x0,_t);
  _x1 = _mm_div_pd(a._x1,_t);
  _y0 = _mm_div_pd(a._y0,_t);
  _y1 = _mm_div_pd(a._y1,_t);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128d _t = _mm_set1_pd(b);
  _x0 = _mm_mul_pd(a._x0,_t);
  _x1 = _mm_mul_pd(a._x1,_t);
  _y0 = _mm_mul_pd(a._y0,_t);
  _y1 = _mm_mul_pd(a._y1,_t);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::dot(XVector4& dp,const XVector2SOA& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&dp);
  XM2_ALIGN_ASSERT(this);
  dp.xy = _mm_add_pd( _mm_mul_pd(_x0,b._x0),_mm_mul_pd(_y0,b._y0) );
  dp.zw = _mm_add_pd( _mm_mul_pd(_x1,b._x1),_mm_mul_pd(_y1,b._y1) );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::lerp(const XVector2SOA& a,const XVector2SOA& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128d _t = _mm_set1_pd(t);
  _x0 = _mm_add_pd(a._x0,_mm_mul_pd(_t,_mm_sub_pd(b._x0,a._x0)));
  _x1 = _mm_add_pd(a._x1,_mm_mul_pd(_t,_mm_sub_pd(b._x1,a._x1)));
  _y0 = _mm_add_pd(a._y0,_mm_mul_pd(_t,_mm_sub_pd(b._y0,a._y0)));
  _y1 = _mm_add_pd(a._y1,_mm_mul_pd(_t,_mm_sub_pd(b._y1,a._y1)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::bezier(const XVector2SOA& a,const XVector2SOA& b,const XVector2SOA& c,const XVector2SOA& d,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128d _t = _mm_set1_pd(t);

  const __m128d x_ab0 = _mm_add_pd(a._x0,_mm_mul_pd(_t,_mm_sub_pd(b._x0,a._x0)));
  const __m128d x_ab1 = _mm_add_pd(a._x1,_mm_mul_pd(_t,_mm_sub_pd(b._x1,a._x1)));
  const __m128d y_ab0 = _mm_add_pd(a._y0,_mm_mul_pd(_t,_mm_sub_pd(b._y0,a._y0)));
  const __m128d y_ab1 = _mm_add_pd(a._y1,_mm_mul_pd(_t,_mm_sub_pd(b._y1,a._y1)));
  const __m128d x_bc0 = _mm_add_pd(b._x0,_mm_mul_pd(_t,_mm_sub_pd(c._x0,b._x0)));
  const __m128d x_bc1 = _mm_add_pd(b._x1,_mm_mul_pd(_t,_mm_sub_pd(c._x1,b._x1)));
  const __m128d y_bc0 = _mm_add_pd(b._y0,_mm_mul_pd(_t,_mm_sub_pd(c._y0,b._y0)));
  const __m128d y_bc1 = _mm_add_pd(b._y1,_mm_mul_pd(_t,_mm_sub_pd(c._y1,b._y1)));
  const __m128d x_cd0 = _mm_add_pd(c._x0,_mm_mul_pd(_t,_mm_sub_pd(d._x0,c._x0)));
  const __m128d x_cd1 = _mm_add_pd(c._x1,_mm_mul_pd(_t,_mm_sub_pd(d._x1,c._x1)));
  const __m128d y_cd0 = _mm_add_pd(c._y0,_mm_mul_pd(_t,_mm_sub_pd(d._y0,c._y0)));
  const __m128d y_cd1 = _mm_add_pd(c._y1,_mm_mul_pd(_t,_mm_sub_pd(d._y1,c._y1)));

  const __m128d x_abc0 = _mm_add_pd(x_ab0,_mm_mul_pd(_t,_mm_sub_pd(x_bc0,x_ab0)));
  const __m128d x_abc1 = _mm_add_pd(x_ab1,_mm_mul_pd(_t,_mm_sub_pd(x_bc1,x_ab1)));
  const __m128d y_abc0 = _mm_add_pd(y_ab0,_mm_mul_pd(_t,_mm_sub_pd(y_bc0,y_ab0)));
  const __m128d y_abc1 = _mm_add_pd(y_ab1,_mm_mul_pd(_t,_mm_sub_pd(y_bc1,y_ab1)));
  const __m128d x_bcd0 = _mm_add_pd(x_bc0,_mm_mul_pd(_t,_mm_sub_pd(x_cd0,x_bc0)));
  const __m128d x_bcd1 = _mm_add_pd(x_bc1,_mm_mul_pd(_t,_mm_sub_pd(x_cd1,x_bc1)));
  const __m128d y_bcd0 = _mm_add_pd(y_bc0,_mm_mul_pd(_t,_mm_sub_pd(y_cd0,y_bc0)));
  const __m128d y_bcd1 = _mm_add_pd(y_bc1,_mm_mul_pd(_t,_mm_sub_pd(y_cd1,y_bc1)));

  _x0 = _mm_add_pd(x_abc0,_mm_mul_pd(_t,_mm_sub_pd(x_bcd0,x_abc0)));
  _x1 = _mm_add_pd(x_abc1,_mm_mul_pd(_t,_mm_sub_pd(x_bcd1,x_abc1)));
  _y0 = _mm_add_pd(y_abc0,_mm_mul_pd(_t,_mm_sub_pd(y_bcd0,y_abc0)));
  _y1 = _mm_add_pd(y_abc1,_mm_mul_pd(_t,_mm_sub_pd(y_bcd1,y_abc1)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::negate(const XVector2SOA& v)
{
  XM2_ALIGN_ASSERT(&v);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_sub_pd(_mm_setzero_pd(),v._x0);
  _x1 = _mm_sub_pd(_mm_setzero_pd(),v._x1);
  _y0 = _mm_sub_pd(_mm_setzero_pd(),v._y0);
  _y1 = _mm_sub_pd(_mm_setzero_pd(),v._y1);
}
}
#endif
