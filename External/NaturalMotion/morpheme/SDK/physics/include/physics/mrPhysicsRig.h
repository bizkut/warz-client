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
#ifndef MR_PHYSICS_RIG_H
#define MR_PHYSICS_RIG_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBuffer.h"
#include "physics/mrPhysicsRigDef.h"
#include "physics/mrPhysicsAttribData.h"

#include "NMPlatform/NMvpu.h"
//----------------------------------------------------------------------------------------------------------------------

#if defined(NM_HOST_64_BIT) && defined(NM_COMPILER_MSVC) 
#pragma warning (push)
#pragma warning (disable : 4324)
#endif // defined(NM_HOST_64_BIT) && defined(NM_COMPILER_MSVC) 

namespace MR
{

class PhysicsScene;
class AnimToPhysicsMap;
class AnimRigDef;
class Dispatcher;
struct PhysicsSerialisationBuffer;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsRig
/// \brief This class represents an instance of a character's physical body.  It is instanced from a PhysicsRigDef.
///  It provides a virtual interface and physics-engine agnostic implementation of the functionality required to
///  create a physics rig for use with morpheme.  The morpheme physics plug-in always interfaces with this base class
///  when dealing with physics rigs.  As part of a physics driver for morpheme, physics engine users need to create
//   a concrete implementation of this class specific to their engine.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsRig
{
protected:
  PhysicsRig();

public:
  //----------------------------------------------------------------------------------------------------------------------
  /// \class MR::PhysicsRig::Part
  /// \brief Provides a virtual interface and physics-engine agnostic implementation of the functionality required to
  ///  create a physics rig part for use with morpheme.  The morpheme physics plug-in always interfaces with this base
  ///  class when dealing with physics rig parts.  As part of a physics driver for morpheme, physics engine users need to
  //   create a concrete implementation of this class specific to their engine.
  //----------------------------------------------------------------------------------------------------------------------
  class Part
  {
    friend class PhysicsRig;
  public:

    ///------------------------------------------------------------------------------------------------------------
    /// This is the collection of functions that an engine driver derivation of the physics rig needs to implement
    ///------------------------------------------------------------------------------------------------------------

    virtual NMP::Vector3  getPosition() const = 0;
    virtual void          setPosition(const NMP::Vector3& p) = 0;
    virtual NMP::Quat     getQuaternion() const = 0;
    virtual void          setQuaternion(const NMP::Quat& q) = 0;

    virtual NMP::Matrix34 getTransform() const = 0;
    /// \brief Sets the transform without resulting in a sweep.
    virtual void          setTransform(const NMP::Matrix34& tm) = 0;
    /// \brief This sets a velocity so that after the update the kinematic actor will have the desired pose - sweeps
    ///  the collision shape, so is different to just setting the transform.
    virtual void          moveTo(const NMP::Matrix34& t) = 0;

    /// \brief Get the linear velocity of the Part CoM.
    virtual NMP::Vector3  getVel() const = 0;
    /// \brief Set the linear velocity of the Part CoM.
    virtual void          setVel(const NMP::Vector3& v) = 0;
    /// \brief Get the angular velocity of the Part.
    virtual NMP::Vector3  getAngVel() const = 0;
    /// \brief Set the angular velocity of the Part.
    virtual void          setAngVel(const NMP::Vector3& r) = 0;
    /// \brief Get the linear velocity of the Part at a given point.  Note that this may be different to the linear
    ///  velocity at the CoM if the Part has an angular velocity component too.
    virtual NMP::Vector3  getVelocityAtPoint(const NMP::Vector3& point) const = 0;

    virtual NMP::Vector3  getLinearMomentum() const = 0;
    virtual NMP::Vector3  getAngularMomentum() const = 0;
    /// \brief Returns the mass of the part.  Note that this will return the actual dynamic mass of the part even if
    ///  the underlying implementation has to set it to another value for kinematics or other effects.
    virtual float         getMass() const = 0;

    /// \brief Returns the mass moment of inertia in the top 3x3 components along with the CoM position in the
    ///  translation component.
    virtual NMP::Matrix34 getGlobalInertiaTensor() const = 0;
    /// \brief Returns inertia tensor in the principle coordinates of the mass
    virtual NMP::Vector3 getMassSpaceInertiaTensor() const = 0;

    /// \brief Returns COM position in world space
    virtual NMP::Vector3  getCOMPosition() const = 0;
    /// \brief Returns COM offset in local space
    virtual NMP::Matrix34 getCOMOffsetLocal() const = 0;

    /// \brief Makes the part either (effectively) kinematic or dynamic. If kinematic it should set
    ///        the properties if appropriate.
    virtual void          makeKinematic(bool kinematic, float massMultiplier, bool enableConstraint) = 0;
    /// \brief Returns true if the part is effectively kinematic
    virtual bool          isKinematic() const = 0;

    virtual void          enableCollision(bool enable) = 0;
    virtual bool          getCollisionEnabled() const = 0;

    /// This returns the deviation of the part position from its SK target, or 0 if it's not being
    /// soft keyframed.
    virtual float         getSKDeviation() const = 0;
    /// This returns the angular deviation (in radians) of the part position from its SK target, or
    /// 0 if it's not being soft keyframed.
    virtual float         getSKDeviationAngle() const = 0;

    /// \brief Apply a velocity change at the world space point, with an optional multiplier for the angular amount
    virtual void          addVelocityChange(const NMP::Vector3& velChange, const NMP::Vector3& worldPos, float angularMultiplier = 1.0f) = 0;
    /// \brief Apply an impulse to the part: impulse coordinates are relative and with respect to global frame
    virtual void          addImpulse(const NMP::Vector3 &impulse) = 0;
    /// \brief Apply a pure torqueImpulse (i.e. as if via a couple): torqueImpulse coordinates with respect to global frame
    virtual void          addTorqueImpulse(const NMP::Vector3& torqueImpulse) = 0;
    /// \brief Apply a pure torque (i.e. as if via a couple): torque coordinates with respect to global frame
    virtual void          addTorque(const NMP::Vector3& torque) = 0;
    /// \brief Apply a force to the actor: force is in the global frame
    virtual void          addForce(const NMP::Vector3 &force) = 0;
    /// \brief Apply a linear velocity change
    virtual void          addLinearVelocityChange(const NMP::Vector3& velChange) = 0;
    /// \brief Apply an angular acceleration.
    virtual void          addAngularAcceleration(const NMP::Vector3& angularAcceleration) = 0;

    /// \brief Writes the state of the body into the supplied serialisation buffer, for later restoration.
    virtual bool          storeState(MR::PhysicsSerialisationBuffer& savedState) = 0;
    /// \brief Reads the state of the body from the supplied serialisation buffer.
    virtual bool          restoreState(MR::PhysicsSerialisationBuffer& savedState) = 0;

#if defined(MR_OUTPUT_DEBUGGING)
    virtual uint32_t serializeTxPersistentData(uint16_t stringToken, uint32_t objectID, void* outputBuffer, uint32_t outputBufferSize) const = 0;
    virtual uint32_t serializeTxFrameData(void* outputBuffer, uint32_t outputBufferSize) const = 0;
#endif // MR_OUTPUT_DEBUGGING

    ///-----------------------------------------------------------------------------------------------------------------
    /// This is the collection of functions that are provided by the base class
    ///-----------------------------------------------------------------------------------------------------------------

    /// \brief Calculate the position of the Part in a given time, assuming that the velocity stays constant.
    NMP::Vector3          getPredictedPosition(float predTime) const;
    /// \brief Calculate the orientation of the Part in a given time, assuming that the velocity stays constant.
    NMP::Quat             getPredictedQuaternion(float predTime) const;

    /// \brief Returns the PhysicsRig that this part belongs to.
    PhysicsRig*           getOwningPhysicsRig() { return m_physicsRig; }
    PhysicsRig*           getOwningPhysicsRig() const { return m_physicsRig; }

    /// \brief Return the index of the Part which this Part is parented to.  Note: Parent part index will be -1 if
    ///  this Part is the root of the PhysicsRig
    int32_t getParentPartIndex() const { return m_parentPartIndex; }
    /// \brief Set the index of the Part which this Part is parented to.  Note: set this index to -1 if this Part
    ///  is the root of the PhysicsRig
    void setParentPartIndex(int32_t parentPartIndex) { m_parentPartIndex = parentPartIndex; }

    /// \brief Get the User data pointer.  This is a pointer to application owned data. Note that when using
    ///  Euphoria this will point to a Euphoria structure - application user data can be accessed from there.
    void* getUserData() const { return m_userData; }
    /// \brief Set the User data pointer.  This is a pointer to application owned data. Note that when using
    ///  Euphoria this will point to a Euphoria structure - application user data can be accessed from there.
    void setUserData(void* userData) { m_userData = userData; }

    /// \brief Indicates if the part is currently being key-framed.  It will be set true if the part is having HK or SK
    ///  applied.
    bool isBeingKeyframed() const {return m_isBeingKeyframed;}

    /// This provides a mechanism for the user to register an increases in the effective mass of the
    /// part - e.g. if the part is attached to a heavy object, entities using getAugmentedMass will
    /// be able to compensate. If the derived class handles "extra mass" then it must override this.
    virtual void setExtraMass(float NMP_UNUSED(extraMass), const NMP::Vector3& NMP_UNUSED(massCOMPosition)) {}

    /// When implemented this should return the part's mass, including any extra contributions. If
    /// the derived class handles "extra mass" then it must override this.
    virtual float getAugmentedMass() const {return getMass();}

    /// When implemented fully this should return the part's COM position, including any extra mass
    /// contributions. If the derived class handles "extra mass" then it must override this.
    virtual NMP::Vector3 getAugmentedCOMPosition() const {return getCOMPosition();}

  protected:
    virtual ~Part() {};

    PhysicsRig* m_physicsRig;

    /// Our parent (can be -1 if we're the root)
    int32_t     m_parentPartIndex;

    /// Pointer to application owned data. Note that when using Euphoria this will point to
    /// a Euphoria structure - application user data can be accessed from there.
    void* m_userData;

    bool    m_isBeingKeyframed; // true for hard or soft keyframing.
  };

  //----------------------------------------------------------------------------------------------------------------------
  /// \class MR::PhysicsRig::Joint
  /// \brief Provides a virtual interface and physics-engine agnostic implementation of the functionality required to
  ///  create a physics rig joint for use with morpheme.  The morpheme physics plug-in always interfaces with this base
  ///  class when dealing with physics rig joints.  As part of a physics driver for morpheme, physics engine users need to
  //   create a concrete implementation of this class specific to their engine.
  //----------------------------------------------------------------------------------------------------------------------
  class Joint
  {
  public:

    ///------------------------------------------------------------------------------------------------------------
    /// This is the collection of functions that an engine driver derivation of the physics rig needs to implement
    ///------------------------------------------------------------------------------------------------------------

#if defined(MR_OUTPUT_DEBUGGING)
    virtual uint32_t serializeTxPersistentData(
      const PhysicsJointDef* jointDef,
      uint16_t stringToken,
      void* outputBuffer,
      uint32_t outputBufferSize) const = 0;
    virtual uint32_t serializeTxFrameData(void* outputBuffer, uint32_t outputBufferSize) const = 0;
#endif // MR_OUTPUT_DEBUGGING

    virtual void setStrength(float strength) = 0;
    virtual float getStrength() const = 0;

    virtual void setDamping(float damping) = 0;
    virtual float getDamping() const = 0;

    virtual NMP::Quat getTargetOrientation() = 0;
    virtual void setTargetOrientation(const NMP::Quat &orientation) = 0;

    /// \brief Modifies orientation, the rotation from parent to child joint body, to comply with this joint's hard
    ///  limits. The limits are multiplied by limitFrac to allow conservative limiting. If origQ is supplied and found
    ///  to be outside the joint limits then the limits are temporarily expanded to include origQ before orientation is
    ///  clamped. This is used to prevent rotations going even further outside the limits, without snapping them back.
    virtual void clampToLimits(NMP::Quat& orientation, float limitFrac, const NMP::Quat* origQ) const = 0;

    /// \brief Modifies this joint's hard limits, expanding them so that they are at least large enough to include 
    /// orientation, the rotation from parent to child joint body.
    virtual void expandLimits(const NMP::Quat& orientation) = 0;

    /// \brief Modifies this joint's hard limits,scaling them by \a scaleFactor.
    virtual void scaleLimits(float scaleFactor) = 0;

    /// \brief Overwrite limit values with those in joint definition.
    virtual void resetLimits() = 0;

    /// \brief Write limit values to the physics rig representation in the physics engine.
    virtual void writeLimits() = 0;

    // \brief Enable or disable the joint limit.
    virtual void enableLimit(bool enable) = 0;

    /// \brief Writes the state of the joint into the supplied serialisation buffer, for later restoration.
    virtual bool storeState(MR::PhysicsSerialisationBuffer &savedState) = 0;
    /// \brief Reads the state of the joint from the supplied serialisation buffer.
    virtual bool restoreState(MR::PhysicsSerialisationBuffer &savedState) = 0;

    ///------------------------------------------------------------------------------------------------------------
    /// This is the collection of functions that are provided by the base class
    ///------------------------------------------------------------------------------------------------------------

    float getDriveMinDampingScale() const { return m_driveMinDampingScale; }

    // This sets the internal/external compliance appropriately to provide drive compensation
    // Note that by default drive compensation is not supported and the set/get functions should not be called.
    virtual bool supportsDriveCompensation();
    virtual void setDriveCompensation(float driveCompensation);
    virtual float getDriveCompensation() const;
    virtual float getExternalCompliance() const;
    // Note that the external compliance may get modified for leaf joints (e.g. ankles)
    virtual void setExternalCompliance(float compliance);

    // Note: these can be overridem if they are used by a given physics engine
    virtual float getDriveStrengthScale() const {return 1.0f;}
    virtual float getDriveDampingScale() const {return 1.0f;}
    virtual float getDriveCompensationScale() const {return 1.0f;}

  protected:

    virtual ~Joint() {};

    float        m_driveMinDampingScale;
#ifndef NM_HOST_64_BIT
    uint32_t pad0[1];
#endif
  };

  //----------------------------------------------------------------------------------------------------------------------
  /// \class MR::PhysicsRig::PartChooser
  /// \brief Callback class used with some physics functionality if it is only required to operate on certain rig parts.
  //----------------------------------------------------------------------------------------------------------------------
  class PartChooser
  {
  public:
    PartChooser(class AttribDataBoolArray* mask, class AnimToPhysicsMap* am) : m_mask(mask), m_am(am) {}
    bool usePart(int32_t index) const;
    const class AttribDataBoolArray*             m_mask;
    class AnimToPhysicsMap*                      m_am;
  };

  //----------------------------------------------------------------------------------------------------------------------
  /// \class MR::PhysicsRig::PartChooser
  /// \brief Callback class used with some physics functionality if it is only required to operate on certain rig joints.
  //----------------------------------------------------------------------------------------------------------------------
  class JointChooser
  {
  public:
    JointChooser(AttribDataBoolArray* mask, PhysicsRigDef* physicsRigDef, AnimToPhysicsMap* am) :
      m_mask(mask),
      m_physicsRigDef(physicsRigDef),
      m_am(am) { }
    bool useJoint(int32_t index) const;
    const AttribDataBoolArray*            m_mask;
    PhysicsRigDef*                        m_physicsRigDef;
    AnimToPhysicsMap*                     m_am;
  };

  virtual ~PhysicsRig() {};

  ///------------------------------------------------------------------------------------------------------------
  /// This is the collection of functions that an engine driver derivation of the physics rig needs to implement
  ///------------------------------------------------------------------------------------------------------------

  virtual bool term() = 0;

  /// Calculates a root position/orientation based on the part positions etc. If originalRoot is set then the original
  /// root (at the start of a transition to physics) is copied/set. physicsRootFraction is the fraction between the
  /// original root and the one calculated from physics This is done so that any camera that tracks the root will adjust
  /// its focus gradually - it won't affect any real rig output
  virtual NMP::Matrix34 getRoot(
    NMP::Matrix34*          originalRoot,
    float                   physicsRootFraction,
    CharacterControllerDef* characterControllerDef);

  /// \brief Application should call this (if it's not using LiveLink) to allow the rig to update its state
  /// immediately before the physics update.
  ///
  /// At this point any cached rig values should be applied to physics system.
  /// When updating networks in parallel with multi-threading caching is required because some
  /// physics API calls may be non thread safe.
  /// It follows that this operation may not be thread safe, this is dependent in the physics system.
  virtual void updatePrePhysics(float timeStep) = 0;

  /// \brief Application should call this (if it's not using LiveLink) to allow the rig to update its state (e.g. cached data)
  /// after the physics update.
  ///
  /// At this point any cached rig values should be updated from the physics system.
  /// Caching is necessary in order to allow us to update rigs concurrently from multiple threads without directly
  /// accessing the physics API, which may not be thread safe.
  virtual void updatePostPhysics(float timeStep) = 0;

#if defined(NMP_PLATFORM_SIMD) && !defined(NM_HOST_IOS)
  /// The fallbackBuffer is used for missing channels.
  /// It's required to have all channels set.
  void calculateWorldSpacePartTMsCache(  
    NMP::vpu::Matrix*          TMcache,
    const NMP::DataBuffer&     inputBuffer,
    const NMP::DataBuffer&     fallbackBuffer,
    const NMP::Matrix34       &worldRoot
    );

  /// The fallbackBuffer is used for missing channels.
  /// It's required to have all channels set.
  void calculateWorldSpacePartTMsCacheWithVelocity(  
    NMP::vpu::Matrix*          TMcache,
    NMP::vpu::Matrix*          TMcacheOld,
    const NMP::DataBuffer&     inputBuffer,
    const NMP::DataBuffer&     inputBufferOld,
    const NMP::DataBuffer&     fallbackBuffer,
    const NMP::Matrix34       &worldRoot,
    const NMP::Matrix34       &previousWorldRoot
    );
#else
  /// The fallbackBuffer is used for missing channels.
  /// It's required to have all channels set.
  void calculateWorldSpacePartTMsCache(  
    NMP::Matrix34*          TMcache,
    const NMP::DataBuffer&  inputBuffer,
    const NMP::DataBuffer&  fallbackBuffer,
    const NMP::Matrix34    &worldRoot
    );

  /// The fallbackBuffer is used for missing channels.
  /// It's required to have all channels set.
  void calculateWorldSpacePartTMsCacheWithVelocity(  
    NMP::Matrix34*          TMcache,
    NMP::Matrix34*          TMcacheOld,
    const NMP::DataBuffer&  inputBuffer,
    const NMP::DataBuffer&  inputBufferOld,
    const NMP::DataBuffer&  fallbackBuffer,
    const NMP::Matrix34    &worldRoot,
    const NMP::Matrix34    &previousWorldRoot
    );
#endif

  /// Calculates the world space TM for a single part
  /// The fallbackBuffer is used for missing channels.
  /// It's required to have all channels set.
  void calculateWorldSpacePartTM(
    NMP::Matrix34&          TM,
    int32_t                 physicsPartIndex,
    const NMP::DataBuffer&  inputBuffer,
    const NMP::DataBuffer&  fallbackBuffer,
    const NMP::Matrix34&    worldRoot,
    bool                    enforceJointLimits) const;

  /// Calculates the part TMs for all parts relative to the physics root, returning them in the
  /// array (which must be size getNumParts())
  /// The fallbackBuffer is used for missing channels.
  /// It's required to have all channels set.
  void calculatePartTMsRelativeToRoot(
    NMP::Matrix34          TMs[],
    const NMP::DataBuffer& inputBuffer,
    const NMP::DataBuffer& fallbackBuffer) const;

  virtual void disableSleeping() = 0;
  virtual void reenableSleeping() = 0; // returns threshold to its given value, which isn't necessarily enabled

  /// Applies soft keyframing in global space to make the skeleton approach the target transforms offset from worldRoot
  /// over a time dt. This is mixed with the original physics motion using weight. The maximum part accelerations can be
  /// specified - if -ve then the max acceleration values are ignored. Optional partChooser can be used to make it
  /// operate only on some parts.
  /// The fallbackBuffer is used for missing channels.
  /// It's required to have all channels set.
  virtual void applySoftKeyframing(
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
    const PartChooser&     partChooser) = 0;

  /// Applies hard keyframing in global space to make the skeleton move to the target transforms offset from worldRoot.
  /// This converts the actors to kinematic, if they are not already. enableCollision indicates if collision should be
  /// enabled for the keyframed parts Optional partChooser can be used to make it operate only on some parts. Previous
  /// buffer and world roots are optional - if non-zero they are used to set the velocities too.
  /// The fallbackBuffer is used for missing channels.
  /// It's required to have all channels set.
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
    const PartChooser&     partChooser) = 0;

