//----------------------------------------------------------------------------------------------------------------------
/// \file   BlendShapeOptimiser.cpp
/// \date   11-9-2004
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines routines to take an input geometry object, and some blend shape targets. It then 
///         proceeds to reduce these down to a set of targets defined by offsets of only those vertices that 
///         actually change.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMU/BlendShapeOptimiser.h"
using namespace XMD;

namespace XMU  
{
NMTL_POD_VECTOR_EXPORT(XMUOffset,XMD_EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
XMUOffset::XMUOffset() 
  : index(0),x(0),y(0),z(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUOffset::XMUOffset(XM2::XU32 i,XM2::XReal x_,XM2::XReal y_,XM2::XReal z_) 
  : index(i),x(x_),y(y_),z(z_) 
{
} 

//----------------------------------------------------------------------------------------------------------------------
XMUOffset::XMUOffset(const XMUOffset& o) 
  : index(o.index),x(o.x),y(o.y),z(o.z) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUOffsetList::XMUOffsetList() 
  : XM2::pod_vector<XMUOffset>() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUOffsetList::XMUOffsetList(const XMUOffsetList& ol)
  : XM2::pod_vector<XMUOffset>(ol) 
{
}

//----------------------------------------------------------------------------------------------------------------------
void XMUOffsetList::SumInfluence(XVector3Array& points,XM2::XReal w) const
{
  //  if(w<-0.0001f || w>0.0001f)
  {
    const_iterator it = begin();
    for( ; it != end(); ++it )
    {
      XM2::XVector3& fl = points[ it->index ];
      fl.x += (w * it->x);  
      fl.y += (w * it->y);  
      fl.z += (w * it->z);  
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XMUBlendShapeOptimiser::XMUBlendShapeOptimiser()
  :  XM2::vector<XMUOffsetList>(),
  m_aWeights(),
  m_aDeformedVertices(),
  m_aOrigVertices()
{
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XRealArray& XMUBlendShapeOptimiser::Weights() const 
{
  return m_aWeights;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XVector3Array& XMUBlendShapeOptimiser::DeformedVertices() const
{
  return m_aDeformedVertices;
}

//----------------------------------------------------------------------------------------------------------------------
const XM2::XVector3Array& XMUBlendShapeOptimiser::OriginalVertices() const 
{
  return m_aOrigVertices;
}

//----------------------------------------------------------------------------------------------------------------------
void XMUBlendShapeOptimiser::AddTarget(const XM2::XVector3Array &points) 
{
  XMUOffsetList new_target;
  XM2::XVector3Array::iterator it = m_aOrigVertices.begin();
  XM2::XVector3Array::const_iterator itp = points.begin();
  for( XU32 idx=0; points.end() != itp; ++it,++itp,++idx )
  {
    XM2::XVector3 diff(itp->x,itp->y,itp->z);
    diff.x -= it->x; diff.y -= it->y; diff.z -= it->z;

    // if offset has a measurable effect
    if ( diff.length() > 0.05f)
    {
      new_target.push_back( XMUOffset(idx,diff.x,diff.y,diff.z) );
    }
  }
  m_aWeights.push_back(0.0f);
  push_back(new_target);  
}

//----------------------------------------------------------------------------------------------------------------------
void XMUBlendShapeOptimiser::SetPoints(const XM2::XVector3Array &points) 
{
  // clear any targets created using different points
  resize(0);

  m_aDeformedVertices.resize(points.size());
  m_aOrigVertices.resize(points.size());

  XM2::XVector3Array::iterator itdp = m_aDeformedVertices.begin();
  XM2::XVector3Array::iterator itop = m_aOrigVertices.begin();
  XM2::XVector3Array::const_iterator it = points.begin();

  for( ; itop != m_aOrigVertices.end(); ++itop,++itdp,++it )
  {
    itop->x = itdp->x = it->x;
    itop->y = itdp->y = it->y;
    itop->z = itdp->z = it->z;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUBlendShapeOptimiser::SetPoints(const XM2::XU32 num,XM2::XReal* points) 
{
  // clear any targets created using different points
  resize(0);

  m_aDeformedVertices.resize(num);
  m_aOrigVertices.resize(num);

  XM2::XVector3Array::iterator itdp = m_aDeformedVertices.begin();
  XM2::XVector3Array::iterator itop = m_aOrigVertices.begin();

  for( ; itop != m_aOrigVertices.end(); ++itop,++itdp )
  {
    itop->x = itdp->x = *points;  ++points;
    itop->y = itdp->y = *points;  ++points;
    itop->z = itdp->z = *points;  ++points;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUBlendShapeOptimiser::NewTarget(const XM2::XReal* points,const XM2::XU32 stride,const XM2::XU32 offset)
{
  XMUOffsetList new_target;

  XM2::XVector3Array::iterator it = m_aOrigVertices.begin();

  XM2::XU8* _begin = (XM2::XU8*)((void*)points);
  XM2::XU8* _start = _begin + offset;
  XM2::XU8* _end = _start + stride*m_aOrigVertices.size();
  XM2::XU8* _it = _start;
  

  for( XM2::XU32 idx=0; _it != _end; _it+=stride, ++idx )
  {  
    XM2::XReal* point_data = (XM2::XReal*)((void*)_it);
    XM2::XReal dx = point_data[0] - it->x;
    XM2::XReal dy = point_data[1] - it->y;
    XM2::XReal dz = point_data[2] - it->z;

    // if offset has a measurable effect
    if ( (dx*dx+dy*dy+dz*dz) > 0.005f) 
    {
      new_target.push_back( XMUOffset(idx,dx,dy,dz) );
    }
  }
  m_aWeights.push_back(0.0f);

  push_back(new_target);
}

//----------------------------------------------------------------------------------------------------------------------
const XMUOffsetList& XMUBlendShapeOptimiser::GetBlendTarget(const XM2::XU32 Target) const 
{
  // an invalid index has been specified
  XMD_ASSERT(Target<size());
  return at(Target);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMUBlendShapeOptimiser::GetNumTargets() const 
{
  return static_cast<XM2::XU32>(size());
}

//-------------------------------------------------------------------  XMUBlendShapeOptimiser :: GetWeights
//
void XMUBlendShapeOptimiser::GetWeights(XM2::XRealArray& weights)
{
  weights = m_aWeights;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMUBlendShapeOptimiser::SetWeights(const XM2::XRealArray& wieghts) 
{
  if(wieghts.size()!=m_aWeights.size())
    return false;
  m_aWeights = wieghts;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XMUBlendShapeOptimiser::GetDeformedPoints(XM2::XVector3Array& defp)
{
  EvalDeformedPoints();

  //defp.resize(m_aDeformedVertices.size());

  XM2::XVector3Array::iterator itdp = m_aDeformedVertices.begin();

  XM2::XVector3Array::iterator it = defp.begin();
  for( ; it != defp.end(); ++it,++itdp )
  {
    it->x = itdp->x;
    it->y = itdp->y;
    it->z = itdp->z;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUBlendShapeOptimiser::GetDeformedPoints(XM2::XRealArray &defp)
{
  EvalDeformedPoints();

  //defp.resize(3*m_aDeformedVertices.size());

  XM2::XVector3Array::iterator itdp = m_aDeformedVertices.begin();

  XM2::XRealArray::iterator it = defp.begin();
  for( ; it != defp.end(); ++itdp )
  {
    *it = itdp->x; ++it;
    *it = itdp->y; ++it;
    *it = itdp->z; ++it;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUBlendShapeOptimiser::GetDeformedPoints(XM2::XReal *defp)
{
  EvalDeformedPoints();

  XM2::XVector3Array::iterator itdp = m_aDeformedVertices.begin();

  XM2::XReal* _end = defp + m_aDeformedVertices.size()*3;
  for( ; defp != _end; ++defp )
  {
    *defp = itdp->x; ++defp;
    *defp = itdp->y; ++defp;
    *defp = itdp->z; ++defp;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUBlendShapeOptimiser::EvalDeformedPoints()
{
  // copy original into deformed array
  m_aDeformedVertices = m_aOrigVertices;

  XM2::XRealArray::iterator itw = m_aWeights.begin();

  // sum target influences
  iterator it = begin();
  for( ; it != end(); ++it, ++itw )
  {
    if(*itw>0.01f)
      it->SumInfluence( m_aDeformedVertices, *itw );
  }
}
}
