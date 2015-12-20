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

#include "MDFPrecompiled.h"
#include "MDF.h"
#include "Utils.h"
#include "StringBuilder.h"
#include "ParserMemory.h"
#include "nmtl/hashfunc.h"

#include <Shlwapi.h>


//----------------------------------------------------------------------------------------------------------------------
ModuleBinding* ModuleBinding::deepCopy()
{
  parserMemoryAlloc(ModuleBinding, ret);

  for (ModuleBinding::BindOps::const_iterator it = m_ops.begin(),
                                              end = m_ops.end(); it != end; ++it)
  {
    parserMemoryAlloc(ModuleBinding::BindOp, newOp);
    newOp->m_op = (*it)->m_op;
    newOp->m_text = (*it)->m_text;
    newOp->m_arrayIndex = (*it)->m_arrayIndex;
    ret->m_ops.push_back(newOp);
  }

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
ModuleInstance* ModuleBinding::findCorrespondingInstance(ModuleInstance* startInst) const
{
  ModuleInstance *inst = startInst;

  for (ModuleBinding::BindOps::const_iterator it = m_ops.begin(), end = m_ops.end(); it != end; ++it)
  {
    ModuleBinding::BindOp *bo = (*it);

    switch (bo->m_op)
    {
      case ModuleBinding::GetOwner:
        assert(inst->m_parent);
        inst = inst->m_parent;
        break;

      case ModuleBinding::LookupChildByName:
        {
          ModuleInstance *saved = inst;
          inst = 0;

          iterate_begin_ptr(ModuleInstance, saved->m_children, chMi)
          {
            if (chMi->m_name == bo->m_text)
            {
              if (chMi->m_isArrayInstance)
              {
                if (chMi->m_arrayIndex == bo->m_arrayIndex)
                {
                  inst = chMi;
                  break;
                }
              }
              else
              {
                inst = chMi;
                break;
              }
            }
          }
          iterate_end

          assert(inst != 0);
        }
        break;
    }
  }

  return inst;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int BoundModuleDependency::genHash() const
{
  unsigned int hash = nmtl::hfn::hashFunction(m_def);

  unsigned int sprinkle = 0x5bd1e995;
  for (ModuleBinding::BindOps::const_iterator it = m_binding->m_ops.begin(),
                                              end = m_binding->m_ops.end(); it != end; ++it)
  {
    switch ((*it)->m_op)
    {
      case ModuleBinding::GetOwner:
        {
          hash ^= sprinkle;
          sprinkle = nmtl::hfn::hashFunction(sprinkle);
        }
        break;

      case ModuleBinding::LookupChildByName:
        {
          hash ^= (*it)->m_text.hashValueA();          
        }
        break;
    }
  }

  return hash;
}

//----------------------------------------------------------------------------------------------------------------------
void VarDeclConstruction::getNamespaceFromIndices(StringBuilder& out) const
{
  out.reset();

  bool firstNs = true;
  if (m_namespaceStart != -1)
  {
    for (int i=m_namespaceStart; i<=m_namespaceEnd; i++)
    {
      if (firstNs)
      {
        firstNs = false;
      }
      else
      {
        out.appendCharBuf("::", 2);
      }

      out.appendPString(m_typespec.get(i));
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VarDeclConstruction::createNamespaceStack(PStringStack16& out) const
{
  out.reset();
  if (m_namespaceStart != -1)
  {
    for (int i=m_namespaceStart; i<=m_namespaceEnd; i++)
    {
      out.push(m_typespec.get(i).c_str());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* ModuleDef::VarDecl::varDeclTargetsConnectable(
  const ModuleDef::VarDecl::Target& lhsTarget, 
  const ModuleDef::VarDecl::Target& rhsTarget)
{
  assert(lhsTarget.m_varDeclPtr != 0 && lhsTarget.m_varDeclPtr != 0);
  const ModuleDef::VarDecl &lhs = *lhsTarget.m_varDeclPtr;
  const ModuleDef::VarDecl &rhs = *rhsTarget.m_varDeclPtr;

  if (!lhs.m_knownType || !rhs.m_knownType)
    return "Damaged Type Metadata";

  if (lhs.m_knownType != rhs.m_knownType)
    return "Incompatible Types";

  if (lhs.m_storageSpecifier != rhs.m_storageSpecifier)
    return "Pointer/Reference Mismatch";      

  // neither target has array element specifier, just check that they are either both non-array or both array
  if (!lhsTarget.m_arrayElementAccess && !rhsTarget.m_arrayElementAccess)
  {
    if (lhs.m_arraySize != rhs.m_arraySize)
    {
      if (lhs.m_arraySize == 0 || rhs.m_arraySize == 0)
        return "Array into Non-Array";

      return "Array Size Mismatch";
    }
  }
  else
  {
    // ..it gets more complicated

    // array+target <- array+target
    if (lhsTarget.m_arrayElementAccess && rhsTarget.m_arrayElementAccess)
    {
      // this is fine
      return 0;
    }
    // array+target <- non-array
    // non-array <- array+target
    else if (lhsTarget.m_arrayElementAccess && (!rhsTarget.m_arrayElementAccess && rhs.m_arraySize > 0))
    {
      return "Array into Non-Array";
    }
    else if (rhsTarget.m_arrayElementAccess && (!lhsTarget.m_arrayElementAccess && lhs.m_arraySize > 0))
    {
      return "Non-Array into Array";
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool VarDeclBase::isCompoundType() const
{
  assert(m_knownType);
  return m_knownType->isCompoundType();
}

//----------------------------------------------------------------------------------------------------------------------
bool VarDeclBase::hasCombineFunction() const
{
  assert(m_knownType);
  return m_knownType->hasCombineFunction();
}

//----------------------------------------------------------------------------------------------------------------------
void VarDeclBase::toString(StringBuilder& out, bool justType, bool bypassAlias) const
{
  out.reset();

  // if this decl is based on an aliased type, use the original type
  if (m_knownType && m_knownType->isAliasType() && bypassAlias)
  {
    const PString& aliasNS = m_knownType->m_aliasType->m_namespace;
    if (!aliasNS.empty())
    {
      out.appendPString(aliasNS);
      out.appendCharBuf("::", 2);
    }

    out.appendPString(m_knownType->m_aliasType->m_name);
  }
  else
  {
    if (!m_namespace.empty())
    {
      out.appendPString(m_namespace);
      out.appendCharBuf("::", 2);
    }

    out.appendPString(m_typename);
  }

  if (m_storageSpecifier != '\0')
  {
    out.appendChar(m_storageSpecifier);
  }

  if (justType)
    return;

  out.appendChar(' ');
  out.appendPString(m_varname);

  // append array specifier if size > 0
  if (m_arraySize > 0)
  {
    out.appendChar('[');
    if (m_arraySizeAsString.empty())
    {
      char itoaBuf[33];
      _itoa_s(m_arraySize, itoaBuf, sizeof(itoaBuf), 10);
      out.appendCharBuf(itoaBuf);
    }
    else
    {
      out.appendPString(m_arraySizeAsString);
    }
    out.appendChar(']');
  }
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int VarDeclBase::getSize() const
{
  assert(m_knownType);

  if (m_storageSpecifier != '\0')
    return sizeof(intptr_t);     // 64b systems? :|

  unsigned int sizeInBytes = 0;
  
  // if type has zero-size, it has yet to be calculated
  // (and must be a struct, not built in)
  if (m_knownType->m_sizeInBytes == 0)
  {
    assert(m_knownType->m_struct);
    sizeInBytes = m_knownType->m_struct->getSize();
  }
  else
    sizeInBytes = m_knownType->m_sizeInBytes;

  if (m_arraySize > 0)
    sizeInBytes *= m_arraySize;

  return sizeInBytes;
}

//----------------------------------------------------------------------------------------------------------------------
bool VarDeclTypeSizeSorter::operator ()(VarDeclBase& elem1, VarDeclBase& elem2)
{
  assert(elem1.m_knownType->m_alignment < 255 && elem2.m_knownType->m_alignment < 255);
  assert(elem1.m_knownType->m_sizeInBytes < 0xFFFFFF);
  assert(elem2.m_knownType->m_sizeInBytes < 0xFFFFFF);

  // shuffle alignment-sensitive variables to the top, next to each other. they can be padded during code-gen.
  unsigned int elem1Size = elem1.m_knownType->m_sizeInBytes + ((unsigned char)elem1.m_knownType->m_alignment << 24);
  unsigned int elem2Size = elem2.m_knownType->m_sizeInBytes + ((unsigned char)elem2.m_knownType->m_alignment << 24);

  return elem1Size > elem2Size;
}

//----------------------------------------------------------------------------------------------------------------------
void TypeDependencies::registerDependentType(const Type* t, bool ptrOrRef)
{
  if (t->isCompoundType() || 
    hasTypeModifier(t->m_modifiers, tmExternalClass) ||
    hasTypeModifier(t->m_modifiers, tmExternalStruct) ||
    hasTypeModifier(t->m_modifiers, tmImportAsBuiltIn))
  {
    if (ptrOrRef)
      m_dependentTypesByPtrRef.add(t);
    else
      m_dependentTypes.add(t);
  }
  else
  {
    m_dependentBaseTypes.add(t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EnumDeclBase::add(const PString& name, unsigned int lineDefined)
{
  m_entries.push_back( Struct::EnumDecl::Entry(name, m_currentValue++, false, lineDefined) );
}

//----------------------------------------------------------------------------------------------------------------------
void EnumDeclBase::add(const PString& name, int value, unsigned int lineDefined, const char* expr)
{
  m_entries.push_back( Struct::EnumDecl::Entry(name, value, true, lineDefined, expr) );
  m_currentValue = ++ value;
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::ModuleDef() : FileResource(),
  m_NUID(ModuleDef::InvalidNUID),
  m_lineDefined(0),
  m_limbIndex(0),
  m_parsedTypeDependencies(ActiveMemoryManager::getForNMTL()),
  m_moduleOwnerPtr(0),
  m_updateExecutionDependencies(8, ActiveMemoryManager::getForNMTL()),
  m_feedbackExecutionDependencies(8, ActiveMemoryManager::getForNMTL()),
  m_reducedUpdateDependencies(8, ActiveMemoryManager::getForNMTL()),
  m_reducedFeedbackDependencies(8, ActiveMemoryManager::getForNMTL()),
  m_defaultSet(0),
  m_children(ActiveMemoryManager::getForNMTL()),
  m_private(ActiveMemoryManager::getForNMTL()),
  m_enumDecl(ActiveMemoryManager::getForNMTL()),
  m_privateAPI(ActiveMemoryManager::getForNMTL()),
  m_publicAPI(ActiveMemoryManager::getForNMTL()),
  m_connectionSets(ActiveMemoryManager::getForNMTL())
{
  // reconstruct array with memory manager
  for (unsigned int i=0; i<msNumVarDeclBlocks; i++)
  {
    new (&m_vardecl[i]) VarDecls(ActiveMemoryManager::getForNMTL());
    m_varDeclBlockHasJunctionAssignments[i] = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
const ModuleDef::Child* ModuleDef::getChildByName(const PString &name) const
{
  for (ModuleDef::Childs::const_iterator chIt = m_children.begin(), chEnd = m_children.end(); 
        chIt != chEnd; 
        ++chIt)
  {
    const ModuleDef::Child& ch = (*chIt);
    if (ch.m_name == name)
      return &ch;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const ModuleDef::ConnectionSet* ModuleDef::lookupConnectionSetBySku(const PString &sku) const
{
  for (ModuleDef::ConnectionSets::const_iterator chIt = m_connectionSets.begin(), chEnd = m_connectionSets.end(); 
        chIt != chEnd; 
        ++chIt)
  {
    const ModuleDef::ConnectionSet* ch = (*chIt);
    if (ch->m_name == sku)
      return ch;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Junction::Junction(unsigned int lineDefined, JunctionMode mode, const PString& name, const PString& sku) : 
  m_lineDefined(lineDefined), 
  m_mode(mode), 
  m_name(name), 
  m_connectionSku(sku), 
  m_connectionSources(ActiveMemoryManager::getForNMTL()),
  m_moduleReferences(4, ActiveMemoryManager::getForNMTL()), 
  m_unused(true),
  m_expandToMultiple(-1)
{
  m_connectionSources.reserve(4);
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::ConnectionSet::ConnectionSet() : 
  m_connectionDefs(ActiveMemoryManager::getForNMTL()),
  m_junctions(ActiveMemoryManager::getForNMTL()),
  m_moduleReferences(8, ActiveMemoryManager::getForNMTL()),
  m_newJunctionTasks(ActiveMemoryManager::getForNMTL()),
  m_assignJunctionTask(ActiveMemoryManager::getForNMTL()),
  m_instantiated(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool ModuleDef::Junction::supportsUnblendableTypes(JunctionMode jm)
{
  switch (jm)
  {
    case jmPriority:
    case jmAverage:
      return false;
      break;

    default:
    case jmDirectInput:
    case jmWinnerTakesAll:
    case jmSum:
    case jmMaxFloat:
    case jmMaxFloatValue:
    case jmIsBoolSet:
      return true;
      break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Junction* ModuleDef::ConnectionSet::lookupJunctionByName(const PString &name)
{
  for (ModuleDef::Junctions::iterator jIt = m_junctions.begin(), jEnd = m_junctions.end(); 
        jIt != jEnd; 
        ++jIt)
  {
    ModuleDef::Junction* jnc = (*jIt);
    if (jnc->m_name == name)
      return jnc;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ModuleDef::getSize() const
{
  unsigned int blockSize, totalSize = 0;

  for (unsigned int n=0; n<msNumVarDeclBlocks; n++)
  {
    blockSize = 0;
    iterate_begin_const(ModuleDef::VarDecl, m_vardecl[n], vd)
    {
      blockSize += vd.getSize();

      unsigned int szMlt = (vd.m_arraySize > 0)?vd.m_arraySize:1;

      blockSize += sizeof(float) * szMlt; // importance values     
    }
    iterate_end

    blockSize += (32 - (blockSize % 32)); // align to 32 bytes
    totalSize += blockSize;
  }

  return totalSize;
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path::Path() : 
  m_components(ActiveMemoryManager::getForNMTL()),
  m_componentsHashKey(0),
  m_localDataOnly(false) 
{
  m_components.reserve(4);
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleDef::Connection::Path::toString(StringBuilder& out, unsigned int highlightIndex) const
{
  out.reset();

  Components::const_iterator itemIt = m_components.begin(), 
                             itemEnd = m_components.end();

  unsigned int itemIndex = 0;
  for (; itemIt != itemEnd; ++itemIt, ++itemIndex)
  {
    const Component* cmp = (*itemIt);

    if (itemIndex != 0)
      out.appendChar('.');

    if (itemIndex == highlightIndex)
      out.appendChar('*');

    out.appendPString(cmp->m_text);

    cmp->formatArrayAccess(out, true);

    if (itemIndex == highlightIndex)
      out.appendChar('*');
  }
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path::Component::Component(const char* text, Type t) : 
  m_text(text),
  m_type(t),
  m_context(ccInvalid),
  m_contextData(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path::Component* ModuleDef::Connection::Path::Component::deepCopy() const
{
  parserMemoryAllocArgs(ModuleDef::Connection::Path::Component, ret, m_text.c_str(), m_type);
  ret->m_context = this->m_context;
  ret->m_contextData = this->m_contextData;
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool ModuleDef::Connection::Path::Component::formatArrayAccess(StringBuilder& strOut, bool addArrayBraces, unsigned int arrayIndex) const
{
  // is this an array access?
  if (isType(ModuleDef::Connection::Path::Component::cArrayAccess))
  {
    ModuleDef::Connection::Path::ComponentArrayAccess* caa = (ModuleDef::Connection::Path::ComponentArrayAccess*)this;
    if (addArrayBraces)
      strOut.appendChar('[');
    strOut.appendf("%i", caa->m_arrayIndex);
    if (addArrayBraces)
      strOut.appendChar(']');
    return true;
  }
  else if (isType(ModuleDef::Connection::Path::Component::cArrayAccessByLink))
  {
    ModuleDef::Connection::Path::ComponentArrayAccessByLink* caal = (ModuleDef::Connection::Path::ComponentArrayAccessByLink*)this;
    
    // turn the link into a full access chain
    StringBuilder linkPath(32);
    caal->formatLinkPath(linkPath);

    if (addArrayBraces)
      strOut.appendChar('[');
    strOut.appendf("module->%s", linkPath.c_str());
    if (addArrayBraces)
      strOut.appendChar(']');
    return true;
  }
  else if (isType(ModuleDef::Connection::Path::Component::cIterateArray))
  {
    if (addArrayBraces)
      strOut.appendChar('[');
    strOut.appendf("%i", arrayIndex);
    if (addArrayBraces)
      strOut.appendChar(']');
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path::ComponentArrayAccess::ComponentArrayAccess(const char* text) : 
  Component(text, Component::cArrayAccess), 
  m_arrayIndex(0) 
{
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path::Component* ModuleDef::Connection::Path::ComponentArrayAccess::deepCopy() const
{
  parserMemoryAllocArgs(ModuleDef::Connection::Path::ComponentArrayAccess, ret, m_text.c_str());
  ret->m_context = this->m_context;
  ret->m_contextData = this->m_contextData;
  ret->m_arrayIndex = this->m_arrayIndex;
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path::ComponentArrayAccessByLink::ComponentArrayAccessByLink(const char* text) : 
  Component(text, Component::cArrayAccessByLink), 
  m_linkPath(0) 
{
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path::Component* ModuleDef::Connection::Path::ComponentArrayAccessByLink::deepCopy() const 
{
  parserMemoryAllocArgs(ModuleDef::Connection::Path::ComponentArrayAccessByLink, ret, m_text.c_str());
  ret->m_context = this->m_context;
  ret->m_contextData = this->m_contextData;
  ret->m_linkPath = m_linkPath->deepCopy();
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleDef::Connection::Path::ComponentArrayAccessByLink::formatLinkPath(StringBuilder& sb) const
{
  ModuleDef::Connection::Path::Components::const_iterator compIt = m_linkPath->m_components.begin(), 
                                                          compEnd  = m_linkPath->m_components.end(); 
  --compEnd; // skip last entry, requires special handling

  for (; compIt != compEnd; ++compIt)
  {
    const ModuleDef::Connection::Path::Component* comp = (*compIt);

    sb.appendPString(comp->m_text);
    sb.appendCharBuf("->", 2);
  }

  // handle the last component
  const ModuleDef::Connection::Path::Component* lastComponent = *(-- m_linkPath->m_components.end());
  if (lastComponent->m_context == ccIVChildIndex)
  {
    sb.appendCharBuf("getChildIndex()");
  }
  else
  {
    // other (invalid) eventualities should be caught during parsing/validation
    assert(0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path::ComponentInstanceVariable::ComponentInstanceVariable(const char* text) : 
  Component(text, Component::cInstanceVariable) 
{
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path::ComponentExpandArray::ComponentExpandArray(const char* text) : 
  Component(text, Component::cExpandArray)
{
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path::ComponentIterateArray::ComponentIterateArray(const char* text) : 
  Component(text, Component::cIterateArray)
{
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Connection(unsigned int line, ConnectionTask task) : 
  m_lineDefined(line), 
  m_task(task), 
  m_newJunctionMode(jmInvalid),
  m_paths(ActiveMemoryManager::getForNMTL()),
  m_wasInheritedViaOverride(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool ModuleDef::Connection::SortByTask(const Connection* lhs, const Connection* rhs)
{
  // sort by task and by line number, to keep connections in definition order
  unsigned int lhsVal = lhs->m_lineDefined;
  unsigned int rhsVal = rhs->m_lineDefined;

  // we want assign-junction connections last in the list, so weight them appropriately
  if (lhs->m_task == ctAssignJunction)
    lhsVal += 1 << 24;

  if (rhs->m_task == ctAssignJunction)
    rhsVal += 1 << 24;

  return (lhsVal < rhsVal);
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Junction::ConnectionSource::ConnectionSource(BoundModuleDependency& bmd) : 
  m_sourcePath(0), 
  m_moduleTarget(bmd), 
  m_junction(0) 
{
}

//----------------------------------------------------------------------------------------------------------------------
Type::Type(const char* nspace, const char* name, unsigned int sizeInBytes) : FileResource(),
  m_namespace(nspace),
  m_name(name),
  m_sizeInBytes(sizeInBytes),
  m_alignment(0),
  m_private(false),
  m_struct(0),
  m_aliasType(0)
{}

//----------------------------------------------------------------------------------------------------------------------
Type::Type(const Type& rhs) : FileResource(rhs),
  m_namespace(rhs.m_namespace),
  m_name(rhs.m_name),
  m_namespaceStack(rhs.m_namespaceStack),
  m_sizeInBytes(rhs.m_sizeInBytes),
  m_alignment(rhs.m_alignment),
  m_modifiers(rhs.m_modifiers),
  m_private(rhs.m_private),
  m_struct(rhs.m_struct),
  m_aliasType(rhs.m_aliasType)
{}

//----------------------------------------------------------------------------------------------------------------------
const char* Type::getFullNamespace(StringBuilder& sb) const
{
  // form namespace extended with the struct name
  sb.reset();
  if (!m_namespace.empty())
  {
    sb.appendPString(m_namespace);
    sb.appendCharBuf("::", 2);
  }
  sb.appendPString(m_name);

  return sb.getBufferPtr();
}

//----------------------------------------------------------------------------------------------------------------------
const char* Type::getFlattenedTypename(StringBuilder& sb) const
{
  sb.reset();
  if (!m_namespace.empty())
  {
    sb.appendPString(m_namespace);
    sb.appendCharBuf("_", 1);
  }
  sb.appendPString(m_name);

  return sb.getBufferPtr();
}

//----------------------------------------------------------------------------------------------------------------------
void Type::appendTypeFilename(StringBuilder& sb) const
{
  if (!m_namespace.empty())
  {
    for (unsigned int i=0; i<m_namespaceStack.count(); i++)
    {
      sb.appendPString(m_namespaceStack.get(i));
      sb.appendChar('_');
    }
  }
  sb.appendPString(m_name);
}

//----------------------------------------------------------------------------------------------------------------------
bool Type::isExternalType() const
{
  return (hasTypeModifier(m_modifiers, tmExternalClass) ||
          hasTypeModifier(m_modifiers, tmExternalStruct));
}

//----------------------------------------------------------------------------------------------------------------------
bool Type::isCompoundType() const
{
  return (m_struct || isExternalType());
}

//----------------------------------------------------------------------------------------------------------------------
bool Type::hasCombineFunction() const
{
  return (m_struct && !hasStructModifier(m_struct->m_modifiers, smNoCombine));
}

bool Type::isBlendableType() const
{
  if (m_struct)
  {
    return m_struct->allVarDeclsAreBlendable();
  }
  else
  {
    // we check base-types only, doesn't make sense that an alias could be
    // combinable but the base wouldn't be
    const Type *toCheck = (m_aliasType == 0) ? this : m_aliasType;

    return (hasTypeModifier(toCheck->m_modifiers, tmBlendable) != 0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Struct::Struct() : FileResource(),
  m_lineDefined(0),
  m_typeDependencies(ActiveMemoryManager::getForNMTL()),
  m_type(0),
  m_hasValidationFunction(false),
  m_hasPostCombineFunction(false),
  m_hasPreProcessorMacro(false),
  m_inheritancePtr(0) 
{
  // reconstruct vector array with memory manager
  for (unsigned int i=0; i<abNumAccessabilityBlocks; i++)
  {
    new (&m_vardecl[i]) VarDecls(ActiveMemoryManager::getForNMTL());
    new (&m_functionDecl[i]) FnDecls(ActiveMemoryManager::getForNMTL());
    new (&m_enumDecl[i]) EnumDecls(ActiveMemoryManager::getForNMTL());
  }
}

//----------------------------------------------------------------------------------------------------------------------
const EnumDeclBase::Entry* Struct::findEnumEntry(const PString& name) const
{
  for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
  {
    const EnumDeclBase::Entry* en = searchAllEnumEntries(name, m_enumDecl[i]);
    if (en)
      return en;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleDef::Connection::Path::generateHashKey()
{
  // create a hash key for the path, based on its contents
  unsigned int nHK[2] = { 0x25BA810D, 0x9402CCA8 };
  iterate_begin_ptr(Component, m_components, comp)
  {
    nHK[0] ^= comp->m_text.hashValueA();
    nHK[1] ^= comp->m_text.hashValueB();
  }
  iterate_end

  m_componentsHashKey = nHK[0] | ((nmtl::hkey64)nHK[1] << 32);
}

//----------------------------------------------------------------------------------------------------------------------
ModuleDef::Connection::Path* ModuleDef::Connection::Path::deepCopy() const
{
  parserMemoryAlloc(ModuleDef::Connection::Path, pathCopy);
  pathCopy->m_localDataOnly = this->m_localDataOnly;

  iterate_begin_const_ptr(ModuleDef::Connection::Path::Component, this->m_components, comp)
  {
    ModuleDef::Connection::Path::Component* compCopy = comp->deepCopy();
    pathCopy->m_components.push_back(compCopy);
  }
  iterate_end

  pathCopy->m_target = this->m_target;

  // hash the components, check that it matches the original 
  pathCopy->generateHashKey();
  assert(pathCopy->m_componentsHashKey == this->m_componentsHashKey); 
  return pathCopy;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Struct::getFullNamespace(StringBuilder& sb) const
{
  // form namespace extended with the struct name
  sb.reset();
  if (!m_namespace.empty())
  {
    sb.appendPString(m_namespace);
    sb.appendCharBuf("::", 2);
  }
  sb.appendPString(m_name);

  // eg "ER::Limb"
  return sb.getBufferPtr();
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int Struct::getSize() const
{
  unsigned int sizeInBytes = 0;

  // size pre-calculated, return it
  if (m_type && m_type->m_sizeInBytes != 0)
    return m_type->m_sizeInBytes;

  // walk the members and tot it up
  for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
  {
    Struct::VarDecls::const_iterator it = m_vardecl[i].begin();
    Struct::VarDecls::const_iterator end = m_vardecl[i].end();
    for (; it != end; ++it)
    {
      const Struct::VarDecl& vd = (*it);
      sizeInBytes += vd.getSize();
    }
  }

  if (m_inheritancePtr)
  {
    sizeInBytes += m_inheritancePtr->m_sizeInBytes;
  }

  // if something in the struct is aligned, the compiler will pad the struct automatically
  // so we emulate that process internally too
  if (m_type->m_alignment > 0 &&
     (sizeInBytes % m_type->m_alignment) != 0)
  {
    sizeInBytes += m_type->m_alignment - (sizeInBytes % m_type->m_alignment);
  }

  return sizeInBytes;
}

//----------------------------------------------------------------------------------------------------------------------
bool Struct::allVarDeclsAreBlendable() const
{
  // look through all var decls - if any cannot be blended, the whole struct cannot be
  for (unsigned int i=0; i<Struct::abNumAccessabilityBlocks; i++)
  {
    Struct::VarDecls::const_iterator it = m_vardecl[i].begin();
    Struct::VarDecls::const_iterator end = m_vardecl[i].end();
    for (; it != end; ++it)
    {
      const Struct::VarDecl& vd = (*it);
      if (!vd.m_knownType->isBlendableType())
        return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
BehaviourDef::AttributeGroup* BehaviourDef::findAttributeGroup(const PString& name) const
{
  BehaviourDef::AttributeGroups::const_iterator it = m_attributeGroups.begin();
  BehaviourDef::AttributeGroups::const_iterator end = m_attributeGroups.end();
  for (; it != end; ++it)
  {
    if ((*it).m_name == name)
    {
      return &(*it);
    }
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
BehaviourDef::AttributeGroup* BehaviourDef::findOrCreateAttributeGroup(const PString& name)
{
  AttributeGroup* foundGroup = 0;

  BehaviourDef::AttributeGroups::iterator it = m_attributeGroups.begin();
  BehaviourDef::AttributeGroups::iterator end = m_attributeGroups.end();
  for (; it != end; ++it)
  {
    if ((*it).m_name == name)
    {
      return &(*it);
    }
  }

  if (foundGroup == 0)
  {
    m_attributeGroups.push_back(BehaviourDef::AttributeGroup(name));
    foundGroup = &m_attributeGroups.back();
  }

  return foundGroup;
}

//----------------------------------------------------------------------------------------------------------------------
BehaviourDef::ControlParamGroup* BehaviourDef::findControlParamGroup(const PString& name) const
{
  BehaviourDef::ControlParamGroups::const_iterator it = m_controlParamGroups.begin();
  BehaviourDef::ControlParamGroups::const_iterator end = m_controlParamGroups.end();
  for (; it != end; ++it)
  {
    if ((*it).m_name == name)
    {
      return &(*it);
    }
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
BehaviourDef::ControlParamGroup* BehaviourDef::findOrCreateControlParamGroup(const PString& name)
{
  ControlParamGroup* foundGroup = 0;

  BehaviourDef::ControlParamGroups::iterator it = m_controlParamGroups.begin();
  BehaviourDef::ControlParamGroups::iterator end = m_controlParamGroups.end();
  for (; it != end; ++it)
  {
    if ((*it).m_name == name)
    {
      return &(*it);
    }
  }

  if (foundGroup == 0)
  {
    m_controlParamGroups.push_back(BehaviourDef::ControlParamGroup(name));
    foundGroup = &m_controlParamGroups.back();
  }

  return foundGroup;
}

//----------------------------------------------------------------------------------------------------------------------
BehaviourDef::BehaviourDef() : FileResource(),
  m_lineDefined(0),
  m_behaviourID(0),
  m_attributeGroups(ActiveMemoryManager::getForNMTL()),
  m_controlParamGroups(ActiveMemoryManager::getForNMTL()),
  m_modulesToEnable(ActiveMemoryManager::getForNMTL()),
  m_emittedMessages(ActiveMemoryManager::getForNMTL()),
  m_animationInputs(ActiveMemoryManager::getForNMTL()),
  m_sortedControlParams(ActiveMemoryManager::getForNMTL()),
  m_sortedOutputControlParams(ActiveMemoryManager::getForNMTL()),
  m_sortedAttributes(ActiveMemoryManager::getForNMTL()),
  m_development(false),
  m_group("Behaviours")
{
}

//----------------------------------------------------------------------------------------------------------------------
BehaviourDef::BaseVariable::BaseVariable(BhDefVariableType vt, const PString& name, unsigned int arraySize) :
  m_type(vt),
  m_name(name),
  m_lineDefined(0),
  m_ID(0),
  m_arraySize(arraySize),
  m_numFloats(0),
  m_numInts(0),
  m_numUInt64s(0),
  m_numVector3s(0)
{
  switch (vt)
  {
    case bhvtFloat:
      m_numFloats = 1 * max(m_arraySize, 1);
      break;

    case bhvtInteger:
      m_numInts = 1 * max(m_arraySize, 1);
      break;

    case bhvtBool:
      m_numInts = 1 * max(m_arraySize, 1);
      break;

    case bhvtVector3:
      m_numVector3s = max(m_arraySize, 1);
      break;

    case bhvtPhysicsObjectID:
      m_numUInt64s = 1 * max(m_arraySize, 1);
      break;

    default:
      assert(0);
      break;
  }

  assert(m_numFloats != 0 || m_numInts != 0 || m_numUInt64s != 0 || m_numVector3s != 0);
}

//----------------------------------------------------------------------------------------------------------------------
BehaviourDef::ControlParam::ControlParam(BhDefVariableType vt, const PString& name, unsigned int arraySize) : 
  AttributeVariable(vt, name, arraySize)
{

}

//----------------------------------------------------------------------------------------------------------------------
BehaviourDef::OutputControlParam::OutputControlParam(BhDefVariableType vt, const PString& name, unsigned int arraySize) : 
  BaseVariable(vt, name, arraySize)
{
}


//----------------------------------------------------------------------------------------------------------------------
BehaviourDef::AttributeVariable::AttributeVariable(BhDefVariableType vt, const PString& name, unsigned int arraySize) : 
  BaseVariable(vt, name, arraySize)
{
  switch (vt)
  {
    case bhvtFloat:
      m_default.f = 0.0f;
      m_min.f = -FLT_MAX;
      m_max.f = FLT_MAX;
      break;

    case bhvtInteger:
      m_default.i = 0;
      m_min.i = -INT_MAX;
      m_max.i = INT_MAX;
      break;

    case bhvtBool:
      m_default.b = false;
      m_min.b = false;
      m_max.b = true;
      break;

    case bhvtVector3:
      m_default.v[0] = m_default.v[1] = m_default.v[2] = 0.0f;
      m_min.v[0] = m_min.v[1] = m_min.v[2] = -FLT_MAX;
      m_max.v[0] = m_max.v[1] = m_max.v[2] = FLT_MAX;
      break;

    case bhvtPhysicsObjectID:
      m_default.ptr = 0;
      m_min.ptr = 0;
      m_max.ptr = 0;
      break;

    default:
      assert(0);
      break;
  }

  m_hasDefault = m_hasMin = m_hasMax = false;
}

//----------------------------------------------------------------------------------------------------------------------
static StringBuilder gSB(128);

//----------------------------------------------------------------------------------------------------------------------
void ModuleDef::createArtefacts()
{
  assert(m_artefacts.empty());
  StringBuilder &sb = gSB;

  m_artefacts.reserve(4);

  sb.reset();
  sb.appendPString(m_name);
  sb.save();

  sb.appendCharBuf(".h");
  m_artefacts.push_back(CodeGenArtefact(ModuleDef::ModuleHeader, sb.c_str()));

  sb.restore();
  sb.appendCharBuf(".cpp");
  m_artefacts.push_back(CodeGenArtefact(ModuleDef::ModuleCode, sb.c_str()));

  sb.restore();
  sb.appendCharBuf("Data.h");
  m_artefacts.push_back(CodeGenArtefact(ModuleDef::ModuleData, sb.c_str()));

  sb.restore();
  sb.appendCharBuf("Packaging.h");
  m_artefacts.push_back(CodeGenArtefact(ModuleDef::ModulePackaging, sb.c_str()));

  sb.restore();
  sb.appendCharBuf("Packaging.cpp");
  m_artefacts.push_back(CodeGenArtefact(ModuleDef::ModulePackagingCode, sb.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
void Type::createArtefacts()
{
  assert(m_artefacts.empty());

  StringBuilder &sb = gSB;

  sb.reset();
  sb.appendPString(m_pathToDef);
  sb.invoke(PathStripPath);
  sb.invoke(PathRemoveExtension);
  sb.prependf("Types/Stub_");
  sb.appendf(".h");

  m_artefacts.push_back(CodeGenArtefact(0, sb.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
void Struct::createArtefacts()
{
  assert(m_artefacts.empty());
  StringBuilder &sb = gSB;

  sb.reset();
  sb.appendCharBuf("Types/");
  m_type->appendTypeFilename(sb);
  sb.appendCharBuf(".h");

  m_artefacts.push_back(CodeGenArtefact(Struct::StructHeader, sb.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
void BehaviourDef::createArtefacts()
{
  assert(m_artefacts.empty());
  StringBuilder &sb = gSB;

  m_artefacts.reserve(2);

  sb.reset();
  sb.appendCharBuf("Behaviours/");
  sb.appendf("%sBehaviour", m_name.c_str());
  sb.save();

  sb.appendCharBuf(".h");
  m_artefacts.push_back(CodeGenArtefact(BehaviourDef::BehaviourHeader, sb.c_str()));

  sb.restore();
  sb.appendCharBuf(".cpp");
  m_artefacts.push_back(CodeGenArtefact(BehaviourDef::BehaviourCode, sb.c_str()));
}

//----------------------------------------------------------------------------------------------------------------------
LimbInstance::LimbInstance() : 
  m_inst(0),
  m_index(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
ExecutionBucket::ExecutionBucket() : 
  m_instances(ActiveMemoryManager::getForNMTL())
{
}

//----------------------------------------------------------------------------------------------------------------------
ModuleInstance::ModuleInstance() :
  m_module(0),
  m_parent(0),
  m_children(ActiveMemoryManager::getForNMTL()),
  m_updateDependencies(8, ActiveMemoryManager::getForNMTL()),
  m_feedbackDependencies(8, ActiveMemoryManager::getForNMTL()),
  m_updateBuckets(ActiveMemoryManager::getForNMTL()),
  m_feedbackBuckets(ActiveMemoryManager::getForNMTL())
{
}

//----------------------------------------------------------------------------------------------------------------------
NetworkDatabase::NetworkDatabase(unsigned int defaultContainerSize) : 
  m_moduleNUID(0),
  m_constants(defaultContainerSize, ActiveMemoryManager::getForNMTL()),
  m_moduleDefs(ActiveMemoryManager::getForNMTL()),
  m_structMap(defaultContainerSize, ActiveMemoryManager::getForNMTL()),
  m_commentMap(defaultContainerSize, ActiveMemoryManager::getForNMTL()),
  m_behaviourDefs(ActiveMemoryManager::getForNMTL()),
  m_limbInstances(ActiveMemoryManager::getForNMTL())
{
  m_moduleDefs.reserve(defaultContainerSize);
  m_behaviourDefs.reserve(defaultContainerSize);
  m_limbInstances.reserve(defaultContainerSize);

  // hardwired for now until we get a build process system in here.
  const int maxNumArms = 2;
  const int maxNumHeads = 1;
  const int maxNumSpines = 1;
  const int maxNumLegs = 2;
  m_constants.insert("networkMaxNumArms",   maxNumArms);
  m_constants.insert("networkMaxNumHeads",  maxNumHeads);
  m_constants.insert("networkMaxNumSpines", maxNumSpines);
  m_constants.insert("networkMaxNumLegs",   maxNumLegs);
  m_constants.insert("networkMaxNumLimbs",  maxNumArms+maxNumHeads+maxNumSpines+maxNumLegs);
  m_constants.insert("rigMaxNumPartsPerLimb",  10);
}

//----------------------------------------------------------------------------------------------------------------------
const char* getLimbNameFromArray(const char* limbArrayName)
{
  const char* arraySize = limbArrayName;
  const char* prefix = "NetworkConstants::";

  if (strncmp(arraySize, prefix, strlen(prefix)) == 0)
  {
    arraySize += strlen(prefix);
    static const char *arrayNames[] = {
      "networkMaxNumArms",
      "networkMaxNumLegs",
      "networkMaxNumHeads",
      0
    };
    static const char *names[] = {
      "{Arm%i}",
      "{Leg%i}",
      "{Head%i}",
      0
    };
    for (int i = 0 ; arrayNames[i] ; ++i)
    {
      if (strncmp(arraySize, arrayNames[i], strlen(arrayNames[i])) == 0)
      {
        return names[i];
      }
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const char* stripWhiteSpace(const char* scData)
{
  // try stripping off prefixed whitespace
  const char* scStrip = scData;
  while (*scStrip == ' ')
  {
    scStrip++;
    if (!*scStrip)
    {
      scStrip = scData;
      break;
    }
  }
  return scStrip;
}
