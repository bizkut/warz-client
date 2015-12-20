/*
 * Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

//----------------------------------------------------------------------------------------------------------------------
#include "MyNetworkPackaging.h"
#include "MyNetwork.h"
#include "EnvironmentAwarenessPackaging.h"
#include "EnvironmentAwarenessData.h"
#include "EnvironmentAwareness.h"

#define SCALING_SOURCE owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"
#include "euphoria/erDebugDraw.h"

//----------------------------------------------------------------------------------------------------------------------


namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
// simple structures for conciseness later on
struct SphereTrajectoryAndImportance
{
  const SphereTrajectory* sphereTrajectory;
  float importance;
};
struct PatchSetAndImportance
{
  PatchSet patchSet;
  float importance;
};

// creates a linear sphere sweep object from the sphere trajectory. This means converting the curve into a single linear sweep
// which cycles along the curve each use
static ER::SphereSweep createSweep(
  const SphereTrajectory& path,
  const Environment::State& state,
  float t1,
  float t2,
  int32_t NMP_UNUSED(sweepSegment),
  int32_t probeID,
  bool MR_OUTPUT_DEBUG_ARG(isDynamic),
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
#if defined(MR_OUTPUT_DEBUGGING)
  SphereTrajectory debugPath = path;
  debugPath.position = path.position;
  debugPath.velocity -= state.velocity;
  debugPath.acceleration -= state.acceleration;
  // draw green for dynamics, and white for statics
  if (isDynamic) // if dynamic then draw line to object we're testing against
  {
    MR_DEBUG_DRAW_LINE(
      pDebugDrawInst,
      debugPath.position + debugPath.velocity * debugPath.maxTime + debugPath.acceleration * 0.5f * NMP::sqr(debugPath.maxTime),
      state.position,
      NMP::Colour::WHITE);
  }
#endif // defined(MR_OUTPUT_DEBUGGING)

  NMP::Vector3 localVelocity = path.velocity - state.velocity;
  NMP::Vector3 localAcceleration = path.acceleration - state.acceleration;
#define IMMEDIATE_DOUBLE_SWEEP // Do up to two sweeps at a time, if the parabola is bendy enough
#if defined(IMMEDIATE_DOUBLE_SWEEP)
  // Physics engines don't support curved sweeps, so we have a conflict of interest:
  // - For best accuracy we want the curve to be divided into many sections
  // - For best performance we want as few sections as possible
  // The solution in this code is to use just 1 or 2 linear sweeps to approximate the curve.
  // Specifically, we generate an optimal break point, and if this point is far enough away
  // from the curve then we use two sections, otherwise we just use one section.

  NMP::Vector3 start = path.position + localVelocity * t1 + 0.5f * localAcceleration * t1 * t1;
  NMP::Vector3 end = path.position + localVelocity * t2 + 0.5f * localAcceleration * t2 * t2;
  NMP::Vector3 toEnd = end - start;
  const float eps = 1e-10f;
  float toEndScale = 1.0f / (eps + toEnd.dot(toEnd));

  // What is the best approximation of a curve by a two connected lines?
  // I.e. what time value along the curve gives the best breakpoint?
  // Since the area under the triangle formed by the two lines will always be less than the area under the
  // quadratic curve, it makes sense that the best approximation is when the triangle has maximum area.
  // In other words, the break point which has largest perpendicular distance from the one-segment sweep (start to end)
  // We find this by taking the perpendicular component of the quadratic equation, and finding its maximum...
  NMP::Vector3 flatAcc = localAcceleration - toEnd * localAcceleration.dot(toEnd) * toEndScale;
  float flatAccSqr = flatAcc.magnitudeSquared();
  float maxAreaTime = 0.0f;
  if (flatAccSqr > 0.01f) // TODO: dimensionally scale
  {
    NMP::Vector3 flatV0 = localVelocity - toEnd * localVelocity.dot(toEnd) * toEndScale;
    // ...If height = 0.5*acc*t^2 + vel*t + c, then the max occurs at -vel/acc
    // Which is -vel*acc / (acc*acc) :
    maxAreaTime = -flatAcc.dot(flatV0) / flatAccSqr;
  }
  else // Special case for when acc is in line with vel
  {
    maxAreaTime = -localVelocity.dot(localAcceleration) / (eps + localAcceleration.magnitudeSquared());
  }
  // Now we have the optimal split time, we clamp it to the known segment bounds
  maxAreaTime = NMP::clampValue(maxAreaTime, 0.0f, t2 - t1);
  // And so we can get the break point position:
  NMP::Vector3 breakPoint = start + localVelocity * maxAreaTime + 0.5f * localAcceleration * maxAreaTime * maxAreaTime;
  // Now we see how far this position is from the straight segment:
  float t = NMP::clampValue((breakPoint - start).dot(toEnd) * toEndScale, 0.0f, 1.0f);
  NMP::Vector3 onLine = start + toEnd * t;
  float errorSqr = (onLine - breakPoint).magnitudeSquared();
  float probeRadiusSqr = NMP::sqr(path.radius);

  ER::SphereSweep sweep;
  sweep.position = path.position + (localVelocity * t1) + (localAcceleration * 0.5f * t1 * t1);

  // And if the break point is close to the linear segment then we just use one segment
  if (errorSqr < probeRadiusSqr)
  {
    // single line:
    NMP::Vector3 endPos = path.position + (localVelocity * t2) + (localAcceleration * 0.5f * t2 * t2);
    sweep.motion = endPos - sweep.position;
    sweep.motion2.setToZero();
  }
  else // Otherwise we'll use two as defined by this breakpoint
  {
    sweep.motion = breakPoint - sweep.position;
    NMP::Vector3 endPos = path.position + (localVelocity * t2) + (localAcceleration * 0.5f * t2 * t2);
    sweep.motion2 = endPos - breakPoint;
  }
  // Convert into position values for linear segment
  sweep.radius = path.radius;
  sweep.targetShapeID = state.shapeID;
  sweep.probeID = probeID;
#else
  // since the trajectory is a quadratic curve, we need to divide it into sections
  // in order that the physics engine can do linear probes.
  // Here is where we work out how many sections to divide the trajectory into
  const int maxSections = 3;
  NMP::Vector3 start = path.position + localVelocity * t1 + 0.5f * localAcceleration * t1 * t1;
  NMP::Vector3 end = path.position + localVelocity * t2 + 0.5f * localAcceleration * t2 * t2;
  NMP::Vector3 handle = start + (localVelocity + localAcceleration * t1) * (t2 - t1);
  float base = vDistanceBetween(start, end);
  float height = vDistanceBetween(handle, end);
  // the multiple on sphere radius is to get it to be more important in this calculation
  float numberOfSections = ((float)maxSections + 1.0f) * height / (base + height + 4.0f * path.radius);
  int numSections = (int)numberOfSections + 1; // the + 1 because casts to int round down

  // get start and end time values for linear segment
  int segment = sweepSegment % numSections;
  float maxTime = t2 - t1;
  float tStart = t1 + maxTime * (float)segment / numSections;
  float tEnd = tStart + maxTime / numSections;

  // convert into position values for linear segment
  ER::SphereSweep sweep;
  sweep.motion2.setToZero(); // addon for above define
  sweep.position = path.position + localVelocity * tStart + localAcceleration * 0.5f * NMP::sqr(tStart);
  sweep.motion = localVelocity * (tEnd - tStart) + localAcceleration * 0.5f * (NMP::sqr(tEnd) - NMP::sqr(tStart));
  sweep.radius = path.radius;
  sweep.targetShapeID = state.shapeID;
  sweep.probeID = probeID;
#endif
  return sweep;
}
//----------------------------------------------------------------------------------------------------------------------
// Find an object in the passed in object list, by its object ID
static const Environment::Object* findObject(
  int numObjects,
  const Environment::Object* dynamicObjectsInRange,
  int64_t shapeID,
  int* index = NULL)
{
  // linear lookup
  for (int j = 0; j < numObjects; j++)
  {
    if (dynamicObjectsInRange[j].state.shapeID == shapeID)
    {
      if (index)
      {
        *index = j;
      }
      // copy across the base data (bounds, velocity etc)
      return &dynamicObjectsInRange[j];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void EnvironmentAwarenessUpdatePackage::update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst)
{
  // no request for a probe, so nothing to do here
  if (feedIn->getRequestNextProbe() == false)
    return;
  uint32_t numImpactPredictorValues = in->calculateNumSphereProjectionsForImpactPredictor();
  uint32_t totalValues = numImpactPredictorValues;

  if (totalValues == 0)
    return;

  // Now we need to collect the separate arrays of sphereTrajectories into one structure
  // the reason they are separate is due to keeping a simple junction system
  SphereTrajectoryAndImportance* in_sphereTrajectoryAndImportances =
    (SphereTrajectoryAndImportance*)alloca(sizeof(SphereTrajectoryAndImportance) * totalValues);
  NMP_ASSERT(in_sphereTrajectoryAndImportances);

  size_t numProbes = 0;
  for (uint32_t i = 0; i < numImpactPredictorValues ; ++i, ++numProbes)
  {
    in_sphereTrajectoryAndImportances[numProbes].sphereTrajectory = &in->getSphereProjectionsForImpactPredictor(i);
    in_sphereTrajectoryAndImportances[numProbes].importance = in->getSphereProjectionsForImpactPredictorImportance(i);
  }

  int numObjects = data->numDynamicObjectsInRange;
  // now we pick which probe to deal with, based on the importance of each probe
  // don't allow more than this many tests per update
  bool foundPotentialDynamic = false;
  bool stationarySoPrioritiseDynamics = false;
  if (data->foundPotentialDynamicLastFrame == true)
  {
    const SphereTrajectory* path = in_sphereTrajectoryAndImportances[data->probeID].sphereTrajectory;
    float motionLengthSqr = (path->velocity * path->maxTime + -0.5f * path->acceleration * NMP::sqr(path->maxTime)).magnitudeSquared();
    if (motionLengthSqr < NMP::sqr(path->radius)) // extra priority for dynamics when character is nearly still
      stationarySoPrioritiseDynamics = true;
  }
  // only look for dynamics on even frames, sort of
  if (data->foundPotentialDynamicLastFrame == false || stationarySoPrioritiseDynamics == true)
  {
    for (size_t probeI = 0; probeI < numProbes; probeI++)
    {
      data->probeID = (data->probeID + 1) % numProbes;
      // if probe not in use then ignore
      if (!in_sphereTrajectoryAndImportances[data->probeID].importance)
        continue;
      PatchStore& store = data->patchStores[data->probeID];

      Environment::State bestState;
      float bestT1 = 1e10f, bestT2 = 1e10f;
      SphereTrajectory bestPath;

      for (int j = 0; j < numObjects; j++)
      {
        // set this path variable inside this loop as it can be altered by adjustPathForAngularVelocity
        SphereTrajectory path = *in_sphereTrajectoryAndImportances[data->probeID].sphereTrajectory;
        store.lastObjectIndex++;
        if (store.lastObjectIndex >= numObjects)
        {
          store.lastObjectIndex = 0;
          store.sweepSegment++;
        }
        float t1, t2;
        NMP_ASSERT(store.lastObjectIndex >= 0 && store.lastObjectIndex < numObjects);
        // create a state structure for this particular object that is in sight
        Environment::State state = data->dynamicObjectsInRange[store.lastObjectIndex].state;
        state.acceleration = data->objectData[store.lastObjectIndex].smoothedAcc;
        state.angularVelocity = data->objectData[store.lastObjectIndex].smoothedAngVel;
        state.accSqr = data->objectData[store.lastObjectIndex].smoothedAccSqr;
        MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngularVelocity, state.position, state.angularVelocity, NMP::Colour::YELLOW);
        state.adjustPathForAngularVelocity(path); // bend the path to compensate for rotation velocity of object

#if defined(MR_OUTPUT_DEBUGGING)
        SphereTrajectory debugPath = path;
        debugPath.position = path.position;
        debugPath.velocity -= state.velocity;
        debugPath.acceleration -= state.acceleration;
        debugPath.debugDraw(NMP::Vector3(0, 0, 1, 1), NMP::Vector3(0, 0, 0, 1), 0.0f, pDebugDrawInst);
        MR_DEBUG_DRAW_LINE(
          pDebugDrawInst,
          debugPath.position + debugPath.velocity * debugPath.maxTime + debugPath.acceleration * 0.5f * NMP::sqr(debugPath.maxTime),
          state.position,
          NMP::Colour::BLACK);
#endif // defined(MR_OUTPUT_DEBUGGING)

        // check if trajectory overlaps the bounding box
        if (!state.getTrajectoryOverlapTimes(path, t1, t2))
        {
          // If it isn't in range, there is one last chance to still probe it...
          // we can add an extra attraction towards the object, but only if it is close by
          // and if there isn't anything more important in sight
          // 1. is object already in working memory
          PatchSet& set = data->patchSets[data->probeID];
          bool knownContactIsFar = true;
          int k = 0;
          for (; k < set.numPatches; k++)
          {
            if (set.patches[k].state.shapeID == state.shapeID)
            {
              // 2. is known contact point far away?
              knownContactIsFar = (set.patches[k].knownContactPoint - path.position).magnitudeSquared() >
                NMP::sqr(2.0f * path.extraSearchRadius);
              break;
            }
          }
          // 3. if last dynamic object probe returned positive then we want to keep our focus on this, no idle time to check other
          // lower priority hazards
          bool objectInRadar =
            (data->dynamicCollidedLastTest &&
             (data->lastDynamicContactPoint - path.position).magnitudeSquared() > NMP::sqr(path.extraSearchRadius)) ||
            (data->staticCollidedLastTest &&
             (data->lastStaticContactPoint  - path.position).magnitudeSquared() > NMP::sqr(path.extraSearchRadius));

          if (!knownContactIsFar || objectInRadar)
            continue;
          // object is close, so try attracting it and looking again...
          NMP::Vector3 offset = NMP::vNormalise(path.position - state.position) * path.extraSearchRadius;
          state.acceleration += 2.0f * offset / NMP::sqr(path.maxTime);
          if (!state.getTrajectoryOverlapTimes(path, t1, t2))
            continue;
        }

        // little adjustment to make it not always choose the closest object
        float tempT1 = (state.shapeID == store.lastObjectShapeID) ? t1 * 1.5f : t1;
        float tempBestT1 = (bestState.shapeID == store.lastObjectShapeID) ? bestT1 * 1.5f : bestT1;
        if (tempT1 < tempBestT1)
        {
          foundPotentialDynamic = true;
          bestState = state;
          bestPath = path;
          bestT1 = t1;
          bestT2 = t2;
        }
      }

      if (foundPotentialDynamic)
      {
        store.lastObjectShapeID = bestState.shapeID;
        // object is within range of the trajectory, so create the sphere sweep structure for it
        data->foundPotentialDynamicLastFrame = foundPotentialDynamic;
        // The current setup means the sweep will occur after the next physics step, this means the
        // objects will have moved from the positions used here. Currently cull problems are avoided
        // by adding a buffer to the line segment:
        //
        // Take away timeStep from both since we'll be on the next frame when sweep is done, then we
        // enlarge by 2*timestep in case of acceleration
        bestT1 -= 3.0f * timeStep;
        bestT2 += timeStep;
        ER::SphereSweep sweep = createSweep(
          bestPath, bestState, bestT1, bestT2, store.sweepSegment, data->probeID, true, pDebugDrawInst);
        // now pass down this sphere sweep which the physics will then perform later
        out->setSphereSweepDynamic(sweep);
        break;
      }
    }
  }
  // if its an even frame or no dynamic object was found
  if (foundPotentialDynamic == false)
  {
    // find a valid probe to test against the static world
    for (size_t probeI = 0; probeI < numProbes; probeI++)
    {
      data->staticProbeID = (data->staticProbeID + 1) % numProbes;
      // If probe not in use then move on
      if (!in_sphereTrajectoryAndImportances[data->staticProbeID].importance)
        continue;
      PatchStore& store = data->patchStores[data->staticProbeID];
      const SphereTrajectory& path = *in_sphereTrajectoryAndImportances[data->staticProbeID].sphereTrajectory;
      store.staticSweepSegment++;
      // creates a dummy state. Constructor ensures default initialisation of data
      Environment::State state;

      // if nothing urgent then apply random offset to find objects that it wouldn't otherwise see
      bool objectInRadar =
        (data->dynamicCollidedLastTest && (data->lastDynamicContactPoint - path.position).magnitudeSquared() > NMP::sqr(path.extraSearchRadius))
        || (data->staticCollidedLastTest && (data->lastStaticContactPoint  - path.position).magnitudeSquared() > NMP::sqr(path.extraSearchRadius));
      if (!objectInRadar)
      {
        NMP::Vector3 randomDir(data->rng.genFloat(-1, 1), data->rng.genFloat(-1, 1), data->rng.genFloat(-1, 1));
        randomDir -= owner->data->up * owner->data->up.dot(randomDir); // flatten
        state.acceleration += 2.0f * NMP::vNormalise(randomDir) * path.extraSearchRadius / NMP::sqr(path.maxTime);
      }
      bool movementIsSlow = path.velocity.magnitudeSquared() < NMP::sqr(SCALE_VEL(0.05f)) &&
                            path.acceleration.magnitudeSquared() < NMP::sqr(SCALE_ACCEL(0.1f));
      if (!movementIsSlow) // don't sweep if movement is too small
      {
        // create linear sphere sweep object
        ER::SphereSweep sweep = createSweep(path, state, 0.0f, path.maxTime,
          store.staticSweepSegment, data->staticProbeID, false, pDebugDrawInst);
        // pass down the sphere sweep object to be used in a lower module
        out->setSphereSweepStatic(sweep);
      }
      data->foundPotentialDynamicLastFrame = false;
      break;
    }
  }
}

// Local space patches have been stored, so this function pulls them back into world space. The
// effect is that you don't need to re-probe an object that is moving, the patch will move with the
// object as long as it is in sight.
static void updatePatchesFromObject(
  const Environment::Object* object,
  PatchSet& set,
  PatchStore& store,
  const ER::SweepResult& sweepResult,
  const ER::DimensionalScaling& dimensionalScaling,
  bool overridePatch = true)
{
  // make a new environment patch from the bound and the sweep result, then decide where to put the new object
  Environment::Patch patch;
  patch.state = object->state;
  patch.updateFromSweepResult(sweepResult, dimensionalScaling);

  Environment::LocalShape localShape;
  localShape.fromWorldSpace(patch, object->matrix);
  int i;
  for (i = 0; i < set.numPatches; i++)
  {
    // compares the local space shapes to see if the patches are connected
    // if they are then one can be removed... this prevents build up of lots of nearby patches
    if (patch.isConnectedTo(set.patches[i], localShape, store.localShapes[i]))
    {
      if (overridePatch || set.patches[i].type == Environment::Patch::EO_ContactPlane)
      {
        // so we keep the acceleration and smoothed angular velocity calculated above
        patch.state.acceleration = set.patches[i].state.acceleration;
        patch.state.position = set.patches[i].state.position;
        patch.state.velocity = set.patches[i].state.velocity;
        patch.state.angularVelocity = set.patches[i].state.angularVelocity;
        patch.state.accSqr = set.patches[i].state.accSqr;
        // if there is a neighbour patch then just replace this
        set.patches[i] = patch;
        store.localShapes[i] = localShape;
      }
      break;
    }
  }
  // object hasn't been found, so find a space to put it
  if (i == set.numPatches)
  {
    // if there is space, then add the object at the end of the list
    if (set.numPatches < PatchSet::MAX_NUM_PATCHES)
      store.patchesHead = set.numPatches++;
    // otherwise write over the oldest potential hazard
    store.localShapes[store.patchesHead] = localShape;
    // otherwise write over the oldest potential hazard
    set.patches[store.patchesHead++] = patch;
    // this implements the rolling queue of collision results
    if (store.patchesHead >= set.numPatches)
      store.patchesHead = 0;
    NMP_ASSERT(store.patchesHead >= 0 && store.patchesHead < PatchSet::MAX_NUM_PATCHES);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// this module implements a probing policy for retrieving potential hazards, but also for
// retrieving information on useful objects in the vicinity, the latter we leave for later.
void EnvironmentAwarenessFeedbackPackage::feedback(float timeStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  int numObjects = data->numDynamicObjectsInRange;
  // Here we calculate smoothed acceleration and angular velocities for all objects that come through
  {
    Environment::ObjectData* tempData =
      (Environment::ObjectData*)alloca(EnvironmentAwarenessData::maxObjectsInRange * sizeof(Environment::ObjectData));
    // for angular velocity smoothing
    float invTimeStep = 1.0f / timeStep;
    float changeRate = SCALE_FREQUENCY(1.0f); // has units 1/time
    float smoothRate = 1.0f / (1.0f + changeRate * timeStep);

    // for acceleration smoothing
    // has units 1/time...
    float accChangeSensitivity = SCALE_FREQUENCY(2.0f);
    // cache smooth coefficients for later use
    float invSmooth = 1.0f / (1.0f + accChangeSensitivity * timeStep);
    float invGrav = 1.0f / (owner->data->gravityMagnitude + 1e-10f);

    // This block finds object state data from the previous frame and uses it to calculate a smooth acceleration
    for (int i = 0; i < numObjects; i++)
    {
      const Environment::Object& object = data->dynamicObjectsInRange[i];
      // if we find the data in last frame's list
      Environment::ObjectData& objData = data->objectData[object.dataIndex];
      if (object.dataIndex >= 0 && object.dataIndex < EnvironmentAwarenessData::maxObjectsInRange &&
          objData.shapeID == object.state.shapeID)
      {
        NMP_ASSERT(objData.inViewFrameCount >= 0);
        // if last acceleration was 0 then it has only just arrived in view so we give it the
        // unsmoothed acceleration as our best guess
        NMP::Vector3 newAcc = (object.state.velocity - objData.lastVel) * invTimeStep;
        float newAccSqr = newAcc.magnitudeSquared();
        // here we decide that very large accelerations are probably caused by impulses, so reduce
        // the perceived acceleration down to 0 at 2*maxAcc
        if (owner->rampDownLargeAccelerations(newAcc))
        {
          objData.inViewFrameCount = 0;
        }
        NMP::Vector3 currentAcc = newAcc;

        if (objData.inViewFrameCount > 0)
        {
          // This block implements an impulse-proof acceleration calculation. By taking the minimum
          // velocity change between 2 consecutive velocities
          NMP::Vector3 acc = (objData.lastVel - objData.secondLastVel) * invTimeStep;
          float accSqr = acc.magnitudeSquared();
          if (accSqr < newAccSqr)
          {
            newAcc = acc;
          }
        }

        if (objData.inViewFrameCount > 2)
        {
          // in place acceleration smoothing for extra speed
          tempData[i].smoothedAcc = newAcc + (objData.smoothedAcc - newAcc) * invSmooth;
          tempData[i].smoothedAccSqr = newAccSqr + (objData.smoothedAccSqr - newAccSqr) * invSmooth;
        }
        else
        {
          // don't smooth the acceleration on the first or 2nd frame as its too early
          tempData[i].smoothedAcc = newAcc;
          tempData[i].smoothedAccSqr = newAccSqr;
        }

        // value ramping 0 up to 1 if acceleration is downwards like gravity
        float smoothedAccDown = tempData[i].smoothedAcc.dot(owner->data->down);
        float currentAccDown = currentAcc.dot(owner->data->down);
        float downScale = NMP::clampValue(currentAccDown * invGrav, 0.0f, 1.0f);
        // people are very sensitive to the feeling of falling, since it is important we give it high weight here,
        // in fact, completely fully high weight for full gravity
        tempData[i].smoothedAcc += ((currentAccDown - smoothedAccDown) * downScale) * owner->data->down;

        tempData[i].secondLastVel = objData.lastVel;
        tempData[i].smoothedAngVel =
          object.state.angularVelocity + (objData.smoothedAngVel - object.state.angularVelocity) * smoothRate;
        tempData[i].inViewFrameCount = objData.inViewFrameCount + 1;
      }
      // if we don't find data in last frame's list then we must just set acceleration to 0
      else
      {
        tempData[i].smoothedAngVel = object.state.angularVelocity;
        tempData[i].smoothedAcc.setToZero();
        tempData[i].smoothedAccSqr = 0.0f;
        // we don't know second last vel here, so assume its same as current
        tempData[i].secondLastVel = object.state.velocity;
        tempData[i].inViewFrameCount = 0;
      }
      tempData[i].lastVel = object.state.velocity;
      tempData[i].shapeID = object.state.shapeID;
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Acceleration, object.state.position, tempData[i].smoothedAcc, NMP::Colour::LIGHT_YELLOW);
    }
    // necessary to copy at the end
    for (int i = 0; i < numObjects; i++)
    {
      data->objectData[i] = tempData[i];
    }
  }
  // OK, object tracking done here, we just pass back the object with the greatest metric
  float largestMetric = -1.0f;
  int bestI = 0;
  if (in->getFindObject().focusShapeID == -1)
  {
    data->lastMetric = 0.0f; // Reset time hysteresis if no object of significance to set it on
  }
  for (int i = 0; i < numObjects; i++)
  {
    data->dynamicObjectsInRange[i].state.acceleration = data->objectData[i].smoothedAcc;
    float metric = in->getFindObject().getMetric(data->dynamicObjectsInRange[i].state, timeStep, data->lastMetric);
    if (metric > largestMetric)
    {
      largestMetric = metric;
      bestI = i;
    }
  }
  if (largestMetric > 0.0f)
  {
    feedOut->setFoundObject(data->dynamicObjectsInRange[bestI].state);
    feedOut->setFoundObjectMetric(largestMetric);
  }

  size_t numImpactPredictorValues = in->calculateNumSphereProjectionsForImpactPredictor();
  size_t totalValues = numImpactPredictorValues;

  if (totalValues == 0)
    return;

  // collect sphere trajectories into a single list, separate lists exist to keep junctions simpler
  SphereTrajectoryAndImportance* in_sphereTrajectoryAndImportances =
    (SphereTrajectoryAndImportance*)alloca(sizeof(SphereTrajectoryAndImportance) * totalValues);
  PatchSetAndImportance* feedOut_patchSetAndImportances =
    (PatchSetAndImportance*)alloca(sizeof(PatchSetAndImportance) * totalValues);
  NMP_ASSERT(feedOut_patchSetAndImportances && in_sphereTrajectoryAndImportances);

  size_t numProbes = 0;
  // fill in the single list
  for (uint32_t i = 0; i < numImpactPredictorValues ; ++i, ++numProbes)
  {
    in_sphereTrajectoryAndImportances[numProbes].sphereTrajectory = &in->getSphereProjectionsForImpactPredictor(i);
    in_sphereTrajectoryAndImportances[numProbes].importance = in->getSphereProjectionsForImpactPredictorImportance(i);
    feedOut_patchSetAndImportances[numProbes].importance = 0.0f;
  }

  // Here we update the set of patches from the set of sphere sweep results that have come back
  for (int j = 0; j < (int) numProbes; j++)
  {
    // If trajectory doesn't exist then move on
    if (!in_sphereTrajectoryAndImportances[j].importance)
    {
      continue;
    }

    PatchStore& store = data->patchStores[j];
    PatchSet& set = data->patchSets[j];

    // update dynamic
    data->dynamicCollidedLastTest = false;
    if (feedIn->getSweepResultDynamic().getProbeID() == j &&
        feedIn->getSweepResultDynamic().getType() != ER::SweepResult::SR_NoContact)
    {
      const Environment::Object* object =
        findObject(numObjects, data->dynamicObjectsInRange, feedIn->getSweepResultDynamic().getShapeID());
      if (object)
      {
        updatePatchesFromObject(object, set, store, feedIn->getSweepResultDynamic(), SCALING_SOURCE);
        // Note, this test becomes a little unusual when you have several probes, however the principle still works,
        // if any object is successfully probed (and is outside the extra radius) then this takes priority over
        // other nearby potential future hazards
        data->dynamicCollidedLastTest = true;
        data->lastDynamicContactPoint = feedIn->getSweepResultDynamic().getContactPoint();
      }
    }
    // update static
    data->staticCollidedLastTest = false;
    if (feedIn->getSweepResultStatic().getProbeID() == j &&
        feedIn->getSweepResultStatic().getType() != ER::SweepResult::SR_NoContact)
    {
      const Environment::Object object = data->staticSweepResultObject;
      updatePatchesFromObject(&object, set, store, feedIn->getSweepResultStatic(), SCALING_SOURCE);
      data->staticCollidedLastTest = true;
      data->lastStaticContactPoint = feedIn->getSweepResultStatic().getContactPoint();
    }
    // update contact results
    if (feedIn->getContactResult().getType() != ER::SweepResult::SR_NoContact)
    {
      // if we have registered a static contact that matches the ID, then use this
      // note, id will be 0 (so the condition false) if there is no staticContactObject
      if (data->staticContactObject.state.shapeID == feedIn->getContactResult().getShapeID())
      {
        // this doesn't overwrite the higher precedence sight patch if it finds one
        updatePatchesFromObject(&data->staticContactObject, set, store, feedIn->getContactResult(), SCALING_SOURCE, false);
      }
      else // otherwise, search for the dynamic object that matches the ID.
      {
        const Environment::Object* object =
          findObject(numObjects, data->dynamicObjectsInRange, feedIn->getContactResult().getShapeID());
        if (object)
        {
          // this doesn't overwrite the higher precedence sight patch if it finds one
          updatePatchesFromObject(object, set, store, feedIn->getContactResult(), SCALING_SOURCE, false);
        }
      }
    }
    // update each potential hazard, removing them if they should be in view but they're gone
    for (int i = set.numPatches - 1; i >= 0; i--)
    {
      if (!set.patches[i].state.isStatic) // if dynamic
      {
        int32_t objectIndex = -1;
        const Environment::Object* object =
          findObject(numObjects, data->dynamicObjectsInRange, set.patches[i].state.shapeID, &objectIndex);
        // if object exists, then simply update from the local space patch
        if (object)
        {
          set.patches[i].state = object->state;
          set.patches[i].state.acceleration = data->objectData[objectIndex].smoothedAcc;
          set.patches[i].state.angularVelocity = data->objectData[objectIndex].smoothedAngVel;
          set.patches[i].state.accSqr = data->objectData[objectIndex].smoothedAccSqr;
          set.patches[i].isPredicted = false;
          if (!object->isStill)
            store.localShapes[i].toWorldSpace(set.patches[i], object->matrix);
        }
        // otherwise we have to extrapolate from the last velocity values known
        else
        {
          // predict motion of patch if it doesn't link to any object in range
          set.patches[i].update(timeStep);
          set.patches[i].isPredicted = true;
          // if it is predicted to be in our line of sight but the bound isn't found, then remove
          float distSqr = set.patches[i].state.position.distanceSquaredTo(feedIn->getFocalCentre());
          if (distSqr < NMP::sqr(feedIn->getFocalRadius())) // object should be in view, but isn't
          {
            store.localShapes[i] = store.localShapes[--set.numPatches];
            set.patches[i] = set.patches[set.numPatches];
          }
        }
      }
    }

    feedOut_patchSetAndImportances[j].patchSet = set;
    feedOut_patchSetAndImportances[j].importance = 1.0f;
    // Draw the patches, which will visualise as orange edges/corners/planes etc.
#if defined(MR_OUTPUT_DEBUGGING)
    for (int i = 0; i < set.numPatches; i++)
    {
      set.patches[i].debugDraw(SCALE_DIST(1.0f), pDebugDrawInst, SCALING_SOURCE);
    }
#endif
  }

  // finally pass up the set of potential hazards resulting from the patches
  numProbes = 0;
  MR_DEBUG_DRAW_POINT(
    pDebugDrawInst,
    in->getSphereProjectionsForImpactPredictor(0).position + NMP::Vector3(0, 0.2f, 0),
    SCALE_DIST(0.2f) * (float)feedOut_patchSetAndImportances[0].patchSet.numPatches,
    NMP::Colour::BLUE);
  for (uint32_t i = 0; i < numImpactPredictorValues ; ++i, ++numProbes)
  {
    feedOut->setPotentialHazardsForImpactPredictorAt(i,
      feedOut_patchSetAndImportances[numProbes].patchSet, feedOut_patchSetAndImportances[numProbes].importance);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EnvironmentAwareness::entry()
{
  data->foundPotentialDynamicLastFrame = false;
  data->rng.setSeed(owner->getAndBumpNetworkRandomSeed());
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE

