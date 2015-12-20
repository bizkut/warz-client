//----------------------------------------------------------------------------------------------------------------------
/// \file Lattice.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Lattice.h"
#include "XMD/BaseLattice.h"
#include "XMD/FFD.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XLattice::XLattice(XModel* pmod)
  : XGeometry(pmod),m_BaseLattice(0),m_FFD(0),
  m_nDivisionsI(0),m_nDivisionsJ(0),m_nDivisionsK(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XLattice::~XLattice()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XLattice::GetApiType() const
{
  return XFn::Lattice;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XLattice::GetFn(XFn::Type type)
{
  if(XFn::Lattice==type)
    return (XLattice*)this;
  return XGeometry::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XLattice::GetFn(XFn::Type type) const
{
  if(XFn::Lattice==type)
    return (const XLattice*)this;
  return XGeometry::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XLattice::GetNumDivisions(XU32& i,XU32& j,XU32& k) const
{
  i = m_nDivisionsI;
  j = m_nDivisionsJ;
  k = m_nDivisionsK;
}

//----------------------------------------------------------------------------------------------------------------------
void XLattice::SetNumDivisions(const XU32 i,const XU32 j,const XU32 k)
{
  m_nDivisionsI = i;
  m_nDivisionsJ = j;
  m_nDivisionsK = k;
}

//----------------------------------------------------------------------------------------------------------------------
XBaseLattice* XLattice::GetBaseLattice()
{
  XBase* ptr = m_pModel->FindNode(m_BaseLattice);
  if (ptr)
  {
    return ptr->HasFn< XBaseLattice >();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XLattice::GetDataSize() const
{
  XU32 sz = sizeof(XU32)*3;
  return sz + XGeometry::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XFFD* XLattice::GetFFD()
{
  XBase* ptr = m_pModel->FindNode(m_FFD);
  if (ptr) 
  {
    return ptr->HasFn< XFFD >();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XLattice::GetPoint(XU32 i,XU32 j,XU32 k) const 
{
  return m_Points[ i*m_nDivisionsJ*m_nDivisionsK + j*m_nDivisionsK + k  ];
}

//----------------------------------------------------------------------------------------------------------------------
bool XLattice::NodeDeath(XId id)
{
  if(id == m_BaseLattice)
    return false;
  if(id == m_FFD)
    return false;
  return XGeometry::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XLattice::PreDelete(XIdSet& extra_nodes)
{
  XGeometry::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
void XLattice::PostLoad() 
{
  XGeometry::PostLoad();

  // build list of deformers in the queue. First blend shapes, skinning, lattices
  XList objs;

  m_pModel->List(objs,XFn::FFd);

  XList::iterator it = objs.begin();
  for( ; it != objs.end(); ++it )
  {
    XFFD* pd = (*it)->HasFn<XFFD>();

    if( pd->GetDeformedLattice() == this ) 
    {
      m_FFD = pd->GetID();
      XBaseLattice* pbase = pd->GetBaseLattice();
      m_BaseLattice = pbase? pbase->GetID() : 0;
      return;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XLattice::ReadChunk(std::istream& ifs)
{
  if(!XGeometry::ReadChunk(ifs))
    return false;

  READ_CHECK("divisions_s",ifs);
  ifs >> m_nDivisionsI;

  READ_CHECK("divisions_t",ifs);
  ifs >> m_nDivisionsJ;

  READ_CHECK("divisions_u",ifs);
  ifs >> m_nDivisionsK ;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XLattice::SetPoint(const XU32 i,const XU32 j,const XU32 k,const XVector3& p) 
{
  m_Points[ i*m_nDivisionsJ*m_nDivisionsK + j*m_nDivisionsK + k  ] = p;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLattice::WriteChunk(std::ostream& ofs)
{
  if(!XGeometry::WriteChunk(ofs))
    return false;

  ofs << "\tdivisions_s " << m_nDivisionsI << "\n";
  ofs << "\tdivisions_t " << m_nDivisionsJ << "\n";
  ofs << "\tdivisions_u " << m_nDivisionsK << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XLattice::DoData(XFileIO& io) 
{
  DUMPER(XLattice);

  IO_CHECK( XGeometry::DoData(io) );

  IO_CHECK( io.DoData(&m_nDivisionsI) );
  DPARAM( m_nDivisionsI );

  IO_CHECK( io.DoData(&m_nDivisionsJ) );
  DPARAM( m_nDivisionsJ );

  IO_CHECK( io.DoData(&m_nDivisionsK) );
  DPARAM( m_nDivisionsK );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XLattice::DoCopy(const XBase* rhs)
{
  const XLattice* cb = rhs->HasFn<XLattice>();
  XMD_ASSERT(cb);

  m_nDivisionsI = cb->m_nDivisionsI;
  m_nDivisionsJ = cb->m_nDivisionsJ;
  m_nDivisionsK = cb->m_nDivisionsK;

  XGeometry::DoCopy(cb);
}
}
