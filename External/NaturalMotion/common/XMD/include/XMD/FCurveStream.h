//----------------------------------------------------------------------------------------------------------------------
/// \file   FCurveStream.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  The XAnimationTake class is intended as a replacement to the old XAnimCycle data node. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector2.h"
#include "XM2/Vector3.h"
#include "XM2/Vector4.h"
#include "XM2/Quaternion.h"
#include "XM2/Transform.h"
#include "XMD/AnimationStream.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XFCurve
/// \brief Holds the animation data for a single animation curve
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XFCurve
{
  friend class XMD_EXPORT XFCurveStream;
public:

  /// \brief ctor
  XFCurve() {}

  /// \brief dtor
  ~XFCurve() {}

  /// \brief  the interpolation scheme used by the curve
  enum InterpolationType
  {
    kStep,        ///< [0:time] [1:value]
    kLinear,      ///< [0:time] [1:value]
    kCatmullRom1D,///< [0:time] [1:value] 
    kCardinal1D,  ///< [0:time] [1:value] [2:tension]
    kTCB1D,       ///< [0:time] [1:value] [2:tension] [3:continuity] [4:bias]
    kBezier1D,    ///< [0:time] [1:value] [2:in_value] [3:out_value]
    kBezier2D,    ///< [0:time] [1:value] [2:in_valueX] [3:in_valueY] [4:out_valueX] [5:out_valueY]
    kHermite1D,   ///< [0:time] [1:value] [2:in_tan] [3:out_tan]
    kHermite2D,   ///< [0:time] [1:value] [2:in_tanX] [3:in_tanY] [4:out_tanX] [5:out_tanY]
    kCustom,

    kCatmullRom2D,///< [0:time] [1:valueX] [2:valueY] 
    kCardinal2D,  ///< [0:time] [1:valueX] [1:valueY] [2:tension]
    kTCB2D,       ///< [0:time] [1:valueX] [1:valueY] [2:tension] [3:continuity] [4:bias]
    kSmoothStep,  ///< [0:time] [1:valueX] [2:threshold] 
    
    // backwards compatibility
    kCatmullRom=kCatmullRom1D,
    kCardinal=kCardinal1D,  
    kTCB=kTCB1D
  };

  /// \brief  evaluates the F-Curve at the requested time value. 
  /// \param  time - the time value to evaluate the curve at. 
  /// \param  result - the returned curve value. 
  /// \return true if OK
  /// \note   This function is a work in progress at the moment. A 
  ///         number of optimisations are still needed behind the scenes, 
  ///         so until they are done, be aware that this by no means optimal.
  bool Evaluate(XReal time, XReal& result) const;

  /// \brief  adds a new key-frame to the FCurve
  /// \param  time - the time value to set
  /// \param  value - the value of the keyed value
  /// \return the index of the new key-frame. Useful if you want to 
  ///         set additional data such as tangents.
  XU32 AddKeyFrame(XReal time, XReal value);

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   common to all FCurve types
  XReal GetKeyTime(XU32 idx) const;

  /// \brief  returns the value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame value
  /// \note   common to all FCurve types
  XReal GetKeyValue(XU32 idx) const;

  /// \brief  returns the tension value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame threshold value
  /// \note   only used when the curve type is XFCurve::kSmoothStep
  XReal GetKeyThreshold(XU32 idx) const;

  /// \brief  returns the tension value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame tension value
  /// \note   only used when the curve type is XFCurve::kCardinal, or XFCurve::kTCB
  XReal GetKeyTension(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kTCB
  XReal GetKeyContinuity(XU32 idx) const;

  /// \brief  returns the bias value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame bias value
  /// \note   only used when the curve type XFCurve::kTCB
  XReal GetKeyBias(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kBezier1D
  XReal GetKeyInValue(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kBezier2D
  XReal GetKeyInValueX(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kBezier2D
  XReal GetKeyInValueY(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kBezier1D
  XReal GetKeyOutValue(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kBezier2D
  XReal GetKeyOutValueX(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kBezier2D
  XReal GetKeyOutValueY(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kHermite1D
  XReal GetKeyInTangent(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kHermite2D
  XReal GetKeyInTangentX(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kHermite2D
  XReal GetKeyInTangentY(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kHermite1D
  XReal GetKeyOutTangent(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kHermite2D
  XReal GetKeyOutTangentX(XU32 idx) const;

  /// \brief  returns the time value of the requested key-frame
  /// \param  idx - the key-frame index
  /// \return the key-frame time value
  /// \note   only used when the curve type XFCurve::kHermite2D
  XReal GetKeyOutTangentY(XU32 idx) const;

  /// \brief  returns a pointer to the keyframe data
  /// \param  idx - the index of the keyframe to access
  /// \return a pointer to the key frame data
  const XReal* GetKeyData(XU32 idx) const;

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  t - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyTime(XU32 idx, XReal t);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyValue(XU32 idx, XReal v);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new threshold value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyThreshold(XU32 idx, XReal v);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyTension(XU32 idx, XReal t);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyContinuity(XU32 idx, XReal c);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyBias(XU32 idx, XReal b);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyInValue(XU32 idx, XReal v);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyInValueX(XU32 idx, XReal vx);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyInValueY(XU32 idx, XReal vy);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyOutValue(XU32 idx, XReal v);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyOutValueX(XU32 idx, XReal vx);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  v - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyOutValueY(XU32 idx, XReal vy);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  t - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyInTangent(XU32 idx, XReal t);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  tx - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyInTangentX(XU32 idx, XReal tx);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  ty - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyInTangentY(XU32 idx, XReal ty);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  t - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyOutTangent(XU32 idx, XReal t);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  tx - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyOutTangentX(XU32 idx, XReal tx);

  /// \brief  sets the key frames time value
  /// \param  idx - the index of the key to set
  /// \param  ty - the new time value for the key-frame
  /// \note   common to all FCurve types
  bool SetKeyOutTangentY(XU32 idx, XReal ty);

  /// \brief  returns the pre-infinity type. This defines the behavior of the curve
  ///         prior to the first key frame value
  /// \return the pre-infinity type
  InterpolationType GetInterpolationType() const;

  /// \brief  sets the interpolation type of the FCurve
  /// \param  type - the type of interpolation to use. 
  /// \note   This function will potentially invalidate the data stored in the key frame data. 
  void SetInterpolationType(InterpolationType type);

  /// \brief  the post and pre infinity type
  enum InfinityType
  {
    kInfinityConstant,
    kInfinityLinear,
    kInfinityOscillate,
    kInfinityCycle,
    kInfinityCycleWithOffset
  };

  /// \brief  returns the pre-infinity type. This defines the behavior of the curve
  ///         prior to the first key frame value
  /// \return the pre-infinity type
  InfinityType GetPreInfinity() const;

  /// \brief  returns the post-infinity type. This defines the behavior of the curve
  ///         prior to the first key frame value
  /// \return the post-infinity type
  InfinityType GetPostInfinity() const;


  /// \brief  returns the pre-infinity type. This defines the behavior of the curve
  ///         prior to the first key frame value
  /// \param  infinity_type - the pre infinity type
  void SetPreInfinity(InfinityType infinity_type);

  /// \brief  returns the post-infinity type. This defines the behavior of the curve
  ///         prior to the first key frame value
  /// \param  infinity_type - the pre infinity type
  void SetPostInfinity(InfinityType infinity_type);

  /// \brief  returns the total number of key-frames within the animation curve
  /// \return the number of key-frames in the curve
  XU32 GetNumKeyframes() const;

  /// \brief  returns the number of elements required for each key-frame value
  /// \return the number of elements per key value
  /// \note   This must be a minimum of 2, since at the very least the curve must have 
  ///         a time and value. 
  XU32 GetNumElementsPerKey() const;

  /// \brief  returns the time value at the first key-frame in the animation curve
  /// \return the time of the first frame
  XReal GetMinTime() const;

  /// \brief  returns the time value at the last key-frame in the animation curve
  /// \return the time of the last key-frame frame
  XReal GetMaxTime() const;

private:

  bool Read(std::istream&);
  bool Write(std::ostream&);
  XU32 GetDataSize() const;
  bool DoData(XFileIO& io);

  // F-Curve evaluation methods
  XReal EvaluateStep(XReal time) const;
  XReal EvaluateSmoothStep(XReal time) const;
  XReal EvaluateLinear(XReal time) const;
  XReal EvaluateCatmullRom1D(XReal time) const;
  XReal EvaluateCatmullRom2D(XReal time) const;
  XReal EvaluateCardinal1D(XReal time) const;
  XReal EvaluateCardinal2D(XReal time) const;
  XReal EvaluateTCB1D(XReal time) const;
  XReal EvaluateTCB2D(XReal time) const;
  XReal EvaluateBezier1D(XReal time) const;
  XReal EvaluateBezier2D(XReal time) const;
  XReal EvaluateHermite1D(XReal time) const;
  XReal EvaluateHermite2D(XReal time) const;
  bool EvaluateCurveValue(XReal &result, XReal time) const;

  // curve evaluation utilities
  void  FindStartAndEndKeys(XReal t, XU32& key_start, XU32& key_end) const;
  XReal Get1DCardinalTangent(XReal p0, XU32 key_index0) const;
  XReal Get1DCatmullTangent(XReal p0, XU32 key_index0) const;
  XReal Get1DTCBTangent(XReal p0, XU32 key_index0) const;
  void Get2DCardinalTangent(XU32 key_index0, XReal& x, XReal& y) const;
  void Get2DCatmullTangent(XU32 key_index0, XReal& x, XReal& y) const;
  void Get2DTCBInTangent(XU32 key_index0, XReal& x, XReal& y) const;
  void Get2DTCBOutTangent(XU32 key_index0, XReal& x, XReal& y) const;

  // bezier/hermite computation
  static XReal EvaluateBezier(XReal p0, XReal p1,XReal p2, XReal p3, XReal t);
  static XReal EvaluateHermite(XReal p0, XReal t0, XReal p1, XReal t1, XReal t);
  static XReal InvertBezier(XReal x, XReal x0, XReal x1, XReal x2, XReal x3);
  static XReal InvertBezierLong(XReal x, XReal x0, XReal x1, XReal x2, XReal x3);
  static XReal InvertHermite(XReal x, XReal x0, XReal t0, XReal x1, XReal t1);
  static XReal InvertHermiteLong(XReal x, XReal p0, XReal t0, XReal p1, XReal t1);

  XReal GetPreLinearInc() const;
  XReal GetPostLinearInc() const;

private:
  /// the component index (0->3) or -1 for no component
  XS32 m_Component;
  /// the array index (or -1 if the animated attr is not an array)
  XS32 m_Index;
  /// the interpolation scheme used by the curve
  InterpolationType m_InterpolationType;
  /// describes how the curve will behave before the first key frame
  InfinityType m_PreInfinity;
  /// describes how the curve will behave after the last key frame
  InfinityType m_PostInfinity;
  /// the total number of key-frames
  XU32 m_NumberKeyframes;
  /// the number of data elements per key frame
  XU32 m_ElementsPerKey;
  /// the key data
  XRealArray m_KeyFrameData;
};
typedef XM2::pod_vector<XFCurve*> XFCurveArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XFCurveStream
/// \brief An FCurve stream is used to hold animation data for an attribute as a set of animation curves. 
///        Typically, this will be held alongside a set of sampled keys. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XFCurveStream
  : public XAnimationStream
{
  friend class XMD_EXPORT XAnimatedAttribute;
public:
  XFCurveStream(XAnimatedAttribute* attr);

  /// \brief  returns true if the FCurve stream relates to an animated array.
  ///         If this is true, 
  bool GetIsAnimatedArray() const;

  /// \brief  returns true if the data stream has components (i.e. x,y,z)
  ///         If false, then the attr to be animated is a internal type,
  ///         i.e. float, int, bool etc. 
  /// \return true if components are used. 
  bool GetHasComponents() const;

  /// \brief  returns the animation curve for 
  /// 
  ///             node.attr
  /// 
  ///         This method assumes that there is only 1 FCurve that could be present
  XFCurve* GetCurve();

  /// \brief  returns the animation curve for 
  /// 
  ///             node.attr.component
  /// 
  ///         where component will be 
  /// 
  ///             0  =  x = r 
  ///             1  =  y = g 
  ///             2  =  z = b 
  ///             3  =  w = a 
  /// \param  component - 
  XFCurve* GetCurve(XU32 component);

  /// \brief  returns the animation curve for 
  /// 
  ///             node.attr[index].component
  /// 
  ///         where component will be 
  /// 
  ///             0  =  x = r 
  ///             1  =  y = g 
  ///             2  =  z = b 
  ///             3  =  w = a 
  /// 
  ///         and index is the array index of the element
  XFCurve* GetCurve(XU32 component, XU32 index);

  /// \brief  returns the animation curve for 
  /// 
  ///             node.attr
  /// 
  ///         This method assumes that there is only 1 FCurve that could
  ///         be present
  const XFCurve* GetCurve() const;

  /// \brief  returns the animation curve for 
  /// 
  ///             node.attr.component
  /// 
  ///         where component will be 
  /// 
  ///             0  =  x = r 
  ///             1  =  y = g 
  ///             2  =  z = b 
  ///             3  =  w = a 
  const XFCurve* GetCurve(XU32 component) const;

  /// \brief  returns the animation curve for 
  /// 
  ///             node.attr[index].component
  /// 
  ///         where component will be 
  /// 
  ///             0  =  x = r 
  ///             1  =  y = g 
  ///             2  =  z = b 
  ///             3  =  w = a 
  /// 
  ///         and index is the array index of the element
  const XFCurve* GetCurve(XU32 component, XU32 index) const;

private:
  virtual XU32 GetDataSize() const;
  virtual bool DoData(XFileIO& io);
  virtual bool ReadChunk(std::istream&);
  virtual bool WriteChunk(std::ostream&);
  virtual void Evaluate(XReal time, XReal* output) const;
  XFCurveArray m_FCurves;
};
}
