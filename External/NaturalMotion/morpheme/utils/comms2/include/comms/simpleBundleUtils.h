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
#ifndef MCOMMS_SIMPLEBUNDLEUTILS_H
#define MCOMMS_SIMPLEBUNDLEUTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/mcomms.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
class OrderedStringTable;
}

namespace MR
{
class NetworkDef;
class NetworkPredictionDef;
namespace UTILS
{
  class SimpleAnimRuntimeIDtoFilenameLookup;
} // namespace UTILS
} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

/// \brief Callback used when a plug-in list is found in a simple bundle.  The implementation can verify the list 
/// matches the contents and order of libraries used in the application.  Returning false will terminate the bundle reading.
class IPluginValidator
{
public:
  virtual ~IPluginValidator() {}

  virtual bool validatePluginList(const NMP::OrderedStringTable& pluginList) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Attempt to load an MR::NetworkDef from the supplied byte stream as a SimpleBundle.  Returns a pointer to
/// the loaded networkDef on success, or NULL on failure.  Note that the guid parameter will return the GUID in the
/// file. The validateCompilerPlugins will be called if a plug-in list is found in the bundle.  This parameter is optional
//----------------------------------------------------------------------------------------------------------------------
MR::NetworkDef* readNetworkFromSimpleBundle(
  void*                      fileBuffer,
  size_t                     fileSize,
  MCOMMS::GUID&              guid,
  IPluginValidator*          validateCompilerPlugins,
  MR::NetworkPredictionDef*& netPredictionDef);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Attempt to load an MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup from the supplied byte stream as a
/// SimpleBundle.
/// Returns a pointer to the loaded map on success, or NULL on failure.
//----------------------------------------------------------------------------------------------------------------------
MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* readAnimIDToFilenameFromSimpleBundle(
  void*  fileBuffer,
  size_t fileSize);

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_SIMPLEBUNDLEUTILS_H
//----------------------------------------------------------------------------------------------------------------------
