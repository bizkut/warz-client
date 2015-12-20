//----------------------------------------------------------------------------------------------------------------------
/// \file Constraint.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Constraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XConstraint::XConstraint(XModel* pmod) 
  : XBone(pmod),m_Constrained(0),m_Targets(),m_AttrNames(),m_Weights()
{
}

//----------------------------------------------------------------------------------------------------------------------
XConstraint::~XConstraint()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XConstraint::GetApiType() const
{
  return XFn::Constraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XConstraint::GetFn(XFn::Type type)
{
  if(XFn::Constraint==type)
    return (XConstraint*)this;
  return XBone::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XConstraint::GetFn(XFn::Type type) const
{
  if(XFn::Constraint==type)
    return (const XConstraint*)this;
  return XBone::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::NodeDeath(XId id)
{
  for (XU32 i=0;i<m_Targets.size();) 
  {
    if(m_Targets[i]==id)
    {
      m_Targets.erase( m_Targets.begin()+i );
      m_AttrNames.erase( m_AttrNames.begin()+i );
      m_Weights.erase( m_Weights.begin()+i );
    }
    else
    {
      ++i;
    }
  }
  if (m_Constrained == id) 
  {
    m_Constrained=0;
  }

  return XBone::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XConstraint::PreDelete(XIdSet& extra_nodes)
{
  XBone::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::ReadChunk(std::istream& ifs)
{  
  if (!XBone::ReadChunk(ifs)) 
    return false;

  READ_CHECK("affected",ifs);
  ifs >> m_Constrained;
  DPARAM(m_Constrained);

  XU32 num_targets;
  READ_CHECK("num_targets",ifs);
  ifs >> num_targets;
  DPARAM(num_targets);

  m_Targets.resize(num_targets);
  m_AttrNames.resize(num_targets);
  m_Weights.resize(num_targets);
  for (XU32 i=0;i!=num_targets;++i) 
  {
    ifs >> m_Targets[i] >> m_AttrNames[i] >> m_Weights[i];
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XBone::WriteChunk(ofs))
    return false;

  ofs << "\taffected " << m_Constrained << "\n";
  ofs << "\tnum_targets " << GetNumTargets() << "\n";

  for(XU32 j=0;j<GetNumTargets();++j) 
  {
    ofs << "\t\t" << m_Targets[j] 
      << " " << m_AttrNames[j].c_str()
      << " " << m_Weights[j] << std::endl;
  }

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XConstraint::DoCopy(const XBase* rhs)
{
  const XConstraint* cb = rhs->HasFn<XConstraint>();
  XMD_ASSERT(cb);
  m_Targets = cb->m_Targets;
  m_AttrNames = cb->m_AttrNames;
  m_Weights = cb->m_Weights;
  m_Constrained = cb->m_Constrained;
  XBone::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::DoData(XFileIO& io)
{
  DUMPER(XConstraint);

  IO_CHECK( XBone::DoData(io) );

  IO_CHECK( io.DoData(&m_Constrained) );
  IO_CHECK( io.DoDataVector(m_Targets) );
  IO_CHECK( io.DoDataVector(m_Weights) );

  XU32 num=static_cast<XU32>(m_AttrNames.size());
  IO_CHECK( io.DoData(&num) );

  if (io.IsReading()) 
  {
    m_AttrNames.resize(num);
  }

  for (XU32 i=0;i!=num;++i) 
  {
    io.DoDataString( m_AttrNames[i] );
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XConstraint::GetDataSize() const 
{
  size_t sz = 4*sizeof(XU32) + m_Targets.size()*sizeof(XU32) + m_Weights.size()*sizeof(XReal) + XBone::GetDataSize();

  for (XU32 i=0;i!=m_AttrNames.size();++i) 
  {
    sz += m_AttrNames[i].size() + sizeof(XU16);
  }
  return static_cast<XU32>(sz);
}

//----------------------------------------------------------------------------------------------------------------------
const XIndexList& XConstraint::Targets() const
{ 
  return m_Targets;
}

//----------------------------------------------------------------------------------------------------------------------
const XRealArray& XConstraint::Weights() const
{ 
  return m_Weights; 
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XConstraint::GetNumTargets() const 
{
  return static_cast<XU32>(m_Targets.size());
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XConstraint::GetTarget(const XU32 target_index) const
{
  if (target_index<GetNumTargets()) 
  {
    XBase* base = m_pModel->FindNode(m_Targets[target_index]);
    return base;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XConstraint::GetTargetWeight(const XU32 target_index) const
{
  if (target_index<GetNumTargets()) 
  {
    return m_Weights[target_index];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::AddTarget(const XBone* bone, const XReal weight)
{
  // if you assert here, bone is either null or is not from this
  // XModel.
  XMD_ASSERT( (bone) && IsValidObj(bone) );

  if (!bone) 
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError,"XConstraint::AddTarget - null target provided to function" );
    return false;
  }

  XIndexList::iterator it = m_Targets.begin();
  for (;it != m_Targets.end(); ++it) 
  {
    if (*it == bone->GetID()) 
    {
      // already a target
      XGlobal::GetLogger()->Log(XBasicLogger::kWarning,"XConstraint::AddTarget - target provided is already a target" );
      return false;
    }
  }
  m_Targets.push_back(bone->GetID());
  m_Weights.push_back(weight);
  return  true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::RemoveTarget(const XU32 target_index)
{
  if (target_index<GetNumTargets()) 
  {
    m_Targets.erase(m_Targets.begin()+target_index);
    m_Weights.erase(m_Weights.begin()+target_index);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::SetTargetWeight(const XU32 target_index,const XReal weight)
{
  if (target_index<GetNumTargets()) 
  {
    m_Weights[target_index] = weight;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XConstraint::GetConstrained() const
{
  return m_pModel->FindNode(m_Constrained);
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::SetConstrained(const XBase* base)
{
  if(base)
  {
    // if you assert here, base is either null or is not from this
    // XModel.
    XMD_ASSERT( IsValidObj(base) );

    const XBone* bone = base->HasFn<XBone>();
    // if you assert here, you have not provided an XBone to be constrained.
    XMD_ASSERT( bone );
    if(bone)
    {
      m_Constrained = bone->GetID();
    }
  }
  else
  {
    m_Constrained = 0;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XConstraint::GetTargets(XBoneList& targets) const
{
  targets.clear();
  for (XU32 i=0;i!=GetNumTargets();++i)
  {
    targets.push_back( GetTarget(i)->HasFn<XBone>() );
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::SetTargets(const XBoneList& targets)
{
  // make sure all targets are valid
  for (XU32 i=0;i!=targets.size();++i)
  {
    if(!IsValidObj(targets[i]))
    {
      return false;
    }
  }

  // generate a map of the old bones to old weights
  std::map<XBase*,XReal> wmap;
  for (XU32 i=0;i!=GetNumTargets();++i)
  {
    wmap.insert( std::make_pair( GetTarget(i), GetTargetWeight(i)) );
  }

  m_Targets.resize(targets.size());
  m_Weights.resize(targets.size());

  for (XU32 i=0;i!=targets.size();++i)
  {
    XMD_ASSERT(targets[i]);
    m_Targets[i] = targets[i]->GetID();

    std::map<XBase*,XReal>::iterator itw = wmap.find( targets[i] );
    if (itw == wmap.end())
    {
      m_Weights[i] = 0.0f;
    }
    else
    {
      m_Weights[i] = itw->second;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::SetTargets(const XBoneList& targets,const XRealArray& weights)
{
  if(targets.size() != weights.size() )
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "XConstraint::SetTargets(): target and weight arrays do not match");
    return false;
  }

  // make sure all targets are valid
  for (XU32 i=0;i!=targets.size();++i)
  {
    if(!IsValidObj(targets[i]))
    {
      return false;
    }
  }

  m_Targets.resize(targets.size());
  m_Weights = weights;

  for (XU32 i=0;i!=targets.size();++i)
  {
    XMD_ASSERT(targets[i]);
    m_Targets[i] = targets[i]->GetID();
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XConstraint::SetWeights(const XRealArray& weights)
{
  if(m_Targets.size() != weights.size() )
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "XConstraint::SetWeights():  weights array does not match number of targets");
    return false;
  }

  m_Weights = weights;

  return true;
}
}
