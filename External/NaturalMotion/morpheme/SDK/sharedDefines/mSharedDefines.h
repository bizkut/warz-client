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
#ifndef M_SHARED_DEFINES_H
#define M_SHARED_DEFINES_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "mVersion.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mSharedDefines.h
/// \brief This header contains all values that are to be shared between morpheme and other libraries that do not wish 
/// to include the full morpheme headers, or link against the runtime.
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// Enables the transmission of debug information from runtime to a debugging connection (usually connect).
/// Disabling this define ensures that no extra code is executed or compiled in when building for performance.
/// Disable by defining MR_DISABLE_OUTPUT_DEBUGGING in your project. 
/// Not enabled on SPU.
/// NOTE: Builds that do not have this define enabled will transmit no data to morpheme connect.
///       This includes transform buffers, events, playback positions etc.      
#if !defined(NM_HOST_CELL_SPU) && !defined(MR_DISABLE_OUTPUT_DEBUGGING)
  #define MR_OUTPUT_DEBUGGING
#endif

#if defined(MR_OUTPUT_DEBUGGING)
  #define MR_OUTPUT_DEBUG_ARG(_wat) _wat
  #define MR_NULL_NO_OUTPUT_DEBUGGING(_wat) _wat
  #define MR_USED_FOR_OUTPUT_DEBUGGING(_wat) _wat
#else
  #define MR_OUTPUT_DEBUG_ARG(_wat) NMP_UNUSED(_wat)
  #define MR_NULL_NO_OUTPUT_DEBUGGING(_wat) NULL
  #define MR_USED_FOR_OUTPUT_DEBUGGING(_wat) NMP_UNUSED(_wat)
#endif

//----------------------------------------------------------------------------------------------------------------------
/// Allows us to turn off buffering of node output data independent of the rest of the MR_OUTPUT_DEBUGGING system.
#define MR_ENABLE_ATTRIB_DEBUG_BUFFERING
#if defined(MR_OUTPUT_DEBUGGING) && defined(MR_ENABLE_ATTRIB_DEBUG_BUFFERING)
  #define MR_ATTRIB_DEBUG_BUFFERING
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \typedef TaskID
/// \brief The dispatcher maps TaskIDs to an actual function call that has been registered with it.
/// \ingroup Tasks
typedef uint32_t TaskID;
const TaskID TASK_ID_UNSPECIFIED = 0xFFFFFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef NodeID
/// \brief Type definition for the unique integer ID of a node in the network.
/// \ingroup CoreModule
///
/// Each node has a unique ID assigned to it. Node IDs are monotonically incrementing integer values that are assigned
///  in the asset compiler.
///
/// \see NETWORK_NODE_ID
/// \see INVALID_NODE_ID
typedef uint16_t NodeID;
static const NodeID INVALID_NODE_ID = 0xFFFF;
static const NodeID NETWORK_NODE_ID = 0; ///< The network itself is a node. It always has the ID zero.

//----------------------------------------------------------------------------------------------------------------------
/// \typedef StateID
/// \brief Type definition for the integer ID of a state within a state machine
/// \ingroup CoreModule
///
/// Each state has a unique ID assigned to it. State IDs are monotonically incrementing integer values that are
///  assigned in the asset compiler.
typedef uint16_t StateID;

/// Value used to indicate a bad state machine state ID.
static const StateID INVALID_STATE_ID = 0xFFFF;

/// Value used to represent all possible active states.
static const StateID GLOBAL_STATE_ID = 0xFFFE;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef ConditionIndex
/// \brief Type definition for the index of a condition within a state machine.
/// \ingroup CoreModule
typedef uint16_t ConditionIndex;

/// \typedef StateConditionIndex
/// \brief Used to distinguish a condition index that is local to a specific states array of conditions
///  from the state machines array.
typedef ConditionIndex StateConditionIndex; 

/// Value used to indicate a bad state machine state ID.
static const ConditionIndex INVALID_CONDITION_INDEX = 0xFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef LimbIndex
/// \brief Type definition for the integer ID of a limb
/// \ingroup CoreModule
///
/// Each limb has id, this index is the number of a limb - limb numbers start from zero.
///  in the asset compiler.
typedef uint16_t LimbIndex;
static const LimbIndex INVALID_LIMB_INDEX = 0xFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef PinIndex
/// \brief Type definition for the integer ID of a connection pin on a node.
/// \ingroup CoreModule
typedef uint16_t PinIndex;
static const PinIndex INVALID_PIN_INDEX = 0xFFFF;
static const PinIndex CONTROL_PARAMETER_NODE_PIN_0 = 0; ///< User specifiable Control parameter nodes have single pin, index 0.

//----------------------------------------------------------------------------------------------------------------------
/// \typedef MessageID
/// \brief Type definition for the integer ID of a message in the network
/// \ingroup CoreModule
///
/// Each message has a unique ID assigned to it.  Message IDs are monotonically incrementing integer values that are assigned
///  by morpheme connect.
typedef uint32_t MessageID;
static const MessageID INVALID_MESSAGE_ID = 0xFFFFFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef MessageType
/// \brief Type definition for the message type identifier.
/// \ingroup CoreModule
typedef uint32_t MessageType;
static const MessageType INVALID_MESSAGE_TYPE_ID = 0xFFFFFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef TransitConditType
/// \brief Type definition for transition condition types used on state machine transitions.
/// \ingroup CoreModule
typedef uint16_t TransitConditType;
static const TransitConditType INVALID_CONDITION_TYPE_ID = 0xFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef NodeType
/// \brief Type definition for the node type identifier.
/// \ingroup CoreModule
typedef uint32_t NodeType;
static const NodeType INVALID_NODE_TYPE_ID = 0xFFFFFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// Utilities for generating customer specific identifiers for Nodes, TransitConditions and AttribData types.
#define GEN_NAMESPACED_ID_32(customerID, typeID) ((customerID << 16) + typeID) /// Requires at minimum a 32 bit value: upper 16 bits = customerID, lower 16 bits = typeID.
#define GEN_NAMESPACED_ID_16(customerID, typeID) ((customerID << 14) + typeID) /// Requires at minimum a 16 bit value: upper 2 bits = customerID, lower 14 bits = typeID.
#define NM_ID_NAMESPACE (0) /// Unique customer identifier for NaturalMotion.

