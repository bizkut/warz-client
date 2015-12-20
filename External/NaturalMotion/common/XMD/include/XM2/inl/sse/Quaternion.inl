#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_QUATERNION__INL__
#define XM2_SSE_QUATERNION__INL__
#if (!XM2_USE_FLOAT)
# error SSE2 float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternion::XQuaternion(const __m128& q)
{
  XM2_ALIGN_ASSERT(this);
  sse = q;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::zero()
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::toMatrix(XMatrix& mat) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&mat);

  // X*This  Y*This  Z*this
  const __m128 XX = _mm_mul_ps(sse,_mm_shuffle_ps(sse,sse,_MM_SHUFFLE(0,0,0,0)));
  const __m128 YY = _mm_mul_ps(sse,_mm_shuffle_ps(sse,sse,_MM_SHUFFLE(1,1,1,1)));
  const __m128 ZZ = _mm_mul_ps(sse,_mm_shuffle_ps(sse,sse,_MM_SHUFFLE(2,2,2,2)));

  // swizzle values on RHS
  const __m128 _XX = _mm_shuffle_ps(XX,XX,_MM_SHUFFLE(1,0,3,2));
  const __m128 _YY = _mm_shuffle_ps(YY,YY,_MM_SHUFFLE(2,3,0,1));
  const __m128 _ZZ = _mm_shuffle_ps(ZZ,ZZ,_MM_SHUFFLE(0,1,2,3));

  /*
     YY[0] + ZZ[3];
     YY[1] + ZZ[2];
    -YY[3] + ZZ[0];

     ZZ[1] + XX[3];
     ZZ[2] + XX[0];
    -ZZ[3] + XX[1];

     XX[0] + YY[1];
     XX[2] + YY[3];
    -XX[3] + YY[2];
  */

  // W is always negative on LHS of eqn
  static const __m128 temp = XM2_SSE_INIT(1.0f,1.0f,1.0f,-1.0f);
  __m128 YY_ZZ = _mm_add_ps(_mm_mul_ps(temp,YY),_ZZ);
  __m128 ZZ_XX = _mm_add_ps(_mm_mul_ps(temp,ZZ),_XX);
  __m128 XX_YY = _mm_add_ps(_mm_mul_ps(temp,XX),_YY);

  // swizzle back for xaxis - i.e.
  //    YY[1] + ZZ[2];
  //    YY[0] + ZZ[3];
  //   -YY[3] + ZZ[0];
  //    
  YY_ZZ = _mm_shuffle_ps(YY_ZZ,YY_ZZ,_MM_SHUFFLE(2,3,0,1));

  // swizzle back for yaxis - i.e.
  //   -ZZ[3]+XX[1];
  //    ZZ[2]+XX[0];
  //    ZZ[1]+XX[3];
  ZZ_XX = _mm_shuffle_ps(ZZ_XX,ZZ_XX,_MM_SHUFFLE(0,1,2,3));

  // swizzle back for zaxis - i.e.
  //    XX[2]+YY[3];
  //   -XX[3]+YY[2];
  //    XX[0]+YY[1];
  XX_YY = _mm_shuffle_ps(XX_YY,XX_YY,_MM_SHUFFLE(1,0,3,2));

  static const __m128 one1 = XM2_SSE_INIT(1.0f,0.0f,0.0f,0.0f);
  static const __m128 one2 = XM2_SSE_INIT(0.0f,1.0f,0.0f,0.0f);
  static const __m128 one3 = XM2_SSE_INIT(0.0f,0.0f,1.0f,0.0f);
  static const __m128 one4 = XM2_SSE_INIT(0.0f,0.0f,0.0f,1.0f);
  static const __m128 sc1 = XM2_SSE_INIT(-2.0f,2.0f,2.0f,0.0f);
  static const __m128 sc2 = XM2_SSE_INIT(2.0f,-2.0f,2.0f,0.0f);
  static const __m128 sc3 = XM2_SSE_INIT(2.0f,2.0f,-2.0f,0.0f);

  mat._xaxis = _mm_add_ps(one1,_mm_mul_ps(sc1,YY_ZZ));
  mat._yaxis = _mm_add_ps(one2,_mm_mul_ps(sc2,ZZ_XX));
  mat._zaxis = _mm_add_ps(one3,_mm_mul_ps(sc3,XX_YY));
  mat._waxis = one4;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::mul(const XQuaternion& a,const XQuaternion& b)
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128 arg0 = _mm_mul_ps(_mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(3,3,3,3)),b.sse);
  __m128 arg1 = _mm_mul_ps(_mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(0,2,1,0)),
                           _mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(0,3,3,3)));

  __m128 arg2 = _mm_mul_ps(_mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(1,1,0,2)),
                           _mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(1,0,2,1)));

  __m128 arg3 = _mm_mul_ps(_mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(2,0,2,1)),
                           _mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(2,1,0,2)));
  XM2_ALIGN_PREFIX(16) static const unsigned _temp[] = {0,0,0,0xFFFFFFFF} XM2_ALIGN_SUFFIX(16);
  const __m128& arg4 = *(const __m128*)((const void*)_temp);
  arg1 = _mm_or_ps( _mm_and_ps(arg4,_mm_sub_ps(_mm_setzero_ps(),arg1)),
                    _mm_andnot_ps(arg4,arg1) );
  arg2 = _mm_or_ps( _mm_and_ps(arg4,_mm_sub_ps(_mm_setzero_ps(),arg2)),
                    _mm_andnot_ps(arg4,arg2) );
  sse = _mm_add_ps(_mm_add_ps(arg0,arg1),_mm_sub_ps(arg2,arg3));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternion::equal(const XQuaternion& b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XQuaternion temp = *this;
  temp.sse = _mm_sub_ps(temp.sse,b.sse);
  temp.sse = _mm_mul_ps(temp.sse,temp.sse);
  return 15 == _mm_movemask_ps(_mm_cmplt_ps(temp.sse,_mm_set_ps1(eps*eps)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::copy(const XQuaternion& q)
{
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  sse = q.sse;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XReal XQuaternion::dot(const XQuaternion& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128 q = _mm_mul_ps(sse,b.sse);
  q = _mm_add_ss( _mm_shuffle_ps(q,q,_MM_SHUFFLE(0,0,0,0)),
        _mm_add_ss( _mm_shuffle_ps(q,q,_MM_SHUFFLE(1,1,1,1)),
          _mm_add_ss( _mm_shuffle_ps(q,q,_MM_SHUFFLE(2,2,2,2)),
                      _mm_shuffle_ps(q,q,_MM_SHUFFLE(3,3,3,3)))));
  return *((float*)((void*)&q));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XQuaternion::dotSSE(const XQuaternion& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128 q = _mm_mul_ps(sse,b.sse);
  q = _mm_add_ss( _mm_shuffle_ps(q,q,_MM_SHUFFLE(0,0,0,0)),
        _mm_add_ss( _mm_shuffle_ps(q,q,_MM_SHUFFLE(1,1,1,1)),
          _mm_add_ss( _mm_shuffle_ps(q,q,_MM_SHUFFLE(2,2,2,2)),
                      _mm_shuffle_ps(q,q,_MM_SHUFFLE(3,3,3,3)))));
  return q;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::identity()
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_set_ps(1.0f,0.0f,0.0f,0.0f);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::set(const XReal x,const XReal y,const XReal z,const XReal w)
{
  XM2_ALIGN_ASSERT(this);
  sse = _mm_set_ps(w,z,y,x);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::sum(const XQuaternion& a,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps(sse,_mm_mul_ps(a.sse,_mm_set_ps1(t)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::mul(const XQuaternion& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_mul_ps(a.sse,_mm_set_ps1(b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::add(const XQuaternion& a,const XQuaternion& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_add_ps(a.sse,b.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::fromMatrix(const XMatrix& mat)
{ 
  XM2_ALIGN_ASSERT(&mat);
  XM2_ALIGN_ASSERT(this);
  const __m128 xx = _mm_shuffle_ps(mat._xaxis,_mm_sub_ps(_mm_setzero_ps(),mat._xaxis),_MM_SHUFFLE(0,0,0,0));
  const __m128 yy = _mm_shuffle_ps(mat._yaxis,_mm_sub_ps(_mm_setzero_ps(),mat._yaxis),_MM_SHUFFLE(1,1,1,1));
  const __m128 zz = _mm_shuffle_ps(mat._zaxis,_mm_sub_ps(_mm_setzero_ps(),mat._zaxis),_MM_SHUFFLE(2,2,2,2));
  const __m128 _t00 = _mm_shuffle_ps(xx,xx,_MM_SHUFFLE(0,2,2,0));
  const __m128 _t11 = _mm_shuffle_ps(yy,yy,_MM_SHUFFLE(0,2,0,2));
  const __m128 _t22 = _mm_shuffle_ps(zz,zz,_MM_SHUFFLE(0,0,2,2));
  const __m128 temp = _mm_add_ps(_mm_set_ps1(1.0f),_mm_add_ps(_t00,_mm_add_ps(_t11,_t22)));
  sse = _mm_mul_ps( _mm_sqrt_ps( _mm_max_ps(_mm_setzero_ps(),temp) ),_mm_set_ps1(0.5f) );

  // some temp vars. 
  // W must be 1 in sign_test0 in order to make (1-0) < 0 always false. 
  // this prevents the w axis from getting incorrectly negated. 
  const __m128 sign_test0 = _mm_set_ps(0,mat.m10,mat.m02,mat.m21); 
  const __m128 sign_test1 = _mm_set_ps(0,mat.m01,mat.m20,mat.m12);
  const __m128 sign_test = _mm_cmplt_ps(_mm_sub_ps(sign_test1,sign_test0),_mm_setzero_ps());

  // flip signs on required (XYZ) elements.
  sse = _mm_add_ps( _mm_andnot_ps(sign_test,sse), 
                    _mm_and_ps(sign_test,_mm_sub_ps(_mm_setzero_ps(),sse)) );
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
  axis.sse = _mm_mul_ps( _mm_set_ps1(sin_a), sse );
  axis.w = 0.0f;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::shortestPath(const XQuaternion &a,const XQuaternion &b,XReal& product)
{
  __m128 temp;
  shortestPath(a,b,temp);
  _mm_store_ss(&product,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::shortestPath(const XQuaternion &a,const XQuaternion &b,__m128& product)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  // perform dot product
  __m128 product = a.dotSSE(b);
  {
    const __m128 compare_gt = _mm_cmpgt_ps(_mm_setzero_ps(),product);

    product = _mm_add_ps( _mm_and_ps(compare_gt,product),
                          _mm_andnot_ps(compare_gt,_mm_sub_ps(_mm_setzero_ps(),product)) );

    sse     = _mm_add_ps( _mm_and_ps(compare_gt,b.sse),
                          _mm_andnot_ps(compare_gt,_mm_sub_ps(_mm_setzero_ps(),b.sse)) );
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::longestPath(const XQuaternion &a,const XQuaternion &b,XReal& product)
{
  __m128 temp;
  longestPath(a,b,temp);
  _mm_store_ss(&product,temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::longestPath(const XQuaternion &a,const XQuaternion &b,__m128& product)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  // perform dot product
  __m128 product = a.dotSSE(b);
  {
    const __m128 compare_lt = _mm_cmplt_ps(_mm_setzero_ps(),product);

    product = _mm_add_ps( _mm_and_ps(compare_lt,product),
                          _mm_andnot_ps(compare_lt,_mm_sub_ps(_mm_setzero_ps(),product)) );

    sse     = _mm_add_ps( _mm_and_ps(compare_lt,b.sse),
                          _mm_andnot_ps(compare_lt,_mm_sub_ps(_mm_setzero_ps(),b.sse)) );
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::slerp(const XQuaternion& a,const XQuaternion& b,const XReal t)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128 interpolating_t,inv_interpolating_t,theta,sinTheta,product;

  shortestPath(a,b,product);

  // calculate the linear t values for the interpolation
  interpolating_t = _mm_set_ps1(t);

  // spherical interpolation takes place around a sphere. Therefore we use the
  // product (remember dot product to finds angle between 2 vectors) to get the
  // angle we have to rotate to go from q1 to q2.
  //
  theta = XMacos(product);

  // pre-calculate the sin value
  //
  sinTheta = _mm_rcp_ps(XMsin(theta));

  /// \bug need to check for sinTheta being zero, otherwise 
  interpolating_t     = _mm_mul_ps(XMsin(_mm_mul_ps(_mm_set_ps1(     t),theta)) , sinTheta);
  inv_interpolating_t = _mm_mul_ps(XMsin(_mm_mul_ps(_mm_set_ps1(1.0f-t),theta)) , sinTheta);

  // perform the interpolation
  sse = _mm_add_ps(_mm_mul_ps(inv_interpolating_t,a.sse), 
                   _mm_mul_ps(interpolating_t,sse));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::lerp(const XQuaternion& a,const XQuaternion& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XReal product;
  shortestPath(a,b,product);
  // perform the interpolation
  sse = _mm_add_ps(a.sse, _mm_mul_ps(_mm_set_ps1(t),_mm_sub_ps(sse,a.sse)) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::invert(const XQuaternion& q)
{
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  /// \todo improve
  sse = _mm_mul_ps(q.sse,_mm_set_ps(1.0f,-1.0f,-1.0f,-1.0f));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::negate(const XQuaternion& q)
{
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  sse = _mm_sub_ps(_mm_setzero_ps(),q.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternion::normalise(const XQuaternion& q)
{
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  /// \todo improve
  const XReal l_inv = q.length();
  if(!float_equal(l_inv,0))
  {
    sse = _mm_div_ps(q.sse, _mm_set_ps1(l_inv));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternion& XQuaternion::operator += (const XQuaternion& c)
{
  sse = _mm_add_ps(sse,c.sse);
  return *this;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE const XQuaternion& XQuaternion::operator -= (const XQuaternion& c)
{
  sse = _mm_sub_ps(sse,c.sse);
  return *this;
}
}
#endif
