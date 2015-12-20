/*---------------------------------------------------------------------------------------*/
/*!
 *  \file   XMatrix.cpp
 *  \date   19-05-2008
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Vector3.h"
#include "XM2/Vector4.h"
#include "XM2/Vector3SOA.h"
#include "XM2/Vector4SOA.h"
#include "XM2/Quaternion.h"
#include "XM2/Matrix34.h"

namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
XMatrix::XMatrix(const XTransform& c)
{
  c.toMatrix(*this);
}

//----------------------------------------------------------------------------------------------------------------------
void submatrix(const XMatrix& m,const unsigned i,const unsigned j,XReal output[9]) 
{
  unsigned ti, tj, idst=0, jdst=0;

  for( ti = 0; ti < 4; ti++ )
  {
    if(ti<i)
    {
      idst = ti;
    }
    else
    if(ti>i)
    {
      idst = ti-1;
    }

    for(tj=0;tj<4;tj++)
    {
      if(tj<j)
      {
        jdst = tj;
      }
      else
      if(tj>j)
      {
        jdst = tj-1;
      }

      if( ti!=i && tj!=j )
      {
        output[idst*3 + jdst] = m.data[ti*4 + tj];
      }
    }
  }
}

#if XM2_SSE
//----------------------------------------------------------------------------------------------------------------------
void XMatrix::invert(const XMatrix& a)
{
  // can't be the same data!
  XM2_ASSERT(&a != this);

  // ensure not NULL
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);

  // from http://download.intel.com/design/PentiumIII/sml/24504301.pdf
  __m128 minor0, minor1, minor2, minor3;
  __m128 row0, row1, row2, row3;
  __m128 det, tmp1;
  row1=tmp1=row3  =_mm_setzero_ps();
  tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(a.data+0)), (__m64*)(a.data+ 4));
  row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(a.data+8)), (__m64*)(a.data+12));
  row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
  row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
  tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(a.data+ 2)), (__m64*)(a.data+ 6));
  row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(a.data+10)), (__m64*)(a.data+14));
  row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
  row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);
  // -----------------------------------------------
  tmp1 = _mm_mul_ps(row2, row3);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
  minor0 = _mm_mul_ps(row1, tmp1);
  minor1 = _mm_mul_ps(row0, tmp1);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
  minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
  minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
  minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);
  // -----------------------------------------------
  tmp1 = _mm_mul_ps(row1, row2);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
  minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
  minor3 = _mm_mul_ps(row0, tmp1);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
  minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
  minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
  minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);
  // -----------------------------------------------
  tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
  row2 = _mm_shuffle_ps(row2, row2, 0x4E);
  minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
  minor2 = _mm_mul_ps(row0, tmp1);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
  minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
  minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
  minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);
  // -----------------------------------------------
  tmp1 = _mm_mul_ps(row0, row1);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
  minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
  minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
  minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
  minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));
  // -----------------------------------------------
  tmp1 = _mm_mul_ps(row0, row3);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
  minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
  minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
  minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
  minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));
  // -----------------------------------------------
  tmp1 = _mm_mul_ps(row0, row2);
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
  minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
  minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
  tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
  minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
  minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);
  // -----------------------------------------------
  det = _mm_mul_ps(row0, minor0);
  det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
  det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
  tmp1 = _mm_rcp_ss(det);
  det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
  det = _mm_shuffle_ps(det, det, 0x00);
  minor0 = _mm_mul_ps(det, minor0);
  _mm_storel_pi((__m64*)(data), minor0);
  _mm_storeh_pi((__m64*)(data+2), minor0);
  minor1 = _mm_mul_ps(det, minor1);
  _mm_storel_pi((__m64*)(data+4), minor1);
  _mm_storeh_pi((__m64*)(data+6), minor1);
  minor2 = _mm_mul_ps(det, minor2);
  _mm_storel_pi((__m64*)(data+ 8), minor2);
  _mm_storeh_pi((__m64*)(data+10), minor2);
  minor3 = _mm_mul_ps(det, minor3);
  _mm_storel_pi((__m64*)(data+12), minor3);
  _mm_storeh_pi((__m64*)(data+14), minor3);
}
#elif XM2_SSE2
//----------------------------------------------------------------------------------------------------------------------
void XMatrix::invert(const XMatrix& _src)
{
  // can't be the same data!
  XM2_ASSERT(&_src != this);

  // ensure not NULL
  XM2_ALIGN_ASSERT(&_src);
  XM2_ALIGN_ASSERT(this);

  XMatrix src;
  src.transpose(_src);

  __m128d tmp[6];


  /* 
   *  calculate pairs for first 8 elements (cofactors) 
   */

  // tmp[0] = src[10] * src[15];
  // tmp[1] = src[11] * src[14];
  tmp[0] = _mm_mul_pd( src._zaxis_zw, _mm_shuffle_pd(src._waxis_zw,src._waxis_zw,_MM_SHUFFLE2(0,1)));

  // tmp[2] = src[9] * src[15];
  // tmp[3] = src[11] * src[13];
  tmp[1] = _mm_mul_pd( _mm_unpackhi_pd(src._zaxis_xy,src._zaxis_zw), 
                       _mm_unpackhi_pd(src._waxis_xy,src._waxis_zw) );

  //tmp[4]  = src[9]  * src[14];
  //tmp[5]  = src[10] * src[13];
  tmp[2] = _mm_mul_pd( _mm_shuffle_pd(src._zaxis_xy,src._zaxis_zw,_MM_SHUFFLE2(0,1)), 
                       _mm_shuffle_pd(src._waxis_zw,src._waxis_xy,_MM_SHUFFLE2(1,0)) );

  //tmp[6]  = src[8]  * src[15];
  //tmp[7]  = src[11] * src[12];
  tmp[3] = _mm_mul_pd( _mm_shuffle_pd(src._zaxis_xy,src._zaxis_zw,_MM_SHUFFLE2(1,0)), 
                       _mm_shuffle_pd(src._waxis_zw,src._waxis_xy,_MM_SHUFFLE2(0,1)) );

  //tmp[8]  = src[8]  * src[14];
  //tmp[9]  = src[10] * src[12];
  tmp[4] = _mm_mul_pd( _mm_unpacklo_pd(src._zaxis_xy,src._zaxis_zw), 
                       _mm_unpacklo_pd(src._waxis_zw,src._waxis_xy) );

  //tmp[10] = src[8]  * src[13];
  //tmp[11] = src[9]  * src[12];
  tmp[5] = _mm_mul_pd( src._zaxis_xy,_mm_shuffle_pd(src._waxis_xy,src._waxis_xy,_MM_SHUFFLE2(0,1)) );


  /* 
   *  calculate first 8 elements (cofactors) 
   */

  // dst[0]  = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7] - tmp[1]*src[5] - tmp[2]*src[6] - tmp[5]*src[7];
  // dst[1]  = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7] - tmp[0]*src[4] - tmp[7]*src[6] - tmp[8]*src[7];
  {
    const __m128d src_54 = _mm_shuffle_pd(src._yaxis_xy,src._yaxis_xy,_MM_SHUFFLE2(0,1));
    const __m128d src_66 = _mm_unpacklo_pd(src._yaxis_zw,src._yaxis_zw);
    const __m128d src_77 = _mm_unpackhi_pd(src._yaxis_zw,src._yaxis_zw);
    const __m128d tmp_36 = _mm_shuffle_pd(tmp[1],tmp[3],_MM_SHUFFLE2(0,1));
    const __m128d tmp_49 = _mm_shuffle_pd(tmp[2],tmp[4],_MM_SHUFFLE2(1,0));
    const __m128d tmp_10 = _mm_shuffle_pd(tmp[0],tmp[0],_MM_SHUFFLE2(0,1));
    const __m128d tmp_27 = _mm_shuffle_pd(tmp[1],tmp[3],_MM_SHUFFLE2(1,0));
    const __m128d tmp_58 = _mm_shuffle_pd(tmp[2],tmp[4],_MM_SHUFFLE2(0,1));
    
    _xaxis_xy = _mm_sub_pd
                (
                  _mm_add_pd
                  (
                    _mm_add_pd(_mm_mul_pd(tmp[0],src_54),_mm_mul_pd(tmp_36,src_66)),
                    _mm_sub_pd(_mm_mul_pd(tmp_49,src_77),_mm_mul_pd(tmp_10,src_54))
                  ),
                  _mm_add_pd(_mm_mul_pd(tmp_27,src_66),_mm_mul_pd(tmp_58,src_77))
                );
  }

  //   0  1  2  3 
  //   4  5  6  7
  //   8  9 10 11
  //  12 13 14 15


  // dst[2]  = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7] - tmp[3]*src[4] - tmp[6]*src[5] - tmp[11]*src[7];
  // dst[3]  = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6] - tmp[4]*src[4] - tmp[9]*src[5] - tmp[10]*src[6];
  {
    const __m128d src_44 = _mm_unpacklo_pd(src._yaxis_xy,src._yaxis_xy);
    const __m128d src_55 = _mm_unpackhi_pd(src._yaxis_xy,src._yaxis_xy);
    const __m128d src_76 = _mm_shuffle_pd(src._yaxis_zw,src._yaxis_zw,_MM_SHUFFLE2(0,1));

    const __m128d tmp_25 = _mm_shuffle_pd(tmp[1],tmp[3],_MM_SHUFFLE2(0,1));
    const __m128d tmp_78 = _mm_shuffle_pd(tmp[2],tmp[4],_MM_SHUFFLE2(1,0));
    const __m128d tmp_1110 = _mm_shuffle_pd(tmp[5],tmp[5],_MM_SHUFFLE2(0,1));
    const __m128d tmp_34 = _mm_shuffle_pd(tmp[1],tmp[3],_MM_SHUFFLE2(1,0));
    const __m128d tmp_69 = _mm_shuffle_pd(tmp[2],tmp[4],_MM_SHUFFLE2(0,1));
    
    _xaxis_zw = _mm_sub_pd
                (
                  _mm_add_pd
                  (
                    _mm_add_pd(_mm_mul_pd(tmp[0],src_54),_mm_mul_pd(tmp_36,src_66)),
                    _mm_sub_pd(_mm_mul_pd(tmp_49,src_77),_mm_mul_pd(tmp_10,src_54))
                  ),
                  _mm_add_pd(_mm_mul_pd(tmp_27,src_66),_mm_mul_pd(tmp_58,src_77))
                );

  }

  // dst[4]  = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3] - tmp[0]*src[1] - tmp[3]*src[2] - tmp[4]*src[3];
  // dst[5]  = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3] - tmp[1]*src[0] - tmp[6]*src[2] - tmp[9]*src[3];
  {

  }

  // dst[6]  = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3] - tmp[2]*src[0] - tmp[7]*src[1] - tmp[10]*src[3];
  // dst[7]  = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2] - tmp[5]*src[0] - tmp[8]*src[1] - tmp[11]*src[2];
  {

  }
}
#elif XM2_FPU
//----------------------------------------------------------------------------------------------------------------------
void XMatrix::invert(const XMatrix& a)
{
  // can't be the same data!
  XM2_ASSERT(&a != this);

  // ensure not NULL
  XM2_ALIGN_ASSERT(&a);
  XM2_ALIGN_ASSERT(this);

  XReal _determinant =0;
  XReal matrixSub3x3[9];
  _determinant = a.determinant();

  if(XMabs(_determinant) < 0.0005f)
    return;

  for(unsigned i=0;i<4;i++)
  {
    for(unsigned j=0;j<4;j++)
    {
      const int sign = 1 - ((i+j)%2) * 2;
      XReal det;

      submatrix(a,i,j,matrixSub3x3);

      det =   matrixSub3x3[0] * (matrixSub3x3[4]*matrixSub3x3[8] - matrixSub3x3[7]*matrixSub3x3[5])
            - matrixSub3x3[1] * (matrixSub3x3[3]*matrixSub3x3[8] - matrixSub3x3[6]*matrixSub3x3[5])
            + matrixSub3x3[2] * (matrixSub3x3[3]*matrixSub3x3[7] - matrixSub3x3[6]*matrixSub3x3[4]);

      data[i+j*4] = (det*sign)/_determinant;
    }
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
XReal XMatrix::determinant() const
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);

#if 1

  XReal det, result = 0.0f, i = 1.0f;
  XReal matrixSub3x3[9];
  unsigned n;

  for(n = 0; n < 4; n++, i *= -1.0f)
  {
    submatrix(*this,0,n,matrixSub3x3);

    det =   matrixSub3x3[0] * ( matrixSub3x3[4]*matrixSub3x3[8] - matrixSub3x3[7]*matrixSub3x3[5] )
          - matrixSub3x3[1] * ( matrixSub3x3[3]*matrixSub3x3[8] - matrixSub3x3[6]*matrixSub3x3[5] )
          + matrixSub3x3[2] * ( matrixSub3x3[3]*matrixSub3x3[7] - matrixSub3x3[6]*matrixSub3x3[4] );

    result += data[n] * det * i;
  }
  return result;
#else

  #define DET2x2(A,B,C,D) (((A)*(C))- ((B)*(D)))
  XReal S0 = DET2x2(m00,m01,m10,m11);
  XReal C5 = DET2x2(m22,m23,m32,m33);

  XReal S1 = DET2x2(m00,m02,m10,m12);
  XReal C4 = DET2x2(m21,m23,m31,m33);

  XReal S2 = DET2x2(m00,m03,m10,m13);
  XReal C3 = DET2x2(m21,m22,m31,m32);

  XReal S3 = DET2x2(m01,m02,m11,m12);
  XReal C2 = DET2x2(m20,m23,m30,m33);

  XReal S4 = DET2x2(m01,m03,m11,m13);
  XReal C1 = DET2x2(m20,m22,m30,m32);

  XReal S5 = DET2x2(m02,m03,m12,m13);
  XReal C0 = DET2x2(m20,m21,m30,m31);
  #undef DET2x2

 /* +a11c5 - a12c4 + a13c3  |  -a01c5 + a02c4 - a03c3   |  +a31s5 - a32s4 + a33s3   |  -a21s5 + a22s4 - a23s3
  -a10c5 + a12c2 - a13c1  |  +a00c5 - a02c2 + a03c1   |  -a30s5 + a32s2 - a33s1   |  +a20s5 - a22s2 + a23s1
  +a10c4 - a11c2 + a13c0  |  -a00c4 + a01c2 - a03c0   |  +a30s4 - a31s2 + a33s0   |  -a20s4 + a21s2 - a23s0
  -a10c3 + a11c1 - a12c0  |  +a00c3 - a01c1 + a02c0   |  -a30s3 + a31s1 - a32s0   |  +a20s3 - a21s1 + a22s0  */
  return S0*C5 - S1*C4 + S2*C3 + S3*C2 - S4*C1 +S5*C0;
#endif
}
/*
//----------------------------------------------------------------------------------------------------------------------
XReal XMatrix::determinant3x3() const
{
  __m128d deta;
  deta = _mm_shuffle_pd();
  deta = _mm_mul_pd(deta,_mm_shuffle_pd(_xaxis_xy,_xaxis_xy,_mm_shuffle_pd(0,0)))
}
*/
//----------------------------------------------------------------------------------------------------------------------
void XMatrix::fprint32(FILE* fp) const
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);

  // ensure valid
  XM2_ASSERT(fp);

  fprintf(fp,"%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
          (float)m00,(float)m01,(float)m02,(float)m03,
          (float)m10,(float)m11,(float)m12,(float)m13,
          (float)m20,(float)m21,(float)m22,(float)m23,
          (float)m30,(float)m31,(float)m32,(float)m33 );
}
//----------------------------------------------------------------------------------------------------------------------
void XMatrix::fprint64(FILE* fp) const
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);

  // ensure valid
  XM2_ASSERT(fp);

  fprintf(fp,"%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf\n%lf %lf %lf %lf\n",
          (double)m00,(double)m01,(double)m02,(double)m03,
          (double)m10,(double)m11,(double)m12,(double)m13,
          (double)m20,(double)m21,(double)m22,(double)m23,
          (double)m30,(double)m31,(double)m32,(double)m33 );
}