//----------------------------------------------------------------------------------------------------------------------
/// NaturalMotion Node Type identifiers.  
/// State machine and control parameter type ids are used by Morpheme:Connect, and cannot be modified by customers. 
#define GEN_NODE_TYPE_ID(customerID, typeID) ((MR::NodeType) GEN_NAMESPACED_ID_32(customerID, typeID))

#define NODE_TYPE_NETWORK              GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 9)
#define NODE_TYPE_STATE_MACHINE        GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 10)

#define NODE_TYPE_CP_FLOAT             GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 20)
#define NODE_TYPE_CP_VECTOR3           GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 21)
#define NODE_TYPE_CP_VECTOR4           GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 22)
#define NODE_TYPE_CP_BOOL              GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 23)
#define NODE_TYPE_CP_INT               GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 24)
#define NODE_TYPE_CP_UINT              GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 25)
#define NODE_TYPE_CP_PHYSICS_OBJECT_POINTER GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 26)

#define NODE_TYPE_ANIMATION            GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 103)
#define NODE_TYPE_ANIM_EVENTS          GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 104)
#define NODE_TYPE_FILTER_TRANSFORMS    GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 105)
#define NODE_TYPE_BLEND_2              GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 107)
#define NODE_TYPE_BLEND_N              GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 108)
#define NODE_TYPE_SINGLEFRAME          GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 109)
#define NODE_TYPE_FREEZE               GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 126)
#define NODE_TYPE_BLEND_2X2            GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 149)
#define NODE_TYPE_BLEND_NXM            GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 147)
#define NODE_TYPE_BLEND_ALL            GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 169)
#define NODE_TYPE_SUBTRACTIVE_BLEND    GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 170)

#define NODE_TYPE_SCATTER_BLEND_1D     GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 180)
#define NODE_TYPE_SCATTER_BLEND_2D     GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 181)
#define NODE_TYPE_SCATTER_BLEND_3D     GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 182)

#define NODE_TYPE_CP_OP_BOOLEAN              GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 102)
#define NODE_TYPE_CP_OP_FUNCTION             GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 110)
#define NODE_TYPE_CP_OP_ARITHMETIC           GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 111)
#define NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3   GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 184)
#define NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 112)
#define NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3 GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 185)
#define NODE_TYPE_CP_OP_NOISE_GEN            GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 113)

#define NODE_TYPE_FEATHER_BLEND_2            GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 114)
#define NODE_TYPE_APPLY_BIND_POSE            GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 115)
#define NODE_TYPE_PHYSICS_GROUPER            GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 116)
#define NODE_TYPE_BEHAVIOUR_GROUPER          GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 117)
#define NODE_TYPE_APPLY_GLOBAL_TIME          GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 119)
#define NODE_TYPE_APPLY_PHYSICS_JOINT_LIMITS GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 124)
#define NODE_TYPE_SET_NON_PHYSICS_TRANSFORMS GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 127)

#define NODE_TYPE_TWO_BONE_IK                GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 120)
#define NODE_TYPE_LOCK_FOOT                  GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 121)
#define NODE_TYPE_HEAD_LOOK                  GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 122)
#define NODE_TYPE_PHYSICS                    GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 123)
#define NODE_TYPE_BEHAVIOUR                  GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 128)
#define NODE_TYPE_HIPS_IK                    GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 129)

#define NODE_TYPE_PERFORMANCE_BEHAVIOUR      GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 183)

#define NODE_TYPE_PLAY_SPEED_MODIFIER        GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 125)
#define NODE_TYPE_SCALE_TO_DURATION          GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 151)

#define NODE_TYPE_EMIT_JOINT_CP_INFO             GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 152)
#define NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 153)

#define NODE_TYPE_CLOSEST_ANIM               GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 130)
#define NODE_TYPE_SWITCH                     GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 131)
#define NODE_TYPE_SEQUENCE                   GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 133)
#define NODE_TYPE_PASSTHROUGH                GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 134)
#define NODE_MIRROR_TRANSFORMS_ID            GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 135)
#define NODE_TYPE_BASIC_UNEVEN_TERRAIN       GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 136)
#define NODE_TYPE_ACTIVE_STATE               GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 137)
#define NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN  GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 138)

#define NODE_TYPE_CP_OP_INT_TO_FLOAT         GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 139)

#define NODE_TYPE_CP_OP_RAY_CAST             GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 141)
#define NODE_TYPE_CP_OP_SMOOTH_FLOAT         GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 142)
#define NODE_TYPE_CP_OP_RAMP_FLOAT           GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 143)
#define NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3    GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 144)
#define NODE_TYPE_CP_OP_FLOAT_TO_INT         GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 145)
#define NODE_TYPE_CP_OP_RANDOM_FLOAT         GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 146)
#define NODE_TYPE_CP_OP_PHYSICS_INFO         GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 148)
#define NODE_TYPE_CP_OP_CONVERT_TO_CHARACTER_SPACE  GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 155)

