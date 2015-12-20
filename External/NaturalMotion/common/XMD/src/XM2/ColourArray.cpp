/*---------------------------------------------------------------------------------------*/
/*!
 *  \file   ColourArray.cpp
 *  \date   19-05-2008
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Colour.h"
#include "XM2/ColourArray.h"

namespace XM2
{
NMTL_POD_VECTOR_EXPORT(XColour,XM2EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
bool XColour::arrayEqual(const XColour* a,const XColour* b,const int num,const XReal eps)
{
  for (int i =0;i!=num;++i)
  {
    if(!a[i].equal(b[i],eps))
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XColour::arrayNotEqual(const XColour* a,const XColour* b,const int num,const XReal eps)
{
  for (int i =0;i!=num;++i)
  {
    if(!a[i].notEqual(b[i],eps))
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayZero(XColour* output,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      const __m128 _zero = _mm_setzero_ps();
      #if XM2_USE_OMP
      #pragma omp parallel for
      #endif
      for(int i=0;i<num;++i)
      {
        _mm_stream_ps(output[i].data,_zero);
      }
    #else
      const __m128d _zero = _mm_setzero_pd();
      #if XM2_USE_OMP
      #pragma omp parallel for
      #endif
      for(int i=0;i<num;++i)
      {
        _mm_stream_pd(output[i].data,_zero);
        _mm_stream_pd(output[i].data+2,_zero);
      }
    #endif
  #else
    memset(output,0,sizeof(XColour)*num);
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayWhite(XColour* output,const int num)
{
  XM2_ASSERT(output);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      const __m128 _white = _mm_set_ps1(1.0f);
      #if XM2_USE_OMP
      #pragma omp parallel for
      #endif
      for(int i=0;i<num;++i)
      {
        _mm_stream_ps(output[i].data,_white);
      }
    #else
      const __m128d _white = _mm_set1_pd(1.0);
      #if XM2_USE_OMP
      #pragma omp parallel for
      #endif
      for(int i=0;i<num;++i)
      {
        _mm_stream_pd(output[i].data,_white);
        _mm_stream_pd(output[i].data+2,_white);
      }
    #endif
  #else
    #if XM2_USE_OMP
    #pragma omp parallel for
    #endif
    for(int i=0;i<num;++i)
    {
      output[i].white();
    }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayMul(XColour* output,const XColour* a,const XColour* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].mul(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayMul(XColour* output,const XColour* a,const XColour& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].mul(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayMul(XColour* output,const XColour* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].mul(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayMul(XColour* output,const XColour* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      __m128 _b = _mm_set_ps1(b);
    #else
      __m128d _b = _mm_set1_pd(b);
    #endif
  #endif

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        output[i].sse = _mm_mul_ps(_b,a[i].sse);
      #else
        output[i].rg = _mm_mul_pd(_b,a[i].rg);
        output[i].ba = _mm_mul_pd(_b,a[i].ba);
      #endif
    #else
      output[i].mul(a[i],b);
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayDiv(XColour* output,const XColour* a,const XColour* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].div(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayDiv(XColour* output,const XColour* a,const XColour& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].div(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayDiv(XColour* output,const XColour* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].div(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayDiv(XColour* output,const XColour* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);
  
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      __m128 _b = _mm_set_ps1(b);
    #else
      __m128d _b = _mm_set1_pd(b);
    #endif
  #endif

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        output[i].sse = _mm_div_ps(a[i].sse,_b);
      #else
        output[i].rg = _mm_div_pd(a[i].rg,_b);
        output[i].ba = _mm_div_pd(a[i].ba,_b);
      #endif
    #else
      output[i].div(a[i],b);
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayAdd(XColour* output,const XColour* a,const XColour* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].add(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayAdd(XColour* output,const XColour* a,const XColour& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].add(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayAdd(XColour* output,const XColour* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].add(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayAdd(XColour* output,const XColour* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      __m128 _b = _mm_set_ps1(b);
    #else
      __m128d _b = _mm_set1_pd(b);
    #endif
  #endif

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        output[i].sse = _mm_add_ps(_b,a[i].sse);
      #else
        output[i].rg = _mm_add_pd(_b,a[i].rg);
        output[i].ba = _mm_add_pd(_b,a[i].ba);
      #endif
    #else
      output[i].add(a[i],b);
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arraySub(XColour* output,const XColour* a,const XColour* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].sub(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arraySub(XColour* output,const XColour* a,const XColour& b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(&b);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].sub(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arraySub(XColour* output,const XColour* a,const XReal* b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].sub(a[i],b[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arraySub(XColour* output,const XColour* a,const XReal b,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);

  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      __m128 _b = _mm_set_ps1(b);
    #else
      __m128d _b = _mm_set1_pd(b);
    #endif
  #endif

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    output[i].sub(a[i],b);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayInvert(XColour* output,const XColour* input,const int num)
{
  XM2_ALIGN_ASSERT(input);
  XM2_ALIGN_ASSERT(output);

  XM2_ASSERT(input);
  XM2_ASSERT(output);

  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT  
      const __m128 _one = _mm_set_ps1(1.0f);
      #if XM2_USE_OMP
      #pragma omp parallel for
      #endif
      for(int i=0;i<num;++i)
      {
        output[i].sse = _mm_sub_ps(_one,input[i].sse);          
      }
    #else
      const __m128d _one = _mm_set1_pd(1.0f);
      #if XM2_USE_OMP
      #pragma omp parallel for
      #endif
      for(int i=0;i<num;++i)
      {
        output[i].rg = _mm_sub_pd(_one,input[i].rg);
        output[i].ba = _mm_sub_pd(_one,input[i].ba);
      }
    #endif
  #else
    #if XM2_USE_OMP
    #pragma omp parallel for
    #endif
    for(int i=0;i<num;++i)
    {
      output[i].invert(input[i]);
    }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayNegate(XColour* output,const XColour* input,const int num)
{
  XM2_ALIGN_ASSERT(input);
  XM2_ALIGN_ASSERT(output);

  XM2_ASSERT(input);
  XM2_ASSERT(output);

  #ifdef XM2_USE_SSE
    XColour::arrayMul(output,input,-1.0f,num);
  #else
    #if XM2_USE_OMP
    #pragma omp parallel for
    #endif
    for(int i=0;i<num;++i)
    {
      output[i].negate(input[i]);
    }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arraySum(XColour* output,const XColour* a,const XReal& weight,const int num)
{
  XM2_ASSERT(a);
  XM2_ASSERT(output);

  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(output);
  
  #ifdef XM2_USE_SSE
    #if XM2_USE_FLOAT
      __m128 _b = _mm_set_ps1(weight);
    #else
      __m128d _b = _mm_set1_pd(weight);
    #endif
  #endif

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for(int i=0;i<num;++i)
  {
    #ifdef XM2_USE_SSE
      #if XM2_USE_FLOAT
        output[i].sse = _mm_add_ps(_mm_mul_ps(_b,a[i].sse),output[i].sse);
      #else
        output[i].rg = _mm_add_pd(_mm_mul_pd(_b,a[i].rg),output[i].rg);
        output[i].ba = _mm_add_pd(_mm_mul_pd(_b,a[i].ba),output[i].ba);
      #endif
    #else
      output[i].mul(a[i],weight);
    #endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayLerp(XColour* output,
                        const XColour* a,
                        const XColour* b,
                        const XReal t,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].lerp(a[i],b[i],t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arrayBezier(XColour* output,
                          const XColour* a,const XColour* b,
                          const XColour* c,const XColour* d,
                          const XReal t,const int num)
{
  XM2_ASSERT(output);
  XM2_ASSERT(a);
  XM2_ASSERT(b);
  XM2_ASSERT(c);
  XM2_ASSERT(d);

  XM2_ALIGN_ASSERT(output);
  XM2_ALIGN_ASSERT(a);
  XM2_ALIGN_ASSERT(b);
  XM2_ALIGN_ASSERT(c);
  XM2_ALIGN_ASSERT(d);

  #if XM2_USE_OMP
  #pragma omp parallel for
  #endif
  for (int i=0;i<num;++i)
  {
    output[i].bezier(a[i],b[i],c[i],d[i],t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColour::arraySum(XColour* output,const XColour& a,const XReal& weight,const int num)
{
  XM2_ALIGN_ASSERT(&a);
  XColour temp;
  temp.mul(a,weight);
  XColour::arrayAdd(output,temp,num);
}

#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XColourArray::fprint32(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint32(fp);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void XColourArray::fprint64(FILE* fp) const
{
  ::fprintf(fp,"%d\n",size());
  for (const_iterator it = begin();it != end(); ++it)
  {
    it->fprint64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColourArray::fscan32(FILE* fp)
{
  int n;
  #if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%d",&n);
  #else
  ::fscanf(fp,"%d",&n);
  #endif
  resize((size_t)n);
  for (iterator it = begin();it != end(); ++it)
  {
    it->fscan32(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColourArray::fscan64(FILE* fp)
{
  int n;
  #if XM2_USE_SECURE_SCL
  ::fscanf_s(fp,"%d",&n);
  #else
  ::fscanf(fp,"%d",&n);
  #endif
  resize((size_t)n);
  for (iterator it = begin();it != end(); ++it)
  {
    it->fscan64(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XColourArray::print32() const
{
  fprint32(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XColourArray::print64() const
{
  fprint64(stdout);
}

//----------------------------------------------------------------------------------------------------------------------
void XColourArray::fwrite32(FILE* fp,bool flip_bytes) const
{
  unsigned n = (unsigned)size();
  ::fwrite(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  if(flip_bytes)
  {
    for (const_iterator it = begin();it != end();++it)
    {
      it->fwrite32(fp,flip_bytes);
    }
  }
  else
    ::fwrite(buffer(),sizeof(XColour),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XColourArray::fwrite64(FILE* fp,bool flip_bytes) const
{
  unsigned n = (unsigned)size();
  ::fwrite(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  if(flip_bytes)
  {
    for (const_iterator it = begin();it != end();++it)
    {
      it->fwrite32(fp,flip_bytes);
    }
  }
  else
    ::fwrite(buffer(),sizeof(XColour),n,fp);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fwrite64(fp,flip_bytes);
  }
  #endif  
}

//----------------------------------------------------------------------------------------------------------------------
void XColourArray::fread32(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XColour),n,fp);
  if(flip_bytes) 
    XM2swap32(buffer(),n*4);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread32(fp,flip_bytes);
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
void XColourArray::fread64(FILE* fp,bool flip_bytes)
{
  unsigned n = (unsigned)size();
  ::fread(&n,sizeof(unsigned),1,fp);

  #if !XM2_USE_FLOAT
  ::fread(buffer(),sizeof(XColour),n,fp);
  if(flip_bytes) 
    XM2swap64(buffer(),n*4);
  #else
  for (const_iterator it = begin();it != end();++it)
  {
    it->fread64(fp,flip_bytes);
  }
  #endif  
}
#endif

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XColourArray& c)
{
  ofs << (unsigned)c.size() << std::endl;
  for (XM2::XColourArray::const_iterator it = c.begin();it != c.end();++it)
  {
    ofs << it->r << " " << it->g << " " << it->b << " " << it->a << std::endl;
  }
  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ifs,XColourArray& c)
{
  unsigned sz;
  ifs >> sz;
  c.resize(sz);
  for (XM2::XColourArray::iterator it = c.begin();it != c.end();++it)
  {
    ifs >> it->r >> it->g >> it->b >> it->a;
  }
  return ifs;
}
#endif

}
