//----------------------------------------------------------------------------------------------------------------------
/// \file ShadingNode.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ShadingNode.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
NMTL_POD_VECTOR_EXPORT(XShadingNode::XColourConnection*,XMD_EXPORT,XM2);

//----------------------------------------------------------------------------------------------------------------------
XShadingNode::XShadingNode(XModel* pmod)
  : XBase(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode::~XShadingNode()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XShadingNode::ListConnections(XList& list,bool inputs,bool outputs)
{
  if(inputs)
  {
    XConnectionArray::const_iterator it = m_Connections.begin();
    for( ; it != m_Connections.end(); ++it)
    {
      XBase* node = m_pModel->FindNode( (*it)->m_InputNode );
      if(node)
      {
        list.push_back(node);
      }
    }
  }
  if(outputs)
  {
    XList shading_nodes;
    m_pModel->List(shading_nodes,XFn::ShadingNode);
    XList::iterator it = shading_nodes.begin();
    for (;it != shading_nodes.end();++it) 
    {
      XShadingNode* node = (*it)->HasFn<XShadingNode>();
      XMD_ASSERT(node);
      if(node)
      {
        // skip the node if it is this one
        if(node == this) continue;

        XConnectionArray::const_iterator conn = node->m_Connections.begin();
        for ( ; conn != node->m_Connections.end(); ++conn )
        {
          if((*conn)->m_InputNode == m_ThisID)
          {
            list.push_back(node);
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XShadingNode::GetApiType() const
{
  return XFn::ShadingNode;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XShadingNode::GetFn(XFn::Type type)
{
  if(XFn::ShadingNode==type)
    return (XShadingNode*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XShadingNode::GetFn(XFn::Type type) const
{
  if(XFn::ShadingNode==type)
    return (const XShadingNode*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XShadingNode::DoCopy(const XBase* rhs)
{
  const XShadingNode* cb = rhs->HasFn<XShadingNode>();
  XMD_ASSERT(cb);
  XBase::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XShadingNode::GetDataSize() const
{
  return XBase::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XShadingNode::DoData(XFileIO& io)
{
  DUMPER(XShadingNode);

  IO_CHECK( XBase::DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XShadingNode::ReadChunk(std::istream& ifs)
{
  DUMPER(XShadingNode__ReadChunk);
  
  XBase::ReadChunk(ifs);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XShadingNode::WriteChunk(std::ostream& ofs)
{
  DUMPER(XShadingNode__WriteChunk);

  XBase::WriteChunk(ofs);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XShadingNode::NodeDeath(XId id)
{
  XConnectionArray::iterator it = m_Connections.begin();
  for(;it!=m_Connections.end();++it)
  {
    if(id==(*it)->m_InputNode)
      (*it)->m_InputNode=0;
  }
  return XBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XShadingNode::PreDelete(XIdSet& extra_nodes)
{
  XBase::PreDelete(extra_nodes);

  const XDeletePolicy& dp = GetModel()->GetDeletePolicy();

  if(!dp.IsEnabled(XDeletePolicy::RelatedMaterials) &&
     !dp.IsEnabled(XDeletePolicy::RelatedTextures))
  {
    return;
  }
  

  XConnectionArray::const_iterator it = m_Connections.begin();
  for(;it!=m_Connections.end();++it)
  {
    if((*it)->m_InputNode==0)
      continue;

    XBase* node = GetModel()->FindNode((*it)->m_InputNode);
    if(node)
    {
      if( dp.IsEnabled(XDeletePolicy::RelatedMaterials) )
      {
        XMaterial* material = node->HasFn<XMaterial>();
        if( material )
        {
          if(material->NumTimesReferenced(&m_ThisID) == 0)
            extra_nodes.insert((*it)->m_InputNode);
          
          continue;
        }
      }
      if( dp.IsEnabled(XDeletePolicy::RelatedTextures) )
      {
        XTexture* texture = node->HasFn<XTexture>();
        if( texture )
        {
          //if(texture->NumTimesReferenced(GetID()) == 0)
          //  extra_nodes.insert((*it)->m_InputNode);
          continue;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XShadingNode::NumTimesReferenced(XId* ignore_id) const
{
  XU32 num=0;
  XList shading_nodes;
  m_pModel->List(shading_nodes,XFn::ShadingNode);
  XList::const_iterator it = shading_nodes.begin();
  for (;it != shading_nodes.end();++it) 
  {
    const XShadingNode* node = (*it)->HasFn<XShadingNode>();
    XMD_ASSERT(node);
    if(node)
    {
      // skip the node if it is this one
      if(node == this) continue;
      // if a node to ignore was specified skip it
      if(ignore_id && *ignore_id == node->GetID()) continue;

      XConnectionArray::const_iterator conn = node->m_Connections.begin();
      for (;conn != node->m_Connections.end();++conn)
      {
        if((*conn)->m_InputNode == m_ThisID)
        {
          ++num;
        }
      }
    }
  }

  return num;
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode::XColourConnection::XColourConnection(const XChar* name)
  : m_Name(name), m_InputNode(0), m_Colour(0.0f, 0.0f, 0.0f, 1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode::XColourConnection::XColourConnection(const XChar* name,const XId input)
  : m_Name(name), m_InputNode(input), m_Colour(0.0f, 0.0f, 0.0f, 1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode::XColourConnection::XColourConnection(const XChar* name,const XColour& colour)
  : m_Name(name), m_InputNode(0), m_Colour(colour)
{
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode::XColourConnection::XColourConnection(const XChar* name,const XId input,const XColour& colour)
  : m_Name(name), m_InputNode(input), m_Colour(colour)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XShadingNode::XColourConnection::ReadChunk(std::istream& ifs)
{
  READ_CHECK(m_Name.c_str(),ifs);
  ifs >> m_Colour;
  READ_CHECK("input",ifs);
  ifs >> m_InputNode;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XShadingNode::XColourConnection::WriteChunk(std::ostream& ofs)
{
  ofs << "\t"
      << m_Name << " "
      << m_Colour
      << " input " << m_InputNode
      << std::endl;
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XShadingNode::XColourConnection::DoData(XFileIO& io)
{
  DUMPER(XColourConnection);

  IO_CHECK( io.DoData(&m_InputNode) );
  DPARAM( m_InputNode );
  IO_CHECK( io.DoData(&m_Colour.r) ); DPARAM( m_Colour.r );
  IO_CHECK( io.DoData(&m_Colour.g) ); DPARAM( m_Colour.g );
  IO_CHECK( io.DoData(&m_Colour.b) ); DPARAM( m_Colour.b );
  IO_CHECK( io.DoData(&m_Colour.a) ); DPARAM( m_Colour.a );
  IO_CHECK( io.DoDataString(m_Name) );
  DPARAM( m_Name );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XShadingNode::XColourConnection::GetDataSize() const {
  XU32 sz = sizeof(XReal)*4 + sizeof(XU32) + 2;
  sz += static_cast<XU32>(m_Name.size());
  return sz;
}
}
