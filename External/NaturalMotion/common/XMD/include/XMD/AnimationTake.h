//----------------------------------------------------------------------------------------------------------------------
/// \file   AnimationTake.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  The XAnimationTake class is intended as a replacement to the
///         old XAnimCycle data node. There are *no* benefits in using
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include <map>
#include "XM2/Vector4.h"
#include "XM2/Quaternion.h"
#include "XM2/Transform.h"
#include "XMD/Base.h"
#include "XMD/AnimationStream.h"
#include "XM2/vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
class XMD_EXPORT XPoseStream;
class XMD_EXPORT XFCurveStream;
class XMD_EXPORT XSampledStream;
class XMD_EXPORT XAnimationStream;
class XMD_EXPORT XAnimationTake;
class XMD_EXPORT XAnimatedNode;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XAnimatedAttribute
/// \brief contains the animation data for a single attribute. The anim
///        data is stored in one or more streams. This is to allow more
///        than one representation of the animation data. So, for example
///        you can store a set of sampled key-frames alongside a set of
///        FCurves. Consider the following case, you have a set of bones
///        that are animated via IK handles, constraints, or expressions.
///        In that case, extracting the FCurve data is only useful if you
///        actually evaluate the result of those constraints and IK
///        systems. By providing the sampled data alongside the original
///        FCurves, you can be sure you always have a fallback if the
///        FCurves do not provide you the correct results.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XAnimatedAttribute
{
  friend class XMD_EXPORT XAnimationTake;
  friend class XMD_EXPORT XAnimatedNode;
public:

  /// \brief  creates a pose stream to hold a posed key-frame
  /// \return a new pose stream
  XPoseStream* CreatePoseStream();

  /// \brief  creates an F-Curve stream to hold a set of F-Curves
  /// \return a new FCurve stream
  XFCurveStream* CreateFCurveStream();

  /// \brief  creates a sampled stream to hold a set of sampled key-frames
  /// \return a new sampled key stream
  XSampledStream* CreateSampledStream();

  /// \brief  returns the number of streams of data available for the attribute.
  /// \return the stream count
  XU32 GetNumStreams() const;

  /// \brief  returns the a pose stream on the attribute if found
  /// \return the stream requested or NULL if the stream is not found
  XPoseStream* GetPoseStream();

  /// \brief  returns the an FCurve stream on the attribute if found
  /// \return the stream requested or NULL if the stream is not found
  XFCurveStream* GetFCurveStream();

  /// \brief  returns the a sampled key stream on the attribute if found
  /// \return the stream requested or NULL if the stream is not found
  XSampledStream* GetSampledStream();

  /// \brief  returns the a pose stream on the attribute if found
  /// \return the stream requested or NULL if the stream is not found
  const XPoseStream* GetPoseStream() const;

  /// \brief  returns the an FCurve stream on the attribute if found
  /// \return the stream requested or NULL if the stream is not found
  const XFCurveStream* GetFCurveStream() const;

  /// \brief  returns the a sampled key stream on the attribute if found
  /// \return the stream requested or NULL if the stream is not found
  const XSampledStream* GetSampledStream() const;

  /// \brief  returns the requested data stream  for the attribute
  /// \param  stream_id - the stream ID
  /// \return the stream requested or NULL if the stream id is invalid
  XAnimationStream* GetStream(XU32 stream_id);

  /// \brief  returns the requested data stream  for the attribute
  /// \param  stream_id - the stream ID
  /// \return the stream requested or NULL if the stream id is invalid
  const XAnimationStream* GetStream(XU32 stream_id) const;

  /// \brief  returns the type of data stored in the stream
  /// \return the stream type
  XAnimationStream::StreamDataType GetStreamDataType() const;

  /// \brief  returns the ID of the XMD node that this stream relates to
  /// \return the Node ID
  XId GetNodeID() const;

  /// \brief  returns the ID of the XMD property that this stream animates
  /// \return the property ID
  XU32 GetPropertyID() const;

  /// \brief  returns the name of the XMD property that this stream animates
  /// \return the property name
  const XString GetPropertyName() const;

  /// \brief  returns access to the animated node that contains this animated attribute. 
  /// \return the animated node
  const XAnimatedNode* GetAnimatedNode() const;

  /// \brief  returns access to the animated node that contains this animated attribute. 
  /// \return the animated node
  XAnimatedNode* GetAnimatedNode();

  /// \brief  returns access to the animated node that contains this animated attribute. 
  /// \return the animated node
  const XBase* GetNode() const;

  /// \brief  returns access to the animated node that contains this animated attribute. 
  /// \return the animated node
  XBase* GetNode();

private:

  XAnimatedAttribute(XAnimatedNode* anode, XAnimationStream::StreamDataType sdt, XU32 prop_id);
  XAnimatedAttribute(XAnimatedNode* anode, XAnimationStream::StreamDataType sdt, XAttribute* user_attr);
  ~XAnimatedAttribute();

private:
  bool WriteChunk(std::ostream&);
  bool ReadChunk(std::istream&);
  XU32 GetDataSize() const;
  bool DoData(XFileIO& io);
  /// the type of data this attr requires
  XAnimationStream::StreamDataType m_StreamDataType;
  /// the number of elements per sample, i.e 1,2,3,4
  XU32 m_NumElementsPerSample;

  /// id of the property that is animated 
  XU32 m_PropertyID;

  /// pointer to the user attribute (used if m_PropertyID is null).
  XAttribute* m_Attribute;

  /// the array of animation streams
  XAnimationStreamArray m_Streams;

  /// the animated node this attribute belongs to.
  XAnimatedNode* m_Node;
};

