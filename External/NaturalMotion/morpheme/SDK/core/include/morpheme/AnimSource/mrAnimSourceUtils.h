// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_ANIM_SOURCE_UTILS_H
#define MR_ANIM_SOURCE_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "morpheme/mrRigToAnimMap.h"

#include "NMPlatform/NMvpu.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

/// Determines the maximum chunk size that animations will be divided into.  This figure is important in PS3 SPU
///  operation to ensure that anim sections will fit into local store.
///  It defaults to 4MiB, and the PS3 configuration in connect overrides it for required platforms.
extern uint32_t s_maxAnimSectionSize;

//----------------------------------------------------------------------------------------------------------------------
/// \brief Set the maximum size of a compressed anim section.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void setMaxAnimSectionSize(uint32_t maxAnimSectionSize)
{
  s_maxAnimSectionSize = maxAnimSectionSize;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Get the maximum size of a compressed anim section.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t getMaxAnimSectionSize()
{
  return s_maxAnimSectionSize;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::UnchangingKeyVec3
/// \brief A 3-vector corresponding to an unchanging data channel.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class UnchangingKeyVec3
{
public:
  UnchangingKeyVec3() {}
  ~UnchangingKeyVec3() {}

  void locate();
  void dislocate();

  NM_INLINE void zero();
  NM_INLINE void pack(const uint32_t* v);
  NM_INLINE void unpack(uint32_t* v) const;
  NM_INLINE void unpack(NMP::Vector3& v) const;

protected:
  uint16_t m_data[3];
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void UnchangingKeyVec3::zero()
{
  m_data[0] = m_data[1] = m_data[2] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void UnchangingKeyVec3::pack(const uint32_t* v)
{
  NMP_ASSERT(v[0] <= 0xFFFF);
  m_data[0] = (uint16_t)v[0];
  NMP_ASSERT(v[1] <= 0xFFFF);
  m_data[1] = (uint16_t)v[1];
  NMP_ASSERT(v[2] <= 0xFFFF);
  m_data[2] = (uint16_t)v[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void UnchangingKeyVec3::unpack(uint32_t* v) const
{
  v[0] = (uint32_t)m_data[0];
  v[1] = (uint32_t)m_data[1];
  v[2] = (uint32_t)m_data[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void UnchangingKeyVec3::unpack(NMP::Vector3& v) const
{
  v.set((float)m_data[0], (float)m_data[1], (float)m_data[2]);
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::SampledPosKey
/// \brief A Vector3 position quantised into 32 bits.
/// \ingroup SourceAnimationCompressedModule
///
/// Compressed in the following fashion:
///   - 10 bits,  (0 -  9 = Unsigned z component).
///   - 11 bits,  (10 - 20 = Unsigned y component).
///   - 11 bits,  (21 - 31 = Unsigned x component).
//----------------------------------------------------------------------------------------------------------------------
class SampledPosKey
{
public:
  SampledPosKey() {}
  ~SampledPosKey() {}

  void locate();
  void dislocate();
  
  NM_INLINE void zero();
  NM_INLINE void pack(const uint32_t* v);
  NM_INLINE void unpack(uint32_t* v) const;
  NM_INLINE void unpack(NMP::Vector3& v) const;

protected:
  uint32_t m_data;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledPosKey::zero()
{
  m_data = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledPosKey::pack(const uint32_t* v)
{
  NMP_ASSERT(v[0] <= 0x7FF); // 11 - bits
  NMP_ASSERT(v[1] <= 0x7FF); // 11 - bits
  NMP_ASSERT(v[2] <= 0x3FF); // 10 - bits 
  m_data = (v[0] << 21) | (v[1] << 10) | v[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledPosKey::unpack(uint32_t* v) const
{
  v[0] = ((m_data >> 21) & 0x7FF);  // 11 bits
  v[1] = ((m_data >> 10) & 0x7FF);  // 11 bits
  v[2] = (m_data & 0x3FF);   // 10 bits
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledPosKey::unpack(NMP::Vector3& v) const
{
  v.x = (float)((m_data >> 21) & 0x7FF);  // 11 bits
  v.y = (float)((m_data >> 10) & 0x7FF);  // 11 bits
  v.z = (float)(m_data & 0x3FF);   // 10 bits
  v.w = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::SampledQuatKeyTQA
/// \brief A tan quarter angle rotation vector representation of rotation quantised into 48 bits.
/// \ingroup SourceAnimationCompressedModule
///
/// Quaternions q = [cos(theta/2), sin(theta/2)*a] have a double covering of the rotational space,
/// i.e. q = -q. Where theta is the angle of rotation and a is the unit-vector axis of rotation.
/// A quaternion can therefore represent the set of rotations for angles in the range:
/// theta = [-2*pi : 2*pi], with angles theta = [-pi : pi] representing quaternions in the positive
/// hemisphere w >= 0, and the remaining angles the negative hemisphere w < 0.
///
/// A tan quarter angle rotation vector has the advantage that it can encode a representation of
/// a quaternion that lies in the positive hemisphere (w >= 0) into a three-vector: tan(theta/4) * a
/// Since theta = [-pi : pi], tan(theta/4) = [-1 : 1] and is actually pretty linear in this range,
/// certainly more so than sin(theta/2).
///
/// If we quantise the values [-1 : 1] of a tan quarter angle rotation vector, (i.e. represent
/// the theta angles corresponding to the set of evenly sampled tan(theta/4) plot values), then
/// because of the function's near linearity the representable resolution of angles is accurate
/// over the entire range (good quantisation). This is in contrast to sin(theta/2), which is a lot
/// less linear and in fact has zero function gradient at theta = -pi or pi, resulting in good
/// angle resolution near theta = 0, but increasingly poorer resolution towards -pi and pi.
///
/// Importantly, conversion from tan quarter angle rotation vector back to a quaternion in the
/// positive hemisphere requires no trigonometric functions or square roots. Nor does the
/// resulting quaternion need to be re-normalised to unit length since the math guarantees a
/// unit quaternion.
///
/// When SLERPing between two quat keyframes, we interpolate along the shortest arc of the great
/// circle. Since we can only represent quaternions in the positive hemisphere, shortest arc
/// interpolation is achieved by computing the dot product between the two keyframes and just
/// negating one of them if the dot was negative).
//----------------------------------------------------------------------------------------------------------------------
class SampledQuatKeyTQA
{
public:
  SampledQuatKeyTQA() {}
  ~SampledQuatKeyTQA() {}

  void locate();
  void dislocate();
  
  NM_INLINE void zero();
  NM_INLINE void pack(const uint32_t* v);
  NM_INLINE void unpack(uint32_t* v) const;
  NM_INLINE void unpack(NMP::Vector3& v) const;

protected:
  uint16_t m_data[3];
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledQuatKeyTQA::zero()
{
  m_data[0] = m_data[1] = m_data[2] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledQuatKeyTQA::pack(const uint32_t* v)
{
  NMP_ASSERT(v[0] <= 0xFFFF);
  m_data[0] = (uint16_t)v[0];
  NMP_ASSERT(v[1] <= 0xFFFF);
  m_data[1] = (uint16_t)v[1];
  NMP_ASSERT(v[2] <= 0xFFFF);
  m_data[2] = (uint16_t)v[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledQuatKeyTQA::unpack(uint32_t* v) const
{
  v[0] = (uint32_t)m_data[0];
  v[1] = (uint32_t)m_data[1];
  v[2] = (uint32_t)m_data[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledQuatKeyTQA::unpack(NMP::Vector3& v) const
{
  v.set((float)m_data[0], (float)m_data[1], (float)m_data[2]);
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::QuantisationScaleAndOffsetVec3
/// \brief Uniform quantisation range information for 3-vector channels.
/// \ingroup SourceAnimationCompressedModule
///
/// A value x can be quantised within the range [qMin, qMax] with p bits of precision:
/// u = ((x - qMin) / (qMax - qMin)) * (2^p - 1)
///
/// Correspondingly a sample u can be dequantised as:
/// x = u * ((qMax - qMin) / (2^p - 1)) + qMin
///
/// For fast dequantisation this structure encodes scale and offset coefficients
//----------------------------------------------------------------------------------------------------------------------
class QuantisationScaleAndOffsetVec3
{
public:
  QuantisationScaleAndOffsetVec3() {}
  ~QuantisationScaleAndOffsetVec3() {}

  void locate();
  void dislocate();
  
  NM_INLINE const float* getQuantisationSetOffset() const;
  NM_INLINE float getQuantisationSetOffsetX() const;
  NM_INLINE float getQuantisationSetOffsetY() const;
  NM_INLINE float getQuantisationSetOffsetZ() const;
  
  NM_INLINE const float* getQuantisationSetScale() const;
  NM_INLINE float getQuantisationSetScaleX() const;
  NM_INLINE float getQuantisationSetScaleY() const;
  NM_INLINE float getQuantisationSetScaleZ() const;

  NM_INLINE void zero();
  NM_INLINE void pack3(const float* v);
  NM_INLINE void pack4(const float* v);
  NM_INLINE void unpack4(float* v) const;
  NM_INLINE void pack(const NMP::Vector3& qScale, const NMP::Vector3& qOffset);
  NM_INLINE void unpack(NMP::Vector3& qScale, NMP::Vector3& qOffset) const;

protected:
  float m_qOffset[3];
  float m_qScale[3];
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* QuantisationScaleAndOffsetVec3::getQuantisationSetOffset() const
{
  return (const float*) m_qOffset;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float QuantisationScaleAndOffsetVec3::getQuantisationSetOffsetX() const
{
  return m_qOffset[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float QuantisationScaleAndOffsetVec3::getQuantisationSetOffsetY() const
{
  return m_qOffset[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float QuantisationScaleAndOffsetVec3::getQuantisationSetOffsetZ() const
{
  return m_qOffset[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const float* QuantisationScaleAndOffsetVec3::getQuantisationSetScale() const
{
  return (const float*) m_qScale;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float QuantisationScaleAndOffsetVec3::getQuantisationSetScaleX() const
{
  return m_qScale[0];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float QuantisationScaleAndOffsetVec3::getQuantisationSetScaleY() const
{
  return m_qScale[1];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float QuantisationScaleAndOffsetVec3::getQuantisationSetScaleZ() const
{
  return m_qScale[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationScaleAndOffsetVec3::zero()
{
  m_qScale[0] = m_qScale[1] = m_qScale[2] = 0.0f;
  m_qOffset[0] = m_qOffset[1] = m_qOffset[2] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationScaleAndOffsetVec3::pack3(const float* v)
{
  float* ptr = (float*)this;
  ptr[0] = v[0];
  ptr[1] = v[1];
  ptr[2] = v[2];
  ptr[3] = 0.0f;
  ptr[4] = 0.0f;
  ptr[5] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationScaleAndOffsetVec3::pack4(const float* v)
{
  float* ptr = (float*)this;
  ptr[0] = v[0];
  ptr[1] = v[1];
  ptr[2] = v[2];
  ptr[3] = v[3];
  ptr[4] = 0.0f;
  ptr[5] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationScaleAndOffsetVec3::unpack4(float* v) const
{
  const float* ptr = (float*)this;
  v[0] = ptr[0];
  v[1] = ptr[1];
  v[2] = ptr[2];
  v[3] = ptr[3];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationScaleAndOffsetVec3::pack(const NMP::Vector3& qScale, const NMP::Vector3& qOffset)
{
  m_qScale[0] = qScale.x;
  m_qScale[1] = qScale.y;
  m_qScale[2] = qScale.z;
  m_qOffset[0] = qOffset.x;
  m_qOffset[1] = qOffset.y;
  m_qOffset[2] = qOffset.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationScaleAndOffsetVec3::unpack(NMP::Vector3& qScale, NMP::Vector3& qOffset) const
{
  qScale.set(m_qScale[0], m_qScale[1], m_qScale[2]);
  qOffset.set(m_qOffset[0], m_qOffset[1], m_qOffset[2]);
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::QuantisationMeanAndSetVec3
/// \brief Uniform quantisation mean and set data for 3-vector channels.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class QuantisationMeanAndSetVec3
{
public:
  QuantisationMeanAndSetVec3() {}
  ~QuantisationMeanAndSetVec3() {}

  void locate();
  void dislocate();

  NM_INLINE const uint8_t* getQuantisedMean() const;
  NM_INLINE const uint8_t* getQuantisationSet() const;
  
  NM_INLINE void zero();
  NM_INLINE void pack(const uint32_t* qMean, const uint32_t* qSet);
  NM_INLINE void unpack(uint32_t* qMean, uint32_t* qSet) const;
  
  NM_INLINE void unpackQMean(NMP::Vector3& qMean) const;
  NM_INLINE void unpackQSet(uint32_t* qSet) const;

protected:
  uint8_t     m_qMean[3];   ///< Coarse estimate for the mean of the channel data
  uint8_t     m_qSet[3];    ///< Quantisation set assignments
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint8_t* QuantisationMeanAndSetVec3::getQuantisedMean() const
{
  return (const uint8_t*) m_qMean;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint8_t* QuantisationMeanAndSetVec3::getQuantisationSet() const
{
  return (const uint8_t*) m_qSet;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationMeanAndSetVec3::zero()
{
  m_qMean[0] = m_qMean[1] = m_qMean[2] = 0;
  m_qSet[0] = m_qSet[1] = m_qSet[2] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationMeanAndSetVec3::pack(const uint32_t* qMean, const uint32_t* qSet)
{
  NMP_ASSERT(qMean[0] <= 0xFF);
  m_qMean[0] = (uint8_t)qMean[0];
  NMP_ASSERT(qMean[1] <= 0xFF);
  m_qMean[1] = (uint8_t)qMean[1];
  NMP_ASSERT(qMean[2] <= 0xFF);
  m_qMean[2] = (uint8_t)qMean[2];
  NMP_ASSERT(qSet[0] <= 0xFF);
  m_qSet[0] = (uint8_t)qSet[0];
  NMP_ASSERT(qSet[1] <= 0xFF);
  m_qSet[1] = (uint8_t)qSet[1];
  NMP_ASSERT(qSet[2] <= 0xFF);
  m_qSet[2] = (uint8_t)qSet[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationMeanAndSetVec3::unpack(uint32_t* qMean, uint32_t* qSet) const
{
  qMean[0] = (uint32_t)m_qMean[0];
  qMean[1] = (uint32_t)m_qMean[1];
  qMean[2] = (uint32_t)m_qMean[2];
  qSet[0] = (uint32_t)m_qSet[0];
  qSet[1] = (uint32_t)m_qSet[1];
  qSet[2] = (uint32_t)m_qSet[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationMeanAndSetVec3::unpackQMean(NMP::Vector3& qMean) const
{
  qMean.set((float)m_qMean[0], (float)m_qMean[1], (float)m_qMean[2]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void QuantisationMeanAndSetVec3::unpackQSet(uint32_t* qSet) const
{
  qSet[0] = (uint32_t)m_qSet[0];
  qSet[1] = (uint32_t)m_qSet[1];
  qSet[2] = (uint32_t)m_qSet[2];
}

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::CompToAnimChannelMap
/// \brief Stores compression channel to animation channel index mapping (One way mapping)
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class CompToAnimChannelMap
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numChannels);

  static MR::CompToAnimChannelMap* init(
    NMP::Memory::Resource& resource,
    uint32_t numChannels,
    const uint32_t* animChannels);

  static CompToAnimChannelMap* relocate(void*& ptr);

public:
  CompToAnimChannelMap() {}
  ~CompToAnimChannelMap() {}

  NM_INLINE NMP::Memory::Format getInstanceMemoryRequirements() const;

  void locate();
  void dislocate();

  NM_INLINE uint32_t getNumChannels() const;
  NM_INLINE const uint16_t* getAnimChannels() const;

protected:
  uint16_t          m_numChannels;      ///< The number of compression channels
  uint16_t          m_animChannels[1];  ///< The anim channel indices that map to the compression channels
                                        ///< Do not place any data after this entry.
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format CompToAnimChannelMap::getInstanceMemoryRequirements() const
{
  return getMemoryRequirements(m_numChannels);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t CompToAnimChannelMap::getNumChannels() const
{
  return m_numChannels;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const uint16_t* CompToAnimChannelMap::getAnimChannels() const
{
  return m_animChannels;
}

//----------------------------------------------------------------------------------------------------------------------
// Dequantisation functions
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void dequantise(
  const NMP::Vector3& qScale,
  const NMP::Vector3& qOffset,
  const uint32_t*     inData,
  NMP::Vector3&       outData)
{
  NMP_ASSERT(inData);
  outData.set((float)inData[0], (float)inData[1], (float)inData[2]);
  outData.multiplyElements(qScale);
  outData += qOffset;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void dequantise(
  const NMP::Vector3& qScale,
  const NMP::Vector3& qOffset,
  const uint16_t*     inData,
  NMP::Vector3&       outData)
{
  NMP_ASSERT(inData);
  outData.set((float)inData[0], (float)inData[1], (float)inData[2]);
  outData.multiplyElements(qScale);
  outData += qOffset;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void dequantise(
  const NMP::Vector3& qScale,
  const NMP::Vector3& qOffset,
  const uint8_t*      inData,
  NMP::Vector3&       outData)
{
  NMP_ASSERT(inData);
  outData.set((float)inData[0], (float)inData[1], (float)inData[2]);
  outData.multiplyElements(qScale);
  outData += qOffset;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void uniformQuantisation16(
  const float*    qMin,
  const float*    qMax,
  NMP::Vector3&   qScale,
  NMP::Vector3&   qOffset)
{
  qOffset.set(qMin[0], qMin[1], qMin[2]);
  NMP::Vector3 vmax(qMax[0], qMax[1], qMax[2]);
  float stepSize = 1.0f / 65535; // 1 / (2^16 - 1)
  qScale = (vmax - qOffset) * stepSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void uniformQuantisation8(
  const float*    qMin,
  const float*    qMax,
  NMP::Vector3&   qScale,
  NMP::Vector3&   qOffset)
{
  qOffset.set(qMin[0], qMin[1], qMin[2]);
  NMP::Vector3 vmax(qMax[0], qMax[1], qMax[2]);
  float stepSize = 1.0f / 255; // 1 / (2^8 - 1)
  qScale = (vmax - qOffset) * stepSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void uniformQuantisation(
  const float*    qMin,
  const float*    qMax,
  const uint32_t* precisions,
  NMP::Vector3&   qScale,
  NMP::Vector3&   qOffset)
{
  NMP_ASSERT(precisions);

  // Compute the scale factors
  uint32_t numSteps[3];
  numSteps[0] = (0x80000001 << precisions[0]) - 1;
  numSteps[1] = (0x80000001 << precisions[1]) - 1;
  numSteps[2] = (0x80000001 << precisions[2]) - 1;
  
  qScale.x = (qMax[0] - qMin[0]) / (float)numSteps[0];
  qScale.y = (qMax[1] - qMin[1]) / (float)numSteps[1];
  qScale.z = (qMax[2] - qMin[2]) / (float)numSteps[2];
  
  // Compute the offsets
  qOffset.set(qMin[0], qMin[1], qMin[2]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void uniformQuantisation(
  const NMP::Vector3& qMin,
  const NMP::Vector3& qMax,
  const uint32_t*     precisions,
  NMP::Vector3&       qScale,
  NMP::Vector3&       qOffset)
{
  NMP_ASSERT(precisions);

  // Compute the scale factors
  uint32_t numSteps[3];
  numSteps[0] = (0x80000001 << precisions[0]) - 1;
  numSteps[1] = (0x80000001 << precisions[1]) - 1;
  numSteps[2] = (0x80000001 << precisions[2]) - 1;

  // Scale
  qScale.x = (qMax.x - qMin.x) / (float)numSteps[0];
  qScale.y = (qMax.y - qMin.y) / (float)numSteps[1];
  qScale.z = (qMax.z - qMin.z) / (float)numSteps[2];
  
  // Offset
  qOffset = qMin;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void unpackQuantisedVector(uint32_t key, uint32_t* v)
{
  v[0] = ((key >> 21) & 0x07ff);  // 11 bits
  v[1] = ((key >> 10) & 0x07ff);  // 11 bits
  v[2] = (key & 0x03ff);   // 10 bits
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE uint32_t packQuantisedVector(uint32_t x, uint32_t y, uint32_t z)
{
  return (x << 21) | (y << 10) | z;
}

//----------------------------------------------------------------------------------------------------------------------
// Tan quarter angle rotation vector functions
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void fromRotationVectorTQA(
  const NMP::Vector3& inVec,
  NMP::Quat&          outQuat)
{
  NMP::Vector3 v;
  v = inVec;
  float mag2 = v.magnitudeSquared();
  float opm2 = 1 + mag2;
  float fac = 2 / opm2;
  v *= fac;
  outQuat.w = (1 - mag2) / opm2;
  outQuat.x = v.x;
  outQuat.y = v.y;
  outQuat.z = v.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void fromRotationVectorTQA(
  const NMP::Vector3& inVec,
  NMP::Quat&          outQuat,
  uint32_t            wNeg) // 0 if w >= 0, 1 if w < 0
{
  NMP_ASSERT(wNeg <= 1);
  const float wSigns[2] = { 1.0f, -1.0f };

  NMP::Vector3 v;
  v = inVec;
  float mag2 = v.magnitudeSquared();
  float opm2 = wSigns[wNeg] * (1 + mag2);
  float fac = 2 / opm2;
  v *= fac;
  outQuat.w = (1 - mag2) / opm2;
  outQuat.x = v.x;
  outQuat.y = v.y;
  outQuat.z = v.z;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIM_SOURCE_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
