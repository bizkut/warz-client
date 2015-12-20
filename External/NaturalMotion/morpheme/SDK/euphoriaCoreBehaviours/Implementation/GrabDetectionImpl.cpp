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
#include "mrPhysX3.h"
#include "GrabDetectionPackaging.h"
#include "GrabDetection.h"
#include "MyNetworkPackaging.h"
#include "MyNetwork.h"
#include "Helpers/Helpers.h"
#include "mrPhysicsScenePhysX3.h"
#include "euphoria/erBody.h"
#include "euphoria/erDebugDraw.h"
#include "Types/Environment_CollideResult.h"
#include "HazardManagement.h"
#include "NMGeomUtils/NMGeomUtils.h"

#define SCALING_SOURCE grabDetection.owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------


// Even though the EA system has a small memory of perceived objects, in busy environments it can often "forget" about
// objects that are still appropriate for grabbing. The edge buffer enabled via this option, stores those edges
// previously deemed grabbable, and will only forget them once better edges are detected.
#define USE_EDGE_BUFFER 1

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

// for querying the number of edges a patch consists of
extern int cNumberOfEdges [3];
extern int cNumberOfFaces [3];

//----------------------------------------------------------------------------------------------------------------------
// Analyze an edge's grabbability using information about distance, approach velocity and its orientation in space
// The edge could be coming from an environment awareness patch, or from user specified information
//----------------------------------------------------------------------------------------------------------------------
static void processEdge(
  Edge& gEdge,
  const GrabDetection& grabDetection,
  const NMP::Vector3& grabPosVel,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  // Early out: if edge is too short, or the edge joins two surfaces which are too similar in orientation,
  // then we can't grab it
  const float minEdgeLengthSq = NMP::sqr(grabDetection.in->getParams().minEdgeLength);
  const float minAngleBetweenNormals = grabDetection.in->getParams().minAngleBetweenNormals;
  const float maxReachDistance = grabDetection.in->getParams().maxReachDistance;

  const float angleBetweenNormals = NMP::fastArccos(NMP::clampValue(
    gEdge.uprightNormal.dot(gEdge.otherNormal), -1.0f, 1.0f));
  NMP::Vector3 chestToGrabPointDir = gEdge.point
    - grabDetection.owner->owner->data->spineLimbSharedStates[0].m_endTM.translation();
  const float chestToGrabPointDist = chestToGrabPointDir.normaliseGetLength();

  const float edgeLengthSq = gEdge.edge.magnitudeSquared();
  const bool edgeTooShort = edgeLengthSq < minEdgeLengthSq;
  const bool surfacesTooPlanar = angleBetweenNormals < minAngleBetweenNormals;
  const bool edgeTooFarAway = chestToGrabPointDist > maxReachDistance;

  if (edgeTooShort || surfacesTooPlanar || edgeTooFarAway)
  {
    // Edge should be ignored.
    gEdge.quality = 0.0f;
    return;
  }

  // Calculate current distance measured in proportion to arm length.
  float edgeProximity;
  // Most people are probably not very good at accurately judging their arm length. Therefore we're slightly
  // conservative here and err on the side of longer arms, meaning the character will try to grab even if
  // the target is slightly further away.
  const float armLengthOverestimationFactor = 1.1f; // give ourselves some wiggle room
  // Any edge closer than preferedMaxReachDist is automatically assigned the maximum quality
  const float preferedMaxReachDist = armLengthOverestimationFactor * grabDetection.data->maxArmLength;
  const float armRelImpactDist = chestToGrabPointDist / preferedMaxReachDist;
  bool isCurrentlyReachable = armRelImpactDist < 1.0f;

  if (isCurrentlyReachable)
  {
    // All edges closer than arm length are perfectly and equally good.
    edgeProximity = 1.0f;
  }
  else
  {
    // Calculate closest point of approach, as well as actual distance at and time to closest point of approach
    // If at predicted position we won't be able to reach, discard this edge
    const NMP::Vector3& chestPos = grabDetection.owner->feedIn->getChestState().position;
    // note: CoM velocity is much more useful for extrapolating character's velocity.
    const NMP::Vector3& chestVel = grabDetection.feedIn->getCentreOfMassBodyState().velocity;
    const NMP::Vector3& chestAcc = grabDetection.owner->owner->data->gravity;

    const ClosestPointOfApproachData closestPointOfApproachData = calculateClosestPointOfApproach(
      chestPos, gEdge.point, chestVel, grabPosVel, chestAcc, pDebugDrawInst);

    // Project closest point of approach on chest trajectory onto edge to find best mean grab position.
    const NMP::Vector3 chestPosAtCpa = chestPos + (chestVel * closestPointOfApproachData.time) + 
      (chestAcc * (closestPointOfApproachData.time * closestPointOfApproachData.time * 0.5f));
    NMP::Vector3 edgeDir = gEdge.edge;
    edgeDir.fastNormalise();
    const float edgeLength = NMP::vDot(edgeDir, gEdge.edge);
    gEdge.point = projectPointOntoLineSegment(gEdge.corner, edgeDir, chestPosAtCpa, 0.0f, edgeLength);
    const float chestToEdgeDist = gEdge.point.distanceTo(chestPosAtCpa);

    MR_DEBUG_DRAW_POINT(pDebugDrawInst, chestPosAtCpa, 0.4f, NMP::Colour::DARK_RED);
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, gEdge.point, 0.4f, NMP::Colour::DARK_RED);
    MR_DEBUG_DRAW_LINE(pDebugDrawInst, chestPosAtCpa, gEdge.point, NMP::Colour::DARK_RED);

    // Don't grab this edge if it takes longer than this until we get close enough. Empirically determined value
    // that seems to be working.
    const float maxTimeToImpact = SCALE_TIME(3.0f);
    const bool willApproach = closestPointOfApproachData.time > 0 && closestPointOfApproachData.time < maxTimeToImpact;

    float reachDistProportion = chestToEdgeDist / preferedMaxReachDist;
    const bool willBeInReach = willApproach && reachDistProportion < 1.0f;
    if (willBeInReach)
    {
      edgeProximity = 1.0f - closestPointOfApproachData.time / maxTimeToImpact;
#if defined(MR_OUTPUT_DEBUGGING)
      NMP::Colour colour = NMP::Colour(getRainbowColourMapped(1.0f - chestToEdgeDist / 2.0f), 255);
      MR_DEBUG_DRAW_POINT(pDebugDrawInst, chestPos, SCALE_DIST(0.07f), NMP::Colour::RED);
      MR_DEBUG_DRAW_POINT(pDebugDrawInst, closestPointOfApproachData.positionA, SCALE_DIST(0.07f), colour);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, 
        MR::VT_Normal, chestPos, closestPointOfApproachData.positionB - chestPos, colour);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, 
        MR::VT_Normal, closestPointOfApproachData.positionA, gEdge.point - closestPointOfApproachData.positionA, colour);