typedef XM2::pod_vector<XAnimatedAttribute*> XAnimatedAttributeArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XAnimatedNode
/// \brief All of the animated attributes are stored together within the XAnimatedNode chunk.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XAnimatedNode
{
  friend class XMD_EXPORT XAnimatedAttribute;
  friend class XMD_EXPORT XAnimationTake;
  ~XAnimatedNode();
public:

  /// \brief  returns the XMD node this object represents
  /// \return the animated node
  XBase* GetNode();

  /// \brief  returns the XMD node this object represents
  /// \return the animated node
  const XBase* GetNode() const;

  /// \brief  returns the XMD node this object represents
  /// \return the animated node
  XId GetNodeId() const;

  /// \brief  returns the number of attributes that are animated
  /// \return the num of animated attributes
  XU32 GetNumAttributes() const;

  /// \brief  returns the number of user attributes that are animated
  /// \return the num of animated user attributes
  XU32 GetNumUserAttributes() const;

  /// \brief  returns the requested animated attribute
  /// \param  id - the index of the attribute to return
  /// \return a pointer to the specified attribute, or NULL if id is invalid
  XAnimatedAttribute* GetAttribute(XU32 id);

  /// \brief  returns the requested animated attribute
  /// \param  id - the index of the attribute to return
  /// \return a pointer to the specified attribute, or NULL if id is invalid
  const XAnimatedAttribute* GetAttribute(XU32 id) const;

  /// \brief  returns the requested animated attribute
  /// \param  id - the index of the attribute to return
  /// \return a pointer to the specified attribute, or NULL if id is invalid
  XAnimatedAttribute* GetAttributeById(XU32 id);

  /// \brief  returns the requested animated attribute
  /// \param  id - the index of the attribute to return
  /// \return a pointer to the specified attribute, or NULL if id is invalid
  const XAnimatedAttribute* GetAttributeById(XU32 id) const;

  /// \brief  returns the requested animated attribute
  /// \param  id - the index of the attribute to return
  /// \return a pointer to the specified attribute, or NULL if id is invalid
  XAnimatedAttribute* GetAttributeByName(const XString& name);

  /// \brief  returns the requested animated attribute
  /// \param  id - the index of the attribute to return
  /// \return a pointer to the specified attribute, or NULL if id is invalid
  const XAnimatedAttribute* GetAttributeByName(const XString& name) const;

  /// \brief  returns the requested animated attribute
  /// \param  id - the index of the attribute to return
  /// \return a pointer to the specified attribute, or NULL if id is invalid
  XAnimatedAttribute* GetUserAttribute(XU32 id);

  /// \brief  returns the requested animated attribute
  /// \param  id - the index of the attribute to return
  /// \return a pointer to the specified attribute, or NULL if id is invalid
  const XAnimatedAttribute* GetUserAttribute(XU32 id) const;

  /// \brief  creates animation data on the specified node's property
  /// \param  property_id - the ID of the node's property that is
  ///         to be animated.
  /// \return the new animated attribute data, or NULL if either the
  ///         property_id is invalid, or the property is not animatable
  XAnimatedAttribute* CreateAnimatedAttribute(XU32 property_id);

  /// \brief  creates animation data on the specified node's property
  /// \param  property_id - the ID of the node's property that is
  ///         to be animated.
  /// \return the new animated attribute data, or NULL if either the
  ///         property_id is invalid, or the property is not animatable
  XAnimatedAttribute* CreateAnimatedAttribute(const XString& property_name);

  /// \brief  creates animation data on the specified node's property
  /// \param  property_id - the ID of the node's property that is
  ///         to be animated.
  /// \return the new animated attribute data, or NULL if either the
  ///         property_id is invalid, or the property is not animatable
  XAnimatedAttribute* CreateAnimatedUserAttribute(XU32 property_id);

  /// \brief  creates animation data on the specified node's property
  /// \param  property_id - the ID of the node's property that is
  ///         to be animated.
  /// \return the new animated attribute data, or NULL if either the
  ///         property_id is invalid, or the property is not animatable
  XAnimatedAttribute* CreateAnimatedUserAttribute(const XString& property_name);

  /// \brief  returns the animation take this node belongs to
  /// \return the animation take
  XAnimationTake* GetTake();

  /// \brief  returns the animation take this node belongs to
  /// \return the animation take  
  const XAnimationTake* GetTake() const;

private:
  bool WriteChunk(std::ostream&);
  bool ReadChunk(std::istream&);
  XU32 GetDataSize() const;
  bool DoData(XFileIO& io);
private:
  /// the ID of the animated node
  XId m_StreamType;
  /// the ID of the animated node
  XId m_Node;
  /// an array of the animated properties within the anim data
  XAnimatedAttributeArray m_AnimatedAttributes;
  /// an array of animated attrs
  XAnimatedAttributeArray m_AnimatedUserAttributes;
  /// the animated take this node belongs to
  XAnimationTake* m_Take;
};
typedef XM2::pod_vector<XAnimatedNode*> XAnimatedNodeArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XAnimationTake
/// \brief This class contains all information needed for an animation
///        cycle. This includes sampled bone animation, as well as any
///        anim curves for custom node params
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XAnimationTake
  : public XBase
{
  friend class XMD_EXPORT XAnimatedAttribute;
  XMD_CHUNK(XAnimationTake);
public:

  /// \brief  returns the current time store within the clip. This is
  ///         the time that was set at the time of export from the authoring package.
  /// \return the current time
  XReal GetTime() const;

  /// \brief  returns the starting time of all sampled data streams
  /// \return the start time
  XReal GetStartTime() const;

  /// \brief  returns the starting time of all sampled data streams
  /// \return the end time
  XReal GetEndTime() const;

  /// \brief  this returns the total number of frames within the sampled
  ///         animation data
  /// \return the total number of frames
  XU32 GetNumFrames() const;

  /// \brief  returns the minimum time of the animation. This relates to the
  ///         time of the first FCurve key frame
  /// \return the time of the first FCurve key frame
  XReal GetMinTime() const;

  /// \brief  returns the maximum time of the animation. This relates to the
  ///         time of the last FCurve key frame
  /// \return the time of the last FCurve key frame
  XReal GetMaxTime() const;

  /// \brief  returns the frames per second of the animation
  /// \return the FPS
  XReal GetFramesPerSecond() const;

  /// \brief  sets the current time store within the clip. This is the time that was set at the time 
  ///         of export from the authoring package.
  /// \param  t - the current time
  void SetTime(XReal t);

  /// \brief  sets the starting time of all sampled data streams
  /// \param  t - the start time
  void SetStartTime(XReal t);

  /// \brief  sets the starting time of all sampled data streams
  /// \param  t - the end time
  void SetEndTime(XReal t);

  /// \brief  sets the frames per second for the animation
  /// \param  fps - the FPS
  void SetFramesPerSecond(XReal fps);

  /// \brief  sets the total num frames contained in the take
  /// \param  t - the num of frames
  void SetNumFrames(XU32 t);

  /// \brief  returns the number of nodes that have animation on them.
  /// \return the number of animated nodes
  XU32 GetNumberOfAnimatedNodes() const;

  /// \brief  returns the node animation for the specified node
  /// \param  node - the node to find the animation for
  /// \return the node animation.
  XAnimatedNode* GetNodeAnimation(XU32 index);

  /// \brief  returns the node animation for the specified index
  /// \param  index - the index of the animated node data to return.
  ///         Valid values are   0  <= index < GetNumberOfAnimatedNodes()
  /// \return the node animation.
  XAnimatedNode* GetNodeAnimation(const XBase* node);

  /// \brief  returns the node animation data for the specified node ID
  /// \param  node_id - the node ID to find the animation for
  /// \return the node animation.
  XAnimatedNode* GetNodeAnimationById(XId node_id);

  /// \brief  returns the node animation data for the specified node ID
  /// \param  str - the name of the node to find the animation for
  /// \return the node animation.
  XAnimatedNode* GetNodeAnimationByName(const XString& str);

  /// \brief  returns the node animation for the specified node
  /// \param  node - the node to find the animation for
  /// \return the node animation.
  const XAnimatedNode* GetNodeAnimation(const XBase* node) const;

  /// \brief  returns the node animation for the specified index
  /// \param  index - the index of the animated node data to return.
  ///         Valid values are   0  <= index < GetNumberOfAnimatedNodes()
  /// \return the node animation.
  const XAnimatedNode* GetNodeAnimation(XU32 index) const;

  /// \brief  returns the node animation data for the specified node ID
  /// \param  node_id - the node ID to find the animation for
  /// \return the node animation.
  const XAnimatedNode* GetNodeAnimationById(XId node_id) const;

  /// \brief  returns the node animation data for the specified node ID
  /// \param  str - the name of the node to find the animation for
  /// \return the node animation.
  const XAnimatedNode* GetNodeAnimationByName(const XString& str) const;

  /// \brief  creates a node animation chunk for the specified node
  /// \param  node - the node to create the animation for
  /// \return the newly allocated node animation
  XAnimatedNode* CreateNodeAnimation(XBase* node);

  /// \brief  deletes the specified node animation
  /// \param  node - the node to delete
  /// \return true if ok
  bool DeleteNodeAnimation(XAnimatedNode* node);

private:
#ifndef DOXYGEN_PROCESSING
  /// the start time of the animation range (primarily used by sampled data)
  XReal m_fStart;
  /// the end time of the animation range (primarily used by sampled data)
  XReal m_fEnd;
  /// the minimum animation time (as determined by the animation curves present)
  XReal m_fMinAnimTime;
  /// the maximum animation time (as determined by the animation curves present)
  XReal m_fMaxAnimTime;
  /// the number of frames in the animation
  XReal m_fFramesPerSecond;
  /// the number of frames in the animation
  XReal m_fCurrentTime;
  /// the number of frames in the sampled animation
  XU32 m_iNumFrames;
  /// the animated nodes
  XAnimatedNodeArray m_AnimatedNodes;
#endif
};

#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::AnimationTake, XAnimationTake, "animationTake", "ANIMATION_TAKE");
#endif
}
