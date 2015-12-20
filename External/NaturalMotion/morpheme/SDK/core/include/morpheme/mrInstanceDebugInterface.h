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
#ifndef MR_INSTANCE_DEBUG_INTERFACE_H
#define MR_INSTANCE_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMFlags.h"
#include "NMPlatform/NMColour.h"
#include "sharedDefines/mSharedDefines.h"
#include "sharedDefines/mCoreDebugInterface.h"
#include "sharedDefines/mDebugDrawTessellator.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mrInstanceDebugInterface.h
/// \brief Used by morpheme Network/Character instances to output debug information.
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class DebugManager;
class Network;

//----------------------------------------------------------------------------------------------------------------------
/// Used by morpheme Network/Character instances to record debug information.
///
/// + Each Network/Character has its own instance of this class.
/// + Stores debugging status info for the Network/Character that it is associated with; Including:
///   . What filtering of debug info is set for the Character.
///   . Profiling information.
///   etc.
/// + All debug output commands are redirected to the singleton instance of MR:DebugManager which then distributes
///   them to registered clients.
//----------------------------------------------------------------------------------------------------------------------
class InstanceDebugInterface
{
public:
  InstanceDebugInterface();
  ~InstanceDebugInterface();
   
  void init(
    uint32_t          instanceID,    ///< Unique instance identifier for the Character/Network that we are attached to.
    MR::Network*      network,       ///< The network that that we are providing a debug interface for.
    MR::DebugManager* debugManager); ///< The central singleton debug manager.
    
  /// Deallocates storage. Must be called before a destructor is called.
  void term();

  /// Allocates storage for arrays of module details.
  void initModuleNames(
    uint32_t     numModules,    // Number of module names.
    char* const* moduleNames,   // Table of module names.
    char* const* parentNames);  // Table of module parent names.

  /// Allocates storage for arrays of control details.
  void initControlNames(
    uint32_t     numControls,    // Number of control names.
    const char** controlNames);  // Table of control names.

  // -------------- DebugOuput Filtering --------------
  /// Debug outputs are defined in mCoreDebugInterface.h and should be combinations of MR::eDebugOutputFlags.
  void debugOutputsAllOff();                    ///< Turns off all debug output.
  void debugOutputsAllOn();                     ///< Turns on all debug output.
  void debugOutputsTurnOn(uint32_t flags);      ///< Turn on specified and leave others in their current state.
  bool debugOutputsAreOn(uint32_t flags) const; ///< All all the specified flags on.
  void debugOutputsTurnOff(uint32_t flags);     ///< Turn off specified and leave others in their current state.
  void debugOutputsOnlyTurnOn(uint32_t flags);  ///< Any existing flags are cleared.


  // -------------- Module Filtering --------------
  bool isModuleDebugEnabled(const char* moduleName) const;
  bool isModuleDebugEnabled(uint32_t moduleIndex) const; ///< Index in to MyNetwork::m_allModules.
  void enableModuleDebug(const char* moduleName, bool bEnable = true);
  void enableModuleDebug(uint32_t moduleIndex, bool bEnable = true); ///< Index in to MyNetwork::m_allModules.

  /// Indicates if debug draw is enabled based on the current module in scope
  NM_INLINE bool isDrawEnabled();

  /// Pushing and popping module updates from the stack.
  enum ScopeType
  {
    kModule,
    kControl
  };

  bool enterScopeUpdate(ScopeType scopeType, const char* scopeName, MR::LimbIndex limbIndex); ///< Returns whether the entered scope is debug enabled or not.
  bool enterScopeUpdate(ScopeType scopeType, uint32_t scopeIndex, MR::LimbIndex limbIndex); ///< Returns whether the scope module is debug enabled or not.
  void exitScopeUpdate();
  
  /// There may be more than one scope (module or control) currently updating in a stack, this function returns the scope at the top of the stack
  const char* getUpdatingScopeName();
  const char* getUpdatingModuleName();

  //
  MR::LimbIndex getUpdatingLimbIndex();

  /// Returns the total number of modules.
  uint32_t totalModules() const;
  /// Returns cm_moduleNotFound if not found.
  uint32_t findModuleIndex(const char* moduleName) const;
  /// Returns the index of the module on the top of the module stack.
  uint32_t getCurrentModuleIndex() const;
  /// Returns the name of a module.
  const char* getModuleName(uint32_t moduleIndex) const;
  /// Returns the parent of a module.
  const char* getModuleParentName(uint32_t moduleIndex) const;

  /// Returns the total number of controls.
  uint32_t totalControls() const;
  /// Returns cm_controlNotFound if not found.
  uint32_t findControlIndex(const char* controlName) const;
  /// Returns the index of the module on the top of the module stack.
  uint32_t getCurrentControlIndex() const;
  /// Returns the name of a module.
  const char* getControlName(uint32_t controlIndex) const;

