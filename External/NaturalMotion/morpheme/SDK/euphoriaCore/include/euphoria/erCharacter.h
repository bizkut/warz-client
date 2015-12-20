// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_CHARACTER_H
#define NM_CHARACTER_H

#include "NMPlatform/NMHashMap.h"
#include "euphoria/erAttribData.h"
#include "erDebugDraw.h"

namespace MR
{
class AnimRigDef;
class PhysicsRig;
class PhysicsScene;

struct BehaviourAnimationData;
struct PhysicsSerialisationBuffer;
}

namespace ER
{
class Behaviour;
class Body;
class CharacterDef;
class Module;
class RootModule;

struct BehaviourMessageParams;
struct BehaviourDefsBase;

//----------------------------------------------------------------------------------------------------------------------
/// Container class for everything a single euphoria character (instance) needs to operate.
/// 
/// This is the main class used to access euphoria. Through this class you can find individual behaviours, get a pointer
/// to the ER::Body, and from there access the limbs. An instance of this class can be retrived from MR::Network with a
/// call to 'networkGetCharacter'.
//----------------------------------------------------------------------------------------------------------------------
class Character
{
public:
  typedef NMP::hash_map<int, Behaviour*> Behaviours;

  Character();
  virtual ~Character();

  virtual void create(CharacterDef* definition, ER::RootModule* network);
  virtual void initialise(uint32_t collisionIgnoreGroups, Body* body, MR::InstanceDebugInterface* debugInterface);
  virtual void destroy();


  // Access to the euphoria body.
  ER::Body& getBody() {return *m_body;}
  const ER::Body& getBody() const {return *m_body;}

  // Access to behaviours.
  Behaviour* getBehaviour(uint32_t behaviourID) const; /// Returns the behaviour with the specified ID.
  const Behaviours& getBehaviours() const;             /// Returns a hash map of behaviour IDs to behaviour pointers.
  virtual void disableBehaviourEffects();              /// Prevent behaviours from affecting the character
  NM_INLINE bool isBehaviourUpdateDisabled() const { return m_disableBehaviourUpdates; }
  bool isAnimationRequiredByBehaviour(int behaviourID, int32_t animationID) const; /// Is animation used by a behaviour.
  void startBehaviour(int behaviourID);                /// Enable the specified behaviour.
  void stopBehaviour(int behaviourID);                 /// Disable the specified behaviour.
  /// This forces all behaviours to stop and clears any internal state - e.g. just prior to assigning to a new network.
  void reset();

  /// The debug interface can be added after creation if it was not available at the time. This will
  /// propagate the debug interface down through the modules too.
  void setDebugInterface(MR::InstanceDebugInterface* debugInterface);

  /// Call this to store the current state in savedState. Returns false if there is not enough space.
  bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  /// Call this to restore the state to savedState (assuming nothing has been created/destroyed).
  bool restoreState(MR::PhysicsSerialisationBuffer& savedState);

  /// Call this to put the character into a special 'pose modification' state. This will disable gravity on rig parts,
  /// disable network updates and increase world drag amongst other things. Pass in a structure containing enough space
  /// to store the state so that it can be restored afterwards. Returns false if there is not enough space (in which
  /// case nothing will have changed)
  bool startPoseModification(MR::PhysicsSerialisationBuffer& savedState);
  /// Call this to end the pose modification and restore the character state
  bool stopPoseModification(MR::PhysicsSerialisationBuffer& savedState);

  virtual void prePhysicsStep(float timeDelta);  /// Called before every physics update.
  virtual void postPhysicsStep(float timeDelta); /// Called after every physics update.
 
  /// Get / Set the priority of the highest priority node of the supplied behaviours type. This priority is used to 
  /// determine which instance of a particular behaviour node in the morpheme network should be used to configure that
  /// behaviour this frame.
  int32_t getHighestPriority(uint32_t id);
  void setHighestPriority(uint32_t id, int32_t priority);

  /// Copies output control parameter values from the behaviour to the outputControlParams struct so that they can then
  /// be passed to the connected nodes in the morpheme network.
  void handleOutputControlParams(
    uint32_t behaviourID,
    ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams,
    size_t numOutputControlParams);

  /// Add any messages emitted by this behaviour to the set of emitted messages for the highest priority node of this
  /// behaviour type.
  void handleEmitMessages(const uint32_t behaviourID, uint32_t& messages);

  /// Allows the specified behaviour to influence any joints that are enabled in the supplied joint mask.
  void handlePhysicsJointMask(uint32_t behaviourID, const MR::PhysicsRig::JointChooser& jointChooser);

  MR::InstanceDebugInterface* getDebugInterface() const { return m_debugInterface; }

  CharacterDef* m_definition;            /// Definition data for this character that doesn't change at runtime.
  uint32_t      m_collisionIgnoreGroups; /// Copy of rig's physics collision groups. Used by environment awareness.
  RootModule*   m_euphoriaRootModule;    /// Single reference to the network.

protected:
  virtual void updateNetwork(float timeStep);
  virtual void feedbackNetwork(float timeStep, bool clearUserInputs);

  /// The Body always exists and remains assigned to this character. In the future we might support
  /// multiple bodies - e.g. for different physics level of detail.
  Body* m_body;

  MR::InstanceDebugInterface* m_debugInterface;
  
  Behaviours m_behaviours;               /// List of behaviours that we own (and are responsible for creating/deleting).
  bool       m_disableBehaviourUpdates;  /// Indicates that character is in the 'Pose Modification' State.
  bool       m_behavioursActiveLastUpdate;
};

}
#endif // NM_CHARACTER_H
