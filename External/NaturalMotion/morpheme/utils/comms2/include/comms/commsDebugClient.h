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
#ifndef MCOMMS_COMMSDEBUGCLIENT_H
#define MCOMMS_COMMSDEBUGCLIENT_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMColour.h"

#include "morpheme/mrDebugClient.h"

#include "comms/debugDrawObjects.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
class CommsServer;
class ConnectionManager;

//----------------------------------------------------------------------------------------------------------------------
// Implementation of MR::DebugClient that does no tessellation, but rather sends COMMS messages to a connection for each command.
//----------------------------------------------------------------------------------------------------------------------
class CommsDebugClient : public MR::DebugClient
{
public:
  CommsDebugClient(CommsServer* commsServer);
  virtual ~CommsDebugClient();

  /// -------------- Scratch Pad --------------
  virtual void logScratchpadInt(
    uint32_t       sourceInstanceID,        ///< Unique Character/Network instance identifier.
    MR::NodeID     sourceNodeID,            ///< INVALID_NODE_ID if its not from a specific node.
    const char*    sourceTagName,           ///< NULL if not from a specific module.
    MR::FrameCount sourceFrame,             ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    const char*    tag,                     ///< Arbitrary label, that receiving clients can filter on.
    int32_t        value,                   ///< Actual data value.
    const char*    variableName,            ///< Variable name where the value was stored.
    uint16_t       lineNumber) NM_OVERRIDE; ///< In file where request originated.
  virtual void logScratchpadFloat(
    uint32_t       sourceInstanceID,        ///< Unique Character/Network instance identifier.
    MR::NodeID     sourceNodeID,            ///< INVALID_NODE_ID if its not from a specific node.
    const char*    sourceTagName,           ///< NULL if not from a specific module.
    MR::FrameCount sourceFrame,             ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    const char*    tag,                     ///< Arbitrary label, that receiving clients can filter on.
    float          value,                   ///< Actual data value.
    const char*    variableName,            ///< Variable name where the value was stored.
    uint16_t       lineNumber) NM_OVERRIDE; ///< In file where request originated.
  virtual void logScratchpadBool(
    uint32_t       sourceInstanceID,        ///< Unique Character/Network instance identifier.
    MR::NodeID     sourceNodeID,            ///< INVALID_NODE_ID if its not from a specific node.
    const char*    sourceTagName,           ///< NULL if not from a specific module.
    MR::FrameCount sourceFrame,             ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    const char*    tag,                     ///< Arbitrary label, that receiving clients can filter on.
    bool           value,                   ///< Actual data value.
    const char*    variableName,            ///< Variable name where the value was stored.
    uint16_t       lineNumber) NM_OVERRIDE; ///< In file where request originated.
  virtual void logScratchpadVector3(
    uint32_t       sourceInstanceID,        ///< Unique Character/Network instance identifier.
    MR::NodeID     sourceNodeID,            ///< INVALID_NODE_ID if its not from a specific node.
    const char*    sourceTagName,           ///< NULL if not from a specific module.
    MR::FrameCount sourceFrame,             ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    const char*    tag,                     ///< Arbitrary label, that receiving clients can filter on.
    const NMP::Vector3& value,              ///< Actual data value.
    const char*    variableName,            ///< Variable name where the value was stored.
    uint16_t       lineNumber) NM_OVERRIDE; ///< In file where request originated.


  // -------------- Task Execution --------------
  /// Signify the beginning of the definition of a queued task.
  virtual void beginDispatcherTaskExecute(
    uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::FrameCount sourceFrame,      ///< What frame this data is from.
    MR::TaskID     taskID,
    const char*    taskName,
    MR::NodeID     owningNodeID) NM_OVERRIDE;
  virtual void addDispatcherTaskExecuteParameter(
    uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::FrameCount sourceFrame,      ///< What frame this data is from.
    MR::TaskID     taskID,
    const char*    semanticName,
    MR::NodeID     owningNodeID,
    MR::NodeID     targetNodeID,
    uint32_t       validFrame,
    bool           isInput,
    bool           isOutput,
    uint16_t       lifespan) NM_OVERRIDE;
  virtual void endDispatcherTaskExecute(
    uint32_t       sourceInstanceID,          ///< Unique Character/Network instance identifier.
    MR::FrameCount sourceFrame) NM_OVERRIDE;  ///< What frame this data is from.


  // -------------- State Machine Requests --------------
  virtual void logStateMachineMessageEventMsg(
    uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::FrameCount sourceFrame,      ///< What frame this data is from.
    MR::MessageID  messageID,        ///< ID of a request that has been sent to 1 or more state machines. INVALID_MESSAGE_ID if clearing all requests.
    MR::NodeID     targetSMNodeID,   ///< INVALID_NODE_ID means that this is a broadcast request that goes to all state machines.
    bool           set) NM_OVERRIDE; ///< true = setting the request on, false = setting the request off.


  // -------------- Profiling Points --------------
  /// \brief Log some profiling information.
  virtual void logProfilePointTiming(
    uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::FrameCount sourceFrame,      ///< What frame this data is from.
    const char    *name,
    float          timing) NM_OVERRIDE;


