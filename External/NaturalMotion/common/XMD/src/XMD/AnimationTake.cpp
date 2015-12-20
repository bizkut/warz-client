//----------------------------------------------------------------------------------------------------------------------
/// \file CollisionBox.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/AnimationTake.h"
#include "XMD/FCurveStream.h"
#include "XMD/PoseStream.h"
#include "XMD/SampledStream.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"
#include <math.h>

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute::XAnimatedAttribute(XAnimatedNode* anode, XAnimationStream::StreamDataType sdt,XU32 prop_id)
{
  m_StreamDataType = sdt;
  m_Attribute = 0;
  m_PropertyID = prop_id;
  m_Node = anode;

  m_NumElementsPerSample = XAnimationStream::GetStreamTypeNumElements(m_StreamDataType);
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute::XAnimatedAttribute(XAnimatedNode* anode, XAnimationStream::StreamDataType sdt,XAttribute* user_attr)
  : m_Streams()
{
  m_StreamDataType = sdt;
  m_Attribute = user_attr;
  m_PropertyID = 0;
  m_Node = anode;
  m_NumElementsPerSample = XAnimationStream::GetStreamTypeNumElements(m_StreamDataType);
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute::~XAnimatedAttribute()
{
  XAnimationStreamArray::iterator it = m_Streams.begin();
  for (;it != m_Streams.end(); ++it)
  {
    delete *it;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XPoseStream* XAnimatedAttribute::CreatePoseStream()
{
  XPoseStream* str = GetPoseStream();
  if (!str)
  {
    str = new XPoseStream(this,m_Node->GetNode(), this->m_StreamDataType);
    m_Streams.push_back(str);
  }
  return str;
}

//----------------------------------------------------------------------------------------------------------------------
XFCurveStream* XAnimatedAttribute::CreateFCurveStream()
{
  XFCurveStream* str = GetFCurveStream();
  if (!str)
  {
    str = new XFCurveStream(this);
    m_Streams.push_back(str);
  }
  return str;
}

//----------------------------------------------------------------------------------------------------------------------
XSampledStream* XAnimatedAttribute::CreateSampledStream()
{
  XSampledStream* str = GetSampledStream();
  if (!str)
  {
    unsigned numElements = XAnimationStream::GetStreamTypeNumElements(GetStreamDataType());
    XAnimationTake* take = m_Node->GetTake();
    XMD_ASSERT(take);

    str = new XSampledStream(this,numElements,take->GetNumFrames());
    m_Streams.push_back(str);

    switch(this->GetStreamDataType())
    {
    case XAnimationStream::kReal:
      {
        XReal v;
        const XBase* b = m_Node->GetNode();
        b->GetProperty(m_PropertyID,v);
        for (unsigned i=0;i!=take->GetNumFrames();++i)
        {
          str->SetSample(i,v);
        }
      }
      break;

    case XAnimationStream::kVector2:
      {
       /* XM::XVector2 v;
        const XBase* b = m_Node->GetNode();
        b->GetProperty(m_PropertyID,v);
        for (unsigned i=0;i!=take->GetNumFrames();++i)
        {
          str->SetSample(i,v);
        }*/
      }
      break;

    case XAnimationStream::kVector3:
      {
        XVector3 v;
        const XBase* b = m_Node->GetNode();
        b->GetProperty(m_PropertyID,v);
        for (unsigned i=0;i!=take->GetNumFrames();++i)
        {
          str->SetSample(i,v);
        }
      }
      break;

    case XAnimationStream::kVector4:
    case XAnimationStream::kQuaternion:
      {
        XQuaternion v;
        const XBase* b = m_Node->GetNode();
        b->GetProperty(m_PropertyID,v);
        for (unsigned i=0;i!=take->GetNumFrames();++i)
        {
          str->SetSample(i,v);
        }
      }
      break;

    case XAnimationStream::kMatrix:
      {
        XMatrix v;
        const XBase* b = m_Node->GetNode();
        b->GetProperty(m_PropertyID,v);
        for (unsigned i=0;i!=take->GetNumFrames();++i)
        {
          str->SetSample(i,v);
        }
      }
      break;

    default:
      break;
    }
  }
  return str;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimatedAttribute::GetNumStreams() const
{
  return static_cast<XU32>(m_Streams.size());
}

//----------------------------------------------------------------------------------------------------------------------
XPoseStream* XAnimatedAttribute::GetPoseStream()
{
  XAnimationStreamArray::iterator it = m_Streams.begin();
  for (;it != m_Streams.end(); ++it)
  {
    if ((*it)->AsPoseStream())
    {
      return (*it)->AsPoseStream();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XPoseStream* XAnimatedAttribute::GetPoseStream() const
{
  XAnimationStreamArray::const_iterator it = m_Streams.begin();
  for (;it != m_Streams.end(); ++it)
  {
    if ((*it)->AsPoseStream())
    {
      return (*it)->AsPoseStream();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XFCurveStream* XAnimatedAttribute::GetFCurveStream()
{
  XAnimationStreamArray::iterator it = m_Streams.begin();
  for (;it != m_Streams.end(); ++it)
  {
    if ((*it)->AsFCurveStream())
    {
      return (*it)->AsFCurveStream();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XFCurveStream* XAnimatedAttribute::GetFCurveStream() const
{
  XAnimationStreamArray::const_iterator it = m_Streams.begin();
  for (;it != m_Streams.end(); ++it)
  {
    if ((*it)->AsFCurveStream())
    {
      return (*it)->AsFCurveStream();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XSampledStream* XAnimatedAttribute::GetSampledStream()
{
  XAnimationStreamArray::iterator it = m_Streams.begin();
  for (;it != m_Streams.end(); ++it)
  {
    if ((*it)->AsSampledStream())
    {
      return (*it)->AsSampledStream();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XSampledStream* XAnimatedAttribute::GetSampledStream() const
{
  XAnimationStreamArray::const_iterator it = m_Streams.begin();
  for (;it != m_Streams.end(); ++it)
  {
    if ((*it)->AsSampledStream())
    {
      return (*it)->AsSampledStream();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimationStream* XAnimatedAttribute::GetStream(const XU32 stream_id)
{
  if (GetNumStreams()>stream_id)
  {
    return m_Streams[stream_id];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimationStream* XAnimatedAttribute::GetStream(const XU32 stream_id) const
{
  if (GetNumStreams()>stream_id)
  {
    return m_Streams[stream_id];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimationStream::StreamDataType XAnimatedAttribute::GetStreamDataType() const
{
  return m_StreamDataType;
}

//----------------------------------------------------------------------------------------------------------------------
XId XAnimatedAttribute::GetNodeID() const
{
  return m_Node->GetNodeId();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimatedAttribute::GetPropertyID() const
{
  return m_PropertyID;
}

//----------------------------------------------------------------------------------------------------------------------
const XString XAnimatedAttribute::GetPropertyName() const
{
  const XBase* n = m_Node->GetNode();
  return n->GetPropertyName(m_PropertyID);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimatedAttribute::WriteChunk(std::ostream& ofs)
{
  XMD_ASSERT(m_Node);

  XModel* model = m_Node->m_Take->GetModel();
  XMD_ASSERT(model);

  XBase* base = model->FindNode(m_Node->m_Node);
  XMD_ASSERT(base);

  ofs << "\t\tATTR " << base->GetPropertyName(m_PropertyID) << "\n";
  ofs << "\t\t{\n";

  ofs << "\t\t\tnum_streams " << m_Streams.size() << "\n";
  ofs << "\t\t\telements_per_sample " << m_NumElementsPerSample << "\n";

  for(XU32 i=0;i!=m_Streams.size();++i)
  {
    if(!m_Streams[i]->WriteChunk(ofs))
      return false;
  }

  ofs << "\t\t}\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimatedAttribute::ReadChunk(std::istream& ifs)
{
  XString attr_name;
  READ_CHECK("ATTR",ifs);
  ifs >> attr_name;

  XMD::XBase* node = m_Node->GetNode();
  if(!node)
  {
    return false;
  }

  XU32 prop_id = node->GetPropertyID(attr_name);
  XPropertyType::Type property_type = node->GetPropertyType(prop_id);

  READ_CHECK("{",ifs);

  XU32 temp=0;
  READ_CHECK("num_streams",ifs);
  ifs >> temp;
  m_Streams.resize(temp, 0);

  READ_CHECK("elements_per_sample",ifs);
  ifs >> m_NumElementsPerSample;

  switch(property_type)
  {
  case XPropertyType::kBool:
  case XPropertyType::kReal:
  case XPropertyType::kInt:
    m_StreamDataType = XAnimationStream::kReal;
    break;

  case XPropertyType::kColour:
    m_StreamDataType = XAnimationStream::kColour4;
    break;

  case XPropertyType::kVector3:
    m_StreamDataType = XAnimationStream::kVector3;
    break;

  case XPropertyType::kMatrix:
    m_StreamDataType = XAnimationStream::kMatrix;
    break;

  case XPropertyType::kQuaternion:
    m_StreamDataType = XAnimationStream::kQuaternion;
    break;

  case XPropertyType::kRealArray:
    m_StreamDataType = XAnimationStream::kRealArray;
    break;

  case XPropertyType::kVector3Array:
    m_StreamDataType = XAnimationStream::kVector3Array;
    break;


  default:
    XMD_ASSERT(0);
    break;
  }


  m_PropertyID = prop_id;

  if(prop_id == 0)
  {
    XAttribute* attr = node->GetAttribute(attr_name.c_str());
    if (attr)
    {
      m_Attribute = attr;
    }
    else
    {
      /// \todo  skip past the attr data to the closing }
    }
  }

  for (XU32 i=0;i!=temp;++i)
  {
    XString stream_type;
    ifs >> stream_type;
    if (stream_type == "POSE_KEY")
    {
      m_Streams[i] = new XPoseStream(this,m_Node->GetNode(), this->m_StreamDataType);
    }
    else
    if (stream_type == "SAMPLED_KEYS")
    {
      XAnimatedNode* ani_node = this->m_Node;
      XAnimationTake* take = ani_node->GetTake();
      m_Streams[i] = new XSampledStream(this,m_NumElementsPerSample,take->GetNumFrames());
    }
    else
    if (stream_type == "FCURVE_STREAM")
    {
      m_Streams[i] = new XFCurveStream(this);
    }
    READ_CHECK("{",ifs);
    if(!m_Streams[i]->ReadChunk(ifs))
      return false;
    READ_CHECK("}",ifs);
  }
  READ_CHECK("}",ifs);

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimatedAttribute::GetDataSize() const
{
  size_t s = sizeof(XU32)*3;
  XAnimationStreamArray::const_iterator it = m_Streams.begin();
  for(; it != m_Streams.end();++it)
  {
    s += (*it)->GetDataSize();
  }
  return (XU32)s;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimatedAttribute::DoData(XFileIO& io)
{
  io.DoData(&m_PropertyID);
  io.DoData(&m_NumElementsPerSample);

  XU32 num = (XU32)m_Streams.size();
  io.DoData(&num);

  if (io.IsReading())
  {
    m_Streams.resize(num);
    XAnimationStreamArray::iterator it = m_Streams.begin();
    for(; it != m_Streams.end();++it)
    {
      XAnimationStream::StreamType stream_type;
      XAnimationStream::StreamDataType stream_data_type;
      io.DoData(&stream_type);
      io.DoData(&stream_data_type);

      switch (stream_type)
      {
      case XAnimationStream::kPoseStream:
        *it = new XPoseStream(this,GetNode(), stream_data_type);
        break;
      case XAnimationStream::kSampledStream:
        {
          XAnimatedNode* ani_node = this->m_Node;
          XAnimationTake* take = ani_node->GetTake();
          *it = new XSampledStream(this,m_NumElementsPerSample,take->GetNumFrames());
        }
        break;
      case XAnimationStream::kFCurveStream:
        *it = new XFCurveStream(this);
        break;
      default:
        break;
      }

      (*it)->m_StreamDataType = stream_data_type;
      if(!(*it)->DoData(io))
        return false;
    }
  }
  else
  {
    XAnimationStreamArray::iterator it = m_Streams.begin();
    for(; it != m_Streams.end();++it)
    {
      XAnimationStream::StreamType stream_type = (*it)->GetStreamType();
      XAnimationStream::StreamDataType stream_data_type = (*it)->GetStreamDataType();
      io.DoData(&stream_type);
      io.DoData(&stream_data_type);
      if(!(*it)->DoData(io))
        return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimatedNode* XAnimatedAttribute::GetAnimatedNode() const
{
  return m_Node;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedNode* XAnimatedAttribute::GetAnimatedNode()
{
  return m_Node;
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XAnimatedAttribute::GetNode() const
{
  return m_Node ? m_Node->GetNode() : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAnimatedAttribute::GetNode()
{
  return m_Node ? m_Node->GetNode() : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedNode::~XAnimatedNode()
{
  XAnimatedAttributeArray::iterator it = m_AnimatedAttributes.begin();
  for (;it != m_AnimatedAttributes.end(); ++it)
  {
    delete *it;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimatedNode::WriteChunk(std::ostream& ofs)
{
  ofs << "\tANIM_NODE " << m_Node << "\n\t{\n";
  ofs << "\t\tnum_animated_attrs " << static_cast<unsigned>(m_AnimatedAttributes.size()) << "\n";
  ofs << "\t\tnum_animated_attrs_user " << static_cast<unsigned>(m_AnimatedUserAttributes.size()) << "\n";
  XAnimatedAttributeArray::iterator it = m_AnimatedAttributes.begin();
  for (;it != m_AnimatedAttributes.end(); ++it)
  {
    if(!(*it)->WriteChunk(ofs))
      return false;
  }
  ofs << "\t}\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimatedNode::ReadChunk(std::istream& ifs)
{
  READ_CHECK("ANIM_NODE",ifs);
  ifs >> m_Node;

  READ_CHECK("{",ifs);
  READ_CHECK("num_animated_attrs",ifs);
  XU32 animatedAttributeCount;
  ifs >> animatedAttributeCount;
  m_AnimatedAttributes.resize(animatedAttributeCount, 0);

  XU32 animatedUserAttributeCount;
  READ_CHECK("num_animated_attrs_user",ifs);
  ifs >> animatedUserAttributeCount;
  m_AnimatedUserAttributes.resize(animatedUserAttributeCount, 0);


  XAnimatedAttributeArray::iterator itEnd = m_AnimatedAttributes.end();
  for(XAnimatedAttributeArray::iterator it = m_AnimatedAttributes.begin(); it != itEnd; ++it)
  {
    XAttribute* n=0;
    (*it) = new XAnimatedAttribute(this,XAnimationStream::kReal,n);
    if(!(*it)->ReadChunk(ifs))
    {
      return false;
    }
  }

  READ_CHECK("}",ifs);
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimatedNode::GetDataSize() const
{
  size_t size = sizeof(XU32)*3;

  XAnimatedAttributeArray::const_iterator it = m_AnimatedAttributes.begin();
  for (;it != m_AnimatedAttributes.end();++it)
  {
    size += (*it)->GetDataSize();
  }
  return (XU32)size;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimatedNode::DoData(XFileIO& io)
{
  io.DoData(&m_Node);
  XU32 num = (XU32)m_AnimatedAttributes.size();
  io.DoData(&num);
  if (io.IsReading())
  {
    m_AnimatedAttributes.resize(num);
  }

  num = (XU32)m_AnimatedUserAttributes.size();
  io.DoData(&num);
  if (io.IsReading())
  {
    m_AnimatedUserAttributes.resize(num);
  }

  XAnimatedAttributeArray::iterator it = m_AnimatedAttributes.begin();
  for (;it != m_AnimatedAttributes.end();++it)
  {
    if (io.IsReading())
    {
      (*it) = new XAnimatedAttribute(this,XAnimationStream::kReal,(XAttribute*)0);
    }
    if(!(*it)->DoData(io))
      return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAnimatedNode::GetNode()
{
  return m_Take->GetModel()->FindNode(m_Node);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XAnimatedNode::GetNode() const
{
  return m_Take->GetModel()->FindNode(m_Node);
}

//----------------------------------------------------------------------------------------------------------------------
XId XAnimatedNode::GetNodeId() const
{
  return m_Node;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimatedNode::GetNumAttributes() const
{
  return static_cast<XU32>(m_AnimatedAttributes.size());
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimatedNode::GetNumUserAttributes() const
{
  return static_cast<XU32>(m_AnimatedUserAttributes.size());
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute* XAnimatedNode::GetAttributeById(const XU32 id)
{
  XAnimatedAttributeArray::iterator it = m_AnimatedAttributes.begin();
  for (;it != m_AnimatedAttributes.end();++it)
  {
    XAnimatedAttribute* attr = *it;
    if(id == attr->GetPropertyID())
      return attr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimatedAttribute* XAnimatedNode::GetAttributeById(const XU32 id) const
{
  XAnimatedAttributeArray::const_iterator it = m_AnimatedAttributes.begin();
  for (;it != m_AnimatedAttributes.end();++it)
  {
    XAnimatedAttribute* attr = *it;
    if(id == attr->GetPropertyID())
      return attr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute* XAnimatedNode::GetAttributeByName(const XString& name)
{
  XAnimatedAttributeArray::iterator it = m_AnimatedAttributes.begin();
  for (;it != m_AnimatedAttributes.end();++it)
  {
    XAnimatedAttribute* attr = *it;
    if(name == GetNode()->GetPropertyName(attr->GetPropertyID()))
      return attr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimatedAttribute* XAnimatedNode::GetAttributeByName(const XString& name) const
{
  XAnimatedAttributeArray::const_iterator it = m_AnimatedAttributes.begin();
  for (;it != m_AnimatedAttributes.end();++it)
  {
    XAnimatedAttribute* attr = *it;
    if(name == GetNode()->GetPropertyName(attr->GetPropertyID()))
      return attr;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute* XAnimatedNode::GetAttribute(const XU32 id)
{
  return (id < GetNumAttributes()) ? m_AnimatedAttributes[id] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimatedAttribute* XAnimatedNode::GetAttribute(const XU32 id) const
{
  return (id < GetNumAttributes()) ? m_AnimatedAttributes[id] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimationTake* XAnimatedNode::GetTake()
{
  return m_Take;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimationTake* XAnimatedNode::GetTake() const
{
  return m_Take;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute* XAnimatedNode::GetUserAttribute(const XU32 id)
{
  return (id < GetNumUserAttributes()) ? m_AnimatedUserAttributes[id] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimatedAttribute* XAnimatedNode::GetUserAttribute(const XU32 id) const
{
  return (id < GetNumUserAttributes()) ? m_AnimatedUserAttributes[id] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimationStream::StreamDataType getPropertyTypeAsStreamType(const XPropertyType::Type type)
{
  switch(type)
  {
  case XPropertyType::kBool:
  case XPropertyType::kInt:
  case XPropertyType::kReal:
    return XAnimationStream::kReal;

  case XPropertyType::kUnsignedArray:
  case XPropertyType::kRealArray:
    return XAnimationStream::kReal;

  case XPropertyType::kVector3:
  case XPropertyType::kVector3Array:
    return XAnimationStream::kVector3;

  case XPropertyType::kQuaternion:
    return XAnimationStream::kQuaternion;

  case XPropertyType::kColour:
    return XAnimationStream::kVector4;

  case XPropertyType::kMatrix:
    return XAnimationStream::kMatrix;

  default:
    break;
  }
  return XAnimationStream::kInvalid;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute* XAnimatedNode::CreateAnimatedAttribute(const XU32 property_id)
{
  XBase* node = GetNode();
  if (node)
  {
    if(node->IsPropertyAnimatable(property_id))
    {

      XAnimationStream::StreamDataType type = getPropertyTypeAsStreamType(node->GetPropertyType(property_id));
      if(type != XAnimationStream::kInvalid)
      {
        XAnimatedAttribute* attr = new XAnimatedAttribute(this,type,property_id);
        m_AnimatedAttributes.push_back(attr);
        return attr;
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute* XAnimatedNode::CreateAnimatedAttribute(const XString& property_name)
{
  XBase* node = GetNode();
  if (node)
  {
    const XU32 property_id = node->GetPropertyID(property_name);

    if(node->IsPropertyAnimatable(property_id))
    {

      XAnimationStream::StreamDataType type = getPropertyTypeAsStreamType(node->GetPropertyType(property_id));
      if(type != XAnimationStream::kInvalid)
      {
        XAnimatedAttribute* attr = new XAnimatedAttribute(this,type,property_id);
        m_AnimatedAttributes.push_back(attr);
        return attr;
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute* XAnimatedNode::CreateAnimatedUserAttribute(const XU32 property_id)
{
  /// \todo implement
  (void)property_id;
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute* XAnimatedNode::CreateAnimatedUserAttribute(const XString& property_name)
{
  /// \todo implement
  (void)property_name;
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimationTake::XAnimationTake(XModel* model) : XBase(model),m_AnimatedNodes()
{
  m_fStart=0;
  m_fEnd=0;
  m_fMinAnimTime=0;
  m_fMaxAnimTime=0;
  m_fFramesPerSecond=0;
  m_fCurrentTime=0;
  m_iNumFrames=0;
  m_fStart=0;
  m_fStart=0;
  m_fStart=0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimationTake::~XAnimationTake()
{
  XAnimatedNodeArray::iterator it = m_AnimatedNodes.begin();
  for (;it != m_AnimatedNodes.end(); ++it)
  {
    delete *it;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimationTake::GetTime() const
{
  return m_fCurrentTime;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimationTake::GetStartTime() const
{
  return m_fStart;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimationTake::GetEndTime() const
{
  return m_fEnd;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimationTake::GetNumFrames() const
{
  return m_iNumFrames;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimationTake::GetMinTime() const
{
  return m_fMinAnimTime;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimationTake::GetMaxTime() const
{
  return m_fMaxAnimTime;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimationTake::GetFramesPerSecond() const
{
  return m_fFramesPerSecond;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationTake::SetTime(const XReal t)
{
  m_fCurrentTime = t;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationTake::SetStartTime(const XReal t)
{
  m_fStart = t;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationTake::SetEndTime(const XReal t)
{
  m_fEnd = t;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationTake::SetNumFrames(const XU32 t)
{
  m_iNumFrames = t;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationTake::SetFramesPerSecond(const XReal fps)
{
  m_fFramesPerSecond = fps;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimationTake::GetNumberOfAnimatedNodes() const
{
  return static_cast<XU32>(m_AnimatedNodes.size());
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedNode* XAnimationTake::GetNodeAnimation(const XU32 index)
{
  return (index < GetNumberOfAnimatedNodes()) ? m_AnimatedNodes[index] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedNode* XAnimationTake::GetNodeAnimation(const XBase* node)
{
  XMD_ASSERT(IsValidObj(node));
  if(!IsValidObj(node))
    return 0;
  return GetNodeAnimationById(node->GetID());
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedNode* XAnimationTake::GetNodeAnimationById(const XId node_id)
{
  XAnimatedNodeArray::iterator it = m_AnimatedNodes.begin();
  for (;it != m_AnimatedNodes.end(); ++it)
  {
    if (node_id==(*it)->GetNodeId())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedNode* XAnimationTake::GetNodeAnimationByName(const XString& str)
{
  XAnimatedNodeArray::iterator it = m_AnimatedNodes.begin();
  for (;it != m_AnimatedNodes.end(); ++it)
  {
    if (str==(*it)->GetNode()->GetName())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedNode* XAnimationTake::CreateNodeAnimation(XBase* node)
{
  XAnimatedNode* anode = 0;
  anode = GetNodeAnimation(node);
  if(!anode)
  {
    anode = new XAnimatedNode();
    anode->m_Node = node->GetID();
    anode->m_Take = this;
    m_AnimatedNodes.push_back(anode);
  }
  return anode;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimationTake::DeleteNodeAnimation(XAnimatedNode* node)
{
  XAnimatedNodeArray::iterator it = m_AnimatedNodes.begin();
  for (; it != m_AnimatedNodes.end(); ++it)
  {
    if (*it == node)
    {
      delete *it;
      m_AnimatedNodes.erase( it );
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimatedNode* XAnimationTake::GetNodeAnimation(const XU32 index) const
{
  return (index < GetNumberOfAnimatedNodes()) ? m_AnimatedNodes[index] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimatedNode* XAnimationTake::GetNodeAnimation(const XBase* node) const
{
  XMD_ASSERT(IsValidObj(node));
  if(!IsValidObj(node))
    return 0;
  return GetNodeAnimationById(node->GetID());
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimatedNode* XAnimationTake::GetNodeAnimationById(const XId node_id) const
{
  XAnimatedNodeArray::const_iterator it = m_AnimatedNodes.begin();
  for (;it != m_AnimatedNodes.end(); ++it)
  {
    if (node_id==(*it)->GetNodeId())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimatedNode* XAnimationTake::GetNodeAnimationByName(const XString& str) const
{
  XAnimatedNodeArray::const_iterator it = m_AnimatedNodes.begin();
  for (;it != m_AnimatedNodes.end(); ++it)
  {
    if (str==(*it)->GetNode()->GetName())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimationTake::DoData(XFileIO& io)
{
  IO_CHECK( XBase::DoData(io) );
  io.DoData(&m_fStart);
  io.DoData(&m_fEnd);

  if(io.IsReading())
  {
    // Round up the number of frames
    m_iNumFrames = (int)(ceilf(m_fEnd - m_fStart)) + 1;
  }

  io.DoData(&m_fFramesPerSecond);
  io.DoData(&m_fCurrentTime);
  XU32 num = (XU32)m_AnimatedNodes.size();
  io.DoData(&num);
  if (io.IsReading())
  {
    m_AnimatedNodes.resize(num);
  }
  num = 0;
  io.DoData(&num);
  if (io.IsReading())
  {
    XAnimatedNodeArray::iterator it = m_AnimatedNodes.begin();
    for (;it != m_AnimatedNodes.end(); ++it)
    {
      *it = new XAnimatedNode();
      (*it)->m_Take = this;
      if(!(*it)->DoData(io))
        return false;
    }
  }
  else
  {
    XAnimatedNodeArray::iterator it = m_AnimatedNodes.begin();
    for (;it != m_AnimatedNodes.end(); ++it)
    {
      if(!(*it)->DoData(io))
        return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimationTake::GetDataSize() const
{
  size_t s = XBase::GetDataSize();
  s += sizeof(XReal)*4 + sizeof(XU32)*2;
  XAnimatedNodeArray::const_iterator it = m_AnimatedNodes.begin();
  for (;it != m_AnimatedNodes.end(); ++it)
  {
    s += (*it)->GetDataSize();
  }
  return (XU32)s;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationTake::DoCopy(const XBase* ptr)
{
  const XAnimationTake* cb = ptr->HasFn<XAnimationTake>();
  XMD_ASSERT(cb);
  m_fStart = cb->m_fStart;
  m_fEnd = cb->m_fEnd;
  m_fFramesPerSecond = cb->m_fFramesPerSecond;
  m_fCurrentTime = cb->m_fCurrentTime;
  // m_fStart = cb->m_fStart;
  /// \todo  copy animated attrs
  /// \todo  copy animated events
}


//----------------------------------------------------------------------------------------------------------------------
XFn::Type XAnimationTake::GetApiType() const
{
  return XFn::AnimationTake;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAnimationTake::GetFn(XFn::Type type)
{
  if (type == XFn::AnimationTake)
    return (XAnimationTake*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XAnimationTake::GetFn(XFn::Type type) const
{
  if (type == XFn::AnimationTake)
    return (XAnimationTake*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimationTake::WriteChunk(std::ostream& ofs)
{
  ofs << "\tstart_time " << m_fStart << "\n";
  ofs << "\tend_time " << m_fEnd << "\n";
  ofs << "\tfps " << m_fFramesPerSecond << "\n";
  ofs << "\tcurrent_time " << m_fCurrentTime << "\n";
  ofs << "\tnum_animated_nodes " << static_cast<unsigned>(m_AnimatedNodes.size()) << "\n";
  ofs << "\tnum_event_tracks " << 0 << "\n";

  XAnimatedNodeArray::iterator it = m_AnimatedNodes.begin();
  for (;it != m_AnimatedNodes.end(); ++it)
  {
    if(!(*it)->WriteChunk(ofs))
      return false;
  }

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimationTake::ReadChunk(std::istream& ifs)
{
  READ_CHECK("start_time",ifs);
  ifs >> m_fStart;

  READ_CHECK("end_time",ifs);
  ifs >> m_fEnd;

  // Round up the number of frames
  m_iNumFrames = (int)(ceilf(m_fEnd - m_fStart)) + 1;

  READ_CHECK("fps",ifs);
  ifs >> m_fFramesPerSecond;

  READ_CHECK("current_time",ifs);
  ifs >> m_fCurrentTime;

  XU32 animatedNodeCount;
  READ_CHECK("num_animated_nodes",ifs);
  ifs >> animatedNodeCount;
  m_AnimatedNodes.resize(animatedNodeCount, 0);

  XU32 eventTrackCount;
  READ_CHECK("num_event_tracks",ifs);
  ifs >> eventTrackCount;
  //m_EventTracks.resize(temp);

  XAnimatedNodeArray::iterator itEnd = m_AnimatedNodes.end();
  for (XAnimatedNodeArray::iterator it = m_AnimatedNodes.begin(); it != itEnd; ++it)
  {
    (*it) = new XAnimatedNode();
    (*it)->m_Take = this;
    if(!(*it)->ReadChunk(ifs))
    {
      return false;
    }
  }

  // remove slashes and quotations from the name so it can be used in a filename
  for (XU32 i=0;i<m_Name.size();)
  {
    if (m_Name[i]=='\"'||m_Name[i]=='/'||m_Name[i]=='\\')
    {
      m_Name.erase( m_Name.begin() + i );
    }
    else ++i;
  }

  /// \todo read event tracks!

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimationTake::NodeDeath(XId id)
{
  XAnimatedNodeArray::iterator it = m_AnimatedNodes.begin();
  for (;it != m_AnimatedNodes.end(); ++it)
  {
    if ((*it)->m_Node==id)
    {
      m_AnimatedNodes.erase(it);
      return true;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationTake::PreDelete(XIdSet& ids)
{
  (void)ids;
  /// \todo  implement
}

}
