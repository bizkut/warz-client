//----------------------------------------------------------------------------------------------------------------------
/// \file   AnimationStream.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
#include "XM2/vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
class XMD_EXPORT XAnimatedAttribute;
class XMD_EXPORT XFileIO;
class XMD_EXPORT XSampledStream;
class XMD_EXPORT XFCurveStream;
class XMD_EXPORT XPoseStream;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XAnimationStream
/// \brief an animation stream holds the animation data for a specific 
///        attribute contained within an animation take. For example, 
///        this may be an XBone's translate (Vec3), or an XPhong's 
///        shininess (float). 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XAnimationStream
{
  friend class XMD_EXPORT XAnimatedAttribute;
public:

  enum StreamType
  {
    /// the stream consists of a single pose key for the anim
    kPoseStream,

    /// the stream consists of a sampled set of data
    kSampledStream,

    /// the stream consists of a set of FCurves
    kFCurveStream
  };

  /// \brief  enum to determine the type of data held in the stream, and
  ///         give an indication to XMD on how to interpolate the data. 
  enum StreamDataType 
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
    kQuaternion,

    /// not sure about this. Should probably support it due
    /// to the extra attr mechanism. definitely one to slerp.
    kMatrix,

    /// RGB colour type to be linearly interpolated
    kColour3 = kVector3,

    /// RGBA colour type to be linearly interpolated
    kColour4 = kVector4,

    /// kRealArray
    kRealArray=kMatrix+1,

    /// kVector3Array stream type
    kVector3Array,

    kInvalid 
  };

  /// \brief  evaluates the stream at the specified time
  /// \param  time - the time to evaluate the stream at
  /// \param  output - the output value
  void Evaluate(XReal time, XReal& output) const;

  /// \brief  evaluates the stream at the specified time
  /// \param  time - the time to evaluate the stream at
  /// \param  output - the output value
  void Evaluate(XReal time, XVector2& output) const;

  /// \brief  evaluates the stream at the specified time
  /// \param  time - the time to evaluate the stream at
  /// \param  output - the output value
  void Evaluate(XReal time, XVector3& output) const;

  /// \brief  evaluates the stream at the specified time
  /// \param  time - the time to evaluate the stream at
  /// \param  output - the output value
  void Evaluate(XReal time, XVector4& output) const;

  /// \brief  evaluates the stream at the specified time
  /// \param  time - the time to evaluate the stream at
  /// \param  output - the output value
  void Evaluate(XReal time, XQuaternion& output) const;

  /// \brief  evaluates the stream at the specified time
  /// \param  time - the time to evaluate the stream at
  /// \param  output - the output value
  void Evaluate(XReal time, XMatrix& output) const;

  /// \brief  evaluates the stream at the specified time
  /// \param  time - the time to evaluate the stream at
  /// \param  output - the output value
  void Evaluate(XReal time, XColour& output) const;

  /// \brief  returns the time of the first key-frame
  /// \return the time of the first key-frame
  virtual XReal GetMinTime() const;

  /// \brief  returns the time of the last key-frame
  /// \return the time of the last key-frame
  virtual XReal GetMaxTime() const;

  /// \brief  returns the type of data stored in the stream
  /// \return the stream type
  StreamDataType GetStreamDataType() const;

  /// \brief  returns the type of this animation data stream
  /// \return the stream type
  StreamType GetStreamType() const;

  /// \brief  returns the ID of the XMD node that this stream relates to
  /// \return the Node ID
  XId GetNodeID() const;

  /// \brief  returns the ID of the XMD property that this stream animates
  /// \return the property ID
  XU32 GetPropertyID() const;

  /// \brief  returns the ID of the XMD property that this stream animates
  /// \return the property ID
  XU32 GetNumArrayElements() const;

  /// \brief  returns the stream as a pose stream
  /// \return the stream as a pose stream or null if not a pose stream.
  XPoseStream* AsPoseStream();

  /// \brief  returns the stream as a pose stream
  /// \return the stream as a pose stream or null if not a pose stream.
  XSampledStream* AsSampledStream();

  /// \brief  returns the stream as a pose stream
  /// \return the stream as a pose stream or null if not a pose stream.
  XFCurveStream* AsFCurveStream();

  /// \brief  returns the stream as a pose stream
  /// \return the stream as a pose stream or null if not a pose stream.
  const XPoseStream* AsPoseStream() const;

  /// \brief  returns the stream as a pose stream
  /// \return the stream as a pose stream or null if not a pose stream.
  const XSampledStream* AsSampledStream() const;

  /// \brief  returns the stream as a pose stream
  /// \return the stream as a pose stream or null if not a pose stream.
  const XFCurveStream* AsFCurveStream() const;

protected:

  /// \brief  returns the animated attribute
  /// \return the animated attribute 
  XAnimatedAttribute* GetAttribute();

  /// \brief  returns the animated attribute
  /// \return the animated attribute 
  const XAnimatedAttribute* GetAttribute() const;

  /// \brief  returns the number of elements in the stream type
  static XU32 GetStreamTypeNumElements(StreamDataType type);

protected:

  virtual ~XAnimationStream() {};
  XAnimationStream(XAnimatedAttribute* parent);

  virtual XU32 GetDataSize() const;
  virtual void Evaluate(XReal time, XReal* output) const =0;
  virtual bool DoData(XFileIO& io);
  virtual bool ReadChunk(std::istream&);
  virtual bool WriteChunk(std::ostream&);

protected:

  /// returns the type of this stream
  StreamType m_StreamType;

  /// returns the type of stream data contained
  StreamDataType m_StreamDataType;

  /// pointer to the attribute this stream animates
  XAnimatedAttribute* m_ParentAttr;

};
typedef XM2::pod_vector<XAnimationStream*> XAnimationStreamArray;
}
