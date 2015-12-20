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

#include "ParserBase.h"
#include "PString.h"
#include "MDF.h"

namespace MDFTypes
{

//----------------------------------------------------------------------------------------------------------------------
struct Context : ContextBase
{
  Context() : ContextBase(),
    m_enumdecl(0), 
    m_activeAccess(Struct::abPublic), 
    m_activeStruct(0), 
    m_parsingFnDecls(false), 
    m_structMap(0),
    m_commentMap(0)
  {}

  // variable declaration under construction
  Struct::VarDecl             m_vardecl;

  // current namespace, items pushed/popped as encountered in text
  PStringStack16              m_namespaceStack;

  // enum being built
  Struct::EnumDecl           *m_enumdecl;

  Struct::AccessibilityBlock  m_activeAccess;
  Struct                     *m_activeStruct;

  bool                        m_parsingFnDecls;

  // the output map for created structs
  StructMap                  *m_structMap;
  // Helper map for structs with processed comments.
  StructMap                  *m_commentMap;
};

} // namespace MDFTypes
