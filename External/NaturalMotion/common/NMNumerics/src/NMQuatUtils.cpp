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
#include "NMNumerics/NMQuatUtils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
void quatMean(uint32_t numSamples, const Quat* quats, Quat& qbar)
{
  if (numSamples == 0)
  {
    qbar.identity();
    return;
  }

  NMP_ASSERT(quats);
  if (numSamples == 1)
  {
    qbar = quats[0];
    return;
  }

  Vector<float> costs(numSamples);
  costs.set(0.0f);

  // Shortest geodesic path cost vector. This is essentially the
  // column summation of the covariant |cos(theta/2)| angular measures
  // between every pairwise quaternion.
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    costs[i] += 1.0f; // Compare with itself
    for (uint32_t k = i + 1; k < numSamples; ++k)
    {
      float d = fabs(quats[i].dot(quats[k])); // | cos(theta/2) |
      costs[i] += d; // This row
      costs[k] += d; // Row below diagonal
    }
  }

  // Pick the rotation that is closest to all others. i.e. find distance
  // measure with highest dot product.
  float qMax = costs[0];
  uint32_t qIndx = 0;
  for (uint32_t i = 1; i < numSamples; ++i)
  {
    if (costs[i] > qMax)
    {
      qMax = costs[i];
      qIndx = i;
    }
  }

  // Transform all rotations relative to the chosen quat and compute the
  // exponential map mean of these relative quats
  Quat dq, inv_qc = quats[qIndx];
  inv_qc.conjugate();
  Vector3 v, expMean;
  expMean.setToZero();
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    dq = inv_qc * quats[i];
    if (dq.w < 0.0f)
    {
      dq = -dq; // Representation in the positive hemisphere
    }
    v = dq.log();
    expMean += v;
  }
  expMean /= (float)numSamples;

  // Recover the mean quaternion
  dq.exp(expMean);
  qbar = quats[qIndx] * dq;
}

//----------------------------------------------------------------------------------------------------------------------
float quatVar(uint32_t numSamples, const Quat* quats, bool zeroMean)
{
  if (numSamples == 0)
    return 0.0f;

  float d, theta, var;
  NMP::Quat qbar;
  NMP_ASSERT(quats);

  if (zeroMean)
  {
    var = 0.0f;
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      d = fabs(quats[i].w); // | cos(theta/2) |
      if (d > 1.0f)
        d = 1.0f; // Clamp to range
      theta = 2 * acos(d);
      var += (theta * theta);
    }
    var /= (float)numSamples;
  }
  else
  {
    quatMean(numSamples, quats, qbar);

    var = 0.0f;
    for (uint32_t i = 0; i < numSamples; ++i)
    {
      d = fabs(qbar.dot(quats[i])); // | cos(theta/2) |
      if (d > 1.0f)
        d = 1.0f; // Clamp to range
      theta = 2 * acos(d);
      var += (theta * theta);
    }
    var /= (float)numSamples;
  }

  return var;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
