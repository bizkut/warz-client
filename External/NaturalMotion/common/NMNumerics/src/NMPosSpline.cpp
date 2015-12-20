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
#include "NMNumerics/NMPosSpline.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
PosSpline::PosSpline(
  uint32_t       numKnots,
  const float*   knots,
  const Vector3* keys,
  const Vector3* tangentsA,
  const Vector3* tangentsB) : SimpleKnotVector(numKnots, knots)
{
  NMP_ASSERT(keys);
  NMP_ASSERT(tangentsA);
  NMP_ASSERT(tangentsB);

  uint32_t numSpans = m_numKnots - 1;

  // Allocate the memory for the position spline data
  m_keys = new Vector3[m_numKnots];
  m_tangentsA = new Vector3[numSpans];
  m_tangentsB = new Vector3[numSpans];

  // Set the position spline data
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    m_keys[i] = keys[i];
    m_tangentsA[i] = tangentsA[i];
    m_tangentsB[i] = tangentsB[i];
  }
  m_keys[numSpans] = keys[numSpans];
}

//----------------------------------------------------------------------------------------------------------------------
PosSpline::PosSpline(
  float          ka,
  float          kb,
  const Vector3& p0,
  const Vector3& p1,
  const Vector3& p2,
  const Vector3& p3) : SimpleKnotVector(2)
{
  uint32_t numSpans = m_numKnots - 1;

  // Allocate the memory for the quaternion spline data
  m_keys = new Vector3[m_numKnots];
  m_tangentsA = new Vector3[numSpans];
  m_tangentsB = new Vector3[numSpans];

  // Set the quaternion spline data
  set(ka, kb, p0, p1, p2, p3);
}

//----------------------------------------------------------------------------------------------------------------------
PosSpline::PosSpline(uint32_t maxNumKnots) : SimpleKnotVector(maxNumKnots)
{
  uint32_t numSpans = m_numKnots - 1;

  // Allocate the memory for the position spline data
  m_keys = new Vector3[m_maxNumKnots];
  m_tangentsA = new Vector3[numSpans];
  m_tangentsB = new Vector3[numSpans];
}

//----------------------------------------------------------------------------------------------------------------------
PosSpline::PosSpline(const PosSpline& psp) : SimpleKnotVector(psp.m_numKnots)
{
  uint32_t numSpans = m_numKnots - 1;

  // Allocate the memory for the position spline data
  m_keys = new Vector3[m_maxNumKnots];
  m_tangentsA = new Vector3[numSpans];
  m_tangentsB = new Vector3[numSpans];

  // Set the position spline data
  PosSpline::operator=(psp);
}

