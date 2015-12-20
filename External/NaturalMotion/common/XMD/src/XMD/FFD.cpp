//----------------------------------------------------------------------------------------------------------------------
/// \file FFD.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/FFD.h"
#include "XMD/Lattice.h"
#include "XMD/BaseLattice.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XFFD::XFFD(XModel* pmod) 
  : XDeformer(pmod),
  m_BaseLattice(0),
  m_DeformedLattice(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XFFD::~XFFD(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XFFD::GetApiType() const
{
  return XFn::FFd;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XFFD::GetDataSize() const 
{
  return XDeformer::GetDataSize() + 2*sizeof(XId);
}

//----------------------------------------------------------------------------------------------------------------------
XBaseLattice* XFFD::GetBaseLattice() const
{
  XBase* node = m_pModel->FindNode(m_BaseLattice);
  if (node) 
  {
    return node->HasFn< XBaseLattice >();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XLattice* XFFD::GetDeformedLattice() const
{
  XBase* node = m_pModel->FindNode(m_DeformedLattice);
  if (node) 
  {
    return node->HasFn< XLattice >();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XFFD::GetFn(XFn::Type type)
{
  if(XFn::FFd==type)
    return (XFFD*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XFFD::GetFn(XFn::Type type) const
{
  if(XFn::FFd==type)
    return (const XFFD*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XFFD::NodeDeath(XId id)
{
  // kill the emitter if the system it spits particles into dies
  if (id == m_BaseLattice ||
    id == m_DeformedLattice )
  {
    return false;
  }

  return XDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XFFD::PreDelete(XIdSet& extra_nodes)
{
  XDeformer::PreDelete(extra_nodes);

  const XDeletePolicy& dp = GetModel()->GetDeletePolicy();

  if ( dp.IsEnabled(XDeletePolicy::RelatedDeformerInputs) ) 
  {
    if( m_BaseLattice )
    {
      extra_nodes.insert(m_BaseLattice);
    }
    if( m_DeformedLattice )
    {
      extra_nodes.insert(m_DeformedLattice);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XFFD::ReadChunk(std::istream& ifs)
{
  if (!XDeformer::ReadChunk(ifs)) 
    return false;

  // the base lattice for the free-form deformation

  READ_CHECK("base",ifs);
  ifs  >> m_BaseLattice;    //    

  READ_CHECK("deformed",ifs);
  ifs  >> m_DeformedLattice;  //

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XFFD::SetBaseLattice(const XBase* p)
{
  if(!p)
  {
    m_BaseLattice = 0;
    return true;
  }

  // if you assert here, p is NULL or not from this XModel
  XMD_ASSERT( IsValidObj(p) );
  if ( p->HasFn<XBaseLattice>()) 
  {
    m_BaseLattice = p->GetID();
    XLattice* lattice = GetDeformedLattice();
    if(lattice)
    {
      lattice->m_BaseLattice = m_BaseLattice;
    }
    return true;
  }

  // if you assert here, you have attempted to assign a node that is
  // not an XBaseLattice.
  XMD_ASSERT(0);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFFD::SetDeformedLattice(const XBase* p)
{
  if(!p)
  {
    m_DeformedLattice = 0;
    return true;
  }

  // if you assert here, p is NULL or not from this XModel
  XMD_ASSERT( IsValidObj(p) );
  XLattice* lattice = (XLattice*)p->HasFn<XLattice>();
  if(lattice)
  {
    m_DeformedLattice = lattice->GetID();
    lattice->m_BaseLattice = m_BaseLattice;
    lattice->m_FFD = GetID();      
    return true;
  }

  // if you assert here, you have attempted to assign a node that is
  // not an XLattice.
  XMD_ASSERT(0);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFFD::WriteChunk(std::ostream& ofs)
{
  if (!XDeformer::WriteChunk(ofs))
    return false;

  // the base lattice for the free-form deformation
  ofs << "\tbase "        //    base
      << m_BaseLattice      //
      << "\n\tdeformed "      //    deformed
      << m_DeformedLattice;
    ofs << "\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XFFD::DoData(XFileIO& io)
{
  DUMPER(XFFD);

  IO_CHECK( XDeformer::DoData(io) );

  IO_CHECK( io.DoData(&m_BaseLattice) );
  DPARAM( m_BaseLattice );

  IO_CHECK( io.DoData(&m_DeformedLattice) );
  DPARAM( m_DeformedLattice );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XFFD::DoCopy(const XBase* rhs)
{
  const XFFD* cb = rhs->HasFn<XFFD>();
  XMD_ASSERT(cb);
  m_BaseLattice = cb->m_BaseLattice;
  m_DeformedLattice = cb->m_DeformedLattice;
  XDeformer::DoCopy(cb);
}
}

