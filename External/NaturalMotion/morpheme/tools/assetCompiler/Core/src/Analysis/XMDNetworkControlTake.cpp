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
#include "morpheme/mrNetwork.h"
#include "export/mcExport.h"
#include "export/mcExportXml.h"
#include "Analysis/XMDNetworkControlTake.h"
#include "XMD/Model.h"
#include "XMD/AnimCycle.h"
#include "XMD/PoseStream.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelTake Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ChannelTake::getMemoryRequirementsExcludeBaseMem(const char* channelName)
{
  // Memory requirements of the channel name
  NMP_VERIFY(channelName);
  size_t len = strlen(channelName);
  NMP::Memory::Format result(sizeof(char) * (len + 1), NMP_NATURAL_TYPE_ALIGNMENT);

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelTake::initExcludeBaseMem(
  NMP::Memory::Resource& resource,
  ChannelTake* channel,
  const char* channelName,
  MR::AttribDataType type)
{
  // Channel name
  NMP_VERIFY(channel);
  NMP_VERIFY(channelName);
  size_t len = strlen(channelName);
  NMP::Memory::Format memReqs(sizeof(char) * (len + 1), NMP_NATURAL_TYPE_ALIGNMENT);
  channel->m_channelName = (char*)resource.alignAndIncrement(memReqs);
  strcpy(channel->m_channelName, channelName);

  // Type
  channel->m_type = type;

  // Update function
  channel->m_updateNetworkFn = NULL;

  // Multiple of the alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelTake::getControlParameterId(MR::NodeID& controlParamId, MR::Network* network) const
{
  char path[1024];
  sprintf_s(path, 1024-1, "ControlParameters|%s", m_channelName);

  const MR::NetworkDef* networkDef = network->getNetworkDef();
  NMP_VERIFY(networkDef);
  controlParamId = networkDef->getNodeIDFromNodeName(path);
  if(controlParamId == MR::INVALID_NODE_ID)
  {
    NMP_MSG("WARNING: The control parameter '%s' is required for analysis but is missing from the network", m_channelName);
  }
  return controlParamId != MR::INVALID_NODE_ID;
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelTake::getMessageId(MR::MessageID& messageId, MR::Network* network) const
{
  const MR::NetworkDef* networkDef = network->getNetworkDef();
  NMP_VERIFY(networkDef);
  messageId = networkDef->getMessageIDFromMessageName(m_channelName);
  if(messageId == MR::INVALID_MESSAGE_ID)
  {
    NMP_MSG("WARNING: The message '%s' is required for analysis but is missing from the network", m_channelName);
  }
  return messageId != MR::INVALID_MESSAGE_ID;
}

//----------------------------------------------------------------------------------------------------------------------
// ControlChannelTakeBool Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ControlChannelTakeBool::getMemoryRequirements(
  const char* channelName,
  uint32_t numFrames)
{
  // Header
  NMP::Memory::Format result(sizeof(ControlChannelTakeBool), NMP_VECTOR_ALIGNMENT);

  // Channel name
  NMP::Memory::Format memReqsBase = ChannelTake::getMemoryRequirementsExcludeBaseMem(channelName);
  result += memReqsBase;

  // Frame data
  NMP::Memory::Format memReqsData(sizeof(bool) * numFrames, NMP_VECTOR_ALIGNMENT);
  result += memReqsData;

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ControlChannelTakeBool* ControlChannelTakeBool::init(
  NMP::Memory::Resource& resource,
  const char* channelName,
  uint32_t numFrames,
  const bool* data)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(ControlChannelTakeBool), NMP_VECTOR_ALIGNMENT);
  ControlChannelTakeBool* result = (ControlChannelTakeBool*)resource.alignAndIncrement(memReqsHdr);

  // Channel name
  ChannelTake::initExcludeBaseMem(resource, result, channelName, MR::ATTRIB_TYPE_BOOL);

  // Frame data
  NMP::Memory::Format memReqsData(sizeof(bool) * numFrames, NMP_VECTOR_ALIGNMENT);
  result->m_data = (bool*)resource.alignAndIncrement(memReqsData);
  result->m_numFrames = numFrames;
  for (uint32_t i = 0; i < numFrames; ++i)
  {
    result->m_data[i] = data[i];
  }

  // Update function
  result->m_updateNetworkFn = ControlChannelTakeBool::updateNetworkFn;

  // Multiple of the alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ControlChannelTakeBool::updateNetworkFn(
  const ChannelTake* channel,  
  MR::Network* network,
  uint32_t frameIndex)
{
  ControlChannelTakeBool* target = (ControlChannelTakeBool*)channel;
  NMP_VERIFY(target);
  NMP_VERIFY(target->m_type == MR::ATTRIB_TYPE_BOOL);

  if (frameIndex < target->m_numFrames)
  {
    MR::NodeID controlParamId = MR::INVALID_NODE_ID;
    if(channel->getControlParameterId(controlParamId, network))
    {
      MR::AttribDataBool attribData;
      attribData.m_value = target->m_data[frameIndex];
      network->setControlParameter(controlParamId, &attribData);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// ControlChannelTakeFloat Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ControlChannelTakeFloat::getMemoryRequirements(
  const char* channelName,
  uint32_t numFrames)
{
  // Header
  NMP::Memory::Format result(sizeof(ControlChannelTakeFloat), NMP_VECTOR_ALIGNMENT);

  // Channel name
  NMP::Memory::Format memReqsBase = ChannelTake::getMemoryRequirementsExcludeBaseMem(channelName);
  result += memReqsBase;

  // Frame data
  NMP::Memory::Format memReqsData(sizeof(float) * numFrames, NMP_VECTOR_ALIGNMENT);
  result += memReqsData;

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ControlChannelTakeFloat* ControlChannelTakeFloat::init(
  NMP::Memory::Resource& resource,
  const char* channelName,
  uint32_t numFrames,
  const float* data)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(ControlChannelTakeFloat), NMP_VECTOR_ALIGNMENT);
  ControlChannelTakeFloat* result = (ControlChannelTakeFloat*)resource.alignAndIncrement(memReqsHdr);

  // Channel name
  ChannelTake::initExcludeBaseMem(resource, result, channelName, MR::ATTRIB_TYPE_FLOAT);

  // Frame data
  NMP::Memory::Format memReqsData(sizeof(float) * numFrames, NMP_VECTOR_ALIGNMENT);
  result->m_data = (float*)resource.alignAndIncrement(memReqsData);
  result->m_numFrames = numFrames;
  for (uint32_t i = 0; i < numFrames; ++i)
  {
    result->m_data[i] = data[i];
  }

  // Update function
  result->m_updateNetworkFn = ControlChannelTakeFloat::updateNetworkFn;

  // Multiple of the alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ControlChannelTakeFloat::updateNetworkFn(
  const ChannelTake* channel,  
  MR::Network* network,
  uint32_t frameIndex)
{
  ControlChannelTakeFloat* target = (ControlChannelTakeFloat*)channel;
  NMP_VERIFY(target);
  NMP_VERIFY(target->m_type == MR::ATTRIB_TYPE_FLOAT);

  if (frameIndex < target->m_numFrames)
  {
    MR::NodeID controlParamId = MR::INVALID_NODE_ID;
    if(channel->getControlParameterId(controlParamId, network))
    {
      MR::AttribDataFloat attribData;
      attribData.m_value = target->m_data[frameIndex];

      network->setControlParameter(controlParamId, &attribData);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// ControlChannelTakeVector3 Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ControlChannelTakeVector3::getMemoryRequirements(
  const char* channelName,
  uint32_t numFrames)
{
  // Header
  NMP::Memory::Format result(sizeof(ControlChannelTakeVector3), NMP_VECTOR_ALIGNMENT);

  // Channel name
  NMP::Memory::Format memReqsBase = ChannelTake::getMemoryRequirementsExcludeBaseMem(channelName);
  result += memReqsBase;

  // Frame data
  NMP::Memory::Format memReqsData(sizeof(NMP::Vector3) * numFrames, NMP_VECTOR_ALIGNMENT);
  result += memReqsData;

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ControlChannelTakeVector3* ControlChannelTakeVector3::init(
  NMP::Memory::Resource& resource,
  const char* channelName,
  uint32_t numFrames,
  const NMP::Vector3* data)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(ControlChannelTakeVector3), NMP_VECTOR_ALIGNMENT);
  ControlChannelTakeVector3* result = (ControlChannelTakeVector3*)resource.alignAndIncrement(memReqsHdr);

  // Channel name
  ChannelTake::initExcludeBaseMem(resource, result, channelName, MR::ATTRIB_TYPE_VECTOR3);

  // Frame data
  NMP::Memory::Format memReqsData(sizeof(NMP::Vector3) * numFrames, NMP_VECTOR_ALIGNMENT);
  result->m_data = (NMP::Vector3*)resource.alignAndIncrement(memReqsData);
  result->m_numFrames = numFrames;
  for (uint32_t i = 0; i < numFrames; ++i)
  {
    result->m_data[i] = data[i];
  }

  // Update function
  result->m_updateNetworkFn = ControlChannelTakeVector3::updateNetworkFn;

  // Multiple of the alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ControlChannelTakeVector3::updateNetworkFn(
  const ChannelTake* channel,  
  MR::Network* network,
  uint32_t frameIndex)
{
  ControlChannelTakeVector3* target = (ControlChannelTakeVector3*)channel;
  NMP_VERIFY(target);
  NMP_VERIFY(target->m_type == MR::ATTRIB_TYPE_VECTOR3);

  if (frameIndex < target->m_numFrames)
  {
    MR::NodeID controlParamId = MR::INVALID_NODE_ID;
    if(channel->getControlParameterId(controlParamId, network))
    {
      MR::AttribDataVector3 attribData;
      attribData.m_value = target->m_data[frameIndex];
      network->setControlParameter(controlParamId, &attribData);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// ControlChannelTakeQuat Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ControlChannelTakeQuat::getMemoryRequirements(
  const char* channelName,
  uint32_t numFrames)
{
  // Header
  NMP::Memory::Format result(sizeof(ControlChannelTakeQuat), NMP_VECTOR_ALIGNMENT);

  // Channel name
  NMP::Memory::Format memReqsBase = ChannelTake::getMemoryRequirementsExcludeBaseMem(channelName);
  result += memReqsBase;

  // Frame data
  NMP::Memory::Format memReqsData(sizeof(NMP::Quat) * numFrames, NMP_VECTOR_ALIGNMENT);
  result += memReqsData;

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ControlChannelTakeQuat* ControlChannelTakeQuat::init(
  NMP::Memory::Resource& resource,
  const char* channelName,
  uint32_t numFrames,
  const NMP::Quat* data)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(ControlChannelTakeQuat), NMP_VECTOR_ALIGNMENT);
  ControlChannelTakeQuat* result = (ControlChannelTakeQuat*)resource.alignAndIncrement(memReqsHdr);

  // Channel name
  ChannelTake::initExcludeBaseMem(resource, result, channelName, MR::ATTRIB_TYPE_VECTOR4);

  // Frame data
  NMP::Memory::Format memReqsData(sizeof(NMP::Quat) * numFrames, NMP_VECTOR_ALIGNMENT);
  result->m_data = (NMP::Quat*)resource.alignAndIncrement(memReqsData);
  result->m_numFrames = numFrames;
  for (uint32_t i = 0; i < numFrames; ++i)
  {
    result->m_data[i] = data[i];
  }

  // Update function
  result->m_updateNetworkFn = ControlChannelTakeQuat::updateNetworkFn;

  // Multiple of the alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ControlChannelTakeQuat::updateNetworkFn(
  const ChannelTake* channel,
  MR::Network* network,
  uint32_t frameIndex)
{
  ControlChannelTakeQuat* target = (ControlChannelTakeQuat*)channel;
  NMP_VERIFY(target);
  NMP_VERIFY(target->m_type == MR::ATTRIB_TYPE_VECTOR4);

  if (frameIndex < target->m_numFrames)
  {
    MR::NodeID controlParamId = MR::INVALID_NODE_ID;
    if(channel->getControlParameterId(controlParamId, network))
    {
      MR::AttribDataVector4 attribData;
      attribData.m_value = target->m_data[frameIndex];
      network->setControlParameter(controlParamId, &attribData);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// RequestChannelTake Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format RequestChannelTake::getMemoryRequirements(
  const char* channelName,
  uint32_t numFrames)
{
  // Header
  NMP::Memory::Format result(sizeof(RequestChannelTake), NMP_VECTOR_ALIGNMENT);

  // Channel name
  NMP::Memory::Format memReqsBase = ChannelTake::getMemoryRequirementsExcludeBaseMem(channelName);
  result += memReqsBase;

  // Frame data
  NMP::Memory::Format memReqsData(sizeof(bool) * numFrames, NMP_VECTOR_ALIGNMENT);
  result += memReqsData;

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
RequestChannelTake* RequestChannelTake::init(
  NMP::Memory::Resource& resource,
  const char* channelName,
  uint32_t numFrames,
  const bool* data)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(RequestChannelTake), NMP_VECTOR_ALIGNMENT);
  RequestChannelTake* result = (RequestChannelTake*)resource.alignAndIncrement(memReqsHdr);

  // Channel name
  ChannelTake::initExcludeBaseMem(resource, result, channelName, MR::ATTRIB_TYPE_BOOL);

  // Frame data
  NMP::Memory::Format memReqsData(sizeof(bool) * numFrames, NMP_VECTOR_ALIGNMENT);
  result->m_data = (bool*)resource.alignAndIncrement(memReqsData);
  result->m_numFrames = numFrames;
  for (uint32_t i = 0; i < numFrames; ++i)
  {
    result->m_data[i] = data[i];
  }

  // Update function
  result->m_updateNetworkFn = RequestChannelTake::updateNetworkFn;

  // Multiple of the alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void RequestChannelTake::updateNetworkFn(
  const ChannelTake* channel,
  MR::Network* network,
  uint32_t frameIndex)
{
  RequestChannelTake* target = (RequestChannelTake*)channel;
  NMP_VERIFY(target);
  NMP_VERIFY(target->m_type == MR::ATTRIB_TYPE_BOOL);

  if (frameIndex < target->m_numFrames)
  {
    MR::MessageID messageId = MR::INVALID_MESSAGE_ID;
    if(channel->getMessageId(messageId, network))
    {
      if(target->m_data[frameIndex])
      {
        MR::Message request(messageId, MESSAGE_TYPE_REQUEST, true, 0, 0);
        network->broadcastMessage(request);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// ControlTake Functions
//----------------------------------------------------------------------------------------------------------------------
void ControlTake::extractTakes(
  const XMD::XModel& controlFileModel,
  std::vector<std::string>& controlTakeNames,
  std::vector<ControlTake*>& controlTakes)
{
  //---------------------------
  // Get the global list of control parameters
  const XMD::XBase* cparamTargetObject = controlFileModel.FindNode("ControlParameters");
  NMP_VERIFY_MSG(
    cparamTargetObject,
    "Network control export is missing the ControlParameters node");
  XMD::XId cparamsNodeID = cparamTargetObject->GetID();

  std::vector<const char*> cparamNames;
  std::vector<XMD::XFn::Type> cparamTypes;
  XMD::XStringList cparamAttrs;
  cparamTargetObject->ListAttributes(cparamAttrs);
  for (XMD::XStringList::iterator it = cparamAttrs.begin(), itEnd = cparamAttrs.end(); it != itEnd; ++it)
  {
    const XMD::XString& cparamName = *it;
    const XMD::XAttribute* cparamAttr = cparamTargetObject->GetAttribute(cparamName.c_str());
    NMP_VERIFY(cparamAttr);
    XMD::XFn::Type attribType = cparamAttr->Type();

    // Supported types. Note that the attributes attached to the "ControlParameters"
    // locator node determine the type of the input control parameter. The keyframe
    // data is attached to the anim curve list of the animation cycle for the node.
    if (attribType == XMD::XFn::BoolAttribute ||
        attribType == XMD::XFn::FloatAttribute ||
        attribType == XMD::XFn::Float3Attribute ||
        attribType == XMD::XFn::Float4Attribute)
    {
      cparamNames.push_back(cparamName.c_str());
      cparamTypes.push_back(attribType);
    }
  }

  //---------------------------
  // Get the global list of requests
  const XMD::XBase* broadcastMessageTargetObject = controlFileModel.FindNode("BroadcastMessages");
  NMP_VERIFY_MSG(
    broadcastMessageTargetObject,
    "Network control export is missing the BroadcastMessages node");
  XMD::XId requestsNodeID = broadcastMessageTargetObject->GetID();

  std::vector<const char*> requestNames;
  XMD::XStringList requestAttrs;
  broadcastMessageTargetObject->ListAttributes(requestAttrs);
  for (XMD::XStringList::iterator it = requestAttrs.begin(), itEnd = requestAttrs.end(); it != itEnd; ++it)
  {
    const XMD::XString& requestName = *it;
    const XMD::XAttribute* requestAttr = broadcastMessageTargetObject->GetAttribute(requestName.c_str());
    NMP_VERIFY(requestAttr);
    XMD::XFn::Type attribType = requestAttr->Type();

    // Supported types. Note that the attributes attached to the "BroadcastMessages"
    // locator node determine the type of the request. The keyframe data stored as
    // float parameter curves in the anim curve list of the animation cycle for the node.
    if (attribType == XMD::XFn::BoolAttribute)
    {
      requestNames.push_back(requestName.c_str());
    }
  }

  //---------------------------
  // Iterate over the set of control takes
  uint32_t numControlTakes = controlFileModel.NumAnimCycles();
  for (uint32_t takeIndex = 0; takeIndex < numControlTakes; ++takeIndex)
  {
    const XMD::XAnimCycle* animCycle = controlFileModel.GetAnimCycle(takeIndex);
    NMP_VERIFY(animCycle);
    const XMD::XChar* takeName = animCycle->GetName();

    NMP::Memory::Format memReqs = ControlTake::getMemoryRequirements(
      animCycle,
      cparamsNodeID,
      requestsNodeID,
      cparamNames,
      cparamTypes,
      requestNames);

    NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
    ZeroMemory(memRes.ptr, memReqs.size);

    ControlTake* take = ControlTake::init(
      memRes,
      animCycle,
      cparamsNodeID,
      requestsNodeID,
      cparamNames,
      cparamTypes,
      requestNames);

    controlTakeNames.push_back(takeName);
    controlTakes.push_back(take);
  }
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XAnimCurve* ControlTake::findAnimCurve(
  const XMD::XAnimCurveSet* animCurveSet,
  const char* channelName,
  const char* componentExt)
{
  // Get the anim curve name
  NMP_VERIFY(channelName);
  NMP_VERIFY(componentExt);
  char curveName[1024];
  sprintf_s(curveName, 1024-1, "%s%s", channelName, componentExt);

  // Find the corresponding curve
  uint32_t numCurves = animCurveSet->GetNumCurves();
  for(uint32_t curveIndex = 0; curveIndex < numCurves; ++curveIndex)
  {
    const XMD::XAnimCurve* curve = animCurveSet->GetCurve(curveIndex);
    if (strcmp(curve->GetAttrName(), curveName) == 0)
    {
      return curve;
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void ControlTake::findControlParamAnimCurves(
  const XMD::XAnimCycle* animCycle,
  XMD::XId cparamsNodeID,
  const std::vector<const char*>& cparamNames,
  const std::vector<XMD::XFn::Type>& cparamTypes,
  std::vector<ControlParamAnimCurveInfo>& cparamAnimCurves)
{
  NMP_VERIFY(cparamNames.size() == cparamTypes.size());

  // Check if there are any control parameter animation curves
  const XMD::XAnimCurveSet* cparamsCurveSet = animCycle->GetAnimCurvesForNode(cparamsNodeID);
  if (!cparamsCurveSet)
    return;

  uint32_t numControlParams = (uint32_t)cparamNames.size();
  cparamAnimCurves.reserve(numControlParams);

  for (uint32_t i = 0; i < numControlParams; ++i)
  {
    //---------------------------
    if (cparamTypes[i] == XMD::XFn::BoolAttribute ||
        cparamTypes[i] == XMD::XFn::FloatAttribute)
    {
      const XMD::XAnimCurve* cparamCurve = findAnimCurve(cparamsCurveSet, cparamNames[i], "");
      if (cparamCurve)
      {
        ControlParamAnimCurveInfo info;
        ZeroMemory(&info, sizeof(ControlParamAnimCurveInfo));
        info.m_name = cparamNames[i];
        info.m_type = cparamTypes[i];
        info.m_curves[0] = cparamCurve;

        cparamAnimCurves.push_back(info);
      }
    }
    //---------------------------
    else if (cparamTypes[i] == XMD::XFn::Float3Attribute)
    {
      const XMD::XAnimCurve* cparamCurveX = findAnimCurve(cparamsCurveSet, cparamNames[i], "_x");
      const XMD::XAnimCurve* cparamCurveY = findAnimCurve(cparamsCurveSet, cparamNames[i], "_y");
      const XMD::XAnimCurve* cparamCurveZ = findAnimCurve(cparamsCurveSet, cparamNames[i], "_z");
      if (cparamCurveX && cparamCurveY && cparamCurveZ)
      {
        ControlParamAnimCurveInfo info;
        ZeroMemory(&info, sizeof(ControlParamAnimCurveInfo));
        info.m_name = cparamNames[i];
        info.m_type = cparamTypes[i];
        info.m_curves[0] = cparamCurveX;
        info.m_curves[1] = cparamCurveY;
        info.m_curves[2] = cparamCurveZ;

        cparamAnimCurves.push_back(info);
      }
    }
    //---------------------------
    else if (cparamTypes[i] == XMD::XFn::Float4Attribute)
    {
      const XMD::XAnimCurve* cparamCurveX = findAnimCurve(cparamsCurveSet, cparamNames[i], "_x");
      const XMD::XAnimCurve* cparamCurveY = findAnimCurve(cparamsCurveSet, cparamNames[i], "_y");
      const XMD::XAnimCurve* cparamCurveZ = findAnimCurve(cparamsCurveSet, cparamNames[i], "_z");
      const XMD::XAnimCurve* cparamCurveW = findAnimCurve(cparamsCurveSet, cparamNames[i], "_w");
      if (cparamCurveX && cparamCurveY && cparamCurveZ && cparamCurveW)
      {
        ControlParamAnimCurveInfo info;
        ZeroMemory(&info, sizeof(ControlParamAnimCurveInfo));
        info.m_name = cparamNames[i];
        info.m_type = cparamTypes[i];
        info.m_curves[0] = cparamCurveX;
        info.m_curves[1] = cparamCurveY;
        info.m_curves[2] = cparamCurveZ;
        info.m_curves[3] = cparamCurveW;

        cparamAnimCurves.push_back(info);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ControlTake::findRequestAnimCurves(
  const XMD::XAnimCycle* animCycle,
  XMD::XId requestsNodeID,
  const std::vector<const char*>& requestNames,
  std::vector<RequestAnimCurveInfo>& requestAnimCurves)
{
  // Check if there are any request animation curves
  const XMD::XAnimCurveSet* requestsCurveSet = animCycle->GetAnimCurvesForNode(requestsNodeID);
  if (!requestsCurveSet)
    return;

  uint32_t numRequests = (uint32_t)requestNames.size();
  requestAnimCurves.reserve(numRequests);

  for (uint32_t i = 0; i < numRequests; ++i)
  {
    const XMD::XAnimCurve* requestCurve = findAnimCurve(requestsCurveSet, requestNames[i], "");
    if (requestCurve)
    {
      RequestAnimCurveInfo info;
      ZeroMemory(&info, sizeof(RequestAnimCurveInfo));
      info.m_name = requestNames[i];
      info.m_curve = requestCurve;

      requestAnimCurves.push_back(info);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ControlTake::getMemoryRequirements(
  const XMD::XAnimCycle* animCycle,
  XMD::XId cparamsNodeID,
  XMD::XId requestsNodeID,
  const std::vector<const char*>& cparamNames,
  const std::vector<XMD::XFn::Type>& cparamTypes,
  const std::vector<const char*>& requestNames)
{
  // Header memory requirements
  NMP::Memory::Format result(sizeof(ControlTake), NMP_NATURAL_TYPE_ALIGNMENT);

  // If the anim cycle has no frames then the network will be run with the defaults
  uint32_t numFrames = animCycle->GetNumFrames();
  if (numFrames > 0)
  {
    //---------------------------
    // Get the valid anim curves for the current take
    std::vector<ControlParamAnimCurveInfo> cparamAnimCurves;
    findControlParamAnimCurves(
      animCycle,
      cparamsNodeID,
      cparamNames,
      cparamTypes,
      cparamAnimCurves);

    std::vector<RequestAnimCurveInfo> requestAnimCurves;
    findRequestAnimCurves(
      animCycle,
      requestsNodeID,
      requestNames,
      requestAnimCurves);

    //---------------------------
    // Channel tables
    uint32_t numControlParamChannels = (uint32_t)cparamAnimCurves.size();
    if (numControlParamChannels > 0)
    {
      NMP::Memory::Format memReqsCPTable(sizeof(ChannelTake*) * numControlParamChannels, NMP_NATURAL_TYPE_ALIGNMENT);
      result += memReqsCPTable;
    }

    uint32_t numRequestChannels = (uint32_t)requestAnimCurves.size();
    if (numRequestChannels > 0)
    {
      NMP::Memory::Format memReqsReqTable(sizeof(ChannelTake*) * numRequestChannels, NMP_NATURAL_TYPE_ALIGNMENT);
      result += memReqsReqTable;
    }
 
    //---------------------------
    // Control parameter channels
    for (uint32_t channelIndex = 0; channelIndex < numControlParamChannels; ++channelIndex)
    {
      const ControlParamAnimCurveInfo& info = cparamAnimCurves[channelIndex];
      switch (info.m_type)
      {
      case XMD::XFn::BoolAttribute:
        {
          NMP::Memory::Format memReqs = ControlChannelTakeBool::getMemoryRequirements(
            info.m_name,
            numFrames);
          result += memReqs;
        }
        break;

      case XMD::XFn::FloatAttribute:
        {
          NMP::Memory::Format memReqs = ControlChannelTakeFloat::getMemoryRequirements(
            info.m_name,
            numFrames);
          result += memReqs;
        }
        break;

      case XMD::XFn::Float3Attribute:
        {
          NMP::Memory::Format memReqs = ControlChannelTakeVector3::getMemoryRequirements(
            info.m_name,
            numFrames);
          result += memReqs;
        }
        break;

      case XMD::XFn::Float4Attribute:
        {
          NMP::Memory::Format memReqs = ControlChannelTakeQuat::getMemoryRequirements(
            info.m_name,
            numFrames);
          result += memReqs;
        }
        break;

      default:
        NMP_VERIFY_FAIL("Unknown format type");
      }
    }

    //---------------------------
    // Request channels
    for (uint32_t channelIndex = 0; channelIndex < numRequestChannels; ++channelIndex)
    {
      const RequestAnimCurveInfo& info = requestAnimCurves[channelIndex];
      NMP::Memory::Format memReqs = RequestChannelTake::getMemoryRequirements(
        info.m_name,
        numFrames);
      result += memReqs;
    }
  }

  // Multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ControlTake* ControlTake::init(
  NMP::Memory::Resource& resource,
  const XMD::XAnimCycle* animCycle,
  XMD::XId cparamsNodeID,
  XMD::XId requestsNodeID,
  const std::vector<const char*>& cparamNames,
  const std::vector<XMD::XFn::Type>& cparamTypes,
  const std::vector<const char*>& requestNames)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(ControlTake), NMP_NATURAL_TYPE_ALIGNMENT);
  ControlTake* result = (ControlTake*)resource.alignAndIncrement(memReqsHdr);

  result->m_numControlParameterChannels = 0;
  result->m_numRequestChannels = 0;
  result->m_controlChannels = NULL;
  result->m_requestChannels = NULL;

  // If the anim cycle has no frames then the network will be run with the defaults
  uint32_t numFrames = animCycle->GetNumFrames();
  if (numFrames > 0)
  {
    float startTime = animCycle->GetStartTime();

    //---------------------------
    // Get the valid anim curves for the current take
    std::vector<ControlParamAnimCurveInfo> cparamAnimCurves;
    findControlParamAnimCurves(
      animCycle,
      cparamsNodeID,
      cparamNames,
      cparamTypes,
      cparamAnimCurves);

    std::vector<RequestAnimCurveInfo> requestAnimCurves;
    findRequestAnimCurves(
      animCycle,
      requestsNodeID,
      requestNames,
      requestAnimCurves);

    //---------------------------
    // Channel tables
    uint32_t numControlParamChannels = (uint32_t)cparamAnimCurves.size();
    if (numControlParamChannels > 0)
    {
      NMP::Memory::Format memReqsCPTable(sizeof(ChannelTake*) * numControlParamChannels, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_numControlParameterChannels = numControlParamChannels;
      result->m_controlChannels = (ChannelTake**)resource.alignAndIncrement(memReqsCPTable);
    }

    uint32_t numRequestChannels = (uint32_t)requestAnimCurves.size();
    if (numRequestChannels > 0)
    {
      NMP::Memory::Format memReqsReqTable(sizeof(ChannelTake*) * numRequestChannels, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_numRequestChannels = numRequestChannels;
      result->m_requestChannels = (ChannelTake**)resource.alignAndIncrement(memReqsReqTable);
    }

    //---------------------------
    // Control parameter channels
    for (uint32_t channelIndex = 0; channelIndex < numControlParamChannels; ++channelIndex)
    {
      const ControlParamAnimCurveInfo& info = cparamAnimCurves[channelIndex];
      switch (info.m_type)
      {
      case XMD::XFn::BoolAttribute:
        {
          // Extract the sample data
          NMP::Memory::Format memReqsData(sizeof(bool) * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
          NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
          bool* data = (bool*)memResData.ptr;

          for (uint32_t frameIndex = 0; frameIndex < numFrames; ++frameIndex)
          {
            // Convert the float stored as 0.0f, or 1.0 to a bool
            float value = info.m_curves[0]->Evaluate(startTime + (float)frameIndex);
            data[frameIndex] = (value > 0.5f);
          }

          // Initialise the control take
          result->m_controlChannels[channelIndex] = ControlChannelTakeBool::init(
            resource,
            info.m_name,
            numFrames,
            data);

          NMP::Memory::memFree(data);
        }
        break;

      case XMD::XFn::FloatAttribute:
        {
          // Extract the sample data
          NMP::Memory::Format memReqsData(sizeof(float) * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
          NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
          float* data = (float*)memResData.ptr;

          for (uint32_t frameIndex = 0; frameIndex < numFrames; ++frameIndex)
          {
            float value = info.m_curves[0]->Evaluate(startTime + (float)frameIndex);
            data[frameIndex] = value;
          }

          // Initialise the control take
          result->m_controlChannels[channelIndex] = ControlChannelTakeFloat::init(
            resource,
            info.m_name,
            numFrames,
            data);

          NMP::Memory::memFree(data);
        }
        break;

      case XMD::XFn::Float3Attribute:
        {
          // Extract the sample data
          NMP::Memory::Format memReqsData(sizeof(NMP::Vector3) * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
          NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
          NMP::Vector3* data = (NMP::Vector3*)memResData.ptr;

          for (uint32_t frameIndex = 0; frameIndex < numFrames; ++frameIndex)
          {
            float time = startTime + (float)frameIndex;
            NMP::Vector3 value;
            value.x = info.m_curves[0]->Evaluate(time);
            value.y = info.m_curves[1]->Evaluate(time);
            value.z = info.m_curves[2]->Evaluate(time);
            value.w = 0.0f;
            data[frameIndex] = value;
          }

          // Initialise the control take
          result->m_controlChannels[channelIndex] = ControlChannelTakeVector3::init(
            resource,
            info.m_name,
            numFrames,
            data);

          NMP::Memory::memFree(data);
        }
        break;

      case XMD::XFn::Float4Attribute:
        {
          // Extract the sample data
          NMP::Memory::Format memReqsData(sizeof(NMP::Quat) * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
          NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
          NMP::Quat* data = (NMP::Quat*)memResData.ptr;

          for (uint32_t frameIndex = 0; frameIndex < numFrames; ++frameIndex)
          {
            float time = startTime + (float)frameIndex;
            NMP::Quat value;
            value.x = info.m_curves[0]->Evaluate(time);
            value.y = info.m_curves[1]->Evaluate(time);
            value.z = info.m_curves[2]->Evaluate(time);
            value.w = info.m_curves[3]->Evaluate(time);
            value.normalise();
            data[frameIndex] = value;
          }

          // Initialise the control take
          result->m_controlChannels[channelIndex] = ControlChannelTakeQuat::init(
            resource,
            info.m_name,
            numFrames,
            data);

          NMP::Memory::memFree(data);
        }
        break;

      default:
        NMP_VERIFY_FAIL("Unknown format type");
      }
    }

    //---------------------------
    // Request channels
    for (uint32_t channelIndex = 0; channelIndex < numRequestChannels; ++channelIndex)
    {
      const RequestAnimCurveInfo& info = requestAnimCurves[channelIndex];

      // Extract the sample data
      NMP::Memory::Format memReqsData(sizeof(bool) * numFrames, NMP_NATURAL_TYPE_ALIGNMENT);
      NMP::Memory::Resource memResData = NMPMemoryAllocateFromFormat(memReqsData);
      bool* data = (bool*)memResData.ptr;

      for (uint32_t frameIndex = 0; frameIndex < numFrames; ++frameIndex)
      {
        // Convert the float stored as 0.0f, or 1.0 to a bool
        float value = info.m_curve->Evaluate(startTime + (float)frameIndex);
        data[frameIndex] = (value > 0.5f);
      }

      // Initialise the control take
      result->m_requestChannels[channelIndex] = RequestChannelTake::init(
        resource,
        info.m_name,
        numFrames,
        data);

      NMP::Memory::memFree(data);
    }
  }

  // Multiple of the alignment requirement.
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ControlTake::updateNetwork(MR::Network* network, uint32_t frameIndex) const
{
  // Control parameters
  for(uint32_t i = 0; i < m_numControlParameterChannels; ++i)
  {
    const ChannelTake* controlChannel = m_controlChannels[i];
    NMP_VERIFY(controlChannel);
    controlChannel->updateNetwork(network, frameIndex);
  }

  // Requests
  for(uint32_t i = 0; i < m_numRequestChannels; ++i)
  {
    const ChannelTake* requestChannel = m_requestChannels[i];
    NMP_VERIFY(requestChannel);
    requestChannel->updateNetwork(network, frameIndex);
  }
}

}
