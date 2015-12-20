// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_BODY_H
#define NM_BODY_H

#include "physics/mrPhysicsRig.h"

#include "euphoria/erBodyDef.h"
#include "euphoria/erSharedEnums.h"
#include "euphoria/erDimensionalScaling.h"

// no more than this many will be supported by the physics engine
#define MAX_NUM_LINKS 64

namespace physx
{
class PxActor;
}

namespace NMP
{
  class BitArray;
}

namespace MR
{
  struct PhysicsSerialisationBuffer;
  class PhysicsRig;
}

namespace ER
{
class GravityCompensation;
class BodyDef;
class Limb;
class LimbInterface;
class UserContactHandler;
class RigConstraintManager;
class RigConstraint;

//----------------------------------------------------------------------------------------------------------------------
/// The general body control. Different instances when swapping rigs on a character.
//----------------------------------------------------------------------------------------------------------------------
class Body
{
public:

  enum DebugDrawId
  {
    kDrawLimits,
    kDrawDials,
    kDrawSoftLimits,
    kNumDebugDrawFlags
  };

  enum FeatureId
  {
    kFeatureGravityCompensation,
    kFeatureSoftLimits,
    kFeatureHamstrings,
    kFeatureJointLimits,
    kNumFeatureFlags,
  };

  enum LimbType
  {
    /*  0 */ Core,
    /*  1 */ Head,
    /*  2 */ Leg,
    /*  3 */ Arm,
    /*  4 */ World,
    /*  5 */ NumLimbTypes,
  };

  ~Body() {}

  // API's for creating, destroying and tickover
  //
  /// Allocates memory and initialises from the definition provided. It also registers the Body as
  /// the PhysicsRig user data. If addPartsToEuphoria is set then the physics rig parts will have
  /// per shape data added to them, so that other Euphoria characters can interact with the
  /// individual parts. This isn't needed if the character will also have an interaction proxy.
  static Body* createInstance(
    BodyDef*           bodyDef,
    MR::PhysicsRig*    physicsRig,
    int                animSetIndex,
    bool               addPartsToEuphoria);
  /// Destroys the body and frees associated memory, including any per-shape data on the physics rig parts.
  static void destroyInstance(Body*  body);
  /// Runs pre-physics step
  void prePhysicsStep(float timeDelta, MR::InstanceDebugInterface* pDebugDrawInst);
  /// Runs post-physics step
  void postPhysicsStep(float timeDelta);
  /// Disables euphoria on each limb
  void disableBehaviourEffects();
  /// This will save all physical aspects of the body state
  bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  /// This will restore all physical aspects of the body state
  bool restoreState(MR::PhysicsSerialisationBuffer& savedState);

  /// Helper to get the Body from the user data in the physics rig
  static Body* getFromPhysicsRig(const MR::PhysicsRig* physicsRig) {return (Body*) (physicsRig->getUserData());}

  MR::PhysicsRig* getPhysicsRig() {return m_physicsRig;}
  const MR::PhysicsRig* getPhysicsRig() const {return m_physicsRig;}

public:

  // API's for access to body state and data
  //
  /// Get a reference to the ith limb's interface
  LimbInterface& getLimbInterface(uint32_t i);
  /// Get const reference to the ith limb's interface
  const LimbInterface& getLimbInterface(uint32_t i) const;

  /// Access the dimensional scaling
  const DimensionalScaling& getDimensionalScaling() const {return m_dimensionalScaling;}

  /// This will save the relevant aspects of the body state and set it up for pose modification
  bool startPoseModification(MR::PhysicsSerialisationBuffer& savedState);
  /// Call this to end the pose modification and restore the body state
  bool stopPoseModification(MR::PhysicsSerialisationBuffer& savedState);
  /// toggles freezing of the pose whilst doing pose modification
  void togglePoseFreeze();
  /// Returns true if pose modifcation is active
  bool getDoingPoseModification() const { return m_doingPoseModification; }
  /// Returns true if any limb physics part is in contact with a physics object that is not part of a limb.
  bool isInContact() const { return m_isInContact; }

  /// Returns the index of the root limb (i.e "spine 0")
  int getRootLimbIndex() const;
  /// Returns the number of limbs in this body
  int getNumLimbs() const;
  /// Returns the number of arms in this body
  int getNumArms() const;
  /// Returns the number of heads in this body
  int getNumHeads() const;
  /// Returns the number of legs in this body
  int getNumLegs() const;
  /// Returns the number of spines in this body
  int getNumSpines() const;
  /// Returns the total mass of the body
  float getMass() const { return m_mass; }

  /// Returns centre of mass position
  NMP::Vector3 getCentreOfMass() const { return m_centreOfMass; }

  /// Return the self avoidance shape radius
  float getSelfAvoidanceRadius() const;

