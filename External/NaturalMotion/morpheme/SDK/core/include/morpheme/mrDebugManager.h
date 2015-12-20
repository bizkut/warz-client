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
# pragma once
#endif
#ifndef MR_DEBUG_MANAGER_H
#define MR_DEBUG_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMColour.h"
#include "sharedDefines/mSharedDefines.h"
#include "sharedDefines/mDebugDrawTessellator.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mrDebugManager.h
/// \brief Central singleton debug output manager. Distributes debug commands to a set of registered DebugClients.
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

// Debug data whose sorce frame is unknown.
static const uint32_t UNKNOWN_FRAME = 0xFFFFFFFF;

class DebugClient;

//----------------------------------------------------------------------------------------------------------------------
/// Central singleton debug output manager. All debug requests are forwarded to this central manager,
///  the manager then forwards this to a set of registered clients which can process the information however they want.
///  For example:
///  + CommsDebugClient forwards debug requests as comms packets to connect (or another receiver).
///  + A GameDebugClient could respond to requests locally. For example debugDraw could be dealt with by the game 
///     renderer straight away.
//----------------------------------------------------------------------------------------------------------------------
class DebugManager
{
public:
  /// Get the singleton instance of this class.
  static DebugManager* getInstance();

  DebugManager();
  ~DebugManager();

  void addDebugClient(DebugClient* client);
  bool removeDebugClient(DebugClient* client);
  void clearDebugClients();

  // -------------- Scratch Pad --------------
  void logScratchPadInt(
    uint32_t       sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID     sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*    sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    const char*    tag,               ///< Arbitrary label, that receiving clients can filter on.
    int32_t        value,             ///< Actual data value.
    const char*    variableName,      ///< Variable name where the value was stored.
    uint16_t       lineNumber);       ///< In file where request originated.
  void logScratchPadFloat(
    uint32_t       sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID     sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*    sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    const char*    tag,               ///< Arbitrary label, that receiving clients can filter on.
    float          value,             ///< Actual data value.
    const char*    variableName,      ///< Variable name where the value was stored.
    uint16_t       lineNumber);       ///< In file where request originated.
  void logScratchPadBool(
    uint32_t       sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID     sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*    sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    const char*    tag,               ///< Arbitrary label, that receiving clients can filter on.
    bool           value,             ///< Actual data value.
    const char*    variableName,      ///< Variable name where the value was stored.
    uint16_t       lineNumber);       ///< In file where request originated.
  void logScratchPadVector3(
    uint32_t       sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID     sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*    sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    const char*    tag,               ///< Arbitrary label, that receiving clients can filter on.
    const NMP::Vector3& value,        ///< Actual data value.
    const char*    variableName,      ///< Variable name where the value was stored.
    uint16_t       lineNumber);       ///< In file where request originated.


  // -------------- Task Execution --------------
  /// Signify the beginning of the definition of a queued task.
  void beginDispatcherTaskExecute(
    uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::FrameCount sourceFrame,      ///< What frame this data is from.
    MR::TaskID     taskID,
    const char*    taskName,
    MR::NodeID     owningNodeID);
  void addDispatcherTaskExecuteParameter(
    uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::FrameCount sourceFrame,      ///< What frame this data is from.
    MR::TaskID     taskID,
    const char*    semanticName,
    MR::NodeID     owningNodeID,
    MR::NodeID     targetNodeID,
    uint32_t       validFrame,
    bool           isInput,
    bool           isOutput,
    uint16_t       lifespan);
  void endDispatcherTaskExecute(
    uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::FrameCount sourceFrame);     ///< What frame this data is from.


  // -------------- State Machine Requests --------------
  void logStateMachineMessageEventMsg(
    uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::FrameCount sourceFrame,      ///< What frame this data is from.
    MR::MessageID  messageID,        ///< ID of a request that has been sent to 1 or more state machines. INVALID_REQUEST_ID if clearing all requests.
    MR::NodeID     targetSMNodeID,   ///< INVALID_NODE_ID means that this is a broadcast request that goes to all state machines.
    bool           set);             ///< true = setting the request on, false = setting the request off.


  // -------------- Profiling Points --------------
  /// \brief Log some profiling information.
  void logProfilePointTiming(
    uint32_t       sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::FrameCount sourceFrame,      ///< What frame this data is from.
    const char    *name,
    float          timing);


  // -------------- Debug Draw --------------
  void clearDebugDraw();

