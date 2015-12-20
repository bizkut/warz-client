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

#include "StringBuilder.h"

//----------------------------------------------------------------------------------------------------------------------

class KeywordsManager;
class CodeWriter;
struct ModuleDef;

//----------------------------------------------------------------------------------------------------------------------
class ModuleGen
{
public:
  ModuleGen(const KeywordsManager& keymgr, CodeWriter* codewriter, const char* root) :
    keywords(keymgr), 
    code(*codewriter),
    projectRootDir(root),
    sb(128)
  {}

  bool writeHeader(const ModuleDef &module);
  bool writeDataHeader(const ModuleDef &module);
  bool writePackagingHeader(const ModuleDef &module);
  bool writeCode(const ModuleDef &module);
  bool writePackagingCode(const ModuleDef &module);
  bool writeInterface(const ModuleDef &module, CodeWriter &code);

  const KeywordsManager    &keywords;
  CodeWriter               &code;
  const char               *projectRootDir;
  StringBuilder             sb;

private:
  
  void writeDataPacket(const ModuleDef &module, ModuleSection blk, bool emitImportanceAPI);


  void writeConstructor(const ModuleDef& module);
  void writeDestructor(const ModuleDef& module);
  void writeCreateFn(const ModuleDef& module);
  void writeLoadStoreStateFns(const ModuleDef &module);
  void writeClearAllDataFn(const ModuleDef& module);
  void writeUpdateAndFeedbackFns(const ModuleDef& module);
  void writeGetChildNameFn(const ModuleDef& module);

  void writeDataAllocatorFns(const ModuleDef& module);


  ModuleGen();
  ModuleGen(const ModuleGen& rhs);
  ModuleGen& operator = (ModuleGen& rhs);
};
