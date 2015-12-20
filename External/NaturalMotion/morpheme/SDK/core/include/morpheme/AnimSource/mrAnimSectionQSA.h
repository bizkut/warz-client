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
#ifndef MR_ANIM_SECTION_QSA_H
#define MR_ANIM_SECTION_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/AnimSource/mrAnimSourceUtils.h"
#include "NMPlatform/NMBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class AnimSourceQSA;

#define QSA_CHAN_LOCAL_FLAG           (0x01)
#define QSA_CHAN_WORLD_FLAG           (0x00)

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::UnchangingKeyQSA
/// \brief A 3-vector corresponding to an unchanging data channel.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class UnchangingKeyQSA
{
public:
  UnchangingKeyQSA() {}

  void locate();
  void dislocate();

public:
  uint16_t m_data[3];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::QuantisationInfoQSA
/// \brief Uniform quantisation range information for 3-vector channels.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class QuantisationInfoQSA
{
public:
  QuantisationInfoQSA() {}

  void locate();
  void dislocate();
  void clear();

public:
  float m_qMin[3];
  float m_qMax[3];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::QuantisationDataQSA
/// \brief Uniform quantisation data for 3-vector channels.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class QuantisationDataQSA
{
public:
  QuantisationDataQSA() {}

  static size_t getMemoryRequirementsSize(uint32_t numChannels)
  {
    // The quantization data is padded to a multiple of 4 entries to allow for faster decoding.
    // Quantisation data is fetched for 4 bones at a time. This data is then used to look up
    // the quantisation info tables, so the contents of the padded data must be correct (The
    // last entry is repeated).
    // Check AP::SampledPosCompressorQSA::computeRelativeTransforms, AP>>SampledPosCompressorQSA::compileData,
    // AP::SampledQuatCompressorQSA::computeRelativeTransforms and AP::SampledQuatCompressorQSA::compileData.
    //
    // Invalid sample data is written to the output buffer at rig channel 0, so we don't need
    // to worry about padding the keyframe data to a multiple of 4 entries (except after the very
    // last data section to prevent a potential buffer overrun).
    // Check MR::RigToAnimMap::getMemoryRequirements, MR::RigToAnimMap::init,
    // MR::CompToAnimChannelMap::getMemoryRequirements and MR::CompToAnimChannelMap::init.
    //
    // We have to make sure that we have enough padding for any trailing keyframes to
    // prevent reading beyond the allocated animation memory. There are at most 3
    // keyframes that need to be padded, each with XYZ components of uint16_t. Note that
    // each XYZ component is loaded using unpacku4i16, unpacku4i16f. i.e. 4 uint16_t's
    // so we need at most 20 bytes of padding.
    // Check MR::UnpackedQuantisationDataQSA::getMemoryRequirements.
    return sizeof(QuantisationDataQSA) * NMP::nmAlignedValue4(numChannels);
  }

  void locate();
  void dislocate();

  uint8_t* getPrecision() const { return (uint8_t*)m_prec; }
  uint8_t* getMean() const { return (uint8_t*)m_mean; }
  uint8_t* getQuantisationSet() const { return (uint8_t*)m_qSet; }

  // Precisions
  uint16_t getPrecisionX() const { return (uint16_t)m_prec[0]; }
  uint16_t getPrecisionY() const { return (uint16_t)m_prec[1]; }
  uint16_t getPrecisionZ() const { return (uint16_t)m_prec[2]; }

  // Means
  uint16_t getMeanX() const { return (uint16_t)m_mean[0]; }
  uint16_t getMeanY() const { return (uint16_t)m_mean[1]; }
  uint16_t getMeanZ() const { return (uint16_t)m_mean[2]; }

  // Quantisation sets
  uint16_t getQuantisationSetX() const { return (uint16_t)m_qSet[0]; }
  uint16_t getQuantisationSetY() const { return (uint16_t)m_qSet[1]; }
  uint16_t getQuantisationSetZ() const { return (uint16_t)m_qSet[2]; }

protected:
  uint8_t     m_prec[3];    ///< Quantisation precisions for the allocation of bits for each channel
  uint8_t     m_mean[3];    ///< Coarse estimate for the mean of the channel data
  uint8_t     m_qSet[3];    ///< Quantisation set assignments
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ChannelSetInfoQSA
/// \brief Stores channel set information that is global to all sections within the compressed animation.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetInfoQSA
{
public:
  //-----------------------
  ChannelSetInfoQSA() {}
  ~ChannelSetInfoQSA() {}

  //-----------------------
  void locate();
  void dislocate();
  void relocate();
  
  void unchangingPosDecompress(
    const AnimToRigTableMap*    animToRigTableMap,
    const CompToAnimChannelMap* compToAnimTableMap,
    NMP::DataBuffer*            outputBuffer) const;
    
  void unchangingQuatDecompress(
    const AnimToRigTableMap*    animToRigTableMap,
    const CompToAnimChannelMap* compToAnimTableMap,
    NMP::DataBuffer*            outputBuffer) const;

  const NMP::Quat& getWorldspaceRootQuat() const { return m_worldspaceRootQuat; }
  uint16_t getWorldspaceRootID() const { return m_worldspaceRootID; }

protected:
  //-----------------------
  // Header info
  NMP::Quat                           m_worldspaceRootQuat;             ///< Worldspace quat required to recover the topmost animation data local quat.
                                                                        ///< This is stored since animations

  uint16_t                            m_worldspaceRootID;               ///< Rig index of the parent to the topmost animation data channel.
                                                                        ///< Note that this may not be the CharacterWorldSpaceTM rig channel (0).

  uint16_t                            m_numChannelSets;                 ///< The number of channel sets contained within this animation section.

  uint16_t                            m_unchangingDefaultPosNumChannels;   ///< The number of pos channels that are the same as the default pose
  uint16_t                            m_unchangingDefaultQuatNumChannels;  ///< The number of quat channels that are the same as the default pose
  uint16_t                            m_unchangingPosNumChannels;   ///< The number of pos channels with unvarying data
  uint16_t                            m_unchangingQuatNumChannels;  ///< The number of quat channels with unvarying data

  //-----------------------
  // Unchanging channels quantisation info
  QuantisationInfoQSA                 m_unchangingPosQuantisationInfo;  ///< Quantisation info about ranges of all unchanging pos channels
  QuantisationInfoQSA                 m_unchangingQuatQuantisationInfo; ///< Quantisation info about ranges of all unchanging quat channels

  UnchangingKeyQSA*                   m_unchangingPosData;              ///< Table of default values for unvarying pos channels quantised
                                                                        ///< into the [qMin, qMax] range of unchangingPosKeysInfo
  UnchangingKeyQSA*                   m_unchangingQuatData;             ///< Table of default values for unvarying quat channels (tan quarter rot vecs)
                                                                        ///< quantised into the [qMin, qMax] range of unchangingQuatKeysInfo
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimSectionInfoQSA
/// \brief Describes aspects of an AnimSection.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSectionInfoQSA
{
public:
  //-----------------------
  AnimSectionInfoQSA() {}
  ~AnimSectionInfoQSA() {}

  //-----------------------
  void locate();
  void dislocate();

  //-----------------------
  uint32_t getStartFrame() const { return m_startFrame; }
  uint32_t getSectionSize() const { return m_sectionSize; }

  void setStartFrame(uint32_t startFrame) { m_startFrame = startFrame; }
  void setSectionSize(uint32_t sectionSize) { m_sectionSize = sectionSize; }

protected:
  uint32_t                            m_startFrame;                 ///< Specified within the space of the animations total frame count.
  uint32_t                            m_sectionSize;                ///< How large is this section in bytes.
};

//----------------------------------------------------------------------------------------------------------------------
// InternalDataQSA
//----------------------------------------------------------------------------------------------------------------------
class InternalDataQSA
{
public:
  static NMP::Memory::Format getMemoryRequirements(uint32_t numRigBones);

  static InternalDataQSA* init(
    NMP::Memory::Resource memRes,
    uint32_t numRigBones);

public:
  // Sampled channel interpolants
  uint32_t                            m_sampledIndex;
  float                               m_sampledUParam;              ///< Interpolant between sampled keyframes
  
  // Spline channel interpolants
  uint32_t                            m_knotIndex;
  float                               m_splineUParam;               ///< Interpolant between the knot sites
  float                               m_splinePosBasis[4];          ///< Bezier basis functions for pos spline interpolation
  float                               m_splineQuatBasis[4];         ///< Bezier basis functions for quat spline interpolation

  // Channel quat spaces
  uint8_t                             m_useSpace;                   ///< Quat channels encoded in local or worldspace
  uint8_t*                            m_chanQuatSpaces;             ///< rig entries worth of space flags
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimSectionQSA
/// \brief Cross section of a quaternion spline animation.
/// \ingroup SourceAnimationCompressedModule
///
/// \details Keyframe data is divided into sections. Each section records all bone
/// transforms over a specified time chunk of the whole animation. Chunks are designed to be of
/// optimum size for DMAing to SPUs or to reduce cache misses on other platforms.
///
/// A section of QSA data is composed of:
///   - Header data block.
///   - Sampled position channel quantisation min, max information.
///   - Uniformly quantised sampled position data
///   - Sampled rotation vector channel quantisation min, max information.
///   - Uniformly quantised sampled rotation vector data.
///   - A knot vector for the spline channels
///   - Spline position channel quantisation min, max information.
///   - Uniformly quantised spline position control point data
///   - Spline quat key quantisation min, max information.
///   - Uniformly quantised spline quat key data
///   - Spline quat tangent quantisation min, max information.
///   - Uniformly quantised spline quat tangent data
///
/// The packed data is stored continuous on channels for each frame in memory to ensure that the
/// data is not fragmented which can lead to cache miss performance hits at decompression time.
//----------------------------------------------------------------------------------------------------------------------
class AnimSectionQSA
{
public:
  //-----------------------
  AnimSectionQSA() {}
  ~AnimSectionQSA() {}

  //-----------------------
  void locate();
  void dislocate();
  void relocate();

  void sampledPosDecompress(
    const AnimToRigTableMap* animToRigTableMap,
    const CompToAnimChannelMap* compToAnimMap,
    InternalDataQSA* internalData,
    NMP::DataBuffer* outputTransformBuffer) const;

  void sampledQuatDecompress(
    const AnimToRigTableMap* animToRigTableMap,
    const CompToAnimChannelMap* compToAnimMap,
    InternalDataQSA* internalData,
    NMP::DataBuffer* outputTransformBuffer) const;

  void splinePosDecompress(
    const AnimToRigTableMap* animToRigTableMap,
    const CompToAnimChannelMap* compToAnimMap,
    InternalDataQSA* internalData,
    NMP::DataBuffer* outputTransformBuffer) const;

  void splineQuatDecompress(
    const AnimToRigTableMap* animToRigTableMap,
    const CompToAnimChannelMap* compToAnimMap,
    InternalDataQSA* internalData,
    NMP::DataBuffer* outputTransformBuffer) const;

  uint16_t getSectionAnimStartFrame() const { return m_sectionAnimStartFrame; }
  uint16_t getSectionNumAnimFrames() const { return m_sectionNumAnimFrames; }
  
  uint16_t getSplineNumKnots() const { return m_splineNumKnots; }
  const uint16_t* getSplineKnots() const { return m_splineKnots; }

protected:
  //-----------------------
  // Section header info
  uint16_t                            m_sectionSize;                ///< The size of this section in bytes
  uint16_t                            m_numChannelSets;             ///< The number of channel sets contained within this animation section.
  uint16_t                            m_sectionAnimStartFrame;      ///< The start frame of the section within the whole animation
  uint16_t                            m_sectionNumAnimFrames;       ///< The number of frames contained within this section

  // Sampled channels
  uint16_t                            m_sampledPosNumChannels;
  uint16_t                            m_sampledQuatNumChannels;

  // Spline channels
  uint16_t                            m_splineNumKnots;
  uint16_t                            m_splinePosNumChannels;
  uint16_t                            m_splineQuatNumChannels;

  // Bitstream data strides
  uint16_t                            m_sampledPosByteStride;
  uint16_t                            m_sampledQuatByteStride;
  uint16_t                            m_splinePosByteStride;
  uint16_t                            m_splineQuatWNegsByteStride;
  uint16_t                            m_splineQuatKeysByteStride;
  uint16_t                            m_splineQuatTangentsByteStride;

  // Quantisation sets info
  uint16_t                            m_sampledPosNumQuantisationSets;
  uint16_t                            m_sampledQuatNumQuantisationSets;
  uint16_t                            m_splinePosNumQuantisationSets;
  uint16_t                            m_splineQuatKeysNumQuantisationSets;
  uint16_t                            m_splineQuatTangentsNumQuantisationSets;

  //-----------------------
  QuantisationInfoQSA                 m_posMeansQuantisationInfo;               ///< Global quantisation range for the pos keys in this section
  // Global quantisation range for the quat means [qMin, qMax] range is assumed to be between [-1:1] (tan quarter angle rotation vector range)
  QuantisationInfoQSA                 m_splineQuatTangentMeansQuantisationInfo; ///< Global quantisation range for the quat spline tangents in this section

  //-----------------------
  // Sampled channels

  // Pos
  QuantisationInfoQSA*                m_sampledPosQuantisationInfo;   ///< Quantisation info about ranges of all sampled pos channels
  QuantisationDataQSA*                m_sampledPosQuantisationData;   ///< Array of quantisation mean/prec/qset data for the sampled pos channels
  uint8_t*                            m_sampledPosData;               ///< Bitstream data for the sampled pos channels

  // Quat
  QuantisationInfoQSA*                m_sampledQuatQuantisationInfo;  ///< Quantisation info about ranges of all sampled quat channels
  QuantisationDataQSA*                m_sampledQuatQuantisationData;  ///< Array of quantisation mean/prec/qset data for the sampled quat channels
  uint8_t*                            m_sampledQuatData;              ///< Bitstream data for the sampled quat channels

  //-----------------------
  // Spline channels
  uint16_t*                           m_splineKnots;                  ///< Common knot vector for the spline channels

  // Pos spline
  QuantisationInfoQSA*                m_splinePosQuantisationInfo;    ///< Quantisation info about ranges of all spline pos channels
  QuantisationDataQSA*                m_splinePosQuantisationData;    ///< Array of quantisation mean/prec/qset data for the spline pos channels
  uint8_t*                            m_splinePosData;                ///< Bitstream data for the spline pos channels

  // Quat spline
  QuantisationInfoQSA*                m_splineQuatKeysQuantisationInfo; ///< Quantisation info about ranges of all spline quat key channels
  QuantisationDataQSA*                m_splineQuatKeysQuantisationData; ///< Array of quantisation mean/prec/qset data for the spline quat key channels
  uint8_t*                            m_splineQuatWNegsData;            ///< Bitstream data for the spline quat key wNegs
  uint8_t*                            m_splineQuatKeysData;             ///< Bitstream data for the spline quat key channels

  QuantisationInfoQSA*                m_splineQuatTangentsQuantisationInfo; ///< Quantisation info about ranges of all spline quat tangent channels
  QuantisationDataQSA*                m_splineQuatTangentsQuantisationData; ///< Array of quantisation data for the spline quat tangent channels
  uint8_t*                            m_splineQuatTangentsData;             ///< Bitstream data for the spline quat tangent channels
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIM_SECTION_QSA_H
//----------------------------------------------------------------------------------------------------------------------
