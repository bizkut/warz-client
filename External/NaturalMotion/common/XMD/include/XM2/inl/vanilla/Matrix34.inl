#ifdef _MSC_VER
# pragma once
#endif
#ifndef XM2_VANILLA_MATRIX__INL__
#define XM2_VANILLA_MATRIX__INL__
#ifdef XM2_USE_SSE
# error FPU routines included when set to build with SSE
#endif
namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::set(const XReal _00,const XReal _01,const XReal _02,const XReal _03,
                             const XReal _10,const XReal _11,const XReal _12,const XReal _13,
                             const XReal _20,const XReal _21,const XReal _22,const XReal _23,
                             const XReal _30,const XReal _31,const XReal _32,const XReal _33)
{
  m00 = _00; m01 = _01; m02 = _02; m03 = _03;
  m10 = _10; m11 = _11; m12 = _12; m13 = _13;
  m20 = _20; m21 = _21; m22 = _22; m23 = _23;
  m30 = _30; m31 = _31; m32 = _32; m33 = _33;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::set(const XVector3& x_axis,
                             const XVector3& y_axis,
                             const XVector3& z_axis,
                             const XVector4& position)
{
  m00 = x_axis.x; m01 = x_axis.y; m02 = x_axis.z; m03 = x_axis.w;
  m10 = y_axis.x; m11 = y_axis.y; m12 = y_axis.z; m13 = y_axis.w;
  m20 = z_axis.x; m21 = z_axis.y; m22 = z_axis.z; m23 = z_axis.w;
  m30 = position.x; m31 = position.y; m32 = position.z; m33 = position.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::identity()
{
  m00 = 1.0f; m01 = 0.0f; m02 = 0.0f; m03 = 0.0f; 
  m10 = 0.0f; m11 = 1.0f; m12 = 0.0f; m13 = 0.0f; 
  m20 = 0.0f; m21 = 0.0f; m22 = 1.0f; m23 = 0.0f; 
  m30 = 0.0f; m31 = 0.0f; m32 = 0.0f; m33 = 1.0f; 
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::mul(const XMatrix& a,const XMatrix& b)
{
  XM2_ASSERT( &a != this );
  XM2_ASSERT( &b != this );

  // row 0
  m00  =  a.m00 * b.m00;
  m01  =  a.m01 * b.m00;
  m02  =  a.m02 * b.m00;
  m03  =  a.m03 * b.m00;
  m00 +=  a.m10 * b.m01;
  m01 +=  a.m11 * b.m01;
  m02 +=  a.m12 * b.m01;
  m03 +=  a.m13 * b.m01;
  m00 +=  a.m20 * b.m02;
  m01 +=  a.m21 * b.m02;
  m02 +=  a.m22 * b.m02;
  m03 +=  a.m23 * b.m02;
  m00 +=  a.m30 * b.m03;
  m01 +=  a.m31 * b.m03;
  m02 +=  a.m32 * b.m03;
  m03 +=  a.m33 * b.m03;
  // row 1
  m10  =  a.m00 * b.m10;
  m11  =  a.m01 * b.m10;
  m12  =  a.m02 * b.m10;
  m13  =  a.m03 * b.m10;
  m10 +=  a.m10 * b.m11;
  m11 +=  a.m11 * b.m11;
  m12 +=  a.m12 * b.m11;
  m13 +=  a.m13 * b.m11;
  m10 +=  a.m20 * b.m12;
  m11 +=  a.m21 * b.m12;
  m12 +=  a.m22 * b.m12;
  m13 +=  a.m23 * b.m12;
  m10 +=  a.m30 * b.m13;
  m11 +=  a.m31 * b.m13;
  m12 +=  a.m32 * b.m13;
  m13 +=  a.m33 * b.m13;
  // row 2
  m20  =  a.m00 * b.m20;
  m21  =  a.m01 * b.m20;
  m22  =  a.m02 * b.m20;
  m23  =  a.m03 * b.m20;
  m20 +=  a.m10 * b.m21;
  m21 +=  a.m11 * b.m21;
  m22 +=  a.m12 * b.m21;
  m23 +=  a.m13 * b.m21;
  m20 +=  a.m20 * b.m22;
  m21 +=  a.m21 * b.m22;
  m22 +=  a.m22 * b.m22;
  m23 +=  a.m23 * b.m22;
  m20 +=  a.m30 * b.m23;
  m21 +=  a.m31 * b.m23;
  m22 +=  a.m32 * b.m23;
  m23 +=  a.m33 * b.m23;
  // row 3
  m30  =  a.m00 * b.m30;
  m31  =  a.m01 * b.m30;
  m32  =  a.m02 * b.m30;
  m33  =  a.m03 * b.m30;
  m30 +=  a.m10 * b.m31;
  m31 +=  a.m11 * b.m31;
  m32 +=  a.m12 * b.m31;
  m33 +=  a.m13 * b.m31;
  m30 +=  a.m20 * b.m32;
  m31 +=  a.m21 * b.m32;
  m32 +=  a.m22 * b.m32;
  m33 +=  a.m23 * b.m32;
  m30 +=  a.m30 * b.m33;
  m31 +=  a.m31 * b.m33;
  m32 +=  a.m32 * b.m33;
  m33 +=  a.m33 * b.m33;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::quickMult(const XMatrix& a,const XMatrix& b)
{
  XM2_ASSERT( &a != this );
  XM2_ASSERT( &b != this );

  // row 0
  m00  =  a.m00 * b.m00;
  m01  =  a.m01 * b.m00;
  m02  =  a.m02 * b.m00;
  m00 +=  a.m10 * b.m01;
  m01 +=  a.m11 * b.m01;
  m02 +=  a.m12 * b.m01;
  m00 +=  a.m20 * b.m02;
  m01 +=  a.m21 * b.m02;
  m02 +=  a.m22 * b.m02;
  // row 1
  m10  =  a.m00 * b.m10;
  m11  =  a.m01 * b.m10;
  m12  =  a.m02 * b.m10;
  m10 +=  a.m10 * b.m11;
  m11 +=  a.m11 * b.m11;
  m12 +=  a.m12 * b.m11;
  m10 +=  a.m20 * b.m12;
  m11 +=  a.m21 * b.m12;
  m12 +=  a.m22 * b.m12;
  // row 2
  m20  =  a.m00 * b.m20;
  m21  =  a.m01 * b.m20;
  m22  =  a.m02 * b.m20;
  m20 +=  a.m10 * b.m21;
  m21 +=  a.m11 * b.m21;
  m22 +=  a.m12 * b.m21;
  m20 +=  a.m20 * b.m22;
  m21 +=  a.m21 * b.m22;
  m22 +=  a.m22 * b.m22;
  // row 3
  m30  =  a.m00 * b.m30;
  m31  =  a.m01 * b.m30;
  m32  =  a.m02 * b.m30;
  m30 +=  a.m10 * b.m31;
  m31 +=  a.m11 * b.m31;
  m32 +=  a.m12 * b.m31;
  m30 +=  a.m20 * b.m32;
  m31 +=  a.m21 * b.m32;
  m32 +=  a.m22 * b.m32;
  m30 +=  a.m30;
  m31 +=  a.m31;
  m32 +=  a.m32;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::mul(const XMatrix& a,const XReal b)
{
  for(int i=0;i!=16;++i)
    data[i] = b * a.data[i];
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector3SOA& output,const XVector3SOA& input) const
{
  // make sure they are not the same vectors...
  XM2_ASSERT(&output!=&input);

  for (int i=0;i!=4;++i)
  {
    output.x[i]   = m00 * input.x[i];
    output.y[i]   = m01 * input.x[i];
    output.z[i]   = m02 * input.x[i];
    output.x[i]  += m10 * input.y[i];
    output.y[i]  += m11 * input.y[i];
    output.z[i]  += m12 * input.y[i];
    output.x[i]  += m20 * input.z[i];
    output.y[i]  += m21 * input.z[i];
    output.z[i]  += m22 * input.z[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector4SOA& output,const XVector4SOA& input) const
{
  // make sure they are not the same vectors...
  XM2_ASSERT(&output!=&input);

  for (int i=0;i!=4;++i)
  {
    output.x[i]   = m00 * input.x[i];
    output.y[i]   = m01 * input.x[i];
    output.z[i]   = m02 * input.x[i];
    output.w[i]   = m03 * input.x[i];
    output.x[i]  += m10 * input.y[i];
    output.y[i]  += m11 * input.y[i];
    output.z[i]  += m12 * input.y[i];
    output.w[i]  += m13 * input.y[i];
    output.x[i]  += m20 * input.z[i];
    output.y[i]  += m21 * input.z[i];
    output.z[i]  += m22 * input.z[i];
    output.w[i]  += m23 * input.z[i];
    output.x[i]  += m30 * input.w[i];
    output.y[i]  += m31 * input.w[i];
    output.z[i]  += m32 * input.w[i];
    output.w[i]  += m33 * input.w[i];
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformPoint(XVector3SOA& output,const XVector3SOA& input) const
{
  // make sure they are not the same vectors...
  XM2_ASSERT(&output!=&input);

  for (int i=0;i!=4;++i)
  {
    output.x[i]   = m00 * input.x[i];
    output.y[i]   = m01 * input.x[i];
    output.z[i]   = m02 * input.x[i];
    output.x[i]  += m10 * input.y[i];
    output.y[i]  += m11 * input.y[i];
    output.z[i]  += m12 * input.y[i];
    output.x[i]  += m20 * input.z[i];
    output.y[i]  += m21 * input.z[i];
    output.z[i]  += m22 * input.z[i];
    output.x[i]  += m30;
    output.y[i]  += m31;
    output.z[i]  += m32;
  }
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector3& output,const XVector3& input) const
{
  // make sure they are not the same vectors...
  XM2_ASSERT(&output!=&input);

  output.x   = m00 * input.x;
  output.y   = m01 * input.x;
  output.z   = m02 * input.x;
  output.x  += m10 * input.y;
  output.y  += m11 * input.y;
  output.z  += m12 * input.y;
  output.x  += m20 * input.z;
  output.y  += m21 * input.z;
  output.z  += m22 * input.z;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformVector(XVector4& output,const XVector4& input) const
{
  // make sure they are not the same vectors...
  XM2_ASSERT(&output!=&input);

  output.x   = m00 * input.x;
  output.y   = m01 * input.x;
  output.z   = m02 * input.x;
  output.w   = m03 * input.x;
  output.x  += m10 * input.y;
  output.y  += m11 * input.y;
  output.z  += m12 * input.y;
  output.w  += m13 * input.y;
  output.x  += m20 * input.z;
  output.y  += m21 * input.z;
  output.z  += m22 * input.z;
  output.w  += m23 * input.z;
  output.x  += m30 * input.w;
  output.y  += m31 * input.w;
  output.z  += m32 * input.w;
  output.w  += m33 * input.w;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transformPoint(XVector3& output,const XVector3& input) const
{
  // make sure they are not the same vectors...
  XM2_ASSERT(&output!=&input);

  output.x   = m00 * input.x;
  output.y   = m01 * input.x;
  output.z   = m02 * input.x;
  output.x  += m10 * input.y;
  output.y  += m11 * input.y;
  output.z  += m12 * input.y;
  output.x  += m20 * input.z;
  output.y  += m21 * input.z;
  output.z  += m22 * input.z;
  output.x  += m30;
  output.y  += m31;
  output.z  += m32;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transpose()
{
  XM2_ALIGN_ASSERT(this);
  XReal temp;
  temp = m01; m01 = m10; m10 = temp;
  temp = m02; m02 = m20; m20 = temp;
  temp = m03; m03 = m30; m30 = temp;
  temp = m12; m12 = m21; m21 = temp;
  temp = m13; m13 = m31; m31 = temp;
  temp = m23; m23 = m32; m32 = temp;
}
//----------------------------------------------------------------------------------------------------------------------
XM2_INLINE void XMatrix::transpose(const XMatrix& m)
{
  XM2_ALIGN_ASSERT(this);

  m00 = m.m00;
  m01 = m.m10;
  m02 = m.m20;
  m03 = m.m30;

  m10 = m.m01;
  m11 = m.m11;
  m12 = m.m21;
  m13 = m.m31;

  m20 = m.m02;
  m21 = m.m12;
  m22 = m.m22;
  m23 = m.m32;

  m30 = m.m03;
  m31 = m.m13;
  m32 = m.m23;
  m33 = m.m33;
}
}
#endif
