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
#ifndef MR_CHARACTER_CONTROLLER_ATTRIB_DATA_H
#define MR_CHARACTER_CONTROLLER_ATTRIB_DATA_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"
#include "morpheme/mrCharacterControllerInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerDef;
class CharacterControllerInterface;

//----------------------------------------------------------------------------------------------------------------------
/// \enum MR::PhysicsAndCharacterControllerUpdate
/// \brief Describes how character controllers are being updated in relation to the Network and Physics updates.
//----------------------------------------------------------------------------------------------------------------------
enum PhysicsAndCharacterControllerUpdate
{
  /// This (the default method) indicates that the application implements separate updates for
  /// physics and the character controller, so soft/hard keyframing can place the physics bodies at
  /// the correct location.
  /// Example update steps:
  ///  1. Execute all Networks to MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER
  ///  2. Update character all controllers
  ///  3. Execute all Networks to MR_TASKID_NETWORKUPDATEPHYSICS
  ///     Which amongst other things updates physics body target positions based on the position of the character controller (calculated in 2).
  ///  4. Global physics simulation update.
  ///  5. Complete Network execution.
  /// Requires that we update character controller positions outside of the main physics simulation,
  /// which in reality requires that we have a kinematic character controller update.
  /// Gives accurate placement of the physics bodies relative to the character controller position from the current frame.
  PHYSICS_AND_CC_UPDATE_SEPARATE,

  /// This uses the un-updated character controller location to set targets for soft/hard
  /// keyframing, so these methods will result in the physics parts lagging behind animation.
  /// Example update steps:
  ///  1. Execute all Networks to MR_TASKID_NETWORKUPDATEPHYSICS
  ///  2. Global physics simulation and character controller update.
  ///  3. Complete Network execution.
  //  Means that physics bodies will be updated relative to the character controller position from the previous frame.
  //  There is no restriction on whether we use a dynamic or a kinematic character controller, the only restriction is that
  //  it is not able to use any trajectory update data from Network task updating in this frame.
  PHYSICS_AND_CC_UPDATE_COMBINED_NO_PREDICTION,

  /// This uses the network trajectory output to predict the position and orientation of the
  /// character when in soft/hard keyframing. This can result in the physics parts pushing through
  /// walls, and not following vertical movement perfectly (1 frame's lag).
  /// Example update steps:
  ///  1. Execute all Networks to MR_TASKID_NETWORKUPDATEPHYSICS
  ///  2. Global physics simulation and character controller update.
  ///  3. Complete Network execution.
  /// This is the same style of update as PHYSICS_AND_CC_UPDATE_COMBINED_NO_PREDICTION, but it uses the current frames 
  /// trajectory delta transform to predict the movement of the character controller prior to the physics simulation step.
  /// Physics bodies will be placed with respect to this predicted position.
  /// The only time this is inaccurate is if the character controller does not actually move by the same amount as the prediction
  /// (because of a collision or movement on slopes, for example)
  PHYSICS_AND_CC_UPDATE_COMBINED_USE_NETWORK_TRAJ_DELTA,

  /// This uses the network trajectory output to predict the orientation of the character, and an
  /// extrapolation of the previous two frames to predict the position. This results in a very
  /// small amount of error (and no lag) in the physics bodies' positions with normal movement,
  /// though sudden trajectory changes will result in larger errors.
  /// Example update steps:
  ///  1. Execute all Networks to MR_TASKID_NETWORKUPDATEPHYSICS
  ///  2. Global physics simulation and character controller update.
  ///  3. Complete Network execution.
  /// This is the same as PHYSICS_AND_CC_UPDATE_COMBINED_USE_NETWORK_TRAJ_DELTA but with an alternative method for predicting 
  /// the continued movement of the character controller that is better when there is a collision or when moving on slopes.
  PHYSICS_AND_CC_UPDATE_COMBINED_EXTRAPOLATE_POSITION
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataCharacterProperties
/// \brief The current world root transform of the Network/game character and information reflecting the state of the
///  Networks associated character controller, if there is one.
///
/// The world root position of the Network/game character is updated every frame in 1 of 2 possible ways:
///   1. No character controller is active and physics is driving the world root position of the Network e.g. ragdoll.
///   2. Character controller is active and animation is driving the world root position of the Network e.g. animation, soft keyframing.
/// Either way the resulting world transform is copied into the Network held instance of this AttribData.
/// This allows tasks and other functions to access the characters root position without having to directly access
/// external structures like CharacterControllers (not possible within a task running on SPU for example).
//----------------------------------------------------------------------------------------------------------------------
class AttribDataCharacterProperties : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount = 0);

  NM_INLINE AttribDataCharacterProperties() { setType(ATTRIB_TYPE_CHARACTER_PROPERTIES); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_CHARACTER_PROPERTIES; }

  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

#ifdef NM_DEBUG
  // Relocate function traps illegal use of CC on SPU.
  static void relocate(AttribData* target, void* location);
  static void relocate(AttribData* target) { relocate(target, target); }
#endif

  NMP::Matrix34 m_worldRootTransform;
  NMP::Matrix34 m_prevWorldRootTransform;

  float         m_groundContactTime;
  PhysicsAndCharacterControllerUpdate m_physicsAndCharacterControllerUpdate;
  bool          m_achievedRequestedMovement;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataCharacterControllerDef
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class AttribDataCharacterControllerDef : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataCharacterControllerDef* init(
    NMP::Memory::Resource&     resource,
    CharacterControllerDef*    characterControllerDef);

  NM_INLINE AttribDataCharacterControllerDef() { setType(ATTRIB_TYPE_CHARACTER_CONTROLLER_DEF); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_CHARACTER_CONTROLLER_DEF; }

  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

  CharacterControllerDef* m_characterControllerDef;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHARACTER_CONTROLLER_ATTRIB_DATA_H
//----------------------------------------------------------------------------------------------------------------------