  /// Sets the AA target for an individual joint. targetQuat represents a rotation of the joint limit frame (not the
  /// bodies). makeChildDynamic indicates if the joint child body should be made dynamic if it is currently kinematic.
  /// Note that the target velocity is just added on to the orientation at the moment, later we will want an
  /// angular velocity drive to achieve this more cleanly.
  virtual void applyActiveAnimation(uint32_t jointIndex, const NMP::Quat& targetQuat, bool makeChildDynamic) = 0;

  /// Applies active animation. Optional jointChooser can be used to make it operate only on some
  /// joints. limitClampFraction allows clamping of the target orientation to within the limits (-ve
  /// disables clamping)
  /// The fallbackBuffer is used for missing channels.
  /// It's required to have all channels set.
  virtual void applyActiveAnimation(
    const NMP::DataBuffer& targetBuffer,
    const NMP::DataBuffer& fallbackBuffer,
    float                  strengthMultiplier,
    float                  dampingMultiplier,
    float                  internalCompliance,
    float                  externalCompliance,
    bool                   enableJointLimits,
    const JointChooser&    jointChooser,
    float                  limitClampFraction) = 0;

  /// The implementation can use this to control the separation correction on this update -
  /// subsequently it would normally revert to the defaults. It will be called by the physics task,
  /// so can be called multiple times per morpheme update (once per physics node). The
  /// implementation needs to combine the parameter values - e.g. pick the highest iteration value
  /// and the lowest tolerance.
  virtual void requestJointProjectionParameters(
    int NMP_UNUSED(iterations), 
    float NMP_UNUSED(linearTolerance), 
    float NMP_UNUSED(angularTolerance)) {}

