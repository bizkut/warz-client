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
#ifndef MCOMMS_DEBUG_DRAW_OBJECTS_H
#define MCOMMS_DEBUG_DRAW_OBJECTS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMColour.h"

#include "sharedDefines/mCoreDebugInterface.h"
#include "sharedDefines/mDebugDrawTessellator.h"

#include "comms/attribute.h"
#include "comms/packet.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
// Debug draw types.
enum DebugDrawTypes
{
  DDTYPE_POINT = 0,              // 
  DDTYPE_SPHERE,                 // 
  DDTYPE_LINE,                   // 
  DDTYPE_POLY_LINE,              // 
  DDTYPE_VECTOR,                 // Inc: Lines with arrows at beginning, end or both.
  DDTYPE_ARROW_HEAD,             //
  DDTYPE_TWIST_ARC,              //
  DDTYPE_PLANE,                  //
  DDTYPE_TRIANGLE,               // Filled triangle.
  DDTYPE_MATRIX,                 //
  DDTYPE_CONE_AND_DIAL,          //
  DDTYPE_CONTACT_POINT,          // 
  DDTYPE_CONTACT_POINT_DETAILED, //
  DDTYPE_CHARACTER_ROOT,         //
  DDTYPE_BOX,                    //
  DDTYPE_BBOX,                   //
  DDTYPE_SPHERE_SWEEP,           //
  DDTYPE_TEXT,                   //
  DDTYPE_ENVIRONMENT_PATCH,      //
  NUM_DDTYPES
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawPoint
{
  NMP::Vector3 m_point;
  float        m_radius;
  NMP::Colour  m_colour;

  // Endian swaps in place on buffer.
  static DebugDrawPoint* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawPoint* deserialiseTx(void *buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& point,
    float               radius,
    NMP::Colour         colour,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawSphere
{
  NMP::Matrix34 m_tm;
  float        m_radius;
  NMP::Colour  m_colour;

  // Endian swaps in place on buffer.
  static DebugDrawSphere* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawSphere* deserialiseTx(void *buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Matrix34& tm,
    float               radius,
    NMP::Colour         colour,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawLine
{
  NMP::Vector3 m_start;
  NMP::Vector3 m_end;
  NMP::Colour  m_colour;

  // Endian swaps in place on buffer.
  static DebugDrawLine* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawLine* deserialiseTx(void *buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& start,
    const NMP::Vector3& end,
    NMP::Colour         colour,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawPolyLine
{
  uint32_t    m_numVertices;
  NMP::Colour m_colour;
  uint32_t    m_pad[2];

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawPolyLine* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    uint32_t            numVertices, // Min of 2 and numLines = numVertices - 1;
    const NMP::Vector3* vertices,
    NMP::Colour         colour,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);

  const NMP::Vector3& getVertex(uint32_t i) const;
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawVector
{
  NMP::Vector3  m_start;
  NMP::Vector3  m_offset;
  NMP::Colour   m_colour;
  MR::VectorType m_vectorType;

  // Endian swaps in place on buffer.
  static DebugDrawVector* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawVector* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& start,
    const NMP::Vector3& offset,
    NMP::Colour         colour,
    MR::VectorType      vectorType,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawArrowHead
{
  NMP::Vector3    m_position;
  NMP::Vector3    m_direction;
  NMP::Vector3    m_tangent;
  NMP::Colour     m_colour;
  bool            m_hasMass;
  bool            m_isDelta;

  // Endian swaps in place on buffer.
  static DebugDrawArrowHead* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawArrowHead* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& position,
    const NMP::Vector3& direction,
    const NMP::Vector3& tangent,
    NMP::Colour         colour,
    bool                hasMass,
    bool                isDelta,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawTwistArc
{
  NMP::Vector3 m_position;           // Center of arc.
  NMP::Vector3 m_primaryDirection;   // Normal of the plane which the arc lies on (Not necessarily normalised).
  NMP::Vector3 m_direction;
  NMP::Vector3 m_direction2;
  NMP::Colour  m_colour;
  bool         m_doubleArrowHead;    // Draw an arrow at both ends of arc.
  bool         m_hasMass;            // Arrow heads are drawn as solid triangles.
  bool         m_isDelta;

  // Endian swaps in place on buffer.
  static DebugDrawTwistArc* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawTwistArc* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& position,           // Center of arc.
    const NMP::Vector3& primaryDirection,   // Normal of the plane which the arc lies on (Not necessarily normalised).
    const NMP::Vector3& direction,
    const NMP::Vector3& direction2,
    NMP::Colour         colour,
    bool                doubleArrowHead,    // Draw an arrow at both ends of arc.
    bool                hasMass,            // Arrow heads are drawn as solid triangles.
    bool                isDelta,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawPlane
{
  NMP::Vector3  m_position;
  NMP::Vector3  m_normal;
  float         m_halfSize;
  NMP::Colour   m_colour;
  float         m_normalScale;

  // Endian swaps in place on buffer.
  static DebugDrawPlane* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawPlane* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& position,
    const NMP::Vector3& normal,
    float               halfSize,
    NMP::Colour         colour,
    float               normalScale,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawTriangle
{
  NMP::Vector3  m_vertices[3];
  NMP::Colour   m_colour;

  // Endian swaps in place on buffer.
  static DebugDrawTriangle* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawTriangle* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& vertex0,
    const NMP::Vector3& vertex1,
    const NMP::Vector3& vertex2,
    NMP::Colour         colour,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawMatrix
{
  enum MatrixTypes
  {
    MT_UNIFORM_MATRIX,
    MT_NONUNIFORM_MATRIX,
  };

  NMP::Matrix34 m_matrix;
  float         m_scale;
  float         m_alpha;
  MatrixTypes   m_matrixType;

  // Endian swaps in place on buffer.
  static DebugDrawMatrix* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawMatrix* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Matrix34&  matrix,
    float                 scale,
    float                 alpha,
    MatrixTypes           matrixType,
    void*                 outputBuffer,
    uint32_t              outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawConeAndDial
{
  NMP::Vector3  m_position;
  NMP::Vector3  m_direction;
  NMP::Vector3  m_dialDirection;
  float         m_angle;
  float         m_size;
  NMP::Colour   m_colour;

  // Endian swaps in place on buffer.
  static DebugDrawConeAndDial* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawConeAndDial* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& position,
    const NMP::Vector3& direction,
    const NMP::Vector3& dialDirection,
    float               angle,
    float               size,
    NMP::Colour         colour,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawContactPoint
{
  NMP::Vector3  m_position;
  float         m_forceMagnitudeSquared;

  // Endian swaps in place on buffer.
  static DebugDrawContactPoint* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawContactPoint* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& position,
    float               forceMagnitudeSquared,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawContactPointDetailed
{
  NMP::Vector3 m_position;             // Contact point.
  NMP::Vector3 m_normal;               // Contact normal.
  NMP::Vector3 m_force;                // Force at contact point. 
  NMP::Vector3 m_actor0Position;       // Actor 0 root position.
  NMP::Vector3 m_actor1Position;       // Actor 1 root position.

  // Endian swaps in place on buffer.
  static DebugDrawContactPointDetailed* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawContactPointDetailed* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& point,
    const NMP::Vector3& normal,
    const NMP::Vector3& force,
    const NMP::Vector3& actor0Position,
    const NMP::Vector3& actor1Position,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawCharacterControllerRoot
{
  NMP::Matrix34 m_controllerRoot;

  // Endian swaps in place on buffer.
  static DebugDrawCharacterControllerRoot* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawCharacterControllerRoot* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Matrix34&  controllerRoot,
    void*                 outputBuffer,
    uint32_t              outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawBox
{
  NMP::Vector3  m_position;
  NMP::Vector3  m_halfSizes;
  NMP::Colour   m_colour;

  // Endian swaps in place on buffer.
  static DebugDrawBox* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawBox* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& position,
    const NMP::Vector3& halfSizes,
    const NMP::Colour&  colour,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawBBox
{
  NMP::Matrix34  m_tm;
  NMP::Vector3  m_halfSizes;
  NMP::Colour   m_colour;

  // Endian swaps in place on buffer.
  static DebugDrawBBox* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawBBox* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Matrix34& tm,
    const NMP::Vector3& halfSizes,
    const NMP::Colour&  colour,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawSphereSweep
{
  NMP::Vector3  m_position;
  NMP::Vector3  m_direction;
  float         m_radius;
  NMP::Colour   m_colour;

  // Endian swaps in place on buffer.
  static DebugDrawSphereSweep* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawSphereSweep* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& position,
    const NMP::Vector3& direction,
    float               radius,
    const NMP::Colour&  colour,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawText
{
  NMP::Vector3  m_position;
  NMP::Colour   m_colour;

  // We do not tokenise strings because we can run out of available tokens.
  //  For example if we are sending a string of x, y, z position it will be different every frame.
  char*         m_text;
  uint32_t      m_pad[2];

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawText* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& position,
    const NMP::Colour&  colour,
    const char*         text,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);

  const char* getText() const { return ((char*)(this + 1)); }
};

//------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct DebugDrawEnvironmentPatch
{
  NMP::Vector3  m_position;
  NMP::Vector3  m_normal;
  float         m_size;
  NMP::Colour   m_colour;

  // Endian swaps in place on buffer.
  static DebugDrawEnvironmentPatch* endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  static DebugDrawEnvironmentPatch* deserialiseTx(void* buffer);

  // Returns size of data written to buffer.
  static uint32_t serializeTx(
    const NMP::Vector3& position,
    const NMP::Vector3& normal,
    float               size,
    const NMP::Colour&  colour,
    void*               outputBuffer,
    uint32_t            outputBufferSize = 0);
};

} // MCOMMS

#include "comms/debugDrawObjects.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_DEBUG_DRAW_OBJECTS_H
//----------------------------------------------------------------------------------------------------------------------
