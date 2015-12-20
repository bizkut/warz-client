/*---------------------------------------------------------------------------------------*/
/*
 *  \file   XTransform.cpp
 *  \date   19-05-2008
 */
/*---------------------------------------------------------------------------------------*/

#include "XM2/Transform.h"

namespace XM2
{
#if !XM2_NO_C_IO
//----------------------------------------------------------------------------------------------------------------------
void XTransform::fprint32(FILE* fp) const
{
  translate.fprint32(fp);
  rotate.fprint32(fp);
  scale.fprint32(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::fprint64(FILE* fp) const
{
  translate.fprint64(fp);
  rotate.fprint64(fp);
  scale.fprint64(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::fscan32(FILE* fp)
{
  translate.fscan32(fp);
  rotate.fscan32(fp);
  scale.fscan32(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::fscan64(FILE* fp)
{
  translate.fscan64(fp);
  rotate.fscan64(fp);
  scale.fscan64(fp);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::print32() const
{
  translate.print32();
  rotate.print32();
  scale.print32();
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::print64() const
{
  translate.print64();
  rotate.print64();
  scale.print64();
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::fread32(FILE* fp,bool flip_bytes)
{
  translate.fread32(fp,flip_bytes);
  rotate.fread32(fp,flip_bytes);
  scale.fread32(fp,flip_bytes);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::fread64(FILE* fp,bool flip_bytes)
{
  translate.fread64(fp,flip_bytes);
  rotate.fread64(fp,flip_bytes);
  scale.fread64(fp,flip_bytes);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::fwrite32(FILE* fp,bool flip_bytes) const
{
  translate.fwrite32(fp,flip_bytes);
  rotate.fwrite32(fp,flip_bytes);
  scale.fwrite32(fp,flip_bytes);
}

//----------------------------------------------------------------------------------------------------------------------
void XTransform::fwrite64(FILE* fp,bool flip_bytes) const
{
  translate.fwrite64(fp,flip_bytes);
  rotate.fwrite64(fp,flip_bytes);
  scale.fwrite64(fp,flip_bytes);
}
#endif

#if !XM2_NO_STREAM_IO
//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::ostream& operator<<(std::ostream& ofs,const XTransform& c)
{
  return ofs << c.translate << std::endl 
             << c.rotate << std::endl 
             << c.scale << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
XM2EXPORT std::istream& operator>>(std::istream& ifs,XTransform& c)
{
  return ifs >> c.translate >> c.rotate >> c.scale;
}
#endif
}
