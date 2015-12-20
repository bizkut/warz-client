//----------------------------------------------------------------------------------------------------------------------
/// \file   PoseStream.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the class XPoseStream to hold a posed key (i.e. a value
///         of a parameter that does not animate).            
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/AnimationStream.h"
#include "XM2/Vector4.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
class XMD_EXPORT XBase;
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XPoseStream
/// \brief A pose stream holds a single value for an animated attribute.
///        It indicates that the attributes value does not change over the
///        course of an animation take. However, since the value is flagged 
///        as an animated attribute, it's value may differ between animation
///        takes. This class will not store a pose key of an array attribute. 
///        It was felt that doing so would cause a nasty bloat to ensue
///        in the exported files. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XPoseStream
  : public XAnimationStream
{
public:

  /// \brief  dtor
  ~XPoseStream();

    //-------------------------------------------------------------------
    /// \brief  ctor
    /// 
    XPoseStream(XAnimatedAttribute* attr,XBase* node, StreamDataType dataType);

  /// \brief  returns the pose data as a quaternion 
  /// \return the pose key as a quaternion
  XQuaternion AsQuaternion() const;

  /// \brief  returns the pose data as a vector2 
  /// \return the pose key as a quaternion
  XVector2 AsVector2() const;

  /// \brief  returns the pose data as a vector3 
  /// \return the pose key as a quaternion
  XVector3 AsVector3() const;

  /// \brief  returns the pose data as a vector4 
  /// \return the pose key as a quaternion
  XVector4 AsVector4() const;

  /// \brief  returns the pose data as a colour3 
  /// \return the pose key as a quaternion
  XColour AsColour3() const;

  /// \brief  returns the pose data as a colour4 
  /// \return the pose key as a quaternion
  XColour AsColour4() const;

  /// \brief  returns the pose data as a float 
  /// \return the pose key as a quaternion
  XReal AsFloat() const;

  /// \brief  returns the pose data as a integer 
  /// \return the pose key as a quaternion
  XS32 AsInteger() const;

  /// \brief  returns the pose data as a boolean 
  /// \return the pose key as a quaternion
  bool AsBoolean() const;

  /// \brief  sets the pose value as a quaternion
  /// \param  value - the new value of the pose stream 
  bool Set(const XQuaternion& value);

  /// \brief  sets the pose value as a vector 2
  /// \param  value - the new value of the pose stream 
  bool Set(const XVector2& value);

  /// \brief  sets the pose value as a vector 3
  /// \param  value - the new value of the pose stream 
  bool Set(const XVector3& value);

  /// \brief  sets the pose value as a vector 4
  /// \param  value - the new value of the pose stream 
  bool Set(const XVector4& value);

  /// \brief  sets the pose value as a colour
  /// \param  value - the new value of the pose stream 
  bool Set(const XColour& value);

  /// \brief  sets the pose value as a float
  /// \param  value - the new value of the pose stream 
  bool Set(const XReal& value);

  /// \brief  sets the pose value as a integer
  /// \param  value - the new value of the pose stream 
  bool Set(const XS32& value);

  /// \brief  sets the pose value as a boolean
  /// \param  value - the new value of the pose stream 
  bool Set(const bool& value);

protected:

  XU32 GetDataSize() const;
  void Evaluate(XReal time, XReal* output) const;
  XU32 GetPoseDataSize() const;
  bool DoData(XFileIO& io);
  bool ReadChunk(std::istream&);
  bool WriteChunk(std::ostream&);
  /// only used when the pose stream is for an array type
  XM2::XU32 m_NumElements;
  /// the internal data storage for the posed key
  XM2::XReal* m_PoseData;
};
}
