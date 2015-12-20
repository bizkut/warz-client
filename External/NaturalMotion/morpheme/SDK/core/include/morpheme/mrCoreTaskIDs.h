// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_CORE_TASK_IDS_H
#define MR_CORE_TASK_IDS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrDispatcher.h"

#if defined(NM_HOST_CELL_PPU) || defined(NM_HOST_CELL_SPU)
  #include <cell/spurs.h>
#endif
//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_HOST_CELL_SPU
// We do not bother having string literals on SPU because it would waste precious memory.
  #define TASK_NAME_ARG(_fnName) _fnName, NULL
#else
  #define TASK_NAME_ARG(_fnName) _fnName, #_fnName
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::CoreTaskIDs
/// \brief
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class CoreTaskIDs
{
public:
  static bool registerNMCoreTasks(Dispatcher* dispatcher);
  static bool registerNMPhysicsTasks(Dispatcher* dispatcher);

  /// \brief Find the appropriate create reference function to queue dependent on the specified semantic.
  static TaskID getCreateReferenceToInputTaskID(AttribDataSemantic semantic);

public:
  //---------------------------------
  /// \enum  MR::CoreTaskIDs::TaskSourceFiles
  /// \brief
  /// \ingroup
  //---------------------------------
  enum TaskOverlays
  {
    // Core task overlays
    MR_TASKOVERLAY_COMMONTASKS,
    MR_TASKOVERLAY_EXTRACTJOINTINFOTASKS,
    MR_TASKOVERLAY_EXTRACTJOINTINFOTRAJECTORYANDTRANSFORMSTASKS,
    MR_TASKOVERLAY_MIRRORTASKS,
    MR_TASKOVERLAY_ANIMSAMPLETASKSASA,
    MR_TASKOVERLAY_ANIMSAMPLETASKSMBA,
    MR_TASKOVERLAY_ANIMSAMPLETASKSNSA,
    MR_TASKOVERLAY_ANIMSAMPLETASKSQSA,
    MR_TASKOVERLAY_TRAJECTORYSAMPLETASKSASA,
    MR_TASKOVERLAY_TRAJECTORYSAMPLETASKSMBA,
    MR_TASKOVERLAY_TRAJECTORYSAMPLETASKSNSA,
    MR_TASKOVERLAY_TRAJECTORYSAMPLETASKSQSA,
    MR_TASKOVERLAY_TRAJECTORYANDTRANSFORMSSAMPLETASKSASA,
    MR_TASKOVERLAY_TRAJECTORYANDTRANSFORMSSAMPLETASKSMBA,
    MR_TASKOVERLAY_TRAJECTORYANDTRANSFORMSSAMPLETASKSNSA,
    MR_TASKOVERLAY_TRAJECTORYANDTRANSFORMSSAMPLETASKSQSA,
    MR_TASKOVERLAY_EVENTTRACKDURATIONTASKS,
    MR_TASKOVERLAY_BLENDTRANSFORMTASKS,
    MR_TASKOVERLAY_BLENDTRAJECTORYANDTRANSFORMSTASKS,
    MR_TASKOVERLAY_BLENDTRANSFORMSPASSDESTTRAJTRASKS,
    MR_TASKOVERLAY_BLENDTRANSFORMSPASSSOURCETRAJTRASKS,
    MR_TASKOVERLAY_BLENDALLTASKS,
    MR_TASKOVERLAY_FEATHERTRAJECTORYANDTRANSFORMBLENDTASKS,
    MR_TASKOVERLAY_HEADLOOKIKTASKS,
    MR_TASKOVERLAY_HEADLOOKTRAJECTORYANDTRANSFORMSIKTASKS,
    MR_TASKOVERLAY_GUNAIMIKTASKS,
    MR_TASKOVERLAY_GUNAIMTRAJECTORYANDTRANSFORMSIKTASKS,
    MR_TASKOVERLAY_TWOBONEIKTASKS,
    MR_TASKOVERLAY_HIPSIKTASKS,
    MR_TASKOVERLAY_FOOTLOCKIKTASKS,
    MR_TASKOVERLAY_FOOTLOCKTRAJECTORYANDTRANSFORMIKTASKS,
    MR_TASKOVERLAY_TRAJECTORYBLENDTASKS,
    MR_TASKOVERLAY_TRANSITTASKS,
    MR_TASKOVERLAY_REFERENCETASKS,
    MR_TASKOVERLAY_RETARGETTASKS,
    MR_TASKOVERLAY_BASICUNEVENTERRAINIKTRANSFORMTASKS,
    MR_TASKOVERLAY_BASICUNEVENTERRAINIKSETUPTASKS,
    MR_TASKOVERLAY_PREDICTIVEUNEVENTERRAINIKTRANSFORMTASKS,
    MR_TASKOVERLAY_PREDICTIVEUNEVENTERRAINIKSETUPTASKS,
    MR_TASKOVERLAY_BLEND2x2TASKS,
    MR_TASKOVERLAY_TRIANGLEBLENDTASKS,
    MR_TASKOVERLAY_SCALETASKS,
    MR_TASKOVERLAY_NETWORKPREDICTIONTASKS,
    MR_TASKOVERLAY_MODIFYTRANSFORMTASKS,

    // Physics task overlays
    MR_TASKOVERLAY_PHYSICS_TRANSITTASKS,

    MR_TASKOVERLAY_MAX_FILES
  };

  NM_ASSERT_COMPILE_TIME(MR_TASKOVERLAY_MAX_FILES < 256);

  //---------------------------------
  /// \enum  MR::CoreTaskIDs::TaskIDs
  /// \brief
  /// \ingroup
  //---------------------------------
  enum TaskIDs
  {
    MR_TASKID_ANIMSYNCEVENTSUPDATETIMEPOS,                  ///< Using a delta time update: time pos, event pos and bufferpos.
    MR_TASKID_ANIMSYNCEVENTSUPDATESYNCEVENTPOS,             ///< Using a delta event update: time pos, event pos and bufferpos.

    MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCEASA,            ///< Use a buffer playback position to sample the transforms from a source anim.
    MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCEMBA,            ///< Use a buffer playback position to sample the transforms from a source anim.
    MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCENSA,            ///< NOTE: We have a task per anim source type so that polymorphism happens
    MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCEQSA,            ///< on queuing rather than in the task.
                                                            ///< Reserving this decision until the task would mean that we would have to
                                                            ///< queue the decompression code for all possible anim formats; this might
                                                            ///< not fit in to the available code space (on SPU for example).

    MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCEASA, ///< Update the trajectory transform from a source ASA trajectory channel.
    MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCEMBA, ///< Update the trajectory transform from a source MBA trajectory channel.
    MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCENSA, ///< Update the trajectory transform from a source NSA trajectory channel.
    MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCEQSA, ///< Update the trajectory transform from a source QSA trajectory channel.

    MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCEASA, ///< Update the transforms including the trajectory from a ASA source.
    MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCEMBA, ///< Update the transforms including the trajectory from a MBA source.
    MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCENSA, ///< Update the transforms including the trajectory from a NSA source.
    MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCEQSA, ///< Update the transforms including the trajectory from a QSA source.

    MR_TASKID_INITSYNCEVENTTRACKFROMDISCRETEEVENTTRACK,
    MR_TASKID_INIT_UNIT_LENGTH_SYNC_EVENT_TRACK,
    MR_TASKID_SAMPLEEVENTSFROMSOURCETRACKS,
    MR_TASKID_SAMPLEEVENTSFROMSOURCETRACKSINCDURATIONEVENTS,
    MR_TASKID_INITEVENTTRACKDURATIONSETFROMSOURCE,
    MR_TASKID_INIT_EMPTY_EVENTTRACKDURATIONSET,
    MR_TASKID_INIT_SAMPLED_EVENTS_TRACK,
    MR_TASKID_BUFFER_LAST_FRAME_TRANSFORM_BUFFER,
    MR_TASKID_BUFFER_LAST_FRAME_TRAJECTORY_AND_TRANSFORMS,
    MR_TASKID_BUFFER_LAST_FRAME_TRANSFORMS_ZERO_TRAJECTORY,  ///< Buffers the previous frames transforms and sets the trajectory delta to zero.

    MR_TASKID_BLEND2TRANSFORMBUFFSADDATTADDPOS,              ///< Blend 2 transform buffers together, Additive Attitude and Additive Position.
    MR_TASKID_BLEND2TRANSFORMBUFFSADDATTINTERPPOS,           ///< Blend 2 transform buffers together, Additive Attitude and Interpolate Position.
    MR_TASKID_BLEND2TRANSFORMBUFFSINTERPATTADDPOS,           ///< Blend 2 transform buffers together, Interpolate Attitude and Additive Position.
    MR_TASKID_BLEND2TRANSFORMBUFFSINTERPATTINTERPPOS,        ///< Blend 2 transform buffers together, Interpolate Attitude and Interpolate Position.
    MR_TASKID_BLEND2TRANSFORMBUFFSSUBTRACTATTSUBTRACTPOS,    ///< Blend 2 transform buffers together, Subtractive Attitude and Subtractive Position.

    MR_TASKID_BLEND2X2TRANSFORMBUFFSINTERPATTINTERPPOS,      ///< Blend 2x2 transform buffers together, Interpolate Attitude and Interpolate Position.
    MR_TASKID_BLEND2X2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
    MR_TASKID_BLEND2X2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,
    MR_TASKID_BLEND2X2TRAJECTORYANDTRANSFORMSINTERPTRAJ,
    MR_TASKID_BLEND2X2TRAJECTORYANDTRANSFORMSSLERPTRAJ,
    MR_TASKID_COMBINE2X2SAMPLEDEVENTSBUFFERS,
    MR_TASKID_COMBINE2X2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS,
    MR_TASKID_BLEND2X2DURATIONEVENTTRACKSETS,
    MR_TASKID_BLEND2X2SYNCEVENTTRACKS,                       ///< Blend 4 synchronization event tracks with a weighting.

    MR_TASKID_TRIANGLE_BLEND_TRANSFORMBUFFSINTERPATTINTERPPOS,
    MR_TASKID_TRIANGLE_BLEND_TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
    MR_TASKID_TRIANGLE_BLEND_TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,
    MR_TASKID_TRIANGLE_BLEND_TRAJECTORYANDTRANSFORMSINTERPTRAJ,
    MR_TASKID_TRIANGLE_BLEND_TRAJECTORYANDTRANSFORMSSLERPTRAJ,
    MR_TASKID_TRIANGLE_COMBINE_SAMPLEDEVENTSBUFFERS,
    MR_TASKID_TRIANGLE_COMBINE_SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS,
    MR_TASKID_TRIANGLE_BLEND_DURATIONEVENTTRACKSETS,
    MR_TASKID_TRIANGLE_BLEND_SYNCEVENTTRACKS,

    MR_TASKID_BLENDALLTRANSFORMBUFFSINTERPATTINTERPPOS,      ///< Blend every transform buffers together, Interpolate Attitude and Interpolate Position.
    MR_TASKID_COMBINEALLSAMPLEDEVENTSBUFFERS,
    MR_TASKID_COMBINEALLSAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS,
    MR_TASKID_BLENDALLTRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
    MR_TASKID_BLENDALLTRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,

    MR_TASKID_BLENDALLTRAJECTORYDELTAANDTRANSFORMSSLERPPOS,
    MR_TASKID_BLENDALLTRAJECTORYDELTAANDTRANSFORMSINTERPPOS,

    MR_TASKID_BLENDALLDURATIONEVENTTRACKSETS,
    MR_TASKID_BLENDALLSYNCEVENTTRACKS,                       ///< Blend every synchronization event tracks with a weighting.

    MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSADDATTADDPOS,       ///< Feather Blend 2 transform buffers together, Additive Attitude and Additive Position.
    MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSADDATTINTERPPOS,    ///< Feather Blend 2 transform buffers together, Additive Attitude and Interpolate Position.
    MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSINTERPATTADDPOS,    ///< Feather Blend 2 transform buffers together, Interpolate Attitude and Additive Position.
    MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSINTERPATTINTERPPOS, ///< Feather Blend 2 transform buffers together, Interpolate Attitude and Interpolate Position.

    MR_TASKID_SETNONPHYSICSTRANSFORMS,                       ///< Non Physics Transforms.

    MR_TASKID_HEADLOOKTRANSFORMS,                            ///< HeadLook transforms
    MR_TASKID_HEADLOOKTRAJECTORY_DELTA_AND_TRANSFORMS,       ///< HeadLook trajectory delta and transforms
    MR_TASKID_HEADLOOKSETUP,                                 ///< HeadLook setup data
    MR_TASKID_GUNAIMTRANSFORMS,                              ///< GunAim transforms
    MR_TASKID_GUNAIMTRAJECTORY_DELTA_AND_TRANSFORMS,         ///< GunAim trajectory delta and transforms
    MR_TASKID_GUNAIMSETUP,                                   ///< GunAim setup data
    MR_TASKID_TWOBONEIKTRANSFORMS,                           ///< TwoBoneIK transforms
    MR_TASKID_TWOBONEIKTRAJECTORY_DELTA_AND_TRANSFORMS,      ///< TwoBoneIK trajectory delta and transforms
    MR_TASKID_TWOBONEIKSETUP,                                ///< TwoBoneIK setup data
    MR_TASKID_LOCKFOOTTRANSFORMS,                            ///< LockFoot transforms
    MR_TASKID_HIPSIKTRANSFORMS,                              ///< HipsIK transforms
    MR_TASKID_HIPSIKTRAJECTORY_DELTA_AND_TRANSFORMS,         ///< HipsIK trajectory delta and transforms
    MR_TASKID_RETARGETTRANSFORMS,                            ///< Retarget transforms
    MR_TASKID_RETARGET_TRAJECTORY_DELTA,                     ///< Retarget trajectory delta
    MR_TASKID_RETARGET_TRAJECTORY_DELTA_AND_TRANSFORMS,      ///< Retarget trajectory delta and transforms
    MR_TASKID_LOCKFOOTTRAJECTORY_DELTA_AND_TRANSFORMS,       ///< LockFoot trajectory delta and transforms

    MR_TASKID_MODIFYJOINTTRANSFORM_TRANSFORMS,                           ///< ModifyJointTransform transforms
    MR_TASKID_MODIFYJOINTTRANSFORM_TRAJECTORY_DELTA_AND_TRANSFORMS,      ///< ModifyJointTransform trajectory delta and transforms
    
    MR_TASKID_MODIFYTRAJECTORYTRANSFORM_TRAJECTORY_DELTA,                ///< ModifyTrajectoryTransform trajectory delta
    MR_TASKID_MODIFYTRAJECTORYTRANSFORM_TRAJECTORY_DELTA_AND_TRANSFORMS, ///< ModifyTrajectoryTransform trajectory delta and transforms
    
    MR_TASKID_SCALECHARACTERTRANSFORMS,                      ///< ScaleCharacter transforms
    MR_TASKID_SCALECHARACTERTRAJECTORYDELTA,                 ///< ScaleCharacter trajectory delta
    MR_TASKID_SCALECHARACTERTRAJECTORYDELTA_AND_TRANSFORMS,  ///< ScaleCharacter trajectory delta and transforms
    MR_TASKID_BASIC_UNEVEN_TERRAIN_IK_SETUP,
    MR_TASKID_BASIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET,
    MR_TASKID_BASIC_UNEVEN_TERRAIN_TRANSFORMS,

    MR_TASKID_PREDICTIVE_UNEVEN_TERRAIN_IK_SETUP,
    MR_TASKID_PREDICTIVE_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET,

    MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERS,                        ///< Combine 2 sampled events buffers.
    MR_TASKID_ADD2SAMPLEDEVENTSBUFFERS,
    MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS,
    MR_TASKID_ADD2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS,
    MR_TASKID_ADDSAMPLEDDURATIONEVENTSTOSAMPLEDEVENTBUFFER,
    MR_TASKID_BLEND2DURATIONEVENTTRACKSETS,

    MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTADDPOS,            ///< Blend 2 trajectory delta transforms with a weighting, Additive Attitude and Additive Position.
    MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTINTERPPOS,         ///< Blend 2 trajectory delta transforms with a weighting, Additive Attitude and Interpolate Position.
    MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTSLERPPOS,          ///< Blend 2 trajectory delta transforms with a weighting, Additive Attitude and Slerp Position.
    MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTADDPOS,         ///< Blend 2 trajectory delta transforms with a weighting, Interpolate Attitude and Additive Position.
    MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,      ///< Blend 2 trajectory delta transforms with a weighting, Interpolate Attitude and Interpolate Position.
    MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,       ///< Blend 2 trajectory delta transforms with a weighting, Interpolate Attitude and Slerp Position.
    MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSSUBTRACTATTSUBTRACTPOS,  ///< Blend 2 trajectory delta transforms with a weighting, Subtractive Attitude and Subtractive Position.
    MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSSUBTRACTATTSLERPPOS,     ///< Blend 2 trajectory delta transforms with a weighting, Subtractive Attitude and Slerp Position.

    MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTADDPOSSLERPTRAJ,              ///< Blend 2 transform buffers together, Additive Attitude and Additive Position.
    MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTINTERPPOSSLERPTRAJ,           ///< Blend 2 transform buffers together, Additive Attitude and Interpolate Position.
    MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTADDPOSSLERPTRAJ,           ///< Blend 2 transform buffers together, Interpolate Attitude and Additive Position.
    MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSSLERPTRAJ,        ///< Blend 2 transform buffers together, Interpolate Attitude and Interpolate Position.
    MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSSUBTRACTATTSUBTRACTPOSSLERPTRAJ,    ///< Blend 2 transform buffers together, Subtractive Attitude and Subtractive Position.
    MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTADDPOSINTERPTRAJ,              ///< Blend 2 transform buffers together, Additive Attitude and Additive Position.
    MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTINTERPPOSINTERPTRAJ,           ///< Blend 2 transform buffers together, Additive Attitude and Interpolate Position.
    MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTADDPOSINTERPTRAJ,           ///< Blend 2 transform buffers together, Interpolate Attitude and Additive Position.
    MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSINTERPTRAJ,        ///< Blend 2 transform buffers together, Interpolate Attitude and Interpolate Position.
    MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSSUBTRACTATTSUBTRACTPOSINTERPTRAJ,    ///< Blend 2 transform buffers together, Subtractive Attitude and Subtractive Position.

    MR_TASKID_BLEND2TRANSFORMSADDATTADDPOSPASSDESTTRAJ,       ///< Blend 2 transform buffers together, Additive Attitude and Additive Position, pass through the trajectory delta.
    MR_TASKID_BLEND2TRANSFORMSADDATTINTERPPOSPASSDESTTRAJ,    ///< Blend 2 transform buffers together, Additive Attitude and Interpolate Position, pass through the trajectory delta.
    MR_TASKID_BLEND2TRANSFORMSINTERPATTADDPOSPASSDESTTRAJ,    ///< Blend 2 transform buffers together, Interpolate Attitude and Additive Position, pass through the trajectory delta.
    MR_TASKID_BLEND2TRANSFORMSINTERPATTINTERPPOSPASSDESTTRAJ, ///< Blend 2 transform buffers together, Interpolate Attitude and Interpolate Position, pass through the trajectory delta.

    MR_TASKID_BLEND2TRANSFORMSADDATTADDPOSPASSSOURCETRAJ,       ///< Blend 2 transform buffers together, Additive Attitude and Additive Position, pass through the trajectory delta.
    MR_TASKID_BLEND2TRANSFORMSADDATTINTERPPOSPASSSOURCETRAJ,    ///< Blend 2 transform buffers together, Additive Attitude and Interpolate Position, pass through the trajectory delta.
    MR_TASKID_BLEND2TRANSFORMSINTERPATTADDPOSPASSSOURCETRAJ,    ///< Blend 2 transform buffers together, Interpolate Attitude and Additive Position, pass through the trajectory delta.
    MR_TASKID_BLEND2TRANSFORMSINTERPATTINTERPPOSPASSSOURCETRAJ, ///< Blend 2 transform buffers together, Interpolate Attitude and Interpolate Position, pass through the trajectory delta.

    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSADDATTADDPOS,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSADDATTINTERPPOS,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSADDATTSLERPPOS,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSINTERPATTADDPOS,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,

    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTADDPOSSLERPTRAJ,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTINTERPPOSSLERPTRAJ,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTADDPOSSLERPTRAJ,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTINTERPPOSSLERPTRAJ,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTADDPOSINTERPTRAJ,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTINTERPPOSINTERPTRAJ,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTADDPOSINTERPTRAJ,
    MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTINTERPPOSINTERPTRAJ,

    MR_TASKID_BLEND2SYNCEVENTSUPDATETIMEVIATIMEPOS,      ///< Using a delta time update: time pos and event pos.
    MR_TASKID_BLEND2SYNCEVENTSUPDATETIMEVIASYNCEVENTPOS, ///< Using a delta event update: time pos and event pos.

    MR_TASKID_BLEND2SYNCEVENTTRACKS,                     ///< Blend 2 synchronization event tracks with a weighting.

    MR_TASKID_SCALEUPDATETIME,                           ///< Modify input time with scaling factor.
    MR_TASKID_SCALEUPDATESYNCEVENTTRACK,                 ///< Modify input sync event track with scaling factor.
    MR_TASKID_SETUPDATETIMEVIACONTROLPARAM,              ///< Set the update time value with a fraction based on an input control param value.

    MR_TASKID_FILTERTRANSFORMS,                          ///< Filter out a set of transforms from a transform buffer.
    MR_TASKID_FILTERTRAJECTORYDELTA,                     ///< Filter out the delta trajectory if its channel is flagged as filtered.
    MR_TASKID_FILTERTRAJECTORYDELTAANDTRANSFORMS,        ///< Filter out a set of transforms and the trajectory delta from a transform buffer.
    MR_TASKID_APPLYBINDPOSETRANSFORMS,                   ///< Apply bind pose to missing transforms.
    MR_TASKID_APPLYBINDPOSETRAJECTORYDELTAANDTRANSFORMS, ///< Apply bind pose to missing transforms.
    MR_TASKID_APPLYGLOBALTIME,                           ///< Apply network global time to child.
    MR_TASKID_GETBINDPOSETRANSFORMS,                     ///< Get bind pose transforms.
    MR_TASKID_MIRROR_TRANSFORMS,                         ///< Mirror transforms along the desired axis from a transform
                                                         ///<  buffer using channel mappings when provided
    MR_TASKID_MIRROR_TRAJECTORY_DELTA,                   ///< Mirror trajectory delta transform along the desired axis
    MR_TASKID_MIRROR_TRAJECTORY_DELTA_TRANSFORMS,        ///< Mirror the trajectory delta and transforms along the 
                                                         ///< desired axis from a transform buffer using channel mappings
                                                         ///< when provided
    MR_TASKID_MIRROR_SAMPLED_EVENTS,                     ///< Mirror the sampled events
    MR_TASKID_MIRROR_SAMPLED_AND_DURATION_EVENTS,        ///< Sample duration events and mirror
    MR_TASKID_MIRROR_SYNC_EVENTS_AND_OFFSET,             ///<
    MR_TASKID_MIRROR_DURATION_EVENTS,                    ///< Mirror the duration events
    MR_TASKID_MIRROR_UPDATE_TIME_VIA_SYNC_EVENT_POS,     ///< Applies a sync event offset
    MR_TASKID_MIRROR_UPDATE_TIME_VIA_TIME_POS,           ///<

    MR_TASKID_CLOSESTANIM_TRANSFORMS,
    MR_TASKID_CLOSESTANIM_TRAJECTORYDELTA,
    MR_TASKID_CLOSESTANIM_TRAJECTORY_DELTA_AND_TRANSFORMS,

    MR_TASKID_APPLYPHYSICSJOINTLIMITSTRANSFORMS,

    MR_TASKID_PHYSICSUPDATEANIMATEDTRAJECTORY,
    MR_TASKID_PHYSICSUPDATEPHYSICALTRAJECTORYPOSTPHYSICS,
    MR_TASKID_PHYSICSUPDATETRANSFORMSPREPHYSICS,
    MR_TASKID_PHYSICSUPDATETRANSFORMSPOSTPHYSICS,

    MR_TASKID_PHYSICSGROUPERUPDATETRANSFORMS,
    MR_TASKID_PHYSICSGROUPERUPDATETRANSFORMSPREPHYSICS,
    MR_TASKID_PHYSICSGROUPERUPDATETRAJECTORY,
    MR_TASKID_PHYSICSGROUPERGETOUPUTMASKBASE,
    MR_TASKID_PHYSICSGROUPERGETOUPUTMASKOVERRIDE,

    MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER,
    MR_TASKID_NETWORKUPDATEPHYSICS,
    MR_TASKID_NETWORKUPDATEROOT,
    MR_TASKID_NETWORKMERGEPHYSICSRIGTRANSFORMBUFFERS,

    MR_TASKID_NETWORKDUMMY, ///< Dummy task that shouldn't get executed - placeholder for a real task.
    MR_TASKID_EMPTYTASK,    ///< A dummy task that's dependent on everything that you actually queue.

    MR_TASKID_TRANSITUPDATETIMEPOS,
    MR_TASKID_TRANSITDEADBLENDUPDATETIMEPOS,

    MR_TASKID_TRANSITSYNCEVENTSUPDATETIMEVIASYNCEVENTPOS,
    MR_TASKID_TRANSITSYNCEVENTSUPDATETIMEVIATIMEPOS,
    MR_TASKID_TRANSITSYNCEVENTSBLENDSYNCEVENTTRACKS,
    MR_TASKID_TRANSITSYNCEVENTSPASSTHROUGHSYNCEVENTTRACK,

    MR_TASKID_DEADBLENDCACHESTATE,                            ///< Cache transforms, trajectory and time delta for dead blending
    MR_TASKID_PHYSICALDEADBLENDTRANSFORMBUFFS,                ///< Dead blend on transform buffer, applying rig limits if available
    MR_TASKID_ANIMDEADBLENDTRANSFORMBUFFS,                    ///< Dead blend on transform buffer
    MR_TASKID_ANIMDEADBLENDTRAJECTORYDELTAANDTRANSFORMBUFFS,  ///< Dead blend on trajectory and transform buffer
    MR_TASKID_DEADBLENDTRAJECTORY,                            ///< Dead blend on the trajectory
    MR_TASKID_FEATHERBLENDTOPHYSICSTRANSFORMBUFFS,

    MR_TASKID_OUTPUT_SMOOTH_TRANSFORMS_TRANSFORMS,
    MR_TASKID_OUTPUT_SMOOTH_TRANSFORMS_TRAJECTORY_DELTA_AND_TRANSFORMS,

    MR_TASKID_SCALETODURATION,                     ///< Scale input time to a specific duration
    MR_TASKID_SCALETODURATION_SYNCEVENTTRACK,      ///< Scale input sync event tracks to a specific duration

    MR_TASKID_BEHAVIOURUPDATETRANSFORMSPREPHYSICS,
    MR_TASKID_BEHAVIOURUPDATEPHYSICALTRAJECTORYPOSTPHYSICS,
    MR_TASKID_BEHAVIOURUPDATEANIMATEDTRAJECTORY,
    MR_TASKID_BEHAVIOURUPDATETRANSFORMSPOSTPHYSICS,

    MR_TASKID_PERFORMANCEBEHAVIOURUPDATETRANSFORMSPREPHYSICS,

    MR_TASKID_KINECT_OUTPUT_TRANSFORMS,
    MR_TASKID_KINECT_OUTPUT_TRAJECTORY_DELTAS,
    MR_TASKID_KINECT_OUTPUT_TRAJECTORY_DELTA_TRANSFORMS,
    MR_TASKID_KINECT_OUTPUT_ANIM_SOURCE_SAMPLED_EVENTS,
    MR_TASKID_KINECT_OUTPUT_GESTURE_SAMPLED_EVENTS,
    MR_TASKID_KINECT_OUTPUT_GESTURE_TRANSFORMS,
    MR_TASKID_KINECT_OUTPUT_GESTURE_TRAJECTORY_DELTAS,
    MR_TASKID_KINECT_OUTPUT_GESTURE_TRAJECTORY_DELTA_TRANSFORMS,

    MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE,             ///< Extract joint info in object space.
    MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE,              ///< Extract joint info in local space.
    MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE_JOINTSELECT, ///< Extract joint info in object space. Joint to output is dynamically selected.
    MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE_JOINTSELECT,  ///< Extract joint info in local space. Joint to output is dynamically selected.

    MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE,             ///< Extract joint info in object space.
    MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE,              ///< Extract joint info in local space.
    MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE_JOINTSELECT, ///< Extract joint info in object space. Joint to output is dynamically selected.
    MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE_JOINTSELECT,  ///< Extract joint info in local space. Joint to output is dynamically selected.

    MR_TASKID_SAMPLEDEVENTSBUFFEREMITMESSAGEANDPASSTHROUGH,

    MR_TASKID_TRAJECTORYOVERRIDE,
    MR_TASKID_CHARACTERCONTROLLERSTATE,
    MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_LIMBINFO,
    MR_TASKID_PASSTHROUGHTRANSFORMS_LIMBINFO,

    MR_TASKID_PASSTHROUGHTRANSFORMS_EXPANDLIMITS,
    MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXPANDLIMITS,

    MR_TASKID_MAX
  };
};

