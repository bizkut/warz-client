#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_QUATERNION_SOA__INL__
#define XM2_SSE_QUATERNION_SOA__INL__
#if (!XM2_USE_FLOAT)
# error SSE float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
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
  _x = q._x;
  _y = q._y;
  _z = q._z;
  _w = q._w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionSOA::XQuaternionSOA(const __m128& x_,
                                          const __m128& y_,
                                          const __m128& z_,
                                          const __m128& w_)
{
  XM2_ALIGN_ASSERT(this);
  _x = x_;
  _y = y_;
  _z = z_;
  _w = w_;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::streamIn(const XQuaternion* input)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  const __m128 tmp0 = _mm_shuffle_ps(input[0].sse, input[1].sse, 0x44);
  const __m128 tmp2 = _mm_shuffle_ps(input[0].sse, input[1].sse, 0xEE);
  const __m128 tmp1 = _mm_shuffle_ps(input[2].sse, input[3].sse, 0x44);
  const __m128 tmp3 = _mm_shuffle_ps(input[2].sse, input[3].sse, 0xEE);
  _x = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  _y = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  _z = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  _w = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::streamOut(XQuaternion* output) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(output);
  const __m128 tmp0 = _mm_shuffle_ps(_x, _y, 0x44);
  const __m128 tmp2 = _mm_shuffle_ps(_x, _y, 0xEE);
  const __m128 tmp1 = _mm_shuffle_ps(_z, _w, 0x44);
  const __m128 tmp3 = _mm_shuffle_ps(_z, _w, 0xEE);
  output[0].sse = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  output[1].sse = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  output[2].sse = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  output[3].sse = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XQuaternionSOA::XQuaternionSOA(const XQuaternion quats[4])
{
  streamIn(quats);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromAxisAngleD(const XQuaternionSOA& axis)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&axis);
  const __m128 angle = degToRad(axis._w);
  const __m128 len = _mm_sqrt_ps( _mm_add_ps( _mm_mul_ps(axis._x,axis._x),
                                  _mm_add_ps( _mm_mul_ps(axis._y,axis._y),
                                              _mm_mul_ps(axis._z,axis._z))));
  const __m128 cos_half =  _mm_mul_ps(angle,_mm_set_ps1(0.5f));
  const __m128 sin_a = XMsin( cos_half );
  _x = _mm_mul_ps(sin_a,_mm_div_ps(axis._x,len));
  _y = _mm_mul_ps(sin_a,_mm_div_ps(axis._y,len));
  _z = _mm_mul_ps(sin_a,_mm_div_ps(axis._z,len));
  _w = XMcos( cos_half );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromAxisAngleR(const XQuaternionSOA& axis)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&axis);
  const __m128 len = _mm_sqrt_ps( _mm_add_ps( _mm_mul_ps(axis._x,axis._x),
                                  _mm_add_ps( _mm_mul_ps(axis._y,axis._y),
                                              _mm_mul_ps(axis._z,axis._z))));
  const __m128 cos_half =  _mm_mul_ps(axis._w,_mm_set_ps1(0.5f));
  const __m128 sin_a = XMsin( cos_half );
  _w = XMcos( cos_half );
  _x = _mm_mul_ps(sin_a,_mm_div_ps(axis._x,len));
  _y = _mm_mul_ps(sin_a,_mm_div_ps(axis._y,len));
  _z = _mm_mul_ps(sin_a,_mm_div_ps(axis._z,len));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toAxisAngleD(XQuaternionSOA& axis) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&axis);
  toAxisAngleR(axis);
  axis._w = radToDeg(axis._w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toAxisAngleR(XQuaternionSOA& axis) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&axis);
  const __m128 cos_a = _w;
  __m128 sin_a = _mm_sqrt_ps( _mm_sub_ps(_mm_set_ps1(1.0f), _mm_mul_ps(cos_a,cos_a)) );


  const __m128 cmp = _mm_cmpgt_ps(XMabs( sin_a ), _mm_set_ps1(XM2_FLOAT_ZERO) );

  sin_a = _mm_or_ps( _mm_and_ps(cmp,_mm_div_ps(_mm_set_ps1(1.0f),sin_a)),
                     _mm_andnot_ps(cmp,_mm_setzero_ps()) );
  axis._x = _mm_mul_ps(_x,sin_a);
  axis._y = _mm_mul_ps(_y,sin_a);
  axis._z = _mm_mul_ps(_z,sin_a);
  axis._w = _mm_mul_ps(XMacos(cos_a),_mm_set_ps1(2.0f));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toEulerAnglesD(XVector3SOA& eulers) const
{
  toEulerAnglesR(eulers);
  eulers._x = radToDeg(eulers._x);
  eulers._y = radToDeg(eulers._y);
  eulers._z = radToDeg(eulers._z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::toEulerAnglesR(XVector3SOA& eulers) const
{
  XVector4 Nq;
  length(Nq);

  //XReal s  = (Nq > 0.0f) ? (2.0f / Nq) : 0.0f;
  const __m128 cmp = _mm_cmpgt_ps(Nq.sse,_mm_setzero_ps());
  const __m128 s  = _mm_or_ps(_mm_and_ps(cmp,_mm_div_ps(_mm_set_ps1(2.0f),Nq.sse)),
                              _mm_andnot_ps(cmp,_mm_setzero_ps()));

  //XReal xs = x*s,     ys = y*s,    zs = z*s;
  //XReal wx = w*xs,    wy = w*ys,   wz = w*zs;
  //XReal xx = x*xs,    xy = x*ys,   xz = x*zs;
  //XReal yy = y*ys,    yz = y*zs,   zz = z*zs;
  const __m128 xs = _mm_mul_ps(_x,s);  const __m128 ys = _mm_mul_ps(_y,s);  const __m128 zs = _mm_mul_ps(_z,s);
  const __m128 wx = _mm_mul_ps(_w,xs); const __m128 wy = _mm_mul_ps(_w,ys); const __m128 wz = _mm_mul_ps(_w,zs);
  const __m128 xx = _mm_mul_ps(_x,xs); const __m128 xy = _mm_mul_ps(_x,ys); const __m128 xz = _mm_mul_ps(_x,zs);
  const __m128 yy = _mm_mul_ps(_y,ys); const __m128 yz = _mm_mul_ps(_y,zs); const __m128 zz = _mm_mul_ps(_z,zs);

  //M.m00 = 1.0f - (yy + zz); M.m01 = xy - wz;          M.m02 = xz + wy;
  //M.m10 = xy + wz;          M.m11 = 1.0f - (xx + zz); M.m12 = yz - wx;
  //M.m20 = xz - wy;          M.m21 = yz + wx;          M.m22 = 1.0f - (xx + yy);
  const __m128 one = _mm_set_ps1(1.0f);
  const __m128 m00 = _mm_sub_ps(one,_mm_add_ps(yy,zz));
  const __m128 m01 = _mm_sub_ps(xy,wz);
  const __m128 m02 = _mm_add_ps(xz,wy);

  const __m128 m10 = _mm_add_ps(xy,wz);
  const __m128 m11 = _mm_sub_ps(one,_mm_add_ps(xx,zz));
  const __m128 m12 = _mm_sub_ps(yz,wx);

  const __m128 m20 = _mm_add_ps(xz,wy);
  const __m128 m21 = _mm_add_ps(yz,wx);
  const __m128 m22 = _mm_sub_ps(one,_mm_add_ps(xx,yy));

  // cy = XMsqrt( M.m00*M.m00 + M.m10*M.m10 );
  const __m128 cy = XMsqrt( _mm_add_ps( _mm_mul_ps(m00,m00), _mm_mul_ps(m10,m10) ) );

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
  const __m128 _xtrue = XMatan2( m21, m22 );
  const __m128 _ytrue = XMatan2( _mm_sub_ps(_mm_setzero_ps(),m20), cy );
  const __m128 _ztrue = XMatan2( m10, m00 );

  const __m128 _xfalse = XMatan2( _mm_sub_ps(_mm_setzero_ps(),m12), m11 );
  const __m128 _yfalse = XMatan2( _mm_sub_ps(_mm_setzero_ps(),m20), cy );
  const __m128 _zfalse = _mm_setzero_ps();

  const __m128 cmp2 = _mm_cmpgt_ps(cy,_mm_set_ps1(XM2_FLOAT_ZERO));
  eulers._x = _mm_or_ps(_mm_and_ps(cmp2,_xtrue),
                        _mm_andnot_ps(cmp2,_xfalse));
  eulers._y = _mm_or_ps(_mm_and_ps(cmp2,_ytrue),
                        _mm_andnot_ps(cmp2,_yfalse));
  eulers._z = _mm_or_ps(_mm_and_ps(cmp2,_ztrue),
                        _mm_andnot_ps(cmp2,_zfalse));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::fromEulerAnglesD(const XVector3SOA& eulers)
{
  const __m128 _zero = _mm_setzero_ps(); 
  const __m128 _one  = _mm_set_ps1(1.0f); 
  XQuaternionSOA qx(_one,_zero,_zero,degToRad(eulers._x));
  XQuaternionSOA qy(_zero,_one,_zero,degToRad(eulers._y));
  XQuaternionSOA qz(_zero,_zero,_one,degToRad(eulers._z));
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
  const __m128 _zero = _mm_setzero_ps(); 
  const __m128 _one  = _mm_set_ps1(1.0f); 
  XQuaternionSOA qx(_one,_zero,_zero,eulers._x);
  XQuaternionSOA qy(_zero,_one,_zero,eulers._y);
  XQuaternionSOA qz(_zero,_zero,_one,eulers._z);
  XQuaternionSOA qt;
  qx.fromAxisAngleR(qx);
  qy.fromAxisAngleR(qy);
  qz.fromAxisAngleR(qz);
  qt.mul(qx,qy);
  mul(qt,qz);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set0(const XQuaternion& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  const __m128 temp_x = _mm_shuffle_ps(_x,vec.sse,_MM_SHUFFLE(0,0,1,0));
  const __m128 temp_y = _mm_shuffle_ps(_y,vec.sse,_MM_SHUFFLE(1,1,1,0));
  const __m128 temp_z = _mm_shuffle_ps(_z,vec.sse,_MM_SHUFFLE(2,2,1,0));
  const __m128 temp_w = _mm_shuffle_ps(_w,vec.sse,_MM_SHUFFLE(3,3,1,0));
  _x = _mm_shuffle_ps(temp_x,_x,_MM_SHUFFLE(3,2,1,2));
  _y = _mm_shuffle_ps(temp_y,_y,_MM_SHUFFLE(3,2,1,2));
  _z = _mm_shuffle_ps(temp_z,_z,_MM_SHUFFLE(3,2,1,2));
  _w = _mm_shuffle_ps(temp_w,_w,_MM_SHUFFLE(3,2,1,2));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set1(const XQuaternion& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  const __m128 temp_x = _mm_shuffle_ps(_x,vec.sse,_MM_SHUFFLE(0,0,1,0));
  const __m128 temp_y = _mm_shuffle_ps(_y,vec.sse,_MM_SHUFFLE(1,1,1,0));
  const __m128 temp_z = _mm_shuffle_ps(_z,vec.sse,_MM_SHUFFLE(2,2,1,0));
  const __m128 temp_w = _mm_shuffle_ps(_w,vec.sse,_MM_SHUFFLE(3,3,1,0));
  _x = _mm_shuffle_ps(temp_x,_x,_MM_SHUFFLE(3,2,2,0));
  _y = _mm_shuffle_ps(temp_y,_y,_MM_SHUFFLE(3,2,2,0));
  _z = _mm_shuffle_ps(temp_z,_z,_MM_SHUFFLE(3,2,2,0));
  _w = _mm_shuffle_ps(temp_w,_w,_MM_SHUFFLE(3,2,2,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set2(const XQuaternion& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  const __m128 temp_x = _mm_shuffle_ps(_x,vec.sse,_MM_SHUFFLE(0,0,3,2));
  const __m128 temp_y = _mm_shuffle_ps(_y,vec.sse,_MM_SHUFFLE(1,1,3,2));
  const __m128 temp_z = _mm_shuffle_ps(_z,vec.sse,_MM_SHUFFLE(2,2,3,2));
  const __m128 temp_w = _mm_shuffle_ps(_w,vec.sse,_MM_SHUFFLE(3,3,3,2));
  _x = _mm_shuffle_ps(_x,temp_x,_MM_SHUFFLE(1,2,1,0));
  _y = _mm_shuffle_ps(_y,temp_y,_MM_SHUFFLE(1,2,1,0));
  _z = _mm_shuffle_ps(_z,temp_z,_MM_SHUFFLE(1,2,1,0));
  _w = _mm_shuffle_ps(_w,temp_w,_MM_SHUFFLE(1,2,1,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::set3(const XQuaternion& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  const __m128 temp_x = _mm_shuffle_ps(_x,vec.sse,_MM_SHUFFLE(0,0,3,2));
  const __m128 temp_y = _mm_shuffle_ps(_y,vec.sse,_MM_SHUFFLE(1,1,3,2));
  const __m128 temp_z = _mm_shuffle_ps(_z,vec.sse,_MM_SHUFFLE(2,2,3,2));
  const __m128 temp_w = _mm_shuffle_ps(_w,vec.sse,_MM_SHUFFLE(3,3,3,2));
  _x = _mm_shuffle_ps(_x,temp_x,_MM_SHUFFLE(2,0,1,0));
  _y = _mm_shuffle_ps(_y,temp_y,_MM_SHUFFLE(2,0,1,0));
  _z = _mm_shuffle_ps(_z,temp_z,_MM_SHUFFLE(2,0,1,0));
  _w = _mm_shuffle_ps(_w,temp_w,_MM_SHUFFLE(2,0,1,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get0(XQuaternion& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  __m128 xy = _mm_shuffle_ps(_x,_y,_MM_SHUFFLE(0,0,0,0));
  __m128 zw = _mm_shuffle_ps(_z,_w,_MM_SHUFFLE(0,0,0,0));
  vec.sse = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(2,0,2,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get1(XQuaternion& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  __m128 xy = _mm_shuffle_ps(_x,_y,_MM_SHUFFLE(1,1,1,1));
  __m128 zw = _mm_shuffle_ps(_z,_w,_MM_SHUFFLE(1,1,1,1));
  vec.sse = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(2,0,2,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get2(XQuaternion& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  __m128 xy = _mm_shuffle_ps(_x,_y,_MM_SHUFFLE(2,2,2,2));
  __m128 zw = _mm_shuffle_ps(_z,_w,_MM_SHUFFLE(2,2,2,2));
  vec.sse = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(2,0,2,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::get3(XQuaternion& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  __m128 xy = _mm_shuffle_ps(_x,_y,_MM_SHUFFLE(3,3,3,3));
  __m128 zw = _mm_shuffle_ps(_z,_w,_MM_SHUFFLE(3,3,3,3));
  vec.sse = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(2,0,2,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::identity()
{
  XM2_ALIGN_ASSERT(this);
  _mm_stream_ps(x,_mm_setzero_ps());
  _mm_stream_ps(y,_mm_setzero_ps());
  _mm_stream_ps(z,_mm_setzero_ps());
  _mm_stream_ps(w,_mm_set_ps1(1.0f));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::copy(const XQuaternionSOA& q)
{
  XM2_ALIGN_ASSERT(&q);
  XM2_ALIGN_ASSERT(this);
  _mm_stream_ps(x,_mm_load_ps(q.x));
  _mm_stream_ps(y,_mm_load_ps(q.y));
  _mm_stream_ps(z,_mm_load_ps(q.z));
  _mm_stream_ps(w,_mm_load_ps(q.w));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE bool XQuaternionSOA::equal(const XQuaternionSOA& b,const XReal eps) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128 a =_mm_and_ps(_mm_and_ps(_mm_cmpeq_ps(_x,b._x),_mm_cmpeq_ps(_y,b._y)),
                       _mm_and_ps(_mm_cmpeq_ps(_z,b._z),_mm_cmpeq_ps(_w,b._w)));
  return _mm_movemask_ps(a)==15;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::dot(XVector4& product,const XQuaternionSOA& b) const
{
  XM2_ALIGN_ASSERT(&product);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  product.sse = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_x,b._x),_mm_mul_ps(_y,b._y)),
                           _mm_add_ps(_mm_mul_ps(_z,b._z),_mm_mul_ps(_w,b._w)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::add(const XQuaternionSOA& a,const XQuaternionSOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps(a._x,b._x);
  _y = _mm_add_ps(a._y,b._y);
  _z = _mm_add_ps(a._z,b._z);
  _w = _mm_add_ps(a._w,b._w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::add(const XQuaternionSOA& a,const XQuaternion& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps(a._x,_mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(0,0,0,0)));
  _y = _mm_add_ps(a._y,_mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(1,1,1,1)));
  _z = _mm_add_ps(a._z,_mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(2,2,2,2)));
  _w = _mm_add_ps(a._w,_mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(3,3,3,3)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::add(const XQuaternionSOA& a)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps(a._x,_x);
  _y = _mm_add_ps(a._y,_y);
  _z = _mm_add_ps(a._z,_z);
  _w = _mm_add_ps(a._w,_w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const XQuaternionSOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps(_mm_add_ps(_mm_mul_ps(a._w,b._x),_mm_mul_ps(a._x,b._w)), 
                  _mm_sub_ps(_mm_mul_ps(b._y,a._z),_mm_mul_ps(b._z,a._y)));
  _y = _mm_add_ps(_mm_add_ps(_mm_mul_ps(a._w,b._x),_mm_mul_ps(a._y,b._w)), 
                  _mm_sub_ps(_mm_mul_ps(b._z,a._x),_mm_mul_ps(b._x,a._z)));
  _z = _mm_add_ps(_mm_add_ps(_mm_mul_ps(a._w,b._y),_mm_mul_ps(a._z,b._w)), 
                  _mm_sub_ps(_mm_mul_ps(b._x,a._y),_mm_mul_ps(b._y,a._x)));
  _w = _mm_add_ps(_mm_add_ps(_mm_mul_ps(a._w,b._z),_mm_mul_ps(a._x,b._x)), 
                  _mm_sub_ps(_mm_mul_ps(b._y,a._y),_mm_mul_ps(b._z,a._x)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const XQuaternion& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XQuaternionSOA _b( _mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(0,0,0,0)),
                     _mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(1,1,1,1)),
                     _mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(2,2,2,2)),
                     _mm_shuffle_ps(b.sse,b.sse,_MM_SHUFFLE(3,3,3,3)) );
  mul(a,_b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const __m128 b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_mul_ps(b,a._x);
  _y = _mm_mul_ps(b,a._y);
  _z = _mm_mul_ps(b,a._z);
  _w = _mm_mul_ps(b,a._w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternionSOA& a,const XReal b)
{
  mul(a,_mm_set_ps1(b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XQuaternion& b)
{
  XQuaternionSOA temp(*this);
  mul(temp,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const __m128 b)
{
  mul(*this,b);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::mul(const XReal b)
{
  mul(*this,_mm_set_ps1(b));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::slerp(const XQuaternionSOA& a,const XQuaternionSOA& b,const XReal t)
{
  slerp(a,b,_mm_set_ps1(t));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::shortestPath(const XQuaternionSOA &a,const XQuaternionSOA &b,XVector4& product)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&product);
  a.dot(product,b);
  __m128 inv_product = _mm_sub_ps(_mm_setzero_ps(),product.sse);

  // if(product < 0)
  __m128 less_than_zero = _mm_cmplt_ps(product.sse,_mm_setzero_ps());

  // product = -product    (only if product is less than 0)
  product = _mm_or_ps(_mm_and_ps(inv_product,less_than_zero),
                      _mm_andnot_ps(less_than_zero,product.sse));

  // negate the quats that need to be negated
  _x = _mm_and_ps(_mm_and_ps(_mm_sub_ps(_mm_setzero_ps(),b._x),less_than_zero),
                  _mm_andnot_ps(less_than_zero,b._x));
  _y = _mm_and_ps(_mm_and_ps(_mm_sub_ps(_mm_setzero_ps(),b._y),less_than_zero),
                  _mm_andnot_ps(less_than_zero,b._y));
  _z = _mm_and_ps(_mm_and_ps(_mm_sub_ps(_mm_setzero_ps(),b._z),less_than_zero),
                  _mm_andnot_ps(less_than_zero,b._z));
  _w = _mm_and_ps(_mm_and_ps(_mm_sub_ps(_mm_setzero_ps(),b._w),less_than_zero),
                  _mm_andnot_ps(less_than_zero,b._w));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::longestPath(const XQuaternionSOA &a,const XQuaternionSOA &b,XVector4& product)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&product);
  a.dot(product,b);
  __m128 inv_product = _mm_sub_ps(_mm_setzero_ps(),product.sse);

  // if(product > 0)
  __m128 gt_than_zero = _mm_cmpgt_ps(product.sse,_mm_setzero_ps());

  // product = -product    (only if product is greater than 0)
  product = _mm_or_ps(_mm_and_ps(inv_product,gt_than_zero),
                      _mm_andnot_ps(gt_than_zero,product.sse));

  // negate the quats that need to be negated
  _x = _mm_and_ps(_mm_and_ps(_mm_sub_ps(_mm_setzero_ps(),b._x),gt_than_zero),
                  _mm_andnot_ps(gt_than_zero,b._x));
  _y = _mm_and_ps(_mm_and_ps(_mm_sub_ps(_mm_setzero_ps(),b._y),gt_than_zero),
                  _mm_andnot_ps(gt_than_zero,b._y));
  _z = _mm_and_ps(_mm_and_ps(_mm_sub_ps(_mm_setzero_ps(),b._z),gt_than_zero),
                  _mm_andnot_ps(gt_than_zero,b._z));
  _w = _mm_and_ps(_mm_and_ps(_mm_sub_ps(_mm_setzero_ps(),b._w),gt_than_zero),
                  _mm_andnot_ps(gt_than_zero,b._w));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::slerp(const XQuaternionSOA& a,const XQuaternionSOA& b,const __m128 t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  
  // compare both quats, and negate b if needed. result stored in this
  XVector4 product;
  shortestPath(a,b,product);

  // our t values
  __m128 _inv_t = _mm_sub_ps(_mm_setzero_ps(),t);

  // compute sin theta 
  __m128 theta    = XMacos(product.sse);
  __m128 sinTheta = _mm_div_ps(_mm_set_ps1(1.0f),XMsin(theta));

  // calc interpolation values
  _inv_t    = _mm_mul_ps(XMsin(_mm_mul_ps(_inv_t , theta)) , sinTheta);
  __m128 _t = _mm_mul_ps(XMsin(_mm_mul_ps(t      , theta)) , sinTheta);

  // perform the interpolation
  _x = _mm_add_ps(_mm_mul_ps(a._x,_inv_t),_mm_mul_ps(_x,_t));
  _y = _mm_add_ps(_mm_mul_ps(a._y,_inv_t),_mm_mul_ps(_y,_t));
  _z = _mm_add_ps(_mm_mul_ps(a._z,_inv_t),_mm_mul_ps(_z,_t));
  _w = _mm_add_ps(_mm_mul_ps(a._w,_inv_t),_mm_mul_ps(_w,_t));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::lerp(const XQuaternionSOA& a,const XQuaternionSOA& b,const XReal t)
{
  lerp(a,b,_mm_set_ps1(t));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::lerp(const XQuaternionSOA& a,const XQuaternionSOA& b,const __m128 t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  // compare both quats, and negate b if needed. result stored in this
  XVector4 product;
  shortestPath(a,b,product);
  _x = _mm_add_ps(a._x, _mm_mul_ps(t, _mm_sub_ps(_x,a._x) ) );
  _y = _mm_add_ps(a._y, _mm_mul_ps(t, _mm_sub_ps(_y,a._y) ) );
  _z = _mm_add_ps(a._z, _mm_mul_ps(t, _mm_sub_ps(_z,a._z) ) );
  _w = _mm_add_ps(a._w, _mm_mul_ps(t, _mm_sub_ps(_w,a._w) ) );
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
  _x = _mm_sub_ps(_mm_setzero_ps(),q._x);
  _y = _mm_sub_ps(_mm_setzero_ps(),q._y);
  _z = _mm_sub_ps(_mm_setzero_ps(),q._z);
  _w = q._w;
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
  _x = _mm_sub_ps(_mm_setzero_ps(),q._x);
  _y = _mm_sub_ps(_mm_setzero_ps(),q._y);
  _z = _mm_sub_ps(_mm_setzero_ps(),q._z);
  _w = _mm_sub_ps(_mm_setzero_ps(),q._w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::length(XVector4& len) const
{
  dot(len,*this);
  len.sse = _mm_sqrt_ps(len.sse);
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
  __m128 inv_len = _mm_rcp_ps(len.sse);
  _x = _mm_mul_ps(inv_len,q._x);
  _y = _mm_mul_ps(inv_len,q._y);
  _z = _mm_mul_ps(inv_len,q._z);
  _w = _mm_mul_ps(inv_len,q._w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::sum(const XQuaternionSOA& a,const XReal t)
{
  sum(a,_mm_set_ps1(t));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XQuaternionSOA::sum(const XQuaternionSOA& a,const __m128 t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps(_x,_mm_mul_ps(t,a._x));
  _y = _mm_add_ps(_y,_mm_mul_ps(t,a._y));
  _z = _mm_add_ps(_z,_mm_mul_ps(t,a._z));
  _w = _mm_add_ps(_w,_mm_mul_ps(t,a._w));
}
}
#endif