//----------------------------------------------------------------------------------------------------------------------
/// \file   AnimCycle.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Contains the definition for the main animation class, XAnimCycle.
///         There is nothing clever about this class, it just contains 
///         bone anim structures full of sampled key-frame data.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include <map>
#include "XM2/Vector3.h"
#include "XM2/Quaternion.h"
#include "XM2/QuaternionArray.h"
#include "XM2/Transform.h"
#include "XMD/Base.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This structure holds a key-frame on a hermite animation
///         curve.
/// \deprecated This class has been deprecated in favour of the 
///          XAnimationTake class. This class no only exists to provide
///          backwards compatibility to old data files.
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XKeyFrame
{
  #ifndef DOXYGEN_PROCESSING
  friend class XAnimCurve;
  friend std::ostream& operator << (std::ostream& ofs, const XKeyFrame& kf);
  #endif

  /// \brief  ctor
  XKeyFrame();

  /// \brief  copy ctor
  /// \param  kf  -  the key-frame to copy
  XKeyFrame(const XKeyFrame& kf);

  /// \brief  equality operator
  bool operator == (const XKeyFrame& kf) const;

private:
#ifndef DOXYGEN_PROCESSING

  /// \brief  ctor
  /// \param  t  -  the time of the key-frame
  /// \param  v  -  the value of the key
  /// \param  i  -  the in tangent
  /// \param  o  -  the out tangent
  XKeyFrame(XReal t, XReal v, XReal i, XReal o);

  /// \brief  This function writes the data for the chunk. The header,
  ///         name, extra attributes and surrounding braces will have
  ///         been written already.
  /// \param  ofs  -  the output file stream
  /// \return true if OK
  bool WriteChunk(std::ostream& ofs);

  /// \brief  This function reads the data for the chunk. The header,
  ///         name, extra attributes and surrounding braces get read 
  ///         for you.
  /// \param  ifs  -  the input file stream
  /// \return true if OK
  bool ReadChunk(std::istream& ifs);

#endif
public:

  /// the time of the key-frame
  XReal time;
  /// the value of the key-frame
  XReal value;
  /// the in tangent value
  XReal in;
  /// the out tangent value
  XReal out;
};
/// \n array of keyframes for an animation curve
typedef XM2::pod_vector<XKeyFrame> XKeyFrameList;

//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class holds the data for an animation curve for a 
///         single floating point attribute.
/// \deprecated This class has been deprecated in favour of the 
///          XAnimationTake class. This class no only exists to provide
///          backwards compatibility to old data files.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XAnimCurve  
{
#ifndef DOXYGEN_PROCESSING
  friend class XMD_EXPORT XAnimCurveSet;
  friend class XMD_EXPORT XAnimCycle;
protected:

  /// ctor
  XAnimCurve(const XBase* node, const XString& attr_name);

  /// ctor
  XAnimCurve();

  /// \brief  ctor
  /// \param  rhs - the curve to copy
  XAnimCurve(const XAnimCurve& rhs);

  /// dtor
  ~XAnimCurve();
#endif
public:

  /// \brief  returns the name of the animation curve
  /// \return the ascii name of the curve
  const XChar* GetName() const;

  /// \brief  returns the name of the animated attribute
  /// \return the attribute name as a string
  const XChar* GetAttrName() const;

  /// \brief  returns the list of key-frames for this curve
  /// \return reference to the key-frame data
  const XKeyFrameList& GetKeys() const;

  /// \brief  inserts a key-frame into the Animation Curve
  /// \param  t  -  the time value
  /// \param  k  -  the key-frame value
  /// \param  i  -  the in tangent
  /// \param  o  -  the output tangent
  void AddKey(XReal t, XReal k, XReal i, XReal o);

  /// \brief  evaluates the curve at the specified time interval
  /// \param  time  -  the time at which to evaluate the curve
  /// \return the evaluated value
  XReal Evaluate(XReal time) const;

protected:
#ifndef DOXYGEN_PROCESSING

  bool DoData(XFileIO&);
  XU32 GetDataSize() const;
  bool WriteChunk(std::ostream&);
  bool ReadChunk(std::istream& ifs);

private:
  /// the name of the animation curve
  XString m_Name;
  /// the name of the attribute
  XString m_AttrName;
  /// the key-frames of that value
  XKeyFrameList m_Keys;
#endif
};
/// an array of animation curves
typedef XM2::pod_vector<XAnimCurve*> XAnimCurveList;