#if defined(NM_HOST_CELL_PPU) || defined(NM_HOST_CELL_SPU)

  #if defined(NM_HOST_CELL_SPU)
typedef MR::Dispatcher::TaskFunction* TaskFn;
  #else // defined(NM_HOST_CELL_SPU)
typedef uint32_t TaskFn;
  #endif // defined(NM_HOST_CELL_SPU)

struct TaskFnMap
{
  uint32_t taskID;
  TaskFn taskFn;
};

struct TaskIDMap
{
  void setTaskFn(MR::Dispatcher::TaskFunction* _taskFn) { taskFn = (unsigned int)_taskFn; };
  MR::Dispatcher::TaskFunction* getTaskFn() { return (MR::Dispatcher::TaskFunction*)taskFn; };

  unsigned int taskFn : 24; // Local store address, only 18 bits required
  uint8_t overlayID;
};

typedef void (*TorListFn) (void);

struct TaskFnsOverlay
{
  uint8_t*  codeEA;

  void setBaseAddress(uint32_t baseAddress) { m_baseAddress = static_cast<uint16_t>(baseAddress >> 2); };
  void setEntryPoint(uint32_t entryPoint)   { m_entryPoint  = static_cast<uint16_t>(entryPoint  >> 2); };
  void setSize(uint32_t size)               { m_size        = static_cast<uint16_t>(size        >> 2); };
  void setBssSize(uint32_t bssSize)         { m_bssSize     = static_cast<uint16_t>(bssSize     >> 2); };
  void setTors(uint32_t tors)               { m_tors        = static_cast<uint16_t>(tors        >> 2); };
  void setInit(uint32_t init)               { m_init        = static_cast<uint16_t>(init        >> 2); };

