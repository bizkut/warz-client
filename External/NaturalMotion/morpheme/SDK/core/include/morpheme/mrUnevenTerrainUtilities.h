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
#ifndef MR_UNEVEN_TERRAIN_UTILITIES_H
#define MR_UNEVEN_TERRAIN_UTILITIES_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"
#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrUnevenTerrainIK.h"
//----------------------------------------------------------------------------------------------------------------------

#define TEST_UT_NODE_LOGGINGx
#define TEST_UT_TASK_LOGGINGx
#define TEST_UT_TASK_DEBUG_DRAWx

// To enable profiling output
#define NM_UT_PROFILINGx
#if defined(NM_UT_PROFILING)
  #define NM_UT_BEGIN_PROFILING(name)    NM_BEGIN_PROFILING(name)
  #define NM_UT_END_PROFILING()          NM_END_PROFILING()
#else
  #define NM_UT_BEGIN_PROFILING(name)
  #define NM_UT_END_PROFILING()
#endif

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void unevenTerrainIKSetup(
  UnevenTerrainHipsIK*                      hipsIKSolver,
  UnevenTerrainLegIK*                       legIKSolvers,
  UnevenTerrainFootIK*                      footIKSolvers,
  AttribDataBasicUnevenTerrainIKSetup*      outputIKSetupAttrib,
  const AttribDataBasicUnevenTerrainSetup*  setupAttrib,
  const AttribDataCharacterProperties*      characterControllerAttrib,
  const AttribDataBasicUnevenTerrainChain*  chainAttrib,
  const AttribDataRig*                      rigAttrib,
  const AttribDataTransformBuffer*          inputTransformsAttrib,
  const NMP::Vector3&                       inputTrajectoryDeltaPos,
  const NMP::Quat&                          inputTrajectoryDeltaAtt);

//----------------------------------------------------------------------------------------------------------------------
void unevenTerrainInitSolve(
  UnevenTerrainHipsIK*                 hipsIKSolver,
  UnevenTerrainLegIK*                  legIKSolvers,
  UnevenTerrainFootIK*                 footIKSolvers,
  AttribDataTransformBuffer*           outputTransformsAttrib,
  const AttribDataRig*                 rigAttrib,
  const AttribDataBasicUnevenTerrainChain* chainAttrib,
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib);

//----------------------------------------------------------------------------------------------------------------------
void unevenTerrainRayCastFromCurrentFootbasePos(
  const NMP::Vector3&                            upAxisWS,
  Network*                                       net,
  const CharacterControllerInterface*            cc,
  const AttribDataBasicUnevenTerrainChain*       chainAttrib,
  const AttribDataBasicUnevenTerrainIKSetup*     inputIKSetupAttrib,
  AttribDataBasicUnevenTerrainFootLiftingTarget* outputFootLiftingTargetAttrib);

//----------------------------------------------------------------------------------------------------------------------
void unevenTerrainDefaultFootLiftingTargets(
  const NMP::Vector3&                            upAxisWS,
  const AttribDataBasicUnevenTerrainIKSetup*     inputIKSetupAttrib,
  AttribDataBasicUnevenTerrainFootLiftingTarget* outputFootLiftingTargetAttrib);

//----------------------------------------------------------------------------------------------------------------------
void unevenTerrainMotionSolve(
  const NMP::Vector3&                            upAxisWS,
  UnevenTerrainHipsIK*                           hipsIKSolver,
  UnevenTerrainLegIK*                            legIKSolvers,
  UnevenTerrainFootIK*                           footIKSolvers,
  const AttribDataTransformBuffer*               inputTransformsAttrib,
  const AttribDataUpdatePlaybackPos*             networkUpdateTimeAttrib,
  const AttribDataBasicUnevenTerrainSetup*       setupAttrib,
  const AttribDataBasicUnevenTerrainChain*       chainAttrib,
  const AttribDataBasicUnevenTerrainIKSetup*     inputIKSetupAttrib,
  const AttribDataBasicUnevenTerrainFootLiftingTarget* inputFootLiftingTargetAttrib,
  const AttribDataFloat*                         ikHipsWeightAttrib,
  AttribDataBasicUnevenTerrainIKState*           inputAndOutputIKStateAttrib);

//----------------------------------------------------------------------------------------------------------------------
void unevenTerrainIKFKBlend(
  UnevenTerrainHipsIK*                 hipsIKSolver,
  UnevenTerrainLegIK*                  legIKSolvers,
  const AttribDataTransformBuffer*     inputTransformsAttrib,
  AttribDataTransformBuffer*           outputTransformsAttrib,
  const AttribDataBasicUnevenTerrainIKState* inputIKStateAttrib,
  const AttribDataFloat*               ikFkBlendWeightAttrib);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_UNEVEN_TERRAIN_UTILITIES_H
//----------------------------------------------------------------------------------------------------------------------
