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
#ifndef MR_NODE_OPERATOR_FALLOVERWALL_H
#define MR_NODE_OPERATOR_FALLOVERWALL_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "euphoria/erAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
/// \class ER::AttribDataOperatorFallOverWall
/// \brief Container for fallOverWall operator def data.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataOperatorFallOverWallDef : public MR::AttribData
{

public:

  static AttribDataOperatorFallOverWallDef* create(NMP::MemoryAllocator* allocator, uint16_t refCount);

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataOperatorFallOverWallDef* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount);

  
  NM_INLINE AttribDataOperatorFallOverWallDef() { setType(ATTRIB_TYPE_OPERATOR_FALLOVERWALL_DEF); setRefCount(0); }
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_FALLOVERWALL_DEF; }

  /// For Manager registration.
  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);
  void endianSwap();
  static bool copy(MR::AttribData* source, MR::AttribData* dest);

  // Attributes
  float m_reachTriggerDistance;
  float m_reachTriggerVelocity;
  float m_fallTriggerDistance;
  float m_attractorTriggerDistance;
  float m_maxWallHeightOffset;
  float m_targetRollSpeed;
  float m_sideRollAmount;
  float m_overWallTimeout;
  float m_fallenNotTriggeredTimeout;
  float m_stalledTimeout;
  float m_settleTimeout;
  float m_legKickAmountDefault;
  float m_targetApproachVelDefault;
  float m_targetRollOverVelDefault;
};