#define NODE_TYPE_CP_OP_VECTOR3_TO_FLOATS     GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 154)
#define NODE_TYPE_CP_OP_VECTOR3_DOT           GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 164)
#define NODE_TYPE_CP_OP_ORIENTATIONINFREEFALL GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 163)
#define NODE_TYPE_CP_OP_HIT                   GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 165)
#define NODE_TYPE_CP_OP_ROLLDOWNSTAIRS        GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 166)
#define NODE_TYPE_CP_OP_APPLYIMPULSE          GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 168)
#define NODE_TYPE_CP_OP_CONTACTREPORTER       GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 167)
#define NODE_TYPE_CP_OP_VECTOR3_DISTANCE      GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 171)// 169 & 170 used earlier on
#define NODE_TYPE_CP_OP_VECTOR3_CROSSPRODUCT  GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 172)
#define NODE_TYPE_CP_OP_RATE_OF_CHANGE        GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 173)
#define NODE_TYPE_CP_OP_VECTOR3_ANGLE         GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 174)
#define NODE_TYPE_CP_OP_VECTOR3_NORMALISE     GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 175)
#define NODE_TYPE_CP_OP_PHYSICALCONSTRAINT    GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 176)
#define NODE_TYPE_CP_OP_FALLOVERWALL          GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 177)
                             
#define NODE_TYPE_RETARGET                    GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 161)
#define NODE_TYPE_SCALE_CHARACTER             GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 162)

#define NODE_TYPE_GUN_AIM_IK                  GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 150)
#define NODE_TYPE_TRANSIT_SYNC_EVENTS         GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 400)
#define NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 401)
#define NODE_TYPE_TRANSIT                     GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 402)
#define NODE_TYPE_TRANSIT_PHYSICS             GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 403)
#define NODE_TYPE_MODIFY_JOINT_TRANSFORM      GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 178)
#define NODE_TYPE_MODIFY_TRAJECTORY_TRANSFORM GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 179)

#define NODE_TYPE_SMOOTH_TRANSFORMS           GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 500)
#define NODE_TYPE_CP_OP_EMIT_MESSAGE_AND_CP   GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 501)

#define NODE_TYPE_TRAJECTORY_OVERRIDE         GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 502)
#define NODE_TYPE_C_C_OVERRIDE                GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 503)  ///< Character controller override.
#define NODE_TYPE_LIMB_INFO                   GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 504)
#define NODE_TYPE_EXPAND_LIMITS               GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 505)

#define NODE_TYPE_CP_OP_COND_ON_MESSAGE       GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 200)
#define NODE_TYPE_CP_OP_COND_CP_COMPARE       GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 201)

#define NODE_TYPE_CP_OP_TIME_LAG_CP           GEN_NODE_TYPE_ID(NM_ID_NAMESPACE, 202)

//----------------------------------------------------------------------------------------------------------------------
/// NaturalMotion Transition Condition Type identifiers.
/// NOTE: Please do not change these NaturalMotion namespaced type IDs as they may be used by both runtime and connect.
///       (Must be able to recompile connect and runtime).
#define GEN_TRANSITCONDIT_TYPE_ID(customerID, typeID) ((MR::TransitConditType) GEN_NAMESPACED_ID_16(customerID, typeID))

#define TRANSCOND_ON_MESSAGE_ID                           GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 601)
#define TRANSCOND_DISCRETE_EVENT_TRIGGERED_ID             GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 602)
#define TRANSCOND_CROSSED_DURATION_FRACTION_ID            GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 603)
#define TRANSCOND_CROSSED_SYNC_EVENT_BOUNDARY_ID          GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 604)
#define TRANSCOND_IN_SYNC_EVENT_SEGMENT_ID                GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 605)
#define TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID          GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 606)
#define TRANSCOND_FALSE_ID                                GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 607)
#define TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID             GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 608)
#define TRANSCOND_CONTROL_PARAM_IN_RANGE_ID               GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 609)
#define TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 610)
#define TRANSCOND_IN_SYNC_EVENT_RANGE_ID                  GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 611)
#define TRANSCOND_PHYSICS_AVAILABLE_ID                    GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 612)
#define TRANSCOND_PHYSICS_IN_USE_ID                       GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 613)
#define TRANSCOND_GROUND_CONTACT_ID                       GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 614)
#define TRANSCOND_RAY_HIT_ID                              GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 615)
#define TRANSCOND_CONTROL_PARAM_TEST_ID                   GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 616)
#define TRANSCOND_NODE_ACTIVE_ID                          GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 617)
#define TRANSCOND_IN_DURATION_EVENT_ID                    GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 618)
#define TRANSCOND_PHYSICS_MOVING_ID                       GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 619)
#define TRANSCOND_SK_DEVIATION_ID                         GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 620)
#define TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID           GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 621)
#define TRANSCOND_CONTROL_PARAM_UINT_LESS_ID              GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 622)
#define TRANSCOND_CONTROL_PARAM_INT_GREATER_ID            GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 623)
#define TRANSCOND_CONTROL_PARAM_INT_LESS_ID               GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 624)
#define TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID           GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 625)
#define TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID          GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 626)
#define TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID         GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 627)
#define TRANSCOND_CONTROL_PARAM_BOOL_SET_ID               GEN_TRANSITCONDIT_TYPE_ID(NM_ID_NAMESPACE, 628)

//----------------------------------------------------------------------------------------------------------------------
/// NaturalMotion Transition Dead Blend Type identifiers.
#define TRANSDEADBLEND_DEFAULT_ID                         GEN_NAMESPACED_ID_32(NM_ID_NAMESPACE, 701)

//----------------------------------------------------------------------------------------------------------------------
/// \typedef AnimSetIndex
/// \brief A Network can have multiple animation sets, this number identifies one of these.
/// \ingroup CoreModule
typedef uint16_t AnimSetIndex;

