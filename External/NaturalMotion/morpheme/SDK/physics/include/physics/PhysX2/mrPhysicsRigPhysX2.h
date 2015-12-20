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
#ifndef MR_PHYSICS_RIG_PHYSX2_H
#define MR_PHYSICS_RIG_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
#include "physics/mrPhysicsRig.h"
#include "mrPhysX2Configure.h"
#if defined(MR_OUTPUT_DEBUGGING)
#include "sharedDefines/mPhysicsDebugInterface.h"
#endif // MR_OUTPUT_DEBUGGING
//----------------------------------------------------------------------------------------------------------------------

class NxMaterial;
class NxActor;
class NxJoint;
class NxD6JointDesc;

namespace MR
{

class PhysicsRigDef;
class PhysicsScenePhysX2;
class AnimRigDef;

//----------------------------------------------------------------------------------------------------------------------
bool locatePhysicsRigDefPhysX2(uint32_t assetType, void* assetMemory);

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsRigPhysX2
/// \brief Container for the PhysX 2 implementation of a physics rig def.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsRigPhysX2 : public PhysicsRig
{
  enum { MAX_SHAPES_IN_VOLUME = 64 }; ///< The maximum number of shapes that will appear in a physics volume. Used to size temporary internal memory.
public:
  //--------------------------------------------------------------------------------------------------------------------
  class PartPhysX : public Part
  {
    friend class PhysicsRigPhysX2;

  public:
    enum DirtyFlags
    {
      kDirty_GlobalPose = (1 << 0),
      kDirty_LinearVel = (1 << 1),
      kDirty_AngularVel = (1 << 2),
      kDirty_BodyFlags = (1 << 3),
      kDirty_ActorFlags = (1 << 4),
      kDirty_KinematicTarget = (1 << 5),
    };

    struct CachedData
    {
      NMP::Matrix34 globalPose;
      NMP::Matrix34 kinematicTarget;
      NMP::Vector3 linearVel;
      NMP::Vector3 angularVel;
      NMP::Vector3  COMPosition;
      NMP::Vector3  COMVelocity;
      NMP::Vector3  angularMomentum;
      NMP::Matrix34 COMOffsetLocal;
      NMP::Matrix34 globalInertiaTensor;
      float mass;
      uint32_t bodyFlags;
      uint32_t actorFlags;
    };

    PartPhysX();
    PartPhysX(const PartPhysX& other);
    ~PartPhysX();
    PartPhysX& operator=(const PartPhysX& other);

    NxActor* getActor() { return m_actor; }
    const NxActor* getActor() const { return m_actor; }

    //-----------------------------------------------------------------------------------------------
    // The following member functions make up the implementation of the engine independent base class
    //-----------------------------------------------------------------------------------------------

    NMP::Vector3  getPosition() const NM_OVERRIDE;
    void          setPosition(const NMP::Vector3& p) NM_OVERRIDE;
    NMP::Quat     getQuaternion() const NM_OVERRIDE;
    void          setQuaternion(const NMP::Quat& q) NM_OVERRIDE;

    NMP::Matrix34 getTransform() const NM_OVERRIDE;
    void          setTransform(const NMP::Matrix34& tm) NM_OVERRIDE;
    void          moveTo(const NMP::Matrix34& tm) NM_OVERRIDE;

    NMP::Vector3  getVel() const NM_OVERRIDE;
    void          setVel(const NMP::Vector3& v) NM_OVERRIDE;
    NMP::Vector3  getAngVel() const NM_OVERRIDE;
    void          setAngVel(const NMP::Vector3& r) NM_OVERRIDE;
    NMP::Vector3  getVelocityAtPoint(const NMP::Vector3& point) const NM_OVERRIDE;

    NMP::Vector3  getLinearMomentum() const NM_OVERRIDE;
    NMP::Vector3  getAngularMomentum() const NM_OVERRIDE;
    float         getMass() const NM_OVERRIDE;

    NMP::Vector3  getMassSpaceInertiaTensor() const NM_OVERRIDE;
    NMP::Matrix34 getGlobalInertiaTensor() const NM_OVERRIDE;

    NMP::Vector3  getCOMPosition() const NM_OVERRIDE;
    NMP::Matrix34 getCOMOffsetLocal() const NM_OVERRIDE;

    void          makeKinematic(bool kinematic, float massMultiplier, bool enableConstraint) NM_OVERRIDE;
    bool          isKinematic() const NM_OVERRIDE;

    void          enableCollision(bool enable) NM_OVERRIDE;
    bool          getCollisionEnabled() const NM_OVERRIDE;

    void          addVelocityChange(const NMP::Vector3& velChange, const NMP::Vector3& worldPos, float angularMultiplier = 1.0f) NM_OVERRIDE;
    void          addImpulse(const NMP::Vector3 &impulse) NM_OVERRIDE;
    void          addTorqueImpulse(const NMP::Vector3& torqueImpulse) NM_OVERRIDE;
    void          addTorque(const NMP::Vector3& torque) NM_OVERRIDE;
    void          addForce(const NMP::Vector3 &force) NM_OVERRIDE;
    void          addLinearVelocityChange(const NMP::Vector3& velChange) NM_OVERRIDE;
    void          addAngularAcceleration(const NMP::Vector3& angularAcceleration) NM_OVERRIDE;

    bool          storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
    bool          restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

    float         getSKDeviation() const NM_OVERRIDE;
    float         getSKDeviationAngle() const NM_OVERRIDE;

#if defined(MR_OUTPUT_DEBUGGING)
    uint32_t serializeTxPersistentData(uint16_t nameToken, uint32_t objectID, void* outputBuffer, uint32_t outputBufferSize) const NM_OVERRIDE;
    uint32_t serializeTxFrameData(void* outputBuffer, uint32_t outputBufferSize) const NM_OVERRIDE;
#endif // MR_OUTPUT_DEBUGGING

  protected:

    void applyModifiedValues();

    // These cached calls are used to reduce the number of costly calls to the physx API
    void generateCachedValues();

    const CachedData& getCachedData() { return m_cache; }
    void setCachedData(const CachedData& cache) { m_cache = cache; }

    void updateCOMPosition();

    NxActor* m_actor;
    float   m_SKDeviation; ///< If SK, then this is the current position deviation from the target.
    float   m_SKDeviationAngle; ///< If SK then this is the current orientation deviation (in radians) from the target.

    CachedData    m_cache;
    uint32_t      m_dirtyFlags;
  };
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  class JointPhysX : public Joint
  {
    friend class PhysicsRigPhysX2;

  public:
    struct CacheData
    {
      NMP::Quat     driveOrientation;
      NxD6JointDesc jointDesc;
    };

    JointPhysX();

    // Joint overrides.

#if defined(MR_OUTPUT_DEBUGGING)
    virtual uint32_t serializeTxPersistentData(
      const PhysicsJointDef* jointDef,
      uint16_t stringToken,
      void* outputBuffer,
      uint32_t outputBufferSize) const NM_OVERRIDE;
    virtual uint32_t serializeTxFrameData(void* outputBuffer, uint32_t outputBufferSize) const NM_OVERRIDE;
#endif // MR_OUTPUT_DEBUGGING

    float getMaxTwistDamping() const { return m_maxTwistDamping; }
    float getMaxTwistStrength() const { return m_maxTwistStrength; }
    float getMaxSwingDamping() const { return m_maxSwingDamping; }
    float getMaxSwingStrength() const { return m_maxSwingStrength; }
    float getMaxSlerpDamping() const { return m_maxSlerpDamping; }
    float getMaxSlerpStrength() const { return m_maxSlerpStrength; }

    NMP::Quat getRotation(const MR::PhysicsJointDef* jointDef, const NMP::Matrix34& part1TM, const NMP::Matrix34& part2TM) const;

    void enableLimit(bool enable);
    void setDriveStrength(float twistStrength, float swingStrength, float slerpStrength);
    void setDriveDamping(float twistDamping, float swingDamping, float slerpDamping);
    void setDriveOrientation(const NMP::Quat &quat);

    // TODO: rationalise these with the above setDriveStrength etc, if possible
    void setStrength(float strength) NM_OVERRIDE;
    float getStrength() const NM_OVERRIDE;

    void setDamping(float damping) NM_OVERRIDE;
    float getDamping() const NM_OVERRIDE;

    NMP::Quat getTargetOrientation() NM_OVERRIDE;
    void setTargetOrientation(const NMP::Quat &orientation) NM_OVERRIDE;

    // Providing this function to set the drive strength and damping at the same time will reduce
    // the number of times we have to serialise to a joint def and back, greatly increasing performance.
    // by caching the values and only accessing via the descriptor when needed
    void setSlerpDriveStrengthAndDamping(float slerpStrength, float slerpDamping);

    void getDriveParams(
      float& twistStrength, float& swingStrength, float& slerpStrength,
      float& twistDamping, float& swingDamping, float& slerpDamping);

    bool storeState(MR::PhysicsSerialisationBuffer& savedState);
    bool restoreState(MR::PhysicsSerialisationBuffer& savedState);

    PhysicsSixDOFJointModifiableLimits&       getLimits()       { return m_modifiableLimits; }
    const PhysicsSixDOFJointModifiableLimits& getLimits() const { return m_modifiableLimits; }

    void clampToLimits(NMP::Quat& orientation, float limitFrac, const NMP::Quat* origQ) const NM_OVERRIDE;
    void expandLimits(const NMP::Quat& orientation) NM_OVERRIDE;
    void scaleLimits(float scaleFactor) NM_OVERRIDE;

    void resetLimits() NM_OVERRIDE;
    void writeLimits() NM_OVERRIDE;

  protected:
#if defined(MR_OUTPUT_DEBUGGING)
    //----------------------------------------------------------------------------------------------------------------------
    void updateSerializeTxFrameData();

    PhysicsSixDOFJointFrameData   m_serializeTxFrameData; // Copy of data to be serialized for debug render.
#endif // MR_OUTPUT_DEBUGGING

    enum DirtyFlags
    {
      kDirty_Desc = (1 << 0),
      kDirty_DriveOrientation = (1 << 1)
    };

    CacheData     m_cache;
    uint32_t      m_dirtyFlags;

    NxJoint*      m_joint;

    float         m_maxTwistDamping;
    float         m_maxTwistStrength;
    float         m_maxSwingDamping;
    float         m_maxSwingStrength;
    float         m_maxSlerpDamping;
    float         m_maxSlerpStrength;

    bool          m_limitsEnabled;

    PhysicsSixDOFJointModifiableLimits     m_modifiableLimits;  // Copy of joint limits that can be modified at runtime.
    const PhysicsSixDOFJointDef* m_def;     // Pointer to jointDef (owned by parent rig).

    void applyModifiedValues(bool enableProjection, float linearTolerance, float angularTolerance);

    void generateCachedValues();
  };
  //--------------------------------------------------------------------------------------------------------------------

  /// Physics rig constructor
  PhysicsRigPhysX2();

  /// \brief Get the memory required to create an instance from the supplied PhysicsRigDef.
  static NMP::Memory::Format getMemoryRequirements(PhysicsRigDef* phyiscsRigDef);

  /// Create an instance of a PhysicsRigPhysX2.
  ///
  /// collisionTypeMask is a bit mask using MR::GameGroup, possibly some game-specific bits too, indicating the type
  /// that this rig claims to be. Would normally be at least 1 << MR::GameGroup::GROUP_CHARACTER_PART
  ///
  /// collisionIgnoreMask is a bit mask indicating what this character doesn't collide with. Will normally be at least
  /// (1 << GROUP_NON_COLLIDABLE) | (GROUP_CHARACTER_CONTROLLER)
  static PhysicsRigPhysX2 *init(
    NMP::Memory::Resource &resource,
    PhysicsRigDef         *physicsRigDef,
    PhysicsScene          *physicsScene,
    AnimRigDef            *animRigDef,
    AnimToPhysicsMap      *animToPhysicsMap,
    int32_t                collisionTypeMask,
    int32_t                collisionIgnoreMask);

  //-----------------------------------------------------------------------------------------------
  // The following member functions make up the implementation of the engine independent base class
  //-----------------------------------------------------------------------------------------------

  bool term() NM_OVERRIDE;

  /// \brief This applies any cached updates to the PhysX rig properties. It must be called just 
  ///  before updating the physics simulation.
  ///
  /// When updating networks in parallel with multi-threading caching is required because some
  /// physics API calls may be non thread safe.
  /// It follows that this operation may not be thread safe, this is dependent in the physics system.
  void updatePrePhysics(float timeStep) NM_OVERRIDE;

  /// \brief This reads PhysX rig properties in to the cache for this rig. It must be called just 
  ///  after updating the physics simulation.
  ///
  /// Caching is necessary in order to allow us to update rigs concurrently from multiple threads without directly
  /// accessing the physics API, which may not be thread safe.
  void updatePostPhysics(float timeStep) NM_OVERRIDE;

  NMP::Quat getJointQuat(uint32_t jointIndex) NM_OVERRIDE;

  void applySoftKeyframing(
    const NMP::DataBuffer& targetBuffer,
    const NMP::DataBuffer& previousTargetBuffer,
    const NMP::DataBuffer& fallbackBuffer,
    const NMP::Matrix34&   worldRoot,
    const NMP::Matrix34&   previousWorldRoot,
    bool                   enableCollision,
    bool                   enableJointLimits,
    bool                   preserveMomentum,
    float                  externalJointCompliance,
    float                  gravityCompensationFrac,
    float                  dt,
    float                  weight,
    float                  maxAccel,
    float                  maxAngAccel,
    const PartChooser&     partChooser) NM_OVERRIDE;

  /// \note previousTargetBuffer and previousWorldRoot are not used.
  virtual void applyHardKeyframing(
    const NMP::DataBuffer& targetBuffer,
    const NMP::DataBuffer* previousTargetBuffer,
    const NMP::DataBuffer& fallbackBuffer,
    const NMP::Matrix34&   worldRoot,
    const NMP::Matrix34*   previousWorldRoot,
    bool                   enableCollision,
    float                  massMultiplier,
    bool                   enableConstraint,
    float                  dt,
    const PartChooser&     partChooser) NM_OVERRIDE;

  void applyActiveAnimation(uint32_t jointIndex, const NMP::Quat& targetQuat, bool makeChildDynamic);

  void applyActiveAnimation(
    const NMP::DataBuffer& targetBuffer,
    const NMP::DataBuffer& fallbackBuffer,
    float                  strengthMultiplier,
    float                  dampingMultiplier,
    float                  internalCompliance,
    float                  externalCompliance,
    bool                   enableJointLimits,
    const JointChooser&    jointChooser,
    float                  limitClampFraction) NM_OVERRIDE;

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

  float getMaxSKDeviation() const NM_OVERRIDE;

  void requestJointProjectionParameters(int iterations, float linearTolerance, float angularTolerance) NM_OVERRIDE;

  void setKinematicPos(const NMP::Vector3&) NM_OVERRIDE;

  // Support for sleeping control
  void disableSleeping() NM_OVERRIDE;
  void reenableSleeping() NM_OVERRIDE {};

  void setSkinWidthIncrease(uint32_t partIndex, float skinWidthIncrease) NM_OVERRIDE;
  
  void scaleFrictionOnPart(const int32_t partIndex, const float frictionScale) NM_OVERRIDE;

  //-----------------------------------------------------------------------------------------------
  // The following member functions are specific to PhysicsRigPhysX2
  //-----------------------------------------------------------------------------------------------

  /// application should pass in any actor being used for capsule/character movement
  void setCharacterControllerActor(NxActor* characterControllerActor) { m_characterControllerActor = characterControllerActor; }

  /// Returns the PhysX actor used by the Character Controller associated with this physics rig.
  NxActor* getCharacterControllerActor() { return m_characterControllerActor; }
  const NxActor* getCharacterControllerActor() const { return m_characterControllerActor; }

  /// Get the PhysX2 specific scene.
  PhysicsScenePhysX2* getPhysicsScenePhysX() const { return (PhysicsScenePhysX2*)m_physicsScene; };

  /// \name Gets the PhysX2 specific part given the part index
  /// @{
  PartPhysX* getPartPhysX(uint32_t partIndex);
  const PartPhysX* getPartPhysX(uint32_t partIndex) const;
  /// @}

protected:
  void addToScene() NM_OVERRIDE;
  void removeFromScene() NM_OVERRIDE;

  /// \brief This applies any cached updates to the PhysX rig properties.
  ///
  /// When updating networks in parallel with multi-threading caching is required because some physics API calls may be non thread safe.
  void applyModifiedValues();

  /// Generate a cache of body and joint values to reduce the number of costly calls to the physx API
  void generateCachedValues();

  void makeKinematic(bool moveToKinematicPos) NM_OVERRIDE;

  void makeDynamic() NM_OVERRIDE;

  void restoreAllJointDrivesToDefault() NM_OVERRIDE;

  // Fills in m_materials with the different materials in the physicsScene
  void createMaterialsList(PhysicsRigDef* physicsRigDef, PhysicsScene* physicsScene);

  // Returns a NxMaterial that matches with materialDesc in m_materials or a new NxMaterial in other case.
  NxMaterial* getMaterial(const NxMaterialDesc& materialDesc);

  /// Position we move the actors to when we disable the body (and make it kinematic)
  NMP::Vector3        m_kinematicPos;

  // Array of pointers to the materials used by each part.  There is one NxMaterial per part
  NxMaterial**        m_materials;

  /// the actor being used for character control
  NxActor*            m_characterControllerActor;

  /// The desired (in this frame) request for resolving joint separation. Will be reset after the physics step.
  bool m_enableJointProjection;
  /// The desired (in this frame) distance tolerance for resolving joint separation. Will be reset after the physics step.
  float m_desiredJointProjectionLinearTolerance;
  /// The desired (in this frame) angular tolerance for resolving joint separation. Will be reset after the physics step.
  float m_desiredJointProjectionAngularTolerance;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_RIG_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
