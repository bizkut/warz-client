// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef ER_PININTERFACE_H
#define ER_PININTERFACE_H

//----------------------------------------------------------------------------------------------------------------------
// Node types.
#include "morpheme/mrNodeBin.h"
#include "morpheme/mrNetwork.h"

//----------------------------------------------------------------------------------------------------------------------


namespace ER
{

// Machinery for PinInterface::writeValue() PinInterface::createPin() api's.
//
// Define a template to associate cpp-type with AttribData-type.
// (A specicialisation of this type must exist for every data type used with the writeValue() fn)
template<typename T> struct AttribDataTypeSelector{};
// Use specialization to make it so.
template<> struct AttribDataTypeSelector<int32_t>{ typedef MR::AttribDataInt AttribDataType; };
template<> struct AttribDataTypeSelector<float>{ typedef MR::AttribDataFloat AttribDataType; };
template<> struct AttribDataTypeSelector<bool>{ typedef MR::AttribDataBool AttribDataType; };
template<> struct AttribDataTypeSelector<NMP::Vector3>{ typedef MR::AttribDataVector3 AttribDataType; };

class PinInterface 
{
public:
  PinInterface(MR::NodeBin* nodeBin) : m_nodeBin(nodeBin) {}

  MR::OutputCPPin* getOutputCPPin(MR::PinIndex pinID)
  {
    MR::OutputCPPin* outputCPPin = m_nodeBin->getOutputCPPin(pinID);
    NMP_ASSERT_MSG(outputCPPin, "Output pin not found.");
    return outputCPPin;
  }

  MR::FrameCount getLastUpdateFrame(MR::PinIndex pinID)
  {
    MR::OutputCPPin* outputCPPin = getOutputCPPin(pinID);
    return outputCPPin ? outputCPPin->m_lastUpdateFrame : MR::NOT_FRAME_UPDATED;
  }

  void setLastUpdateFrame(MR::FrameCount frameCount, MR::PinIndex begin, MR::PinIndex end)
  {
    MR::OutputCPPin* outputCPPin = NULL;
    for (MR::PinIndex i = begin; i < end; ++i)
    {
      outputCPPin = getOutputCPPin(i);
      if (outputCPPin)
      {
        NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData);
        outputCPPin->m_lastUpdateFrame = frameCount;
      }
    }
  }

  // Template api to write to morpheme output pins of the type covered by the above specializations.
  //
  template<typename T> void writeValue(MR::PinIndex pinID, const T& value)
  {
    MR::OutputCPPin* outputCPPin = getOutputCPPin(pinID);
    if (outputCPPin)
    {
      typename ER::AttribDataTypeSelector<T>::AttribDataType* outputAttribData
        = outputCPPin->getAttribData<typename ER::AttribDataTypeSelector<T>::AttribDataType>();
      if (outputAttribData != NULL)
      {
        outputAttribData->m_value = value;
      }
    }
  }

  void createPin(MR::Network* net, MR::PinIndex pinIndex, float initValue)
  {
    MR::OutputCPPin* pin = m_nodeBin->getOutputCPPin(pinIndex);
    NMP_ASSERT(!pin->m_attribDataHandle.m_attribData);
    pin->m_attribDataHandle = MR::AttribDataFloat::create(net->getPersistentMemoryAllocator(), initValue);
  }

  void createPin(MR::Network* net, MR::PinIndex pinIndex, const NMP::Vector3& initValue)
  {
    MR::OutputCPPin* pin = m_nodeBin->getOutputCPPin(pinIndex);
    NMP_ASSERT(!pin->m_attribDataHandle.m_attribData);
    pin->m_attribDataHandle = MR::AttribDataVector3::create(net->getPersistentMemoryAllocator(), initValue);
  }

private:
  MR::NodeBin* m_nodeBin;
};
} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // ER_PININTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