  // Debug and features flags
  bool getDebugDrawFlag(DebugDrawId id) const { return m_debugDrawFlags->isBitSet(id) != 0; };
  void setDebugDrawFlag(DebugDrawId id, bool flag) { setBitFlag(m_debugDrawFlags, id, flag); };
  bool getFeatureFlag(FeatureId id) const { return m_featureFlags->isBitSet(id) != 0; };
  void setFeatureFlag(FeatureId id, bool flag) { setBitFlag(m_featureFlags, id, flag); };

  /// Set the guide pose associated with the guide pose ID (and if they were set, or just defaulted)
  void setHamstringStiffness(float stiffness) { m_hamstringStiffness = stiffness; }
  void setSoftLimitStiffness(float stiffness) { m_softLimitStiffness = stiffness; }
  void handlePhysicsJointMask(const MR::PhysicsRig::JointChooser& jointChooser);

  /// Indicates if Euphoria is allowed to operate on the joint (determined by the physics output mask)
  bool getOutputEnabledOnJoint(uint32_t iJoint) { return m_outputEnabledOnJoints[iJoint]; }
  struct PerLinkData
  {
    NMP::Vector3 currentAngle;     ///< twist swing, but multiplied by 4 so approximately twist lean.
    NMP::Vector3 softLimit;        ///< swing1, swing2, twist low, twist high.
    NMP::Vector3 targetRot;        ///< twist swing, but multiplied by 4 so approximately twist lean.
    NMP::Vector3 softLimitOffset;  ///< twist swing, but multiplied by 4 so approximately twist lean.
    float strength;
    float damping;
    float softLimitStrength;
    bool applyThisPart;
  };

  /// Allows Euphoria to be informed about additional gravity-type accelerations that are being
  /// applied to the character, apart from the global gravity set in the physics scene - e.g. due to
  /// an anti-gravity well, or if the game is implementing its own gravity field.
  void setExtraGravity(const NMP::Vector3& extraGravity) {m_extraGravity = extraGravity;}
  /// Returns the gravity acceleration that is being applied in addition to the global gravity.
  const NMP::Vector3& getExtraGravity() const {return m_extraGravity;}
  /// Returns the total gravity
  NMP::Vector3 getTotalGravity() const;

  // If the actor is associated with a part of the body returns the corresponding limb/part indices
  // otherwise returns false, and indices == -1
  LimbTypeEnum::Type getActorLimbPartIndex(const physx::PxActor* actor, int& limbRigIndex, int& partLimbIndex) const;
  LimbTypeEnum::Type getKinematicActorLimbPartIndex(const physx::PxActor* actor, int& limbRigIndex, int& partLimbIndex) const;
  // Retrieve the Part from a given limb and part index
  const MR::PhysicsRig::Part* getPartFromLimbPartIndex(const int limbRigIndex, const int partLimbIndex)const ;

  // Returns the limbtype given the limb/part index
  LimbTypeEnum::Type getLimbTypeFromLimbPartIndex(int32_t limb, int32_t part) const;

  /// Register user handler for contact reports
  void setUserContactHandler(UserContactHandler* handler);

  ER::RigConstraintManager* getRigConstraintManager() { return m_rigConstraintManager; }

  ER::RigConstraint* createRigConstraint(
    const uint32_t partIndexA,
    const NMP::Matrix34& partJointFrameA,
    const uint32_t partIndexB,
    const NMP::Matrix34& partJointFrameB);

  void destroyRigConstraint(const uint32_t partIndexA, const uint32_t partIndexB);

  // Debug Draw
  static bool getDrawContactsFlag();
  static void setDrawContactsFlag(bool b);
  static bool getDrawDetailedContactsFlag();
  static void setDrawDetailedContactsFlag(bool b);

  /// \name Gets the low-level Limb - only Euphoria code should use this
  /// @{
  Limb& getLimb(uint32_t limbIndex);
  const Limb& getLimb(uint32_t limbIndex) const;
  /// @}

private:
  // Aux for instance creation and destruction
  //
  /// Allocate memory (for limbs etc.) aux for createInstance()
  void create(BodyDef* bodyDef, class MR::PhysicsScene* physicsScene);
  /// Initialise (instance specific properties, limbs etc.) aux for createInstance()
  void initialise(MR::PhysicsRig* physicsRig, int animSetIndex, bool addPartsToEuphoria);
  /// Deallocates associated memory (limbs etc.) aux for destroyInstance()
  void destroy();

  /// Calculate the total mass of the body
  void calculateMass();
  void updateCentreOfMass();
  /// update isInContact flag.
  void updateContactState();