//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class holds all animation curve for a specific node.
///         Hopefully this will help to estimate what the likely values
///         represent.
/// \deprecated This class has been deprecated in favour of the 
///          XAnimationTake class. This class no only exists to provide
///          backwards compatibility to old data files.
class XMD_EXPORT XAnimCurveSet
  : public XAnimCurveList
{
#ifndef DOXYGEN_PROCESSING
  friend class XAnimCycle;

protected:

  /// \brief  ctor
  /// \param  model - the model the anim belongs to
  XAnimCurveSet(XModel* model);

  /// \brief  copy ctor
  /// \param  rhs - the anim curve set to copy
  XAnimCurveSet(const XAnimCurveSet& rhs);

  /// \brief  ctor
  XAnimCurveSet();

  /// \brief  ctor
  XAnimCurveSet(XModel*, XId node);

  /// \brief  dtor
  ~XAnimCurveSet();
#endif
public:

  /// \brief  returns the number of curves for the node
  /// \return the number of curves applied to the Animated Node
  XU32 GetNumCurves() const;

  /// \brief  this allows you to query the requested animation curve.
  /// \param  idx  -  the anim curve index
  /// \return a reference to the anim curve or NULL
  const XAnimCurve* GetCurve(XU32 idx) const;

  /// \brief  this allows you to query the requested animation curve.
  /// \param  idx  -  the anim curve index
  /// \return a reference to the anim curve or NULL
  XAnimCurve* GetCurve(XU32 idx);

  /// \brief  returns a pointer to the animated node
  /// \return a pointer to the node
  const XBase* GetNode() const;

protected:
#ifndef DOXYGEN_PROCESSING
  bool WriteChunk(std::ostream&);
  bool ReadChunk(std::istream&);
  bool DoData(XFileIO&);
  XU32 GetDataSize() const;
private:
  /// the name of the node?!?
  XString m_Name;
  /// the ID of the node 
  XId m_NodeID;
  /// a pointer to the model
  mutable XModel* m_pModel;
#endif
};
/// an array of animation curves attached to specific nodes.
typedef XM2::pod_vector<XAnimCurveSet*> XAnimCurveSetList;


