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
#include "NMGeomUtils/NMGeomUtilsMP.h"
//----------------------------------------------------------------------------------------------------------------------

#define OUTPUT_MESSAGESx
#ifdef OUTPUT_MESSAGES
  #include <iostream>
#endif

namespace NMRU
{

namespace GeomUtilsMP
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
void closestPointOnEllipse_newton(
  vector4& x,
  vector4& y,
  const vector4& Ain,
  const vector4& Bin)
{
  vector4 A = Ain;
  vector4 B = Bin;

  // Initialisations
  static const uint32_t maxIterations = 100;  // Should always be enough
  static const vector4 zero4f = NMP::vpu::zero4f();
  static const vector4 one4f = NMP::vpu::one4f();
  static const vector4 two4f = NMP::vpu::set4f(2.0f);
  static const vector4 exitTolerance = NMP::vpu::set4f(1e-7f);

  // By forcing the point into the positive quarter of the space we can avoid our t variable
  // having to cross singularities.  xswitch and yswitch keep track of whether we flipped the
  // x and y axes.

  vector4 xswitch = NMP::vpu::mask4cmpLT(x, zero4f);
  x = vAbs(x);
  vector4 yswitch = NMP::vpu::mask4cmpLT(y, zero4f);
  y = vAbs(y);

  // Get some values up front
  vector4 Ainv = vRecip(A);
  vector4 Binv = vRecip(B);
  vector4 AoverB = vMul(A, Binv);
  vector4 BoverA = vMul(B, Ainv);
  vector4 AAoverBB = vMul(AoverB, AoverB);
  vector4 BBoverAA = vMul(BoverA, BoverA);

  // Now clamp to independent limits, which is effectively clamping to a rectangle fitting
  // around the ellipse.  This improves our first guess and ensures convergence to the
  // minimum rather than a maximum.
  vector4 xClamped = NMP::vpu::sel4cmpLT(x, A, x, A);
  vector4 yClamped = NMP::vpu::sel4cmpLT(y, B, y, B);

  // Normalised magnitude = sqrt(x^2/A^2 + y^2/B^2)
  vector4 xClampedNormMag = NMP::vpu::sqrt4f(
    vAdd(vDiv(vMul(xClamped, xClamped), vMul(A, A)), vDiv(vMul(yClamped, yClamped), vMul(B, B))));

  // Get our initial guess for our closest point, as the intersection with the ellipse boundary
  // of a line through the origin.
  vector4 xClampedNormMagInv = vRecip(xClampedNormMag);
  vector4 cx = vMul(xClamped, xClampedNormMagInv);
  vector4 cy = vMul(yClamped, xClampedNormMagInv);

  // Calculate t = tan(theta/2) using half angle formulae.
  vector4 cosTheta = vMul(cx, Ainv); // Always >= 0
  vector4 t2 = vDiv(vSub(one4f, cosTheta), vAdd(one4f, cosTheta));
  vector4 t = NMP::vpu::sqrt4f(NMP::vpu::sel4cmpLT(t2, zero4f, zero4f, t2));

  // Newton iteration loop
  uint32_t iter = 0;
  vector4 deltat;
  do
  {
    ++iter;

    // Get some more values up front
    vector4 xsubcx = vSub(x, cx);
    vector4 ysubcy = vSub(y, cy);
    vector4 dthetaBydt = vAdd(one4f, vMul(cx, Ainv));
    vector4 d2thetaBydthetadt = vMul(vNeg(cy), Binv);

    // Calculate cost function derivative fdash and second derivative fddash
    vector4 fdash = vMul(two4f, vSub(vMul(AoverB, vMul(xsubcx, cy)), vMul(BoverA, vMul(ysubcy, cx))));
    vector4 fddash = vMul(two4f,
      (vAdd(vMul(BBoverAA, vMul(cx, cx)), vAdd(vMul(AAoverBB, vMul(cy, cy)),
       vAdd(vMul(xsubcx, cx), vMul(ysubcy, cy))))));

    // Modify fddash to account for the parameter change theta -> t.  We would normally
    // have to modify both derivatives, but one dtheta/dt term cancels out later.
    fddash = vAdd(vMul(fddash, dthetaBydt), vMul(fdash, d2thetaBydthetadt));

    // Do the update
    deltat = vDiv(fdash, fddash);
    t = vSub(t, deltat);
    //  Clamp between 0 and 1 for safety
    t = NMP::vpu::sel4cmpLT(t, zero4f, zero4f, t);
    t = NMP::vpu::sel4cmpLT(t, one4f, t, one4f);

    // Update our closest point
    vector4 tSquared = vMul(t, t);
    vector4 t2plus1Inv = vRecip(vAdd(one4f, tSquared));
    cosTheta = vMul(vSub(one4f, tSquared), t2plus1Inv);
    vector4 sinTheta = vMul(vMul(two4f, t), t2plus1Inv);
    cx = vMul(A, cosTheta);
    cy = vMul(B, sinTheta);

    vector4 error = vAbs(deltat);
    vector4 exitMask = NMP::vpu::isBound4f(error, exitTolerance);
    // It's cheaper to extract this mask out to a scalar than it is to execute unnecessary additional loops
    if (NMP::vpu::elemX(exitMask) != 0)
    {
      break;
    }
  }
  while (iter < maxIterations);

#ifdef OUTPUT_MESSAGES
  std::cerr << "Num Newton iterations = " << iter << std::endl;
#endif

  // Copy result out
  x = NMP::vpu::sel4cmpMask(xswitch, vNeg(cx), cx);
  y = NMP::vpu::sel4cmpMask(yswitch, vNeg(cy), cy);
}

} // end of namespace GeomUtilsMP

} // end of namespace NMRU
