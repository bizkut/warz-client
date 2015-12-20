//----------------------------------------------------------------------------------------------------------------------
/// \file WireDeformer.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/WireDeformer.h"
#include "XMD/Model.h"
#include "XMD/ParametricCurve.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XWireDeformer::XWire::XWire() 
  : m_BaseWire(0),m_Wire(0),m_DropoffDistance(1.0f),m_WireScale(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XWireDeformer::XWire::XWire(const XWire& wire)
  : m_BaseWire(wire.m_BaseWire),m_Wire(wire.m_Wire),
    m_DropoffDistance(wire.m_DropoffDistance),m_WireScale(wire.m_WireScale)
{
}

//----------------------------------------------------------------------------------------------------------------------
XWireDeformer::XWireDeformer(XModel* pmod) 
  : XDeformer(pmod),m_Wires()
{
  m_CrossingEffect=0;
  m_LocalIntensity=0;
  m_Rotation=0;
}

//----------------------------------------------------------------------------------------------------------------------
XWireDeformer::~XWireDeformer(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XWireDeformer::GetApiType() const
{
  return XFn::WireDeformer;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XWireDeformer::GetFn(XFn::Type type)
{
  if(XFn::WireDeformer==type)
    return (XWireDeformer*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XWireDeformer::GetFn(XFn::Type type) const
{
  if(XFn::WireDeformer==type)
    return (const XWireDeformer*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XWireDeformer::NodeDeath(XId id)
{
  return XDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XWireDeformer::PreDelete(XIdSet& extra_nodes)
{
  XDeformer::PreDelete(extra_nodes);

  const XDeletePolicy& dp = GetModel()->GetDeletePolicy();
  if ( dp.IsEnabled(XDeletePolicy::RelatedDeformerInputs) ) 
  {
    for (XU32 i=0;i!=GetNumWires();++i) 
    {
      if( m_Wires[i].m_Wire )
        extra_nodes.insert(m_Wires[i].m_Wire);
      if( m_Wires[i].m_BaseWire )
        extra_nodes.insert(m_Wires[i].m_BaseWire);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XWireDeformer::ReadChunk(std::istream& ifs)
{  
  if (!XDeformer::ReadChunk(ifs)) 
    return false;

  READ_CHECK("crossing_effect",ifs);
  ifs >> m_CrossingEffect;

  READ_CHECK("local_intensity",ifs);
  ifs >> m_LocalIntensity;

  READ_CHECK("rotation",ifs);
  ifs >> m_Rotation;

  XU32 temp;
  READ_CHECK("num_wires",ifs);
  ifs >> temp;

  m_Wires.resize(temp);

  if(GetModel()->GetInputVersion()>=4)
  {
    for (XU32 i=0;i!=GetNumWires();++i) 
    {
      ifs >> m_Wires[i].m_BaseWire >> m_Wires[i].m_Wire >> m_Wires[i].m_DropoffDistance >> m_Wires[i].m_WireScale;
    }
  }
  else
  {
    for (XU32 i=0;i!=GetNumWires();++i) 
    {
      ifs >> m_Wires[i].m_Wire >> m_Wires[i].m_DropoffDistance >> m_Wires[i].m_WireScale;
      m_Wires[i].m_BaseWire = m_Wires[i].m_Wire;
    }
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XWireDeformer::WriteChunk(std::ostream& ofs) 
{
  if (!m_AffectedGeometry.size())
  {
    return true;
  }

  if (!XDeformer::WriteChunk(ofs))
    return false;

  ofs << "\tcrossing_effect " << m_CrossingEffect << "\n";
  ofs << "\tlocal_intensity " << m_LocalIntensity << "\n";
  ofs << "\trotation " << m_Rotation << "\n";

  ofs << "\tnum_wires " << GetNumWires() << "\n";
  for (XU32 i=0;i!=GetNumWires();++i) 
  {
    ofs << "\t\t" << m_Wires[i].m_BaseWire << " " 
        << m_Wires[i].m_Wire << " " 
        << m_Wires[i].m_DropoffDistance << " " 
        << m_Wires[i].m_WireScale << "\n";
  }
  ofs << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XWireDeformer::DoData(XFileIO& io)
{
  DUMPER(XWireDeformer);

  IO_CHECK( XDeformer::DoData(io) );

  if (GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( io.DoDataVector( m_Wires ) );
  }
  else
  {
    XU32 num;
    IO_CHECK( io.DoData(&num) );
    m_Wires.resize(num);
    for (XU32 i=0;i!=num;++i)
    {
      IO_CHECK( io.DoData( &m_Wires[i].m_Wire ) );
    }
    IO_CHECK( io.DoData(&num) );
    for (XU32 i=0;i!=num;++i)
    {
      IO_CHECK( io.DoData( &m_Wires[i].m_DropoffDistance ) );
    }
    IO_CHECK( io.DoData(&num) );
    for (XU32 i=0;i!=num;++i)
    {
      IO_CHECK( io.DoData( &m_Wires[i].m_WireScale ) );
    }
  }

  IO_CHECK( io.DoData( &m_CrossingEffect ) );
  DPARAM( m_CrossingEffect );

  IO_CHECK( io.DoData( &m_LocalIntensity ) );
  DPARAM( m_LocalIntensity );

  IO_CHECK( io.DoData( &m_Rotation ) );
  DPARAM( m_Rotation );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XWireDeformer::DoCopy(const XBase* rhs)
{
  const XWireDeformer* cb = rhs->HasFn<XWireDeformer>();
  XMD_ASSERT(cb);
  m_Wires = cb->m_Wires;
  m_CrossingEffect = cb->m_CrossingEffect;
  m_LocalIntensity = cb->m_LocalIntensity;
  m_Rotation = cb->m_Rotation;
  XDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XWireDeformer::GetDataSize() const
{
  return 1*sizeof(XU32) + 
         3*sizeof(XReal) +  
         sizeof(XWire)*GetNumWires() +
         XDeformer::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XWireDeformer::GetNumWires() const
{
  return static_cast<XU32>(m_Wires.size());
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XWireDeformer::GetWire(const XU32 wire_index) const
{
  if(wire_index<GetNumWires())
    return m_pModel->FindNode( m_Wires[wire_index].m_Wire );
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XWireDeformer::GetBaseWire(const XU32 wire_index) const
{
  if(wire_index<GetNumWires())
    return m_pModel->FindNode( m_Wires[wire_index].m_BaseWire );
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XWireDeformer::GetWireDropoffDistance(XU32 wire_index) const
{
  if(wire_index<GetNumWires())
    return m_Wires[wire_index].m_DropoffDistance;
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XWireDeformer::GetWireScale(XU32 wire_index) const
{
  if(wire_index<GetNumWires())
    return m_Wires[wire_index].m_WireScale;
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XWireDeformer::GetCrossingEffect() const
{
  return  m_CrossingEffect;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XWireDeformer::GetLocalIntensity() const
{
  return m_LocalIntensity;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XWireDeformer::GetRotation() const
{
  return m_Rotation;
}

//----------------------------------------------------------------------------------------------------------------------
bool XWireDeformer::AddWire(const XBase* deformed_wire_object,
                            const XBase* base_wire_object)
{
  // if you assert here, the wire_object is NULL or from another XModel
  XMD_ASSERT( (deformed_wire_object) && IsValidObj(deformed_wire_object) );
  XMD_ASSERT( (base_wire_object) && IsValidObj(base_wire_object) );

  // check to make sure that the wire has not been used before
  XWires::iterator it = m_Wires.begin();
  for (; it != m_Wires.end(); ++it) 
  {
    if(it->m_BaseWire == deformed_wire_object->GetID() || 
        it->m_Wire == deformed_wire_object->GetID()) 
    {
      return false;
    }
    if(it->m_BaseWire == base_wire_object->GetID() || 
       it->m_Wire == base_wire_object->GetID() ) 
    {
      return false;
    }
  }

  if ( deformed_wire_object->HasFn<XParametricCurve>() &&
       base_wire_object->HasFn<XParametricCurve>() ) 
  {
    XWire wire;
    wire.m_BaseWire = base_wire_object->GetID();
    wire.m_Wire = deformed_wire_object->GetID();
    wire.m_DropoffDistance = 1.0f;
    wire.m_WireScale = 1.0f;
    m_Wires.push_back( wire );
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XWireDeformer::SetWireDropoffDistance(const XU32 wire_index,const XReal value)
{

  if(wire_index<GetNumWires()) 
  {
    m_Wires[wire_index].m_DropoffDistance = value;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XWireDeformer::SetWireScale(const XU32 wire_index,const XReal value)
{
  if(wire_index<GetNumWires()) 
  {
    m_Wires[wire_index].m_WireScale = value;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XWireDeformer::SetCrossingEffect(const XReal value)
{
  m_CrossingEffect = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XWireDeformer::SetLocalIntensity(const XReal value)
{
  m_LocalIntensity = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XWireDeformer::SetRotation(const XReal value)
{
  m_Rotation = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XWireDeformer::GetWires(XList& wires) const
{
  wires.clear();
  XWires::const_iterator it = m_Wires.begin();
  for (; it != m_Wires.end(); ++it) 
  {
    wires.push_back( m_pModel->FindNode(it->m_Wire) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XWireDeformer::GetBaseWires(XList& wires) const
{
  wires.clear();
  XWires::const_iterator it = m_Wires.begin();
  for (; it != m_Wires.end(); ++it) 
  {
    wires.push_back( m_pModel->FindNode(it->m_BaseWire) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XWireDeformer::RemoveWire(const XU32 wire_index)
{
  if(m_Wires.size()>wire_index)
  {
    m_Wires.erase(m_Wires.begin()+wire_index);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XWireDeformer::SetWires(const XList& wires,const XList& base_wires)
{
  // make sure we have the same number of wires as we do base wires
  if (base_wires.size() != wires.size())
  {
    return false;
  }

  XList::const_iterator it = wires.begin();
  XList::const_iterator it_base = base_wires.begin();
  for (;it != wires.end(); ++it,++it_base)
  {
    if(!(*it))
      return false;
    if(!(*it_base))
      return false;

    //
    // If you assert here, then you have provided an invalid 
    // wire. Either the node is not from this XModel, null, 
    // or simply not a parametric curve type
    //
    XMD_ASSERT(IsValidObj(*it));
    if(!IsValidObj(*it)) 
    {
      return false;
    }

    //
    // If you assert here, then you have provided an invalid 
    // wire. Either the node is not from this XModel, null, 
    // or simply not a parametric curve type
    //
    XMD_ASSERT(IsValidObj(*it_base));
    if(!IsValidObj(*it_base)) 
    {
      return false;
    }
  }
 
  // resize internal wires array
  m_Wires.resize(wires.size());

  // set new wire params
  XWires::iterator it_internal = m_Wires.begin();
  it = wires.begin();
  it_base = base_wires.begin();
  for (;it_internal != m_Wires.end(); ++it_internal, ++it, ++it_base)
  {
    it_internal->m_Wire = (*it)->GetID();
    it_internal->m_BaseWire = (*it_base)->GetID();
    it_internal->m_DropoffDistance = 1.0f;
    it_internal->m_WireScale = 1.0f;
  }
  return true;
}
}
