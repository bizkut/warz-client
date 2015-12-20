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

#include "MDF.h"

//----------------------------------------------------------------------------------------------------------------------

class CodeWriter;

//----------------------------------------------------------------------------------------------------------------------
/**
 * simple tracking class used to insert automatic padding while writing out variables
 */
class DataFootprint
{
public:
  DataFootprint();

  inline void append(unsigned int amt) { m_currentUsage += amt; }

  void alignTo(CodeWriter& code, unsigned int av);
  
private:
  unsigned int  m_currentUsage;
  unsigned int  m_paddingIndex;
};


//----------------------------------------------------------------------------------------------------------------------
void calculateDependentHeadersAndFwdDecls(const TypeDependencies& dep, bool forHeader, CodeWriter& code,StringBuilder* fwdDeclsExternal, StringBuilder* fwdDeclsInternal);
void writeDefInclude(CodeWriter& code, const char* projectRootDir, const char* pathToDefinitionFile);


const ModifierOption* getValidatorForVarDecl(const VarDeclBase& vd);
bool varDeclHasValidatorHook(const VarDeclBase& vd);
void writeVarDeclValidator(CodeWriter& code, const VarDeclBase& vd, bool useArrayAPI = false);
void writeVarDeclPostCombine(CodeWriter& code, const VarDeclBase& vd, const char* blockPrefix = 0);

void writeVarDecl(CodeWriter& code, const VarDeclBase& vd, DataFootprint& fc);
void writeVarDecls(const ModuleDef &module, CodeWriter& code, ModuleSection blk, DataFootprint& fc);

void writeEnumDecl(CodeWriter& code, const EnumDeclBase& enm);
void writeComment(CodeWriter& code, const std::string& comment, bool eraseTrailingTag);

void populateDescendentList(const ModuleDef* def, ModuleDef::ConstChildPtrs &descendentList);
