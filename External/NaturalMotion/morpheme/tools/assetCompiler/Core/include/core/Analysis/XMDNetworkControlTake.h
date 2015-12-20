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
#ifndef AP_XMD_NETWORK_CONTROL_TAKE_H
#define AP_XMD_NETWORK_CONTROL_TAKE_H
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMMemory.h"
#include "XMD/Base.h"
#include <vector>

namespace NMP
{
class OrderedStringTable;
}

namespace MR
{
class NetworkDef;
class Network;
}

namespace XMD
{
class XAnimCurveSet;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
class ChannelTake;

//----------------------------------------------------------------------------------------------------------------------
typedef void (*ChannelTakeUpdateNetworkFn)(
  const ChannelTake* channel,
  MR::Network* network,
  uint32_t frameIndex);

//----------------------------------------------------------------------------------------------------------------------
/// \class ChannelTake base class
//----------------------------------------------------------------------------------------------------------------------
class ChannelTake
{
public:
  static NMP::Memory::Format getMemoryRequirementsExcludeBaseMem(
    const char* channelName);

  static void initExcludeBaseMem(
    NMP::Memory::Resource& resource,
    ChannelTake* channel,
    const char* channelName,
    MR::AttribDataType type);

  NM_INLINE void updateNetwork(
    MR::Network* network,
    uint32_t frameIndex) const;

  bool getControlParameterId(
    MR::NodeID& controlParamId,
    MR::Network* network) const;

  bool getMessageId(
    MR::MessageID& messageId,
    MR::Network* network) const;

  protected:
  ChannelTakeUpdateNetworkFn    m_updateNetworkFn;
  char*                         m_channelName;
  MR::AttribDataType            m_type;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ChannelTake::updateNetwork(
  MR::Network* network,
  uint32_t frameIndex) const
{
  NMP_VERIFY(m_updateNetworkFn);
  m_updateNetworkFn(this, network, frameIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/// \class ControlChannelTakeBool
//----------------------------------------------------------------------------------------------------------------------
class ControlChannelTakeBool : public ChannelTake
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    const char* channelName,
    uint32_t numFrames);

  static ControlChannelTakeBool* init(
    NMP::Memory::Resource& resource,
    const char* channelName,
    uint32_t numFrames,
    const bool* data);

  static void updateNetworkFn(
    const ChannelTake* channel,
    MR::Network* network,
    uint32_t frameIndex);

protected:
  uint32_t  m_numFrames;
  bool*     m_data;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ControlChannelTakeFloat
//----------------------------------------------------------------------------------------------------------------------
class ControlChannelTakeFloat : public ChannelTake
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    const char* channelName,
    uint32_t numFrames);

  static ControlChannelTakeFloat* init(
    NMP::Memory::Resource& resource,
    const char* channelName,
    uint32_t numFrames,
    const float* data);

  static void updateNetworkFn(
    const ChannelTake* channel,
    MR::Network* network,
    uint32_t frameIndex);

protected:
  uint32_t  m_numFrames;
  float*    m_data;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ControlChannelTakeVector3
//----------------------------------------------------------------------------------------------------------------------
class ControlChannelTakeVector3 : public ChannelTake
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    const char* channelName,
    uint32_t numFrames);

  static ControlChannelTakeVector3* init(
    NMP::Memory::Resource& resource,
    const char* channelName,
    uint32_t numFrames,
    const NMP::Vector3* data);

  static void updateNetworkFn(
    const ChannelTake* channel,
    MR::Network* network,
    uint32_t frameIndex);

protected:
  uint32_t        m_numFrames;
  NMP::Vector3*   m_data;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ControlChannelTakeQuat
//----------------------------------------------------------------------------------------------------------------------
class ControlChannelTakeQuat : public ChannelTake
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    const char* channelName,
    uint32_t numFrames);

  static ControlChannelTakeQuat* init(
    NMP::Memory::Resource& resource,
    const char* channelName,
    uint32_t numFrames,
    const NMP::Quat* data);

  static void updateNetworkFn(
    const ChannelTake* channel,
    MR::Network* network,
    uint32_t frameIndex);

protected:
  uint32_t        m_numFrames;
  NMP::Quat*      m_data;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class RequestChannelTake
//----------------------------------------------------------------------------------------------------------------------
class RequestChannelTake : public ChannelTake
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    const char* channelName,
    uint32_t numFrames);

  static RequestChannelTake* init(
    NMP::Memory::Resource& resource,
    const char* channelName,
    uint32_t numFrames,
    const bool* data);

  static void updateNetworkFn(
    const ChannelTake* channel,
    MR::Network* network,
    uint32_t frameIndex);

protected:
  uint32_t  m_numFrames;
  bool*     m_data;
};

//----------------------------------------------------------------------------------------------------------------------
class ControlParamAnimCurveInfo
{
public:
  const char*             m_name;
  XMD::XFn::Type          m_type;
  const XMD::XAnimCurve*  m_curves[4];
};

//----------------------------------------------------------------------------------------------------------------------
class RequestAnimCurveInfo
{
public:
  const char*             m_name;
  const XMD::XAnimCurve*  m_curve;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ControlTake
/// \brief holds a set of control streams, of control parameter or request data, to set on a network.
//----------------------------------------------------------------------------------------------------------------------
class ControlTake
{
public:

  static void extractTakes(
    const XMD::XModel& controlFileModel,
    std::vector<std::string>& controlTakeNames,
    std::vector<ControlTake*>& controlTakes);

  static const XMD::XAnimCurve* findAnimCurve(
    const XMD::XAnimCurveSet* animCurveSet,
    const char* channelName,
    const char* componentExt);

  static void findControlParamAnimCurves(
    const XMD::XAnimCycle* animCycle,
    XMD::XId cparamsNodeID,
    const std::vector<const char*>& cparamNames,
    const std::vector<XMD::XFn::Type>& cparamTypes,
    std::vector<ControlParamAnimCurveInfo>& cparamAnimCurves);

  static void findRequestAnimCurves(
    const XMD::XAnimCycle* animCycle,
    XMD::XId requestsNodeID,
    const std::vector<const char*>& requestNames,
    std::vector<RequestAnimCurveInfo>& requestAnimCurves);

  static NMP::Memory::Format getMemoryRequirements(
    const XMD::XAnimCycle* animCycle,
    XMD::XId cparamsNodeID,
    XMD::XId requestsNodeID,
    const std::vector<const char*>& cparamNames,
    const std::vector<XMD::XFn::Type>& cparamTypes,
    const std::vector<const char*>& requestNames);

  static ControlTake* init(
    NMP::Memory::Resource& resource,
    const XMD::XAnimCycle* animCycle,
    XMD::XId cparamsNodeID,
    XMD::XId requestsNodeID,
    const std::vector<const char*>& cparamNames,
    const std::vector<XMD::XFn::Type>& cparamTypes,
    const std::vector<const char*>& requestNames);

  void updateNetwork(
    MR::Network* network,
    uint32_t frameIndex) const;

private:
  uint32_t        m_numControlParameterChannels;
  uint32_t        m_numRequestChannels;
  ChannelTake**   m_controlChannels;
  ChannelTake**   m_requestChannels;
};

}

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_XMD_NETWORK_CONTROL_TAKE_H
//----------------------------------------------------------------------------------------------------------------------
