// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_NODE_MODIFY_TRAJECTORY_H
#define MR_NODE_MODIFY_TRAJECTORY_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations

/// Queue task for trajectory delta.
Task* nodeModifyTrajectoryTransformQueueTrajectoryDelta(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

/// Queue task for combined transforms and trajectory.
Task* nodeModifyTrajectoryTransformQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);
  
//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataModifyTrajectoryDef
/// \brief Specifies the configuration of a ModifyTrajectory node.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataModifyTrajectoryDef : public AttribData
{
public:
  /// \brief Additional rotation is expressed as.
  enum RotationType
  {
    ROTATION_TYPE_EULER = 1,
    ROTATION_TYPE_QUATERNION
  };

  /// \brief Should the trajectory transform be modified with a velocity value or a displacement value.
  enum ModifyType
  {
    MODIFY_TYPE_DISPLACEMENT = 1, ///< Input control param transform specifies a displacement (position).
    MODIFY_TYPE_VELOCITY          ///< Input control param transform specifies a velocity.
  };

  /// \brief What type of operation to perform on the trajectory transform.
  enum OperationType
  {
    OPERATION_TYPE_ADD_TO_DELTA = 1, ///< Add to the input delta trajectory from the child.
    OPERATION_TYPE_SET_DELTA,        ///< Replace the input delta trajectory from the child.
    OPERATION_TYPE_SET_TRANSFORM     ///< Set the world transform of the character.
  };
  
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataModifyTrajectoryDef* init(
    NMP::Memory::Resource& resource,
    RotationType           rotationType,                 ///< How is any rotation modification expressed (euler or quat).
    ModifyType             linearModifyType,             ///< Modifying with a displacement or a velocity.
    OperationType          linearOperation,              ///< Delta add, delta set or set transform.
    ModifyType             angularModifyType,            ///< Modifying with a displacement or a velocity.
    OperationType          angularOperation,             ///< Delta add, delta set or set transform.
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator*  allocator,
    RotationType           rotationType,                 ///< How is any rotation modification expressed (euler or quat).
    ModifyType             linearModifyType,             ///< Modifying with a displacement or a velocity.
    OperationType          linearOperation,              ///< Delta add, delta set or set transform.
    ModifyType             angularModifyType,            ///< Modifying with a displacement or a velocity.
    OperationType          angularOperation,             ///< Delta add, delta set or set transform.
    uint16_t               refCount = 0);

  NM_INLINE AttribDataModifyTrajectoryDef() { setType(ATTRIB_TYPE_MODIFY_TRAJECTORY_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_MODIFY_TRAJECTORY_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

public:
  RotationType  m_rotationType;      ///< How is any rotation modification expressed (euler or quat).
  ModifyType    m_linearModifyType;  ///< Modifying with a displacement or a velocity.
  OperationType m_linearOperation;   ///< Delta add, delta set or set transform.
  ModifyType    m_angularModifyType; ///< Modifying with a displacement or a velocity.
  OperationType m_angularOperation;  ///< Delta add, delta set or set transform.
};
  
} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_MODIFY_TRAJECTORY_H
//----------------------------------------------------------------------------------------------------------------------

