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

#ifndef NMRU_GEOMUTILS_H
#define NMRU_GEOMUTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMMathUtils.h"
//----------------------------------------------------------------------------------------------------------------------

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  // Remove the 'x' to enable certain bits of test code, not to be used in production.
  //
  #define NM_UNIT_TESTINGx
  #ifdef NM_UNIT_TESTING
    // Note: Including this header requires exceptions to be enabled to prevent
    // a compile error in xlocale. Exceptions are off by default in this library.
    #include <ostream>
  #endif
#endif

namespace NMRU
{

namespace GeomUtils
{

//----------------------------------------------------------------------------------------------------------------------
/// \name Global tolerance values
/// @{

// NOTE gTol and gTolSq are still essentially based on /numerical/ accuracy, not local distance, so
// as not to suffer from dependence on choice of scale.  However, sometimes manipulation of geometric
// entities results in more numerical drift than in normal algebra, and gTol allows for a greater
// margin of error in those cases.  gTol also tells you something about what kind of size of entity
// you will be able to manipulate geometrically, e.g. how much magnitude does a vector need before it
// can be validly rotated, or normalised?
/// \brief A geometrical distance tolerance, for checking stability of rotation axes and the like.
const float gTol = 1e-4f;
/// \brief A geometrical distance tolerance, for checking stability of rotation axes and the like.
const float gTolSq = gTol * gTol;
/// \brief A general numerical tolerance - geometrical entities greater than this in magnitude
/// should be able to be manipulated stably.
const float nTol = 1e-6f;
/// \brief A general numerical tolerance - geometrical entities greater than this in magnitude
/// should be able to be manipulated stably.
const float nTolSq = nTol * nTol;
/// \brief Safe floating-point arccosine macro.
#define NMRU_ACOSF_SAFE(x) ((x) >= 1. ? 0 : (x) <= -1. ? NM_PI : acosf(x))
/// \brief Safe floating-point fast arccosine macro.
#define NMRU_FASTACOSF_SAFE(x) ((x) >= 1. ? 0 : (x) <= -1. ? NM_PI : NMP::fastArccos(x))
/// \brief Safe floating-point arcsine macro
#define NMRU_ASINF_SAFE(x) ((x) >= 1. ? NM_PI_OVER_TWO : (x) <= -1. ? -NM_PI_OVER_TWO : asinf(x))

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \brief A transform made from a quaternion and a translation vector
struct PosQuat
{
  NMP::Vector3 t;
  NMP::Quat q;
};

//----------------------------------------------------------------------------------------------------------------------
/// \name Functions for PosQuats
/// Note that the terminology is based on treating a PosQuat as if it were a 4x4 homogeneous transform
/// matrix which acts on the right - the translation part would be the bottom row.  For instance,
/// multiplyPosQuat() means multiply the first PosQuat on the right by the second, the result being
/// the first moved into the frame of the second (the rows of the first transformed by the second transform);
/// or, if you prefer, the second rotated by the first.
/// Actual quaternion algebra will convert this to left-multiplication because this is the way round
/// our implementation of unit quaternions operates.
/// @{
PosQuat identityPosQuat();
void identityPosQuat(PosQuat& pq);
void fromMatrix34PosQuat(PosQuat& pq, const NMP::Matrix34& m);
void fromPosQuatMatrix34(NMP::Matrix34& m, const PosQuat& pq);
void multiplyPosQuat(PosQuat& result, const PosQuat& pq1, const PosQuat& pq2);
void multiplyPosQuat(PosQuat& result, const PosQuat& pq);
void premultiplyPosQuat(const PosQuat& pq, PosQuat& result);
void premultiplyInversePosQuat(const PosQuat& pq, PosQuat& result);
void multiplyInversePosQuat(PosQuat& result, const PosQuat& pq1, const PosQuat& pq2);
void multiplyInversePosQuat(PosQuat& result, const PosQuat& pq);
void rotatePosQuat(const NMP::Quat& q, PosQuat& result);
void inverseRotatePosQuat(const NMP::Quat& q, PosQuat& result);
void transformVectorPosQuat(const PosQuat& pq, NMP::Vector3& result);
void inverseTransformVectorPosQuat(const PosQuat& pq, NMP::Vector3& result);
void invertPosQuat(PosQuat& result, const PosQuat& pq);
void invertPosQuat(PosQuat& result);
void interpolatePosQuat(PosQuat& result, const PosQuat& pqFrom, const PosQuat& pqTo, float factor);
void interpolatePosQuat(PosQuat& result, const PosQuat& pq, float factor);
void interpolatePosQuat(
  PosQuat& result, const PosQuat& pqFrom, const PosQuat& pqTo, float orientationFactor, float positionFactor);
void interpolatePosQuat(PosQuat& result, const PosQuat& pq, float orientationFactor, float positionFactor);

/// \brief Inverts rotation and translation along the axes identified.
void mirrorPosQuat(PosQuat& result, bool rx, bool ry, bool rz, bool tx, bool ty, bool tz);
/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name Functions for Quats
/// @{

/// Computes the rotation about a specified axis which is close as possible (in terms of remaining
/// rotation) to given quat.
NMP::Quat closestRotationAroundAxis(const NMP::Quat& q, const NMP::Vector3& axis);

/// Computes the rotation about a specified axis which is close as possible (in terms of remaining
/// rotation) to given quat.
/// \param condition 0-1 condition number returned, 0 represents the best axis of rotation and
/// the \a axis being orthogonal - so the result is unstable for low condition numbers.
NMP::Quat closestRotationAroundAxis(const NMP::Quat& q, const NMP::Vector3& axis, float* condition);

/// Computes the rotation about a specified axis which is close as possible (in terms of remaining
/// rotation) to given quat.
/// \param softness Reduce amount of rotation down to zero for fully ill-conditioned input, ramping
/// up to the full rotation as calculated when the condition number equals the input softness value.
NMP::Quat closestRotationAroundAxisSoft(const NMP::Quat& q, const NMP::Vector3& axis, float softness);

/// Computes the rotation which brings fromV to toV, except where they are close to antiparallel,
/// in which case the amount of rotation drops off to zero according to the softness parameter.
/// \param softness Ramp the amount of rotation down to zero from full in the range
/// condition = softness to 0, where the condition is fabs(-1 - the dot product between the vectors).
NMP::Quat forRotationSoft(const NMP::Vector3& fromV, const NMP::Vector3& toV, float softness);

/// Computes the rotation about a specified axis which brings fromV as close as possible to toV.
/// \a fromV, \a toV and the axis must be unit vectors.
NMP::Quat forRotationAroundAxis(const NMP::Vector3& fromV, const NMP::Vector3& toV, const NMP::Vector3& axis);

/// Computes the rotation about a specified axis which brings fromV as close as possible to toV.
/// fromV, toV and the axis must be unit vectors.
/// \param condition 0-1 condition number returned, 0 represents the best axis of rotation and
/// the \a axis being orthogonal - so the result is unstable for low condition numbers.
NMP::Quat forRotationAroundAxis(
  const NMP::Vector3& fromV, const NMP::Vector3& toV, const NMP::Vector3& axis, float* condition);

/// Computes the rotation about a specified axis which brings fromV as close as possible to toV.
/// fromV, toV and the axis must be unit vectors.
/// \param softness Reduce amount of rotation down to zero for fully ill-conditioned input, ramping
/// up to the full rotation as calculated when the condition number equals the input softness value.
NMP::Quat forRotationAroundAxisSoft(const NMP::Vector3& fromV, const NMP::Vector3& toV,
                                    const NMP::Vector3& axis, float axisSoftness, float antiparallelSoftness = 0);

/// Negates a quat based on the sign of a selector using float-select.  Separated out because usage
/// may change depending on how Quat may be vectorised.
void quatSelect(NMP::Quat& q, const float& selector);

/// Makes use of NMP::Quat::fastSlerp(), but calculates the dot product; if the dot product is
/// negative, it negates the output quat to ensure a shortest-arc interpolation.
void shortestArcFastSlerp(NMP::Quat& qFrom, const NMP::Quat& qTo, float t);

/// A kind of quat interpolation that allows the weight to be greater 1 or less than 0, which
/// results in extrapolation.
void interpOrExtrapSlerp(NMP::Quat& qFrom, const NMP::Quat& qTo, float t);

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name Functions for Joints
/// @{

/// Convert a quaternion into a twist-lean orientation vector.
void quatToTwistLean(const NMP::Quat& q, NMP::Vector3& tl);

/// Convert a twist-lean orientation vector into a quaternion.
void twistleanToQuat(const NMP::Vector3& tl, NMP::Quat& q);

/// Convert a quaternion into a twist-swing orientation vector.
void quatToTwistSwing(const NMP::Quat& q, NMP::Vector3& ts);

/// Convert a twist-swing orientation vector into a quaternion.
void twistSwingToQuat(const NMP::Vector3& ts, NMP::Quat& q);

/// Extract the twist (x-axis) rotation from a general rotation.
void separateTwistFromTwistAndSwing(const NMP::Quat& q, NMP::Quat& qtwist);

/// Extract the swing (y-z plane rotation) from a general rotation.
void separateSwingFromTwistAndSwing(const NMP::Quat& q, NMP::Quat& qswing);

/// Convert a twist cone plane orientation into a quaternion.
void quatToTwistConePlane(const NMP::Quat& q, NMP::Vector3& tcp);

/// Convert a quaternion into a twist cone plane orientation.
void twistConePlaneToQuat(const NMP::Vector3& tcp, NMP::Quat& q);

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name Useful utilities
/// @{

/// \brief Solve a quadratic with real solutions, asserts when there's no solution.
void solveQuadratic(float a, float b, float c, float& x1, float& x2);

/// \brief Calculates the projection of \a point onto the line interpolating \a p0 and \a p1.
/// ( result = p0 + t * (p1 - p0) ). If \a t is non null then it's value is also returned.
/// If \a p0 and \a p1 are colocated the function returns the value of \a p0 and \a t == 0.
NMP::Vector3 getPointProjectedOntoLine(
  const NMP::Vector3& p0, const NMP::Vector3& p1,
  const NMP::Vector3& point,
  float *t);

/// \brief Find the closest point on a 2D ellipse boundary ((x/A)^2 + (y/B)^2 = 1) to a given point.
/// This method will often return the wrong result for points inside the ellipse - in this case use
/// bisection search, which is slower but will always find the closest point.  At best it is robust
/// when the point in question drifts inside the ellipse to about 98% of the distance from the centre.
/// \param x The x component of the input point, replaced with the output x value.
/// \param y The y component of the input point, replaced with the output y value.
/// \param A The x-axis radius.  Must be greater than 0.
/// \param B The y-axis radius.  Must be greater than 0.
void closestPointOnEllipse_newton(float& x, float& y, float A, float B);

/// \brief Find the closest point on a 2D ellipse boundary ((x/A)^2 + (y/B)^2 = 1) to a given point.
/// This method will always return the right result but will usually take 20 to 40 iterations to
/// reach it, even if the initial guess was good.
/// \param x The x component of the input point, replaced with the output x value.
/// \param y The y component of the input point, replaced with the output y value.
/// \param A The x-axis radius.  Must be greater than 0.
/// \param B The y-axis radius.  Must be greater than 0.
void closestPointOnEllipse_bisection(float& x, float& y, float A, float B);

/// \brief Find the closest point on a 2D ellipse boundary ((x/A)^2 + (y/B)^2 = 1) to a given point.
/// Uses Halley's method which can under certain circumstances be faster than Newton.
/// \param x The x component of the input point, replaced with the output x value.
/// \param y The y component of the input point, replaced with the output y value.
/// \param A The x-axis radius.  Must be greater than 0.
/// \param B The y-axis radius.  Must be greater than 0.
void closestPointOnEllipse_halley(float& x, float& y, float A, float B);

/// \brief Returns true if a point (x, y) is outside of an ellipse (a * cos(t), b * sin(t)), or (x/a)^2 + (y/b)^2 = 1
bool isPointOutsideEllipse(float a, float b, float x, float y);

/// \brief Divide \a a by \a b except when b is zero, in which case return zero.
float safelyDivide(float a, float b, float defaultValue = 0);

/// \brief A very simple approximate lookup table implementation of log10(x), for x between 0 and 1.
float fastLog10(float x);

/// \brief Change the value of \a origValue to be \a newValue, if the difference is greater than \a tolerance.
/// \return true if the value was changed.
bool changeValueIfDifferent(float& origValue, const float newValue, float tolerance);

/// \brief Change the value of \a origValue to be \a newValue, if it is different.
/// \return true if the value was changed.
template<typename T> bool changeValueIfDifferent(T& origValue, const T newValue);

/// \brief Clamp a value if it is moving even further outside the limits than a previous value.
void preventValueDivergingFurtherFromLimits(float& value, float minValue, float maxValue, float origValue);

/// \brief Find a vector orthogonal to fixed and perpendicular to the plane described by fixed and the guide vector.
NMP::Vector3 calculateOrthogonalPerpendicularVector(const NMP::Vector3& fixed, const NMP::Vector3& guide);

/// \brief Find a vector orthogonal to fixed and parallel to the plane described by fixed and the guide vector.
NMP::Vector3 calculateOrthogonalPlanarVector(const NMP::Vector3& fixed, const NMP::Vector3& guide);

/// \brief Find a set of three orthogonal vectors when given one of those vectors and a second guiding vector.
void calculateOrthogonalVectors(NMP::Vector3& perpendicular, NMP::Vector3& planar, const NMP::Vector3& fixed, const NMP::Vector3& guide);

/// \brief Find a set of three orthonormal vectors when given one of those vectors and a second guiding vector.
void calculateOrthonormalVectors(NMP::Vector3& perpendicular, NMP::Vector3& planar, const NMP::Vector3& fixed, const NMP::Vector3& guide);
/// @}

/// Represents an axis aligned bounding box, specified using minimum and maximum points along each
/// axis. It will report as being empty if the minimum is greater than the maximum in any direction.
/// Note that there is a difference between a point being inside the AABB and it being included.
class AABB
{
public:
  /// Creates the AABB uninitialised
  AABB() {}
  /// Initialises the AABB to just include the point
  AABB(const NMP::Vector3& p) : m_min(p), m_max(p) {}
  /// Initialises the AABB with the range passed in
  AABB(const NMP::Vector3& min, const NMP::Vector3& max) : m_min(min), m_max(max) {}

