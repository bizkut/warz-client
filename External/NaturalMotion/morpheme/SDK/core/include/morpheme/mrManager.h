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
#ifndef MR_MANAGER_H
#define MR_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrDefines.h"
#include "NMPlatform/NMStaticFreeList.h"

#include "sharedDefines/mVersion.h"
#include "morpheme/mrNode.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/Prediction/mrNetworkPredictionDef.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \mainpage Morpheme Runtime Animation System
///
/// Welcome to the documentation for the Morpheme Runtime Animation System.
/// This file is generated from documentation in code.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \namespace MR
/// \brief Morpheme Runtime animation system.
///
/// Encompasses all code for:
///   - Morpheme runtime API and libraries.
///   - Platform specific implementations.
///   - Tools for compilation of morpheme assets.
///   - Example applications.
/// This code is intended to be added to and amended by clients.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup CoreModule Core.
///
/// The classes, functions, data types and structures forming the basis of morpheme runtime.
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Function to locate an animation source
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE void locateDefaultAnimFormatFn(MR::AnimSourceBase* animSource)
{
  NMP_ASSERT(animSource);
  T* animFormat = static_cast<T*> (animSource);
  animFormat->locate();
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE void dislocateDefaultAnimFormatFn(MR::AnimSourceBase* animSource)
{
  NMP_ASSERT(animSource);
  T* animFormat = static_cast<T*> (animSource);
  animFormat->dislocate();
}

//----- Typedefs for register-able MessageData functions -----
typedef bool (*MessageDataLocateFn)(Message* target);
typedef bool (*MessageDataDislocateFn)(Message* target);

//----- Typedefs for register-able NodeInitData functions -----
typedef bool (*NodeInitDataLocateFn)(NodeInitData* target);
typedef bool (*NodeInitDataDislocateFn)(NodeInitData* target);

//----- Typedefs for register-able AttribData functions -----
typedef NMP::Memory::Format (*AttribOutputMemReqsFn)(Network* network);
typedef void (*AttribLocateFn)(AttribData* target);
typedef void (*AttribDislocateFn)(AttribData* target);
typedef void (*AttribRelocateFn)(AttribData* target, void* location);
typedef void (*AttribPrepForSpuFn)(AttribData* target, NMP::MemoryAllocator* allocator);

//----- Typedefs for register-able PredictionModelType functions -----
typedef void (*PredictionModelLocateFn)(PredictionModelDef* target);
typedef void (*PredictionModelDislocateFn)(PredictionModelDef* target);

/// Writes an AttribData in its output data format (data provided to debug tools such as connect) into the provided buffer.
/// Returns number of bytes written in to buffer.
/// If no buffer is provided returns number of bytes required to write output.
typedef uint32_t (*AttribSerializeTxFn) (MR::Network* net, MR::NodeID owningNodeID, AttribData* toOutput, void* outputBuffer, uint32_t outputBufferSize);

typedef bool (*AssetLocateFn)(uint32_t assetDesc, void* assetMemory);

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::Manager
/// \brief The central resource management class for Morpheme.
/// \ingroup CoreModule
///
/// Only one instance of this class should exist at any one time.
/// Assets are registered here and Networks access shared resources from here.
//----------------------------------------------------------------------------------------------------------------------
class Manager
{
public:
  /// \enum AssetType
  /// \brief NaturalMotion fundamental asset types.
  /// \ingroup
  enum AssetType
  {
    kAsset_Rig                    = 1,  ///<
    kAsset_RigToAnimMap           = 2,  ///<
    kAsset_EventTrackDiscrete     = 3,  ///<
    kAsset_EventTrackDuration     = 4,  ///<
    kAsset_EventTrackCurve        = 5,  ///<
    kAsset_PhysicsRigDef          = 6,  ///<
    kAsset_CharacterControllerDef = 7,  ///<
    kAsset_InteractionProxyDef    = 8,  ///<
    kAsset_BodyDef                = 9,  ///<
    kAsset_NetworkDef             = 10, ///<
    kAsset_NetworkPredictionDef   = 11, ///<  
    kAsset_PluginList             = 12, ///<
    kAsset_NumAssetTypes,               ///< Total number of defined asset types
    kAsset_Invalid                = 0xFFFFFFFF
  };

  /// \enum PlatformFormat
  enum PlatformFormat
  {
    kPlatform_BigEndian           = 1,  ///<
    kPlatform_LittleEndian        = 2,  ///<
    kPlatform_32Bit               = 4,  ///<
    kPlatform_64Bit               = 8,  ///<
    kPlatform_Invalid             = 0xFFFFFFFF
  };

  /// \brief
  static uint32_t getTargetPlatformAssetFmt()
  {
    uint32_t result = 0;
#if NM_TARGET_BIGENDIAN
    result |= kPlatform_BigEndian;
#else
    result |= kPlatform_LittleEndian;
#endif

#ifdef NM_HOST_64_BIT
    result |= kPlatform_64Bit;
#else
    result |= kPlatform_32Bit;
#endif
    return result;
  }

  /// \brief
  static uint32_t getRuntimeBinaryVersion() { return MR_VERSION_RUNTIME_BINARY; }

  /// \brief Typedef for the registered animation loading function.
  ///
  /// Allows the use of any source animation format with morpheme as long as it can inherit from and implement
  /// The required AnimBase interface.
  typedef MR::AnimSourceBase *(RequestAnimFn)(
    const RuntimeAnimAssetID, ///< GlobalID.
    void*                     ///< Userdata
  );

  /// \brief Typedef for the registered animation unloading function.
  ///
  /// Allows the use of any source animation format with morpheme as long as it can inherit from and implement
  /// The required AnimBase interface.
  typedef void (ReleaseAnimFn)(
    const RuntimeAnimAssetID, ///< GlobalID
    MR::AnimSourceBase*,      ///< Ptr to loadedAnimation.
    void*                     ///< Userdata
  );

  /// \brief Helper function providing simple initialization of the morpheme core library.
  ///
  /// Creates Manager instance. Registers task queuing functions, attribute data types etc with Manager.
  /// Accumulates manager registry requirements etc.
  static void initMorphemeLib();

  /// \brief Allocates Manager registry arrays, after computeRegistryRequirements has been used to determine the required array sizes.
  void allocateRegistry();
#ifdef NM_HOST_CELL_SPU
  /// \brief The same as allocateRegistry, but the already known requirements are passed to SPU from PPU.
  void allocateRegistrySPU(uint32_t highestRegisteredAttribSemantic);
#endif

  /// \brief Registration of semantics etc with the manager, post Manager::allocateRegistry().
  static void finaliseInitMorphemeLib();

  /// \brief Termination routine for morpheme lib.
  static void termMorphemeLib();

  /// Set the animation file handling functions that morpheme should use.
  void setAnimFileHandlingFunctions(
      RequestAnimFn* requestAnimFn, ///< Tells manager the function to call when a reference to an identified 
                                    ///<  animation is required by an animation set.
                                    ///<  External function may for example need to load the animation and
                                    ///<  perhaps increase a ref count on it.
      ReleaseAnimFn* releaseAnimFn  ///< Tells manager the function to call when a reference to an animation
                                    ///<  is no longer needed by an animation set.
                                    ///<  External function may for example need to decrease a ref count and
                                    ///<  possibly deallocate an animation.
  );

  /// \brief retrieves the function used to request animations
  RequestAnimFn* getRequestAnimFn() const;
  /// \brief retrieves the function used to release animations
  ReleaseAnimFn* getReleaseAnimFn() const;

  MR::AnimSourceBase* requestAnimation(RuntimeAnimAssetID animAssetID, void* userdata = NULL);
  void releaseAnimation(RuntimeAnimAssetID animAssetID, MR::AnimSourceBase* loadedAnimation, void* userdata = NULL);

  const void* getObjectPtrFromObjectID(ObjectID objectID);
  ObjectID getObjectIDFromObjectPtr(const void* ptr);
  AssetType getAssetTypeFromObjectPtr(const void* ptr);

  const char* getAssetTypeName(uint32_t NMP_UNUSED(assetType)) const { return "getAssetTypeName not implemented"; }

  /// \brief, note that this function will fail if an object with the same ID has already been registered.
  ///
  /// All assets to be used by Morpheme must be registered here.
  /// \return true if the object was successfully registered.
  bool registerObject(
    const void* ptr,      ///< Pointer to the asset structure.
    AssetType   typeID,   ///< Asset type.
    ObjectID    objectID  ///< Unique runtime asset ID.
  );
  bool unregisterObject(const void* ptr);
  bool unregisterObject(uint32_t objectID);
  bool objectIsRegistered(uint32_t objectID);
  AssetType getRegisteredObjectType(uint32_t objectID);

  /// \return Current ref count if successful or 0xFFFFFFFF if object does not exist.
  static uint32_t incObjectRefCount(const void* ptr);
  static uint32_t incObjectRefCount(ObjectID objectID);

  /// \return Current ref count if successful or 0xFFFFFFFF if object does not exist or is already zero.
  static uint32_t decObjectRefCount(const void* ptr);
  static uint32_t decObjectRefCount(ObjectID objectID);

  /// \brief Get current reference count of the specified Object/Asset.
  /// \return 0xFFFFFFFF if object does not exist in registry.
  static uint32_t getObjectRefCount(ObjectID objectID);
  static uint32_t getObjectRefCount(const void* ptr);

  /// \brief Register a queuing function against its ID.
  uint32_t registerTaskQueuingFn(QueueAttrTaskFn queuingFn, const char* fnName);
  QueueAttrTaskFn getTaskQueuingFn(uint32_t fnID) const;
  uint32_t getTaskQueuingFnID(QueueAttrTaskFn queuingFn) const;
  uint32_t getTaskQueuingFnID(const char* fnName) const;
  const char* getTaskQueuingFnName(QueueAttrTaskFn queuingFn) const;

  /// \brief Register an output control param task against its ID.
  uint32_t registerOutputCPTask(OutputCPTask outputCPTask, const char* fnName);
  OutputCPTask getOutputCPTask(uint32_t fnID) const;
  uint32_t getOutputCPTaskID(OutputCPTask outputCPTask) const;
  uint32_t getOutputCPTaskID(const char* fnName) const;
  const char* getOutputCPTaskName(OutputCPTask outputCPTask) const;

  /// \brief Register a node instance initialisation function against its ID.
  uint32_t registerInitNodeInstanceFn(InitNodeInstance initNodeFn, const char* fnName);
  InitNodeInstance getInitNodeInstanceFn(uint32_t fnID) const;
  uint32_t getInitNodeInstanceFnID(InitNodeInstance initNodeFn) const;
  uint32_t getInitNodeInstanceFnID(const char* fnName) const ;
  const char* getInitNodeInstanceFnName(InitNodeInstance initNodeFn) const;
  
  /// \brief Used either to determine how many semantics will be registered or to actually store semantic info in the registry.
  void registerAttributeSemanticID(
    uint32_t    id,
    const char* semanticName,                 ///< Only  used for debug purposes.
                                              ///<  Note that a direct pointer to this data will be cached internally,
                                              ///<  so be careful if it is not statically allocated.
    bool        computeRegistryRequirements,  ///< Registartion of semantics is 2 phase:
                                              ///<  1. pre allocateRegistry: registerAttributeSemanticID calls are used to determine the number of semantics that will be registered.
                                              ///<  2. pre allocateRegistry: registerAttributeSemanticID calls store semantic info in the allocated manager arrays.
    AttribSemanticSense attribSemanticSense); ///< The directional sense of the attrib data (i.e. does it get passed from parent to child, or child to parent).

  uint32_t getHighestRegisteredAttribSemantics() const;
  uint32_t getNumRegisteredAttribSemantics() const;
  const char* getAttributeSemanticName(uint32_t id) const;
  AttribSemanticSense getAttributeSemanticSense(uint32_t id) const;

  /// \brief
  TaskID getCreateReferenceToInputTaskID(uint32_t semantic_id) const;
  void setCreateReferenceToInputTaskID(uint32_t semantic_id, TaskID task_id);

  /// \brief Register delete function against ID.
  uint32_t registerDeleteNodeInstanceFn(DeleteNodeInstance deleteFn, const char* fnName);
  DeleteNodeInstance getDeleteNodeInstanceFn(uint32_t fnID) const;
  uint32_t getDeleteNodeInstanceFnID(DeleteNodeInstance deleteFn) const;
  uint32_t getDeleteNodeInstanceFnID(const char* fnName) const;
  const char* getDeleteNodeInstanceFnName(DeleteNodeInstance deleteFn) const;
  
  /// \brief Register update node connectivity function against ID.
  uint32_t registerUpdateNodeConnectionsFn(UpdateNodeConnections updateNodeConnectionsFn, const char* fnName);
  UpdateNodeConnections getUpdateNodeConnectionsFn(uint32_t fnID) const;
  uint32_t getUpdateNodeConnectionsFnID(UpdateNodeConnections updateNodeConnectionsFn) const;
  uint32_t getUpdateNodeConnectionsFnID(const char* fnName) const;
  const char* getUpdateNodeConnectionsFnName(UpdateNodeConnections updateNodeConnectionsFn) const;
  
  /// \brief Register node generates attrib semantic type function against ID.
  uint32_t registerFindGeneratingNodeForSemanticFn(FindGeneratingNodeForSemanticFn findGeneratingNodeForSemanticFn, const char* fnName);
  FindGeneratingNodeForSemanticFn getFindGeneratingNodeForSemanticFn(uint32_t fnID) const;
  uint32_t getFindGeneratingNodeForSemanticFnID(FindGeneratingNodeForSemanticFn findGeneratingNodeForSemanticFn) const;
  uint32_t getFindGeneratingNodeForSemanticFnID(const char* fnName) const;
  const char* getFindGeneratingNodeForSemanticFnName(FindGeneratingNodeForSemanticFn findGeneratingNodeForSemanticFn) const;

  /// \brief Register a message type.
  bool registerMessageDataType(
    MessageType              messageType,
    MessageDataLocateFn      locateFn,
    MessageDataDislocateFn   dislocateFn);
  MessageDataLocateFn getMessageDataLocateFn(MessageType messageType);
  MessageDataDislocateFn getMessageDataDislocateFn(MessageType messageType);

  /// \brief Register message handler function against ID.
  uint32_t registerMessageHandlerFn(MessageHandlerFn messageHandlerFn, const char* fnName);
  MessageHandlerFn getMessageHandlerFn(uint32_t fnID) const;
  uint32_t getMessageHandlerFnID(MessageHandlerFn messageHandlerFn) const;
  uint32_t getMessageHandlerFnID(const char* fnName) const;
  const char* getMessageHandlerFnName(MessageHandlerFn messageHandlerFn) const;
  
  //---------------
  /// \brief Register an NodeInitData type's location and dislocation functions.
  bool registerNodeInitDataType(
    NodeInitDataType          initDataType,
    NodeInitDataLocateFn      locateFn,
    NodeInitDataDislocateFn   dislocateFn);
  NodeInitDataLocateFn getNodeInitDataLocateFn(NodeInitDataType initDataType);
  NodeInitDataDislocateFn getNodeInitDataDislocateFn(NodeInitDataType initDataType);
  /// \brief Register asset.

  uint32_t registerAsset(uint32_t assetType, AssetLocateFn locateFn);

  /// \brief Retrieve the locate function for the asset.
  AssetLocateFn getAssetLocateFn(MR::Manager::AssetType assetType);

  //---------------
  /// \brief Register an AttribData type's functions functions.
  void registerAttrDataType(
    AttribDataType        attribType,
    const char*           attribTypeName = NULL, ///< Only  used for debug purposes.
                                                 ///<  Note that a direct pointer to this data will be cached internally,
                                                 ///<  so be careful if it is not statically allocated.
    AttribLocateFn        locateFn = NULL,
    AttribDislocateFn     dislocateFn = NULL,
    AttribOutputMemReqsFn outputMemReqsFn = NULL,
    AttribRelocateFn      relocateFn = NULL,
    AttribPrepForSpuFn    prepForSpuFn = NULL,   ///< Only used by ManagerSPU.
    AttribSerializeTxFn   srlzTxFn = NULL        ///< Writes the AttribData to a buffer ready for transmission to a debug tool such as connect.
  );

  uint32_t getHighestRegisteredAttribDataType() { return m_highestRegisteredAttribDataType; }
  bool isAttribDataTypeRegistered(AttribDataType attribType);
  AttribLocateFn getAttribLocateFn(AttribDataType attribType);
  AttribDislocateFn getAttribDislocateFn(AttribDataType attribType);
  AttribOutputMemReqsFn getAttribOutputMemReqsFn(AttribDataType attribType);
  AttribRelocateFn getAttribRelocateFn(AttribDataType attribType);
  AttribPrepForSpuFn getAttribPrepForSpuFn(AttribDataType attribType);
  AttribSerializeTxFn getAttribSerializeTxFn(AttribDataType attribType);

  /// Generally only for debug use.
  const char* getAttribTypeName(AttribDataType attribType);
  
  //---------------
  /// \brief Register a TransitCondition type's location and dislocation functions.
  bool registerTransitCondType(
    TransitConditType               conditType,
    TransitCondDefLocateFn          locateFn,
    TransitCondDefDislocateFn       dislocateFn,
    TransitCondInstanceRelocateFn   relocateFn,
    TransitCondInstanceGetMemReqsFn memReqsFn,
    TransitCondInstanceInitFn       initFn,
    TransitCondInstanceUpdateFn     updateFn,
    TransitCondInstanceQueueDepsFn  queueDepsFn,
    TransitCondInstanceResetFn      resetFn);

  struct TransitCondRegistryEntry
  {
    TransitCondInstanceGetMemReqsFn m_memReqsFn;
    TransitCondInstanceInitFn       m_initFn;
    TransitCondInstanceUpdateFn     m_updateFn;
    TransitCondInstanceQueueDepsFn  m_queueDepsFn;
    TransitCondInstanceResetFn      m_resetFn;
    TransitCondDefLocateFn          m_locateFn;
    TransitCondDefDislocateFn       m_dislocateFn;
    TransitCondInstanceRelocateFn           m_relocateFn;
    TransitConditType               m_type;
  };
  TransitCondRegistryEntry* getTransitCondRegistryEntry(TransitConditType conditType);

  //---------------------------------------------------------------------
  /// \name   Animation format registry
  /// \brief  Functions to register animation compression formats
  ///         with the runtime manager
  //---------------------------------------------------------------------
  //@{
  typedef void (*LocateAnimFormatFn)(MR::AnimSourceBase*);
  typedef void (*DislocateAnimFormatFn)(MR::AnimSourceBase*);

  /// \struct AnimationFormatRegistryEntry
  /// \brief Contains the animation format string identifier and function
  /// pointers to the locate, dislocate and task queuing functions.
  struct AnimationFormatRegistryEntry
  {
    char                  m_animationFormatString[ANIM_FORMAT_MAX_STRING_LENGTH];
    AnimType              m_animType;
    LocateAnimFormatFn    m_locateAnimFormatFn;
    DislocateAnimFormatFn m_dislocateAnimFormatFn;
    QueueAttrTaskFn       m_queueAttrTaskAnimSourceFn;
    QueueAttrTaskFn       m_queueAttrTaskTrajectorySourceFn;
    QueueAttrTaskFn       m_queueAttrTaskTrajectoryTransformSourceFn;
  };

  /// \brief Function to register an animation source format with the manager
  uint32_t registerAnimationFormat(
    const char*            animationFormatString,
    AnimType               animType,
    LocateAnimFormatFn     locateAnimFormatFn,
    DislocateAnimFormatFn  dislocateAnimFormatFn,
    QueueAttrTaskFn        queueAttrTaskAnimSourceFn,
    QueueAttrTaskFn        queueAttrTaskTrajectorySourceFn,
    QueueAttrTaskFn        queueAttrTaskTrajectoryAndTransformsFromSourceFn);

  /// \brief Function to get the number to registered animation types
  uint32_t getNumAnimationFormatRegistryEntries();

  /// \brief Function to retrieve an animation registry entry data from the registry index
  const AnimationFormatRegistryEntry* getAnimationFormatRegistryEntry(uint8_t index);

  /// \brief Function to find the registry index corresponding to the animation format string
  bool findAnimationFormatRegistryEntryIndex(const char* animationFormatString, uint8_t& index);

  /// \brief Function to find the registry index corresponding to the animation type
  bool findAnimationFormatRegistryEntryIndex(AnimType animType, uint8_t& index);

  /// \brief Function to find the registry entry corresponding to the animation format string
  const AnimationFormatRegistryEntry* findAnimationFormatRegistryEntry(const char* animationFormatString);

  /// \brief Function to find the registry entry corresponding to the animation type
  const AnimationFormatRegistryEntry* findAnimationFormatRegistryEntry(AnimType animType);

  /// \brief Function to retrieve the number of objects of a given type.
  uint32_t getNumObjectsOfType(AssetType assetType);

  /// \brief Function to retrieve all the object IDs of a given type. Return the number of objects
  /// allowing a sanity check on the retrieved data.
  uint32_t getObjectsOfType(AssetType assetType, ObjectID* buffer);

  /// \brief Registration of a new prediction model type.
  void registerPredictionModel(
    PredictionType             type,      ///< Values must be less than MAX_NUM_PREDICTION_MODELS. 
                                          ///<  This allows for fast array lookup by type.  
    const char*                typeName,  ///< Only  used for debug purposes.
                                          ///<  Note that a direct pointer to this data will be cached internally,
                                          ///<  so be careful if it is not statically allocated.
    PredictionModelLocateFn    locateFn,
    PredictionModelDislocateFn dislocateFn);


  /// \brief Retrieving registered prediction model type functions.
  PredictionModelLocateFn getPredictionModelLocateFn(PredictionType type);
  PredictionModelDislocateFn getPredictionModelDislocateFn(PredictionType type);

protected:

  /// \struct ObjectRegistryEntry
  ///
  /// Associates a pointer to an object with a unique ID for that object.
  /// Allows for relocation of these objects in memory.
  /// This is done by converting all external pointers to this object into the held ID,
  /// moving the object and changing the pointer held here appropriately,
  /// all external pointers (that now hold IDs) can then be converted back to the the new pointer.
  /// These are the Locate and Dislocate functions.
  struct ObjectRegistryEntry
  {
    ObjectID             m_objectID;
    AssetType            m_typeID;
    const void*          m_ptr;
    uint32_t             m_refCount;
    bool                 m_located;
    ObjectRegistryEntry* m_next;
  };

  /// Use these functions to find entries in the registry (you can optionally provide a pointer to receive
  /// the previous reg entry in case you want to remove this one or insert something before it).
  ObjectRegistryEntry* FindRegEntryFromID(ObjectID objectID, ObjectRegistryEntry** prev = NULL);
  ObjectRegistryEntry* FindRegEntryFromPtr(const void* ptr, ObjectRegistryEntry** prev = NULL);

  bool m_initialised;

private:
  /// We don't allow instantiation of the manager - only one ever needs to exist
  Manager();
  ~Manager();

  //----------------------------
  struct TaskQueuingFnRegistryEntry
  {
    uint32_t        fnID;
    QueueAttrTaskFn queuingFn;
    const char*      fnName;
  };

  //----------------------------
  struct OutputCPTaskRegistryEntry
  {
    uint32_t     fnID;
    OutputCPTask outputCPTask;
    const char*  fnName;
  };

  //----------------------------
  struct InitNodeInstanceFnRegistryEntry
  {
    uint32_t          fnID;
    InitNodeInstance  initFn;
    const char*       fnName;
  };

  //----------------------------
  struct DeleteNodeInstanceFnRegistryEntry
  {
    uint32_t            fnID;
    DeleteNodeInstance  deleteFn;
    const char*         fnName;
  };

  //----------------------------
  struct UpdateNodeConnectionsFnRegistryEntry
  {
    uint32_t               fnID;
    UpdateNodeConnections  updateConnectionsFn;
    const char*            fnName;
  };

  //----------------------------
  struct NodeFindGeneratingNodeForSemanticFnRegistryEntry
  {
    uint32_t                         fnID;
    FindGeneratingNodeForSemanticFn  findGeneratingNodeForSemanticFn;
    const char*                      fnName;
  };

  //----------------------------
  struct MessageDataRegistryEntry
  {
    MessageDataLocateFn     m_locateFn;
    MessageDataDislocateFn  m_dislocateFn;
    MessageType             m_type;
  };

  //----------------------------
  struct MessageHandlerFnRegistryEntry
  {
    uint32_t            fnID;
    MessageHandlerFn    messageHandlerFn;
    const char*         fnName;
  };

  //----------------------------
  struct NodeInitDataRegistryEntry
  {
    NodeInitDataLocateFn    m_locateFn;
    NodeInitDataDislocateFn m_dislocateFn;
    NodeInitDataType        m_type;
  };

  //----------------------------
  struct AssetRegistryEntry
  {
    uint32_t       assetType;
    uint32_t       entryID;
    AssetLocateFn  locateFn;
  };

  //----------------------------
  struct AttrDataRegistryEntry
  {
#ifndef NM_HOST_CELL_SPU
    AttribLocateFn      m_locateFn;
    AttribDislocateFn   m_dislocateFn;
    AttribOutputMemReqsFn m_outputMemReqsFn;
#else
    AttribPrepForSpuFn  m_prepForSpuFn;
#endif // NM_HOST_CELL_SPU
    AttribRelocateFn    m_relocateFn;

#if defined(MR_OUTPUT_DEBUGGING)
    AttribSerializeTxFn m_serializeTxFn;
#endif // MR_OUTPUT_DEBUGGING

#if defined(NMP_ENABLE_ASSERTS)
    const char*         m_attribTypeName;  ///< Only  used for debug purposes.
                                           ///<  Note the memory for this string is not allocated by the Manager.
                                           ///<  It is up to the caller of registerAttrDataType() to manage this memory.
#endif
  };

  //----------------------------
  struct PredictionRegistryEntry
  {
    PredictionModelLocateFn    m_locateFn;
    PredictionModelDislocateFn m_dislocateFn;
#if defined(NMP_ENABLE_ASSERTS)
    const char*                m_predictionModelTypeName; ///< Only  used for debug purposes.
                                                          ///<  Note the memory for this string is not allocated by the Manager.
                                                          ///<  It is up to the caller of registerAttrDataType() to manage this memory.
#endif
  };

  RequestAnimFn* m_requestAnimFn;    ///< User registered animation loading function.
  ReleaseAnimFn* m_releaseAnimFn;    ///< User registered animation unloading function.

#ifndef NM_HOST_CELL_SPU
  /// All objects/assets used by Morpheme (NetworkDefs, AnimationsLibraries, Rigs, EventTracks, RigToAnimMaps etc.)
  NMP::StaticFreeList*         m_objectPool;
  ObjectRegistryEntry          m_objRegistry; ///< Empty list head structure.

  TaskQueuingFnRegistryEntry   m_taskQueuingFns[MAX_NUM_QUEUING_FNS];
  uint32_t                     m_numRegisteredTaskQueuingFns;

  OutputCPTaskRegistryEntry    m_outputCPTasks[MAX_NUM_IMMEDIATE_FNS];
  uint32_t                     m_numRegisteredOutputCPTasks;

  NodeInitDataRegistryEntry    m_nodeInitDataRegistry[MAX_NUM_NODE_INIT_DATA_TYPES];
  uint32_t                     m_numRegisteredNodeInitDatas;

  /// Prediction model information for lookup by PredictionType.
  ///  (This makes the assumption that PredictionType values are reasonably contiguous).
  PredictionRegistryEntry      m_predictionRegistry[MAX_NUM_PREDICTION_MODEL_TYPES];
  uint32_t                     m_highestRegisteredPredictionType;
#endif // NM_HOST_CELL_SPU

  AttrDataRegistryEntry        m_attrDataRegistry[MAX_NUM_ATTR_DATA_TYPES];
  uint32_t                     m_highestRegisteredAttribDataType;

  uint32_t                     m_highestRegisteredAttribSemantics;
  const char**                 m_attribSemanticNames; ///< Only used for debug purposes.
                                                      ///<  Note the memory for these strings is not allocated by the Manager.
                                                      ///<  It is up to the caller of registerAttributeSemanticID() to manage this memory.

  AttribSemanticSense*         m_attribSemanticSenses;

  TaskID*                      m_taskIDsCreateReferenceToInput;

#ifndef NM_HOST_CELL_SPU
  TransitCondRegistryEntry     m_transitCondRegistry[MAX_NUM_TRANSIT_COND_TYPES];
  uint32_t                     m_numRegisteredTransitConds;

  InitNodeInstanceFnRegistryEntry                  m_initNodeInstanceFns[MAX_NUM_NODE_TYPES];
  uint32_t                                         m_numInitNodeInstanceFns;

  DeleteNodeInstanceFnRegistryEntry                m_deleteNodeInstanceFns[MAX_NUM_NODE_TYPES];
  uint32_t                                         m_numRegisteredDeleteNodeInstanceFns;

  UpdateNodeConnectionsFnRegistryEntry             m_updateNodeConnectionsFns[MAX_NUM_NODE_TYPES];
  uint32_t                                         m_numRegisteredUpdateNodeConnectionsFns;

  NodeFindGeneratingNodeForSemanticFnRegistryEntry m_nodeFindGeneratingNodeForSemanticFns[MAX_NUM_NODE_TYPES];
  uint32_t                                         m_numRegisteredFindGeneratingNodeForSemanticFns;

  MessageDataRegistryEntry                         m_messageDataRegistry[MAX_NUM_MESSAGE_TYPES];
  uint32_t                                         m_numRegisteredMessageDatas;

  MessageHandlerFnRegistryEntry                    m_messageHandlerFns[MAX_NUM_MESSAGE_TYPES];
  uint32_t                                         m_numRegisteredMessageHandlerFns;

  AssetRegistryEntry                               m_assetEntries[kAsset_NumAssetTypes];
  uint32_t                                         m_numRegisteredAssets;

  AnimationFormatRegistryEntry m_animationFormatRegistry[MAX_NUM_ANIMATION_FORMAT_TYPES];
  uint32_t                     m_numRegisteredAnimationFormats;
#endif // NM_HOST_CELL_SPU
public:
  static Manager& getInstance() { return *sm_instance; }
  static void setInstance(Manager* manager) { sm_instance = manager; }
private:
  static Manager* sm_instance;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
