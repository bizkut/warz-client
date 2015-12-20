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
#ifndef MR_PHYSICS_RIG_PHYSX3ARTICULATION_H
#define MR_PHYSICS_RIG_PHYSX3ARTICULATION_H

// Define this to enable aggregates, which should provide a performance gain. Sometimes it's useful
// to disable them when investigating PhysX bugs.
#define USE_ARTICULATION_AGGREGATE

//----------------------------------------------------------------------------------------------------------------------
#include "physics/PhysX3/mrPhysicsRigPhysX3.h"
#include "mrPhysX3Configure.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_DEBUG
  #define STORE_PART_AND_JOINT_NAMES
#endif

class PxArticulationLinkDesc;

namespace MR
{
class PhysicsRigDef;
class PhysicsScenePhysX3;
struct PhysicsSerialisationBuffer;
class AnimRigDef;

/// Handler for articulation explosions.
typedef void (*ArticulationExplosionHandler)(
  class PhysicsRigPhysX3Articulation* articulation,
  NMP::Matrix34& initTM);

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsRigPhysX3Articulation
/// \brief Container for the PhysX 3 implementation of a physics rig def.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsRigPhysX3Articulation : public PhysicsRigPhysX3
{
  enum { MAX_NAME_SIZE = 64 };
public:
  //--------------------------------------------------------------------------------------------------------------------
  class PartPhysX3Articulation : public PartPhysX3
  {
    friend class PhysicsRigPhysX3Articulation;

  public:

    struct CachedData
    {
      NMP::Matrix34 cachedTransform;
      NMP::Matrix34 cachedCOMOffset;
      NMP::Vector3 cachedAngVel;
      NMP::Vector3 cachedVel;
      NMP::Vector3 cachedCOMPosition;
    };

    PartPhysX3Articulation(int32_t defaultCollisionMask, int32_t allowedCollisionMask);
    PartPhysX3Articulation(const PartPhysX3Articulation& other);
    virtual ~PartPhysX3Articulation();
    PartPhysX3Articulation& operator=(const PartPhysX3Articulation& other);

#ifdef STORE_PART_AND_JOINT_NAMES
    const char*   getName() const { return m_name; }
#endif

    NMP::Vector3  getPosition() const NM_OVERRIDE { return m_cache.cachedTransform.translation(); }
    void          setPosition(const NMP::Vector3& p) NM_OVERRIDE;
    NMP::Quat     getQuaternion() const NM_OVERRIDE;
    void          setQuaternion(const NMP::Quat& q) NM_OVERRIDE;

    NMP::Matrix34 getTransform() const NM_OVERRIDE { return m_cache.cachedTransform; }
    
    NMP::Vector3  getVelocityAtPoint(const NMP::Vector3 &point) const NM_OVERRIDE;

    NMP::Vector3  getLinearMomentum() const;
    NMP::Vector3  getAngularMomentum() const;
    NMP::Vector3  calcFrameRelativeAngularMomentum(const NMP::Vector3 &framePos, const NMP::Vector3 &frameVel ) const;
    NMP::Vector3  calcFrameRelativeAngularMomentumCached(const NMP::Vector3 &framePos, const NMP::Vector3 &frameVel ) const 
      {return calcFrameRelativeAngularMomentum(framePos, frameVel);};

    NMP::Vector3 getMassSpaceInertiaTensor() const NM_OVERRIDE;
    /// Sets the mass-space inertia tensor. Note that modifications will be checked in the
    /// updatePrePhysics call, and if there was no modification then it will be reset to the
    /// default value.
    void setMassSpaceInertia(const NMP::Vector3& inertia);

    /// Returns the mass moment of inertia in the top 3x3 components along with the com position in the translation
    /// component
    NMP::Matrix34 getGlobalInertiaTensor() const NM_OVERRIDE;
    /// Returns the original (i.e. when created) mass-space inertia tensor (diagonalised form) of the dynamic part
    NMP::Vector3 getOriginalMassSpaceInertia() const {return m_inertia;}

    /// Returns COM offset of the part in local space
    NMP::Matrix34 getCOMOffsetLocal() const NM_OVERRIDE { return m_cache.cachedCOMOffset; }
    /// Returns COM position in world space including any extra mass
    NMP::Vector3 getAugmentedCOMPosition() const NM_OVERRIDE;
    /// Returns part mass - note that this is the original mass (even if it's been changed for HK
    /// etc) plus any extra mass registered.
    float getAugmentedMass() const NM_OVERRIDE { return m_mass + m_extraMass; }
    /// Stores extra mass associated with this part
    void setExtraMass(float mass, const NMP::Vector3& massCOMPosition) NM_OVERRIDE;
    /// Returns the mass of the part only
    float getMass() const NM_OVERRIDE { return m_mass; }
    /// Returns the centre of mass of the part only
    NMP::Vector3 getCOMPosition() const NM_OVERRIDE;

    NMP::Vector3 getVel() const NM_OVERRIDE { return m_cache.cachedVel; }
    NMP::Vector3 getAngVel() const NM_OVERRIDE { return m_cache.cachedAngVel; }

    void setVel(const NMP::Vector3 &v) NM_OVERRIDE;
    void setAngVel(const NMP::Vector3 &r) NM_OVERRIDE;
    void recalcNextVel(){ m_recalcVels = true; }

    /// This sets a velocity so that after the update the kinematic actor will
    /// have the desired pose - sweeps the collision shape, so is different to
    /// just setting the transform.
    void moveTo(const NMP::Matrix34 &tm)  NM_OVERRIDE { moveTo(tm, true); }
    void moveTo(const NMP::Matrix34 &t, bool updateCache);
    /// Sets the transform without resulting in a sweep.
    void setTransform(const NMP::Matrix34 &tm) NM_OVERRIDE;

    void enableCollision(bool enable) NM_OVERRIDE;
    bool getCollisionEnabled() const NM_OVERRIDE;

    physx::PxArticulationLink *getArticulationLink() { return (physx::PxArticulationLink*) m_rigidBody; }
    const physx::PxArticulationLink *getArticulationLink() const {return (physx::PxArticulationLink*) m_rigidBody;}
    physx::PxRigidDynamic *getKinematicActor() const {return m_kinematicActor;}

    bool storeState(MR::PhysicsSerialisationBuffer &savedState) NM_OVERRIDE;
    bool restoreState(MR::PhysicsSerialisationBuffer &savedState) NM_OVERRIDE;

#if defined(MR_OUTPUT_DEBUGGING)
    uint32_t serializeTxPersistentData(uint16_t nameToken, uint32_t objectID, void* outputBuffer, uint32_t outputBufferSize) const NM_OVERRIDE;
    uint32_t serializeTxFrameData(void* outputBuffer, uint32_t outputBufferSize) const NM_OVERRIDE;
#endif // MR_OUTPUT_DEBUGGING

    void makeKinematic(bool kinematic, float massMultiplier, bool enableConstraint) NM_OVERRIDE;
    bool isKinematic() const NM_OVERRIDE;

    /// The following Mask functions refer to word3 of the simulation filter data, where each bit
    /// indicates whether the shape is in one of the 32 allowed collision groups.

    /// The active collision groups that the part was authored to have
    uint32_t getDefaultCollisionGroupMask() const {return m_defaultCollisionGroupMask;}
    /// The collision groups that the part is allowed to have (this will be the default mask plus
    /// the collision groups that were disabled by default).
    uint32_t getAllowedCollisionGroupMask() const {return m_allowedCollisionGroupMask;}
    /// The current collision groups.
    uint32_t getCurrentCollisionGroupMask() const {return m_currentCollisionGroupMask;}
    /// This sets the collision group mask on all the shapes (to avoid needless physx api calls,
    /// check the current mask before calling this) and sets the current collision group mask.
    void setCurrentCollisionGroupMask(uint32_t mask);

    void createLink(
      physx::PxArticulation *articulation, 
      physx::PxArticulationLink* parent, 
      PhysicsRigPhysX3Articulation::PartPhysX3Articulation* part, 
      PhysicsRigPhysX3Articulation* physXBody,
      PxArticulationLinkDesc& linkDesc);

  protected:
    enum ModifiedFlags
    {
      MODIFIED_EXTRA_MASS = 1 << 0,
      MODIFIED_INERTIA    = 1 << 1
    };

    /// This will return false if it detects the simulation has exploded, in which case it will not
    /// overwrite the cached values.
    bool generateCachedValues(float timeStep);
    void enableActorCollision(physx::PxActor *actor, bool enable);

    CachedData  m_cache;

    NMP::Vector3 m_inertia;

    /// When extra mass is registered, this is the position of the extra mass
    NMP::Vector3 m_extraMassCOMPosition;
    /// Registered extra mass. This will be cleared in updatePrePhysics if it hasn't been set
    float   m_extraMass;
    /// The basic part pass (not including any extra mass)
    float   m_mass;

    /// This is the initial/default collision set mask that will go into each shape's
    /// collisionFilterData.word3. We store it so that it can be restored when collision sets are
    /// enabled dynamically.
    uint32_t m_defaultCollisionGroupMask;
    /// This is the allowed mask - when the initially disabled bits are set
    uint32_t m_allowedCollisionGroupMask;
    /// This is the current mask
    uint32_t m_currentCollisionGroupMask;

    // Kinematic actor for proper hard key framing.
    physx::PxRigidDynamic* m_kinematicActor;

    physx::PxD6Joint* m_constraintToKinematic;

    float   m_massMultiplier;
    bool    m_isKinematic;
    bool    m_recalcVels;
    /// Bitmask of ModifiedFlags showing if the application has modified values, or if they should
    /// be reset.
    uint8_t m_modifiedFlags;
#ifdef STORE_PART_AND_JOINT_NAMES
    char    m_name[MAX_NAME_SIZE];
#endif
  };
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  class JointPhysX3Articulation : public JointPhysX3
  {
    friend class PhysicsRigPhysX3Articulation;

  public:

    JointPhysX3Articulation(const PhysicsSixDOFJointDef* const def);

    // Joint overrides.
#if defined(MR_OUTPUT_DEBUGGING)
    virtual uint32_t serializeTxPersistentData(
      const PhysicsJointDef* jointDef,
      uint16_t stringToken,
      void* outputBuffer,
      uint32_t outputBufferSize) const NM_OVERRIDE;
#endif // MR_OUTPUT_DEBUGGING

#ifdef STORE_PART_AND_JOINT_NAMES
    const char* getName() const {return m_name;}
#endif
    float getMaxDamping() const {return m_maxDamping;}
    float getMaxStrength() const {return m_maxStrength;}

    float getDriveStrengthScale() const NM_OVERRIDE {return m_driveStrengthScale;}
    float getDriveDampingScale() const NM_OVERRIDE {return m_driveDampingScale;}
    float getDriveCompensationScale() const NM_OVERRIDE {return m_driveCompensationScale;}

    // This sets the internal/external compliance appropriately to provide drive compensation
    bool supportsDriveCompensation() NM_OVERRIDE;
    void setDriveCompensation(float driveCompensation) NM_OVERRIDE;
    float getDriveCompensation() const NM_OVERRIDE;
    float getExternalCompliance() const NM_OVERRIDE;
    // Note that the external compliance may get modified for leaf joints (e.g. ankles)
    void setExternalCompliance(float compliance) NM_OVERRIDE;


    float getInternalCompliance() const;
    void setInternalCompliance(float compliance);


    void setStrength(float strength) NM_OVERRIDE;
    float getStrength() const NM_OVERRIDE;

    void setDamping(float damping) NM_OVERRIDE;
    float getDamping() const NM_OVERRIDE;

    NMP::Quat getTargetOrientation() NM_OVERRIDE;
    void setTargetOrientation(const NMP::Quat &orientation) NM_OVERRIDE;
    void setVelocity(const NMP::Vector3 &velocity);
    void enableLimit(bool enable) NM_OVERRIDE;

    // Copy joint limits to physX joints.
    virtual void writeLimits() NM_OVERRIDE;

    bool storeState(MR::PhysicsSerialisationBuffer &savedState) NM_OVERRIDE;
    bool restoreState(MR::PhysicsSerialisationBuffer &savedState) NM_OVERRIDE;

    physx::PxArticulationJoint* getInternalJoint() {return m_jointInternal;}
  protected:
    float        m_maxDamping;
    float        m_maxStrength;
    float        m_driveDampingScale;
    float        m_driveCompensationScale;

#ifndef NM_HOST_64_BIT
#if defined(MR_OUTPUT_DEBUGGING)
    uint32_t padding1[3];
#endif
#else
#if !defined(MR_OUTPUT_DEBUGGING)
    uint32_t padding2[2];
#endif
#endif


    NMP::Quat     m_lastTargetOrientation;
    NMP::Quat     m_actualRotation;
#ifdef STORE_PART_AND_JOINT_NAMES
    char         m_name[MAX_NAME_SIZE];
#endif

    float        m_strength;
    float        m_damping;
    float        m_driveStrengthScale;

    physx::PxArticulationJoint *m_jointInternal;
  public:
    bool          m_rotationDirty;

  };
  //--------------------------------------------------------------------------------------------------------------------

  /// Physics rig constructor - the physics rig needs to be associated with a PhysX scene.
  PhysicsRigPhysX3Articulation(PhysicsScenePhysX3* scene);

  /// \brief Get the memory required to create an instance from the supplied PhysicsRigDef.
  static NMP::Memory::Format getMemoryRequirements(PhysicsRigDef* phyiscsRigDef);

  /// Create an instance of a PhysicsRigPhysX3.
  ///
  /// collisionTypeMask is a bit mask using MR::GameGroup, possibly some game-specific bits too, indicating the type
  /// that this rig claims to be. Would normally be at least 1 << MR::GameGroup::GROUP_CHARACTER_PART
  ///
  /// collisionIgnoreMask is a bit mask indicating what this character doesn't collide with. Will normally be at least
  /// (1 << GROUP_NON_COLLIDABLE) | (GROUP_CHARACTER_CONTROLLER)
  ///
  /// See PhysX documentation for information on the ownerClientID and clientBehaviourBits
  static PhysicsRigPhysX3Articulation* init(
    NMP::Memory::Resource& resource,
    PhysicsRigDef*         physicsRigDef,
    PhysicsScene*          physicsScene,
    physx::PxClientID      ownerClientID,
    uint32_t               clientBehaviourBits,
    AnimRigDef*            rig,
    AnimToPhysicsMap      *animToPhysicsMap,
    int32_t                collisionTypeMask,
    int32_t                collisionIgnoreMask);

  bool term() NM_OVERRIDE;

  void updatePrePhysics(float timeStep) NM_OVERRIDE;
  void updatePostPhysics(float timeStep) NM_OVERRIDE;

  /// Register a handler for when the articulation simulation explodes.
  static void setExplosionHandler(ArticulationExplosionHandler* handler);

  /// This is the default explosion handler - it will be called if there is no handler registered,
  /// or it can be called from the registered handler if desired.
  void handleExplosion(const NMP::Matrix34& worldRoot);


  /// Sets the AA target for an individual joint. targetQuat represents a rotation of the    
  /// joint limit frame (not the bodies)    
  /// makeChildDynamic indicates if the joint child body should be made dynamic if it is    
  /// currently kinematic    
  /// Note that the target velocity is just added on to the orientation at the moment, later we will want an 
  /// angular velocity drive to achieve this more cleanly    
  void applyActiveAnimation(uint32_t jointIndex, const NMP::Quat &targetQuat, bool makeChildDynamic); 

  void setArticulation(physx::PxArticulation *art){ m_articulation = art; }
  physx::PxArticulation *getArticulation() const { return m_articulation; }
  /// sets the values in the alpha buffer so that 1 means non-physical and 0 means physical for each animation channel
  /// buffer should already be created with the same number of channels as the rig
  void setAlphaBuffer(bool *alphaValues, int32_t n_alphaValues);

  /// Get the joint orientation as a quaternion
  NMP::Quat getJointQuat(uint32_t jointIndex) NM_OVERRIDE;

  /// Write out the physics rig to a repx collection
  void dumpToRepX(physx::repx::RepXCollection *collection, physx::repx::RepXIdToRepXObjectMap *idMap) const NM_OVERRIDE;
  
  void applySoftKeyframing(
    const NMP::DataBuffer &targetBuffer,
    const NMP::DataBuffer &previousTargetBuffer,
    const NMP::DataBuffer &fallbackBuffer,
    const NMP::Matrix34   &worldRoot,
    const NMP::Matrix34   &previousWorldRoot,
    bool                   enableCollision,
    bool                   enableJointLimits,
    bool                   preserveMomentum,
    float                  externalJointCompliance,
    float                  gravityCompensationFrac,
    float                  dt,
    float                  weight,
    float                  maxAccel,
    float                  maxAngAccel,
    const PartChooser     &partChooser) NM_OVERRIDE;

  virtual void applyHardKeyframing(
    const NMP::DataBuffer &targetBuffer,
    const NMP::DataBuffer *previousTargetBuffer,
    const NMP::DataBuffer &fallbackBuffer,
    const NMP::Matrix34   &worldRoot,
    const NMP::Matrix34   *previousWorldRoot,
    bool                   enableCollision,
    float                  massMultiplier,
    bool                   enableConstraint,
    float                  dt,
    const PartChooser     &partChooser) NM_OVERRIDE;

  void applyActiveAnimation(
    const NMP::DataBuffer &targetBuffer,
    const NMP::DataBuffer& fallbackBuffer,
    float                  strengthMultiplier,
    float                  dampingMultiplier,
    float                  internalCompliance,
    float                  externalCompliance,
    bool                   enableJointLimits,
    const JointChooser    &jointChooser,
    float                  limitClampFraction) NM_OVERRIDE;

  void disableSleeping() NM_OVERRIDE;
  void reenableSleeping() NM_OVERRIDE;

  // These accessors are helpers to make client code that uses the PhysX3 rig clearer
  PartPhysX3Articulation *getPartPhysX3Articulation(uint32_t index) {return (PartPhysX3Articulation*)getPart(index);};
  const PartPhysX3Articulation *getPartPhysX3Articulation(uint32_t index) const {return (PartPhysX3Articulation*)getPart(index);};
  JointPhysX3Articulation *getJointPhysX3Articulation(uint32_t index) {return (JointPhysX3Articulation*)getJoint(index);};
  const JointPhysX3Articulation *getJointPhysX3Articulation(uint32_t index) const {return (JointPhysX3Articulation*)getJoint(index);};

  void setCollisionGroupsToActivate(const int *collisionGroupIndices, int numCollisionGroupIndices);

protected:
  /// Generate a cache of body and joint values to reduce the number of costly calls to the physx API
  void generateCachedValues(float timeStep);

  void makeKinematic(bool moveToKinematicPos) NM_OVERRIDE;
  void makeDynamic() NM_OVERRIDE;
  void removeFromScene() NM_OVERRIDE;
  void addToScene() NM_OVERRIDE;

  void restoreAllJointDrivesToDefault() NM_OVERRIDE;

private:
  /// Moves all kinematic and dynamic parts to the kinematic position and disables them completely.
  void moveAllToKinematicPos();

  static void createJoints(PhysicsScenePhysX3 *physicsScene, PhysicsRigDef *physicsRigsDef, 
    PhysicsRigPhysX3Articulation *physXBody, PxArticulationLinkDesc **links);

  /// Adds the articulation to the scene if it's not already added.
  void addArticulationToScene();
  /// Removes the articulation from the scene if it's been added
  void removeArticulationFromScene();

  physx::PxArticulation     *m_articulation;
#ifdef USE_ARTICULATION_AGGREGATE
  physx::PxAggregate        *m_aggregate;
#endif

  static ArticulationExplosionHandler* s_explosionHandler;

  /// Bit mask using MR::GameGroup, possibly some game-specific bits too, indicating the type that this rig claims
  /// to be. Would normally be at least 1 << MR::GameGroup::GROUP_CHARACTER_PART
  int32_t             m_collisionTypeMask;
  /// Bit mask indicating what this character doesn't collide with. 
  /// Will normally be at least (1 << GROUP_NON_COLLIDABLE) | (GROUP_CHARACTER_CONTROLLER)
  int32_t             m_collisionIgnoreMask;

  static const int    m_maxCollisionGroupIndices = 32; // 32 bits available!
  int                 m_collisionGroupIndicesToActivate[m_maxCollisionGroupIndices];
  int                 m_numCollisionGroupIndices;
  float               m_cachedSleepThreshold; // So clients can set their own threshold and we can disable it
  /// Used in the optimisation of not adding articulation to scene in full-body HK
  bool                m_isArticulationAddedToScene; 
};


} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_RIG_PHYSX3ARTICULATION_H
//----------------------------------------------------------------------------------------------------------------------
