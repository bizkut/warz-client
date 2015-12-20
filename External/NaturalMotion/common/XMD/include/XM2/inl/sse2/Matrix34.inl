#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE2_MATRIX__INL__
#define XM2_SSE2_MATRIX__INL__
#if XM2_USE_FLOAT
# error SSE2 double precision routines included when using float
#endif
#ifndef XM2_USE_SSE
# error SSE2 routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrix::XMatrix(const __m128d& _x0,const __m128d& _x1,
                            const __m128d& _y0,const __m128d& _y1,
                            const __m128d& _z0,const __m128d& _z1,
                            const __m128d& _w0,const __m128d& _w1)
{
  XM2_ALIGN_ASSERT(this);
  _xaxis_xy = _x0;
  _xaxis_zw = _x1;
  _yaxis_xy = _y0;
  _yaxis_zw = _y1;
  _zaxis_xy = _z0;
  _zaxis_zw = _z1;
  _waxis_xy = _w0;
  _waxis_zw = _w1;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::set(const XReal _00,const XReal _01,const XReal _02,const XReal _03,
                             const XReal _10,const XReal _11,const XReal _12,const XReal _13,
                             const XReal _20,const XReal _21,const XReal _22,const XReal _23,
                             const XReal _30,const XReal _31,const XReal _32,const XReal _33)
{
  XM2_ALIGN_ASSERT(this);
  _xaxis_xy = _mm_set_pd(_01,_00);
  _xaxis_zw = _mm_set_pd(_03,_02);
  _yaxis_xy = _mm_set_pd(_11,_10);
  _yaxis_zw = _mm_set_pd(_13,_12);
  _zaxis_xy = _mm_set_pd(_21,_20);
  _zaxis_zw = _mm_set_pd(_23,_22);
  _waxis_xy = _mm_set_pd(_31,_30);
  _waxis_zw = _mm_set_pd(_33,_32);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::set(const XVector3& x_axis,
                             const XVector3& y_axis,
                             const XVector3& z_axis,
                             const XVector4& position)
{
  XM2_ALIGN_ASSERT(this);
  _xaxis_xy = x_axis.xy;
  _xaxis_zw = x_axis.zw;
  _yaxis_xy = y_axis.xy;
  _yaxis_zw = y_axis.zw;
  _zaxis_xy = z_axis.xy;
  _zaxis_zw = z_axis.zw;
  _waxis_xy = position.xy;
  _waxis_zw = position.zw;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::identity()
{
  XM2_ALIGN_ASSERT(this);
  _xaxis_xy = _mm_set_pd(0.0,1.0);
  _xaxis_zw = _mm_set_pd(0.0,0.0);
  _yaxis_xy = _mm_set_pd(1.0,0.0);
  _yaxis_zw = _mm_set_pd(0.0,0.0);
  _zaxis_xy = _mm_set_pd(0.0,0.0);
  _zaxis_zw = _mm_set_pd(0.0,1.0);
  _waxis_xy = _mm_set_pd(0.0,0.0);
  _waxis_zw = _mm_set_pd(1.0,0.0);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::mul(const XMatrix& a,const XMatrix& b)
{
  // a and b can't be this
  XM2_ASSERT( &a != this );
  XM2_ASSERT( &b != this );

  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);
  {
    const __m128d _00 = _mm_shuffle_pd(b._xaxis_xy,b._xaxis_xy,_MM_SHUFFLE2(0,0));
    const __m128d _01 = _mm_shuffle_pd(b._xaxis_xy,b._xaxis_xy,_MM_SHUFFLE2(1,1));
    const __m128d _02 = _mm_shuffle_pd(b._xaxis_zw,b._xaxis_zw,_MM_SHUFFLE2(0,0));
    const __m128d _03 = _mm_shuffle_pd(b._xaxis_zw,b._xaxis_zw,_MM_SHUFFLE2(1,1));

    _xaxis_xy = _mm_mul_pd(_00,a._xaxis_xy);
    _xaxis_zw = _mm_mul_pd(_00,a._xaxis_zw);
    _xaxis_xy = _mm_add_pd(_mm_mul_pd(_01,a._yaxis_xy),_xaxis_xy);
    _xaxis_zw = _mm_add_pd(_mm_mul_pd(_01,a._yaxis_zw),_xaxis_zw);
    _xaxis_xy = _mm_add_pd(_mm_mul_pd(_02,a._zaxis_xy),_xaxis_xy);
    _xaxis_zw = _mm_add_pd(_mm_mul_pd(_02,a._zaxis_zw),_xaxis_zw);
    _xaxis_xy = _mm_add_pd(_mm_mul_pd(_03,a._waxis_xy),_xaxis_xy);
    _xaxis_zw = _mm_add_pd(_mm_mul_pd(_03,a._waxis_zw),_xaxis_zw);
  }

  {
    const __m128d _10 = _mm_shuffle_pd(b._yaxis_xy,b._yaxis_xy,_MM_SHUFFLE2(0,0));
    const __m128d _11 = _mm_shuffle_pd(b._yaxis_xy,b._yaxis_xy,_MM_SHUFFLE2(1,1));
    const __m128d _12 = _mm_shuffle_pd(b._yaxis_zw,b._yaxis_zw,_MM_SHUFFLE2(0,0));
    const __m128d _13 = _mm_shuffle_pd(b._yaxis_zw,b._yaxis_zw,_MM_SHUFFLE2(1,1));

    _yaxis_xy = _mm_mul_pd(_10,a._xaxis_xy);
    _yaxis_zw = _mm_mul_pd(_10,a._xaxis_zw);
    _yaxis_xy = _mm_add_pd(_mm_mul_pd(_11,a._yaxis_xy),_yaxis_xy);
    _yaxis_zw = _mm_add_pd(_mm_mul_pd(_11,a._yaxis_zw),_yaxis_zw);
    _yaxis_xy = _mm_add_pd(_mm_mul_pd(_12,a._zaxis_xy),_yaxis_xy);
    _yaxis_zw = _mm_add_pd(_mm_mul_pd(_12,a._zaxis_zw),_yaxis_zw);
    _yaxis_xy = _mm_add_pd(_mm_mul_pd(_13,a._waxis_xy),_yaxis_xy);
    _yaxis_zw = _mm_add_pd(_mm_mul_pd(_13,a._waxis_zw),_yaxis_zw);
  }

  {
    const __m128d _20 = _mm_shuffle_pd(b._zaxis_xy,b._zaxis_xy,_MM_SHUFFLE2(0,0));
    const __m128d _21 = _mm_shuffle_pd(b._zaxis_xy,b._zaxis_xy,_MM_SHUFFLE2(1,1));
    const __m128d _22 = _mm_shuffle_pd(b._zaxis_zw,b._zaxis_zw,_MM_SHUFFLE2(0,0));
    const __m128d _23 = _mm_shuffle_pd(b._zaxis_zw,b._zaxis_zw,_MM_SHUFFLE2(1,1));

    _zaxis_xy = _mm_mul_pd(_20,a._xaxis_xy);
    _zaxis_zw = _mm_mul_pd(_20,a._xaxis_zw);
    _zaxis_xy = _mm_add_pd(_mm_mul_pd(_21,a._yaxis_xy),_zaxis_xy);
    _zaxis_zw = _mm_add_pd(_mm_mul_pd(_21,a._yaxis_zw),_zaxis_zw);
    _zaxis_xy = _mm_add_pd(_mm_mul_pd(_22,a._zaxis_xy),_zaxis_xy);
    _zaxis_zw = _mm_add_pd(_mm_mul_pd(_22,a._zaxis_zw),_zaxis_zw);
    _zaxis_xy = _mm_add_pd(_mm_mul_pd(_23,a._waxis_xy),_zaxis_xy);
    _zaxis_zw = _mm_add_pd(_mm_mul_pd(_23,a._waxis_zw),_zaxis_zw);
  }

  {
    const __m128d _30 = _mm_shuffle_pd(b._waxis_xy,b._waxis_xy,_MM_SHUFFLE2(0,0));
    const __m128d _31 = _mm_shuffle_pd(b._waxis_xy,b._waxis_xy,_MM_SHUFFLE2(1,1));
    const __m128d _32 = _mm_shuffle_pd(b._waxis_zw,b._waxis_zw,_MM_SHUFFLE2(0,0));
    const __m128d _33 = _mm_shuffle_pd(b._waxis_zw,b._waxis_zw,_MM_SHUFFLE2(1,1));

    _waxis_xy = _mm_mul_pd(_30,a._xaxis_xy);
    _waxis_zw = _mm_mul_pd(_30,a._xaxis_zw);
    _waxis_xy = _mm_add_pd(_mm_mul_pd(_31,a._yaxis_xy),_waxis_xy);
    _waxis_zw = _mm_add_pd(_mm_mul_pd(_31,a._yaxis_zw),_waxis_zw);
    _waxis_xy = _mm_add_pd(_mm_mul_pd(_32,a._zaxis_xy),_waxis_xy);
    _waxis_zw = _mm_add_pd(_mm_mul_pd(_32,a._zaxis_zw),_waxis_zw);
    _waxis_xy = _mm_add_pd(_mm_mul_pd(_33,a._waxis_xy),_waxis_xy);
    _waxis_zw = _mm_add_pd(_mm_mul_pd(_33,a._waxis_zw),_waxis_zw);
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::quickMult(const XMatrix& a,const XMatrix& b)
{
  XM2_ASSERT( &a != this );
  XM2_ASSERT( &b != this );

  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(this);

  {
    const __m128d _00 = _mm_shuffle_pd(b._xaxis_xy,b._xaxis_xy,_MM_SHUFFLE2(0,0));
    const __m128d _01 = _mm_shuffle_pd(b._xaxis_xy,b._xaxis_xy,_MM_SHUFFLE2(1,1));
    const __m128d _02 = _mm_shuffle_pd(b._xaxis_zw,b._xaxis_zw,_MM_SHUFFLE2(0,0));

    _xaxis_xy = _mm_mul_pd(_00,a._xaxis_xy);
    _xaxis_zw = _mm_mul_pd(_00,a._xaxis_zw);
    _xaxis_xy = _mm_add_pd(_mm_mul_pd(_01,a._yaxis_xy),_xaxis_xy);
    _xaxis_zw = _mm_add_pd(_mm_mul_pd(_01,a._yaxis_zw),_xaxis_zw);
    _xaxis_xy = _mm_add_pd(_mm_mul_pd(_02,a._zaxis_xy),_xaxis_xy);
    _xaxis_zw = _mm_add_pd(_mm_mul_pd(_02,a._zaxis_zw),_xaxis_zw);
  }

  {
    const __m128d _10 = _mm_shuffle_pd(b._yaxis_xy,b._yaxis_xy,_MM_SHUFFLE2(0,0));
    const __m128d _11 = _mm_shuffle_pd(b._yaxis_xy,b._yaxis_xy,_MM_SHUFFLE2(1,1));
    const __m128d _12 = _mm_shuffle_pd(b._yaxis_zw,b._yaxis_zw,_MM_SHUFFLE2(0,0));

    _yaxis_xy = _mm_mul_pd(_10,a._xaxis_xy);
    _yaxis_zw = _mm_mul_pd(_10,a._xaxis_zw);
    _yaxis_xy = _mm_add_pd(_mm_mul_pd(_11,a._yaxis_xy),_yaxis_xy);
    _yaxis_zw = _mm_add_pd(_mm_mul_pd(_11,a._yaxis_zw),_yaxis_zw);
    _yaxis_xy = _mm_add_pd(_mm_mul_pd(_12,a._zaxis_xy),_yaxis_xy);
    _yaxis_zw = _mm_add_pd(_mm_mul_pd(_12,a._zaxis_zw),_yaxis_zw);
  }

  {
    const __m128d _20 = _mm_shuffle_pd(b._zaxis_xy,b._zaxis_xy,_MM_SHUFFLE2(0,0));
    const __m128d _21 = _mm_shuffle_pd(b._zaxis_xy,b._zaxis_xy,_MM_SHUFFLE2(1,1));
    const __m128d _22 = _mm_shuffle_pd(b._zaxis_zw,b._zaxis_zw,_MM_SHUFFLE2(0,0));

    _zaxis_xy = _mm_mul_pd(_20,a._xaxis_xy);
    _zaxis_zw = _mm_mul_pd(_20,a._xaxis_zw);
    _zaxis_xy = _mm_add_pd(_mm_mul_pd(_21,a._yaxis_xy),_zaxis_xy);
    _zaxis_zw = _mm_add_pd(_mm_mul_pd(_21,a._yaxis_zw),_zaxis_zw);
    _zaxis_xy = _mm_add_pd(_mm_mul_pd(_22,a._zaxis_xy),_zaxis_xy);
    _zaxis_zw = _mm_add_pd(_mm_mul_pd(_22,a._zaxis_zw),_zaxis_zw);
  }

  {
    const __m128d _30 = _mm_shuffle_pd(b._waxis_xy,b._waxis_xy,_MM_SHUFFLE2(0,0));
    const __m128d _31 = _mm_shuffle_pd(b._waxis_xy,b._waxis_xy,_MM_SHUFFLE2(1,1));
    const __m128d _32 = _mm_shuffle_pd(b._waxis_zw,b._waxis_zw,_MM_SHUFFLE2(0,0));

    _waxis_xy = _mm_mul_pd(_30,a._xaxis_xy);
    _waxis_zw = _mm_mul_pd(_30,a._xaxis_zw);
    _waxis_xy = _mm_add_pd(_mm_mul_pd(_31,a._yaxis_xy),_waxis_xy);
    _waxis_zw = _mm_add_pd(_mm_mul_pd(_31,a._yaxis_zw),_waxis_zw);
    _waxis_xy = _mm_add_pd(_mm_mul_pd(_32,a._zaxis_xy),_waxis_xy);
    _waxis_zw = _mm_add_pd(_mm_mul_pd(_32,a._zaxis_zw),_waxis_zw);
    _waxis_xy = _mm_add_pd(a._waxis_xy,_waxis_xy);
    _waxis_zw = _mm_add_pd(a._waxis_zw,_waxis_zw);
  }
 }
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::mul(const XMatrix& a,const XReal b)
{
  XM2_ASSERT( &a != this );
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);

  const __m128d f = _mm_set1_pd(b);
  _mm_stream_pd( gl+0,  _mm_mul_pd(a._xaxis_xy,f) );
  _mm_stream_pd( gl+2,  _mm_mul_pd(a._xaxis_zw,f) );
  _mm_stream_pd( gl+4,  _mm_mul_pd(a._yaxis_xy,f) );
  _mm_stream_pd( gl+6,  _mm_mul_pd(a._yaxis_zw,f) );
  _mm_stream_pd( gl+8,  _mm_mul_pd(a._zaxis_xy,f) );
  _mm_stream_pd( gl+10, _mm_mul_pd(a._zaxis_zw,f) );
  _mm_stream_pd( gl+12, _mm_mul_pd(a._waxis_xy,f) );
  _mm_stream_pd( gl+14, _mm_mul_pd(a._waxis_zw,f) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector3SOA& output,const XVector3SOA& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);

  // values get modified during transform, so output cannot be the 
  // same as the input otherwise the values will get screwed
  XM2_ASSERT(&output != &b);

  {
    __m128d temp = _mm_shuffle_pd( _xaxis_xy, _xaxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_mul_pd(temp,b._x0);
    output._x1  = _mm_mul_pd(temp,b._x1);
    temp = _mm_shuffle_pd( _xaxis_xy, _xaxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_mul_pd(temp,b._y0);
    output._y1  = _mm_mul_pd(temp,b._y1);
    temp = _mm_shuffle_pd( _xaxis_zw, _xaxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_mul_pd(temp,b._z0);
    output._z1  = _mm_mul_pd(temp,b._z1);

    temp = _mm_shuffle_pd( _yaxis_xy, _yaxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_add_pd(output._x0,_mm_mul_pd(temp,b._x0));
    output._x1  = _mm_add_pd(output._x1,_mm_mul_pd(temp,b._x1));
    temp = _mm_shuffle_pd( _yaxis_xy, _yaxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_add_pd(output._y0,_mm_mul_pd(temp,b._y0));
    output._y1  = _mm_add_pd(output._y1,_mm_mul_pd(temp,b._y1));
    temp = _mm_shuffle_pd( _yaxis_zw, _yaxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_add_pd(output._z0,_mm_mul_pd(temp,b._z0));
    output._z1  = _mm_add_pd(output._z1,_mm_mul_pd(temp,b._z1));

    temp = _mm_shuffle_pd( _zaxis_xy, _zaxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_add_pd(output._x0,_mm_mul_pd(temp,b._x0));
    output._x1  = _mm_add_pd(output._x1,_mm_mul_pd(temp,b._x1));
    temp = _mm_shuffle_pd( _zaxis_xy, _zaxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_add_pd(output._y0,_mm_mul_pd(temp,b._y0));
    output._y1  = _mm_add_pd(output._y1,_mm_mul_pd(temp,b._y1));
    temp = _mm_shuffle_pd( _zaxis_zw, _zaxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_add_pd(output._z0,_mm_mul_pd(temp,b._z0));
    output._z1  = _mm_add_pd(output._z1,_mm_mul_pd(temp,b._z1));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector4SOA& output,const XVector4SOA& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);

  // values get modified during transform, so output cannot be the 
  // same as the input otherwise the values will get screwed
  XM2_ASSERT(&output != &b);

  {
    __m128d temp = _mm_shuffle_pd( _xaxis_xy, _xaxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_mul_pd(temp,b._x0);
    output._x1  = _mm_mul_pd(temp,b._x1);
    temp = _mm_shuffle_pd( _xaxis_xy, _xaxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_mul_pd(temp,b._y0);
    output._y1  = _mm_mul_pd(temp,b._y1);
    temp = _mm_shuffle_pd( _xaxis_zw, _xaxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_mul_pd(temp,b._z0);
    output._z1  = _mm_mul_pd(temp,b._z1);
    temp = _mm_shuffle_pd( _xaxis_zw, _xaxis_zw, _MM_SHUFFLE2(1,1) );
    output._w0  = _mm_mul_pd(temp,b._w0);
    output._w1  = _mm_mul_pd(temp,b._w1);

    temp = _mm_shuffle_pd( _yaxis_xy, _yaxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_add_pd(output._x0,_mm_mul_pd(temp,b._x0));
    output._x1  = _mm_add_pd(output._x1,_mm_mul_pd(temp,b._x1));
    temp = _mm_shuffle_pd( _yaxis_xy, _yaxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_add_pd(output._y0,_mm_mul_pd(temp,b._y0));
    output._y1  = _mm_add_pd(output._y1,_mm_mul_pd(temp,b._y1));
    temp = _mm_shuffle_pd( _yaxis_zw, _yaxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_add_pd(output._z0,_mm_mul_pd(temp,b._z0));
    output._z1  = _mm_add_pd(output._z1,_mm_mul_pd(temp,b._z1));
    temp = _mm_shuffle_pd( _yaxis_zw, _yaxis_zw, _MM_SHUFFLE2(1,1) );
    output._w0  = _mm_add_pd(output._w0,_mm_mul_pd(temp,b._w0));
    output._w1  = _mm_add_pd(output._w1,_mm_mul_pd(temp,b._w1));

    temp = _mm_shuffle_pd( _zaxis_xy, _zaxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_add_pd(output._x0,_mm_mul_pd(temp,b._x0));
    output._x1  = _mm_add_pd(output._x1,_mm_mul_pd(temp,b._x1));
    temp = _mm_shuffle_pd( _zaxis_xy, _zaxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_add_pd(output._y0,_mm_mul_pd(temp,b._y0));
    output._y1  = _mm_add_pd(output._y1,_mm_mul_pd(temp,b._y1));
    temp = _mm_shuffle_pd( _zaxis_zw, _zaxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_add_pd(output._z0,_mm_mul_pd(temp,b._z0));
    output._z1  = _mm_add_pd(output._z1,_mm_mul_pd(temp,b._z1));
    temp = _mm_shuffle_pd( _zaxis_zw, _zaxis_zw, _MM_SHUFFLE2(1,1) );
    output._w0  = _mm_add_pd(output._w0,_mm_mul_pd(temp,b._w0));
    output._w1  = _mm_add_pd(output._w1,_mm_mul_pd(temp,b._w1));

    temp = _mm_shuffle_pd( _waxis_xy, _waxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_add_pd(output._x0,_mm_mul_pd(temp,b._x0));
    output._x1  = _mm_add_pd(output._x1,_mm_mul_pd(temp,b._x1));
    temp = _mm_shuffle_pd( _waxis_xy, _waxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_add_pd(output._y0,_mm_mul_pd(temp,b._y0));
    output._y1  = _mm_add_pd(output._y1,_mm_mul_pd(temp,b._y1));
    temp = _mm_shuffle_pd( _waxis_zw, _waxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_add_pd(output._z0,_mm_mul_pd(temp,b._z0));
    output._z1  = _mm_add_pd(output._z1,_mm_mul_pd(temp,b._z1));
    temp = _mm_shuffle_pd( _waxis_zw, _waxis_zw, _MM_SHUFFLE2(1,1) );
    output._w0  = _mm_add_pd(output._w0,_mm_mul_pd(temp,b._w0));
    output._w1  = _mm_add_pd(output._w1,_mm_mul_pd(temp,b._w1));
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformPoint(XVector3SOA& output,const XVector3SOA& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);

  // values get modified during transform, so output cannot be the 
  // same as the input otherwise the values will get screwed
  XM2_ASSERT(&output != &b);
  {
    __m128d temp = _mm_shuffle_pd( _xaxis_xy, _xaxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_mul_pd(temp,b._x0);
    output._x1  = _mm_mul_pd(temp,b._x1);
    temp = _mm_shuffle_pd( _xaxis_xy, _xaxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_mul_pd(temp,b._y0);
    output._y1  = _mm_mul_pd(temp,b._y1);
    temp = _mm_shuffle_pd( _xaxis_zw, _xaxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_mul_pd(temp,b._z0);
    output._z1  = _mm_mul_pd(temp,b._z1);

    temp = _mm_shuffle_pd( _yaxis_xy, _yaxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_add_pd(output._x0,_mm_mul_pd(temp,b._x0));
    output._x1  = _mm_add_pd(output._x1,_mm_mul_pd(temp,b._x1));
    temp = _mm_shuffle_pd( _yaxis_xy, _yaxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_add_pd(output._y0,_mm_mul_pd(temp,b._y0));
    output._y1  = _mm_add_pd(output._y1,_mm_mul_pd(temp,b._y1));
    temp = _mm_shuffle_pd( _yaxis_zw, _yaxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_add_pd(output._z0,_mm_mul_pd(temp,b._z0));
    output._z1  = _mm_add_pd(output._z1,_mm_mul_pd(temp,b._z1));

    temp = _mm_shuffle_pd( _zaxis_xy, _zaxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_add_pd(output._x0,_mm_mul_pd(temp,b._x0));
    output._x1  = _mm_add_pd(output._x1,_mm_mul_pd(temp,b._x1));
    temp = _mm_shuffle_pd( _zaxis_xy, _zaxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_add_pd(output._y0,_mm_mul_pd(temp,b._y0));
    output._y1  = _mm_add_pd(output._y1,_mm_mul_pd(temp,b._y1));
    temp = _mm_shuffle_pd( _zaxis_zw, _zaxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_add_pd(output._z0,_mm_mul_pd(temp,b._z0));
    output._z1  = _mm_add_pd(output._z1,_mm_mul_pd(temp,b._z1));

    temp = _mm_shuffle_pd( _waxis_xy, _waxis_xy, _MM_SHUFFLE2(0,0) );
    output._x0  = _mm_add_pd(output._x0,temp);
    output._x1  = _mm_add_pd(output._x1,temp);
    temp = _mm_shuffle_pd( _waxis_xy, _waxis_xy, _MM_SHUFFLE2(1,1) );
    output._y0  = _mm_add_pd(output._y0,temp);
    output._y1  = _mm_add_pd(output._y1,temp);
    temp = _mm_shuffle_pd( _waxis_zw, _waxis_zw, _MM_SHUFFLE2(0,0) );
    output._z0  = _mm_add_pd(output._z0,temp);
    output._z1  = _mm_add_pd(output._z1,temp);
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector3& output,const XVector3& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);

  {
    const __m128d x = _mm_shuffle_pd(b.xy,b.xy,_MM_SHUFFLE2(0,0));
    const __m128d y = _mm_shuffle_pd(b.xy,b.xy,_MM_SHUFFLE2(1,1));
    const __m128d z = _mm_shuffle_pd(b.zw,b.zw,_MM_SHUFFLE2(0,0));
    output.xy = _mm_mul_pd(x,_xaxis_xy);
    output.zw = _mm_mul_pd(x,_xaxis_zw);
    output.xy = _mm_add_pd(_mm_mul_pd(y,_yaxis_xy),output.xy);
    output.zw = _mm_add_pd(_mm_mul_pd(y,_yaxis_zw),output.zw);
    output.xy = _mm_add_pd(_mm_mul_pd(z,_zaxis_xy),output.xy);
    output.zw = _mm_add_pd(_mm_mul_pd(z,_zaxis_zw),output.zw);
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector4& output,const XVector4& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);
  {
    const __m128d x = _mm_set1_pd(b.x);
    const __m128d y = _mm_set1_pd(b.y);
    const __m128d z = _mm_set1_pd(b.z);
    const __m128d w = _mm_set1_pd(b.w);
    output.xy = _mm_mul_pd(x,_xaxis_xy);
    output.zw = _mm_mul_pd(x,_xaxis_zw);
    output.xy = _mm_add_pd(_mm_mul_pd(y,_yaxis_xy),output.xy);
    output.zw = _mm_add_pd(_mm_mul_pd(y,_yaxis_zw),output.zw);
    output.xy = _mm_add_pd(_mm_mul_pd(z,_zaxis_xy),output.xy);
    output.zw = _mm_add_pd(_mm_mul_pd(z,_zaxis_zw),output.zw);
    output.xy = _mm_add_pd(_mm_mul_pd(w,_waxis_xy),output.xy);
    output.zw = _mm_add_pd(_mm_mul_pd(w,_waxis_zw),output.zw);
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformPoint(XVector3& output,const XVector3& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);

  const __m128d x = _mm_set1_pd(b.x);
  const __m128d y = _mm_set1_pd(b.y);
  const __m128d z = _mm_set1_pd(b.z);
  output.xy = _mm_mul_pd(x,_xaxis_xy);
  output.zw = _mm_mul_pd(x,_xaxis_zw);
  output.xy = _mm_add_pd(_mm_mul_pd(y,_yaxis_xy),output.xy);
  output.zw = _mm_add_pd(_mm_mul_pd(y,_yaxis_zw),output.zw);
  output.xy = _mm_add_pd(_mm_mul_pd(z,_zaxis_xy),output.xy);
  output.zw = _mm_add_pd(_mm_mul_pd(z,_zaxis_zw),output.zw);
  output.xy = _mm_add_pd(_waxis_xy,output.xy);
  output.zw = _mm_add_pd(_waxis_zw,output.zw);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transpose(const XMatrix& a)
{
  XM2_ALIGN_ASSERT(this);

  // transpose 4 in top left corner
  {
    const __m128d temp_y0 = _mm_shuffle_pd(a._xaxis_xy,a._yaxis_xy,_MM_SHUFFLE2(1,1));
    _xaxis_xy = _mm_shuffle_pd(a._xaxis_xy,a._yaxis_xy,_MM_SHUFFLE2(0,0));
    _yaxis_xy = temp_y0;
  }
  // transpose top right and bottom left corners, then assign on top of each other
  {
    const __m128d temp_x1 = _mm_shuffle_pd(a._xaxis_zw,a._yaxis_zw,_MM_SHUFFLE2(1,1));
    const __m128d temp_y1 = _mm_shuffle_pd(a._xaxis_zw,a._yaxis_zw,_MM_SHUFFLE2(0,0));
    const __m128d temp_z0 = _mm_shuffle_pd(a._zaxis_xy,a._waxis_xy,_MM_SHUFFLE2(1,1));
    const __m128d temp_w0 = _mm_shuffle_pd(a._zaxis_xy,a._waxis_xy,_MM_SHUFFLE2(0,0));
    _xaxis_zw = temp_w0; // 9 13
    _yaxis_zw = temp_z0; // 10 14
    _waxis_xy = temp_x1; // 4 8
    _zaxis_xy = temp_y1; // 3 7
  }
  // transpose 4 in bottom right corner
  {
    const __m128d temp_w1 = _mm_shuffle_pd(a._zaxis_zw,a._waxis_zw,_MM_SHUFFLE2(0,0));
    _waxis_zw = _mm_shuffle_pd(a._zaxis_zw,a._waxis_zw,_MM_SHUFFLE2(1,1));
    _zaxis_zw = temp_w1;
  }
}
}
#endif
