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
#include "morpheme/mrDebugManager.h"
#include "morpheme/mrDebugClient.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

// Static singleton instance.
DebugManager DebugManager::sm_instance;

//----------------------------------------------------------------------------------------------------------------------
DebugManager* DebugManager::getInstance()
{
  return &sm_instance;
}

//----------------------------------------------------------------------------------------------------------------------
DebugManager::DebugManager()
{
  m_numClients = 0;
}

//----------------------------------------------------------------------------------------------------------------------
DebugManager::~DebugManager()
{
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::addDebugClient(DebugClient* client)
{
  NMP_ASSERT(m_numClients < m_maxNumClients);
  m_clients[m_numClients] = client;
  ++m_numClients;
}

//----------------------------------------------------------------------------------------------------------------------
bool DebugManager::removeDebugClient(DebugClient* client)
{
  bool found = false;
  for (uint32_t i = 0; i != m_numClients; ++i)
  {
    if (found)
    {
      // make sure any clients after the removed one are shifted down an index so they are all still contiguous.
      m_clients[i - 1] = m_clients[i];
      m_clients[i] = 0;
    }
    else if (client == m_clients[i])
    {
      found = true;
      m_clients[i] = 0;
    }
  }

  if (found)
  {
    --m_numClients;
  }

  return found;
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::clearDebugClients()
{
  m_numClients = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Scratch Pad
//----------------------------------------------------------------------------------------------------------------------
void DebugManager::logScratchPadInt(
  uint32_t       sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID     sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*    sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  const char*    tag,               // Arbitrary label, that receiving clients can filter on.
  int32_t        value,             // Actual data value.
  const char*    variableName,      // Variable name where the value was stored.
  uint16_t       lineNumber)        // In file where request originated.
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->logScratchpadInt(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      tag,
      value,
      variableName,
      lineNumber);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::logScratchPadFloat(
  uint32_t       sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID     sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*    sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  const char*    tag,               // Arbitrary label, that receiving clients can filter on.
  float          value,             // Actual data value.
  const char*    variableName,      // Variable name where the value was stored.
  uint16_t       lineNumber)        // In file where request originated.
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->logScratchpadFloat(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      tag,
      value,
      variableName,
      lineNumber);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::logScratchPadBool(
  uint32_t       sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID     sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*    sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  const char*    tag,               // Arbitrary label, that receiving clients can filter on.
  bool           value,             // Actual data value.
  const char*    variableName,      // Variable name where the value was stored.
  uint16_t       lineNumber)        // In file where request originated.
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->logScratchpadBool(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      tag,
      value,
      variableName,
      lineNumber);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::logScratchPadVector3(
  uint32_t       sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID     sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*    sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  const char*    tag,               // Arbitrary label, that receiving clients can filter on.
  const NMP::Vector3& value,        // Actual data value.
  const char*    variableName,      // Variable name where the value was stored.
  uint16_t       lineNumber)        // In file where request originated.
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->logScratchpadVector3(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      tag,
      value,
      variableName,
      lineNumber);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Task Execution
//----------------------------------------------------------------------------------------------------------------------
void DebugManager::beginDispatcherTaskExecute(
  uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
  MR::FrameCount sourceFrame,      ///< What frame this data is from.
  MR::TaskID     taskID,
  const char*    taskName,
  MR::NodeID     owningNodeID)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->beginDispatcherTaskExecute(
      sourceInstanceID,
      sourceFrame,     
      taskID,
      taskName,
      owningNodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::addDispatcherTaskExecuteParameter(
  uint32_t       sourceInstanceID, // Unique Character/Network instance identifier.
  MR::FrameCount sourceFrame,      // What frame this data is from.
  MR::TaskID     taskID,
  const char*    semanticName,
  MR::NodeID     owningNodeID,
  MR::NodeID     targetNodeID,
  uint32_t       validFrame,
  bool           isInput,
  bool           isOutput,
  uint16_t       lifespan)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->addDispatcherTaskExecuteParameter(
      sourceInstanceID,
      sourceFrame,     
      taskID,
      semanticName,
      owningNodeID,
      targetNodeID,
      validFrame,
      isInput,
      isOutput,
      lifespan);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::endDispatcherTaskExecute(
  uint32_t       sourceInstanceID, // Unique Character/Network instance identifier.
  MR::FrameCount sourceFrame)      // What frame this data is from.
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->endDispatcherTaskExecute(sourceInstanceID, sourceFrame);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// State Machine Requests
//----------------------------------------------------------------------------------------------------------------------
void DebugManager::logStateMachineMessageEventMsg(
  uint32_t       sourceInstanceID, // Unique Character/Network instance identifier.
  MR::FrameCount sourceFrame,      // What frame this data is from.
  MR::MessageID  messageID,        // ID of a request that has been sent to 1 or more state machines. INVALID_REQUEST_ID if clearing all requests.
  MR::NodeID     targetSMNodeID,   // INVALID_NODE_ID means that this is a broadcast request that goes to all state machines.
  bool           set)              // true = setting the request on, false = setting the request off.
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->logStateMachineMessageEventMsg(
      sourceInstanceID,
      sourceFrame,
      messageID,
      targetSMNodeID,
      set);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Profiling Points
//----------------------------------------------------------------------------------------------------------------------
void DebugManager::logProfilePointTiming(
  uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
  MR::FrameCount sourceFrame,      ///< What frame this data is from.
  const char*    name,
  float          timing)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->logProfilePointTiming(sourceInstanceID, sourceFrame, name, timing);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Debug Draw
//----------------------------------------------------------------------------------------------------------------------
void DebugManager::clearDebugDraw()
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->clearDebugDraw();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawPoint(
  uint32_t            sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,
  float               radius,
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawPoint(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex, position, radius, colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawSphere(
  uint32_t            sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Matrix34& tm,
  float               radius,
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawSphere(sourceInstanceID, sourceNodeID, sourceTagName, sourceFrame, sourceLimbIndex, tm, radius, colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawLine(
  uint32_t            sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& start,
  const NMP::Vector3& end,
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawLine(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex, start, end, colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawPolyLine(
  uint32_t            sourceInstanceID, // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,     // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,    // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,      // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,  // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  uint32_t            numVertices,      // Min of 2 and numLines = numVetices - 1;
  const NMP::Vector3* vertices,         // Array of vertices between lines.
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawPolyLine(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex, numVertices, vertices, colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawVector(
  uint32_t            sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  VectorType          type,
  const NMP::Vector3& position,
  const NMP::Vector3& offset,
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawVector(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      type,
      position,
      offset,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawArrowHead(
  uint32_t            sourceInstanceID, // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,     // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,    // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,      // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,         // Position of arrow point (if not a delta).
  const NMP::Vector3& direction,        // Arrow direction.
  const NMP::Vector3& tangent,          // Generally at 90 degrees to the direction. Defines the width of the arrow.
  NMP::Colour         colour,
  bool                hasMass,          // If true draws solid arrows, else draw line arrows.
  bool                isDelta)          // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawArrowHead(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      position, 
      direction,
      tangent,  
      colour,
      hasMass,  
      isDelta);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawTwistArc(
  uint32_t            sourceInstanceID, // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,     // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,    // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,      // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,         // Center of arc.
  const NMP::Vector3& primaryDir,       // Normal of the plane which the arc lies on (Not necessarily normalised).
  const NMP::Vector3& dir,              //
  const NMP::Vector3& dir2,             //
  NMP::Colour         colour,
  bool                doubleArrowHead,  // Draw an arrow at both ends of arc.
  bool                hasMass,          // Arrow heads are drawn as solid triangles.
  bool                isDelta)          // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawTwistArc(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      position,            
      primaryDir,     
      dir,            
      dir2,           
      colour,
      doubleArrowHead,
      hasMass,        
      isDelta);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawPlane(
  uint32_t            sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  float               radius,
  NMP::Colour         colour,
  float               normalScale)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawPlane(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      position,
      normal,
      radius,
      colour,
      normalScale);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawTriangle(
  uint32_t            sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& v1,                // 1-2-3 should form a clockwise face.
  const NMP::Vector3& v2,
  const NMP::Vector3& v3,
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawTriangle(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex, v1, v2, v3, colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawMatrix(
  uint32_t             sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID           sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*          sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount       sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Matrix34& matrix,
  float                scale,
  uint8_t              alpha)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawMatrix(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      matrix,
      scale,
      alpha);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawNonUniformMatrix(
  uint32_t             sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID           sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*          sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount       sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Matrix34& matrix,
  float                scale,
  uint8_t              alpha)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawNonUniformMatrix(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      matrix,
      scale,
      alpha);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawConeAndDial(
  uint32_t            sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,
  const NMP::Vector3& direction,
  float               angle,
  float               size,
  const NMP::Vector3& dialDirection,
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawConeAndDial(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      position,
      direction,
      angle,
      size,
      dialDirection,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawContactPointSimple(
  uint32_t            sourceInstanceID,       // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,           // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,          // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,            // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,               // Contact point.
  float               forceMagnitudeSquared)  // Force magnitude squared at contact point.
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawContactPointSimple(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      position,                
      forceMagnitudeSquared);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawContactPointDetailed(
  uint32_t            sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,          // Contact point.
  const NMP::Vector3& normal,            // Contact normal.
  const NMP::Vector3& force,             // Force at contact point. 
  const NMP::Vector3& actor0Pos,         // Actor 0 root position.
  const NMP::Vector3& actor1Pos)         // Actor 1 root position.
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawContactPointDetailed(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      position,    
      normal,   
      force,    
      actor0Pos,
      actor1Pos);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawCharacterRoot(
  uint32_t             sourceInstanceID,        // Unique Character/Network instance identifier.
  MR::NodeID           sourceNodeID,            // INVALID_NODE_ID if its not from a specific node.
  const char*          sourceTagName,           // NULL if not from a specific module.
  MR::FrameCount       sourceFrame,             // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Matrix34& characterControllerRoot)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawCharacterRoot(sourceInstanceID, sourceNodeID, sourceTagName,    sourceFrame, sourceLimbIndex, characterControllerRoot);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawBox(
  uint32_t            sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,
  const NMP::Vector3& halfSizes,
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawBox(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      position,
      halfSizes,
      colour);
  }
}

void DebugManager::drawBBox(
              uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
              MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
              const char*         sourceTagName,     ///< NULL if not from a specific module.
              MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
              MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
              const NMP::Matrix34& tm,
              const NMP::Vector3& halfSizes,
              NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawBBox(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      tm,
      halfSizes,
      colour);
  }
}


//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawSphereSweep(
  uint32_t            sourceInstanceID,  // Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      // INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     // NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       // What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,
  const NMP::Vector3& motion,
  float               radius,
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawSphereSweep(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      position,
      motion,
      radius,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawEnvironmentPatch(
  uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     ///< NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,          ///< Position of patch center in world space.
  const NMP::Vector3& normal,
  const float         radius,
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawEnvironmentPatch(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      position,
      normal,
      radius,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DebugManager::drawText(
  uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
  MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
  const char*         sourceTagName,     ///< NULL if not from a specific module.
  MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
  MR::LimbIndex       sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
  const NMP::Vector3& position,          ///< Bottom left hand corner of string.
  const char*         text,
  NMP::Colour         colour)
{
  for (uint32_t i = 0; i < m_numClients; ++i)
  {
    m_clients[i]->drawText(
      sourceInstanceID,
      sourceNodeID,
      sourceTagName,   
      sourceFrame,
      sourceLimbIndex,
      position,
      text,
      colour);
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
