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
#ifndef MR_DEBUG_DRAW_CLIENT_INL
#define MR_DEBUG_DRAW_CLIENT_INL
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Scratch Pad
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::logScratchpadInt(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::NodeID     NMP_UNUSED(sourceNodeID),
  const char*    NMP_UNUSED(sourceTagName),
  MR::FrameCount NMP_UNUSED(sourceFrame),
  const char*    NMP_UNUSED(tag),
  int32_t        NMP_UNUSED(value),
  const char*    NMP_UNUSED(variableName),
  uint16_t       NMP_UNUSED(lineNumber))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::logScratchpadFloat(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::NodeID     NMP_UNUSED(sourceNodeID),
  const char*    NMP_UNUSED(sourceTagName),
  MR::FrameCount NMP_UNUSED(sourceFrame),
  const char*    NMP_UNUSED(tag),
  float          NMP_UNUSED(value),
  const char*    NMP_UNUSED(variableName),
  uint16_t       NMP_UNUSED(lineNumber))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::logScratchpadBool(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::NodeID     NMP_UNUSED(sourceNodeID),
  const char*    NMP_UNUSED(sourceTagName),
  MR::FrameCount NMP_UNUSED(sourceFrame),
  const char*    NMP_UNUSED(tag),
  bool           NMP_UNUSED(value),
  const char*    NMP_UNUSED(variableName),
  uint16_t       NMP_UNUSED(lineNumber))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::logScratchpadVector3(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::NodeID     NMP_UNUSED(sourceNodeID),
  const char*    NMP_UNUSED(sourceTagName),
  MR::FrameCount NMP_UNUSED(sourceFrame),
  const char*    NMP_UNUSED(tag),
  const NMP::Vector3& NMP_UNUSED(value),
  const char*    NMP_UNUSED(variableName),
  uint16_t       NMP_UNUSED(lineNumber))
{}

//----------------------------------------------------------------------------------------------------------------------
// Task Execution
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::beginDispatcherTaskExecute(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::FrameCount NMP_UNUSED(sourceFrame),
  MR::TaskID     NMP_UNUSED(taskID),
  const char*    NMP_UNUSED(taskName),
  MR::NodeID     NMP_UNUSED(owningNodeID))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::addDispatcherTaskExecuteParameter(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::FrameCount NMP_UNUSED(sourceFrame),
  MR::TaskID     NMP_UNUSED(taskID),
  const char*    NMP_UNUSED(semanticName),
  MR::NodeID     NMP_UNUSED(owningNodeID),
  MR::NodeID     NMP_UNUSED(targetNodeID),
  uint32_t       NMP_UNUSED(validFrame),
  bool           NMP_UNUSED(isInput),
  bool           NMP_UNUSED(isOutput),
  uint16_t       NMP_UNUSED(lifespan))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::endDispatcherTaskExecute(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::FrameCount NMP_UNUSED(sourceFrame))
{}

//----------------------------------------------------------------------------------------------------------------------
// State Machine Requests
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::logStateMachineMessageEventMsg(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::FrameCount NMP_UNUSED(sourceFrame),
  MR::MessageID  NMP_UNUSED(messageID), 
  MR::NodeID     NMP_UNUSED(targetSMNodeID),
  bool           NMP_UNUSED(set))
{}

//----------------------------------------------------------------------------------------------------------------------
// Profiling Points
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::logProfilePointTiming(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::FrameCount NMP_UNUSED(sourceFrame),
  const char*    NMP_UNUSED(name),
  float          NMP_UNUSED(timing))
{}