#endif // defined(MR_OUTPUT_DEBUGGING)
    }
    else
    {
      // Edge won't be reachable
      gEdge.quality = 0.0f;
      return;
    }
  }

  // Early out if game has specified a grabbability. This is here combined with distance info before
  // being returned. I.e. the specified value replaces calculations of how horizontal the edge is
  // (for example)
  if (gEdge.quality > 0.0f)
  {
    gEdge.quality *= edgeProximity;
    return;
  }
  else
  {
    gEdge.quality = edgeProximity;
  }

  // Make sure that the normals are the right way round
  float n1DotUp = gEdge.uprightNormal.dot(grabDetection.owner->owner->data->up);
  float n2DotUp = gEdge.otherNormal.dot(grabDetection.owner->owner->data->up);
  if (n2DotUp > n1DotUp)
  {
    // swap normals so they reflect their name in the Edge (uprightNormal & otherNormal)
    NMP::nmSwap(gEdge.uprightNormal, gEdge.otherNormal);
  }

  // Check if the binormal (average of the two normals) is in the allowed range
  float allowedBinormalDotUp = cosf(grabDetection.in->getParams().maxSlope);
  NMP::Vector3 binormal = gEdge.uprightNormal + gEdge.otherNormal;
  binormal.normaliseOrDef(grabDetection.owner->owner->data->up);

  float binormalDotUp = binormal.dot(grabDetection.owner->owner->data->up);
  if (binormalDotUp < allowedBinormalDotUp)
  {
    gEdge.quality = 0.0f;
    return;
  }

  if (allowedBinormalDotUp < 0.999f)
  {
    float qualityFrac = (binormalDotUp - allowedBinormalDotUp) / (1.0f - allowedBinormalDotUp);
    // We only want to prefer more upright edges - but don't let the quality drop just because they're not upright
    float minQualityFrac = 0.999f;
    qualityFrac = minQualityFrac + qualityFrac * (1.0f - minQualityFrac);
    gEdge.quality *= qualityFrac;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Get user edge quality, and extract and store some extra information
//----------------------------------------------------------------------------------------------------------------------
static void processGameEdge(Edge& userEdge, const GrabDetection& grabDetection, MR::InstanceDebugInterface* pDebugDrawInst)
{
  // For game input edges we don't perform sphereSweeps to determine the predicted impact, but simply
  // project the current chest position on to the edge.
  NMP::Vector3 edgeNormalised = userEdge.edge;
  float edgeLength = edgeNormalised.normaliseGetLength();
  userEdge.point = projectPointOntoLineSegment(userEdge.corner, edgeNormalised,
    grabDetection.owner->owner->data->spineLimbSharedStates[0].m_endTM.translation(),
    0.0f, edgeLength);

  // Quality is set by game:
  // -1: completely determined by euphoria
  // [0, 1]: user value multiplied by proximity (don't try when out of reach)
  // > 1: forced reach action (don't try when out of reach)
  if (userEdge.quality <= 1.0f)
  {
    // For game input edges we assume they're stationary. If the behaviour gets extended to deal with passed-in
    // edges that are local to dynamic objects, this assumption needs to be relaxed.
    NMP::Vector3 grabPosVel(0.0, 0.0f, 0.0f);
    processEdge(userEdge, grabDetection, grabPosVel, pDebugDrawInst);
  }
  else
  {
    // Do not process the edge but still take maxReachDistance to consideration when edge quality is > 1.
    const float chestToGrabPointDist =
      NMP::fastSqrt((userEdge.point
        - grabDetection.owner->owner->data->spineLimbSharedStates[0].m_endTM.translation()).magnitudeSquared());
    const bool edgeTooFarAway = chestToGrabPointDist > grabDetection.in->getParams().maxReachDistance;
    if (edgeTooFarAway)
    {
      // Edge should be ignored.
      userEdge.quality = 0.0f;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Analyzes the edges of a patch returned by the environment awareness system for grabbability.
//----------------------------------------------------------------------------------------------------------------------
static bool processPatchEdges(
  const GrabDetection& grabDetection, const Environment::Patch& patch, Edge& bestEdgeSoFar,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  NMP::Vector3 patchEdges[3];
  patch.getEdgeDirections(&patchEdges[0]);

  Edge edge;
  edge.shapeID = patch.state.shapeID;
  edge.corner = patch.corner;

  // Pre-processing: we need to know whether we're dealing with a pole object or a large surface
  // before we can evaluate each edge. The reason being that the steepness of the surface only
  // matters if it's a wall/ground object, but not a pole. It also matters, because a pole doesn't
  // have a preferred side that the character should be hanging on. For a wall/box on the other
  // hand, the character wants to face the outside of the wall when hanging.
  const float maxPoleWidth = SCALE_DIST(0.2f);
  // A pole is a either thin capsule ...
  bool isPole = (patch.type == Environment::Patch::EO_Capsule) && (patch.radius < maxPoleWidth);
  // ... or any object with at least two short edges
  if (!isPole)
  {
    uint16_t numShortEdges = 0;
    for (uint16_t i = 0; i < cNumberOfEdges[patch.type]; i++)
    {
      if (patch.edgeLengths[i] <= maxPoleWidth)
        numShortEdges++;
    }
    isPole = numShortEdges >= 2;
  }
  edge.isWall = !isPole;

  // Now evaluate each edge
  bool betterEdgeFound = false;
  for (uint16_t i = 0; i < cNumberOfEdges[patch.type]; i++)
  {
    edge.edge = patchEdges[i] * patch.edgeLengths[i];

#if USE_EDGE_BUFFER
    // early out: if this edge is already in the buffer, skip it here, it will be evaluated later on
    // in a separate loop
    if (grabDetection.data->edgeBuffer.find(edge) >= 0)
    {
  #if defined(MR_OUTPUT_DEBUGGING)
      edge.debugDraw(pDebugDrawInst);
  #endif
      continue;
    }
#endif

    // reset as we're using the same edge repeatedly
    edge.quality = 0.0f;

    // For edge types, there is no corner. Instead we use the knownContactPoint projected onto the edge
    // and shift it back by half of edge length
    if (patch.type == Environment::Patch::EO_Edge)
    {
      edge.corner = projectPointOntoLineSegment(edge.corner, patchEdges[i], patch.knownContactPoint);
      edge.corner -= 0.5f * patch.edgeLengths[i] * patchEdges[i];
    }

    // Capsules store data differently from other patch types. The capsule's edge vector is of the
    // actual length, while for non-capsules the edges are stored normalised, and lengths are stored
    // separately.
    if (patch.type == Environment::Patch::EO_Capsule)
    {
      // construct normals with the upright one approximating world up
      edge.uprightNormal = grabDetection.owner->owner->data->up;
      edge.otherNormal = NMRU::GeomUtils::calculateOrthogonalPerpendicularVector(edge.edge, edge.uprightNormal);
      edge.otherNormal.fastNormalise();
      edge.uprightNormal = NMRU::GeomUtils::calculateOrthogonalPlanarVector(edge.edge, edge.uprightNormal);
      edge.uprightNormal.fastNormalise();

      edge.corner += patch.radius * edge.uprightNormal;
      // Using a predicted contact point slightly in the future seems to be working better. Value
      // determined empirically.
      const float predictionTime = SCALE_TIME(0.2f);
      NMP::Vector3 chestPos = grabDetection.owner->feedIn->getChestState().position + 
        predictionTime * grabDetection.data->smoothedChestVel;
      edge.point = projectPointOntoLineSegment(edge.corner, patchEdges[i], chestPos, 0, patch.edgeLengths[0]);
    }
    // all other types
    else
    {
      // Using a predicted contact point slightly in the future seems to be working better. Value
      // determined empirically.
      const float predictionTime = SCALE_TIME(0.2f);
      NMP::Vector3 chestPos = grabDetection.owner->feedIn->getChestState().position + 
        predictionTime * grabDetection.data->smoothedChestVel;
      edge.point = projectPointOntoLineSegment(edge.corner, patchEdges[i], chestPos, 0, patch.edgeLengths[i]);

      // Extract normals for this edge. They're temporarily stored in potentially wrong order. I.e.
      // edge.uprightNormal should hold the more upright one and otherNormal the less upright one.
      // They'll be ordered by the processEdge function. This is because we want to use the same
      // function on game specified edges, without bothering users with the correct order of the
      // normals.
      NMP_ASSERT(patch.getNormal1Index(i) != patch.getNormal2Index(i));
      edge.uprightNormal = patch.faceNormals[patch.getNormal1Index(i)];
      edge.otherNormal   = patch.faceNormals[patch.getNormal2Index(i)];
    }

    // Approach velocity (relative velocity between grab point and chest) :
    NMP::Vector3 grabPosVel = patch.state.isStatic ? 
      NMP::Vector3(0, 0, 0) : patch.state.getVelocityAtPoint(edge.point);

    // calculate quality given this information
    processEdge(edge, grabDetection, grabPosVel, pDebugDrawInst);

    // save better edge for returning to caller
    if (edge.quality > bestEdgeSoFar.quality)
    {
      bestEdgeSoFar = edge;
      betterEdgeFound = true;
    }

#if defined(MR_OUTPUT_DEBUGGING)
    edge.debugDraw(pDebugDrawInst);
#endif
  } // for all edges

  return betterEdgeFound;
}

//----------------------------------------------------------------------------------------------------------------------
// Loop over patch's edges and return best one
//----------------------------------------------------------------------------------------------------------------------
static bool processPatch(
  const GrabDetection& grabDetection, const Environment::Patch& patch, Edge& bestEdgeSoFar,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  // we can't grab edges that are not guaranteed to be correct
  if (patch.isPredicted)
  {
    return false;
  }

  // Size and mass thresholds. If too light, not appropriate for holding on. So don't bother calculating
  // quality of individual edges.
  if (!patch.state.isStatic)
  {
    const float minMass = grabDetection.in->getParams().minMass;
    const float minVol = grabDetection.in->getParams().minVolume;
    float volume = patch.state.aabb.getVolume();
    if (patch.state.mass < minMass || volume < minVol)
    {
      return false;
    }
  }

  // get the best edge from this patch
  bool foundBetterEdge = processPatchEdges(grabDetection, patch, bestEdgeSoFar, pDebugDrawInst);

  // modulate edge quality with volume and mass data from this patch if if is a dynamic object
  if (foundBetterEdge)
  {
    if (!patch.state.isStatic)
    {
      // extents / size
      // These could be exposed in behaviour interface.
      const float volumeMax = SCALE_VOLUME(16.0f); // m^3
      float volume = patch.state.aabb.getVolume();
      volume = NMP::minimum(volume, volumeMax);
      float volumeScaler = volume / volumeMax;
      bestEdgeSoFar.quality *= volumeScaler;

      // mass
      const float massMax = SCALE_MASS(100.0f);
      float mass = patch.state.mass;
      mass = NMP::minimum(mass, massMax);
      float massScaler = mass / massMax;
      bestEdgeSoFar.quality *= massScaler;
    }
  }
  return foundBetterEdge;
}

//----------------------------------------------------------------------------------------------------------------------
// Loop over all patches and find best edge
//----------------------------------------------------------------------------------------------------------------------
static void processAllPatches(
  const GrabDetection& grabDetection,
  Edge& bestEdgeSoFar,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  // early out if no patches received this frame
  if (grabDetection.feedIn->getPotentialGrabPatchesImportance() == 0.0f)
  {
    return;
  }

  // otherwise we do have incoming patches
  const PatchSet& potentialGrabPatches = grabDetection.feedIn->getPotentialGrabPatches();
  const int numPatches = potentialGrabPatches.numPatches;
  for (int i = 0; i < numPatches; i++)
  {
    // only consider these types of patches
    const Environment::Patch& patch = potentialGrabPatches.patches[i];
    NMP_ASSERT(patch.state.shapeID >= 0);
    bool isAllowedType = (patch.type == Environment::Patch::EO_Corner) || (patch.type == Environment::Patch::EO_Edge) ||
                         (patch.type == Environment::Patch::EO_Capsule);

    if (grabDetection.in->getGrabFilteringImportance() != 0.0f)
    {
      const ObjectFiltering userFilter = grabDetection.in->getGrabFiltering();
      const physx::PxFilterData userFilterData(userFilter.word0, userFilter.word1, userFilter.word2, userFilter.word3);

      // If there is a shape associated with a patch verify whether shape is grabbable.
      const physx::PxShape* const shape = ER::getPxShapeFromShapeID(patch.state.shapeID);
      if (shape)
      {
        const bool shapeGrabbable = MR::applyFilterShader(shape, &userFilterData);
        isAllowedType = isAllowedType && shapeGrabbable;
      }
    }

    if (!isAllowedType)
    {
      continue;
    }

    // extract best edge from this patch
    bool betterEdgeFound = processPatch(grabDetection, patch, bestEdgeSoFar, pDebugDrawInst);
    if (betterEdgeFound)
    {
#if USE_EDGE_BUFFER
      // Cache the good ones. Statics only, as otherwise the spatial information won't be correct later on.
      if (patch.state.isStatic && bestEdgeSoFar.quality > grabDetection.in->getParams().minEdgeQuality)
      {
        grabDetection.data->edgeBuffer.priorityInsert(bestEdgeSoFar);
      }
#endif
    } // if better edge found
  } // for all patches
}

//----------------------------------------------------------------------------------------------------------------------
// Compare best edge found so far against buffered edges and clean up buffer
//----------------------------------------------------------------------------------------------------------------------
#if USE_EDGE_BUFFER
static void processBufferedEdges(const GrabDetection& grabDetection, 
                                 Edge& bestEdgeSoFar, 
                                 MR::InstanceDebugInterface* pDebugDrawInst)
{
  for (uint8_t i = 0; i < grabDetection.data->edgeBuffer.size(); i++)
  {
    Edge& bufferedEdge = *grabDetection.data->edgeBuffer.get(i);
    bufferedEdge.quality = 0.0f; // reset quality as we want it to be calculated again from scratch

    // update grab point position and velocity
    NMP::Vector3 grabPosVel = NMP::Vector3(0, 0, 0); // zero for static objects (and only those are cached)
    NMP::Vector3 chestPos = grabDetection.owner->feedIn->getChestState().position + 
      SCALE_TIME(0.2f) * grabDetection.data->smoothedChestVel;
    bufferedEdge.point = projectPointOntoLineSegment(bufferedEdge.corner, bufferedEdge.edge.getNormalised(),
      chestPos, 0.0f, bufferedEdge.edge.magnitude());

    processEdge(bufferedEdge, grabDetection, grabPosVel, pDebugDrawInst);
    if (bufferedEdge.quality > bestEdgeSoFar.quality)
    {
      bestEdgeSoFar = bufferedEdge;
    }

    MR_DEBUG_DRAW_PLANE(
      pDebugDrawInst,
      bufferedEdge.point,
      bufferedEdge.edge.getNormalised(),
      SCALE_DIST(0.2f),
      NMP::Colour(getRainbowColourMapped(bufferedEdge.quality)),
      1.0f);
  }

#define GRABDETECTION_CULL_BUFFERx
#ifdef GRABDETECTION_CULL_BUFFER
  // pop old edges from buffer if they are not grabbable anymore
  const float minBufferQuality = 0.2f;
  grabDetection.data->edgeBuffer.cullLowQuality(minBufferQuality);
#endif
}
#endif //USE_EDGE_BUFFER

//----------------------------------------------------------------------------------------------------------------------
// Compare best edge found by EA system or in buffer against game-specified edges
//----------------------------------------------------------------------------------------------------------------------
static void processGameEdges(
  const GrabDetection& grabDetection, 
  Edge& bestEdgeSoFar, 
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  int numGameEdges = grabDetection.in->calculateNumGameEdges();
  for (int i = 0; i < numGameEdges; i++)
  {
    Edge gameEdge = grabDetection.in->getGameEdges(i);
    gameEdge.gameEdgeID = (int16_t) i;
    processGameEdge(gameEdge, grabDetection, pDebugDrawInst);
    if (gameEdge.quality > bestEdgeSoFar.quality)
    {
      bestEdgeSoFar = gameEdge;
    }
#if defined(MR_OUTPUT_DEBUGGING)
    gameEdge.debugDraw(pDebugDrawInst);
#endif // defined(MR_OUTPUT_DEBUGGING)
  }
}

#undef SCALING_SOURCE
#define SCALING_SOURCE owner->owner->data->dimensionalScaling
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
void GrabDetection::feedback(float NMP_UNUSED(timeStep))
{
}

//----------------------------------------------------------------------------------------------------------------------
// Loop over all patches received from the environment awareness system, as well as those
// in an internal buffer or provided by the game, and identify the best edge for a grab attempt
//----------------------------------------------------------------------------------------------------------------------
void GrabDetection::update(float timeStep)
{
  MR::InstanceDebugInterface* pDebugDrawInst = getRootModule()->getDebugInterface();
  MR_DEBUG_MODULE_ENTER(pDebugDrawInst, getClassName());

  // Make Grab stop updating for a single frame to release the edge because shape that is associated
  // with this edge either was deleted or was filtered out.
  if (feedIn->getIsHanging() && (data->bestEdgeShapeID > 0))
  {
    // Note that data->bestEdgeShapeID is the best edge that was sent to Grab.
    physx::PxShape* const shape = ER::getPxShapeFromShapeID(data->bestEdgeShapeID);
    if (shape)
    {
      if (in->getGrabFilteringImportance() != 0.0f)
      {
        const ObjectFiltering& userFilter = in->getGrabFiltering();
        const physx::PxFilterData userFilterData(userFilter.word0, userFilter.word1, userFilter.word2, userFilter.word3);

        const bool shapeGrabbable = MR::applyFilterShader(shape, &userFilterData);
        if (!shapeGrabbable)
        {
          // Make Grab stop updating for a single frame to release.
          out->setLetGoEdge(true);

#if USE_EDGE_BUFFER
          // Cull all edges that belong to the filtered out shape inside the edge buffer.
          data->edgeBuffer.cullShapeID(data->bestEdgeShapeID);
#endif //USE_EDGE_BUFFER
        }
      }
    }
    else // There is no shape associated with the edge that was last sent to Grab i.e. shape was deleted.
    {
      // Make Grab stop updating for a single frame to release.
      out->setLetGoEdge(true);

#if USE_EDGE_BUFFER
      // Cull all edges that belong to the deleted shape inside the edge buffer.
      data->edgeBuffer.cullShapeID(data->bestEdgeShapeID);
#endif //USE_EDGE_BUFFER
    }
  }

  // Early out when module hasn't received any patch/edge data
  // We've been told not to look for edges, EA hasn't found any, and there are non in our memory
  bool noEAPatches = !feedIn->getUseEnvironmentAwareness() || feedIn->getPotentialGrabPatchesImportance() == 0.0f;
  bool noBufferedEdges = !feedIn->getUseEnvironmentAwareness() || data->edgeBuffer.isEmpty();
  bool noGameEdges = in->getGameEdgesImportance(0) == 0.0f; // assumes it's filled up starting at i=0
  if (noEAPatches && noBufferedEdges && noGameEdges)
  {
    return;
  }

  // Smooth the chest velocity for less erratic prediction of motion
  NMP::Vector3 chestVelNow = owner->feedIn->getChestState().velocity;

  if (data->isFirstUpdate)
  {
    data->smoothedChestVel = chestVelNow;
    data->isFirstUpdate = false;
  }

  float smoothTime = SCALE_TIME(0.3f);
  NMP::smoothExponential(data->smoothedChestVel, timeStep, chestVelNow, smoothTime);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal,
    owner->feedIn->getChestState().position, data->smoothedChestVel, NMP::Colour::WHITE);

  // get best edge from patches returned by environment awareness
  Edge bestEdge;
  bestEdge.quality = 0.0f;

  if (feedIn->getUseEnvironmentAwareness())
  {
    processAllPatches(*this, bestEdge, pDebugDrawInst);
  }

  // Compare best edge found so far against buffered edges
#if USE_EDGE_BUFFER
  if (feedIn->getUseEnvironmentAwareness())
  {
    processBufferedEdges(*this, bestEdge, pDebugDrawInst);
  }
#endif

  // Compare best edge found so far against those provided by the game
  processGameEdges(*this, bestEdge, pDebugDrawInst);

  // Return best edge
  if (bestEdge.quality > 0.0f &&
    (bestEdge.quality > in->getParams().minEdgeQuality ||
     bestEdge.shapeID == -1) // This ensures we carry on passing out user edges when we're already holding them
     )
  {
    // Set edge
    out->setGrabbableEdge(bestEdge, 1.0f);

    // Save shapeID.
    data->bestEdgeShapeID = bestEdge.shapeID;

    // Set look target
    TargetRequest lookTarget;
#if defined(MR_OUTPUT_DEBUGGING)
    lookTarget.debugControlAmounts.setControlAmount(ER::grabControl, 1.0f);
#endif
    // Imminence for the look is the same than the chest's imminence.
    lookTarget.imminence = SCALE_IMMINENCE(6.0f);
    lookTarget.passOnAmount = 0.0f;
    lookTarget.lookOutOfRangeWeight = 0.7f; // Not 1 to avoid head jitters when target totally behind.
    lookTarget.target = bestEdge.point;

    out->setLookTarget(lookTarget, 1.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GrabDetection::entry()
{
  data->isFirstUpdate = true;

  // Determine largest arm length. Used in later calculations that check whether an edge is reachable.
  data->maxArmLength = 0.0f;
  for (uint32_t i = 0; i < owner->owner->data->numArms; i++)
  {
    float armLength = owner->owner->data->baseToEndLength[MyNetworkData::firstArmNetworkIndex + i];
    if (armLength > data->maxArmLength)
    {
      data->maxArmLength = armLength;
    }
  }
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

