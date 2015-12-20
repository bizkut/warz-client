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

namespace MDFModule
{

//----------------------------------------------------------------------------------------------------------------------
struct Context : ContextBase
{
  Context() : ContextBase(),
    m_activeBlock(msInvalid), 
    m_enumdecl(0),
    m_childDecl(0),
    m_connectionSet(0), 
    m_connection(0),
    m_connectionPath(0),
    m_stringTmp(128),
    m_sku(32),
    m_lastSeenFnName(32),
    m_parsingFnDecls(false),
    m_def(0)
  {}


  //--------------------------------------------------------------------------------------------------------------------
  // intermediary data used while stepping through def

  ModuleSection                     m_activeBlock;        // which block we are currently walking through
  
  // variable declaration under construction
  ModuleDef::VarDecl                m_vardecl;

  // enum being built
  ModuleDef::EnumDecl              *m_enumdecl;

  // child decl being built
  ModuleDef::Child                 *m_childDecl;

  ModuleDef::ConnectionSet         *m_connectionSet;      // ptr to set being built, points to connection set array in m_def
  ModuleDef::Connection            *m_connection;         // ptr to connection being built, points to array in m_connectionSet
  ModuleDef::Connection::Path      *m_connectionPath;     // path currently under construction

  PString                           m_stringTmp;          // general temporary string buffer
  PString                           m_sku;                // used when resolving <Identifier> rules, holds sku name if present
                                                          // identifier name is stored in m_stringTmp

  PString                           m_lastSeenFnName;
  bool                              m_parsingFnDecls;

  //--------------------------------------------------------------------------------------------------------------------
  // definition currently being populated

  ModuleDef                        *m_def;
};

} // namespace MDFGrammar

//----------------------------------------------------------------------------------------------------------------------
// thrown by exceptions during connectivity phase
//
struct ConnectivityException 
{
  ConnectivityException(const ModuleDef* def, unsigned int atLine, const char* ref, const char* msg) :
    m_def(def), m_atLine(atLine), m_ref(ref), m_msg(msg) {}

  const ModuleDef    *m_def;
  unsigned int        m_atLine;
  PString             m_ref;
  PString             m_msg;

private:
  ConnectivityException();
};


