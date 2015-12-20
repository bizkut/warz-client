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

#include "nmtl/hashmap_dense_dynamic.h"

//----------------------------------------------------------------------------------------------------------------------
// type registry, storing a type ID => name mapping for all encountered types
typedef nmtl::hashmap_dense_dynamic<TypeKey64, Type*> TypeRegistryDB;

struct TypesRegistryData
{
  inline TypesRegistryData() : 
    m_db(128, ActiveMemoryManager::getForNMTL()) 
  {}

  TypeRegistryDB    m_db;
};


//----------------------------------------------------------------------------------------------------------------------
TypesRegistry::TypesRegistry()
{
  parserMemoryAllocDirect(TypesRegistryData, m_data);
}

//----------------------------------------------------------------------------------------------------------------------
TypesRegistry::~TypesRegistry()
{
  m_data = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool TypesRegistry::registerType(Type* t)
{
  assert(m_data);

  TypeKey64 tk;
  tk  = t->m_namespace.hashValueA();
  tk |= (TypeKey64)t->m_name.hashValueA() << 32;

  // check to see if the type has already been registered
  if (m_data->m_db.find(tk))
  {
    return false;
  }

  m_data->m_db.insert(tk, t);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TypesRegistry::addTypeAlias(const PString& alias, const TypesRegistry::Alias* target)
{
  assert(m_data);

  parserMemoryAllocArgs(Type, aliasedType, *target->m_type);

  // reconfigure type to have given alias name
  aliasedType->m_name = alias;
  aliasedType->m_namespace.clear();
  aliasedType->m_namespaceStack.reset();

  // copy in the alias' modifiers, then add any unrepeated ones from the base type
  aliasedType->m_modifiers = target->m_modifiers;

  unsigned int typeOptionsCount = target->m_type->m_modifiers.stack.count();
  for (unsigned int i=0; i<typeOptionsCount; i++)
  {
    const ModifierOption& mo = target->m_type->m_modifiers.stack.get(i);

    bool conflict = false;

    // search current list for existing modifier option
    unsigned int optsCount = aliasedType->m_modifiers.stack.count();
    for (unsigned int i=0; i<optsCount; i++)
    {
      const ModifierOption& existingMO = aliasedType->m_modifiers.stack.get(i);
      if (mo.m_keyword == existingMO.m_keyword)
      {
        conflict = true;
        break;
      }
    }

    // this option was not already applied, so add it onto the aliased type
    if (!conflict)
    {
      aliasedType->m_modifiers.stack.push(mo);
    }
  }


  // point back to original
  aliasedType->m_aliasType = target->m_type;

  bool wasReg = registerType(aliasedType);
  if (!wasReg)
  {
    delete aliasedType;
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int TypesRegistry::getNumberRegisteredTypes() const
{
  assert(m_data);

  return m_data->m_db.getNumUsedSlots();
}

//----------------------------------------------------------------------------------------------------------------------
void TypesRegistry::const_iterate(ConstTypeIterator ti, void* userdata) const
{
  assert(m_data);

  TypeRegistryDB::const_value_walker types = m_data->m_db.constWalker();
  while (types.next())
  {
    const Type* t = types();
    ti(t, userdata);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TypesRegistry::iterate(TypeIterator ti, void* userdata)
{
  assert(m_data);

  TypeRegistryDB::value_walker types = m_data->m_db.walker();
  while (types.next())
  {
    Type* t = types();
    ti(t, userdata);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TypesRegistry::getAllTypes(Types& ty) const
{
  assert(m_data);

  ty.reserve(m_data->m_db.getNumUsedSlots());

  TypeRegistryDB::const_value_walker types = m_data->m_db.constWalker();
  while (types.next())
  {
    const Type* t = types();
    ty.push_back(t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
const Type* TypesRegistry::lookupFromVarDecl(const VarDeclBase& vd) const
{
  assert(m_data);

  return lookupByName(vd.m_namespace, vd.m_typename);
}

//----------------------------------------------------------------------------------------------------------------------
const Type* TypesRegistry::lookupByName(const PString& sNamespace, const PString& sName) const
{
  assert(m_data);

  TypeKey64 tk;
  tk  = sNamespace.hashValueA();
  tk |= (TypeKey64)sName.hashValueA() << 32;

  Type* t = 0;
  m_data->m_db.find(tk, &t);

  return t;
}