  uint32_t getBaseAddress()                 { return m_baseAddress << 2; };
  uint32_t getEntryPoint()                  { return m_entryPoint  << 2; };
  uint32_t getSize()                        { return m_size        << 2; };
  uint32_t getBssSize()                     { return m_bssSize     << 2; };
  uint32_t getTors()                        { return m_tors        << 2; };
  uint32_t getInit()                        { return m_init        << 2; };

private:
  uint16_t m_baseAddress; // Currently fixed, not using PIC code
  uint16_t m_entryPoint;  // Unused
  uint16_t m_size;
  uint16_t m_bssSize;
  uint16_t m_tors;
  uint16_t m_init;
};

struct SPUTaskFnMap
{
  TaskIDMap*     taskIDMap __attribute__((aligned(CELL_SPURS_EVENT_FLAG_ALIGN)));
  TaskFnsOverlay overlayIDMap[MR::CoreTaskIDs::MR_TASKOVERLAY_MAX_FILES];
  uint32_t       maxOverlaySize;
  uint32_t       maxNumTasks;
};
NM_ASSERT_COMPILE_TIME((sizeof(SPUTaskFnMap) % 16) == 0);

#endif // defined(NM_HOST_CELL_PPU) || defined(NM_HOST_CELL_SPU)

// Semantic reference task ID generation
#if !defined(MR_GETTASKIDFORSEMANTICREFERENCE)
  #define MR_GETTASKIDFORSEMANTICREFERENCE(_AttribSemantic) manager.getCreateReferenceToInputTaskID(_AttribSemantic)
