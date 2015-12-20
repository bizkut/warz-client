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

struct BehaviourGen
{
  static bool writeBehaviourHeader(const BehaviourDef &bdef, CodeWriter& code, const LimbInstances& limbInstances, const char* projectRootDir);
  static bool writeBehaviourClass(const BehaviourDef &bdef, CodeWriter& code, const char* projectRootDir);
};
