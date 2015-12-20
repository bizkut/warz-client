//----------------------------------------------------------------------------------------------------------------------
/// \file   KeyStripper.cpp
/// \date   11-9-2004
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file provides some re-usable template objects to strip rdeundent key frames from sampled animation 
///         data. As you may be aware, i seem to always utilise CPU intensive brute force techniques for all of this
///         stuff. If anyone has suggestions for faster processing i'm all ears. (i'm just getting lazy due to the 
///         luxury of a fast work pc)
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMU/KeyStripper.h"
#include "XMD/AnimCycle.h"
#include "XMD/AnimationTake.h"
#include "XMD/FCurveStream.h"
#include "XMD/SampledStream.h"
#include "XMD/PoseStream.h"
#include "XMD/Bone.h"
#include "XMD/Model.h"
#include <iostream>
#include <intrin.h>

/// loader utils namespace
namespace XMU  
{
// export vector template to DLL
NMTL_POD_VECTOR_EXPORT(XMUKeyFrames*,XM2EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XMUStrippedNodeAnim*,XM2EXPORT,XM2);

//----------------------------------------------------------------------------------------------------------------------
XU32 XMUKeyFrames::GetNumKeys() const
{
  return (XU32)m_times.size();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMUKeyFrames::GetAttribute() const
{
  return m_attributeID;
}

//----------------------------------------------------------------------------------------------------------------------
XMUKeyFrames::XMUKeyFrames(XU32 attribute,XU32 numFrames,XReal startTime,XReal endTime)
  : m_attributeID(attribute),m_times()
{
  m_times.reserve(numFrames);
  XReal sc_ = (endTime-startTime)/(numFrames-1);
  for (XU32 i=0;i!=numFrames;++i)
  {
    m_times.push_back((XReal)i*sc_);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUKeyFrames::RemoveKey(XU32 index)
{
  m_times.erase(m_times.begin()+index);
}

//----------------------------------------------------------------------------------------------------------------------
XMUFloatKeyFrames::XMUFloatKeyFrames(const XU32 attribute,const XRealArray& keys,XReal startTime,XReal endTime)
  : XMUKeyFrames(attribute,(XU32)keys.size(),startTime,endTime), m_values(keys)
{
}

//----------------------------------------------------------------------------------------------------------------------
void XMUFloatKeyFrames::GetKey(const XU32 index,XReal& time,XReal& value) const
{
  if (index<GetNumKeys())
  {
    time  = m_times[index];
    value = m_values[index];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUFloatKeyFrames::Strip(const XReal eps)
{
  for (size_t i=0;i<(m_values.size()-2);)
  {
    XReal v0 = m_values[i+0];
    XReal v1 = m_values[i+1];
    XReal v2 = m_values[i+2];
    XReal diff0 = (v1-v0) / (m_times[i+1]-m_times[i+0]);
    XReal diff1 = (v2-v1) / (m_times[i+2]-m_times[i+1]);
    if (XM2::float_equal(diff0,diff1,eps))
    {
      RemoveKey((XU32)(i+1));
      m_values.erase(m_values.begin()+i+1);
    }
    else
      ++i;
  }

  // run a test where there are only 2 left, if the same, remove the last key
  if (m_values.size()==2)
  {
    if(XM2::float_equal(m_values[0],m_values[1],eps))
    {
      m_times.pop_back();
      m_values.pop_back();
    }
  }

  // force a resize on the array
  m_values.reserve(m_values.size());
}

//----------------------------------------------------------------------------------------------------------------------
XMUVector2KeyFrames::XMUVector2KeyFrames(const XU32 attribute,const XVector2Array& keys,XReal startTime,XReal endTime)
  : XMUKeyFrames(attribute,(XU32)keys.size(),startTime,endTime), m_values(keys)
{
}

//----------------------------------------------------------------------------------------------------------------------
void XMUVector2KeyFrames::GetKey(const XU32 index,XReal& time,XVector2& value) const
{
  if (index<GetNumKeys())
  {
    time  = m_times[index];
    value = m_values[index];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUVector2KeyFrames::Strip(const XReal eps)
{
  XVector2 diff0,diff1;
  for (size_t i=0;i<(m_values.size()-2);)
  {
    const XVector2& v0 = m_values[i+0];
    const XVector2& v1 = m_values[i+1];
    const XVector2& v2 = m_values[i+2];
    diff0.sub(v1,v0);
    diff1.sub(v2,v1);
    diff0.div(m_times[i+1]-m_times[i+0]);
    diff1.div(m_times[i+2]-m_times[i+1]);
    if (diff0.equal(diff1,eps))
    {
      RemoveKey((XU32)(i+1));
      m_values.erase(m_values.begin()+i+1);
    }
    else
      ++i;
  }

  // run a test where there are only 2 left, if the same, remove the last key
  if (m_values.size()==2)
  {
    if(m_values[0].equal(m_values[1],eps))
    {
      m_times.pop_back();
      m_values.pop_back();
    }
  }

  // force a resize on the array
  m_values.reserve(m_values.size());
}

//----------------------------------------------------------------------------------------------------------------------
XMUVector3KeyFrames::XMUVector3KeyFrames(const XU32 attribute,const XVector3Array& keys,XReal startTime,XReal endTime)
  : XMUKeyFrames(attribute,(XU32)keys.size(),startTime,endTime), m_values(keys)
{
}
//----------------------------------------------------------------------------------------------------------------------
void XMUVector3KeyFrames::GetKey(const XU32 index,XReal& time,XVector3& value) const
{
  if (index<GetNumKeys())
  {
    time  = m_times[index];
    value = m_values[index];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUVector3KeyFrames::Strip(const XReal eps)
{
  XVector3 diff0,diff1;
  for (size_t i=0;i<(m_values.size()-2);)
  {
    const XVector3& v0 = m_values[i+0];
    const XVector3& v1 = m_values[i+1];
    const XVector3& v2 = m_values[i+2];
    diff0.sub(v1,v0);
    diff1.sub(v2,v1);
    diff0.div(m_times[i+1]-m_times[i+0]);
    diff1.div(m_times[i+2]-m_times[i+1]);
    if (diff0.equal(diff1,eps))
    {
      RemoveKey((XU32)(i+1));
      m_values.erase(m_values.begin()+i+1);
    }
    else
      ++i;
  }

  // run a test where there are only 2 left, if the same, remove the last key
  if (m_values.size()==2)
  {
    if(m_values[0].equal(m_values[1],eps))
    {
      m_times.pop_back();
      m_values.pop_back();
    }
  }

  // force a resize on the array
  m_values.reserve(m_values.size());
}

//----------------------------------------------------------------------------------------------------------------------
XMUVector4KeyFrames::XMUVector4KeyFrames(const XU32 attribute,const XVector4Array& keys,XReal startTime,XReal endTime)
  : XMUKeyFrames(attribute,(XU32)keys.size(),startTime,endTime), m_values(keys)
{
}

//----------------------------------------------------------------------------------------------------------------------
void XMUVector4KeyFrames::GetKey(const XU32 index,XReal& time,XVector4& value) const
{
  if (index<GetNumKeys())
  {
    time  = m_times[index];
    value = m_values[index];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUVector4KeyFrames::Strip(const XReal eps)
{
  XVector4 diff0,diff1;
  for (size_t i=0;i<(m_values.size()-2);)
  {
    const XVector4& v0 = m_values[i+0];
    const XVector4& v1 = m_values[i+1];
    const XVector4& v2 = m_values[i+2];
    diff0.sub(v1,v0);
    diff1.sub(v2,v1);
    diff0.div(m_times[i+1]-m_times[i+0]);
    diff1.div(m_times[i+2]-m_times[i+1]);
    if (diff0.equal(diff1,eps))
    {
      RemoveKey((XU32)(i+1));
      m_values.erase(m_values.begin()+i+1);
    }
    else
      ++i;
  }

  // run a test where there are only 2 left, if the same, remove the last key
  if (m_values.size()==2)
  {
    if(m_values[0].equal(m_values[1],eps))
    {
      m_times.pop_back();
      m_values.pop_back();
    }
  }

  // force a resize on the array
  m_values.reserve(m_values.size());
}

//----------------------------------------------------------------------------------------------------------------------
XMUQuaternionKeyFrames::XMUQuaternionKeyFrames(const XU32 attribute,const XQuaternionArray& keys,XReal startTime,XReal endTime)
  : XMUKeyFrames(attribute,(XU32)keys.size(),startTime,endTime), m_values(keys)
{
}

//----------------------------------------------------------------------------------------------------------------------
void XMUQuaternionKeyFrames::GetKey(const XU32 index,XReal& time,XQuaternion& value) const
{
  if (index<GetNumKeys())
  {
    time  = m_times[index];
    value = m_values[index];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMUQuaternionKeyFrames::Strip(const XReal eps)
{
  XQuaternion diff;
  for (size_t i=0;i<(m_values.size()-2);)
  {
    const XQuaternion& v0 = m_values[i+0];
    const XQuaternion& v1 = m_values[i+1];
    const XQuaternion& v2 = m_values[i+2];

    XReal t_ = (m_times[i+1]-m_times[i+0]) / (m_times[i+2]-m_times[i+0]);
    diff.slerp(v0,v2,t_);

    if ( diff.equivalent(v1,eps) )
    {
      RemoveKey( (XU32)(i+1) );
      m_values.erase(m_values.begin()+i+1);
    }
    else
      ++i;
  }

  // run a test where there are only 2 left, if the same, remove the last key
  if (m_values.size()==2)
  {
    if(m_values[0].equal(m_values[1],eps))
    {
      m_times.pop_back();
      m_values.pop_back();
    }
  }

  // force a resize on the array
  m_times.reserve(m_times.size());
  m_values.reserve(m_values.size());
}

//----------------------------------------------------------------------------------------------------------------------
XMUStrippedNodeAnim::XMUStrippedNodeAnim(XBase* node,XSampledKeys* keyframes,XReal start,XReal end)
: m_node(node),m_keyedAttributes()
{
  XM2_ASSERT(node);
  XM2_ASSERT(keyframes);
  if(node && keyframes)
  {
    XMD::XBone* bone = node->HasFn<XMD::XBone>();
    XM2_ASSERT(bone);
    if(bone)
    {
      XMUQuaternionKeyFrames* rot_keys = new XMUQuaternionKeyFrames(XMD::XBone::kRotation,keyframes->RotationKeys(),start,end);
      m_keyedAttributes.push_back( rot_keys );

      XMUVector3KeyFrames* trn_keys = new XMUVector3KeyFrames(XMD::XBone::kTranslation,keyframes->TranslationKeys(),start,end);
      m_keyedAttributes.push_back( trn_keys );

      XMUVector3KeyFrames* scl_keys = new XMUVector3KeyFrames(XMD::XBone::kScale,keyframes->ScaleKeys(),start,end);
      m_keyedAttributes.push_back( scl_keys );
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XMUStrippedNodeAnim::GetNode()
{
  return m_node;
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XMUStrippedNodeAnim::GetNode() const
{
  return m_node;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMUStrippedNodeAnim::GetNumAttributes() const
{
  return (XU32)m_keyedAttributes.size();
}

//----------------------------------------------------------------------------------------------------------------------
const XMUKeyFrames* XMUStrippedNodeAnim::GetAttribute(XM2::XU32 index) const
{
  return (index<GetNumAttributes()) ? m_keyedAttributes[index] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XMUKeyFrames* XMUStrippedNodeAnim::GetAttributeFromID(XM2::XU32 attribute_id) const
{
  XM2::pod_vector< XMUKeyFrames* >::const_iterator it = m_keyedAttributes.begin();
  for (;it != m_keyedAttributes.end();++it)
  {
    if((*it)->GetAttribute()==attribute_id)
      return *it;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XMUStrippedNodeAnim::Strip(const XReal tolerance)
{
  XM2::pod_vector< XMUKeyFrames* >::iterator it = m_keyedAttributes.begin();
  for (;it != m_keyedAttributes.end();++it)
  {
    (*it)->Strip(tolerance);
  }  
}

//----------------------------------------------------------------------------------------------------------------------
XMUStrippedNodeAnim::XMUStrippedNodeAnim(XBase* node,XAnimatedNode* keyframes,XReal start,XReal end)
: m_node(node),m_keyedAttributes()
{
  XM2_ASSERT(node);
  XM2_ASSERT(keyframes);
  if(node && keyframes)
  {
    XU32 num_properties = keyframes->GetNumAttributes();
    m_keyedAttributes.reserve(num_properties);
    for (XU32 i=0;i<num_properties;++i)
    {
      XAnimatedAttribute* attr = keyframes->GetAttribute(i);
      if (attr)
      {
        XSampledStream* samples = attr->GetSampledStream();
        if (samples)
        {
          switch(samples->GetStreamDataType())
          {
          case XAnimationStream::kReal:
            {
              XRealArray vars;
              XU32 num_elements = samples->GetNumElemtents();
              vars.reserve(num_elements);
              for (XU32 j=0;j!=num_elements;++j)
              {
                vars.push_back(samples->AsReal(j));
              }
              XMUFloatKeyFrames* keys_ = new XMUFloatKeyFrames(attr->GetPropertyID(),vars,start,end);
              m_keyedAttributes.push_back( keys_ );
            }
            break;
          case XAnimationStream::kVector2:
            {
              XVector2Array vars;
              XU32 num_elements = samples->GetNumElemtents();
              vars.reserve(num_elements);
              for (XU32 j=0;j!=num_elements;++j)
              {
                vars.push_back(samples->AsVector2(j));
              }
              XMUVector2KeyFrames* keys_ = new XMUVector2KeyFrames(attr->GetPropertyID(),vars,start,end);
              m_keyedAttributes.push_back( keys_ );
            }
            break;
          case XAnimationStream::kVector3:
            {
              XVector3Array vars;
              XU32 num_elements = samples->GetNumElemtents();
              vars.reserve(num_elements);
              for (XU32 j=0;j!=num_elements;++j)
              {
                vars.push_back(samples->AsVector3(j));
              }
              XMUVector3KeyFrames* keys_ = new XMUVector3KeyFrames(attr->GetPropertyID(),vars,start,end);
              m_keyedAttributes.push_back( keys_ );
            }
            break;
          case XAnimationStream::kVector4:
            {
              XVector4Array vars;
              XU32 num_elements = samples->GetNumElemtents();
              vars.reserve(num_elements);
              for (XU32 j=0;j!=num_elements;++j)
              {
                vars.push_back(samples->AsVector4(j));
              }
              XMUVector4KeyFrames* keys_ = new XMUVector4KeyFrames(attr->GetPropertyID(),vars,start,end);
              m_keyedAttributes.push_back( keys_ );
            }
            break;
          case XAnimationStream::kQuaternion:
            {
              XQuaternionArray vars;
              XU32 num_elements = samples->GetNumElemtents();
              vars.reserve(num_elements);
              for (XU32 j=0;j!=num_elements;++j)
              {
                vars.push_back(samples->AsQuaternion(j));
              }
              XMUQuaternionKeyFrames* keys_ = new XMUQuaternionKeyFrames(attr->GetPropertyID(),vars,start,end);
              m_keyedAttributes.push_back( keys_ );
            }
            break;
          default:
            /// un-supported
            break;
          }
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XMUStrippedAnimTake::XMUStrippedAnimTake(XAnimationTake* take)
  : m_anim(take),m_nodeAnims()
{
  XReal start = take->GetStartTime();
  XReal end = take->GetEndTime();

  XU32 num = take->GetNumberOfAnimatedNodes();
  m_nodeAnims.reserve(num);
  for (XU32 i=0;i!=num;++i)
  {
    XAnimatedNode* anode = take->GetNodeAnimation(i);
    if (anode)
    {
      m_nodeAnims.push_back(new XMUStrippedNodeAnim(anode->GetNode(),anode,start,end));
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XMUStrippedAnimTake::XMUStrippedAnimTake(XAnimCycle* cycle)
  : m_anim(cycle),m_nodeAnims()
{
  XReal start = cycle->GetStartTime();
  XReal end = cycle->GetEndTime();
  XMD::XIndexList nodes;
  cycle->GetSampledNodes(nodes);

  m_nodeAnims.reserve(nodes.size());
  for (XU32 i=0;i!=(XU32)nodes.size();++i)
  {
    XBase* node = cycle->GetModel()->FindNode(nodes[i]);
    if(node)
    {
      m_nodeAnims.push_back(new XMUStrippedNodeAnim(node,cycle->GetBoneKeys(node),start,end));
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XMUStrippedAnimTake::~XMUStrippedAnimTake()
{
  XM2::pod_vector<XMUStrippedNodeAnim*>::iterator it = m_nodeAnims.begin();
  for (;it != m_nodeAnims.end();++it)
  {
    delete *it;
  }
  m_nodeAnims.reserve(0);
}

//----------------------------------------------------------------------------------------------------------------------
void XMUStrippedAnimTake::Strip(const XReal tolerance)
{
  int n = (int)m_nodeAnims.size();

  #if XM2_USE_OMP
  # pragma omp parallel for schedule(dynamic)
  #endif
  for (int i=0;i<n;++i)
  {
    m_nodeAnims[i]->Strip(tolerance);
  }  
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMUStrippedAnimTake::GetNumNodes() const
{
  return (XU32)m_nodeAnims.size();
}

//----------------------------------------------------------------------------------------------------------------------
const XMUStrippedNodeAnim* XMUStrippedAnimTake::GetAnimatedNode(const XU32 node) const
{
  return node<GetNumNodes() ? m_nodeAnims[node] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XMUStrippedAnimTake::GetAnimation()
{
  return m_anim;
}

}
