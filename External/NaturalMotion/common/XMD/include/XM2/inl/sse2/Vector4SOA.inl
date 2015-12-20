#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_VECTOR4SOA__INL__
#define XM2_SSE2_VECTOR4SOA__INL__
#if XM2_USE_FLOAT
# error SSE2 double precision routines included when using float
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
#pragma warning(push)
#ifdef _MSC_VER
# pragma warning(disable:4328)
#endif
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA()
{
  XM2_ALIGN_ASSERT(this);
  #if XM2_INIT_CLASSES
  _x0 = _x1 = _y0 = _y1 = _z0 = _z1 = _w0 = _w1 =_mm_setzero_pd();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector3& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x0 = _x1 = _mm_shuffle_pd(a.xy,a.xy,_MM_SHUFFLE2(0,0));
  _y0 = _y1 = _mm_shuffle_pd(a.xy,a.xy,_MM_SHUFFLE2(1,1));
  _z0 = _z1 = _mm_shuffle_pd(a.zw,a.zw,_MM_SHUFFLE2(0,0));
  _w0 = _w1 = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector4& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x0 = _x1 = _mm_shuffle_pd(a.xy,a.xy,_MM_SHUFFLE2(0,0));
  _y0 = _y1 = _mm_shuffle_pd(a.xy,a.xy,_MM_SHUFFLE2(1,1));
  _z0 = _z1 = _mm_shuffle_pd(a.zw,a.zw,_MM_SHUFFLE2(0,0));
  _w0 = _w1 = _mm_shuffle_pd(a.zw,a.zw,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector3Packed& a)
{
  XM2_ALIGN_ASSERT(this);
  _x0 = _x1 = _mm_set1_pd(a.x);
  _y0 = _y1 = _mm_set1_pd(a.y);
  _z0 = _z1 = _mm_set1_pd(a.z);
  _w0 = _w1 = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector3SOA& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x0 = a._x0;
  _x1 = a._x1;
  _y0 = a._y0;
  _y1 = a._y1;
  _z0 = a._z0;
  _z1 = a._z1;
  _w0 = _w1 = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector4SOA::XVector4SOA(const XVector4SOA& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x0 = a._x0;
  _x1 = a._x1;
  _y0 = a._y0;
  _y1 = a._y1;
  _z0 = a._z0;
  _z1 = a._z1;
  _w0 = a._w0;
  _w1 = a._w1;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::lengthSquared(XVector4& _length_sqr) const
{
  XM2_ALIGN_ASSERT(&_length_sqr);
  XM2_ALIGN_ASSERT(this);
  dot(_length_sqr,*this);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::zero()
{
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_setzero_pd();
  _x1 = _mm_setzero_pd();
  _y0 = _mm_setzero_pd();
  _y1 = _mm_setzero_pd();
  _z0 = _mm_setzero_pd();
  _z1 = _mm_setzero_pd();
  _w0 = _mm_setzero_pd();
  _w1 = _mm_setzero_pd();
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set0(const XVector4& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x0 = _mm_shuffle_pd(vec.xy,_x0,_MM_SHUFFLE2(1,0));
  _y0 = _mm_shuffle_pd(vec.xy,_y0,_MM_SHUFFLE2(1,1));
  _z0 = _mm_shuffle_pd(vec.zw,_z0,_MM_SHUFFLE2(1,0));
  _w0 = _mm_shuffle_pd(vec.zw,_w0,_MM_SHUFFLE2(1,1));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set1(const XVector4& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x0 = _mm_shuffle_pd(_x0,vec.xy,_MM_SHUFFLE2(0,0));
  _y0 = _mm_shuffle_pd(_y0,vec.xy,_MM_SHUFFLE2(1,0));
  _z0 = _mm_shuffle_pd(_z0,vec.zw,_MM_SHUFFLE2(0,0));
  _w0 = _mm_shuffle_pd(_w0,vec.zw,_MM_SHUFFLE2(1,0));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set2(const XVector4& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x1 = _mm_shuffle_pd(vec.xy,_x1,_MM_SHUFFLE2(1,0));
  _y1 = _mm_shuffle_pd(vec.xy,_y1,_MM_SHUFFLE2(1,1));
  _z1 = _mm_shuffle_pd(vec.zw,_z1,_MM_SHUFFLE2(1,0));
  _w1 = _mm_shuffle_pd(vec.zw,_w1,_MM_SHUFFLE2(1,1));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::set3(const XVector4& vec)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  _x1 = _mm_shuffle_pd(_x1,vec.xy,_MM_SHUFFLE2(0,0));
  _y1 = _mm_shuffle_pd(_y1,vec.xy,_MM_SHUFFLE2(1,0));
  _z1 = _mm_shuffle_pd(_z1,vec.zw,_MM_SHUFFLE2(0,0));
  _w1 = _mm_shuffle_pd(_w1,vec.zw,_MM_SHUFFLE2(1,0));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::get0(XVector4& vec) const 
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  vec.xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(0,0));
  vec.zw = _mm_shuffle_pd(_z0,_w0,_MM_SHUFFLE2(0,0));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::get1(XVector4& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  vec.xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(1,1));
  vec.zw = _mm_shuffle_pd(_z0,_w0,_MM_SHUFFLE2(1,1));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::get2(XVector4& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  vec.xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(0,0));
  vec.zw = _mm_shuffle_pd(_z1,_w1,_MM_SHUFFLE2(0,0));
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::get3(XVector4& vec) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&vec);
  vec.xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(1,1));
  vec.zw = _mm_shuffle_pd(_z1,_w1,_MM_SHUFFLE2(1,1));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector3* input)
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
    _z0 = _mm_shuffle_pd(input[0].zw,input[1].zw,_MM_SHUFFLE2(0,0));
    _y1 = _mm_shuffle_pd(input[2].xy,input[3].xy,_MM_SHUFFLE2(1,1));
    _x1 = _mm_shuffle_pd(input[2].xy,input[3].xy,_MM_SHUFFLE2(0,0));
  }
  // transpose 4 in bottom right corner
  {
    _z1 = _mm_shuffle_pd(input[2].zw,input[3].zw,_MM_SHUFFLE2(0,0));
  }
  _w0 = _mm_setzero_pd();
  _w1 = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector4* input)
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
XM2_INLINE void XVector4SOA::streamIn(const XVector3SOA& input)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&input);
  _mm_stream_pd(x+0,input._x0);
  _mm_stream_pd(x+2,input._x1);
  _mm_stream_pd(y+0,input._y0);
  _mm_stream_pd(y+2,input._y1);
  _mm_stream_pd(z+0,input._z0);
  _mm_stream_pd(z+2,input._z1);
  _mm_stream_pd(w+0,_mm_setzero_pd());
  _mm_stream_pd(w+2,_mm_setzero_pd());
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamIn(const XVector3Packed* input)
{
  /// \todo
  x[0] = input[0].x; x[1] = input[1].x; x[2] = input[2].x; x[3] = input[3].x;
  y[0] = input[0].y; y[1] = input[1].y; y[2] = input[2].y; y[3] = input[3].y;
  z[0] = input[0].z; z[1] = input[1].z; z[2] = input[2].z; z[3] = input[3].z;
  w[0]=w[1]=w[2]=w[3]=0;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector3* output) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(output);
  const __m128d zero = _mm_setzero_pd();

  // transpose 4 in top left corner
  {
    output[0].xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(0,0));
    output[1].xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(1,1));
  }
  // transpose top right and bottom left corners, then assign on top of each other
  {
    output[0].zw = _mm_shuffle_pd(_z0,zero,_MM_SHUFFLE2(0,0)); // 9 13
    output[1].zw = _mm_shuffle_pd(_z0,zero,_MM_SHUFFLE2(1,1)); // 10 14
    output[2].xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(0,0)); // 3 7
    output[3].xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(1,1));; // 4 8
  }
  // transpose 4 in bottom right corner
  {
    output[3].zw = _mm_shuffle_pd(_z1,zero,_MM_SHUFFLE2(1,1));
    output[2].zw = _mm_shuffle_pd(_z1,zero,_MM_SHUFFLE2(0,0));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector4* output) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(output);
  // transpose 4 in top left corner
  {
    output[0].xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(0,0));
    output[1].xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(1,1));
  }
  // transpose top right and bottom left corners, then assign on top of each other
  {
    output[0].zw = _mm_shuffle_pd(_z0,_w0,_MM_SHUFFLE2(0,0)); // 9 13
    output[1].zw = _mm_shuffle_pd(_z0,_w0,_MM_SHUFFLE2(1,1)); // 10 14
    output[2].xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(0,0)); // 3 7
    output[3].xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(1,1));; // 4 8
  }
  // transpose 4 in bottom right corner
  {
    output[3].zw = _mm_shuffle_pd(_z1,_w1,_MM_SHUFFLE2(1,1));
    output[2].zw = _mm_shuffle_pd(_z1,_w1,_MM_SHUFFLE2(0,0));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector3SOA& output) const
{
  XM2_ALIGN_ASSERT(&output);
  XM2_ALIGN_ASSERT(this);
  _mm_stream_pd(output.x+0,_x0);
  _mm_stream_pd(output.x+2,_x1);
  _mm_stream_pd(output.y+0,_y0);
  _mm_stream_pd(output.y+2,_y1);
  _mm_stream_pd(output.z+0,_z0);
  _mm_stream_pd(output.z+2,_z1);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector4SOA::streamOut(XVector3Packed* output) const
{
  /// \todo
  output[0].x = x[0]; output[1].x = x[1]; output[2].x = x[2]; output[3].x = x[3];
  output[0].y = y[0]; output[1].y = y[1]; output[2].y = y[2]; output[3].y = y[3];
  output[0].z = z[0]; output[1].z = z[1]; output[2].z = z[2]; output[3].z = z[3];
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector4SOA::add(const XVector4SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_add_pd( a._x0, b._x0 );
  _x1 = _mm_add_pd( a._x1, b._x1 );
  _y0 = _mm_add_pd( a._y0, b._y0 );
  _y1 = _mm_add_pd( a._y1, b._y1 );
  _z0 = _mm_add_pd( a._z0, b._z0 );
  _z1 = _mm_add_pd( a._z1, b._z1 );
  _w0 = _mm_add_pd( a._w0, b._w0 );
  _w1 = _mm_add_pd( a._w1, b._w1 );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_sub_pd( a._x0, b._x0 );
  _x1 = _mm_sub_pd( a._x1, b._x1 );
  _y0 = _mm_sub_pd( a._y0, b._y0 );
  _y1 = _mm_sub_pd( a._y1, b._y1 );
  _z0 = _mm_sub_pd( a._z0, b._z0 );
  _z1 = _mm_sub_pd( a._z1, b._z1 );
  _w0 = _mm_sub_pd( a._w0, b._w0 );
  _w1 = _mm_sub_pd( a._w1, b._w1 );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_mul_pd( a._x0, b._x0 );
  _x1 = _mm_mul_pd( a._x1, b._x1 );
  _y0 = _mm_mul_pd( a._y0, b._y0 );
  _y1 = _mm_mul_pd( a._y1, b._y1 );
  _z0 = _mm_mul_pd( a._z0, b._z0 );
  _z1 = _mm_mul_pd( a._z1, b._z1 );
  _w0 = _mm_mul_pd( a._w0, b._w0 );
  _w1 = _mm_mul_pd( a._w1, b._w1 );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_div_pd( a._x0, b._x0 );
  _x1 = _mm_div_pd( a._x1, b._x1 );
  _y0 = _mm_div_pd( a._y0, b._y0 );
  _y1 = _mm_div_pd( a._y1, b._y1 );
  _z0 = _mm_div_pd( a._z0, b._z0 );
  _z1 = _mm_div_pd( a._z1, b._z1 );
  _w0 = _mm_div_pd( a._w0, b._w0 );
  _w1 = _mm_div_pd( a._w1, b._w1 );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XVector4SOA& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d __x = _mm_set1_pd(b.x);
  const __m128d __y = _mm_set1_pd(b.y);
  const __m128d __z = _mm_set1_pd(b.z);
  const __m128d __w = _mm_set1_pd(b.w);
  _x0 = _mm_add_pd( a._x0, __x );
  _x1 = _mm_add_pd( a._x1, __x );
  _y0 = _mm_add_pd( a._y0, __y );
  _y1 = _mm_add_pd( a._y1, __y );
  _z0 = _mm_add_pd( a._z0, __z );
  _z1 = _mm_add_pd( a._z1, __z );
  _w0 = _mm_add_pd( a._w0, __w );
  _w1 = _mm_add_pd( a._w1, __w );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4SOA& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d __x = _mm_set1_pd(b.x);
  const __m128d __y = _mm_set1_pd(b.y);
  const __m128d __z = _mm_set1_pd(b.z);
  const __m128d __w = _mm_set1_pd(b.w);
  _x0 = _mm_sub_pd( a._x0, __x );
  _x1 = _mm_sub_pd( a._x1, __x );
  _y0 = _mm_sub_pd( a._y0, __y );
  _y1 = _mm_sub_pd( a._y1, __y );
  _z0 = _mm_sub_pd( a._z0, __z );
  _z1 = _mm_sub_pd( a._z1, __z );
  _w0 = _mm_sub_pd( a._w0, __w );
  _w1 = _mm_sub_pd( a._w1, __w );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4SOA& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d __x = _mm_set1_pd(b.x);
  const __m128d __y = _mm_set1_pd(b.y);
  const __m128d __z = _mm_set1_pd(b.z);
  const __m128d __w = _mm_set1_pd(b.w);
  _x0 = _mm_mul_pd( a._x0, __x );
  _x1 = _mm_mul_pd( a._x1, __x );
  _y0 = _mm_mul_pd( a._y0, __y );
  _y1 = _mm_mul_pd( a._y1, __y );
  _z0 = _mm_mul_pd( a._z0, __z );
  _z1 = _mm_mul_pd( a._z1, __z );
  _w0 = _mm_mul_pd( a._w0, __w );
  _w1 = _mm_mul_pd( a._w1, __w );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4SOA& a,const XVector4& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d __x = _mm_set1_pd(b.x);
  const __m128d __y = _mm_set1_pd(b.y);
  const __m128d __z = _mm_set1_pd(b.z);
  const __m128d __w = _mm_set1_pd(b.w);
  _x0 = _mm_div_pd( a._x0, __x );
  _x1 = _mm_div_pd( a._x1, __x );
  _y0 = _mm_div_pd( a._y0, __y );
  _y1 = _mm_div_pd( a._y1, __y );
  _z0 = _mm_div_pd( a._z0, __z );
  _z1 = _mm_div_pd( a._z1, __z );
  _w0 = _mm_div_pd( a._w0, __w );
  _w1 = _mm_div_pd( a._w1, __w );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::add(const XVector4SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d _b = _mm_set1_pd(b);
  _x0 = _mm_add_pd( a._x0, _b );
  _x1 = _mm_add_pd( a._x1, _b );
  _y0 = _mm_add_pd( a._y0, _b );
  _y1 = _mm_add_pd( a._y1, _b );
  _z0 = _mm_add_pd( a._z0, _b );
  _z1 = _mm_add_pd( a._z1, _b );
  _w0 = _mm_add_pd( a._w0, _b );
  _w1 = _mm_add_pd( a._w1, _b );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::sub(const XVector4SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d _b = _mm_set1_pd(b);
  _x0 = _mm_sub_pd( a._x0, _b );
  _x1 = _mm_sub_pd( a._x1, _b );
  _y0 = _mm_sub_pd( a._y0, _b );
  _y1 = _mm_sub_pd( a._y1, _b );
  _z0 = _mm_sub_pd( a._z0, _b );
  _z1 = _mm_sub_pd( a._z1, _b );
  _w0 = _mm_sub_pd( a._w0, _b );
  _w1 = _mm_sub_pd( a._w1, _b );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::div(const XVector4SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d _b = _mm_set1_pd( (XMabs(b)>XM2_FLOAT_ZERO) ? 1.0/b : 0.0 );
  _x0 = _mm_mul_pd( a._x0, _b );
  _x1 = _mm_mul_pd( a._x1, _b );
  _y0 = _mm_mul_pd( a._y0, _b );
  _y1 = _mm_mul_pd( a._y1, _b );
  _z0 = _mm_mul_pd( a._z0, _b );
  _z1 = _mm_mul_pd( a._z1, _b );
  _w0 = _mm_mul_pd( a._w0, _b );
  _w1 = _mm_mul_pd( a._w1, _b );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::mul(const XVector4SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d _b = _mm_set1_pd( b );
  _x0 = _mm_mul_pd( a._x0, _b );
  _x1 = _mm_mul_pd( a._x1, _b );
  _y0 = _mm_mul_pd( a._y0, _b );
  _y1 = _mm_mul_pd( a._y1, _b );
  _z0 = _mm_mul_pd( a._z0, _b );
  _z1 = _mm_mul_pd( a._z1, _b );
  _w0 = _mm_mul_pd( a._w0, _b );
  _w1 = _mm_mul_pd( a._w1, _b );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::dot(XVector4& _dot,const XVector4SOA& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&_dot);
  const __m128d x2_0 = _mm_mul_pd(_x0,b._x0);
  const __m128d x2_1 = _mm_mul_pd(_x1,b._x1);
  const __m128d y2_0 = _mm_mul_pd(_y0,b._y0);
  const __m128d y2_1 = _mm_mul_pd(_y1,b._y1);
  const __m128d z2_0 = _mm_mul_pd(_z0,b._z0);
  const __m128d z2_1 = _mm_mul_pd(_z1,b._z1);
  const __m128d w2_0 = _mm_mul_pd(_w0,b._w0);
  const __m128d w2_1 = _mm_mul_pd(_w1,b._w1);
  _dot.xy = _mm_add_pd( x2_0, _mm_add_pd( y2_0, _mm_add_pd( z2_0, w2_0 ) ) );
  _dot.zw = _mm_add_pd( x2_1, _mm_add_pd( y2_1, _mm_add_pd( z2_1, w2_1 ) ) );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::cross(const XVector4SOA& a,const XVector4SOA& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_sub_pd( _mm_mul_pd(a._y0,b._z0), _mm_mul_pd(a._z0,b._y0) );
  _x1 = _mm_sub_pd( _mm_mul_pd(a._y1,b._z1), _mm_mul_pd(a._z1,b._y1) );
  _y0 = _mm_sub_pd( _mm_mul_pd(a._z0,b._x0), _mm_mul_pd(a._x0,b._z0) );
  _y1 = _mm_sub_pd( _mm_mul_pd(a._z1,b._x1), _mm_mul_pd(a._x1,b._z1) );
  _z0 = _mm_sub_pd( _mm_mul_pd(a._x0,b._y0), _mm_mul_pd(a._y0,b._x0) );
  _z1 = _mm_sub_pd( _mm_mul_pd(a._x1,b._y1), _mm_mul_pd(a._y1,b._x1) );
  _w0 = _mm_setzero_pd();
  _w1 = _mm_setzero_pd();
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::length(XVector4& out_length) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&out_length);
  dot(out_length,*this);
  out_length.xy = _mm_sqrt_pd(out_length.xy);
  out_length.zw = _mm_sqrt_pd(out_length.zw);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::normalise()
{
  XM2_ALIGN_ASSERT(this);
  XVector4 l;
  length(l);
  const __m128d txy = _mm_div_pd(_mm_set1_pd(1.0),l.xy);
  const __m128d tzw = _mm_div_pd(_mm_set1_pd(1.0),l.zw);
  _x0 = _mm_mul_pd( _x0, txy );
  _x1 = _mm_mul_pd( _x1, tzw );
  _y0 = _mm_mul_pd( _y0, txy );
  _y1 = _mm_mul_pd( _y1, tzw );
  _z0 = _mm_mul_pd( _z0, txy );
  _z1 = _mm_mul_pd( _z1, tzw );
  _w0 = _mm_mul_pd( _w0, txy );
  _w1 = _mm_mul_pd( _w1, tzw );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::normalise(const XVector4SOA& a)
{
  XVector4 l;
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  a.length(l);
  
  
  // test if any of the lengths are zero
  __m128d test = _mm_and_pd(_mm_cmpgt_pd(l.xy,_mm_sub_pd(_mm_setzero_pd(),_mm_set1_pd(XM2_FLOAT_ZERO))),
                            _mm_cmplt_pd(l.xy,_mm_set1_pd(XM2_FLOAT_ZERO)));

  // replace any zero's with 1.0f
  l.xy = _mm_or_pd(_mm_and_pd(test,_mm_set1_pd(1.0f)),
                   _mm_andnot_pd(test,l.xy));

  _x0 = _mm_div_pd(a._x0,l.xy);
  _y0 = _mm_div_pd(a._y0,l.xy);
  _z0 = _mm_div_pd(a._z0,l.xy);
  _w0 = _mm_div_pd(a._w0,l.xy);
  
  test = _mm_and_pd(_mm_cmpgt_pd(l.zw,_mm_sub_pd(_mm_setzero_pd(),_mm_set1_pd(XM2_FLOAT_ZERO))),
                    _mm_cmplt_pd(l.zw,_mm_set1_pd(XM2_FLOAT_ZERO)));

  // replace any zero's with 1.0f
  l.zw = _mm_or_pd(_mm_and_pd(test,_mm_set1_pd(1.0f)),
                   _mm_andnot_pd(test,l.zw));

  _x1 = _mm_div_pd(a._x1,l.zw);
  _y1 = _mm_div_pd(a._y1,l.zw);
  _z1 = _mm_div_pd(a._z1,l.zw);
  _w1 = _mm_div_pd(a._w1,l.zw);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::lerp(const XVector4SOA& a,const XVector4SOA& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  __m128d _t = _mm_set1_pd(t);
  _x0 = _mm_add_pd( a._x0, _mm_mul_pd( _t, _mm_sub_pd(b._x0,a._x0) ) );
  _x1 = _mm_add_pd( a._x1, _mm_mul_pd( _t, _mm_sub_pd(b._x1,a._x1) ) );
  _y0 = _mm_add_pd( a._y0, _mm_mul_pd( _t, _mm_sub_pd(b._y0,a._y0) ) );
  _y1 = _mm_add_pd( a._y1, _mm_mul_pd( _t, _mm_sub_pd(b._y1,a._y1) ) );
  _z0 = _mm_add_pd( a._z0, _mm_mul_pd( _t, _mm_sub_pd(b._z0,a._z0) ) );
  _z1 = _mm_add_pd( a._z1, _mm_mul_pd( _t, _mm_sub_pd(b._z1,a._z1) ) );
  _w0 = _mm_add_pd( a._w0, _mm_mul_pd( _t, _mm_sub_pd(b._w0,a._w0) ) );
  _w1 = _mm_add_pd( a._w1, _mm_mul_pd( _t, _mm_sub_pd(b._w1,a._w1) ) );
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
  const XReal t2 = t*t;
  const XReal t3 = t2*t;
  const XReal it = 1.0f - t;
  const XReal it2 = it*it;
  const XReal it3 = it*it2;
  const __m128d B0 = _mm_set1_pd(it3);
  const __m128d B1 = _mm_set1_pd(3.0f*it2*t);
  const __m128d B2 = _mm_set1_pd(3.0f*it*t2);
  const __m128d B3 = _mm_set1_pd(t3);
  _x0 = _mm_mul_pd( B0, a._x0 );
  _x1 = _mm_mul_pd( B0, a._x1 );
  _y0 = _mm_mul_pd( B0, a._y0 );
  _y1 = _mm_mul_pd( B0, a._y1 );
  _z0 = _mm_mul_pd( B0, a._z0 );
  _z1 = _mm_mul_pd( B0, a._z1 );
  _w0 = _mm_mul_pd( B0, a._w0 );
  _w1 = _mm_mul_pd( B0, a._w1 );
  _x0 = _mm_add_pd( _x0, _mm_mul_pd( B1, b._x0) );
  _x1 = _mm_add_pd( _x1, _mm_mul_pd( B1, b._x1) );
  _y0 = _mm_add_pd( _y0, _mm_mul_pd( B1, b._y0) );
  _y1 = _mm_add_pd( _y1, _mm_mul_pd( B1, b._y1) );
  _z0 = _mm_add_pd( _z0, _mm_mul_pd( B1, b._z0) );
  _z1 = _mm_add_pd( _z1, _mm_mul_pd( B1, b._z1) );
  _w0 = _mm_add_pd( _w0, _mm_mul_pd( B1, b._w0) );
  _w1 = _mm_add_pd( _w1, _mm_mul_pd( B1, b._w1) );
  _x0 = _mm_add_pd( _x0, _mm_mul_pd( B2, c._x0) );
  _x1 = _mm_add_pd( _x1, _mm_mul_pd( B2, c._x1) );
  _y0 = _mm_add_pd( _y0, _mm_mul_pd( B2, c._y0) );
  _y1 = _mm_add_pd( _y1, _mm_mul_pd( B2, c._y1) );
  _z0 = _mm_add_pd( _z0, _mm_mul_pd( B2, c._z0) );
  _z1 = _mm_add_pd( _z1, _mm_mul_pd( B2, c._z1) );
  _w0 = _mm_add_pd( _w0, _mm_mul_pd( B2, c._w0) );
  _w1 = _mm_add_pd( _w1, _mm_mul_pd( B2, c._w1) );
  _x0 = _mm_add_pd( _x0, _mm_mul_pd( B3, d._x0) );
  _x1 = _mm_add_pd( _x1, _mm_mul_pd( B3, d._x1) );
  _y0 = _mm_add_pd( _y0, _mm_mul_pd( B3, d._y0) );
  _y1 = _mm_add_pd( _y1, _mm_mul_pd( B3, d._y1) );
  _z0 = _mm_add_pd( _z0, _mm_mul_pd( B3, d._z0) );
  _z1 = _mm_add_pd( _z1, _mm_mul_pd( B3, d._z1) );
  _w0 = _mm_add_pd( _w0, _mm_mul_pd( B3, d._w0) );
  _w1 = _mm_add_pd( _w1, _mm_mul_pd( B3, d._w1) );
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::negate(const XVector4SOA& a)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  __m128d _inv = _mm_set1_pd(-1.0f);
  _x0 = _mm_mul_pd(_inv,a._x0);
  _x1 = _mm_mul_pd(_inv,a._x1);
  _y0 = _mm_mul_pd(_inv,a._y0);
  _y1 = _mm_mul_pd(_inv,a._y1);
  _z0 = _mm_mul_pd(_inv,a._z0);
  _z1 = _mm_mul_pd(_inv,a._z1);
  _w0 = _mm_mul_pd(_inv,a._w0);
  _w1 = _mm_mul_pd(_inv,a._w1);
}
//---------------------------------------------------------------------------------------------------------------------- 
XM2_INLINE void XVector4SOA::negate()
{
  XM2_ALIGN_ASSERT(this);
  __m128d _inv = _mm_set1_pd(-1.0f);
  _x0 = _mm_mul_pd(_inv,_x0);
  _x1 = _mm_mul_pd(_inv,_x1);
  _y0 = _mm_mul_pd(_inv,_y0);
  _y1 = _mm_mul_pd(_inv,_y1);
  _z0 = _mm_mul_pd(_inv,_z0);
  _z1 = _mm_mul_pd(_inv,_z1);
  _w0 = _mm_mul_pd(_inv,_w0);
  _w1 = _mm_mul_pd(_inv,_w1);
}
#pragma warning(pop)
}
#endif
