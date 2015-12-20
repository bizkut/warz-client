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
#include "morpheme/mrFootCyclePrediction.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void FootCyclePredictor::init()
{
  resetFootCycleCurveFittingState();
  resetFootCycleCurve();
}

//----------------------------------------------------------------------------------------------------------------------
void FootCyclePredictor::resetFootCycleCurveFittingState()
{
  // Reset the current foot cycle curve fitting state. Note that the
  // current curve state, control points and prediction state history
  // (for scaling the current curve) remain unchanged.
  m_footCycleCurveNumSamples = 0;
  for (uint32_t i = 0; i < 4; ++i)
    m_footCycleCurveHist[i] = 0;
  for (uint32_t i = 0; i < 3; ++i)
    m_footCycleCurveAtA0r1[i] = 0.0f;
  for (uint32_t i = 0; i < 6; ++i)
    m_footCycleCurveAtA0[i] = 0.0f;
  for (uint32_t i = 0; i < 3; ++i)
    m_footCycleCurveAtx0[i] = 0.0f;
  for (uint32_t i = 0; i < 3; ++i)
    m_footCycleCurveAtz0[i] = 0.0f;
  for (uint32_t i = 0; i < 3; ++i)
    m_footCycleCurveAtA1r4[i] = 0.0f;
  for (uint32_t i = 0; i < 6; ++i)
    m_footCycleCurveAtA1[i] = 0.0f;
  for (uint32_t i = 0; i < 3; ++i)
    m_footCycleCurveAtx1[i] = 0.0f;
  for (uint32_t i = 0; i < 3; ++i)
    m_footCycleCurveAtz1[i] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void FootCyclePredictor::resetFootCycleCurve()
{
  // Reset the current curve state and control points
  m_footCycleCurveState = FootCyclePredictor::kInvalidCurveState;
  for (uint32_t i = 0; i < 4; ++i)
    m_footCycleCurveVc[i] = 0.0f;
  for (uint32_t i = 0; i < 2; ++i)
    m_footCycleCurveXbar[i] = 0.0f;
  for (uint32_t i = 0; i < 6; ++i)
    m_footCycleCurveCtrlPtsX[i] = 0.0f;
  for (uint32_t i = 0; i < 6; ++i)
    m_footCycleCurveCtrlPtsZ[i] = 0.0f;

  // Reset the foot cycle curve prediction history state (Unscaled points sampled from the current curve)
  for (uint32_t i = 0; i < 4; ++i)
    m_footCycleCurvePosValid[i] = false;
  for (uint32_t i = 0; i < 4; ++i)
    m_footCycleCurvePosX[i] = 0.0f;
  for (uint32_t i = 0; i < 4; ++i)
    m_footCycleCurvePosZ[i] = 0.0f;

  // Reset the foot cycle curve scaling factors
  for (uint32_t i = 0; i < 2; ++i)
    m_footCycleCurveScale[i] = 0.0f;

  // Reset the foot cycle sample history state (Raw points from the user input)
  m_footCycleNumSamples = 0;
  m_footCycleSampleIndx = 0;
  for (uint32_t i = 0; i < 4; ++i)
    m_footCyclePosX[i] = 0.0f;
  for (uint32_t i = 0; i < 4; ++i)
    m_footCyclePosZ[i] = 0.0f;
  for (uint32_t i = 0; i < 4; ++i)
    m_footCycleU[i] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void FootCyclePredictor::appendFootCycleSample(
  float footCycleU,
  float footCyclePosX,
  float footCyclePosZ,
  bool footCycleCurvePosValid,
  float footCycleCurvePosX,
  float footCycleCurvePosZ)
{
  // Append a new foot cycle sample
  m_footCycleU[m_footCycleSampleIndx] = footCycleU;
  m_footCyclePosX[m_footCycleSampleIndx] = footCyclePosX;
  m_footCyclePosZ[m_footCycleSampleIndx] = footCyclePosZ;
  m_footCycleCurvePosValid[m_footCycleSampleIndx] = footCycleCurvePosValid;
  m_footCycleCurvePosX[m_footCycleSampleIndx] = footCycleCurvePosX;
  m_footCycleCurvePosZ[m_footCycleSampleIndx] = footCycleCurvePosZ;

  // Update the insertion sample index
  m_footCycleSampleIndx = (m_footCycleSampleIndx + 1) & 0x03;
  m_footCycleNumSamples++;
  if (m_footCycleNumSamples > 4)
    m_footCycleNumSamples = 4;
}

//----------------------------------------------------------------------------------------------------------------------
void FootCyclePredictor::updateFootCycle(
  const NMP::Vector3& upAxisV,
  bool hasFootCycleLooped,
  float footCycleUMid,
  float footCycleU,
  const NMP::Vector3& footCyclePos)
{
  // Compute the alignment transform H_y that rotates the canonical Y axis
  // onto the world up axis vector
  m_alignUpAxisQuat.forRotation(NMP::Vector3(0, 1, 0), upAxisV);
  // Inverse alignment rotation
  NMP::Quat invAlignUpAxisQuat = m_alignUpAxisQuat;
  invAlignUpAxisQuat.conjugate();

  // Compute the foot cycle position in the canonical Y up coordinate frame
  NMP::Vector3 footCycleAdjPos = invAlignUpAxisQuat.rotateVector(footCyclePos);

  // Check if we should compute the new foot cycle curve
  if (hasFootCycleLooped)
  {
    // Fit the foot cycle curve to the data
    computeFootCycleCurve(
      footCycleU,
      footCycleAdjPos.x,
      footCycleAdjPos.z);

    // Compute the principal directions of the foot cycle curve
    computeFootCyclePCs();
  }

  // Update the foot cycle fitting state
  updateFootCycleCurve(
    footCycleUMid,
    footCycleU,
    footCycleAdjPos.x,
    footCycleAdjPos.z);
}

//----------------------------------------------------------------------------------------------------------------------
void FootCyclePredictor::computeFootCycleCurve(
  float footCycleU,
  float footCyclePosX,
  float footCyclePosZ)
{
  // Reset the foot cycle curve state
  m_footCycleCurveState = FootCyclePredictor::kInvalidCurveState;
  if (m_footCycleNumSamples == 0)
  {
    resetFootCycleCurveFittingState();
    return;
  }

  //------------------------
  // Use the previous and current sample points to interpolate a value for
  // the curve control point at the start of the foot cycle
  uint32_t footCycleSampleIndx = getFootCycleSampleEnd();
  float prevFootCycleU = m_footCycleU[footCycleSampleIndx];
  if (prevFootCycleU > footCycleU)
    prevFootCycleU -= 1.0f;
  NMP_ASSERT(footCycleU >= 0.0f && footCycleU < 1.0f);
  NMP_ASSERT(prevFootCycleU <= footCycleU);

  // Compute the interpolant
  float footCycleUDelta = footCycleU - prevFootCycleU;
  float u = 0.0f;
  // Guard against a divide by zero. This can happen presumably on the first frame a node becomes active (I have crash reports but no repro case).
  // I have spoken with the author who assures me in this case 0.0f is the correct value for 'u'.
  if( footCycleUDelta != 0.0f )
  {
    u = -prevFootCycleU / footCycleUDelta;
  }
  float omu = 1.0f - u;

  // Compute the interpolated control point
  m_footCycleCurveCtrlPtsX[0] = omu * m_footCyclePosX[footCycleSampleIndx] + u * footCyclePosX;
  m_footCycleCurveCtrlPtsZ[0] = omu * m_footCyclePosZ[footCycleSampleIndx] + u * footCyclePosZ;
  m_footCycleCurveState = FootCyclePredictor::kValidCurveCtrlPt0;

  //------------------------
  // Check we have at least one sample point in each quadrant of the foot cycle curve
  for (uint32_t i = 0; i < 4; ++i)
  {
    if (m_footCycleCurveHist[i] < 1)
    {
      resetFootCycleCurveFittingState();
      return;
    }
  }

  //---------------------------
  // First curve segment: Symmetric 3x3 design matrix
  NMP::Matrix34 AtA;
  NMP::Vector3& r1 = AtA.xAxis();
  NMP::Vector3& r2 = AtA.yAxis();
  NMP::Vector3& r3 = AtA.zAxis();
  r1[0] = m_footCycleCurveAtA0[0];
  r1[1] = m_footCycleCurveAtA0[1];
  r1[2] = m_footCycleCurveAtA0[2];
  r2[0] = m_footCycleCurveAtA0[1];
  r2[1] = m_footCycleCurveAtA0[3];
  r2[2] = m_footCycleCurveAtA0[4];
  r3[0] = m_footCycleCurveAtA0[2];
  r3[1] = m_footCycleCurveAtA0[4];
  r3[2] = m_footCycleCurveAtA0[5];
  // Invert the matrix
  AtA.invert3x3();

  // Right hand side vector
  NMP::Vector3 bx0;
  bx0.set(m_footCycleCurveAtx0[0] - m_footCycleCurveCtrlPtsX[0]*m_footCycleCurveAtA0r1[0],
          m_footCycleCurveAtx0[1] - m_footCycleCurveCtrlPtsX[0]*m_footCycleCurveAtA0r1[1],
          m_footCycleCurveAtx0[2] - m_footCycleCurveCtrlPtsX[0]*m_footCycleCurveAtA0r1[2]);
  AtA.rotateVector(bx0);

  NMP::Vector3 bz0;
  bz0.set(m_footCycleCurveAtz0[0] - m_footCycleCurveCtrlPtsZ[0]*m_footCycleCurveAtA0r1[0],
          m_footCycleCurveAtz0[1] - m_footCycleCurveCtrlPtsZ[0]*m_footCycleCurveAtA0r1[1],
          m_footCycleCurveAtz0[2] - m_footCycleCurveCtrlPtsZ[0]*m_footCycleCurveAtA0r1[2]);
  AtA.rotateVector(bz0);

  //---------------------------
  // Second curve segment: Symmetric 3x3 design matrix
  r1[0] = m_footCycleCurveAtA1[0];
  r1[1] = m_footCycleCurveAtA1[1];
  r1[2] = m_footCycleCurveAtA1[2];
  r2[0] = m_footCycleCurveAtA1[1];
  r2[1] = m_footCycleCurveAtA1[3];
  r2[2] = m_footCycleCurveAtA1[4];
  r3[0] = m_footCycleCurveAtA1[2];
  r3[1] = m_footCycleCurveAtA1[4];
  r3[2] = m_footCycleCurveAtA1[5];
  // Invert the matrix
  AtA.invert3x3();

  // Right hand side vector
  NMP::Vector3 bx1;
  bx1.set(m_footCycleCurveAtx1[0] - m_footCycleCurveCtrlPtsX[0]*m_footCycleCurveAtA1r4[0],
          m_footCycleCurveAtx1[1] - m_footCycleCurveCtrlPtsX[0]*m_footCycleCurveAtA1r4[1],
          m_footCycleCurveAtx1[2] - m_footCycleCurveCtrlPtsX[0]*m_footCycleCurveAtA1r4[2]);
  AtA.rotateVector(bx1);

  NMP::Vector3 bz1;
  bz1.set(m_footCycleCurveAtz1[0] - m_footCycleCurveCtrlPtsZ[0]*m_footCycleCurveAtA1r4[0],
          m_footCycleCurveAtz1[1] - m_footCycleCurveCtrlPtsZ[0]*m_footCycleCurveAtA1r4[1],
          m_footCycleCurveAtz1[2] - m_footCycleCurveCtrlPtsZ[0]*m_footCycleCurveAtA1r4[2]);
  AtA.rotateVector(bz1);

  //---------------------------
  // Set the control points
  m_footCycleCurveCtrlPtsX[1] = bx0.x;
  m_footCycleCurveCtrlPtsX[2] = bx0.y;
  m_footCycleCurveCtrlPtsX[3] = 0.5f * (bx0.z + bx1.x);
  m_footCycleCurveCtrlPtsX[4] = bx1.y;
  m_footCycleCurveCtrlPtsX[5] = bx1.z;

  m_footCycleCurveCtrlPtsZ[1] = bz0.x;
  m_footCycleCurveCtrlPtsZ[2] = bz0.y;
  m_footCycleCurveCtrlPtsZ[3] = 0.5f * (bz0.z + bz1.x);
  m_footCycleCurveCtrlPtsZ[4] = bz1.y;
  m_footCycleCurveCtrlPtsZ[5] = bz1.z;

  m_footCycleCurveState = FootCyclePredictor::kValidCurveCtrlPts;

  // Reset the curve fitting state
  resetFootCycleCurveFittingState();
}

//----------------------------------------------------------------------------------------------------------------------
void FootCyclePredictor::computeFootCyclePCs()
{
  // Check if we have a valid foot cycle curve
  if (m_footCycleCurveState != FootCyclePredictor::kValidCurveCtrlPts)
  {
    return;
  }

  //---------------------------
  // Resample the fitted foot cycle curve
  const float fac = 1.0f / 8.0f;
  float samplesX[16];
  float samplesZ[16];

  // INITIAL SWING
  for (uint32_t i = 0; i < 8; ++i)
  {
    // Basis functions
    float u = i * fac;
    float omu = 1 - u;
    float omu2 = omu * omu;
    float u2 = u * u;
    float B[4];
    B[0] = omu * omu2;
    B[1] = 3 * u * omu2;
    B[2] = 3 * u2 * omu;
    B[3] = u * u2;

    // Evaluate curve
    samplesX[i] = B[0] * m_footCycleCurveCtrlPtsX[0] +
                  B[1] * m_footCycleCurveCtrlPtsX[1] +
                  B[2] * m_footCycleCurveCtrlPtsX[2] +
                  B[3] * m_footCycleCurveCtrlPtsX[3];

    samplesZ[i] = B[0] * m_footCycleCurveCtrlPtsZ[0] +
                  B[1] * m_footCycleCurveCtrlPtsZ[1] +
                  B[2] * m_footCycleCurveCtrlPtsZ[2] +
                  B[3] * m_footCycleCurveCtrlPtsZ[3];
  }

  // TERMINAL SWING
  for (uint32_t i = 0; i < 8; ++i)
  {
    // Basis functions
    float u = i * fac;
    float omu = 1 - u;
    float omu2 = omu * omu;
    float u2 = u * u;
    float B[4];
    B[0] = omu * omu2;
    B[1] = 3 * u * omu2;
    B[2] = 3 * u2 * omu;
    B[3] = u * u2;

    // Evaluate curve
    uint32_t j = i + 8;
    samplesX[j] = B[0] * m_footCycleCurveCtrlPtsX[3] +
                  B[1] * m_footCycleCurveCtrlPtsX[4] +
                  B[2] * m_footCycleCurveCtrlPtsX[5] +
                  B[3] * m_footCycleCurveCtrlPtsX[0];

    samplesZ[j] = B[0] * m_footCycleCurveCtrlPtsZ[3] +
                  B[1] * m_footCycleCurveCtrlPtsZ[4] +
                  B[2] * m_footCycleCurveCtrlPtsZ[5] +
                  B[3] * m_footCycleCurveCtrlPtsZ[0];
  }

  //---------------------------
  // Find the principal directions of the resampled points
  const float recipS00 = 1.0f / 16.0f;
  float S10 = 0.0f;
  float S01 = 0.0f;
  float S11 = 0.0f;
  float S20 = 0.0f;
  float S02 = 0.0f;

  // Compute the moment sums
  for (uint32_t i = 0; i < 16; ++i)
  {
    S10 += samplesX[i];
    S01 += samplesZ[i];
    S11 += samplesX[i] * samplesZ[i];
    S20 += samplesX[i] * samplesX[i];
    S02 += samplesZ[i] * samplesZ[i];
  }

  // Compute the moment matrix
  float M[3];
  m_footCycleCurveXbar[0] = S10 * recipS00;
  m_footCycleCurveXbar[1] = S01 * recipS00;
  M[0] = S20 - m_footCycleCurveXbar[0] * S10;
  M[1] = S11 - m_footCycleCurveXbar[0] * S01;
  M[2] = S02 - m_footCycleCurveXbar[1] * S01;

  // Compute the Jacobi rotation that diagonalises M
  float c, s;
  NMP::jacobi(M[0], M[1], M[2], c, s);
  // Diagonalise M
  float cc = c * c;
  float ss = s * s;
  float cs_a12 = 2 * c * s * M[1];
  float D[2];
  D[0] = cc * M[0] - cs_a12 + ss * M[2];
  D[1] = ss * M[0] + cs_a12 + cc * M[2];

  // Set the output Eigen-vectors (principal directions)
  if (D[0] >= D[1])
  {
    m_footCycleCurveVc[0] = c;
    m_footCycleCurveVc[1] = s;
    m_footCycleCurveVc[2] = -s;
    m_footCycleCurveVc[3] = c;
  }
  else
  {
    m_footCycleCurveVc[0] = s;
    m_footCycleCurveVc[1] = c;
    m_footCycleCurveVc[2] = c;
    m_footCycleCurveVc[3] = -s;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FootCyclePredictor::updateFootCycleCurve(
  float footCycleUMid,
  float footCycleU,
  float footCyclePosX,
  float footCyclePosZ)
{
  // Initialise the evaluated foot cycle curve point
  float footCycleCurvePosX = 0.0f;
  float footCycleCurvePosZ = 0.0f;
  bool footCycleCurvePosValid = false;

  // Determine which segment of the gait cycle curve to update
  if (footCycleU < footCycleUMid)
  {
    // INITIAL SWING
    float u = footCycleU / footCycleUMid;

    // Basis functions
    float omu = 1.0f - u;
    float omu2 = omu * omu;
    float u2 = u * u;
    float B[4];
    B[0] = omu * omu2;
    B[1] = 3 * u * omu2;
    B[2] = 3 * u2 * omu;
    B[3] = u * u2;

    // Update the quadrant histogram
    uint32_t indx = (uint32_t)(2 * u);
    m_footCycleCurveHist[indx]++;
    m_footCycleCurveNumSamples++;

    // Update initial swing portion of the design matrix
    m_footCycleCurveAtA0r1[0] += B[0] * B[1];
    m_footCycleCurveAtA0r1[1] += B[0] * B[2];
    m_footCycleCurveAtA0r1[2] += B[0] * B[3];

    m_footCycleCurveAtA0[0] += B[1] * B[1];
    m_footCycleCurveAtA0[1] += B[1] * B[2];
    m_footCycleCurveAtA0[2] += B[1] * B[3];
    m_footCycleCurveAtA0[3] += B[2] * B[2];
    m_footCycleCurveAtA0[4] += B[2] * B[3];
    m_footCycleCurveAtA0[5] += B[3] * B[3];

    // Update initial swing portion of the right hand side vector
    m_footCycleCurveAtx0[0] += B[1] * footCyclePosX;
    m_footCycleCurveAtx0[1] += B[2] * footCyclePosX;
    m_footCycleCurveAtx0[2] += B[3] * footCyclePosX;

    m_footCycleCurveAtz0[0] += B[1] * footCyclePosZ;
    m_footCycleCurveAtz0[1] += B[2] * footCyclePosZ;
    m_footCycleCurveAtz0[2] += B[3] * footCyclePosZ;

    // Update the current foot cycle curve sample point
    if (m_footCycleCurveState == FootCyclePredictor::kValidCurveCtrlPts)
    {
      footCycleCurvePosX = B[0] * m_footCycleCurveCtrlPtsX[0] +
                           B[1] * m_footCycleCurveCtrlPtsX[1] +
                           B[2] * m_footCycleCurveCtrlPtsX[2] +
                           B[3] * m_footCycleCurveCtrlPtsX[3];

      footCycleCurvePosZ = B[0] * m_footCycleCurveCtrlPtsZ[0] +
                           B[1] * m_footCycleCurveCtrlPtsZ[1] +
                           B[2] * m_footCycleCurveCtrlPtsZ[2] +
                           B[3] * m_footCycleCurveCtrlPtsZ[3];

      footCycleCurvePosValid = true;
    }
  }
  else
  {
    // TERMINAL SWING
    float u = (footCycleU - footCycleUMid) / (1.0f - footCycleUMid);

    // Basis functions
    float omu = 1.0f - u;
    float omu2 = omu * omu;
    float u2 = u * u;
    float B[4];
    B[0] = omu * omu2;
    B[1] = 3 * u * omu2;
    B[2] = 3 * u2 * omu;
    B[3] = u * u2;

    // Update the quadrant histogram
    uint32_t indx = (uint32_t)(2 * u);
    m_footCycleCurveHist[indx+2]++;
    m_footCycleCurveNumSamples++;

    // Update initial swing portion of the design matrix
    m_footCycleCurveAtA1r4[0] += B[0] * B[3];
    m_footCycleCurveAtA1r4[1] += B[1] * B[3];
    m_footCycleCurveAtA1r4[2] += B[2] * B[3];

    m_footCycleCurveAtA1[0] += B[0] * B[0];
    m_footCycleCurveAtA1[1] += B[0] * B[1];
    m_footCycleCurveAtA1[2] += B[0] * B[2];
    m_footCycleCurveAtA1[3] += B[1] * B[1];
    m_footCycleCurveAtA1[4] += B[1] * B[2];
    m_footCycleCurveAtA1[5] += B[2] * B[2];

    // Update initial swing portion of the right hand side vector
    m_footCycleCurveAtx1[0] += B[0] * footCyclePosX;
    m_footCycleCurveAtx1[1] += B[1] * footCyclePosX;
    m_footCycleCurveAtx1[2] += B[2] * footCyclePosX;

    m_footCycleCurveAtz1[0] += B[0] * footCyclePosZ;
    m_footCycleCurveAtz1[1] += B[1] * footCyclePosZ;
    m_footCycleCurveAtz1[2] += B[2] * footCyclePosZ;

    // Update the current foot cycle curve sample point
    if (m_footCycleCurveState == FootCyclePredictor::kValidCurveCtrlPts)
    {
      footCycleCurvePosX = B[0] * m_footCycleCurveCtrlPtsX[3] +
                           B[1] * m_footCycleCurveCtrlPtsX[4] +
                           B[2] * m_footCycleCurveCtrlPtsX[5] +
                           B[3] * m_footCycleCurveCtrlPtsX[0];

      footCycleCurvePosZ = B[0] * m_footCycleCurveCtrlPtsZ[3] +
                           B[1] * m_footCycleCurveCtrlPtsZ[4] +
                           B[2] * m_footCycleCurveCtrlPtsZ[5] +
                           B[3] * m_footCycleCurveCtrlPtsZ[0];

      footCycleCurvePosValid = true;
    }
  }

  // Append the sample data to the history
  appendFootCycleSample(
    footCycleU,
    footCyclePosX,
    footCyclePosZ,
    footCycleCurvePosValid,
    footCycleCurvePosX,
    footCycleCurvePosZ);

  // Update the scaling factors that warp the current foot cycle curve to fit the samples
  computeScaleFromSamples();
}

//----------------------------------------------------------------------------------------------------------------------
void FootCyclePredictor::computeScaleFromSamples()
{
  if (m_footCycleCurveState != FootCyclePredictor::kValidCurveCtrlPts)
    return;

  //---------------------------
  // Transform the input samples into the space of the adjusted curve
  uint32_t numSamples;
  float x, z;
  float footCyclePosX[4];
  float footCyclePosZ[4];
  float footCycleCurvePosX[4];
  float footCycleCurvePosZ[4];

  numSamples = 0;
  uint32_t indx = getFootCycleSampleBegin();
  for (uint32_t i = 0; i < m_footCycleNumSamples; ++i)
  {
    if (m_footCycleCurvePosValid[indx])
    {
      x = m_footCyclePosX[indx] - m_footCycleCurveXbar[0];
      z = m_footCyclePosZ[indx] - m_footCycleCurveXbar[1];
      footCyclePosX[numSamples] = m_footCycleCurveVc[0] * x + m_footCycleCurveVc[2] * z;
      footCyclePosZ[numSamples] = m_footCycleCurveVc[1] * x + m_footCycleCurveVc[3] * z;

      x = m_footCycleCurvePosX[indx] - m_footCycleCurveXbar[0];
      z = m_footCycleCurvePosZ[indx] - m_footCycleCurveXbar[1];
      footCycleCurvePosX[numSamples] = m_footCycleCurveVc[0] * x + m_footCycleCurveVc[2] * z;
      footCycleCurvePosZ[numSamples] = m_footCycleCurveVc[1] * x + m_footCycleCurveVc[3] * z;
      ++numSamples;
    }

    indx = getFootCycleSampleNext(indx);
  }

  // Check for a valid number of samples
  if (numSamples < 1)
  {
    m_footCycleCurveScale[0] = 1.0f;
    m_footCycleCurveScale[1] = 1.0f;
    return;
  }

  //---------------------------
  // Compute the linear transforms that map the curve sample points to the
  // adjusted input sample points

  // In X direction
  float S20 = 0.0f;
  float S11 = 0.0f;
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    S20 += footCycleCurvePosX[i] * footCycleCurvePosX[i];
    S11 += footCycleCurvePosX[i] * footCyclePosX[i];
  }
  if (S20 < 1e-7f)
  {
    m_footCycleCurveScale[0] = 1.0f;
  }
  else
  {
    m_footCycleCurveScale[0] = S11 / S20;
  }

  // In Z direction
  S20 = 0.0f;
  S11 = 0.0f;
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    S20 += footCycleCurvePosZ[i] * footCycleCurvePosZ[i];
    S11 += footCycleCurvePosZ[i] * footCyclePosZ[i];
  }
  if (S20 < 1e-7f)
  {
    m_footCycleCurveScale[1] = 1.0f;
  }
  else
  {
    m_footCycleCurveScale[1] = S11 / S20;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool FootCyclePredictor::evaluateFootCycleCurve(
  float footCycleUMid,
  float footCycleU,
  NMP::Vector3& footCyclePos)
{
  NMP::Vector3 footCycleAdjPos;
  bool result = false;
  footCycleAdjPos.setToZero();

  // Check for a valid foot cycle curve
  if (m_footCycleCurveState == FootCyclePredictor::kValidCurveCtrlPts)
  {
    float footCycleCurvePosX, footCycleCurvePosZ;

    //---------------------------
    // Determine which segment of the gait cycle curve to use
    if (footCycleU < footCycleUMid)
    {
      // INITIAL SWING
      float u = footCycleU / footCycleUMid;

      // Basis functions
      float omu = 1.0f - u;
      float omu2 = omu * omu;
      float u2 = u * u;
      float B[4];
      B[0] = omu * omu2;
      B[1] = 3 * u * omu2;
      B[2] = 3 * u2 * omu;
      B[3] = u * u2;

      // Evaluate the Bezier curve
      footCycleCurvePosX = B[0] * m_footCycleCurveCtrlPtsX[0] +
                           B[1] * m_footCycleCurveCtrlPtsX[1] +
                           B[2] * m_footCycleCurveCtrlPtsX[2] +
                           B[3] * m_footCycleCurveCtrlPtsX[3];

      footCycleCurvePosZ = B[0] * m_footCycleCurveCtrlPtsZ[0] +
                           B[1] * m_footCycleCurveCtrlPtsZ[1] +
                           B[2] * m_footCycleCurveCtrlPtsZ[2] +
                           B[3] * m_footCycleCurveCtrlPtsZ[3];
    }
    else
    {
      // TERMINAL SWING
      float u = (footCycleU - footCycleUMid) / (1.0f - footCycleUMid);

      // Basis functions
      float omu = 1.0f - u;
      float omu2 = omu * omu;
      float u2 = u * u;
      float B[4];
      B[0] = omu * omu2;
      B[1] = 3 * u * omu2;
      B[2] = 3 * u2 * omu;
      B[3] = u * u2;

      // Evaluate the Bezier curve
      footCycleCurvePosX = B[0] * m_footCycleCurveCtrlPtsX[3] +
                           B[1] * m_footCycleCurveCtrlPtsX[4] +
                           B[2] * m_footCycleCurveCtrlPtsX[5] +
                           B[3] * m_footCycleCurveCtrlPtsX[0];

      footCycleCurvePosZ = B[0] * m_footCycleCurveCtrlPtsZ[3] +
                           B[1] * m_footCycleCurveCtrlPtsZ[4] +
                           B[2] * m_footCycleCurveCtrlPtsZ[5] +
                           B[3] * m_footCycleCurveCtrlPtsZ[0];
    }

    //---------------------------
    // Apply the sample scaling factors
    float P0[2];
    float x = footCycleCurvePosX - m_footCycleCurveXbar[0];
    float z = footCycleCurvePosZ - m_footCycleCurveXbar[1];
    P0[0] = m_footCycleCurveVc[0] * x + m_footCycleCurveVc[2] * z;
    P0[1] = m_footCycleCurveVc[1] * x + m_footCycleCurveVc[3] * z;
    P0[0] *= m_footCycleCurveScale[0];
    P0[1] *= m_footCycleCurveScale[1];

    footCycleAdjPos.x = m_footCycleCurveVc[0] * P0[0] + m_footCycleCurveVc[1] * P0[1];
    footCycleAdjPos.x += m_footCycleCurveXbar[0];
    footCycleAdjPos.z = m_footCycleCurveVc[2] * P0[0] + m_footCycleCurveVc[3] * P0[1];
    footCycleAdjPos.z += m_footCycleCurveXbar[1];

    result = true;
  }
  else if (m_footCycleCurveState == FootCyclePredictor::kValidCurveCtrlPt0)
  {
    // Use the control point at the start of the foot cycle curve
    footCycleAdjPos.x = m_footCycleCurveCtrlPtsX[0];
    footCycleAdjPos.z = m_footCycleCurveCtrlPtsZ[0];
    result = true;
  }

  // Transform the predicted foot cycle start position back to the world coordinate frame.
  footCyclePos = m_alignUpAxisQuat.rotateVector(footCycleAdjPos);
  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