//----------------------------------------------------------------------------------------------------------------------
// Debug Draw
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::clearDebugDraw()
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawPoint(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(point),
  float               NMP_UNUSED(radius),
  NMP::Colour         NMP_UNUSED(colour))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawSphere(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Matrix34& NMP_UNUSED(tm),
  float               NMP_UNUSED(radius),
  NMP::Colour         NMP_UNUSED(colour))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawLine(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(start),
  const NMP::Vector3& NMP_UNUSED(end),
  NMP::Colour         NMP_UNUSED(colour))
{
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawPolyLine(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  uint32_t            NMP_UNUSED(numVetices), // Min of 2 and numLines = numVetices - 1;
  const NMP::Vector3* NMP_UNUSED(vertices),   // Array of vertices between lines.
  NMP::Colour         NMP_UNUSED(colour))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawVector(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  VectorType          NMP_UNUSED(type),
  const NMP::Vector3& NMP_UNUSED(start),
  const NMP::Vector3& NMP_UNUSED(offset),
  NMP::Colour         NMP_UNUSED(colour))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawArrowHead(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(startPos),   // Position of arrow point (if not a delta).
  const NMP::Vector3& NMP_UNUSED(direction),  // Arrow direction.
  const NMP::Vector3& NMP_UNUSED(tangent),    // Generally at 90 degrees to the direction. Defines the width of the arrow.
  NMP::Colour         NMP_UNUSED(colour),
  bool                NMP_UNUSED(hasMass),    // If true draws solid arrows, else draw line arrows.
  bool                NMP_UNUSED(isDelta))    // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawTwistArc(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(pos),             // Center of arc.
  const NMP::Vector3& NMP_UNUSED(primaryDir),      // Normal of the plane which the arc lies on (Not necessarily normalised).
  const NMP::Vector3& NMP_UNUSED(dir),             //
  const NMP::Vector3& NMP_UNUSED(dir2),            //
  NMP::Colour         NMP_UNUSED(colour),
  bool                NMP_UNUSED(doubleArrowHead), // Draw an arrow at both ends of arc.
  bool                NMP_UNUSED(hasMass),         // Arrow heads are drawn as solid triangles.
  bool                NMP_UNUSED(isDelta))         // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawPlane(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(point),
  const NMP::Vector3& NMP_UNUSED(normal),
  float               NMP_UNUSED(radius),
  NMP::Colour         NMP_UNUSED(colour),
  float               NMP_UNUSED(normalScale))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawTriangle(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(v1),     // 1-2-3 should form a clockwise face.
  const NMP::Vector3& NMP_UNUSED(v2),
  const NMP::Vector3& NMP_UNUSED(v3),
  NMP::Colour         NMP_UNUSED(colour))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawMatrix(
  uint32_t             NMP_UNUSED(sourceInstanceID),
  MR::NodeID           NMP_UNUSED(sourceNodeID),
  const char*          NMP_UNUSED(sourceTagName),
  MR::FrameCount       NMP_UNUSED(sourceFrame),
  MR::LimbIndex        NMP_UNUSED(sourceLimbIndex),
  const NMP::Matrix34& NMP_UNUSED(matrix),
  float                NMP_UNUSED(scale),
  uint8_t              NMP_UNUSED(alpha))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawNonUniformMatrix(
  uint32_t             NMP_UNUSED(sourceInstanceID),
  MR::NodeID           NMP_UNUSED(sourceNodeID),
  const char*          NMP_UNUSED(sourceTagName),
  MR::FrameCount       NMP_UNUSED(sourceFrame),
  MR::LimbIndex        NMP_UNUSED(sourceLimbIndex),
  const NMP::Matrix34& NMP_UNUSED(matrix),
  float                NMP_UNUSED(scale),
  uint8_t              NMP_UNUSED(alpha))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawConeAndDial(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(pos),
  const NMP::Vector3& NMP_UNUSED(dir),
  float               NMP_UNUSED(angle),
  float               NMP_UNUSED(size),
  const NMP::Vector3& NMP_UNUSED(dialDirection),
  NMP::Colour         NMP_UNUSED(colour)) // Red by default
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawContactPointSimple(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(point),                  // Contact point.
  float               NMP_UNUSED(forceMagnitudeSquared))  // Force magnitude squared at contact point. 
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawContactPointDetailed(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(point),      // Contact point.
  const NMP::Vector3& NMP_UNUSED(normal),     // Contact normal.
  const NMP::Vector3& NMP_UNUSED(force),      // Force at contact point. 
  const NMP::Vector3& NMP_UNUSED(actor0Pos),  // Actor 0 root position.
  const NMP::Vector3& NMP_UNUSED(actor1Pos))  // Actor 1 root position.
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawCharacterRoot(
  uint32_t             NMP_UNUSED(sourceInstanceID),
  MR::NodeID           NMP_UNUSED(sourceNodeID),
  const char*          NMP_UNUSED(sourceTagName),
  MR::FrameCount       NMP_UNUSED(sourceFrame),
  MR::LimbIndex        NMP_UNUSED(sourceLimbIndex),
  const NMP::Matrix34& NMP_UNUSED(characterControllerRoot))
{
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawBox(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(focalCentre),
  const NMP::Vector3& NMP_UNUSED(focalRadii),
  NMP::Colour         NMP_UNUSED(colour))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawBBox(
                                    uint32_t            NMP_UNUSED(sourceInstanceID),
                                    MR::NodeID          NMP_UNUSED(sourceNodeID),
                                    const char*         NMP_UNUSED(sourceTagName),
                                    MR::FrameCount      NMP_UNUSED(sourceFrame),
                                    MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
                                    const NMP::Matrix34& NMP_UNUSED(tm),
                                    const NMP::Vector3& NMP_UNUSED(focalRadii),
                                    NMP::Colour         NMP_UNUSED(colour))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawSphereSweep(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(position),
  const NMP::Vector3& NMP_UNUSED(motion),
  float               NMP_UNUSED(radius),
  NMP::Colour         NMP_UNUSED(colour))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawText(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(position),    // Bottom left hand corner of string.
  const char*         NMP_UNUSED(text),
  NMP::Colour         NMP_UNUSED(colour))
{}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DebugClient::drawEnvironmentPatch(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& NMP_UNUSED(position),
  const NMP::Vector3& NMP_UNUSED(normal),
  float               NMP_UNUSED(radius),
  NMP::Colour         NMP_UNUSED(colour))
{}

//----------------------------------------------------------------------------------------------------------------------
// DDClientTessellate functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawPoint(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& point,
  float               radius,
  NMP::Colour         colour)
{
  tessPoint(point, radius, colour);
}