  /// -------------- Scratch Pad --------------
  /// Send a tagged basic value type.
  /// \var nodeID       The ID of the Node that is originating this data. If the data is not specific to a node use INVALID_NODE_ID.
  /// \var tag          An arbitrary string tag that will be displayed in connect.
  /// \var value        The value to display.
  /// \var variableName The string name of the variable being displayed.
  /// \var m_lineNumber The code line number where this output originates from.
  void logScratchPadInt(MR::NodeID nodeID, const char* tag, int32_t value, const char* variableName, uint16_t lineNumber);
  void logScratchPadFloat(MR::NodeID nodeID, const char* tag, float value, const char* variableName, uint16_t lineNumber);
  void logScratchPadBool(MR::NodeID nodeID, const char* tag, bool value, const char* variableName, uint16_t lineNumber);
  void logScratchPadVector3(MR::NodeID nodeID, const char* tag, const NMP::Vector3& value, const char* variableName, uint16_t lineNumber);


  // -------------- Task Execution --------------
  /// Signify the beginning of the definition of a queued task.
  void beginDispatcherTaskExecute(
    MR::TaskID  taskID,
    const char* taskName,
    MR::NodeID  owningNodeID);
  void addDispatcherTaskExecuteParameter(
    MR::TaskID  taskID,
    const char* semanticName,
    MR::NodeID  owningNodeID,
    MR::NodeID  targetNodeID,
    uint32_t    validFrame,
    bool        isInput,
    bool        isOutput,
    uint16_t    lifespan);
  void endDispatcherTaskExecute();
  

  // -------------- State Machine Requests --------------
  void logStateMachineMessageEventMsg(
    MR::MessageID messageID,      ///< ID of a request that has been sent to 1 or more state machines. INVALID_REQUEST_ID if clearing all requests.
    MR::NodeID    targetSMNodeID, ///< INVALID_NODE_ID means that this is a broadcast request that goes to all state machines.
    bool          set);           ///< true = setting the request on, false = setting the request off.


  // -------------- Profiling Points --------------
  /// \brief Log some profiling information.
  void logProfilePointTiming(
    const char* name,    ///< Profile point name.
    float       timing); ///< Timing value/

  // -------------- Node Filtering --------------
  /// These are called automatically by the dispatcher. Only one node can be updated at a time so there is no stack for node ids.
  /// Allows draw calls without the need for passing the node id.
  /// \brief Sets the new node id returning the old node id
  MR::NodeID setCurrentNodeID(MR::NodeID nodeID);
  MR::NodeID getCurrentNodeID() const;