#endif // !defined(MR_GETTASKIDFORSEMANTICREFERENCE)

// Interpret task function registration for SPU or everything else
#if !defined(MR_DISPATCHERREGISTERFN)
  #if defined(NM_HOST_CELL_SPU)
    #define MR_DISPATCHERREGISTERFN(_fnName, _taskID)
  #else // defined(NM_HOST_CELL_SPU)
    #define MR_DISPATCHERREGISTERFN(_fnName, _taskID) dispatcher->registerTask(_fnName, #_fnName, _taskID);
  #endif // defined(NM_HOST_CELL_SPU)
#endif

// Interpret task overlay registration for SPU or everything else
#if !defined(MR_DISPATCHERREGISTER_TASKOVERLAY)
  #if defined(NM_HOST_CELL_PPU)
    #define MR_DISPATCHERREGISTER_TASKOVERLAY(_taskOverlayName, _regSPUElf) \
    if (_regSPUElf)registerSPUTaskFnOverlayElf(_binary_task_mrSPU_##_taskOverlayName##_elf_start, MR_TASKOVERLAY_##_taskOverlayName);
  #else // defined(NM_HOST_CELL_PPU)
    #define MR_DISPATCHERREGISTER_TASKOVERLAY(_taskOverlayName, _regSPUElf)
  #endif // defined(NM_HOST_CELL_PPU)
#endif

// Core Tasks

// ASA anim format.
#if !defined(MR_REGISTER_ANIMSAMPLETASKSASA)
  #define MR_REGISTER_ANIMSAMPLETASKSASA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(ANIMSAMPLETASKSASA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskSampleTransformsFromAnimSourceASA, MR::CoreTaskIDs::MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCEASA)
#endif

#if !defined(MR_REGISTER_TRAJECTORYSAMPLETASKSASA)
  #define MR_REGISTER_TRAJECTORYSAMPLETASKSASA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRAJECTORYSAMPLETASKSASA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskUpdateDeltaTrajectoryFromTrajectorySourceASA, MR::CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCEASA)
#endif

#if !defined(MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSASA)
  #define MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSASA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRAJECTORYANDTRANSFORMSSAMPLETASKSASA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskUpdateDeltaTrajectoryAndTransformsFromSourceASA, MR::CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCEASA)
#endif

// MBA anim format.
#if !defined(MR_REGISTER_ANIMSAMPLETASKSMBA)
  #define MR_REGISTER_ANIMSAMPLETASKSMBA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(ANIMSAMPLETASKSMBA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskSampleTransformsFromAnimSourceMBA, MR::CoreTaskIDs::MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCEMBA)
#endif

#if !defined(MR_REGISTER_TRAJECTORYSAMPLETASKSMBA)
  #define MR_REGISTER_TRAJECTORYSAMPLETASKSMBA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRAJECTORYSAMPLETASKSMBA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskUpdateDeltaTrajectoryFromTrajectorySourceMBA, MR::CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCEMBA)
#endif

#if !defined(MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSMBA)
  #define MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSMBA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRAJECTORYANDTRANSFORMSSAMPLETASKSMBA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskUpdateDeltaTrajectoryAndTransformsFromSourceMBA, MR::CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCEMBA)
#endif

// NSA anim format.
#if !defined(MR_REGISTER_ANIMSAMPLETASKSNSA)
  #define MR_REGISTER_ANIMSAMPLETASKSNSA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(ANIMSAMPLETASKSNSA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskSampleTransformsFromAnimSourceNSA, MR::CoreTaskIDs::MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCENSA)
#endif

#if !defined(MR_REGISTER_TRAJECTORYSAMPLETASKSNSA)
  #define MR_REGISTER_TRAJECTORYSAMPLETASKSNSA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRAJECTORYSAMPLETASKSNSA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskUpdateDeltaTrajectoryFromTrajectorySourceNSA, MR::CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCENSA)
#endif

#if !defined(MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSNSA)
  #define MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSNSA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRAJECTORYANDTRANSFORMSSAMPLETASKSNSA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskUpdateDeltaTrajectoryAndTransformsFromSourceNSA, MR::CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCENSA)
#endif

// QSA anim format.
#if !defined(MR_REGISTER_ANIMSAMPLETASKSQSA)
  #define MR_REGISTER_ANIMSAMPLETASKSQSA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(ANIMSAMPLETASKSQSA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskSampleTransformsFromAnimSourceQSA, MR::CoreTaskIDs::MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCEQSA)
#endif

#if !defined(MR_REGISTER_TRAJECTORYSAMPLETASKSQSA)
  #define MR_REGISTER_TRAJECTORYSAMPLETASKSQSA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRAJECTORYSAMPLETASKSQSA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskUpdateDeltaTrajectoryFromTrajectorySourceQSA, MR::CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCEQSA)
#endif

#if !defined(MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSQSA)
#define MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSQSA(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRAJECTORYANDTRANSFORMSSAMPLETASKSQSA, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskUpdateDeltaTrajectoryAndTransformsFromSourceQSA, MR::CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCEQSA)
#endif

#if !defined(MR_REGISTER_BLENDTRANSFORMTASKS)
  #define MR_REGISTER_BLENDTRANSFORMTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(BLENDTRANSFORMTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformBuffsAddAttAddPos,              MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSADDATTADDPOS             ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformBuffsAddAttInterpPos,           MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSADDATTINTERPPOS          ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformBuffsInterpAttAddPos,           MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSINTERPATTADDPOS          ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformBuffsInterpAttInterpPos,        MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSINTERPATTINTERPPOS       ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformBuffsSubtractAttSubtractPos,    MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSSUBTRACTATTSUBTRACTPOS   ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TransformBuffsAddAttAddPos,       MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSADDATTADDPOS      ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TransformBuffsAddAttInterpPos,    MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSADDATTINTERPPOS   ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TransformBuffsInterpAttAddPos,    MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSINTERPATTADDPOS   ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TransformBuffsInterpAttInterpPos, MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSINTERPATTINTERPPOS) \
  MR_DISPATCHERREGISTERFN(TaskClosestAnimTransforms,                         MR::CoreTaskIDs::MR_TASKID_CLOSESTANIM_TRANSFORMS                       ) \
  MR_DISPATCHERREGISTERFN(TaskClosestAnimTrajectoryDelta,                    MR::CoreTaskIDs::MR_TASKID_CLOSESTANIM_TRAJECTORYDELTA                  ) \
  MR_DISPATCHERREGISTERFN(TaskClosestAnimTrajectoryDeltaAndTransforms,       MR::CoreTaskIDs::MR_TASKID_CLOSESTANIM_TRAJECTORY_DELTA_AND_TRANSFORMS  )
#endif

#if !defined(MR_REGISTER_BLENDTRAJECTORYANDTRANSFORMSTASKS)
  #define MR_REGISTER_BLENDTRAJECTORYANDTRANSFORMSTASKS(_regSPUELF) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(BLENDTRAJECTORYANDTRANSFORMSTASKS, _regSPUELF) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryAndTransformsAddAttAddPosSlerpTraj,           MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTADDPOSSLERPTRAJ           ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryAndTransformsAddAttInterpPosSlerpTraj,        MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTINTERPPOSSLERPTRAJ        ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryAndTransformsInterpAttAddPosSlerpTraj,        MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTADDPOSSLERPTRAJ        ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryAndTransformsInterpAttInterpPosSlerpTraj,     MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSSLERPTRAJ     ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryAndTransformsSubtractAttSubtractPosSlerpTraj, MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSSUBTRACTATTSUBTRACTPOSSLERPTRAJ ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryAndTransformsAddAttAddPosInterpTraj,          MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTADDPOSINTERPTRAJ          ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryAndTransformsAddAttInterpPosInterpTraj,       MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTINTERPPOSINTERPTRAJ       ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryAndTransformsInterpAttAddPosInterpTraj,       MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTADDPOSINTERPTRAJ       ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryAndTransformsInterpAttInterpPosInterpTraj,    MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSINTERPTRAJ    ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryAndTransformsSubtractAttSubtractPosInterpTraj,MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSSUBTRACTATTSUBTRACTPOSINTERPTRAJ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2x2TrajectoryDeltaAndTransformsInterpTraj,               MR::CoreTaskIDs::MR_TASKID_BLEND2X2TRAJECTORYANDTRANSFORMSINTERPTRAJ                    ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2x2TrajectoryDeltaAndTransformsSlerpTraj,                MR::CoreTaskIDs::MR_TASKID_BLEND2X2TRAJECTORYANDTRANSFORMSSLERPTRAJ                     ) \
  MR_DISPATCHERREGISTERFN(TaskTriangleBlendTrajectoryDeltaAndTransformsInterpTraj,          MR::CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_TRAJECTORYANDTRANSFORMSINTERPTRAJ             ) \
  MR_DISPATCHERREGISTERFN(TaskTriangleBlendTrajectoryDeltaAndTransformsSlerpTraj,           MR::CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_TRAJECTORYANDTRANSFORMSSLERPTRAJ              )
#endif

#if !defined(MR_REGISTER_BLENDTRANSFORMSPASSDESTTRAJTRASKS)
#define MR_REGISTER_BLENDTRANSFORMSPASSDESTTRAJTRASKS(_regSPUELF) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(BLENDTRANSFORMSPASSDESTTRAJTRASKS, _regSPUELF) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformsAddAttAddPosPassDestTraj,       MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSADDATTADDPOSPASSDESTTRAJ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformsAddAttInterpPosPassDestTraj,    MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSADDATTINTERPPOSPASSDESTTRAJ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformsInterpAttAddPosPassDestTraj,    MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSINTERPATTADDPOSPASSDESTTRAJ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformsInterpAttInterpPosPassDestTraj, MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSINTERPATTINTERPPOSPASSDESTTRAJ)
#endif

#if !defined(MR_REGISTER_BLENDTRANSFORMSPASSSOURCETRAJTRASKS)
#define MR_REGISTER_BLENDTRANSFORMSPASSSOURCETRAJTRASKS(_regSPUELF) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(BLENDTRANSFORMSPASSSOURCETRAJTRASKS, _regSPUELF) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformsAddAttAddPosPassSourceTraj,       MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSADDATTADDPOSPASSSOURCETRAJ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformsAddAttInterpPosPassSourceTraj,    MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSADDATTINTERPPOSPASSSOURCETRAJ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformsInterpAttAddPosPassSourceTraj,    MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSINTERPATTADDPOSPASSSOURCETRAJ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TransformsInterpAttInterpPosPassSourceTraj, MR::CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSINTERPATTINTERPPOSPASSSOURCETRAJ)
#endif

#if !defined(MR_REGISTER_BLENDALLTASKS)
  #define MR_REGISTER_BLENDALLTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(BLENDALLTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskBlendAllTrajectoryDeltaTransformsInterpAttInterpPos,      MR::CoreTaskIDs::MR_TASKID_BLENDALLTRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS  ) \
  MR_DISPATCHERREGISTERFN(TaskBlendAllTransformBuffsInterpAttInterpPos,                 MR::CoreTaskIDs::MR_TASKID_BLENDALLTRANSFORMBUFFSINTERPATTINTERPPOS             ) \
  MR_DISPATCHERREGISTERFN(TaskBlendAllSyncEventTracks,                                  MR::CoreTaskIDs::MR_TASKID_BLENDALLSYNCEVENTTRACKS                              ) \
  MR_DISPATCHERREGISTERFN(TaskBlendAllDurationEventTrackSets,                           MR::CoreTaskIDs::MR_TASKID_BLENDALLDURATIONEVENTTRACKSETS                       ) \
  MR_DISPATCHERREGISTERFN(TaskCombineAllSampledEventsBuffers,                           MR::CoreTaskIDs::MR_TASKID_COMBINEALLSAMPLEDEVENTSBUFFERS                       ) \
  MR_DISPATCHERREGISTERFN(TaskCombineAllSampledEventsBuffersAndSampleDurationEvents,    MR::CoreTaskIDs::MR_TASKID_COMBINEALLSAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS) \
  MR_DISPATCHERREGISTERFN(TaskBlendAllTrajectoryDeltaTransformsInterpAttSlerpPos,       MR::CoreTaskIDs::MR_TASKID_BLENDALLTRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS   ) \
  MR_DISPATCHERREGISTERFN(TaskBlendAllTrajectoryDeltaAndTransformsInterpTraj,           MR::CoreTaskIDs::MR_TASKID_BLENDALLTRAJECTORYDELTAANDTRANSFORMSINTERPPOS        ) \
  MR_DISPATCHERREGISTERFN(TaskBlendAllTrajectoryDeltaAndTransformsSlerpTraj,            MR::CoreTaskIDs::MR_TASKID_BLENDALLTRAJECTORYDELTAANDTRANSFORMSSLERPPOS         ) 
#endif

#if !defined(MR_REGISTER_COMMONTASKS)
  #define MR_REGISTER_COMMONTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(COMMONTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskAnimSyncEventsUpdateTimePos,                         MR::CoreTaskIDs::MR_TASKID_ANIMSYNCEVENTSUPDATETIMEPOS                             ) \
  MR_DISPATCHERREGISTERFN(TaskAnimSyncEventsUpdateSyncEventPos,                    MR::CoreTaskIDs::MR_TASKID_ANIMSYNCEVENTSUPDATESYNCEVENTPOS                        ) \
  MR_DISPATCHERREGISTERFN(TaskInitSampledEventsBuffer,                             MR::CoreTaskIDs::MR_TASKID_INIT_SAMPLED_EVENTS_TRACK                               ) \
  MR_DISPATCHERREGISTERFN(TaskBufferLastFramesTransformBuffer,                     MR::CoreTaskIDs::MR_TASKID_BUFFER_LAST_FRAME_TRANSFORM_BUFFER                      ) \
  MR_DISPATCHERREGISTERFN(TaskBufferLastFramesTrajectoryDeltaAndTransformBuffer,   MR::CoreTaskIDs::MR_TASKID_BUFFER_LAST_FRAME_TRAJECTORY_AND_TRANSFORMS             ) \
  MR_DISPATCHERREGISTERFN(TaskBufferLastFramesTransformsZeroTrajectory,            MR::CoreTaskIDs::MR_TASKID_BUFFER_LAST_FRAME_TRANSFORMS_ZERO_TRAJECTORY            ) \
  MR_DISPATCHERREGISTERFN(TaskInitSyncEventTrackFromDiscreteEventTrack,            MR::CoreTaskIDs::MR_TASKID_INITSYNCEVENTTRACKFROMDISCRETEEVENTTRACK                ) \
  MR_DISPATCHERREGISTERFN(TaskInitUnitLengthSyncEventTrack,                        MR::CoreTaskIDs::MR_TASKID_INIT_UNIT_LENGTH_SYNC_EVENT_TRACK                       ) \
  MR_DISPATCHERREGISTERFN(TaskSampleEventsFromSourceTracks,                        MR::CoreTaskIDs::MR_TASKID_SAMPLEEVENTSFROMSOURCETRACKS                            ) \
  MR_DISPATCHERREGISTERFN(TaskCombine2SampledEventsBuffers,                        MR::CoreTaskIDs::MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERS                            ) \
  MR_DISPATCHERREGISTERFN(TaskAdd2SampledEventsBuffers,                            MR::CoreTaskIDs::MR_TASKID_ADD2SAMPLEDEVENTSBUFFERS                                ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2SyncEventsUpdateTimeViaTimePos,                MR::CoreTaskIDs::MR_TASKID_BLEND2SYNCEVENTSUPDATETIMEVIATIMEPOS                    ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2SyncEventsUpdateTimeViaSyncEventPos,           MR::CoreTaskIDs::MR_TASKID_BLEND2SYNCEVENTSUPDATETIMEVIASYNCEVENTPOS               ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2SyncEventTracks,                               MR::CoreTaskIDs::MR_TASKID_BLEND2SYNCEVENTTRACKS                                   ) \
  MR_DISPATCHERREGISTERFN(TaskFilterTransforms,                                    MR::CoreTaskIDs::MR_TASKID_FILTERTRANSFORMS                                        ) \
  MR_DISPATCHERREGISTERFN(TaskFilterTrajectoryDelta,                               MR::CoreTaskIDs::MR_TASKID_FILTERTRAJECTORYDELTA                                   ) \
  MR_DISPATCHERREGISTERFN(TaskFilterTrajectoryDeltaAndTransforms,                  MR::CoreTaskIDs::MR_TASKID_FILTERTRAJECTORYDELTAANDTRANSFORMS                      ) \
  MR_DISPATCHERREGISTERFN(TaskApplyBindPoseTransforms,                             MR::CoreTaskIDs::MR_TASKID_APPLYBINDPOSETRANSFORMS                                 ) \
  MR_DISPATCHERREGISTERFN(TaskApplyBindPoseTrajectoryDeltaAndTransforms,           MR::CoreTaskIDs::MR_TASKID_APPLYBINDPOSETRAJECTORYDELTAANDTRANSFORMS               ) \
  MR_DISPATCHERREGISTERFN(TaskApplyGlobalTimeUpdateTimePos,                        MR::CoreTaskIDs::MR_TASKID_APPLYGLOBALTIME                                         ) \
  MR_DISPATCHERREGISTERFN(TaskGetBindPoseTransforms,                               MR::CoreTaskIDs::MR_TASKID_GETBINDPOSETRANSFORMS                                   ) \
  MR_DISPATCHERREGISTERFN(TaskSetUpdateTimeViaControlParam,                        MR::CoreTaskIDs::MR_TASKID_SETUPDATETIMEVIACONTROLPARAM                            ) \
  MR_DISPATCHERREGISTERFN(TaskNetworkUpdateCharacterController,                    MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER                        ) \
  MR_DISPATCHERREGISTERFN(TaskNetworkUpdatePhysics,                                MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS                                    ) \
  MR_DISPATCHERREGISTERFN(TaskNetworkUpdateRoot,                                   MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT                                       ) \
  MR_DISPATCHERREGISTERFN(TaskNetworkMergePhysicsRigTransformBuffers,              MR::CoreTaskIDs::MR_TASKID_NETWORKMERGEPHYSICSRIGTRANSFORMBUFFERS                  ) \
  MR_DISPATCHERREGISTERFN(TaskNetworkDummyTask,                                    MR::CoreTaskIDs::MR_TASKID_NETWORKDUMMY                                            ) \
  MR_DISPATCHERREGISTERFN(TaskEmptyTask,                                           MR::CoreTaskIDs::MR_TASKID_EMPTYTASK                                               ) \
  MR_DISPATCHERREGISTERFN(TaskOutputSmoothTransformsTransforms,                    MR::CoreTaskIDs::MR_TASKID_OUTPUT_SMOOTH_TRANSFORMS_TRANSFORMS                     ) \
  MR_DISPATCHERREGISTERFN(TaskOutputSmoothTransformsTrajectoryDeltaAndTransforms,  MR::CoreTaskIDs::MR_TASKID_OUTPUT_SMOOTH_TRANSFORMS_TRAJECTORY_DELTA_AND_TRANSFORMS)
#endif

#if !defined(MR_REGISTER_BLEND2x2TASKS)
  #define MR_REGISTER_BLEND2x2TASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(BLEND2x2TASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskBlend2x2TransformBuffsInterpAttInterpPos,               MR::CoreTaskIDs::MR_TASKID_BLEND2X2TRANSFORMBUFFSINTERPATTINTERPPOS             ) \
  MR_DISPATCHERREGISTERFN(TaskCombine2x2SampledEventsBuffers,                         MR::CoreTaskIDs::MR_TASKID_COMBINE2X2SAMPLEDEVENTSBUFFERS                       ) \
  MR_DISPATCHERREGISTERFN(TaskCombine2x2SampledEventsBuffersAndSampleDurationEvents,  MR::CoreTaskIDs::MR_TASKID_COMBINE2X2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS) \
  MR_DISPATCHERREGISTERFN(TaskBlend2x2SyncEventTracks,                                MR::CoreTaskIDs::MR_TASKID_BLEND2X2SYNCEVENTTRACKS                              )
#endif

#if !defined(MR_REGISTER_TRIANGLEBLENDTASKS)
#define MR_REGISTER_TRIANGLEBLENDTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRIANGLEBLENDTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskTriangleBlendTransformBuffsInterpAttInterpPos,               MR::CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_TRANSFORMBUFFSINTERPATTINTERPPOS             ) \
  MR_DISPATCHERREGISTERFN(TaskTriangleCombineSampledEventsBuffers,                         MR::CoreTaskIDs::MR_TASKID_TRIANGLE_COMBINE_SAMPLEDEVENTSBUFFERS                       ) \
  MR_DISPATCHERREGISTERFN(TaskTriangleCombineSampledEventsBuffersAndSampleDurationEvents,  MR::CoreTaskIDs::MR_TASKID_TRIANGLE_COMBINE_SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS) \
  MR_DISPATCHERREGISTERFN(TaskTriangleBlendSyncEventTracks,                                MR::CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_SYNCEVENTTRACKS                              )
#endif

#if !defined(MR_REGISTER_SCALETASKS)
  #define MR_REGISTER_SCALETASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(SCALETASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskScaleUpdateTime,               MR::CoreTaskIDs::MR_TASKID_SCALEUPDATETIME                 ) \
  MR_DISPATCHERREGISTERFN(TaskScaleUpdateSyncEventTrack,     MR::CoreTaskIDs::MR_TASKID_SCALEUPDATESYNCEVENTTRACK       ) \
  MR_DISPATCHERREGISTERFN(TaskScaleToDuration,               MR::CoreTaskIDs::MR_TASKID_SCALETODURATION                 ) \
  MR_DISPATCHERREGISTERFN(TaskScaleToDurationSyncEventTrack, MR::CoreTaskIDs::MR_TASKID_SCALETODURATION_SYNCEVENTTRACK  )
#endif

#if !defined(MR_REGISTER_EXTRACTJOINTINFOTASKS)
  #define MR_REGISTER_EXTRACTJOINTINFOTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(EXTRACTJOINTINFOTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskPassThroughTransformsExtractJointInfoObjectSpace,            MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE   ) \
  MR_DISPATCHERREGISTERFN(TaskPassThroughTransformsExtractJointInfoLocalSpace,             MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE    ) \
  MR_DISPATCHERREGISTERFN(TaskPassThroughTransformsExtractJointInfoObjectSpaceJointSelect, MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE_JOINTSELECT   ) \
  MR_DISPATCHERREGISTERFN(TaskPassThroughTransformsExtractJointInfoLocalSpaceJointSelect,  MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE_JOINTSELECT    )
#endif

#if !defined(MR_REGISTER_EXTRACTJOINTINFOTRAJECTORYANDTRANSFORMSTASKS)
  #define MR_REGISTER_EXTRACTJOINTINFOTRAJECTORYANDTRANSFORMSTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(EXTRACTJOINTINFOTRAJECTORYANDTRANSFORMSTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoObjectSpace,            MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE   ) \
  MR_DISPATCHERREGISTERFN(TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoLocalSpace,             MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE    ) \
  MR_DISPATCHERREGISTERFN(TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoObjectSpaceJointSelect, MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE_JOINTSELECT   ) \
  MR_DISPATCHERREGISTERFN(TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoLocalSpaceJointSelect,  MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE_JOINTSELECT    )
#endif

#if !defined(MR_REGISTER_MIRRORTASKS)
  #define MR_REGISTER_MIRRORTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(MIRRORTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskMirrorTransforms,                      MR::CoreTaskIDs::MR_TASKID_MIRROR_TRANSFORMS                      ) \
  MR_DISPATCHERREGISTERFN(TaskMirrorTrajectoryDelta,                 MR::CoreTaskIDs::MR_TASKID_MIRROR_TRAJECTORY_DELTA                ) \
  MR_DISPATCHERREGISTERFN(TaskMirrorTrajectoryDeltaAndTransforms,    MR::CoreTaskIDs::MR_TASKID_MIRROR_TRAJECTORY_DELTA_TRANSFORMS     ) \
  MR_DISPATCHERREGISTERFN(TaskMirrorSampledEvents,                   MR::CoreTaskIDs::MR_TASKID_MIRROR_SAMPLED_EVENTS                  ) \
  MR_DISPATCHERREGISTERFN(TaskMirrorSyncEventsAndOffset,             MR::CoreTaskIDs::MR_TASKID_MIRROR_SYNC_EVENTS_AND_OFFSET          ) \
  MR_DISPATCHERREGISTERFN(TaskMirrorUpdateTimeViaSyncEventPos,       MR::CoreTaskIDs::MR_TASKID_MIRROR_UPDATE_TIME_VIA_SYNC_EVENT_POS  ) \
  MR_DISPATCHERREGISTERFN(TaskMirrorUpdateTimeViaTimePos,            MR::CoreTaskIDs::MR_TASKID_MIRROR_UPDATE_TIME_VIA_TIME_POS        ) \
  MR_DISPATCHERREGISTERFN(TaskMirrorSampledAndSampleDurationEvents,  MR::CoreTaskIDs::MR_TASKID_MIRROR_SAMPLED_AND_DURATION_EVENTS     ) \
  MR_DISPATCHERREGISTERFN(TaskMirrorDurationEvents,                  MR::CoreTaskIDs::MR_TASKID_MIRROR_DURATION_EVENTS                 )
#endif

#if !defined(MR_REGISTER_EVENTTRACKDURATIONTASKS)
  #define MR_REGISTER_EVENTTRACKDURATIONTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(EVENTTRACKDURATIONTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskSampleEventsFromSourceTracksIncDurationEvents,         MR::CoreTaskIDs::MR_TASKID_SAMPLEEVENTSFROMSOURCETRACKSINCDURATIONEVENTS      ) \
  MR_DISPATCHERREGISTERFN(TaskInitEventTrackDurationSetFromSource,                   MR::CoreTaskIDs::MR_TASKID_INITEVENTTRACKDURATIONSETFROMSOURCE                ) \
  MR_DISPATCHERREGISTERFN(TaskInitEmptyEventTrackDurationSet,                        MR::CoreTaskIDs::MR_TASKID_INIT_EMPTY_EVENTTRACKDURATIONSET                   ) \
  MR_DISPATCHERREGISTERFN(TaskCombine2SampledEventsBuffersAndSampleDurationEvents,   MR::CoreTaskIDs::MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS) \
  MR_DISPATCHERREGISTERFN(TaskAdd2SampledEventsBuffersAndSampleDurationEvents,       MR::CoreTaskIDs::MR_TASKID_ADD2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS    ) \
  MR_DISPATCHERREGISTERFN(TaskAddSampledDurationEventsToSampleEventBuffer,           MR::CoreTaskIDs::MR_TASKID_ADDSAMPLEDDURATIONEVENTSTOSAMPLEDEVENTBUFFER       ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2DurationEventTrackSets,                          MR::CoreTaskIDs::MR_TASKID_BLEND2DURATIONEVENTTRACKSETS                       ) \
  MR_DISPATCHERREGISTERFN(TaskTriangleBlendDurationEventTrackSets,                   MR::CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_DURATIONEVENTTRACKSETS              ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2x2DurationEventTrackSets,                        MR::CoreTaskIDs::MR_TASKID_BLEND2X2DURATIONEVENTTRACKSETS                     ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeDurationEventTrackSet, MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET)    )
