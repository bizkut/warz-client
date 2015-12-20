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
/// \file mDebugDrawTessellator.inl
/// \brief Use mDebugDrawTessellator.h, do not include this file directly.
/// \ingroup 
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
# pragma once
#endif
#ifndef M_PHYSICS_DEBUG_DRAW_TESSELLATOR_INL
#define M_PHYSICS_DEBUG_DRAW_TESSELLATOR_INL
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// DDRenderer functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DDRenderer::~DDRenderer()
{}

//----------------------------------------------------------------------------------------------------------------------
// DDTessellator functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DDTessellator::DDTessellator()
{
  // Set the default configuration values.
  m_up.set(0.0f, 1.0f, 0.0f);
  m_twistArcNumSegments = 8;
  m_planeNumCorners = 8;
  m_matrixXColour.set(255, 0, 0, 255);
  m_matrixYColour.set(0, 255, 0, 255);
  m_matrixZColour.set(0, 0, 255, 255);
  m_coneAndDialNumSegments = 13;
  m_contactPointSimpleRadius = 0.01f;
  m_contactPointSimpleWithMagnitudeColour.set(255, 255, 255, 255);
  m_contactPointSimpleNoMagnitudeColour.set(255, 0, 0, 255);
  m_contactPointDetailedNormalScale = 0.03f;
  m_contactPointDetailedNormalColour.set(255, 255, 255, 255);
  m_contactPointDetailedForceColour.set(0, 0, 255, 255);
  m_contactPointDetailedActor0Colour.set(128, 0, 0, 255);
  m_contactPointDetailedActor1Colour.set(0, 128, 0, 255);
  m_characterRootSize = 1.0f;
  m_characterRootAlpha = 255;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::setDDRenderer(DDRenderer* ddRenderer)
{
  NMP_ASSERT(ddRenderer);
  m_ddRenderer = ddRenderer;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessPoint(const NMP::Vector3& point, float radius, NMP::Colour colour)
{
  m_ddRenderer->drawLine(point - NMP::Vector3(radius,0,0), point + NMP::Vector3(radius,0,0), colour);
  m_ddRenderer->drawLine(point - NMP::Vector3(0,radius,0), point + NMP::Vector3(0,radius,0), colour);
  m_ddRenderer->drawLine(point - NMP::Vector3(0,0,radius), point + NMP::Vector3(0,0,radius), colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessSphere(const NMP::Matrix34& point, float radius, NMP::Colour colour)
{
  // Number of segments to divide each circle up into
  const int numSegments = 12;
  float lastS = 0.0f;
  float lastC = radius;
  for (int i = 0 ; i != numSegments ; ++i)
  {
    float angle = (2.0f * NM_PI * (i + 1)) / numSegments;
    float s = sinf(angle) * radius;
    float c = cosf(angle) * radius;

    m_ddRenderer->drawLine(point.getTransformedVector(NMP::Vector3(lastS, lastC, 0.0f)), point.getTransformedVector(NMP::Vector3(s, c, 0.0f)), colour);
    m_ddRenderer->drawLine(point.getTransformedVector(NMP::Vector3(0.0f, lastS, lastC)), point.getTransformedVector(NMP::Vector3(0.0f, s, c)), colour);
    m_ddRenderer->drawLine(point.getTransformedVector(NMP::Vector3(lastS, 0.0f, lastC)), point.getTransformedVector(NMP::Vector3(s, 0.0f, c)), colour);

    lastS = s;
    lastC = c;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Draw a basic line from start to end.
NM_INLINE void DDTessellator::tessLine(const NMP::Vector3& start, const NMP::Vector3& end, NMP::Colour colour)
{
  m_ddRenderer->drawLine(start, end, colour);
}

//----------------------------------------------------------------------------------------------------------------------
// Draw a list of connected lines between each of the provided vertices.
NM_INLINE void DDTessellator::tessPolyLine(
  uint32_t            numVetices, // Min of 2 and numLines = numVetices - 1;
  const NMP::Vector3* vertices,   // Array of vertices between lines.
  NMP::Colour         colour)
{
  NMP_ASSERT(numVetices >= 2);

  for (uint32_t i = 0; i < numVetices-1; ++i)
  {
    m_ddRenderer->drawLine(vertices[i], vertices[i+1], colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Draw a line with various line endings (arrows etc, depending on type) from start to start + offset.
NM_INLINE void DDTessellator::tessVector(
  VectorType          type,
  const NMP::Vector3& start,
  const NMP::Vector3& offset,
  NMP::Colour         colour)
{
  // Draw the main line.
  m_ddRenderer->drawLine(start, start + offset, colour);

  if (type == VT_Normal) // no head
    return;

  bool isDelta = type >= VT_Push && type <= VT_AngleDelta;
  bool hasMass = (type / 2) % 2 == 0;
  float arrowHeadScale = 0.03f;
  float twistScale = 0.06f;
  NMP::Vector3 perp;
  perp.cross(offset, m_up);
  NMP::Vector3 dir2;
  dir2.cross(offset, perp);
  perp.normalise();
  dir2.normalise();
  NMP::Vector3 dir = offset;
  float length = sqrtf(dir.normaliseGetLength());

  if (type % 2 == 1) // if its a rotation
  {
    tessTwistArc(
            start + offset,
            dir * length * twistScale,
            dir2 * length * twistScale,
            perp * length * twistScale,
            colour,
            type >= VT_Force && type <= VT_AngularAcceleration,
            hasMass,
            isDelta);
  }
  else
  {
    NMP::Vector3 tangent = type == VT_Delta ? NMP::Vector3(0, 0, 0) : dir * length * arrowHeadScale;
    tessArrowHead(start + offset, tangent, dir2 * length * arrowHeadScale, colour, hasMass, isDelta);
    if (type >= VT_Force && type <= VT_AngularAcceleration) // if its a second derivative
    {
      tessArrowHead(start + offset - tangent, tangent, dir2 * length * arrowHeadScale, colour, hasMass, isDelta);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// There are 3 arrow head draw states:
// 1. !isDelta
//              c1=sp
//                ^
//             c3/  \c2
// 2. isDelta & hasMass     
//                sp
//             c3\  /c2
//                \/
//                c1
// 3. isDelta & !hasMass    
//             c3----c2
//                c1 
NM_INLINE void DDTessellator::tessArrowHead(
  const NMP::Vector3& startPos,   // Position of arrow point (if not a delta).
  const NMP::Vector3& direction,  // Arrow direction.
  const NMP::Vector3& tangent,    // Generally at 90 degrees to the direction. Defines the width of the arrow.
  NMP::Colour         colour,
  bool                hasMass,    // If true draws solid arrows, else draw line arrows.
  bool                isDelta)    // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{
  // The two lines of code below just rearrange the arrow shape for deltas (ie shifts in space).
  NMP::Vector3 directionOffset = isDelta ? (hasMass ? -direction : NMP::Vector3(0, 0, 0)) : direction;
  NMP::Vector3 start = isDelta && hasMass ? startPos + directionOffset : startPos;


  NMP::Vector3 corner1 = start;
  NMP::Vector3 corner2 = start - directionOffset + tangent;
  NMP::Vector3 corner3 = start - directionOffset - tangent;
  if (hasMass)
  {
    m_ddRenderer->drawTriangle(corner1, corner2, corner3, colour);
  }
  else
  {
    m_ddRenderer->drawLine(corner1, corner2, colour);
    m_ddRenderer->drawLine(corner1, corner3, colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessTwistArc(
  const NMP::Vector3& pos,             // Center of arc.
  const NMP::Vector3& primaryDir,      // Normal of the plane which the arc lies on (Not necessarily normalised).
  const NMP::Vector3& dir,
  const NMP::Vector3& dir2,
  NMP::Colour         colour,
  bool                doubleArrowHead, // Draw an arrow at both ends of arc.
  bool                hasMass,         // Arrow heads are drawn as solid triangles.
  bool                isDelta)         // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{
  NMP::Vector3 oldPos = pos + dir;
  NMP::Vector3 newDir = -dir2 * cosf(0.3f) + dir * sinf(0.3f);
  tessArrowHead(oldPos, newDir * 0.4f, primaryDir * 0.4f, colour, hasMass, isDelta);
  if (doubleArrowHead)
  {
    float t = 0.4f;
    NMP::Vector3 point = pos + dir * cosf(t) + dir2 * sinf(t);
    tessArrowHead(point, newDir * 0.4f, primaryDir * 0.4f, colour, hasMass, isDelta);
  }
  for (int32_t i = 1; i < m_twistArcNumSegments; i++)
  {
    float t = 4.0f * (float)i / ((float)m_twistArcNumSegments - 1.0f);
    NMP::Vector3 point = pos + dir * cosf(t) + dir2 * sinf(t);
    m_ddRenderer->drawLine(oldPos, point, colour);
    oldPos = point;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessPlane(
  const NMP::Vector3& point,
  const NMP::Vector3& normal,
  float               radius,
  NMP::Colour         colour,
  float               normalScale)
{
  NMP::Vector3 randomAxis(1,2,3);
  NMP::Vector3 xAxis = radius * NMP::vNormalise(NMP::vCross(randomAxis, normal));
  NMP::Vector3 yAxis = NMP::vCross(xAxis, normal);
  float x = 0.f, y = 1.f;
  for (int32_t i = 0; i < m_planeNumCorners; i++)
  {
    float angle = 2.f*3.1415f*(float)(i+1) / (float)(m_planeNumCorners);
    float newX = sinf(angle); float newY = cosf(angle);
    m_ddRenderer->drawLine(point+x*xAxis + y*yAxis, point + newX*xAxis + newY*yAxis, colour);
    x = newX; y = newY;
  }
  m_ddRenderer->drawLine(point, point + normal*radius*normalScale, colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessTriangle(
  const NMP::Vector3& v1,     // 1-2-3 should form a clockwise face.
  const NMP::Vector3& v2,
  const NMP::Vector3& v3,
  NMP::Colour         colour)
{
  m_ddRenderer->drawTriangle(v1, v2, v3, colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessMatrix(const NMP::Matrix34& matrix, float scale, uint8_t alpha)
{
  NMP::Vector3 org = matrix.translation();
  NMP::Vector3 x(matrix.xAxis() * scale);
  NMP::Vector3 y(matrix.yAxis() * scale);
  NMP::Vector3 z(matrix.zAxis() * scale);
  m_ddRenderer->drawLine(org, org + x, NMP::Colour(m_matrixXColour, alpha));
  m_ddRenderer->drawLine(org, org + y, NMP::Colour(m_matrixYColour, alpha));
  m_ddRenderer->drawLine(org, org + z, NMP::Colour(m_matrixZColour, alpha));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessNonUniformMatrix(const NMP::Matrix34& matrix, float scale, uint8_t alpha)
{
  NMP::Vector3 org = matrix.translation();
  NMP::Vector3 x(matrix.xAxis() * scale);
  NMP::Vector3 y(matrix.yAxis() * scale);
  NMP::Vector3 z(matrix.zAxis() * scale);
  m_ddRenderer->drawLine(org - x, org + x, NMP::Colour(m_matrixXColour, alpha));
  m_ddRenderer->drawLine(org - y, org + y, NMP::Colour(m_matrixYColour, alpha));
  m_ddRenderer->drawLine(org - z, org + z, NMP::Colour(m_matrixZColour, alpha));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessConeAndDial(
    const NMP::Vector3& pos,
    const NMP::Vector3& dir,
    float               angle,
    float               size,
    const NMP::Vector3& dialDirection,
    NMP::Colour         colour)
{
  NMP::Vector3 randomVec(1, 2, 3);
  NMP::Vector3 xAxis = vCross(randomVec, dir);
  xAxis.normalise();
  NMP::Vector3 yAxis = vCross(dir, xAxis);
  yAxis.normalise();

  uint8_t alpha = colour.getA();
  float sideScale = size * sinf(angle);
  float frontScale = size * cosf(angle);
  NMP::Vector3 oldP(0,0,0);
  for (int32_t i = 0; i < m_coneAndDialNumSegments; i++)
  {
    float phase = 2.f * NM_PI * (float)i / (float)(m_coneAndDialNumSegments - 1.f);
    NMP::Vector3 p = pos + frontScale * dir + xAxis * sideScale * sinf(phase) + yAxis * sideScale * cosf(phase);
    if (i)
    {
      m_ddRenderer->drawLine(oldP, p, colour); 
      colour.setA(alpha/2);
      m_ddRenderer->drawLine(pos, p, colour);   // Half alpha line.
    }
    oldP = p;
  }
  m_ddRenderer->drawLine(pos, pos + dialDirection * size, NMP::Colour(255, 255, 255, alpha)); // White line.
}

//----------------------------------------------------------------------------------------------------------------------
// Draw a point at the position. Different colours depending on whether there is any contact force or not.
NM_INLINE void DDTessellator::tessContactPointSimple(const NMP::Vector3& point, float forceMagnitudeSquared) 
{
  if (forceMagnitudeSquared > 0.0f)
    tessPoint(point, m_contactPointSimpleRadius, m_contactPointSimpleWithMagnitudeColour);
  else
    tessPoint(point, m_contactPointSimpleRadius, m_contactPointSimpleNoMagnitudeColour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessContactPointDetailed(
  const NMP::Vector3& point,      // Contact point.
  const NMP::Vector3& normal,     // Contact normal.
  const NMP::Vector3& force,      // Force at contact point. 
  const NMP::Vector3& actor0Pos,  // Actor 0 root position.
  const NMP::Vector3& actor1Pos)  // Actor 1 root position.
{
  m_ddRenderer->drawLine(point, point + (normal * m_contactPointDetailedNormalScale), m_contactPointDetailedNormalColour); // Contact normal.
  m_ddRenderer->drawLine(point, actor0Pos, m_contactPointDetailedActor0Colour);  // Line to actor 0 root.
  m_ddRenderer->drawLine(point, actor1Pos, m_contactPointDetailedActor1Colour);  // Line to actor 1 root.
  tessVector(VT_Force, point, force, m_contactPointDetailedForceColour);   // Force at contact point.
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessCharacterRoot(const NMP::Matrix34& characterControllerRoot)
{
  tessMatrix(characterControllerRoot, m_characterRootSize, m_characterRootAlpha);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessBox(const NMP::Vector3 &focalCentre, const NMP::Vector3& focalRadii, NMP::Colour colour)
{
  NMP::Vector3 corners[8];
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      for (int k = 0; k < 2; k++)
      {
        corners[4*i + 2*j + k].set(
          (float)(2 * i - 1) * focalRadii[0],
          (float)(2 * j - 1) * focalRadii[1],
          (float)(2 * k - 1) * focalRadii[2]
        );
      }
    }
  }

  // top
  m_ddRenderer->drawLine(focalCentre+corners[0], focalCentre+corners[1], colour);
  m_ddRenderer->drawLine(focalCentre+corners[0], focalCentre+corners[2], colour);
  m_ddRenderer->drawLine(focalCentre+corners[2], focalCentre+corners[3], colour);
  m_ddRenderer->drawLine(focalCentre+corners[1], focalCentre+corners[3], colour);
  // base
  m_ddRenderer->drawLine(focalCentre+corners[4], focalCentre+corners[5], colour);
  m_ddRenderer->drawLine(focalCentre+corners[4], focalCentre+corners[6], colour);
  m_ddRenderer->drawLine(focalCentre+corners[6], focalCentre+corners[7], colour);
  m_ddRenderer->drawLine(focalCentre+corners[5], focalCentre+corners[7], colour);
  // struts
  m_ddRenderer->drawLine(focalCentre+corners[0], focalCentre+corners[4], colour);
  m_ddRenderer->drawLine(focalCentre+corners[1], focalCentre+corners[5], colour);
  m_ddRenderer->drawLine(focalCentre+corners[2], focalCentre+corners[6], colour);
  m_ddRenderer->drawLine(focalCentre+corners[3], focalCentre+corners[7], colour);
}

void DDTessellator::tessBBox(const NMP::Matrix34 &tm, const NMP::Vector3 &halfSizes, NMP::Colour colour)
{
  NMP::Vector3 corners[8];
  NMP::Vector3 x = halfSizes[0] * tm.xAxis();
  NMP::Vector3 y = halfSizes[1] * tm.yAxis();
  NMP::Vector3 z = halfSizes[2] * tm.zAxis();
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      for (int k = 0; k < 2; k++)
      {
        corners[4*i + 2*j + k] = 
          (float)(2 * i - 1) * x +
          (float)(2 * j - 1) * y +
          (float)(2 * k - 1) * z;
      }
    }
  }

  NMP::Vector3 pos = tm.translation();

  // top
  m_ddRenderer->drawLine(pos+corners[0], pos+corners[1], colour);
  m_ddRenderer->drawLine(pos+corners[0], pos+corners[2], colour);
  m_ddRenderer->drawLine(pos+corners[2], pos+corners[3], colour);
  m_ddRenderer->drawLine(pos+corners[1], pos+corners[3], colour);
  // base
  m_ddRenderer->drawLine(pos+corners[4], pos+corners[5], colour);
  m_ddRenderer->drawLine(pos+corners[4], pos+corners[6], colour);
  m_ddRenderer->drawLine(pos+corners[6], pos+corners[7], colour);
  m_ddRenderer->drawLine(pos+corners[5], pos+corners[7], colour);
  // struts
  m_ddRenderer->drawLine(pos+corners[0], pos+corners[4], colour);
  m_ddRenderer->drawLine(pos+corners[1], pos+corners[5], colour);
  m_ddRenderer->drawLine(pos+corners[2], pos+corners[6], colour);
  m_ddRenderer->drawLine(pos+corners[3], pos+corners[7], colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDTessellator::tessSphereSweep(
  const NMP::Vector3& position,
  const NMP::Vector3& motion,
  float               radius,
  NMP::Colour         colour)
{
  NMP::Vector3 up(-0.1f,1,0.1f);
  up.normalise();
  NMP::Vector3 side = NMP::vCross(up, motion);
  side.normalise();
  up = NMP::vCross(motion, side);
  up.normalise();
  tessVector(VT_Normal, position + up*radius, motion, colour);
  tessVector(VT_Normal, position - up*radius, motion, colour);
  tessVector(VT_Normal, position + side*radius, motion, colour);
  tessVector(VT_Normal, position - side*radius, motion, colour);
  NMP::Vector3 oldPos(0,0,0);
  NMP::Vector3 oldPos2(0,0,0);
  int numSides = 6;
  for (int i = 0; i<2*numSides+1; i++)
  {
    float angle = 2.f*NM_PI*(float)i/((float)numSides * 2.f);
    NMP::Vector3 pos = position + motion + (sinf(angle)*up + cosf(angle)*side)*radius;
    if (i)
    {
      m_ddRenderer->drawLine(oldPos, pos, colour);
    }
    oldPos = pos;
  }
  NMP::Vector3 fwd = motion;
  fwd.normalise();
  for (int i = 0; i<numSides+1; i++)
  {
    float angle = NM_PI*(float)i/(float)numSides;
    NMP::Vector3 pos = position + motion + (sinf(angle)*fwd + cosf(angle)*up)*radius;
    NMP::Vector3 pos2 = position + motion + (sinf(angle)*fwd + cosf(angle)*side)*radius;
    if (i)
    {
      m_ddRenderer->drawLine(oldPos, pos, colour);
      m_ddRenderer->drawLine(oldPos2, pos2, colour);
    }
    oldPos = pos;
    oldPos2 = pos2;
  }
}

NM_INLINE void DDTessellator::tessEnvironmentPatch(
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  float               size,
  NMP::Colour         colour)
{
  // to get two orthogonal axes to a normal, you start with a random vector which is not axis aligned, to prevent an easy singularity 
  NMP::Vector3 obscureVector(1,2,3); 
  NMP::Vector3 side0 = NMP::vNormalise(NMP::vCross(normal, obscureVector));
  NMP::Vector3 side1 = NMP::vCross(normal, side0) * size;
  side0 *= size;

  const NMP::Vector3 vertex0(position - (side0 * 0.5f) - (side1 * 0.5f));
  const NMP::Vector3 vertex1(vertex0 + side1);
  const NMP::Vector3 vertex2(vertex1 + side0);
  const NMP::Vector3 vertex3(vertex0 + side0);

  // Render outline of a square with both diagonals.
  m_ddRenderer->drawLine(vertex0, vertex1, colour);
  m_ddRenderer->drawLine(vertex1, vertex2, colour);
  m_ddRenderer->drawLine(vertex2, vertex3, colour);
  m_ddRenderer->drawLine(vertex3, vertex0, colour);

  // Render square's diagonals.
  m_ddRenderer->drawLine(vertex0, vertex2, colour);
  m_ddRenderer->drawLine(vertex1, vertex3, colour);

  // Render normal at square's center.
  tessVector(MR::VT_Normal, position, normal, colour);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // M_PHYSICS_DEBUG_DRAW_TESSELLATOR_INL
//----------------------------------------------------------------------------------------------------------------------
