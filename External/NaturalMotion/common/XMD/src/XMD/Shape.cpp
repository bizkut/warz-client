//----------------------------------------------------------------------------------------------------------------------
/// \file Shape.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Shape.h"
#include "XMD/Model.h"
#include "XMD/Geometry.h"
#include "XMD/FileIO.h"
#include "nmtl/algorithm.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
void XShape::PostLoad()
{
  XInstanceList::iterator iti = m_pModel->m_Instances.begin();
  for( ; iti != m_pModel->m_Instances.end(); ++iti )
  {
    if((*iti)->GetShape())
    {
      if( (*iti)->GetShape()->GetID() == GetID() )
      {
        m_Instances.push_back( (*iti)->GetID() );
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XShape::XShape(XModel* pmod)
  : XBase(pmod),m_Instances()
{
}

//----------------------------------------------------------------------------------------------------------------------
XShape::~XShape()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XShape::GetApiType() const
{
  return XFn::Shape;
}

//----------------------------------------------------------------------------------------------------------------------
bool XShape::DoData(XFileIO& io)
{
  IO_CHECK( XBase::DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XShape::GetDataSize() const
{
  return XBase::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XShape::GetObjectInstances(XInstanceList& il) const
{
  il.clear();
  XInstanceList instances;
  m_pModel->GetObjectInstances(instances);


  XIndexList temp = m_Instances;
  nmtl::sort(temp.begin(),temp.end());

  XInstanceList::const_iterator it = instances.begin();
  for( ; it != instances.end(); ++it )
  {
    XIndexList::const_iterator itt = m_Instances.begin();
    for( ; itt != m_Instances.end(); ++itt) 
    {
      if ( *itt == (*it)->GetID() ) 
      {
        il.push_back(*it);
        break;
      }
    }
  }

  return il.size()!=0 ? true : false;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XShape::GetFn(XFn::Type type)
{
  if(XFn::Shape==type)
    return (XShape*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XShape::GetFn(XFn::Type type) const
{
  if(XFn::Shape==type)
    return (const XShape*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XShape::PreDelete(XIdSet& extra_nodes)
{
  XBase::PreDelete(extra_nodes);

  const XDeletePolicy& dp = GetModel()->GetDeletePolicy();


  if( dp.IsEnabled(XDeletePolicy::ShapesDeleteTransform) )
  {
    XInstanceList instances;
    GetObjectInstances(instances);
    XInstanceList::iterator it = instances.begin();
    for (; it != instances.end(); ++it) 
    {
      XInstance* inst = *it;

      XBone* bone = inst->GetParent();
      if(bone->ApiType()==XFn::Bone)
      {
        extra_nodes.insert(bone->GetID());
      }
    }
  }

  // delete all assigned materials (assuming they are not used too much anymore)
  if ( dp.IsEnabled(XDeletePolicy::RelatedMaterials) ) 
  {
     XInstanceList instances;
     GetObjectInstances(instances);
     XInstanceList::iterator it = instances.begin();
     for (; it != instances.end(); ++it) 
     {
       XInstance* inst = *it;

       XU32 num = inst->GetNumMaterialGroups();
       if (num) 
       {
         for (XU32 i=0;i!=num;++i) 
         {
           XMaterial* mat = inst->GetMaterial(i);
           XInstanceList assignedGeometries;
           mat->GetObjectInstances(assignedGeometries);
           if (assignedGeometries.size() <= 1) 
           {
             extra_nodes.insert(mat->GetID());
           }
         }
       }
       else
       {
         XMaterial* mat = inst->GetMaterial();
         if(mat)
         {
           XInstanceList assignedGeometries;
           mat->GetObjectInstances(assignedGeometries);
           if (assignedGeometries.size() <= 1) 
           {
             extra_nodes.insert(mat->GetID());
           }
         }
       }
     }
  }  
}

//----------------------------------------------------------------------------------------------------------------------
bool XShape::NodeDeath(XId id)
{
  XIndexList::iterator it = m_Instances.begin();
  for( ; it != m_Instances.end(); ++it )
  {
    XInstance* pI = m_pModel->GetInstance(*it);
    if( pI && pI->GetParent()->GetID() == id )
    {
      m_pModel->DeleteInstance( *it );
      m_Instances.erase(it);
      return m_Instances.size() != 0;
    }
  }
  return XBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XShape::ReadChunk(std::istream& ifs)
{
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XShape::WriteChunk(std::ostream& ofs)
{
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XShape::DoCopy(const XBase* rhs)
{
  const XShape* cb = rhs->HasFn<XShape>();
  XMD_ASSERT(cb);

  // Instances of a shape are not duplicated by default. This only 
  // happens when you duplicate a bone with instances underneath it.

  XBase::DoCopy(cb);
}
}

