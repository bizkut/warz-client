//----------------------------------------------------------------------------------------------------------------------
///  \file    Attribute.cpp
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Attribute.h"
#include "XMD/Base.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
NMTL_POD_VECTOR_EXPORT(XInt2,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XInt3,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XInt4,XMD_EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& ofs,const XInt2& out)
{
  return ofs << out.u << " " << out.v;
}

//----------------------------------------------------------------------------------------------------------------------
std::istream& operator>>(std::istream& ifs,XInt2& dat)
{
  return ifs >> dat.u >> dat.v;
}

//----------------------------------------------------------------------------------------------------------------------
XInt2::XInt2()
{
  u=v=0;
}

XInt2::XInt2(XS32 a, XS32 b) 
{ 
  u=a;
  v=b; 
}

//----------------------------------------------------------------------------------------------------------------------
XInt2::~XInt2()
{
}

//----------------------------------------------------------------------------------------------------------------------
XInt2::XInt2(const XInt2& i2)
{
  u=i2.u;
  v=i2.v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInt2::operator == (const XInt2& i2) const
{
  return x == i2.x && y == i2.y;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInt2::operator != (const XInt2& i2) const
{
  return x != i2.x || y != i2.y;
}

//----------------------------------------------------------------------------------------------------------------------
const XInt2& XInt2::operator = (const XInt2& i2)
{
  u=i2.u;
  v=i2.v;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void XInt2::Set(XS32 x_,XS32 y_)
{
  x=x_;
  y=y_;
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& ofs,const XInt3& out)
{
  return ofs << out.x << " " << out.y << " " << out.z ;
}

//----------------------------------------------------------------------------------------------------------------------
std::istream& operator>>(std::istream& ifs,XInt3& dat)
{
  return ifs >> dat.x >> dat.y >> dat.z ;
}

//----------------------------------------------------------------------------------------------------------------------
XInt3::XInt3()
{
  x=y=z=0;
}

//----------------------------------------------------------------------------------------------------------------------
XInt3::XInt3(XS32 a, XS32 b, XS32 c) 
{ 
  x=a;
  y=b;
  z=c; 
}

//----------------------------------------------------------------------------------------------------------------------
XInt3::~XInt3()
{
}

//----------------------------------------------------------------------------------------------------------------------
XInt3::XInt3(const XInt3& i3)
{
  x=i3.x;
  y=i3.y;
  z=i3.z;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInt3::operator == (const XInt3& i2) const
{
  return x == i2.x && y == i2.y && z == i2.z;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInt3::operator != (const XInt3& i2) const
{
  return x != i2.x || y != i2.y || z != i2.z;
}

//----------------------------------------------------------------------------------------------------------------------
const XInt3& XInt3::operator = (const XInt3& i3)
{
  x=i3.x;
  y=i3.y;
  z=i3.z;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void XInt3::Set(XS32 x_,XS32 y_,XS32 z_)
{
  x=x_;
  y=y_;
  z=z_;
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& ofs,const XInt4& out)
{
  return ofs << out.x << " " << out.y << " " << out.z << " " << out.w;
}

//----------------------------------------------------------------------------------------------------------------------
std::istream& operator>>(std::istream& ifs,XInt4& dat)
{
  return ifs >> dat.x >> dat.y >> dat.z >> dat.w;
}

//----------------------------------------------------------------------------------------------------------------------
XInt4::XInt4()
{
  x=y=z=w=0;
}

//----------------------------------------------------------------------------------------------------------------------
XInt4::~XInt4()
{
}

//----------------------------------------------------------------------------------------------------------------------
XInt4::XInt4(const XInt4& i4)
{
  x=i4.x;
  y=i4.y;
  z=i4.z;
  w=i4.w;
}

//----------------------------------------------------------------------------------------------------------------------
XInt4::XInt4(XS32 a, XS32 b, XS32 c, XS32 d) 
{ 
  x=a;
  y=b;
  z=c;
  w=d; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XInt4::operator == (const XInt4& i2) const
{
  return x == i2.x && y == i2.y && z == i2.z && w == i2.w;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInt4::operator != (const XInt4& i2) const
{
  return x != i2.x || y != i2.y || z != i2.z || w != i2.w;
}

//----------------------------------------------------------------------------------------------------------------------
const XInt4& XInt4::operator = (const XInt4& i4)
{
  x=i4.x;
  y=i4.y;
  z=i4.z;
  w=i4.w;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void XInt4::Set(XS32 x_,XS32 y_,XS32 z_,XS32 w_)
{
  x=x_;
  y=y_;
  z=z_;
  w=w_;
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute::Connection::Connection()
  : node_id(0),array_idx(0),extra_flag()
{
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute::Connection::Connection(const XId id_,const XChar* str,const XId array_idx_)
  : node_id(id_),array_idx(array_idx_),extra_flag(str)
{
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute::Connection::Connection(const XAttribute::Connection& n)
  : node_id(n.node_id),array_idx(n.array_idx),extra_flag(n.extra_flag)
{
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAttribute::Connection::GetDataSize() const
{
  XU32 sz = (XU32)sizeof(XId);
  sz += (XU32)sizeof(XId);
  sz += (XU32)sizeof(XU16);
  sz += (XU32)extra_flag.size();
  return sz;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Connection::DoData(XFileIO& io)
{
  IO_CHECK( io.DoData(&node_id) );
  IO_CHECK( io.DoData(&array_idx) );
  IO_CHECK( io.DoDataString(extra_flag) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::SetName(const XChar* n)
{
  m_AttrName = n;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAttribute::GetDataSize() const
{
  XU32 sz=static_cast<XU32>(sizeof(XFn::Type));
  switch(m_AttributeType)
  {
  case XFn::BoolAttribute:
    sz += static_cast<XU32>(sizeof(bool));
    break;

  case XFn::ColourAttribute:
    sz += static_cast<XU32>(sizeof(XColour));
    break;

  case XFn::FloatAttribute:
    sz += static_cast<XU32>(sizeof(XReal));
    break;

  case XFn::Float2Attribute:
    sz += static_cast<XU32>(sizeof(XVector2));
    break;

  case XFn::Float3Attribute:
    sz += static_cast<XU32>(3*sizeof(XReal));
    break;

  case XFn::Float4Attribute:
    sz += static_cast<XU32>(sizeof(XVector4));
    break;

  case XFn::IntAttribute:
    sz += static_cast<XU32>(sizeof(XS32));
    break;

  case XFn::Int2Attribute:
    sz += static_cast<XU32>(sizeof(XInt2));
    break;

  case XFn::Int3Attribute:
    sz += static_cast<XU32>(sizeof(XInt3));
    break;

  case XFn::Int4Attribute:
    sz += static_cast<XU32>(sizeof(XInt4));
    break;

  case XFn::StringAttribute:
    sz += static_cast<XU32>(sizeof(XU16) + s->size());
    break;

  case XFn::MatrixAttribute:
    sz += static_cast<XU32>(sizeof(XMatrix));
    break;

  case XFn::MessageAttribute:
    sz += 0;
    break;


  case XFn::BoolArrayAttribute:
    sz += static_cast<XU32>(4 + ab->size()*sizeof(bool));
    break;

  case XFn::ColourArrayAttribute:
    sz += static_cast<XU32>(4 + acol->size()*sizeof(XColour));
    break;

  case XFn::FloatArrayAttribute:
    sz += static_cast<XU32>(4 + af->size()*sizeof(XReal));
    break;

  case XFn::Float2ArrayAttribute:
    sz += static_cast<XU32>(4 + af2->size()*sizeof(XVector2));
    break;

  case XFn::Float3ArrayAttribute:
    sz += static_cast<XU32>(4 + af3->size()*3*sizeof(XReal));
    break;

  case XFn::Float4ArrayAttribute:
    sz += static_cast<XU32>(4 + af4->size()*sizeof(XVector4));
    break;

  case XFn::IntArrayAttribute:
    sz += static_cast<XU32>(4 + ai->size()*sizeof(XS32));
    break;

  case XFn::Int2ArrayAttribute:
    sz += static_cast<XU32>(4 + ai2->size()*sizeof(XInt2));
    break;

  case XFn::Int3ArrayAttribute:
    sz += static_cast<XU32>(4 + ai3->size()*sizeof(XInt3));
    break;

  case XFn::Int4ArrayAttribute:
    sz += static_cast<XU32>(4 + ai4->size()*sizeof(XInt4));
    break;


  case XFn::StringArrayAttribute:
    {
      sz+=4;
      for(XU32 i=0;i!=as->size();++i)
      {
        sz += static_cast<XU32>((*as)[i].size() + 2);
      }
    }
    break;

  case XFn::MatrixArrayAttribute:
    sz += static_cast<XU32>(4 + am->size()*sizeof(XMatrix));
    break;

  case XFn::MessageArrayAttribute:
    sz += sizeof(XS32);
    break;

  default:
    // dont know what's happened, or why, but you really should not
    // get here!!!
    XMD_ASSERT(0);
    break;
  }

  // add connection data size
  sz += 2*sizeof(XU32);
  if(m_Inputs)
  {
    ConnectionArray::const_iterator it = m_Inputs->begin();
    ConnectionArray::const_iterator end = m_Inputs->end();
    for (;it != end;++it)
    {
      sz += it->GetDataSize();
    }
  }
  if(m_Outputs)
  {
    ConnectionArray::const_iterator it = m_Outputs->begin();
    ConnectionArray::const_iterator end = m_Outputs->end();
    for (;it != end;++it)
    {
      sz += it->GetDataSize();
    }
  }

  return sz + 2 + static_cast<XU32>(m_AttrName.size());
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute::XAttribute(const XAttribute& attr)
{
  m_AttrName = attr.m_AttrName;
  m_AttributeType = attr.m_AttributeType;
  switch(m_AttributeType)
  {
  case XFn::MessageAttribute:
    break;
  case XFn::BoolAttribute:
    b = attr.b;
    break;
  case XFn::ColourAttribute:
    col = new XColour(*attr.col);
    break;
  case XFn::FloatAttribute:
    f = attr.f;
    break;
  case XFn::Float2Attribute:
    f2 = new XVector2(*attr.f2);
    break;
  case XFn::Float3Attribute:
    f3 = new XVector3(*attr.f3);
    break;
  case XFn::Float4Attribute:
    f4 = new XVector4(*attr.f4);
    break;
  case XFn::IntAttribute:
    i = attr.i;
    break;
  case XFn::Int2Attribute:
    i2 = new XInt2(*attr.i2);
    break;
  case XFn::Int3Attribute:
    i3 = new XInt3(*attr.i3);
    break;
  case XFn::Int4Attribute:
    i4 = new XInt4(*attr.i4);
    break;
  case XFn::StringAttribute:
    s = new XString(*attr.s);
    break;
  case XFn::MatrixAttribute:
    m = new XMatrix(*attr.m);
    break;
  case XFn::BoolArrayAttribute:
    ab = new XBoolArray(*attr.ab);
    break;
  case XFn::ColourArrayAttribute:
    acol = new XColourArray(*attr.acol);
    break;
  case XFn::FloatArrayAttribute:
    af = new XRealArray(*attr.af);
    break;
  case XFn::Float2ArrayAttribute:
    af2 = new XVector2Array(*attr.af2);
    break;
  case XFn::Float3ArrayAttribute:
    af3 = new XVector3Array(*attr.af3);
    break;
  case XFn::Float4ArrayAttribute:
    af4 = new XVector4Array(*attr.af4);
    break;
  case XFn::IntArrayAttribute:
    ai = new XS32Array(*attr.ai);
    break;
  case XFn::Int2ArrayAttribute:
    ai2 = new XInt2Array(*attr.ai2);
    break;
  case XFn::Int3ArrayAttribute:
    ai3 = new XInt3Array(*attr.ai3);
    break;
  case XFn::Int4ArrayAttribute:
    ai4 = new XInt4Array(*attr.ai4);
    break;
  case XFn::StringArrayAttribute:
    as = new XStringList(*attr.as);
    break;
  case XFn::MatrixArrayAttribute:
    am = new XMatrixArray(*attr.am);
    break;
  case XFn::MessageArrayAttribute:
    break;

  default:
    // dont know what's happened, or why, but you really should not
    // get here!!!
    XMD_ASSERT(0);
    break;
  }

  // if we are duplicating attributes, then the connections are not
  // going to be duplicated as well. In theory you could do, but
  // chances are that you probably don't want them duplicated ;)
  //
  // The only reason an attribute would be duplicated is because
  // the node to which it belongs has been duplicated. If that happens
  // then you probably don't want to maintain connections to the original
  // data since it may confuse a shading network or other node/attr
  // connection scheme. For now, it is advisable to maintain attr
  // connections yourself if they are required.
  //
  m_Inputs=0;
  m_Outputs=0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::DoData(XFileIO& io,XModel* model)
{
  DUMPER(XAttribute);

  if(io.IsReading())
  {
    XFn::Type AttributeType;

    CHECK_IO( io.DoData(&AttributeType) );
    DPARAM(AttributeType);

    SetType(AttributeType);
  }
  else
  {
    CHECK_IO( io.DoData(&m_AttributeType) );
    DPARAM(m_AttributeType);
  }

  CHECK_IO( io.DoDataString(m_AttrName) );
  DPARAM(m_AttrName);

  switch(m_AttributeType)
  {
  case XFn::BoolAttribute:
    CHECK_IO( io.DoData(&b) );
    DPARAM(b);
    break;

  case XFn::ColourAttribute:
    CHECK_IO( io.DoData(col) );
    DPARAM(*col);
    break;

  case XFn::FloatAttribute:
    CHECK_IO( io.DoData(&f) );
    DPARAM(f);
    break;

  case XFn::Float2Attribute:
    CHECK_IO( io.DoData(f2) );
    DPARAM(*f2);
    break;

  case XFn::Float3Attribute:
    CHECK_IO( io.DoData(f3) );
    DPARAM(*f3);
    break;

  case XFn::Float4Attribute:
    CHECK_IO( io.DoData(f4) );
    DPARAM(*f4);
    break;

  case XFn::IntAttribute:
    CHECK_IO( io.DoData(&i) );
    DPARAM(i);
    break;

  case XFn::Int2Attribute:
    CHECK_IO( io.DoData(i2) );
    DPARAM(*i2);
    break;

  case XFn::Int3Attribute:
    CHECK_IO( io.DoData(i3) );
    DPARAM(*i3);
    break;

  case XFn::Int4Attribute:
    CHECK_IO( io.DoData(i4) );
    DPARAM(*i4);
    break;

  case XFn::StringAttribute:
    CHECK_IO( io.DoDataString(*s) );
    DPARAM(*s);
    break;

  case XFn::MatrixAttribute:
    CHECK_IO( io.DoData(m->data,16) );
    DPARAM(*m);
    break;

  case XFn::BoolArrayAttribute:
    CHECK_IO( io.DoDataVector( (*ab) ) );
    DAPARAM((*ab));
    break;

  case XFn::ColourArrayAttribute:
    CHECK_IO( io.DoDataVector(*acol) );
    DAPARAM(*acol);
    break;

  case XFn::FloatArrayAttribute:
    CHECK_IO( io.DoDataVector(*af) );
    DAPARAM(*af);
    break;

  case XFn::Float2ArrayAttribute:
    CHECK_IO( io.DoDataVector(*af2) );
    DAPARAM(*af2);
    break;

  case XFn::Float3ArrayAttribute:
    CHECK_IO( io.DoDataVector(*af3) );
    DAPARAM(*af3);
    break;

  case XFn::Float4ArrayAttribute:
    CHECK_IO( io.DoDataVector(*af4) );
    DAPARAM(*af4);
    break;

  case XFn::IntArrayAttribute:
    CHECK_IO( io.DoDataVector(*ai) );
    DAPARAM(*ai);
    break;

  case XFn::Int2ArrayAttribute:
    CHECK_IO( io.DoDataVector(*ai2) );
    DAPARAM(*ai2);
    break;

  case XFn::Int3ArrayAttribute:
    CHECK_IO( io.DoDataVector(*ai3) );
    DAPARAM(*ai3);
    break;

  case XFn::Int4ArrayAttribute:
    CHECK_IO( io.DoDataVector(*ai4) );
    DAPARAM(*ai4);
    break;

  case XFn::StringArrayAttribute:
    CHECK_IO( io.DoStringVector(*as) );
    DAPARAM(*as);
    break;

  case XFn::MatrixArrayAttribute:
    CHECK_IO( io.DoDataVector(*am) );
    DAPARAM(*am);
    break;

  case XFn::MessageArrayAttribute:
    if( (io.IsReading() && model->GetInputVersion() > 2) || io.IsWriting() )
    {
      CHECK_IO( io.DoData(&i) );
      DPARAM(i);
    }
    break;

  default:
    break;
  }

  if( (io.IsReading() && model->GetInputVersion() > 2) || io.IsWriting() )
  {
    // add connection data size
    XU32 ni = m_Inputs ? static_cast<XU32>(m_Inputs->size()) : 0;
    io.DoData(&ni,1);
    if(io.IsReading())
    {
      if(ni)
      {
        m_Inputs =new ConnectionArray;
        m_Inputs->resize(ni);
      }
    }
    if(m_Inputs)
    {
      ConnectionArray::iterator it = m_Inputs->begin();
      ConnectionArray::iterator end = m_Inputs->end();
      for (;it != end;++it)
      {
        it->DoData(io);
      }
    }
    XU32 no = m_Outputs ? static_cast<XU32>(m_Outputs->size()) : 0;
    io.DoData(&no,1);
    if(io.IsReading())
    {
      if(no)
      {
        m_Outputs =new ConnectionArray;
        m_Outputs->resize(no);
      }
    }
    if(m_Outputs)
    {
      ConnectionArray::iterator it = m_Outputs->begin();
      ConnectionArray::iterator end = m_Outputs->end();
      for (;it != end;++it)
      {
        it->DoData(io);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XAttribute::Name() const
{
  return m_AttrName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
const XFn::Type& XAttribute::Type() const
{
  return m_AttributeType;
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute::XAttribute()
{ 
  DUMPER(XAttribute__ctor);
  m_AttributeType = XFn::BoolAttribute;
  i=0; m_Inputs=m_Outputs=0;
  m_AttrName = "default";
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Clean()
{
  DUMPER(XAttribute__Clean);
  switch(m_AttributeType)
  {
  case XFn::BoolArrayAttribute:
    delete ab;
    break;
  case XFn::FloatArrayAttribute:
    delete af;
    break;
  case XFn::Float2ArrayAttribute:
    delete af2;
    break;
  case XFn::Float3ArrayAttribute:
    delete af3;
    break;
  case XFn::Float4ArrayAttribute:
    delete af4;
    break;
  case XFn::IntArrayAttribute:
    delete ai;
    break;
  case XFn::Int2ArrayAttribute:
    delete ai2;
    break;
  case XFn::Int3ArrayAttribute:
    delete ai3;
    break;
  case XFn::Int4ArrayAttribute:
    delete ai4;
    break;
  case XFn::ColourArrayAttribute:
    delete acol;
    break;
  case XFn::StringArrayAttribute:
    delete as;
    break;
  case XFn::StringAttribute:
    delete s;
    break;
  case XFn::ColourAttribute:
    delete col;
    break;
  case XFn::Int2Attribute:
    delete i2;
    break;
  case XFn::Int3Attribute:
    delete i3;
    break;
  case XFn::Int4Attribute:
    delete i4;
    break;
  case XFn::Float2Attribute:
    delete f2;
    break;
  case XFn::Float3Attribute:
    delete f3;
    break;
  case XFn::Float4Attribute:
    delete f4;
    break;
  case XFn::MatrixAttribute:
    delete m;
    break;
  case XFn::MatrixArrayAttribute:
    delete am;
    break;
  default:
    break;
  }
  i=0;
  delete m_Outputs;
  delete m_Inputs;
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute::~XAttribute()
{
  DUMPER(XAttribute__dtor);
  Clean();
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::SetType(XFn::Type type)
{
  DUMPER(XAttribute__SetType);
  Clean();

  m_AttributeType = type;

  switch(m_AttributeType)
  {
  case XFn::BoolArrayAttribute:
    ab = new XBoolArray;
    break;

  case XFn::FloatArrayAttribute:
    af = new XRealArray;
    break;

  case XFn::Float2ArrayAttribute:
    af2 = new XVector2Array;
    break;

  case XFn::Float3ArrayAttribute:
    af3 = new XVector3Array;
    break;

  case XFn::Float4ArrayAttribute:
    af4 = new XVector4Array;
    break;

  case XFn::IntArrayAttribute:
    ai = new XS32Array;
    break;

  case XFn::Int2ArrayAttribute:
    ai2 = new XInt2Array;
    break;

  case XFn::Int3ArrayAttribute:
    ai3 = new XInt3Array;
    break;

  case XFn::Int4ArrayAttribute:
    ai4 = new XInt4Array;
    break;

  case XFn::ColourArrayAttribute:
    acol = new XColourArray;
    break;

  case XFn::StringArrayAttribute:
    as = new XStringList;
    break;

  case XFn::MatrixArrayAttribute:
    am = new XMatrixArray;
    break;

  case XFn::StringAttribute:
    s = new XString;
    break;

  case XFn::ColourAttribute:
    col = new XColour;
    break;

  case XFn::MatrixAttribute:
    m = new XMatrix;
    break;

  case XFn::Float2Attribute:
    f2 = new XVector2;
    break;

  case XFn::Float3Attribute:
    f3 = new XVector3;
    break;

  case XFn::Float4Attribute:
    f4 = new XVector4;
    break;

  case XFn::Int2Attribute:
    i2 = new XInt2;
    break;

  case XFn::Int3Attribute:
    i3 = new XInt3;
    break;

  case XFn::Int4Attribute:
    i4 = new XInt4;
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAttribute::NumElements() const
{
  switch(m_AttributeType)
  {
  case XFn::BoolArrayAttribute:
    return static_cast<XU32>(ab->size());

  case XFn::FloatArrayAttribute:
    return static_cast<XU32>(af->size());

  case XFn::Float2ArrayAttribute:
    return static_cast<XU32>(af2->size());

  case XFn::Float3ArrayAttribute:
    return static_cast<XU32>(af3->size());

  case XFn::Float4ArrayAttribute:
    return static_cast<XU32>(af4->size());

  case XFn::IntArrayAttribute:
    return static_cast<XU32>(ai->size());

  case XFn::Int2ArrayAttribute:
    return static_cast<XU32>(ai2->size());

  case XFn::Int3ArrayAttribute:
    return static_cast<XU32>(ai3->size());

  case XFn::Int4ArrayAttribute:
    return static_cast<XU32>(ai4->size());

  case XFn::ColourArrayAttribute:
    return static_cast<XU32>(acol->size());

  case XFn::StringArrayAttribute:
    return static_cast<XU32>(as->size());

  case XFn::MatrixArrayAttribute:
    return static_cast<XU32>(am->size());

  case XFn::MessageArrayAttribute:
    return static_cast<XU32>(i);

  default:
    break;
  }
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Resize(XU32 sz)
{
  DUMPER(XAttribute__Resize);
  switch(m_AttributeType)
  {
  case XFn::BoolArrayAttribute:
    ab->resize(sz);
    break;

  case XFn::FloatArrayAttribute:
    af->resize(sz);
    break;

  case XFn::Float2ArrayAttribute:
    af2->resize(sz);
    break;

  case XFn::Float3ArrayAttribute:
    af3->resize(sz);
    break;

  case XFn::Float4ArrayAttribute:
    af4->resize(sz);
    break;

  case XFn::IntArrayAttribute:
    ai->resize(sz);
    break;

  case XFn::Int2ArrayAttribute:
    ai2->resize(sz);
    break;

  case XFn::Int3ArrayAttribute:
    ai3->resize(sz);
    break;

  case XFn::Int4ArrayAttribute:
    ai4->resize(sz);
    break;

  case XFn::ColourArrayAttribute:
    acol->resize(sz);
    break;

  case XFn::StringArrayAttribute:
    as->resize(sz);
    break;

  case XFn::MatrixArrayAttribute:
    am->resize(sz);
    break;

  case XFn::MessageArrayAttribute:
    i = sz;
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(bool& val,XU32 idx) const
{

  switch(m_AttributeType)
  {
  case XFn::BoolAttribute:
    val = b;
    break;

  case XFn::BoolArrayAttribute:
    if(ab->size() > idx)
      val = (*ab)[idx];
    break;

  case XFn::ColourAttribute:
    val = false;
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      val = (*acol).size() ? true : false;
    }
    break;

  case XFn::IntAttribute:
    val = i ? true : false;
    break;

  case XFn::Int2Attribute:
    val = (i2->x || i2->y) ? true : false;
    break;

  case XFn::Int3Attribute:
    val = (i3->x || i3->y || i3->z) ? true : false;
    break;

  case XFn::Int4Attribute:
    val = (i4->x || i4->y || i4->z || i4->w) ? true : false;
    break;

  case XFn::IntArrayAttribute:
    val = (*ai).size() ? true : false;
    break;

  case XFn::Int2ArrayAttribute:
    val = (*ai2).size() ? true : false;
    break;

  case XFn::Int3ArrayAttribute:
    val = (*ai3).size() ? true : false;
    break;

  case XFn::Int4ArrayAttribute:
    val = (*ai4).size() ? true : false;
    break;

  case XFn::FloatAttribute:
    val = (f>-0.0001f && f<0.0001f) ?  false : true;
    break;

  case XFn::Float2Attribute:
    val = ((f2->x>-0.0001f && f2->x<0.0001f) &&
           (f2->y>-0.0001f && f2->y<0.0001f)) ?  false : true;
    break;

  case XFn::Float3Attribute:
    val = ((f3->x>-0.0001f && f3->x<0.0001f) &&
           (f3->y>-0.0001f && f3->y<0.0001f) &&
           (f3->z>-0.0001f && f3->z<0.0001f)) ?  false : true;
    break;

  case XFn::Float4Attribute:
    val = ((f4->x>-0.0001f && f4->x<0.0001f) &&
           (f4->y>-0.0001f && f4->y<0.0001f) &&
           (f4->z>-0.0001f && f4->z<0.0001f) &&
           (f4->w>-0.0001f && f4->w<0.0001f)) ?  false : true;
    break;

  case XFn::FloatArrayAttribute:
    val = (*af).size() ? true : false;
    break;

  case XFn::Float2ArrayAttribute:
    val = (*af2).size() ? true : false;
    break;

  case XFn::Float3ArrayAttribute:
    val = (*af3).size() ? true : false;
    break;

  case XFn::Float4ArrayAttribute:
    val = (*af4).size() ? true : false;
    break;

  default:
    val=false;
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XS32& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    val = static_cast<XS32>( col->r );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      val = static_cast<XS32>( (*acol)[idx].r );
    }
    break;

  case XFn::IntAttribute:
    val = static_cast<XS32>( i );
    break;

  case XFn::Int2Attribute:
    val = static_cast<XS32>( i2->x );
    break;

  case XFn::Int3Attribute:
    val = static_cast<XS32>( i3->x );
    break;

  case XFn::Int4Attribute:
    val = static_cast<XS32>( i4->x );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      val = static_cast<XS32>( (*ai)[idx] );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      val = static_cast<XS32>( (*ai2)[idx].x );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      val = static_cast<XS32>( (*ai3)[idx].x );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      val = static_cast<XS32>( (*ai4)[idx].x );
    }
    break;

  case XFn::FloatAttribute:
    val = static_cast<XS32>( f );
    break;

  case XFn::Float2Attribute:
    val = static_cast<XS32>( f2->x );
    break;

  case XFn::Float3Attribute:
    val = static_cast<XS32>( f3->x );
    break;

  case XFn::Float4Attribute:
    val = static_cast<XS32>( f4->x );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af2->size())
    {
      val = static_cast<XS32>( (*af)[idx] );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      val = static_cast<XS32>( (*af2)[idx].x );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      val = static_cast<XS32>( (*af3)[idx].x );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      val = static_cast<XS32>( (*af4)[idx].x );
    }
    break;

  default:
    val=0;
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XInt2& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    val.x = static_cast<XS32>( col->r );
    val.y = static_cast<XS32>( col->g );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      val.x = static_cast<XS32>( (*acol)[idx].r );
      val.y = static_cast<XS32>( (*acol)[idx].g );
    }
    break;

  case XFn::IntAttribute:
    val.x = static_cast<XS32>( i );
    val.y = 0;
    break;

  case XFn::Int2Attribute:
    val.x = static_cast<XS32>( i2->x );
    val.y = static_cast<XS32>( i2->y );
    break;

  case XFn::Int3Attribute:
    val.x = static_cast<XS32>( i3->x );
    val.y = static_cast<XS32>( i3->y );
    break;

  case XFn::Int4Attribute:
    val.x = static_cast<XS32>( i4->x );
    val.y = static_cast<XS32>( i4->y );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      val.x = static_cast<XS32>( (*ai)[idx] );
      val.y=0;
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      val.x = static_cast<XS32>( (*ai2)[idx].x );
      val.y = static_cast<XS32>( (*ai2)[idx].y );
    }
    break;
  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      val.x = static_cast<XS32>( (*ai3)[idx].x );
      val.y = static_cast<XS32>( (*ai3)[idx].y );
    }
    break;
  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      val.x = static_cast<XS32>( (*ai4)[idx].x );
      val.y = static_cast<XS32>( (*ai4)[idx].y );
    }
    break;

  case XFn::FloatAttribute:
    val.x = static_cast<XS32>( f );
    val.y = 0;
    break;

  case XFn::Float2Attribute:
    val.x = static_cast<XS32>( f2->x );
    val.y = static_cast<XS32>( f2->y );
    break;

  case XFn::Float3Attribute:
    val.x = static_cast<XS32>( f3->x );
    val.y = static_cast<XS32>( f3->y );
    break;

  case XFn::Float4Attribute:
    val.x = static_cast<XS32>( f4->x );
    val.y = static_cast<XS32>( f4->y );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      val.x = static_cast<XS32>( (*af)[idx] );
      val.y=0;
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      val.x = static_cast<XS32>( (*af2)[idx].x );
      val.y = static_cast<XS32>( (*af2)[idx].y );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      val.x = static_cast<XS32>( (*af3)[idx].x );
      val.y = static_cast<XS32>( (*af3)[idx].y );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      val.x = static_cast<XS32>( (*af4)[idx].x );
      val.y = static_cast<XS32>( (*af4)[idx].y );
    }
    break;

  default:
    val.Set(0,0);
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XInt3& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    val.x = static_cast<XS32>( col->r );
    val.y = static_cast<XS32>( col->g );
    val.z = static_cast<XS32>( col->b );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      val.x = static_cast<XS32>( (*acol)[idx].r );
      val.y = static_cast<XS32>( (*acol)[idx].g );
      val.z = static_cast<XS32>( (*acol)[idx].b );
    }
    break;

  case XFn::IntAttribute:
    val.x = static_cast<XS32>( i );
    val.y = 0;
    val.z = 0;
    break;

  case XFn::Int2Attribute:
    val.x = static_cast<XS32>( i2->x );
    val.y = static_cast<XS32>( i2->y );
    val.z = 0;
    break;

  case XFn::Int3Attribute:
    val.x = static_cast<XS32>( i3->x );
    val.y = static_cast<XS32>( i3->y );
    val.z = static_cast<XS32>( i3->z );
    break;

  case XFn::Int4Attribute:
    val.x = static_cast<XS32>( i4->x );
    val.y = static_cast<XS32>( i4->y );
    val.z = static_cast<XS32>( i4->z );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      val.x = static_cast<XS32>( (*ai)[idx] );
      val.y=0;
      val.z = 0;
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      val.x = static_cast<XS32>( (*ai2)[idx].x );
      val.y = static_cast<XS32>( (*ai2)[idx].y );
      val.z = 0;
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      val.x = static_cast<XS32>( (*ai3)[idx].x );
      val.y = static_cast<XS32>( (*ai3)[idx].y );
      val.z = static_cast<XS32>( (*ai3)[idx].z );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      val.x = static_cast<XS32>( (*ai4)[idx].x );
      val.y = static_cast<XS32>( (*ai4)[idx].y );
      val.z = static_cast<XS32>( (*ai4)[idx].z );
    }
    break;

  case XFn::FloatAttribute:
    val.x = static_cast<XS32>( f );
    val.y = 0;
    val.z = 0;
    break;

  case XFn::Float2Attribute:
    val.x = static_cast<XS32>( f2->x );
    val.y = static_cast<XS32>( f2->y );
    val.z = 0;
    break;

  case XFn::Float3Attribute:
    val.x = static_cast<XS32>( f3->x );
    val.y = static_cast<XS32>( f3->y );
    val.z = static_cast<XS32>( f3->z );
    break;

  case XFn::Float4Attribute:
    val.x = static_cast<XS32>( f4->x );
    val.y = static_cast<XS32>( f4->y );
    val.z = static_cast<XS32>( f4->z );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      val.x = static_cast<XS32>( (*af)[idx] );
      val.y=0;
      val.z = 0;
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      val.x = static_cast<XS32>( (*af2)[idx].x );
      val.y = static_cast<XS32>( (*af2)[idx].y );
      val.z = 0;
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      val.x = static_cast<XS32>( (*af3)[idx].x );
      val.y = static_cast<XS32>( (*af3)[idx].y );
      val.z = static_cast<XS32>( (*af3)[idx].z );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      val.x = static_cast<XS32>( (*af4)[idx].x );
      val.y = static_cast<XS32>( (*af4)[idx].y );
      val.z = static_cast<XS32>( (*af4)[idx].z );
    }
    break;

  default:
    val.Set(0,0,0);
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XInt4& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    val.x = static_cast<XS32>( col->r );
    val.y = static_cast<XS32>( col->g );
    val.z = static_cast<XS32>( col->b );
    val.w = static_cast<XS32>( col->a );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      val.x = static_cast<XS32>( (*acol)[idx].r );
      val.y = static_cast<XS32>( (*acol)[idx].g );
      val.z = static_cast<XS32>( (*acol)[idx].b );
      val.w = static_cast<XS32>( (*acol)[idx].a );
    }
    break;

  case XFn::IntAttribute:
    val.x = static_cast<XS32>( i );
    val.y = 0;
    val.z = 0;
    val.w = 0;
    break;

  case XFn::Int2Attribute:
    val.x = static_cast<XS32>( i2->x );
    val.y = static_cast<XS32>( i2->y );
    val.z = 0;
    val.w = 0;
    break;

  case XFn::Int3Attribute:
    val.x = static_cast<XS32>( i3->x );
    val.y = static_cast<XS32>( i3->y );
    val.z = static_cast<XS32>( i3->z );
    val.w = 0;
    break;

  case XFn::Int4Attribute:
    val.x = static_cast<XS32>( i4->x );
    val.y = static_cast<XS32>( i4->y );
    val.z = static_cast<XS32>( i4->z );
    val.w = static_cast<XS32>( i4->w );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      val.x = static_cast<XS32>( (*ai)[idx] );
      val.y=0;
      val.z = 0;
      val.w = 0;
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      val.x = static_cast<XS32>( (*ai2)[idx].x );
      val.y = static_cast<XS32>( (*ai2)[idx].y );
      val.z = 0;
      val.w = 0;
    }
    break;
  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      val.x = static_cast<XS32>( (*ai3)[idx].x );
      val.y = static_cast<XS32>( (*ai3)[idx].y );
      val.z = static_cast<XS32>( (*ai3)[idx].z );
      val.w = 0;
    }
    break;
  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      val.x = static_cast<XS32>( (*ai4)[idx].x );
      val.y = static_cast<XS32>( (*ai4)[idx].y );
      val.z = static_cast<XS32>( (*ai4)[idx].z );
      val.w = static_cast<XS32>( (*ai4)[idx].w );
    }
    break;

  case XFn::FloatAttribute:
    val.x = static_cast<XS32>( f );
    val.y = 0;
    val.z = 0;
    val.w = 0;
    break;

  case XFn::Float2Attribute:
    val.x = static_cast<XS32>( f2->x );
    val.y = static_cast<XS32>( f2->y );
    val.z = 0;
    val.w = 0;
    break;

  case XFn::Float3Attribute:
    val.x = static_cast<XS32>( f3->x );
    val.y = static_cast<XS32>( f3->y );
    val.z = static_cast<XS32>( f3->z );
    val.w = 0;
    break;

  case XFn::Float4Attribute:
    val.x = static_cast<XS32>( f4->x );
    val.y = static_cast<XS32>( f4->y );
    val.z = static_cast<XS32>( f4->z );
    val.w = static_cast<XS32>( f4->w );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      val.x = static_cast<XS32>( (*af)[idx] );
      val.y = 0;
      val.z = 0;
      val.w = 0;
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      val.x = static_cast<XS32>( (*af2)[idx].x );
      val.y = static_cast<XS32>( (*af2)[idx].y );
      val.z = 0;
      val.w = 0;
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      val.x = static_cast<XS32>( (*af3)[idx].x );
      val.y = static_cast<XS32>( (*af3)[idx].y );
      val.z = static_cast<XS32>( (*af3)[idx].z );
      val.w = 0;
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      val.x = static_cast<XS32>( (*af4)[idx].x );
      val.y = static_cast<XS32>( (*af4)[idx].y );
      val.z = static_cast<XS32>( (*af4)[idx].z );
      val.w = static_cast<XS32>( (*af4)[idx].w );
    }
    break;

  default:
    val.Set(0,0,0,0);
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XReal& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    val = col->r;
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      val = (*acol)[idx].r;
    }
    break;

  case XFn::IntAttribute:
    val = static_cast<XReal>( i );
    break;

  case XFn::Int2Attribute:
    val = static_cast<XReal>( i2->x );
    break;

  case XFn::Int3Attribute:
    val = static_cast<XReal>( i3->x );
    break;

  case XFn::Int4Attribute:
    val = static_cast<XReal>( i4->x );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      val = static_cast<XReal>( (*ai)[idx] );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      val = static_cast<XReal>( (*ai2)[idx].x );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      val = static_cast<XReal>( (*ai3)[idx].x );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      val = static_cast<XReal>( (*ai4)[idx].x );
    }
    break;

  case XFn::FloatAttribute:
    val = f;
    break;

  case XFn::Float2Attribute:
    val = f2->x;
    break;

  case XFn::Float3Attribute:
    val = f3->x;
    break;

  case XFn::Float4Attribute:
    val = f4->x;
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      val = (*af)[idx];
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      val = (*af2)[idx].x;
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      val = (*af3)[idx].x;
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      val = (*af4)[idx].x;
    }
    break;

  default:
    val=0.0f;
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XVector2& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    val.x = col->r;
    val.y = col->g;
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      val.x = (*acol)[idx].r;
      val.y = (*acol)[idx].g;
    }
    break;

  case XFn::IntAttribute:
    val.x = static_cast<XReal>( i );
    val.y = 0;
    break;

  case XFn::Int2Attribute:
    val.x = static_cast<XReal>( i2->x );
    val.y = static_cast<XReal>( i2->y );
    break;

  case XFn::Int3Attribute:
    val.x = static_cast<XReal>( i3->x );
    val.y = static_cast<XReal>( i3->y );
    break;

  case XFn::Int4Attribute:
    val.x = static_cast<XReal>( i4->x );
    val.y = static_cast<XReal>( i4->y );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      val.x = static_cast<XReal>( (*ai)[idx] );
      val.y=0;
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      val.x = static_cast<XReal>( (*ai2)[idx].x );
      val.y = static_cast<XReal>( (*ai2)[idx].y );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      val.x = static_cast<XReal>( (*ai3)[idx].x );
      val.y = static_cast<XReal>( (*ai3)[idx].y );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      val.x = static_cast<XReal>( (*ai4)[idx].x );
      val.y = static_cast<XReal>( (*ai4)[idx].y );
    }
    break;

  case XFn::FloatAttribute:
    val.x = f;
    val.y = 0;
    break;

  case XFn::Float2Attribute:
    val.x = f2->x;
    val.y = f2->y;
    break;

  case XFn::Float3Attribute:
    val.x = f3->x;
    val.y = f3->y;
    break;

  case XFn::Float4Attribute:
    val.x = f4->x;
    val.y = f4->y;
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      val.x = (*af)[idx];
      val.y=0;
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      val.x = (*af2)[idx].x;
      val.y = (*af2)[idx].y;
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      val.x = (*af3)[idx].x;
      val.y = (*af3)[idx].y;
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      val.x = (*af4)[idx].x;
      val.y = (*af4)[idx].y;
    }
    break;

  default:
    val.set(0.0f,0.0f);
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XVector3& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    val.x = col->r;
    val.y = col->g;
    val.z = col->b;
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      val.x = (*acol)[idx].r;
      val.y = (*acol)[idx].g;
      val.z = (*acol)[idx].b;
    }
    break;

  case XFn::IntAttribute:
    val.x = static_cast<XReal>( i );
    val.y = 0;
    val.z = 0;
    break;

  case XFn::Int2Attribute:
    val.x = static_cast<XReal>( i2->x );
    val.y = static_cast<XReal>( i2->y );
    val.z = 0;
    break;

  case XFn::Int3Attribute:
    val.x = static_cast<XReal>( i3->x );
    val.y = static_cast<XReal>( i3->y );
    val.z = static_cast<XReal>( i3->z );
    break;

  case XFn::Int4Attribute:
    val.x = static_cast<XReal>( i4->x );
    val.y = static_cast<XReal>( i4->y );
    val.z = static_cast<XReal>( i4->z );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      val.x = static_cast<XReal>( (*ai)[idx] );
      val.y=0;
      val.z = 0;
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      val.x = static_cast<XReal>( (*ai2)[idx].x );
      val.y = static_cast<XReal>( (*ai2)[idx].y );
      val.z = 0;
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      val.x = static_cast<XReal>( (*ai3)[idx].x );
      val.y = static_cast<XReal>( (*ai3)[idx].y );
      val.z = static_cast<XReal>( (*ai3)[idx].z );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      val.x = static_cast<XReal>( (*ai4)[idx].x );
      val.y = static_cast<XReal>( (*ai4)[idx].y );
      val.z = static_cast<XReal>( (*ai4)[idx].z );
    }
    break;

  case XFn::FloatAttribute:
    val.x = f;
    val.y = 0;
    val.z = 0;
    break;

  case XFn::Float2Attribute:
    val.x = f2->x;
    val.y = f2->y;
    val.z = 0;
    break;

  case XFn::Float3Attribute:
    val.x = f3->x;
    val.y = f3->y;
    val.z = f3->z;
    break;

  case XFn::Float4Attribute:
    val.x = f4->x;
    val.y = f4->y;
    val.z = f4->z;
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      val.x = (*af)[idx];
      val.y=0;
      val.z = 0;
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      val.x = (*af2)[idx].x;
      val.y = (*af2)[idx].y;
      val.z = 0;
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      val.x = (*af3)[idx].x;
      val.y = (*af3)[idx].y;
      val.z = (*af3)[idx].z;
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      val.x = (*af4)[idx].x;
      val.y = (*af4)[idx].y;
      val.z = (*af4)[idx].z;
    }
    break;

  default:
    val.set(0.0f,0.0f,0.0f);
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XVector4& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    val.x = col->r;
    val.y = col->g;
    val.z = col->b;
    val.w = col->a;
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      val.x = (*acol)[idx].r;
      val.y = (*acol)[idx].g;
      val.z = (*acol)[idx].b;
      val.w = (*acol)[idx].a;
    }
    break;

  case XFn::IntAttribute:
    val.x = static_cast<XReal>( i );
    val.y = 0;
    val.z = 0;
    val.w = 0;
    break;

  case XFn::Int2Attribute:
    val.x = static_cast<XReal>( i2->x );
    val.y = static_cast<XReal>( i2->y );
    val.z = 0;
    val.w = 0;
    break;

  case XFn::Int3Attribute:
    val.x = static_cast<XReal>( i3->x );
    val.y = static_cast<XReal>( i3->y );
    val.z = static_cast<XReal>( i3->z );
    val.w = 0;
    break;

  case XFn::Int4Attribute:
    val.x = static_cast<XReal>( i4->x );
    val.y = static_cast<XReal>( i4->y );
    val.z = static_cast<XReal>( i4->z );
    val.w = static_cast<XReal>( i4->w );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      val.x = static_cast<XReal>( (*ai)[idx] );
      val.y=0;
      val.z = 0;
      val.w = 0;
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      val.x = static_cast<XReal>( (*ai2)[idx].x );
      val.y = static_cast<XReal>( (*ai2)[idx].y );
      val.z = 0;
      val.w = 0;
    }
    break;
  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      val.x = static_cast<XReal>( (*ai3)[idx].x );
      val.y = static_cast<XReal>( (*ai3)[idx].y );
      val.z = static_cast<XReal>( (*ai3)[idx].z );
      val.w = 0;
    }
    break;
  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      val.x = static_cast<XReal>( (*ai4)[idx].x );
      val.y = static_cast<XReal>( (*ai4)[idx].y );
      val.z = static_cast<XReal>( (*ai4)[idx].z );
      val.w = static_cast<XReal>( (*ai4)[idx].w );
    }
    break;

  case XFn::FloatAttribute:
    val.x = f;
    val.y = 0;
    val.z = 0;
    val.w = 0;
    break;

  case XFn::Float2Attribute:
    val.x = f2->x;
    val.y = f2->y;
    val.z = 0;
    val.w = 0;
    break;

  case XFn::Float3Attribute:
    val.x = f3->x;
    val.y = f3->y;
    val.z = f3->z;
    val.w = 0;
    break;

  case XFn::Float4Attribute:
    val.x = f4->x;
    val.y = f4->y;
    val.z = f4->z;
    val.w = f4->w;
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      val.x = (*af)[idx];
      val.y = 0;
      val.z = 0;
      val.w = 0;
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      val.x = (*af2)[idx].x;
      val.y = (*af2)[idx].y;
      val.z = 0;
      val.w = 0;
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      val.x = (*af3)[idx].x;
      val.y = (*af3)[idx].y;
      val.z = (*af3)[idx].z;
      val.w = 0;
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      val.x = (*af4)[idx].x;
      val.y = (*af4)[idx].y;
      val.z = (*af4)[idx].z;
      val.w = (*af4)[idx].w;
    }
    break;

  default:
    val.set(0.0f,0.0f,0.0f,0.0f);
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XString& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::StringAttribute:
    val = *s;
    break;

  case XFn::StringArrayAttribute:
    if (idx < as->size())
    {
      val = (*as)[idx];
    }
    break;

  default:
    val="";
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XColour& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    val = *col;
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      val = (*acol)[idx];
    }
    break;

  case XFn::FloatAttribute:
    val.r=val.g=val.b=val.a= f;
    break;

  case XFn::Float2Attribute:
    val.r = f2->x;
    val.g = f2->y;
    val.b = 0;
    val.a = 0;;
    break;

  case XFn::Float3Attribute:
    val.r = f3->x;
    val.g = f3->y;
    val.b = f3->z;
    val.a = 0;
    break;

  case XFn::Float4Attribute:
    val.r = f4->x;
    val.g = f4->y;
    val.b = f4->z;
    val.a = f4->w;
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      val.r=val.g=val.b=val.a= (*af)[idx];
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      val.r = (*af2)[idx].x;
      val.g = (*af2)[idx].y;
      val.b = 0;
      val.a = 0;
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      val.r = (*af3)[idx].x;
      val.g = (*af3)[idx].y;
      val.b = (*af3)[idx].z;
      val.a = 0;
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      val.r = (*af4)[idx].x;
      val.g = (*af4)[idx].y;
      val.b = (*af4)[idx].z;
      val.a = (*af4)[idx].w;
    }
    break;

  case XFn::IntAttribute:
    val.r=val.g=val.b=val.a= static_cast<XReal>( i );
    break;

  case XFn::Int2Attribute:
    val.r = static_cast<XReal>( i2->x );
    val.g = static_cast<XReal>( i2->y );
    val.b = 0;
    val.a = 0;;
    break;

  case XFn::Int3Attribute:
    val.r = static_cast<XReal>( i3->x );
    val.g = static_cast<XReal>( i3->y );
    val.b = static_cast<XReal>( i3->z );
    val.a = 0;
    break;

  case XFn::Int4Attribute:
    val.r = static_cast<XReal>( i4->x );
    val.g = static_cast<XReal>( i4->y );
    val.b = static_cast<XReal>( i4->z );
    val.a = static_cast<XReal>( i4->w );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      val.r=val.g=val.b=val.a= static_cast<XReal>( (*ai)[idx] );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      val.r = static_cast<XReal>( (*ai2)[idx].x );
      val.g = static_cast<XReal>( (*ai2)[idx].y );
      val.b = 0;
      val.a = 0;
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      val.r = static_cast<XReal>( (*ai3)[idx].x );
      val.g = static_cast<XReal>( (*ai3)[idx].y );
      val.b = static_cast<XReal>( (*ai3)[idx].z );
      val.a = 0;
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      val.r = static_cast<XReal>( (*ai4)[idx].x );
      val.g = static_cast<XReal>( (*ai4)[idx].y );
      val.b = static_cast<XReal>( (*ai4)[idx].z );
      val.a = static_cast<XReal>( (*ai4)[idx].w );
    }
    break;

  default:
    val.r=val.g=val.b=val.a=0.0f;
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Get(XMatrix& val,XU32 idx) const
{
  switch(m_AttributeType)
  {
  case XFn::MatrixAttribute:
    val = *m;
    break;

  case XFn::MatrixArrayAttribute:
    if (idx < am->size())
    {
      val = (*am)[idx];
    }
    break;
  default:
    val.identity();
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const bool& val,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    break;

  case XFn::ColourArrayAttribute:
    break;

  case XFn::BoolAttribute:
    b = static_cast<bool>( val );
    break;

  case XFn::BoolArrayAttribute:
    if (idx < ab->size())
    {
      (*ab)[idx] = static_cast<bool>( val );
    }
    break;

  case XFn::IntAttribute:
    i = static_cast<XS32>( val );
    break;

  case XFn::Int2Attribute:
    i2->x = static_cast<XS32>( val );
    break;

  case XFn::Int3Attribute:
    i3->x = static_cast<XS32>( val );
    break;

  case XFn::Int4Attribute:
    i4->x = static_cast<XS32>( val );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      (*ai)[idx] = static_cast<XS32>( val );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      (*ai2)[idx].x = static_cast<XS32>( val );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      (*ai3)[idx].x = static_cast<XS32>( val );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      (*ai4)[idx].x = static_cast<XS32>( val );
    }
    break;

  case XFn::FloatAttribute:
    f = static_cast<XReal>( val );
    break;

  case XFn::Float2Attribute:
    f2->x = static_cast<XReal>( val );
    break;

  case XFn::Float3Attribute:
    f3->x = static_cast<XReal>( val );
    break;

  case XFn::Float4Attribute:
    f4->x = static_cast<XReal>( val );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      (*af)[idx] = static_cast<XReal>( val );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      (*af2)[idx].x = static_cast<XReal>( val );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      (*af3)[idx].x = static_cast<XReal>( val );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      (*af4)[idx].x = static_cast<XReal>( val );
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XS32& val,XU32 idx)
{
  switch(m_AttributeType)
  {

  case XFn::ColourAttribute:
    col->r = static_cast<XReal>( val );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      (*acol)[idx].r = static_cast<XReal>( val );
    }
    break;

  case XFn::IntAttribute:
    i = static_cast<XS32>( val );
    break;

  case XFn::Int2Attribute:
    i2->x = static_cast<XS32>( val );
    break;

  case XFn::Int3Attribute:
    i3->x = static_cast<XS32>( val );
    break;

  case XFn::Int4Attribute:
    i4->x = static_cast<XS32>( val );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      (*ai)[idx] = static_cast<XS32>( val );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      (*ai2)[idx].x = static_cast<XS32>( val );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      (*ai3)[idx].x = static_cast<XS32>( val );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      (*ai4)[idx].x = static_cast<XS32>( val );
    }
    break;

  case XFn::FloatAttribute:
    f = static_cast<XReal>( val );
    break;

  case XFn::Float2Attribute:
    f2->x = static_cast<XReal>( val );
    break;

  case XFn::Float3Attribute:
    f3->x = static_cast<XReal>( val );
    break;

  case XFn::Float4Attribute:
    f4->x = static_cast<XReal>( val );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      (*af)[idx] = static_cast<XReal>( val );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      (*af2)[idx].x = static_cast<XReal>( val );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      (*af3)[idx].x = static_cast<XReal>( val );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      (*af4)[idx].x = static_cast<XReal>( val );
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XInt2& val,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    col->r = static_cast<XReal>( val.x );
    col->g = static_cast<XReal>( val.y );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      (*acol)[idx].r = static_cast<XReal>( val.x );
      (*acol)[idx].g = static_cast<XReal>( val.y );
    }
    break;

  case XFn::IntAttribute:
    i = static_cast<XS32>( val.x );
    break;

  case XFn::Int2Attribute:
    i2->x = static_cast<XS32>( val.x );
    i2->y = static_cast<XS32>( val.y );
    break;

  case XFn::Int3Attribute:
    i3->x = static_cast<XS32>( val.x );
    i3->y = static_cast<XS32>( val.y );
    break;

  case XFn::Int4Attribute:
    i4->x = static_cast<XS32>( val.x );
    i4->y = static_cast<XS32>( val.y );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      (*ai)[idx] = static_cast<XS32>( val.x );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      (*ai2)[idx].x = static_cast<XS32>( val.x );
      (*ai2)[idx].y = static_cast<XS32>( val.y );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      (*ai3)[idx].x = static_cast<XS32>( val.x );
      (*ai3)[idx].y = static_cast<XS32>( val.y );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      (*ai4)[idx].x = static_cast<XS32>( val.x );
      (*ai4)[idx].y = static_cast<XS32>( val.y );
    }
    break;

  case XFn::FloatAttribute:
    f = static_cast<XReal>( val.x );
    break;

  case XFn::Float2Attribute:
    f2->x = static_cast<XReal>( val.x );
    f2->y = static_cast<XReal>( val.y );
    break;

  case XFn::Float3Attribute:
    f3->x = static_cast<XReal>( val.x );
    f3->y = static_cast<XReal>( val.y );
    break;

  case XFn::Float4Attribute:
    f4->x = static_cast<XReal>( val.x );
    f4->y = static_cast<XReal>( val.y );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      (*af)[idx] = static_cast<XReal>( val.x );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      (*af2)[idx].x = static_cast<XReal>( val.x );
      (*af2)[idx].y = static_cast<XReal>( val.y );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      (*af3)[idx].x = static_cast<XReal>( val.x );
      (*af3)[idx].y = static_cast<XReal>( val.y );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      (*af4)[idx].x = static_cast<XReal>( val.x );
      (*af4)[idx].y = static_cast<XReal>( val.y );
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XInt3& val,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    col->r = static_cast<XReal>( val.x );
    col->g = static_cast<XReal>( val.y );
    col->b = static_cast<XReal>( val.z );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      (*acol)[idx].r = static_cast<XReal>( val.x );
      (*acol)[idx].g = static_cast<XReal>( val.y );
      (*acol)[idx].b = static_cast<XReal>( val.z );
    }
    break;

  case XFn::IntAttribute:
    i = static_cast<XS32>( val.x );
    break;

  case XFn::Int2Attribute:
    i2->x = static_cast<XS32>( val.x );
    i2->y = static_cast<XS32>( val.y );
    break;

  case XFn::Int3Attribute:
    i3->x = static_cast<XS32>( val.x );
    i3->y = static_cast<XS32>( val.y );
    i3->z = static_cast<XS32>( val.z );
    break;

  case XFn::Int4Attribute:
    i4->x = static_cast<XS32>( val.x );
    i4->y = static_cast<XS32>( val.y );
    i4->z = static_cast<XS32>( val.z );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      (*ai)[idx] = static_cast<XS32>( val.x );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      (*ai2)[idx].x = static_cast<XS32>( val.x );
      (*ai2)[idx].y = static_cast<XS32>( val.y );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      (*ai3)[idx].x = static_cast<XS32>( val.x );
      (*ai3)[idx].y = static_cast<XS32>( val.y );
      (*ai3)[idx].z = static_cast<XS32>( val.z );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      (*ai4)[idx].x = static_cast<XS32>( val.x );
      (*ai4)[idx].y = static_cast<XS32>( val.y );
      (*ai4)[idx].z = static_cast<XS32>( val.z );
    }
    break;

  case XFn::FloatAttribute:
    f = static_cast<XReal>( val.x );
    break;

  case XFn::Float2Attribute:
    f2->x = static_cast<XReal>( val.x );
    f2->y = static_cast<XReal>( val.y );
    break;

  case XFn::Float3Attribute:
    f3->x = static_cast<XReal>( val.x );
    f3->y = static_cast<XReal>( val.y );
    f3->z = static_cast<XReal>( val.z );
    break;

  case XFn::Float4Attribute:
    f4->x = static_cast<XReal>( val.x );
    f4->y = static_cast<XReal>( val.y );
    f4->z = static_cast<XReal>( val.z );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      (*af)[idx] = static_cast<XReal>( val.x );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      (*af2)[idx].x = static_cast<XReal>( val.x );
      (*af2)[idx].y = static_cast<XReal>( val.y );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      (*af3)[idx].x = static_cast<XReal>( val.x );
      (*af3)[idx].y = static_cast<XReal>( val.y );
      (*af3)[idx].z = static_cast<XReal>( val.z );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      (*af4)[idx].x = static_cast<XReal>( val.x );
      (*af4)[idx].y = static_cast<XReal>( val.y );
      (*af4)[idx].z = static_cast<XReal>( val.z );
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XInt4& val,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    col->r = static_cast<XReal>( val.x );
    col->g = static_cast<XReal>( val.y );
    col->b = static_cast<XReal>( val.w );
    col->a = static_cast<XReal>( val.z );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      (*acol)[idx].r = static_cast<XReal>( val.x );
      (*acol)[idx].g = static_cast<XReal>( val.y );
      (*acol)[idx].b = static_cast<XReal>( val.z );
      (*acol)[idx].a = static_cast<XReal>( val.w );
    }
    break;

  case XFn::IntAttribute:
    i = static_cast<XS32>( val.x );
    break;

  case XFn::Int2Attribute:
    i2->x = static_cast<XS32>( val.x );
    i2->y = static_cast<XS32>( val.y );
    break;

  case XFn::Int3Attribute:
    i3->x = static_cast<XS32>( val.x );
    i3->y = static_cast<XS32>( val.y );
    i3->z = static_cast<XS32>( val.z );
    break;

  case XFn::Int4Attribute:
    i4->x = static_cast<XS32>( val.x );
    i4->y = static_cast<XS32>( val.y );
    i4->z = static_cast<XS32>( val.z );
    i4->w = static_cast<XS32>( val.w );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      (*ai)[idx] = static_cast<XS32>( val.x );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      (*ai2)[idx].x = static_cast<XS32>( val.x );
      (*ai2)[idx].y = static_cast<XS32>( val.y );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      (*ai3)[idx].x = static_cast<XS32>( val.x );
      (*ai3)[idx].y = static_cast<XS32>( val.y );
      (*ai3)[idx].z = static_cast<XS32>( val.z );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      (*ai4)[idx].x = static_cast<XS32>( val.x );
      (*ai4)[idx].y = static_cast<XS32>( val.y );
      (*ai4)[idx].z = static_cast<XS32>( val.z );
      (*ai4)[idx].w = static_cast<XS32>( val.w );
    }
    break;

  case XFn::FloatAttribute:
    f = static_cast<XReal>( val.x );
    break;

  case XFn::Float2Attribute:
    f2->x = static_cast<XReal>( val.x );
    f2->y = static_cast<XReal>( val.y );
    break;

  case XFn::Float3Attribute:
    f3->x = static_cast<XReal>( val.x );
    f3->y = static_cast<XReal>( val.y );
    f3->z = static_cast<XReal>( val.z );
    break;

  case XFn::Float4Attribute:
    f4->x = static_cast<XReal>( val.x );
    f4->y = static_cast<XReal>( val.y );
    f4->z = static_cast<XReal>( val.z );
    f4->w = static_cast<XReal>( val.w );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      (*af)[idx] = static_cast<XReal>( val.x );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      (*af2)[idx].x = static_cast<XReal>( val.x );
      (*af2)[idx].y = static_cast<XReal>( val.y );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      (*af3)[idx].x = static_cast<XReal>( val.x );
      (*af3)[idx].y = static_cast<XReal>( val.y );
      (*af3)[idx].z = static_cast<XReal>( val.z );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      (*af4)[idx].x = static_cast<XReal>( val.x );
      (*af4)[idx].y = static_cast<XReal>( val.y );
      (*af4)[idx].z = static_cast<XReal>( val.z );
      (*af4)[idx].w = static_cast<XReal>( val.w );
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XReal val,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    col->r = static_cast<XReal>( val );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      (*acol)[idx].r = static_cast<XReal>( val );
    }
    break;

  case XFn::IntAttribute:
    i = static_cast<XS32>( val );
    break;

  case XFn::Int2Attribute:
    i2->x = static_cast<XS32>( val );
    break;

  case XFn::Int3Attribute:
    i3->x = static_cast<XS32>( val );
    break;

  case XFn::Int4Attribute:
    i4->x = static_cast<XS32>( val );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      (*ai)[idx] = static_cast<XS32>( val );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      (*ai2)[idx].x = static_cast<XS32>( val );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      (*ai3)[idx].x = static_cast<XS32>( val );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      (*ai4)[idx].x = static_cast<XS32>( val );
    }
    break;

  case XFn::FloatAttribute:
    f = static_cast<XReal>( val );
    break;

  case XFn::Float2Attribute:
    f2->x = static_cast<XReal>( val );
    break;

  case XFn::Float3Attribute:
    f3->x = static_cast<XReal>( val );
    break;

  case XFn::Float4Attribute:
    f4->x = static_cast<XReal>( val );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      (*af)[idx] = static_cast<XReal>( val );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      (*af2)[idx].x = static_cast<XReal>( val );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      (*af3)[idx].x = static_cast<XReal>( val );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      (*af4)[idx].x = static_cast<XReal>( val );
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XVector2& val,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    col->r = static_cast<XReal>( val.x );
    col->g = static_cast<XReal>( val.y );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      (*acol)[idx].r = static_cast<XReal>( val.x );
      (*acol)[idx].g = static_cast<XReal>( val.y );
    }
    break;

  case XFn::IntAttribute:
    i = static_cast<XS32>( val.x );
    break;

  case XFn::Int2Attribute:
    i2->x = static_cast<XS32>( val.x );
    i2->y = static_cast<XS32>( val.y );
    break;

  case XFn::Int3Attribute:
    i3->x = static_cast<XS32>( val.x );
    i3->y = static_cast<XS32>( val.y );
    break;

  case XFn::Int4Attribute:
    i4->x = static_cast<XS32>( val.x );
    i4->y = static_cast<XS32>( val.y );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      (*ai)[idx] = static_cast<XS32>( val.x );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      (*ai2)[idx].x = static_cast<XS32>( val.x );
      (*ai2)[idx].y = static_cast<XS32>( val.y );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      (*ai3)[idx].x = static_cast<XS32>( val.x );
      (*ai3)[idx].y = static_cast<XS32>( val.y );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      (*ai4)[idx].x = static_cast<XS32>( val.x );
      (*ai4)[idx].y = static_cast<XS32>( val.y );
    }
    break;

  case XFn::FloatAttribute:
    f = static_cast<XReal>( val.x );
    break;

  case XFn::Float2Attribute:
    f2->x = static_cast<XReal>( val.x );
    f2->y = static_cast<XReal>( val.y );
    break;

  case XFn::Float3Attribute:
    f3->x = static_cast<XReal>( val.x );
    f3->y = static_cast<XReal>( val.y );
    break;

  case XFn::Float4Attribute:
    f4->x = static_cast<XReal>( val.x );
    f4->y = static_cast<XReal>( val.y );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      (*af)[idx] = static_cast<XReal>( val.x );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      (*af2)[idx].x = static_cast<XReal>( val.x );
      (*af2)[idx].y = static_cast<XReal>( val.y );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      (*af3)[idx].x = static_cast<XReal>( val.x );
      (*af3)[idx].y = static_cast<XReal>( val.y );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      (*af4)[idx].x = static_cast<XReal>( val.x );
      (*af4)[idx].y = static_cast<XReal>( val.y );
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XVector3& val,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    col->r = static_cast<XReal>( val.x );
    col->g = static_cast<XReal>( val.y );
    col->b = static_cast<XReal>( val.z );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      (*acol)[idx].r = static_cast<XReal>( val.x );
      (*acol)[idx].g = static_cast<XReal>( val.y );
      (*acol)[idx].b = static_cast<XReal>( val.z );
    }
    break;

  case XFn::IntAttribute:
    i = static_cast<XS32>( val.x );
    break;

  case XFn::Int2Attribute:
    i2->x = static_cast<XS32>( val.x );
    i2->y = static_cast<XS32>( val.y );
    break;

  case XFn::Int3Attribute:
    i3->x = static_cast<XS32>( val.x );
    i3->y = static_cast<XS32>( val.y );
    i3->z = static_cast<XS32>( val.z );
    break;

  case XFn::Int4Attribute:
    i4->x = static_cast<XS32>( val.x );
    i4->y = static_cast<XS32>( val.y );
    i4->z = static_cast<XS32>( val.z );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      (*ai)[idx] = static_cast<XS32>( val.x );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      (*ai2)[idx].x = static_cast<XS32>( val.x );
      (*ai2)[idx].y = static_cast<XS32>( val.y );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      (*ai3)[idx].x = static_cast<XS32>( val.x );
      (*ai3)[idx].y = static_cast<XS32>( val.y );
      (*ai3)[idx].z = static_cast<XS32>( val.z );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      (*ai4)[idx].x = static_cast<XS32>( val.x );
      (*ai4)[idx].y = static_cast<XS32>( val.y );
      (*ai4)[idx].z = static_cast<XS32>( val.z );
    }
    break;

  case XFn::FloatAttribute:
    f = static_cast<XReal>( val.x );
    break;

  case XFn::Float2Attribute:
    f2->x = static_cast<XReal>( val.x );
    f2->y = static_cast<XReal>( val.y );
    break;

  case XFn::Float3Attribute:
    f3->x = static_cast<XReal>( val.x );
    f3->y = static_cast<XReal>( val.y );
    f3->z = static_cast<XReal>( val.z );
    break;

  case XFn::Float4Attribute:
    f4->x = static_cast<XReal>( val.x );
    f4->y = static_cast<XReal>( val.y );
    f4->z = static_cast<XReal>( val.z );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      (*af)[idx] = static_cast<XReal>( val.x );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      (*af2)[idx].x = static_cast<XReal>( val.x );
      (*af2)[idx].y = static_cast<XReal>( val.y );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      (*af3)[idx].x = static_cast<XReal>( val.x );
      (*af3)[idx].y = static_cast<XReal>( val.y );
      (*af3)[idx].z = static_cast<XReal>( val.z );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      (*af4)[idx].x = static_cast<XReal>( val.x );
      (*af4)[idx].y = static_cast<XReal>( val.y );
      (*af4)[idx].z = static_cast<XReal>( val.z );
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XVector4& val,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    col->r = static_cast<XReal>( val.x );
    col->g = static_cast<XReal>( val.y );
    col->b = static_cast<XReal>( val.w );
    col->a = static_cast<XReal>( val.z );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      (*acol)[idx].r = static_cast<XReal>( val.x );
      (*acol)[idx].g = static_cast<XReal>( val.y );
      (*acol)[idx].b = static_cast<XReal>( val.z );
      (*acol)[idx].a = static_cast<XReal>( val.w );
    }
    break;

  case XFn::IntAttribute:
    i = static_cast<XS32>( val.x );
    break;

  case XFn::Int2Attribute:
    i2->x = static_cast<XS32>( val.x );
    i2->y = static_cast<XS32>( val.y );
    break;

  case XFn::Int3Attribute:
    i3->x = static_cast<XS32>( val.x );
    i3->y = static_cast<XS32>( val.y );
    i3->z = static_cast<XS32>( val.z );
    break;

  case XFn::Int4Attribute:
    i4->x = static_cast<XS32>( val.x );
    i4->y = static_cast<XS32>( val.y );
    i4->z = static_cast<XS32>( val.z );
    i4->w = static_cast<XS32>( val.w );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      (*ai)[idx] = static_cast<XS32>( val.x );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      (*ai2)[idx].x = static_cast<XS32>( val.x );
      (*ai2)[idx].y = static_cast<XS32>( val.y );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      (*ai3)[idx].x = static_cast<XS32>( val.x );
      (*ai3)[idx].y = static_cast<XS32>( val.y );
      (*ai3)[idx].z = static_cast<XS32>( val.z );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      (*ai4)[idx].x = static_cast<XS32>( val.x );
      (*ai4)[idx].y = static_cast<XS32>( val.y );
      (*ai4)[idx].z = static_cast<XS32>( val.z );
      (*ai4)[idx].w = static_cast<XS32>( val.w );
    }
    break;

  case XFn::FloatAttribute:
    f = static_cast<XReal>( val.x );
    break;

  case XFn::Float2Attribute:
    f2->x = static_cast<XReal>( val.x );
    f2->y = static_cast<XReal>( val.y );
    break;

  case XFn::Float3Attribute:
    f3->x = static_cast<XReal>( val.x );
    f3->y = static_cast<XReal>( val.y );
    f3->z = static_cast<XReal>( val.z );
    break;

  case XFn::Float4Attribute:
    f4->x = static_cast<XReal>( val.x );
    f4->y = static_cast<XReal>( val.y );
    f4->z = static_cast<XReal>( val.z );
    f4->w = static_cast<XReal>( val.w );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      (*af)[idx] = static_cast<XReal>( val.x );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      (*af2)[idx].x = static_cast<XReal>( val.x );
      (*af2)[idx].y = static_cast<XReal>( val.y );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      (*af3)[idx].x = static_cast<XReal>( val.x );
      (*af3)[idx].y = static_cast<XReal>( val.y );
      (*af3)[idx].z = static_cast<XReal>( val.z );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      (*af4)[idx].x = static_cast<XReal>( val.x );
      (*af4)[idx].y = static_cast<XReal>( val.y );
      (*af4)[idx].z = static_cast<XReal>( val.z );
      (*af4)[idx].w = static_cast<XReal>( val.w );
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XColour& val,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::ColourAttribute:
    col->r = static_cast<XReal>( val.r );
    col->g = static_cast<XReal>( val.g );
    col->b = static_cast<XReal>( val.b );
    col->a = static_cast<XReal>( val.a );
    break;

  case XFn::ColourArrayAttribute:
    if (idx < acol->size())
    {
      (*acol)[idx].r = static_cast<XReal>( val.r );
      (*acol)[idx].g = static_cast<XReal>( val.g );
      (*acol)[idx].b = static_cast<XReal>( val.b );
      (*acol)[idx].a = static_cast<XReal>( val.a );
    }
    break;

  case XFn::IntAttribute:
    i = static_cast<XS32>( val.r );
    break;

  case XFn::Int2Attribute:
    i2->x = static_cast<XS32>( val.r );
    i2->y = static_cast<XS32>( val.g );
    break;

  case XFn::Int3Attribute:
    i3->x = static_cast<XS32>( val.r );
    i3->y = static_cast<XS32>( val.g );
    i3->z = static_cast<XS32>( val.b );
    break;

  case XFn::Int4Attribute:
    i4->x = static_cast<XS32>( val.r );
    i4->y = static_cast<XS32>( val.g );
    i4->z = static_cast<XS32>( val.b );
    break;

  case XFn::IntArrayAttribute:
    if (idx < ai->size())
    {
      (*ai)[idx] = static_cast<XS32>( val.r );
    }
    break;

  case XFn::Int2ArrayAttribute:
    if (idx < ai2->size())
    {
      (*ai2)[idx].x = static_cast<XS32>( val.r );
      (*ai2)[idx].y = static_cast<XS32>( val.g );
    }
    break;

  case XFn::Int3ArrayAttribute:
    if (idx < ai3->size())
    {
      (*ai3)[idx].x = static_cast<XS32>( val.r );
      (*ai3)[idx].y = static_cast<XS32>( val.g );
      (*ai3)[idx].z = static_cast<XS32>( val.b );
    }
    break;

  case XFn::Int4ArrayAttribute:
    if (idx < ai4->size())
    {
      (*ai4)[idx].x = static_cast<XS32>( val.r );
      (*ai4)[idx].y = static_cast<XS32>( val.g );
      (*ai4)[idx].z = static_cast<XS32>( val.b );
      (*ai4)[idx].w = static_cast<XS32>( val.a );
    }
    break;

  case XFn::FloatAttribute:
    f = static_cast<XReal>( val.r );
    break;

  case XFn::Float2Attribute:
    f2->x = static_cast<XReal>( val.r );
    f2->y = static_cast<XReal>( val.g );
    break;

  case XFn::Float3Attribute:
    f3->x = static_cast<XReal>( val.r );
    f3->y = static_cast<XReal>( val.g );
    f3->z = static_cast<XReal>( val.b );
    break;

  case XFn::Float4Attribute:
    f4->x = static_cast<XReal>( val.r );
    f4->y = static_cast<XReal>( val.g );
    f4->z = static_cast<XReal>( val.b );
    f4->w = static_cast<XReal>( val.a );
    break;

  case XFn::FloatArrayAttribute:
    if (idx < af->size())
    {
      (*af)[idx] = static_cast<XReal>( val.r );
    }
    break;

  case XFn::Float2ArrayAttribute:
    if (idx < af2->size())
    {
      (*af2)[idx].x = static_cast<XReal>( val.r );
      (*af2)[idx].y = static_cast<XReal>( val.g );
    }
    break;

  case XFn::Float3ArrayAttribute:
    if (idx < af3->size())
    {
      (*af3)[idx].x = static_cast<XReal>( val.r );
      (*af3)[idx].y = static_cast<XReal>( val.g );
      (*af3)[idx].z = static_cast<XReal>( val.b );
    }
    break;

  case XFn::Float4ArrayAttribute:
    if (idx < af4->size())
    {
      (*af4)[idx].x = static_cast<XReal>( val.r );
      (*af4)[idx].y = static_cast<XReal>( val.g );
      (*af4)[idx].z = static_cast<XReal>( val.b );
      (*af4)[idx].w = static_cast<XReal>( val.a );
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XString& value,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::StringAttribute:
    *s=value;
    break;

  case XFn::StringArrayAttribute:
    if (idx < as->size())
    {
      (*as)[idx]=value;
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::Set(const XMatrix& val,XU32 idx)
{
  switch(m_AttributeType)
  {
  case XFn::MatrixAttribute:
    *m=val;
    break;

  case XFn::MatrixArrayAttribute:
    if (idx < am->size())
    {
      (*am)[idx]=val;
    }
    break;

  default:
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::IsArray() const
{
  return  m_AttributeType == XFn::ColourArrayAttribute ||
          m_AttributeType == XFn::BoolArrayAttribute ||
          m_AttributeType == XFn::IntArrayAttribute ||
          m_AttributeType == XFn::Int2ArrayAttribute ||
          m_AttributeType == XFn::Int3ArrayAttribute ||
          m_AttributeType == XFn::Int4ArrayAttribute ||
          m_AttributeType == XFn::FloatArrayAttribute ||
          m_AttributeType == XFn::Float2ArrayAttribute ||
          m_AttributeType == XFn::Float3ArrayAttribute ||
          m_AttributeType == XFn::Float4ArrayAttribute ||
          m_AttributeType == XFn::MessageArrayAttribute ||
          m_AttributeType == XFn::MatrixArrayAttribute ||
          m_AttributeType == XFn::StringArrayAttribute;
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute::Connection* XAttribute::FindInput(XU32 idx)
{
  DUMPER(XAttribute__FindInput);
  if (HasInputConnection(idx))
  {
    ConnectionArray::iterator it = m_Inputs->begin();
    ConnectionArray::iterator end = m_Inputs->end();
    for ( ; it != end; ++it )
    {
      if (it->array_idx == idx)
      {
        return &(*it);
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute::Connection* XAttribute::FindOutput(const XU32 idx,const XU32 connection_num)
{
  DUMPER(XAttribute__FindOutput);
  if (HasOutputConnection(idx))
  {
    ConnectionArray::iterator it = m_Outputs->begin();
    ConnectionArray::iterator end = m_Outputs->end();
    for (XU32 j=0; it != end; ++it,++j )
    {
      if (it->array_idx == idx)
      {
        if(j==connection_num)
          return &(*it);
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute::Connection* XAttribute::FindOutputFronNode(const XU32 idx,const XU32 node_num)
{
  DUMPER(XAttribute__FindOutputFronNode);
  if (HasOutputConnection(idx))
  {
    ConnectionArray::iterator it = m_Outputs->begin();
    ConnectionArray::iterator end = m_Outputs->end();
    for ( ; it != end; ++it )
    {
      if (it->array_idx == idx)
      {
        if(it->node_id == node_num)
          return &(*it);
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAttribute::Connection* XAttribute::FindInput(const XU32 idx) const
{
  DUMPER(XAttribute__FindInput);
  if (HasInputConnection(idx))
  {
    ConnectionArray::const_iterator it = m_Inputs->begin();
    ConnectionArray::const_iterator end = m_Inputs->end();
    for ( ; it != end; ++it )
    {
      if (it->array_idx == idx)
      {
        return &(*it);
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAttribute::Connection* XAttribute::FindOutput(const XU32 idx,const XU32 connection_num) const
{
  DUMPER(XAttribute__FindOutput);
  if (HasOutputConnection(idx))
  {
    ConnectionArray::const_iterator it = m_Outputs->begin();
    ConnectionArray::const_iterator end = m_Outputs->end();
    for (XU32 j=0; it != end; ++it,++j )
    {
      if (it->array_idx == idx)
      {
        if(j==connection_num)
          return &(*it);
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAttribute::Connection* XAttribute::FindOutputFronNode(const XU32 idx,const XU32 node_num) const
{
  DUMPER(XAttribute__FindOutputFronNode);
  if (HasOutputConnection(idx))
  {
    ConnectionArray::const_iterator it = m_Outputs->begin();
    ConnectionArray::const_iterator end = m_Outputs->end();
    for ( ; it != end; ++it )
    {
      if (it->array_idx == idx)
      {
        if(it->node_id == node_num)
          return &(*it);
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::HasInputConnection(const XU32 array_idx) const
{
  DUMPER(XAttribute__HasInputConnection);
  if(!m_Inputs) return false;
  ConnectionArray::const_iterator it  = m_Inputs->begin();
  ConnectionArray::const_iterator end = m_Inputs->end();
  for ( ; it != end; ++it )
  {
    if (it->array_idx == array_idx)
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::HasOutputConnection(const XU32 array_idx) const
{
  DUMPER(XAttribute__HasOutputConnection);
  if(!m_Outputs) return false;
  ConnectionArray::const_iterator it = m_Outputs->begin();
  ConnectionArray::const_iterator end = m_Outputs->end();
  for ( ; it != end; ++it )
  {
    if (it->array_idx == array_idx)
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XAttribute::NodeDeath(XModel* inmodel,XId node)
{
  DUMPER(XAttribute__NodeDeath);
  const XDeletePolicy& dp = inmodel->GetDeletePolicy();

  if(m_Outputs && dp.IsEnabled(XDeletePolicy::Outputs) )
  {
    ConnectionArray::iterator it = m_Outputs->begin();
    for (XU32 i=0 ; it < m_Outputs->end();  )
    {
      Connection& conn = (*it);

      if (conn.node_id == node)
      {
        m_Outputs->erase( m_Outputs->begin() + i );
        it = m_Outputs->begin() + i ;
      }
      else
      {
        ++i;
        ++it;
      }
    }
  }

  if(m_Inputs && dp.IsEnabled(XDeletePolicy::Inputs))
  {
    ConnectionArray::iterator it = m_Inputs->begin();
    for (XU32 i=0 ; it < m_Inputs->end();  )
    {
      Connection& conn = (*it);

      if (conn.node_id == node)
      {
        m_Inputs->erase( m_Inputs->begin() + i );
        it = m_Inputs->begin() + i ;
      }
      else
      {
        ++i;
        ++it;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::SetInputConnection(const XBase* inputnode,const XU32 array_idx,const XChar* extra_info)
{
  DUMPER(XAttribute__SetInputConnection);
  Connection* pc = FindInput(array_idx);
  if (pc)
  {
    BreakInputConnections(array_idx);
  }

  Connection c(inputnode->GetID(),extra_info,array_idx);
  if (!m_Inputs)
  {
    m_Inputs = new ConnectionArray;
  }
  m_Inputs->push_back(c);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::SetInputConnection(const XId inputnode,const XU32 array_idx,const XChar* extra_info)
{
  DUMPER(XAttribute__SetInputConnection);
  Connection* pc = FindInput(array_idx);
  if (pc)
  {
    BreakInputConnections(array_idx);
  }

  Connection c(inputnode,extra_info,array_idx);
  if (!m_Inputs)
  {
    m_Inputs = new ConnectionArray;
  }
  m_Inputs->push_back(c);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::SetOutputConnection(const XBase* outputnode,bool add_connection, const XU32 array_idx,const XChar* extra_info)
{
  DUMPER(XAttribute__SetOutputConnection);
  if(add_connection)
  {
    Connection conn;
    conn.array_idx = array_idx;
    conn.extra_flag=extra_info;
    conn.node_id=outputnode->GetID();
    if (!m_Outputs)
    {
      m_Outputs = new ConnectionArray;
    }
    m_Outputs->push_back(conn);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::SetOutputConnection(const XId outputnode,bool add_connection,const XU32 array_idx,const XChar* extra_info)
{
  DUMPER(XAttribute__SetOutputConnection);
  if(add_connection)
  {
    Connection conn;
    conn.array_idx = array_idx;
    conn.extra_flag=extra_info;
    conn.node_id=outputnode;
    if (!m_Outputs)
    {
      m_Outputs = new ConnectionArray;
    }
    m_Outputs->push_back(conn);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAttribute::GetInputConnectionNode(XModel* inmodel,const XU32 array_idx) const
{
  DUMPER(XAttribute__GetInputConnectionNode);
  const Connection* conn = FindInput(array_idx);
  if (conn)
  {
    return inmodel->FindNode(conn->node_id);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAttribute::GetOutputConnectionNode(XModel* inmodel,const XU32 connection_num) const
{
  DUMPER(XAttribute__GetOutputConnectionNode);
  const Connection* conn = FindOutput(0,connection_num);
  if (conn)
  {
    return inmodel->FindNode(conn->node_id);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAttribute::GetOutputConnectionNode(XModel* inmodel,const XU32 array_idx,const XU32 connection_num) const
{
  DUMPER(XAttribute__GetOutputConnectionNode);
  const Connection* conn = FindOutput(array_idx,connection_num);
  if (conn)
  {
    return inmodel->FindNode(conn->node_id);
  }
  return 0;
}

XString emptyString;

//----------------------------------------------------------------------------------------------------------------------
const XString& XAttribute::GetInputConnectionName(XModel* inmodel,const XU32 array_idx) const
{
  DUMPER(XAttribute__GetInputConnectionName);
  (void)inmodel;
  const Connection* conn = FindInput(array_idx);
  if (conn)
  {
    return conn->extra_flag;
  }
  return emptyString;
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XAttribute::GetOutputConnectionName(XModel* inmodel,const XU32 array_idx,const XU32 connection_num) const
{
  DUMPER(XAttribute__GetOutputConnectionName);
  (void)inmodel;
  const Connection* conn = FindOutput(array_idx,connection_num);
  if (conn)
  {
    return conn->extra_flag;
  }
  return emptyString;
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XAttribute::GetOutputConnectionName(XModel* inmodel,const XU32 connection_num) const
{
  DUMPER(XAttribute__GetOutputConnectionName);
  (void)inmodel;
  const Connection* conn = FindOutput(0,connection_num);
  if (conn)
  {
    return conn->extra_flag;
  }
  return emptyString;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAttribute::GetNumOutputConnections(const XU32 array_idx) const
{
  DUMPER(XAttribute__GetNumOutputConnections);
  XU32 returnVal=0;
  if (HasOutputConnection(array_idx))
  {
    ConnectionArray::iterator it = m_Outputs->begin();
    ConnectionArray::iterator end = m_Outputs->end();
    for ( ; it != end; ++it )
    {
      if (it->array_idx == array_idx)
      {
        ++returnVal;
      }
    }
  }
  return returnVal;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::BreakInputConnections()
{
  DUMPER(XAttribute__BreakInputConnections);
  delete m_Inputs;
  m_Inputs = 0;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::BreakInputConnections(XU32 array_idx)
{
  DUMPER(XAttribute__BreakInputConnections);
  if (HasInputConnection(array_idx))
  {
    ConnectionArray::iterator it = m_Inputs->begin();
    ConnectionArray::iterator end = m_Inputs->end();
    for ( ; it != end; ++it )
    {
      if (it->array_idx == array_idx)
      {
        m_Inputs->erase(it);
        if (m_Inputs->size()==0)
        {
          BreakInputConnections();
        }
        return true;
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::BreakOutputConnection(XU32 array_idx,XU32 connection_num)
{
  DUMPER(XAttribute__BreakInputConnections);
  if (HasOutputConnection(array_idx))
  {
    XU32 i=0;
    ConnectionArray::iterator it = m_Outputs->begin();
    ConnectionArray::iterator end = m_Outputs->end();
    for ( ; it != end; ++it )
    {
      if (it->array_idx == array_idx)
      {
        if(i == connection_num)
        {
          m_Outputs->erase(it);
          if (m_Outputs->size()==0)
          {
            BreakAllOutputConnections();
          }
          return true;
        }
        ++i;
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::BreakOutputConnection(XU32 connection_num)
{
  DUMPER(XAttribute__BreakOutputConnection);
  return BreakOutputConnection(0,connection_num);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::BreakAllOutputConnections(XU32 array_idx)
{
  DUMPER(XAttribute__BreakAllOutputConnections);
  if (HasOutputConnection(array_idx))
  {
    XU32 i=0;
    for ( i=0; i < m_Outputs->size(); ++i )
    {
      if (m_Outputs->at(i).array_idx == array_idx)
      {
        m_Outputs->erase(m_Outputs->begin()+i);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::BreakAllOutputConnections()
{
  DUMPER(XAttribute__BreakAllOutputConnections);
  delete m_Outputs;
  m_Outputs=0;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XBoolArray& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      bool b;
      Get(b,i);
      value[i] = b;
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XS32Array& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XInt2Array& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XInt3Array& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XInt4Array& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XRealArray& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XVector2Array& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XVector3Array& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XVector4Array& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XStringList& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XColourArray& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Get(XMatrixArray& value) const
{
  if(IsArray())
  {
    value.resize(NumElements());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Get(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XBoolArray& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XS32Array& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XInt2Array& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XInt3Array& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XInt4Array& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XRealArray value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XVector2Array& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XVector3Array& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XVector4Array& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XStringList& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XColourArray& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAttribute::Set(const XMatrixArray& value)
{
  if(IsArray())
  {
    Resize((XU32)value.size());
    for (XU32 i=0;i!=NumElements();++i)
    {
      Set(value[i],i);
    }
    return true;
  }
  return false;
}
}

