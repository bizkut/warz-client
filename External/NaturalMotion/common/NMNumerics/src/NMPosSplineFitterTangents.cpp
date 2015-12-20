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
#include "NMNumerics/NMPosSplineFitterTangents.h"
#include "NMNumerics/NMSimpleSplineFittingUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// PosSplineFitterTangents
//----------------------------------------------------------------------------------------------------------------------
PosSplineFitterTangents::
PosSplineFitterTangents(uint32_t numSamples, const float* ts, const Vector3* data, uint32_t numKnots, const float* knots) :
  PosSplineFitterBase(numSamples),
  m_pspan(2)
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Set the knot vector
  setKnots(numKnots, knots);

  // Fit a quaternion spline to the data samples
  fit(numSamples, ts, data);
}

//----------------------------------------------------------------------------------------------------------------------
PosSplineFitterTangents::
PosSplineFitterTangents(uint32_t numSamples, const Vector3* data, uint32_t numKnots, const float* knots) :
  PosSplineFitterBase(numSamples),
  m_pspan(2)
{
  NMP_ASSERT(m_maxNumSamples >= 2);

  // Set the knot vector
  setKnots(numKnots, knots);

  // Fit a quaternion spline to the data samples
  fitRegular(numSamples, data);
}

//----------------------------------------------------------------------------------------------------------------------
PosSplineFitterTangents::PosSplineFitterTangents(uint32_t maxNumSamples) :
  PosSplineFitterBase(maxNumSamples),
  m_pspan(2)
{
  NMP_ASSERT(m_maxNumSamples >= 2);
}

//----------------------------------------------------------------------------------------------------------------------
PosSplineFitterTangents::~PosSplineFitterTangents()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool PosSplineFitterTangents::fit(uint32_t numSamples, const float* ts, const Vector3* data)
{
  NMP_ASSERT(ts);
  NMP_ASSERT(data);
  NMP_ASSERT(numSamples >= 2);

  bool     status, result;
  Vector3  keyA, keyB, tangentA, tangentB;
  float    ta, tb, tfac;
  uint32_t ia, ib;
  uint32_t numSpanSamples;
  const float*   spanTs;
  const Vector3* spanPosData;
  float   u, omu, omu2, u2;
  float   B0, B1, B2, B3;
  float   A[3], iA[3], det, desc;
  Vector3 r, b[2];

  //------------------------------
  // Get the knot vector
  uint32_t numKnots = m_psp.getNumKnots();
  const float* knots = m_psp.getKnots();
  NMP_ASSERT(SimpleSplineFittingUtils::isKnotVectorValid(numKnots, knots));

  // Set the time samples
  setTs(numSamples, ts);

  // Set the position data
  setPosData(numSamples, data);

  // Information
  uint32_t numSpans = numKnots - 1;

  // First position key
  ta = knots[0];
  keyA = SimpleSplineFittingUtils::posInterpolate(ta, m_numSamples, m_ts, m_posData);
  m_psp.setKey(0, keyA);

  //------------------------------
  // Iterate over the knot spans and fit single span Bezier curves
  result = true;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    // Recover the knot sites
    tb = knots[i+1];

    // Compute the interpolated positions at the knot sites
    keyB = SimpleSplineFittingUtils::posInterpolate(tb, m_numSamples, m_ts, m_posData);

    // Recover the data samples within the knot vector. We exclude samples that are located at
    // ta or tb since they do not contribute to the solution
    status = SimpleSplineFittingUtils::findSamplesExclusive(ta, tb, ia, ib, m_numSamples, m_ts);
    if (!status)
    {
      // Set the span from the linear interpolation
      m_pspan.set(ta, tb, keyA, keyB);
      tangentA = m_pspan.getTangentA(0);
      tangentB = m_pspan.getTangentB(0);
    }
    else
    {
      // Get the span data
      numSpanSamples = ib - ia + 1;
      spanTs = &m_ts[ia];
      spanPosData = &m_posData[ia];

      // Check for the identity channel
      if (SimpleSplineFittingUtils::isPosChannelUnchanging(keyA, keyB) &&
          SimpleSplineFittingUtils::isPosChannelUnchanging(keyA, numSpanSamples, spanPosData))
      {
        // Set the span from the linear interpolation
        m_pspan.set(ta, tb, keyA, keyB);
        tangentA = m_pspan.getTangentA(0);
        tangentB = m_pspan.getTangentB(0);
      }
      else
      {
        // Compute the knot difference factor
        tfac = 1.0f / (tb - ta);

        //------------------------------
        // Single sample only case: Fit to quadratic Bezier curve
        if (numSpanSamples == 1)
        {
          // Quadratic Bezier basis functions
          u = (spanTs[0] - ta) * tfac;
          omu = 1.0f - u;
          B0 = omu * omu;
          B1 = 2.0f * u * omu;
          B2 = u * u;

          // Solve for the control point P1
          b[0] = spanPosData[0] - (keyA * B0) - (keyB * B2);
          r = b[0] / B1; // P1

          // Set the cubic Bezier curve from the quadratic curve control points.
          // The cubic curve is computed by degree elevation.
          m_pspan.set(ta, tb, keyA, r, keyB);
          tangentA = m_pspan.getTangentA(0);
          tangentB = m_pspan.getTangentB(0);
        }
        else
        {
          //------------------------------
          // General case
          //
          A[0] = A[1] = A[2] = 0.0f;
          b[0].setToZero();
          b[1].setToZero();
          for (uint32_t j = 0; j < numSpanSamples; ++j)
          {
            // Cubic Bezier basis functions
            u = (spanTs[j] - ta) * tfac;
            omu = 1.0f - u;
            omu2 = omu * omu;
            u2 = u * u;
            B0 = omu2 * omu;
            B1 = 3.0f * u * omu2;
            B2 = 3.0f * u2 * omu;
            B3 = u2 * u;

            // Design matrix
            r = spanPosData[j] - (keyA * B0) - (keyB * B3);
            A[0] += (B1 * B1);
            A[1] += (B1 * B2);
            A[2] += (B2 * B2);
            b[0] += (r * B1);
            b[1] += (r * B2);
          }

          // Invert matrix A
          det = A[0] * A[2] - A[1] * A[1];
          NMP_ASSERT(det != 0.0f);
          desc = 1.0f / det;
          iA[0] = desc * A[2];
          iA[1] = desc * -A[1];
          iA[2] = desc * A[0];

          // Solve for the tangent control points P1, P2
          tangentA = b[0] * iA[0] + b[1] * iA[1]; // P1
          tangentB = b[0] * iA[1] + b[1] * iA[2]; // P2
        }

      } // End of general case
    }

    //------------------------------
    // Append the single span curve segment
    m_psp.setTangentA(i, tangentA);
    m_psp.setTangentB(i, tangentB);
    m_psp.setKey(i + 1, keyB);

    // Update the iteration parameters
    ta = tb;
    keyA = keyB;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
