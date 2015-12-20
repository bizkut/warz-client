#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_QUATERNION_SOA__INL__
#define XM2_SSE_QUATERNION_SOA__INL__
#if (XM2_USE_FLOAT)
# error SSE2 double precision routines included when set to build using floats
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionSOA::XQuaternionSOA()
{
  XM2_ALIGN_ASSERT(this);
  #if XM2_INIT_CLASSES
  identity();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionSOA::XQuaternionSOA(const XQuaternionSOA& q)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&q);
  _mm_stream_pd(x  ,_mm_load_pd(q.x  ));
  _mm_stream_pd(x+2,_mm_load_pd(q.x+2));
  _mm_stream_pd(y  ,_mm_load_pd(q.y  ));
  _mm_stream_pd(y+2,_mm_load_pd(q.y+2));
  _mm_stream_pd(z  ,_mm_load_pd(q.z  ));
  _mm_stream_pd(z+2,_mm_load_pd(q.z+2));
  _mm_stream_pd(w  ,_mm_load_pd(q.w  ));
  _mm_stream_pd(w+2,_mm_load_pd(q.w+2));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionSOA::XQuaternionSOA(const XQuaternion quats[4])
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(quats);
  _x0 = _mm_shuffle_pd(quats[0].xy,quats[1].xy,_MM_SHUFFLE2(0,0));
  _x1 = _mm_shuffle_pd(quats[2].xy,quats[3].xy,_MM_SHUFFLE2(0,0));
  _y0 = _mm_shuffle_pd(quats[0].xy,quats[1].xy,_MM_SHUFFLE2(1,1));
  _y1 = _mm_shuffle_pd(quats[2].xy,quats[3].xy,_MM_SHUFFLE2(1,1));
  _z0 = _mm_shuffle_pd(quats[0].zw,quats[1].zw,_MM_SHUFFLE2(0,0));
  _z1 = _mm_shuffle_pd(quats[2].zw,quats[3].zw,_MM_SHUFFLE2(0,0));
  _w0 = _mm_shuffle_pd(quats[0].zw,quats[1].zw,_MM_SHUFFLE2(1,1));
  _w1 = _mm_shuffle_pd(quats[2].zw,quats[3].zw,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionSOA::XQuaternionSOA(const __m128d& x0_,const __m128d& x1_,
                                          const __m128d& y0_,const __m128d& y1_,
                                          const __m128d& z0_,const __m128d& z1_,
                                          const __m128d& w0_,const __m128d& w1_)
{
  _x0 = x0_; _x1 = x1_;
  _y0 = y0_; _y1 = y1_;
  _z0 = z0_; _z1 = z1_;
  _w0 = w0_; _w1 = w1_;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::streamIn(const XQuaternion* input)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  // transpose 4 in top left corner
  {
    _x0 = _mm_shuffle_pd(input[0].xy,input[1].xy,_MM_SHUFFLE2(0,0));
    _y0 = _mm_shuffle_pd(input[0].xy,input[1].xy,_MM_SHUFFLE2(1,1));
  }
  // transpose top right and bottom left corners, then assign on top of each other
  {
    _w0 = _mm_shuffle_pd(input[0].zw,input[1].zw,_MM_SHUFFLE2(1,1));
    _z0 = _mm_shuffle_pd(input[0].zw,input[1].zw,_MM_SHUFFLE2(0,0));
    _y1 = _mm_shuffle_pd(input[2].xy,input[3].xy,_MM_SHUFFLE2(1,1));
    _x1 = _mm_shuffle_pd(input[2].xy,input[3].xy,_MM_SHUFFLE2(0,0));
  }
  // transpose 4 in bottom right corner
  {
    _w1 = _mm_shuffle_pd(input[2].zw,input[3].zw,_MM_SHUFFLE2(1,1));
    _z1 = _mm_shuffle_pd(input[2].zw,input[3].zw,_MM_SHUFFLE2(0,0));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::streamOut(XQuaternion* input) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  // transpose 4 in top left corner
  {
    input[0].xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(0,0));
    input[1].xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(1,1));
  }
  // transpose top right and bottom left corners, then assign on top of each other
  {
    input[0].zw = _mm_shuffle_pd(_z0,_w0,_MM_SHUFFLE2(0,0)); // 9 13
    input[1].zw = _mm_shuffle_pd(_z0,_w0,_MM_SHUFFLE2(1,1)); // 10 14
    input[2].xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(0,0)); // 3 7
    input[3].xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(1,1));; // 4 8
  }
  // transpose 4 in bottom right corner
  {
    input[3].zw = _mm_shuffle_pd(_z1,_w1,_MM_SHUFFLE2(1,1));
    input[2].zw = _mm_shuffle_pd(_z1,_w1,_MM_SHUFFLE2(0,0));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromAxisAngleD(const XQuaternionSOA& axis)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&axis);
  const __m128d angle0 = degToRad(axis._w0);
  const __m128d angle1 = degToRad(axis._w1);
  const __m128d len0 = _mm_sqrt_pd( _mm_add_pd( _mm_mul_pd(axis._x0,axis._x0),
                                    _mm_add_pd( _mm_mul_pd(axis._y0,axis._y0),
                                                _mm_mul_pd(axis._z0,axis._z0))));
  const __m128d len1 = _mm_sqrt_pd( _mm_add_pd( _mm_mul_pd(axis._x1,axis._x1),
                                    _mm_add_pd( _mm_mul_pd(axis._y1,axis._y1),
                                                _mm_mul_pd(axis._z1,axis._z1))));

  const __m128d cos_half0 =  _mm_mul_pd(angle0,_mm_set1_pd(0.5f));
  const __m128d cos_half1 =  _mm_mul_pd(angle1,_mm_set1_pd(0.5f));
  const __m128d sin_a0 = XMsin( cos_half0 );
  const __m128d sin_a1 = XMsin( cos_half1 );
  _x0 = _mm_mul_pd(sin_a0,_mm_div_pd(axis._x0,len0));
  _x1 = _mm_mul_pd(sin_a1,_mm_div_pd(axis._x1,len1));
  _y0 = _mm_mul_pd(sin_a0,_mm_div_pd(axis._y0,len0));
  _y1 = _mm_mul_pd(sin_a1,_mm_div_pd(axis._y1,len1));
  _z0 = _mm_mul_pd(sin_a0,_mm_div_pd(axis._z0,len0));
  _z1 = _mm_mul_pd(sin_a1,_mm_div_pd(axis._z1,len1));
  _w0 = XMcos( cos_half0 );
  _w1 = XMcos( cos_half1 );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromAxisAngleR(const XQuaternionSOA& axis)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&axis);
  const __m128d len0 = _mm_sqrt_pd( _mm_add_pd( _mm_mul_pd(axis._x0,axis._x0),
                                    _mm_add_pd( _mm_mul_pd(axis._y0,axis._y0),
                                                _mm_mul_pd(axis._z0,axis._z0))));
  const __m128d len1 = _mm_sqrt_pd( _mm_add_pd( _mm_mul_pd(axis._x1,axis._x1),
                                    _mm_add_pd( _mm_mul_pd(axis._y1,axis._y1),
                                                _mm_mul_pd(axis._z1,axis._z1))));

  const __m128d cos_half0 =  _mm_mul_pd(axis._w0,_mm_set1_pd(0.5f));
  const __m128d cos_half1 =  _mm_mul_pd(axis._w1,_mm_set1_pd(0.5f));
  const __m128d sin_a0 = XMsin( cos_half0 );
  const __m128d sin_a1 = XMsin( cos_half1 );
  _x0 = _mm_mul_pd(sin_a0,_mm_div_pd(axis._x0,len0));
  _x1 = _mm_mul_pd(sin_a1,_mm_div_pd(axis._x1,len1));
  _y0 = _mm_mul_pd(sin_a0,_mm_div_pd(axis._y0,len0));
  _y1 = _mm_mul_pd(sin_a1,_mm_div_pd(axis._y1,len1));
  _z0 = _mm_mul_pd(sin_a0,_mm_div_pd(axis._z0,len0));
  _z1 = _mm_mul_pd(sin_a1,_mm_div_pd(axis._z1,len1));
  _w0 = XMcos( cos_half0 );
  _w1 = XMcos( cos_half1 );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toAxisAngleD(XQuaternionSOA& axis) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&axis);
  toAxisAngleR(axis);
  axis._w0 = radToDeg(axis._w0);
  axis._w1 = radToDeg(axis._w1);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toAxisAngleR(XQuaternionSOA& axis) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&axis);
  const __m128d cos_a0 = _w0;
  const __m128d cos_a1 = _w1;
  __m128d sin_a0 = _mm_sqrt_pd( _mm_sub_pd(_mm_set1_pd(1.0f), _mm_mul_pd(cos_a0,cos_a0)) );
  __m128d sin_a1 = _mm_sqrt_pd( _mm_sub_pd(_mm_set1_pd(1.0f), _mm_mul_pd(cos_a1,cos_a1)) );


  const __m128d cmp0 = _mm_cmpgt_pd(XMabs( sin_a0 ), _mm_set1_pd(XM2_FLOAT_ZERO) );
  const __m128d cmp1 = _mm_cmpgt_pd(XMabs( sin_a1 ), _mm_set1_pd(XM2_FLOAT_ZERO) );

  sin_a0 = _mm_or_pd( _mm_and_pd(cmp0,_mm_div_pd(_mm_set1_pd(1.0f),sin_a0)),
                      _mm_andnot_pd(cmp0,_mm_setzero_pd()) );
  sin_a1 = _mm_or_pd( _mm_and_pd(cmp1,_mm_div_pd(_mm_set1_pd(1.0f),sin_a1)),
                      _mm_andnot_pd(cmp1,_mm_setzero_pd()) );
  axis._x0 = _mm_mul_pd(_x0,sin_a0);
  axis._x1 = _mm_mul_pd(_x1,sin_a1);
  axis._y0 = _mm_mul_pd(_y0,sin_a0);
  axis._y1 = _mm_mul_pd(_y1,sin_a1);
  axis._z0 = _mm_mul_pd(_z0,sin_a0);
  axis._z1 = _mm_mul_pd(_z1,sin_a1);
  axis._w0 = _mm_mul_pd(XMacos(cos_a0),_mm_set1_pd(2.0f));
  axis._w1 = _mm_mul_pd(XMacos(cos_a1),_mm_set1_pd(2.0f));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toEulerAnglesD(XVector3SOA& eulers) const
{
  toEulerAnglesR(eulers);
  eulers._x0 = radToDeg(eulers._x0);
  eulers._x1 = radToDeg(eulers._x1);
  eulers._y0 = radToDeg(eulers._y0);
  eulers._y1 = radToDeg(eulers._y1);
  eulers._z0 = radToDeg(eulers._z0);
  eulers._z1 = radToDeg(eulers._z1);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toEulerAnglesR(XVector3SOA& eulers) const
{
  XVector4 Nq;
  length(Nq);

  //XReal s  = (Nq > 0.0f) ? (2.0f / Nq) : 0.0f;
  const __m128d cmp0 = _mm_cmpgt_pd(Nq.xy,_mm_setzero_pd());
  const __m128d cmp1 = _mm_cmpgt_pd(Nq.zw,_mm_setzero_pd());
  const __m128d s0   = _mm_or_pd(_mm_and_pd(cmp0,_mm_div_pd(_mm_set1_pd(2.0f),Nq.xy)),
                                 _mm_andnot_pd(cmp1,_mm_setzero_pd()));
  const __m128d s1   = _mm_or_pd(_mm_and_pd(cmp0,_mm_div_pd(_mm_set1_pd(2.0f),Nq.zw)),
                                 _mm_andnot_pd(cmp1,_mm_setzero_pd()));

  //XReal xs = x*s,     ys = y*s,    zs = z*s;
  //XReal wx = w*xs,    wy = w*ys,   wz = w*zs;
  //XReal xx = x*xs,    xy = x*ys,   xz = x*zs;
  //XReal yy = y*ys,    yz = y*zs,   zz = z*zs;
  const __m128d xs0 = _mm_mul_pd(_x0,s0);  const __m128d ys0 = _mm_mul_pd(_y0,s0);  const __m128d zs0 = _mm_mul_pd(_z0,s0);
  const __m128d wx0 = _mm_mul_pd(_w0,xs0); const __m128d wy0 = _mm_mul_pd(_w0,ys0); const __m128d wz0 = _mm_mul_pd(_w0,zs0);
  const __m128d xx0 = _mm_mul_pd(_x0,xs0); const __m128d xy0 = _mm_mul_pd(_x0,ys0); const __m128d xz0 = _mm_mul_pd(_x0,zs0);
  const __m128d yy0 = _mm_mul_pd(_y0,ys0); const __m128d yz0 = _mm_mul_pd(_y0,zs0); const __m128d zz0 = _mm_mul_pd(_z0,zs0);

  const __m128d xs1 = _mm_mul_pd(_x1,s1);  const __m128d ys1 = _mm_mul_pd(_y1,s1);  const __m128d zs1 = _mm_mul_pd(_z1,s1);
  const __m128d wx1 = _mm_mul_pd(_w1,xs1); const __m128d wy1 = _mm_mul_pd(_w1,ys1); const __m128d wz1 = _mm_mul_pd(_w1,zs1);
  const __m128d xx1 = _mm_mul_pd(_x1,xs1); const __m128d xy1 = _mm_mul_pd(_x1,ys1); const __m128d xz1 = _mm_mul_pd(_x1,zs1);
  const __m128d yy1 = _mm_mul_pd(_y1,ys1); const __m128d yz1 = _mm_mul_pd(_y1,zs1); const __m128d zz1 = _mm_mul_pd(_z1,zs1);

  //M.m00 = 1.0f - (yy + zz); M.m01 = xy - wz;          M.m02 = xz + wy;
  //M.m10 = xy + wz;          M.m11 = 1.0f - (xx + zz); M.m12 = yz - wx;
  //M.m20 = xz - wy;          M.m21 = yz + wx;          M.m22 = 1.0f - (xx + yy);
  const __m128d one = _mm_set1_pd(1.0f);
  const __m128d m00_0 = _mm_sub_pd(one,_mm_add_pd(yy0,zz0));
  const __m128d m01_0 = _mm_sub_pd(xy0,wz0);
  const __m128d m02_0 = _mm_add_pd(xz0,wy0);

  const __m128d m10_0 = _mm_add_pd(xy0,wz0);
  const __m128d m11_0 = _mm_sub_pd(one,_mm_add_pd(xx0,zz0));
  const __m128d m12_0 = _mm_sub_pd(yz0,wx0);

  const __m128d m20_0 = _mm_add_pd(xz0,wy0);
  const __m128d m21_0 = _mm_add_pd(yz0,wx0);
  const __m128d m22_0 = _mm_sub_pd(one,_mm_add_pd(xx0,yy0));

  const __m128d m00_1 = _mm_sub_pd(one,_mm_add_pd(yy1,zz1));
  const __m128d m01_1 = _mm_sub_pd(xy1,wz1);
  const __m128d m02_1 = _mm_add_pd(xz1,wy1);

  const __m128d m10_1 = _mm_add_pd(xy1,wz1);
  const __m128d m11_1 = _mm_sub_pd(one,_mm_add_pd(xx1,zz1));
  const __m128d m12_1 = _mm_sub_pd(yz1,wx1);

  const __m128d m20_1 = _mm_add_pd(xz1,wy1);
  const __m128d m21_1 = _mm_add_pd(yz1,wx1);
  const __m128d m22_1 = _mm_sub_pd(one,_mm_add_pd(xx1,yy1));

  // cy = XMsqrt( M.m00*M.m00 + M.m10*M.m10 );
  const __m128d cy0 = XMsqrt( _mm_add_pd( _mm_mul_pd(m00_0,m00_0), _mm_mul_pd(m10_0,m10_0) ) );
  const __m128d cy1 = XMsqrt( _mm_add_pd( _mm_mul_pd(m00_1,m00_1), _mm_mul_pd(m10_1,m10_1) ) );

  // if (cy > XM2_FLOAT_ZERO)
  // {
  //   _x = XMatan2( M.m21, M.m22 );
  //   _y = XMatan2(-M.m20, cy );
  //   _z = XMatan2( M.m10, M.m00 );
  // } 
  // else
  // {
  //   _x = XMatan2(-M.m12, M.m11 );
  //   _y = XMatan2(-M.m20, cy );
  //   _z = 0;
  // }
  const __m128d _xtrue0 = XMatan2( m21_0, m22_0 );
  const __m128d _ytrue0 = XMatan2( _mm_sub_pd(_mm_setzero_pd(),m20_0), cy0 );
  const __m128d _ztrue0 = XMatan2( m10_0, m00_0 );

  const __m128d _xtrue1 = XMatan2( m21_1, m22_1 );
  const __m128d _ytrue1 = XMatan2( _mm_sub_pd(_mm_setzero_pd(),m20_1), cy1 );
  const __m128d _ztrue1 = XMatan2( m10_1, m00_1 );

  const __m128d _xfalse0 = XMatan2( _mm_sub_pd(_mm_setzero_pd(),m12_0), m11_0 );
  const __m128d _yfalse0 = XMatan2( _mm_sub_pd(_mm_setzero_pd(),m20_0), cy0 );
  const __m128d _zfalse0 = _mm_setzero_pd();

  const __m128d _xfalse1 = XMatan2( _mm_sub_pd(_mm_setzero_pd(),m12_1), m11_1 );
  const __m128d _yfalse1 = XMatan2( _mm_sub_pd(_mm_setzero_pd(),m20_1), cy1 );
  const __m128d _zfalse1 = _mm_setzero_pd();

  const __m128d cmp20 = _mm_cmpgt_pd(cy0,_mm_set1_pd(XM2_FLOAT_ZERO));
  const __m128d cmp21 = _mm_cmpgt_pd(cy1,_mm_set1_pd(XM2_FLOAT_ZERO));
  eulers._x0 = _mm_or_pd(_mm_and_pd(cmp20,_xtrue0),
                         _mm_andnot_pd(cmp20,_xfalse0));
  eulers._y0 = _mm_or_pd(_mm_and_pd(cmp20,_ytrue0),
                         _mm_andnot_pd(cmp20,_yfalse0));
  eulers._z0 = _mm_or_pd(_mm_and_pd(cmp20,_ztrue0),
                         _mm_andnot_pd(cmp20,_zfalse0));
  eulers._x1 = _mm_or_pd(_mm_and_pd(cmp21,_xtrue1),
                         _mm_andnot_pd(cmp21,_xfalse1));
  eulers._y1 = _mm_or_pd(_mm_and_pd(cmp21,_ytrue1),
                         _mm_andnot_pd(cmp21,_yfalse1));
  eulers._z1 = _mm_or_pd(_mm_and_pd(cmp21,_ztrue1),
                         _mm_andnot_pd(cmp21,_zfalse1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromEulerAnglesD(const XVector3SOA& eulers)
{
  const __m128d _zero = _mm_setzero_pd(); 
  const __m128d _one  = _mm_set1_pd(1.0f); 
  XQuaternionSOA qx(_one,_one,_zero,_zero,_zero,_zero,degToRad(eulers._x0),degToRad(eulers._x1));
  XQuaternionSOA qy(_zero,_zero,_one,_one,_zero,_zero,degToRad(eulers._y0),degToRad(eulers._y1));
  XQuaternionSOA qz(_zero,_zero,_zero,_zero,_one,_one,degToRad(eulers._z0),degToRad(eulers._z1));
  XQuaternionSOA qt;
  qx.fromAxisAngleR(qx);
  qy.fromAxisAngleR(qy);
  qz.fromAxisAngleR(qz);
  qt.mul(qx,qy);
  mul(qt,qz);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromEulerAnglesR(const XVector3SOA& eulers)
{
  XM2_ALIGN_ASSERT(this);
  const __m128d _zero = _mm_setzero_pd(); 
  const __m128d _one  = _mm_set1_pd(1.0f); 
  XQuaternionSOA qx(_one,_one,_zero,_zero,_zero,_zero,eulers._x0,eulers._x1);
  XQuaternionSOA qy(_zero,_zero,_one,_one,_zero,_zero,eulers._y0,eulers._y1);
  XQuaternionSOA qz(_zero,_zero,_zero,_zero,_one,_one,eulers._z0,eulers._z1);
  XQuaternionSOA qt;
  qx.fromAxisAngleR(qx);
  qy.fromAxisAngleR(qy);
  qz.fromAxisAngleR(qz);
  qt.mul(qx,qy);
  mul(qt,qz);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::length(XVector4& len) const
{
  dot(len,*this);
  len.xy = _mm_sqrt_pd(len.xy);
  len.zw = _mm_sqrt_pd(len.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set0(const XQuaternion& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x0 = _mm_shuffle_pd(vec.xy,_x0,_MM_SHUFFLE2(1,0));
  _y0 = _mm_shuffle_pd(vec.xy,_y0,_MM_SHUFFLE2(1,1));
  _z0 = _mm_shuffle_pd(vec.zw,_z0,_MM_SHUFFLE2(1,0));
  _w0 = _mm_shuffle_pd(vec.zw,_w0,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set1(const XQuaternion& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x0 = _mm_shuffle_pd(_x0,vec.xy,_MM_SHUFFLE2(0,0));
  _y0 = _mm_shuffle_pd(_y0,vec.xy,_MM_SHUFFLE2(1,0));
  _z0 = _mm_shuffle_pd(_z0,vec.zw,_MM_SHUFFLE2(0,0));
  _w0 = _mm_shuffle_pd(_w0,vec.zw,_MM_SHUFFLE2(1,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set2(const XQuaternion& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x1 = _mm_shuffle_pd(vec.xy,_x1,_MM_SHUFFLE2(1,0));
  _y1 = _mm_shuffle_pd(vec.xy,_y1,_MM_SHUFFLE2(1,1));
  _z1 = _mm_shuffle_pd(vec.zw,_z1,_MM_SHUFFLE2(1,0));
  _w1 = _mm_shuffle_pd(vec.zw,_w1,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set3(const XQuaternion& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x1 = _mm_shuffle_pd(_x1,vec.xy,_MM_SHUFFLE2(0,0));
  _y1 = _mm_shuffle_pd(_y1,vec.xy,_MM_SHUFFLE2(1,0));
  _z1 = _mm_shuffle_pd(_z1,vec.zw,_MM_SHUFFLE2(0,0));
  _w1 = _mm_shuffle_pd(_w1,vec.zw,_MM_SHUFFLE2(1,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get0(XQuaternion& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  vec.xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(0,0));
  vec.zw = _mm_shuffle_pd(_z0,_w0,_MM_SHUFFLE2(0,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get1(XQuaternion& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  vec.xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(1,1));
  vec.zw = _mm_shuffle_pd(_z0,_w0,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get2(XQuaternion& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  vec.xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(0,0));
  vec.zw = _mm_shuffle_pd(_z1,_w1,_MM_SHUFFLE2(0,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get3(XQuaternion& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  vec.xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(1,1));
  vec.zw = _mm_shuffle_pd(_z1,_w1,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::identity()
{
  XM2_ALIGN_ASSERT(this);
  _mm_stream_pd(x  ,_mm_setzero_pd());
  _mm_stream_pd(x+2,_mm_setzero_pd());
  _mm_stream_pd(y  ,_mm_setzero_pd());
  _mm_stream_pd(y+2,_mm_setzero_pd());
  _mm_stream_pd(z  ,_mm_setzero_pd());
  _mm_stream_pd(z+2,_mm_setzero_pd());
  _mm_stream_pd(w  ,_mm_set1_pd(1.0f));
  _mm_stream_pd(w+2,_mm_set1_pd(1.0f));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::copy(const XQuaternionSOA& q)
{
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  _mm_stream_pd(x  ,_mm_load_pd(q.x  ));
  _mm_stream_pd(x+2,_mm_load_pd(q.x+2));
  _mm_stream_pd(y  ,_mm_load_pd(q.y  ));
  _mm_stream_pd(y+2,_mm_load_pd(q.y+2));
  _mm_stream_pd(z  ,_mm_load_pd(q.z  ));
  _mm_stream_pd(z+2,_mm_load_pd(q.z+2));
  _mm_stream_pd(w  ,_mm_load_pd(q.w  ));
  _mm_stream_pd(w+2,_mm_load_pd(q.w+2));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternionSOA::equal(const XQuaternionSOA& b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  /// \todo use eps 
  __m128d a0 =_mm_and_pd(_mm_and_pd(_mm_cmpeq_pd(_x0,b._x0),_mm_cmpeq_pd(_y0,b._y0)),
                         _mm_and_pd(_mm_cmpeq_pd(_z0,b._z0),_mm_cmpeq_pd(_w0,b._w0)));
  __m128d a1 =_mm_and_pd(_mm_and_pd(_mm_cmpeq_pd(_x1,b._x1),_mm_cmpeq_pd(_y1,b._y1)),
                         _mm_and_pd(_mm_cmpeq_pd(_z1,b._z1),_mm_cmpeq_pd(_w1,b._w1)));
  return _mm_movemask_pd(_mm_and_pd(a0,a1))==3;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::dot(XVector4& product,const XQuaternionSOA& b) const
{
  XM2_ALIGN_ASSERT(&product);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  product.xy  = _mm_add_pd(_mm_add_pd(_mm_mul_pd(_x0,b._x0),_mm_mul_pd(_y0,b._y0)),
                           _mm_add_pd(_mm_mul_pd(_z0,b._z0),_mm_mul_pd(_w0,b._w0)));
  product.zw  = _mm_add_pd(_mm_add_pd(_mm_mul_pd(_x1,b._x1),_mm_mul_pd(_y1,b._y1)),
                           _mm_add_pd(_mm_mul_pd(_z1,b._z1),_mm_mul_pd(_w1,b._w1)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::add(const XQuaternionSOA& a,const XQuaternionSOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_add_pd(a._x0,b._x0);
  _x1 = _mm_add_pd(a._x1,b._x1);
  _y0 = _mm_add_pd(a._y0,b._y0);
  _y1 = _mm_add_pd(a._y1,b._y1);
  _z0 = _mm_add_pd(a._z0,b._z0);
  _z1 = _mm_add_pd(a._z1,b._z1);
  _w0 = _mm_add_pd(a._w0,b._w0);
  _w1 = _mm_add_pd(a._w1,b._w1);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::add(const XQuaternionSOA& a,const XQuaternion& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128d _temp = _mm_shuffle_pd(b.xy,b.xy,_MM_SHUFFLE2(0,0));
  _x0 = _mm_add_pd(a._x1,_temp);
  _x1 = _mm_add_pd(a._x0,_temp);
  _temp = _mm_shuffle_pd(b.xy,b.xy,_MM_SHUFFLE2(1,1));
  _y0 = _mm_add_pd(a._y1,_temp);
  _y1 = _mm_add_pd(a._y0,_temp);
  _temp = _mm_shuffle_pd(b.zw,b.zw,_MM_SHUFFLE2(0,0));
  _z0 = _mm_add_pd(a._z1,_temp);
  _z1 = _mm_add_pd(a._z0,_temp);
  _temp = _mm_shuffle_pd(b.zw,b.zw,_MM_SHUFFLE2(1,1));
  _w0 = _mm_add_pd(a._w1,_temp);
  _w1 = _mm_add_pd(a._w0,_temp);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const XQuaternionSOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_add_pd(_mm_add_pd(_mm_mul_pd(a._w0,b._x0),_mm_mul_pd(a._x0,b._w0)), 
                   _mm_sub_pd(_mm_mul_pd(b._y0,a._z0),_mm_mul_pd(b._z0,a._y0)));
  _x1 = _mm_add_pd(_mm_add_pd(_mm_mul_pd(a._w1,b._x1),_mm_mul_pd(a._x1,b._w1)), 
                   _mm_sub_pd(_mm_mul_pd(b._y1,a._z1),_mm_mul_pd(b._z1,a._y1)));
  _y0 = _mm_add_pd(_mm_add_pd(_mm_mul_pd(a._w0,b._x0),_mm_mul_pd(a._y0,b._w0)), 
                   _mm_sub_pd(_mm_mul_pd(b._z0,a._x0),_mm_mul_pd(b._x0,a._z0)));
  _y1 = _mm_add_pd(_mm_add_pd(_mm_mul_pd(a._w1,b._x1),_mm_mul_pd(a._y1,b._w1)), 
                   _mm_sub_pd(_mm_mul_pd(b._z1,a._x1),_mm_mul_pd(b._x1,a._z1)));
  _z0 = _mm_add_pd(_mm_add_pd(_mm_mul_pd(a._w0,b._y0),_mm_mul_pd(a._z0,b._w0)), 
                   _mm_sub_pd(_mm_mul_pd(b._x0,a._y0),_mm_mul_pd(b._y0,a._x0)));
  _z1 = _mm_add_pd(_mm_add_pd(_mm_mul_pd(a._w1,b._y1),_mm_mul_pd(a._z1,b._w1)), 
                   _mm_sub_pd(_mm_mul_pd(b._x1,a._y1),_mm_mul_pd(b._y1,a._x1)));
  _w0 = _mm_add_pd(_mm_add_pd(_mm_mul_pd(a._w0,b._z0),_mm_mul_pd(a._x0,b._x0)), 
                   _mm_sub_pd(_mm_mul_pd(b._y0,a._y0),_mm_mul_pd(b._z0,a._x0)));
  _w1 = _mm_add_pd(_mm_add_pd(_mm_mul_pd(a._w1,b._z1),_mm_mul_pd(a._x1,b._x1)), 
                   _mm_sub_pd(_mm_mul_pd(b._y1,a._y1),_mm_mul_pd(b._z1,a._x1)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const XQuaternion& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XQuaternionSOA _b;
  _b._x1 = _b._x0 = _mm_shuffle_pd(b.xy,b.xy,_MM_SHUFFLE2(0,0));
  _b._y1 = _b._y0 = _mm_shuffle_pd(b.xy,b.xy,_MM_SHUFFLE2(1,1));
  _b._z1 = _b._z0 = _mm_shuffle_pd(b.zw,b.zw,_MM_SHUFFLE2(0,0));
  _b._w1 = _b._w0 = _mm_shuffle_pd(b.zw,b.zw,_MM_SHUFFLE2(1,1));
  mul(a,_b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const __m128d b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_mul_pd(b,a._x0);
  _x1 = _mm_mul_pd(b,a._x1);
  _y0 = _mm_mul_pd(b,a._y0);
  _y1 = _mm_mul_pd(b,a._y1);
  _z0 = _mm_mul_pd(b,a._z0);
  _z1 = _mm_mul_pd(b,a._z1);
  _w0 = _mm_mul_pd(b,a._w0);
  _w1 = _mm_mul_pd(b,a._w1);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const XReal b)
{
  mul(a,_mm_set1_pd(b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternion& b)
{
  XQuaternionSOA temp(*this);
  mul(temp,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const __m128d b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XReal b)
{
  mul(*this,_mm_set1_pd(b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::slerp(const XQuaternionSOA& a,const XQuaternionSOA& b,const XReal t)
{
  slerp(a,b,_mm_set1_pd(t),_mm_set1_pd(t));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::slerp(const XQuaternionSOA& a,const XQuaternionSOA& b,
                                      const __m128d t0,const __m128d t1)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  // perform dot product to see if the quats need to be inverted
  XVector4 product;
  a.dot(product,b);

  // if(product < 0)
  //
  //  + a few variables to help select which ones to invert
  // 
  __m128d inv_product0 = _mm_sub_pd(_mm_setzero_pd(),product.xy);
  __m128d inv_product1 = _mm_sub_pd(_mm_setzero_pd(),product.zw);
  __m128d less_than_zero0 = _mm_cmple_pd(_mm_setzero_pd(),product.xy);
  __m128d less_than_zero1 = _mm_cmple_pd(_mm_setzero_pd(),product.zw);
  __m128d greater_than_zero0 = _mm_cmpgt_pd(_mm_setzero_pd(),product.xy);
  __m128d greater_than_zero1 = _mm_cmpgt_pd(_mm_setzero_pd(),product.zw);

  // product = -product    (only if product is less than 0)
  product.xy = _mm_and_pd(_mm_and_pd(inv_product0,less_than_zero0),
                          _mm_and_pd(product.xy,greater_than_zero0));
  product.zw = _mm_and_pd(_mm_and_pd(inv_product1,less_than_zero1),
                          _mm_and_pd(product.zw,greater_than_zero1));

  // negate the quats that need to be negated
  __m128d _bx0 = _mm_and_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._x0),less_than_zero0),
                            _mm_and_pd(b._x0,greater_than_zero0));
  __m128d _bx1 = _mm_and_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._x1),less_than_zero1),
                            _mm_and_pd(b._x1,greater_than_zero1));
  
  __m128d _by0 = _mm_and_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._y0),less_than_zero0),
                            _mm_and_pd(b._y0,greater_than_zero0));
  __m128d _by1 = _mm_and_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._y1),less_than_zero1),
                            _mm_and_pd(b._y1,greater_than_zero1));

  __m128d _bz0 = _mm_and_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._z0),less_than_zero0),
                            _mm_and_pd(b._z0,greater_than_zero0));
  __m128d _bz1 = _mm_and_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._z1),less_than_zero1),
                            _mm_and_pd(b._z1,greater_than_zero1));
  
  __m128d _bw0 = _mm_and_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._w0),less_than_zero0),
                            _mm_and_pd(b._w0,greater_than_zero0));
  __m128d _bw1 = _mm_and_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._w1),less_than_zero1),
                            _mm_and_pd(b._w1,greater_than_zero1));

  // our t values
  __m128d _inv_t0 = _mm_sub_pd(_mm_setzero_pd(),t0);
  __m128d _inv_t1 = _mm_sub_pd(_mm_setzero_pd(),t1);

  // compute sin theta 
  __m128d theta0    = XMacos(product.xy);
  __m128d theta1    = XMacos(product.zw);
  __m128d sinTheta0 = _mm_div_pd(_mm_set1_pd(1.0),XMsin(theta0));
  __m128d sinTheta1 = _mm_div_pd(_mm_set1_pd(1.0),XMsin(theta1));

  // calc interpolation values
  _inv_t1     = _mm_mul_pd(XMsin(_mm_mul_pd(_inv_t0, theta0)) , sinTheta0);
  _inv_t0     = _mm_mul_pd(XMsin(_mm_mul_pd(_inv_t1, theta1)) , sinTheta1);
  __m128d _t0 = _mm_mul_pd(XMsin(_mm_mul_pd(t0     , theta0)) , sinTheta0);
  __m128d _t1 = _mm_mul_pd(XMsin(_mm_mul_pd(t1     , theta1)) , sinTheta1);

  // perform the interpolation
  _x0 = _mm_add_pd(_mm_mul_pd(a._x0,_inv_t0),_mm_mul_pd(_bx0,_t0));
  _x1 = _mm_add_pd(_mm_mul_pd(a._x1,_inv_t1),_mm_mul_pd(_bx1,_t1));
  _y0 = _mm_add_pd(_mm_mul_pd(a._y0,_inv_t0),_mm_mul_pd(_by0,_t0));
  _y1 = _mm_add_pd(_mm_mul_pd(a._y1,_inv_t1),_mm_mul_pd(_by1,_t1));
  _z0 = _mm_add_pd(_mm_mul_pd(a._z0,_inv_t0),_mm_mul_pd(_bz0,_t0));
  _z1 = _mm_add_pd(_mm_mul_pd(a._z1,_inv_t1),_mm_mul_pd(_bz1,_t1));
  _w0 = _mm_add_pd(_mm_mul_pd(a._w0,_inv_t0),_mm_mul_pd(_bw0,_t0));
  _w1 = _mm_add_pd(_mm_mul_pd(a._w1,_inv_t1),_mm_mul_pd(_bw1,_t1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::lerp(const XQuaternionSOA& a,const XQuaternionSOA& b,const XReal t)
{
  lerp(a,b,_mm_set1_pd(t));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::shortestPath(const XQuaternionSOA& a,const XQuaternionSOA& b,XVector4& product)
{
  a.dot(product,b);

  // if(product < 0)
  //
  //  + a few variables to help select which ones to invert
  // 
  __m128d inv_product0 = _mm_sub_pd(_mm_setzero_pd(),product.xy);
  __m128d inv_product1 = _mm_sub_pd(_mm_setzero_pd(),product.zw);
  __m128d less_than_zero0 = _mm_cmple_pd(_mm_setzero_pd(),product.xy);
  __m128d less_than_zero1 = _mm_cmple_pd(_mm_setzero_pd(),product.zw);

  // product = -product    (only if product is less than 0)
  product.xy = _mm_and_pd(_mm_and_pd(inv_product0,less_than_zero0),
                          _mm_andnot_pd(less_than_zero0,product.xy));
  product.zw = _mm_and_pd(_mm_and_pd(inv_product1,less_than_zero1),
                          _mm_andnot_pd(less_than_zero0,product.zw));

  // negate the quats that need to be negated
  _x0 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._x0),less_than_zero0),
                   _mm_andnot_pd(less_than_zero0,b._x0));
  _x1 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._x1),less_than_zero1),
                   _mm_andnot_pd(less_than_zero1,b._x1));
  _y0 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._y0),less_than_zero0),
                   _mm_andnot_pd(less_than_zero0,b._y0));
  _y1 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._y1),less_than_zero1),
                   _mm_andnot_pd(less_than_zero1,b._y1));
  _z0 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._z0),less_than_zero0),
                   _mm_andnot_pd(less_than_zero0,b._z0));
  _z1 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._z1),less_than_zero1),
                   _mm_andnot_pd(less_than_zero1,b._z1));
  _w0 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._w0),less_than_zero0),
                   _mm_andnot_pd(less_than_zero0,b._w0));
  _w1 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._w1),less_than_zero1),
                   _mm_andnot_pd(less_than_zero1,b._w1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::longestPath(const XQuaternionSOA& a,const XQuaternionSOA& b,XVector4& product)
{
  a.dot(product,b);

  // if(product < 0)
  //
  //  + a few variables to help select which ones to invert
  // 
  __m128d inv_product0 = _mm_sub_pd(_mm_setzero_pd(),product.xy);
  __m128d inv_product1 = _mm_sub_pd(_mm_setzero_pd(),product.zw);
  __m128d less_than_zero0 = _mm_cmpge_pd(_mm_setzero_pd(),product.xy);
  __m128d less_than_zero1 = _mm_cmpge_pd(_mm_setzero_pd(),product.zw);

  // product = -product    (only if product is less than 0)
  product.xy = _mm_and_pd(_mm_and_pd(inv_product0,less_than_zero0),
                          _mm_andnot_pd(less_than_zero0,product.xy));
  product.zw = _mm_and_pd(_mm_and_pd(inv_product1,less_than_zero1),
                          _mm_andnot_pd(less_than_zero0,product.zw));

  // negate the quats that need to be negated
  _x0 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._x0),less_than_zero0),
                   _mm_andnot_pd(less_than_zero0,b._x0));
  _x1 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._x1),less_than_zero1),
                   _mm_andnot_pd(less_than_zero1,b._x1));
  _y0 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._y0),less_than_zero0),
                   _mm_andnot_pd(less_than_zero0,b._y0));
  _y1 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._y1),less_than_zero1),
                   _mm_andnot_pd(less_than_zero1,b._y1));
  _z0 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._z0),less_than_zero0),
                   _mm_andnot_pd(less_than_zero0,b._z0));
  _z1 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._z1),less_than_zero1),
                   _mm_andnot_pd(less_than_zero1,b._z1));
  _w0 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._w0),less_than_zero0),
                   _mm_andnot_pd(less_than_zero0,b._w0));
  _w1 = _mm_add_pd(_mm_and_pd(_mm_sub_pd(_mm_setzero_pd(),b._w1),less_than_zero1),
                   _mm_andnot_pd(less_than_zero1,b._w1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::lerp(const XQuaternionSOA& a,const XQuaternionSOA& b,
                                      const __m128d t0,const __m128d t1)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  // perform dot product to see if the quats need to be inverted
  XVector4 product;
  shortestPath(a,b,product);
  _x0 = _mm_add_pd(a._x0, _mm_mul_pd(t0, _mm_sub_pd(_x0,a._x0) ) );
  _x1 = _mm_add_pd(a._x1, _mm_mul_pd(t1, _mm_sub_pd(_x1,a._x1) ) );
  _y0 = _mm_add_pd(a._y0, _mm_mul_pd(t0, _mm_sub_pd(_y0,a._y0) ) );
  _y1 = _mm_add_pd(a._y1, _mm_mul_pd(t1, _mm_sub_pd(_y1,a._y1) ) );
  _z0 = _mm_add_pd(a._z0, _mm_mul_pd(t0, _mm_sub_pd(_z0,a._z0) ) );
  _z1 = _mm_add_pd(a._z1, _mm_mul_pd(t1, _mm_sub_pd(_z1,a._z1) ) );
  _w0 = _mm_add_pd(a._w0, _mm_mul_pd(t0, _mm_sub_pd(_w0,a._w0) ) );
  _w1 = _mm_add_pd(a._w1, _mm_mul_pd(t1, _mm_sub_pd(_w1,a._w1) ) );
  normalise();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::invert()
{
  invert(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::invert(const XQuaternionSOA& q)
{
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_sub_pd(_mm_setzero_pd(),q._x0);
  _x1 = _mm_sub_pd(_mm_setzero_pd(),q._x1);
  _y0 = _mm_sub_pd(_mm_setzero_pd(),q._y0);
  _y1 = _mm_sub_pd(_mm_setzero_pd(),q._y1);
  _z0 = _mm_sub_pd(_mm_setzero_pd(),q._z0);
  _z1 = _mm_sub_pd(_mm_setzero_pd(),q._z1);
  _w0 = q._w0;
  _w1 = q._w1;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::negate()
{
  negate(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::negate(const XQuaternionSOA& q)
{
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_sub_pd(_mm_setzero_pd(),q._x0);
  _x1 = _mm_sub_pd(_mm_setzero_pd(),q._x1);
  _y0 = _mm_sub_pd(_mm_setzero_pd(),q._y0);
  _y1 = _mm_sub_pd(_mm_setzero_pd(),q._y1);
  _z0 = _mm_sub_pd(_mm_setzero_pd(),q._z0);
  _z1 = _mm_sub_pd(_mm_setzero_pd(),q._z1);
  _w0 = _mm_sub_pd(_mm_setzero_pd(),q._w0);
  _w1 = _mm_sub_pd(_mm_setzero_pd(),q._w1);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::normalise()
{
  normalise(*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::normalise(const XQuaternionSOA& q)
{
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  XVector4 len;
  q.length(len);
  __m128d inv_len0 = _mm_div_pd(_mm_set1_pd(1.0),len.xy);
  __m128d inv_len1 = _mm_div_pd(_mm_set1_pd(1.0),len.zw);
  _x0 = _mm_mul_pd(inv_len0,q._x0);
  _x1 = _mm_mul_pd(inv_len1,q._x1);
  _y0 = _mm_mul_pd(inv_len0,q._y0);
  _y1 = _mm_mul_pd(inv_len1,q._y1);
  _z0 = _mm_mul_pd(inv_len0,q._z0);
  _z1 = _mm_mul_pd(inv_len1,q._z1);
  _w0 = _mm_mul_pd(inv_len0,q._w0);
  _w1 = _mm_mul_pd(inv_len1,q._w1);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::sum(const XQuaternionSOA& a,const XReal t)
{
  sum(a,_mm_set1_pd(t));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::sum(const XQuaternionSOA& a,const __m128d t0,const __m128d t1)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_add_pd(_x0,_mm_mul_pd(t0,a._x0));
  _x1 = _mm_add_pd(_x1,_mm_mul_pd(t1,a._x1));
  _y0 = _mm_add_pd(_y0,_mm_mul_pd(t0,a._y0));
  _y1 = _mm_add_pd(_y1,_mm_mul_pd(t1,a._y1));
  _z0 = _mm_add_pd(_z0,_mm_mul_pd(t0,a._z0));
  _z1 = _mm_add_pd(_z1,_mm_mul_pd(t1,a._z1));
  _w0 = _mm_add_pd(_w0,_mm_mul_pd(t0,a._w0));
  _w1 = _mm_add_pd(_w1,_mm_mul_pd(t1,a._w1));
}
#pragma warning(pop)
}
#endif