/// Used to denote that an AttribData is valid for any animation set.
static const AnimSetIndex ANIMATION_SET_ANY = 0xFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef MR::AttribDataType
/// \brief Describes the data type of a particular MR::AttribData item.
/// \ingroup AttribData
///
/// The AttribDataType is used  by the MR::Manager to register locate, dislocate, copy and create functions
///  against a particular attrib data type.  In this way the MR::Manager implements a pseudo-virtual function table to
///  allow inheritance from MR::AttribData by other classes (i.e. MR::AttribDataFloat) without the need for a virtual
///  function table which would make it harder to compile assets on non-native platforms.
/// \see MR::Manager::registerAttrDataType
typedef uint16_t AttribDataType;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef FrameCount
/// \brief A Networks frame update count.
/// \ingroup CoreModule
///
/// Each Network instance has a frame update count that is used throughout the Networks update code.
typedef uint32_t FrameCount;

/// Indicates that a node is inactive and has not been updated this frame.
static const FrameCount NOT_FRAME_UPDATED = 0xFFFFFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// \enum MR::AttribDataTypeEnum
/// \brief NaturalMotion AttribDataType identifiers.
/// \ingroup AttribData
///
/// NOTE: Please do not change these NaturalMotion namespaced type IDs as they may be used by both runtime and connect.
///       (Must be able to recompile connect and runtime).
//----------------------------------------------------------------------------------------------------------------------
enum AttribDataTypeEnum
{
  // The following simple types correspond to ATTRIB_SEMANTIC_CP_BOOL ... ATTRIB_SEMANTIC_CP_BOOL for simple generation
  ATTRIB_TYPE_BOOL = 0,     ///< Boolean attrib data type.
  ATTRIB_TYPE_UINT,         ///< Unsigned int attrib data type.
  ATTRIB_TYPE_INT,          ///< Int attrib data type.
  ATTRIB_TYPE_FLOAT,        ///< Float attrib data type.
  ATTRIB_TYPE_VECTOR3,      ///< NMP::Vector3 attrib data type.
  ATTRIB_TYPE_VECTOR4,      ///< NMP::Vector4 or NMP::Quat attrib data type.
  // The previous simple types correspond to ATTRIB_SEMANTIC_CP_BOOL ... ATTRIB_SEMANTIC_CP_BOOL for simple generation

  ATTRIB_TYPE_BOOL_ARRAY,   ///< An array of bools.
  ATTRIB_TYPE_INT_ARRAY,    ///< An array of signed integers.
  ATTRIB_TYPE_UINT_ARRAY,   ///< An array of unsigned integers.
  ATTRIB_TYPE_UINT64_ARRAY, ///< An array of unsigned 64 bit integers.
  ATTRIB_TYPE_FLOAT_ARRAY,  ///< An array of floats.

  ATTRIB_TYPE_UPDATE_PLAYBACK_POS,              ///< For setting an absolute time position or updating time with a delta value.
  ATTRIB_TYPE_PLAYBACK_POS,                     ///< Current and last frame times.
  ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS,   ///< For setting an absolute sync event position or updating with a delta value.

  ATTRIB_TYPE_TRANSFORM_BUFFER,                 ///< Transform buffer.
  ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,       ///< The trajectory channels delta transform for this frame.
  ATTRIB_TYPE_TRANSFORM,                        ///< The trajectory channel transform.  (Needed between frames)
  ATTRIB_TYPE_VELOCITY,                         ///< Linear and angular velocity.
  ATTRIB_TYPE_SYNC_EVENT_TRACK,                 ///< Synchronisation event track.
  ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER,            ///< Buffer of sampled events.
  ATTRIB_TYPE_DURATION_EVENT_TRACK_SET,         ///< A set of duration event tracks.

  ATTRIB_TYPE_RIG,                              ///< Describes hierarchy of bones etc.
  ATTRIB_TYPE_SOURCE_ANIM,                      ///< A source animation in any format.

  ATTRIB_TYPE_RIG_TO_ANIM_MAP,                  ///< Maps rig channels to animation channels.

  ATTRIB_TYPE_SOURCE_EVENT_TRACKS,              ///< A set of source discrete event tracks.

  ATTRIB_TYPE_HEAD_LOOK_SETUP,            ///< Wraps the params needed to run head look IK solver.
  ATTRIB_TYPE_HEAD_LOOK_CHAIN,            ///< Describes a head look IK chain.

  ATTRIB_TYPE_GUN_AIM_SETUP,              ///< Wraps the params needed to run Gun Aim Node.
  ATTRIB_TYPE_GUN_AIM_IK_CHAIN,           ///< Describes a per anim set gun aim IK chain.

  ATTRIB_TYPE_TWO_BONE_IK_SETUP,          ///< Wraps all the params needed to run the two bone IK solver.
  ATTRIB_TYPE_TWO_BONE_IK_CHAIN,          ///< Describes a standard two bone IK chain with end-middle-root joints
                                          ///<  indices.

  ATTRIB_TYPE_LOCK_FOOT_SETUP,            ///< Wraps the params needed to run the lock foot IK solver.
  ATTRIB_TYPE_LOCK_FOOT_CHAIN,            ///< Describes a lock foot ik chain.
  ATTRIB_TYPE_LOCK_FOOT_STATE,            ///< Stores lock foot state variables.

  ATTRIB_TYPE_HIPS_IK_DEF,                ///< Settings for the Hips IK node
  ATTRIB_TYPE_HIPS_IK_ANIM_SET_DEF,       ///< Geometry of the Hips IK chains

  ATTRIB_TYPE_CLOSEST_ANIM_DEF,           ///< Setup data for the closest anim node.
  ATTRIB_TYPE_CLOSEST_ANIM_DEF_ANIM_SET,  ///< Setup data for the closest anim node (anim set specific).
  ATTRIB_TYPE_CLOSEST_ANIM_STATE,         ///< State data for the closest anim node

