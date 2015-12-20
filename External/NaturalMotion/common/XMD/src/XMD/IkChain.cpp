//----------------------------------------------------------------------------------------------------------------------
/// \file RenderLayer.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Model.h"
#include "XMD/IkChain.h"
#include "XMD/FileIO.h"
#include "nmtl/algorithm.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XIkChain::XIkChain(XModel* pModel)
  : XBase(pModel),m_Handle(0),m_Priority(1),m_MaxIterations(2147483647),
  m_Weight(1.0f),m_PoWeight(1.0f),m_Tolerance(static_cast<XReal>(1e-05)),m_Stickiness(0),m_PoleVector(),
  m_PoleConstraint(0),m_IkChain()
{
}

//----------------------------------------------------------------------------------------------------------------------
XIkChain::~XIkChain()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XIkChain::GetApiType() const 
{
  return XFn::IkChain;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XIkChain::GetDataSize() const 
{
  XU32 sz = XBase::GetDataSize();
  sz += sizeof(XId)*2;
  sz += sizeof(XS32);
  sz += sizeof(XU32)*2;
  sz += sizeof(XReal)*4;
  sz += 3*sizeof(XReal);
  sz += sizeof(XId)*static_cast<XU32>(m_IkChain.size());
  return sz;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XIkChain::GetNumBonesInChain() const
{
  return static_cast<XU32>(m_IkChain.size());
}

//----------------------------------------------------------------------------------------------------------------------
XBone* XIkChain::GetBoneInChain(const XU32 idx) const
{
  if(idx< GetNumBonesInChain() )
  {
    XBase* ptr = m_pModel->FindNode(m_IkChain[idx]);
    if(ptr)
    {
      XBone* pBone = ptr->HasFn<XBone>();
      return pBone;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XIkChain::AddBoneToChain(const XBone* bone)
{
  // asserted because ptr is null or not from this XModel
  XMD_ASSERT( (bone) && IsValidObj(bone) );
  if(!bone)
    return false;

  // make sure not already part of the IK chain.
  if (m_IkChain.end() != nmtl::find(m_IkChain.begin(),m_IkChain.end(),bone->GetID()))
  {
    return false;
  }

  m_IkChain.push_back(bone->GetID());
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XIkChain::RemoveBone(const XBone* bone)
{
  if (bone && IsValidObj(bone))
  {
    XIndexList::iterator it = nmtl::find(m_IkChain.begin(),m_IkChain.end(),bone->GetID());
    if(it!=m_IkChain.end())
    {
      m_IkChain.erase(it);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XBone* XIkChain::GetHandle() const
{
  XBase* ptr = m_pModel->FindNode(m_Handle);
  if(ptr)
  {
    XBone* pBone = ptr->HasFn<XBone>();
    return pBone;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XIkChain::GetPoleVector() const
{
  return m_PoleVector;
}

//----------------------------------------------------------------------------------------------------------------------
XBone* XIkChain::GetPoleConstraint() const
{
  XBase* base = m_pModel->FindNode(m_PoleConstraint);
  if (base)
  {
    return base->HasFn<XBone>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XIkChain::GetPriority() const
{
  return m_Priority;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XIkChain::GetMaxIterations() const
{
  return m_MaxIterations;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XIkChain::GetWeight() const
{
  return m_Weight;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XIkChain::GetPoWeight() const
{
  return m_PoWeight;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XIkChain::GetTolerance() const
{
  return m_Tolerance;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XIkChain::GetStickiness() const
{
  return m_Stickiness;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XIkChain::GetFn(XFn::Type type) 
{
  if (XFn::IkChain == type) 
  {
    return (XIkChain*)this;
  }
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XIkChain::GetFn(XFn::Type type) const
{
  if(XFn::IkChain==type)
    return (const XIkChain*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XIkChain::NodeDeath(XId id)
{
  XIndexList::iterator it = m_IkChain.begin();
  for( ; it != m_IkChain.end(); ++it )
  {
    if (*it == id)
    {
      m_IkChain.erase(it);
      break;
    }
  }
  if(m_PoleConstraint == id)
    m_PoleConstraint = 0;
  if(m_Handle == id)
    m_Handle=0;

  return XBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XIkChain::PreDelete(XIdSet& extra_nodes)
{
  XBase::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XIkChain::ReadChunk(std::istream& ifs) 
{
  READ_CHECK("handle",ifs);
  ifs >> m_Handle;

  READ_CHECK("priority",ifs);
  ifs >> m_Priority;

  READ_CHECK("max_iterations",ifs);
  ifs >> m_MaxIterations;

  READ_CHECK("weight",ifs);
  ifs >> m_Weight;

  READ_CHECK("poweight",ifs);
  ifs >> m_PoWeight;

  READ_CHECK("tolerance",ifs);
  ifs >> m_Tolerance;

  READ_CHECK("stickiness",ifs);
  ifs >> m_Stickiness;

  READ_CHECK("pole_vector",ifs);
  ifs >> m_PoleVector;

  READ_CHECK("pole_constraint",ifs);
  ifs >> m_PoleConstraint;

  READ_CHECK("chain",ifs);

  XU32 num;

  ifs >> num;
  m_IkChain.resize(num);

  XIndexList::iterator it = m_IkChain.begin();
  for( ; it != m_IkChain.end(); ++it ) 
  {
      ifs >> *it;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XIkChain::WriteChunk(std::ostream& ofs)
{
  ofs << "\thandle " << m_Handle << "\n"
    << "\tpriority " << m_Priority << "\n"
    << "\tmax_iterations " << m_MaxIterations << "\n"
    << "\tweight " << m_Weight
    << "\tpoweight " << m_PoWeight
    << "\ttolerance " << m_Tolerance
    << "\tstickiness " << m_Stickiness
    << "\tpole_vector " << m_PoleVector.x << " " << m_PoleVector.y << " " << m_PoleVector.z << "\n"
    << "\tpole_constraint " << m_PoleConstraint
    << "\tchain " << GetNumBonesInChain() << "\n\t\t";

  XIndexList::iterator it = m_IkChain.begin();
  for( ; it != m_IkChain.end(); ++it )
  {
      ofs << *it << " ";
  }
  ofs << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XIkChain::DoData(XFileIO& io) 
{
  DUMPER(XIkChain);

  IO_CHECK( XBase::DoData(io) );

  IO_CHECK( io.DoData(&m_Handle) );
  DPARAM( m_Handle );

  IO_CHECK( io.DoData(&m_Priority) );
  DPARAM( m_Priority );

  IO_CHECK( io.DoData(&m_MaxIterations) );
  DPARAM( m_MaxIterations );

  IO_CHECK( io.DoData(&m_Weight) );
  DPARAM( m_Weight );

  IO_CHECK( io.DoData(&m_PoWeight) );
  DPARAM( m_PoWeight );

  IO_CHECK( io.DoData(&m_Tolerance) );
  DPARAM( m_Tolerance );

  IO_CHECK( io.DoData(&m_Stickiness) );
  DPARAM( m_Stickiness );

  IO_CHECK( io.DoData(&m_PoleVector) );
  DPARAM( m_PoleVector );

  IO_CHECK( io.DoData(&m_PoleConstraint) );
  DPARAM( m_PoleConstraint );

  IO_CHECK( io.DoDataVector(m_IkChain) );
  DAPARAM( m_IkChain );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XIkChain::DoCopy(const XBase* rhs)
{
  const XIkChain* cb = rhs->HasFn<XIkChain>();
  XMD_ASSERT(cb);
  m_Handle = cb->m_Handle;
  m_Priority = cb->m_Priority;
  m_MaxIterations = cb->m_MaxIterations;
  m_Weight = cb->m_Weight;
  m_PoWeight = cb->m_PoWeight;
  m_Tolerance = cb->m_Tolerance;
  m_Stickiness = cb->m_Stickiness;
  m_PoleVector = cb->m_PoleVector;
  m_PoleConstraint = cb->m_PoleConstraint;
  m_IkChain = cb->m_IkChain;
  XBase::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XIkChain::SetBonesInChain(const XBoneList& bones)
{
  // run a check. The bones should form a continuous chain where the next
  // bone in the chain is the child of the one before. To make this test
  // easier (and to avoid a recursive function), start at the end of the 
  // chain and make sure that the element before it is actually the parent 
  // of that bone
  // 
  {
    XBoneList::const_iterator it = bones.end()-1;
    XBoneList::const_iterator it2 = bones.end()-2;
    for (;it2 >= bones.begin(); --it,--it2 )
    {
      //
      // if you assert here, the node in the chain does not belong to this 
      // model. 
      // 
      XMD_ASSERT( IsValidObj(*it) );

      // make sure the node is valid
      if(!IsValidObj(*it))
        return false;

      // make sure the bones are continuous
      if ((*it)->GetParent() != *it2)
      {
        return false;
      }
    }
  }

  // OK, the list is a valid chain, lets set it
  m_IkChain.clear();
  XBoneList::const_iterator it = bones.begin();
  for (;it != bones.end(); ++it)
  {
    m_IkChain.push_back( (*it)->GetID() );
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XIkChain::SetBonesInChain(const XBone* ik_root,const XBone* ik_effector)
{
  if( !ik_root || !ik_effector )
    return false;

  bool root_valid = IsValidObj(ik_root);

  // if you assert here, the ik root is not a node from the same XModel
  // as this IK  chain.
  XMD_ASSERT(root_valid);

  bool effector_valid = IsValidObj(ik_effector);

  // if you assert here, the ik effector is not a node from the same XModel
  // as this IK  chain.
  XMD_ASSERT(effector_valid);

  if(!root_valid || !effector_valid) return false;

  // construct a list of bones by working from the effector to the 
  // ik root.
  XIndexList chain;
  const XBone* ptr = ik_effector;
  bool found_root = false;
  while(ptr && !found_root)
  {
    chain.insert(chain.begin(),ptr->GetID());
    if (ptr->GetID() == ik_root->GetID())
    {
      found_root=true;
    }
    ptr = ptr->GetParent();
  }

  if (!found_root)
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError,"could not determine a valid chain between the root and effector");
    return false;
  }
  
  m_IkChain = chain;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XIkChain::GetBonesInChain(XBoneList& bones) const
{
  bones.clear();
  XIndexList::const_iterator it = m_IkChain.begin();
  for (;it != m_IkChain.end(); ++it)
  {
    bones.push_back( m_pModel->FindNode( *it )->HasFn<XBone>() );
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XIkChain::SetPoleVector(const XVector3& pole_vector)
{
  m_PoleVector = pole_vector;
}

//----------------------------------------------------------------------------------------------------------------------
bool XIkChain::SetPoleConstraint(const XBase* transform)
{
  if(!transform)
  {
    m_PoleConstraint = 0;
    return true;
  }
  bool is_valid = IsValidObj(transform);

  // if you assert here, the pole constraint provided is not from the same
  // XModel as this ik chain...
  XMD_ASSERT(is_valid);

  if(!is_valid)
    return false;

  if (transform->HasFn<XBone>())
  {
    m_PoleConstraint = transform->GetID();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XIkChain::SetHandle(const XBase* transform)
{
  if(!transform)
  {
    m_Handle = 0;
    return true;
  }
  bool is_valid = IsValidObj(transform);

  // if you assert here, the pole constraint provided is not from the same
  // XModel as this ik chain...
  XMD_ASSERT(is_valid);

  if(!is_valid)
    return false;

  if (transform->HasFn<XBone>())
  {
    m_Handle = transform->GetID();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XIkChain::SetPriority(const XS32 priority)
{
  m_Priority = priority;
}

//----------------------------------------------------------------------------------------------------------------------
void XIkChain::SetMaxIterations(const XU32 num_iterations)
{
  m_MaxIterations = num_iterations;
}

//----------------------------------------------------------------------------------------------------------------------
void XIkChain::SetWeight(const XReal weight)
{
  m_Weight = weight;
}

//----------------------------------------------------------------------------------------------------------------------
void XIkChain::SetPoWeight(const XReal po_weight)
{
  m_PoWeight = po_weight;
}

//----------------------------------------------------------------------------------------------------------------------
void XIkChain::SetTolerance(const XReal tolerance)
{
  m_Tolerance = tolerance;
}

//----------------------------------------------------------------------------------------------------------------------
void XIkChain::SetStickiness(const XReal stickiness)
{
  m_Stickiness = stickiness;
}
}