  /// Marks the AABB as empty so the maximum is less than the minimum
  void setEmpty();
  /// Marks the AABB as including all valid points
  void setInfinite();

  /// Sets the AABB so that its centre and half extents are as requested
  void setCentreAndHalfExtents(const NMP::Vector3& centre, const NMP::Vector3& halfExtents);

  /// Moves the AABB by delta
  void move(const NMP::Vector3& delta);

  /// Returns true if the maximum point is less than the minimum in any direction
  bool isEmpty() const;

  /// Expands the AABB to include the point
  void addPoint(const NMP::Vector3& p);

  // Returns true if the point is inside the box by at least epsilon in all directions. A point that
  // has been added will be included, but not inside (with tolerance = 0).
  bool isPointInside(const NMP::Vector3& p, float tolerance) const;

  // Returns true if the point is inside or on the boundary (within floating point accuracy). A
  // point that has been added will be included, but not inside (with epsilon = 0).
  bool isPointIncluded(const NMP::Vector3& p) const;

  /// Returns the minimum point
  const NMP::Vector3& getMin() const {return m_min;}

  /// Returns the maximum point
  const NMP::Vector3& getMax() const {return m_max;}

  /// Returns the centre of the AABB
  NMP::Vector3 getCentre() const;

  /// Returns the full extents (maximum - minimum)
  NMP::Vector3 getExtents() const;

