// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
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
# pragma once
#endif
#ifndef M_CORE_DEBUG_INTERFACE_H
#define M_CORE_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBitArray.h"
#include "NMPlatform/NMFlags.h"
#include "mSharedDefines.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mCoreDebugInterface.h
/// \brief abstract interface class used by the morpheme core to record debug data; this will most likely be inherited
/// from by the Comms layer and the data will be packaged and sent across the wire to Connect.
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

// ensure all these structs are the same layout for all compilers
#ifndef NM_HOST_CELL_SPU
  #pragma pack(push, 4)
#endif // NM_HOST_CELL_SPU

typedef uint16_t StringToken;

//----------------------------------------------------------------------------------------------------------------------
enum CorePersistentDataTypes
{
  kNetworkDefPersistentData = 0,
  kAnimRigDefPersistentData,
  kPhysicsRigDefPersistentData,
  kPhysicsRigPersistentData,
  kEuphoriaNetworkPersistentData,
  kPresetGroupTagsDefPersistentData,

  kLastPersistentData, ///< start from here when adding additional def types

  /// connect begins internal persistent data (scene object persistent data for example) types here,
  /// ids at and above this range are reserved, and should not be changed without recompiling connect and
  /// bumping the session file version number.
  kFirstReservedPersistentDataType = 128 
};

//----------------------------------------------------------------------------------------------------------------------
enum CoreFrameDataTypes
{
  kPhysicsRigFrameData,
  kEuphoriaNetworkFrameData,
  kLastFrameData ///< start from here when adding additional def types
};

//----------------------------------------------------------------------------------------------------------------------
/// High Level Debug Output Filtering.
/// What components of debug output to enable from runtime.
/// Do not change the content of this enumeration unless compiling connect as well as runtime.
enum eDebugOutputFlags
{
  DEBUG_OUTPUT_DISABLE_ALL            = 0,         ///< Turn off all debug transmission from runtime.
  DEBUG_OUTPUT_CONTROL_PARAMS         = (1 << 0),  ///< Control parameter and operator node attribute data.
  DEBUG_OUTPUT_PROFILING              = (1 << 1),  ///< Network performance profiling information.
  DEBUG_OUTPUT_TASK_QUEUING           = (1 << 2),  ///< Task queuing information including, task inputs, outputs and dependencies.
  DEBUG_OUTPUT_TREE_NODES             = (1 << 3),  ///< Attribute data from tree nodes.
  DEBUG_OUTPUT_STATE_MACHINE_REQUESTS = (1 << 4),  ///< State machine requests (Only exist for the frame that the request is made, does not reflect StateMachine state).
  DEBUG_OUTPUT_SCRATCH_PAD            = (1 << 5),  ///< Sending simple labeled debug values from anywhere.
  DEBUG_OUTPUT_DEBUG_DRAW             = (1 << 6),  ///< Sending draw requests requests from anywhere.
  DEBUG_OUTPUT_ENABLE_ALL             = 0xFFFFFFFF ///< Turn on all debug transmission from runtime.
};

//----------------------------------------------------------------------------------------------------------------------
// Code section that deals with endian swapping of Node Output Data Blocks, identified by MR::AttribDataType.

//----------------------------------------------------------------------------------------------------------------------
/// Stores the profile timing of an element of node processing (updateConnections, task queuing, task evaluation).
struct NodeTimingElement
{
  MR::NodeID              m_nodeID;
  MR::NodeTaskTimingType  m_type;
  float                   m_time;
};

//----------------------------------------------------------------------------------------------------------------------
// Structures used by runtime to serialise debug output data.
// NOTE: DO NOT CHANGE THESE STRUCTURES UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
//       This is because these structures are also used by morpheme connect when receiving debug data from runtime.
//----------------------------------------------------------------------------------------------------------------------
struct AttribBoolOutputData
{
  bool m_value;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribBoolOutputData *endianSwap(void *buffer);

