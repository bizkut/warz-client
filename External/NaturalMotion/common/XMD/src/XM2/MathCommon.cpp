/*---------------------------------------------------------------------------------------*/
/*!
 *  \file   MathCommon.c
 *  \date   19-05-2008
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/MathCommon.h"
#if !XM2_NO_CRT
#include <stdlib.h>
#endif

namespace XM2
{
#ifdef _MSC_VER
#pragma warning(disable:4311)
#endif

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT bool XM2Is16ByteAligned(const void* ptr)
{
  unsigned char* _ptr = (unsigned char*)ptr;
  unsigned char* _zero =0;
  ptrdiff_t address = _ptr-_zero;
  return (address & 15) ? false : true;
};

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT void XM2swap32(void* _data,unsigned num)
{
  const unsigned char* ub_null = 0;

  unsigned char* ubdata = (unsigned char*)(_data);

  ptrdiff_t address = ubdata - ub_null;

  // if address not divisible by 4, it's best off left to retarded methods....
  if (address%4)
  {
  }
  else
  {
    float* fptr = (float*)_data;
    ptrdiff_t num_floats = 4 - ((address%16U)/4U);
    for (ptrdiff_t i=0;i!=num_floats;++i)
    {
      XM2swap(fptr[i]);
    }
    fptr += num_floats;
    num -= (unsigned)num_floats;

    unsigned num_remaining = num/4;
    XM2swap( (__m128*)((void*)fptr), num/4 );

    num_remaining *= 4;
    fptr += num_remaining;

    num_remaining = num - num_remaining;
    for (unsigned i=0;i!=num_remaining;++i)
    {
      XM2swap(fptr[i]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT void XM2swap64(void* data,unsigned num)
{
  (void) data;
  (void) num;
  /// \todo 
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT void XM2swap(__m128* data,unsigned num)
{
  #if XM2_USE_OMP
  # pragma omp parallel for
  #endif
  for (int i=0;i<((int)num);++i)
  {
    XM2swap(data[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT void XM2swap(__m128d* data,unsigned num)
{
  #if XM2_USE_OMP
  # pragma omp parallel for
  #endif
  for (int i=0;i<((int)num);++i)
  {
    XM2swap(data[i]);
  }
}
#ifdef _MSC_VER
#pragma warning(default:4311)
#endif

}