#endif

#if !defined(MR_REGISTER_FOOTLOCKIKTASKS)
  #define MR_REGISTER_FOOTLOCKIKTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(FOOTLOCKIKTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskLockFootTransforms, MR::CoreTaskIDs::MR_TASKID_LOCKFOOTTRANSFORMS)
#endif

#if !defined(MR_REGISTER_FOOTLOCKTRAJECTORYANDTRANSFORMIKTASKS)
  #define MR_REGISTER_FOOTLOCKTRAJECTORYANDTRANSFORMIKTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(FOOTLOCKTRAJECTORYANDTRANSFORMIKTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskLockFootTrajectoryDeltaAndTransforms, MR::CoreTaskIDs::MR_TASKID_LOCKFOOTTRAJECTORY_DELTA_AND_TRANSFORMS)
#endif

#if !defined(MR_REGISTER_GUNAIMIKTASKS)
  #define MR_REGISTER_GUNAIMIKTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(GUNAIMIKTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskGunAimTransforms, MR::CoreTaskIDs::MR_TASKID_GUNAIMTRANSFORMS) \
  MR_DISPATCHERREGISTERFN(TaskGunAimSetup, MR::CoreTaskIDs::MR_TASKID_GUNAIMSETUP)
#endif

#if !defined(MR_REGISTER_GUNAIMTRAJECTORYANDTRANSFORMSIKTASKS)
  #define MR_REGISTER_GUNAIMTRAJECTORYANDTRANSFORMSIKTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(GUNAIMTRAJECTORYANDTRANSFORMSIKTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskGunAimTrajectoryDeltaAndTransforms, MR::CoreTaskIDs::MR_TASKID_GUNAIMTRAJECTORY_DELTA_AND_TRANSFORMS)
