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
#ifndef MCOMMS_DEBUG_DRAW_OBJECTS_INL
#define MCOMMS_DEBUG_DRAW_OBJECTS_INL
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
// DebugDrawPoint
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawPoint* DebugDrawPoint::endianSwap(void *buffer)
{
  DebugDrawPoint *data = (DebugDrawPoint *) buffer;
  NMP::netEndianSwap(data->m_point);
  NMP::netEndianSwap(data->m_radius);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawPoint* DebugDrawPoint::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawPoint::serializeTx(
  const NMP::Vector3& point,
  float               radius,
  NMP::Colour         colour,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawPoint);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawPoint* output = (DebugDrawPoint*) outputBuffer;
    output->m_point = point;
    output->m_radius = radius;
    output->m_colour = colour;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawSphere
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawSphere* DebugDrawSphere::endianSwap(void *buffer)
{
  DebugDrawSphere *data = (DebugDrawSphere *) buffer;
  NMP::netEndianSwap(data->m_tm);
  NMP::netEndianSwap(data->m_radius);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawSphere* DebugDrawSphere::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawSphere::serializeTx(
  const NMP::Matrix34& tm,
  float               radius,
  NMP::Colour         colour,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawSphere);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawSphere* output = (DebugDrawSphere*) outputBuffer;
    output->m_tm = tm;
    output->m_radius = radius;
    output->m_colour = colour;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawLine
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawLine* DebugDrawLine::endianSwap(void *buffer)
{
  DebugDrawLine *data = (DebugDrawLine *) buffer;
  NMP::netEndianSwap(data->m_start);
  NMP::netEndianSwap(data->m_end);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawLine* DebugDrawLine::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawLine::serializeTx(
  const NMP::Vector3& start,
  const NMP::Vector3& end,
  NMP::Colour         colour,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawLine);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawLine* output = (DebugDrawLine*) outputBuffer;
    output->m_start = start;
    output->m_end = end;
    output->m_colour = colour;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawPolyLine
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawPolyLine* DebugDrawPolyLine::deserialiseTx(void *buffer)
{
  DebugDrawPolyLine* data = (DebugDrawPolyLine*) buffer;

  NMP::netEndianSwap(data->m_numVertices);
  NMP::netEndianSwap(data->m_colour);

  NMP::Vector3* vertices = (NMP::Vector3*)data + 1;
  for (uint32_t i = 0; i != data->m_numVertices; ++i)
  {
    NMP::netEndianSwap(vertices[i]);
  }

  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawPolyLine::serializeTx(
  uint32_t            numVertices,
  const NMP::Vector3* vertices,
  NMP::Colour         colour,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawPolyLine);
  dataSize += numVertices * sizeof(NMP::Vector3);

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawPolyLine* output = (DebugDrawPolyLine*) outputBuffer;
    output->m_numVertices = numVertices;
    output->m_colour = colour;

    NMP::Vector3* outputVertices = (NMP::Vector3*)output + 1;
    for (uint32_t i = 0; i != numVertices; ++i)
    {
      outputVertices[i] = vertices[i];
      NMP::netEndianSwap(outputVertices[i]);
    }

    NMP::netEndianSwap(output->m_numVertices);
    NMP::netEndianSwap(output->m_colour);
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::Vector3& DebugDrawPolyLine::getVertex(uint32_t i) const
{
  NMP_ASSERT(i < m_numVertices);
  const NMP::Vector3* vertices = (NMP::Vector3*)this + 1;
  return vertices[i];
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawVector
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawVector* DebugDrawVector::endianSwap(void *buffer)
{
  DebugDrawVector *data = (DebugDrawVector *) buffer;
  NMP::netEndianSwap(data->m_start);
  NMP::netEndianSwap(data->m_offset);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawVector* DebugDrawVector::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawVector::serializeTx(
  const NMP::Vector3& start,
  const NMP::Vector3& offset,
  NMP::Colour         colour,
  MR::VectorType      vectorType,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawVector);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawVector* output = (DebugDrawVector*)outputBuffer;
    output->m_start = start;
    output->m_offset = offset;
    output->m_colour = colour;
    output->m_vectorType = vectorType;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawArrowHead
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawArrowHead* DebugDrawArrowHead::endianSwap(void *buffer)
{
  DebugDrawArrowHead *data = (DebugDrawArrowHead *) buffer;
  NMP::netEndianSwap(data->m_position);
  NMP::netEndianSwap(data->m_direction);
  NMP::netEndianSwap(data->m_tangent);
  NMP::netEndianSwap(data->m_colour);
  NMP::netEndianSwap(data->m_hasMass);
  NMP::netEndianSwap(data->m_isDelta);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawArrowHead* DebugDrawArrowHead::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawArrowHead::serializeTx(
  const NMP::Vector3& position,
  const NMP::Vector3& direction,
  const NMP::Vector3& tangent,
  NMP::Colour         colour,
  bool                hasMass,
  bool                isDelta,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawArrowHead);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawArrowHead* output = (DebugDrawArrowHead*)outputBuffer;
    output->m_position = position;
    output->m_direction = direction;
    output->m_tangent = tangent;
    output->m_colour = colour;
    output->m_hasMass = hasMass;
    output->m_isDelta = isDelta;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawTwistArc
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawTwistArc* DebugDrawTwistArc::endianSwap(void *buffer)
{
  DebugDrawTwistArc *data = (DebugDrawTwistArc *) buffer;
  NMP::netEndianSwap(data->m_position);
  NMP::netEndianSwap(data->m_primaryDirection);
  NMP::netEndianSwap(data->m_direction);
  NMP::netEndianSwap(data->m_direction2);
  NMP::netEndianSwap(data->m_colour);
  NMP::netEndianSwap(data->m_doubleArrowHead);
  NMP::netEndianSwap(data->m_hasMass);
  NMP::netEndianSwap(data->m_isDelta);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawTwistArc* DebugDrawTwistArc::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawTwistArc::serializeTx(
  const NMP::Vector3& position,
  const NMP::Vector3& primaryDirection,
  const NMP::Vector3& direction,
  const NMP::Vector3& direction2,
  NMP::Colour         colour,
  bool                doubleArrowHead,
  bool                hasMass,
  bool                isDelta,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawTwistArc);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawTwistArc* output = (DebugDrawTwistArc*)outputBuffer;
    output->m_position = position;
    output->m_primaryDirection = primaryDirection;
    output->m_direction = direction;
    output->m_direction2 = direction2;
    output->m_colour = colour;
    output->m_doubleArrowHead = doubleArrowHead;
    output->m_hasMass = hasMass;
    output->m_isDelta = isDelta;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawPlane
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawPlane* DebugDrawPlane::endianSwap(void *buffer)
{
  DebugDrawPlane *data = (DebugDrawPlane *) buffer;
  NMP::netEndianSwap(data->m_position);
  NMP::netEndianSwap(data->m_normal);
  NMP::netEndianSwap(data->m_halfSize);
  NMP::netEndianSwap(data->m_colour);
  NMP::netEndianSwap(data->m_normalScale);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawPlane* DebugDrawPlane::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawPlane::serializeTx(
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  float               halfSize,
  NMP::Colour         colour,
  float               normalScale,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawPlane);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawPlane* output = (DebugDrawPlane*)outputBuffer;
    output->m_position = position;
    output->m_normal = normal;
    output->m_halfSize = halfSize;
    output->m_colour = colour;
    output->m_normalScale = normalScale;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawTriangle
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawTriangle* DebugDrawTriangle::endianSwap(void *buffer)
{
  DebugDrawTriangle *data = (DebugDrawTriangle *) buffer;
  NMP::netEndianSwap(data->m_vertices[0]);
  NMP::netEndianSwap(data->m_vertices[1]);
  NMP::netEndianSwap(data->m_vertices[2]);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawTriangle* DebugDrawTriangle::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawTriangle::serializeTx(
  const NMP::Vector3& vertex0,
  const NMP::Vector3& vertex1,
  const NMP::Vector3& vertex2,
  NMP::Colour         colour,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawTriangle);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawTriangle* output = (DebugDrawTriangle*)outputBuffer;
    output->m_vertices[0] = vertex0;
    output->m_vertices[1] = vertex1;
    output->m_vertices[2] = vertex2;
    output->m_colour = colour;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawMatrix
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawMatrix* DebugDrawMatrix::endianSwap(void *buffer)
{
  DebugDrawMatrix *data = (DebugDrawMatrix *) buffer;
  NMP::netEndianSwap(data->m_matrix);
  NMP::netEndianSwap(data->m_scale);
  NMP::netEndianSwap(data->m_alpha);
  NMP::netEndianSwap(data->m_matrixType);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawMatrix* DebugDrawMatrix::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawMatrix::serializeTx(
  const NMP::Matrix34&  matrix,
  float                 scale,
  float                 alpha,
  MatrixTypes           matrixType,
  void*                 outputBuffer,
  uint32_t              NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawMatrix);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawMatrix* output = (DebugDrawMatrix*)outputBuffer;
    output->m_matrix = matrix;
    output->m_scale = scale;
    output->m_alpha = alpha;
    output->m_matrixType = matrixType;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawConeAndDial
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawConeAndDial* DebugDrawConeAndDial::endianSwap(void *buffer)
{
  DebugDrawConeAndDial *data = (DebugDrawConeAndDial *) buffer;
  NMP::netEndianSwap(data->m_position);
  NMP::netEndianSwap(data->m_direction);
  NMP::netEndianSwap(data->m_dialDirection);
  NMP::netEndianSwap(data->m_angle);
  NMP::netEndianSwap(data->m_size);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawConeAndDial* DebugDrawConeAndDial::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawConeAndDial::serializeTx(
  const NMP::Vector3& position,
  const NMP::Vector3& direction,
  const NMP::Vector3& dialDirection,
  float               angle,
  float               size,
  NMP::Colour         colour,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawConeAndDial);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawConeAndDial* output = (DebugDrawConeAndDial*)outputBuffer;
    output->m_position = position;
    output->m_direction = direction;
    output->m_dialDirection = dialDirection;
    output->m_angle = angle;
    output->m_size = size;
    output->m_colour = colour,
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawContactPoint
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawContactPoint* DebugDrawContactPoint::endianSwap(void *buffer)
{
  DebugDrawContactPoint *data = (DebugDrawContactPoint *) buffer;
  NMP::netEndianSwap(data->m_position);
  NMP::netEndianSwap(data->m_forceMagnitudeSquared);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawContactPoint* DebugDrawContactPoint::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawContactPoint::serializeTx(
  const NMP::Vector3& position,
  float               forceMagnitudeSquared,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawContactPoint);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawContactPoint* output = (DebugDrawContactPoint*)outputBuffer;
    output->m_position = position;
    output->m_forceMagnitudeSquared = forceMagnitudeSquared;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawContactPointDetailed
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawContactPointDetailed* DebugDrawContactPointDetailed::endianSwap(void *buffer)
{
  DebugDrawContactPointDetailed *data = (DebugDrawContactPointDetailed *) buffer;
  NMP::netEndianSwap(data->m_position);
  NMP::netEndianSwap(data->m_normal);
  NMP::netEndianSwap(data->m_force);
  NMP::netEndianSwap(data->m_actor0Position);
  NMP::netEndianSwap(data->m_actor1Position);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawContactPointDetailed* DebugDrawContactPointDetailed::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawContactPointDetailed::serializeTx(
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  const NMP::Vector3& force,
  const NMP::Vector3& actor0Position,
  const NMP::Vector3& actor1Position,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawContactPointDetailed);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawContactPointDetailed* output = (DebugDrawContactPointDetailed*)outputBuffer;
    output->m_position = position;
    output->m_normal = normal;
    output->m_force = force;
    output->m_actor0Position = actor0Position;
    output->m_actor1Position = actor1Position;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawCharacterControllerRoot
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawCharacterControllerRoot* DebugDrawCharacterControllerRoot::endianSwap(void *buffer)
{
  DebugDrawCharacterControllerRoot *data = (DebugDrawCharacterControllerRoot *) buffer;
  NMP::netEndianSwap(data->m_controllerRoot);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawCharacterControllerRoot* DebugDrawCharacterControllerRoot::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawCharacterControllerRoot::serializeTx(
  const NMP::Matrix34&  controllerRoot,
  void*                 outputBuffer,
  uint32_t              NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawCharacterControllerRoot);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawCharacterControllerRoot* output = (DebugDrawCharacterControllerRoot*)outputBuffer;
    output->m_controllerRoot = controllerRoot;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawBox
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawBox* DebugDrawBox::endianSwap(void *buffer)
{
  DebugDrawBox *data = (DebugDrawBox *) buffer;
  NMP::netEndianSwap(data->m_position);
  NMP::netEndianSwap(data->m_halfSizes);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawBox* DebugDrawBox::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawBox::serializeTx(
  const NMP::Vector3& position,
  const NMP::Vector3& halfSizes,
  const NMP::Colour&  colour,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawBox);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawBox* output = (DebugDrawBox*)outputBuffer;
    output->m_position = position;
    output->m_halfSizes = halfSizes;
    output->m_colour = colour;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawBBox
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawBBox* DebugDrawBBox::endianSwap(void *buffer)
{
  DebugDrawBBox *data = (DebugDrawBBox *) buffer;
  NMP::netEndianSwap(data->m_tm);
  NMP::netEndianSwap(data->m_halfSizes);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawBBox* DebugDrawBBox::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawBBox::serializeTx(
  const NMP::Matrix34& tm,
  const NMP::Vector3& halfSizes,
  const NMP::Colour&  colour,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawBBox);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawBBox* output = (DebugDrawBBox*)outputBuffer;
    output->m_tm = tm;
    output->m_halfSizes = halfSizes;
    output->m_colour = colour;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}



//----------------------------------------------------------------------------------------------------------------------
// DebugDrawSphereSweep
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawSphereSweep* DebugDrawSphereSweep::endianSwap(void *buffer)
{
  DebugDrawSphereSweep *data = (DebugDrawSphereSweep *) buffer;
  NMP::netEndianSwap(data->m_position);
  NMP::netEndianSwap(data->m_direction);
  NMP::netEndianSwap(data->m_radius);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawSphereSweep* DebugDrawSphereSweep::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawSphereSweep::serializeTx(
  const NMP::Vector3& position,
  const NMP::Vector3& direction,
  float               radius,
  const NMP::Colour&  colour,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawSphereSweep);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawSphereSweep* output = (DebugDrawSphereSweep*)outputBuffer;
    output->m_position = position;
    output->m_direction = direction;
    output->m_radius = radius;
    output->m_colour = colour;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawText
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawText* DebugDrawText::deserialiseTx(void *buffer)
{
  DebugDrawText *data = (DebugDrawText *) buffer;
  NMP::netEndianSwap(data->m_position);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawText::serializeTx(
  const NMP::Vector3& position,
  const NMP::Colour&  colour,
  const char*         text,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{

  size_t textSize = strlen(text) + 1;
  size_t dataSize = sizeof(DebugDrawText) + NMP::Memory::align(textSize, 16);

  if (outputBuffer)
  {
    NMP_ASSERT((size_t)outputBufferSize >= dataSize);
    DebugDrawText* output = (DebugDrawText*)outputBuffer;
    output->m_position = position;
    NMP::netEndianSwap(output->m_position);
    output->m_colour = colour;
    NMP::netEndianSwap(output->m_colour);
    output->m_text = (char*)(output + 1);
    memcpy(output->m_text, text, textSize);
  }
  return (uint32_t)dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
// DebugDrawEnvironmentPatch
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawEnvironmentPatch* DebugDrawEnvironmentPatch::endianSwap(void *buffer)
{
  DebugDrawEnvironmentPatch* const data = static_cast< DebugDrawEnvironmentPatch* >(buffer);
  NMP::netEndianSwap(data->m_position);
  NMP::netEndianSwap(data->m_normal);
  NMP::netEndianSwap(data->m_size);
  NMP::netEndianSwap(data->m_colour);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE DebugDrawEnvironmentPatch* DebugDrawEnvironmentPatch::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t DebugDrawEnvironmentPatch::serializeTx(
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  float               size,
  const NMP::Colour&  colour,
  void*               outputBuffer,
  uint32_t            NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(DebugDrawEnvironmentPatch);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    DebugDrawEnvironmentPatch* const output = static_cast< DebugDrawEnvironmentPatch* >(outputBuffer);
    output->m_position = position;
    output->m_normal = normal;
    output->m_size = size;
    output->m_colour = colour;
    endianSwap(outputBuffer); 
  }
  return dataSize;
}

} // MCOMMS

#endif // MCOMMS_DEBUG_DRAW_OBJECTS_INL
