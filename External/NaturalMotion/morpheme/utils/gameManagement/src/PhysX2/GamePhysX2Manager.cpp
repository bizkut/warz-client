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
#include "GameManagement/PhysX2/GamePhysX2Manager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

/// Some default initialisation values for PhysX.
const NxReal g_physXSkinWidth = 0.001f;
const NxReal g_physXVisualizationScale = 0.25f;
const NxReal g_physXEnableAdaptiveForce = 0;
const NxReal g_physXInitEnableImprovedSpringSolver = 1;

/// Frequency of internal sub step update for the PhysX2 global simulation step. 
///  One PhysX simulation update can perform many internal sub step updates at this frequency.
uint32_t g_physXSubStepUpdateFrequency = 60;    

/// The PhysX2 max duration of each simulation internal sub-step.
/// The smaller the sub-step the greater the simulation accuracy, but also the greater the processing required.
float    g_physXMaxSubStepInterval     = 1.0f / g_physXSubStepUpdateFrequency; 

/// The maximum number of internal sub-steps to be performed in one simulation step.
uint32_t g_physXMaxNumXSubSteps        = 100;

//----------------------------------------------------------------------------------------------------------------------
// PhysX2Manager functions.
//----------------------------------------------------------------------------------------------------------------------
void PhysX2Manager::init(
#ifdef NM_HOST_CELL_PPU
  CellSpurs* spursObject
#endif // NM_HOST_CELL_PPU
  )
{
  // Make sure we were not already initialised.
  NMP_ASSERT(!m_physXSDK && !m_physXCharacterControllerManager && !m_physXScene);

  // Initialize PhysX SDK.
  NxPhysicsSDKDesc sdkDesc;
  sdkDesc.flags |= NX_SDKF_NO_HARDWARE;
  m_physXSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, &m_physXOutputStream, sdkDesc);
  NMP_ASSERT_MSG(
            m_physXSDK,
            "Unable to create the NVIDIA PhysX SDK. "
            "The NVIDIA PhysX System software is required to run this app.");

  // Connect to the remote debugger, if there is one.
  if (!m_physXSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
  {
    m_physXSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);
  }

  // Set some default values on this instance of the SDK.
  m_physXSDK->setParameter(NX_SKIN_WIDTH, g_physXSkinWidth);                                    
  m_physXSDK->setParameter(NX_VISUALIZATION_SCALE, g_physXVisualizationScale);                  
  m_physXSDK->setParameter(NX_ADAPTIVE_FORCE, g_physXEnableAdaptiveForce);
  m_physXSDK->setParameter(NX_IMPROVED_SPRING_SOLVER, g_physXInitEnableImprovedSpringSolver);

#ifdef NM_HOST_CELL_PPU
  // Initialise PhysX with spurs.
  uint8_t spuPriorities[8] = { 2, 2, 2, 2, 2, 2, 2, 2 };
  NxCellSpursControl::initWithSpurs(spursObject, NX_CELL_SPURS_DEFAULT_SPU_COUNT, spuPriorities, false);

  NMP_ASSERT_MSG(NxCellSpursControl::isSpursInitialized(), "Spurs intialisation test failed");
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX2Manager::term()
{ 
  // Destroy the scene, if there is one.
  destroyScene();
 
  // Shut down the PhysX SDK.
  if (m_physXSDK)
  {
    NxReleasePhysicsSDK(m_physXSDK);
    NMP_ASSERT(!NxGetPhysicsSDK());
    m_physXSDK = NULL;

  }
}

