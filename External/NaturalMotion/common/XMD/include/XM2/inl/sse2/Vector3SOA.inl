#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_VECTOR3SOA__INL__
#define XM2_SSE2_VECTOR3SOA__INL__
#if (XM2_USE_FLOAT)
# error SSE2 double precision routines included when set to build using floats
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
XM2_INLINE XVector3SOA::XVector3SOA()
{
  XM2_ALIGN_ASSERT(this);
  #if XM2_INIT_CLASSES
  _x0 = _x1 = _y0 = _y1 = _z0 = _z1 = _mm_setzero_pd();
  #endif
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector3& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x0 = _x1 = _mm_shuffle_pd(a.xy,a.xy,_MM_SHUFFLE2(0,0));
  _y0 = _y1 = _mm_shuffle_pd(a.xy,a.xy,_MM_SHUFFLE2(1,1));
  _z0 = _z1 = _mm_shuffle_pd(a.zw,a.zw,_MM_SHUFFLE2(0,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector4& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x0 = _x1 = _mm_shuffle_pd(a.xy,a.xy,_MM_SHUFFLE2(0,0));
  _y0 = _y1 = _mm_shuffle_pd(a.xy,a.xy,_MM_SHUFFLE2(1,1));
  _z0 = _z1 = _mm_shuffle_pd(a.zw,a.zw,_MM_SHUFFLE2(0,0));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector3Packed& a)
{
  XM2_ALIGN_ASSERT(this);
  _x0 = _x1 = _mm_set1_pd(a.x);
  _y0 = _y1 = _mm_set1_pd(a.y);
  _z0 = _z1 = _mm_set1_pd(a.z);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector3SOA& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x0 = a._x0;
  _x1 = a._x1;
  _y0 = a._y0;
  _y1 = a._y1;
  _z0 = a._z0;
  _z1 = a._z1;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XVector3SOA::XVector3SOA(const XVector4SOA& a)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&a);
  _x0 = a._x0;
  _x1 = a._x1;
  _y0 = a._y0;
  _y1 = a._y1;
  _z0 = a._z0;
  _z1 = a._z1;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::zero()
{
  XM2_ALIGN_ASSERT(this);
  _x0 = _mm_setzero_pd();
  _x1 = _mm_setzero_pd();
  _y0 = _mm_setzero_pd();
  _y1 = _mm_setzero_pd();
  _z0 = _mm_setzero_pd();
  _z1 = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector3* input)
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
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector3* input) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  __m128d _w = _mm_set1_pd(1.0f);
  // transpose 4 in top left corner
  {
    input[0].xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(0,0));
    input[1].xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(1,1));
  }
  // transpose top right and bottom left corners, then assign on top of each other
  {
    input[0].zw = _mm_shuffle_pd(_z0,_w,_MM_SHUFFLE2(0,0)); // 9 13
    input[1].zw = _mm_shuffle_pd(_z0,_w,_MM_SHUFFLE2(1,1)); // 10 14
    input[2].xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(0,0)); // 3 7
    input[3].xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(1,1));; // 4 8
  }
  // transpose 4 in bottom right corner
  {
    input[3].zw = _mm_shuffle_pd(_z1,_w,_MM_SHUFFLE2(1,1));
    input[2].zw = _mm_shuffle_pd(_z1,_w,_MM_SHUFFLE2(0,0));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector4* input)
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
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector4* input) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);
  const __m128d _w = _mm_setzero_pd();
  // transpose 4 in top left corner
  {
    input[0].xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(0,0));
    input[1].xy = _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(1,1));
  }
  // transpose top right and bottom left corners, then assign on top of each other
  {
    input[0].zw = _mm_shuffle_pd(_z0,_w,_MM_SHUFFLE2(0,0)); // 9 13
    input[1].zw = _mm_shuffle_pd(_z0,_w,_MM_SHUFFLE2(1,1)); // 10 14
    input[2].xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(0,0)); // 3 7
    input[3].xy = _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(1,1));; // 4 8
  }
  // transpose 4 in bottom right corner
  {
    input[3].zw = _mm_shuffle_pd(_z1,_w,_MM_SHUFFLE2(1,1));
    input[2].zw = _mm_shuffle_pd(_z1,_w,_MM_SHUFFLE2(0,0));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector4SOA& input)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&input);
  _x0 = input._x0;
  _x1 = input._x1;
  _y0 = input._y0;
  _y1 = input._y1;
  _z0 = input._z0;
  _z1 = input._z1;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector4SOA& output) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&output);
  output._x0 = _x0;
  output._x1 = _x1;
  output._y0 = _y0;
  output._y1 = _y1;
  output._z0 = _z0;
  output._z1 = _z1;
  output._w0 = _mm_setzero_pd();
  output._w1 = _mm_setzero_pd();
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamIn(const XVector3Packed* input)
{
  XM2_ASSERT(input);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(input);

  //  x0 y0 z0 x1
  const __m128d a0 = _mm_load_pd(input->data+0);
  const __m128d a1 = _mm_load_pd(input->data+2);

  //  y1 z1 x2 y2
  const __m128d b0 = _mm_load_pd(input->data+4);
  const __m128d b1 = _mm_load_pd(input->data+6);

  //  z2 x3 y3 z3
  const __m128d c0 = _mm_load_pd(input->data+8);
  const __m128d c1 = _mm_load_pd(input->data+10);

  _mm_stream_pd(x+0,_mm_shuffle_pd(a0,a1,_MM_SHUFFLE2(1,0)));
  _mm_stream_pd(x+2,_mm_shuffle_pd(b1,c0,_MM_SHUFFLE2(1,0)));
  _mm_stream_pd(y+0,_mm_shuffle_pd(a0,b0,_MM_SHUFFLE2(0,1)));
  _mm_stream_pd(y+2,_mm_shuffle_pd(b1,c1,_MM_SHUFFLE2(0,1)));
  _mm_stream_pd(z+0,_mm_shuffle_pd(a1,b0,_MM_SHUFFLE2(1,0)));
  _mm_stream_pd(z+2,_mm_shuffle_pd(c0,c1,_MM_SHUFFLE2(1,0)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::streamOut(XVector3Packed* output) const
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(output);
  _mm_stream_pd(output->data+0,  _mm_shuffle_pd(_x0,_y0,_MM_SHUFFLE2(0,0)));
  _mm_stream_pd(output->data+2,  _mm_shuffle_pd(_z0,_x0,_MM_SHUFFLE2(1,0)));
  _mm_stream_pd(output->data+4,  _mm_shuffle_pd(_y0,_z0,_MM_SHUFFLE2(1,1)));
  _mm_stream_pd(output->data+6,  _mm_shuffle_pd(_x1,_y1,_MM_SHUFFLE2(0,0)));
  _mm_stream_pd(output->data+8,  _mm_shuffle_pd(_z1,_x1,_MM_SHUFFLE2(1,0)));
  _mm_stream_pd(output->data+10, _mm_shuffle_pd(_y1,_z1,_MM_SHUFFLE2(1,1)));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::lengthSquared(XVector4& _length_sqr) const
{
  XM2_ALIGN_ASSERT(&_length_sqr);
  XM2_ALIGN_ASSERT(this);
  dot(_length_sqr,*this);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XVector3SOA::normalise(const XVector3SOA& a)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  XVector4 l;
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
  
  test = _mm_and_pd(_mm_cmpgt_pd(l.zw,_mm_sub_pd(_mm_setzero_pd(),_mm_set1_pd(XM2_FLOAT_ZERO))),
                    _mm_cmplt_pd(l.zw,_mm_set1_pd(XM2_FLOAT_ZERO)));

  // replace any zero's with 1.0f
  l.zw = _mm_or_pd(_mm_and_pd(test,_mm_set1_pd(1.0f)),
                   _mm_andnot_pd(test,l.zw));

  _x1 = _mm_div_pd(a._x1,l.zw);
  _y1 = _mm_div_pd(a._y1,l.zw);
  _z1 = _mm_div_pd(a._z1,l.zw);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XVector3SOA& b)
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
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XVector3SOA& b)
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
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XVector3SOA& b)
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
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XVector3SOA& b)
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
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128d bx = _mm_set1_pd(b.x);
  const __m128d by = _mm_set1_pd(b.y);
  const __m128d bz = _mm_set1_pd(b.z);
  _x0 = _mm_add_pd( a._x0, bx );
  _x1 = _mm_add_pd( a._x1, bx );
  _y0 = _mm_add_pd( a._y0, by );
  _y1 = _mm_add_pd( a._y1, by );
  _z0 = _mm_add_pd( a._z0, bz );
  _z1 = _mm_add_pd( a._z1, bz );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128d bx = _mm_set1_pd(b.x);
  const __m128d by = _mm_set1_pd(b.y);
  const __m128d bz = _mm_set1_pd(b.z);
  _x0 = _mm_sub_pd( a._x0, bx );
  _x1 = _mm_sub_pd( a._x1, bx );
  _y0 = _mm_sub_pd( a._y0, by );
  _y1 = _mm_sub_pd( a._y1, by );
  _z0 = _mm_sub_pd( a._z0, bz );
  _z1 = _mm_sub_pd( a._z1, bz );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128d bx = _mm_set1_pd(b.x);
  const __m128d by = _mm_set1_pd(b.y);
  const __m128d bz = _mm_set1_pd(b.z);
  _x0 = _mm_mul_pd( a._x0, bx );
  _x1 = _mm_mul_pd( a._x1, bx );
  _y0 = _mm_mul_pd( a._y0, by );
  _y1 = _mm_mul_pd( a._y1, by );
  _z0 = _mm_mul_pd( a._z0, bz );
  _z1 = _mm_mul_pd( a._z1, bz );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XVector3& b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128d bx = _mm_set1_pd(b.x);
  const __m128d by = _mm_set1_pd(b.y);
  const __m128d bz = _mm_set1_pd(b.z);
  _x0 = _mm_div_pd( a._x0, bx );
  _x1 = _mm_div_pd( a._x1, bx );
  _y0 = _mm_div_pd( a._y0, by );
  _y1 = _mm_div_pd( a._y1, by );
  _z0 = _mm_div_pd( a._z0, bz );
  _z1 = _mm_div_pd( a._z1, bz );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XVector4SOA& b)
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
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XVector4SOA& b)
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
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XVector4SOA& b)
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
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XVector4SOA& b)
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
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::add(const XVector3SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(b);
  _x0 = _mm_add_pd( a._x0, temp );
  _x1 = _mm_add_pd( a._x1, temp );
  _y0 = _mm_add_pd( a._y0, temp );
  _y1 = _mm_add_pd( a._y1, temp );
  _z0 = _mm_add_pd( a._z0, temp );
  _z1 = _mm_add_pd( a._z1, temp );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::sub(const XVector3SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(b);
  _x0 = _mm_sub_pd( a._x0, temp );
  _x1 = _mm_sub_pd( a._x1, temp );
  _y0 = _mm_sub_pd( a._y0, temp );
  _y1 = _mm_sub_pd( a._y1, temp );
  _z0 = _mm_sub_pd( a._z0, temp );
  _z1 = _mm_sub_pd( a._z1, temp );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::mul(const XVector3SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd(b);
  _x0 = _mm_mul_pd( a._x0, temp );
  _x1 = _mm_mul_pd( a._x1, temp );
  _y0 = _mm_mul_pd( a._y0, temp );
  _y1 = _mm_mul_pd( a._y1, temp );
  _z0 = _mm_mul_pd( a._z0, temp );
  _z1 = _mm_mul_pd( a._z1, temp );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::div(const XVector3SOA& a,const XReal b)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d temp = _mm_set1_pd((XMabs(b)<XM2_FLOAT_ZERO) ? 0 : (1.0/b));
  _x0 = _mm_mul_pd( a._x0, temp );
  _x1 = _mm_mul_pd( a._x1, temp );
  _y0 = _mm_mul_pd( a._y0, temp );
  _y1 = _mm_mul_pd( a._y1, temp );
  _z0 = _mm_mul_pd( a._z0, temp );
  _z1 = _mm_mul_pd( a._z1, temp );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::dot(XVector4& _dot,const XVector3SOA& b) const
{
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&_dot);
  XM2_ALIGN_ASSERT(this);
  const __m128d x20 = _mm_mul_pd(_x0,b._x0);
  const __m128d x21 = _mm_mul_pd(_x1,b._x1);
  const __m128d y20 = _mm_mul_pd(_y0,b._y0);
  const __m128d y21 = _mm_mul_pd(_y1,b._y1);
  const __m128d z20 = _mm_mul_pd(_z0,b._z0);
  const __m128d z21 = _mm_mul_pd(_z1,b._z1);
  _dot.xy = _mm_add_pd( x20, _mm_add_pd( y20, z20 ) );
  _dot.zw = _mm_add_pd( x21, _mm_add_pd( y21, z21 ) );
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::cross(const XVector3SOA& a,const XVector3SOA& b)
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
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::length(XVector4& out_length) const
{
  XM2_ALIGN_ASSERT(this);
  dot(out_length,*this);
  out_length.xy = _mm_sqrt_pd(out_length.xy);
  out_length.zw = _mm_sqrt_pd(out_length.zw);
}//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::lerp(const XVector3SOA& a,const XVector3SOA& b,const XReal t)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  const __m128d _t = _mm_set1_pd(t);
  _x0 = _mm_add_pd( a._x0, _mm_mul_pd( _t, _mm_sub_pd(b._x0,a._x0) ) );
  _x1 = _mm_add_pd( a._x1, _mm_mul_pd( _t, _mm_sub_pd(b._x1,a._x1) ) );
  _y0 = _mm_add_pd( a._y0, _mm_mul_pd( _t, _mm_sub_pd(b._y0,a._y0) ) );
  _y1 = _mm_add_pd( a._y1, _mm_mul_pd( _t, _mm_sub_pd(b._y1,a._y1) ) );
  _z0 = _mm_add_pd( a._z0, _mm_mul_pd( _t, _mm_sub_pd(b._z0,a._z0) ) );
  _z1 = _mm_add_pd( a._z1, _mm_mul_pd( _t, _mm_sub_pd(b._z1,a._z1) ) );
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
  const __m128d _t = _mm_set1_pd(t);
  {
    const __m128d x_ab0  = _mm_add_pd( a._x0, _mm_mul_pd(_t,_mm_sub_pd(b._x0,a._x0)) );
    const __m128d x_bc0  = _mm_add_pd( b._x0, _mm_mul_pd(_t,_mm_sub_pd(c._x0,b._x0)) );
    const __m128d x_cd0  = _mm_add_pd( c._x0, _mm_mul_pd(_t,_mm_sub_pd(d._x0,c._x0)) );
    const __m128d x_abc0 = _mm_add_pd( x_ab0, _mm_mul_pd(_t,_mm_sub_pd(x_bc0,x_ab0)) );
    const __m128d x_bcd0 = _mm_add_pd( x_bc0, _mm_mul_pd(_t,_mm_sub_pd(x_cd0,x_bc0)) );
    _x0 = _mm_add_pd( x_abc0, _mm_mul_pd(_t,_mm_sub_pd(x_bcd0,x_abc0)) );
  }
  {
    const __m128d x_ab1  = _mm_add_pd( a._x1, _mm_mul_pd(_t,_mm_sub_pd(b._x1,a._x1)) );
    const __m128d x_bc1  = _mm_add_pd( b._x1, _mm_mul_pd(_t,_mm_sub_pd(c._x1,b._x1)) );
    const __m128d x_cd1  = _mm_add_pd( c._x1, _mm_mul_pd(_t,_mm_sub_pd(d._x1,c._x1)) );
    const __m128d x_abc1 = _mm_add_pd( x_ab1, _mm_mul_pd(_t,_mm_sub_pd(x_bc1,x_ab1)) );
    const __m128d x_bcd1 = _mm_add_pd( x_bc1, _mm_mul_pd(_t,_mm_sub_pd(x_cd1,x_bc1)) );
    _x1 = _mm_add_pd( x_abc1, _mm_mul_pd(_t,_mm_sub_pd(x_bcd1,x_abc1)) );
  }
  {
    const __m128d y_ab0  = _mm_add_pd( a._y0, _mm_mul_pd(_t,_mm_sub_pd(b._y0,a._y0)) );
    const __m128d y_bc0  = _mm_add_pd( b._y0, _mm_mul_pd(_t,_mm_sub_pd(c._y0,b._y0)) );
    const __m128d y_cd0  = _mm_add_pd( c._y0, _mm_mul_pd(_t,_mm_sub_pd(d._y0,c._y0)) );
    const __m128d y_abc0 = _mm_add_pd( y_ab0, _mm_mul_pd(_t,_mm_sub_pd(y_bc0,y_ab0)) );
    const __m128d y_bcd0 = _mm_add_pd( y_bc0, _mm_mul_pd(_t,_mm_sub_pd(y_cd0,y_bc0)) );
    _y0 = _mm_add_pd( y_abc0, _mm_mul_pd(_t,_mm_sub_pd(y_bcd0,y_abc0)) );
  }
  {
    const __m128d y_ab1  = _mm_add_pd( a._y1, _mm_mul_pd(_t,_mm_sub_pd(b._y1,a._y1)) );
    const __m128d y_bc1  = _mm_add_pd( b._y1, _mm_mul_pd(_t,_mm_sub_pd(c._y1,b._y1)) );
    const __m128d y_cd1  = _mm_add_pd( c._y1, _mm_mul_pd(_t,_mm_sub_pd(d._y1,c._y1)) );
    const __m128d y_abc1 = _mm_add_pd( y_ab1, _mm_mul_pd(_t,_mm_sub_pd(y_bc1,y_ab1)) );
    const __m128d y_bcd1 = _mm_add_pd( y_bc1, _mm_mul_pd(_t,_mm_sub_pd(y_cd1,y_bc1)) );
    _y1 = _mm_add_pd( y_abc1, _mm_mul_pd(_t,_mm_sub_pd(y_bcd1,y_abc1)) );
  }
  {
    const __m128d z_ab0  = _mm_add_pd( a._z0, _mm_mul_pd(_t,_mm_sub_pd(b._z0,a._z0)) );
    const __m128d z_bc0  = _mm_add_pd( b._z0, _mm_mul_pd(_t,_mm_sub_pd(c._z0,b._z0)) );
    const __m128d z_cd0  = _mm_add_pd( c._z0, _mm_mul_pd(_t,_mm_sub_pd(d._z0,c._z0)) );
    const __m128d z_abc0 = _mm_add_pd( z_ab0, _mm_mul_pd(_t,_mm_sub_pd(z_bc0,z_ab0)) );
    const __m128d z_bcd0 = _mm_add_pd( z_bc0, _mm_mul_pd(_t,_mm_sub_pd(z_cd0,z_bc0)) );
    _z0 = _mm_add_pd( z_abc0, _mm_mul_pd(_t,_mm_sub_pd(z_bcd0,z_abc0)) );
  }
  {
    const __m128d z_ab1  = _mm_add_pd( a._z1, _mm_mul_pd(_t,_mm_sub_pd(b._z1,a._z1)) );
    const __m128d z_bc1  = _mm_add_pd( b._z1, _mm_mul_pd(_t,_mm_sub_pd(c._z1,b._z1)) );
    const __m128d z_cd1  = _mm_add_pd( c._z1, _mm_mul_pd(_t,_mm_sub_pd(d._z1,c._z1)) );
    const __m128d z_abc1 = _mm_add_pd( z_ab1, _mm_mul_pd(_t,_mm_sub_pd(z_bc1,z_ab1)) );
    const __m128d z_bcd1 = _mm_add_pd( z_bc1, _mm_mul_pd(_t,_mm_sub_pd(z_cd1,z_bc1)) );
    _z1 = _mm_add_pd( z_abc1, _mm_mul_pd(_t,_mm_sub_pd(z_bcd1,z_abc1)) );
  }
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::negate(const XVector3SOA& a)
{
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);
  const __m128d _inv = _mm_set1_pd(-1.0);
  _x0 = _mm_mul_pd(_inv,a._x0);
  _x1 = _mm_mul_pd(_inv,a._x1);
  _y0 = _mm_mul_pd(_inv,a._y0);
  _y1 = _mm_mul_pd(_inv,a._y1);
  _z0 = _mm_mul_pd(_inv,a._z0);
  _z1 = _mm_mul_pd(_inv,a._z1);
}
//----------------------------------------------------------------------------------------------------------------------  
XM2_INLINE void XVector3SOA::negate()
{
  XM2_ALIGN_ASSERT(this);
  const __m128d _inv = _mm_set1_pd(-1.0);
  _x0 = _mm_mul_pd(_inv,_x0);
  _x1 = _mm_mul_pd(_inv,_x1);
  _y0 = _mm_mul_pd(_inv,_y0);
  _y1 = _mm_mul_pd(_inv,_y1);
  _z0 = _mm_mul_pd(_inv,_z0);
  _z1 = _mm_mul_pd(_inv,_z1);
}
}
#endif
