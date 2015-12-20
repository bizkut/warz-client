#ifdef _MSC_VER
# pragma once
#endif

#ifndef XM2_SSE_TRIGONOMETRY__INL__
#define XM2_SSE_TRIGONOMETRY__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XMcos(const __m128 d)
{ 
  static const __m128 _t0 = XM2_SSE_INIT1(-1.0f/2.0f);
  static const __m128 _t1 = XM2_SSE_INIT1(1.0f/24.0f);
  static const __m128 _t2 = XM2_SSE_INIT1(-1.0f/720.0f);
  static const __m128 _t3 = XM2_SSE_INIT1(1.0f/40320.0f);
  static const __m128 _t4 = XM2_SSE_INIT1(-1.0f/3628800.0f);
  const __m128 d2  = _mm_mul_ps(d,d);
  const __m128 d4  = _mm_mul_ps(d2,d2);
  const __m128 d6  = _mm_mul_ps(d4,d2);
  const __m128 d8  = _mm_mul_ps(d6,d2);
  const __m128 d10 = _mm_mul_ps(d8,d2);
  const __m128 t2  = _mm_mul_ps(d2, _t0);
  const __m128 t3  = _mm_mul_ps(d4, _t1);
  const __m128 t4  = _mm_mul_ps(d6, _t2);
  const __m128 t5  = _mm_mul_ps(d8, _t3);
  const __m128 t6  = _mm_mul_ps(d10,_t4);
  return _mm_add_ps(_mm_set_ps1(1.0f),
            _mm_add_ps(t2,
               _mm_add_ps(t3,
                 _mm_add_ps(t4,
                    _mm_add_ps(t5,t6)))));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XMsin(const __m128 d) 
{
  static const __m128 _t0 = XM2_SSE_INIT1(-1.0f/6.0f);
  static const __m128 _t1 = XM2_SSE_INIT1(+1.0f/120.0f);
  static const __m128 _t2 = XM2_SSE_INIT1(-1.0f/5040.0f);
  static const __m128 _t3 = XM2_SSE_INIT1(+1.0f/362880.0f);
  static const __m128 _t4 = XM2_SSE_INIT1(-1.0f/39916800.0f);
  static const __m128 _t5 = XM2_SSE_INIT1(+1.0f/6227020800.0f);
  const __m128 d2 = _mm_mul_ps(d,d);
  const __m128 d3 = _mm_mul_ps(d,d2);
  const __m128 d4 = _mm_mul_ps(d3,d2);
  const __m128 d5 = _mm_mul_ps(d4,d2);
  const __m128 d6 = _mm_mul_ps(d5,d2);
  const __m128 d7 = _mm_mul_ps(d6,d2);
  const __m128 d8 = _mm_mul_ps(d7,d2);
  const __m128 t1 = _mm_mul_ps(d3,_t0);
  const __m128 t2 = _mm_mul_ps(d4,_t1);
  const __m128 t3 = _mm_mul_ps(d5,_t2);
  const __m128 t4 = _mm_mul_ps(d6,_t3);
  const __m128 t5 = _mm_mul_ps(d7,_t4);
  const __m128 t6 = _mm_mul_ps(d8,_t5);
  return _mm_add_ps(d,
            _mm_add_ps(t1,
               _mm_add_ps(t2,
                 _mm_add_ps(t3,
                    _mm_add_ps(t4,
                      _mm_add_ps(t5,t6))))));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XMtan(__m128 x)
{
  // arcos(X) = PI/2 - arcsin(X)
  return _mm_div_ps( XMsin(x), XMcos(x) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XMacos(__m128 x)
{
  // arcos(X) = PI/2 - arcsin(X)
  return _mm_sub_ps(_mm_set_ps1(1.5707963267948966192313216916398f),XMasin(x));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XMasin(__m128 x)
{
  // going to use the approximation detailed at the tail end of this page:
  //  http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/arcsin/fastsqrt.html

  // check to see if values are less than zero
  __m128 test = _mm_cmplt_ps(x,_mm_setzero_ps());

  // flip sign if negative
  x = _mm_or_ps(_mm_and_ps(test,_mm_sub_ps(_mm_setzero_ps(),x)),
                _mm_andnot_ps(test,x));

  // init co-effs 
  static const __m128 top0 = XM2_SSE_INIT1(1.507095111f);
  static const __m128 top1 = XM2_SSE_INIT1(1.103007131f);
  static const __m128 top2 = XM2_SSE_INIT1(-2.144008022f);
  static const __m128 btm0 = XM2_SSE_INIT1(1.507171600f);
  static const __m128 btm1 = XM2_SSE_INIT1(-0.4089766186f);
  static const __m128 btm2 = XM2_SSE_INIT1(-0.9315200116f);
  static const __m128 btm3 = XM2_SSE_INIT1(0.2836182315f);
  static const __m128 btm4 = XM2_SSE_INIT1(-0.1535779990f);

  // perform approximation in all 4 elements
  const __m128 top = _mm_add_ps(top0,_mm_mul_ps(_mm_add_ps(top1,_mm_mul_ps(top2,x)),x));
  const __m128 bottom = _mm_add_ps(btm0,
                            _mm_mul_ps(_mm_add_ps(btm1,
                                _mm_mul_ps(_mm_add_ps(btm2,
                                    _mm_mul_ps(_mm_add_ps(btm3,
                                        _mm_mul_ps(btm4,x)),x)),x)),x));
  const __m128 result =  _mm_sub_ps(_mm_div_ps(top,bottom),
                                    _mm_sqrt_ps(_mm_sub_ps(_mm_set_ps1(1.0f),_mm_mul_ps(x,x))));

  // make sure we negate any results that we're negative originally
  return _mm_or_ps(_mm_and_ps(test,_mm_sub_ps(_mm_setzero_ps(),result)),
                   _mm_andnot_ps(test,result));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XMatan(__m128 x)
{
  // arctan(x) = arcsin( x/sqrt(x^2 +1) )
  return XMasin( _mm_div_ps(x, _mm_sqrt_ps(_mm_add_ps(_mm_set1_ps(1.0),_mm_mul_ps(x,x))) ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XMatan2(const __m128 x,const __m128 y) 
{ 
  __m128 coeff_1 = _mm_set1_ps(0.78539816339744830961566084581988f);
  __m128 coeff_2 = _mm_set1_ps(0.3561944901923449288469825374596f);
  __m128 abs_y = XMabs(y);


  __m128 x_minus_y = _mm_sub_ps(x,abs_y);
  __m128 x_plus_y  = _mm_add_ps(x,abs_y);
  __m128 y_minus_x = _mm_sub_ps(abs_y,x);

  __m128 r0 = _mm_div_ps(x_minus_y,x_plus_y); //  (x - abs_y) / (x + abs_y);
  __m128 r1 = _mm_div_ps(x_plus_y,y_minus_x); //  (x + abs_y) / (abs_y - x);

  r0 = _mm_sub_ps(coeff_1,_mm_mul_ps(r0,coeff_1));
  r1 = _mm_sub_ps(coeff_2,_mm_mul_ps(r1,coeff_1));


  __m128 x_gt_0 = _mm_cmpge_ps(x,_mm_setzero_ps());

  __m128 angle = _mm_or_ps(_mm_and_ps(x_gt_0,r0),
                           _mm_andnot_ps(x_gt_0,r1));

  // return y < 0 ? -angle : angle;
  x_gt_0 = _mm_cmplt_ps(y,_mm_setzero_ps());
  return _mm_or_ps(_mm_and_ps(x_gt_0,_mm_sub_ps(_mm_setzero_ps(),angle)),
                   _mm_andnot_ps(x_gt_0,angle));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XMsqrt(const __m128 d) 
{ 
  return _mm_sqrt_ps(d);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XMabs(const __m128 d) 
{ 
  __m128 a = _mm_cmplt_ps(d,_mm_setzero_ps());
  return _mm_or_ps( _mm_andnot_ps(a,d), _mm_and_ps(a,_mm_sub_ps(_mm_setzero_ps(),d)));
}
}
#endif
