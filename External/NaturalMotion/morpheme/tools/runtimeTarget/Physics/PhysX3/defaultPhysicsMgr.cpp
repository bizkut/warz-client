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
#include "defaultPhysicsMgr.h"
#include "NMPlatform/NMProfiler.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"

#include "comms/mcomms.h"
#include "comms/packet.h"

#include "../../iPhysicsMgr.h"
#include "../../iControllerMgr.h"

#include "morpheme/mrNetwork.h"

#include "mrPhysX3.h"
#include "mrPhysX3Includes.h"
#include "physics/mrPhysicsRigDef.h"
#include "mrPhysicsRigPhysX3Articulation.h"
#include "mrPhysicsRigPhysX3Jointed.h"
#include "mrPhysicsScenePhysX3.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsRigDef.h"
#include "../../defaultSceneObjectMgr.h"
#include "../../defaultAssetMgr.h"
#include "../../networkInstanceRecordManager.h"
#include "../../runtimeTargetContext.h"
#include "../../sceneObjectRecordManager.h"
#include "defaultControllerMgr.h"
#include "NMPlatform/NMvpu.h"
#include "physics/Nodes/mrPhysicsNodes.h"
#include "morpheme/mrDefines.h"
#include "morpheme/mrManager.h"
#include "physics/mrPhysics.h"

#include "memoryStream.h"

#include "morpheme/mrDispatcher.h"
#ifdef NM_HOST_CELL_PPU
  #include "morpheme/mrDispatcherPS3.h"
#endif // NM_HOST_CELL_PPU

#if defined(NM_HOST_CELL_PPU)
  #include "NMPlatform/ps3/NMSPUManager.h"
#endif // defined(NM_HOST_CELL_PPU)

#include "../../runtimeTargetLogger.h"

static const float minContactOffset = 0.0001f;

// Specifies which type of events a PhysX3 actor will broadcast to clients.
static physx::PxU32 g_clientBehaviourBits = 
                                physx::PxActorClientBehaviorBit::eREPORT_TO_FOREIGN_CLIENTS_CONTACT_NOTIFY | 
                                physx::PxActorClientBehaviorBit::eREPORT_TO_FOREIGN_CLIENTS_SCENE_QUERY;

/// Registered with PhysX to reduce the suddenness of penetration resolution in character self-contacts.
static MR::MorphemePhysX3ContactModifyCallback morphemePhysX3ContactModifyCallback(1.0f, 1/30.0f);

//----------------------------------------------------------------------------------------------------------------------
class ErrorStream : public physx::PxErrorCallback
{
public:
  void reportError(physx::PxErrorCode::Enum e, const char* message, const char* file, int line)
  {
    NMP_ASSERT_FAIL_MSG("Error reported from PhysX");
    printf("%s (%d) :", file, line);
    switch (e)
    {
    case physx::PxErrorCode::eINVALID_PARAMETER:
      printf("invalid parameter");
      break;
    case physx::PxErrorCode::eINVALID_OPERATION:
      printf("invalid operation");
      break;
    case physx::PxErrorCode::eOUT_OF_MEMORY:
      printf("out of memory");
      break;
    case physx::PxErrorCode::eDEBUG_INFO:
      printf("info");
      break;
    case physx::PxErrorCode::eDEBUG_WARNING:
      printf("warning");
      break;
    default:
      printf("unknown ` error");
    }

    printf(" : %s\n", message);
  }

  void print(const char* message)
  {
    printf("%s", message);
  }
};

class PhysXAllocator : public physx::PxAllocatorCallback
{
#ifdef WIN32
  // on win32 we only have 8-byte alignment guaranteed, but the CRT provides special aligned
  // allocation fns
  void* allocate(size_t size, const char*, const char*, int)
  {
    return _aligned_malloc(size, 16);
  }
  void deallocate(void* ptr)
  {
    _aligned_free(ptr);
  }
#elif defined(NM_HOST_ANDROID)
  void* allocate(size_t size, const char*, const char*, int)
  {
    void *ptr = memalign(16, size);
    PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15)==0);
    return ptr;
  }

  void deallocate(void* ptr)
  {
    free(ptr);
  }
#else
  // on PS3, XBox and Win64 we get 16-byte alignment by default
  void* allocate(size_t size, const char*, const char*, int)
  {
    void *ptr = ::malloc(size);
    PX_ASSERT((reinterpret_cast<size_t>(ptr) & 15)==0);
    return ptr;
  }
  void deallocate(void* ptr)
  {
    ::free(ptr);
  }
#endif
};

//----------------------------------------------------------------------------------------------------------------------
#define ANIM_GRAVITY
//----------------------------------------------------------------------------------------------------------------------
/// \class PhysicsSDK
/// \brief A singleton class to keep track of the instance of the physics sdk object
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class PhysicsSDK
{
public:
  static bool init(float physicsToleranceScale);
  static bool term();
  static bool connectToDebugger();

  static physx::PxCooking* getPxCooking() {return sm_physXCooking;}

  static void setPVDFileName(const char* fileName);

protected:

  PhysicsSDK() {}
  ~PhysicsSDK() {}
  PhysicsSDK& operator=(PhysicsSDK& liveLink);
  PhysicsSDK(const PhysicsSDK& liveLink);

  enum
  {
    kMaxFilenameLength = 260
  };

  static bool                       sm_created;
  static class PhysXAllocator       sm_physXAllocator;
  static class ErrorStream          sm_physXErrorStream;
  static physx::PxCooking*          sm_physXCooking;
  static char                       sm_pvdFilename[kMaxFilenameLength];
  static physx::PxMaterial*         sm_defaultPhysXMaterial;
  static physx::PxProfileZoneManager* sm_profileZoneManager;
};

//----------------------------------------------------------------------------------------------------------------------
// Statics
bool               PhysicsSDK::sm_created = false;
physx::PxCooking*  PhysicsSDK::sm_physXCooking = 0;
PhysXAllocator     PhysicsSDK::sm_physXAllocator;
ErrorStream        PhysicsSDK::sm_physXErrorStream;
char               PhysicsSDK::sm_pvdFilename[PhysicsSDK::kMaxFilenameLength] = { '\0' };
physx::PxMaterial* PhysicsSDK::sm_defaultPhysXMaterial = 0;
physx::PxProfileZoneManager* PhysicsSDK::sm_profileZoneManager;


//----------------------------------------------------------------------------------------------------------------------
/// \class PhysicsUserData
/// \brief Physics Engine user-data object for connecting a physics actor to the scene object that represents it
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class PhysicsUserData
{
public:
  PhysicsUserData(MCOMMS::SceneObjectID objID)
  {
    objectID = objID;
    dummyActor = 0;
    constraint = 0;
    magicString[0] = 'N';
    magicString[1] = 'M';
    magicString[2] = 'S';
    magicString[3] = 'O';
  }

  char magicString[4]; // initialize ad NMSO (NM-scene object)

  bool isValid()
  {
    return magicString[0] == 'N' && magicString[1] == 'M' && magicString[2] == 'S' && magicString[3] == 'O';
  }

  MCOMMS::SceneObjectID objectID;
  physx::PxActor* dummyActor;
  physx::PxJoint* constraint;
};

//----------------------------------------------------------------------------------------------------------------------
// Physics SDK singleton class
//----------------------------------------------------------------------------------------------------------------------
bool PhysicsSDK::init(float physicsToleranceScale)
{
  term();

  physx::PxTolerancesScale scale;
  scale.length *= physicsToleranceScale;
  scale.mass   *= physicsToleranceScale * physicsToleranceScale * physicsToleranceScale;
  scale.speed  *= physicsToleranceScale;

  physx::PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, sm_physXAllocator, sm_physXErrorStream);
  NMP_ASSERT(foundation);

  sm_profileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager(foundation);

  physx::PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale, true, sm_profileZoneManager);
  if (!physics)
  {
    printf(
      "Unable to create the NVIDIA PhysX SDK. "
      "The NVIDIA PhysX System software is required to run this app.");
    NMP_ASSERT_FAIL();
  }

  physx::PxCookingParams cookingParams;
  cookingParams.skinWidth = 0.01f; // copied from earlier, should we use physX default here?
  cookingParams.buildTriangleAdjacencies = true;
#if PX_PHYSICS_VERSION  > ((3<<24) + (2<<16) + (3<<8) + 0)
  cookingParams.scale.length = physicsToleranceScale;