//----------------------------------------------------------------------------------------------------------------------
void XMatrix::fscan32(FILE* fp)
{
  // ensure not NULL
  XM2_ALIGN_ASSERT(this);
  XM2_ASSERT(fp);

  for(int i=0;i<16;++i)
  {
    float temp;
    #if XM2_USE_SECURE_SCL
    fscanf_s(fp,"%f",&temp);
    #else
    fscanf(fp,"%f",&temp);
    #endif
    data[i] = (XReal)temp;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::fscan64(FILE* fp)
{
  // ensure not NULL
  XM2_ALIGN_ASSERT(this);
  XM2_ASSERT(fp);

  for(int i=0;i<16;++i)
  {
    double temp;
    #if XM2_USE_SECURE_SCL
    fscanf_s(fp,"%lf",&temp);
    #else
    fscanf(fp,"%lf",&temp);
    #endif
    data[i] = (XReal)temp;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::fwrite32(FILE* fp,bool flip_bytes) const
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  XM2_ASSERT(fp);

  #if XM2_USE_FLOAT
  if(flip_bytes)
  {
    for(int i=0;i<16;++i)
    {
      XReal temp = data[i];
      XM2swap(temp);
      ::fwrite(&temp,sizeof(float),1,fp);
    }
  }
  else
    ::fwrite(data,sizeof(XMatrix),1,fp);
  #else
  if(flip_bytes)
  {
    for(int i=0;i<16;++i)
    {
      float temp = (float)data[i];
      XM2swap(temp);
      ::fwrite(&temp,sizeof(float),1,fp);
    }
  }
  else
  {
    for(int i=0;i<16;++i)
    {
      float temp = (float)data[i];
      ::fwrite(&temp,sizeof(float),1,fp);
    }
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::fwrite64(FILE* fp,bool flip_bytes) const
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  XM2_ASSERT(fp);

  #if !XM2_USE_FLOAT
  if(flip_bytes)
  {
    for(int i=0;i<16;++i)
    {
      double temp = (double)data[i];
      XM2swap(temp);
      ::fwrite(&temp,sizeof(double),1,fp);
    }
  }
  else
    ::fwrite(data,sizeof(XMatrix),1,fp);
  #else
  if(flip_bytes)
  {
    for(int i=0;i<16;++i)
    {
      double temp = (double)data[i];
      XM2swap(temp);
      ::fwrite(&temp,sizeof(double),1,fp);
    }
  }
  else
  {
    for(int i=0;i<16;++i)
    {
      double temp = (double)data[i];
      ::fwrite(&temp,sizeof(double),1,fp);
    }
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::fread32(FILE* fp,bool flip_bytes)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  XM2_ASSERT(fp);

  #if XM2_USE_FLOAT
  ::fread(data,sizeof(XMatrix),1,fp);
  if(flip_bytes)
    XM2swap32(data,16);
  #else
  if(flip_bytes)
  {
    for(int i=0;i<16;++i)
    {
      float temp;
      ::fread(&temp,sizeof(float),1,fp);
      XM2swap(temp);
      data[i] = (XReal)temp;
    }
  }
  else
  {
    for(int i=0;i<16;++i)
    {
      float temp;
      ::fread(&temp,sizeof(float),1,fp);
      data[i] = (XReal)temp;
    }
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::fread64(FILE* fp,bool flip_bytes)
{
  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  XM2_ASSERT(fp);

  #if !XM2_USE_FLOAT
  ::fread(data,sizeof(XMatrix),1,fp);
  if(flip_bytes)
    XM2swap64(data,16);
  #else
  if(flip_bytes)
  {
    for(int i=0;i<16;++i)
    {
      double temp;
      ::fread(&temp,sizeof(double),1,fp);
      XM2swap(temp);
      data[i] = (XReal)temp;
    }
  }
  else
  {
    for(int i=0;i<16;++i)
    {
      double temp;
      ::fread(&temp,sizeof(double),1,fp);
      data[i] = (XReal)temp;
    }
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::transformPoint(XVector3* output,const XVector3* b,const int num) const
{
  // ensure not NULL
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    transformPoint(output[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::transformPoint(XVector3SOA* output,const XVector3SOA* b,const int num) const
{
  // ensure not NULL
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    transformPoint(output[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::transformVector(XVector4* output,const XVector4* b,const int num) const
{
  // ensure not NULL
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    transformVector(output[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::transformVector(XVector4SOA* output,const XVector4SOA* b,const int num) const
{
  // ensure not NULL
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    transformVector(output[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::transformVector(XVector3* output,const XVector3* b,const int num) const
{
  int i;

  // ensure not NULL
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(i=0;i<num;++i)
  {
    transformVector(output[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMatrix::transformVector(XVector3SOA* output,const XVector3SOA* b,const int num) const
{
  int i;

  // ensure not NULL
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  // ensure 16 byte aligned
  XM2_ALIGN_ASSERT(this);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(i=0;i<num;++i)
  {
    transformVector(output[i],b[i]);
  }
}

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XMatrix& c)
{
  ofs << c.m00 << " " << c.m01 << " " << c.m02 << " " << c.m03 << std::endl
    << c.m10 << " " << c.m11 << " " << c.m12 << " " << c.m13 << std::endl
    << c.m20 << " " << c.m21 << " " << c.m22 << " " << c.m23 << std::endl
    << c.m30 << " " << c.m31 << " " << c.m32 << " " << c.m33 << std::endl;
  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ifs,XMatrix& c)
{
  ifs >> c.m00 >> c.m01 >> c.m02 >> c.m03
    >> c.m10 >> c.m11 >> c.m12 >> c.m13
    >> c.m20 >> c.m21 >> c.m22 >> c.m23
    >> c.m30 >> c.m31 >> c.m32 >> c.m33;
  return ifs;
}
#endif

}