#endif

#if !defined(MR_REGISTER_HEADLOOKIKTASKS)
  #define MR_REGISTER_HEADLOOKIKTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(HEADLOOKIKTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskHeadLookTransforms, MR::CoreTaskIDs::MR_TASKID_HEADLOOKTRANSFORMS) \
  MR_DISPATCHERREGISTERFN(TaskHeadLookSetup, MR::CoreTaskIDs::MR_TASKID_HEADLOOKSETUP)
#endif

#if !defined(MR_REGISTER_HEADLOOKTRAJECTORYANDTRANSFORMSIKTASKS)
  #define MR_REGISTER_HEADLOOKTRAJECTORYANDTRANSFORMSIKTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(HEADLOOKTRAJECTORYANDTRANSFORMSIKTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskHeadLookTrajectoryDeltaAndTransforms, MR::CoreTaskIDs::MR_TASKID_HEADLOOKTRAJECTORY_DELTA_AND_TRANSFORMS)
#endif

#if !defined(MR_REGISTER_RETARGETTASKS)
  #define MR_REGISTER_RETARGETTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(RETARGETTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskRetargetTransforms, MR::CoreTaskIDs::MR_TASKID_RETARGETTRANSFORMS) \
  MR_DISPATCHERREGISTERFN(TaskRetargetTrajectoryDeltaTransform, MR::CoreTaskIDs::MR_TASKID_RETARGET_TRAJECTORY_DELTA) \
  MR_DISPATCHERREGISTERFN(TaskRetargetTrajectoryDeltaAndTransforms, MR::CoreTaskIDs::MR_TASKID_RETARGET_TRAJECTORY_DELTA_AND_TRANSFORMS) \
  MR_DISPATCHERREGISTERFN(TaskScaleCharacterTransforms, MR::CoreTaskIDs::MR_TASKID_SCALECHARACTERTRANSFORMS) \
  MR_DISPATCHERREGISTERFN(TaskScaleCharacterDeltaTransform, MR::CoreTaskIDs::MR_TASKID_SCALECHARACTERTRAJECTORYDELTA) \
  MR_DISPATCHERREGISTERFN(TaskScaleCharacterDeltaAndTransforms, MR::CoreTaskIDs::MR_TASKID_SCALECHARACTERTRAJECTORYDELTA_AND_TRANSFORMS)
