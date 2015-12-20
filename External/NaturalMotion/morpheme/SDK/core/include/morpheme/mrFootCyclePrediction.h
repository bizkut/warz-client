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
#ifndef MR_FOOT_CYCLE_PREDICTION_H
#define MR_FOOT_CYCLE_PREDICTION_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::FootCyclePredictor
/// \brief A foot cycle predictor class that represents the cycle local motion of the
/// foot data by using two Bezier curve segments with shared end points. The foot cycle
/// curve is fitted to the 2D components of motion not in the up axis direction. Since
/// a character can speed up/down and bank during a foot cycle, the fitted curve from the
/// previous cycle's data can be scaled to better fit the small history of current samples.
/// The foot cycle curve can then be evaluated to predict the position of the next foot
/// plant using the current state of the system.
///
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class FootCyclePredictor
{
public:
  // Curve state enumerations
  enum CurveState
  {
    kValidCurveCtrlPt0,                 ///< Only the start point of the foot cycle is valid
    kValidCurveCtrlPts,                 ///< The foot cycle curve is valid
    kNumCurveStateTypes,
    kInvalidCurveState = 0xFFFFFFFF     ///< No control points in the foot cycle curve are valid
  };

public:
  FootCyclePredictor() {}
  ~FootCyclePredictor() {}

  /// \brief Return the memory requirements of an instance.
  static NM_INLINE NMP::Memory::Format getMemoryRequirements();

  /// \brief Initialise the foot cycle prediction state.
  void init();

  /// \brief Update the foot cycle fitting state with a new motion sample. This also recomputes
  /// a new foot cycle curve if we have transitioned into another cycle.
  void updateFootCycle(
    const NMP::Vector3& upAxisV,        ///< The up axis direction vector
    bool hasFootCycleLooped,            ///< If flagged then the predictor will fit a new curve to the current data state
    float footCycleUMid,                ///< The curve break site position between the two curve segments as a fraction of the foot cycle [0:1].
    float footCycleU,                   ///< The current position of the sample as a fraction of the foot cycle [0:1].
    const NMP::Vector3& footCyclePos    ///< The cycle local sample position of the footbase
  );

  /// \brief Computes a predicted estimate of the foot position based on the current state of
  /// the foot cycle.
  bool evaluateFootCycleCurve(
    float footCycleUMid,                ///< The curve break site position between the two curve segments as a fraction of the foot cycle [0:1].
    float footCycleU,                   ///< The current position of the sample as a fraction of the foot cycle [0:1].
    NMP::Vector3& footCyclePos          ///< The cycle local sample position of the footbase
  );

  NM_INLINE CurveState getFootCycleCurveState() const;

protected:
  void resetFootCycleCurveFittingState();
  void resetFootCycleCurve();

  NM_INLINE uint32_t getFootCycleSampleBegin();
  NM_INLINE uint32_t getFootCycleSampleEnd();
  NM_INLINE uint32_t getFootCycleSampleNext(uint32_t indx) const;

  void appendFootCycleSample(
    float footCycleU,
    float footCyclePosX,
    float footCyclePosZ,
    bool footCycleCurvePosValid,
    float footCycleCurvePosX,
    float footCycleCurvePosZ);

  void computeFootCycleCurve(
    float footCycleU,
    float footCyclePosX,
    float footCyclePosZ);

  void computeFootCyclePCs();

  void computeScaleFromSamples();

  void updateFootCycleCurve(
    float footCycleUMid,
    float footCycleU,
    float footCyclePosX,
    float footCyclePosZ);

protected:
  // Alignment quat required to rotate the canonical Y axis onto the specified up axis
  NMP::Quat                         m_alignUpAxisQuat;

  // Current foot cycle curve parameterisation for this foot cycle. This is updated once
  // per foot cycle. The curve can be scaled to better fit the history of input samples.
  CurveState                        m_footCycleCurveState;      ///< Foot cycle curve valid state
  float                             m_footCycleCurveVc[4];      ///< Principal directions of foot cycle curve
  float                             m_footCycleCurveXbar[2];    ///< Sample mean of foot cycle curve
  float                             m_footCycleCurveCtrlPtsX[6];  ///< Foot cycle curve control X points
  float                             m_footCycleCurveCtrlPtsZ[6];  ///< Foot cycle curve control Z points

  // Current foot cycle curve fitting state. This is updated every sample.
  uint32_t                          m_footCycleCurveNumSamples; ///< Number of samples in the curve fitting state
  uint32_t                          m_footCycleCurveHist[4];    ///< Quadrant histogram for curve fitting
  float                             m_footCycleCurveAtA0r1[3];  ///< First row of the 4x4 moment matrix
  float                             m_footCycleCurveAtA0[6];    ///< Lower right 3x3 block of the moment matrix
  float                             m_footCycleCurveAtx0[3];    ///< Right hand side vector X components
  float                             m_footCycleCurveAtz0[3];    ///< Right hand side vector Z components
  float                             m_footCycleCurveAtA1r4[3];  ///< Last row of the 4x4 moment matrix
  float                             m_footCycleCurveAtA1[6];    ///< upper left 3x3 block of the moment matrix
  float                             m_footCycleCurveAtx1[3];    ///< Right hand side vector X components
  float                             m_footCycleCurveAtz1[3];    ///< Right hand side vector Z components

  // Foot cycle curve prediction history state (Unscaled points sampled from the current curve).
  // This is a ring buffer corresponding to the sample history state and is updated every sample.
  bool                              m_footCycleCurvePosValid[4];  ///< Sampled foot cycle position data valid flags
  float                             m_footCycleCurvePosX[4];    ///< Sampled foot cycle position X data
  float                             m_footCycleCurvePosZ[4];    ///< Sampled foot cycle position Z data

  // Foot cycle curve scaling factors. This is updated every sample.
  float                             m_footCycleCurveScale[2];   ///< Scale factors to fit the curve to the samples

  // Foot cycle sample history state (Raw points from the user input). This is a ring buffer
  // corresponding to the curve prediction history state and is updated every sample.
  uint32_t                          m_footCycleNumSamples;      ///< Number of valid points in the history
  uint32_t                          m_footCycleSampleIndx;      ///< Current insertion index of the ring buffer
  float                             m_footCyclePosX[4];         ///< Raw foot cycle position X data
  float                             m_footCyclePosZ[4];         ///< Raw foot cycle position Z data
  float                             m_footCycleU[4];            ///< Raw gait cycle fraction sample times [0:1]
};

//----------------------------------------------------------------------------------------------------------------------
// FootCyclePredictor Inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format FootCyclePredictor::getMemoryRequirements()
{
  NMP::Memory::Format result;

  result.size = sizeof(FootCyclePredictor);
  result.alignment = NMP_NATURAL_TYPE_ALIGNMENT;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE FootCyclePredictor::CurveState FootCyclePredictor::getFootCycleCurveState() const
{
  return m_footCycleCurveState;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t FootCyclePredictor::getFootCycleSampleBegin()
{
  return (m_footCycleSampleIndx - m_footCycleNumSamples) & 0x03;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t FootCyclePredictor::getFootCycleSampleEnd()
{
  return (m_footCycleSampleIndx - 1) & 0x03;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t FootCyclePredictor::getFootCycleSampleNext(uint32_t indx) const
{
  return (indx + 1) & 0x03;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_FOOT_CYCLE_PREDICTION_H
//----------------------------------------------------------------------------------------------------------------------
