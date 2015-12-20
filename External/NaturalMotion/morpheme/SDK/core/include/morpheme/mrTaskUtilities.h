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
#ifndef MR_TASK_UTILITIES
#define MR_TASK_UTILITIES
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"
#include "morpheme/mrDispatcher.h"
#include "morpheme/mrMirroredAnimMapping.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class AnimRigDef;

//----------------------------------------------------------------------------------------------------------------------
/// \brief Given last frame sync event position and a delta time calculate new fraction, time and event positions.
void calcCurrentPlaybackValuesFromParentDeltaTimeAdjSpace(
  EventTrackSync&                       syncEventTrackIn,
  AttribDataUpdatePlaybackPos*          attrDeltaTimePosIn,         ///< Delta time in my adjusted space.
  AttribDataUpdateSyncEventPlaybackPos* attrPreviousSyncEventPosIn, ///< Adjusted and real values.
                                                                    ///<   Must already have been scaled to fit
                                                                    ///<   current sync event track.
  bool                                  loopableIn,
  AttribDataPlaybackPos*                attrFractionOut,            ///< Adjusted and real values.
                                                                    ///< Previous values must already be set.
  AttribDataPlaybackPos*                attrTimeOut,                ///< Adjusted and real values.
  AttribDataUpdateSyncEventPlaybackPos* attrCurrentSyncEventPosOut, ///< Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut = NULL ///< Real space.
);

/// \brief Take parents abs time value and update our previous values.
void calcPreviousPlaybackValuesFromParentAbsTimeAdjSpace(
  EventTrackSync&                       syncEventTrackIn,
  AttribDataUpdatePlaybackPos*          attrAbsTimePosIn,           ///< Abs time in our adjusted space.
  bool                                  loopableIn,
  AttribDataPlaybackPos*                attrFractionOut,            ///< Adjusted and real values.
  AttribDataPlaybackPos*                attrTimeOut,                ///< Adjusted and real values.
  AttribDataUpdateSyncEventPlaybackPos* attrSyncEventPosOut,        ///< Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut = NULL ///< Real space.
);

/// \brief Take parents abs time value and update our current values.
void calcCurrentPlaybackValuesFromParentAbsTimeAdjSpace(
  EventTrackSync&                       syncEventTrackIn,
  AttribDataUpdatePlaybackPos*          attrAbsTimePosIn,           ///< Abs time in our adjusted space.
  bool                                  loopableIn,
  AttribDataPlaybackPos*                attrFractionOut,            ///< Adjusted and real values.
  AttribDataPlaybackPos*                attrTimeOut,                ///< Adjusted and real values.
  AttribDataUpdateSyncEventPlaybackPos* attrSyncEventPosOut,        ///< Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut = NULL ///< Real space.
);

/// \brief Take parents abs sync event value and update our current fraction, time and event positions.
void calcCurrentPlaybackValuesFromParentAbsSyncEventPos(
  EventTrackSync&                       mySyncEventTrackIn,
  bool                                  loopableIn,
  AttribDataUpdateSyncEventPlaybackPos* attrParentSyncEventPosIn,   ///< Adj value in parent adjusted space.
                                                                    ///< Real value in parent real space and our adjusted space.
  AttribDataUpdateSyncEventPlaybackPos* attrMySyncEventPosOut,      ///< Adjusted and real values.
                                                                    ///<   Capped or looped appropriately.
  AttribDataPlaybackPos*                attrMyFractionOut,          ///< Adjusted and real values.
  AttribDataPlaybackPos*                attrMyTimeOut,              ///< Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut = NULL ///< Real space.
);

/// \brief Take our previous abs sync event value and update our previous fraction, time and event positions.
void calcPreviousPlaybackValuesFromMyAbsSyncEventPos(
  EventTrackSync&                       mySyncEventTrackIn,
  bool                                  loopableIn,
  AttribDataUpdateSyncEventPlaybackPos* attrMySyncEventPosIn,  ///< Adjusted value required.
  AttribDataUpdateSyncEventPlaybackPos* attrMySyncEventPosOut, ///< Adjusted and real values, Capped or looped appropriately.
                                                               ///< Can be the same as attrSyncEventPosIn.
  AttribDataPlaybackPos*                attrMyFractionOut,     ///< Adjusted and real values.
  AttribDataPlaybackPos*                attrMyTimeOut          ///< Adjusted and real values.
);

/// \brief Given our last frame sync event position and a parent delta sync event position calculate our new sync event,
///        time and fraction positions.
void calcCurrentPlaybackValuesFromParentDeltaSyncEventPosAdjSpace(
  EventTrackSync&                       mySyncEventTrackIn,
  AttribDataUpdateSyncEventPlaybackPos* attrParentDeltaSyncEventPosIn, ///< Delta sync event position.
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn,  ///< Adjusted and real values.
                                                                       ///<   Must already have been scaled to fit
                                                                       ///<   current sync event track.
  bool                                  loopableIn,
  AttribDataPlaybackPos*                attrMyFractionOut,             ///< Adjusted and real values.
                                                                       ///< Previous values must already be set.
  AttribDataPlaybackPos*                attrMyTimeOut,                 ///< Adjusted and real values.
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut,  ///< Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut = NULL    ///< Real space.
);

void subTaskMirrorSampledEvents(
  AttribDataSampledEvents*       sourceSyncEventTrack,
  AttribDataSampledEvents*       sampledEvents,
  AttribDataMirroredAnimMapping* mirroredAnimMapping);

/// \brief calculate the character space transforms for the passed rig
void calculateCharSpaceTransforms(
  MR::AnimRigDef* rig,                                                    // IN
  const NMP::DataBuffer* localTransf,                                     // IN
  NMP::DataBuffer* charSpaceTransf                                        // OUT
  );

/// \brief For debugging of tasks primarily on SPU.
///
/// Displays for each task parameter:
///  + Attrib pointer location.
///  + Size.
///  + Alignment.
void displayTaskParameterAttribInfo(
  Dispatcher::TaskParameters* parameters,
  uint32_t                    logPriority);

#ifdef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
/// \brief For debugging of tasks on SPU.
///
/// Displays for each task parameter:
///  + Attrib pointer location.
///  + Size.
///  + Alignment.
bool displayTaskParameterAttribInfoSPU(MR::Task* task);
#endif

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TASK_UTILITIES
//----------------------------------------------------------------------------------------------------------------------