//----------------------------------------------------------------------------------------------------------------------
PosSpline::~PosSpline()
{
  delete[] m_keys;
  delete[] m_tangentsA;
  delete[] m_tangentsB;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 PosSpline::getKey(uint32_t i) const
{
  NMP_ASSERT(i < m_numKnots);
  return m_keys[i];
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::setKey(uint32_t i, const Vector3& key)
{
  NMP_ASSERT(i < m_numKnots);
  m_keys[i] = key;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 PosSpline::getTangentA(uint32_t i) const
{
  NMP_ASSERT(i < m_numKnots - 1);
  return m_tangentsA[i];
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::setTangentA(uint32_t i, const Vector3& tangentA)
{
  NMP_ASSERT(i < m_numKnots - 1);
  m_tangentsA[i] = tangentA;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 PosSpline::getTangentB(uint32_t i) const
{
  NMP_ASSERT(i < m_numKnots - 1);
  return m_tangentsB[i];
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::setTangentB(uint32_t i, const Vector3& tangentB)
{
  NMP_ASSERT(i < m_numKnots - 1);
  m_tangentsB[i] = tangentB;
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::setToZero()
{
  uint32_t numSpans = m_numKnots - 1;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    m_keys[i].setToZero();
    m_tangentsA[i].setToZero();
    m_tangentsB[i].setToZero();
  }
  m_keys[numSpans].setToZero();
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::set(
  uint32_t       numKnots,
  const float*   knots,
  const Vector3* keys,
  const Vector3* tangentsA,
  const Vector3* tangentsB)
{
  NMP_ASSERT(keys);
  NMP_ASSERT(tangentsA);
  NMP_ASSERT(tangentsB);

  // Set the knot vector
  setKnots(numKnots, knots);

  // Set the position spline data
  uint32_t numSpans = m_numKnots - 1;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    m_keys[i] = keys[i];
    m_tangentsA[i] = tangentsA[i];
    m_tangentsB[i] = tangentsB[i];
  }
  m_keys[numSpans] = keys[numSpans];
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::set(float ka, float kb, const Vector3& p0, const Vector3& p1)
{
  NMP_ASSERT(m_maxNumKnots >= 2);
  NMP_ASSERT(ka < kb);

  // Set the knot vector
  m_numKnots = 2;
  m_knots[0] = ka;
  m_knots[1] = kb;

  // Degree elevation to cubic Bezier
  const float one_three = 1.0f / 3.0f;
  const float two_three = 2.0f / 3.0f;
  m_keys[0] = p0;
  m_tangentsA[0] = p0 * two_three + p1 * one_three;
  m_tangentsB[0] = p0 * one_three + p1 * two_three;
  m_keys[1] = p1;
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::set(float ka, float kb, const Vector3& p0, const Vector3& p1, const Vector3& p2)
{
  NMP_ASSERT(m_maxNumKnots >= 2);
  NMP_ASSERT(ka < kb);

  // Set the knot vector
  m_numKnots = 2;
  m_knots[0] = ka;
  m_knots[1] = kb;

  // Degree elevation to cubic Bezier
  const float one_three = 1.0f / 3.0f;
  const float two_three = 2.0f / 3.0f;
  m_keys[0] = p0;
  Vector3 p1_fac = p1 * two_three;
  m_tangentsA[0] = p1_fac + p0 * one_three;
  m_tangentsB[0] = p2 * one_three + p1_fac;
  m_keys[1] = p2;
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::set(
  float          ka,
  float          kb,
  const Vector3& p0,
  const Vector3& p1,
  const Vector3& p2,
  const Vector3& p3)
{
  NMP_ASSERT(m_maxNumKnots >= 2);
  NMP_ASSERT(ka < kb);

  // Set the knot vector
  m_numKnots = 2;
  m_knots[0] = ka;
  m_knots[1] = kb;

  // Set the Bezier control points
  m_keys[0] = p0;
  m_tangentsA[0] = p1;
  m_tangentsB[0] = p2;
  m_keys[1] = p3;
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::begin(float t, const Vector3& p0)
{
  m_numKnots = 1;
  m_knots[0] = t;
  m_keys[0] = p0;
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::append(float t, const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
  NMP_ASSERT(m_numKnots < m_maxNumKnots);
  uint32_t numSpans = m_numKnots - 1;
  m_knots[m_numKnots] = t;
  m_tangentsA[numSpans] = p1;
  m_tangentsB[numSpans] = p2;
  m_keys[m_numKnots] = p3;
  m_numKnots++;
}

//----------------------------------------------------------------------------------------------------------------------
bool PosSpline::insertKnot(float t)
{
  const float tol = 1e-4f;
  uint32_t indx, intv;
  uint32_t numSpans = m_numKnots - 1;

  // Check if t lies outside the knot range
  if (t <= m_knots[0] + tol ||
      t >= m_knots[numSpans] - tol)
  {
    return false;
  }

  // Check if t already lies at a knot site
  indx = closestKnot(t);
  if (t >= m_knots[indx] - tol && t <= m_knots[indx] + tol)
  {
    return false;
  }

  // Allocate more space if required
  if (m_numKnots == m_maxNumKnots)
  {
    PosSpline psp(m_maxNumKnots + 1);
    psp = *this;
    swap(psp);
  }

  // Get the interval that t lies within
  if (t > m_knots[indx])
  {
    intv = indx;
  }
  else
  {
    intv = indx - 1;
  }

  // Knot insertion not implemented.
  NMP_ASSERT_FAIL();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PosSpline::removeKnot(float t)
{
  const float tol = 1e-4f;
  uint32_t indx = closestKnot(t);

  // Check if t lies close to a knot site
  if (fabs(m_knots[indx] - t) > tol)
  {
    return false;
  }

  // Remove the knot
  return removeKnotAtIndex(indx);
}

//----------------------------------------------------------------------------------------------------------------------
bool PosSpline::removeKnotAtIndex(uint32_t indx)
{
  NMP_ASSERT(indx < m_numKnots);

  // Check if we are removing the start knot
  if (indx == 0)
  {
    set(m_numKnots - 1,
        &m_knots[1],
        &m_keys[1],
        &m_tangentsA[1],
        &m_tangentsB[1]);

    return true;
  }

  // Check if we are removing the end knot
  if (indx == m_numKnots - 1)
  {
    m_numKnots--;
    return true;
  }

  // Knot removal not implemented.
  NMP_ASSERT_FAIL();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
PosSpline& PosSpline::operator=(const PosSpline& psp)
{
  NMP_ASSERT(m_maxNumKnots >= psp.m_numKnots);
  if (this != &psp)
  {
    m_numKnots = psp.m_numKnots;
    uint32_t numSpans = m_numKnots - 1;

    // Set the position spline data
    for (uint32_t i = 0; i < numSpans; ++i)
    {
      m_knots[i] = psp.m_knots[i];
      m_keys[i] = psp.m_keys[i];
      m_tangentsA[i] = psp.m_tangentsA[i];
      m_tangentsB[i] = psp.m_tangentsB[i];
    }
    m_knots[numSpans] = psp.m_knots[numSpans];
    m_keys[numSpans] = psp.m_keys[numSpans];
  }
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
bool PosSpline::subSpan(const PosSpline& psp, float ka, float kb)
{
  uint32_t startKnotIndex, endKnotIndex;
  uint32_t numSpanKnots, numSpanKnotsEx, extra;

  // Compute the indices of the knots that are inclusive of the required
  // sub-span range [ka : kb]
  if (!psp.getKnotsForSamples(ka, kb, startKnotIndex, endKnotIndex))
  {
    return false;
  }

  // Check how many additional knots need to be inserted
  extra = 0;
  if (psp.m_knots[startKnotIndex] != ka) extra++;
  if (psp.m_knots[endKnotIndex] != kb) extra++;

  // Allocate enough space to copy the inclusive curve spans and an
  // additional knot site if we need to break the curve at ka and kb
  numSpanKnots = endKnotIndex - startKnotIndex + 1;
  numSpanKnotsEx = numSpanKnots;
  if (extra > 0) numSpanKnotsEx++; // Extra one for knot insertion
  if (m_maxNumKnots < numSpanKnotsEx)
  {
    PosSpline pspTemp(numSpanKnotsEx);
    swap(pspTemp);
  }

  // Extract the inclusive curve spans
  set(numSpanKnots,
      &psp.m_knots[startKnotIndex],
      &psp.m_keys[startKnotIndex],
      &psp.m_tangentsA[startKnotIndex],
      &psp.m_tangentsB[startKnotIndex]);

  // Check if we need to break the curve at ka or kb and trim the curve
  if (extra > 0)
  {
    // Insert a knot at ka
    if (insertKnot(ka))
    {
      set(numSpanKnots,
          &m_knots[1],
          &m_keys[1],
          &m_tangentsA[1],
          &m_tangentsB[1]);
    }

    // Insert a knot at kb
    if (insertKnot(kb))
    {
      m_numKnots--;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void PosSpline::swap(PosSpline& psp)
{
  if (this != &psp)
  {
    uint32_t maxNumKnots = m_maxNumKnots;
    uint32_t numKnots = m_numKnots;
    float* knots = m_knots;
    Vector3* keys = m_keys;
    Vector3* tangentsA = m_tangentsA;
    Vector3* tangentsB = m_tangentsB;

    m_maxNumKnots = psp.m_maxNumKnots;
    m_numKnots = psp.m_numKnots;
    m_knots = psp.m_knots;
    m_keys = psp.m_keys;
    m_tangentsA = psp.m_tangentsA;
    m_tangentsB = psp.m_tangentsB;

    psp.m_maxNumKnots = maxNumKnots;
    psp.m_numKnots = numKnots;
    psp.m_knots = knots;
    psp.m_keys = keys;
    psp.m_tangentsA = tangentsA;
    psp.m_tangentsB = tangentsB;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 PosSpline::evaluate(float t) const
{
  // Find the interval that the value t lies on
  uint32_t intv;

#ifdef NMP_ENABLE_ASSERTS
  int32_t rgn =
#endif
    interval(t, intv);
  NMP_ASSERT(rgn == 0);

  // Compute the u-parameter from the appropriate knot interval
  float u = (t - m_knots[intv]) / (m_knots[intv+1] - m_knots[intv]);

  // Compute the degree 3 basis functions
  float omu = 1.0f - u;
  float omu2 = omu * omu;
  float u2 = u * u;
  float B0 = omu2 * omu;
  float B1 = 3.0f * u * omu2;
  float B2 = 3.0f * u2 * omu;
  float B3 = u2 * u;

  // Evaluate the curve
  Vector3 xt;
  xt = (m_keys[intv] * B0) +
       (m_tangentsA[intv] * B1) +
       (m_tangentsB[intv] * B2) +
       (m_keys[intv+1] * B3);

  return xt;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 PosSpline::derivative1(float t) const
{
  // Find the interval that the value t lies on
  uint32_t intv;
#ifdef NMP_ENABLE_ASSERTS
  int32_t rgn =
#endif
    interval(t, intv);
  NMP_ASSERT(rgn == 0);

  // Compute the u-parameter from the appropriate knot interval
  float tFac = 1.0f / (m_knots[intv+1] - m_knots[intv]);
  float u = tFac * (t - m_knots[intv]);

  // Compute the degree 2 basis functions
  float omu = 1.0f - u;
  float dB0 = omu * omu;
  float dB1 = 2.0f * u * omu;
  float dB2 = u * u;

  // Evaluate the first derivative of the curve
  Vector3 dxt;
  dxt = (m_tangentsA[intv] - m_keys[intv]) * dB0 +
        (m_tangentsB[intv] - m_tangentsA[intv]) * dB1 +
        (m_keys[intv+1] - m_tangentsB[intv]) * dB2;

  // The derivative is expressed in value per u-unit, so convert this to
  // value per knot-unit
  dxt *= (3.0f * tFac); // Push the factor of three from the above derivative into here

  return dxt;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 PosSpline::derivative2(float t) const
{
  // Find the interval that the value t lies on
  uint32_t intv;
#ifdef NMP_ENABLE_ASSERTS
  int32_t rgn =
#endif
    interval(t, intv);
  NMP_ASSERT(rgn == 0);

  // Compute the u-parameter from the appropriate knot interval
  float tFac = 1.0f / (m_knots[intv+1] - m_knots[intv]);
  float u = tFac * (t - m_knots[intv]);

  // Compute the degree 1 basis functions
  float omu = 1.0f - u;

  // Evaluate the second derivative of the curve
  Vector3 d2xt;
  d2xt = (m_tangentsB[intv] - 2.0f * m_tangentsA[intv] + m_keys[intv]) * omu +
         (m_keys[intv+1] - 2.0f * m_tangentsB[intv] + m_tangentsA[intv]) * u;

  // The derivative is expressed in value per (u-unit)^2, so convert this to
  // value per (knot-unit)^2
  d2xt *= (6.0f * tFac * tFac); // Push the factor of six from the above derivative into here

  return d2xt;
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const PosSpline& psp)
{
  Vector3 p;
  float t;

  uint32_t numKnots = psp.getNumKnots();
  uint32_t numSpans = numKnots - 1;

  os << "Num knots = " << numKnots << std::endl;
  for (uint32_t i = 0; i < numKnots; ++i)
  {
    t = psp.getKnot(i);
    os << t << std::endl;
  }
  os << std::endl;

  os << "Keys" << std::endl;
  for (uint32_t i = 0; i < numKnots; ++i)
  {
    p = psp.getKey(i);
    os << p.x << " " << p.y << " " << p.z << std::endl;
  }
  os << std::endl;

  os << "tangentsA" << std::endl;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    p = psp.getTangentA(i);
    os << p.x << " " << p.y << " " << p.z << std::endl;
  }
  os << std::endl;

  os << "tangentsB" << std::endl;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    p = psp.getTangentB(i);
    os << p.x << " " << p.y << " " << p.z << std::endl;
  }
  os << std::endl;

  return os;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
