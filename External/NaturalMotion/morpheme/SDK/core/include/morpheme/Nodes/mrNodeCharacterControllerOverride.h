// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_NODE_CHARACTER_CONTROLLER_OVERRIDE_H
#define MR_NODE_CHARACTER_CONTROLLER_OVERRIDE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCharacterControllerInterface.h"
#include "morpheme/TransitConditions/mrTransitCondition.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void nodeCCOverrideInitInstance(
  NodeDef* node,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
void nodeCCOverrideDeleteInstance(
  const NodeDef* ccOverrideNodeDef,
  Network*       net);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeCCOverrideUpdateConnections(
  NodeDef* node,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
bool nodeCCOverrideHandleMessages(
  const Message&  message,
  NodeID          node,
  Network*        network);

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataCCOverrideConditionsDef
/// \brief Def set of conditions that must be satisfied before applying new settings to the active character controller. 
///  Not per anim set. Note, if there are no conditions then evaluation returns true.
//----------------------------------------------------------------------------------------------------------------------
class AttribDataCCOverrideConditionsDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataCCOverrideConditionsDef* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataCCOverrideConditionsDef() { setType(ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  NM_INLINE uint32_t getNumConditions() const { return m_numConditions; }
  NM_INLINE TransitConditionDef* getConditionDef(uint32_t index) const;

  /// The priority of this override vs other active overrides.
  CCOverridePriority    m_priority;

  /// When this set of conditions is satisfied the attributes for the current anim set will be applied to the character controller.
  /// No conditions means that the attributes are always applied.
  uint16_t              m_numConditions;
  TransitConditionDef** m_conditions;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataCCOverridePropertiesDef
/// \brief Set of character controller properties to override the current character controller state with. Per anim set.
///
/// For example height, radius, shape, collision, behaviour state etc.
//----------------------------------------------------------------------------------------------------------------------
class AttribDataCCOverridePropertiesDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataCCOverridePropertiesDef* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataCCOverridePropertiesDef() { setType(ATTRIB_TYPE_C_C_OVERRIDE_PROPERTIES_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_C_C_OVERRIDE_PROPERTIES_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  
  /// \brief Properties access.
  NM_INLINE uint16_t getNumProperties() const { return m_numProperties; }
  NM_INLINE AttribData* getProperty(uint16_t index) const;
  NM_INLINE CCPropertyType getPropertyType(uint16_t index) const;

  /// The attributes that will be applied to the character controller when all conditions are satisfied.
  uint16_t        m_numProperties;
  CCPropertyType* m_propertyTypes;
  AttribData**    m_properties;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataCCOverrideConditions
/// \brief Active state of a set of conditions that must be satisfied before applying new settings to the 
///  active character controller. Not per anim set. Note, if there are no conditions then evaluation returns true.
//----------------------------------------------------------------------------------------------------------------------
class AttribDataCCOverrideConditions : public AttribData
{
public:
  // Initialising from def data. Used on line.
  static NMP::Memory::Format getMemoryRequirements(
    AttribDataCCOverrideConditionsDef* ccOverrideAttribDef);
  static AttribDataCCOverrideConditions* init(
    NMP::Memory::Resource&             resource,
    AttribDataCCOverrideConditionsDef* ccOverrideAttribDef,
    Network*                           net,
    NodeID                             nodeID,        ///< The id of the owner of this condition set.
    uint16_t                           refCount = 0);

  NM_INLINE AttribDataCCOverrideConditions() { setType(ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS; }

  // \brief A specialist relocate function is required because the AttribDataCCOverrideConditions stores a pointer to its originating AttribDataCCOverrideConditionsDef.
  static void relocate(AttribData* target, const NetworkDef* netDef);

  /// \brief A specialist locate function that just calls the relocate and does no endian swapping.
  static void locate(AttribData* target, const NetworkDef* netDef);

  /// \brief Update the status of all conditions.
  /// \return true if all conditions are satisfied or if there are no conditions.
  bool updateConditions(
    NodeID   childNodeID,
    Network* net);

  /// \brief Are the conditions all currently satisfied.
  NM_INLINE bool conditionsSatisfied() const;
  
  /// \brief Clear all of the conditions on the active state.
  NM_INLINE void resetConditions();

  /// \brief Get this active character controller overrides corresponding def structure.
  NM_INLINE const AttribDataCCOverrideConditionsDef* getDef() const { return m_def; }

  /// \brief Flag a message as having been set or cleared.
  /// \return false if a request condition with this messageID does not exist.
  bool setMessage(
    MessageID messageID, ///< Id of messages to set the status of.
    bool      status,    ///< Set this message on or off.
    Network*  net);
  
  /// \brief Make sure that condition dependencies are queued.
  /// 
  /// This is because some bits of AttribData that conditions are attempting to monitor may not otherwise be queued for update.
  NM_INLINE void queueConditionDeps(
    NodeID   childNodeID,
    Network* net);
  
public:
  NodeID                             m_nodeID;     ///< The ID of our owning Node.
                                                   ///<  This is only required when trying to locate or relocate this class because we store a pointer to corresponding def data.
  AttribDataCCOverrideConditionsDef* m_def;        ///< Pointer back to the definition.
  TransitCondition**                 m_conditions; ///< Current active status of conditions.
};

//----------------------------------------------------------------------------------------------------------------------
// AttribDataCCOverrideConditionsDef functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE TransitConditionDef* AttribDataCCOverrideConditionsDef::getConditionDef(uint32_t index) const
{
  NMP_ASSERT(index < m_numConditions);
  return m_conditions[index];
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataCCOverrideAttibutesDef functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribData* AttribDataCCOverridePropertiesDef::getProperty(uint16_t index) const
{
  NMP_ASSERT(index < m_numProperties);
  return m_properties[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE CCPropertyType AttribDataCCOverridePropertiesDef::getPropertyType(uint16_t index) const 
{
  NMP_ASSERT(index < m_numProperties);
  return m_propertyTypes[index];
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataCCOverride functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDataCCOverrideConditions::resetConditions()
{
  for (uint32_t i = 0; i < m_def->getNumConditions(); ++i)
  {
    TransitConditionDef* tcDef = m_def->getConditionDef(i);
    NMP_ASSERT(tcDef && m_conditions[i]);
    tcDef->condReset(m_conditions[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool AttribDataCCOverrideConditions::conditionsSatisfied() const
{
  // Track whether all conditions have been satisfied.
  bool result = true;
  for (uint32_t i = 0; i < m_def->getNumConditions(); ++i)
  {
    result &= m_conditions[i]->condIsSatisfied();
  }
  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_CHARACTER_CONTROLLER_OVERRIDE_H
//----------------------------------------------------------------------------------------------------------------------
