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
#include "GameManagement/NoPhysics/GameAnimSystemModuleNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterManagerNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterNoPhysics.h"

#include "morpheme/Prediction/mrPredictionModelNDMesh.h"
//----------------------------------------------------------------------------------------------------------------------

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/ControlWithPrediction.nmb"

// A structure for storing all the information about a prediction model that we are interested in.
struct PredictionModelInfo
{
  const MR::PredictionModelNDMesh* m_model;      // The prediction model definition.
  MR::NodeID                 m_attachedNodeID;   // The NodeID of the node that this model is attached to.
  const char*                m_modelName;        // The prediction model name (unique within the node).
  const char*                m_attachedNodeName; // The full path name of the node that this model is attached to.
};

// For easy indexing in to the g_predictionModelTable table of models.
enum PredictionModelID
{
  MODEL_ID_STOPPING_DISTANCE = 0,
  MODEL_ID_START_LEFT_SPEED_ANGLE,
  MODEL_ID_START_RIGHT_SPEED_ANGLE,
  MODEL_ID_FORWARD_DISTANCE_TURN,
  MODEL_ID_FORWARD_DISTANCE_TURN_LFOOT,
  MODEL_ID_FORWARD_DISTANCE_TURN_RFOOT,
  MODEL_ID_SIMPLE_STOPPING_DISTANCE,
  MAX_MODEL_ID
};

// A table of all the prediction models we are interested in.
// Any uninitialised data is set in findPredictionModels.
PredictionModelInfo g_predictionModelTable[MAX_MODEL_ID] = 
{
  {NULL, MR::INVALID_NODE_ID, "StoppingDistance",         "StateMachine1"},
  {NULL, MR::INVALID_NODE_ID, "StartLeftSpeedAngle",      "StateMachine1"},
  {NULL, MR::INVALID_NODE_ID, "StartRightSpeedAngle",     "StateMachine1"},
  {NULL, MR::INVALID_NODE_ID, "ForwardDistanceTurn",      "StateMachine1|Forwards|ScatterBlend1"},
  {NULL, MR::INVALID_NODE_ID, "ForwardDistanceTurnLFoot", "StateMachine1|Forwards|ScatterBlend1"},
  {NULL, MR::INVALID_NODE_ID, "ForwardDistanceTurnRFoot", "StateMachine1|Forwards|ScatterBlend1"},
  {NULL, MR::INVALID_NODE_ID, "SimpleStoppingDistance",   "StateMachine1|Forwards_To_Idle|BlendN"}
};

// Prediction query result options.
const char* g_queryResultStringTable[3] = 
{
  "INTERPOLATION",  // The NDMesh prediction model query was successful and the query point was inside the 
                    //  sample region.
  "PROJECTION",     // The NDMesh prediction model query was successful but the query point was outside the
                    //  sample region and had to be projected back on to it.
  "FAILED"          // The NDMesh prediction model query was unsuccessful. The query point was outside the
                    //  sample region and could not projected back on to it successfully. This could be because
                    //  we asked the query not to do a projection.
};

