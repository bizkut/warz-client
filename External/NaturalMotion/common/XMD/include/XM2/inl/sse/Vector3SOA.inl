#ifdef _MSC_VER
# pragma once
#endif

#ifndef XM2_SSE_VECTOR3SOA__INL__
#define XM2_SSE_VECTOR3SOA__INL__
#if (!XM2_USE_FLOAT)
# error SSE float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA()
{
  XM2_ALIGN_ASSERT(this);
  #if XM2_INIT_CLASSES
  _x = _y = _z = _mm_setzero_ps();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector3& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x =  _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(0,0,0,0));
  _y =  _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(1,1,1,1));
  _z =  _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(2,2,2,2));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector4& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x =  _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(0,0,0,0));
  _y =  _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(1,1,1,1));
  _z =  _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(2,2,2,2));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector3Packed& a)
{
  XM2_ALIGN_ASSERT(this);
  _x = _mm_set_ps1(a.x);
  _y = _mm_set_ps1(a.y);
  _z = _mm_set_ps1(a.z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector3SOA& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x = a._x;
  _y = a._y;
  _z = a._z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector4SOA& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x = a._x;
  _y = a._y;
  _z = a._z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector3* input)
{
  XM2_ASSERT(input);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  const __m128 tmp0 = _mm_shuffle_ps((input[0].sse), (input[1].sse), 0x44);
  const __m128 tmp2 = _mm_shuffle_ps((input[0].sse), (input[1].sse), 0xEE);
  const __m128 tmp1 = _mm_shuffle_ps((input[2].sse), (input[3].sse), 0x44);
  const __m128 tmp3 = _mm_shuffle_ps((input[2].sse), (input[3].sse), 0xEE);
  _x = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  _y = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  _z = _mm_shuffle_ps(tmp2, tmp3, 0x88);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector4* input)
{
  XM2_ASSERT(input);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  const __m128 tmp0 = _mm_shuffle_ps((input[0].sse), (input[1].sse), 0x44);
  const __m128 tmp2 = _mm_shuffle_ps((input[0].sse), (input[1].sse), 0xEE);
  const __m128 tmp1 = _mm_shuffle_ps((input[2].sse), (input[3].sse), 0x44);
  const __m128 tmp3 = _mm_shuffle_ps((input[2].sse), (input[3].sse), 0xEE);
  _x = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  _y = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  _z = _mm_shuffle_ps(tmp2, tmp3, 0x88);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector4SOA& input)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&input);
  _x = input._x;
  _y = input._y;
  _z = input._z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector3Packed* input)
{
  XM2_ASSERT(input);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  //
  //  x0 y0 z0 x1
  //  y1 z1 x2 y2
  //  x2 x3 y3 z3
  //
  __m128 a = _mm_load_ps(input->data);
  __m128 b = _mm_load_ps(input->data+4);
  __m128 c = _mm_load_ps(input->data+8);

  //  x0 y0 x2 y2
  const __m128 xy = _mm_shuffle_ps( a,b,_MM_SHUFFLE(3,2,1,0) );

  //  z0 x1 z2 x3
  const __m128 zx = _mm_shuffle_ps( a,c,_MM_SHUFFLE(1,0,3,2) );

  //  y1 z1 y3 z3
  const __m128 yz = _mm_shuffle_ps( b,c,_MM_SHUFFLE(3,2,1,0) );

  // x0 x2 x1 x3
  a = _mm_shuffle_ps( xy, zx, _MM_SHUFFLE(3,1,2,0) );

  // y0 y2 y1 y3
  b = _mm_shuffle_ps( xy, yz, _MM_SHUFFLE(2,0,3,1) );

  // z0 z2 z1 z3
  c = _mm_shuffle_ps( zx, yz, _MM_SHUFFLE(3,1,2,0) );

  // x0 x1 x2 x3
  _x = _mm_shuffle_ps( a, a, _MM_SHUFFLE(3,1,2,0) );

  // y0 y1 y2 y3
  _y = _mm_shuffle_ps( b, b, _MM_SHUFFLE(3,1,2,0) );

  // z0 z1 z2 z3
  _z = _mm_shuffle_ps( c, c, _MM_SHUFFLE(3,1,2,0) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector3* output) const
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(output);
  const __m128 _w = _mm_setzero_ps();
  const __m128 tmp0 = _mm_shuffle_ps((_x), (_y), 0x44);
  const __m128 tmp2 = _mm_shuffle_ps((_x), (_y), 0xEE);
  const __m128 tmp1 = _mm_shuffle_ps((_z), (_w), 0x44);
  const __m128 tmp3 = _mm_shuffle_ps((_z), (_w), 0xEE);
  output[0].sse = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  output[1].sse = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  output[2].sse = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  output[3].sse = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector4* output) const
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(output);
  const __m128 _w = _mm_setzero_ps();
  const __m128 tmp0 = _mm_shuffle_ps((_x), (_y), 0x44);
  const __m128 tmp2 = _mm_shuffle_ps((_x), (_y), 0xEE);
  const __m128 tmp1 = _mm_shuffle_ps((_z), (_w), 0x44);
  const __m128 tmp3 = _mm_shuffle_ps((_z), (_w), 0xEE);
  output[0].sse = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  output[1].sse = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  output[2].sse = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  output[3].sse = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector4SOA& output) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&output);
  output._x = _x;
  output._y = _y;
  output._z = _z;
  output._w = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector3Packed* output) const
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(output);
  const __m128 xy01 = _mm_movelh_ps(_x,_y);  // 1 2 5  6
  const __m128 xz01 = _mm_movelh_ps(_x,_z);  // 1 2 9 10
  const __m128 yz01 = _mm_movelh_ps(_y,_z);  // 5 6 9 10

  const __m128 xy23 = _mm_movehl_ps(_x,_y);  // 11 12 7 8
  const __m128 xz23 = _mm_movehl_ps(_x,_z);  //  7  8 3 4
  const __m128 yz23 = _mm_movehl_ps(_y,_z);  // 11 12 3 4

  // 1 5 9 2
  _mm_stream_ps( output->data +  0, _mm_shuffle_ps( xy01, xz01, _MM_SHUFFLE(1,2,2,0) ) );

  // 6 10 3 7
  _mm_stream_ps( output->data +  4, _mm_shuffle_ps( yz01, xy23, _MM_SHUFFLE(0,2,3,1) ) );

  // 11 4 8 12
  _mm_stream_ps( output->data +  8, _mm_shuffle_ps( xz23, yz23, _MM_SHUFFLE(1,3,3,0) ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::zero() 
{
  XM2_ALIGN_ASSERT(this);
  _x = _mm_setzero_ps();
  _y = _mm_setzero_ps();
  _z = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::lengthSquared(XVector4& _length_sqr) const
{
  XM2_ALIGN_ASSERT(&_length_sqr);
  XM2_ALIGN_ASSERT(this);
  dot(_length_sqr,*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::length(XVector4& _length) const
{
  XM2_ALIGN_ASSERT(&_length);
  XM2_ALIGN_ASSERT(this);
  lengthSquared(_length);
  _length.sse = _mm_sqrt_ps(_length.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::normalise(const XVector3SOA& a)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  XVector4 l;
  a.length(l);

  // test if any of the lengths are zero
  const __m128 test = _mm_and_ps(_mm_cmpgt_ps(l.sse,_mm_sub_ps(_mm_setzero_ps(),_mm_set_ps1(XM2_FLOAT_ZERO))),
                                 _mm_cmplt_ps(l.sse,_mm_set_ps1(XM2_FLOAT_ZERO)));

  // replace any zero's with 1.0f
  l.sse = _mm_rcp_ps(_mm_or_ps(_mm_and_ps(test,_mm_set_ps1(1.0f)),
                               _mm_andnot_ps(test,l.sse)));

  _x = _mm_mul_ps( a._x, l.sse);
  _y = _mm_mul_ps( a._y, l.sse);
  _z = _mm_mul_ps( a._z, l.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XVector3SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps( a._x, b._x );
  _y = _mm_add_ps( a._y, b._y );
  _z = _mm_add_ps( a._z, b._z );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XVector3SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps( a._x, b._x );
  _y = _mm_sub_ps( a._y, b._y );
  _z = _mm_sub_ps( a._z, b._z );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XVector3SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_mul_ps( a._x, b._x );
  _y = _mm_mul_ps( a._y, b._y );
  _z = _mm_mul_ps( a._z, b._z );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XVector3SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_div_ps( a._x, b._x );
  _y = _mm_div_ps( a._y, b._y );
  _z = _mm_div_ps( a._z, b._z );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps( a._x, _mm_set_ps1(b.x) );
  _y = _mm_add_ps( a._y, _mm_set_ps1(b.y) );
  _z = _mm_add_ps( a._z, _mm_set_ps1(b.z) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps( a._x, _mm_set_ps1(b.x) );
  _y = _mm_sub_ps( a._y, _mm_set_ps1(b.y) );
  _z = _mm_sub_ps( a._z, _mm_set_ps1(b.z) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_mul_ps( a._x, _mm_set_ps1(b.x) );
  _y = _mm_mul_ps( a._y, _mm_set_ps1(b.y) );
  _z = _mm_mul_ps( a._z, _mm_set_ps1(b.z) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_div_ps( a._x, _mm_set_ps1(b.x) );
  _y = _mm_div_ps( a._y, _mm_set_ps1(b.y) );
  _z = _mm_div_ps( a._z, _mm_set_ps1(b.z) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps( a._x, b._x );
  _y = _mm_add_ps( a._y, b._y );
  _z = _mm_add_ps( a._z, b._z );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps( a._x, b._x );
  _y = _mm_sub_ps( a._y, b._y );
  _z = _mm_sub_ps( a._z, b._z );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_mul_ps( a._x, b._x );
  _y = _mm_mul_ps( a._y, b._y );
  _z = _mm_mul_ps( a._z, b._z );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_div_ps( a._x, b._x );
  _y = _mm_div_ps( a._y, b._y );
  _z = _mm_div_ps( a._z, b._z );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128 temp = _mm_set_ps1(b);
  _x = _mm_add_ps( a._x, temp );
  _y = _mm_add_ps( a._y, temp );
  _z = _mm_add_ps( a._z, temp );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128 temp = _mm_set_ps1(b);
  _x = _mm_sub_ps( a._x, temp );
  _y = _mm_sub_ps( a._y, temp );
  _z = _mm_sub_ps( a._z, temp );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128 temp = _mm_set_ps1(b);
  _x = _mm_mul_ps( a._x, temp );
  _y = _mm_mul_ps( a._y, temp );
  _z = _mm_mul_ps( a._z, temp );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128 temp = _mm_set_ps1(b);
  _x = _mm_div_ps( a._x, temp );
  _y = _mm_div_ps( a._y, temp );
  _z = _mm_div_ps( a._z, temp );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const __m128 b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps( a._x, b );
  _y = _mm_add_ps( a._y, b );
  _z = _mm_add_ps( a._z, b );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const __m128 b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps( a._x, b );
  _y = _mm_sub_ps( a._y, b );
  _z = _mm_sub_ps( a._z, b );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const __m128 b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_mul_ps( a._x, b );
  _y = _mm_mul_ps( a._y, b );
  _z = _mm_mul_ps( a._z, b );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const __m128 b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128 temp = _mm_rcp_ps(b);
  _x = _mm_mul_ps( a._x, temp );
  _y = _mm_mul_ps( a._y, temp );
  _z = _mm_mul_ps( a._z, temp );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::dot(XVector4& _dot,const XVector3SOA& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&_dot);
  XM2_ALIGN_ASSERT(this);
  const __m128 x2 = _mm_mul_ps(_x,b._x);
  const __m128 y2 = _mm_mul_ps(_y,b._y);
  const __m128 z2 = _mm_mul_ps(_z,b._z);
  _dot.sse = _mm_add_ps( x2, _mm_add_ps( y2, z2 ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XVector3SOA::dotSSE(const XVector3SOA& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128 x2 = _mm_mul_ps(_x,b._x);
  const __m128 y2 = _mm_mul_ps(_y,b._y);
  const __m128 z2 = _mm_mul_ps(_z,b._z);
  return _mm_add_ps( x2, _mm_add_ps( y2, z2 ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::cross(const XVector3SOA& a,const XVector3SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps( _mm_mul_ps(a._y,b._z), _mm_mul_ps(a._z,b._y) );
  _y = _mm_sub_ps( _mm_mul_ps(a._z,b._x), _mm_mul_ps(a._x,b._z) );
  _z = _mm_sub_ps( _mm_mul_ps(a._x,b._y), _mm_mul_ps(a._y,b._x) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE __m128 XVector3SOA::lengthSSE() const
{
  return  _mm_sqrt_ps(dotSSE(*this));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::lerp(const XVector3SOA& a,const XVector3SOA& b,const XReal t)
{
  lerp(a,b,_mm_set_ps1(t));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::lerp(const XVector3SOA& a,const XVector3SOA& b,const __m128 t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps( a._x, _mm_mul_ps( t, _mm_sub_ps(b._x,a._x) ) );
  _y = _mm_add_ps( a._y, _mm_mul_ps( t, _mm_sub_ps(b._y,a._y) ) );
  _z = _mm_add_ps( a._z, _mm_mul_ps( t, _mm_sub_ps(b._z,a._z) ) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::bezier(const XVector3SOA& a,const XVector3SOA& b,
                                const XVector3SOA& c,const XVector3SOA& d,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&c);
  XM2_ALIGN_ASSERT(&d);
  XM2_ALIGN_ASSERT(this);
  // 
  // 18 mults, 
  // 1 set
  // 6 shuffles
  // 9 add
  // 
  const __m128 _t  = _mm_set_ps(0,0,1.0f-t,t);
  const __m128 _t2 = _mm_mul_ps(_t,_t);
  const __m128 _t3 = _mm_mul_ps(_t2,_t);

  const __m128 B0 = _mm_shuffle_ps(_t3,_t3,_MM_SHUFFLE(1,1,1,1));
  const __m128 B1 = _mm_mul_ps( _mm_set_ps1(3.0f),
                                _mm_mul_ps( _mm_shuffle_ps(_t2,_t2,_MM_SHUFFLE(1,1,1,1)), 
                                            _mm_shuffle_ps(_t,_t,_MM_SHUFFLE(0,0,0,0))));
  const __m128 B2 = _mm_mul_ps( _mm_set_ps1(3.0f),
                                _mm_mul_ps( _mm_shuffle_ps(_t,_t,_MM_SHUFFLE(1,1,1,1)), 
                                           _mm_shuffle_ps(_t2,_t2,_MM_SHUFFLE(0,0,0,0))));
  const __m128 B3 = _mm_shuffle_ps(_t3,_t3,_MM_SHUFFLE(0,0,0,0));

  _x = _mm_mul_ps( B0, a._x );
  _y = _mm_mul_ps( B0, a._y );
  _z = _mm_mul_ps( B0, a._z );
  _x = _mm_add_ps( _x, _mm_mul_ps( B1, b._x) );
  _y = _mm_add_ps( _y, _mm_mul_ps( B1, b._y) );
  _z = _mm_add_ps( _z, _mm_mul_ps( B1, b._z) );
  _x = _mm_add_ps( _x, _mm_mul_ps( B2, c._x) );
  _y = _mm_add_ps( _y, _mm_mul_ps( B2, c._y) );
  _z = _mm_add_ps( _z, _mm_mul_ps( B2, c._z) );
  _x = _mm_add_ps( _x, _mm_mul_ps( B3, d._x) );
  _y = _mm_add_ps( _y, _mm_mul_ps( B3, d._y) );
  _z = _mm_add_ps( _z, _mm_mul_ps( B3, d._z) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::bezier(const XVector3SOA& a,const XVector3SOA& b,
                                const XVector3SOA& c,const XVector3SOA& d,const __m128 t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&c);
  XM2_ALIGN_ASSERT(&d);
  XM2_ALIGN_ASSERT(this);
  // 
  // 20 mults, 
  // 1 set
  // 9 add
  // 1 sub
  const __m128 _it  = _mm_sub_ps(_mm_set_ps1(1.0f),t);
  const __m128 _it2 = _mm_mul_ps(_it,_it);
  const __m128 _it3 = _mm_mul_ps(_it,_it2);
  const __m128 _t2 = _mm_mul_ps(t,t);
  const __m128 _t3 = _mm_mul_ps(_t2,t);

  const __m128 B0 = _it3;
  const __m128 B1 = _mm_mul_ps( _mm_set_ps1(3.0f),
                                _mm_mul_ps(t, _it2) );
  const __m128 B2 = _mm_mul_ps( _mm_set_ps1(3.0f),
                                _mm_mul_ps(_it, _t2) );
  const __m128 B3 = _t3;

  _x = _mm_mul_ps( B0, a._x );
  _y = _mm_mul_ps( B0, a._y );
  _z = _mm_mul_ps( B0, a._z );
  _x = _mm_add_ps( _x, _mm_mul_ps( B1, b._x) );
  _y = _mm_add_ps( _y, _mm_mul_ps( B1, b._y) );
  _z = _mm_add_ps( _z, _mm_mul_ps( B1, b._z) );
  _x = _mm_add_ps( _x, _mm_mul_ps( B2, c._x) );
  _y = _mm_add_ps( _y, _mm_mul_ps( B2, c._y) );
  _z = _mm_add_ps( _z, _mm_mul_ps( B2, c._z) );
  _x = _mm_add_ps( _x, _mm_mul_ps( B3, d._x) );
  _y = _mm_add_ps( _y, _mm_mul_ps( B3, d._y) );
  _z = _mm_add_ps( _z, _mm_mul_ps( B3, d._z) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::negate(const XVector3SOA& a)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps(_mm_setzero_ps(),a._x);
  _y = _mm_sub_ps(_mm_setzero_ps(),a._y);
  _z = _mm_sub_ps(_mm_setzero_ps(),a._z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::negate()
{
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps(_mm_setzero_ps(),_x);
  _y = _mm_sub_ps(_mm_setzero_ps(),_y);
  _z = _mm_sub_ps(_mm_setzero_ps(),_z);
}
}
#endif