#endif

  sm_physXCooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, cookingParams);

  if (!sm_physXCooking)
  {
    NMP_ASSERT_FAIL();
  }

  PxInitExtensions(*physics);

  // Create a default material
  sm_defaultPhysXMaterial = physics->createMaterial(1.0f, 1.0f, 0.0f);
  sm_defaultPhysXMaterial->setFrictionCombineMode(physx::PxCombineMode::eMULTIPLY);
  sm_defaultPhysXMaterial->setRestitutionCombineMode(physx::PxCombineMode::eMULTIPLY);
  NMP_ASSERT(sm_defaultPhysXMaterial);

  sm_created = true;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsSDK::term()
{
  if (!sm_created)
  {
    return false;
  }

  if (sm_defaultPhysXMaterial)
  {
    sm_defaultPhysXMaterial->release();
    sm_defaultPhysXMaterial = 0;
  }

  PxCloseExtensions();

  // Release the physX cooking
  if (sm_physXCooking)
  {
    sm_physXCooking->release();
    sm_physXCooking = 0;
  }

  PxGetPhysics().release();
  PxGetFoundation().release();

  sm_created = false;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsSDK::connectToDebugger()
{
  // These calls will return 0 on platforms that don't support the debugger
  if (PxGetPhysics().getPvdConnectionManager() != 0)
  {
    if (sm_pvdFilename[0] == '\0')
    {
      PxGetPhysics().getPvdConnectionManager()->disconnect();
      //The normal way to connect to pvd.  PVD needs to be running at the time this function is called.
      //We don't worry about the return value because we are already registered as a listener for connections
      //and thus our onPvdConnected call will take care of setting up our basic connection state.
      physx::PxVisualDebuggerExt::createConnection(
        PxGetPhysics().getPvdConnectionManager(), 
        "127.0.0.1", 5425, 100, 
        physx::PxVisualDebuggerConnectionFlags(
        physx::PxVisualDebuggerConnectionFlag::Debug |
        physx::PxVisualDebuggerConnectionFlag::Memory |
        physx::PxVisualDebuggerConnectionFlag::Profile
      ));
    }   
    else
    {
      //Create a pvd connection that writes data straight to the filesystem.  This is
      //the fastest connection on windows for various reasons.  First, the transport is quite fast as
      //pvd writes data in blocks and filesystems work well with that abstraction.
      //Second, you don't have the PVD application parsing data and using CPU and memory bandwidth
      //while your application is running.
      physx::debugger::comm::PvdConnectionManager* mgr = PxGetPhysics().getPvdConnectionManager();
      mgr->connect(sm_physXAllocator, sm_pvdFilename);
    }

    if (PxGetPhysics().getVisualDebugger())
      PxGetPhysics().getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlags::eTRANSMIT_CONTACTS, true);

  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSDK::setPVDFileName(const char* filename)
{
  if (filename)
  {
    NMP_STRNCPY_S(sm_pvdFilename, kMaxFilenameLength, filename);
  }
  else
  {
    sm_pvdFilename[0] = '\0';
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Default physics manager
//----------------------------------------------------------------------------------------------------------------------

namespace
{

void incPhysicsAssetRefCounts(MR::NetworkDef* networkDef);
void decPhysicsAssetRefCounts(MR::NetworkDef* networkDef);
void deleteOrphanedPhysicsAsset(MR::NetworkDef* networkDef);

#ifdef NM_PROFILING
  float g_totalTiming = 0.0f;
  float g_maxTime = 0.0f;
  uint32_t g_totalSamples = 0;
#endif

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsMgr::DefaultPhysicsMgr(
  RuntimeTargetContext* context,
  DefaultAssetMgr* assetMgr,
  const NMP::CommandLineProcessor& commandLineArguments) :
  IPhysicsMgr(),
  m_assetMgr(assetMgr),
  m_context(context),
  m_physicsRigType(MR::PhysicsRigPhysX3::TYPE_ARTICULATED),
  m_frameIndex(0),
  m_physicsScene(NULL),
  m_cpuDispatcher(NULL),
  m_characterControllerManager(NULL),
  m_nextPhysicsObjectID(0),
  m_toleranceScalingValue(1.0f),
  m_physicsAndCharacterControllerUpdate(MR::PHYSICS_AND_CC_UPDATE_SEPARATE)
{
  m_assetMgr->setPhysicsManager(this);

  const char* pvdFilename = NULL;
  commandLineArguments.getOptionValue("-pvdFilename", &pvdFilename);
  PhysicsSDK::setPVDFileName(pvdFilename);

  const char* physicsRigType = NULL;
  commandLineArguments.getOptionValue("-physicsRigType", &physicsRigType);

  if (physicsRigType)
  {
    if (strcmp(physicsRigType, "TYPE_ARTICULATED") == 0)
    {
      m_physicsRigType = MR::PhysicsRigPhysX3::TYPE_ARTICULATED;
      NMP_MSG("Using physics rig type TYPE_ARTICULATED\n");
    }
    else if (strcmp(physicsRigType, "TYPE_JOINTED") == 0)
    {
      m_physicsRigType = MR::PhysicsRigPhysX3::TYPE_JOINTED;
      NMP_MSG("Using physics rig type TYPE_JOINTED\n");
    }
  }

  resetSDKS();

  m_assetMgr->registerRefCountIncCallback(&incPhysicsAssetRefCounts);
  m_assetMgr->registerRefCountDecCallback(&decPhysicsAssetRefCounts);
  m_assetMgr->registerDeleteOrphanedAssetCallback(&deleteOrphanedPhysicsAsset);
  
}

//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsMgr::~DefaultPhysicsMgr()
{
  m_assetMgr->setPhysicsManager(NULL);

  deleteScene();
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::getPhysicsEngineID(char* buffer, uint32_t bufferLength) const
{
  NMP_STRNCPY_S(buffer, bufferLength, "PhysX3");
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updateEnvironment()
{
  NMP::Vector3 worldUpVector(0.0f, 1.0f, 0.0f);

  // Get the gravity settings from the environment.
  MCOMMS::EnvironmentManagementInterface* environmentManager = MCOMMS::getRuntimeTarget()->getEnvironmentManager();
  NMP_ASSERT(environmentManager);
  bool gravityEnabled = false;
  NMP::Vector3 gravityVector(0.0f, 0.0f, 0.0f);

  // Configure gravity and up axis
  {
    MCOMMS::Attribute* gravityAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_VECTOR);
    NMP_ASSERT(gravityAttr); // Must exist for us to initialise the gravity correctly.
    gravityVector = *((NMP::Vector3*)gravityAttr->getData());

    // For now set the up vector to be the inverse of the gravity direction. However, this is
    // not always correct, and also check that everything works when gravity = 0.  This line below is
    // OK, but there may be inconsistencies elsewhere.  See MORPH-11297
    worldUpVector = gravityVector * -1.0f;
    worldUpVector.normalise(NMP::Vector3::InitTypeOneY);
  }

  // Get whether gravity is enabled
  {
    MCOMMS::Attribute* gravityAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED);
    NMP_ASSERT(gravityAttr); // Must exist for us to initialise the gravity correctly.

    gravityEnabled = (*(int*)gravityAttr->getData() != 0);
    if (!gravityEnabled)
    {
      // gravity is not enabled, so zero the vector.
      gravityVector.setToZero();
      // Note that the up axis must remain configured!
    }
  }

  NMP_ASSERT(m_physicsScene);
  m_physicsScene->setGravity(gravityVector);
  m_physicsScene->setWorldUpDirection(worldUpVector);
}

//----------------------------------------------------------------------------------------------------------------------
float DefaultPhysicsMgr::getActualTimestep(float deltaTime) const
{
  return deltaTime;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::simulate(float deltaTime)
{
  if (deltaTime < 0.000001f)
    return;

  if (deltaTime > m_maxTimeStep)
    deltaTime = m_maxTimeStep;

  float maxSeparationSpeed = 0.5f*m_toleranceScalingValue;
  morphemePhysX3ContactModifyCallback.setTimeStep(deltaTime);
  morphemePhysX3ContactModifyCallback.setMaxSeparationSpeed(maxSeparationSpeed);

  m_physicsScene->getPhysXScene()->simulate(deltaTime);
  m_physicsScene->getPhysXScene()->fetchResults(true); 

  m_physicsScene->setLastPhysicsTimeStep(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
// Create a PhysX actor of specified geometry type (box, capsule, sphere)
//----------------------------------------------------------------------------------------------------------------------
physx::PxRigidActor* DefaultPhysicsMgr::createActor(
  physx::PxGeometryType::Enum geomType,
  physx::PxGeometry* geometry,
  bool dynamic,
  const NMP::Vector3& pos,
  const NMP::Quat& rot,
  float density,
  bool hasCollision,
  physx::PxMaterial* material,
  float staticFriction,
  float dynamicFriction,
  float skinWidth,
  float restitution,
  float sleepThreshold,
  float maxAngularVelocity,
  float linearDamping,
  float angularDamping,
  NMP::Matrix34* NMP_UNUSED(localPose))
{
  physx::PxPhysics& physics = PxGetPhysics();
  const physx::PxTolerancesScale& tolerancesScale = physics.getTolerancesScale();

  physx::PxTransform globalPose(MR::nmVector3ToPxVec3(pos), MR::nmQuatToPxQuat(rot));
  physx::PxClientID physicsClientID = physx::PX_DEFAULT_CLIENT;

  //-----------------------
  // create the actor
  physx::PxRigidActor *actor = 0;
  if (dynamic)
  {
    physx::PxRigidDynamic* dynamicActor = physics.createRigidDynamic(globalPose);

    dynamicActor->setClientBehaviorBits(g_clientBehaviourBits);
    dynamicActor->setOwnerClient(physicsClientID);

    dynamicActor->setLinearDamping(linearDamping);
    dynamicActor->setAngularDamping(angularDamping);
    dynamicActor->setMaxAngularVelocity(maxAngularVelocity);
    dynamicActor->setSleepThreshold(sleepThreshold * tolerancesScale.speed * tolerancesScale.speed);

    actor = dynamicActor;
  }
  else
  {
    physx::PxRigidStatic* staticActor = physics.createRigidStatic(globalPose);

    staticActor->setClientBehaviorBits(g_clientBehaviourBits);
    staticActor->setOwnerClient(physicsClientID);

    actor = staticActor;
  }
  NMP_ASSERT(actor);

  //-----------------------
  // adjust local pose for capsules so they are oriented correctly
  physx::PxTransform shapeLocalPose = physx::PxTransform::createIdentity();
  if (geomType == physx::PxGeometryType::eCAPSULE)
  {
    shapeLocalPose.q = physx::PxQuat(NM_PI_OVER_TWO, physx::PxVec3(0.0f, 1.0f, 0.0f));
  }

  //-----------------------
  // create a material
  if(!material)
  {
    material = physics.createMaterial(staticFriction, dynamicFriction, restitution);
    material->setFrictionCombineMode(physx::PxCombineMode::eMULTIPLY);
    material->setRestitutionCombineMode(physx::PxCombineMode::eMULTIPLY);
    NMP_ASSERT(material);
    m_materials.push_back(material);
  }

  //-----------------------
  // create the actor's shape
  physx::PxShape* shape = actor->createShape(*geometry, *material, shapeLocalPose);

  //-----------------------
  // For simplicity, make the contact offset of statics effectively zero, and the offset of dynamics > 0
  shape->setContactOffset(dynamic ? skinWidth : minContactOffset);
  shape->setRestOffset(0.0f);

  physx::PxU32 filterWord0 = (dynamic ? 1 << MR::GROUP_COLLIDABLE_PUSHABLE : 1 << MR::GROUP_COLLIDABLE_NON_PUSHABLE);

  //-----------------------
  // set up filter data
  shape->setQueryFilterData(physx::PxFilterData(filterWord0, 0, 0, 0));
  if (!hasCollision)
  {
    shape->setSimulationFilterData(physx::PxFilterData(0, 0xffffffff, 0, 0));
  }
  else
  {
    shape->setSimulationFilterData(physx::PxFilterData(filterWord0, 0, 0, 0));
  }

  if (dynamic)
  {
    NMP_ASSERT(density >= 0.0f);
    physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(actor), density);
  }

  // Add per shape data - needed for the object to be recognised by Euphoria whether it's static or
  // dynamic.
  physx::PxShape *tempShapes[1];
  int numShapes = actor->getShapes(&tempShapes[0], 1);
  for (int i = 0; i<numShapes; i++)
  {
    MR::PhysXPerShapeData::create(tempShapes[i]);
  }

  return actor;
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::createNewPhysicsBody(
  float                 dynamicFriction,
  float                 staticFriction,
  float                 restitution,
  bool                  isDynamic,
  MCOMMS::Attribute::PhysicsShapeType shapeType,
  float                 depth,
  float                 height,
  float                 length,
  float                 radius,
  float                 skinWidth,
  bool                  hasIndices,
  bool                  hasVertices,
  size_t                numPoints,
  size_t                numIndices,
  NMP::Vector3*         points,
  int32_t*              indices,
  MCOMMS::Attribute::VerticesWindingType windingType,
  NMP::Matrix34&        transform,
  float                 density,
  bool                  isConstrained,
  MCOMMS::SceneObjectID objectID,
  NMP::Matrix34&        constraintGlobalTransform,
  float                 constraintDamping,
  float                 constraintStiffness,
  NMP::Matrix34&        constraintLocalTransform,
  uint32_t              positionSolverIterationCount,
  uint32_t              velocitySolverIterationCount,
  float                 sleepThreshold,
  float                 maxAngularVelocity,
  float                 linearDamping,
  float                 angularDamping)
{
  physx::PxActor* actor = 0;
  physx::PxGeometry* geometry = NULL;
  physx::PxGeometryType::Enum geometryType = physx::PxGeometryType::eINVALID;
  physx::PxTriangleMeshGeometry *triangleMeshGeometry = NULL;
  physx::PxConvexMeshGeometry *convexMeshGeometry = NULL;

  switch (shapeType)
  {
  case MCOMMS::Attribute::PHYSICS_SHAPE_BOX:

  {
    geometryType = physx::PxGeometryType::eBOX;
    physx::PxVec3 halfDimensions(depth * 0.5f, height * 0.5f, length * 0.5f);
    geometry = new physx::PxBoxGeometry(halfDimensions);
    break;
  }

  case MCOMMS::Attribute::PHYSICS_SHAPE_CYLINDER: // TODO... proper cylinder
  case MCOMMS::Attribute::PHYSICS_SHAPE_CAPSULE:

    geometryType = physx::PxGeometryType::eCAPSULE;
    geometry = new physx::PxCapsuleGeometry(radius, height * 0.5f);
    break;

  case MCOMMS::Attribute::PHYSICS_SHAPE_SPHERE:

    geometryType = physx::PxGeometryType::eSPHERE;
    geometry = new physx::PxSphereGeometry(radius);
    break;

  case MCOMMS::Attribute::PHYSICS_SHAPE_MESH:

    if (isDynamic) // if the mesh is dynamic then convert it into a convex mesh
    {
      if (hasIndices && hasVertices)
      {
        geometryType = physx::PxGeometryType::eCONVEXMESH;
        physx::PxConvexMeshDesc convexMeshDesc;
        convexMeshGeometry = new physx::PxConvexMeshGeometry();

        convexMeshDesc.points.count = (physx::PxU32)numPoints;
        convexMeshDesc.points.stride = sizeof(NMP::Vector3);
        convexMeshDesc.points.data = points;
        convexMeshDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

        phx::MemoryWriteBuffer buf;
        bool status = PhysicsSDK::getPxCooking()->cookConvexMesh(convexMeshDesc, buf);
        (void)status;
        NMP_ASSERT(status);
        phx::MemoryReadBuffer readBuffer(buf.data);
        physx::PxConvexMesh* mesh = PxGetPhysics().createConvexMesh(readBuffer);
        NMP_ASSERT(mesh);
        convexMeshGeometry->convexMesh = mesh;
        geometry = convexMeshGeometry;
      }
    }
    else
    {
      if (hasIndices && hasVertices)
      {
        geometryType = physx::PxGeometryType::eTRIANGLEMESH;
        physx::PxTriangleMeshDesc triangleMeshDesc;
        triangleMeshGeometry = new physx::PxTriangleMeshGeometry();

        triangleMeshDesc.points.count = (physx::PxU32)numPoints;
        triangleMeshDesc.triangles.count = (physx::PxU32)(numIndices / 3);
        triangleMeshDesc.points.stride = sizeof(NMP::Vector3);
        triangleMeshDesc.triangles.stride = 3 * sizeof(int);
        triangleMeshDesc.points.data = points;
        triangleMeshDesc.triangles.data = indices;
        triangleMeshDesc.flags = (physx::PxMeshFlags) 0;

        // TODO if the winding type is undefined, should we create the mesh anyway?
        if (windingType == MCOMMS::Attribute::VERTICES_WINDING_COUNTERCLOCKWISE)
          triangleMeshDesc.flags = physx::PxMeshFlag::eFLIPNORMALS;

        physx::PxTriangleMesh* meshData = createTriangleMeshData(triangleMeshDesc);
        NMP_ASSERT(meshData);

        triangleMeshGeometry->triangleMesh = meshData;
        geometry = triangleMeshGeometry;
      }
    }
    break;

  // Physics TODO handle the other cases
  default:
    NMP_ASSERT_FAIL();
    return MR_INVALID_PHYSICS_OBJECT_ID;
  }

  if( geometryType != physx::PxGeometryType::eINVALID)
  {
    actor = createActor(
      geometryType,
      geometry,
      isDynamic,
      transform.translation(),
      transform.toQuat(),
      density,
      true,
      NULL,
      staticFriction,
      dynamicFriction,
      skinWidth,
      restitution,
      sleepThreshold,
      maxAngularVelocity,
      linearDamping,
      angularDamping);
  }

  if (actor)
  {
    if (actor->isRigidDynamic())
      actor->isRigidDynamic()->setSolverIterationCounts(positionSolverIterationCount, velocitySolverIterationCount);
    PhysicsUserData* userData = new PhysicsUserData(objectID);
    actor->userData = (void*)userData;
    m_physicsScene->getPhysXScene()->addActor(*actor);

    MR::PhysicsObjectID actorID = assignPhysicsIDToActor(actor);

    // Register the actor in the physics manager.
    getSceneActors().push_back(actor);

    // Create a constraint between the actor and the static world.
    if (isConstrained && isDynamic)
    {
      physx::PxRigidDynamic* rigidDynamic = actor->isRigidDynamic();
      NMP_ASSERT(rigidDynamic);

      physx::PxTransform parentFrame = MR::nmMatrix34ToPxTransform(constraintGlobalTransform);

      NMP::Matrix34 constraintLocalTransformInv = constraintLocalTransform;
      constraintLocalTransformInv.invert();
      physx::PxTransform childFrame = MR::nmMatrix34ToPxTransform(constraintLocalTransformInv);

      physx::PxD6Joint *joint = PxD6JointCreate(
          PxGetPhysics(), 
          0,
          parentFrame,
          rigidDynamic, 
          childFrame);

      joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eFREE);
      joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eFREE);
      joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
      joint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLOCKED);
      joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
      joint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLOCKED);

      physx::PxD6JointDrive jointDrive(constraintStiffness, constraintDamping, PX_MAX_F32, true);
      joint->setDrive(physx::PxD6Drive::eSLERP, jointDrive);

      // Store a reference to the constraint, for deletion.
      userData->constraint = joint;
    }

    return actorID;
  }
  return MR_INVALID_PHYSICS_OBJECT_ID;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::destroyPhysicsBody(MCOMMS::SceneObjectID objectID)
{
  // destroy the physics part if there is one
  physx::PxActor* foundActor = 0;
  size_t foundActorIndex = 0;
  for (size_t i = 0; i < m_sceneActors.size(); ++i)
  {
    if (!m_sceneActors[i])
      continue;

    PhysicsUserData* userData = (PhysicsUserData*)m_sceneActors[i]->userData;
    if (userData && userData->isValid())
    {
      MCOMMS::SceneObjectID foundObjectID = userData->objectID;
      if (foundObjectID == objectID)
      {
        foundActor = m_sceneActors[i];
        foundActorIndex = i;
        break;
      }
    }
  }

  if (foundActor)
  {
    unassignPhysicsID(foundActor);

    // Destroy per-shape user data
    if (foundActor->isRigidActor())
    {
      physx::PxShape *shapes[32];
      physx::PxU32 numShapes = foundActor->isRigidActor()->getShapes(&shapes[0], 32);
      for (physx::PxU32 iShape=0; iShape<numShapes; ++iShape)
      {
        physx::PxShape* shape = shapes[iShape];
        MR::PhysXPerShapeData *data = MR::PhysXPerShapeData::getFromShape(shape);
        MR::PhysXPerShapeData::destroy(data, shape);
      }
    }

    // if the object is constrained, release the physical joint
    PhysicsUserData* userData = (PhysicsUserData*)foundActor->userData;
    if (userData->constraint)
    {
      userData->constraint->release();
      userData->constraint = 0;
    }

    delete userData;
    foundActor->userData = 0;

    // finally release the actor
    foundActor->release();

    m_sceneActors[foundActorIndex] = m_sceneActors.back();
    m_sceneActors.pop_back();
  }

}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::createConstraint(
  uint64_t            constraintGUID,
  MR::PhysicsObjectID physicsObjectID, 
  const NMP::Vector3& constraintPosition, 
  bool                lockOrientation,
  bool                constrainAtCOM)
{
  NMP_ASSERT_MSG(!getConstraint(constraintGUID), "createConstraint called for a GUID that already exists!");

  physx::PxActor *actorToConstrain = getActorByPhysicsID(physicsObjectID);
  NMP_ASSERT_MSG(actorToConstrain, "Could not find physics object of ID %i!", physicsObjectID);

  physx::PxRigidBody *bodyToConstrain = actorToConstrain->isRigidBody();
  if(!bodyToConstrain)
  {
    // We can't constrain bodies if they aren't dynamic
    return false;
  }

  // This constraint object is persisted in order to make it possible to modify it later.
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(Constraint), NMP_VECTOR_ALIGNMENT);
  Constraint *newConstraint = new(alignedMemory)Constraint();

  // If this is a constrainAtCOM type, we need to make sure we store the offset from the clicked
  // position to the COM, then create the constraint at the COM position.
  NMP::Vector3 actualConstraintPosition;
  if(constrainAtCOM)
  {
    // Get the COM position of the rigid body
    physx::PxRigidBody* rigidBody = bodyToConstrain->isRigidBody();
    actualConstraintPosition = 
      MR::nmPxVec3ToVector3(rigidBody->getGlobalPose().transform(rigidBody->getCMassLocalPose().p));
    newConstraint->m_grabOffsetFromCOM = constraintPosition - actualConstraintPosition;
  }
  else
  {
    actualConstraintPosition = constraintPosition;
    newConstraint->m_grabOffsetFromCOM.setToZero();
  }

  physx::PxD6Joint *joint;
  if (lockOrientation)
  {
    joint = PxD6JointCreate(
      PxGetPhysics(), 
      0, 
      physx::PxTransform(MR::nmVector3ToPxVec3(actualConstraintPosition)),
      bodyToConstrain, 
      physx::PxTransform(
      bodyToConstrain->getGlobalPose().transformInv(MR::nmVector3ToPxVec3(actualConstraintPosition)), 
      bodyToConstrain->getGlobalPose().q.getConjugate()));
    joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLOCKED);
    joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLOCKED);
    joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLOCKED);
  }
  else
  {
    joint = PxD6JointCreate(
      PxGetPhysics(), 
      0, 
      physx::PxTransform(MR::nmVector3ToPxVec3(actualConstraintPosition)),
      bodyToConstrain, 
      physx::PxTransform(
      bodyToConstrain->getGlobalPose().transformInv(MR::nmVector3ToPxVec3(actualConstraintPosition))));
    joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eFREE);
    joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eFREE);
    joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
  }

