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
#ifndef MR_TRANSIT_CONDITION_BUILDER_CONTROL_PARAM_IN_RANGE_H
#define MR_TRANSIT_CONDITION_BUILDER_CONTROL_PARAM_IN_RANGE_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/AssetProcessor.h"
#include "assetProcessor/TransitConditionDefBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::TransitConditionDefBuilderControlParamInRange
/// \brief For construction of TransitConditionDefControlParamInRange in the asset compiler.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefBuilderControlParamInRange : public TransitConditionDefBuilder
{
public:
  virtual NMP::Memory::Format getMemoryRequirements(
    const ME::ConditionExport*   condExport,
    const ME::NetworkDefExport*  netDefExport,
    AssetProcessor*              processor);

  /// \brief Initialise the TransitCondition from connects xml data.
  virtual MR::TransitConditionDef* init(
    NMP::Memory::Resource&       memRes,
    const ME::ConditionExport*   condExport,
    const ME::NetworkDefExport*  netDefExport,
    AssetProcessor*              processor,
    MR::NodeID                   sourceNodeID);

private:
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_BUILDER_CONTROL_PARAM_IN_RANGE_H
//----------------------------------------------------------------------------------------------------------------------