  // -------------- Debug Draw --------------
  virtual void drawPoint(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& point,
    float               radius,
    NMP::Colour         colour) NM_OVERRIDE;
  virtual void drawSphere(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Matrix34& tm,
    float               radius,
    NMP::Colour         colour) NM_OVERRIDE;
  // Draw a basic line from start to end.
  virtual void drawLine(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& start,
    const NMP::Vector3& end,
    NMP::Colour         colour) NM_OVERRIDE;
  virtual void drawPolyLine(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    uint32_t            numVertices,       ///< Min of 2 and numLines = numVetices - 1;
    const NMP::Vector3* vertices,          ///< Array of vertices between lines.
    NMP::Colour         colour) NM_OVERRIDE;
  virtual void drawVector(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    MR::VectorType      type,
    const NMP::Vector3& start,
    const NMP::Vector3& offset,
    NMP::Colour         colour) NM_OVERRIDE;
  virtual void drawArrowHead(
    uint32_t            sourceInstanceID,     ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,         ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,        ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,          ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,             ///< Position of arrow point (if not a delta).
    const NMP::Vector3& direction,            ///< Arrow direction.
    const NMP::Vector3& tangent,              ///< Generally at 90 degrees to the direction. Defines the width of the arrow.
    NMP::Colour         colour,
    bool                hasMass,              ///< If true draws solid arrows, else draw line arrows.
    bool                isDelta) NM_OVERRIDE; ///< Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
  virtual void drawTwistArc(
    uint32_t            sourceInstanceID,     ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,         ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,        ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,          ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,             ///< Center of arc.
    const NMP::Vector3& primaryDirection,     ///< Normal of the plane which the arc lies on (Not necessarily normalised).
    const NMP::Vector3& direction,            ///<
    const NMP::Vector3& direction2,           ///<
    NMP::Colour         colour,
    bool                doubleArrowHead,      ///< Draw an arrow at both ends of arc.
    bool                hasMass,              ///< Arrow heads are drawn as solid triangles.
    bool                isDelta) NM_OVERRIDE; ///< Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
  virtual void drawPlane(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,
    const NMP::Vector3& normal,
    float               halfSize,
    NMP::Colour         colour,
    float               normalScale) NM_OVERRIDE;
  virtual void drawTriangle(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& v1,                ///< 1-2-3 should form a clockwise face.
    const NMP::Vector3& v2,
    const NMP::Vector3& v3,
    NMP::Colour         colour) NM_OVERRIDE;
  virtual void drawMatrix(
    uint32_t             sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID           sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*          sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount       sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex        sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Matrix34& matrix,
    float                scale,
    uint8_t              alpha) NM_OVERRIDE;
  virtual void drawNonUniformMatrix(
    uint32_t             sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID           sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*          sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount       sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Matrix34& matrix,
    float                scale,
    uint8_t              alpha) NM_OVERRIDE;
  virtual void drawConeAndDial(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& pos,
    const NMP::Vector3& dir,
    float               angle,
    float               size,
    const NMP::Vector3& dialDirection,
    NMP::Colour         colour) NM_OVERRIDE;
  virtual void drawContactPointSimple(
    uint32_t            sourceInstanceID,       ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,           ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,          ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,            ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,        ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,               ///< Contact point.
    float               forceMagnitudeSquared) NM_OVERRIDE; ///< Force magnitude squared at contact point. 
  virtual void drawContactPointDetailed(
    uint32_t            sourceInstanceID,       ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,           ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,          ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,            ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,        ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,               ///< Contact point.
    const NMP::Vector3& normal,                 ///< Contact normal.
    const NMP::Vector3& force,                  ///< Force at contact point. 
    const NMP::Vector3& actor0Pos,              ///< Actor 0 root position.
    const NMP::Vector3& actor1Pos) NM_OVERRIDE; ///< Actor 1 root position.
  virtual void drawCharacterRoot(
    uint32_t             sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID           sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*          sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount       sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,    ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Matrix34& characterControllerRoot) NM_OVERRIDE;
  virtual void drawBox(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,
    const NMP::Vector3& radii,
    NMP::Colour         colour) NM_OVERRIDE;
  virtual void drawBBox(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Matrix34& tm,
    const NMP::Vector3& radii,
    NMP::Colour         colour) NM_OVERRIDE;
  virtual void drawSphereSweep(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,
    const NMP::Vector3& motion,
    float               radius,
    NMP::Colour         colour) NM_OVERRIDE;
  virtual void drawText(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,          ///< Bottom left hand corner of string.
    const char*         text,
    NMP::Colour         colour) NM_OVERRIDE;

  virtual void drawEnvironmentPatch(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,          ///< Position of patch center in world space.
    const NMP::Vector3& normal,
    const float         size,
    NMP::Colour         colour) NM_OVERRIDE;

protected:
  CommsServer*        m_commsServer;

  void* getTempBuffer(size_t size);

  void sendDebugDrawData(
    uint32_t            sourceInstanceID,
    MR::NodeID          sourceNodeID,
    const char*         sourceTagName,   
    MR::FrameCount      sourceFrame,
    MR::LimbIndex       sourceLimbIndex, 
    DebugDrawTypes      debugDrawType,
    void*               debugDrawData,
    uint32_t            debugDrawDataSize);

private:
  size_t  m_tempBufferSize;
  void*   m_tempBuffer;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_COMMSDEBUGCLIENT_H
//----------------------------------------------------------------------------------------------------------------------