  /// An impulse is applied to the physics rig either as a momentum or a velocity change. It applies
  /// two impulses - a local one to the part, and a fullBody one by simulating the effect on an
  /// object with the same mass properties as the character. The responseRatios determine scaling of
  /// the effect on individual parts, depending on their mass relative to the total character mass.
  /// A value of 0 has no effect. Positive values (e.g. 1) make heavy objects respond more than
  /// lighter objects, and vice versa.
  virtual void receiveImpulse(
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
    bool applyAsVelocityChange) = 0;

  // A linear and angular impulse is applied to the character as a whole.
  // The push and twist are distributed over the rig as a pair of impulses applied to the COM of each part so as to 
  // mimic the response of a rigid body with the same mass properties as the character.
  //
  void receiveWrenchImpulse(
    const NMP::Vector3& linearImpulse,
    const NMP::Vector3& angularImpulse,
    const bool* usePartFlags = (bool*)0);

  /// Disables drive strengths and restores damping on all joints
  virtual void restoreAllJointDrivesToDefault() = 0;

  /// This returns the maximum deviation (distance) of all the SK parts compared to where it is "supposed" to be. If no
  /// parts are SK then it will return 0.
  virtual float getMaxSKDeviation() const = 0;

  /// Sets the position used for when we move the rig out of the way when it's been made kinematic
  virtual void setKinematicPos(const NMP::Vector3&) = 0;

