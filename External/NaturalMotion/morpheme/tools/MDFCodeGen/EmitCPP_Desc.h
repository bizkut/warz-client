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

class CodeWriter;
struct NetworkDatabase;

//----------------------------------------------------------------------------------------------------------------------
struct DescGen
{
  static bool writeDescriptorHeader(CodeWriter& code, const NetworkDatabase &ndb);
  static bool writeDescriptorCode(CodeWriter& code, const NetworkDatabase &ndb);
  static bool writeNetworkConstants(CodeWriter& code, const NetworkDatabase &ndb);
  static bool writeDataMemoryHeader(CodeWriter& code, const NetworkDatabase &ndb);
  static bool writeForceInclude(CodeWriter& code, const NetworkDatabase &ndb);

  static bool writeTaskExec(CodeWriter& code, const NetworkDatabase &ndb);
};
