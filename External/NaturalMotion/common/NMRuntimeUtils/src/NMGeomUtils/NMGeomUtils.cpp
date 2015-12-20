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
#include "NMGeomUtils/NMGeomUtils.h"
//----------------------------------------------------------------------------------------------------------------------

#define OUTPUT_MESSAGESx
#ifdef OUTPUT_MESSAGES
  #include <iostream>
#endif

#define ISFINITE(n)  ((n)*0 == 0)

namespace NMRU
{

namespace GeomUtils
{

//----------------------------------------------------------------------------------------------------------------------
// Be aware that if the point input is inside the ellipse then there are three extrema in the
// cost function, and this algorithm may find the furthest point rather than the closest.  However,
// the algorithm is robust to points that are on the ellipse boundary or just inside.
//
// Taking our cost function for point (x, y) and current closest point estimate (cx, cy) to be
//   f = (x - cx)^2 + (y - cy)^2,
// with derivatives with respect to angle theta where cx = Acos(theta) and cy = Bsin(theta) of
//   df/dtheta   = 2((A/B)(x - cx)cy - (B/A)(y - cy)cx)
//   d2f/dtheta2 = 2((cx(B/A))^2 + (cy(A/B))^2 + (x - cx)cx + (y - cy)cy)
// Then, in order to avoid calling any trigonometric functions in the code, we convert to the
// parameter t where t = tan(theta/2) and
//   dtheta/dt        = 1 + cx/A
//   d2theta/dthetadt = -cy/B
// making relevant substitutions and using half-angle formulae where appropriate.
//
// With this in mind we can do Newton iteration on the parameter t to minimise f, using as our
// initial value the intersection with the ellipse of the line through the origin from (x, y).
void closestPointOnEllipse_newton(float& x, float& y, float A, float B)
{
  NMP_ASSERT(A > 0);
  NMP_ASSERT(B > 0);

  const uint32_t maxIterations = 100;  // Should always be enough

  // By forcing the point into the positive quarter of the space we can avoid our t variable
  // having to cross singularities.  xswitch and yswitch keep track of whether we flipped the
  // x and y axes.
  float xswitch = NMP::floatSelect(x, 1, -1); // if (x < 0) then xswitch = -1, else = 1
  x = NMP::floatSelect(x, x, -x); // if (x < 0) then x = -x, else remain the same
  //
  float yswitch = NMP::floatSelect(y, 1, -1); // if (y < 0) then yswitch = -1, else = 1
  y = NMP::floatSelect(y, y, -y); // if (y < 0) then y = -y, else remain the same

  // Get some values up front
  float AoverB = A / B;
  float BoverA = B / A;
  float AAoverBB = AoverB * AoverB;
  float BBoverAA = BoverA * BoverA;

  // Now clamp to independent limits, which is effectively clamping to a rectangle fitting
  // around the ellipse.  This improves our first guess and ensures convergence to the
  // minimum rather than a maximum.
  float xClamped = x;
  float yClamped = y;
  xClamped = NMP::minimum(xClamped, A);
  yClamped = NMP::minimum(yClamped, B);

  float xClampedNormMag = sqrtf(xClamped * xClamped / (A * A) + yClamped * yClamped / (B * B));

  // Get our initial guess for our closest point, as the intersection with the ellipse boundary
  // of a line through the origin.
  float cx = xClamped / xClampedNormMag;
  float cy = yClamped / xClampedNormMag;

  // Calculate t = tan(theta/2) using half angle formulae.
  float cosTheta = cx / A; // Always >= 0
  float t2 = (1 - cosTheta) / (1 + cosTheta);
  float t = sqrtf(NMP::floatSelect(t2, t2, 0));

  // Newton iteration loop
  uint32_t iter = 0;
  float deltat;
  do
  {
    ++iter;

    // Get some more values up front
    float xsubcx = x - cx;
    float ysubcy = y - cy;
    float dthetaBydt = 1 + cx / A;
    float d2thetaBydthetadt = -cy / B;

    // Calculate cost function derivative fdash and second derivative fddash
    float fdash = 2.0f * (AoverB * xsubcx * cy - BoverA * ysubcy * cx);
    float fddash = 2.0f * (BBoverAA * cx * cx + AAoverBB * cy * cy + xsubcx * cx + ysubcy * cy);

    // Modify fddash to account for the parameter change theta -> t.  We would normally
    // have to modify both derivatives, but one dtheta/dt term cancels out later.
    fddash = fddash * dthetaBydt + fdash * d2thetaBydthetadt;

    // Do the update
    deltat = fdash / fddash;
    NMP_ASSERT(ISFINITE(deltat)); // Shouldn't get any divide-by-zero issues in theory
    t -= deltat;
    t = NMP::clampValue(t, 0.0f, 1.0f);

    // Update our closest point
    float tSquared = t * t;
    float t2plus1 = 1.0f + tSquared;
    cosTheta = (1.0f - tSquared) / t2plus1;
    float sinTheta = 2.0f * t / t2plus1;
    cx = A * cosTheta;
    cy = B * sinTheta;
    NMP_ASSERT(ISFINITE(cx));
    NMP_ASSERT(ISFINITE(cy));
  }
  while (NMP::nmfabs(deltat) > 1e-7f && iter < maxIterations);

#ifdef OUTPUT_MESSAGES
  std::cerr << "Num Newton iterations = " << iter << std::endl;
#endif

  // Copy result out
  x = cx * xswitch;
  y = cy * yswitch;
}

//----------------------------------------------------------------------------------------------------------------------
// The algorithm takes as an initial guess the intersection with the ellipse of the line through
// the origin from (x, y), then finds a point on the other side of the minimum, which will be the
// nearest intersection of the ellipse with the x or y axis depending on whether the ellipse is
// wider in the x or y direction and whether (x, y) is inside or outside the ellipse.  Then it
// moves by means of repeated bisection to the closest point.
void closestPointOnEllipse_bisection(float& x, float& y, float A, float B)
{
  NMP_ASSERT(A > 0);
  NMP_ASSERT(B > 0);

  const uint32_t maxIterations = 20;  // Should always be enough for single precision maths

  // By forcing the point into the positive quarter of the space we can make it far simpler to
  // choose a second point the other side of the minimum.  xswitch and yswitch keep track of
  // whether we have flipped the x and y axes.
  float xswitch = NMP::floatSelect(x, 1, -1); // if (x < 0) then xswitch = -1, else = 1
  x = NMP::floatSelect(x, x, -x); // if (x < 0) then x = -x, else remain the same
  //
  float yswitch = NMP::floatSelect(y, 1, -1); // if (y < 0) then yswitch = -1, else = 1
  y = NMP::floatSelect(y, y, -y); // if (y < 0) then y = -y, else remain the same

  // Get some values up front
  float AA = A * A;
  float BB = B * B;
  float xNormMag = sqrtf(x * x / AA + y * y / BB);

  // Get our initial guess for our closest point, as the intersection with the ellipse boundary
  // of a line through the origin.
  float c1x = x / xNormMag;
  float c1y = y / xNormMag;

  // Get a second point the other side of the minimum (the true closest point).  This is always
  // closer to the 'pointy' end of the ellipse if (x, y) is outside the ellipse, and in the other
  // direction if it is inside.  Because we have restricted our point to have positive x and y,
  // all we have to do is choose the intersection of the ellipse with either the x or y axis,
  // depending on circumstances.
  float c2x = 0;
  float c2y = 0;
  if (xNormMag >= 1.0f) // Outside the ellipse
  {
    if (A > B) // ellipse is wide rather than tall
    {
      c2x = A; // choose (A, 0) as second point
    }
    else // ellipse is tall rather than wide
    {
      // In this case, swap c1 and c2 to get order right
      c2x = c1x;
      c2y = c1y;
      // choose (0, B) as second point
      c1x = 0;
      c1y = B;
    }
  }
  else // Inside the ellipse
  {
    if (A > B) // ellipse is wide rather than tall
    {
      // In this case, swap c1 and c2 to get order right
      c2x = c1x;
      c2y = c1y;
      // choose (0, B) as second point
      c1x = 0;
      c1y = B;
    }
    else // ellipse is tall rather than wide
    {
      c2x = A; // choose (A, 0) as second point
    }
  }

  // Bisection search loop
  // Stopping condition is when the two points are very close
  uint32_t iter = 0;
  while ((c1x - c2x)*(c1x - c2x) + (c1y - c2y)*(c1y - c2y) > GeomUtils::nTolSq && iter < maxIterations)
  {
    ++iter;

    // Find halfway point between (cx, cy) and (c2x, c2y), along ellipse
    float cnewx = c1x + c2x;
    float cnewy = c1y + c2y;
    float cnewNormMag = sqrtf(cnewx * cnewx / AA + cnewy * cnewy / BB);
    cnewx /= cnewNormMag;
    cnewy /= cnewNormMag;

    // Get the gradient of the value function (distance from (x, y)) at this point.
    float gradient = (A / B) * (x - cnewx) * cnewy - (B / A) * (y - cnewy) * cnewx;

    // If distance is increasing with angle around ellipse, replace c1, if decreasing, replace c2
    if (gradient > 0)
    {
      c1x = cnewx;
      c1y = cnewy;
    }
    else
    {
      c2x = cnewx;
      c2y = cnewy;
    }
  }

#ifdef OUTPUT_MESSAGES
  std::cerr << "Num bisection iterations = " << iter << std::endl;
#endif

  // Copy result out
  x = c1x * xswitch;
  y = c1y * yswitch;
}

//----------------------------------------------------------------------------------------------------------------------
// Be aware that if the point input is inside the ellipse then there are three extrema in the
// cost function, and this algorithm may find the furthest point rather than the closest.  However,
// the algorithm is robust to points that are on the ellipse boundary or just inside.
//
// Taking our cost function for point (x, y) and current closest point estimate (cx, cy) to be
//   f = (x - cx)^2 + (y - cy)^2,
// with derivatives with respect to angle theta where cx = Acos(theta) and cy = Bsin(theta) of
//   df/dtheta   = 2((A/B)(x - cx)cy - (B/A)(y - cy)cx
//   d2f/dtheta2 = 2((cx(B/A))^2 + (cy(A/B))^2 + (x - cx)cx + (y - cy)cy)
// Then, in order to avoid calling any trigonometric functions in the code, we convert to the
// parameter t where t = tan(theta/2) and
//   dtheta/dt        = 1 + cx/A
//   d2theta/dthetadt = -cy/B
// making relevant substitutions and using half-angle formulae where appropriate.
//
// With this in mind we can do Halley iteration on the parameter t to minimise f, using as our
// initial value the intersection with the ellipse of the line through the origin from (x, y).
void closestPointOnEllipse_halley(float& x, float& y, float A, float B)
{
  NMP_ASSERT(A > 0);
  NMP_ASSERT(B > 0);

  const uint32_t maxIterations = 100;  // Should always be enough

  // By forcing the point into the positive quarter of the space we can avoid our t variable
  // having to cross singularities.  xswitch and yswitch keep track of whether we flipped the
  // x and y axes.
  float xswitch = NMP::floatSelect(x, 1, -1); // if (x < 0) then xswitch = -1, else = 1
  x = NMP::floatSelect(x, x, -x); // if (x < 0) then x = -x, else remain the same
  //
  float yswitch = NMP::floatSelect(y, 1, -1); // if (y < 0) then yswitch = -1, else = 1
  y = NMP::floatSelect(y, y, -y); // if (y < 0) then y = -y, else remain the same

  // We also ensure that the ellipse is wide rather than tall, to simplify the maths
  bool axisSwitch = false;
  if (A < B)
  {
    float temp = x;
    x = y;
    y = temp;
    temp = A;
    A = B;
    B = temp;
    axisSwitch = true;
  }

  // Get some values up front
  float AA = A * A;
  float BB = B * B;
  float Ax = A * x;
  float By = B * y;
  float cx = x;
  float cy = y;

  // Special case: y is zero
  if (y == 0)
  {
    // Closest point is (A, 0) for x = 0 to (A - B^2/A)
    cx = A;
    cy = 0;

    if (x < A - BB / A)
    {
      cx = AA * x / (AA - BB);
      cy = B * sqrtf(1 - cx * cx / AA);
    }
  }

  // Special case: x is zero
  else if (x == 0)
  {
    cx = 0;
    cy = B;
  }

  // General case
  else
  {
    // Initial guess for parameter t = By - B^2 ensures convergence
    float dt = By - BB;
    float t = 0;

    uint32_t i = 0;
    for (; i < maxIterations; ++i)
    {
      i = i + 1;
      t = t + dt;

      float tplusAA = t + AA;
      float tplusBB = t + BB;
      float invtplusAA = 1.0f / tplusAA;
      float invtplusBB = 1.0f / tplusBB;
      float cxdivA = Ax * invtplusAA;
      float cydivB = By * invtplusBB;

      float f1 = cxdivA * cxdivA;
      float f2 = cydivB * cydivB;
      float f = f1 + f2 - 1.0f;
      float fdash = -2.0f * (f1 * invtplusAA + f2 * invtplusBB);

      dt = -f / fdash;
      if (dt < 1e-7f || NMP::nmfabs(f) < 1e-8f)
      {
        cx = cxdivA * A;
        cy = cydivB * B;
        break;
      }
    }

    // Failed to converge
    NMP_ASSERT(i < maxIterations);

#ifdef OUTPUT_MESSAGES
    std::cerr << "Num Halley iterations = " << i << std::endl;
#endif
  }

  // Copy result out
  x = (axisSwitch ? cy : cx) * xswitch;
  y = (axisSwitch ? cx : cy) * yswitch;
}

//----------------------------------------------------------------------------------------------------------------------
// NMRU::DebugDraw

const NMP::Vector3 DebugDraw::defaultColour = NMP::Vector3(1.0f, 1.0f, 1.0f, 1.0f);

//----------------------------------------------------------------------------------------------------------------------
void DebugDraw::drawLine(const NMP::Vector3& start, const NMP::Vector3& end, const NMP::Vector3& colour)
{
  if (m_numLines < NMRU_DEBUGDRAW_LINEBUFFER_SIZE && drawNextLine())
  {
    m_start[m_numLines].set(start);
    m_end[m_numLines].set(end);
    transformVectorPosQuat(worldFrameOffset, m_start[m_numLines]);
    transformVectorPosQuat(worldFrameOffset, m_end[m_numLines]);
    m_colour[m_numLines].set(colour);
    m_tags[m_numLines] = tag;

    ++m_numLines;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDraw::drawDashedLine(
  const NMP::Vector3& start,
  const NMP::Vector3& end,
  float dashLength,
  const NMP::Vector3& colour)
{
  if (m_numLines < NMRU_DEBUGDRAW_LINEBUFFER_SIZE)
  {
    NMP::Vector3 stepVec = end - start;
    float dist = stepVec.magnitude();
    stepVec.normalise();
    stepVec *= dashLength;
    NMP::Vector3 fromPoint = start;
    NMP::Vector3 toPoint = start;
    float t = 0;
    while (t < dist)
    {
      toPoint = fromPoint + stepVec;
      t = t + dashLength;
      if (t > dist)
      {
        toPoint = end;
      }
      drawLine(fromPoint, toPoint, colour);
      fromPoint = toPoint + stepVec;
      t = t + dashLength;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDraw::drawPosQuatFrame(const NMRU::GeomUtils::PosQuat& pq, float size)
{
  drawLine(pq.t, pq.t + pq.q.getXAxis() * size, NMP::Vector3(1.0f, 0.0f, 0.0f));
  drawLine(pq.t, pq.t + pq.q.getYAxis() * size, NMP::Vector3(0.0f, 1.0f, 0.0f));
  drawLine(pq.t, pq.t + pq.q.getZAxis() * size, NMP::Vector3(0.0f, 0.0f, 1.0f));
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDraw::drawLocator(const NMP::Vector3& pos, float size, const NMP::Vector3& colour)
{
  if (m_numLines < NMRU_DEBUGDRAW_LINEBUFFER_SIZE)
  {
    NMP::Vector3 start(pos.x - size, pos.y, pos.z);
    NMP::Vector3 end(pos.x + size, pos.y, pos.z);
    drawLine(start, end, colour);
    start.set(pos.x, pos.y - size, pos.z);
    end.set(pos.x, pos.y + size, pos.z);
    drawLine(start, end, colour);
    start.set(pos.x, pos.y, pos.z - size);
    end.set(pos.x, pos.y, pos.z + size);
    drawLine(start, end, colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDraw::clearBuffer()
{
  m_numLines = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void DebugDraw::getDebugLine(uint32_t lineIndex, NMP::Vector3& start, NMP::Vector3& end) const
{
  if (lineIndex < NMRU_DEBUGDRAW_LINEBUFFER_SIZE)
  {
    start.set(m_start[lineIndex]);
    end.set(m_end[lineIndex]);
  }
}

void DebugDraw::getColouredDebugLine(
  uint32_t lineIndex,
  NMP::Vector3& start,
  NMP::Vector3& end,
  NMP::Vector3& colour) const
{
  if (lineIndex < NMRU_DEBUGDRAW_LINEBUFFER_SIZE)
  {
    start.set(m_start[lineIndex]);
    end.set(m_end[lineIndex]);
    colour.set(m_colour[lineIndex]);
  }
}

} // end of namespace GeomUtils

} // end of namespace NMRU
