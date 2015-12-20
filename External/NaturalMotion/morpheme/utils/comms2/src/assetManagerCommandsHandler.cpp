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
#include "comms/assetManagerCommandsHandler.h"

#include "comms/connection.h"
#include "comms/runtimeTargetInterface.h"

#include "comms/assetManagerPackets.h"
#include "comms/corePackets.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE T* deserializeCommandPacket(CmdPacketBase* packet)
{
  T* destPacket = (T*)packet;
  destPacket->deserialize();
  return destPacket;
}

//----------------------------------------------------------------------------------------------------------------------
AssetManagerCommandsHandler::AssetManagerCommandsHandler(
  RuntimeTargetInterface* target,
  CoreCommsServerModule* coreModule)
: CommandsHandler(target),
  m_coreModule(coreModule)
{
}

//----------------------------------------------------------------------------------------------------------------------
AssetManagerCommandsHandler::~AssetManagerCommandsHandler()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
void AssetManagerCommandsHandler::clear()
{
}

//----------------------------------------------------------------------------------------------------------------------
void AssetManagerCommandsHandler::onConnectionClosed(Connection* NMP_UNUSED(connection))
{
}

//----------------------------------------------------------------------------------------------------------------------
bool AssetManagerCommandsHandler::doHandleCommand(CmdPacketBase* cmdPacket)
{
  uint16_t pktId = cmdPacket->hdr.m_id;
  NMP::netEndianSwap(pktId);

  switch (pktId)
  {
  case pk_SetAnimBrowserAnimCmd:
    handleSetAnimBrowserAnimCmd(cmdPacket);
    return true;
  case pk_SetAnimBrowserNetworkCmd:
    handleSetAnimBrowserNetworkCmd(cmdPacket);
    return true;
  case pk_LoadAnimBrowserDataCmd:
    handleLoadAnimBrowserDataCmd(cmdPacket);
    return true;
  case pk_SetAnimBrowserTimeCmd:
    handleSetAnimBrowserTimeCmd(cmdPacket);
    return true;
  case pk_DownloadAnimBrowserEventDetectionDataCmd:
    handleDownloadAnimBrowserEventDetectionDataCmd(cmdPacket);
    return true;
  default:
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AssetManagerCommandsHandler::handleSetAnimBrowserAnimCmd(CmdPacketBase* baseCommandPacket)
{
  SetAnimBrowserAnimCmdPacket* commandPacket = deserializeCommandPacket<SetAnimBrowserAnimCmdPacket>(baseCommandPacket);
  AnimationBrowserInterface* animBrowserMgr = m_target->getAnimationBrowser();

  Connection* connection = getCurrentConnection();

  bool result = false;
  if (animBrowserMgr && animBrowserMgr->canLoadAnimSource())
  {
    result = animBrowserMgr->setAnimationBrowserAnim(commandPacket->m_animBrowserAnim, connection);
  }

  ReplyPacket replyPacket(commandPacket->m_requestId, pk_SetAnimBrowserAnimCmd);
  replyPacket.m_result = (result ? ReplyPacket::kResultSuccess : ReplyPacket::kResultFailure);
  mcommsSendDataPacket(replyPacket, connection);
}

//----------------------------------------------------------------------------------------------------------------------
void AssetManagerCommandsHandler::handleSetAnimBrowserNetworkCmd(CmdPacketBase* baseCommandPacket)
{
  SetAnimBrowserNetworkCmdPacket* commandPacket = deserializeCommandPacket<SetAnimBrowserNetworkCmdPacket>(baseCommandPacket);
  AnimationBrowserInterface* animBrowserMgr = m_target->getAnimationBrowser();

  Connection* connection = getCurrentConnection();

  bool result = false;
  if (animBrowserMgr && animBrowserMgr->canLoadAnimSource())
  {
    AnimationBrowserInterface::NetworkDescriptor descriptor;

    descriptor.m_dataSourceNodeName[AnimationBrowserInterface::kAnimationDataSource] =
      &commandPacket->m_animationSourceNodeName[0];
    descriptor.m_dataSourceNodeName[AnimationBrowserInterface::kRetargetDataSource] =
      &commandPacket->m_retargetingSourceNodeName[0];
    descriptor.m_dataSourceNodeName[AnimationBrowserInterface::kRescaleDataSource] =
      &commandPacket->m_rescalingSourceNodeName[0];

    result = animBrowserMgr->setAnimationBrowserNetwork(commandPacket->m_animBrowserNetwork, descriptor, connection);
  }

  ReplyPacket replyPacket(commandPacket->m_requestId, pk_SetAnimBrowserNetworkCmd);
  replyPacket.m_result = (result ? ReplyPacket::kResultSuccess : ReplyPacket::kResultFailure);
  mcommsSendDataPacket(replyPacket, connection);
}

//----------------------------------------------------------------------------------------------------------------------
void AssetManagerCommandsHandler::handleLoadAnimBrowserDataCmd(CmdPacketBase* baseCommandPacket)
{
  LoadAnimBrowserDataCmdPacket* commandPacket = deserializeCommandPacket<LoadAnimBrowserDataCmdPacket>(baseCommandPacket);
  AnimationBrowserInterface* animBrowserMgr = m_target->getAnimationBrowser();

  Connection* connection = getCurrentConnection();

  bool result = false;
  if (animBrowserMgr && animBrowserMgr->canLoadAnimSource())
  {
    result = animBrowserMgr->loadAnimationBrowserData(commandPacket->m_compressionType, connection);

    if (result)
    {
      float duration = animBrowserMgr->getAnimationBrowserDuration(connection);

      AnimBrowserDataLoadedPacket animDataLoadedPacket;
      animDataLoadedPacket.m_duration = duration;
      mcommsBufferDataPacket(animDataLoadedPacket, connection);
    }
  }

  ReplyPacket replyPacket(commandPacket->m_requestId, pk_LoadAnimBrowserDataCmd);
  replyPacket.m_result = (result ? ReplyPacket::kResultSuccess : ReplyPacket::kResultFailure);
  mcommsSendDataPacket(replyPacket, connection);
}

//----------------------------------------------------------------------------------------------------------------------
void AssetManagerCommandsHandler::handleSetAnimBrowserTimeCmd(CmdPacketBase* baseCommandPacket)
{
  SetAnimBrowserTimeCmdPacket* commandPacket = deserializeCommandPacket<SetAnimBrowserTimeCmdPacket>(baseCommandPacket);
  AnimationBrowserInterface* animBrowserMgr = m_target->getAnimationBrowser();

  Connection* connection = getCurrentConnection();
  NetworkDataBuffer* dataBuffer = connection->getDataBuffer();

  if (animBrowserMgr && animBrowserMgr->canLoadAnimSource())
  {
    float actualTime = animBrowserMgr->setAnimationBrowserTime(commandPacket->m_time, connection);

    MCOMMS::BeginAnimBrowserPacket beginPacket(actualTime);
    mcommsBufferDataPacket(beginPacket, connection);

    for (uint32_t i = 0; i != AnimBrowserSourceDataPacket::kNumSourceTypes; ++i)
    {
      AnimBrowserSourceDataPacket::SourceTypes sourceType = static_cast<AnimBrowserSourceDataPacket::SourceTypes>(i);

      // convert between packet source type and browser interface source type.
      // log a message if the source type cannot be provided, would be good to make this generic but there is no
      // enum to string conversion function at the moment.
      //
      AnimationBrowserInterface::AnimationDataSources dataSource = AnimationBrowserInterface::kNumDataSources;
      switch (sourceType)
      {
      case AnimBrowserSourceDataPacket::kAnimationSourceType:
        dataSource = AnimationBrowserInterface::kAnimationDataSource;

        if (!animBrowserMgr->canProvideAnimationBrowserDataSource(dataSource))
        {
          NMP_MSG("MorphemeComms: SetAnimBrowserTime cannot provide animation data.");
          continue;
        }
        break;
      case AnimBrowserSourceDataPacket::kRetargetSourceType:
        dataSource = AnimationBrowserInterface::kRetargetDataSource;

        if (!animBrowserMgr->canProvideAnimationBrowserDataSource(dataSource))
        {
          NMP_MSG("MorphemeComms: SetAnimBrowserTime cannot provide retargeting data.");
          continue;
        }
        break;
      case AnimBrowserSourceDataPacket::kRescaleSourceType:
        dataSource = AnimationBrowserInterface::kRescaleDataSource;

        if (!animBrowserMgr->canProvideAnimationBrowserDataSource(dataSource))
        {
          NMP_MSG("MorphemeComms: SetAnimBrowserTime cannot provide rescaling data.");
          continue;
        }
        break;
      default:
        NMP_ASSERT_FAIL_MSG("invalid animation browser source data type specified.");
        continue;
      }

      uint32_t dataSize = animBrowserMgr->getAnimationBrowserTransformDataSourceSize(dataSource, connection);
      if (dataSize > 0)
      {
        AnimBrowserSourceDataPacket* dataPacket = AnimBrowserSourceDataPacket::create(
          dataBuffer,
          dataSize,
          sourceType);

#if defined(NMP_ENABLE_ASSERTS)
        bool result =
#endif
        animBrowserMgr->serializeAnimationBrowserDataSource(
          dataSource,
          dataPacket->getData(),
          connection);
        NMP_ASSERT(result);

        mcommsSerializeDataPacket(*dataPacket);
      }
    }

    EndAnimBrowserPacket endPacket;
    mcommsBufferDataPacket(endPacket, connection);

    ReplyPacket replyPacket(commandPacket->m_requestId, pk_LoadAnimBrowserDataCmd);
    replyPacket.m_result = ReplyPacket::kResultSuccess;
    mcommsSendDataPacket(replyPacket, connection);
  }
  else
  {
    NMP_MSG("MorphemeComms: SetAnimBrowserTime command not supported.");

    ReplyPacket replyPacket(commandPacket->m_requestId, pk_LoadAnimBrowserDataCmd);
    replyPacket.m_result = ReplyPacket::kResultFailure;
    mcommsSendDataPacket(replyPacket, connection);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AssetManagerCommandsHandler::handleDownloadAnimBrowserEventDetectionDataCmd(CmdPacketBase* cmdPacket)
{
  DownloadAnimBrowserEventDetectionDataCmdPacket* downloadEventDetectionDataCmdPacket =
    deserializeCommandPacket<DownloadAnimBrowserEventDetectionDataCmdPacket>(cmdPacket);

  ReplyPacket replyPacket(downloadEventDetectionDataCmdPacket->m_requestId, pk_DownloadAnimBrowserEventDetectionDataCmd);

  AnimationBrowserInterface* animBrowserMgr = m_target->getAnimationBrowser();
  Connection* connection = getCurrentConnection();

  if (animBrowserMgr &&
      animBrowserMgr->canLoadAnimSource() &&
      animBrowserMgr->canProvideAnimationBrowserDataSource(AnimationBrowserInterface::kAnimationDataSource))
  {
    bool result = false;

    uint32_t transformDataSize = animBrowserMgr->getAnimationBrowserTransformDataSourceSize(
      AnimationBrowserInterface::kAnimationDataSource,
      connection);

    if (transformDataSize > 0)
    {
      NetworkDataBuffer* buffer = connection->getDataBuffer();

      // Prepare the packet.
      AnimBrowserEventDetectionDataPacket* eventDetectionDataPacket = AnimBrowserEventDetectionDataPacket::create(
        buffer,
        downloadEventDetectionDataCmdPacket->m_numFrames,
        transformDataSize);

      for (uint32_t i = 0; i < downloadEventDetectionDataCmdPacket->m_numFrames; ++i)
      {
        MR::AttribTransformBufferOutputData* transformData = eventDetectionDataPacket->getFrameTransformData(i);

        float currentTime = downloadEventDetectionDataCmdPacket->m_minTime + (i * downloadEventDetectionDataCmdPacket->m_dt);
        animBrowserMgr->setAnimationBrowserTime(currentTime, connection);

        result = animBrowserMgr->serializeAnimationBrowserDataSource(
          AnimationBrowserInterface::kAnimationDataSource,
          transformData,
          connection);

        if (!result)
        {
          break;
        }
      }

      mcommsSerializeDataPacket(*eventDetectionDataPacket);
    }

    replyPacket.m_result = result ? ReplyPacket::kResultSuccess : ReplyPacket::kResultFailure;
  }
  else
  {
    NMP_MSG("MorphemeComms: DownloadAnimBrowserEventDetectionData command not supported.");
    replyPacket.m_result = ReplyPacket::kResultFailure;
  }

  mcommsSendDataPacket(replyPacket, connection);
}

} // namespace MCOMMS
