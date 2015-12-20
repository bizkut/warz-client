#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_VECTOR2SOA__INL__
#define XM2_SSE_VECTOR2SOA__INL__
#if (!XM2_USE_FLOAT)
# error SSE float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE XVector2SOA::XVector2SOA(const XVector2SOA& rhs) 
{ 
  memcpy(this,&rhs,sizeof(XVector2SOA)); 
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set0(const XReal x_,const XReal y_)
{
  x[0] = x_;
  y[0] = y_;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set1(const XReal x_,const XReal y_)
{
  x[1] = x_;
  y[1] = y_;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set2(const XReal x_,const XReal y_)
{
  x[2] = x_;
  y[2] = y_;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set3(const XReal x_,const XReal y_)
{
  x[3] = x_;
  y[3] = y_;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set0(const XVector2& vec)
{
  XM2_ALIGN_ASSERT(this);
  x[0] = vec.x;
  y[0] = vec.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set1(const XVector2& vec)
{
  XM2_ALIGN_ASSERT(this);
  x[1] = vec.x;
  y[1] = vec.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set2(const XVector2& vec)
{
  XM2_ALIGN_ASSERT(this);
  x[2] = vec.x;
  y[2] = vec.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::set3(const XVector2& vec)
{
  XM2_ALIGN_ASSERT(this);
  x[3] = vec.x;
  y[3] = vec.y;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get0(XVector2& vec) const
{
  XM2_ALIGN_ASSERT(this);
  _mm_store_ss(vec.data,_x);
  _mm_store_ss(vec.data+1,_y);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get1(XVector2& vec) const
{
  XM2_ALIGN_ASSERT(this);
  _mm_store_ss(vec.data,  _mm_shuffle_ps(_x,_x,_MM_SHUFFLE(1,1,1,1)));
  _mm_store_ss(vec.data+1,_mm_shuffle_ps(_y,_y,_MM_SHUFFLE(1,1,1,1)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get2(XVector2& vec) const
{
  XM2_ALIGN_ASSERT(this);
  _mm_store_ss(vec.data,  _mm_shuffle_ps(_x,_x,_MM_SHUFFLE(2,2,2,2)));
  _mm_store_ss(vec.data+1,_mm_shuffle_ps(_y,_y,_MM_SHUFFLE(2,2,2,2)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::get3(XVector2& vec) const
{
  XM2_ALIGN_ASSERT(this);
  _mm_store_ss(vec.data,  _mm_shuffle_ps(_x,_x,_MM_SHUFFLE(3,3,3,3)));
  _mm_store_ss(vec.data+1,_mm_shuffle_ps(_y,_y,_MM_SHUFFLE(3,3,3,3)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE bool XVector2SOA::equal(const XVector2SOA& b,const XReal eps) const
{
  __m128 _eps = _mm_set_ps1(eps);
  _eps = _mm_mul_ps(_eps,_eps);
  XVector2SOA tmp;
  tmp.sub(*this,b);
  tmp._x = _mm_mul_ps(tmp._x,tmp._x);
  tmp._y = _mm_mul_ps(tmp._y,tmp._y);
  if( 15 != _mm_movemask_ps(_mm_cmplt_ps(tmp._x,_eps)) ) return false;
  if( 15 != _mm_movemask_ps(_mm_cmplt_ps(tmp._y,_eps)) ) return false;
  return true;
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::streamIn(const XVector2* input)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  const __m128* temp = (const __m128*)((const void*)input);
  _x = _mm_shuffle_ps( temp[0], temp[1], _MM_SHUFFLE(2,0,2,0) );
  _y = _mm_shuffle_ps( temp[0], temp[1], _MM_SHUFFLE(3,1,3,1) );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::streamOut(XVector2* output) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(output);
  __m128* temp = (__m128*)((void*)output);
  temp[0] = _mm_shuffle_ps( _x, _y, _MM_SHUFFLE(2,0,2,0) );
  temp[1] = _mm_shuffle_ps( _x, _y, _MM_SHUFFLE(3,1,3,1) );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::length(XVector4& dp) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&dp);
  lengthSquared(dp);
  dp.sse = _mm_sqrt_ps(dp.sse);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::normalise(const XVector2SOA& q)
{
  XM2_ALIGN_ASSERT(this);
  XVector4 l;
  q.length(l);

  // test if any of the lengths are zero
  const __m128 test = _mm_and_ps(_mm_cmpgt_ps(l.sse,_mm_sub_ps(_mm_setzero_ps(),_mm_set_ps1(XM2_FLOAT_ZERO))),
                                 _mm_cmplt_ps(l.sse,_mm_set_ps1(XM2_FLOAT_ZERO)));

  // replace any zero's with 1.0f
  l.sse = _mm_or_ps(_mm_and_ps(test,_mm_set_ps1(1.0f)),
                    _mm_andnot_ps(test,l.sse));

  // now safe to divide without a division by zero happening.
  {
    _x = _mm_div_ps(_x,l.sse);
    _y = _mm_div_ps(_y,l.sse);
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::zero()
{
  XM2_ALIGN_ASSERT(this);
  _x = _mm_setzero_ps();
  _y = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2SOA& a,const XVector2SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps(a._x,b._x);
  _y = _mm_add_ps(a._y,b._y);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2SOA& a,const XVector2SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps(a._x,b._x);
  _y = _mm_sub_ps(a._y,b._y);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2SOA& a,const XVector2SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_mul_ps(a._x,b._x);
  _y = _mm_mul_ps(a._y,b._y);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2SOA& a,const XVector2SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(b.x[0])>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.x[1])>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.x[2])>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.x[3])>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.y[0])>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.y[1])>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.y[2])>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.y[3])>XM2_FLOAT_ZERO);
    _x = _mm_div_ps(a._x,b._x);
    _y = _mm_div_ps(a._y,b._y);
  #else
    // test if any of the lengths are zero
    __m128 test = _mm_and_ps(_mm_cmpgt_ps(b._x,_mm_sub_ps(_mm_setzero_ps(),_mm_set_ps1(XM2_FLOAT_ZERO))),
                             _mm_cmplt_ps(b._x,_mm_set_ps1(XM2_FLOAT_ZERO)));

    // replace any zero's with 1.0f
    __m128 _bx =  = _mm_rcp_ps(_mm_or_ps(_mm_and_ps(test,_mm_set_ps1(1.0f)),
                                         _mm_andnot_ps(test,b._x)));

    // test if any of the lengths are zero
    test = _mm_and_ps(_mm_cmpgt_ps(b._y,_mm_sub_ps(_mm_setzero_ps(),_mm_set_ps1(XM2_FLOAT_ZERO))),
                      _mm_cmplt_ps(b._y,_mm_set_ps1(XM2_FLOAT_ZERO)));

    // replace any zero's with 1.0f
    __m128 _by =  = _mm_rcp_ps(_mm_or_ps(_mm_and_ps(test,_mm_set_ps1(1.0f)),
                                         _mm_andnot_ps(test,b._y)));
    _x = _mm_div_ps(a._x,_bx);
    _y = _mm_div_ps(a._y,_by);
  #endif
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2SOA& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps(a._x,_mm_set_ps1(b.x));
  _y = _mm_add_ps(a._y,_mm_set_ps1(b.y));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2SOA& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps(a._x,_mm_set_ps1(b.x));
  _y = _mm_sub_ps(a._y,_mm_set_ps1(b.y));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2SOA& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(b.x)>XM2_FLOAT_ZERO);
    XM2_ASSERT(XMabs(b.y)>XM2_FLOAT_ZERO);
    _x = _mm_div_ps(a._x,_mm_set_ps1(b.x));
    _y = _mm_div_ps(a._y,_mm_set_ps1(b.y));
  #else
    __m128 bx = _mm_set_ps1( (XMabs(b.x)<XM2_FLOAT_ZERO) ? 0.0f : 1.0f/b.x );
    __m128 by = _mm_set_ps1( (XMabs(b.y)<XM2_FLOAT_ZERO) ? 0.0f : 1.0f/b.y );
    _x = _mm_mul_ps(a._x,bx);
    _y = _mm_mul_ps(a._y,by);
  #endif
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2SOA& a,const XVector2& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_mul_ps(a._x,_mm_set_ps1(b.x));
  _y = _mm_mul_ps(a._y,_mm_set_ps1(b.y));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::add(const XVector2SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128 _t = _mm_set_ps1(b);
  _x = _mm_add_ps(a._x,_t);
  _y = _mm_add_ps(a._y,_t);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::sub(const XVector2SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128 _t = _mm_set_ps1(b);
  _x = _mm_sub_ps(a._x,_t);
  _y = _mm_sub_ps(a._y,_t);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::div(const XVector2SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  #if XM2_DIVIDE_BY_ZERO_MODE
    XM2_ASSERT(XMabs(b)>XM2_FLOAT_ZERO);
    __m128 _t = _mm_set_ps1(1.0f/b);
  #else
    __m128 _t = _mm_set_ps1( (XMabs(b)<XM2_FLOAT_ZERO) ? 0.0f : 1.0f/b );
  #endif
  _x = _mm_mul_ps(a._x,_t);
  _y = _mm_mul_ps(a._y,_t);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::mul(const XVector2SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128 _t = _mm_set_ps1(b);
  _x = _mm_mul_ps(a._x,_t);
  _y = _mm_mul_ps(a._y,_t);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::dot(XVector4& dp,const XVector2SOA& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&dp);
  XM2_ALIGN_ASSERT(this);
  dp.sse = _mm_add_ps( _mm_mul_ps(_x,b._x),_mm_mul_ps(_y,b._y) );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::lerp(const XVector2SOA& a,const XVector2SOA& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128 _t = _mm_set_ps1(t);
  _x = _mm_add_ps(a._x,_mm_mul_ps(_t,_mm_sub_ps(b._x,a._x)));
  _y = _mm_add_ps(a._y,_mm_mul_ps(_t,_mm_sub_ps(b._y,a._y)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::bezier(const XVector2SOA& a,const XVector2SOA& b,const XVector2SOA& c,const XVector2SOA& d,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128 _t = _mm_set1_ps(t);
  const __m128 x_ab = _mm_add_ps(a._x,_mm_mul_ps(_t,_mm_sub_ps(b._x,a._x)));
  const __m128 y_ab = _mm_add_ps(a._y,_mm_mul_ps(_t,_mm_sub_ps(b._y,a._y)));
  const __m128 x_bc = _mm_add_ps(b._x,_mm_mul_ps(_t,_mm_sub_ps(c._x,b._x)));
  const __m128 y_bc = _mm_add_ps(b._y,_mm_mul_ps(_t,_mm_sub_ps(c._y,b._y)));
  const __m128 x_cd = _mm_add_ps(c._x,_mm_mul_ps(_t,_mm_sub_ps(d._x,c._x)));
  const __m128 y_cd = _mm_add_ps(c._y,_mm_mul_ps(_t,_mm_sub_ps(d._y,c._y)));
  const __m128 x_abc = _mm_add_ps(x_ab,_mm_mul_ps(_t,_mm_sub_ps(x_bc,x_ab)));
  const __m128 y_abc = _mm_add_ps(y_ab,_mm_mul_ps(_t,_mm_sub_ps(y_bc,y_ab)));
  const __m128 x_bcd = _mm_add_ps(x_bc,_mm_mul_ps(_t,_mm_sub_ps(x_cd,x_bc)));
  const __m128 y_bcd = _mm_add_ps(y_bc,_mm_mul_ps(_t,_mm_sub_ps(y_cd,y_bc)));
  _x = _mm_add_ps(x_abc,_mm_mul_ps(_t,_mm_sub_ps(x_bcd,x_abc)));
  _y = _mm_add_ps(y_abc,_mm_mul_ps(_t,_mm_sub_ps(y_bcd,y_abc)));
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector2SOA::negate(const XVector2SOA& v)
{
  XM2_ALIGN_ASSERT(&v);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps(_mm_setzero_ps(),v._x);
  _y = _mm_sub_ps(_mm_setzero_ps(),v._y);
}
}
#endif
