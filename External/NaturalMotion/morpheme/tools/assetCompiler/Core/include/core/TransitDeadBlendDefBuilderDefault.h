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
#ifndef MR_TRANSIT_DEADBLEND_BUILDER_DEFAULT_H
#define MR_TRANSIT_DEADBLEND_BUILDER_DEFAULT_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/AssetProcessor.h"
#include "assetProcessor/TransitDeadBlendDefBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::TransitDeadBlendBuilderDefault
/// \brief For construction of TransitDeadBlendDefDefault in the asset compiler.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class TransitDeadBlendDefBuilderDefault : public TransitDeadBlendDefBuilder
{
public:
  virtual NMP::Memory::Format getNodeDefMemoryRequirements(
    const ME::NodeExport*        nodeDefExport,
    const ME::NetworkDefExport*  netDefExport,
    AssetProcessor*              processor);

  /// \brief Initialise the dead blend attribute data from connects xml data.
  virtual MR::AttribData* initAttribData(
    NMP::Memory::Resource&       memRes,
    const ME::NodeExport*        nodeDefExport,
    const ME::NetworkDefExport*  netDefExport,
    AssetProcessor*              processor);

private:
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_DEADBLEND_BUILDER_DEFAULT_H
//----------------------------------------------------------------------------------------------------------------------
