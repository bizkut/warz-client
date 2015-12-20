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
#ifndef MR_DEBUG_DRAW_TESSELLATOR_H
#define MR_DEBUG_DRAW_TESSELLATOR_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMColour.h"
#include "NMPlatform/NMVector3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
enum VectorType
{
  VT_Force,               // kg m / s^2
  VT_Torque,              // kg m^2 rad / s^2
  VT_Acceleration,        // m / s^2
  VT_AngularAcceleration, // rad / s^2
  VT_Impulse,             // kg m / s        (also use for momentum)
  VT_TorqueImpulse,       // kg m^2 rad / s  (also use for angular momentum)
  VT_Velocity,            // m / s
  VT_AngularVelocity,     // rad / s
  VT_Push,                // kg m
  VT_AnglePush,           // kg m^2 rad
  VT_Delta,               // m                (a change in a position)
  VT_AngleDelta,          // rad
  VT_Normal,              // dimensionless, should be unit length really
  VT_size,
};

//----------------------------------------------------------------------------------------------------------------------
// Low level basic drawing commands.
// DDTessellator calls a registered instance of this class.
// Implementations:
//  + Morpheme Runtime Target with visualisation - immediately renders to a local device via NMRuntimeFramework.
//  + Client game - immediately renders to a local device via a game renderer.
//  + Morpheme Connect - can cache draw commands to buffers for later use.
//----------------------------------------------------------------------------------------------------------------------
class DDRenderer
{
public:
  virtual ~DDRenderer();

  virtual void drawLine(
    const NMP::Vector3& start,
    const NMP::Vector3& end,
    NMP::Colour         colour) = 0; // r, g, b, a
  virtual void drawTriangle(
    const NMP::Vector3& v1,
    const NMP::Vector3& v2,
    const NMP::Vector3& v3,
    NMP::Colour         colour) = 0;  // r, g, b, a
};

//----------------------------------------------------------------------------------------------------------------------
// Implements the tessellation of many compound draw shapes, from simple lines to complicated torque arcs.
// Tessellation splits complex shapes in to many simple draw commands such as drawLine or drawTriangle.
// The simple draw commands are handled by an instance of DDRenderer that is registered with the tessellator.
//----------------------------------------------------------------------------------------------------------------------
class DDTessellator
{
public:
  DDTessellator();
  ~DDTessellator() {}

  NM_INLINE void setDDRenderer(DDRenderer* ddRenderer);

  NM_INLINE void tessPoint(const NMP::Vector3& point, float radius, NMP::Colour colour);
  NM_INLINE void tessSphere(const NMP::Matrix34& point, float radius, NMP::Colour colour);
  NM_INLINE void tessLine(const NMP::Vector3& start, const NMP::Vector3& end, NMP::Colour colour); // Draw a basic line from start to end.
  NM_INLINE void tessPolyLine(
    uint32_t            numVetices, // Min of 2 and numLines = numVetices - 1;
    const NMP::Vector3* vertices,   // Array of vertices between lines.
    NMP::Colour         colour);
  NM_INLINE void tessVector(
    VectorType          type,
    const NMP::Vector3& start,
    const NMP::Vector3& offset,
    NMP::Colour         colour); // Draw a line with various line endings (arrows etc, depending on type) from start to start + offset.
  NM_INLINE void tessArrowHead(
    const NMP::Vector3& startPos,   // Position of arrow point (if not a delta).
    const NMP::Vector3& direction,  // Arrow direction.
    const NMP::Vector3& tangent,    // Generally at 90 degrees to the direction. Defines the width of the arrow.
    NMP::Colour         colour,
    bool                hasMass,    // If true draws solid arrows, else draw line arrows.
    bool                isDelta);   // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
  NM_INLINE void tessTwistArc(
    const NMP::Vector3& pos,             // Center of arc.
    const NMP::Vector3& primaryDir,      // Normal of the plane which the arc lies on (Not necessarily normalised).
    const NMP::Vector3& dir,             //
    const NMP::Vector3& dir2,            //
    NMP::Colour         colour,
    bool                doubleArrowHead, // Draw an arrow at both ends of arc.
    bool                hasMass,         // Arrow heads are drawn as solid triangles.
    bool                isDelta);        // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
  NM_INLINE void tessPlane(
    const NMP::Vector3& point,
    const NMP::Vector3& normal,
    float               radius,
    NMP::Colour         colour,
    float               normalScale);
  NM_INLINE void tessTriangle(
    const NMP::Vector3& v1,     // 1-2-3 should form a clockwise face.
    const NMP::Vector3& v2,
    const NMP::Vector3& v3,
    NMP::Colour         colour);
  NM_INLINE void tessMatrix(const NMP::Matrix34& matrix, float scale, uint8_t alpha);
  NM_INLINE void tessNonUniformMatrix(const NMP::Matrix34& matrix, float scale, uint8_t alpha);
  NM_INLINE void tessConeAndDial(
    const NMP::Vector3& pos,
    const NMP::Vector3& dir,
    float               angle,
    float               size,
    const NMP::Vector3& dialDirection,
    NMP::Colour         colour); // Red by default
  NM_INLINE void tessContactPointSimple(
    const NMP::Vector3& point,                  // Contact point.
    float               forceMagnitudeSquared); // Force magnitude squared at contact point. 
  NM_INLINE void tessContactPointDetailed(
    const NMP::Vector3& point,      // Contact point.
    const NMP::Vector3& normal,     // Contact normal.
    const NMP::Vector3& force,      // Force at contact point. 
    const NMP::Vector3& actor0Pos,  // Actor 0 root position.
    const NMP::Vector3& actor1Pos); // Actor 1 root position.
  NM_INLINE void tessCharacterRoot(const NMP::Matrix34& characterControllerRoot);
  NM_INLINE void tessBox(const NMP::Vector3 &focalCentre, const NMP::Vector3 &focalRadii, NMP::Colour colour);
  NM_INLINE void tessBBox(const NMP::Matrix34 &tm, const NMP::Vector3 &halfSizes, NMP::Colour colour);
  NM_INLINE void tessSphereSweep(
    const NMP::Vector3& position,
    const NMP::Vector3& motion,
    float               radius,
    NMP::Colour         colour);
  NM_INLINE void tessEnvironmentPatch(
    const NMP::Vector3& position,
    const NMP::Vector3& normal,
    float               size,
    NMP::Colour         colour);

protected:
  NMP::Vector3 m_up;  // Up direction.

  DDRenderer*  m_ddRenderer;
 
  int32_t      m_twistArcNumSegments;
  int32_t      m_planeNumCorners;
  NMP::Colour  m_matrixXColour;

  NMP::Colour  m_matrixYColour;
  NMP::Colour  m_matrixZColour;
  int32_t      m_coneAndDialNumSegments;
  NMP::Colour  m_contactPointSimpleWithMagnitudeColour;

  NMP::Colour  m_contactPointSimpleNoMagnitudeColour;
  float        m_contactPointSimpleRadius;        // How large to draw simple contact points.
  float        m_contactPointDetailedNormalScale; // How large to draw the normal of the contact.
  NMP::Colour  m_contactPointDetailedNormalColour;

  NMP::Colour  m_contactPointDetailedForceColour;
  NMP::Colour  m_contactPointDetailedActor0Colour;
  NMP::Colour  m_contactPointDetailedActor1Colour;
  float        m_characterRootSize;

  uint8_t      m_characterRootAlpha;
};

} // namespace MR

#include "sharedDefines/mDebugDrawTessellator.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_DEBUG_DRAW_TESSELLATOR_H
//----------------------------------------------------------------------------------------------------------------------
