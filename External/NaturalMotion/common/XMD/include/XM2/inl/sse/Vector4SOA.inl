#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_VECTOR4SOA__INL__
#define XM2_SSE_VECTOR4SOA__INL__
#if (!XM2_USE_FLOAT)
# error SSE2 float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA()
{
  XM2_ALIGN_ASSERT(this);
  #if XM2_INIT_CLASSES
  _x = _y = _z = _w = _mm_setzero_ps();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector3& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x = _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(0,0,0,0));
  _y = _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(1,1,1,1));
  _z = _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(2,2,2,2));
  _w = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector4& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x = _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(0,0,0,0));
  _y = _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(1,1,1,1));
  _z = _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(2,2,2,2));
  _w = _mm_shuffle_ps(a.sse,a.sse,_MM_SHUFFLE(3,3,3,3));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector3Packed& a)
{
  XM2_ALIGN_ASSERT(this);
  _x = _mm_set_ps1(a.x);
  _y = _mm_set_ps1(a.y);
  _z = _mm_set_ps1(a.z);
  _w = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector3SOA& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x = a._x;
  _y = a._y;
  _z = a._z;
  _w = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector4SOA& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x = a._x;
  _y = a._y;
  _z = a._z;
  _w = a._w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector3* input)
{
  XM2_ASSERT(input);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  __m128 tmp0 = _mm_shuffle_ps((input[0].sse), (input[1].sse), 0x44);
  __m128 tmp2 = _mm_shuffle_ps((input[0].sse), (input[1].sse), 0xEE);
  __m128 tmp1 = _mm_shuffle_ps((input[2].sse), (input[3].sse), 0x44);
  __m128 tmp3 = _mm_shuffle_ps((input[2].sse), (input[3].sse), 0xEE);
  _x = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  _y = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  _z = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  _w = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector4* input)
{
  XM2_ASSERT(input);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  __m128 tmp0 = _mm_shuffle_ps((input[0].sse), (input[1].sse), 0x44);
  __m128 tmp2 = _mm_shuffle_ps((input[0].sse), (input[1].sse), 0xEE);
  __m128 tmp1 = _mm_shuffle_ps((input[2].sse), (input[3].sse), 0x44);
  __m128 tmp3 = _mm_shuffle_ps((input[2].sse), (input[3].sse), 0xEE);
  _x = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  _y = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  _z = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  _w = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector3SOA& input)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&input);
  _x = input._x;
  _y = input._y;
  _z = input._z;
  _w = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector3Packed* input)
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
  __m128 xy = _mm_shuffle_ps( a,b,_MM_SHUFFLE(3,2,1,0) );

  //  z0 x1 z2 x3
  __m128 zx = _mm_shuffle_ps( a,c,_MM_SHUFFLE(1,0,3,2) );

  //  y1 z1 y3 z3
  __m128 yz = _mm_shuffle_ps( b,c,_MM_SHUFFLE(3,2,1,0) );

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
  _w = _mm_setzero_ps();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector3* output) const
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(output);
  __m128 tmp0 = _mm_shuffle_ps((_x), (_y), 0x44);
  __m128 tmp2 = _mm_shuffle_ps((_x), (_y), 0xEE);
  __m128 tmp1 = _mm_shuffle_ps((_z), (_w), 0x44);
  __m128 tmp3 = _mm_shuffle_ps((_z), (_w), 0xEE);
  output[0].sse = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  output[1].sse = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  output[2].sse = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  output[3].sse = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector4* output) const
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(output);
  __m128 tmp0 = _mm_shuffle_ps((_x), (_y), 0x44);
  __m128 tmp2 = _mm_shuffle_ps((_x), (_y), 0xEE);
  __m128 tmp1 = _mm_shuffle_ps((_z), (_w), 0x44);
  __m128 tmp3 = _mm_shuffle_ps((_z), (_w), 0xEE);
  output[0].sse = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  output[1].sse = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  output[2].sse = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  output[3].sse = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector3SOA& output) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&output);
  output._x = _x;
  output._y = _y;
  output._z = _z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector3Packed* output) const
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
XM2_INLINE void XVector4SOA::lengthSquared(XVector4& _length_sqr) const
{
  dot(_length_sqr,*this);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::zero()
{
  XM2_ALIGN_ASSERT(this);
  _x = _mm_setzero_ps();
  _y = _mm_setzero_ps();
  _z = _mm_setzero_ps();
  _w = _mm_setzero_ps();
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XVector4SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps( a._x, b._x );
  _y = _mm_add_ps( a._y, b._y );
  _z = _mm_add_ps( a._z, b._z );
  _w = _mm_add_ps( a._w, b._w );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps( a._x, b._x );
  _y = _mm_sub_ps( a._y, b._y );
  _z = _mm_sub_ps( a._z, b._z );
  _w = _mm_sub_ps( a._w, b._w );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_mul_ps( a._x, b._x );
  _y = _mm_mul_ps( a._y, b._y );
  _z = _mm_mul_ps( a._z, b._z );
  _w = _mm_mul_ps( a._w, b._w );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_div_ps( a._x, b._x );
  _y = _mm_div_ps( a._y, b._y );
  _z = _mm_div_ps( a._z, b._z );
  _w = _mm_div_ps( a._w, b._w );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XVector4SOA& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_add_ps( a._x, _mm_set_ps1(b.x) );
  _y = _mm_add_ps( a._y, _mm_set_ps1(b.y) );
  _z = _mm_add_ps( a._z, _mm_set_ps1(b.z) );
  _w = _mm_add_ps( a._w, _mm_set_ps1(b.w) );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4SOA& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps( a._x, _mm_set_ps1(b.x) );
  _y = _mm_sub_ps( a._y, _mm_set_ps1(b.y) );
  _z = _mm_sub_ps( a._z, _mm_set_ps1(b.z) );
  _w = _mm_sub_ps( a._w, _mm_set_ps1(b.w) );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4SOA& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_mul_ps( a._x, _mm_set_ps1(b.x) );
  _y = _mm_mul_ps( a._y, _mm_set_ps1(b.y) );
  _z = _mm_mul_ps( a._z, _mm_set_ps1(b.z) );
  _w = _mm_mul_ps( a._w, _mm_set_ps1(b.w) );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4SOA& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_div_ps( a._x, _mm_set_ps1(b.x) );
  _y = _mm_div_ps( a._y, _mm_set_ps1(b.y) );
  _z = _mm_div_ps( a._z, _mm_set_ps1(b.z) );
  _w = _mm_div_ps( a._w, _mm_set_ps1(b.w) );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XVector4SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128 _b = _mm_set_ps1(b);
  _x = _mm_add_ps( a._x, _b );
  _y = _mm_add_ps( a._y, _b );
  _z = _mm_add_ps( a._z, _b );
  _w = _mm_add_ps( a._w, _b );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128 _b = _mm_set_ps1(b);
  _x = _mm_sub_ps( a._x, _b );
  _y = _mm_sub_ps( a._y, _b );
  _z = _mm_sub_ps( a._z, _b );
  _w = _mm_sub_ps( a._w, _b );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128 _b = _mm_set_ps1( (XMabs(b)>XM2_FLOAT_ZERO) ? 1.0f/b : 0.0f );
  _x = _mm_mul_ps( a._x, _b );
  _y = _mm_mul_ps( a._y, _b );
  _z = _mm_mul_ps( a._z, _b );
  _w = _mm_mul_ps( a._w, _b );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128 _b = _mm_set_ps1(b);
  _x = _mm_mul_ps( a._x, _b );
  _y = _mm_mul_ps( a._y, _b );
  _z = _mm_mul_ps( a._z, _b );
  _w = _mm_mul_ps( a._w, _b );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::dot(XVector4& _dot,const XVector4SOA& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_dot);
  const __m128 x2 = _mm_mul_ps(_x,b._x);
  const __m128 y2 = _mm_mul_ps(_y,b._y);
  const __m128 z2 = _mm_mul_ps(_z,b._z);
  const __m128 w2 = _mm_mul_ps(_w,b._w);
  _dot.sse = _mm_add_ps( x2, _mm_add_ps( y2, _mm_add_ps( z2, w2 ) ) );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::cross(const XVector4SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps( _mm_mul_ps(a._y,b._z), _mm_mul_ps(a._z,b._y) );
  _y = _mm_sub_ps( _mm_mul_ps(a._z,b._x), _mm_mul_ps(a._x,b._z) );
  _z = _mm_sub_ps( _mm_mul_ps(a._x,b._y), _mm_mul_ps(a._y,b._x) );
  _w = _mm_setzero_ps();
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::length(XVector4& out_length) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&out_length);
  dot(out_length,*this);
  out_length.sse = _mm_sqrt_ps(out_length.sse);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::normalise()
{
  XM2_ALIGN_ASSERT(this);
  XVector4 l;
  length(l);
  const __m128 t = _mm_rcp_ps(l.sse);
  _x = _mm_mul_ps( _x, t );
  _y = _mm_mul_ps( _y, t );
  _z = _mm_mul_ps( _z, t );
  _w = _mm_mul_ps( _w, t );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::normalise(const XVector4SOA& a)
{
  XVector4 l;
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  a.length(l);

  // test if any of the lengths are zero
  const __m128 test = _mm_and_ps(_mm_cmpgt_ps(l.sse,_mm_sub_ps(_mm_setzero_ps(),_mm_set_ps1(XM2_FLOAT_ZERO))),
                                 _mm_cmplt_ps(l.sse,_mm_set_ps1(XM2_FLOAT_ZERO)));

  // replace any zero's with 1.0f
  l.sse = _mm_rcp_ps(_mm_or_ps(_mm_and_ps(test,_mm_set_ps1(1.0f)),
                               _mm_andnot_ps(test,l.sse)));
  _x = _mm_mul_ps( a._x, l.sse );
  _y = _mm_mul_ps( a._y, l.sse );
  _z = _mm_mul_ps( a._z, l.sse );
  _w = _mm_mul_ps( a._w, l.sse );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::lerp(const XVector4SOA& a,const XVector4SOA& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128 _t = _mm_set_ps1(t);
  _x = _mm_add_ps( a._x, _mm_mul_ps( _t, _mm_sub_ps(b._x,a._x) ) );
  _y = _mm_add_ps( a._y, _mm_mul_ps( _t, _mm_sub_ps(b._y,a._y) ) );
  _z = _mm_add_ps( a._z, _mm_mul_ps( _t, _mm_sub_ps(b._z,a._z) ) );
  _w = _mm_add_ps( a._w, _mm_mul_ps( _t, _mm_sub_ps(b._w,a._w) ) );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::bezier(const XVector4SOA& a,const XVector4SOA& b,
                                const XVector4SOA& c,const XVector4SOA& d,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&c);
  XM2_ALIGN_ASSERT(&d);
  XM2_ALIGN_ASSERT(this);
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
  _w = _mm_mul_ps( B0, a._w );
  _x = _mm_add_ps( _x, _mm_mul_ps( B1, b._x) );
  _y = _mm_add_ps( _y, _mm_mul_ps( B1, b._y) );
  _z = _mm_add_ps( _z, _mm_mul_ps( B1, b._z) );
  _w = _mm_add_ps( _w, _mm_mul_ps( B1, b._w) );
  _x = _mm_add_ps( _x, _mm_mul_ps( B2, c._x) );
  _y = _mm_add_ps( _y, _mm_mul_ps( B2, c._y) );
  _z = _mm_add_ps( _z, _mm_mul_ps( B2, c._z) );
  _w = _mm_add_ps( _w, _mm_mul_ps( B2, c._w) );
  _x = _mm_add_ps( _x, _mm_mul_ps( B3, d._x) );
  _y = _mm_add_ps( _y, _mm_mul_ps( B3, d._y) );
  _z = _mm_add_ps( _z, _mm_mul_ps( B3, d._z) );
  _w = _mm_add_ps( _w, _mm_mul_ps( B3, d._w) );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::negate(const XVector4SOA& a)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps(_mm_setzero_ps(),a._x);
  _y = _mm_sub_ps(_mm_setzero_ps(),a._y);
  _z = _mm_sub_ps(_mm_setzero_ps(),a._z);
  _w = _mm_sub_ps(_mm_setzero_ps(),a._w);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::negate()
{
  XM2_ALIGN_ASSERT(this);
  _x = _mm_sub_ps(_mm_setzero_ps(),_x);
  _y = _mm_sub_ps(_mm_setzero_ps(),_y);
  _z = _mm_sub_ps(_mm_setzero_ps(),_z);
  _w = _mm_sub_ps(_mm_setzero_ps(),_w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::set0(const XVector4& vec)
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
XM2_INLINE void XVector4SOA::set1(const XVector4& vec)
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
XM2_INLINE void XVector4SOA::set2(const XVector4& vec)
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
XM2_INLINE void XVector4SOA::set3(const XVector4& vec)
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
XM2_INLINE void XVector4SOA::get0(XVector4& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  const __m128 xy = _mm_shuffle_ps(_x,_y,_MM_SHUFFLE(0,0,0,0));
  const __m128 zw = _mm_shuffle_ps(_z,_w,_MM_SHUFFLE(0,0,0,0));
  vec.sse = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(2,0,2,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::get1(XVector4& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  const __m128 xy = _mm_shuffle_ps(_x,_y,_MM_SHUFFLE(1,1,1,1));
  const __m128 zw = _mm_shuffle_ps(_z,_w,_MM_SHUFFLE(1,1,1,1));
  vec.sse = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(2,0,2,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::get2(XVector4& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  const __m128 xy = _mm_shuffle_ps(_x,_y,_MM_SHUFFLE(2,2,2,2));
  const __m128 zw = _mm_shuffle_ps(_z,_w,_MM_SHUFFLE(2,2,2,2));
  vec.sse = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(2,0,2,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::get3(XVector4& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  const __m128 xy = _mm_shuffle_ps(_x,_y,_MM_SHUFFLE(3,3,3,3));
  const __m128 zw = _mm_shuffle_ps(_z,_w,_MM_SHUFFLE(3,3,3,3));
  vec.sse = _mm_shuffle_ps(xy,zw,_MM_SHUFFLE(2,0,2,0));
}
}
#endif
