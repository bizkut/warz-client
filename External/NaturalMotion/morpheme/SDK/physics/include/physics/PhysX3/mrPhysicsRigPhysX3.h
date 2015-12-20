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
#ifndef MR_PHYSICS_RIG_PHYSX3_H
#define MR_PHYSICS_RIG_PHYSX3_H
//----------------------------------------------------------------------------------------------------------------------
#include "physics/mrPhysicsRig.h"
#include "mrPhysX3Configure.h"
#if defined(MR_OUTPUT_DEBUGGING)
#include "sharedDefines/mPhysicsDebugInterface.h"
#endif // MR_OUTPUT_DEBUGGING
#ifdef NM_COMPILER_MSVC
  #pragma warning(disable: 4555)
#endif //NM_COMPILER_MSVC
#include "NMPlatform/NMHashMap.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class PhysicsScenePhysX3;

//----------------------------------------------------------------------------------------------------------------------
bool locatePhysicsRigDefPhysX3(uint32_t assetType, void* assetMemory);

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsRigPhysX3
/// \brief Base class for the PhysX 3 implementation of a physics rig, which can be either jointed
///        or articulation.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsRigPhysX3 : public PhysicsRig
{
protected:
  /// The maximum number of shapes that will appear in a physics volume. Used to size temporary
  /// internal memory.
  enum { MAX_SHAPES_IN_VOLUME = 16 }; 
public:
  enum Type {TYPE_JOINTED, TYPE_ARTICULATED};

  class PartPhysX3 : public Part
  {
    friend class PhysicsRigPhysX3;

  public:
    PartPhysX3();

    /// The underlying PhysX rigid body (not the kinematic object if the rig is articulation).
    physx::PxRigidBody *getRigidBody() { return m_rigidBody; }
    const physx::PxRigidBody *getRigidBody() const {return m_rigidBody;}

    void addVelocityChange(const NMP::Vector3& velChange, const NMP::Vector3& worldPos, float angularMultiplier = 1.0f) NM_OVERRIDE;
    void addImpulse(const NMP::Vector3 &impulse) NM_OVERRIDE;
    void addTorqueImpulse(const NMP::Vector3& torqueImpulse) NM_OVERRIDE;
    void addTorque(const NMP::Vector3& torque) NM_OVERRIDE;
    void addForce(const NMP::Vector3 &force) NM_OVERRIDE;
    void addLinearVelocityChange(const NMP::Vector3& velChange) NM_OVERRIDE;
    void addAngularAcceleration(const NMP::Vector3& angularAcceleration) NM_OVERRIDE;

    float getSKDeviation() const NM_OVERRIDE;
    float getSKDeviationAngle() const NM_OVERRIDE;

  protected:
    physx::PxRigidBody* m_rigidBody;
    /// Current increase in the skin width (contactOffset) above the value that was authoured. Used
    /// to prevent unnecessary calls to change the skin width
    float m_currentSkinWidthIncrease; 

    float   m_SKDeviation; ///< If SK, then this is the current position deviation from the target.
    float   m_SKDeviationAngle; ///< If SK then this is the current orientation deviation (in radians) from the target.
  };

  class JointPhysX3 : public Joint
  {
  public:

    JointPhysX3(const PhysicsSixDOFJointDef* const def);

    PhysicsSixDOFJointModifiableLimits&       getModifiableLimits()       { return m_modifiableLimits; }
    const PhysicsSixDOFJointModifiableLimits& getModifiableLimits() const { return m_modifiableLimits; }

    void clampToLimits(NMP::Quat& orientation, float limitFrac, const NMP::Quat* origQ) const NM_OVERRIDE;
    void expandLimits(const NMP::Quat& orientation) NM_OVERRIDE;
    void scaleLimits(float scaleFactor) NM_OVERRIDE;
    void resetLimits() NM_OVERRIDE;

#if defined(MR_OUTPUT_DEBUGGING)
    virtual uint32_t serializeTxFrameData(void* outputBuffer, uint32_t outputBufferSize) const NM_OVERRIDE;
    void updateSerializeTxFrameData();
#endif // MR_OUTPUT_DEBUGGING

  protected:
#if defined(MR_OUTPUT_DEBUGGING)
    PhysicsSixDOFJointFrameData   m_serializeTxFrameData; // Copy of data to be serialized for debug render.
#endif // MR_OUTPUT_DEBUGGING

    PhysicsSixDOFJointModifiableLimits m_modifiableLimits; // Copy of joint limits that can be modified at runtime.
    const PhysicsSixDOFJointDef*    m_def;    // Pointer to jointDef (owned by parent rig).
  };


  //--------------------------------------------------------------------------------------------------------------------
  /// Initialises rig ID and the type
  /// See PhysX documentation for information on the ownerClientID and clientBehaviourBits
  static void init(
    PhysicsRigPhysX3* physicsRigPhysX3, Type type, physx::PxClientID ownerClientID, uint32_t clientBehaviourBits);

  /// Indicates if this is a jointed or articulation rig
  Type getType() const {return m_type;}

  /// Will return a pointer only if this is of type TYPE_ARTICULATED
  class PhysicsRigPhysX3Articulation* getPhysicsRigPhysX3Articulation();

  /// Will return a pointer only if this is of type TYPE_JOINTED
  class PhysicsRigPhysX3Jointed* getPhysicsRigPhysX3Jointed();

  /// Returns the unique identified for this rig
  int32_t getRigID() const { return m_rigID; }

  void setKinematicPos(const NMP::Vector3& pos) NM_OVERRIDE;

  float getMaxSKDeviation() const NM_OVERRIDE;

  void requestJointProjectionParameters(int iterations, float linearTolerance, float angularTolerance) NM_OVERRIDE;

  /// Application should pass in any actor being used for capsule/character movement
  void setCharacterControllerActor(physx::PxRigidActor* characterControllerActor) { 
    m_characterControllerActor = characterControllerActor; }

  /// Returns the PhysX actor used by the Character Controller associated with this physics rig.
  physx::PxRigidActor* getCharacterControllerActor() { return m_characterControllerActor; }
  const physx::PxRigidActor* getCharacterControllerActor() const { return m_characterControllerActor; }

  /// Get the PhysX3 specific scene.
  PhysicsScenePhysX3* getPhysicsScenePhysX() const { return (PhysicsScenePhysX3*)m_physicsScene; };

  /// See PhysX documentation for information on the ownerClientID and clientBehaviourBits
  physx::PxClientID getClientID() const { return m_ownerClientID; };

  /// This adds the mask to the filter data on all the parts
  void addQueryFilterFlagToParts(uint32_t word0, uint32_t word1, uint32_t word2, uint32_t word3);

  /// \name Gets the PhysX3 specific part given the part index
  /// @{
  PartPhysX3* getPartPhysX3(uint32_t partIndex);
  const PartPhysX3* getPartPhysX3(uint32_t partIndex) const;
  /// @}

  /// Write out the physics rig to a repx collection
  virtual void dumpToRepX(physx::repx::RepXCollection* collection, physx::repx::RepXIdToRepXObjectMap *idMap) const = 0;

  void receiveImpulse(
    int32_t inputPartIndex,
    const NMP::Vector3& position,
    const NMP::Vector3& direction,
    float localMagnitude,
    float localAngularMultiplier,
    float localResponseRatio,
    float fullBodyMagnitude,
    float fullBodyAngularMultiplier,
    float fullBodyLinearMultiplier,
    float fullBodyResponseRatio,
    bool positionInWorldSpace,
    bool directionInWorldSpace,
    bool applyAsVelocityChange) NM_OVERRIDE;

  void receiveTorqueImpulse(
    int32_t inputPartIndex,
    const NMP::Vector3& direction,
    float localMagnitude,
    float localResponseRatio,
    float fullBodyMagnitude,
    float fullBodyResponseRatio,
    bool directionInWorldSpace,
    bool applyAsVelocityChange);

  virtual void setSkinWidthIncrease(uint32_t partIndex, float skinWidthIncrease) NM_OVERRIDE;

  void scaleFrictionOnPart(const int32_t partIndex, const float frictionScale) NM_OVERRIDE;

  /// This should get called each update by morpheme nodes (or possibly anything else) to inform the
  /// rig that a physical joint has been created, or is being actively maintained, on the rig. If
  /// the rig gets an update and the joint hasn't been deregistered, then the joint will be
  /// released. This allows operator nodes to create joints that will be released when the operator
  /// stops being active.
  void registerJointOnRig(physx::PxD6Joint* joint);
  /// Should be called when a joint has been released.
  void deRegisterJointOnRig(physx::PxD6Joint* joint);

protected:
  /// This should be called internally in the pre-physics update
  void updateRegisteredJoints();

  /// Position we move the actors to when we disable the body (and make it kinematic)
  NMP::Matrix34 m_kinematicPose;

  /// \brief Fills the PxMaterialDesc with the PhysicsRigDef::Part::Material object.
  static physx::PxMaterial* createMaterial(const PhysicsRigDef::Part::Material& material);

  /// Array of pointers to the materials used by each part. 
  physx::PxMaterial**  m_materials;

  physx::PxClientID    m_ownerClientID;
  uint32_t             m_clientBehaviourBits;

  /// The desired (in this frame) iterations for resolving joint separation. Will be reset after the physics step.
  int m_desiredJointProjectionIterations;
  /// The desired (in this frame) distance tolerance for resolving joint separation. Will be reset after the physics step.
  float m_desiredJointProjectionLinearTolerance;
  /// The desired (in this frame) angular tolerance for resolving joint separation. Will be reset after the physics step.
  float m_desiredJointProjectionAngularTolerance;

private:

  /// Indicates the PhysX 3 rig type - jointed or articulated
  Type m_type;

  /// Used to keep track of which rig instance is which, e.g. so you can detect a different
  /// character's collision but ignore your own
  int32_t m_rigID; 

  /// the actor being used for character control
  physx::PxRigidActor*  m_characterControllerActor;

  typedef NMP::hash_map<physx::PxD6Joint*, bool> RegisteredJoints;
  RegisteredJoints m_registeredJoints;
};

