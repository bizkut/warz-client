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
#ifndef MR_NODE_MODIFY_JOINT_H
#define MR_NODE_MODIFY_JOINT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations

/// Queue task for transforms.
Task* nodeModifyJointTransformQueueTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

/// Queue task for combined transforms and trajectory.
Task* nodeModifyJointTransformQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataModifyJointDef
/// \brief Specifies the configuration of a ModifyJoint node.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataModifyJointDef : public AttribData
{
public:
  /// \brief Additional rotation is expressed as.
  enum RotationType
  {
    ROTATION_TYPE_EULER = 1,
    ROTATION_TYPE_QUATERNION
  };

  /// \brief The coordinate space that the modification is expressed in.
  enum CoordinateSpace
  {
    COORD_SPACE_LOCAL = 1,
    COORD_SPACE_CHARACTER,
    COORD_SPACE_WORLD
  };
    
  /// \brief What type of operation to perform on the joint transform.
  enum OperationType
  {
    OPERATION_TYPE_ADD = 1,
    OPERATION_TYPE_SET
  };
  
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataModifyJointDef* init(
    NMP::Memory::Resource& resource,
    RotationType           rotationType,                 ///< How is any rotation modification expressed (euler or quat).
    CoordinateSpace        coordinateSpace,              ///< What space are our modifications expressed in.
    OperationType          linearOperation,              ///< Add or set.
    OperationType          angularOperation,             ///< Add or set.
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator*  allocator,
    RotationType           rotationType,                 ///< How is any rotation modification expressed (euler or quat).
    CoordinateSpace        coordinateSpace,              ///< What space are our modifications expressed in.
    OperationType          linearOperation,              ///< Add or set.
    OperationType          angularOperation,             ///< Add or set.
    uint16_t               refCount = 0);

  NM_INLINE AttribDataModifyJointDef() { setType(ATTRIB_TYPE_MODIFY_JOINT_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_MODIFY_JOINT_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

public:
  RotationType    m_rotationType;               ///< How is any rotation modification expressed (euler or quat).
  CoordinateSpace m_coordinateSpace;            ///< What space are our modifications expressed in.
  OperationType   m_linearOperation;            ///< Add or set.
  OperationType   m_angularOperation;           ///< Add or set.
};
  
} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_MODIFY_JOINT_H
//----------------------------------------------------------------------------------------------------------------------