  // Deserialises in place on buffer memory. Assumes endian swapping has already been done.
  NM_INLINE static AttribBoolOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribUIntOutputData
{
  uint32_t m_value;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribUIntOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribUIntOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribPhysicsObjectPointerOutputData
{
  uint64_t m_value;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribPhysicsObjectPointerOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribPhysicsObjectPointerOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribIntOutputData
{
  int32_t m_value;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribIntOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribIntOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribFloatOutputData
{
  float m_value;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribFloatOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribFloatOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribVector3OutputData
{
  NMP::Vector3 m_value;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribVector3OutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribVector3OutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribVector4OutputData
{
  NMP::Quat m_value;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribVector4OutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribVector4OutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribBoolArrayOutputData
{
  uint32_t  m_numValues;

  NM_INLINE bool getValue(uint32_t index);
  NM_INLINE bool *getValues();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribBoolArrayOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribIntArrayOutputData
{
  uint32_t  m_numValues;

  NM_INLINE int32_t getValue(uint32_t index);
  NM_INLINE int32_t *getValues();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribIntArrayOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribUIntArrayOutputData
{
  uint32_t  m_numValues;

  NM_INLINE uint32_t getValue(uint32_t index);
  NM_INLINE uint32_t *getValues();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribUIntArrayOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribUInt64ArrayOutputData
{
  uint32_t  m_numValues;

  NM_INLINE uint64_t getValue(uint32_t index);
  NM_INLINE uint64_t *getValues();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribUInt64ArrayOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribFloatArrayOutputData
{
  uint32_t  m_numValues;

  NM_INLINE float getValue(uint32_t index);
  NM_INLINE float *getValues();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribFloatArrayOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribUpdatePlaybackPosOutputData
{
  float m_value;
  bool  m_isFraction;
  bool  m_isAbs;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribUpdatePlaybackPosOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribUpdatePlaybackPosOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribPlaybackPosOutputData
{
  float m_currentPosAdj;
  float m_previousPosAdj;
  float m_currentPosReal;
  float m_previousPosReal;
  float m_delta;
  bool  m_setWithAbs;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribPlaybackPosOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribPlaybackPosOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribUpdateSyncEventPlaybackPosOutputData
{
  uint32_t     m_absPosAdjIndex;
  float        m_absPosAdjFraction;
  uint32_t     m_absPosRealIndex;
  float        m_absPosRealFraction;
  uint32_t     m_deltaPosIndex;
  float        m_deltaPosFraction;
  bool         m_isAbs;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribUpdateSyncEventPlaybackPosOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribUpdateSyncEventPlaybackPosOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribTransformBufferOutputData
{
  struct Transform
  {
    NMP::Vector3 m_pos;
    NMP::Quat    m_quat;
  };

  uint32_t m_numTransforms;
  uint32_t m_usedFlagsPtrOffset;
  uint32_t m_transformsPtrOffset;

  NM_INLINE NMP::BitArray *getUsedFlags();
  NM_INLINE Transform *getTransforms();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribTransformBufferOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribTrajectoryDeltaTransformOutputData
{
  NMP::Vector3 m_deltaPos;
  NMP::Quat    m_deltaAtt;

  // Endian swaps in place on buffer.
  NM_INLINE static AttribTrajectoryDeltaTransformOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribTrajectoryDeltaTransformOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribTransformOutputData
{
  NMP::Quat    m_att;   ///< Attitude.
  NMP::Vector3 m_pos;   ///< Position.

  // Endian swaps in place on buffer.
  NM_INLINE static AttribTransformOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribTransformOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribSyncEventTrackOutputData
{
  struct Event
  {
    float    m_startTime; ///< Start point.
    float    m_duration;  ///< To the next event.
    uint32_t m_userData;  ///< Allows this event to be associated with arbitrary user data. 

    NM_INLINE void endianSwap();
  };

  NM_INLINE Event *getEvent(uint32_t index);
  NM_INLINE Event *getEvents();

  uint32_t m_startEventIndex; ///< Which event to use as playback start. Also marks the blend start point.
  uint32_t m_numEvents;
  float    m_duration;

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribSyncEventTrackOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribSampledEventsOutputData
{
  // Discrete zero duration events that have either been triggered this frame or not.
  struct TriggeredEvent
  {
    uint32_t m_sourceTrackUserData; ///< Arbitrary user data associated with the source event track.
    uint32_t m_sourceEventUserData; ///< Arbitrary user data associated with the event that has fired.
    float    m_blendWeight;         ///< Cumulative contribution of this event to the end result.

    NM_INLINE void endianSwap();
  };

  // Values sampled from an event that has duration.
  // The value can give an indication of how far through the event the sample was taken as with duration events.
  // Or it could be a sample from an event whose value varies along a curve.
  struct SampledEvent
  {
    uint32_t m_sourceTrackUserData;  ///< Arbitrary user data associated with the source event track.
    uint32_t m_sourceEventUserData;  ///< Arbitrary user data associated with the event that has fired.
    float    m_value;                ///< The sample value from the curve track.
    float    m_blendWeight;          ///< The sample value from the curve track.

    NM_INLINE void endianSwap();
  };

  uint32_t m_numTriggeredDiscreteEvents;       ///< 
  uint32_t m_triggeredDiscreteEventsPtrOffset; ///< Array of triggered events.

  uint32_t m_numSampledCurveEvents;            ///<
  uint32_t m_sampledCurveEventsPtrOffset;      ///< At the moment only sampled duration events are added to this buffer.

  NM_INLINE TriggeredEvent *getTriggeredDiscreteEvents();
  NM_INLINE SampledEvent *getSampledCurveEvents();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribSampledEventsOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribDurationEventTrackSetOutputData
{
  struct Event
  {
    uint32_t m_userData;                 ///< Allows this event to be associated with arbitrary user data. 
    float    m_syncEventSpaceStartPoint; ///< The start point of this event, specified within time synchronisation event space.
    float    m_syncEventSpaceMidPoint;   ///< The middle point of this event, specified within time synchronisation event space.
    float    m_syncEventSpaceDuration;   ///< Time from Start to the end of the event, expressed as a time
                                         ///<  synchronisation event fraction.
    float    m_normalisedStartPoint;     ///<
    float    m_normalisedDuration;       ///<
    float    m_weight;                   ///< What is the maximum weight value of this event. Gives an indication of the 
                                         ///<  contribution that this event will make to the end result of what ever this 
                                         ///<  event is being used for e.g. Foot locking, sound effect volume etc.
                                         ///<  Also controls how this event will be blended to some extent.
    NM_INLINE void endianSwap();
  };

  struct Track
  {
    uint32_t m_numEvents;     ///< How many events are recorded in this track.
    uint32_t m_runtimeID;     ///< Runtime assigned track ID.
    uint32_t m_userData;      ///< Allows this track to be associated with arbitrary user data.
    uint32_t m_numSyncEvents; ///< The number of events in the associated time synchronisation event 
                              ///< track. Duration events are expressed in this space.
    uint32_t m_eventsPtrOffset;

    NM_INLINE Event *getEvents();

    NM_INLINE void deserialiseTx();
  };

  uint32_t m_numEventTracks;
  uint32_t m_tracksPtrOffset;

  NM_INLINE Track *getTracks();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribDurationEventTrackSetOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribVelocityOutputData
{
  NMP::Vector3  m_linearVel;  ///< Linear Velocity.
  NMP::Vector3  m_angularVel; ///< Angular Velocity.

  // Endian swaps in place on buffer.
  NM_INLINE static AttribVelocityOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribVelocityOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribSourceEventTrackSetOutputData
{
  struct Event
  {
    float    m_startTime; ///< Start point.
    uint32_t m_userData;  ///< Allows this event to be associated with arbitrary user data. 

    NM_INLINE void endianSwap();
  };

  struct Track
  {
    uint32_t m_numEvents; ///< How many events are recorded in this track.
    uint32_t m_userData;  ///< Allows this track to be associated with arbitrary user data.
    uint32_t m_eventsPtrOffset;

    NM_INLINE Event *getEvents();

    NM_INLINE void deserialiseTx();
  };

  uint32_t m_numEventTracks;
  uint32_t m_tracksPtrOffset;

  NM_INLINE Track *getTracks();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribSourceEventTrackSetOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribStateMachineOutputData
{
  struct Condition
  {
    bool               m_satisfied; ///< Has this condition been satisfied.
    TransitConditType  m_type;      ///< The unique type ID of this transition condition.

    NM_INLINE void endianSwap();
  };

  struct PossibleTransition
  {
    uint32_t   m_transitionStateID;   ///< Index of the State within the State Machine that
                                      ///<  is our destination if all TransitConditions in the set are satisfied.                                            
                                      ///<  This is likely to be the ID of a transition Node.
    MR::NodeID m_transitionNodeID;    ///< 
    uint32_t   m_numConditions;       ///< If all the conditions that must be satisfied to start the transition.
    uint32_t   m_conditionsPtrOffset;

    NM_INLINE Condition *getConditions();

    NM_INLINE void deserialiseTx();
  };
 
  MR::StateID m_activeStateID;       ///< Index into local array of available states.
  MR::StateID m_targetStateID;       ///< Index the into local array of available states.
                                     ///<  Records any requestSetState calls. 
                                     ///<  Set to INVALID_NODE_ID if no request has been set.
                                     ///<  This is initialised with the default starting state from the definition.
  MR::NodeID  m_activeNodeID;        ///< The NodeID of the state machines active node.
  MR::NodeID  m_targetNodeID;        ///< Set to INVALID_NODE_ID if no request has been set.
  uint32_t    m_numActiveConditions; ///< Total number of conditions that are being evaluated against the active state.
  uint32_t    m_numPossibleTransitions;      ///< Num possible transitions from active state.
  uint32_t    m_possibleTransitionPtrOffset;
  bool        m_active;                      ///< Is this state machine active or inactive.

  NM_INLINE PossibleTransition *getPossibleTransitions();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribStateMachineOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribTransitSyncEventsOutputData
{
  float m_transitionEventCount; ///< How far through transition are we.
  bool  m_completed;            ///< Transition has completed;
  bool  m_reversed;             ///< Transition has reversed direction

  // Endian swaps in place on buffer.
  NM_INLINE static AttribTransitSyncEventsOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribTransitSyncEventsOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribLockFootStateOutputData
{
  NMP::Vector3 m_trajectoryPos;
  NMP::Quat    m_trajectoryAtt;
  NMP::Vector3 m_previousEndEffectorPosition;
  NMP::Vector3 m_lockPosition;
  bool         m_tracking;
  bool         m_previouslyInFootStep;
  bool         m_firstFootStep;
  uint8_t      m_pad[1];

  // Endian swaps in place on buffer.
  NM_INLINE static AttribLockFootStateOutputData *endianSwap(void *buffer);

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribLockFootStateOutputData *deserialiseTx(void *buffer);
};

// ------------------------------------------------------------------
// NOTE: DO NOT CHANGE THIS STRUCTURE UNLESS YOU ARE ABLE TO RECOMPILE MORPHEME CONNECT.
struct AttribDataBlendNxMOutputData
{
  uint16_t m_blendX;
  uint16_t m_blendY;

  NM_INLINE float getValueX(uint32_t index);
  NM_INLINE float getValueY(uint32_t index);

  NM_INLINE float* getValueWeightsX();
  NM_INLINE float* getValueWeightsY();

  // Deserialises (and endian swaps if necessary) in place on buffer memory.
  NM_INLINE static AttribDataBlendNxMOutputData *deserialiseTx(void *buffer);
};

// restore the packing alignment set before this file
#ifndef NM_HOST_CELL_SPU
  #pragma pack(pop)
#endif // NM_HOST_CELL_SPU
} // namespace MR

#include "sharedDefines/mCoreDebugInterface.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // M_CORE_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