  // -------------- Debug Draw --------------
  void drawPoint(
    const NMP::Vector3& position,
    float               radius,
    NMP::Colour         colour);
  void drawSphere(
    const NMP::Matrix34& tm,
    float               radius,
    NMP::Colour         colour);
  // Draw a basic line from start to end.
  void drawLine(
    const NMP::Vector3& start,
    const NMP::Vector3& end,
    NMP::Colour         colour);
  void drawPolyLine(
    uint32_t            numVertices,      ///< Min of 2 and numLines = numVetices - 1;
    const NMP::Vector3* vertices,         ///< Array of vertices between lines.
    NMP::Colour         colour);
  // Draw a line with various line endings (arrows etc, depending on type) from start to start + offset.
  void drawVector(
    VectorType          type,
    const NMP::Vector3& position,
    const NMP::Vector3& offset,
    NMP::Colour         colour);
  void drawArrowHead(
    const NMP::Vector3& position,         ///< Position of arrow point (if not a delta).
    const NMP::Vector3& direction,        ///< Arrow direction.
    const NMP::Vector3& tangent,          ///< Generally at 90 degrees to the direction. Defines the width of the arrow.
    NMP::Colour         colour,
    bool                hasMass,          ///< If true draws solid arrows, else draw line arrows.
    bool                isDelta);         ///< Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
  void drawTwistArc(
    const NMP::Vector3& position,         ///< Center of arc.
    const NMP::Vector3& primaryDir,       ///< Normal of the plane which the arc lies on (Not necessarily normalised).
    const NMP::Vector3& dir,              ///<
    const NMP::Vector3& dir2,             ///<
    NMP::Colour         colour,
    bool                doubleArrowHead,  ///< Draw an arrow at both ends of arc.
    bool                hasMass,          ///< Arrow heads are drawn as solid triangles.
    bool                isDelta);         ///< Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
  void drawPlane(
    const NMP::Vector3& position,
    const NMP::Vector3& normal,
    float               radius,
    NMP::Colour         colour,
    float               normalScale);
  void drawTriangle(
    const NMP::Vector3& v1,               ///< 1-2-3 should form a clockwise face.
    const NMP::Vector3& v2,
    const NMP::Vector3& v3,
    NMP::Colour         colour);
  void drawMatrix(
    const NMP::Matrix34& matrix,
    float                scale,
    uint8_t              alpha);
  void drawNonUniformMatrix(
    const NMP::Matrix34& matrix,
    float                scale,
    uint8_t              alpha);
  void drawConeAndDial(
    const NMP::Vector3& position,
    const NMP::Vector3& direction,
    float               angle,
    float               size,
    const NMP::Vector3& dialDirection,
    NMP::Colour         colour);
  void drawContactPointSimple(
    const NMP::Vector3& position,                  ///< Contact point.
    float               forceMagnitudeSquared); ///< Force magnitude squared at contact point.
  void drawContactPointDetailed(
    const NMP::Vector3& position,            ///< Contact point.
    const NMP::Vector3& normal,           ///< Contact normal.
    const NMP::Vector3& force,            ///< Force at contact point.
    const NMP::Vector3& actor0Pos,        ///< Actor 0 root position.
    const NMP::Vector3& actor1Pos);       ///< Actor 1 root position.
  void drawCharacterRoot(
    const NMP::Matrix34& characterControllerRoot);
  void drawBox(
    const NMP::Vector3& position,
    const NMP::Vector3& halfSizes,
    NMP::Colour         colour);
  void drawBBox(
    const NMP::Matrix34& tm,
    const NMP::Vector3& halfSizes,
    NMP::Colour         colour);
  void drawSphereSweep(
    const NMP::Vector3 &position,
    const NMP::Vector3 &motion,
    float               radius,
    NMP::Colour         colour);
  void drawText(
    const NMP::Vector3 &position,
    const char*         text,
    NMP::Colour         colour);
  void drawEnvironmentPatch(
    const NMP::Vector3& position,           ///< Position of patch center in world space.
    const NMP::Vector3& normal,             ///< Orientation of square.
    const float         size,
    NMP::Colour         colour);


  // -------------- Node Timing --------------
  /// \brief Add a node timing element to the current array of timings.
  bool addNodeTiming(MR::NodeID nodeID, MR::NodeTaskTimingType type, float time);

  uint32_t findNodeTimingIndex(MR::NodeID nodeID, MR::NodeTaskTimingType type);
  void clearNodeTimings();
  uint32_t getNumNodeTimings() const;
  const NodeTimingElement* getNodeTimings() const;

protected:
  /// Reference to the central singleton debug manager. All debug requests are forwarded to this central manager,
  ///  the manager then forwards this to a set of registered clients which can process the information however they want.
  ///  For example:
  ///  + CommsDebugClient forwards debug requests as comms packets to connect (or another receiver).
  ///  + A GameDebugClient could respond to requests locally. For example debugDraw could be dealt with by the game renderer straight away.
  MR::DebugManager*     m_DebugManager;

  /// Character/Network instance identifier. Indicates which character instance this InstanceDebugInterface instance is associated with.
  ///  This is a MCOMMS::InstanceID when used with runtime target NetworkInstanceRecords,
  ///  but it could also be a game specific system of character instance numbering.
  uint32_t              m_instanceID;

  /// The network that that we are providing a debug interface for.
  MR::Network*          m_network;

  /// Current status of debug filtering on this Network.
  NMP::Flags            m_debugOutputsFlags;

  /// Array of node profiling elements that is built up over a frame of Network evaluation.
  /// The contents of this array is transmitted to connect on frame completion.
  static const uint32_t m_maxNumNodeTimings = 1024;
  uint32_t              m_numNodeTimings;    ///< Current count.
  NodeTimingElement     m_framesNodeTimings[m_maxNumNodeTimings];

  /// Array of all modules. This is in the order as MyNetwork::m_allModules.
  uint32_t              m_numModules;
  char**                m_moduleNamesTable;     ///< Table for lookup of a module index via the module name.
  char**                m_moduleParentsTable;   ///< Table for lookup of a parent via its index.
  bool*                 m_modulesDebugEnabled;  ///< Stores which modules have debug output enabled.
  static const uint32_t cm_moduleNotFound = 0xFFFFFFFF;

  /// Array of all modules. This is in the order as MyNetwork::m_allModules.
  uint32_t              m_numControls;
  char**                m_controlNamesTable;     ///< Table for lookup of a module index via the module name.
  static const uint32_t cm_controlNotFound = 0xFFFFFFFF;

