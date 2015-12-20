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
#ifndef MR_PHYSICS_RIG_PHYSX3JOINTED_H
#define MR_PHYSICS_RIG_PHYSX3JOINTED_H
//----------------------------------------------------------------------------------------------------------------------
#include "physics/PhysX3/mrPhysicsRigPhysX3.h"
#include "mrPhysX3Includes.h"
#include "mrPhysX3Configure.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class PhysicsRigDef;
class PhysicsScenePhysX3;
class AnimRigDef;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsRigPhysX3Jointed
/// \brief Container for the PhysX 3 implementation of a jointed physics rig
//----------------------------------------------------------------------------------------------------------------------
class PhysicsRigPhysX3Jointed : public PhysicsRigPhysX3
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  class PartPhysX3Jointed : public PartPhysX3
  {
    friend class PhysicsRigPhysX3Jointed;

  public:
    /// \name Returns the RigidDynamic physx actor that simulates this part
    /// @{
    physx::PxRigidDynamic* getRigidDynamic() { return (physx::PxRigidDynamic*) m_rigidBody; }
    const physx::PxRigidDynamic* getRigidDynamic() const { return (physx::PxRigidDynamic*) m_rigidBody; }
    /// @}

    //-----------------------------------------------------------------------------------------------
    // The following member functions make up the implementation of the engine independent base class
    //-----------------------------------------------------------------------------------------------

    NMP::Vector3  getPosition() const NM_OVERRIDE;
    void          setPosition(const NMP::Vector3& p) NM_OVERRIDE;
    NMP::Quat     getQuaternion() const NM_OVERRIDE;
    void          setQuaternion(const NMP::Quat& q) NM_OVERRIDE;

    NMP::Matrix34 getTransform() const NM_OVERRIDE;
    void          setTransform(const NMP::Matrix34& tm) NM_OVERRIDE;
    void          moveTo(const NMP::Matrix34& t) NM_OVERRIDE;

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

    bool          storeState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;
    bool          restoreState(MR::PhysicsSerialisationBuffer& savedState) NM_OVERRIDE;

#if defined(MR_OUTPUT_DEBUGGING)
    uint32_t serializeTxPersistentData(uint16_t nameToken, uint32_t objectID, void* outputBuffer, uint32_t outputBufferSize) const NM_OVERRIDE;
    uint32_t serializeTxFrameData(void* outputBuffer, uint32_t outputBufferSize) const NM_OVERRIDE;
#endif // MR_OUTPUT_DEBUGGING

  protected:
    enum DirtyFlags
    {
      kDirty_GlobalPose = (1 << 0),
      kDirty_LinearVel = (1 << 1),
      kDirty_AngularVel = (1 << 2),
      kDirty_Collision = (1 << 3),
      kDirty_BodyFlags = (1 << 4),
      kDirty_KinematicTarget = (1 << 5),
    };

    struct CachedData
    {
      NMP::Vector3  COMPosition;
      NMP::Matrix34 COMOffsetLocal;
      NMP::Matrix34 globalPose;
      NMP::Matrix34 kinematicTarget;
      NMP::Vector3 linearVel;
      NMP::Vector3 angularVel;
      float mass;
      uint16_t bodyFlags;
      bool collisionOn;
    };

    /// \name protected as the part should only be created etc by the relevant physics rig
    /// @{
    PartPhysX3Jointed();
    PartPhysX3Jointed(const PartPhysX3Jointed& other);
    ~PartPhysX3Jointed();
    PartPhysX3Jointed& operator=(const PartPhysX3Jointed& other);
    /// @}

    // The cache is used to reduce the number of costly calls to the physx API
    void generateCachedValues();

    // Apply the cached modified values.
    void applyModifiedValues();

    void updateCOMPosition();

    const CachedData& getCachedData() { return m_cache; }
    void setCachedData(const CachedData& cache) { m_cache = cache; }

    CachedData m_cache;
    uint32_t   m_dirtyFlags;
  };
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  class JointPhysX3Jointed : public JointPhysX3
  {
    friend class PhysicsRigPhysX3Jointed;

  public:
    JointPhysX3Jointed(const PhysicsSixDOFJointDef* const def);

    //-----------------------------------------------------------------------------------------------
    // The following member functions make up the implementation of the engine independent base class
    //-----------------------------------------------------------------------------------------------

#if defined(MR_OUTPUT_DEBUGGING)
    virtual uint32_t serializeTxPersistentData(
      const MR::PhysicsJointDef* jointDef,
      uint16_t stringToken,
      void* outputBuffer,
      uint32_t outputBufferSize) const NM_OVERRIDE;
#endif // MR_OUTPUT_DEBUGGING

    //-----------------------------------------------------------------------------------------------
    // The following member functions are specific to this class
    //-----------------------------------------------------------------------------------------------

    /// \name Get the joint drive properties
    /// @{
    float getMaxTwistDamping() const { return m_maxTwistDamping; }
    float getMaxTwistStrength() const { return m_maxTwistStrength; }
    float getMaxSwingDamping() const { return m_maxSwingDamping; }
    float getMaxSwingStrength() const { return m_maxSwingStrength; }
    float getMaxSlerpDamping() const { return m_maxSlerpDamping; }
    float getMaxSlerpStrength() const { return m_maxSlerpStrength; }
    /// @}

    /// Returns the joint rotation between the two parts (using the joint frames)
    NMP::Quat getRotation(const MR::PhysicsJointDef* jointDef, const NMP::Matrix34& part1TM, const NMP::Matrix34& part2TM) const;

    /// Enables/disables the joint limit
    void enableLimit(bool enable);

    virtual void writeLimits() NM_OVERRIDE;

    /// \name Set the joint drive properties
    /// @{
    void setDriveStrength(float twistStrength, float swingStrength, float slerpStrength);
    void setDriveDamping(float twistDamping, float swingDamping, float slerpDamping);
    void setDriveOrientation(const NMP::Quat &quat);
    /// @}

    // TODO: Rationalise these with the above setDriveStrength etc, if possible
    void setStrength(float strength) NM_OVERRIDE;
    float getStrength() const NM_OVERRIDE;

    void setDamping(float damping) NM_OVERRIDE;
    float getDamping() const NM_OVERRIDE;

    NMP::Quat getTargetOrientation() NM_OVERRIDE;
    void setTargetOrientation(const NMP::Quat &orientation) NM_OVERRIDE;

    /// Store the state internal and physx (not currently implemented)
    bool storeState(MR::PhysicsSerialisationBuffer& savedState);
    /// Restore the state internal and physx (not currently implemented)
    bool restoreState(MR::PhysicsSerialisationBuffer& savedState);

  protected:
    enum DirtyFlags
    {
      kDirty_SwingDrive = (1 << 0),
      kDirty_TwistDrive = (1 << 1),
      kDirty_SlerpDrive = (1 << 2),
      kDirty_Limits    =  (1 << 3),
      kDirty_DriveOrientation = (1 << 4)
    };

    struct CachedData
    {
      CachedData() :
        swingDrive(0, 0, 0),
        twistDrive(0, 0, 0),
        slerpDrive(0, 0, 0)
      {
      }

      NMP::Quat driveOrientation;
      physx::PxD6Motion::Enum motions[physx::PxD6Axis::eCOUNT];
      physx::PxD6JointDrive swingDrive;
      physx::PxD6JointDrive twistDrive;
      physx::PxD6JointDrive slerpDrive;
    };

    /// \name Internal functions for manipulating the cache
    /// @{
    const CachedData& getCachedData() { return m_cache; }
    void setCachedData(const CachedData& cache) { m_cache = cache; }
    /// @}

    ///
    void generateCachedValues();

    ///
    void applyModifiedValues();

    physx::PxD6Joint*  m_joint;

    float        m_maxTwistDamping;
    float        m_maxTwistStrength;
    float        m_maxSwingDamping;
    float        m_maxSwingStrength;
    float        m_maxSlerpDamping;
    float        m_maxSlerpStrength;

    bool         m_limitsEnabled;
    CachedData   m_cache;
    uint32_t     m_dirtyFlags;
  };
  //--------------------------------------------------------------------------------------------------------------------


  /// \brief Get the memory required to create an instance from the supplied PhysicsRigDef.
  static NMP::Memory::Format getMemoryRequirements(PhysicsRigDef* phyiscsRigDef);

  /// Create an instance of a PhysicsRigPhysX3Jointed.
  ///
  /// collisionTypeMask is a bit mask using MR::GameGroup, possibly some game-specific bits too, indicating the type
  /// that this rig claims to be. Would normally be at least 1 << MR::GameGroup::GROUP_CHARACTER_PART
  ///
  /// collisionIgnoreMask is a bit mask indicating what this character doesn't collide with. Will normally be at least
  /// (1 << GROUP_NON_COLLIDABLE) | (GROUP_CHARACTER_CONTROLLER)
  ///
  /// See PhysX documentation for information on the ownerClientID and clientBehaviourBits
  static PhysicsRigPhysX3Jointed *init(
    NMP::Memory::Resource &resource,
    PhysicsRigDef         *physicsRigDef,
    PhysicsScene          *physicsScene,
    physx::PxClientID      ownerClientID,
    uint32_t               clientBehaviourBits,
    AnimRigDef            *animRigDef,
    AnimToPhysicsMap      *animToPhysicsMap,
    int32_t                collisionTypeMask,
    int32_t                collisionIgnoreMask);

  //-----------------------------------------------------------------------------------------------
  // The following member functions make up the implementation of the engine independent base class
  //-----------------------------------------------------------------------------------------------

  bool term() NM_OVERRIDE;

  void updatePrePhysics(float timeStep) NM_OVERRIDE;
  void updatePostPhysics(float timeStep) NM_OVERRIDE;

  /// Get the joint orientation as a quaternion
  NMP::Quat getJointQuat(uint32_t jointIndex) NM_OVERRIDE;

  /// Write out the physics rig to a repx collection
  void dumpToRepX(physx::repx::RepXCollection *collection, physx::repx::RepXIdToRepXObjectMap *idMap) const NM_OVERRIDE;

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

  void applyActiveAnimation(uint32_t jointIndex, const NMP::Quat& targetQuat, bool makeChildDynamic) NM_OVERRIDE;

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

  /// Support for sleeping control
  void disableSleeping() NM_OVERRIDE;
  void reenableSleeping() NM_OVERRIDE {};

  /// \n\me These accessors are helpers to make client code that uses the PhysX3 rig clearer
  /// @{
  PartPhysX3Jointed *getPartPhysXJointed(uint32_t index) {return (PartPhysX3Jointed*)getPart(index);};
  const PartPhysX3Jointed *getPartPhysXJointed(uint32_t index) const {return (PartPhysX3Jointed*)getPart(index);};
  JointPhysX3Jointed *getJointPhysXJointed(uint32_t index) {return (JointPhysX3Jointed*)getJoint(index);};
  const JointPhysX3Jointed *getJointPhysXJointed(uint32_t index) const {return (JointPhysX3Jointed*)getJoint(index);};
  /// @}

