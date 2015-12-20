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
#include "NMNumerics/NMQuatSpline.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
QuatSpline::QuatSpline(
  uint32_t       numKnots,
  const float*   knots,
  const Quat*    keys,
  const Vector3* velsA,
  const Vector3* velsB) : SimpleKnotVector(numKnots, knots)
{
  NMP_ASSERT(keys);
  NMP_ASSERT(velsA);
  NMP_ASSERT(velsB);

  uint32_t numSpans = m_numKnots - 1;

  // Allocate the memory for the quaternion spline data
  m_keys = new Quat[m_numKnots];
  m_velsA = new Vector3[numSpans];
  m_velsB = new Vector3[numSpans];

  // Set the quaternion spline data
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    m_keys[i] = keys[i];
    m_velsA[i] = velsA[i];
    m_velsB[i] = velsB[i];
  }
  m_keys[numSpans] = keys[numSpans];
}

//----------------------------------------------------------------------------------------------------------------------
QuatSpline::QuatSpline(float ka, float kb, const Quat& qa, const Quat& qb) : SimpleKnotVector(2)
{
  uint32_t numSpans = m_numKnots - 1;

  // Allocate the memory for the quaternion spline data
  m_keys = new Quat[m_numKnots];
  m_velsA = new Vector3[numSpans];
  m_velsB = new Vector3[numSpans];

  // Set the quaternion spline data
  set(ka, kb, qa, qb);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSpline::QuatSpline(uint32_t maxNumKnots) : SimpleKnotVector(maxNumKnots)
{
  uint32_t numSpans = m_numKnots - 1;

  // Allocate the memory for the quaternion spline data
  m_keys = new Quat[m_maxNumKnots];
  m_velsA = new Vector3[numSpans];
  m_velsB = new Vector3[numSpans];
}

//----------------------------------------------------------------------------------------------------------------------
QuatSpline::QuatSpline(const QuatSpline& qsp) : SimpleKnotVector(qsp.m_numKnots)
{
  uint32_t numSpans = m_numKnots - 1;

  // Allocate the memory for the quaternion spline data
  m_keys = new Quat[m_maxNumKnots];
  m_velsA = new Vector3[numSpans];
  m_velsB = new Vector3[numSpans];

  // Set the quaternion spline data
  QuatSpline::operator=(qsp);
}

//----------------------------------------------------------------------------------------------------------------------
QuatSpline::~QuatSpline()
{
  delete[] m_keys;
  delete[] m_velsA;
  delete[] m_velsB;
}

//----------------------------------------------------------------------------------------------------------------------
Quat QuatSpline::getKey(uint32_t i) const
{
  NMP_ASSERT(i < m_numKnots);
  return m_keys[i];
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSpline::setKey(uint32_t i, const Quat& q)
{
  NMP_ASSERT(i < m_numKnots);
  m_keys[i] = q;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 QuatSpline::getVelA(uint32_t i) const
{
  NMP_ASSERT(i < m_numKnots - 1);
  return m_velsA[i];
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSpline::setVelA(uint32_t i, const Vector3& wa)
{
  NMP_ASSERT(i < m_numKnots - 1);
  m_velsA[i] = wa;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 QuatSpline::getVelB(uint32_t i) const
{
  NMP_ASSERT(i < m_numKnots - 1);
  return m_velsB[i];
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSpline::setVelB(uint32_t i, const Vector3& wb)
{
  NMP_ASSERT(i < m_numKnots - 1);
  m_velsB[i] = wb;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSpline::identity()
{
  uint32_t numSpans = m_numKnots - 1;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    m_keys[i].identity();
    m_velsA[i].setToZero();
    m_velsB[i].setToZero();
  }
  m_keys[numSpans].identity();
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSpline::set(
  uint32_t       numKnots,
  const float*   knots,
  const Quat*    keys,
  const Vector3* velsA,
  const Vector3* velsB)
{
  NMP_ASSERT(keys);
  NMP_ASSERT(velsA);
  NMP_ASSERT(velsB);

  // Set the knot vector
  setKnots(numKnots, knots);

  // Set the quaternion spline data
  uint32_t numSpans = m_numKnots - 1;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    m_keys[i] = keys[i];
    m_velsA[i] = velsA[i];
    m_velsB[i] = velsB[i];
  }
  m_keys[numSpans] = keys[numSpans];
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSpline::set(float ka, float kb, const Quat& qa, const Quat& qb)
{
  NMP_ASSERT(m_maxNumKnots >= 2);
  NMP_ASSERT(ka < kb);

  // Set the quaternion spline data
  m_numKnots = 2;
  m_knots[0] = ka;
  m_knots[1] = kb;
  m_keys[0] = qa;
  m_keys[1] = qb;

  Quat dq;
  Quat iqa = qa;
  iqa.conjugate();
  dq.multiply(iqa, qb);
  m_velsA[0] = m_velsB[0] = (1.0f / 3.0f) * dq.log();
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSpline::set(float ka, float kb, const Quat& qa, const Vector3& wa, const Vector3& wb, const Quat& qb)
{
  NMP_ASSERT(m_maxNumKnots >= 2);
  NMP_ASSERT(ka < kb);

  // Set the quaternion spline data
  m_numKnots = 2;
  m_knots[0] = ka;
  m_knots[1] = kb;
  m_keys[0] = qa;
  m_velsA[0] = wa;
  m_velsB[0] = wb;
  m_keys[1] = qb;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSpline::begin(float t, const Quat& qa)
{
  m_numKnots = 1;
  m_knots[0] = t;
  m_keys[0] = qa;
}

//----------------------------------------------------------------------------------------------------------------------
void QuatSpline::append(float t, const Vector3& wa, const Vector3& wb, const Quat& qb)
{
  NMP_ASSERT(m_numKnots < m_maxNumKnots);
  uint32_t numSpans = m_numKnots - 1;
  m_knots[m_numKnots] = t;
  m_keys[m_numKnots] = qb;
  m_velsA[numSpans] = wa;
  m_velsB[numSpans] = wb;
  m_numKnots++;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSpline::insertKnot(float t)
{
  const float tol = 1e-4f;
  const float third = 1.0f / 3.0f;
  Quat     qa, qb, q, dq, iq, v;
  Vector3  wa, wb, w, wa1, wb1, wa2, wb2;
  float    ta, tb, u, omu, dt;
  uint32_t indx, intv, kv;
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
    QuatSpline qsp(m_maxNumKnots + 1);
    qsp = *this;
    swap(qsp);
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

  // Get the span parameters
  kv = intv + 1;
  ta = m_knots[intv];
  tb = m_knots[kv];
  qa = m_keys[intv];
  qb = m_keys[kv];
  wa = m_velsA[intv];
  wb = m_velsB[intv];

  // Interpolant
  dt = tb - ta;
  u = (t - ta) / dt;
  omu = 1 - u;

  // Compute the angular velocity parameter at the knot site
  q = evaluate(t);
  dq = derivative1(t); // rate of change per knot unit [0:dt]
  dq *= dt; // rate of change per u unit [0:1]
  iq = q;
  iq.conjugate();
  v = iq * dq; // On span [0:1] : dq(0)/dt = q0 * (3*w1)
  w.set(third * v.x, third * v.y, third * v.z); // Angular velocity on span [0:1]

  // Left side of span
  wa1 = wa * u;
  wb1 = w * u;

  // Right side of span
  wa2 = w * omu;
  wb2 = wb * omu;

  // Make the new quaternion spline
  m_knots[m_numKnots] = m_knots[numSpans];
  m_keys[m_numKnots] = m_keys[numSpans];
  for (uint32_t i = numSpans - 1; i > intv; --i)
  {
    m_knots[i+1] = m_knots[i];
    m_keys[i+1] = m_keys[i];
    m_velsA[i+1] = m_velsA[i];
    m_velsB[i+1] = m_velsB[i];
  }

  m_numKnots++;
  m_knots[kv] = t;
  m_keys[kv] = q;
  m_velsA[intv] = wa1;
  m_velsA[kv] = wa2;
  m_velsB[intv] = wb1;
  m_velsB[kv] = wb2;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSpline::removeKnot(float t)
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
bool QuatSpline::removeKnotAtIndex(uint32_t indx)
{
  NMP_ASSERT(indx < m_numKnots);

  Vector3 wa, wb;
  float   alpha_a, alpha_b;
  float   u, omu;

  // Check if we are removing the start knot
  if (indx == 0)
  {
    set(m_numKnots - 1,
        &m_knots[1],
        &m_keys[1],
        &m_velsA[1],
        &m_velsB[1]);

    return true;
  }

  // Check if we are removing the end knot
  if (indx == m_numKnots - 1)
  {
    m_numKnots--;
    return true;
  }

  // Interpolant
  u = (m_knots[indx] - m_knots[indx-1]) / (m_knots[indx+1] - m_knots[indx-1]);
  omu = 1.0f - u;

  // Angular velocity vectors
  alpha_a = 1 / u;
  alpha_b = 1 / (1 - u);
  wa = m_velsA[indx-1] * alpha_a;
  wb = m_velsB[indx] * alpha_b;

  // Make the new quaternion spline
  for (uint32_t i = indx + 1; i < m_numKnots; ++i)
  {
    m_knots[i-1] = m_knots[i];
    m_keys[i-1] = m_keys[i];
  }

  for (uint32_t i = indx + 1; i < m_numKnots - 1; ++i)
  {
    m_velsA[i-1] = m_velsA[i];
    m_velsB[i-1] = m_velsB[i];
  }

  m_velsA[indx-1] = wa;
  m_velsB[indx-1] = wb;

  m_numKnots--;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
QuatSpline& QuatSpline::operator=(const QuatSpline& qsp)
{
  NMP_ASSERT(m_maxNumKnots >= qsp.m_numKnots);
  if (this != &qsp)
  {
    m_numKnots = qsp.m_numKnots;
    uint32_t numSpans = m_numKnots - 1;

    // Set the quaternion spline data
    for (uint32_t i = 0; i < numSpans; ++i)
    {
      m_knots[i] = qsp.m_knots[i];
      m_keys[i] = qsp.m_keys[i];
      m_velsA[i] = qsp.m_velsA[i];
      m_velsB[i] = qsp.m_velsB[i];
    }
    m_knots[numSpans] = qsp.m_knots[numSpans];
    m_keys[numSpans] = qsp.m_keys[numSpans];
  }
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
bool QuatSpline::subSpan(const QuatSpline& qsp, float ka, float kb)
{
  uint32_t startKnotIndex, endKnotIndex;
  uint32_t numSpanKnots, numSpanKnotsEx, extra;

  // Compute the indices of the knots that are inclusive of the required
  // sub-span range [ka : kb]
  if (!qsp.getKnotsForSamples(ka, kb, startKnotIndex, endKnotIndex))
  {
    return false;
  }

  // Check how many additional knots need to be inserted
  extra = 0;
  if (qsp.m_knots[startKnotIndex] != ka) extra++;
  if (qsp.m_knots[endKnotIndex] != kb) extra++;

  // Allocate enough space to copy the inclusive curve spans and an
  // additional knot site if we need to break the curve at ka and kb
  numSpanKnots = endKnotIndex - startKnotIndex + 1;
  numSpanKnotsEx = numSpanKnots;
  if (extra > 0) numSpanKnotsEx++; // Extra one for knot insertion
  if (m_maxNumKnots < numSpanKnotsEx)
  {
    QuatSpline qspTemp(numSpanKnotsEx);
    swap(qspTemp);
  }

  // Extract the inclusive curve spans
  set(numSpanKnots,
      &qsp.m_knots[startKnotIndex],
      &qsp.m_keys[startKnotIndex],
      &qsp.m_velsA[startKnotIndex],
      &qsp.m_velsB[startKnotIndex]);

  // Check if we need to break the curve at ka or kb and trim the curve
  if (extra > 0)
  {
    // Insert a knot at ka
    if (insertKnot(ka))
    {
      set(numSpanKnots,
          &m_knots[1],
          &m_keys[1],
          &m_velsA[1],
          &m_velsB[1]);
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
void QuatSpline::swap(QuatSpline& qsp)
{
  if (this != &qsp)
  {
    uint32_t maxNumKnots = m_maxNumKnots;
    uint32_t numKnots = m_numKnots;
    float* knots = m_knots;
    Quat* keys = m_keys;
    Vector3* velsA = m_velsA;
    Vector3* velsB = m_velsB;

    m_maxNumKnots = qsp.m_maxNumKnots;
    m_numKnots = qsp.m_numKnots;
    m_knots = qsp.m_knots;
    m_keys = qsp.m_keys;
    m_velsA = qsp.m_velsA;
    m_velsB = qsp.m_velsB;

    qsp.m_maxNumKnots = maxNumKnots;
    qsp.m_numKnots = numKnots;
    qsp.m_knots = knots;
    qsp.m_keys = keys;
    qsp.m_velsA = velsA;
    qsp.m_velsB = velsB;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Quat QuatSpline::evaluate(float t) const
{
  // Compute the Bezier curve span coefficients
  int32_t rgn;
  Quat    q0, qt;
  Vector3 w1, w2, w3;
  float   ka, kb;
  rgn = getSpan(t, q0, w1, w2, w3, ka, kb);
  NMP_ASSERT(rgn == 0);

  // Compute the u-parameter from the appropriate knot interval
  float u = (t - ka) / (kb - ka);

  // Compute the cumulative basis functions
  float omu = 1.0f - u;
  float u2 = u * u;
  float omu2 = omu * omu;
  float C3 = u2 * u;
  float C2 = 3.0f * u2 - 2.0f * C3;
  float C1 = 1.0f - omu2 * omu;

  // Compute the quaternion spline value
  // qt = qqmul(q0, qqmul(qexp(w1*C1), qqmul(qexp(w2*C2), qexp(w3*C3))))
  Quat e1, e2, e3;
  e1.exp(w1 * C1);
  e2.exp(w2 * C2);
  e3.exp(w3 * C3);
  qt = q0 * (e1 * (e2 * e3));

  return qt;
}

//----------------------------------------------------------------------------------------------------------------------
Quat QuatSpline::derivative1(float t) const
{
  // Compute the Bezier curve span coefficients
  int32_t rgn;
  Quat    q0, dqt;
  Vector3 w1, w2, w3;
  float   ka, kb;
  rgn = getSpan(t, q0, w1, w2, w3, ka, kb);
  NMP_ASSERT(rgn == 0);

  // Compute the u-parameter from the appropriate knot interval
  float tFac = 1.0f / (kb - ka);
  float u = tFac * (t - ka);

  // Compute the cumulative basis functions
  float omu = 1.0f - u;
  float u2 = u * u;
  float omu2 = omu * omu;
  float C3 = u2 * u;
  float dC3 = 3.0f * u2;
  float C2 = dC3 - 2.0f * C3;
  float C1 = 1.0f - omu2 * omu;
  float dC1 = 3.0f * omu2;
  float dC2 = 6.0f * u * omu;

  // Compute the derivative of the quaternion spline curve
  Quat a, b, c, z;
  z.exp(w1 * C1);
  b.exp(w2 * C2);
  c.exp(w3 * C3);
  a = q0 * z; // a = qqmul(q0, qexp(w1*C1));
  Quat d, e, f;
  d.setXYZW(dC1 * w1.x, dC1 * w1.y, dC1 * w1.z, 0.0f); // d = [0; w1*dC1];
  e.setXYZW(dC2 * w2.x, dC2 * w2.y, dC2 * w2.z, 0.0f); // e = [0; w2*dC2];
  f.setXYZW(dC3 * w3.x, dC3 * w3.y, dC3 * w3.z, 0.0f); // f = [0; w3*dC3];
  Quat a_, b_, c_;
  a_ = a * d; // a_ = qqmul(a, d);
  b_ = b * e; // b_ = qqmul(b, e);
  c_ = c * f; // c_ = qqmul(c, f);
  Quat ab, bc;
  ab = a * b; // ab = qqmul(a, b);
  bc = b * c; // bc = qqmul(b, c);
  Quat b_c;
  b_c = b_ * c; // b_c = qqmul(b_, c)

  // dqt = qqmul(a_, bc) + qqmul(a, b_c) + qqmul(ab, c_);
  dqt = (a_ * bc) + (a * b_c) + (ab * c_);

  // The derivative is expressed in radians per u-unit, so convert this to
  // radians per knot-unit
  dqt *= tFac;

  return dqt;
}

//----------------------------------------------------------------------------------------------------------------------
Quat QuatSpline::derivative2(float t) const
{
  // Compute the Bezier curve span coefficients
  int32_t rgn;
  Quat    q0, dqt;
  Vector3 w1, w2, w3;
  float   ka, kb;
  rgn = getSpan(t, q0, w1, w2, w3, ka, kb);
  NMP_ASSERT(rgn == 0);

  // Compute the u-parameter from the appropriate knot interval
  float tFac = 1.0f / (kb - ka);
  float u = tFac * (t - ka);

  // Compute the cumulative basis functions
  float omu = 1.0f - u;
  float u2 = u * u;
  float omu2 = omu * omu;
  float C3 = u2 * u;
  float dC3 = 3.0f * u2;
  float C2 = dC3 - 2.0f * C3;
  float C1 = 1.0f - omu2 * omu;
  float dC1 = 3.0f * omu2;
  float d2C3 = 6.0f * u;
  float dC2 = d2C3 * omu;
  float d2C1 = -6.0f * omu;
  float d2C2 = 6.0f - 12.0f * u;

  // Compute the second derivative of the quaternion spline curve
  Quat a, b, c, z;
  z.exp(w1 * C1);
  b.exp(w2 * C2);
  c.exp(w3 * C3);
  a = q0 * z; // a = qqmul(q0, qexp(w1*C1));
  Quat d, e, f;
  d.setXYZW(dC1 * w1.x, dC1 * w1.y, dC1 * w1.z, 0.0f); // d = [0; w1*dC1];
  e.setXYZW(dC2 * w2.x, dC2 * w2.y, dC2 * w2.z, 0.0f); // e = [0; w2*dC2];
  f.setXYZW(dC3 * w3.x, dC3 * w3.y, dC3 * w3.z, 0.0f); // f = [0; w3*dC3];
  Quat d_, e_, f_;
  d_.setXYZW(d2C1 * w1.x, d2C1 * w1.y, d2C1 * w1.z, 0.0f); // d_ = [0; w1*d2C1];
  e_.setXYZW(d2C2 * w2.x, d2C2 * w2.y, d2C2 * w2.z, 0.0f); // e_ = [0; w2*d2C2];
  f_.setXYZW(d2C3 * w3.x, d2C3 * w3.y, d2C3 * w3.z, 0.0f); // f_ = [0; w3*d2C3];
  Quat a_, b_, c_;
  a_ = a * d; // a_ = qqmul(a, d);
  b_ = b * e; // b_ = qqmul(b, e);
  c_ = c * f; // c_ = qqmul(c, f);
  Quat ab, ad, bc, cf, ec;
  ab = a * b; // ab = qqmul(a, b);
  ad = a * d; // ad = qqmul(a, d);
  bc = b * c; // bc = qqmul(b, c);
  cf = c * f; // cf = qqmul(c, f);
  ec = e * c; // ec = qqmul(e, c);
  Quat a_b, ab_, a_d, ad_;
  a_b = a_ * b; // a_b = qqmul(a_, b);
  ab_ = a * b_; // ab_ = qqmul(a, b_);
  a_d = a_ * d; // qqmul(a_, d)
  ad_ = a * d_; // qqmul(a, d_)
  Quat b_c, bc_;
  b_c = b_ * c; // qqmul(b_, c)
  bc_ = b * c_; // qqmul(b, c_)
  Quat c_f, cf_;
  c_f = c_ * f; // qqmul(c_, f)
  cf_ = c * f_; // qqmul(c, f_)
  Quat e_c, ec_;
  e_c = e_ * c; // qqmul(e_, c)
  ec_ = e * c_; // qqmul(e, c_)

  // r1 = qqmul(a_d, bc) + qqmul(ad_, bc) + qqmul(ad, b_c) + qqmul(ad, bc_);
  Quat r1 = (a_d * bc) + (ad_ * bc) + (ad * b_c) + (ad * bc_);

  // r2 = qqmul(a_b, ec) + qqmul(ab_, ec) + qqmul(ab, e_c) + qqmul(ab, ec_);
  Quat r2 = (a_b * ec) + (ab_ * ec) + (ab * e_c) + (ab * ec_);

  // r3 = qqmul(a_b, cf) + qqmul(ab_, cf) + qqmul(ab, c_f) + qqmul(ab, cf_);
  Quat r3 = (a_b * cf) + (ab_ * cf) + (ab * c_f) + (ab * cf_);

  Quat d2qt = r1 + r2 + r3;

  // The derivative is expressed in radians per (u-unit)^2, so convert this to
  // radians per (knot-unit)^2
  d2qt *= (tFac * tFac);

  return d2qt;
}

//----------------------------------------------------------------------------------------------------------------------
Quat QuatSpline::tangentialAcceleration(float t) const
{
  Quat q, d2q;
  float alpha;

  // Compute the quaternion spline value
  q = evaluate(t);

  // Compute the second derivative of the quaternion spline curve
  d2q = derivative2(t);

  // Compute the tangential component of angular acceleration
  alpha = d2q.dot(q);
  q *= alpha;
  d2q -= q;

  return d2q;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t QuatSpline::
getSpan(float t, Quat& q0, Vector3& w1, Vector3& w2, Vector3& w3, float& ka, float& kb) const
{
  // Find the interval that the value t lies on
  uint32_t intv;
  int32_t rgn = interval(t, intv);

  // Recover the curve span coefficients
  q0 = m_keys[intv];
  w1 = m_velsA[intv];
  w3 = m_velsB[intv];

  // Compute the q1 control quaternion: q1 = qqmul(q0, qexp(w1));
  Quat q1, dq;
  dq.exp(w1);
  q1.multiply(q0, dq);

  // Compute the q2 control quaternion: q2 = qqmul(q3, qinv(qexp(w3)));
  Quat q2;
  dq.exp(w3);
  dq.conjugate();
  q2.multiply(m_keys[intv+1], dq);

  // Evaluate the mid angular velocity vector w2 = qlog(qqmul(qinv(q1), q2));
  q1.conjugate();
  dq.multiply(q1, q2);
  w2 = dq.log();

  // Knot interval
  ka = m_knots[intv];
  kb = m_knots[intv+1];

  return rgn;
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const QuatSpline& qsp)
{
  Quat q;
  Vector3 v;
  float t;

  uint32_t numKnots = qsp.getNumKnots();
  uint32_t numSpans = numKnots - 1;

  os << "Num knots = " << numKnots << std::endl;
  for (uint32_t i = 0; i < numKnots; ++i)
  {
    t = qsp.getKnot(i);
    os << t << std::endl;
  }
  os << std::endl;

  os << "Keys" << std::endl;
  for (uint32_t i = 0; i < numKnots; ++i)
  {
    q = qsp.getKey(i);
    os << q.x << " " << q.y << " " << q.z << " " << q.w << std::endl;
  }
  os << std::endl;

  os << "VelsA" << std::endl;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    v = qsp.getVelA(i);
    os << v.x << " " << v.y << " " << v.z << std::endl;
  }
  os << std::endl;

  os << "VelsB" << std::endl;
  for (uint32_t i = 0; i < numSpans; ++i)
  {
    v = qsp.getVelB(i);
    os << v.x << " " << v.y << " " << v.z << std::endl;
  }
  os << std::endl;

  return os;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