//----------------------------------------------------------------------------------------------------------------------
NxScene* PhysX2Manager::createScene(const NMP::Vector3* gravity)
{
  // We must have initialised the PhysX SDK and we can only have one scene per instance of PhysX2Manager.
  NMP_ASSERT(m_physXSDK && !m_physXScene && !m_physXCharacterControllerManager);
    
  //----------------------------------
  // Initialize the scene description.
  NxSceneDesc sceneDesc;

  sceneDesc.timeStepMethod = NX_TIMESTEP_FIXED;
  sceneDesc.maxTimestep    = g_physXMaxSubStepInterval;
  sceneDesc.maxIter        = g_physXMaxNumXSubSteps;

  // If no gravity direction is provided assume its acting in the negative Y direction.
  sceneDesc.gravity        = NxVec3(0.0f, -9.81f, 0.0f);
  if (gravity)
    sceneDesc.gravity      = NxVec3(gravity->x, gravity->y, gravity->z);

  //----------------------------------
  // Create the scene.
  m_physXScene = NxGetPhysicsSDK()->createScene(sceneDesc);
  NMP_ASSERT(m_physXScene);
  
  // Initialise the default material for the scene.
  //  We use this as a basis for creating materials for actors in the scene.
  NxMaterial* sceneDefaultMaterial = m_physXScene->getMaterialFromIndex(0);
  sceneDefaultMaterial->setDynamicFriction(1);
  sceneDefaultMaterial->setDynamicFriction(1);
  sceneDefaultMaterial->setRestitution(1);

  //----------------------------------
  // Create the character controller manager.
  //  In this implementation we are a associating the controller manager with the scene
  //  so that they are created and destroyed together.
  m_physXCharacterControllerManager = NxCreateControllerManager(NxGetPhysicsSDKAllocator());

  return m_physXScene;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX2Manager::destroyScene()
{
  NMP_ASSERT(m_physXSDK);

  // Destroy the PhysX character controller manager.
  if (m_physXCharacterControllerManager)
  {
    NxReleaseControllerManager(m_physXCharacterControllerManager);
    m_physXCharacterControllerManager = NULL;
  }

  // Destroy the PhysX scene.
  if (m_physXScene)
  {
    m_physXSDK->releaseScene(*m_physXScene);
    m_physXScene = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NxActor* PhysX2Manager::createPhysicsObjectStaticBox(
  const NMP::Vector3& dimensions,
  const NMP::Vector3& position,
  const NMP::Quat*    rotation)
{
  // Make sure that the SDK and scene are initialised.
  NMP_ASSERT(m_physXSDK && m_physXScene);

  // Create the shape description.
  NxBoxShapeDesc boxDesc;
  boxDesc.dimensions = NxVec3(dimensions.x * 0.5f, dimensions.y* 0.5f, dimensions.z * 0.5f); // Half dimensions.
  boxDesc.group =  (NxCollisionGroup)MR::GROUP_COLLIDABLE_NON_PUSHABLE;
  boxDesc.materialIndex = 0; // Use scene default material.

  // Create the body description.
  NxBodyDesc bodyDesc;
  bodyDesc.mass = 10;
  bodyDesc.flags |= NX_BF_KINEMATIC;

  // Create the actor description.
  NxActorDesc actorDesc;
  actorDesc.body = &bodyDesc;
  actorDesc.shapes.push_back(&boxDesc);

  // Create the actor from the description and set its position.
  NxActor* m_physXActor = m_physXScene->createActor(actorDesc);
  m_physXActor->setGlobalPosition(NxVec3(position.x, position.y, position.z));

  // If a rotation has been provided use it otherwise the box will be world axis aligned.
  if (rotation)
  {
    NxQuat nxRotation;
    nxRotation.setXYZW(rotation->x, rotation->y, rotation->z,rotation->w);
    m_physXActor->setGlobalOrientationQuat(nxRotation);
  }

  return m_physXActor;
}

//----------------------------------------------------------------------------------------------------------------------
NxActor* PhysX2Manager::createPhysicsObjectGroundPlane(
  float               groundHeight,
  float               planeWidth,
  const NMP::Vector3* inputPlaneNormal)
{
  // Make sure that the SDK and scene are initialised.
  NMP_ASSERT(m_physXSDK && m_physXScene);

  // Provide dimensions for the ground plane assuming the Y axis as the world up axis.
  const float planeThickness = 2.0f;
  NMP::Vector3 dimensions(planeWidth, planeThickness, planeWidth);

  //----------------------------
  // Determine our plane normal.
  NMP::Vector3 planeNormal(NMP::Vector3::InitTypeOneY);
  if (inputPlaneNormal)
  {
    // Provided normal.
    planeNormal.normaliseDep(*inputPlaneNormal, NMP::Vector3::InitTypeOneY);
  }
  else
  {
    // Inverted gravity direction.
    NxVec3 physXGravity;
    m_physXScene->getGravity(physXGravity);
    planeNormal.set(-physXGravity.x, -physXGravity.y, -physXGravity.z);
    planeNormal.normalise(NMP::Vector3::InitTypeOneY);
  }

  //----------------------------
  // Rotate the ground plane in order for its normal to be aligned with the true world up a direction.
  NMP::Vector3 yAxis(0.0f, 1.0f, 0.0f);
  NMP::Quat planeAttitude;
  planeAttitude.forRotation(yAxis, planeNormal);

  // Determine the position of the ground plane box.
  NMP::Vector3 position = planeNormal * (groundHeight - (planeThickness * 0.5f));

  //----------------------------
  // Create the box that will act as the ground plane.
  return createPhysicsObjectStaticBox(dimensions, position, &planeAttitude);
}

//----------------------------------------------------------------------------------------------------------------------
NxActor* PhysX2Manager::createPhysicsObject(
  uint32_t             geometryType,
  bool                 isDynamic,
  bool                 hasCollision,
  const NMP::Vector3&  halfDimensions,
  const NMP::Vector3&  position,
  const NMP::Quat&     orientation,
  float                density,
  NxTriangleMeshShape* NMP_UNUSED(triMeshGeom))
{
  // Make sure that the SDK and scene are initialised.
  NMP_ASSERT(m_physXSDK && m_physXScene);

  // Create material.
  NxMaterialDesc materialDesc;
  materialDesc.dynamicFriction = 1.0f;
  materialDesc.staticFriction = 1.0f;
  materialDesc.restitution = 0.0f;
  materialDesc.frictionCombineMode = NX_CM_MULTIPLY;
  materialDesc.restitutionCombineMode = NX_CM_MULTIPLY;
  
  NxMaterial* const material = m_physXScene->createMaterial(materialDesc);

  NxBoxShapeDesc boxDesc;
  boxDesc.dimensions = NxVec3(halfDimensions.x, halfDimensions.y, halfDimensions.z);

  NxCapsuleShapeDesc capsuleDesc;
  capsuleDesc.radius = halfDimensions.x;
  capsuleDesc.height = halfDimensions.y;

  NxSphereShapeDesc sphereDesc;
  sphereDesc.radius = halfDimensions.x;

  NxActorDesc actorDesc;
  {
    NxShapeDesc* shapeDesc = NULL;
    switch(geometryType)
    {
      case NX_SHAPE_BOX:
        shapeDesc = &boxDesc;
        break;
      case NX_SHAPE_CAPSULE:
        shapeDesc = &capsuleDesc;
        break;
      case NX_SHAPE_SPHERE:
        shapeDesc = &sphereDesc;
        break;
      default:
        NMP_ASSERT_FAIL();// Bad argument: Valid geometry type expected!
        return NULL;
    }

    NMP_ASSERT(shapeDesc);
    shapeDesc->skinWidth = (isDynamic ? 0.01f : 0.0001f);
    shapeDesc->group = (NxCollisionGroup)(isDynamic ? (1 << MR::GROUP_COLLIDABLE_PUSHABLE) : (1 << MR::GROUP_COLLIDABLE_NON_PUSHABLE));
    if (!hasCollision)
    {
      shapeDesc->shapeFlags |= NX_SF_DISABLE_COLLISION;
    }

    NMP_ASSERT(material);
    shapeDesc->materialIndex = material->getMaterialIndex();

    actorDesc.shapes.push_back(shapeDesc);
  }

  if (isDynamic)
  {
    NxBodyDesc bodyDesc;
    bodyDesc.maxAngularVelocity = 15.0f;
    bodyDesc.sleepEnergyThreshold = 0.005f;
    NMP_ASSERT_MSG(density >= 0.0f, "Bad argument: Valid density expected!");

    actorDesc.globalPose.t = NxVec3(position.x, position.y, position.z);
    NxQuat ori;
    ori.setXYZW(orientation.x, orientation.y, orientation.z, orientation.w);
    actorDesc.globalPose.M.fromQuat(ori);
    actorDesc.density = density;

    actorDesc.body = &bodyDesc;
    NxActor* rigidDynamic = m_physXScene->createActor(actorDesc);
    return rigidDynamic;
  }
  else
  {
    actorDesc.globalPose.t = NxVec3(position.x, position.y, position.z);
    NxQuat ori;
    ori.setXYZW(orientation.x, orientation.y, orientation.z, orientation.w);
    actorDesc.globalPose.M.fromQuat(ori);

    NxActor* rigidStatic = m_physXScene->createActor(actorDesc);
    return rigidStatic;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX2Manager::destroyPhysicsObject(NxActor* actor)
{
  // Make sure that the SDK and scene are initialised.
  NMP_ASSERT(m_physXSDK && m_physXScene)
  m_physXScene->releaseActor(*actor);
}
//----------------------------------------------------------------------------------------------------------------------
void PhysX2Manager::simulate(float deltaTime)
{
  // Make sure that the SDK and scene are initialised.
  NMP_ASSERT(m_physXSDK && m_physXScene);
    
  m_physXScene->setTiming(g_physXMaxSubStepInterval, g_physXMaxNumXSubSteps, NX_TIMESTEP_FIXED);
  m_physXScene->simulate(deltaTime);
  m_physXScene->flushStream();
  m_physXScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}
  
//----------------------------------------------------------------------------------------------------------------------
void PhysX2Manager::updateCharacterControllerManager()
{
  NMP_ASSERT(m_physXCharacterControllerManager);
  m_physXCharacterControllerManager->updateControllers();
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysX2Manager::validatePluginList(const NMP::OrderedStringTable& pluginList)
{
  if (pluginList.getNumEntries() == 0)
  {
    return true;
  }

  const char* physicsPlugin = "acPluginPhysX2_target_"NM_PLATFORM_FORMAT_STRING;
  const char* debugPhysicsPlugin = "acPluginPhysX2_target_"NM_PLATFORM_FORMAT_STRING"_debug";

  // The AnimModule has the core and the physics library registered.  If the asset contains the physics plug-in then
  // it must be the first plug-in.
  const char* pluginListEntry = pluginList.getEntryString(0);
  if ((NMP_STRCMP(pluginListEntry, debugPhysicsPlugin) == 0 || NMP_STRCMP(pluginListEntry, physicsPlugin) == 0))
  {
    return true;
  }

  NMP_ASSERT_FAIL_MSG(0, "Physics plug-in must be first in the list");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// PhysX2Manager::UserOutputStream functions.
//----------------------------------------------------------------------------------------------------------------------
void PhysX2Manager::UserOutputStream::reportError(
  NxErrorCode code,
  const char* message,
  const char* file,
  int         line)
{
  NMP_STDOUT("PHYSX ERROR: %s, %i: %s (%i)\n", file, line, message, code);
  return;
}

//----------------------------------------------------------------------------------------------------------------------
NxAssertResponse PhysX2Manager::UserOutputStream::reportAssertViolation(
  const char* message,
  const char* file,
  int         line)
{
  NMP_STDOUT("PHYSX ASSERT: %s, %i: %s\n", file, line, message);
  return NX_AR_CONTINUE;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysX2Manager::UserOutputStream::print(const char * message)
{
  NMP_STDOUT("PHYSX MSG: %s\n", message);
  return;
}

} // Game namespace

//----------------------------------------------------------------------------------------------------------------------
