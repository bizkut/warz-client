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
#ifndef MR_TRAJECTORY_SOURCE_QSA_H
#define MR_TRAJECTORY_SOURCE_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBuffer.h"
#include "morpheme/AnimSource/mrTrajectorySourceBase.h"
//----------------------------------------------------------------------------------------------------------------------

// nonstandard extension used : nameless struct/union
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4201)
#endif

// Channel compression methods flags
// Bits 0-7 are for the compression method index of the delta pos channel
// Bits 8-15 are for the compression method index of the delta quat channel
#define QSA_DELTA_CHAN_METHOD_MASK (0x00ff)
#define QSA_DELTAPOS_CHAN_METHOD_SHIFT (0)
#define QSA_DELTAQUAT_CHAN_METHOD_SHIFT (8)

#define QSA_DELTA_CHAN_UNCHANGING (0x0000)
#define QSA_DELTA_CHAN_SAMPLED    (0x0001)

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TrajectoryPosKeyQSA
/// \brief A Vector3 position quantised into 32 bits.
/// \ingroup SourceTrajectoryCompressedModule
///
/// Compressed in the following fashion:
///   - 10 bits,  (0 -  9 = Unsigned z component).
///   - 11 bits,  (10 - 20 = Unsigned y component).
///   - 11 bits,  (21 - 31 = Unsigned x component).
//----------------------------------------------------------------------------------------------------------------------
class TrajectoryPosKeyQSA
{
public:
  TrajectoryPosKeyQSA() {}

  void locate();
  void dislocate();

public:
  uint32_t m_data;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TrajectoryQuatKeyQSA
/// \brief A tan quarter angle rotation vector quantised into 48 bits.
/// \ingroup SourceTrajectoryCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class TrajectoryQuatKeyQSA
{
public:
  TrajectoryQuatKeyQSA() {}

  void locate();
  void dislocate();

public:
  uint16_t m_data[3];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TrajectoryKeyInfoQSA
/// \brief Uniform quantisation information for 3-vectors.
/// \ingroup SourceTrajectoryCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class TrajectoryKeyInfoQSA
{
public:
  TrajectoryKeyInfoQSA() {}

  void locate();
  void dislocate();

public:
  union
  {
    struct
    {
      float m_scales[3];
      float m_offsets[3];
    };

    struct
    {
      float x, y, z, w;
    };
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TrajectorySourceQSA
/// \brief Implementation of MR::TrajectorySourceBase with some level of compression.
/// \ingroup SourceTrajectoryCompressedModule
/// \see MR::TrajectorySourceBase
///
/// Bone index 0 is a non-authored bone which we insert as a Root to the whole hierarchy.
/// The bone can be assumed to lie on the world origin when authoring (identity).
/// This should be the only bone with parent index of -1 in the hierarchy.
/// Bones which when authored had no explicit parent will be parented to this bone in the export process (parent index 0).
/// This Root bone will be used to place the character in the World, by setting a desired World Root Transform prior to
/// accumulating world transforms.
/// Delta transformations of trajectory bones are calculated relative to this Root.
/// Animations never contain data for rig bone 0 as this is automatically added by us.
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceQSA : public TrajectorySourceBase
{
public:
  TrajectorySourceQSA() {}
  ~TrajectorySourceQSA() {}

  void locate();
  void dislocate();
  void relocate();

  static void computeTrajectoryTransformAtTime(
    const TrajectorySourceBase* trajectoryControl,
    float                       time,
    NMP::Quat&                  resultQuat,
    NMP::Vector3&               resultPos);

  NM_INLINE uint16_t getCompChanMethodForDeltaPosChan() const; // 0 - unchanging, 1 - sampled
  NM_INLINE uint16_t getCompChanMethodForDeltaQuatChan() const; // 0 - unchanging, 1 - sampled

protected:
  //-----------------------
  struct DecompressionArgs
  {
    // Trajectory source
    const TrajectorySourceQSA*        source;

    // Output buffer
    NMP::Quat*                        qOut;
    NMP::Vector3*                     pOut;

    // Sampled function interpolants
    uint32_t                          animFrameIndex;
    float                             interpolant;
  };

  //-----------------------
  // Decompression functions
  typedef void (*DecompressFn) (DecompressionArgs& args);

  static void unchangingPosFn(DecompressionArgs& args);
  static void unchangingQuatFn(DecompressionArgs& args);

  static void sampledPosFn(DecompressionArgs& args);
  static void sampledQuatFn(DecompressionArgs& args);

protected:
  // Trajectory Header info
  float                               m_sampleFrequency;            ///< Number of key frame samples per second.
  uint16_t                            m_numAnimFrames;              ///< The number of frames contained within this animation
  uint16_t                            m_flags;                      ///< Channel compression method flags

  // Quantisation information for the delta trajectory channels
  TrajectoryKeyInfoQSA                m_sampledDeltaPosKeysInfo;
  TrajectoryKeyInfoQSA                m_sampledDeltaQuatKeysInfo;

  // Sampled delta trajectory channels
  TrajectoryPosKeyQSA*                m_sampledDeltaPosKeys;
  TrajectoryQuatKeyQSA*               m_sampledDeltaQuatKeys;
};

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceQSA inline functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t TrajectorySourceQSA::getCompChanMethodForDeltaPosChan() const
{
  return ((m_flags >> QSA_DELTAPOS_CHAN_METHOD_SHIFT) & QSA_DELTA_CHAN_METHOD_MASK);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t TrajectorySourceQSA::getCompChanMethodForDeltaQuatChan() const
{
  return ((m_flags >> QSA_DELTAQUAT_CHAN_METHOD_SHIFT) & QSA_DELTA_CHAN_METHOD_MASK);
}

} // namespace MR

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRAJECTORY_SOURCE_QSA_H
//----------------------------------------------------------------------------------------------------------------------
