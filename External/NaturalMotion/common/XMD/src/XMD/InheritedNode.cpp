//----------------------------------------------------------------------------------------------------------------------
/// \file InheritedNode.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/InheritedNode.h"
#include "XMD/NodeTypeRegister.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XFn::Type HeaderToType(const XString& chunk_header)
{
  return XGlobal::NodeTypeFromChunkName(chunk_header.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
XString TypeToHeader(const XFn::Type type)
{
  return XGlobal::ChunkNameFromNodeType(type);
}

//----------------------------------------------------------------------------------------------------------------------
XInheritedNode::XInheritedNode(const XU16 ID)
//  : m_TypeName(),m_BaseType(),m_TypeID(ID) 
{
  (void)ID;
}

//----------------------------------------------------------------------------------------------------------------------
XInheritedNode::XInheritedNode(const XInheritedNode& inode)
  : m_TypeName(inode.m_TypeName),m_BaseType(inode.m_BaseType)
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XCreateNode(XModel* pmod,const XFn::Type type)
{
  XBase* ptr = XGlobal::CreateFromId(type,pmod);
  if(!ptr)
  {
    if(type>=XFn::InheritedStart)
    {
      XU16 TypeID = (XU16)(0x0FFF&type);
      if(TypeID<pmod->m_Inherited.size()) 
      {
        XBase* temp = pmod->m_Inherited[TypeID]->Create(pmod); 
        temp->m_pInherited = pmod->m_Inherited[TypeID];
        return temp;
      }
    }
    else
    if(type>=XFn::CustomStart)
    {
    }
  }
  
  return ptr;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XInheritedNode::Create(XModel*  pmod) const
{
  return XCreateNode(pmod,m_BaseType);
}

//----------------------------------------------------------------------------------------------------------------------
bool XInheritedNode::DoData(XFileIO& io)
{
  DUMPER(XInheritedNode);

  CHECK_IO( io.DoData( &m_BaseType ) );
  DPARAM(m_BaseType);

  CHECK_IO( io.DoDataString( m_TypeName ) );
  DPARAM(m_TypeName);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInheritedNode::DoHeader(XFileIO& io) 
{
  DUMPER(XInheritedNode__DoHeader);
  DUMPER(DoHeader);
  XChunkHeader header;
  memset(&header,0,sizeof(XChunkHeader));
  if(io.IsWriting())
  {
    header.Type = (XU16) XFn::InheritedFileFlag;
    header.HasExtraAttrs = 0;
    header.Reserved = 0;
    header.ByteSize = GetDataSize();
  }
  DPARAM( header.Type );
  DPARAM( header.HasExtraAttrs );
  DPARAM( header.ByteSize );
  CHECK_IO( io.DoData(&header) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XInheritedNode::GetDataSize() const
{
  return static_cast<XU32>(4 + m_TypeName.size() + sizeof(XFn::Type) + sizeof(XChunkHeader));
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XInheritedNode::GetHeader() const
{
  return m_TypeName;
}

//----------------------------------------------------------------------------------------------------------------------
const XFn::Type& XInheritedNode::GetType() const
{
  return m_BaseType;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInheritedNode::Read(std::istream& ifs)
{
  XString buffer;
  DUMPER(XInheritedNode__Read);

  READ_CHECK("base_type",ifs);

  ifs >> buffer;

  m_BaseType = HeaderToType(buffer);
  if(m_BaseType == XFn::Base)
    return false;

  READ_CHECK("header",ifs);
  ifs >> buffer;
  m_TypeName = buffer;

  // make sure not already used
  if( HeaderToType(buffer) != XFn::Base )
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError,"The inherited node type cannot use a reserved chunk name");
    return false;
  }

  READ_CHECK("}",ifs);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInheritedNode::Write(std::ostream& ofs) const
{
  DUMPER(XInheritedNode__Write);
  XString output = TypeToHeader(m_BaseType);
  if(output == "" || m_TypeName == "")
    return false;
  ofs << "INHERITED_NODE\n{\n\t";
  ofs << "base_type " << output.c_str() << "\n\t";
  ofs << "header " << m_TypeName.c_str() << "\n}\n";
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU16 XInheritedNode::CustomTypeID() const
{
  return (XU16)(m_TypeID + ((XU16)XFn::InheritedStart));
}

//----------------------------------------------------------------------------------------------------------------------
const XFn::Type XInheritedNode::ApiType() const
{
  return XFn::InheritedFileFlag;
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XInheritedNode::GetTypeName() const
{
  return m_TypeName;
}
}