//----------------------------------------------------------------------------------------------------------------------
struct PhysicsRigPhysX3ActorData
{
  static void init();
  static void term();

  static PhysicsRigPhysX3ActorData *create(
    physx::PxActor *actor, 
    PhysicsRig::Part* owningRigPart,
    PhysicsRig* owningRig);
  static void destroy(PhysicsRigPhysX3ActorData *data, physx::PxActor *actor);
  PhysicsRigPhysX3ActorData(physx::PxActor *actor, PhysicsRig::Part* owningRigPart, PhysicsRig* owningRig);
  /// More application-specific user data structures will have a function like this - so mirror their API
  static PhysicsRigPhysX3ActorData *getFromActor(physx::PxActor *actor);

  /// If non-zero, this will point to the physics rig part associated with this actor/shape etc
  PhysicsRig::Part* m_owningRigPart;
  /// If non-zero, this will point to the physics rig associated with this actor/shape etc
  PhysicsRig* m_owningRig;

  /// For application use
  void *m_userData;

  typedef NMP::hash_map<physx::PxActor*, PhysicsRigPhysX3ActorData*> ActorToPhysicsRigPhysX3ActorData;

private:
  static ActorToPhysicsRigPhysX3ActorData* m_actorToMorphemeMap;
  static uint32_t m_actorMapRefCount;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_RIG_PHYSX3JOINTED_H
//----------------------------------------------------------------------------------------------------------------------