#endif

#if !defined(MR_REGISTER_TRAJECTORYBLENDTASKS)
  #define MR_REGISTER_TRAJECTORYBLENDTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRAJECTORYBLENDTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryDeltaTransformsAddAttAddPos,              MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTADDPOS              ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryDeltaTransformsAddAttInterpPos,           MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTINTERPPOS           ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryDeltaTransformsAddAttSlerpPos,            MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTSLERPPOS            ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryDeltaTransformsInterpAttAddPos,           MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTADDPOS           ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryDeltaTransformsInterpAttInterpPos,        MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS        ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryDeltaTransformsInterpAttSlerpPos,         MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS         ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryDeltaTransformsSubtractAttSubtractPos,    MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSSUBTRACTATTSUBTRACTPOS    ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2TrajectoryDeltaTransformsSubtractAttSlerpPos,       MR::CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSSUBTRACTATTSLERPPOS       ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaTransformsAddAttAddPos,       MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSADDATTADDPOS       ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaTransformsAddAttInterpPos,    MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSADDATTINTERPPOS    ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaTransformsAddAttSlerpPos,     MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSADDATTSLERPPOS     ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaTransformsInterpAttAddPos,    MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSINTERPATTADDPOS    ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaTransformsInterpAttInterpPos, MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaTransformsInterpAttSlerpPos,  MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS  ) \
  MR_DISPATCHERREGISTERFN(TaskTriangleBlendTrajectoryDeltaTransformsInterpAttInterpPos, MR::CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS ) \
  MR_DISPATCHERREGISTERFN(TaskTriangleBlendTrajectoryDeltaTransformsInterpAttSlerpPos,  MR::CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS  ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2x2TrajectoryDeltaTransformsInterpAttInterpPos,      MR::CoreTaskIDs::MR_TASKID_BLEND2X2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS      ) \
  MR_DISPATCHERREGISTERFN(TaskBlend2x2TrajectoryDeltaTransformsInterpAttSlerpPos,       MR::CoreTaskIDs::MR_TASKID_BLEND2X2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS       )
