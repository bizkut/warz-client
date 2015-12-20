/*---------------------------------------------------------------------------------------*/
/*!
 *  \file   XRay.cpp
 *  \date   19-05-2008
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Ray.h"

namespace XM2
{
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XRay::fprint32(FILE* fp) const
{
  origin.fprint32(fp);
  direction.fprint32(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XRay::fprint64(FILE* fp) const
{
  origin.fprint64(fp);
  direction.fprint64(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XRay::fscan32(FILE* fp)
{
  origin.fscan32(fp);
  direction.fscan32(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XRay::fscan64(FILE* fp)
{
  origin.fscan64(fp);
  direction.fscan64(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XRay::print32() const
{
  origin.print32();
  direction.print32();
}

//----------------------------------------------------------------------------------------------------------------------
void XRay::print64() const
{
  origin.print64();
  direction.print64();
}

//----------------------------------------------------------------------------------------------------------------------
void XRay::fread32(FILE* fp,bool flip_bytes)
{
  origin.fread32(fp,flip_bytes);
  direction.fread32(fp,flip_bytes);
}

//----------------------------------------------------------------------------------------------------------------------
void XRay::fread64(FILE* fp,bool flip_bytes)
{
  origin.fread64(fp,flip_bytes);
  direction.fread64(fp,flip_bytes);
}

//----------------------------------------------------------------------------------------------------------------------
void XRay::fwrite32(FILE* fp,bool flip_bytes) const
{
  origin.fwrite32(fp,flip_bytes);
  direction.fwrite32(fp,flip_bytes);
}

//----------------------------------------------------------------------------------------------------------------------
void XRay::fwrite64(FILE* fp,bool flip_bytes) const
{
  origin.fwrite64(fp,flip_bytes);
  direction.fwrite64(fp,flip_bytes);
}
#endif

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XRay& c)
{
  return ofs << c.origin << std::endl <<  c.direction << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ofs,XRay& c)
{
  return ofs >> c.origin >> c.direction;
}
#endif

}