  void drawPoint(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,
    float               radius,
    NMP::Colour         colour);
  void drawSphere(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Matrix34& tm,
    float               radius,
    NMP::Colour         colour);
  // Draw a basic line from start to end.
  void drawLine(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& start,
    const NMP::Vector3& end,
    NMP::Colour         colour);
  void drawPolyLine(
    uint32_t            sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,     ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,    ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,      ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,  ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    uint32_t            numVertices,      ///< Min of 2 and numLines = numVetices - 1;
    const NMP::Vector3* vertices,         ///< Array of vertices between lines.
    NMP::Colour         colour);
  // Draw a line with various line endings (arrows etc, depending on type) from start to start + offset.
  void drawVector(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    VectorType          type,
    const NMP::Vector3& position,
    const NMP::Vector3& offset,
    NMP::Colour         colour);
  void drawArrowHead(
    uint32_t            sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,     ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,    ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,      ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,         ///< Position of arrow point (if not a delta).
    const NMP::Vector3& direction,        ///< Arrow direction.
    const NMP::Vector3& tangent,          ///< Generally at 90 degrees to the direction. Defines the width of the arrow.
    NMP::Colour         colour,
    bool                hasMass,          ///< If true draws solid arrows, else draw line arrows.
    bool                isDelta);         ///< Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
  void drawTwistArc(
    uint32_t            sourceInstanceID, ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,     ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,    ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,      ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,         ///< Center of arc.
    const NMP::Vector3& primaryDir,       ///< Normal of the plane which the arc lies on (Not necessarily normalised).
    const NMP::Vector3& dir,              ///<
    const NMP::Vector3& dir2,             ///<
    NMP::Colour         colour,
    bool                doubleArrowHead,  ///< Draw an arrow at both ends of arc.
    bool                hasMass,          ///< Arrow heads are drawn as solid triangles.
    bool                isDelta);         ///< Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
  void drawPlane(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,
    const NMP::Vector3& normal,
    float               radius,
    NMP::Colour         colour,
    float               normalScale);
  void drawTriangle(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& v1,     // 1-2-3 should form a clockwise face.
    const NMP::Vector3& v2,
    const NMP::Vector3& v3,
    NMP::Colour         colour);
  void drawMatrix(
    uint32_t             sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID           sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*          sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount       sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex        sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Matrix34& matrix,
    float                scale,
    uint8_t              alpha);
  void drawNonUniformMatrix(
    uint32_t             sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID           sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*          sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount       sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex        sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Matrix34& matrix,
    float                scale,
    uint8_t              alpha);
  void drawConeAndDial(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,
    const NMP::Vector3& direction,
    float               angle,
    float               size,
    const NMP::Vector3& dialDirection,
    NMP::Colour         colour);
  void drawContactPointSimple(
    uint32_t            sourceInstanceID,       ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,           ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,          ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,            ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,        ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,               ///< Contact point.
    float               forceMagnitudeSquared); ///< Force magnitude squared at contact point. 
  void drawContactPointDetailed(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,          ///< Contact point.
    const NMP::Vector3& normal,            ///< Contact normal.
    const NMP::Vector3& force,             ///< Force at contact point. 
    const NMP::Vector3& actor0Pos,         ///< Actor 0 root position.
    const NMP::Vector3& actor1Pos);        ///< Actor 1 root position.
  void drawCharacterRoot(
    uint32_t             sourceInstanceID,        ///< Unique Character/Network instance identifier.
    MR::NodeID           sourceNodeID,            ///< INVALID_NODE_ID if its not from a specific node.
    const char*          sourceTagName,           ///< NULL if not from a specific module.
    MR::FrameCount       sourceFrame,             ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex        sourceLimbIndex,         ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
   const NMP::Matrix34& characterControllerRoot);
  void drawBox(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,
    const NMP::Vector3& halfSizes,
    NMP::Colour         colour);
  void drawBBox(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Matrix34& tm,
    const NMP::Vector3& halfSizes,
    NMP::Colour         colour);

  void drawSphereSweep(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3 &position,
    const NMP::Vector3 &motion,
    float               radius,
    NMP::Colour         colour);
  void drawText(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,          ///< Bottom left hand corner of string.
    const char*         text,
    NMP::Colour         colour);
  void drawEnvironmentPatch(
    uint32_t            sourceInstanceID,  ///< Unique Character/Network instance identifier.
    MR::NodeID          sourceNodeID,      ///< INVALID_NODE_ID if its not from a specific node.
    const char*         sourceTagName,     ///< NULL if not from a specific module.
    MR::FrameCount      sourceFrame,       ///< What frame this data is from. VALID_FRAME_ANY_FRAME if not specific to a frame.
    MR::LimbIndex       sourceLimbIndex,   ///< The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    const NMP::Vector3& position,          ///< Position of patch center in world space.
    const NMP::Vector3& normal,
    const float         radius,
    NMP::Colour         colour);
  
protected:
  static const uint32_t m_maxNumClients = 8;
  uint32_t              m_numClients;
  DebugClient*          m_clients[m_maxNumClients];

  // Single instance; use getInstance().
  static DebugManager   sm_instance;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_DEBUG_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