  ATTRIB_TYPE_STATE_MACHINE_DEF,          ///< Holds the full definition of a state machine.
  ATTRIB_TYPE_STATE_MACHINE,              ///< Holds the current state of a state machine.

  ATTRIB_TYPE_PHYSICS_RIG,                ///< Holds a pointer to the PhysicsRig
  ATTRIB_TYPE_PHYSICS_RIG_DEF,            ///< Holds a pointer to the PhysicsRigDef
  ATTRIB_TYPE_CHARACTER_PROPERTIES,       ///< The current world root transform of the Network/game character and information reflecting
                                          ///<  the state of the Networks associated character controller if there is one.
  ATTRIB_TYPE_CHARACTER_CONTROLLER_DEF,   ///< Holds a pointer to the character controller def
  ATTRIB_TYPE_ANIM_TO_PHYSICS_MAP,        ///< Stores the bone look up table from anim to physics and physics to
                                          ///<  anim.

  ATTRIB_TYPE_PHYSICS_SETUP,              ///< Wraps the params needed for the physics node.
  ATTRIB_TYPE_PHYSICS_SETUP_ANIM_SET,     ///< Wraps the params needed for the physics node (anim set specific).
  ATTRIB_TYPE_PHYSICS_STATE,              ///< Stores physics state variables.
  ATTRIB_TYPE_PHYSICS_INITIALISATION,     ///< Physics initialisation
  ATTRIB_TYPE_PHYSICAL_EFFECT_DATA,       ///< Used to call PhysicalEffect inputs
  ATTRIB_TYPE_COLLISION_FILTER_MASK,      ///< Wraps physics engine specific collision filter mask info.

  ATTRIB_TYPE_PHYSICS_GROUPER_CONFIG,     ///< Stores physics grouper state variables.

  ATTRIB_TYPE_OPERATOR_APPLYIMPULSE_DEF,  ///< Apply impulse operator definition

  ATTRIB_TYPE_FLOAT_OPERATION,            ///< A float control parameter operation.
  ATTRIB_TYPE_2_FLOAT_OPERATION,          ///< A two float control parameter operation.
  ATTRIB_TYPE_SMOOTH_FLOAT_OPERATION,     ///< Smooth float operator per instance data
  ATTRIB_TYPE_RATE_OF_CHANGE_OPERATION,   ///< Rate of change operator per instance data
  ATTRIB_TYPE_RANDOM_FLOAT_OPERATION,     ///< Random float RNG and elapsed duration data
  ATTRIB_TYPE_BOOLEAN_OPERATION,          ///< Describes a boolean operation.
  ATTRIB_TYPE_VALUE_COMPARE_OPERATION,    ///< Describes a comparison of 2 values that results in a bool value e.g. >, <
  ATTRIB_TYPE_DISCRETE_EVENT_TRIGGERED,   ///< Describes a discrete event that is to watched for emission from a specified node.

  ATTRIB_TYPE_RANDOM_FLOAT_DEF,           ///< Random float operator node definition data.
  ATTRIB_TYPE_NOISE_GEN_DEF,              ///< Noise generation node definition data
  ATTRIB_TYPE_SWITCH_DEF,                 ///< Switch node definition data
  ATTRIB_TYPE_RAY_CAST_DEF,               ///< Ray cast operator node definition data

  ATTRIB_TYPE_TRANSIT_DEF,                ///< Transition node definition data.
  ATTRIB_TYPE_TRANSIT_SYNC_EVENTS_DEF,    ///< Transition sync events definition data.
  ATTRIB_TYPE_TRANSIT_SYNC_EVENTS,        ///< Transition sync events state data.

  ATTRIB_TYPE_DEAD_BLEND_DEF,             ///< Definition data of a dead blend.
  ATTRIB_TYPE_DEAD_BLEND_STATE,           ///< Active state data of a dead blend.

  ATTRIB_TYPE_TRAJECTORY_OVERRIDE_DEF,

  ATTRIB_TYPE_BLEND_NXM_DEF,              ///< Definition data of a blend NxM.

  ATTRIB_TYPE_ANIM_MIRRORED_MAPPING,      ///< Mapping info used when mirroring an animation.

  ATTRIB_TYPE_PLAYBACK_POS_INIT,          ///< Structure used by transitions to pass down playback initialisation
                                          ///<  information to its destination state.
  ATTRIB_TYPE_EMITTED_MESSAGE_MAP,        ///< Custom DataBuffer used to hold per node emitted request mappings

  ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_SETUP,                    ///< Wraps the params needed to run the lock foot IK solver.
  ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_SETUP,                 ///< IK setup information for the uneven terrain node
  ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET,      ///< Stores ray cast information from physics step
  ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_STATE,                 ///< Stores uneven terrain IK state variables.
  ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_CHAIN,                    ///< Describes an uneven terrain IK chain.
  ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_IK_PREDICTION_STATE, ///< IK setup information for the predictive uneven terrain node
  ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_FOOT_LIFTING_STATE,  ///< State information for predictive foot lifting
  ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_PREDICTION_DEF,      ///< Definition data for the predictive uneven terrain node

  ATTRIB_TYPE_SCATTER_BLEND_ANALYSIS_DEF, ///< scatter blend analysis def data
  ATTRIB_TYPE_SCATTER_BLEND_1D_DEF,       ///< 1D scatter blend def data
  ATTRIB_TYPE_SCATTER_BLEND_2D_DEF,       ///< 2D scatter blend def data
  ATTRIB_TYPE_SCATTER_BLEND_2D_STATE,     ///< 2D scatter blend state data

  ATTRIB_TYPE_EMIT_MESSAGE_ON_CP_VALUE,   ///< Def date for NodeOperatorEmitMessageOnCPValue.