  /// Returns the half extents
  NMP::Vector3 getHalfExtents() const {return getExtents() * 0.5f;}

  float getVolume() const;

private:
  NMP::Vector3 m_min;
  NMP::Vector3 m_max;
};


//----------------------------------------------------------------------------------------------------------------------
/// Useful for 3D geometry debugging, stores line segments
class DebugDraw
{
#define NMRU_DEBUGDRAW_LINEBUFFER_SIZE 640

public:

  static const NMP::Vector3 defaultColour;

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief Construct.
  DebugDraw();

  //----------------------------------------------------------------------------------------------------------------------

  /// \name Drawing lines and objects
  /// @{
  void drawLine(const NMP::Vector3& start, const NMP::Vector3& end, const NMP::Vector3& colour = defaultColour);
  void drawDashedLine(
    const NMP::Vector3& start, const NMP::Vector3& end, float dashLength, const NMP::Vector3& colour = defaultColour);
  void drawPosQuatFrame(const NMRU::GeomUtils::PosQuat& pq, float size);
  void drawLocator(const NMP::Vector3& pos, float size, const NMP::Vector3& colour = defaultColour);
  /// @}

  /// \name Manage tags, used for identifying groups of lines, to be rendered selectively.
  /// @{
  uint32_t getDebugTag(uint32_t lineIndex) const;
  bool drawNextLine() const;
  /// @}

