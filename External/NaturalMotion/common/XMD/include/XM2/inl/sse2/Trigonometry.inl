#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_TRIGONOMETRY__INL__
#define XM2_SSE2_TRIGONOMETRY__INL__
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128d XMcos(const __m128d d)
{ 
  static const __m128d _t0 = XM2_SSE2_INIT1(-1.0f/2.0f);
  static const __m128d _t1 = XM2_SSE2_INIT1(1.0f/24.0f);
  static const __m128d _t2 = XM2_SSE2_INIT1(-1.0f/720.0f);
  static const __m128d _t3 = XM2_SSE2_INIT1(1.0f/40320.0f);
  static const __m128d _t4 = XM2_SSE2_INIT1(-1.0f/3628800.0f);
  const __m128d d2  = _mm_mul_pd(d,d);
  const __m128d d4  = _mm_mul_pd(d2,d2);
  const __m128d d6  = _mm_mul_pd(d4,d2);
  const __m128d d8  = _mm_mul_pd(d6,d2);
  const __m128d d10 = _mm_mul_pd(d8,d2);
  const __m128d t2  = _mm_mul_pd(d2, _t0);
  const __m128d t3  = _mm_mul_pd(d4, _t1);
  const __m128d t4  = _mm_mul_pd(d6, _t2);
  const __m128d t5  = _mm_mul_pd(d8, _t3);
  const __m128d t6  = _mm_mul_pd(d10,_t4);
  return _mm_add_pd(_mm_set1_pd(1.0),
            _mm_add_pd(t2,
               _mm_add_pd(t3,
                 _mm_add_pd(t4,
                    _mm_add_pd(t5,t6)))));
}
/*
XReal sine(XReal x)
{
  const XReal B = 4.0f/XM2_PI;
  const XReal C = -4.0f/(XM2_PI*XM2_PI);

  XReal y = B * x + C * x * XMabs(x);

  const XReal Q = 0.775;
  const XReal P = 0.225;

  y = P * (y * abs(y) - y) + y;   
  y = Q * y + P * y * abs(y)
}*/
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128d XMsin(const __m128d d) 
{
  static const __m128d _t0 = XM2_SSE2_INIT1(-1.0f/6.0f);
  static const __m128d _t1 = XM2_SSE2_INIT1(+1.0f/120.0f);
  static const __m128d _t2 = XM2_SSE2_INIT1(-1.0f/5040.0f);
  static const __m128d _t3 = XM2_SSE2_INIT1(+1.0f/362880.0f);
  static const __m128d _t4 = XM2_SSE2_INIT1(-1.0f/39916800.0f);
  static const __m128d _t5 = XM2_SSE2_INIT1(+1.0f/6227020800.0f);
  const __m128d d2 = _mm_mul_pd(d,d);
  const __m128d d3 = _mm_mul_pd(d,d2);
  const __m128d d4 = _mm_mul_pd(d3,d2);
  const __m128d d5 = _mm_mul_pd(d4,d2);
  const __m128d d6 = _mm_mul_pd(d5,d2);
  const __m128d d7 = _mm_mul_pd(d6,d2);
  const __m128d d8 = _mm_mul_pd(d7,d2);
  const __m128d t1 = _mm_mul_pd(d3,_t0);
  const __m128d t2 = _mm_mul_pd(d4,_t1);
  const __m128d t3 = _mm_mul_pd(d5,_t2);
  const __m128d t4 = _mm_mul_pd(d6,_t3);
  const __m128d t5 = _mm_mul_pd(d7,_t4);
  const __m128d t6 = _mm_mul_pd(d8,_t5);
  return _mm_add_pd(d,
            _mm_add_pd(t1,
               _mm_add_pd(t2,
                 _mm_add_pd(t3,
                    _mm_add_pd(t4,
                      _mm_add_pd(t5,t6))))));
}  
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128d XMtan(__m128d x)
{
  // arcos(X) = PI/2 - arcsin(X)
  return _mm_div_pd( XMsin(x), XMcos(x) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128d XMacos(__m128d x)
{
  // arcos(X) = PI/2 - arcsin(X)
  static const __m128d offset = XM2_SSE2_INIT1(1.5707963267948966192313216916398f);
  return _mm_sub_pd(offset,XMasin(x));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128d XMasin(__m128d x)
{
  // going to use the approximation detailed at the tail end of this page:
  //  http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/arcsin/fastsqrt.html

  // check to see if values are less than zero
  __m128d test = _mm_cmplt_pd(x,_mm_setzero_pd());

  // flip sign if negative
  x = _mm_or_pd(_mm_and_pd(test,_mm_sub_pd(_mm_setzero_pd(),x)),
                _mm_andnot_pd(test,x));

  // init co-effs 
  static const __m128d top0 = XM2_SSE2_INIT1(1.507095111);
  static const __m128d top1 = XM2_SSE2_INIT1(1.103007131);
  static const __m128d top2 = XM2_SSE2_INIT1(-2.144008022);
  static const __m128d btm0 = XM2_SSE2_INIT1(1.507171600);
  static const __m128d btm1 = XM2_SSE2_INIT1(-0.4089766186);
  static const __m128d btm2 = XM2_SSE2_INIT1(-0.9315200116);
  static const __m128d btm3 = XM2_SSE2_INIT1(0.2836182315f);
  static const __m128d btm4 = XM2_SSE2_INIT1(-0.1535779990);
  // perform approximation in all 4 elements
  const __m128d top = _mm_add_pd(top0,_mm_mul_pd(_mm_add_pd(top1,_mm_mul_pd(top2,x)),x));
  const __m128d bottom = _mm_add_pd(btm0,
                            _mm_mul_pd(_mm_add_pd(btm1,
                                _mm_mul_pd(_mm_add_pd(btm2,
                                    _mm_mul_pd(_mm_add_pd(btm3,
                                        _mm_mul_pd(btm4,x)),x)),x)),x));
  const __m128d result =  _mm_sub_pd(_mm_div_pd(top,bottom),
                                    _mm_sqrt_pd(_mm_sub_pd(_mm_set1_pd(1.0),_mm_mul_pd(x,x))));

  // make sure we negate any results that we're negative originally
  return _mm_or_pd(_mm_and_pd(test,_mm_sub_pd(_mm_setzero_pd(),result)),
                   _mm_andnot_pd(test,result));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128d XMatan(__m128d x)
{
  // arctan(x) = arcsin( x/sqrt(x^2 +1) )
  return XMasin( _mm_div_pd(x, _mm_sqrt_pd(_mm_add_pd(_mm_set1_pd(1.0),_mm_mul_pd(x,x))) ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128d XMatan2(const __m128d x,const __m128d y) 
{ 
  const __m128d coeff_1 = _mm_set1_pd(0.78539816339744830961566084581988);
  const __m128d coeff_2 = _mm_set1_pd(0.3561944901923449288469825374596);
  const __m128d abs_y = XMabs(y);


  const __m128d x_minus_y = _mm_sub_pd(x,abs_y);
  const __m128d x_plus_y  = _mm_add_pd(x,abs_y);
  const __m128d y_minus_x = _mm_sub_pd(abs_y,x);

  __m128d r0 = _mm_div_pd(x_minus_y,x_plus_y); //  (x - abs_y) / (x + abs_y);
  __m128d r1 = _mm_div_pd(x_plus_y,y_minus_x); //  (x + abs_y) / (abs_y - x);

  r0 = _mm_sub_pd(coeff_1,_mm_mul_pd(r0,coeff_1));
  r1 = _mm_sub_pd(coeff_2,_mm_mul_pd(r1,coeff_1));


  __m128d x_gt_0 = _mm_cmpge_pd(x,_mm_setzero_pd());
    
  const __m128d angle = _mm_or_pd(_mm_and_pd(x_gt_0,r0),
                                  _mm_andnot_pd(x_gt_0,r1));

  // return y < 0 ? -angle : angle;
  x_gt_0 = _mm_cmplt_pd(y,_mm_setzero_pd());
  return _mm_or_pd(_mm_and_pd(x_gt_0,_mm_sub_pd(_mm_setzero_pd(),angle)),
                   _mm_andnot_pd(x_gt_0,angle));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128d XMsqrt(const __m128d d) 
{ 
  return _mm_sqrt_pd(d);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128d XMabs(const __m128d d) 
{ 
  __m128d a = _mm_cmplt_pd(d,_mm_setzero_pd());
  return _mm_or_pd( _mm_andnot_pd(a,d), _mm_and_pd(a,_mm_sub_pd(_mm_setzero_pd(),d)));
}
}
#endif
