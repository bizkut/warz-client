//----------------------------------------------------------------------------------------------------------------------
/// \file   KeyStripper.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file provides some classes to strip sets of sampled keyframes down to a smaller set of time/value pairs. 
///         The primary use of these classes is for importing sampled keys into dcc packages (maya/xsi/max) in a way 
///         that minimises the amount of f-curves generated for a specific take. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Matrix34.h"
#include "XMD/AnimationTake.h"
#include "XMD/SampledStream.h"
#include "XMD/AnimCycle.h"
#include "XMD/Bone.h"
#include "XM2/QuaternionArray.h"
#include "XM2/Vector4Array.h"
#include "XM2/Vector3Array.h"
#include "XM2/Vector2Array.h"
#include "XM2/vector.h"
#include <math.h>
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XAnimCycle;
class XMD_EXPORT XAnimationTake;
#endif
}
namespace XMU  
{
using XM2::XU32;
using XM2::XReal;
using XM2::XVector2;
using XM2::XVector3;
using XM2::XVector4;
using XM2::XQuaternion;
using XM2::XRealArray;
using XM2::XVector2Array;
using XM2::XVector3Array;
using XM2::XVector4Array;
using XM2::XQuaternionArray;

using XMD::XBase;
using XMD::XSampledKeys;
using XMD::XAnimatedNode;
using XMD::XAnimationTake;
using XMD::XAnimatedAttribute;
using XMD::XSampledStream;
using XMD::XAnimationStream;
using XMD::XAnimCycle;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUKeyFrames
/// \brief the base class of all animatable attribute stripping classes. It's purpose is to take a set of sampled 
///        key-frames for an attribute, and remove any redundant key-frames. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMUKeyFrames
{
public:

  enum XKeyTypes
  {
    /// kReal is also used for int and bool animated types
    kReal,

    /// 2D vector type to be linearly interpolated
    kVector2,

    /// 3D vector type to be linearly interpolated
    kVector3,

    /// 4D vector type to be linearly interpolated
    kVector4,

    /// quaternion type to be spherically interpolated
    kQuaternion

  };

  /// \brief  returns the type of key frame data 
  /// \param  the type of key frame data
  virtual XKeyTypes GetType() const = 0;

  /// \brief  removes any redundant key frames
  /// \param  eps - floating point tolerance
  virtual void Strip(XReal eps = 0.0001f) = 0;

  /// \brief  returns the number of key frames in this list
  /// \return the number of keys after 
  XU32 GetNumKeys() const;

  /// \brief  returns the attribute ID
  /// \return the attribute ID
  XU32 GetAttribute() const;

protected:

  XMUKeyFrames(XU32 attribute, XU32 numFrames, XReal startTime, XReal endTime);

protected:

  /// \brief  removes the specified index from the time array
  /// \param  index - the index to remove from the key frame list
  void RemoveKey(XU32 index);

protected:

  /// the attribute ID
  XU32 m_attributeID;
  /// the time values for the key-frames
  XRealArray m_times;
};
//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUFloatKeyFrames
/// \brief strips redundant key frames from a set of float keys
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMUFloatKeyFrames
  : public XMUKeyFrames
{
public:

  /// \brief  ctor. constructs a stripped set of key frame data 
  /// \param  attribute - the attribute index 
  /// \param  keys - the key frames to strip 
  /// \param  startTime - the time value for the first frame
  /// \param  endTime - the time value for the last frame 
  XMUFloatKeyFrames(XU32 attribute, const XRealArray& keys, XReal startTime, XReal endTime);

  /// \brief  gets the key frame at the specified index
  /// \param  index - the key frame index
  /// \param  time - the returned time value
  /// \param  value - the returned value
  void GetKey(XU32 index, XReal& time, XReal& value) const;

  /// \brief  returns the type of key frame data 
  /// \param  the type of key frame data
  virtual XKeyTypes GetType() const { return kReal; }

  /// \brief  removes any redundant key frames
  /// \param  eps - floating point tolerance
  virtual void Strip(XReal eps=0.0001f);
private:

  /// the key-frame values
  XRealArray m_values;
};
//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUVector2KeyFrames
/// \brief strips redundant key frames from a set of XVector2 keys
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMUVector2KeyFrames
  : public XMUKeyFrames
{
public:

  /// \brief  ctor. constructs a stripped set of key frame data 
  /// \param  attribute - the attribute index 
  /// \param  keys - the key frames to strip 
  /// \param  startTime - the time value for the first frame
  /// \param  endTime - the time value for the last frame 
  XMUVector2KeyFrames(XU32 attribute, const XVector2Array& keys, XReal startTime, XReal endTime);

  /// \brief  gets the key frame at the specified index
  /// \param  index - the key frame index
  /// \param  time - the returned time value
  /// \param  value - the returned value
  void GetKey(XU32 index, XReal& time, XVector2& value) const;


  /// \brief  returns the type of key frame data 
  /// \param  the type of key frame data
  virtual XKeyTypes GetType() const { return kVector2; }

  /// \brief  removes any redundant key frames
  /// \param  eps - floating point tolerance
  virtual void Strip(XReal eps = 0.0001f);
private:

  /// the key-frame values
  XVector2Array m_values;
};
//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUVector3KeyFrames
/// \brief strips redundant key frames from a set of XVector3 keys
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMUVector3KeyFrames
  : public XMUKeyFrames
{
public:

  /// \brief  ctor. constructs a stripped set of key frame data 
  /// \param  attribute - the attribute index 
  /// \param  keys - the key frames to strip 
  /// \param  startTime - the time value for the first frame
  /// \param  endTime - the time value for the last frame 
  XMUVector3KeyFrames(XU32 attribute, const XVector3Array& keys, XReal startTime, XReal endTime);

  /// \brief  gets the key frame at the specified index
  /// \param  index - the key frame index
  /// \param  time - the returned time value
  /// \param  value - the returned value
  void GetKey(XU32 index, XReal& time, XVector3& value) const;

  /// \brief  returns the type of key frame data 
  /// \param  the type of key frame data
  virtual XKeyTypes GetType() const { return kVector3; }

  /// \brief  removes any redundant key frames
  /// \param  eps - floating point tolerance
  virtual void Strip(XReal eps = 0.0001f);
private:

  /// the key-frame values
  XVector3Array m_values;
};
//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUVector4KeyFrames
/// \brief strips redundant key frames from a set of XVector4 keys
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMUVector4KeyFrames
  : public XMUKeyFrames
{
public:

  /// \brief  ctor. constructs a stripped set of key frame data 
  /// \param  attribute - the attribute index 
  /// \param  keys - the key frames to strip 
  /// \param  startTime - the time value for the first frame
  /// \param  endTime - the time value for the last frame 
  XMUVector4KeyFrames(XU32 attribute, const XVector4Array& keys, XReal startTime, XReal endTime);

  /// \brief  gets the key frame at the specified index
  /// \param  index - the key frame index
  /// \param  time - the returned time value
  /// \param  value - the returned value
  void GetKey(XU32 index, XReal& time, XVector4& value) const;

  /// \brief  returns the type of key frame data 
  /// \param  the type of key frame data
  virtual XKeyTypes GetType() const { return kVector4; }

  /// \brief  removes any redundant key frames
  /// \param  eps - floating point tolerance
  virtual void Strip(XReal eps = 0.0001f);
private:

  /// the key-frame values
  XVector4Array m_values;
};
//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUQuaternionKeyFrames
/// \brief strips redundant key frames from a set of XQuaternion keys
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMUQuaternionKeyFrames
  : public XMUKeyFrames
{
public:

  /// \brief  ctor. constructs a stripped set of key frame data 
  /// \param  attribute - the attribute index 
  /// \param  keys - the key frames to strip 
  /// \param  startTime - the time value for the first frame
  /// \param  endTime - the time value for the last frame 
  XMUQuaternionKeyFrames(XU32 attribute, const XQuaternionArray& keys, XReal startTime, XReal endTime);

  /// \brief  gets the key frame at the specified index
  /// \param  index - the key frame index
  /// \param  time - the returned time value
  /// \param  value - the returned value
  void GetKey(XU32 index, XReal& time, XQuaternion& value) const;

  /// \brief  returns the type of key frame data 
  /// \param  the type of key frame data
  virtual XKeyTypes GetType() const { return kQuaternion; }

  /// \brief  removes any redundant key frames
  /// \param  eps - floating point tolerance
  virtual void Strip(XReal eps = 0.0001f);

private:

  /// the key-frame values
  XQuaternionArray m_values;
};
//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUNodeAnim
/// \brief a structure to hold the key-frames for a single bone in an animation cycle. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMUStrippedNodeAnim
{
public:

  /// \brief  ctor. Sets the keyframes for the node. 
  /// \param  node - the node 
  XMUStrippedNodeAnim(XBase* node, XSampledKeys* keyframes, XReal startTime, XReal endTime);

  /// \brief  ctor. Sets the keyframes for the node. 
  /// \param  node - the node 
  XMUStrippedNodeAnim(XBase* node, XAnimatedNode* keyframes, XReal start, XReal end);

  /// \brief  removes any redundant key-frames from the data
  /// \param  tolerance - the floating point tolerance
  void Strip(XReal tolerance = 0.0001f);

  /// \brief  returns the number of animated 
  /// \return the number attributes
  XU32 GetNumAttributes() const;

  /// \brief  returns the requested attribute
  /// \param  index - the attribute index to query
  /// \return the attribute or NULL if invalid index
  const XMUKeyFrames* GetAttribute(XM2::XU32 index) const;

  /// \brief  returns the requested attribute
  /// \param  index - the attribute index to query
  /// \return the attribute or NULL if invalid index
  const XMUKeyFrames* GetAttributeFromID(XM2::XU32 attribute_id) const;

  /// \brief  returns the node related to this anim data
  XBase* GetNode();

  /// \brief  returns the node related to this anim data
  const XBase* GetNode() const;

private:
  /// the animated node
  XBase* m_node;
  /// all of the key-framed on this node
  XM2::pod_vector< XMUKeyFrames* > m_keyedAttributes;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUNodeAnim
/// \brief a structure to hold the key-frames for a single bone in an animation cycle. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMUStrippedAnimTake
{
public:
  
  /// ctor 
  XMUStrippedAnimTake(XAnimationTake* take);

  /// ctor
  XMUStrippedAnimTake(XAnimCycle* cycle);

  /// dtor
  ~XMUStrippedAnimTake();

  /// \brief  removes any redundant key-frames from the data
  /// \param  tolerance - the floating point tolerance
  void Strip(XReal tolerance = 0.0001f);

  /// \brief  returns the animation this class represents
  XBase* GetAnimation();

  /// \brief  returns the number of animated nodes stored in this class
  /// \return the number of animated nodes
  XU32 GetNumNodes() const;

  /// \brief  returns the animated node requested 
  /// \param  node - the node index to return
  const XMUStrippedNodeAnim* GetAnimatedNode(XU32 node) const;

private:

  /// the original animation (may be of type XMD::XAnimCycle or XMD::XAnimationTake)
  XBase* m_anim;
  /// animation data for each animated node
  XM2::pod_vector<XMUStrippedNodeAnim*> m_nodeAnims;
};
}
