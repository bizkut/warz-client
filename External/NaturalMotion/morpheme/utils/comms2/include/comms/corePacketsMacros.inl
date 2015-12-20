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
// includes in various places, notably packet.h, to define and iterate over
// the various internal packet types.
//----------------------------------------------------------------------------------------------------------------------

PACKET(IdentificationCmd)
PACKET(IdentificationReply)

PACKET(BeginFrame)
PACKET(EndFrame)
PACKET(BeginFrameSegment)
PACKET(EndFrameSegment)
PACKET(BeginInstanceData)
PACKET(EndInstanceData)
PACKET(BeginInstanceSection)
PACKET(EndInstanceSection)

// BeginFrame
//   BeginFrameSegment(Instances)
//     BeginInstanceData(id)
//       BeginInstanceSection(morphemeSectionId)
//         (section data)
//       EndInstanceSection(morphemeSectionId)
//       ...
//       (other sections: physics, euphoria, etc...)
//     EndInstanceData(id)
//     ...
//     (other instances)
//   EndFrameSegment(Instances)
//   ...
//   (other segments: scene objects, etc)
// EndFrame

PACKET(AddStringToCache)

PACKET(AnimRigDef)
PACKET(TransformBuffer)

// Morpheme network descriptor
PACKET(NetworkDef)
PACKET(NodeDef)
PACKET(MessageDef)
PACKET(SceneObjectDef)

PACKET(BeginPersistent)
PACKET(EndPersistent)

PACKET(ConnectActiveInstance)
PACKET(NetworkInstance)
PACKET(NetworkCreatedReply)
PACKET(NetworkDestroyedReply)
PACKET(NetworkDefLoadedReply)
PACKET(NetworkDefDestroyedReply)

PACKET(ID32s)
PACKET(ID16s)
PACKET(ActiveNodes)
PACKET(NodeEventMessages)
PACKET(ActiveInstances)
PACKET(RootStateMachineNodes)
PACKET(NodeOutputData)
PACKET(PersistentData)
PACKET(FrameData)
PACKET(ScatterBlendWeights)

PACKET(ActiveSceneObjects)

PACKET(ConnectFrameData)

// dispatcher debugging
PACKET(BeginDispatcherTaskExecute)
PACKET(AddDispatcherTaskExecuteParameter)
PACKET(EndDispatcherTaskExecute)
PACKET(ProfilePointTiming)
PACKET(FrameNodeTimings)

// Recording messages sent to state machines
PACKET(StateMachineMessageEventMsg)

PACKET(SceneObjectUpdate)

// Attribute packets are used to describe scene objects
PACKET(AttributeDef)
PACKET(AttributeUpdate)

// List of commands. These packets go from app to runtime.

PACKET(PingCmd)
PACKET(PingData)

PACKET(ClearCachedDataCmd)

PACKET(StartSessionCmd)
PACKET(PauseSessionCmd)
PACKET(StopSessionCmd)

PACKET(StepModeCmd)

// Network management
PACKET(LoadNetworkCmd)
PACKET(CreateNetworkInstanceCmd)
PACKET(DestroyNetworkInstanceCmd)
PACKET(DestroyNetworkDefinitionCmd)
PACKET(ReferenceNetworkInstanceCmd)
PACKET(UnreferenceNetworkInstanceCmd)
PACKET(ReferenceNetworkInstanceReply)

PACKET(NetworkDefinitionDestroyedCtrl)
PACKET(TargetStatusCtrl)

// Debugging commands
PACKET(SetAnimationSetCmd)
PACKET(SetAssetManagerAnimationSetCmd)
PACKET(SetControlParameterCmd)
PACKET(SendRequestCmd)
PACKET(BroadcastRequestCmd)
PACKET(MessageBufferCmd)
PACKET(SetCurrentStateCmd)
PACKET(ExecuteCommandCmd)
PACKET(EnableOutputDataCmd)
PACKET(SetDebugOutputFlagsCmd)
PACKET(SetDebugOutputOnNodesCmd)
PACKET(StepCmd)
PACKET(SetRootTransformCmd)

// Scene object and Environment commands.
PACKET(SetAttributeCmd)
PACKET(SetEnvironmentAttributeCmd)

PACKET(DestroySceneObjectCmd)

PACKET(SceneObjectDestroyed)

PACKET(BeginSceneObjectCmd)
PACKET(EndSceneObjectCmd)
PACKET(AttributeCmd)

// Physics manipulation commands
PACKET(CreateConstraintCmd)
PACKET(MoveConstraintCmd)
PACKET(RemoveConstraintCmd)
PACKET(ApplyForceCmd)


// Fileserver packets.
PACKET(Filename)
PACKET(FileSizeRequest)
PACKET(FileRequest)
PACKET(FileSize)
PACKET(File)


PACKET(ConnectScriptCommand)
PACKET(DownloadFrameDataCmd)
PACKET(DownloadSceneObjectsCmd)
PACKET(DownloadGlobalDataCmd)
PACKET(DownloadNetworkDefinitionCmd)

PACKET(Reply)

PACKET(PreviewChannels)