  /// Makes the whole rig kinematic so it doesn't require any simulation time. If desired All the parts will be moved
  /// to the value set by a call to setKinematicPos so that they're "out of the way".
  /// Called by the reference counting interface.
  virtual void makeKinematic(bool moveToKinematicPos) = 0;

  /// Makes the whole rig dynamic.
  /// Called by the reference counting interface.
  virtual void makeDynamic() = 0;

  /// Get the joint orientation as a quaternion (takes frame 2 to frame 1).
  virtual NMP::Quat getJointQuat(uint32_t jointIndex) = 0;

  virtual void expandJointLimits(const uint32_t jointIndex, const NMP::Quat& orientation)
  {
    Joint* const joint = getJoint(jointIndex);

    NMP_ASSERT(joint);

    joint->expandLimits(orientation);

    m_jointLimitsModified = true;
  }

  /// \brief Scales the specified joint limit.
  virtual void scaleJointLimits(const uint32_t jointIndex, float scaleFactor)
  {
    Joint* const joint = getJoint(jointIndex);

    NMP_ASSERT(joint);

    joint->scaleLimits(scaleFactor);

    m_jointLimitsModified = true;
  }

  /// \brief Sets the friction on a part of the rig to the authored value multiplied by frictionScale.
  virtual void scaleFrictionOnPart(const int32_t partIndex, const float frictionScale) = 0;