//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.
MR::AttribData* nodeOperatorFallOverWallOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex, /// The output pin we have been asked to update. We have 1:                             
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// OutputCP op emitted messages.
MR::AttribData* nodeOperatorFallOverWallEmitMessageUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex,
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// OutputCP and state data init.
void nodeOperatorFallOverWallInitInstance(
  MR::NodeDef* node,
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the output control parameter pins.
enum NodeOperatorFallOverWallOutCPPinIDs
{
  FOW_CP_OUT_ARM0REACHIMPORTANCE = 0,
  FOW_CP_OUT_ARM0REACHTARGET,
  FOW_CP_OUT_ARM0REACHNORMAL,
  FOW_CP_OUT_ARM1REACHIMPORTANCE,
  FOW_CP_OUT_ARM1REACHTARGET,
  FOW_CP_OUT_ARM1REACHNORMAL,
  FOW_CP_OUT_LEG0WRITHESTRENGTH,
  FOW_CP_OUT_LEG1WRITHESTRENGTH,
  FOW_CP_OUT_LEG0STRENGTH,
  FOW_CP_OUT_LEG1STRENGTH,
  FOW_CP_OUT_SPINESTRENGTH,
  FOW_CP_OUT_TARGET_VELOCITY,
  FOW_CP_OUT_OVERWALLTIME,
  FOW_CP_OUT_STALLEDTIME,
  FOW_CP_OUT_FALLENNOTTRIGGEREDTIME,
  FOW_CP_OUT_COMANGLETOWALLNORMAL,

  FOW_CP_OUT_COUNT
};

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the node's input pins
enum NodeOperatorFallOverWallInputCPPinIDs
{
  FOW_CP_IN_ENABLE = 0,
  FOW_CP_IN_RIGHT_EDGE_POINT,
  FOW_CP_IN_LEFT_EDGE_POINT,
  FOW_CP_IN_BALANCE_AMOUNT,
  FOW_CP_IN_LEG_KICK_AMOUNT,
  FOW_CP_IN_TARGET_APPROACH_VEL,
  FOW_CP_IN_TARGET_ROLLOVER_VEL,

  FOW_CP_IN_COUNT
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ER::AttribDataOperatorFallOverWallState
/// \brief Container for falloverwall operator state.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataOperatorFallOverWallState : public MR::AttribData
{
public:

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataOperatorFallOverWallState* init(
    NMP::Memory::Resource& resource,
    uint16_t refCount = 0);

  NM_INLINE AttribDataOperatorFallOverWallState() { setType(ATTRIB_TYPE_OPERATOR_FALLOVERWALL_STATE); setRefCount(0); }
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_FALLOVERWALL_STATE; }

  /// For Manager registration.
  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);
  static MR::AttribDataHandle create(NMP::MemoryAllocator* allocator);
  static bool copy(MR::AttribData* source, MR::AttribData* dest);
  static void endianSwap(AttribDataOperatorFallOverWallState* state);


  static const int32_t s_numArms = 2;
  static const int32_t s_numLegs = 2;

private:

  // Node api's that are called by morpheme but need access to the performance internals
  //
  friend void nodeOperatorFallOverWallInitInstance(MR::NodeDef* nodeDef, MR::Network* net);
  friend MR::AttribData* nodeOperatorFallOverWallEmitMessageUpdate(
    MR::NodeDef* nodeDef,
    MR::PinIndex NMP_UNUSED(outputCPPinIndex),
    MR::Network* net);
  friend MR::AttribData* nodeOperatorFallOverWallOutputCPUpdate(
    MR::NodeDef* node,
    MR::PinIndex outputCPPinIndex,
    MR::Network* net);


  //--------------------------------------------------------------------------------------------------------------------
  // Performance internals

  // A structure to hold values that are refreshed at the start of every frame but are not referenced
  // between one frame and the next.
  struct PerFrameState;

  // Retrieves physics state used to determine the character's current relationship to the wall edge.
  bool preUpdate(
    MR::Network* net,
    const NMP::Vector3& wallEdgePoint0_, const NMP::Vector3& wallEdgePoint1_,
    float balanceAmount, PerFrameState& perFrameState);

  // Updates internal timers and determines whether the fall has "stalled".
  void updateTimers(float stalledTimeout, const PerFrameState& perFrameState);
  
  // Uses internal timer status to determine state of the fall and where necessary emit the relevant message.
  void updateEmittedMessages(float overWallTimeout, float settleTimeout, float fallenNotTriggeredTimeout);
  
  // Wipes down performance state variables.
  void reset();

  // Core and leg strength 
  void updateLegAndSpineStrengths(
    float fallTriggerDistance,
    float overWallTimeout,
    float& spineStrength,
    float* legStrength,
    float* legWritheStrength,
    const AttribDataOperatorFallOverWallState::PerFrameState& perFrameState);

  // Reach targets
  void updateReachTargets(
    const PerFrameState& perFrameState, 
    float* reachImportances,
    NMP::Vector3* reachTargets,
    NMP::Vector3* reachNormals);
  NMP::Vector3 calcReachTargetAdjustmentForApproachFromAbove(
    const NMP::Vector3& tgtPos,
    const ER::AttribDataOperatorFallOverWallState::PerFrameState& perFrameState,
    int armIndex);

  // Step velocity demand
  NMP::Vector3 updateStepVelocity(
    float fallTriggerDistance,
    float targetApproachVel,
    const ER::AttribDataOperatorFallOverWallState::PerFrameState& perFrameState);

  // Application of "cheat" impulses
  void calculateAndApplyCheatImpulses( 
    float fallTriggerDistance,
    float torquesTriggerDistance,
    float targetRollOverVel,
    float targetRollSpeed,
    float sideRollAmount,
    const PerFrameState& perFrameState);

  // Torque to approach target angular velocity about the wall edge
  NMP::Vector3 calculateEdgeAngAccelerationTorque(
    float damping,
    float targetAngVel,
    float maxCorrection, 
    const PerFrameState& perFrameState);
  // Torque to align character with wall edge
  NMP::Vector3 calculateSpineAlignmentTorque( const float sideRollAmount, const PerFrameState& perFrameState);
  // Force to pull the character towards the wall
  NMP::Vector3 calculateAttractorForce(
    float fallTriggerDistance,
    float targetRollOverVel,
    const ER::AttribDataOperatorFallOverWallState::PerFrameState& perFrameState);

  //--------------------------------------------------------------------------------------------------------------------
  // Debug draw
#if defined MR_OUTPUT_DEBUGGING

  // Draw the setup/configuration of character wrt wall
  void debugDrawCharacterToWallRelationship(const PerFrameState& perFrameState);

  // Draw the reach targets
  void debugDrawReachTargets(
    const PerFrameState& perFrameState, 
    float* reachImportances,
    NMP::Vector3* reachTargets,
    NMP::Vector3* reachNormals);

  // Visualise specified velocity and torque vectors
  void debugDrawAngAccelTorqueAsImpulse(
    const NMP::Vector3& debugDrawPos,
    const NMP::Vector3& currVel, 
    const NMP::Vector3& targetVel, 
    const NMP::Vector3& correctionTorque,
    float lightness,
    const PerFrameState& perFrameState);

#endif //#if defined MR_OUTPUT_DEBUGGING

  //--------------------------------------------------------------------------------------------------------------------
  // Data

  // Need padding to accommodate base class unaligned.
#ifndef NM_HOST_64_BIT
  uint32_t m_pad[2];
#endif

  //--------------------------------------------------------------------------------------------------------------------
  // Performance state
  //

  // Wall info:
  // - inputs
  NMP::Vector3 m_wallEdgePoint0;
  NMP::Vector3 m_wallEdgePoint1;
  // - derived wall geometry
  NMP::Vector3 m_wallNormal;      // ref normal defines from which side of the wall the fall begins
  NMP::Vector3 m_wallEdgeDir;     // edge direction

  // Reach targets:
  NMP::Vector3 m_reachTargetAnchors[s_numArms];
  
  // Behaviour control:
  uint32_t m_closestHandIndex; // reach state var
  float m_signOfSideRoll;      // fall control state var

  // Timers:
  float m_fallenNotTriggeredTime;  // time since balance was lost and fall not triggered
  float m_stallingTime;            // time since balance was lost and fall triggered
  float m_overWallEdgeTime;        // time since fall triggered and character passed wall edge
  float m_settlingTime;            // time since fall triggered but stalled in progress
  float m_maxCOMAngleToWallNormal; // compared to current to determine whether fall is stalling
  float m_maxCOMHeight;

  // Emitted message identifiers: 1 bit set in a position corresponding to
  // the ordering of messages as listed in the attribute editor.
  enum
  {
    MSG_NONE = 0,
    MSG_COMPLETED = (1 << 0),
    MSG_STALLED = (1 << 1),
    MSG_FALLEN_NOT_TRIGGERED = (1 << 2),
  };
  // Emitted message code set during the course of an update to signal one of the above reset/completion conditions.
  uint32_t m_resetMessageID;

  MR::FrameCount m_lastUpdateFrame;

  // Status flags:
  bool m_active;            // set to true to indicate morpheme/euphoria data required to run has been found
  bool m_fallTriggered;     // set to true when trigger conditions are first met
  bool m_sideRollTriggered; // set to true when side roll torque is first applied
  bool m_overWallEdge;      // true if all parts are over the wall
  bool m_stalled;           // set to true if the fall has been stalling for too long
  bool m_reset;             // set to true to terminate performance (signals reset for next update)
  bool m_reachTriggered;    // set when reaching is triggered, unset during reset()
};


} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_VECTOR3_TO_FLOATS_H
//----------------------------------------------------------------------------------------------------------------------