//----------------------------------------------------------------------------------------------------------------------
// Display a bunch of information about each prediction model in a PredictionModelAnimSetGroup.
void displayPredictionModelDetails(const MR::PredictionModelAnimSetGroup* animSetGroup)
{
  const uint32_t maxNumComponents = 32;
  float aabbMinVals[maxNumComponents];
  float aabbMaxVals[maxNumComponents];
  float sampleCentre[maxNumComponents];

  for (MR::AnimSetIndex i = 0; i < animSetGroup->getNumAnimSets(); ++i)
  {
    NMP_STDOUT("      AnimSetIndex: %d", i);
    const MR::PredictionModelDef* model = animSetGroup->getPredictionModelForAnimSet(i);

    if (model)
    {
      if(model->getType() == PREDICTMODELTYPE_ND_MESH)
      {
        NMP_STDOUT("       Type: PredictionModelNDMesh");
        const MR::PredictionModelNDMesh* modelNDMesh = animSetGroup->getPredictionModelForAnimSet<MR::PredictionModelNDMesh>(i);
        NMP_STDOUT("       Num dimensions: %d", modelNDMesh->getNumDimensions());

        NMP_STDOUT("       Num samples per dimension:");
        const uint32_t* sampleCountsPerDimension = modelNDMesh->getSampleCountsPerDimension();
        for(uint32_t j = 0; j < modelNDMesh->getNumDimensions(); ++j)
        {
          NMP_STDOUT("           Dimension %d = %d", j, sampleCountsPerDimension[j]);
        }
        NMP_STDOUT("       Total num samples: %d", modelNDMesh->getNumSamples());

        NMP_STDOUT("       Num components per sample: %d", modelNDMesh->getNumComponentsPerSample());
        NMP_ASSERT(modelNDMesh->getNumComponentsPerSample() < maxNumComponents);

        NMP_STDOUT("       Component ranges:");
        modelNDMesh->getSampleAABB(modelNDMesh->getNumComponentsPerSample(), aabbMinVals, aabbMaxVals);
        for(uint32_t j = 0; j < modelNDMesh->getNumComponentsPerSample(); ++j)
        {
          NMP_STDOUT("           Component %d: min = %f, max = %f", j, aabbMinVals[j], aabbMaxVals[j]);
        }

        NMP_STDOUT("       Sample Centre:");
        modelNDMesh->getSampleCentre(modelNDMesh->getNumComponentsPerSample(), sampleCentre);
        for(uint32_t j = 0; j < modelNDMesh->getNumComponentsPerSample(); ++j)
        {
          NMP_STDOUT("           Component %d = %f", j, sampleCentre[j]);
        }
      }
      else
      {
        NMP_STDOUT("       Cannot display prediction model info for model type ID: %d", model->getType());
      }
    }
    else
    {
      NMP_STDOUT("       No prediction model")
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Display some information about the contents of the loaded NetworkPredictionDef library.
void displayNetworkPredictionLibraryInfo(
  MR::NetworkDef*           netDef, 
  MR::NetworkPredictionDef* netPredictionDef)
{
  NMP_STDOUT("\n--------------------------------------------------");
  NMP_STDOUT("displayNetworkPredictionLibraryInfo");
  NMP_STDOUT("--------------------------------------------------");

  // Display some information from the loaded NetworkPredictionDef library.
  NMP_STDOUT("Number of nodes with prediction models: %d", netPredictionDef->getNumNodesWithModels());
  for (uint32_t i = 0; i < netPredictionDef->getNumNodesWithModels(); ++i)
  {
    const MR::PredictionNodeModels* nodeModels = netPredictionDef->getNodeEntry(i);
    const char* nodeName = netDef->getNodeNameFromNodeID(nodeModels->getNodeID());
    NMP_STDOUT("  %d: Node: %s ID: %d", i, nodeName, nodeModels->getNodeID());

    // Display information about the content of each node entry.
    for (uint32_t j = 0; j < nodeModels->getNumPredictionModelAnimSetGroups(); ++j)
    {
      const MR::PredictionModelAnimSetGroup* modelAnimSetGroup = nodeModels->getPredictionModelAnimSetGroup(j);
      NMP_STDOUT("    Model: %s", modelAnimSetGroup->getName());
      displayPredictionModelDetails(modelAnimSetGroup);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void findPredictionModels(
  MR::NetworkDef*           netDef, 
  const MR::NetworkPredictionDef* netPredictionDef)
{
  for (uint32_t i = 0; i < MAX_MODEL_ID; ++i)
  {
    g_predictionModelTable[i].m_attachedNodeID = netDef->getNodeIDFromNodeName(g_predictionModelTable[i].m_attachedNodeName);
    g_predictionModelTable[i].m_model = netPredictionDef->findNamedPredictionModelForAnimSet<MR::PredictionModelNDMesh>(
                  g_predictionModelTable[i].m_attachedNodeID, // The NodeID of the node that this model is attached to.
                  g_predictionModelTable[i].m_modelName,      // The prediction model name (unique within the node).
                  0);                                         // The animation set of the prediction model we are looking for.
  }
}

//----------------------------------------------------------------------------------------------------------------------
void displayStoppingDistancePredictionInfo(
  const MR::PredictionModelNDMesh* model,
  const char*                modelName)
{
  // Note when querying a PredictionModelNDMesh it is important to understand a little bit about how the sample data is stored internally.
  //  An NDMesh model is a regularly sampled N-dimensional mesh, whose vertex samples contain both control parameters and motion analysis
  //  parameters. The NDMesh stores the components of each vertex sample in the order in which they were specified in connect;
  //  control parameters followed by motion analysis parameters.
  //
  //  The sample components order on this prediction model = (Speed CP, Distance traveled).

  //-----------------------
  // Lets find and display the minimum and maximum stopping distances.
  uint32_t independentVarIndices[1] = {1};
  float independentVars[1] = {0.0f};
  float aabbComponentMinVals[1];
  float aabbComponentMaxVals[1];
  model->getSampleAABB(1, independentVarIndices, aabbComponentMinVals, aabbComponentMaxVals);
  float minStoppingDist = aabbComponentMinVals[0];
  float maxStoppingDist = aabbComponentMaxVals[0];
  NMP_STDOUT("%s prediction model query info", modelName);
  NMP_STDOUT("    Minimum travel distance: %f", minStoppingDist);
  NMP_STDOUT("    Maximum travel distance: %f", maxStoppingDist);

  //-----------------------
  // Lets find and display the corresponding minimum and maximum speed control parameters.
  independentVarIndices[0] = 0;
  independentVars[0] = 0.0f;
  model->getSampleAABB(1, independentVarIndices, aabbComponentMinVals, aabbComponentMaxVals);
  float minSpeedControlParam = aabbComponentMinVals[0];
  float maxSpeedControlParam = aabbComponentMaxVals[0];
  NMP_STDOUT("    Minimum speed control param input: %f", minSpeedControlParam);
  NMP_STDOUT("    Maximum speed control param input: %f", maxSpeedControlParam);


  //-----------------------
  // Lets display how stopping distance varies across the range of the speed control parameter.
  NMP_STDOUT("  How stopping distance varies across the range of the speed control parameter:");
  float components[2];
  uint32_t numSamples = 6;
  MR::ScatteredData::InterpolationReturnVal result;
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    // Set the speed control parameter component value at regular increments across its range.
    components[0] = minSpeedControlParam + (((float)i) * ((maxSpeedControlParam - minSpeedControlParam) / (((float)numSamples) - 1.0f)));

    // Query the model. 
    //  Note: Looking up analysed parameters from control parameters is fast.
    result = model->calculateAPsFromCPs(2, components);
    NMP_STDOUT("    Speed CP: %f, Position Delta Distance: %f, generated via: %s", components[0], components[1], g_queryResultStringTable[result]);
  }
  // Look at the values displayed; notice the speed control parameter linearly increasing 
  //  does not necessarily produce linear change in stopping distance.
  //  The prediction model query functions deal with this non linear relationship for you.

  
  //-----------------------
  // Lets display how the speed control parameter varies across the range of the stopping distance.
  NMP_STDOUT("  How the speed control parameter varies across the range of the stopping distance:");
  independentVarIndices[0] = 1;
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    // Set the stopping distance value at regular increments across its range.
    components[1] = minStoppingDist + (((float)i) * ((maxStoppingDist - minStoppingDist) / (((float)numSamples) - 1.0f)));

    // Query the model. 
    //  Note: This lookup direction is the inverse to calculateAPsFromCPs.
    //        Looking up control parameters from analysed parameters is slower because the model has to do more work.
    result = model->calculateCPsFromAPs(2, components, 1, independentVarIndices);
    NMP_STDOUT("    Position Delta Distance: %f, Speed CP: %f, generated via: %s", components[1], components[0], g_queryResultStringTable[result]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void simpleStoppingDistancePrediction()
{
  NMP_STDOUT("\n--------------------------------------------------");
  NMP_STDOUT("simpleStoppingDistancePrediction");
  NMP_STDOUT("--------------------------------------------------");

  // This is a simple prediction model.
  //  It is attached to the StateMachine1|Forwards_To_Idle|BlendN node.
  //  It is a 1 dimensional NDMesh model that maps the SpeedControlParameter to the distance traveled, through the period sync event 2 to 5.
  //  We have chosen 2 to 5 because this is the period through which this bridging state is active.
  //  This model provides a good estimation of stopping distance.
  const MR::PredictionModelNDMesh* model = g_predictionModelTable[MODEL_ID_SIMPLE_STOPPING_DISTANCE].m_model;

  displayStoppingDistancePredictionInfo(model, "SimpleStoppingDistance"); 
}

//----------------------------------------------------------------------------------------------------------------------
void accurateStoppingDistancePrediction()
{
  NMP_STDOUT("\n--------------------------------------------------");
  NMP_STDOUT("accurateStoppingDistancePrediction");
  NMP_STDOUT("--------------------------------------------------");

  // At runtime this prediction model is no more complex than the SimpleStoppingDistance prediction model.
  //  The differences are from what source network information the model has been created.
  //  The main difference is that this prediction model spans 3 different states, It includes:
  //    1. The transition from Forwards to ForwardsToIdle.
  //    2. The ForwardsToIdle bridging state.
  //    3. The transition from ForwardsToIdle to Idle.
  //  It is attached to the StateMachine1 node.
  //  It is also a 1 dimensional NDMesh model that maps the SpeedControlParameter to the distance traveled. 
  //  The period measured is:
  //    From when the transition from Forwards to ForwardsToIdle starts.
  //    To when the transition from ForwardsToIdle to Idle ends.
  //  This model should give us a more accurate estimation of stopping distance because it includes the 
  //  transition that are fired when stopping.
  const MR::PredictionModelNDMesh* model = g_predictionModelTable[MODEL_ID_STOPPING_DISTANCE].m_model;

  // Lets display the same information that we did for SimpleStoppingDistance so that we can compare them.
  displayStoppingDistancePredictionInfo(model, "StoppingDistance");   
}

//----------------------------------------------------------------------------------------------------------------------
void displayStartSpeedAndDirectionQueryResults(
  float*                                    components, 
  MR::ScatteredData::InterpolationReturnVal queryResult)
{
  NMP_STDOUT("    Generated via: %s", g_queryResultStringTable[queryResult]);
  NMP_STDOUT("    0. Speed CP: %f", components[0]);
  NMP_STDOUT("    1. InitialHeading CP (degrees): %f", components[1]);
  NMP_STDOUT("    2. Speed: %f", components[2]);
  NMP_STDOUT("    3. RotationDelta about Y (vertical axis) (rads): %f", components[3]);
  NMP_STDOUT("    4. Position Delta in Z (the total offset in Z from the starting position): %f", components[4]);
  NMP_STDOUT("    5. Position Delta in X (the total offset in X from the starting position): %f", components[5]);
}

//----------------------------------------------------------------------------------------------------------------------
void startSpeedAndDirectionPrediction()
{
  NMP_STDOUT("\n--------------------------------------------------");
  NMP_STDOUT("startSpeedAndDirectionPrediction");
  NMP_STDOUT("--------------------------------------------------");

  //-----------------------
  // This model handles starting when turning to the right.
  //  It is a 2 dimensional NDMesh model.
  //  It is attached to the StateMachine1 node.
  //  The prediction model spans 3 different states, It includes:
  //    1. The transition from Idle to Idle_To_ForwardsRight.
  //    2. The Idle_To_ForwardsRight bridging state (Leading with the right foot and turning by up to 180 degrees to the right).
  //    3. The transition from Idle_To_ForwardsRight to Forwards.
  //
  //  The sample components order on this prediction model are:
  //   0. Speed CP
  //   1. InitialHeading CP (degrees)
  //   2. Speed (At end of analysis)
  //   3. RotationDelta about Y (vertical axis) (rads)
  //   4. Position Delta in Z (the total offset in Z from the starting position)
  //   5. Position Delta in X (the total offset in X from the starting position)
  //
  //  This model creates a bidirectional map:
  //   From |  X 0. Speed CP                     To | X 2. Speed
  //        |  Y 1. InitialHeading CP (degrees)     | Y 3. RotationDelta about Y (vertical axis)
  // 
  //  However because we have also sampled Position Delta in Z and X we can also predict where we will be
  //   after the transition has completed.
  const MR::PredictionModelNDMesh* model = g_predictionModelTable[MODEL_ID_START_RIGHT_SPEED_ANGLE].m_model;
  
  //-----------------------
  // Lets find out what control parameters I need to set in order to achieve a specific speed and start rotation.
  float components[6];
  components[2] = 3.5f;  // Speed of 3.5 ms.
  components[3] = -1.0f; // Start rotation of -1.0 radians. (0 = forwards down the character local z)
  uint32_t independentVarIndices[2] = {2, 3};     // The components that we want to map from.
  MR::ScatteredData::InterpolationReturnVal result =
                        model->calculateCPsFromAPs(6, components, 2, independentVarIndices);
  NMP_STDOUT("%s prediction model query info", g_predictionModelTable[MODEL_ID_START_RIGHT_SPEED_ANGLE].m_modelName);
  NMP_STDOUT("  For speed 3.5 and start rotation -1.0 we find the following results");
  displayStartSpeedAndDirectionQueryResults(components, result);
 
  // This model allows us to query what control parameters need to be set in order to achieve a 
  //  specific speed and start direction. In addition it tells us where we will be after we have finished the
  //  full transition from Idle to Forwards.


  //-----------------------
  // Lets make the same query of the opposite model that handles starting turning left.
  model = g_predictionModelTable[MODEL_ID_START_LEFT_SPEED_ANGLE].m_model;
  components[2] = 3.5f; // Speed of 3.5 ms.
  components[3] = 1.0f; // Start rotation of 1.0 radians. (0 = forwards down the character local z)
  result = model->calculateCPsFromAPs(6, components, 2, independentVarIndices);
  NMP_STDOUT("%s prediction model query info", g_predictionModelTable[MODEL_ID_START_LEFT_SPEED_ANGLE].m_modelName);
  NMP_STDOUT("  For speed 3.5 and start rotation 1.0 we find the following results");
  displayStartSpeedAndDirectionQueryResults(components, result);

  // Compare the result from the two model queries above:
  //  Note that they are different because the source animations for the modeled states are different.
  //  If they were mirrored however then the results should be the same.

  //-----------------------
  // Note : 
  //  That because we have also sampled Position Delta in Z and X it is possible to query 2 more bidirectional maps:
  //  From |  X 0. Speed CP                     To | X 4. Position Delta in Z (the total offset in Z from the starting position)
  //       |  Y 1. InitialHeading CP (degrees)     | Y 5. Position Delta in X (the total offset in X from the starting position)
  //  and
  //  From | X 2. Speed                                  To | X 4. Position Delta in Z (the total offset in Z from the starting position)
  //       | Y 3. RotationDelta about Y (vertical axis)     | Y 5. Position Delta in X (the total offset in X from the starting position)
  //
  // Any map only produces meaningful query results when they are well defined.
  //  Badly defined maps are ones where cells on any side of the map overlap or self intersect.
}

//----------------------------------------------------------------------------------------------------------------------
void queryingPredictionModelsOutsideOfTheSampleRegion()
{
  NMP_STDOUT("\n--------------------------------------------------");
  NMP_STDOUT("queryingPredictionModelsOutsideOfTheSampleRegion");
  NMP_STDOUT("--------------------------------------------------");

  //-----------------------
  // In this example we are querying the NDMesh prediction model StartLeftSpeedAngle
  // see startSpeedAndDirectionPrediction for a description.

  //------------------------------------------------
  // Results from NDMesh prediction model queries.
  //------------------------------------------------
  // When you query an NDMesh model with calculateCPsFromAPs or calculateAPsFromCPs there are 3 possible results:
  //  1. INTERPOLATION_SUCCESS
  //     The NDMesh prediction model query was successful and the query point was inside the 
  //     sample region.
  //  2. INTERPOLATION_PROJECTED
  //     The NDMesh prediction model query was successful but the query point was outside the
  //     sample region and had to be projected back on to it.
  //  3. INTERPOLATION_FAILED
  //     The NDMesh prediction model query was unsuccessful. The query point was outside the
  //     sample region and could not projected back on to it successfully. This could also be because
  //     we asked the query not to do a projection.


  //------------------------------------------------
  // Query points that lie within the sample space.
  //------------------------------------------------
  // Lets query the StartLeftSpeedAngle model well within its sample area.
  //
  //  The sample components order on this prediction model are:
  //   0. Speed CP
  //   1. InitialHeading CP (degrees)
  //   2. Speed (At end of analysis)
  //   3. RotationDelta about Y (vertical axis) (rads)
  //   4. Position Delta in Z (the total offset in Z from the starting position)
  //   5. Position Delta in X (the total offset in X from the starting position)
  const MR::PredictionModelNDMesh* model = g_predictionModelTable[MODEL_ID_START_LEFT_SPEED_ANGLE].m_model;
  float components[6];
  components[2] = 3.5f;  // Speed of 3.5 ms.
  components[3] = 0.0f;  // Start rotation of 0.0 radians. (0 = forwards down the character local z)
  uint32_t independentVarIndices[2] = {2, 3};     // The components that we are wanting to map from.
  MR::ScatteredData::InterpolationReturnVal result = model->calculateCPsFromAPs(6, components, 2, independentVarIndices);
  NMP_STDOUT("%s prediction model query info", g_predictionModelTable[MODEL_ID_START_LEFT_SPEED_ANGLE].m_modelName);
  NMP_STDOUT("  Querying within the sample area:");
  NMP_STDOUT("   For speed 3.5 and start rotation 0.0 we find the following results");
  displayStartSpeedAndDirectionQueryResults(components, result);
  // Note that the result is INTERPOLATION_SUCCESS,
  // and because the speed and angle are within the capabilities of this blend the input query values do not change
  // and the calculated values are based on this input.


  //------------------------------------------------
  // Query points that lie outside the sample space.
  //------------------------------------------------
  // How do we deal with a query point that lies outside the sample space.
  // In all our calls to calculateCPsFromAPs so far we don't, it would return INTERPOLATION_FAILED.
  // This is because by default it does not try to do any projection back on to the sample space.
  // This can be the desired behavior if the user wants to take control of making sure all the query points are within the sample area.
  //
  // However there are 2 ways in which projection can be performed internally:
  //  1. ProjectAlongDimension
  //     With this method the user specifies which independent variables to change, in which order.
  //     You do this by providing an orderOfVariablesSacrifice array.
  //  2. ProjectThroughPoint
  //     A ray is projected from the query point to another specified point and the first intersection with
  //     the edge of the sample space is returned as the resulting query point. If there is no intersection
  //     INTERPOLATION_FAILED is returned.


  // 1. ProjectAlongDimension
  // -------------------------------
  // Lets query the StartLeftSpeedAngle model with a speed that is well outside of its capabilities.
  components[2] = 10.0f; // Speed of 10.0 ms.
  components[3] = 0.0f;  // Start rotation of 0.0 radians. (0 = forwards down the character local z)
  uint32_t orderOfVariableSacrifice[2];
  orderOfVariableSacrifice[0] = 0;  // When projecting, sacrifice the speed component first (cast a ray down the speed axis and see if there is an intersection).
  orderOfVariableSacrifice[1] = 1;  // When projecting, sacrifice the rotation component second (cast a ray down the rotation axis and see if there is an intersection).
  MR::ScatteredData::Projection projectionParams(2, orderOfVariableSacrifice); // Create the projection description structure to send to the query function.
  result = model->calculateCPsFromAPs(6, components, 2, independentVarIndices, true, &projectionParams);
  NMP_STDOUT("  Querying with, Speed outside the sample area, Projection in the speed axis then the rotation axis:");
  NMP_STDOUT("   For speed 10.0 and start rotation 0.0, with projection in axes, we find the following results");
  displayStartSpeedAndDirectionQueryResults(components, result);
  // Because the speed is well beyond the capabilities of this blend the prediction model has projected the sample point
  //  along the speed axis to find the nearest intersection with the sample space.
  //  This results in the input and output components changing in the result.
  //  The INTERPOLATION_PROJECTED result is telling you, that the desired target cannot be achieved but this is the closest I can get.
  //  If after projecting in both speed and rotation axes there is no intersection, the query will return INTERPOLATION_FAILED.
  //
  // Projection along specified dimensions is important when you want to prioritise one dimension over another.
  // For example, I need to achieve the specified rotation, but I don't mind so much if the speed is scaled.
  
  //-----------------------
  // Lets try the same thing but with the query speed and rotation both being well out side of their sample ranges.
  components[2] = 10.0f; // Speed of 10.0 ms.
  components[3] = 5.0f;  // Start rotation of 5.0 radians. (0 = forwards down the character local z)
  orderOfVariableSacrifice[0] = 0;  // When projecting, sacrifice the speed component first (cast a ray down the speed axis and see if there is an intersection).
  orderOfVariableSacrifice[1] = 1;  // When projecting, sacrifice the rotation component second (cast a ray down the rotation axis and see if there is an intersection).
  result = model->calculateCPsFromAPs(6, components, 2, independentVarIndices, true, &projectionParams);
  NMP_STDOUT("  Querying with, Speed and Rotation outside the sample area, Projection in the speed axis then the rotation axis:");
  NMP_STDOUT("   For speed 10.0 and start rotation 5.0, with projection in axes, we find the following results");
  displayStartSpeedAndDirectionQueryResults(components, result);
  // Note that the result is INTERPOLATION_FAILED. This is because the query point cannot be moved back in to the sample space
  //  by projecting in either the speed or rotation axes.
  //  To help prevent this from occurring, when using project in dimension, limit your query point to lie within the sample space bounding box.
  //  This will guarantee that projection will succeed. (Unless the sample space is broken in to sub sections)


  // 2. ProjectThroughPoint
  // -------------------------------
  // Lets repeat the same query as above (Speed of 10.0 ms and Start rotation of 5.0 radians) but rather that projecting along
  // dimension axes lets project through a chosen point.
  components[2] = 10.0f; // Speed of 10.0 ms.
  components[3] = 5.0f;  // Start rotation of 5.0 radians. (0 = forwards down the character local z)
  float projectionCentre[2] = {0.0f, 0.0f}; // The point to cast the projection ray to, is needed.
  model->getSampleCentre(2, projectionCentre, independentVarIndices); // Get the center of the sample area for our projection point.
                                                                      //  This makes projection very likely to succeed.
  projectionParams.init(2, projectionCentre);
  result = model->calculateCPsFromAPs(6, components, 2, independentVarIndices, true, &projectionParams);
  NMP_STDOUT("  Querying outside the sample area with projection to the centre of, speed 0.0, rotation 0.0:");
  NMP_STDOUT("   For speed 10.0 and start rotation 5.0, with projection to point, we find the following results");
  displayStartSpeedAndDirectionQueryResults(components, result);
  // Note that this time the result is INTERPOLATION_PROJECTED.
  //  This method of projection is more robust that projection along axes.
  //  In fact it is fool proof as long as the projection point is within the sample space.
}

//----------------------------------------------------------------------------------------------------------------------
void displaySpeedBankQueryResults(
  float*                                    components, 
  MR::ScatteredData::InterpolationReturnVal queryResult)
{
  NMP_STDOUT("    Generated via: %s", g_queryResultStringTable[queryResult]);
  NMP_STDOUT("    0. Speed CP: %f", components[0]);
  NMP_STDOUT("    1. Banking CP: %f", components[1]);
  NMP_STDOUT("    2. Position Delta Distance: %f", components[2]);
  NMP_STDOUT("    3. Rate Of Turn With Forward Travel (rads/m): %f", components[3]);
}

//----------------------------------------------------------------------------------------------------------------------
// Querying a 2 dimensional prediction model to control a character when following a curved path.
void controlForFollowingACurvedPath()
{
  NMP_STDOUT("\n--------------------------------------------------");
  NMP_STDOUT("controlForFollowingACurvedPath");
  NMP_STDOUT("--------------------------------------------------");

  //-----------------------
  // This model allows us to accurately control speed and rate of turn in order to follow a chosen curving path.
  //  It is a 2 dimensional NDMesh model.
  //  It is authored on the state machine state StateMachine1|Forwards in Connect,
  //   but because blend tree containers do not actually get instanced at runtime it is attached to the root node
  //   of the blend tree, which in this case is StateMachine1|Forwards|ScatterBlend1.
  //  It models the behavior of the trajectory in the Forwards state.
  //
  //  The sample components order on this prediction model are:
  //   0. Speed CP
  //   1. Banking CP
  //   2. Position Delta Distance
  //   3. Rate Of Turn With Forward Travel 
  //
  //  This model creates a bidirectional map:
  //   From |  X 0. Speed CP    To | X 2. PositionDeltaDistance
  //        |  Y 1. Banking CP     | Y 3. RateOfTurnWithForwardTravel (radians/m)
  const MR::PredictionModelNDMesh* model = g_predictionModelTable[MODEL_ID_FORWARD_DISTANCE_TURN].m_model;
  NMP_STDOUT("%s prediction model query info", g_predictionModelTable[MODEL_ID_FORWARD_DISTANCE_TURN].m_modelName);
  
  // Lets assume we are trying to archive a specific speed and rate of turn in order to stay on our path.
  // Some tighter rates of turn preclude some higher (and in fact some lower) speeds.
  // In this situation achieving the correct rate of turn is more important to us than accurate speed.
  // This ensures that we stay as close to the path as possible.
  //
  // When we are outside of the sample region we will chose to project in axes rather than projecting to a point
  // This allows us to keep the required rate of turn.
  

  //-----------------------
  // Lets find the minimum and maximum ranges for both axes (PositionDeltaDistance,RateOfTurnWithForwardTravel ) 
  // so we can ensure that any query points can always be projected on to the sample space.
  uint32_t independentVarIndices[2] = {2, 3};
  float aabbComponentMinVals[2];
  float aabbComponentMaxVals[2];
  model->getSampleAABB(2, independentVarIndices, aabbComponentMinVals, aabbComponentMaxVals);
  float minPositionDeltaDistance = aabbComponentMinVals[0];
  float maxPositionDeltaDistance = aabbComponentMaxVals[0];
  float minRateOfTurnWithForwardTravel = aabbComponentMinVals[1];
  float maxRateOfTurnWithForwardTravel = aabbComponentMaxVals[1];


  //-----------------------
  // Lets make a query that we know will lie within the sample space.
  float components[4];
  components[2] = (maxPositionDeltaDistance + minPositionDeltaDistance) * 0.5f;              // Middle of range travel distance.
  components[3] = (maxRateOfTurnWithForwardTravel + minRateOfTurnWithForwardTravel) * 0.5f;  // Middle of range rate of turn with forward travel.
  NMP_STDOUT("  Querying within the sample area:");
  NMP_STDOUT("    Travel distance = %f, Rate of turn with forward travel = %f", components[2], components[3]);
  uint32_t orderOfVariableSacrifice[2];
  orderOfVariableSacrifice[0] = 0;  // When projecting, sacrifice the travel distance component first (cast a ray down the speed axis and see if there is an intersection).
  orderOfVariableSacrifice[1] = 1;  // When projecting, sacrifice the rate of turn with forward travel component second (cast a ray down the rotation axis and see if there is an intersection).
                                    //  Projection in the 2nd axis should never happen because we will make sure that our query point lies
                                    //  within the travel distance range.
  MR::ScatteredData::Projection projectionParams(2, orderOfVariableSacrifice); // Create the projection description structure to send to the query function.
  MR::ScatteredData::InterpolationReturnVal result = model->calculateCPsFromAPs(4, components, 2, independentVarIndices, true, &projectionParams);
  displaySpeedBankQueryResults(components, result);

  //-----------------------
  // Lets make a query that we know will lie within the bounding box of the sample area,
  //  but actually outside the sample area itself (Found by examining the analysis results in Connect).
  NMP_STDOUT("  Querying outside the sample area, but within the sample area bounding box:");
  components[2] = 3.0f;  // A high travel distance.
  components[3] = 2.25f; // a high range rate of turn with forward travel.
  NMP_STDOUT("    Travel distance = %f, Rate of turn with forward travel = %f", components[2], components[3]);
  result = model->calculateCPsFromAPs(4, components, 2, independentVarIndices, true, &projectionParams);
  displaySpeedBankQueryResults(components, result);
  // Note how the requested travel distance has changed, but the requested rate of turn with forward travel remains relatively unchanged.


  //-----------------------
  // Finding a components range given fixed values in the remaining components.
  //
  //  For example, from the example above, given a desired range rate of turn with forward travel,
  //  what would the minimum and maximum values for travel distance be?
  float independentVarValues[2] = {0.0f, 2.25f}; // (travel distance, rate of turn with forward travel).
  float minTravelDist;
  float maxTravelDist;
  model->calculateRange(2, independentVarIndices, independentVarValues, 0, minTravelDist, maxTravelDist);
  NMP_STDOUT("  Given the fixed desired range rate of turn: %f, the min and max travel distance values are:", independentVarValues[1]);
  NMP_STDOUT("    Minimum travel distance: %f", minTravelDist);
  NMP_STDOUT("    Maximum travel distance: %f", maxTravelDist);
  // calculateRange queries provide finer range information than getSampleAABB, which can be very useful when planning motion.

  //-----------------------
  // Lets find the travel distance range for a less extreme rate of turn with forward travel.
  independentVarValues[0] = 0.0f;
  independentVarValues[1] = 0.2f;
  model->calculateRange(2, independentVarIndices, independentVarValues, 0, minTravelDist, maxTravelDist);
  NMP_STDOUT("  Given the fixed desired range rate of turn: %f, the min and max travel distance values are:", independentVarValues[1]);
  NMP_STDOUT("    Minimum travel distance: %f", minTravelDist);
  NMP_STDOUT("    Maximum travel distance: %f", maxTravelDist);
  // Notice how the range of travel distance has markedly increased with the reduction in rate of turn with forward travel.
}

//----------------------------------------------------------------------------------------------------------------------
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // also be loaded from this location; see Game::CharacterManager::requestAnim() for more details.
  NMP_STDOUT("Defaulting <network1 filename> to : '%s'", NETWORK_DEF_BUNDLE_NAME);

  //----------------------------
  // Game::CharacterManagerNoPhysics which will act as a manager for all the morpheme network definitions 
  // and network instances. Initialise will set up the morpheme libraries and load all the default node types
  // and other default functions.
  Game::CharacterManagerNoPhysics characterManager;

  //---------------------------
  // Initialize morpheme.
  Game::AnimSystemModuleNoPhysics::init();
  characterManager.init();

  //----------------------------
  // We also need an instance of a Game::CharacterDef that will be initialised with the binary asset data. Using the
  // Game::CharacterManager manager class we will initialise our CharacterDef. This will load the file found at 
  // ASSETLOCATION and use that information to apply to the character definition instance.
  // This also registers the definition with our manager. Management of this definitions memory is the responsibility of the characterManager.
  NMP_STDOUT("\nCreating GameCharacterDef:");
  Game::CharacterDef* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME);
  if(!gameCharacterDef)
  {
    NMP_STDOUT("\nError: Failed to create Character Definition");
    return NULL;
  }
  
  //----------------------------
  // Store a reference to the NetworkDef loaded by this Game::CharacterDef.
  MR::NetworkDef* netDef = gameCharacterDef->getNetworkDef();
      
  //----------------------------
  // Get a handle to the NetworkPredictionDef library that was loaded on our game character.
  // This structure stores all NetwortPredictionModels that have been created against this network.
  // It is stored as a separate asset in the simple bundle.
  // It can be loaded and used independently of any other asset.
  MR::NetworkPredictionDef* netPredictionDef = gameCharacterDef->getNetworkPredictionDef();  
  if(!netPredictionDef)
  {
    NMP_STDOUT("\nError: A NetworkPredictionDef library was not loaded");
    return NULL;
  }

  //------------------------------------------------
  // Display some information about the loaded NetworkPredictionDef library.
  //------------------------------------------------
  displayNetworkPredictionLibraryInfo(netDef, netPredictionDef);
  

  //------------------------------------------------
  // Access the prediction models library and find all of the prediction models that we want to query.
  // This fills in the g_predictionModelTable, finding and storing all the prediction models we are interested in.
  //------------------------------------------------
  findPredictionModels(netDef, netPredictionDef);
   
  //------------------------------------------------
  // Here are several separate examples of how prediction models can be used.
  //
  // Note that we do not need to instantiate a Game::Character or load Animations or Rigs in 
  //  order to querying the models. This means that we can, very usefully, 
  //  query Network capabilities at any point independent of any other asset.
  //------------------------------------------------

  // Querying a very simple 1 dimensional model for stopping distance prediction.
  simpleStoppingDistancePrediction();

  // Querying a more accurate 1 dimensional model for stopping distance prediction.
  accurateStoppingDistancePrediction();

  // Querying a 2 dimensional prediction model for start direction and speed control.
  startSpeedAndDirectionPrediction();

  // What happens when we query a prediction model outside of the sample region.
  queryingPredictionModelsOutsideOfTheSampleRegion();
    
  // Querying a 2 dimensional prediction model to control a character when following a curved path.
  controlForFollowingACurvedPath();
    
  
  //----------------------------
  // When we are ready to exit we can terminate Game::CharacterManager to free any classes/memory it has stored.
  // Since Game::CharacterManager stores a pointer to the characterDef and gameCharacter instances it will
  // manage their termination as well.
  NMP_STDOUT("Releasing data:");
  characterManager.term();
  Game::AnimSystemModuleNoPhysics::term();
  NMP_STDOUT("Release complete");

  NMP_STDOUT("\nPress Enter to Exit");
  getchar();

  return 0;
}
//----------------------------------------------------------------------------------------------------------------------