  /// \name Retrieve entries in the line buffer
  /// @{
  void getDebugLine(uint32_t lineIndex, NMP::Vector3& start, NMP::Vector3& end) const;
  void getColouredDebugLine(uint32_t lineIndex, NMP::Vector3& start, NMP::Vector3& end, NMP::Vector3& colour) const;
  /// @}

  /// \name Utilities
  /// @{
  void clearBuffer();
  uint32_t getNumLines() const;
  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  // Public data

  /// \brief Move all line data into a different coordinate frame before storing
  GeomUtils::PosQuat worldFrameOffset;

  /// \brief Mark all debug lines with this number
  int32_t tag;

  /// \brief Only draw lines that have this tag number or -1
  int32_t restrictToTag;

protected:
  //----------------------------------------------------------------------------------------------------------------------
  // Private data

  /// \brief Size of the line buffers.
  uint32_t m_numLines;

  /// \brief Identifiers stored with each line that make it easy to select particular groups for rendering.
  int32_t m_tags[NMRU_DEBUGDRAW_LINEBUFFER_SIZE];

  /// \brief Enforces valid alignment without requiring the mixing of public and private data.
  uint32_t m_pad[1];

  /// \name The line buffers.
  /// @{
  NMP::Vector3 m_start[NMRU_DEBUGDRAW_LINEBUFFER_SIZE];
  NMP::Vector3 m_end[NMRU_DEBUGDRAW_LINEBUFFER_SIZE];
  NMP::Vector3 m_colour[NMRU_DEBUGDRAW_LINEBUFFER_SIZE];
  /// @}
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDraw::DebugDraw() :
  tag(-1),
  restrictToTag(-1),
  m_numLines(0)
{
  GeomUtils::identityPosQuat(worldFrameOffset);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool DebugDraw::drawNextLine() const
{
  return tag == restrictToTag || restrictToTag == -1 || tag == -1;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDraw::getDebugTag(uint32_t lineIndex) const
{
  return (lineIndex < NMRU_DEBUGDRAW_LINEBUFFER_SIZE) ? m_tags[lineIndex] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDraw::getNumLines() const
{
  return m_numLines;
}

}  // end of namespace GeomUtils

}  // end of namespace NMRU

#include "NMGeomUtils/NMGeomUtils.inl"

#endif // NMRU_GEOMUTILS_H