//----------------------------------------------------------------------------------------------------------------------
// DDClientTessellate functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawSphere(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Matrix34& point,
  float               radius,
  NMP::Colour         colour)
{
  tessSphere(point, radius, colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawLine(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& start,
  const NMP::Vector3& end,
  NMP::Colour         colour)
{
  tessLine(start, end, colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawPolyLine(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  uint32_t            numVetices, // Min of 2 and numLines = numVetices - 1;
  const NMP::Vector3* vertices,   // Array of vertices between lines.
  NMP::Colour         colour)
{
  tessPolyLine(numVetices, vertices, colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawVector(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  VectorType          type,
  const NMP::Vector3& start,
  const NMP::Vector3& offset,
  NMP::Colour         colour) 
{
  tessVector(type, start, offset, colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawArrowHead(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& startPos,   // Position of arrow point (if not a delta).
  const NMP::Vector3& direction,  // Arrow direction.
  const NMP::Vector3& tangent,    // Generally at 90 degrees to the direction. Defines the width of the arrow.
  NMP::Colour         colour,
  bool                hasMass,    // If true draws solid arrows, else draw line arrows.
  bool                isDelta)    // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{
  tessArrowHead(startPos, direction, tangent, colour, hasMass, isDelta);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawTwistArc(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& pos,             // Center of arc.
  const NMP::Vector3& primaryDir,      // Normal of the plane which the arc lies on (Not necessarily normalised).
  const NMP::Vector3& dir,             //
  const NMP::Vector3& dir2,            //
  NMP::Colour         colour,
  bool                doubleArrowHead, // Draw an arrow at both ends of arc.
  bool                hasMass,         // Arrow heads are drawn as solid triangles.
  bool                isDelta)         // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{
  tessTwistArc(pos, primaryDir, dir, dir2, colour, doubleArrowHead, hasMass, isDelta);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawPlane(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& point,
  const NMP::Vector3& normal,
  float               radius,
  NMP::Colour         colour,
  float               normalScale)
{
  tessPlane(point, normal, radius, colour, normalScale);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawTriangle(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& v1,     // 1-2-3 should form a clockwise face.
  const NMP::Vector3& v2,
  const NMP::Vector3& v3,
  NMP::Colour         colour)
{
  tessTriangle(v1, v2, v3, colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawMatrix(
  uint32_t             NMP_UNUSED(sourceInstanceID),
  MR::NodeID           NMP_UNUSED(sourceNodeID),
  const char*          NMP_UNUSED(sourceTagName),
  MR::FrameCount       NMP_UNUSED(sourceFrame),
  MR::LimbIndex        NMP_UNUSED(sourceLimbIndex),
  const NMP::Matrix34& matrix,
  float                scale,
  uint8_t              alpha)
{
  tessMatrix(matrix, scale, alpha);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawNonUniformMatrix(
  uint32_t             NMP_UNUSED(sourceInstanceID),
  MR::NodeID           NMP_UNUSED(sourceNodeID),
  const char*          NMP_UNUSED(sourceTagName),
  MR::FrameCount       NMP_UNUSED(sourceFrame),
  MR::LimbIndex        NMP_UNUSED(sourceLimbIndex),
  const NMP::Matrix34& matrix,
  float                scale,
  uint8_t              alpha)
{
  tessNonUniformMatrix(matrix, scale, alpha);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawConeAndDial(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& pos,
  const NMP::Vector3& dir,
  float               angle,
  float               size,
  const NMP::Vector3& dialDirection,
  NMP::Colour         colour) // Red by default
{
  tessConeAndDial(pos, dir, angle, size, dialDirection, colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawContactPointSimple(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& point,                 // Contact point.
  float               forceMagnitudeSquared) // Force magnitude squared at contact point. 
{
  tessContactPointSimple(point, forceMagnitudeSquared);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawContactPointDetailed(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& point,     // Contact point.
  const NMP::Vector3& normal,    // Contact normal.
  const NMP::Vector3& force,     // Force at contact point. 
  const NMP::Vector3& actor0Pos, // Actor 0 root position.
  const NMP::Vector3& actor1Pos) // Actor 1 root position.
{
  tessContactPointDetailed(point, normal, force, actor0Pos, actor1Pos);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawCharacterRoot(
  uint32_t             NMP_UNUSED(sourceInstanceID),
  MR::NodeID           NMP_UNUSED(sourceNodeID),
  const char*          NMP_UNUSED(sourceTagName),
  MR::FrameCount       NMP_UNUSED(sourceFrame),
  MR::LimbIndex        NMP_UNUSED(sourceLimbIndex),
  const NMP::Matrix34& characterControllerRoot)
{
  tessCharacterRoot(characterControllerRoot);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawBox(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& focalCentre,
  const NMP::Vector3& focalRadii,
  NMP::Colour         colour)
{
  tessBox(focalCentre, focalRadii, colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawSphereSweep(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& position,
  const NMP::Vector3& motion,
  float               radius,
  NMP::Colour         colour)
{
  tessSphereSweep(position, motion, radius, colour);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void DDClientTessellate::drawEnvironmentPatch(
  uint32_t            NMP_UNUSED(sourceInstanceID),
  MR::NodeID          NMP_UNUSED(sourceNodeID),
  const char*         NMP_UNUSED(sourceTagName),
  MR::FrameCount      NMP_UNUSED(sourceFrame),
  MR::LimbIndex       NMP_UNUSED(sourceLimbIndex),
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  float               size,
  NMP::Colour         colour)
{
  tessEnvironmentPatch(position, normal, size, colour);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_DEBUG_DRAW_CLIENT_INL
//----------------------------------------------------------------------------------------------------------------------
