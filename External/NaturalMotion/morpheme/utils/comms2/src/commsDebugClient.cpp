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
#include "comms/commsDebugClient.h"
#include "comms/commsServer.h"

#include "sharedDefines/mCoreDebugInterface.h"

#include "comms/connection.h"
#include "comms/connectionManager.h"
#include "comms/debugPackets.h"
#include "comms/debugDrawObjects.h"
#include "comms/networkDataBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
// Send immediately.
template<typename T>
NM_INLINE void sendPacketToListeningConnections(CommsServer* commsServer, T& packet);

//----------------------------------------------------------------------------------------------------------------------
// Buffer to connections, and send when connection is ready.
template<typename T>
NM_INLINE void bufferPacketOnListeningConnections(CommsServer* commsServer, T& packet);

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
NM_INLINE void sendPacketToListeningConnections(CommsServer* commsServer, T& packet)
{
  for (uint32_t i = 0, num = commsServer->getNumConnections(); i < num; ++i)
  {
    mcommsSendDataPacket(packet, commsServer->getConnection(i));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
NM_INLINE void bufferPacketOnListeningConnections(CommsServer* commsServer, T& packet)
{
  for (uint32_t i = 0, num = commsServer->getNumConnections(); i < num; ++i)
  {
    mcommsBufferDataPacket(packet, commsServer->getConnection(i));
  }
}

//----------------------------------------------------------------------------------------------------------------------
CommsDebugClient::CommsDebugClient(CommsServer* commsServer)
: m_commsServer(commsServer),
  m_tempBufferSize(0),
  m_tempBuffer(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
CommsDebugClient::~CommsDebugClient()
{
  if (m_tempBuffer)
  {
    m_tempBufferSize = 0;

    NMP::Memory::memFree(m_tempBuffer);
    m_tempBuffer = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Scratch Pad
//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::logScratchpadInt(
  uint32_t       sourceInstanceID,
  MR::NodeID     sourceNodeID,
  const char*    sourceTagName,   
  MR::FrameCount sourceFrame,
  const char*    tag,
  int32_t        value,
  const char*    variableName,
  uint16_t       lineNumber)
{
  MCOMMS::MorphemeScratchpadPacket spp(sourceInstanceID, MCOMMS::ValueUnion::kInt);
  spp.m_data.m_value.i = value;
  spp.m_data.m_name = m_commsServer->getTokenForString(variableName);
  spp.m_tag = m_commsServer->getTokenForString(tag);
  spp.m_sourceLineNumber = lineNumber;
  spp.m_sourceNodeID = sourceNodeID;
  spp.m_sourceNetworkID = sourceInstanceID;
  spp.m_sourceModuleName = m_commsServer->getTokenForString(sourceTagName);
  spp.m_sourceFrame = sourceFrame;
  bufferPacketOnListeningConnections(m_commsServer, spp);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::logScratchpadFloat(
  uint32_t       sourceInstanceID,
  MR::NodeID     sourceNodeID,
  const char*    sourceTagName,   
  MR::FrameCount sourceFrame,
  const char*    tag,
  float          value,
  const char*    variableName,
  uint16_t       lineNumber)
{
  MCOMMS::MorphemeScratchpadPacket spp(sourceInstanceID, MCOMMS::ValueUnion::kFloat);
  spp.m_data.m_value.f = value;
  spp.m_data.m_name = m_commsServer->getTokenForString(variableName);
  if (tag)
  {
    spp.m_tag = m_commsServer->getTokenForString(tag);
  }
  spp.m_sourceLineNumber = lineNumber;
  spp.m_sourceNodeID = sourceNodeID;
  spp.m_sourceNetworkID = sourceInstanceID;
  spp.m_sourceModuleName = m_commsServer->getTokenForString(sourceTagName);
  spp.m_sourceFrame = sourceFrame;
  bufferPacketOnListeningConnections(m_commsServer, spp);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::logScratchpadBool(
  uint32_t       sourceInstanceID,
  MR::NodeID     sourceNodeID,
  const char*    sourceTagName,   
  MR::FrameCount sourceFrame,
  const char*    tag,
  bool           value,
  const char*    variableName,
  uint16_t       lineNumber)
{
  MCOMMS::MorphemeScratchpadPacket spp(sourceInstanceID, MCOMMS::ValueUnion::kBool);
  spp.m_data.m_value.b = value;
  spp.m_data.m_name = m_commsServer->getTokenForString(variableName);
  spp.m_tag = m_commsServer->getTokenForString(tag);
  spp.m_sourceLineNumber = lineNumber;
  spp.m_sourceNodeID = sourceNodeID;
  spp.m_sourceNetworkID = sourceInstanceID;
  spp.m_sourceModuleName = m_commsServer->getTokenForString(sourceTagName);
  spp.m_sourceFrame = sourceFrame;
  bufferPacketOnListeningConnections(m_commsServer, spp);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::logScratchpadVector3(
  uint32_t       sourceInstanceID,
  MR::NodeID     sourceNodeID,
  const char*    sourceTagName,   
  MR::FrameCount sourceFrame,
  const char*    tag,
  const NMP::Vector3& value,
  const char*    variableName,
  uint16_t       lineNumber)
{
  MCOMMS::MorphemeScratchpadPacket spp(sourceInstanceID, MCOMMS::ValueUnion::kVector3);
  spp.m_data.set(value.x, value.y, value.z);
  spp.m_data.m_name = m_commsServer->getTokenForString(variableName);
  spp.m_tag = m_commsServer->getTokenForString(tag);
  spp.m_sourceLineNumber = lineNumber;
  spp.m_sourceNodeID = sourceNodeID;
  spp.m_sourceNetworkID = sourceInstanceID;
  spp.m_sourceModuleName = m_commsServer->getTokenForString(sourceTagName);
  spp.m_sourceFrame = sourceFrame;
  bufferPacketOnListeningConnections(m_commsServer, spp);
}

//----------------------------------------------------------------------------------------------------------------------
// Task Execution
//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::beginDispatcherTaskExecute(
  uint32_t       sourceInstanceID,
  MR::FrameCount sourceFrame,
  MR::TaskID     taskID,
  const char*    taskName,
  MR::NodeID     owningNodeID)
{
  MCOMMS::BeginDispatcherTaskExecutePacket packet;
  packet.m_networkInstanceID = sourceInstanceID;
  packet.m_taskid = taskID;
  packet.m_taskNameTag = m_commsServer->getTokenForString(taskName);
  packet.m_owningNodeID = owningNodeID;
  packet.m_sourceFrame = sourceFrame;
  sendPacketToListeningConnections(m_commsServer, packet);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::addDispatcherTaskExecuteParameter(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::FrameCount NMP_UNUSED(sourceFrame),
  MR::TaskID     NMP_UNUSED(taskID),
  const char*    semanticName,
  MR::NodeID     owningNodeID,
  MR::NodeID     targetNodeID,
  uint32_t       validFrame,
  bool           isInput,
  bool           isOutput,
  uint16_t       lifespan)
{
  MCOMMS::AddDispatcherTaskExecuteParameterPacket packet;
  packet.m_semanticName = m_commsServer->getTokenForString(semanticName);
  packet.m_owningNodeID = owningNodeID;
  packet.m_targetNodeID = targetNodeID;
  packet.m_validFrame = validFrame;
  packet.m_isInput = isInput;
  packet.m_isOutput = isOutput;
  packet.m_lifespan = lifespan;
  sendPacketToListeningConnections(m_commsServer, packet);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::endDispatcherTaskExecute(
  uint32_t       NMP_UNUSED(sourceInstanceID),
  MR::FrameCount NMP_UNUSED(sourceFrame))
{
  MCOMMS::EndDispatcherTaskExecutePacket packet;
  sendPacketToListeningConnections(m_commsServer, packet);
}

//----------------------------------------------------------------------------------------------------------------------
// State Machine Messages
//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::logStateMachineMessageEventMsg(
  uint32_t       sourceInstanceID,
  MR::FrameCount NMP_UNUSED(sourceFrame),
  MR::MessageID  messageID, 
  MR::NodeID     targetSMNodeID,
  bool           set)
{
  MCOMMS::StateMachineMessageEventMsgPacket packet;
  packet.m_targetSMNodeID = targetSMNodeID;
  packet.m_messageID = messageID;
  packet.m_set = set;
  packet.m_networkInstanceID = sourceInstanceID;

  bufferPacketOnListeningConnections(m_commsServer, packet);
}

//----------------------------------------------------------------------------------------------------------------------
// Profiling Points
//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::logProfilePointTiming(
  uint32_t       sourceInstanceID,
  MR::FrameCount NMP_UNUSED(sourceFrame),
  const char*    name,
  float          timing)
{
  MCOMMS::ProfilePointTimingPacket packet;
  packet.m_name = m_commsServer->getTokenForString(name);
  packet.m_timing = timing;
  packet.m_networkInstanceID = sourceInstanceID;

  bufferPacketOnListeningConnections(m_commsServer, packet);
}

//----------------------------------------------------------------------------------------------------------------------
// Debug Draw
//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawPoint(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& point,
  float               radius,
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawPoint::serializeTx(point, radius, colour, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawPoint::serializeTx(point, radius, colour, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_POINT, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawSphere(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Matrix34& tm,
  float               radius,
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawSphere::serializeTx(tm, radius, colour, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawSphere::serializeTx(tm, radius, colour, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName, sourceFrame, sourceLimbIndex, DDTYPE_SPHERE, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawLine(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& start,
  const NMP::Vector3& end,
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawLine::serializeTx(start, end, colour, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawLine::serializeTx(start, end, colour, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_LINE, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawPolyLine(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  uint32_t            numVertices, // Min of 2 and numLines = numVetices - 1;
  const NMP::Vector3* vertices,   // Array of vertices between lines.
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawPolyLine::serializeTx(numVertices, vertices, colour, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawPolyLine::serializeTx(numVertices, vertices, colour, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_POLY_LINE, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawVector(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  MR::VectorType      type,
  const NMP::Vector3& start,
  const NMP::Vector3& offset,
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawVector::serializeTx(start, offset, colour, type, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawVector::serializeTx(start, offset, colour, type, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName, sourceFrame, sourceLimbIndex, DDTYPE_VECTOR, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawArrowHead(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& position,             // Position of arrow point (if not a delta).
  const NMP::Vector3& direction,            // Arrow direction.
  const NMP::Vector3& tangent,              // Generally at 90 degrees to the direction. Defines the width of the arrow.
  NMP::Colour         colour,
  bool                hasMass,              // If true draws solid arrows, else draw line arrows.
  bool                isDelta)              // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{
  uint32_t dataSize = DebugDrawArrowHead::serializeTx(position, direction, tangent, colour, hasMass, isDelta, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawArrowHead::serializeTx(position, direction, tangent, colour, hasMass, isDelta, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_ARROW_HEAD, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawTwistArc(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& position,                  // Center of arc.
  const NMP::Vector3& primaryDirection,           // Normal of the plane which the arc lies on (Not necessarily normalised).
  const NMP::Vector3& direction,                  //
  const NMP::Vector3& direction2,                 //
  NMP::Colour         colour,
  bool                doubleArrowHead,      // Draw an arrow at both ends of arc.
  bool                hasMass,              // Arrow heads are drawn as solid triangles.
  bool                isDelta)              // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{
  uint32_t dataSize = DebugDrawTwistArc::serializeTx(
    position,
    primaryDirection,
    direction,
    direction2,
    colour,
    doubleArrowHead,
    hasMass,
    isDelta,
    NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawTwistArc::serializeTx(
      position,
      primaryDirection,
      direction,
      direction2,
      colour,
      doubleArrowHead,
      hasMass,
      isDelta,
      data,
      dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_TWIST_ARC, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawPlane(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  float               halfSize,
  NMP::Colour         colour,
  float               normalScale)
{
  uint32_t dataSize = DebugDrawPlane::serializeTx(position, normal, halfSize, colour, normalScale, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawPlane::serializeTx(position, normal, halfSize, colour, normalScale, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_PLANE, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawTriangle(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& vertex0,     // 1-2-3 should form a clockwise face.
  const NMP::Vector3& vertex1,
  const NMP::Vector3& vertex2,
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawTriangle::serializeTx(vertex0, vertex1, vertex2, colour, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawTriangle::serializeTx(vertex0, vertex1, vertex2, colour, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_TRIANGLE, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawMatrix(
  uint32_t             sourceInstanceID,
  MR::NodeID           sourceNodeID,
  const char*          sourceTagName,   
  MR::FrameCount       sourceFrame,
  MR::LimbIndex        sourceLimbIndex,
  const NMP::Matrix34& matrix,
  float                scale,
  uint8_t              alpha)
{
  uint32_t dataSize = DebugDrawMatrix::serializeTx(matrix, scale, alpha, DebugDrawMatrix::MT_UNIFORM_MATRIX, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawMatrix::serializeTx(matrix, scale, alpha, DebugDrawMatrix::MT_UNIFORM_MATRIX, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_MATRIX, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawNonUniformMatrix(
  uint32_t             sourceInstanceID,
  MR::NodeID           sourceNodeID,
  const char*          sourceTagName,   
  MR::FrameCount       sourceFrame,
  MR::LimbIndex        sourceLimbIndex,
  const NMP::Matrix34& matrix,
  float                scale,
  uint8_t              alpha)
{
  uint32_t dataSize = DebugDrawMatrix::serializeTx(matrix, scale, alpha, DebugDrawMatrix::MT_NONUNIFORM_MATRIX, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawMatrix::serializeTx(matrix, scale, alpha, DebugDrawMatrix::MT_UNIFORM_MATRIX, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_MATRIX, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawConeAndDial(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& position,
  const NMP::Vector3& direction,
  float               angle,
  float               size,
  const NMP::Vector3& dialDirection,
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawConeAndDial::serializeTx(position, direction, dialDirection, angle, size, colour, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawConeAndDial::serializeTx(position, direction, dialDirection, angle, size, colour, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_CONE_AND_DIAL, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawContactPointSimple(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& position,              // Contact point.
  float               forceMagnitudeSquared) // Force magnitude squared at contact point. 
{
  uint32_t dataSize = DebugDrawContactPoint::serializeTx(position, forceMagnitudeSquared, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawContactPoint::serializeTx(position, forceMagnitudeSquared, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_CONTACT_POINT, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawContactPointDetailed(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& position,       // Contact point.
  const NMP::Vector3& normal,         // Contact normal.
  const NMP::Vector3& force,          // Force at contact point. 
  const NMP::Vector3& actor0Position, // Actor 0 root position.
  const NMP::Vector3& actor1Position) // Actor 1 root position.
{
  uint32_t dataSize = DebugDrawContactPointDetailed::serializeTx(position, normal, force, actor0Position, actor1Position, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawContactPointDetailed::serializeTx(position, normal, force, actor0Position, actor1Position, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_CONTACT_POINT_DETAILED, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawCharacterRoot(
  uint32_t             sourceInstanceID,
  MR::NodeID           sourceNodeID,
  const char*          sourceTagName,   
  MR::FrameCount       sourceFrame,
  MR::LimbIndex        sourceLimbIndex,
  const NMP::Matrix34& characterControllerRoot)
{
  uint32_t dataSize = DebugDrawCharacterControllerRoot::serializeTx(characterControllerRoot, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawCharacterControllerRoot::serializeTx(characterControllerRoot, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_CHARACTER_ROOT, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawBox(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& position,
  const NMP::Vector3& halfSizes,
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawBox::serializeTx(position, halfSizes, colour, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawBox::serializeTx(position, halfSizes, colour, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_BOX, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawBBox(
                      uint32_t            sourceInstanceID,
                      MR::NodeID          sourceNodeID,
                      const char*         sourceTagName,
                      MR::FrameCount      sourceFrame,
                      MR::LimbIndex       sourceLimbIndex,
                      const NMP::Matrix34& tm,
                      const NMP::Vector3& halfSizes,
                      NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawBBox::serializeTx(tm, halfSizes, colour, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawBBox::serializeTx(tm, halfSizes, colour, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_BBOX, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawSphereSweep(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& position,
  const NMP::Vector3& motion,
  float               radius,
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawSphereSweep::serializeTx(position, motion, radius, colour, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawSphereSweep::serializeTx(position, motion, radius, colour, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex,DDTYPE_SPHERE_SWEEP, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawText(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& position,            // Bottom left hand corner of string.
  const char*         text,
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawText::serializeTx(position, colour, text, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawText::serializeTx(position, colour, text, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(sourceInstanceID, sourceNodeID, sourceTagName, sourceFrame, sourceLimbIndex, DDTYPE_TEXT, data, dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::drawEnvironmentPatch(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  float               radius,
  NMP::Colour         colour)
{
  uint32_t dataSize = DebugDrawEnvironmentPatch::serializeTx(position, normal, radius, colour, NULL);
  NMP_ASSERT(dataSize > 0);

  void* data = getTempBuffer(dataSize);
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t usedSize =
#endif
    DebugDrawEnvironmentPatch::serializeTx(position, normal, radius, colour, data, dataSize);
  NMP_ASSERT(usedSize <= dataSize);

  sendDebugDrawData(
    sourceInstanceID,
    sourceNodeID,
    sourceTagName,
    sourceFrame,
    sourceLimbIndex,
    DDTYPE_ENVIRONMENT_PATCH,
    data,
    dataSize);
}

//----------------------------------------------------------------------------------------------------------------------
void* CommsDebugClient::getTempBuffer(size_t size)
{
  if (size > m_tempBufferSize)
  {
    if (m_tempBuffer)
    {
      NMP::Memory::memFree(m_tempBuffer);
    }
    m_tempBuffer = NMPMemoryAlloc(size);
    NMP_ASSERT(m_tempBuffer);
  }
  return m_tempBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
void CommsDebugClient::sendDebugDrawData(
  uint32_t            sourceInstanceID,
  MR::NodeID          sourceNodeID,
  const char*         sourceTagName,   
  MR::FrameCount      sourceFrame,
  MR::LimbIndex       sourceLimbIndex,
  DebugDrawTypes      debugDrawType,
  void*               debugDrawData,
  uint32_t            debugDrawDataSize)
{
  for (uint32_t i = 0, numConnections = m_commsServer->getNumConnections(); i != numConnections; ++i)
  {
    Connection* connection = m_commsServer->getConnection(i);

    NetworkDataBuffer* networkDataBuffer = connection->getDataBuffer();
    DebugDrawPacket* pkt = DebugDrawPacket::create(
      networkDataBuffer,
      debugDrawType,
      sourceInstanceID,
      sourceNodeID,
      m_commsServer->getTokenForString(sourceTagName),
      sourceFrame,
      sourceLimbIndex,
      debugDrawDataSize);
    void* data = pkt->getData();

    NMP::Memory::memcpy(data, debugDrawData, debugDrawDataSize);

    mcommsSerializeDataPacket(*pkt);
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
