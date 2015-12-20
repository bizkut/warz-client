#pragma once

// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------

/**
 * generate the Euphoria behaviour network C++ code
 */
bool emitCPPCode(
  const char* outputDirectory,        ///< root directory for C++ code files
  const char* projectRootDir,         ///< directory of eu3 project root, for calculating relative paths
  const NetworkDatabase &ndb,         ///< behaviour network to emit data from
  bool forceGeneration);              ///< if true, disregard file times and always generate everything