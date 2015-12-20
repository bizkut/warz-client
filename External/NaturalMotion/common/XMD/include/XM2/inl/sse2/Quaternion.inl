#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_QUATERNION__INL__
#define XM2_SSE2_QUATERNION__INL__
#if XM2_USE_FLOAT
# error SSE2 double precision routines included when using float
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion::XQuaternion(const __m128d& _xy,const __m128d& _zw)
{
  xy = _xy;
  zw = _zw;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::toMatrix(XMatrix& mat) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&mat);
  const __m128d XX0 = _mm_mul_pd(xy,_mm_shuffle_pd(xy,xy,_MM_SHUFFLE2(0,0)));
  const __m128d XX1 = _mm_mul_pd(zw,_mm_shuffle_pd(xy,xy,_MM_SHUFFLE2(0,0)));
  const __m128d YY0 = _mm_mul_pd(xy,_mm_shuffle_pd(xy,xy,_MM_SHUFFLE2(1,1)));
  const __m128d YY1 = _mm_mul_pd(zw,_mm_shuffle_pd(xy,xy,_MM_SHUFFLE2(1,1)));
  const __m128d ZZ0 = _mm_mul_pd(xy,_mm_shuffle_pd(zw,zw,_MM_SHUFFLE2(0,0)));
  const __m128d ZZ1 = _mm_mul_pd(zw,_mm_shuffle_pd(zw,zw,_MM_SHUFFLE2(0,0)));
  const __m128d _XX0 = XX1;
  const __m128d _XX1 = XX0;
  const __m128d _YY0 = _mm_shuffle_pd(YY0,YY0,_MM_SHUFFLE2(0,1));
  const __m128d _YY1 = _mm_shuffle_pd(YY1,YY1,_MM_SHUFFLE2(0,1));
  const __m128d _ZZ0 = _mm_shuffle_pd(ZZ1,ZZ1,_MM_SHUFFLE2(0,1));
  const __m128d _ZZ1 = _mm_shuffle_pd(ZZ0,ZZ0,_MM_SHUFFLE2(0,1));
  static const __m128d temp = XM2_SSE2_INIT(1.0f,-1.0f);
  __m128d YY_ZZ0 = _mm_add_pd(YY0,_ZZ0);
  __m128d YY_ZZ1 = _mm_add_pd(_mm_mul_pd(temp,YY1),_ZZ1);
  __m128d ZZ_XX0 = _mm_add_pd(ZZ0,_XX0);
  __m128d ZZ_XX1 = _mm_add_pd(_mm_mul_pd(temp,ZZ1),_XX1);
  __m128d XX_YY0 = _mm_add_pd(XX0,_YY0);
  __m128d XX_YY1 = _mm_add_pd(_mm_mul_pd(temp,XX1),_YY1);

  YY_ZZ0 = _mm_shuffle_pd(YY_ZZ0,YY_ZZ0,_MM_SHUFFLE2(0,1));
  YY_ZZ1 = _mm_shuffle_pd(YY_ZZ1,YY_ZZ1,_MM_SHUFFLE2(0,1));
  ZZ_XX1 = _mm_shuffle_pd(ZZ_XX1,ZZ_XX1,_MM_SHUFFLE2(0,1));
  ZZ_XX0 = _mm_shuffle_pd(ZZ_XX0,ZZ_XX0,_MM_SHUFFLE2(0,1));
  static const __m128d one1 = XM2_SSE2_INIT(1.0f,0.0f);
  static const __m128d one2 = XM2_SSE2_INIT(0.0f,1.0f);
  static const __m128d sc1 = XM2_SSE2_INIT(-2.0f,2.0f);
  static const __m128d sc2 = XM2_SSE2_INIT(2.0f,-2.0f);
  static const __m128d sc3 = XM2_SSE2_INIT(-2.0f,0.0f);
  static const __m128d sc4 = XM2_SSE2_INIT(2.0f,0.0f);
  static const __m128d sc5 = XM2_SSE2_INIT(2.0f,2.0f);
  mat._xaxis_xy = _mm_add_pd(one1,_mm_mul_pd(sc1,YY_ZZ0));
  mat._xaxis_zw = _mm_mul_pd(sc4,YY_ZZ1);
  mat._yaxis_xy = _mm_add_pd(one2,_mm_mul_pd(sc2,ZZ_XX1));
  mat._yaxis_zw = _mm_mul_pd(sc4,ZZ_XX0);
  mat._zaxis_xy = _mm_mul_pd(sc5,XX_YY1);
  mat._zaxis_zw = _mm_add_pd(one1,_mm_mul_pd(sc3,XX_YY0));
  mat._waxis_xy = _mm_setzero_pd();
  mat._waxis_zw = one2;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::mul(const XQuaternion& a,const XQuaternion& b)
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128d arg0x = _mm_mul_pd(_mm_shuffle_pd(a.zw,a.zw,_MM_SHUFFLE2(1,1)),b.xy);
  __m128d arg0y = _mm_mul_pd(_mm_shuffle_pd(a.zw,a.zw,_MM_SHUFFLE2(1,1)),b.zw);
  __m128d arg1x = _mm_mul_pd(_mm_shuffle_pd(a.xy,a.xy,_MM_SHUFFLE2(1,0)),
                             _mm_shuffle_pd(b.zw,b.zw,_MM_SHUFFLE2(1,1)));
  __m128d arg1y = _mm_mul_pd(_mm_shuffle_pd(a.zw,a.xy,_MM_SHUFFLE2(0,0)),
                             _mm_shuffle_pd(b.zw,b.xy,_MM_SHUFFLE2(0,1)));
  __m128d arg2x = _mm_mul_pd(_mm_shuffle_pd(a.zw,a.xy,_MM_SHUFFLE2(0,0)),
                             _mm_shuffle_pd(b.xy,b.zw,_MM_SHUFFLE2(0,1)));
  __m128d arg2y = _mm_mul_pd(_mm_shuffle_pd(a.xy,a.xy,_MM_SHUFFLE2(1,1)),
                             _mm_shuffle_pd(b.xy,b.xy,_MM_SHUFFLE2(1,0)));
  __m128d arg3x = _mm_mul_pd(_mm_shuffle_pd(a.xy,a.zw,_MM_SHUFFLE2(0,1)),
                             _mm_shuffle_pd(b.zw,b.xy,_MM_SHUFFLE2(0,0)));
  __m128d arg3y = _mm_mul_pd(_mm_shuffle_pd(a.xy,a.zw,_MM_SHUFFLE2(0,0)),
                             _mm_shuffle_pd(b.xy,b.zw,_MM_SHUFFLE2(0,1)));
  XM2_ALIGN_PREFIX(16) static const unsigned _temp[] = {0,0,0xFFFFFFFF,0xFFFFFFFF} XM2_ALIGN_SUFFIX(16);
  const __m128d& arg4 = *(const __m128d*)((const void*)_temp);
  arg1y = _mm_or_pd( _mm_and_pd(arg4,_mm_sub_pd(_mm_setzero_pd(),arg1y)),
                     _mm_andnot_pd(arg4,arg1y) );
  arg2y = _mm_or_pd( _mm_and_pd(arg4,_mm_sub_pd(_mm_setzero_pd(),arg2y)),
                     _mm_andnot_pd(arg4,arg2y) );
  xy = _mm_add_pd(_mm_add_pd(arg0x,arg1x),_mm_sub_pd(arg2x,arg3x));
  zw = _mm_add_pd(_mm_add_pd(arg0y,arg1y),_mm_sub_pd(arg2y,arg3y));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternion::equal(const XQuaternion& b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XQuaternion temp = *this;
  
  temp.xy = _mm_sub_pd(temp.xy,b.xy);
  temp.zw = _mm_sub_pd(temp.zw,b.zw);
  temp.xy = _mm_mul_pd(temp.xy,temp.xy);
  temp.zw = _mm_mul_pd(temp.zw,temp.zw);
  __m128d _eps = _mm_set1_pd(eps*eps);
  return (3 == _mm_movemask_pd(_mm_cmplt_pd(temp.xy,_eps))) &&
         (3 == _mm_movemask_pd(_mm_cmplt_pd(temp.zw,_eps)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::zero()
{
  XM2_ALIGN_ASSERT(this);
  xy = _mm_setzero_pd();
  zw = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::copy(const XQuaternion& q)
{
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  xy = q.xy;
  zw = q.zw;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XQuaternion::dot(const XQuaternion& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  // add xy+xy and zw+zw
  XQuaternion q( _mm_mul_pd(xy,b.xy), _mm_mul_pd(zw,b.zw) );

  // sum xy+zw
  q.xy = _mm_add_pd(q.xy,q.zw);

  // shuffle input to get  xx, yy and sum
  q.xy = _mm_add_sd(_mm_shuffle_pd(q.xy,q.xy,_MM_SHUFFLE2(0,0)),
                    _mm_shuffle_pd(q.xy,q.xy,_MM_SHUFFLE2(1,1)));
  return q.x;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::identity()
{
  XM2_ALIGN_ASSERT(this);
  xy = _mm_set_pd(0.0f,0.0f);
  zw = _mm_set_pd(1.0f,0.0f);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::set(const XReal x,const XReal y,const XReal z,const XReal w)
{
  XM2_ALIGN_ASSERT(this);
  xy = _mm_set_pd(y,x);
  zw = _mm_set_pd(w,z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::sum(const XQuaternion& a,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(t);
  xy = _mm_add_pd(xy,_mm_mul_pd(a.xy,temp));
  zw = _mm_add_pd(zw,_mm_mul_pd(a.zw,temp));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::mul(const XQuaternion& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_mul_pd(a.xy,_mm_set1_pd(b));
  zw = _mm_mul_pd(a.zw,_mm_set1_pd(b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::add(const XQuaternion& a,const XQuaternion& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_add_pd(a.xy,b.xy);
  zw = _mm_add_pd(a.zw,b.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::fromMatrix(const XMatrix& mat)
{ 
  XM2_ALIGN_ASSERT(&mat);
  XM2_ALIGN_ASSERT(this);

  const __m128d _t00x = _mm_shuffle_pd(mat._xaxis_xy,_mm_sub_pd(_mm_setzero_pd(),mat._xaxis_xy),_MM_SHUFFLE2(0,0));  //  + - 
  const __m128d _t00z = _mm_shuffle_pd(_t00x,_t00x,_MM_SHUFFLE2(0,1));
  const __m128d _t11  = _mm_shuffle_pd(_mm_sub_pd(_mm_setzero_pd(),mat._yaxis_xy),mat._yaxis_xy,_MM_SHUFFLE2(1,1));  //  - + 
  const __m128d _t22z = _mm_shuffle_pd(mat._zaxis_zw,mat._zaxis_zw,_MM_SHUFFLE2(0,0)); //  + + 
  const __m128d _t22x = _mm_sub_pd(_mm_setzero_pd(),_t22z);   //  - - 

  const __m128d temp0 = _mm_add_pd(_mm_set1_pd(1.0f),_mm_add_pd(_t00x,_mm_add_pd(_t11,_t22x)));
  const __m128d temp1 = _mm_add_pd(_mm_set1_pd(1.0f),_mm_add_pd(_t00z,_mm_add_pd(_t11,_t22z)));
  xy = _mm_mul_pd( _mm_sqrt_pd( _mm_max_pd(_mm_setzero_pd(),temp0) ),_mm_set1_pd(0.5f) );
  zw = _mm_mul_pd( _mm_sqrt_pd( _mm_max_pd(_mm_setzero_pd(),temp1) ),_mm_set1_pd(0.5f) );

  // some temp vars. 
  // W must be 1 in sign_test0 in order to make (1-0) < 0 always false. 
  // this prevents the w axis from getting incorrectly negated. 
  /// \todo use a shuffle to avoid the generated _mm_loadu_ss intructions
  const __m128d sign_test0x = _mm_set_pd(mat.m02,mat.m21); 
  const __m128d sign_test0y = _mm_set_pd(0,mat.m10); 
  const __m128d sign_test1x = _mm_set_pd(mat.m20,mat.m12);
  const __m128d sign_test1y = _mm_set_pd(0,mat.m01);
  const __m128d sign_testx = _mm_cmplt_pd(_mm_sub_pd(sign_test1x,sign_test0x),_mm_setzero_pd());
  const __m128d sign_testy = _mm_cmplt_pd(_mm_sub_pd(sign_test1y,sign_test0y),_mm_setzero_pd());

  // flip signs on required (XYZ) elements.
  xy = _mm_or_pd( _mm_andnot_pd(sign_testx,xy), 
                  _mm_and_pd(sign_testx,_mm_sub_pd(_mm_setzero_pd(),xy)) );
  zw = _mm_or_pd( _mm_andnot_pd(sign_testy,zw), 
                  _mm_and_pd(sign_testy,_mm_sub_pd(_mm_setzero_pd(),zw)) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::toAxisAngleR(XVector3& axis,XReal& radians) const
{
  XM2_ALIGN_ASSERT(&axis);
  XM2_ALIGN_ASSERT(this);

  XReal cos_a = w;
  XReal sin_a = XMsqrt( 1.0f - cos_a * cos_a );

  radians = XMacos( cos_a ) * 2.0f;
  if ( XMabs( sin_a ) < XM2_FLOAT_ZERO )
  {
    sin_a = 1.0f;
  }
  sin_a = 1.0f/sin_a;
  const __m128d temp = _mm_set1_pd(sin_a);
  axis.xy = _mm_mul_pd( temp, xy );
  axis.zw = _mm_shuffle_pd(_mm_mul_pd( temp, zw ),_mm_setzero_pd(),_MM_SHUFFLE2(1,0)) ;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::shortestPath(const XQuaternion &a,const XQuaternion &b,XReal& product)
{
  __m128d temp;
  shortestPath(a,b,temp);
  _mm_store_sd(&product,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::shortestPath(const XQuaternion &a,const XQuaternion &b,__m128d& product)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  // perform dot product
  product = a.dotSSE(b);
  {
    const __m128d compare_gt = _mm_cmpgt_pd(_mm_setzero_pd(),product);

    product = _mm_or_pd( _mm_and_pd(compare_gt,product),
                         _mm_andnot_pd(compare_gt,_mm_sub_pd(_mm_setzero_pd(),product)) );

    xy      = _mm_or_pd( _mm_and_pd(compare_gt,b.xy),
                         _mm_andnot_pd(compare_gt,_mm_sub_pd(_mm_setzero_pd(),b.xy)) );
    zw      = _mm_or_pd( _mm_and_pd(compare_gt,b.zw),
                         _mm_andnot_pd(compare_gt,_mm_sub_pd(_mm_setzero_pd(),b.zw)) );
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::longestPath(const XQuaternion &a,const XQuaternion &b,XReal& product)
{
  __m128d temp;
  longestPath(a,b,temp);
  _mm_store_sd(&product,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::longestPath(const XQuaternion &a,const XQuaternion &b,__m128d& product)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  // perform dot product
  product = a.dotSSE(b);
  {
    const __m128d compare_lt = _mm_cmplt_ps(_mm_setzero_ps(),product);

    product = _mm_or_pd( _mm_and_pd(compare_lt,product),
                         _mm_andnot_pd(compare_lt,_mm_sub_pd(_mm_setzero_ps(),product)) );

    xy     = _mm_or_pd( _mm_and_pd(compare_lt,b.xy),
                        _mm_andnot_pd(compare_lt,_mm_sub_pd(_mm_setzero_ps(),b.xy)) );
    zw     = _mm_or_pd( _mm_and_pd(compare_lt,b.zw),
                        _mm_andnot_pd(compare_lt,_mm_sub_pd(_mm_setzero_ps(),b.zw)) );
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::slerp(const XQuaternion& a,const XQuaternion& b,const XReal t)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  __m128d interpolating_t,inv_interpolating_t,theta,sinTheta,product;

  shortestPath(a,b,product);

  // calculate the linear t values for the interpolation
  interpolating_t = _mm_set_pd(t);

  theta = XMacos(product);

  // pre-calculate the sin value
  sinTheta = XMsin(theta);
  sinTheta = float_equal(sinTheta,0.0f) ? 0.0f : 1.0f/sinTheta;

  sinTheta = _mm_rcp_ps(XMsin(theta));

  interpolating_t     = _mm_mul_ps(XMsin(_mm_mul_ps(_mm_set_ps1(     t),theta)) , sinTheta);
  inv_interpolating_t = _mm_mul_ps(XMsin(_mm_mul_ps(_mm_set_ps1(1.0f-t),theta)) , sinTheta);

  // perform the interpolation
  xy = _mm_add_ps(_mm_mul_ps(inv_interpolating_t,a.xy), 
                  _mm_mul_ps(interpolating_t,xy));
  zw = _mm_add_ps(_mm_mul_ps(inv_interpolating_t,a.zw), 
                  _mm_mul_ps(interpolating_t,zw));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::lerp(const XQuaternion& a,const XQuaternion& b,const XReal t)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  __m128d product;
  shortestPath(a,b,product);

  // perform the interpolation
  const __m128d temp = _mm_set1_pd(t);
  xy = _mm_add_pd(a.xy, _mm_mul_pd(temp,_mm_sub_pd(xy,a.xy)) );
  zw = _mm_add_pd(a.zw, _mm_mul_pd(temp,_mm_sub_pd(zw,a.zw)) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::invert(const XQuaternion& q)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_mul_pd(q.xy,_mm_set_pd(-1.0f,-1.0f));
  zw = _mm_mul_pd(q.zw,_mm_set_pd(1.0f,-1.0f));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::negate(const XQuaternion& q)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_sub_pd(_mm_setzero_pd(),q.xy);
  zw = _mm_sub_pd(_mm_setzero_pd(),q.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::normalise(const XQuaternion& q)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);

  {
    const XReal l_inv = q.length();
    if(!float_equal(l_inv,0))
    {
      __m128d temp = _mm_set1_pd(1.0/l_inv);
      xy = _mm_mul_pd(q.xy, temp);
      zw = _mm_mul_pd(q.zw, temp);
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternion& XQuaternion::operator += (const XQuaternion& c)
{
  XM2_ALIGN_ASSERT(&c);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_add_pd(xy,c.xy);
  zw = _mm_add_pd(zw,c.zw);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternion& XQuaternion::operator -= (const XQuaternion& c)
{
  XM2_ALIGN_ASSERT(&c);
  XM2_ALIGN_ASSERT(this);
  xy = _mm_sub_pd(xy,c.xy);
  zw = _mm_sub_pd(zw,c.zw);
  return *this;
}
}
#endif