  ATTRIB_TYPE_PHYSICS_INFO_DEF,           ///< Physics info operator node definition data

  ATTRIB_TYPE_PHYSICS_OBJECT_POINTER,     ///< Pointer to a physics object (whatever physics SDK is being used)

  ATTRIB_TYPE_JOINT_LIMITS,               ///< Kinematic joint limits for a rig.

  ATTRIB_TYPE_BLEND_FLAGS,
  ATTRIB_TYPE_BLEND_WEIGHTS,
  ATTRIB_TYPE_FEATHER_BLEND2_CHANNEL_ALPHAS,

  ATTRIB_TYPE_RETARGET_STATE,              ///< Retarget solver and retarget state data.
  ATTRIB_TYPE_RIG_RETARGET_MAPPING,        ///< Info for retargeting between rigs.
  ATTRIB_TYPE_SCALECHARACTER_STATE,        ///< Scale character state data.
  ATTRIB_TYPE_RETARGET_STORAGE_STATS,      ///< Retarget solver memory allocation info for a network (or group of rigs).

  ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS_DEF, ///< A non anim set specific set of conditions controlling when to apply a bunch of character controller override attributes.
  ATTRIB_TYPE_C_C_OVERRIDE_PROPERTIES_DEF, ///< Anim set specific set of character controller override attributes.
  ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS,     ///< Active state data for character controller override node.

  ATTRIB_TYPE_MODIFY_JOINT_DEF,            ///< State definition data for the ModifyJoint node.
  ATTRIB_TYPE_MODIFY_TRAJECTORY_DEF,       ///< State definition data for the ModifyTrajectory node.

  ATTRIB_TYPE_TIME_LAG_DEF,                ///< Def data for the time lag operator node.
  ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT,        ///< Float state data for a TimeLagInput operator node. (Specialization of AttribDataTimeLagInput).
  ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3,      ///< Vector3 state data for a TimeLagInput operator node. (Specialization of AttribDataTimeLagInput).
  ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4,      ///< Vector4 state data for a TimeLagInput operator node. (Specialization of AttribDataTimeLagInput).
  ATTRIB_TYPE_TIME_LAG_INPUT_INT,          ///< Int state data for a TimeLagInput operator node. (Specialization of AttribDataTimeLagInput).
  ATTRIB_TYPE_TIME_LAG_INPUT_BOOL,         ///< Bool state data for a TimeLagInput operator node. (Specialization of AttribDataTimeLagInput).

  ATTRIB_TYPES_CORE_MAX,

  INVALID_ATTRIB_TYPE = 0xFFFF
};

//----------------------------------------------------------------------------------------------------------------------
/// \enum MR::AttribDataSemantic
/// \brief Describes the meaning of a particular MR::AttribData item.
/// \ingroup AttribData
///
/// The AttribDataSemantic enumeration is a list of all the different semantics that an individual MR::AttribData item
///  may represent. The semantic forms part of the MR::AttribAddress which uniquely identifies a piece of AttribData
///  in the network. It is used in queuing functions to identify the required data for an operation to be executed, and
///  in the Task functions themselves in debug configurations to ensure that the correct data has been retrieved.
///  NOTE: Please do not change these NaturalMotion semantic IDs as they may be used by both runtime and connect.
///       (Must be able to recompile connect and runtime).
//----------------------------------------------------------------------------------------------------------------------
typedef uint16_t AttribDataSemantic;

enum AttribDataSemanticEnum
{
  ATTRIB_SEMANTIC_UPDATE_TIME_POS = 0,              ///< For setting an absolute time position.
                                                    ///<  or updating time with a delta value.
  ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS,            ///< For setting an absolute event position
                                                    ///<  or updating with a delta value.
  ATTRIB_SEMANTIC_TIME_POS,                         ///< Current and last frame times.
  ATTRIB_SEMANTIC_ANIM_SAMPLE_POS,                  ///< Current and last frame anim sample positions.
  ATTRIB_SEMANTIC_FRACTION_POS,                     ///< Current and last frame fractional positions.

  ATTRIB_SEMANTIC_LOOPED_ON_UPDATE,                 ///< Has the playback pos of this node looped.

  ATTRIB_SEMANTIC_TRANSFORM_BUFFER,                 ///< Transform buffer.
  ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,       ///< The trajectory channels delta transform for this frame.
  ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM,             ///< The trajectory channel transform.  (Needed between frames)
  ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,///< A transform buffer with the trajectory in the first channel
  ATTRIB_SEMANTIC_VELOCITY,                         ///<
  ATTRIB_SEMANTIC_TRANSFORM_RATES,                  ///<
  ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,                 ///< Synchronization event track.
  ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,            ///< Buffer of sampled events.
  ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET,         ///< A set of duration event tracks.

  ATTRIB_SEMANTIC_LOOP,                             ///< Bool - should the update of this node loop when it reached the end.
  ATTRIB_SEMANTIC_RIG,                              ///< Describes hierarchy of bones etc.
  ATTRIB_SEMANTIC_SOURCE_ANIM,                      ///< A source animation in any format.

  ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX,           ///< DefData: Where this node should start playback, specified in
                                                    ///<  sync space.
 
  ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS,              ///<

  ATTRIB_SEMANTIC_ACTIVE_ANIM_SET_INDEX,            ///< The index of the animation set currently in use by the network.