  /// \brief Sets the skin width to the maximum of authored value plus skinWidthIncrease and the current
  /// skin width. A -ve value forces the increase to zero (i.e. to reset it to the authored value).
  virtual void setSkinWidthIncrease(uint32_t partIndex, float skinWidthIncrease) = 0;

  ///------------------------------------------------------------------------------------------------------------
  /// This is the collection of functions which are provided by the base class
  ///------------------------------------------------------------------------------------------------------------

  /// Returns the scene this character is associated with
  PhysicsScene* getPhysicsScene() const { return m_physicsScene; };

  /// Converts this skeleton into a transform buffer. Optional partChooser can be used to make it operate only on some
  /// parts.
  void toTransformBuffer(
    NMP::DataBuffer&     outputBuffer,
    const NMP::Matrix34& worldRoot,
    const PartChooser*   partChooser);

  /// converts a transform buffer into this, setting the velocities using two buffers/roots and the
  /// delta time (must be > 0)
  void fromTransformBuffer(
    const NMP::DataBuffer& inputBuffer,
    const NMP::DataBuffer& previousInputBuffer,
    const NMP::Matrix34&   worldRoot,
    const NMP::Matrix34&   previousWorldRoot,
    float                  dt,
    Dispatcher*            dispatcher);

  /// Integrates the transforms forwards in time using the rates and the physical joint limits if
  /// desired. limitForcing indicates how the limits should be enforced - 0 means no limits and 1
  /// means gentle clamping and > 1 results in harder clamping. If rateDamping > 0 then an
  /// exponential damping is also applied to transformRates.
  void integrateAnimation(
    NMP::DataBuffer& transforms,
    NMP::DataBuffer& transformRates,
    float            dt,
    float            rateDamping,
    bool             clampToLimits);