  static const uint32_t cm_maxUpdatingModulesStackSize = 32; 
  uint32_t              m_updatingScopeStackSize;
  uint32_t              m_updatingScopeStack[cm_maxUpdatingModulesStackSize];
  uint32_t              m_updatingScopeTypesStack[cm_maxUpdatingModulesStackSize];
  MR::LimbIndex         m_updatingLimbIndexStack[cm_maxUpdatingModulesStackSize];

  /// Stored while sending the full description of a queued task.
  MR::TaskID            m_currentDispatcherTask;

  MR::NodeID            m_currentNodeID;
};


//----------------------------------------------------------------------------------------------------------------------
// Utility that automatically handles popping modules off the debug stack when exiting a 
// function where it is declared on the stack.
//----------------------------------------------------------------------------------------------------------------------
class ScopeEntryExit
{
public:
  MR::InstanceDebugInterface* m_debugInterface;

  NM_INLINE ~ScopeEntryExit();

  NM_INLINE bool enterScope(MR::InstanceDebugInterface* debugInterface, MR::InstanceDebugInterface::ScopeType scopeType, const char *name, MR::LimbIndex limbIndex = MR::INVALID_LIMB_INDEX);
  NM_INLINE bool enterScope(MR::InstanceDebugInterface* debugInterface, MR::InstanceDebugInterface::ScopeType scopeType, uint32_t scopeIndex, MR::LimbIndex limbIndex = MR::INVALID_LIMB_INDEX);
};

//----------------------------------------------------------------------------------------------------------------------
// InstanceDebugInterface inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void InstanceDebugInterface::debugOutputsAllOff()
{
  m_debugOutputsFlags.clearAll();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void InstanceDebugInterface::debugOutputsAllOn()
{
  m_debugOutputsFlags.setAll();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void InstanceDebugInterface::debugOutputsTurnOn(uint32_t flags)
{
  m_debugOutputsFlags.set(flags);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool InstanceDebugInterface::debugOutputsAreOn(uint32_t flags) const
{
  return m_debugOutputsFlags.areSet(flags);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void InstanceDebugInterface::debugOutputsTurnOff(uint32_t flags)
{
  m_debugOutputsFlags.clear(flags);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void InstanceDebugInterface::debugOutputsOnlyTurnOn(uint32_t flags)
{
  m_debugOutputsFlags.clearThenSet(flags);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t InstanceDebugInterface::getCurrentModuleIndex() const
{
  uint32_t currentModuleIndex = cm_moduleNotFound;
  if (m_updatingScopeStackSize > 0)
  {
    currentModuleIndex = m_updatingScopeStack[m_updatingScopeStackSize - 1];
  }
  return currentModuleIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::NodeID InstanceDebugInterface::setCurrentNodeID(MR::NodeID nodeID)
{
  MR::NodeID previousNodeID = m_currentNodeID;
  m_currentNodeID = nodeID;
  return previousNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::NodeID InstanceDebugInterface::getCurrentNodeID() const
{
  return m_currentNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void InstanceDebugInterface::clearNodeTimings()
{
  m_numNodeTimings = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t InstanceDebugInterface::getNumNodeTimings() const
{
  return m_numNodeTimings;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NodeTimingElement* InstanceDebugInterface::getNodeTimings() const
{
  return m_framesNodeTimings;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool InstanceDebugInterface::isDrawEnabled()
{
  if (debugOutputsAreOn(DEBUG_OUTPUT_DEBUG_DRAW))
  {
    if (m_updatingScopeStackSize > 0)
    {
      uint32_t currentScope = m_updatingScopeTypesStack[m_updatingScopeStackSize - 1];
      uint32_t currentModuleIndex = m_updatingScopeStack[m_updatingScopeStackSize - 1];
      return currentScope == kControl || isModuleDebugEnabled(currentModuleIndex);
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// ScopeEntryExit
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE ScopeEntryExit::~ScopeEntryExit()
{
  if (m_debugInterface)
  {
    m_debugInterface->exitScopeUpdate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool ScopeEntryExit::enterScope(InstanceDebugInterface *debugInterface, InstanceDebugInterface::ScopeType scope,
                                          const char *name, MR::LimbIndex limbIndex)
{
  m_debugInterface = debugInterface;

  if (m_debugInterface)
  {
    return m_debugInterface->enterScopeUpdate(scope, name, limbIndex);
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool ScopeEntryExit::enterScope(InstanceDebugInterface *debugInterface, InstanceDebugInterface::ScopeType scope,
                                          uint32_t scopeIndex, MR::LimbIndex limbIndex)
{
  m_debugInterface = debugInterface;

  if (m_debugInterface)
  {
    return m_debugInterface->enterScopeUpdate(scope, scopeIndex, limbIndex);
  }

  return false;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_INSTANCE_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