//----------------------------------------------------------------------------------------------------------------------
/// \brief  This structure holds the sampled key frames for a bone.
/// \deprecated This class has been deprecated in favour of the 
///          XAnimationTake class. This class no only exists to provide
///          backwards compatibility to old data files.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XSampledKeys
{
  friend class XAnimCycle;
public:

  /// \brief  this function sets the number of key-frames in this struct
  /// \param  sz  -  the number of key frames
  void SetSize(XU32 sz);

  /// \brief  returns a reference to the internal scale keys
  /// \return a reference to the scale keys
  XVector3Array& ScaleKeys();

  /// \brief  returns a reference to the internal translation keys
  /// \return a reference to the scale keys
  XVector3Array& TranslationKeys();

  /// \brief  returns a reference to the internal rotation keys
  /// \return a reference to the scale keys
  XQuaternionArray& RotationKeys();

  /// \brief  returns a reference to the internal scale keys
  /// \return a reference to the scale keys
  const XVector3Array& ScaleKeys() const;

  /// \brief  returns a reference to the internal translation keys
  /// \return a reference to the scale keys
  const XVector3Array& TranslationKeys() const;

  /// \brief  returns a reference to the internal rotation keys
  /// \return a reference to the scale keys
  const XQuaternionArray& RotationKeys() const;

private:
#ifndef DOXYGEN_PROCESSING

  bool WriteChunk(std::ostream&);
  bool ReadChunk(std::istream&);
  bool DoData(XFileIO&);
  XU32 GetDataSize() const;

  /// \brief  dtor
  ~XSampledKeys();

  /// \brief  ctor
  XSampledKeys();

  /// \brief  copy ctor
  XSampledKeys(const XSampledKeys& sk);

  /// the sampled key frames for the scale
  XVector3Array m_ScaleKeys;

  /// the sampled key frames for the translation
  XVector3Array m_TranslationKeys;

  /// the sampled key frames for the rotation
  XQuaternionArray m_RotationKeys;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
///  \brief  This class contains all information needed for an animation
///          cycle. This includes sampled bone animation, as well as any
///          anim curves for custom node params
/// \deprecated This class has been deprecated in favour of the 
///          XAnimationTake class. This class no only exists to provide
///          backwards compatibility to old data files.
class XMD_EXPORT XAnimCycle
  : public XBase
{
  XMD_CHUNK(XAnimCycle);
public:


  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kStartFrame      = XBase::kLast+1; // XReal
  static const XU32 kEndFrame        = XBase::kLast+2; // XReal
  static const XU32 kFramesPerSecond = XBase::kLast+3; // XReal
  static const XU32 kNumFrames       = XBase::kLast+4; // XU32
  static const XU32 kLast            = kNumFrames;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Animation Evaluation Functions
  /// \brief  Functions to set the current XModel to a specified frame,
  ///         evaluate at a point in a cycle, or sample the currently
  ///         set transforms.
  //@{

  /// \brief  sets the model t be at the specified frame within this anim
  void SetToFrame(XU32 frame_index);

  /// \brief  uses the current transform values for the model and stores 
  ///         those values as the specified frame in this anim cycle.
  void UpdateFrame(XU32 frame_index);

  /// \brief  evaluates the bone at the specified time.
  /// \param  bone  -  the ID of the bone to calculate
  /// \param  out    -  the output transform
  /// \param  t    -  the animation time
  /// \return true if ok
  bool EvaluateAt(XId bone, XTransform& out, XReal t);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Sampled Bone Key Functions
  /// \brief  Functions to set sampled key frames on bones and derived
  ///         classes. 
  //@{

  /// \brief  Inserts a set of key frames into the Animation cycle. 
  /// \param  id  - the bone ID
  /// \return a new keys struct to fill with anim data
  XSampledKeys* AddSampledKeys(XId id);

  /// \brief  returns the sampled key-frames for the specified bone_id
  /// \param  bone_id  -  the ID of the bone to get the data from
  /// \return the key-frame data
  XSampledKeys* GetBoneKeys(XId bone_id);

  /// \brief  returns the sampled key-frames for the specified bone_id
  /// \param  bone  -  the bone to get the data from
  /// \return the key-frame data
  XSampledKeys* GetBoneKeys(const XBase* bone);

  /// \brief  returns the sampled key-frames for the specified bone_id
  /// \param  bone_id  -  the ID of the bone to get the data from
  /// \return the key-frame data
  const XSampledKeys* GetBoneKeys(XId bone_id) const;

  /// \brief  returns the sampled key-frames for the specified bone_id
  /// \param  bone  -  the bone to get the data from
  /// \return the key-frame data
  const XSampledKeys* GetBoneKeys(const XBase* bone) const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Animation Curve Functions
  /// \brief  functions to allow you to manipulate hermite based animation
  ///         curves on custom attributes.
  //@{

  /// \brief  This function returns a list of all animation curves
  ///         associated with a specific node ID
  /// \param  node_id  -  the id of the node to get the curves for
  /// \return A list of animation curves
  /// \note   Currently, the API does not specifically deal with attributes
  ///         as such. The names for the attributes will be the same as
  ///         those within Maya. It's up to you how you actually interpret
  ///         that data!
  const XAnimCurveSet* GetAnimCurvesForNode(XId node_id) const;

  /// \brief  This function returns a list of all animation curves
  ///         associated with a specific node ID
  /// \param  node_id  -  the id of the node to get the curves for
  /// \return A list of animation curves
  /// \note   Currently, the API does not specifically deal with attributes
  ///         as such. The names for the attributes will be the same as
  ///         those within Maya. It's up to you how you actually interpret
  ///         that data!
  XAnimCurveSet* GetAnimCurvesForNode(XId node_id);

  /// \brief  Allows you to create an animation curve for the specified
  ///         attribute on the node for this cycle. 
  /// \param  node - the node to attach the animation to
  /// \param  attr_name - the name of the attribute to animate, e.g.
  ///         "tx", "colorg", etc
  /// \return the newly created animation curve
  XAnimCurve* AddAnimCurve(const XBase* node, const XString& attr_name);

  /// \brief  Deletes the specified animation curve from the animation
  ///         cycle. 
  /// \param  node - the node on which the anim curve is attached
  /// \param  attr_name - the name of the animated attribute to which
  ///         the curve is attached.
  void DeleteAnimCurve(const XBase* node, const XString& attr_name);

  /// \brief  Deletes the specified animation curve from the animation
  ///         cycle. 
  /// \param  node - the node on which the anim curve is attached
  /// \param  curve - the animation curve to delete
  void DeleteAnimCurve(const XBase* node, XAnimCurve* curve);

  /// \brief  this function returns a list of the nodes that actually
  ///         have animation curves attached.
  /// \param  nodes  -  a list of indices
  void GetAnimatedNodes(XIndexList& nodes) const;

  /// \brief  this function returns a list of the nodes that actually
  ///     have animation curves attached.
  /// \param  nodes  -  a list of indices
  void GetSampledNodes(XIndexList& nodes) const;

  /// \brief  When loading an animation cycle and trying to merge it
  ///         with an existing model. It is likely that the node ID's
  ///         will be very different. To handle this - this function is
  ///         called after loading the file and provides a map between
  ///         the file ID's and the ID's on the actual model.
  /// \param  remap  -  a map between the original and re-mapped ID's
  void RemapAnimNodes(std::map<XId, XId>& remap);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Time Manipulation Functions
  /// \brief  functions to allow you to set or get information relating
  ///         to the length of the cycle.
  //@{

  /// \brief  returns the start time of the anim
  /// \return the anim start (in secs)
  XReal GetStartTime() const;

  /// \brief  returns the end time of the anim
  /// \return the anim end (in secs)
  XReal GetEndTime() const;

  /// \brief  returns the end time increment for the anim
  /// \return the time increment between frames (in secs)
  XReal GetTimeInc() const;

  /// \brief  this function returns the number of frames per second
  ///         for this cycle
  /// \return returns the number of frames per second
  XReal GetFramesPerSecond() const;

  /// \brief  this function returns the total number of frames 
  ///         for this cycle
  /// \return returns the number of frames in the cycle
  XU32 GetNumFrames() const;

  /// \brief  Sets the start/end times, and the number of frames
  /// \param  start_  - the start time (in frames)
  /// \param  end_  - the end time (in frames)
  /// \param  nFrames - the number of frames
  void SetFrameTimes(XReal start_, XReal end_, XU32 nFrames);

  /// \brief  sets the number of frames per second of the anim cycle.
  /// \param  frame_rate - the number of frames per second. 
  void SetFrameRate(XReal frame_rate);

  //@}

private:
#ifndef DOXYGEN_PROCESSING

  /// the start time of the animation
  XReal m_fStart;

  /// the end time of the animation
  XReal m_fEnd;

  /// the number of frames in the animation
  XU32 m_iNumFrames;

  /// the number of frames in the animation
  XReal m_iFramesPerSecond;

  /// the animation curves
  XAnimCurveSetList m_AnimCurves;

  /// the key-frames for the sampled bones
  std::map<XId,XSampledKeys*> m_BoneAnims;
#endif
};
/// \brief  An array of animation cycles
typedef XM2::pod_vector<XAnimCycle*> XAnimList;

#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::AnimCycle, XAnimCycle, "animCycle", "ANIM_CYCLE");
#endif
}