protected:
  //-----------------------------------------------------------------------------------------------
  // The following member functions make up the implementation of the engine independent base class
  //-----------------------------------------------------------------------------------------------
  void makeKinematic(bool moveToKinematicPos) NM_OVERRIDE;
  void makeDynamic() NM_OVERRIDE;
  void addToScene() NM_OVERRIDE;
  void removeFromScene() NM_OVERRIDE;
  void restoreAllJointDrivesToDefault() NM_OVERRIDE;

  /// Physics rig constructor - only called internally
  PhysicsRigPhysX3Jointed(PhysicsScenePhysX3 *physicsScene);

  /// Generate a cache of body and joint values to reduce the number of costly calls to the physx API
  void generateCachedValues();

  /// Apply the cached modified values in one shot.
  void applyModifiedValues();

  /// Bit mask using MR::GameGroup, possibly some game-specific bits too, indicating the type that this rig claims
  /// to be. Would normally be at least 1 << MR::GameGroup::GROUP_CHARACTER_PART
  int32_t             m_collisionTypeMask;
  /// Bit mask indicating what this character doesn't collide with. 
  /// Will normally be at least (1 << GROUP_NON_COLLIDABLE) | (GROUP_CHARACTER_CONTROLLER)
  int32_t             m_collisionIgnoreMask;

  static const float s_limitContactAngle;// this is like a limit "skin width"
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_RIG_PHYSX3JOINTED_H
//----------------------------------------------------------------------------------------------------------------------