  /// Gets the local rotation for a physics joint from a transform buffer taking in to account
  /// sparse physics to animation joint mappings.
  /// The fallbackBuffer is used for missing channels.
  /// It's required to have all channels set.
  void getQuatFromTransformBuffer(
    uint32_t                partIndex,
    const NMP::DataBuffer&  inputBuffer,
    const NMP::DataBuffer&  fallbackBuffer,
    NMP::Quat&              quat);

  /// Returns the animation joint quaternion that is clamped to the physics joint limits (which are
  /// scaled by limitFrac)
  NMP::Quat getLimitedJointQuat(int animJointIndex, const NMP::Quat& inputQuat, float limitFrac) const;

  /// Return the joint frame1 axes in world space
  NMP::Matrix34 getJointFrame1World(uint32_t jointIndex) const;

  /// Return the joint frame2 axes in world space
  NMP::Matrix34 getJointFrame2World(uint32_t jointIndex) const;

  /// Not all physics engines will have a rig ID
  virtual int32_t getRigID() const { return-1; } 

  /// Calculates the total mass
  float calculateMass() const;

  /// Calculates the world inertia tensor
  NMP::Matrix34 calculateGlobalInertiaTensor() const;

  /// Returns the centre of mass of the whole physics rig.
  NMP::Vector3 calculateCentreOfMass() const;