  /// Debug draw joint limit
  void drawDials(MR::InstanceDebugInterface* pDebugDrawInst, int index, float size);
  void drawLimit(
    MR::InstanceDebugInterface* pDebugDrawInst, int index, float swing1Limit, float swing2Limit, float twistMin, float twistMax, float size, bool drawFrame, const NMP::Vector3& offset,
    bool swingEnabled = true, bool twistEnabled = true);

  /// Apply soft limits and hamstrings to physics rig,
  void applySmartJointLimits(MR::InstanceDebugInterface* pDebugDrawInst);
  /// Calculate desired orientation for joint at index from soft limits
  void calculateSoftLimits(
    MR::InstanceDebugInterface* pDebugDrawInst, 
    Body::PerLinkData& linkData, 
    bool applyToSwing, 
    bool applyToTwist, 
    int index) const;

  // The algorithm for this is as follows
  // 1. Expand the limits according to strength (thereby, soft limits only affect behaviour at lowish strengths.
  // 2. Set limit to the current orientation of the joint.
  // 3. Clamp limit inwards for the swing and twist limits (note the swing limit needs an approximate normal clamping).
  // 4. Set the target rotation to be a weighted average between the limit and the target rot, depending on the respective
  //    spring strengths.
  // 5. Set the new (combined) spring strength in the linkData.
  void calculateSoftLimitsData(
    int index,
    PerLinkData& linkData) const;

  /// Auxiliary function to simplify setting of bit in NMP::BitArray
  void setBitFlag(NMP::BitArray* bitArray, uint32_t bit, bool val)
  {
    if (val) bitArray->setBit(bit);
    else bitArray->clearBit(bit);
  };

  // Data ////////////////////////////////////////////////////////////////////////////////
  //
private:

  /// Flag set before we update that indicates which joints we should write to.
  bool m_outputEnabledOnJoints[MAX_NUM_LINKS];

  NMP::Vector3 m_extraGravity;
  NMP::Vector3 m_centreOfMass;
  bool m_doingPoseModification;
  bool m_isInContact; // true if any limb physics part is in contact with a non-limb physics part.

  float m_mass;
  float m_hamstringStiffness;
  float m_softLimitStiffness;

  Limb* m_limbs;

  UserContactHandler* m_userContactHandler;

  RigConstraintManager* m_rigConstraintManager;

public:
  BodyDef* m_definition;
  MR::PhysicsScene* m_physicsScene;
  GravityCompensation* m_gravComp;

  int m_animSetIndex;
  /// The adjustment value is given by strength/normalStrength and is used to scale the SL angle
  /// ranges. So 0 means use full SL, 1 (or more) will disable the SL. 
  float m_softLimitAdjustment[MAX_NUM_LINKS]; 
  /// Scales the SL stiffness. 0 stops the SL from acting, 1 is normal limit stiffness
  float m_softLimitStiffnessScale[MAX_NUM_LINKS]; 
  typedef void (DebugCallback)(Body* body, float timeStep);
  static void setPrePhysDebugCallback(DebugCallback* cb);

  DimensionalScaling m_dimensionalScaling;

#ifdef NMP_ENABLE_ASSERTS
  // MORPH-21377: All of this data should be moved out of the body class (and made the correct size for the body)
  struct DebugJointControl
  {
    NMP::Vector3 ts;
    bool debugEnabled;
    const MR::PhysicsRig::Part* childPart;
    MR::PhysicsRig::Joint* joint;
  };
  static const int m_maxNumDebugJointControls = 64;
  DebugJointControl m_debugJointControls[m_maxNumDebugJointControls];

  static DebugCallback* m_prePhysDebugCallback;
#endif // NMP_ENABLE_ASSERTS

private:
  MR::PhysicsRig* m_physicsRig;

  NMP::BitArray* m_debugDrawFlags;
  NMP::BitArray* m_featureFlags;

};

//----------------------------------------------------------------------------------------------------------------------
// Simple inlines
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
inline int Body::getNumLimbs() const
{
  return m_definition->m_numLimbs;
}

//----------------------------------------------------------------------------------------------------------------------
inline int Body::getNumArms() const
{
  return m_definition->m_numArmLimbs;
}

//----------------------------------------------------------------------------------------------------------------------
inline int Body::getNumHeads() const
{
  return m_definition->m_numHeadLimbs;
}

//----------------------------------------------------------------------------------------------------------------------
inline int Body::getNumLegs() const
{
  return m_definition->m_numLegLimbs;
}

//----------------------------------------------------------------------------------------------------------------------
inline int Body::getNumSpines() const
{
  return m_definition->m_numSpineLimbs;
}

//----------------------------------------------------------------------------------------------------------------------
inline float Body::getSelfAvoidanceRadius() const
{
  return m_definition->m_selfAvoidance.radius;
}

}

#endif // NM_BODY_H