  ATTRIB_SEMANTIC_BLEND_FLAGS,                      ///< Blend node def data flags
  ATTRIB_SEMANTIC_BLEND_WEIGHTS,                    ///< Blend node internal blend weight interpolants
  // The following CP semantics correspond to ATTRIB_TYPE_BOOL ... ATTRIB_TYPE_VECTOR4 for simple generation
  ATTRIB_SEMANTIC_CP_BOOL,                          ///< A control parameter of type bool
  ATTRIB_SEMANTIC_CP_UINT,                          ///< A control parameter of type unsigned int 
  ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER,        ///< A control parameter of type physics object pointer
  ATTRIB_SEMANTIC_CP_INT,                           ///< A control parameter of type int
  ATTRIB_SEMANTIC_CP_FLOAT,                         ///< A control parameter of type float
  ATTRIB_SEMANTIC_CP_VECTOR3,                       ///< A control parameter of type NMP::Vector3
  ATTRIB_SEMANTIC_CP_VECTOR4,                       ///< A control parameter of type NMP::Vector4 or NMP::Quat
  // The previous CP semantics correspond to ATTRIB_TYPE_BOOL ... ATTRIB_TYPE_VECTOR4 for simple generation

  ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING,             ///< Info about a rig for retargeting it to other rigs
  ATTRIB_SEMANTIC_RETARGET_STORAGE_STATS,           ///< Network-wide info for correct runtime allocation of retargeter
  ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING,            ///< A mapping attribute used to map left and right bones on the rig
  ATTRIB_SEMANTIC_SYNC_EVENT_OFFSET,                ///< An offset applied to the sync event track
                                                    ///<  modification in the mirror node

  ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS,               ///< 
  ATTRIB_SEMANTIC_BONE_WEIGHTS,                     ///< 

  ATTRIB_SEMANTIC_BONE_IDS,                         ///< An array of bone IDs. (Usually specified against a Rig)

  ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                ///< Holds node specific static data.
  ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,       ///< Holds node specific animation set dependent data.
  ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,              ///< Holds node specific state variables.
  ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2,            ///< Holds *another* node specific state. TODO: Try and remove this
                                                    ///<  complexity.

  ATTRIB_SEMANTIC_PHYSICS_RIG_DEF,                  ///< Holds a pointer to the PhysicsRigDef.
  ATTRIB_SEMANTIC_PHYSICS_RIG,                      ///< Holds a pointer to the PhysicsRig

  ATTRIB_SEMANTIC_CHARACTER_PROPERTIES,             ///< The current world root transform of the Network/game character and information reflecting
                                                    ///<  the state of the Networks associated character controller if there is one.
  ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF,         ///<
  ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP,              ///<

  ATTRIB_SEMANTIC_PHYSICS_INITIALISATION,           ///< Data needed to initialise physics rig
  ATTRIB_SEMANTIC_OUTPUT_MASK,                      ///< Array of bools indicating if the anim channel should be calculated.
  ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS,           ///<
  ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED,     ///<
  ATTRIB_SEMANTIC_PHYSICS_UPDATED,                  ///<
  ATTRIB_SEMANTIC_ROOT_UPDATED,                     ///<
  ATTRIB_SEMANTIC_MERGED_PHYSICS_RIG_TRANSFORM_BUFFER, ///< Used to make sure that multiple physics nodes synchronise
                                                       ///<  their control of the physical body.

  ATTRIB_SEMANTIC_TRANSIT_COMPLETE,                 ///< 
  ATTRIB_SEMANTIC_TRANSIT_REVERSED,                 ///< Used to determine wheteher we should transit to origin or dest
                                                    ///<  in reversible transitions.

  ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP,       ///< Whether and how to match up events when blending 2 duration event tracks.
                                                    ///<  See DURATION_EVENT_MATCH_...

  ATTRIB_SEMANTIC_DEAD_BLEND_DEF,                   ///< Definition data of a dead blend.
  ATTRIB_SEMANTIC_DEAD_BLEND_STATE,                 ///< Active state data of a dead blend.
  ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE,      ///< Usually a AttribDataTransformBuffer that is used to cache a nodes 
                                                    ///<  transform buffer internally. (Dead blending transforms etc).

  ATTRIB_SEMANTIC_PLAYBACK_POS_INIT,                ///< Structure used by transitions to pass down playback
                                                    ///<  initialisation information to its destination state.

  ATTRIB_SEMANTIC_EMITTED_MESSAGES_MAP,             ///< 
  ATTRIB_SEMANTIC_EMITTED_MESSAGES,                 ///< 

  ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP,            ///< IK setup information for the uneven terrain node
  ATTRIB_SEMANTIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET, ///< Foot lifting target for the uneven terrain node
  ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_DEF,    ///< Node definition data for the predictive uneven terrain node
  ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_STATE,  ///< Foot plant prediction state for the uneven terrain node

  ATTRIB_SEMANTIC_JOINT_LIMITS,                     ///< Kinematic joint limits for a rig.
  ATTRIB_SEMANTIC_RETARGET_RIG_MAP,                 ///< Generic mapping between joints of two rigs.

  ATTRIB_SEMANTIC_UPDATE_TIME_POS_T_MINUS_ONE,      ///< Dead blend cache data, update time pos from previous frame
  ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE,     ///< Dead blend cache data, trajectory delta from previous frame
  ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_ONE,     ///< Dead blend cache data, transform buffer from previous frame
  ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_TWO,     ///< Dead blend cache data, transform buffer from previous but 1 frame

  ATTRIB_SEMANTIC_RETARGET_STATE,                   ///< Persistent state data for retargeting.

  ATTRIB_SEMANTIC_NM_MAX,

  ATTRIB_SEMANTIC_NA = 0xFFFF
};

//----------------------------------------------------------------------------------------------------------------------
/// Animation format type identifier.
typedef uint8_t AnimType;