#if 1
  // use the drive, as otherwise we don't preserve momentum when releasing 
  float dampingRatio = 1.0f;
  float spring = 1000.0f;
  float damping = 2.f*dampingRatio*sqrtf(spring); // critical damping
  physx::PxD6JointDrive drive(spring,damping,PX_MAX_F32,physx::PxD6JointDriveFlag::eACCELERATION);
  joint->setDrive(physx::PxD6Drive::eX, drive);
  joint->setDrive(physx::PxD6Drive::eY, drive);
  joint->setDrive(physx::PxD6Drive::eZ, drive);
  // Free constraint to let the drive work
  joint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eFREE);
  joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eFREE);
  joint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eFREE);
#else
  // Use a locked constraint
  joint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLOCKED);
  joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
  joint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLOCKED);
#endif

  newConstraint->m_constrainedActor = actorToConstrain;
  newConstraint->m_jointConstraint = joint;

  newConstraint->m_isCOMConstraint = constrainAtCOM;

  m_constraintMap.insert(constraintGUID, newConstraint);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::destroyConstraint(uint64_t constraintGUID)
{
  Constraint* constraint = NULL;
  bool found = m_constraintMap.find(constraintGUID, &constraint);
  
  if (!found)
  {
    return false;
  }

  // Destroy the joint itself
  constraint->m_jointConstraint->release();

  // Remove the constraint from the map
  m_constraintMap.erase(constraintGUID);

  // Delete the constraint record itself
  NMP::Memory::memFree(constraint);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::moveConstraint(uint64_t constraintGUID, const NMP::Vector3 &newGrabPosition)
{
  Constraint* constraint = NULL;
  bool found = m_constraintMap.find(constraintGUID, &constraint);

  if (!found)
  {
    return false;
  }

  // Wake up the body that's connected to the constraint.
  physx::PxRigidBody *rigidBody = constraint->m_constrainedActor->isRigidBody();
  rigidBody->addForce(physx::PxVec3(0,0,0));

  // Move the constraint to the new point in space as requested.
  // TODO: Make use of the com offset if we are a com grab!
  constraint->m_jointConstraint->setLocalPose(physx::PxJointActorIndex::eACTOR0, physx::PxTransform(MR::nmVector3ToPxVec3(newGrabPosition), physx::PxQuat::createIdentity()));

  return true;
}


//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 DefaultPhysicsMgr::getActorCOMPos(physx::PxActor* actor) const
{
  physx::PxRigidBody* rigidBody = actor->isRigidBody();
  return MR::nmPxVec3ToVector3(rigidBody->getGlobalPose().transform(rigidBody->getCMassLocalPose().p));
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::applyForce(
  uint32_t            physicsEngineObjectID, 
  ForceMode           mode,
  const NMP::Vector3& force,
  bool                applyAtCOM,
  const NMP::Vector3& NMP_UNUSED(localSpacePosition),
  const NMP::Vector3& worldSpacePosition)
{
  physx::PxActor * actor = getActorByPhysicsID(physicsEngineObjectID);
  
  NMP_ASSERT_MSG(actor, "Could not find physics object of ID %i!", physicsEngineObjectID);
  if (!actor)
  {
    return false; // actor not found
  }

  // PhysX3 can't apply forces to static bodies
  physx::PxActorType::Enum actorType = actor->getType();
  if (actorType == physx::PxActorType::eRIGID_STATIC)
  {
    return false;
  }

  NMP::Vector3 forcePosition = applyAtCOM ? getActorCOMPos(actor) : worldSpacePosition;
  switch(mode)
  {
    case IPhysicsMgr::kFORCE:
      MR::addForceToActor(*actor, force, forcePosition);
      break; 

    case IPhysicsMgr::kIMPULSE:
      MR::addImpulseToActor(*actor, force, forcePosition);
      break; 

    case IPhysicsMgr::kVELOCITY_CHANGE:
      MR::addVelocityChangeToActor(*actor, force, forcePosition);
      break; 

    default:
      NMP_ASSERT_FAIL_MSG("Unknown force mode in apply force");
  }
  return true; 
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::setPhysicsObjectAttribute(
  uint32_t physicsEngineObjectID,
  const MCOMMS::Attribute* physicsObjAttribute)
{
  physx::PxActor * actor = getActorByPhysicsID(physicsEngineObjectID);
  NMP_ASSERT_MSG(actor, "Could not find physics object of ID %i!", physicsEngineObjectID);
  if(!actor)
  {
    return false;
  }

  // PhysX3 advises against modifying transforms on static bodies during normal simulation.
  // (hence, disallowed in this case).
  physx::PxActorType::Enum actorType = actor->getType();
  if (actorType == physx::PxActorType::eRIGID_STATIC)
  {
    return false;
  }

  switch(physicsObjAttribute->getSemantic())
  {
    // Instantaneously teleport the object to the world space transform specified in the attribute data
    case MCOMMS::Attribute::SEMANTIC_TRANSFORM:
    {
      const NMP::Matrix34* newWorldSpaceTransform = (const NMP::Matrix34*)(physicsObjAttribute->getData());
      MR::setActorGlobalPoseTM(*actor, *newWorldSpaceTransform);
      MR::setActorLinVelW(*actor, NMP::Vector3(NMP::Vector3::InitZero));
      MR::setActorAngVelW(*actor, NMP::Vector3(NMP::Vector3::InitZero));
      return true;
    }

  default:
    break;
  }

  // attribute semantic not handled
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::setPhysicsEnvironmentAttribute(const MCOMMS::Attribute* physicsEnvAttribute)
{
  // We must have a physics scene to work with
  NMP_ASSERT(m_physicsScene);
  if(!m_physicsScene)
  {
    return false;
  }
  
  switch(physicsEnvAttribute->getSemantic())
  {
    // Only handling gravity related attibutes at the moment, which we pass on to updateEnvironment()
    // provided the environment has got both attribs already setup
    case MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED:
    case MCOMMS::Attribute::SEMANTIC_GRAVITY_VECTOR:
    {
      MCOMMS::EnvironmentManagementInterface* environmentManager = MCOMMS::getRuntimeTarget()->getEnvironmentManager();
      if(environmentManager)
      {
        MCOMMS::Attribute* gravityAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_VECTOR);
        MCOMMS::Attribute* gravityEnabledAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED);
        if(gravityAttr && gravityEnabledAttr)
        {
          updateEnvironment();
        }
      }
      break;
    }
    default:
      break;
  }

  // attribute semantic not handled
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::validatePluginList(const NMP::OrderedStringTable& pluginList)
{
  if (pluginList.getNumEntries() == 0)
  {
    return true;
  }

  const char* physicsPlugin = "acPluginPhysX3_target_" NM_PLATFORM_FORMAT_STRING;
  const char* debugPhysicsPlugin = "acPluginPhysX3_target_" NM_PLATFORM_FORMAT_STRING "_debug";

  // Physics is registered first so we expect it to be the first compiled plug-in
  const char* pluginEntry0 = pluginList.getEntryString(0);
  if (NMP_STRCMP(pluginEntry0, physicsPlugin) == 0 || NMP_STRCMP(pluginEntry0, debugPhysicsPlugin) == 0)
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxTriangleMesh* DefaultPhysicsMgr::createTriangleMeshData(const physx::PxTriangleMeshDesc& meshDesc)
{
  phx::MemoryWriteBuffer buf;

  bool status = PhysicsSDK::getPxCooking()->cookTriangleMesh(meshDesc, buf);
  if (!status) 
  {
    NMP_ASSERT_FAIL();
  }
  phx::MemoryReadBuffer readBuffer(buf.data);
  physx::PxTriangleMesh* mesh = PxGetPhysics().createTriangleMesh(readBuffer);
  return mesh;
}


//----------------------------------------------------------------------------------------------------------------------
PhysXConvexMesh* DefaultPhysicsMgr::createConvexMeshData(const PhysXConvexMeshDesc& NMP_UNUSED(meshDesc))
{
  PhysXConvexMesh* mesh = NULL;

  return mesh;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::initializeTriangleMeshShapeDesc(
  const PhysXTriangleMeshDesc& NMP_UNUSED(meshDesc),
  PhysXTriangleMeshShapeDesc&  NMP_UNUSED(shapeDesc))
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::initializeConvexShapeDesc(
  const PhysXConvexMeshDesc& NMP_UNUSED(meshDesc),
  PhysXConvexShapeDesc&      NMP_UNUSED(shapeDesc))
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::resetScene()
{
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::initializeScene()
{
  // Materials aren't released when objects are, as they may (in principle) be shared. So, clear
  // everything out here.
  clearScene();

  // Simulate a couple of times to flush the visual debugger - it lets us see the state of things
  // after we've (hopefully) tidied everything up. Also it's needed to flush the material releases
  // from physx, otherwise the debugger code crashes when we try to connect. Possibly remove this -
  // see MORPH-15537
  m_physicsScene->getPhysXScene()->simulate(0.00001f);
  m_physicsScene->getPhysXScene()->fetchResults(true); 
 
  m_physicsScene->getPhysXScene()->simulate(0.00001f);
  m_physicsScene->getPhysXScene()->fetchResults(true); 

#ifdef NM_PROFILING
  g_totalTiming = 0.0f;
  g_maxTime = 0.0f;
  g_totalSamples = 0;
#endif
  PhysicsSDK::connectToDebugger();
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::clearScene()
{
  for (size_t i = 0; i < m_sceneActors.size(); ++i)
  {
    if (m_sceneActors[i])
      m_sceneActors[i]->release();
  }
  m_sceneActors.clear();

  while (!m_materials.empty())
  {
    m_materials.back()->release();
    m_materials.pop_back();
  }

  for (size_t i = 0; i < m_joints.size(); ++i)
  {
    m_joints[i]->release();
  }
  m_joints.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::createPhysicsRig(MR::Network *network, NMP::Vector3* initialPosition)
{
  NMP_ASSERT(network);
  NMP_ASSERT(initialPosition);

  MR::AnimRigDef* animRigDef = network->getActiveRig();
  MR::PhysicsRigDef* physicsRigDef = getActivePhysicsRigDef(network);

  if (physicsRigDef != NULL)
  {
    if (m_physicsRigType == MR::PhysicsRigPhysX3::TYPE_ARTICULATED)
    {
      
      NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(
        MR::PhysicsRigPhysX3Articulation::getMemoryRequirements(physicsRigDef));
        
      //NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(MR::PhysicsRigPhysX3Articulation::getMemoryRequirements(physicsRigDef));
      NMP_ASSERT(resource.ptr);
      MR::PhysicsRigPhysX3Articulation* physicsRig = MR::PhysicsRigPhysX3Articulation::init(
        resource,
        physicsRigDef,
        getPhysicsScene(),
        physx::PX_DEFAULT_CLIENT,
        15, // all client behaviour bits
        animRigDef,
        getAnimToPhysicsMap(network->getNetworkDef(), network->getActiveAnimSetIndex()),
        1 << MR::GROUP_CHARACTER_PART,
        (1 << MR::GROUP_CHARACTER_CONTROLLER) | (1 << MR::GROUP_NON_COLLIDABLE) | (1 << MR::GROUP_INTERACTION_PROXY));
      physicsRig->setKinematicPos(*initialPosition);
      setPhysicsRig(network, physicsRig);

      uint32_t numParts = physicsRig->getNumParts();
      for (uint32_t i = 0; i != numParts; ++i)
      {
        MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation* part = physicsRig->getPartPhysX3Articulation(i);
        physx::PxActor* actor = part->getArticulationLink();
        assignPhysicsIDToActor(actor);
      }
    }
    else
    {
      NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(
        MR::PhysicsRigPhysX3Jointed::getMemoryRequirements(physicsRigDef)); 
      NMP_ASSERT(resource.ptr);
      MR::PhysicsRigPhysX3Jointed* physicsRig = MR::PhysicsRigPhysX3Jointed::init(
        resource,
        physicsRigDef,
        getPhysicsScene(),
        physx::PX_DEFAULT_CLIENT,
        15, // all client behaviour bits
        animRigDef,
        getAnimToPhysicsMap(network->getNetworkDef(), network->getActiveAnimSetIndex()),
        1 << MR::GROUP_CHARACTER_PART,
        (1 << MR::GROUP_CHARACTER_CONTROLLER) | (1 << MR::GROUP_NON_COLLIDABLE) | (1 << MR::GROUP_INTERACTION_PROXY));
      physicsRig->setKinematicPos(*initialPosition);
      setPhysicsRig(network, physicsRig);

      uint32_t numParts = physicsRig->getNumParts();
      for (uint32_t i = 0; i != numParts; ++i)
      {
        MR::PhysicsRigPhysX3Jointed::PartPhysX3Jointed* part = physicsRig->getPartPhysXJointed(i);
        physx::PxActor* actor = part->getRigidDynamic();
        assignPhysicsIDToActor(actor);
      }
    }
  }
  else
  { // There is no physics rig in this anim set.
    setPhysicsRig(network, NULL);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::destroyPhysicsRig(MR::Network *network)
{
  NMP_ASSERT(network);

  // Check whether this network has a physics rig
  MR::PhysicsRig* physicsRigExists = getPhysicsRig(network);

  if (physicsRigExists)
  {
    MR::PhysicsRigDef* physicsRigDef = getActivePhysicsRigDef(network);
    if (physicsRigDef)
    {
      MR::PhysicsRigPhysX3* physicsRigPhysX3 = (MR::PhysicsRigPhysX3*) getPhysicsRig(network);

      if (physicsRigPhysX3)
      {
        uint32_t numParts = physicsRigPhysX3->getNumParts();
        for (uint32_t i = 0; i != numParts; ++i)
        {
          physx::PxActor* actor = ((MR::PhysicsRigPhysX3::PartPhysX3*) physicsRigPhysX3->getPart(i))->getRigidBody();
          unassignPhysicsID(actor);
        }

        physicsRigPhysX3->term();
        NMP::Memory::memFree(physicsRigPhysX3);
      }

      setPhysicsRig(network, NULL);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updateConnections(
  MCOMMS::InstanceID NMP_UNUSED(instanceID),
  float              NMP_UNUSED(deltaTime))
{
  NET_LOG_ENTER_FUNC(); 
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePreController(
  MCOMMS::InstanceID instanceID,
  float              deltaTime)
{
  NET_LOG_ENTER_FUNC();
  ControllerRecord* charControllerRecord = m_characterControllerManager->getControllerRecord(instanceID);

  NMP_ASSERT(m_characterControllerManager);

  if (charControllerRecord->m_network->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
  {
    m_characterControllerManager->disableCollision(instanceID);
  }
  else
  {
    NET_LOG_MESSAGE(99, "NetworkInstanceRecord::updatePrePhysics: Updating trajectory\n");
    if (!m_characterControllerManager->getCollisionEnabled(instanceID))
    {
      m_characterControllerManager->enableCollision(instanceID);
    }

    // trajectories should be done now - get them and set in the controller
    charControllerRecord->m_deltaTranslation = charControllerRecord->m_network->getTranslationChange();
    charControllerRecord->m_deltaOrientation = charControllerRecord->m_network->getOrientationChange();

    NMP::Vector3 charDeltaTranslation = charControllerRecord->m_characterOrientation.rotateVector(charControllerRecord->m_deltaTranslation);
    charControllerRecord->m_characterOrientationOld = charControllerRecord->m_characterOrientation;
    charControllerRecord->m_characterOrientation *= charControllerRecord->m_deltaOrientation;
    charControllerRecord->m_characterOrientation.normalise();

    m_context->getNetworkInstanceManager()->findInstanceRecord(instanceID)->setRootTransform(
      NMP::Matrix34(charControllerRecord->m_characterOrientation, charControllerRecord->m_characterPosition));

    charControllerRecord->m_velocityInGravityDirectionDt = deltaTime;

#ifdef ANIM_GRAVITY
    if (charControllerRecord->m_overrideBasics.m_currentVerticalMoveState != MR::CC_STATE_VERTICAL_MOVEMENT_GRAVITY)
    {
        // Don't apply gravity to the CC but set the velocity in gravity direction to the component of the animation
        // delta in the gravity direction so that CC will inherit anim's velocity when switching back to the default
        // (gravity enabled) state.

        // Find the direction of gravity.
        NMP::Vector3 gravityDirection(getPhysicsScene()->getGravity());
        gravityDirection.fastNormalise();
        
        // Set the velocity in the gravity direction.
        charControllerRecord->m_velocityInGravityDirection =
          gravityDirection * charDeltaTranslation.dot(gravityDirection) * deltaTime;
    }
    else
    {
      // apply gravity in real world units
      MCOMMS::EnvironmentManagementInterface* environmentManager = MCOMMS::getRuntimeTarget()->getEnvironmentManager();
      NMP_ASSERT(environmentManager);

      bool gravityEnabled = true;
      MCOMMS::Attribute* gravityEnabledAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED);

      if (gravityEnabledAttr != 0)
      {
        gravityEnabled = (*(int*)gravityEnabledAttr->getData() != 0);
      }
      gravityEnabled &= (charControllerRecord->m_overrideBasics.m_currentVerticalMoveState == MR::CC_STATE_VERTICAL_MOVEMENT_GRAVITY);

      if (gravityEnabled)
      {
        NMP::Vector3 gravity = getPhysicsScene()->getGravity();
        charControllerRecord->m_velocityInGravityDirection += (gravity * deltaTime);
        charDeltaTranslation += (charControllerRecord->m_velocityInGravityDirection * deltaTime);
      }
    }
#endif

    // this should overwrite our root translation via setRootFromCharacterController when the manager
    // updates anyway
    m_characterControllerManager->setRequestedMovement(instanceID, charDeltaTranslation);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePrePhysics(
  MCOMMS::InstanceID instanceID, 
  float deltaTime,
  bool updatePhysicsRig)
{
  NET_LOG_ENTER_FUNC();
#if defined(NM_DEBUG)
  // For detecting memory leaks
#define NODE_MEMORY_DEBUGGINGx
#ifdef NODE_MEMORY_DEBUGGING
  ControllerRecord* ccmRecord = m_characterControllerManager->getControllerRecord(instanceID);
  ccmRecord->m_network->attribNodeAccounting(10000);
#endif // NODE_MEMORY_DEBUGGING
#endif // NM_DEBUG

  ControllerRecord *charControllerRecord = m_characterControllerManager->getControllerRecord(instanceID);

  if(updatePhysicsRig)
  {
    if (getPhysicsRig(charControllerRecord->m_network))
    {
      getPhysicsRig(charControllerRecord->m_network)->updatePrePhysics(deltaTime);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePostPhysicsInit(
  MCOMMS::InstanceID instanceID, 
  float              deltaTime)
{  
  ControllerRecord *charControllerRecord = m_characterControllerManager->getControllerRecord(instanceID);

  if (getPhysicsRig(charControllerRecord->m_network))
    getPhysicsRig(charControllerRecord->m_network)->updatePostPhysics(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePostPhysics(
  MCOMMS::InstanceID instanceID,
  float              deltaTime,
  bool               NMP_UNUSED(updatePhysicsRig))
{
  NET_LOG_ENTER_FUNC();
  ControllerRecord* charControllerRecord = m_characterControllerManager->getControllerRecord(instanceID);

  if (charControllerRecord->m_network->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
  {
    NET_LOG_MESSAGE(99, "NetworkInstanceRecord::updatePostPhysics: Updating trajectory");
    // Update the root translation and orientation.
    // NOTE: When using physics the root needs to be updated in updatePostPhysics so that the network
    // has the new root location, and can thus convert from world to local space. This required removing
    // setting from the root position from SceneObjectRecord::update (since that would be too late)
    charControllerRecord->m_deltaOrientation = charControllerRecord->m_network->getOrientationChange();
    charControllerRecord->m_deltaTranslation = charControllerRecord->m_network->getTranslationChange();
    NMP::Vector3 charDeltaTranslation = charControllerRecord->m_characterOrientation.rotateVector(charControllerRecord->m_deltaTranslation);
    NMP::Vector3 newPos = charControllerRecord->m_characterPosition + charDeltaTranslation;

    // make the root follow the ragdoll
    m_characterControllerManager->setPosition(instanceID, newPos);
    charControllerRecord->m_characterPosition = newPos;
    charControllerRecord->m_characterOrientationOld = charControllerRecord->m_characterOrientation;
    charControllerRecord->m_characterOrientation *= charControllerRecord->m_deltaOrientation;
    charControllerRecord->m_characterOrientation.normalise();
    charControllerRecord->m_network->updateCharacterPropertiesWorldRootTransform(
      NMP::Matrix34(charControllerRecord->m_characterOrientation, charControllerRecord->m_characterPosition),
      true);
    
    // Find the component of delta translation in the gravity direction.
    NMP::Vector3 gravity = getPhysicsScene()->getGravity();
    NMP::Vector3 gravityDirectionVector;
    gravityDirectionVector.normaliseDep(gravity);
    float deltaTranslationInGravityDirection = gravityDirectionVector.dot(charDeltaTranslation);

    // Calculate velocity in gravity direction
    if (deltaTime > 0.0f)
    {
      charControllerRecord->m_velocityInGravityDirection = gravityDirectionVector *
          (deltaTranslationInGravityDirection / deltaTime);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::setRoot(
  MCOMMS::InstanceID  instanceID,
  const NMP::Vector3& NMP_UNUSED(pos))
{
  // Set the position of the network root.  This function is called from updateControllers(), which
  // has already taken the movement requested by the animation system and filtered it through the
  // PxController's fake physics (if the controller is switched on, or just keyframed the animation along
  // if not).  It has also calculated whether the controller is on the ground or in the air, and whether
  // it has just transitioned between the two.  The ccmRecord's m_characterPosition is at this stage set
  // to point to the foot centre position as processed by the above.

  // TODO: convert to local space
  ControllerRecord* charControllerRecord = m_characterControllerManager->getControllerRecord(instanceID);
  NMP::Vector3 actualTranslation = (charControllerRecord->m_characterPosition -
                                    charControllerRecord->m_characterPositionOld);
  MCOMMS::EnvironmentManagementInterface* environmentManager = MCOMMS::getRuntimeTarget()->getEnvironmentManager();
  NMP_ASSERT(environmentManager);

  if (charControllerRecord->m_network->getRootControlMethod() != MR::Network::ROOT_CONTROL_PHYSICS)
  {
    // record the movement of the entity - but don't add on vertical velocity due to stepping.
    bool gravityEnabled = true;
    MCOMMS::Attribute* gravityEnabledAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_GRAVITY_ENABLED);
    if (gravityEnabledAttr)
      gravityEnabled = (*(int*)gravityEnabledAttr->getData() != 0);
    gravityEnabled &= (charControllerRecord->m_overrideBasics.m_currentVerticalMoveState == MR::CC_STATE_VERTICAL_MOVEMENT_GRAVITY);

    // Find the component of actual translation in the gravity direction.
    NMP::Vector3 gravity = getPhysicsScene()->getGravity();
    NMP::Vector3 gravityDirectionVector;
    gravityDirectionVector.normaliseDep(gravity);
    float actualTranslationInGravityDirection = gravityDirectionVector.dot(actualTranslation);

    // Find the component of delta translation in the gravity direction.
    float deltaTranslationInGravityDirection = gravityDirectionVector.dot(charControllerRecord->m_deltaTranslation);

    if (actualTranslationInGravityDirection > deltaTranslationInGravityDirection && gravityEnabled == true)
    {
      // Find the component of delta translation in the gravity direction.
      charControllerRecord->m_velocityInGravityDirection = actualTranslation - charControllerRecord->m_deltaTranslation;
      deltaTranslationInGravityDirection = gravityDirectionVector.dot(charControllerRecord->m_velocityInGravityDirection);

      // Calculate velocity in gravity direction
      if (charControllerRecord->m_velocityInGravityDirectionDt > 0)
      {
        charControllerRecord->m_velocityInGravityDirection =
          gravityDirectionVector * (deltaTranslationInGravityDirection / charControllerRecord->m_velocityInGravityDirectionDt);
      }
    }
    else
    {
      charControllerRecord->m_velocityInGravityDirection.setToZero();
    }

    // if stepping down then zero the velocity so that we run off cliff edges starting horizontal.
    if (m_characterControllerManager->getOnGround(instanceID))
      charControllerRecord->m_velocityInGravityDirection.setToZero();
  }

  charControllerRecord->m_deltaTranslation = actualTranslation;
  NMP::Quat oldOrientInv(charControllerRecord->m_characterOrientationOld);
  oldOrientInv.conjugate();
  charControllerRecord->m_deltaTranslation = oldOrientInv.rotateVector(charControllerRecord->m_deltaTranslation);

  charControllerRecord->m_network->updateCharacterPropertiesWorldRootTransform(
    NMP::Matrix34(charControllerRecord->m_characterOrientation, charControllerRecord->m_characterPosition),
    true);
}

//----------------------------------------------------------------------------------------------------------------------
IControllerMgr* DefaultPhysicsMgr::getControllerManager()
{
  return m_characterControllerManager;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updatePhysicalSceneObjects()
{
  const MCOMMS::SceneObjectManagementInterface* const sceneObjectManager =
    MCOMMS::getRuntimeTarget()->getSceneObjectManager();
  NMP_ASSERT(sceneObjectManager);

  if (sceneObjectManager)
  {
    std::vector<physx::PxActor*>& sceneActors = getSceneActors();
    size_t nActors = sceneActors.size();

    for (size_t i = 0 ; i < nActors ; ++i)
    {
      physx::PxActor* actor = sceneActors[i];
      if (!actor)
        continue;

      if (!actor->userData)
        continue;

      PhysicsUserData* userData = (PhysicsUserData*)actor->userData;
      if (!userData->isValid())
        continue;

      const MCOMMS::SceneObjectID objectID = userData->objectID;

      // Find the scene object.
      MCOMMS::SceneObject* const sceneObect = sceneObjectManager->getSceneObject(objectID);
      if (sceneObect)
      {
        MCOMMS::Attribute* const transformAttribute = sceneObect->getAttribute(MCOMMS::Attribute::SEMANTIC_TRANSFORM);
        if (transformAttribute)
        {
          const NMP::Matrix34 transform = MR::nmPxTransformToNmMatrix34(actor->isRigidActor()->getGlobalPose());
          *(NMP::Matrix34*)transformAttribute->getData() = transform;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updateSceneObjects(float deltaTime)
{
  m_context->getSceneObjectManager()->updateSceneObjectRecords(
    deltaTime,
    m_context->getNetworkInstanceManager(),
    getControllerManager());
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::updateControllers(float deltaTime)
{
  m_characterControllerManager->updateControllers(deltaTime);
}

namespace
{
#ifdef NM_PROFILING
//----------------------------------------------------------------------------------------------------------------------
void recursiveTraverseRecords(NMP::Profiler* profiler, const NMP::Profiler::ProfilerRecord* record, size_t depth, void (*processRecord)(NMP::Profiler*, const NMP::Profiler::ProfilerRecord*, size_t))
{
  (*processRecord)(profiler, record, depth);
  const NMP::Profiler::ProfilerRecord* end = profiler->getRecordChildrenEnd(record);
  for (const NMP::Profiler::ProfilerRecord* child = profiler->getRecordChildrenBegin(record); child != end; child = profiler->getRecordNextSibling(child))
  {
    recursiveTraverseRecords(profiler, child, depth + 1, processRecord);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void printRecord(NMP::Profiler* profiler, const NMP::Profiler::ProfilerRecord* record, size_t depth)
{
  for (uint32_t tab = 1; tab < depth; ++tab)
  {
    LOG_PROFILING_MESSAGE(" | ");
  }
  if (depth)
  {
    LOG_PROFILING_MESSAGE(" |_");
  }
  else
  {
    g_maxTime = (profiler->getRecordTime(record) > g_maxTime) ? profiler->getRecordTime(record): g_maxTime;
    g_totalTiming += profiler->getRecordTime(record);
    LOG_PROFILING_MESSAGE("Average time: %6.3f ms\n", g_totalTiming/++g_totalSamples);
    LOG_PROFILING_MESSAGE("Peak time: %6.3f ms\n", g_maxTime);
  }
  LOG_PROFILING_MESSAGE("%6.3f ms - %s\n", profiler->getRecordTime(record), profiler->getRecordBlock(record)->getTag());
}

//----------------------------------------------------------------------------------------------------------------------
void gatherProfilingData(NMP::Profiler* profiler)
{
  const size_t threadCount = profiler->getNumThreads();
  for (size_t threadIndex = 0; threadIndex != threadCount; ++threadIndex)
  {
    LOG_PROFILING_MESSAGE("=================================================\n");

    const NMP::Profiler::ProfilerRecord* rootRecord = profiler->getRootRecord(threadIndex);

    if (rootRecord)
    {
      recursiveTraverseRecords(profiler, rootRecord, 0, &printRecord);
    }
  }
}
#endif // NM_PROFILING
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::update(float deltaTime)
{
  if (!m_physicsScene || !m_characterControllerManager)
  {
    // No need to update if there isn't a physics scene or controller manager.  There might not be one if the 
    // runtime target is not simulating a scene.
    return;
  }

  NM_BEGIN_PROFILING("------- Update Root -------");

  // Update all instances
  m_context->getNetworkInstanceManager()->applyAnimationSetChanges();

  PHYSICS_LOG_LINE_DIVIDE();
  NM_BEGIN_PROFILING("setNextPhysicsTimeStep");
  m_physicsScene->setNextPhysicsTimeStep(getActualTimestep(deltaTime));
  NM_END_PROFILING(); //"setNextPhysicsTimeStep"

  m_characterControllerManager->updateInstanceNetworksBegin(deltaTime);

  PHYSICS_LOG_MESSAGE("updateInstancesConnections");
  NM_BEGIN_PROFILING("updateInstancesConnections");
  m_characterControllerManager->updateInstancesConnections(deltaTime);
  NM_END_PROFILING(); //"updateInstancesConnections"

  m_characterControllerManager->updateInstanceNetworksContinue(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER);

  PHYSICS_LOG_MESSAGE("updateInstancesPreController");
  NM_BEGIN_PROFILING("updateInstancesPreController");
  m_characterControllerManager->updateInstancesPreController(deltaTime);
  NM_END_PROFILING(); //"updateInstancesPreController"

  if (m_physicsAndCharacterControllerUpdate == MR::PHYSICS_AND_CC_UPDATE_SEPARATE)
  {
    PHYSICS_LOG_MESSAGE("updating character controllers");
    NM_BEGIN_PROFILING("updateControllers");
    m_characterControllerManager->updateControllers(deltaTime);
    NM_END_PROFILING(); //"updateControllers"
  }

  m_characterControllerManager->updateInstanceNetworksContinue(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS);

  PHYSICS_LOG_MESSAGE("updateInstancesPrePhysics");
  NM_BEGIN_PROFILING("updateInstancesPrePhysics");
  m_characterControllerManager->updateInstancesPrePhysics(deltaTime);
  NM_END_PROFILING(); //"updateInstancesPrePhysics"


  if (m_physicsAndCharacterControllerUpdate != MR::PHYSICS_AND_CC_UPDATE_SEPARATE)
  {
    PHYSICS_LOG_MESSAGE("updating character controllers");
    NM_BEGIN_PROFILING("updateControllers");
    m_characterControllerManager->updateControllers(deltaTime);
    NM_END_PROFILING(); //"updateControllers"
  }

  // update scene object pre-physics
  PHYSICS_LOG_MESSAGE("Stepping Physics");
  NM_BEGIN_PROFILING("simulate");
  simulate(deltaTime);
  NM_END_PROFILING(); //"simulate"

  m_characterControllerManager->updateInstancesPostPhysicsInit(deltaTime);
  m_characterControllerManager->updateInstanceNetworksContinue(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT);

  PHYSICS_LOG_MESSAGE("updateInstancesPostPhysics");
  NM_BEGIN_PROFILING("updateInstancesPostPhysics");
  m_characterControllerManager->updateInstancesPostPhysics(deltaTime);
  NM_END_PROFILING(); //"updateInstancesPostPhysics"

  m_characterControllerManager->updateInstanceNetworksContinue(MR::TASK_ID_UNSPECIFIED);
  // we don't expect any external tasks, or any physics related tasks, to be left here
  m_characterControllerManager->updateInstanceNetworksEnd();

  PHYSICS_LOG_MESSAGE("updateSceneObjects");

  // update the physics objects
  NM_BEGIN_PROFILING("updateSceneObjectsFromPhysics");
  updatePhysicalSceneObjects();
  NM_END_PROFILING(); //"updateSceneObjectsFromPhysics"

  // Update all objects
  NM_BEGIN_PROFILING("updateSceneObjects");
  updateSceneObjects(deltaTime);
  NM_END_PROFILING(); //"updateSceneObjects"

  // Update the Connect-side representations of the character controllers
  m_characterControllerManager->updateControllerRepresentations();

  NM_END_PROFILING(); //"updateInstances"

#ifdef MR_OUTPUT_DEBUGGING
  m_characterControllerManager->sendInstanceProfileTimingDebugOutput();
#endif // MR_OUTPUT_DEBUGGING

#ifdef NM_PROFILING
  NMP::Profiler::getProfiler()->endProfilingFrame();
  gatherProfilingData(NMP::Profiler::getProfiler());
  NMP::Profiler::getProfiler()->reset();
#endif // NM_PROFILING

  ++m_frameIndex;

  return;
}

namespace
{

//----------------------------------------------------------------------------------------------------------------------
void incPhysicsAssetRefCounts(MR::NetworkDef* networkDef)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (MR::AnimSetIndex animSetIdx = 0; animSetIdx < numAnimSets; ++animSetIdx)
  {
    // Physics Rigs.
    MR::PhysicsRigDef* physicsRig = getPhysicsRigDef(networkDef, animSetIdx);
    if (physicsRig)
    {
      uint32_t refCount = MR::Manager::incObjectRefCount(physicsRig);
      NM_LOG_MESSAGE(
        RTT_LOGGER,
        RTT_MESSAGE_PRIORITY,
        "  Increasing RefCount - PhysicsRigDef AssetID: %p, RefCount: %d\n",
        MR::Manager::getInstance().getObjectIDFromObjectPtr(physicsRig),
        refCount);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void decPhysicsAssetRefCounts(MR::NetworkDef* networkDef)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (MR::AnimSetIndex animSetIdx = 0; animSetIdx < numAnimSets; ++animSetIdx)
  {
    // Physics Rigs.
    MR::PhysicsRigDef* physicsRig = getPhysicsRigDef(networkDef, animSetIdx);
    if (physicsRig)
    {
      uint32_t refCount = MR::Manager::decObjectRefCount(physicsRig);
      NM_LOG_MESSAGE(
        RTT_LOGGER,
        RTT_MESSAGE_PRIORITY,
        "    Decreasing RefCount - PhysicsRigDef AssetID: %p, RefCount: %d\n",
        MR::Manager::getInstance().getObjectIDFromObjectPtr(physicsRig),
        refCount);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void deleteOrphanedPhysicsAsset(MR::NetworkDef* networkDef)
{
  uint32_t numAnimSets = networkDef->getNumAnimSets();
  for (uint16_t animSetIndex = 0 ; animSetIndex < numAnimSets ; ++animSetIndex)
  {
    //----------
    // Physics Rig.
    MR::PhysicsRigDef* physicsRig = getPhysicsRigDef(networkDef, animSetIndex);
    if (physicsRig && (MR::Manager::getInstance().getObjectRefCount(physicsRig) == 0))
    {
      NM_LOG_MESSAGE(
        RTT_LOGGER,
        RTT_MESSAGE_PRIORITY,
        "  Deleting referenced PhysicsRigDef - AssetID: %p.\n",
        MR::Manager::getInstance().getObjectIDFromObjectPtr(physicsRig));
      MR::Manager::getInstance().unregisterObject(physicsRig);
      NMP::Memory::memFree(physicsRig);
    }
  }
}

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::initializePhysicsCore(uint32_t numDispatchers, MR::Dispatcher** dispatchers)
{
  // Engine specific assets
  MR::Manager::getInstance().registerAsset(MR::Manager::kAsset_PhysicsRigDef, MR::locatePhysicsRigDefPhysX3);

  MR::initMorphemePhysics(numDispatchers, dispatchers);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::finaliseInitPhysicsCore()
{
  MR::finaliseInitMorphemePhysics();
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxActor* DefaultPhysicsMgr::getActorByPhysicsID(MR::PhysicsObjectID id) const
{
  PhysXActor* actor = 0;
  m_physicsIDActorMap.find(id, &actor);
  return actor;
}

//----------------------------------------------------------------------------------------------------------------------
void* DefaultPhysicsMgr::getPhysicsObjectPointerFromPhysicsID(MR::PhysicsObjectID id) const
{
  return getActorByPhysicsID(id);
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::getPhysicsObjectIDFromPhysicsObjectPointer(void* physicsObject) const
{
  return getPhysicsIDForActor((const physx::PxActor*) physicsObject);
}


//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::getPhysicsIDForPart(const MR::PhysicsRig::Part* part) const
{
  physx::PxActor* actor = ((MR::PhysicsRigPhysX3::PartPhysX3*) part)->getRigidBody();
  return getPhysicsIDForActor(actor);
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::getPhysicsIDForActor(const physx::PxActor* actor) const
{
  MR::PhysicsObjectID id = MR_INVALID_PHYSICS_OBJECT_ID;
  m_actorPhysicsIDMap.find(const_cast<PhysXActor*>(actor), &id);
  return id;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::resetSDKS()
{
  deleteScene();

  float assetScale = 1.0f;

  MCOMMS::RuntimeTargetInterface* runtimeTarget = MCOMMS::getRuntimeTarget();
  if (runtimeTarget)
  {
    MCOMMS::EnvironmentManagementInterface* environmentManager = runtimeTarget->getEnvironmentManager();
    MCOMMS::Attribute* assetScaleAttr = environmentManager->getEnvironmentAttribute(MCOMMS::Attribute::SEMANTIC_ASSET_SCALE);
    NMP_ASSERT(assetScaleAttr);
    if (assetScaleAttr)
    {
      assetScale = *((float*)assetScaleAttr->getData());
    }
  }
  m_toleranceScalingValue = assetScale;

  PhysicsSDK::init(m_toleranceScalingValue);

  m_maxTimeStep = 1.0f / 15.0f;

  physx::PxVec3 gravity(0, -9.81f, 0);

  // create the scene
  physx::PxSceneDesc sceneDesc(PxGetPhysics().getTolerancesScale());

  // task dispatchers - pass in the number of worker threads
  m_cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(0);
  sceneDesc.cpuDispatcher = m_cpuDispatcher; 

#ifdef NM_HOST_CELL_PPU
  physx::PxPS3Ext& ps3ext = physx::PxGetPS3Extension();

  CellSpurs2* spurs = ps3ext.createSpurs(5, false);

  uint8_t simPrio[8] = {2,2,2,2, 2,2,2,2};
  CellSpursTaskset2* simulationTaskset = ps3ext.createTaskset(*spurs,5,simPrio);

  uint8_t queryPrio[8] = {1,1,1,1, 1,1,1,1};
  CellSpursTaskset2* queryTaskset = ps3ext.createTaskset(*spurs,5,queryPrio);

  bool physXInitResult = physx::PxPS3Control::initializePhysXWithTaskset(simulationTaskset, queryTaskset,5,5);

  physx::pxtask::SpuDispatcher* spuDispatcher = PxDefaultSpuDispatcherCreate(simulationTaskset, queryTaskset);

  sceneDesc.spuDispatcher = spuDispatcher; 

  // We initialise the NMP SPU manager here with the SPURS object which PhysX creates, so it doesn't init SPURS again.
  if(!NMP::SPUManager::initialised())
  {
    NMP::SPUManager::init(NMP::SPUManager::MAX_SPUS, NULL, spurs, true);
  }
#endif

  // It is required to register either the morpheme filter shader, or one that is very similar
  // (perhaps one that has additional functionality).
  sceneDesc.filterShader = MR::morphemePhysX3FilterShader;
  // This is optional, in order to improve character self-collision. It has a slight performance
  // cost.
  sceneDesc.contactModifyCallback = &morphemePhysX3ContactModifyCallback; 
  sceneDesc.gravity      = gravity;
  physx::PxScene* scene = PxGetPhysics().createScene(sceneDesc);
  NMP_ASSERT(scene);
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(MR::PhysicsScenePhysX3), NMP_VECTOR_ALIGNMENT);
  m_physicsScene = new(alignedMemory) MR::PhysicsScenePhysX3(scene);
  scene->setClientBehaviorBits(0, 15); // Setting all the basic bits for now, see MORPH-11272

  // create CCM and SOM
  m_characterControllerManager = new DefaultControllerMgr(this, m_context);

  resetScene();

#ifdef NM_HOST_CELL_PPU
  // Disable SPU use to work around physx crashes. See MORPH-8393.
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_NARROWPHASE, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_ISLAND_GEN, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_DYNAMICS, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_CLOTH, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_HEIGHT_FIELD, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_RAYCAST, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_FLUID_PACKETS, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_FLUID_DYNAMICS, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_FLUID_COLLISION, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_FLUID_PARTICLES_UPDATE, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_FLUID_SPH, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_FLUID_HASH, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_NARROWPHASE1, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_NARROWPHASE2, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_NARROWPHASE3, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_NARROWPHASE4, 0);
  physx::PxPS3Config::setSceneParamInt(*scene, physx::PxPS3ConfigParam::eSPU_BROADPHASE, 0);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::onAssetScaleChanged(float assetScale)
{
  if (NMP::nmfabs(assetScale - m_toleranceScalingValue) > 0.000001f && m_sceneActors.empty() && m_joints.empty())
  {
    resetSDKS();
  }
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsObjectID DefaultPhysicsMgr::assignPhysicsIDToActor(physx::PxActor* actor)
{
  NMP_ASSERT(actor != 0);

  bool found = m_actorPhysicsIDMap.find(actor);
  if (found)
  {
    // already added to map
    return false;
  }

  // get a free id for this object
  MR::PhysicsObjectID actorID = m_nextPhysicsObjectID++;

  bool result = m_physicsIDActorMap.insert(actorID, actor);

  if (!result)
  {
    // insertion failed
    return MR_INVALID_PHYSICS_OBJECT_ID;
  }

  result = m_actorPhysicsIDMap.insert(actor, actorID);

  if (!result)
  {
    // insertion failed, remove the entry from m_physicsIdActorMap so it still contains the same
    // objects as m_actorPhysicsIdMap.
    m_physicsIDActorMap.erase(actorID);
    return MR_INVALID_PHYSICS_OBJECT_ID;
  }

  return actorID;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultPhysicsMgr::unassignPhysicsID(physx::PxActor* actor)
{
  NMP_ASSERT(actor != 0);

  MR::PhysicsObjectID actorId = MR_INVALID_PHYSICS_OBJECT_ID;

  bool found = m_actorPhysicsIDMap.find(actor, &actorId);
  if (!found)
  {
    // object was never given a physics object id.
    return false;
  }

  bool result = m_physicsIDActorMap.erase(actorId);
  if (!result)
  {
    // the maps got out of sync somehow
    return false;
  }

  m_actorPhysicsIDMap.erase(actor);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::clearAllPhysicsIDs()
{
  m_nextPhysicsObjectID = 0;
  m_physicsIDActorMap.clear();
  m_actorPhysicsIDMap.clear();
}

//----------------------------------------------------------------------------------------------------------------------
DefaultPhysicsMgr::Constraint* DefaultPhysicsMgr::getConstraint(uint64_t guid) const
{
  Constraint* constraint = 0;
  m_constraintMap.find(guid);
  return constraint;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultPhysicsMgr::deleteScene()
{
  clearScene();
  if (m_physicsScene)
  {
    m_cpuDispatcher->release();
    m_cpuDispatcher = 0;

    m_physicsScene->getPhysXScene()->release();
    NMP::Memory::memFree(m_physicsScene);
    m_physicsScene = 0;
  }

  delete m_characterControllerManager;
  m_characterControllerManager = 0;

  PhysicsSDK::term();
}