  /// Returns the velocity of the centre of mass of the whole physics rig.
  NMP::Vector3 calculateCentreOfMassVelocity() const;

  /// Calculates the total mass, centre of mass and inertia of the whole physics rig or part thereof as specified
  /// by entries in the optional usePart mask.
  void caclulateMassProperties(float& partsMass, NMP::Vector3& partsCOMPos, NMP::Matrix34& partsInertia,
    const bool* usePart = NULL);

  /// Makes the whole rig dynamic and increases the reference count. The bone positions are likely
  /// to need setting after this.
  void addReference();

  /// decreases the reference count and if 0 calls makes it kinematic so it doesn't require any
  /// simulation time. If desired All the bones will be moved to the value set by a call to
  /// setKinematicPos so that they're "out of the way", for those physics engines requiring it.
  void removeReference();

  /// Indicates if there is a reference to us (i.e. we are dynamic)
  bool isReferenced() const;

  uint32_t getNumParts() const;
  Part* getPart(uint32_t index);
  const Part* getPart(uint32_t index) const;

  uint32_t getNumJoints() const;
  Joint* getJoint(uint32_t index);
  const Joint* getJoint(uint32_t index) const;

  uint32_t getNumMaterials() const;

  /// Returns the joint index that connects the two parts, or -1 if there is no such joint
  int32_t getJointIndex(int32_t partIndex1, int32_t partIndex2) const;

  /// Return the index of the part which was marked up as the character root in morpheme:connect.
  int32_t getRootPartIndex() const;

  /// Get the rig's root part transform matrix. The root part is marked up in morpheme:connect. This
  /// transform will include the translation and orientation offset as marked up on the rig.
  NMP::Matrix34 getRootPartTransform() const;

  /// Get the trajectory position - the mean position of all the marked up trajectory parts
  NMP::Vector3 getMarkedUpMeanTrajectoryPosition() const;

  /// Gets the transformation matrix of the Part, relative to the root part when the rig is in the bind pose.
  NMP::Matrix34 getBindPoseTM(uint32_t partIndex);

  /// Gets the dimensions (height etc) of the bind pose (calculates, so may not be fast)
  NMP::Vector3 getBindPoseDimensions() const;

  PhysicsRigDef* getPhysicsRigDef() const { return m_physicsRigDef; }

  /// Gets the animation rig that this physics rig is associated with.
  AnimRigDef* getAnimRigDef() const { return m_animRigDef; }

  AnimToPhysicsMap* getAnimToPhysicsMap() const { return m_animToPhysicsMap; };

  void setRigAndAnimToPhysicsMap(AnimRigDef* rig, AnimToPhysicsMap* animToPhysics);

