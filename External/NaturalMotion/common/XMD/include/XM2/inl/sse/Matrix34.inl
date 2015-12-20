#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_SSE_MATRIX__INL__
#define XM2_SSE_MATRIX__INL__
#if (!XM2_USE_FLOAT)
# error SSE2 float precision routines included when set to build using doubles
#endif
#ifndef XM2_USE_SSE
# error SSE routines included when set to build with FPU
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE XMatrix::XMatrix(const __m128& _x,const __m128& _y,const __m128& _z,const __m128& _w)
{
  set(_x,_y,_z,_w);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::set(const __m128& _x,const __m128& _y,const __m128& _z,const __m128& _w)
{
  XM2_ALIGN_ASSERT(this);
  _xaxis = _x;
  _yaxis = _y;
  _zaxis = _z;
  _waxis = _w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::set(const XReal _00,const XReal _01,const XReal _02,const XReal _03,
                             const XReal _10,const XReal _11,const XReal _12,const XReal _13,
                             const XReal _20,const XReal _21,const XReal _22,const XReal _23,
                             const XReal _30,const XReal _31,const XReal _32,const XReal _33)
{ 
  XM2_ALIGN_ASSERT(this);
  _xaxis = _mm_set_ps(_03,_02,_01,_00);
  _yaxis = _mm_set_ps(_13,_12,_11,_10);
  _zaxis = _mm_set_ps(_23,_22,_21,_20);
  _waxis = _mm_set_ps(_33,_32,_31,_30);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::set(const XVector3& x_axis,
                             const XVector3& y_axis,
                             const XVector3& z_axis,
                             const XVector4& position)
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&x_axis);
  XM2_ALIGN_ASSERT(&y_axis);
  XM2_ALIGN_ASSERT(&z_axis);
  XM2_ALIGN_ASSERT(&position);
  _xaxis = x_axis.sse;
  _yaxis = y_axis.sse;
  _zaxis = z_axis.sse;
  _waxis = position.sse;
}
//----------------------------------------------------------------------------------------------------------------------
void XMatrix::identity()
{
  XM2_ALIGN_ASSERT(this);
  _xaxis = _mm_set_ps(0.0f,0.0f,0.0f,1.0f);
  _yaxis = _mm_set_ps(0.0f,0.0f,1.0f,0.0f);
  _zaxis = _mm_set_ps(0.0f,1.0f,0.0f,0.0f);
  _waxis = _mm_set_ps(1.0f,0.0f,0.0f,0.0f);
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

  __m128 tmp0 = _mm_shuffle_ps(b._xaxis, b._yaxis, 0x44);
  __m128 tmp2 = _mm_shuffle_ps(b._xaxis, b._yaxis, 0xEE);
  __m128 tmp1 = _mm_shuffle_ps(b._zaxis, b._waxis, 0x44);
  __m128 tmp3 = _mm_shuffle_ps(b._zaxis, b._waxis, 0xEE);
  const __m128 row0 = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  const __m128 row1 = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  const __m128 row2 = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  const __m128 row3 = _mm_shuffle_ps(tmp2, tmp3, 0xDD);

  {
    __m128 temp0 = _mm_mul_ps(row0,a._xaxis);
    __m128 temp1 = _mm_mul_ps(row1,a._xaxis);
    __m128 temp2 = _mm_mul_ps(row2,a._xaxis);
    __m128 temp3 = _mm_mul_ps(row3,a._xaxis);
    tmp0   = _mm_shuffle_ps(temp0, temp1, 0x44);          
    tmp2   = _mm_shuffle_ps(temp0, temp1, 0xEE);          
    tmp1   = _mm_shuffle_ps(temp2, temp3, 0x44);          
    tmp3   = _mm_shuffle_ps(temp2, temp3, 0xEE);  
    temp0  = _mm_shuffle_ps(tmp0, tmp1, 0x88);
    temp1  = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
    temp2  = _mm_shuffle_ps(tmp2, tmp3, 0x88);
    temp3  = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
    _xaxis = _mm_add_ps(_mm_add_ps(temp0,temp1),_mm_add_ps(temp2,temp3));
  }

  {
    __m128 temp0 = _mm_mul_ps(row0,a._yaxis);
    __m128 temp1 = _mm_mul_ps(row1,a._yaxis);
    __m128 temp2 = _mm_mul_ps(row2,a._yaxis);
    __m128 temp3 = _mm_mul_ps(row3,a._yaxis);
    tmp0   = _mm_shuffle_ps(temp0, temp1, 0x44);          
    tmp2   = _mm_shuffle_ps(temp0, temp1, 0xEE);          
    tmp1   = _mm_shuffle_ps(temp2, temp3, 0x44);          
    tmp3   = _mm_shuffle_ps(temp2, temp3, 0xEE);  
    temp0  = _mm_shuffle_ps(tmp0, tmp1, 0x88);
    temp1  = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
    temp2  = _mm_shuffle_ps(tmp2, tmp3, 0x88);
    temp3  = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
    _yaxis = _mm_add_ps(_mm_add_ps(temp0,temp1),_mm_add_ps(temp2,temp3));
  }

  {
    __m128 temp0 = _mm_mul_ps(row0,a._zaxis);
    __m128 temp1 = _mm_mul_ps(row1,a._zaxis);
    __m128 temp2 = _mm_mul_ps(row2,a._zaxis);
    __m128 temp3 = _mm_mul_ps(row3,a._zaxis);
    tmp0   = _mm_shuffle_ps(temp0, temp1, 0x44);          
    tmp2   = _mm_shuffle_ps(temp0, temp1, 0xEE);          
    tmp1   = _mm_shuffle_ps(temp2, temp3, 0x44);          
    tmp3   = _mm_shuffle_ps(temp2, temp3, 0xEE);  
    temp0  = _mm_shuffle_ps(tmp0, tmp1, 0x88);
    temp1  = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
    temp2  = _mm_shuffle_ps(tmp2, tmp3, 0x88);
    temp3  = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
    _zaxis = _mm_add_ps(_mm_add_ps(temp0,temp1),_mm_add_ps(temp2,temp3));
  }

  {
    __m128 temp0 = _mm_mul_ps(row0,a._waxis);
    __m128 temp1 = _mm_mul_ps(row1,a._waxis);
    __m128 temp2 = _mm_mul_ps(row2,a._waxis);
    __m128 temp3 = _mm_mul_ps(row3,a._waxis);
    tmp0   = _mm_shuffle_ps(temp0, temp1, 0x44);          
    tmp2   = _mm_shuffle_ps(temp0, temp1, 0xEE);          
    tmp1   = _mm_shuffle_ps(temp2, temp3, 0x44);          
    tmp3   = _mm_shuffle_ps(temp2, temp3, 0xEE);  
    temp0  = _mm_shuffle_ps(tmp0, tmp1, 0x88);
    temp1  = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
    temp2  = _mm_shuffle_ps(tmp2, tmp3, 0x88);
    temp3  = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
    _waxis = _mm_add_ps(_mm_add_ps(temp0,temp1),_mm_add_ps(temp2,temp3));
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
    const __m128 _00 = _mm_shuffle_ps(b._xaxis,b._xaxis,_MM_SHUFFLE(0,0,0,0));
    const __m128 _01 = _mm_shuffle_ps(b._xaxis,b._xaxis,_MM_SHUFFLE(1,1,1,1));
    const __m128 _02 = _mm_shuffle_ps(b._xaxis,b._xaxis,_MM_SHUFFLE(2,2,2,2));
    _xaxis = _mm_mul_ps(_00,a._xaxis);
    _xaxis = _mm_add_ps(_mm_mul_ps(_01,a._yaxis),_xaxis);
    _xaxis = _mm_add_ps(_mm_mul_ps(_02,a._zaxis),_xaxis);
  }
  {
    const __m128 _10 = _mm_shuffle_ps(b._yaxis,b._yaxis,_MM_SHUFFLE(0,0,0,0));
    const __m128 _11 = _mm_shuffle_ps(b._yaxis,b._yaxis,_MM_SHUFFLE(1,1,1,1));
    const __m128 _12 = _mm_shuffle_ps(b._yaxis,b._yaxis,_MM_SHUFFLE(2,2,2,2));
    _yaxis = _mm_mul_ps(_10,a._xaxis);
    _yaxis = _mm_add_ps(_mm_mul_ps(_11,a._yaxis),_yaxis);
    _yaxis = _mm_add_ps(_mm_mul_ps(_12,a._zaxis),_yaxis);
  }
  {
    const __m128 _20 = _mm_shuffle_ps(b._zaxis,b._zaxis,_MM_SHUFFLE(0,0,0,0));
    const __m128 _21 = _mm_shuffle_ps(b._zaxis,b._zaxis,_MM_SHUFFLE(1,1,1,1));
    const __m128 _22 = _mm_shuffle_ps(b._zaxis,b._zaxis,_MM_SHUFFLE(2,2,2,2));;
    _zaxis = _mm_mul_ps(_20,a._xaxis);
    _zaxis = _mm_add_ps(_mm_mul_ps(_21,a._yaxis),_zaxis);
    _zaxis = _mm_add_ps(_mm_mul_ps(_22,a._zaxis),_zaxis);
  }
  {
    const __m128 _30 = _mm_shuffle_ps(b._waxis,b._waxis,_MM_SHUFFLE(0,0,0,0));
    const __m128 _31 = _mm_shuffle_ps(b._waxis,b._waxis,_MM_SHUFFLE(1,1,1,1));
    const __m128 _32 = _mm_shuffle_ps(b._waxis,b._waxis,_MM_SHUFFLE(2,2,2,2));;
    _waxis = _mm_mul_ps(_30,a._xaxis);
    _waxis = _mm_add_ps(_mm_mul_ps(_31,a._yaxis),_waxis);
    _waxis = _mm_add_ps(_mm_mul_ps(_32,a._zaxis),_waxis);
    _waxis = _mm_add_ps(a._waxis,_waxis);
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::mul(const XMatrix& a,const XReal b)
{
  XM2_ASSERT( &a != this );
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);

  const __m128 f = _mm_set_ps1(b);
  _mm_stream_ps( gl+0,  _mm_mul_ps(a._xaxis,f) );
  _mm_stream_ps( gl+4,  _mm_mul_ps(a._yaxis,f) );
  _mm_stream_ps( gl+8,  _mm_mul_ps(a._zaxis,f) );
  _mm_stream_ps( gl+12, _mm_mul_ps(a._waxis,f) );
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector3SOA& output,const XVector3SOA& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);
  XM2_ASSERT(&output != &b);
  {
    output._x  = _mm_mul_ps(_mm_set_ps1(m00),b._x);
    output._y  = _mm_mul_ps(_mm_set_ps1(m01),b._x);
    output._z  = _mm_mul_ps(_mm_set_ps1(m02),b._x);
    output._x  = _mm_add_ps(output._x,_mm_mul_ps(_mm_set_ps1(m10),b._y));
    output._y  = _mm_add_ps(output._y,_mm_mul_ps(_mm_set_ps1(m11),b._y));
    output._z  = _mm_add_ps(output._z,_mm_mul_ps(_mm_set_ps1(m12),b._y));
    output._x  = _mm_add_ps(output._x,_mm_mul_ps(_mm_set_ps1(m20),b._z));
    output._y  = _mm_add_ps(output._y,_mm_mul_ps(_mm_set_ps1(m21),b._z));
    output._z  = _mm_add_ps(output._z,_mm_mul_ps(_mm_set_ps1(m22),b._z));
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

  output._x  = _mm_mul_ps(_mm_set_ps1(m00),b._x);
  output._y  = _mm_mul_ps(_mm_set_ps1(m01),b._x);
  output._z  = _mm_mul_ps(_mm_set_ps1(m02),b._x);
  output._w  = _mm_mul_ps(_mm_set_ps1(m03),b._x);
  output._x  = _mm_add_ps(output._x,_mm_mul_ps(_mm_set_ps1(m10),b._y));
  output._y  = _mm_add_ps(output._y,_mm_mul_ps(_mm_set_ps1(m11),b._y));
  output._z  = _mm_add_ps(output._z,_mm_mul_ps(_mm_set_ps1(m12),b._y));
  output._w  = _mm_add_ps(output._w,_mm_mul_ps(_mm_set_ps1(m13),b._y));
  output._x  = _mm_add_ps(output._x,_mm_mul_ps(_mm_set_ps1(m20),b._z));
  output._y  = _mm_add_ps(output._y,_mm_mul_ps(_mm_set_ps1(m21),b._z));
  output._z  = _mm_add_ps(output._z,_mm_mul_ps(_mm_set_ps1(m22),b._z));
  output._w  = _mm_add_ps(output._w,_mm_mul_ps(_mm_set_ps1(m23),b._z));
  output._x  = _mm_add_ps(output._x,_mm_mul_ps(_mm_set_ps1(m30),b._w));
  output._y  = _mm_add_ps(output._y,_mm_mul_ps(_mm_set_ps1(m31),b._w));
  output._z  = _mm_add_ps(output._z,_mm_mul_ps(_mm_set_ps1(m32),b._w));
  output._w  = _mm_add_ps(output._w,_mm_mul_ps(_mm_set_ps1(m33),b._w));
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

  output._x  = _mm_mul_ps(_mm_set_ps1(m00),b._x);
  output._y  = _mm_mul_ps(_mm_set_ps1(m01),b._x);
  output._z  = _mm_mul_ps(_mm_set_ps1(m02),b._x);
  output._x  = _mm_add_ps(output._x,_mm_mul_ps(_mm_set_ps1(m10),b._y));
  output._y  = _mm_add_ps(output._y,_mm_mul_ps(_mm_set_ps1(m11),b._y));
  output._z  = _mm_add_ps(output._z,_mm_mul_ps(_mm_set_ps1(m12),b._y));
  output._x  = _mm_add_ps(output._x,_mm_mul_ps(_mm_set_ps1(m20),b._z));
  output._y  = _mm_add_ps(output._y,_mm_mul_ps(_mm_set_ps1(m21),b._z));
  output._z  = _mm_add_ps(output._z,_mm_mul_ps(_mm_set_ps1(m22),b._z));
  output._x  = _mm_add_ps(output._x,_mm_set_ps1(m30));
  output._y  = _mm_add_ps(output._y,_mm_set_ps1(m31));
  output._z  = _mm_add_ps(output._z,_mm_set_ps1(m32));
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector3& output,const XVector3& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);

  const __m128 x = _mm_set_ps1(b.x);
  const __m128 y = _mm_set_ps1(b.y);
  const __m128 z = _mm_set_ps1(b.z);
  output.sse = _mm_mul_ps(x,_xaxis);
  output.sse = _mm_add_ps(_mm_mul_ps(y,_yaxis),output.sse);
  output.sse = _mm_add_ps(_mm_mul_ps(z,_zaxis),output.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector4& output,const XVector4& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);

  const __m128 x = _mm_set_ps1(b.x);
  const __m128 y = _mm_set_ps1(b.y);
  const __m128 z = _mm_set_ps1(b.z);
  const __m128 w = _mm_set_ps1(b.w);
  output.sse = _mm_mul_ps(x,_xaxis);
  output.sse = _mm_add_ps(_mm_mul_ps(y,_yaxis),output.sse);
  output.sse = _mm_add_ps(_mm_mul_ps(z,_zaxis),output.sse);
  output.sse = _mm_add_ps(_mm_mul_ps(w,_waxis),output.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformPoint(XVector3& output,const XVector3& b) const
{
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(&output);

  const __m128 x = _mm_set_ps1(b.x);
  const __m128 y = _mm_set_ps1(b.y);
  const __m128 z = _mm_set_ps1(b.z);
  output.sse = _mm_mul_ps(x,_xaxis);
  output.sse = _mm_add_ps(_mm_mul_ps(y,_yaxis),output.sse);
  output.sse = _mm_add_ps(_mm_mul_ps(z,_zaxis),output.sse);
  output.sse = _mm_add_ps(_waxis,output.sse);
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transpose(const XMatrix& m)
{
  XM2_ALIGN_ASSERT(this);
  __m128 tmp0 = _mm_shuffle_ps((m._xaxis), (m._yaxis), 0x44);
  __m128 tmp2 = _mm_shuffle_ps((m._xaxis), (m._yaxis), 0xEE);
  __m128 tmp1 = _mm_shuffle_ps((m._zaxis), (m._waxis), 0x44);
  __m128 tmp3 = _mm_shuffle_ps((m._zaxis), (m._waxis), 0xEE);
  _xaxis = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  _yaxis = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  _zaxis = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  _waxis = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
}
}
#endif