#endif

#if !defined(MR_REGISTER_FEATHERTRAJECTORYANDTRANSFORMBLENDTASKS)
  #define MR_REGISTER_FEATHERTRAJECTORYANDTRANSFORMBLENDTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(FEATHERTRAJECTORYANDTRANSFORMBLENDTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj,        MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTADDPOSSLERPTRAJ        ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj,     MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTINTERPPOSSLERPTRAJ     ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj,     MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTADDPOSSLERPTRAJ     ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj,  MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTINTERPPOSSLERPTRAJ  ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttAddPosInterpTraj,       MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTADDPOSINTERPTRAJ       ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj,    MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTINTERPPOSINTERPTRAJ    ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj,    MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTADDPOSINTERPTRAJ    ) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj, MR::CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTINTERPPOSINTERPTRAJ )
#endif

#if !defined(MR_REGISTER_TRANSITTASKS)
  #define MR_REGISTER_TRANSITTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TRANSITTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskTransitSyncEventsUpdateTimeViaSyncEventPos,     MR::CoreTaskIDs::MR_TASKID_TRANSITSYNCEVENTSUPDATETIMEVIASYNCEVENTPOS) \
  MR_DISPATCHERREGISTERFN(TaskTransitSyncEventsUpdateTimeViaTimePos,          MR::CoreTaskIDs::MR_TASKID_TRANSITSYNCEVENTSUPDATETIMEVIATIMEPOS     ) \
  MR_DISPATCHERREGISTERFN(TaskTransitSyncEventsBlendSyncEventTracks,          MR::CoreTaskIDs::MR_TASKID_TRANSITSYNCEVENTSBLENDSYNCEVENTTRACKS     ) \
  MR_DISPATCHERREGISTERFN(TaskTransitSyncEventsPassThroughSyncEventTrack,     MR::CoreTaskIDs::MR_TASKID_TRANSITSYNCEVENTSPASSTHROUGHSYNCEVENTTRACK) \
  MR_DISPATCHERREGISTERFN(TaskDeadBlendTrajectory,                            MR::CoreTaskIDs::MR_TASKID_DEADBLENDTRAJECTORY                       ) \
  MR_DISPATCHERREGISTERFN(TaskAnimDeadBlendTransformBuffs,                    MR::CoreTaskIDs::MR_TASKID_ANIMDEADBLENDTRANSFORMBUFFS               ) \
  MR_DISPATCHERREGISTERFN(TaskAnimDeadBlendTrajectoryDeltaAndTransformBuffs,  MR::CoreTaskIDs::MR_TASKID_ANIMDEADBLENDTRAJECTORYDELTAANDTRANSFORMBUFFS               ) \
  MR_DISPATCHERREGISTERFN(TaskTransitUpdateTimePos,                           MR::CoreTaskIDs::MR_TASKID_TRANSITUPDATETIMEPOS                      ) \
  MR_DISPATCHERREGISTERFN(TaskTransitDeadBlendUpdateTimePos,                  MR::CoreTaskIDs::MR_TASKID_TRANSITDEADBLENDUPDATETIMEPOS             )
#endif

#if !defined(MR_REGISTER_TWOBONEIKTASKS)
  #define MR_REGISTER_TWOBONEIKTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(TWOBONEIKTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskTwoBoneIKTransforms, MR::CoreTaskIDs::MR_TASKID_TWOBONEIKTRANSFORMS) \
  MR_DISPATCHERREGISTERFN(TaskTwoBoneIKTrajectoryDeltaAndTransforms, MR::CoreTaskIDs::MR_TASKID_TWOBONEIKTRAJECTORY_DELTA_AND_TRANSFORMS) \
  MR_DISPATCHERREGISTERFN(TaskTwoBoneIKSetup, MR::CoreTaskIDs::MR_TASKID_TWOBONEIKSETUP)
#endif

#if !defined(MR_REGISTER_HIPSIKTASKS)
  #define MR_REGISTER_HIPSIKTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(HIPSIKTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskHipsIKTransforms, MR::CoreTaskIDs::MR_TASKID_HIPSIKTRANSFORMS) \
  MR_DISPATCHERREGISTERFN(TaskHipsIKTrajectoryDeltaAndTransforms, MR::CoreTaskIDs::MR_TASKID_HIPSIKTRAJECTORY_DELTA_AND_TRANSFORMS)
#endif

#if !defined(MR_REGISTER_MODIFYTRANSFORMTASKS)
  #define MR_REGISTER_MODIFYTRANSFORMTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(MODIFYTRANSFORMTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskModifyJointTransformTransforms, MR::CoreTaskIDs::MR_TASKID_MODIFYJOINTTRANSFORM_TRANSFORMS) \
  MR_DISPATCHERREGISTERFN(TaskModifyJointTransformTrajectoryDeltaAndTransforms, MR::CoreTaskIDs::MR_TASKID_MODIFYJOINTTRANSFORM_TRAJECTORY_DELTA_AND_TRANSFORMS) \
  MR_DISPATCHERREGISTERFN(TaskModifyTrajectoryTransformTrajectoryDelta, MR::CoreTaskIDs::MR_TASKID_MODIFYTRAJECTORYTRANSFORM_TRAJECTORY_DELTA) \
  MR_DISPATCHERREGISTERFN(TaskModifyTrajectoryTransformTrajectoryDeltaAndTransforms, MR::CoreTaskIDs::MR_TASKID_MODIFYTRAJECTORYTRANSFORM_TRAJECTORY_DELTA_AND_TRANSFORMS)
#endif

#if !defined(MR_REGISTER_BASICUNEVENTERRAINIKTRANSFORMTASKS)
  #define MR_REGISTER_BASICUNEVENTERRAINIKTRANSFORMTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(BASICUNEVENTERRAINIKTRANSFORMTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskBasicUnevenTerrainTransforms, MR::CoreTaskIDs::MR_TASKID_BASIC_UNEVEN_TERRAIN_TRANSFORMS)
#endif

#if !defined(MR_REGISTER_BASICUNEVENTERRAINIKSETUPTASKS)
  #define MR_REGISTER_BASICUNEVENTERRAINIKSETUPTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(BASICUNEVENTERRAINIKSETUPTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskBasicUnevenTerrainIKSetup, MR::CoreTaskIDs::MR_TASKID_BASIC_UNEVEN_TERRAIN_IK_SETUP)
#endif

#if !defined(MR_REGISTER_PREDICTIVEUNEVENTERRAINIKSETUPTASKS)
  #define MR_REGISTER_PREDICTIVEUNEVENTERRAINIKSETUPTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(PREDICTIVEUNEVENTERRAINIKSETUPTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskPredictiveUnevenTerrainIKSetup, MR::CoreTaskIDs::MR_TASKID_PREDICTIVE_UNEVEN_TERRAIN_IK_SETUP)
#endif

#if !defined(MR_REGISTER_REFERENCETASKS)
  #define MR_REGISTER_REFERENCETASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(REFERENCETASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeUIntArray,                  MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_BONE_IDS)                         ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeFloat,                      MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_CP_FLOAT)                         ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeVector3,                    MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_CP_VECTOR3)                       ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeVector4,                    MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_CP_VECTOR4)                       ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeBoolArray,                  MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_OUTPUT_MASK)                      ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeSampledEvents,              MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER)            ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeSyncEventTrack,             MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_SYNC_EVENT_TRACK)                 ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypePlaybackPos,                MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_TIME_POS)                         ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypePlaybackPos,                MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_FRACTION_POS)                     ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypePlaybackPos,                MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_ANIM_SAMPLE_POS)                  ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeTrajectoryDeltaTransform,   MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM)       ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeTransform,                  MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM)             ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeTransformBuffer,            MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_TRANSFORM_BUFFER)                 ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeTransformBuffer,            MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER)) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeUpdateSyncEventPlaybackPos, MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS)            ) \
  MR_DISPATCHERREGISTERFN(TaskCreateReferenceToInputAttribTypeUpdatePlaybackPos,          MR_GETTASKIDFORSEMANTICREFERENCE(ATTRIB_SEMANTIC_UPDATE_TIME_POS)                  ) \
  MR_DISPATCHERREGISTERFN(TaskSampledEventsBufferEmitMessageAndPassThrough,               MR::CoreTaskIDs::MR_TASKID_SAMPLEDEVENTSBUFFEREMITMESSAGEANDPASSTHROUGH            )
#endif

// Physics Tasks
#if !defined(MR_REGISTER_PHYSICS_TRANSITTASKS)
  #define MR_REGISTER_PHYSICS_TRANSITTASKS(_regSPUElf) \
  MR_DISPATCHERREGISTER_TASKOVERLAY(PHYSICS_TRANSITTASKS, _regSPUElf) \
  MR_DISPATCHERREGISTERFN(TaskFeatherBlendToPhysicsTransformBuffs, MR::CoreTaskIDs::MR_TASKID_FEATHERBLENDTOPHYSICSTRANSFORMBUFFS)
#endif

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif //MR_CORE_TASK_IDS_H
//----------------------------------------------------------------------------------------------------------------------