  /// Get the User data pointer. This is a pointer to application owned data. Note that when using
  /// Euphoria this will point to a Euphoria structure (ER::Body)- application user data can be
  /// accessed from there.
  void* getUserData() const { return m_userData; }
  /// Set the User data pointer. This is a pointer to application owned data. Note that when using
  /// Euphoria this will point to a Euphoria structure (ER::Body) - application user data can be
  /// accessed from there.
  void setUserData(void* userData) { m_userData = userData; }

#if defined(MR_OUTPUT_DEBUGGING)
  uint32_t serializeTxPersistentData(MR::AnimSetIndex animSetIndex, void* outputBuffer, uint32_t outputBufferSize = 0) const;
  uint32_t serializeTxFrameData(MR::AnimSetIndex animSetIndex, void* outputBuffer, uint32_t outputBufferSize = 0) const;
#endif // MR_OUTPUT_DEBUGGING

protected:
  /// Direct add and removal. This is called be add and remove reference when switch between 0 and 1 reference.
  virtual void removeFromScene() = 0;
  virtual void addToScene() = 0;

  void writeJointLimits();
  void resetJointLimits();

  // Used in getRoot
  NMP::Vector3        m_rootOriginalOffsetTranslation;
  NMP::Quat           m_rootOriginalOffsetRotation;

  /// The number of active physics nodes using this rig.
  int32_t             m_refCount;

  /// Animation rig that this physics rig is associated with
  AnimRigDef         *m_animRigDef;
  AnimToPhysicsMap   *m_animToPhysicsMap;

  /// Pointers to the parts and joints
  Part**          m_parts;
  Joint**         m_joints;

  /// Definition we were created from
  PhysicsRigDef* m_physicsRigDef;

  /// Scene that this physics rig is associated with
  PhysicsScene*  m_physicsScene;

  /// If true, derived morpheme physics rigs will write joint limits to the physics engine's rig, set limits back to
  /// those in joint def, then reset this flag.
  bool           m_jointLimitsModified;

  /// Application owned user data
  void* m_userData;
};

#if defined(NM_HOST_64_BIT) && defined(NM_COMPILER_MSVC) 
#pragma warning (pop)
#endif


//----------------------------------------------------------------------------------------------------------------------
// Inline implementations
//----------------------------------------------------------------------------------------------------------------------
inline uint32_t PhysicsRig::getNumJoints() const
{
  return m_physicsRigDef->getNumJoints();
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t PhysicsRig::getNumParts() const
{
  return m_physicsRigDef->getNumParts();
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRig::Part *PhysicsRig::getPart(uint32_t partIndex)
{
  NMP_ASSERT(partIndex < getNumParts());
  return partIndex < getNumParts() ? m_parts[partIndex] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline const PhysicsRig::Part *PhysicsRig::getPart(uint32_t partIndex) const 
{
  NMP_ASSERT(partIndex < getNumParts());
  return partIndex < getNumParts() ? m_parts[partIndex] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline PhysicsRig::Joint *PhysicsRig::getJoint(uint32_t jointIndex) 
{
  NMP_ASSERT(jointIndex < getNumJoints());
  return jointIndex < getNumJoints() ? m_joints[jointIndex] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline const PhysicsRig::Joint *PhysicsRig::getJoint(uint32_t jointIndex) const 
{
  NMP_ASSERT(jointIndex < getNumJoints());
  return jointIndex < getNumJoints() ? m_joints[jointIndex] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
inline int32_t PhysicsRig::getRootPartIndex() const
{
  return m_physicsRigDef->m_rootPart.index;
}

//----------------------------------------------------------------------------------------------------------------------
inline bool PhysicsRig::PartChooser::usePart(int32_t index) const
{
  int32_t animIndex = m_am->getAnimIndexFromPhysicsIndex(index);
  if ( animIndex == -1 )
    return false;
  return (m_mask->m_values[animIndex] == true);
}

//----------------------------------------------------------------------------------------------------------------------
inline bool PhysicsRig::JointChooser::useJoint(int32_t index) const
{
  int32_t childIndex = m_physicsRigDef->m_joints[index]->m_childPartIndex;
  NMP_ASSERT(childIndex >= 0);
  return (m_mask->m_values[m_am->getAnimIndexFromPhysicsIndex(childIndex)] == true);
}

//----------------------------------------------------------------------------------------------------------------------
inline bool PhysicsRig::isReferenced() const
{
  return m_refCount > 0;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_RIG_H
//----------------------------------------------------------------------------------------------------------------------