#define ANIM_TYPE_MBA ((MR::AnimType) GEN_NAMESPACED_ID_32(NM_ID_NAMESPACE, 0))
#define ANIM_TYPE_ASA ((MR::AnimType) GEN_NAMESPACED_ID_32(NM_ID_NAMESPACE, 1))
#define ANIM_TYPE_NSA ((MR::AnimType) GEN_NAMESPACED_ID_32(NM_ID_NAMESPACE, 2))
#define ANIM_TYPE_QSA ((MR::AnimType) GEN_NAMESPACED_ID_32(NM_ID_NAMESPACE, 3))

/// The maximum name length for an animation format.
static const uint32_t ANIM_FORMAT_MAX_STRING_LENGTH = 19;

//----------------------------------------------------------------------------------------------------------------------
/// \brief Describes the types of data that can be output from a node.
///
/// This covers debugging data and AttributeData objects that are passed between nodes.
typedef uint32_t NodeOutputDataType;
static const NodeOutputDataType INVALID_NODE_OUTPUT_DATA_TYPE = 0xFFFF;

#define NODE_OUTPUT_DATA_TYPE_TRANSFORMS                  GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 0)  ///< Count (unsigned int), Rotation buffer (NMP::Quat), Translation buffer (NMP::Vector3)
#define NODE_OUTPUT_DATA_TYPE_TRAJECTORY_CHANNEL_DELTAS   GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 1)  ///< Translation delta (NMP::Vector3), Rotation delta (NMP::Quat)
#define NODE_OUTPUT_DATA_TYPE_TIME                        GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 2)  ///< Float
#define NODE_OUTPUT_DATA_TYPE_TIME_UPDATE                 GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 3) 
#define NODE_OUTPUT_DATA_TYPE_SYNC_EVENT_TRACK            GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 4)  ///< Events count (unsigned int), Events buffer (start float, duration float)
#define NODE_OUTPUT_DATA_TYPE_SYNC_EVENT                  GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 5)  ///< Event index (unsigned int), Event fraction (float)
#define NODE_OUTPUT_DATA_TYPE_FLOAT                       GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 6)  ///< 
#define NODE_OUTPUT_DATA_TYPE_VECTOR3                     GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 7)  ///< 
#define NODE_OUTPUT_DATA_TYPE_VECTOR4                     GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 8)  ///< 
#define NODE_OUTPUT_DATA_TYPE_BOOL                        GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 9)  ///< 
#define NODE_OUTPUT_DATA_TYPE_CONDITION_SET_STATE         GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 10) ///< Gets the status of a Transition Condition in a StateMachine.
#define NODE_OUTPUT_DATA_TYPE_INT                         GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 11)
#define NODE_OUTPUT_DATA_TYPE_UINT                        GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 12)
#define NODE_OUTPUT_DATA_TYPE_PHYSICS_OBJECT_POINTER      GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 13)
#define NODE_OUTPUT_DATA_TYPE_TRAJECTORY_DELTA_TRANSFORMS GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 14)

#define NODE_OUTPUT_DATA_TYPE_RUNTIME_TYPE                GEN_NAMESPACED_ID_16(NM_ID_NAMESPACE, 50) ///< Indicates that we are using a runtime AttribAddress to identify this data
                                                                                                    ///< rather than the old NODE_OUTPUT_DATA_TYPE system.

//----------------------------------------------------------------------------------------------------------------------
/// \brief When reporting performance timings of various node/task activities, this enum
/// distinguishes what was timed
enum NodeTaskTimingType
{
  NTTaskExecution,         ///< timing of all tasks executed for a given node
  NTTaskQueueing           ///< time taken to queue tasks for a given node
};

//----------------------------------------------------------------------------------------------------------------------
/// NaturalMotion Node Type identifiers.
/// Message type ids are used by Morpheme:Connect, and cannot be modified by customers.
#define GEN_MESSAGE_TYPE_ID(customerID, typeID) ((MR::MessageType) GEN_NAMESPACED_ID_32(customerID, typeID))

#define MESSAGE_TYPE_REQUEST                 GEN_MESSAGE_TYPE_ID(NM_ID_NAMESPACE, 10)

#define MESSAGE_TYPE_HIT                     GEN_MESSAGE_TYPE_ID(NM_ID_NAMESPACE, 101)
#define MESSAGE_TYPE_RETARGET_UPDATE_OFFSETS GEN_MESSAGE_TYPE_ID(NM_ID_NAMESPACE, 102)
#define MESSAGE_TYPE_RETARGET_UPDATE_SCALE   GEN_MESSAGE_TYPE_ID(NM_ID_NAMESPACE, 103)
#define MESSAGE_TYPE_SCALE_CHARACTER         GEN_MESSAGE_TYPE_ID(NM_ID_NAMESPACE, 104)


//----------------------------------------------------------------------------------------------------------------------
/// NaturalMotion Prediction Model identifiers.
#define PREDICTMODELTYPE_ND_MESH  0


//----------------------------------------------------------------------------------------------------------------------
/// \def ANIM_BROWSER_ANIM_FILE_NAME
/// \brief Name of the file where output animation data will be written to for later loading and use by the animation
///  browser.This define is also used by MorphemeAssetCompiler.cpp which also enforces the convention of writing to this file
#define ANIM_BROWSER_ANIM_FILE_NAME "animBrowserAnim"

/// \def ANIM_BROWSER_BUNDLE_FILE_NAME
///
/// Playing an animation in the anim browser creates and runs a small single node network to provide the playback
///  facilities. This is the name of the file where an output temporary Network will be written to for later loading and
///  use by the animation browser. The file extension is .nmb (NaturalMotion bundle).
#define ANIM_BROWSER_BUNDLE_FILE_NAME "animBrowserNetwork.nmb"

} // namespace MR


namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
/// Specific to euphoria modules.
static const uint32_t INVALID_MODULE_INDEX = 0xffffffff;

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // M_SHARED_DEFINES_H
//----------------------------------------------------------------------------------------------------------------------
