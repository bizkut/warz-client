//----------------------------------------------------------------------------------------------------------------------
/// \file   SampledStream.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  The XSampledStream class stores a stream of values at a given
///         time interval for a specific value.   
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include <map>
#include "XM2/Vector4.h"
#include "XM2/Quaternion.h"
#include "XM2/Transform.h"
#include "XM2/RealArray.h"
#include "XMD/AnimationStream.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
using XM2::XRealArray;
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XSampledStream
/// \brief stores a set of sampled key frames for a specific attribute
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XSampledStream
  : public XAnimationStream
{
  friend class XMD_EXPORT XAnimatedAttribute;
  XSampledStream(XAnimatedAttribute* attr, XU32 elementSize, XU32 stream_size);
  XSampledStream(XAnimatedAttribute* attr);

public:

  /// \brief  returns the number of elements in the samples array 
  ///         (i.e. the number of keys)
  /// \return the number of samples in the stream
  XU32 GetNumElemtents() const;

  /// \brief  returns the number of (floating point) elements in the stream
  /// \return the number of elements per sample
  XU32 GetElemtentSize() const;

  /// \brief  returns the requested element as a Quaternion
  /// \param  index - the index to return
  /// \return the data as a Quaternion
  XQuaternion AsQuaternion(XU32 index) const;

  /// \brief  returns the requested element as a Vector2
  /// \param  index - the index to return
  /// \return the data as a Vector2
  XVector2 AsVector2(XU32 index) const;

  /// \brief  returns the requested element as a Vector3
  /// \param  index - the index to return
  /// \return the data as a Vector3
  XVector3 AsVector3(XU32 index) const;

  /// \brief  returns the requested element as a Vector4
  /// \param  index - the index to return
  /// \return the data as a Vector4
  XVector4 AsVector4(XU32 index) const;

  /// \brief  returns the requested element as a Colour3
  /// \param  index - the index to return
  /// \return the data as a Colour3
  XColour AsColour(XU32 index) const;

  /// \brief  returns the requested element as a raw float ptr
  /// \param  index - the index to return
  /// \return the data as a raw float ptr
  const XReal* AsRawData(XU32 index) const;

  /// \brief  returns the requested element as a Boolean
  /// \param  index - the index to return
  /// \return the data as a Boolean
  bool AsBoolean(XU32 index) const;

  /// \brief  returns the requested element as an Integer
  /// \param  index - the index to return
  /// \return the data as a Integer
  XS32 AsInteger(XU32 index) const;

  /// \brief  returns the requested element as a float
  /// \param  index - the index to return
  /// \return the data as a float
  XReal AsReal(XU32 index) const;

  /// \brief  sets the specified element as a quaternion
  /// \param  index - the element to set
  /// \param  value - the value to assign
  /// \return true if OK
  bool SetSample(XU32 index, const XQuaternion& value);

  /// \brief  sets the specified element as a vector2
  /// \param  index - the element to set
  /// \param  value - the value to assign
  /// \return true if OK
  bool SetSample(XU32 index, const XVector2& value);

  /// \brief  sets the specified element as a vector3
  /// \param  index - the element to set
  /// \param  value - the value to assign
  /// \return true if OK
  bool SetSample(XU32 index, const XVector3& value);

  /// \brief  sets the specified element as a vector4
  /// \param  index - the element to set
  /// \param  value - the value to assign
  /// \return true if OK
  bool SetSample(XU32 index, const XVector4& value);

  /// \brief  sets the specified element as a colour3
  /// \param  index - the element to set
  /// \param  value - the value to assign
  /// \return true if OK
  bool SetSample(XU32 index, const XColour& value);

  /// \brief  sets the specified element from raw data
  /// \param  index - the element to set
  /// \param  value - the value to assign
  /// \return true if OK
  bool SetSample(XU32 index, const XReal* raw_data);

  /// \brief  sets the specified element as a boolean
  /// \param  index - the element to set
  /// \param  value - the value to assign
  /// \return true if OK
  bool SetSample(XU32 index, bool value);

  /// \brief  sets the specified element as an integer
  /// \param  index - the element to set
  /// \param  value - the value to assign
  /// \return true if OK
  bool SetSample(XU32 index, XS32 value);

  /// \brief  sets the specified element as a float
  /// \param  index - the element to set
  /// \param  value - the value to assign
  /// \return true if OK
  bool SetSample(XU32 index, XReal value);

private:

  XU32 GetDataSize() const;
  bool DoData(XFileIO& io);
  bool ReadChunk(std::istream&);
  bool WriteChunk(std::ostream&);
  void Evaluate(XReal time,XReal* output) const;
  /// the sampled 
  XU32 m_ElementsPerSample;
  /// the sampled data in the stream
  XRealArray m_Samples;
};
}
