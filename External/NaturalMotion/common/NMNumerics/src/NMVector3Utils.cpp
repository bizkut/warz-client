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
#include "NMNumerics/NMVector3Utils.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
void vecBounds(uint32_t numSamples, const Vector3* v, Vector3& minV, Vector3& maxV)
{
  if (numSamples > 0)
  {
    NMP_ASSERT(v);
    minV.set(v[0].x, v[0].y, v[0].z);
    maxV.set(v[0].x, v[0].y, v[0].z);

    for (uint32_t i = 1; i < numSamples; ++i)
    {
      // X component
      if (v[i].x < minV.x)
      {
        minV.x = v[i].x;
      }
      else if (v[i].x > maxV.x)
      {
        maxV.x = v[i].x;
      }

      // Y Component
      if (v[i].y < minV.y)
      {
        minV.y = v[i].y;
      }
      else if (v[i].y > maxV.y)
      {
        maxV.y = v[i].y;
      }

      // Z Component
      if (v[i].z < minV.z)
      {
        minV.z = v[i].z;
      }
      else if (v[i].z > maxV.z)
      {
        maxV.z = v[i].z;
      }
    }
  }
  else
  {
    minV.set(0.0f, 0.0f, 0.0f);
    maxV.set(0.0f, 0.0f, 0.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void vecMean(uint32_t numSamples, const Vector3* v, Vector3& vbar)
{
  vbar.set(0.0f, 0.0f, 0.0f);

  if (numSamples > 0)
  {
    NMP_ASSERT(v);
    for (uint32_t i = 0; i < numSamples; ++i)
      vbar += v[i];

    float fac = 1.0f / (float)numSamples;
    vbar *= fac;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void vecMean(
  uint32_t              numArrays,
  const uint32_t*       vecSizes,
  const Vector3* const* vecArrays,
  Vector3&              vbar)
{
  vbar.set(0.0f, 0.0f, 0.0f);
  uint32_t numSamples = 0;

  for (uint32_t k = 0; k < numArrays; ++k)
  {
    uint32_t n = vecSizes[k];
    const Vector3* v = vecArrays[k];

    for (uint32_t i = 0; i < n; ++i, ++numSamples)
      vbar += v[i];
  }

  if (numSamples > 0)
  {
    float fac = 1.0f / (float)numSamples;
    vbar *= fac;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void vecVar(uint32_t numSamples, const Vector3* v, Vector3& vvar, bool zeroMean)
{
  vvar.set(0.0f, 0.0f, 0.0f);

  if (numSamples > 0)
  {
    NMP_ASSERT(v);

    NMP::Vector3 v2, vbar;
    float fac = 1.0f / (float)numSamples;

    if (zeroMean)
    {
      for (uint32_t i = 0; i < numSamples; ++i)
      {
        v2 = v[i];
        v2.multiplyElements(v[i]);
        vvar += v2;
      }
      vvar *= fac;
    }
    else
    {
      vbar.set(0.0f, 0.0f, 0.0f);
      for (uint32_t i = 0; i < numSamples; ++i)
      {
        // Sums
        vbar += v[i];

        // Sum of squares
        v2 = v[i];
        v2.multiplyElements(v[i]);
        vvar += v2;
      }

      // Mean
      vbar *= fac;

      // Variance
      vvar *= fac;
      v2 = vbar;
      v2.multiplyElements(vbar);
      vvar -= v2;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
