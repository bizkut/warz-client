//----------------------------------------------------------------------------------------------------------------------
/// \file ParticleShape.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ParticleShape.h"
#include "XMD/Field.h"
#include "XMD/Emitter.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include "nmtl/algorithm.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::PostLoad() 
{
  XGeometry::PostLoad();

  // build list of deformers in the queue. First blend shapes, skinning, lattices
  XList objs;

  m_pModel->List(objs,XFn::Emitter);

  XList::iterator it = objs.begin();
  for( ; it != objs.end(); ++it )
  {
    XEmitter* pd = (*it)->HasFn<XEmitter>();

    if( pd && pd->m_Shape == GetID()) 
    {
      m_Emitters.push_back( pd->GetID() );
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XParticleShape::XParticleShape(XModel* pmod)
  : XGeometry(pmod),m_Emitters(),m_Fields(),
    mDepthSort(false),mLifespan(1.0f),mLifespanRandom(0.0f),
    mLifespanType(kLiveForever),mParticleType(kPoint),m_Material(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XParticleShape::~XParticleShape()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XParticleShape::AddEmitter(const XBase* ptr) 
{
  // if you assert here, an invalid emitter set has been specified
  XMD_ASSERT( (ptr) && IsValidObj(ptr) );
  if (ptr->HasFn<XEmitter>()) 
  {
    XIndexList::iterator it = m_Emitters.begin();
    for( ; it != m_Emitters.end(); ++it )
    {
      if (ptr->GetID()==*it)
      {
        return false;
      }
    }

    m_Emitters.push_back(ptr->GetID());

    return true;
  }

  // if you assert here, ptr does not point to an XEmitter derived class.
  XMD_ASSERT( 0 );
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XParticleShape::AddField(const XBase* ptr) 
{
  // if you assert here, an invalid field set has been specified
  XMD_ASSERT( (ptr) && IsValidObj(ptr) );
  if (ptr->HasFn<XField>()) 
  {
    XIndexList::iterator it = m_Fields.begin();
    for( ; it != m_Fields.end(); ++it )
    {
      if (ptr->GetID()==*it) 
      {
        return false;
      }
    }

    m_Fields.push_back(ptr->GetID());

    return true;
  }

  // if you assert here, ptr does not point to an XField derived class.
  XMD_ASSERT( 0 );
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XParticleShape::GetApiType() const
{
  return XFn::ParticleShape;
}

//----------------------------------------------------------------------------------------------------------------------
XEmitter* XParticleShape::GetEmitter(const XU32 idx) const 
{
  if(m_Emitters.size()>idx) 
  {
    XBase* ptr = m_pModel->FindNode(m_Emitters[idx]);
    if (ptr) 
    {
      return ptr->HasFn<XEmitter>();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XField* XParticleShape::GetField(const XU32 idx) const 
{
  if(m_Fields.size()>idx)
  {
    XBase* ptr = m_pModel->FindNode(m_Fields[idx]);
    if (ptr) 
    {
      return ptr->HasFn<XField>();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XParticleShape::GetNumEmitters() const 
{
  return static_cast<XU32>(m_Emitters.size());
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XParticleShape::GetNumFields() const 
{
  return static_cast<XU32>(m_Fields.size());
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XParticleShape::GetFn(XFn::Type type)
{
  if(XFn::ParticleShape==type)
    return (XParticleShape*)this;
  return XGeometry::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XParticleShape::GetFn(XFn::Type type) const
{
  if(XFn::ParticleShape==type)
    return (const XParticleShape*)this;
  return XGeometry::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParticleShape::NodeDeath(XId id)
{
  XIndexList::iterator it = m_Emitters.begin();
  for( ; it != m_Emitters.end(); ++it )
  {
    if( *it == id ) 
    {
      m_Emitters.erase(it);
      break;
    }
  }
  it = m_Fields.begin();
  for( ; it != m_Fields.end(); ++it )
  {
    if( *it == id )
    {
      m_Fields.erase(it);
      break;
    }
  }
  return XGeometry::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::PreDelete(XIdSet& extra_nodes)
{
  XGeometry::PreDelete(extra_nodes);

  const XDeletePolicy& dp = GetModel()->GetDeletePolicy();
  if ( dp.IsEnabled(XDeletePolicy::RelatedDynamics) ) 
  {
    std::map<XId,XU32> pUsage;

    XList particle_shapes;
    GetModel()->List(particle_shapes,XFn::ParticleShape);
    XList::iterator itp = particle_shapes.begin();
    for (; itp!=particle_shapes.end(); ++itp) 
    {
      XParticleShape* party = (*itp)->HasFn<XParticleShape>();
      XMD_ASSERT(party);

      if (party==this) 
      {
        continue;
      }

      for( XU32 i =0; i!= party->GetNumEmitters();++i)
      {
        XId idx = party->m_Emitters[i];

        std::map<XId,XU32>::iterator itu = pUsage.find(idx);
        if (itu != pUsage.end()) 
        {
          ++itu->second;
        }
        else
        {
          pUsage.insert(std::make_pair(idx,1));
        }          
      }

      for( XU32 i =0; i!= party->GetNumFields();++i)
      {
        XId idx = party->m_Fields[i];

        std::map<XId,XU32>::iterator itu = pUsage.find(idx);
        if (itu != pUsage.end()) 
        {
          ++itu->second;
        }
        else
        {
          pUsage.insert(std::make_pair(idx,1));
        }          
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XMaterial* XParticleShape::GetMaterial()
{
  XBase* mat = m_pModel->FindNode(m_Material);
  return mat ? mat->HasFn<XMaterial>() : 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::SetMaterial(XMaterial* material)
{
  if (material)
    m_Material = material->GetID();
  else
    m_Material=0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XParticleShape::ReadChunk(std::istream& ifs)
{
  if (!XGeometry::ReadChunk(ifs))
    return false;

  XS32 num;
  READ_CHECK("fields",ifs);
  ifs >> num;

  m_Fields.resize(num);

  XIndexList::iterator it = m_Fields.begin();
  for( ; it != m_Fields.end(); ++it )
  {
    ifs >> *it;
  }

  if (GetModel()->GetInputVersion()>=4)
  {
    XString temp;
    READ_CHECK("depth_sort",ifs);
    ifs >> mDepthSort;

    READ_CHECK("lifespan_type",ifs);
    ifs >> temp;
    if (temp=="CONSTANT")
      mLifespanType = kConstant;
    else
    if (temp=="RANDOM")
      mLifespanType = kRandom;
    else
    if (temp=="LIVE_FOREVER")
      mLifespanType = kLiveForever;
    else
    {
      XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,
          "Unknown lifespan type when loading XParticleShape \"%s\". Defaulting to kLiveForever",GetName());
      mLifespanType = kLiveForever;
    }

    READ_CHECK("lifespan",ifs);
    ifs >> mLifespan;

    READ_CHECK("lifespan_random",ifs);
    ifs >> mLifespanRandom;

    READ_CHECK("particle_type",ifs);
    ifs >> temp;
    //ifs >> temp;
    if (temp=="POINT_SPRITES")
      mParticleType = kPointSprites;
    else
    if (temp=="STREAK")
      mParticleType = kStreak;
    else
    if (temp=="SPHERE")
      mParticleType = kSphere;
    else
    if (temp=="MULTI_POINT")
      mParticleType = kMultiPoint;
    else
    if (temp=="MULTI_STREAK")
      mParticleType = kMultiStreak;
    else
    if (temp=="SPRITES")
      mParticleType = kSprites;
    else
    if (temp=="NUMERIC")
      mParticleType = kNumeric;
    else
    if (temp=="POINT"||temp=="POINTS")
      mParticleType = kPoint;
    else
    {
      XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,
          "Unknown lifespan type when loading XParticleShape \"%s\". Defaulting to kPoint",GetName());
      mParticleType = kPoint;
    }
    READ_CHECK("material",ifs);
    ifs >> m_Material;
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XParticleShape::WriteChunk(std::ostream& ofs)
{
  if (!XGeometry::WriteChunk(ofs))
    return false;

  ofs << "\tfields " << static_cast<XU32>(m_Fields.size()) << "\n";

  XIndexList::iterator it = m_Fields.begin();
  for( ; it != m_Fields.end(); ++it )
  {
    ofs << "\t\t" << *it << "\n";
  }

  ofs << "\tdepth_sort " << mDepthSort << "\n";
  ofs << "\tlifespan_type ";

  switch(mLifespanType)
  {
  case kConstant: ofs << "CONSTANT\n"; break;
  case kRandom: ofs << "RANDOM\n"; break;
  case kLiveForever: 
  default: ofs << "LIVE_FOREVER\n"; break;
  }

  ofs << "\tlifespan " << mLifespan << "\n";
  ofs << "\tlifespan_random " << mLifespanRandom << "\n";
  ofs << "\tparticle_type ";

  switch(mParticleType)
  {
  case kPointSprites: ofs << "POINT_SPRITES\n"; break;
  case kStreak: ofs << "STREAK\n"; break;
  case kSphere: ofs << "SPHERE\n"; break;
  case kMultiPoint: ofs << "MULTI_POINT\n"; break;
  case kMultiStreak: ofs << "MULTI_STREAK\n"; break;
  case kSprites: ofs << "SPRITES\n"; break;
  case kNumeric: ofs << "NUMERIC\n"; break;
  case kPoint: 
  default: ofs << "POINT\n"; break;
  }

  ofs << "\t material " << m_Material << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XParticleShape::DoData(XFileIO& io) 
{
  DUMPER(XParticleShape);

  IO_CHECK( XGeometry::DoData(io) );

  IO_CHECK( io.DoDataVector(m_Fields) );
  DAPARAM( m_Fields );

  if (GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( io.DoData(&mDepthSort) );
    IO_CHECK( io.DoData(&mLifespanType) );
    IO_CHECK( io.DoData(&mLifespan) );
    IO_CHECK( io.DoData(&mLifespanRandom) );
    IO_CHECK( io.DoData(&mParticleType) );
    if (GetModel()->GetInputVersion()>=5)
    {
      IO_CHECK( io.DoData(&m_Material) );
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XParticleShape::GetDataSize() const
{
  size_t sz = XGeometry::GetDataSize(); // V3
  sz += sizeof(XU32) + sizeof(XId)*m_Fields.size();

  sz += sizeof(bool); // mDepthSort
  sz += sizeof(XLifespanType); // mLifespanType
  sz += sizeof(XReal); // mLifespan
  sz += sizeof(XReal); // mLifespanRandom
  sz += sizeof(XParticleType); // mParticleType

  /// \todo This needs to be fixed for XMD version 5. 
  #if 0
  sz += sizeof(XId); // m_Material
  #endif
  return (XU32)sz;
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::DoCopy(const XBase* rhs)
{
  const XParticleShape* cb = rhs->HasFn<XParticleShape>();
  XMD_ASSERT(cb);

  // if duplicating all particle fields
  if (GetModel()->GetClonePolicy().IsEnabled(XClonePolicy::DuplicateParticleShapes))
  {
    // duplicate one by one and add to back of fields
    for(XU32 i=0;i!=cb->GetNumFields();++i)
    {
      XField* field = cb->GetField(i);
      XField* newfield = field->Clone()->HasFn<XField>();
      m_Fields.push_back(newfield->GetID());
    }
  }
  else
  {
    // just re-use the ones assigned to the original particle shape
    m_Fields = cb->m_Fields;
  }

  mDepthSort = cb->mDepthSort;
  mLifespanType = cb->mLifespanType;
  mLifespan = cb->mLifespan;
  mLifespanRandom = cb->mLifespanRandom;
  mParticleType = cb->mParticleType;
  m_Material = cb->m_Material;

  XGeometry::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParticleShape::RemoveEmitter(const XBase* ptr)
{
  bool valid = ptr && IsValidObj(ptr);

  // if you assert here, then ptr is either null, or you have attempted 
  // to remove a node that does not belong to the same XModel as this
  // particle shape.
  XMD_ASSERT(valid);

  if(valid)
  {
    XIndexList::iterator it = nmtl::find(m_Emitters.begin(),m_Emitters.end(),ptr->GetID());
    if (it != m_Emitters.end())
    {
      m_Emitters.erase(it);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::GetEmitters(XList& emitters) const
{
  emitters.clear();
  XIndexList::const_iterator it = m_Emitters.begin();
  for (;it != m_Emitters.end();++it)
  {
    XBase* base = m_pModel->FindNode(*it);
    if (base)
    {
      emitters.push_back(base);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::SetEmitters(const XList& emitters)
{
  m_Emitters.clear();
  XList::const_iterator it = emitters.begin();
  for (;it != emitters.end();++it)
  {
    XBase* b = *it;
    if (b && IsValidObj(b))
    {
      m_Emitters.push_back(b->GetID());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XParticleShape::RemoveField(const XBase* ptr)
{
  bool valid = ptr && IsValidObj(ptr);

  // if you assert here, then ptr is either null, or you have attempted 
  // to remove a node that does not belong to the same XModel as this
  // particle shape.
  XMD_ASSERT(valid);

  if(valid)
  {
    XIndexList::iterator it = nmtl::find(m_Fields.begin(),m_Fields.end(),ptr->GetID());
    if (it != m_Fields.end())
    {
      m_Fields.erase(it);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::GetFields(XList& fields) const
{
  fields.clear();
  XIndexList::const_iterator it = m_Fields.begin();
  for (;it != m_Fields.end();++it)
  {
    XBase* base = m_pModel->FindNode(*it);
    if (base)
    {
      fields.push_back(base);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::SetFields(const XList& fields)
{
  m_Fields.clear();
  XList::const_iterator it = fields.begin();
  for (;it != fields.end();++it)
  {
    XBase* b = *it;
    if (b && IsValidObj(b))
    {
      m_Fields.push_back(b->GetID());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::SetDepthSort(const bool f)
{
  mDepthSort = f;
}

//----------------------------------------------------------------------------------------------------------------------
bool XParticleShape::GetDepthSort() const
{
  return mDepthSort;
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::SetLifespan(const XReal f)
{
  mLifespan = f;
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::SetLifespanRandom(const XReal f)
{
  mLifespanRandom = f;
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::SetLifespanType(const XLifespanType f)
{
  mLifespanType = f;
}

//----------------------------------------------------------------------------------------------------------------------
void XParticleShape::SetParticleType(const XParticleType f)
{
  mParticleType = f;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XParticleShape::GetLifespan() const
{
  return mLifespan;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XParticleShape::GetLifespanRandom() const
{
  return mLifespanRandom;
}

//----------------------------------------------------------------------------------------------------------------------
XParticleShape::XLifespanType XParticleShape::GetLifespanType() const
{
  return mLifespanType;
}

//----------------------------------------------------------------------------------------------------------------------
XParticleShape::XParticleType XParticleShape::GetParticleType() const
{
  return mParticleType;
}
}
