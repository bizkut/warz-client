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
#include "morpheme/mrInstanceDebugInterface.h"
#include "morpheme/mrDebugManager.h"
#include "morpheme/mrNetwork.h"
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
InstanceDebugInterface::InstanceDebugInterface() :
    m_DebugManager(NULL),
    m_numNodeTimings(0),
    m_numModules(0),
    m_moduleNamesTable(NULL),
    m_moduleParentsTable(NULL),
    m_modulesDebugEnabled(NULL),
    m_numControls(0),
    m_controlNamesTable(NULL),
    m_updatingScopeStackSize(0),
    m_currentNodeID(MR::INVALID_NODE_ID)
{
  m_debugOutputsFlags.clearAll();
  m_currentDispatcherTask = MR::TASK_ID_UNSPECIFIED;
}

//----------------------------------------------------------------------------------------------------------------------
InstanceDebugInterface::~InstanceDebugInterface()
{
  NMP_ASSERT(!m_numModules && !m_moduleNamesTable && !m_moduleParentsTable && !m_modulesDebugEnabled && !m_DebugManager);
  NMP_ASSERT(!m_numControls && !m_controlNamesTable);
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::init(
  uint32_t          instanceID,    // Unique instance identifier for the Character/Network that we are attached to.
  MR::Network*      network,       // The network that that we are providing a debug interface for.
  MR::DebugManager* debugManager)  // The central singleton debug manager.
{
  NMP_ASSERT(debugManager);
  m_DebugManager = debugManager;

  m_instanceID = instanceID;
  m_network = network;

  m_currentDispatcherTask = MR::TASK_ID_UNSPECIFIED;

  m_numModules = 0;
  m_updatingScopeStackSize = 0;
  m_moduleNamesTable = NULL;
  m_moduleParentsTable = NULL;
  m_modulesDebugEnabled = NULL;
  m_controlNamesTable = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::initModuleNames(
  uint32_t     numModules,    // Number of module names.
  char* const* moduleNames,   // Table of module names.
  char* const* parentNames)   // Table of module parent names.
{
  m_numModules = numModules;

  NMP::Memory::Format memReqs;

  // Reserve space for array of name pointers.
  memReqs.size += (numModules * sizeof(char*)); // m_moduleNamesTable
  memReqs.size += (numModules * sizeof(char*)); // m_moduleParentsTable

  // Reserve space for the string data itself.
  for (uint32_t i = 0; i < numModules; i++)
  {
    memReqs.size += (NMP_STRLEN(moduleNames[i]) + 1) * sizeof(char);  // m_moduleNamesTable
    memReqs.size += (NMP_STRLEN(parentNames[i]) + 1) * sizeof(char);  // m_moduleParentsTable
  }

  // Reserve space for flags indicating if module is enabled.
  memReqs.size += (numModules * sizeof(bool));

  // Allocate storage.
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(memReqs);

  // Set up array of module name pointers.
  m_moduleNamesTable = (char**) resource.ptr;
  resource.increment(numModules * sizeof(char*));

  // Copy the module names.
  for (uint32_t i = 0; i < m_numModules; i++)
  {
    m_moduleNamesTable[i] = (char*) resource.ptr;
    resource.increment((NMP_STRLEN(moduleNames[i]) + 1) * sizeof(char));
    NMP_STRNCPY_S(m_moduleNamesTable[i], (NMP_STRLEN(moduleNames[i]) + 1), moduleNames[i]);
  }

  // Set up array of module name pointers.
  m_moduleParentsTable = (char**) resource.ptr;
  resource.increment(numModules * sizeof(char*));

  // Copy the module names.
  for (uint32_t i = 0; i < m_numModules; i++)
  {
    m_moduleParentsTable[i] = (char*) resource.ptr;
    resource.increment((NMP_STRLEN(parentNames[i]) + 1) * sizeof(char));
    NMP_STRNCPY_S(m_moduleParentsTable[i], (NMP_STRLEN(parentNames[i]) + 1), parentNames[i]);
  }

  // Set up array of enabled flags.
  m_modulesDebugEnabled = (bool*) resource.ptr;
  resource.increment(numModules * sizeof(bool));
  for (uint32_t i = 0; i < m_numModules; i++)
  {
    m_modulesDebugEnabled[i] = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::initControlNames(
  uint32_t     numControls,    // Number of control names.
  const char** controlNames)   // Table of control names.
{
  NMP_ASSERT(!m_numControls && !m_controlNamesTable);
  m_numControls = numControls;

  NMP::Memory::Format memReqs;

  // Reserve space for array of name pointers.
  memReqs.size += (numControls * sizeof(char*)); // m_controlNamesTable

  // Reserve space for the string data itself.
  for (uint32_t i = 0; i < numControls; i++)
  {
    memReqs.size += (NMP_STRLEN(controlNames[i]) + 1) * sizeof(char);  // m_controlNamesTable
  }

  // Allocate storage.
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(memReqs);

  // Set up array of control name pointers.
  m_controlNamesTable = (char**) resource.ptr;
  resource.increment(numControls * sizeof(char*));

  // Copy the control names.
  for (uint32_t i = 0; i < m_numControls; i++)
  {
    m_controlNamesTable[i] = (char*) resource.ptr;
    resource.increment((NMP_STRLEN(controlNames[i]) + 1) * sizeof(char));
    NMP_STRNCPY_S(m_controlNamesTable[i], (NMP_STRLEN(controlNames[i]) + 1), controlNames[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::term()
{
  if (m_DebugManager)
  {
    m_DebugManager = NULL;
    m_updatingScopeStackSize = 0;
    m_numModules = 0;
    if(m_moduleNamesTable)
    {
      NMP::Memory::memFree(m_moduleNamesTable); // This also points to the start of the allocated memory
    }
    m_moduleNamesTable = NULL;
    m_moduleParentsTable = NULL;
    m_modulesDebugEnabled = NULL;
    m_currentDispatcherTask = MR::TASK_ID_UNSPECIFIED;

    if(m_controlNamesTable)
    {
      NMP::Memory::memFree(m_controlNamesTable);
    }

    m_controlNamesTable = NULL;
    m_numControls = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Module Filtering
//----------------------------------------------------------------------------------------------------------------------
uint32_t InstanceDebugInterface::totalModules() const
{
  return m_numModules;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t InstanceDebugInterface::findModuleIndex(const char *moduleName) const
{
  for (uint32_t i = 0; i < m_numModules; i++)
  {
    if (NMP_STRCMP(moduleName, m_moduleNamesTable[i]) == 0)
      return i;
  }

  return cm_moduleNotFound;
}

//----------------------------------------------------------------------------------------------------------------------
bool InstanceDebugInterface::isModuleDebugEnabled(const char* moduleName) const
{
  uint32_t moduleIndex = findModuleIndex(moduleName);
  NMP_ASSERT(moduleIndex != cm_moduleNotFound);
  return m_modulesDebugEnabled[moduleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
bool InstanceDebugInterface::isModuleDebugEnabled(uint32_t moduleIndex) const
{
  NMP_ASSERT(moduleIndex < m_numModules);
  return m_modulesDebugEnabled[moduleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::enableModuleDebug(const char* moduleName, bool bEnable)
{
  uint32_t moduleIndex = findModuleIndex(moduleName);
  NMP_ASSERT(moduleIndex != cm_moduleNotFound);
  m_modulesDebugEnabled[moduleIndex] = bEnable;
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::enableModuleDebug(uint32_t moduleIndex, bool bEnable)
{
  NMP_ASSERT(moduleIndex < m_numModules);
  m_modulesDebugEnabled[moduleIndex] = bEnable;
}

//----------------------------------------------------------------------------------------------------------------------
const char* InstanceDebugInterface::getModuleName(uint32_t moduleIndex) const
{
  NMP_ASSERT(moduleIndex < m_numModules);
  return m_moduleNamesTable[moduleIndex];
}
//----------------------------------------------------------------------------------------------------------------------
const char* InstanceDebugInterface::getModuleParentName(uint32_t moduleIndex) const
{
  NMP_ASSERT(moduleIndex < m_numModules);
  return m_moduleParentsTable[moduleIndex];
}

//----------------------------------------------------------------------------------------------------------------------
// Control Filtering
//----------------------------------------------------------------------------------------------------------------------
uint32_t InstanceDebugInterface::totalControls() const
{
  return m_numControls;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t InstanceDebugInterface::findControlIndex(const char *controlName) const
{
  for (uint32_t i = 0; i < m_numControls; i++)
  {
    if (NMP_STRCMP(controlName, m_controlNamesTable[i]) == 0)
      return i;
  }

  return cm_controlNotFound;
}

//----------------------------------------------------------------------------------------------------------------------
const char* InstanceDebugInterface::getControlName(uint32_t controlIndex) const
{
  NMP_ASSERT(controlIndex < m_numControls);
  return m_controlNamesTable[controlIndex];
}

//----------------------------------------------------------------------------------------------------------------------
bool InstanceDebugInterface::enterScopeUpdate(ScopeType scopeType, const char* scopeName, MR::LimbIndex limbIndex)
{
  NMP_ASSERT(m_updatingScopeStackSize < cm_maxUpdatingModulesStackSize);

  uint32_t scopeIndex = scopeType == kModule ? findModuleIndex(scopeName) : findControlIndex(scopeName);

  return enterScopeUpdate(scopeType, scopeIndex, limbIndex);
}

//----------------------------------------------------------------------------------------------------------------------
bool InstanceDebugInterface::enterScopeUpdate(ScopeType scopeType, uint32_t scopeIndex, MR::LimbIndex limbIndex)
{
  NMP_ASSERT(m_updatingScopeStackSize < cm_maxUpdatingModulesStackSize);

  // May want to check that the new module does not exist anywhere on the current stack?
  NMP_ASSERT_MSG((m_updatingScopeStackSize > 0) ?
                 (m_updatingScopeStack[m_updatingScopeStackSize - 1] != scopeIndex) :
                 true,
                 "Trying to push the same module on to the update stack twice");

  m_updatingScopeStack[m_updatingScopeStackSize] = scopeIndex;
  m_updatingScopeTypesStack[m_updatingScopeStackSize] = scopeType;
  m_updatingLimbIndexStack[m_updatingScopeStackSize] = limbIndex;
  ++m_updatingScopeStackSize;
  return m_modulesDebugEnabled[scopeIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::exitScopeUpdate()
{
  NMP_ASSERT(m_updatingScopeStackSize > 0);
  --m_updatingScopeStackSize;
}

//----------------------------------------------------------------------------------------------------------------------
const char* InstanceDebugInterface::getUpdatingModuleName()
{
  if (m_updatingScopeStackSize > 0)
  {
    uint32_t scopeType = m_updatingScopeStack[m_updatingScopeStackSize - 1];
    uint32_t moduleIndex = m_updatingScopeStack[m_updatingScopeStackSize - 1];
    NMP_ASSERT(scopeType == kModule);
    if (scopeType == kModule)
    {
      NMP_ASSERT(moduleIndex < m_numModules);
      return m_moduleNamesTable[moduleIndex];
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const char* InstanceDebugInterface::getUpdatingScopeName()
{
  if (m_updatingScopeStackSize > 0)
  {
    uint32_t scopeType = m_updatingScopeTypesStack[m_updatingScopeStackSize - 1];
    uint32_t moduleIndex = m_updatingScopeStack[m_updatingScopeStackSize - 1];
    if (scopeType == kModule)
    {
      if (moduleIndex < m_numModules)
        return m_moduleNamesTable[moduleIndex];
    }
    else
    {
      if (moduleIndex < m_numControls)
        return m_controlNamesTable[moduleIndex];
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
MR::LimbIndex InstanceDebugInterface::getUpdatingLimbIndex()
{
  if (m_updatingScopeStackSize > 0)
  {
    MR::LimbIndex part = m_updatingLimbIndexStack[m_updatingScopeStackSize - 1];
    return part;
  }

  return INVALID_LIMB_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
// Scratch Pad
//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::logScratchPadInt(
  MR::NodeID  nodeID,
  const char* tag,
  int32_t     value,
  const char* variableName,
  uint16_t    lineNumber)
{
  if (debugOutputsAreOn(DEBUG_OUTPUT_SCRATCH_PAD))
  {
    m_DebugManager->logScratchPadInt(m_instanceID, nodeID, getUpdatingModuleName(), m_network->getCurrentFrameNo(), tag, value, variableName, lineNumber);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::logScratchPadFloat(
  MR::NodeID  nodeID,
  const char* tag,
  float       value,
  const char* variableName,
  uint16_t    lineNumber)
{
  if (debugOutputsAreOn(DEBUG_OUTPUT_SCRATCH_PAD))
  {
    m_DebugManager->logScratchPadFloat(m_instanceID, nodeID, getUpdatingModuleName(), m_network->getCurrentFrameNo(), tag, value, variableName, lineNumber);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::logScratchPadBool(
  MR::NodeID  nodeID,
  const char* tag,
  bool        value,
  const char* variableName,
  uint16_t    lineNumber)
{
  if (debugOutputsAreOn(DEBUG_OUTPUT_SCRATCH_PAD))
  {
    m_DebugManager->logScratchPadBool(m_instanceID, nodeID, getUpdatingModuleName(), m_network->getCurrentFrameNo(), tag, value, variableName, lineNumber);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::logScratchPadVector3(
  MR::NodeID          nodeID,
  const char*         tag,
  const NMP::Vector3& value,
  const char*         variableName,
  uint16_t            lineNumber)
{
  if (debugOutputsAreOn(DEBUG_OUTPUT_SCRATCH_PAD))
  {
    m_DebugManager->logScratchPadVector3(m_instanceID, nodeID, getUpdatingModuleName(), m_network->getCurrentFrameNo(), tag, value, variableName, lineNumber);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Task Execution
//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::beginDispatcherTaskExecute(
  MR::TaskID  taskID,
  const char *taskName,
  MR::NodeID  owningNodeID)
{
  if (debugOutputsAreOn(DEBUG_OUTPUT_TASK_QUEUING))
  {
    m_DebugManager->beginDispatcherTaskExecute(m_instanceID, m_network->getCurrentFrameNo(), taskID, taskName, owningNodeID);
    m_currentDispatcherTask = taskID;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::addDispatcherTaskExecuteParameter(
  MR::TaskID  taskID,
  const char *semanticName,
  MR::NodeID  owningNodeID,
  MR::NodeID  targetNodeID,
  uint32_t    validFrame,
  bool        isInput,
  bool        isOutput,
  uint16_t    lifespan)
{
  if (debugOutputsAreOn(DEBUG_OUTPUT_TASK_QUEUING))
  {
    NMP_ASSERT(m_currentDispatcherTask == MR::TASK_ID_UNSPECIFIED || m_currentDispatcherTask == taskID);
    m_DebugManager->addDispatcherTaskExecuteParameter(m_instanceID, m_network->getCurrentFrameNo(), taskID, semanticName, owningNodeID, targetNodeID, validFrame, isInput, isOutput, lifespan);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::endDispatcherTaskExecute()
{
  if (debugOutputsAreOn(DEBUG_OUTPUT_TASK_QUEUING))
  {
    m_DebugManager->endDispatcherTaskExecute(m_instanceID, m_network->getCurrentFrameNo());
    m_currentDispatcherTask = MR::TASK_ID_UNSPECIFIED;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// State Machine Requests
//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::logStateMachineMessageEventMsg(
  MR::MessageID messageID,
  MR::NodeID    targetSMNodeID,
  bool          set)
{
  if (debugOutputsAreOn(DEBUG_OUTPUT_STATE_MACHINE_REQUESTS))
  {
    m_DebugManager->logStateMachineMessageEventMsg(m_instanceID, m_network->getCurrentFrameNo(), messageID, targetSMNodeID, set);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Profiling Points
//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::logProfilePointTiming(const char *name, float timing)
{
  if (debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
  {
    m_DebugManager->logProfilePointTiming(m_instanceID, m_network->getCurrentFrameNo(), name, timing);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Debug Draw
//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawPoint(
  const NMP::Vector3& point,
  float               radius,
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawPoint(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      point,
      radius,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawSphere(
  const NMP::Matrix34& tm,
  float               radius,
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawSphere(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      tm,
      radius,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawLine(
  const NMP::Vector3& start,
  const NMP::Vector3& end,
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawLine(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      start,
      end,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawPolyLine(
  uint32_t            numVertices,       // Min of 2 and numLines = numVetices - 1;
  const NMP::Vector3* vertices,         // Array of vertices between lines.
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawPolyLine(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      numVertices,
      vertices,  
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawVector(
  VectorType          type,
  const NMP::Vector3& position,
  const NMP::Vector3& offset,
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawVector(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      type,
      position,
      offset,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawArrowHead(
  const NMP::Vector3& position,         // Position of arrow point (if not a delta).
  const NMP::Vector3& direction,        // Arrow direction.
  const NMP::Vector3& tangent,          // Generally at 90 degrees to the direction. Defines the width of the arrow.
  NMP::Colour         colour,
  bool                hasMass,          // If true draws solid arrows, else draw line arrows.
  bool                isDelta)          // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawArrowHead(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      position,
      direction,
      tangent,
      colour,
      hasMass,
      isDelta);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawTwistArc(
  const NMP::Vector3& position,         // Center of arc.
  const NMP::Vector3& primaryDir,       // Normal of the plane which the arc lies on (Not necessarily normalised).
  const NMP::Vector3& dir,              //
  const NMP::Vector3& dir2,             //
  NMP::Colour         colour,
  bool                doubleArrowHead,  // Draw an arrow at both ends of arc.
  bool                hasMass,          // Arrow heads are drawn as solid triangles.
  bool                isDelta)          // Controls how arrow heads are drawn (conventionally, inverted, or as a line cap)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawTwistArc(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
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
void InstanceDebugInterface::drawPlane(
  const NMP::Vector3& position,
  const NMP::Vector3& normal,
  float               radius,
  NMP::Colour         colour,
  float               normalScale)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawPlane(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      position,
      normal,
      radius,
      colour,
      normalScale);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawTriangle(
  const NMP::Vector3& v1,     // 1-2-3 should form a clockwise face.
  const NMP::Vector3& v2,
  const NMP::Vector3& v3,
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawTriangle(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      v1,
      v2,
      v3,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawMatrix(
  const NMP::Matrix34& matrix,
  float                scale,
  uint8_t              alpha)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawMatrix(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      matrix,
      scale,
      alpha);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawNonUniformMatrix(
  const NMP::Matrix34& matrix,
  float                scale,
  uint8_t              alpha)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawNonUniformMatrix(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      matrix,
      scale,
      alpha);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawConeAndDial(
  const NMP::Vector3& position,
  const NMP::Vector3& direction,
  float               angle,
  float               size,
  const NMP::Vector3& dialDirection,
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawConeAndDial(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      position,
      direction,
      angle,
      size,
      dialDirection,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawContactPointSimple(
  const NMP::Vector3& position,               // Contact point.
  float               forceMagnitudeSquared)  // Force magnitude squared at contact point. 
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawContactPointSimple(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      position,
      forceMagnitudeSquared);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawContactPointDetailed(
  const NMP::Vector3& position,          // Contact point.
  const NMP::Vector3& normal,            // Contact normal.
  const NMP::Vector3& force,             // Force at contact point. 
  const NMP::Vector3& actor0Pos,         // Actor 0 root position.
  const NMP::Vector3& actor1Pos)         // Actor 1 root position.
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawContactPointDetailed(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(), 
      getUpdatingLimbIndex(),
      position,
      normal,
      force,
      actor0Pos,
      actor1Pos);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawCharacterRoot(
  const NMP::Matrix34& characterControllerRoot)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawCharacterRoot(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      characterControllerRoot);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawBox(
  const NMP::Vector3& position,
  const NMP::Vector3& halfSizes,
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawBox(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      position,
      halfSizes,
      colour);
  }
}

void InstanceDebugInterface::drawBBox(
              const NMP::Matrix34& tm,
              const NMP::Vector3& halfSizes,
              NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawBBox(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(),
      getUpdatingLimbIndex(),
      tm,
      halfSizes,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawSphereSweep(
  const NMP::Vector3 &position,
  const NMP::Vector3 &motion,
  float               radius,
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawSphereSweep(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(), 
      getUpdatingLimbIndex(),
      position,
      motion,
      radius,
      colour);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InstanceDebugInterface::drawText(
  const NMP::Vector3 &position,
  const char         *text,
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawText(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(), 
      getUpdatingLimbIndex(),
      position,
      text,
      colour);
  }
}

void InstanceDebugInterface::drawEnvironmentPatch(
  const NMP::Vector3& position,           ///< Position of patch center in world space. 
  const NMP::Vector3& normal,             ///< Orientation of square.
  const float         size,
  NMP::Colour         colour)
{
  if (isDrawEnabled())
  {
    m_DebugManager->drawEnvironmentPatch(
      m_instanceID,
      getCurrentNodeID(),
      getUpdatingScopeName(),
      m_network->getCurrentFrameNo(), 
      getUpdatingLimbIndex(),
      position,
      normal,
      size,
      colour);
  }
}


//----------------------------------------------------------------------------------------------------------------------
bool InstanceDebugInterface::addNodeTiming(MR::NodeID nodeID, MR::NodeTaskTimingType type, float time)
{
  uint32_t entryIndex = findNodeTimingIndex(nodeID, type);

  if (entryIndex != 0xFFFFFFFF)
  {
    m_framesNodeTimings[entryIndex].m_time += time;
    return true;
  }
  else
  {
    if (m_numNodeTimings < m_maxNumNodeTimings)
    {
      m_framesNodeTimings[m_numNodeTimings].m_nodeID = nodeID;
      m_framesNodeTimings[m_numNodeTimings].m_type = type;
      m_framesNodeTimings[m_numNodeTimings].m_time = time;
      ++m_numNodeTimings;
      return true;
    }
#ifdef NMP_ENABLE_ASSERTS
    else
    {
      NMP_DEBUG_MSG("Exceeded CoreDebugInterface::m_maxNumNodeTimings when recoding node operation timings.");
    }
#endif // NMP_ENABLE_ASSERTS
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Find the index of a node timing from the specified node and with the specified type.
// returns 0xFFFFFFFF if not found.
uint32_t InstanceDebugInterface::findNodeTimingIndex(MR::NodeID nodeID, MR::NodeTaskTimingType type)
{
  for (uint32_t i = 0; i < m_numNodeTimings; ++i)
  {
    if ((m_framesNodeTimings[i].m_nodeID == nodeID) &&
        (m_framesNodeTimings[i].m_type == type))
    {
      return i;
    }
  }
  return 0xFFFFFFFF;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
