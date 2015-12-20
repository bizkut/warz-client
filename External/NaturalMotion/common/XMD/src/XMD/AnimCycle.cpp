//----------------------------------------------------------------------------------------------------------------------
///  \file    AnimCycle.cpp
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/AnimCycle.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
NMTL_POD_VECTOR_EXPORT(XKeyFrame,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XAnimCurve*,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XAnimCurveSet*,XMD_EXPORT,XM2);

//----------------------------------------------------------------------------------------------------------------------
std::ostream& operator << (std::ostream& ofs,const XKeyFrame& kf)
{
  ofs << "{" <<kf.time<<","<<kf.value<<","<<kf.in<<","<<kf.out<<"}";
  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
XKeyFrame::XKeyFrame() 
  : time(0),value(0),in(0),out(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XKeyFrame::XKeyFrame(const XKeyFrame& kf)
  : time(kf.time),value(kf.value),in(kf.in),out(kf.out)
{
}

//----------------------------------------------------------------------------------------------------------------------
XKeyFrame::XKeyFrame(XReal t,XReal v,XReal i,XReal o)
  : time(t),value(v),in(i),out(o) 
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XKeyFrame::operator ==(const XKeyFrame& kf) const
{
  return XM2::float_equal(time,kf.time) &&
         XM2::float_equal(value,kf.value) &&
         XM2::float_equal(in,kf.in) &&
         XM2::float_equal(out,kf.out);
}


//----------------------------------------------------------------------------------------------------------------------
bool XKeyFrame::ReadChunk(std::istream& ifs)
{
  DUMPER(XModel__ReadChunk);
  READ_CHECK("time",ifs);
  ifs >> time;

  READ_CHECK("value",ifs);
  ifs >> value;

  READ_CHECK("in",ifs);
  ifs >> in;

  READ_CHECK("out",ifs);
  ifs >> out;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XKeyFrame::WriteChunk(std::ostream& ofs)
{
  DUMPER(XModel__WriteChunk);
  ofs << "\t\t\t\ttime " << time
    << " value " << value
    << " in " << in
    << " out " << out
    << "\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCurve::DoData(XFileIO& io) 
{
  DUMPER(XAnimCurve);

  CHECK_IO( io.DoDataString(m_Name) );
  DPARAM(m_Name);

  CHECK_IO( io.DoDataString(m_AttrName) );
  DPARAM(m_AttrName);

  CHECK_IO( io.DoDataVector(m_Keys) );
  DAPARAM(m_Keys);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimCurve::GetDataSize() const 
{
  return static_cast<XU32>(m_Name.size() + m_AttrName.size() + sizeof(XKeyFrame)*m_Keys.size() + 8);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimCurve::Evaluate(const XReal time) const
{
  DUMPER(XModel__Evaluate);
  XU32 sz = static_cast<XU32>(m_Keys.size());
  if(sz)
  {
    XKeyFrameList::const_iterator first_ = m_Keys.begin();

    if (sz==1||first_->time>=time)
    {
      return first_->value;
    }
    XKeyFrameList::const_iterator last_ = m_Keys.end()-1;

    if ( last_->time<=time) 
    {
      return last_->value;
    }
    XKeyFrameList::const_iterator second_ = first_+1;
    XKeyFrameList::const_iterator end_ = last_+1;

    for( ; second_ != end_ ; ++first_, ++second_ ) 
    {
      if ( (time>first_->time) && 
        (time<=second_->time) ) 
      {
        XReal len = second_->time - first_->time;
        XReal t = (time-first_->time)/len;
        XReal t2 = t*t;
        XReal t3 = t2 * t;

        XReal b0 =  2*t3 - 3*t2 + 1;
        XReal b1 = -2*t3 + 3*t2;
        XReal b2 = t3 - 2*t2 + t;
        XReal b3 = t3 - t2;

        return b0*first_->value +
          b1*second_->value +
          b2*first_->out +
          b3*second_->in;
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurve::XAnimCurve(const XBase* node,const XString& attr_name)
  : m_Name(node->GetName()),
  m_AttrName(attr_name),
  m_Keys()
{
  DUMPER(XAnimCurve__ctor);
  /// \todo look at the ramifications of what this does
  XString::iterator it = m_AttrName.begin();
  for( ; it != m_AttrName.end(); ++it )  
  {
    if (*it == '[' ||
      *it == ']' ||
      *it == '.') 
    {
      m_Name += '_';
    }
    else
    {
      m_Name += *it;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurve::XAnimCurve()
  : m_Name(), m_AttrName(), m_Keys()
{
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurve::~XAnimCurve()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCurve::AddKey(const XReal t,const XReal v,const XReal i,const XReal o)  
{
  m_Keys.push_back( XKeyFrame(t,v,i,o) );
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XAnimCurve::GetAttrName() const 
{
  return m_AttrName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
const XKeyFrameList& XAnimCurve::GetKeys() const
{
  return m_Keys;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XAnimCurve::GetName() const 
{
  return m_Name.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCurve::ReadChunk(std::istream& ifs)
{
  DUMPER(XAnimCurve__ReadChunk);

  READ_CHECK("ANIM_CURVE",ifs);

  ifs >> m_Name;  

  /// \todo look at the ramifications of this. Can it be moved 
  /// into a function if it's required
  for(XU32 i=0;i<m_Name.size();)
  {
    if (m_Name[i]=='\"' || m_Name[i] =='/' || m_Name[i] == '\\') 
    {
      m_Name.erase( m_Name.begin() + i );
    }
    else 
    {
      ++i;
    }
  }

  READ_CHECK("{",ifs);
  READ_CHECK("attr_name",ifs);

  ifs >> m_AttrName;

  READ_CHECK("num_keys",ifs);
  XU32 num_keys=0;
  ifs >> num_keys;
  m_Keys.resize(num_keys);

  XKeyFrameList::iterator it = m_Keys.begin();
  for( ; it != m_Keys.end(); ++it )
  {
    if(!it->ReadChunk(ifs))
      return false;
  }

  READ_CHECK("}",ifs);
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCurve::WriteChunk(std::ostream& ofs)
{
  DUMPER(XAnimCurve__WriteChunk);
  ofs << "\t\tANIM_CURVE " << m_Name.c_str() <<"\n\t\t{\n";
  ofs << "\t\t\tattr_name " << m_AttrName.c_str() << "\n";
  ofs << "\t\t\tnum_keys " << static_cast<XU32>(m_Keys.size()) << "\n";
  XKeyFrameList::iterator it = m_Keys.begin();
  for( ; it != m_Keys.end(); ++it )
  {
    if(!it->WriteChunk(ofs))
      return false;
  }
  ofs << "\t\t}\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurve::XAnimCurve(const XAnimCurve& rhs)
  : m_Name(rhs.m_Name),m_AttrName(rhs.m_AttrName),m_Keys(rhs.m_Keys)
{
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurveSet::XAnimCurveSet(const XAnimCurveSet& rhs)
  : m_Name(rhs.m_Name),m_NodeID(rhs.m_NodeID)
{
  resize(rhs.size());
  iterator it = begin();
  const_iterator itc = rhs.begin();
  for (;it != end();++it,++itc)
  {
    *it = new XAnimCurve( *(*itc) );
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCurveSet::DoData(XFileIO& io) 
{
  DUMPER(XAnimCurveSet);

  IO_CHECK( io.DoDataString(m_Name) );
  DPARAM( m_Name );

  IO_CHECK( io.DoData(&m_NodeID) );
  DPARAM( m_NodeID );

  XU32 nCurves = static_cast<XU32>(size());
  IO_CHECK( io.DoData(&nCurves) );
  DPARAM( nCurves );

  resize(nCurves);
  iterator it= begin();
  for (; it != end(); ++it) 
  {
    if (io.IsReading()) 
    {
      *it = new XAnimCurve();
    }
    (*it)->DoData(io);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimCurveSet::GetDataSize() const
{
  XU32 size = static_cast<XU32>(m_Name.size() + 2 + sizeof(XId) + 4);
  for( const_iterator it = begin(); it != end(); ++it ) 
  {
    size += (*it)->GetDataSize();
  }
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurveSet::XAnimCurveSet(XModel* mod)
  : XAnimCurveList(),
  m_NodeID(0),m_pModel(mod)
{  
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurveSet::XAnimCurveSet(XModel* mod,XId node)
  : XAnimCurveList(),
  m_NodeID(node),m_pModel(mod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurveSet::~XAnimCurveSet()
{
  iterator it = begin();
  for( ; it != end() ; ++it)
  {
    delete *it;
  }
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimCurve* XAnimCurveSet::GetCurve(XU32 idx) const 
{
  if (!m_pModel) 
  {
    return 0;
  }
  if (idx<GetNumCurves()) 
  {
    return (*this)[idx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurve* XAnimCurveSet::GetCurve(XU32 idx) 
{
  if (!m_pModel) 
  {
    return 0;
  }
  if (idx<GetNumCurves()) 
  {
    return (*this)[idx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XAnimCurveSet::GetNode() const 
{
  XMD_ASSERT(m_pModel);
  if(!m_pModel)
  {
    return 0;
  }
  return m_pModel->FindNode(m_NodeID);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimCurveSet::GetNumCurves() const
{
  return static_cast<XU32>(size());
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCurveSet::ReadChunk(std::istream& ifs) 
{

  READ_CHECK("node",ifs);
  ifs >> m_NodeID;

  XU32 num_curves=0;
  READ_CHECK("num_curves",ifs);
  ifs >> num_curves;

  resize(num_curves);

  iterator it = begin();
  for( ; it != end() ; ++it)
  {
    *it = new XAnimCurve();
    if( !(*it)->ReadChunk(ifs) ) 
    {
      return false;
    }
  }

  READ_CHECK("}",ifs);

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCurveSet::WriteChunk(std::ostream& ofs) 
{
  ofs << "\tANIM_CURVE_LIST " << m_Name.c_str() << "\n\t{\n";
  ofs << "\t\tnode " << m_NodeID << "\n";
  ofs << "\t\tnum_curves " << static_cast<XU32>(size()) << "\n";

  iterator it = begin();
  for( ; it != end() ; ++it)
  {
    if( !(*it)->WriteChunk(ofs) ) 
    {
      return false;
    }
  }
  ofs << "\t}\n";
  return ofs.good();

}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::SetToFrame(XU32 frame_index)
{
  // ignore out of range indices
  if (frame_index>=GetNumFrames())
    return;

  std::map<XId,XSampledKeys*>::iterator it = m_BoneAnims.begin();
  for (;it != m_BoneAnims.end(); ++it )
  {
    XId bone_id = it->first;
    XSampledKeys* skeys = it->second;

    XBase* node = GetModel()->FindNode(bone_id);
    if(node)
    {
      XBone* bone = node->HasFn<XBone>();
      if (bone)
      {
        bone->SetScale( skeys->ScaleKeys()[frame_index] );
        bone->SetTranslation( skeys->TranslationKeys()[frame_index] );
        bone->SetRotation( skeys->RotationKeys()[frame_index] );
      }
    }
  }
  GetModel()->BuildMatrices();
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::UpdateFrame(XU32 frame_index)
{
  std::map<XId,XSampledKeys*>::iterator it = m_BoneAnims.begin();
  for (;it != m_BoneAnims.end(); ++it )
  {
    XId bone_id = it->first;
    XSampledKeys* skeys = it->second;

    XBase* node = GetModel()->FindNode(bone_id);
    if(node)
    {
      XBone* bone = node->HasFn<XBone>();
      if (bone)
      {
        skeys->ScaleKeys()[frame_index] = bone->GetScale( false );
        skeys->TranslationKeys()[frame_index] = bone->GetTranslation( false );
        skeys->RotationKeys()[frame_index] = bone->GetRotation( false );
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCycle::DoData(XFileIO& io) 
{
  DUMPER(XAnimCycle);

  CHECK_IO( XBase::DoData(io) );

  CHECK_IO( io.DoData(&m_fStart) );
  DPARAM(m_fStart);

  CHECK_IO( io.DoData(&m_fEnd) );
  DPARAM(m_fEnd);

  CHECK_IO( io.DoData(&m_iNumFrames) );
  DPARAM(m_iNumFrames);

  CHECK_IO( io.DoData(&m_iFramesPerSecond) );
  DPARAM(m_iFramesPerSecond);

  XU32 num_curves=static_cast<XU32>(m_AnimCurves.size());
  CHECK_IO( io.DoData(&num_curves) );
  DPARAM(num_curves);

  if(num_curves)
  {
    if( io.IsReading() )
    {
      m_AnimCurves.resize(num_curves);
      XAnimCurveSetList::iterator it = m_AnimCurves.begin();
      for (;it != m_AnimCurves.end();++it) 
      {
        (*it)= new XAnimCurveSet(GetModel());
      }
    }
    XAnimCurveSetList::iterator it = m_AnimCurves.begin();
    for (;it != m_AnimCurves.end();++it) 
    {
      (*it)->DoData(io);
    }
  }

  XU32 sz = static_cast<XU32>(m_BoneAnims.size());
  CHECK_IO( io.DoData(&sz) );
  DPARAM(sz);

  if(io.IsReading()) 
  {
    for(XU32 i=0;i!=sz;++i)
    {
      XU32 id ;
      CHECK_IO( io.DoData(&id) );
      DPARAM(id);

      XSampledKeys* keys = new XSampledKeys;
      XMD_ASSERT(keys);
      CHECK_IO( keys->DoData(io) );

      m_BoneAnims.insert( std::make_pair( id, keys ) );
    }
  }
  else
  {
    std::map<XId,XSampledKeys*>::iterator it = m_BoneAnims.begin();
    for( ; it != m_BoneAnims.end(); ++it ) 
    {
      XU32 id = it->first;
      CHECK_IO( io.DoData(&id) );
      DPARAM(id);

      XSampledKeys* keys = it->second;
      CHECK_IO( keys->DoData(io) );
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::DoCopy(const XBase* rhs)
{
  const XAnimCycle* cb = rhs->HasFn<XAnimCycle>();
  XMD_ASSERT(cb);

  m_fStart = cb->m_fStart;
  m_fEnd = cb->m_fEnd;
  m_iNumFrames = cb->m_iNumFrames;
  m_iFramesPerSecond = cb->m_iFramesPerSecond;
  
  m_AnimCurves.resize(cb->m_AnimCurves.size());
  for (XU32 i=0;i!=cb->m_AnimCurves.size();++i)
  {
    m_AnimCurves[i] = new XAnimCurveSet( *cb->m_AnimCurves[i] );
  }

  std::map<XId,XSampledKeys*>::const_iterator it = cb->m_BoneAnims.begin();
  for( ; it != cb->m_BoneAnims.end(); ++it ) 
  {
    XSampledKeys* skeys = new XSampledKeys();
    XSampledKeys* scopy = it->second;
    skeys->m_RotationKeys = scopy->m_RotationKeys;
    skeys->m_TranslationKeys = scopy->m_TranslationKeys;
    skeys->m_ScaleKeys = scopy->m_ScaleKeys;

    m_BoneAnims.insert( std::make_pair( it->first, skeys ) );
  }

  XBase::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimCycle::GetDataSize() const 
{
  XU32 size = static_cast<XU32>(sizeof(XReal)*3 + XBase::GetDataSize());
  size += static_cast<XU32>(sizeof(XU32)*3);
  {
    std::map<XId,XSampledKeys*>::const_iterator it = m_BoneAnims.begin();
    for( ; it != m_BoneAnims.end(); ++it ) 
    {
      size += static_cast<XU32>(sizeof(XU32) + it->second->GetDataSize());
    }
  }
  {
    XAnimCurveSetList::const_iterator it = m_AnimCurves.begin();
    for( ; it != m_AnimCurves.end(); ++it ) 
    {
      size += (*it)->GetDataSize();
    }
  }
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimCycle::GetTimeInc() const 
{
  return 1.0f/GetFramesPerSecond();
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimCycle::GetFramesPerSecond() const 
{
  return m_iFramesPerSecond;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimCycle::GetNumFrames() const 
{
  return m_iNumFrames;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimCurveSet* XAnimCycle::GetAnimCurvesForNode(const XId node_id) const
{
  DUMPER(XAnimCycle__GetAnimCurvesForNode);
  XAnimCurveSetList::const_iterator it = m_AnimCurves.begin();
  for( ; it != m_AnimCurves.end(); ++it )
  {
    if( (*it)->GetNode()->GetID() == node_id ) 
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurveSet* XAnimCycle::GetAnimCurvesForNode(const XId node_id)
{
  DUMPER(XAnimCycle__GetAnimCurvesForNode);
  XAnimCurveSetList::iterator it = m_AnimCurves.begin();
  for( ; it != m_AnimCurves.end(); ++it )
  {
    if( (*it)->GetNode()->GetID() == node_id ) 
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XSampledKeys* XAnimCycle::GetBoneKeys(const XId bone_id) const
{
  DUMPER(XAnimCycle__GetBoneKeys);
  std::map<XId,XSampledKeys*>::const_iterator it = m_BoneAnims.find(bone_id);
  if (it != m_BoneAnims.end()) 
  {
    return it->second;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XSampledKeys* XAnimCycle::GetBoneKeys(const XId bone_id) 
{
  DUMPER(XAnimCycle__GetBoneKeys);
  std::map<XId,XSampledKeys*>::iterator it = m_BoneAnims.find(bone_id);
  if (it != m_BoneAnims.end()) 
  {
    return it->second;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XSampledKeys* XAnimCycle::GetBoneKeys(const XBase* bone) const
{
  DUMPER(XAnimCycle__GetBoneKeys);
  return bone ? GetBoneKeys( bone->GetID() ) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XSampledKeys* XAnimCycle::GetBoneKeys(const XBase* bone) 
{
  DUMPER(XAnimCycle__GetBoneKeys);
  return bone ? GetBoneKeys( bone->GetID() ) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCurve* XAnimCycle::AddAnimCurve(const XBase* node,const XString& attr_name)
{
  DUMPER(XAnimCycle__AddAnimCurve);

  bool valid = node && IsValidObj(node);

  // if you assert here, node is either NULL or not from this XModel
  XMD_ASSERT( valid );

  if(!valid)
    return 0;

  XAnimCurve* curve = new XAnimCurve(node,attr_name);

  XAnimCurveSetList::iterator it = m_AnimCurves.begin();
  XAnimCurveSetList::iterator end = m_AnimCurves.end();
  for (;it !=end;++it) 
  {
    XAnimCurveSet* aset = *it;
    if (aset && aset->GetNode() == node) 
    {
      for (XU32 i=0;i!=aset->GetNumCurves();++i)
      {
        const XAnimCurve* ac = aset->GetCurve(i);
        if (attr_name == ac->GetName())
        {
          // if you assert here, there is already an animation curve attached
          // to the node that matches attr_name
          XMD_ASSERT(0);
          delete curve;
          return 0;
        }
      }
      aset->push_back(curve);
      break;
    }
  }
  if(it == end)
  {
    XAnimCurveSet* aset = new XAnimCurveSet((XModel*)node->GetModel(),node->GetID());
    XMD_ASSERT(aset);
    aset->push_back(curve);      
    m_AnimCurves.push_back(aset);
  }

  return curve;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::DeleteAnimCurve(const XBase* node,const XString& attr_name)
{
  DUMPER(XAnimCycle__DeleteAnimCurve);
  XAnimCurveSetList::iterator it = m_AnimCurves.begin();
  for( ; it != m_AnimCurves.end(); ++it )
  {
    if( (*it)->GetNode()->GetID() == node->GetID() ) 
    {
      XAnimCurveSet* _set = *it;
      XAnimCurveSet::iterator itac = _set->begin();
      for (; itac != _set->end(); ++itac)
      {
        XAnimCurve* curve = *itac;
        if(attr_name == curve->GetAttrName())
        {
          _set->erase(itac);
          delete curve;
          return;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::DeleteAnimCurve(const XBase* node,XAnimCurve* curve)
{
  DUMPER(XAnimCycle__DeleteAnimCurve);
  XAnimCurveSetList::iterator it = m_AnimCurves.begin();
  for( ; it != m_AnimCurves.end(); ++it )
  {
    if( (*it)->GetNode()->GetID() == node->GetID() ) 
    {
      XAnimCurveSet* _set = *it;
      XAnimCurveSet::iterator itac = _set->begin();
      for (; itac != _set->end(); ++itac)
      {
        XAnimCurve* acurve = *itac;
        if( curve == acurve )
        {
          _set->erase(itac);
          delete acurve;
          return;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XSampledKeys* XAnimCycle::AddSampledKeys(const XId id)
{
  DUMPER(XAnimCycle__AddSampledKeys);
  XSampledKeys* keys = GetBoneKeys(id);
  if(!keys)
  { 
    keys = new XSampledKeys;
    m_BoneAnims.insert( std::make_pair(id,keys) );
  }
  return keys;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::SetFrameTimes(const XReal start_,const XReal end_,const XU32 nFrames)
{
  DUMPER(XAnimCycle__SetFrameTimes);
  m_fStart = start_;
  m_fEnd = end_;
  m_iNumFrames = nFrames;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::SetFrameRate(const XReal frame_rate)
{
  DUMPER(XAnimCycle__SetFrameTimes);
  m_iFramesPerSecond = frame_rate;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCycle::XAnimCycle(XModel* pmod)
  : XBase(pmod),m_iFramesPerSecond(24),m_BoneAnims()
{
    DUMPER(XAnimCycle__ctor);

    m_fStart=0;
    m_fEnd=0;
    m_iNumFrames=1;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCycle::~XAnimCycle()
{
  {
    DUMPER(XAnimCycle__dtor);
    std::map<XId,XSampledKeys*>::iterator it = m_BoneAnims.begin();
    for( ; it != m_BoneAnims.end(); ++it )
    {
      if( it->second )
        delete it->second;
    }
  }

  {
    XAnimCurveSetList::iterator it = m_AnimCurves.begin();
    for( ; it != m_AnimCurves.end(); ++it )
    {
      delete *it;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XAnimCycle::GetApiType() const
{
  return XFn::AnimCycle;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCycle::EvaluateAt(const XId bone,XTransform& out,const XReal t)
{
  DUMPER(XAnimCycle__EvaluateAt);
  std::map<XId,XSampledKeys*>::iterator it = m_BoneAnims.find(bone);
  if( it != m_BoneAnims.end() )
  {
    XSampledKeys* ps = it->second;

    if( t <= m_fStart ) 
    {
      out.scale = ps->ScaleKeys()[0];
      out.translate = ps->TranslationKeys()[0];
      out.rotate = ps->RotationKeys()[0];
    }
    else
    if( t >= m_fEnd )
    {
      XU32 i= static_cast<XU32>(ps->ScaleKeys().size())-1;
      out.scale = ps->ScaleKeys()[i];
      out.translate = ps->TranslationKeys()[i];
      out.rotate = ps->RotationKeys()[i];
    }
    else 
    {
      XReal tinc = (m_fEnd-m_fStart)/m_iNumFrames;
      XU32 start_frame = 0;
      XReal tt =  t - m_fStart;
      while( tt > 0 )
      {
        tt -= tinc;
        ++start_frame;
      }
      tt += tinc;

      XU32 _prev = start_frame-1;
      if(start_frame<ps->ScaleKeys().size())
      {
        out.scale.lerp(ps->ScaleKeys()[_prev],ps->ScaleKeys()[start_frame],tt);
        out.translate.lerp(ps->TranslationKeys()[_prev],ps->TranslationKeys()[start_frame],tt);
        out.rotate.slerp(ps->RotationKeys()[_prev],ps->RotationKeys()[start_frame],tt);
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::GetAnimatedNodes(XIndexList& nodes) const
{
  DUMPER(XAnimCycle__GetAnimatedNodes);
  nodes.clear();

  XAnimCurveSetList::const_iterator it = m_AnimCurves.begin();
  for( ; it != m_AnimCurves.end(); ++it )
  {
    nodes.push_back( (*it)->GetNode()->GetID() );
  }
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimCycle::GetEndTime() const 
{
  return m_fEnd;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::GetSampledNodes(XIndexList& nodes) const 
{
  DUMPER(XAnimCycle__GetSampledNodes);
  std::map<XId,XSampledKeys*>::const_iterator it= m_BoneAnims.begin();
  for( ; it != m_BoneAnims.end(); ++it )
  {
    nodes.push_back( it->first );
  }
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimCycle::GetStartTime() const
{
  return m_fStart;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAnimCycle::GetFn(XFn::Type type)
{
  if (GetApiType()==type) 
  {
    return (XAnimCycle*)this;
  }
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XAnimCycle::GetFn(XFn::Type type) const
{
  if(XFn::AnimCycle==type)
    return (const XAnimCycle*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::PreDelete(XIdSet& extra_nodes)
{
  DUMPER(XAnimCycle__PreDelete);
  XBase::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCycle::NodeDeath(XId id)
{
  DUMPER(XAnimCycle__NodeDeath);
  // remove any sampled animation for this node
  {
    std::map<XId,XSampledKeys*>::iterator it = m_BoneAnims.find(id);
    if (it != m_BoneAnims.end())
    {
      delete it->second;
      m_BoneAnims.erase(it);
    }
  }
  // remove any animation curves for the node
  {
    XAnimCurveSetList::iterator it = m_AnimCurves.begin();
    for( ; it != m_AnimCurves.end(); ++it )
    {
      if( (*it)->GetNode()->GetID() == id )
      {
        delete *it;
        m_AnimCurves.erase(it);
        break;
      }
    }
  }
  return XBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimCycle::RemapAnimNodes(std::map<XId,XId>& remap)
{
  DUMPER(XAnimCycle__RemapAnimNodes);
  {
    // remap all animation curves
    for( XU32 i=0; i < m_AnimCurves.size(); ++i)
    {
      std::map<XId,XId>::iterator it = remap.find( m_AnimCurves[i]->m_NodeID );
      if(it == remap.end())
      {
        delete m_AnimCurves[i];
        m_AnimCurves.erase( m_AnimCurves.begin()+i );
      }
      else 
      {
        m_AnimCurves[i]->m_NodeID = it->second;
      }
    }
  }

  {
    // the keyframes for the sampled bones
    std::map<XId,XSampledKeys*>::iterator it = m_BoneAnims.begin();
    std::map<XId,XSampledKeys*> NewMap;

    for( ; it != m_BoneAnims.end(); ++it )
    {
      std::map<XId,XId>::iterator itm = remap.find( it->first );

      if( itm == remap.end()) 
      {
        delete it->second;
        it->second=0;
      }
      else 
      {
        NewMap.insert( std::make_pair( itm->second, it->second) );
      }
    }
    m_BoneAnims = NewMap;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCycle::ReadChunk(std::istream& ifs)
{
  DUMPER(XAnimCycle__ReadChunk);
  XString buffer;
  READ_CHECK("num_bones",ifs);
  XU32 num_bones=0;
  ifs >> num_bones;

  READ_CHECK("start",ifs);
  ifs >> m_fStart;

  READ_CHECK("end",ifs);
  ifs >> m_fEnd;

  READ_CHECK("num_frames",ifs);
  ifs >> m_iNumFrames;

  READ_CHECK("fps",ifs);
  ifs >> m_iFramesPerSecond;

  for (XU32 i=0;i<m_Name.size();) 
  {
    if (m_Name[i]=='\"'||m_Name[i]=='/'||m_Name[i]=='\\') 
    {
      m_Name.erase( m_Name.begin() + i );
    }
    else ++i;
  }

  // write sampled animation data for each bone
  {
    for( ; num_bones>0; --num_bones )
    {
      XSampledKeys* keys = new XSampledKeys;
      XMD_ASSERT(keys);
      XId id =0;

      keys->SetSize(m_iNumFrames);

      READ_CHECK("BONE_ANIM",ifs);
      ifs >> id >> buffer;

      if(!keys->ReadChunk(ifs)) 
      {
        delete keys;
        return false;
      }
      m_BoneAnims.insert( std::make_pair( id, keys ) );
      READ_CHECK("}",ifs);
    }
  }

  XS32 num=1;
  buffer.resize(1);

  while(num) 
  {
    buffer[0]='\0';
    ifs >> buffer;
    if(ifs.fail() || ifs.eof())
      return false;

    if (buffer[0] == '}') 
    {
      --num;
    }
    else
    if (buffer[0] == '{') 
    {
      ++num;
    }
    else
    if( buffer == "ANIM_CURVE_LIST" )
    {
      XAnimCurveSet* pset = new XAnimCurveSet(m_pModel);
      XMD_ASSERT(pset);

      ifs >> buffer;
      
      // currently no api to specify name, so accept a nil entry if we read one in
      if(buffer != "{")
      {
        pset->m_Name = buffer;
        READ_CHECK("{",ifs);
      }

      if(!pset->ReadChunk(ifs)) 
      {
        delete pset;
        return false;
      }

      m_AnimCurves.push_back(pset);
    }
  }

  return ifs.good(); 
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimCycle::WriteChunk(std::ostream& ofs)
{
  DUMPER(XAnimCycle__WriteChunk);
  ofs << "ANIM_CYCLE \"" << m_Name.c_str() << "\"\n{\n";
  ofs << "\tnum_bones " << static_cast<XU32>(m_BoneAnims.size()) << "\n";
  ofs << "\tstart " << m_fStart << "\n";
  ofs << "\tend " << m_fEnd << "\n";
  ofs << "\tnum_frames " << m_iNumFrames << "\n";
  ofs << "\tfps " << m_iFramesPerSecond << "\n";


  // write sampled animation data for each bone
  {
    std::map<XId,XSampledKeys*>::iterator it = m_BoneAnims.begin();
    for( ; it != m_BoneAnims.end(); ++it )
    {
      ofs << "\tBONE_ANIM " << it->first << "\n\t{\n";
      if(!it->second->WriteChunk(ofs))
        return false;
      ofs << "\t}\n";
    }
  }

  // write out each animation curve
  {
    XAnimCurveSetList::iterator it = m_AnimCurves.begin();
    for( ; it != m_AnimCurves.end(); ++it )
    {
      if (!(*it)->WriteChunk(ofs))
        return false;
    }
  }
  ofs << "}\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XSampledKeys::XSampledKeys()
  : m_ScaleKeys(), m_TranslationKeys(), m_RotationKeys()
{
}

//----------------------------------------------------------------------------------------------------------------------
XSampledKeys::XSampledKeys(const XSampledKeys& sk)
  : m_ScaleKeys(sk.m_ScaleKeys), m_TranslationKeys(sk.m_TranslationKeys), 
  m_RotationKeys(sk.m_RotationKeys)
{
}

//----------------------------------------------------------------------------------------------------------------------
XSampledKeys::~XSampledKeys() 
{
  m_ScaleKeys.clear();
  m_RotationKeys.clear();
  m_TranslationKeys.clear();
}

//----------------------------------------------------------------------------------------------------------------------
XVector3Array& XSampledKeys::ScaleKeys()
{
  return m_ScaleKeys;
}

//----------------------------------------------------------------------------------------------------------------------
XVector3Array& XSampledKeys::TranslationKeys()
{
  return m_TranslationKeys;
}

//----------------------------------------------------------------------------------------------------------------------
XQuaternionArray& XSampledKeys::RotationKeys()
{
  return m_RotationKeys;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3Array& XSampledKeys::ScaleKeys() const
{
  return m_ScaleKeys;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3Array& XSampledKeys::TranslationKeys() const
{
  return m_TranslationKeys;
}

//----------------------------------------------------------------------------------------------------------------------
const XQuaternionArray& XSampledKeys::RotationKeys() const
{
  return m_RotationKeys;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledKeys::DoData(XFileIO& io) 
{
  DUMPER(XSampledKeys);

  CHECK_IO( io.DoDataVector(m_ScaleKeys) );
  DAPARAM(m_ScaleKeys);

  CHECK_IO( io.DoDataVector(m_TranslationKeys) );
  DAPARAM(m_TranslationKeys);

  CHECK_IO( io.DoDataVector(m_RotationKeys) );
  DAPARAM(m_RotationKeys);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSampledKeys::GetDataSize() const 
{
  return static_cast<XU32>(3*sizeof(XU32) + sizeof(XQuaternion) * m_RotationKeys.size() +
        3*sizeof(XReal) * (m_TranslationKeys.size() + m_ScaleKeys.size()));
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledKeys::ReadChunk(std::istream& ifs)
{
  DUMPER(XSampledKeys__ReadChunk);
  XVector3Array::iterator its    = m_ScaleKeys.begin();
  XVector3Array::iterator itt    = m_TranslationKeys.begin();
  XQuaternionArray::iterator itr = m_RotationKeys.begin();

  for( ; its != m_ScaleKeys.end(); ++its,++itt,++itr )
  {
    ifs >> *itr >> *itt >> *its;
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XSampledKeys::SetSize(XU32 sz) 
{
  DUMPER(XSampledKeys__SetSize);
  m_ScaleKeys.resize(sz,XVector3(1.0f,1.0f,1.0f));
  m_TranslationKeys.resize(sz);
  m_RotationKeys.resize(sz);
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledKeys::WriteChunk(std::ostream& ofs) 
{
  DUMPER(XSampledKeys__WriteChunk);

  XVector3Array::const_iterator its    = m_ScaleKeys.begin();
  XVector3Array::const_iterator itt    = m_TranslationKeys.begin();
  XQuaternionArray::const_iterator itr = m_RotationKeys.begin();

  for( ; its != m_ScaleKeys.end(); ++its,++itt,++itr )
  {
    ofs << "\t\t" << *itr << "\t" << *itt << "\t" << *its << "\n";
  }
  return ofs.good();
}
}
