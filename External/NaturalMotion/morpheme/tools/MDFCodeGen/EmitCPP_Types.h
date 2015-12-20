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

class KeywordsManager;
class CodeWriter;
struct Type;

//----------------------------------------------------------------------------------------------------------------------
class TypeGen
{
public:
  TypeGen(const KeywordsManager& keymgr, CodeWriter* codewriter, const char* root) :
    keywords(keymgr), 
    code(*codewriter),
    projectRootDir(root)
  {}

  bool writeType(const Type* ty);

  const KeywordsManager    &keywords;
  CodeWriter               &code;
  const char               *projectRootDir;

private:

  enum CombineOperator
  {
    MulAcc,     // *=
    Mul,        // *
    AddAcc,     // +=
    Add,        // +
  };

  void writeCombineOperator(const Type* ty, CombineOperator co);

  TypeGen();
  TypeGen(const TypeGen& rhs);
  TypeGen& operator = (TypeGen& rhs);
};
