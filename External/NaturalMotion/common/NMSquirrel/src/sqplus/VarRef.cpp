#include "sqplus/VarRef.h"

#include <stdio.h>

#include "sqplus/StackHandler.h"
#include "sqplus/ClassHelpers.h"

/*lint -e408 type mismatch with switch expression*/
/*lint -e788 enum constant not used within defaulted switch*/

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
void VarRef::GetNameTag(SQChar *buffer, size_t size, const SQChar *scriptName)
{
  if (size > 3)
  {
    SQChar *d = buffer;
    d[0] = '_';
    d[1] = 'v';
    d = &d[2];
    size -= (2 + 1);
    size_t pos = 0;

    while (scriptName[pos] && pos < size)
    {
      d[pos] = scriptName[pos];
      ++pos;
    }
    d[pos] = '\0';
  }
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger VarRef::GetVarInfo(StackHandler& sa, VarRef *&ref)
{
  SquirrelObject table = sa.GetObject(1);
  const SQChar *scriptName = sa.GetString(2);
  SQChar varNameTag[256];
  GetNameTag(varNameTag, 256, scriptName);
  SQUserPointer data = 0;
  if (!table.RawGetUserData(varNameTag, &data))
  {
    return SQ_ERROR;
  }
  ref = static_cast<VarRef*>(data);
  return SQ_OK;
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger VarRef::GetInstanceVarInfo(StackHandler& sa, VarRef *&ref, SQUserPointer& data)
{
  SquirrelObject instance = sa.GetObject(1);
  const SQChar *scriptName = sa.GetString(2);
  SQChar varNameTag[256];
  GetNameTag(varNameTag, 256, scriptName);
  SQUserPointer userData = 0;
  if (!instance.RawGetUserData(varNameTag, &userData))
  {
    return SQ_ERROR;
  }
  ref = static_cast<VarRef*>(userData);
  
  char *up = 0;
  if (!(ref->m_flags & static_cast<int32_t>(kStaticVar)))
  {
    SQUserPointer typetag;
    if (instance.GetTypeTag(&typetag) != SQ_OK)
    {
      return SQ_ERROR;
    }

    const ClassTypeBase *ctb = ref->m_instanceType;
    up = static_cast<char*>(instance.GetInstanceUP(0));
    // Walk base classes until type tag match, adjust for inheritance offset
    while (ctb && typetag != ctb)
    {
      up = up - ctb->m_offset;
      ctb = ctb->m_pbase;
    }

    if (!ctb)
    {
      return SQ_ERROR;
    }
    up += reinterpret_cast<size_t>(ref->m_offsetOrAddress);
  }
  else
  {
    up = static_cast<char*>(ref->m_offsetOrAddress);
  }

  data = up;
  return SQ_OK;
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger VarRef::SetVar(StackHandler& sa, const VarRef *ref, void *data)
{
  if (data == 0)
  {
    return SQ_ERROR;
  }

  switch (ref->m_type)
  {
    case TypeInfo<bool>::TypeID:
    {
      bool *val = static_cast<bool*>(data);
      *val = sa.GetBool(3);
      return sa.Return(*val);
    }
    case TypeInfo<char>::TypeID:
    {
      char *val = static_cast<char*>(data);
      *val = static_cast<char>(sa.GetInt(3));
      return sa.Return(static_cast<SQInteger>(*val));
    }
    case TypeInfo<unsigned char>::TypeID:
    {
      unsigned char *val = static_cast<unsigned char*>(data);
      *val = static_cast<unsigned char>(sa.GetInt(3));
      return sa.Return(static_cast<SQInteger>(*val));
    }
    case TypeInfo<short>::TypeID:
    {
      short *val = static_cast<short*>(data);
      *val = static_cast<short>(sa.GetInt(3));
      return sa.Return(static_cast<SQInteger>(*val));
    }
    case TypeInfo<unsigned short>::TypeID:
    {
      unsigned short *val = static_cast<unsigned short*>(data);
      *val = static_cast<unsigned short>(sa.GetInt(3));
      return sa.Return(static_cast<SQInteger>(*val));
    }
    case TypeInfo<int32_t>::TypeID:
    {
      int32_t *val = static_cast<int32_t*>(data);
      *val = static_cast<int32_t>(sa.GetInt(3));
      return sa.Return(static_cast<SQInteger>(*val));
    }
    case TypeInfo<uint32_t>::TypeID:
    {
      uint32_t *val = static_cast<uint32_t*>(data);
      *val = static_cast<uint32_t>(sa.GetInt(3));
      return sa.Return(static_cast<SQInteger>(*val));
    }
    case TypeInfo<int64_t>::TypeID:
    {
      int64_t *val = static_cast<int64_t*>(data);
      *val = static_cast<int64_t>(sa.GetInt(3));
      return sa.Return(static_cast<SQInteger>(*val));
    }
    case TypeInfo<uint64_t>::TypeID:
    {
      uint64_t *val = static_cast<uint64_t*>(data);
      // cast it to the bigger size then cast away the signedness
      *val = static_cast<uint64_t>(static_cast<int64_t>(sa.GetInt(3)));
      return sa.Return(static_cast<SQInteger>(*val));
    }
    case TypeInfo<float>::TypeID:
    {
      float *val = static_cast<float*>(data);
      *val = sa.GetFloat(3);
      return sa.Return(static_cast<SQFloat>(*val));
    }
    case TypeInfo<double>::TypeID:
    {
      double *val = static_cast<double*>(data);
      *val = sa.GetFloat(3);
      return sa.Return(static_cast<SQFloat>(*val));
    }
    case TypeInfo<const SQChar*>::TypeID:
    {
      const SQChar* *val = static_cast<const SQChar**>(data);
      *val = sa.GetString(3);
      return sa.Return(static_cast<const SQChar*>(*val));
    }
    case TypeInfo<SQUserPointer>::TypeID:
    {
      return sa.ThrowError(_SC("Cannot set user pointer '%s'"), sa.GetString(2));
    }
    case VAR_TYPE_INSTANCE:
    {
      ClassTypeBase *varType = ref->m_varType;
      SQUserPointer src = sa.GetInstanceUp(3, static_cast<SQUserPointer>(varType));
      if (src == 0)
      {
        return sa.ThrowError(_SC("Cannot set user pointer '%s'"), sa.GetString(2));
      }
      varType->GetCopyFunc()(data, src);
      return 0;
    }
    default:
      break;
  }
  
  return sa.ThrowError(_SC("Unknown ScriptVarType '%s'"), sa.GetString(2));
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger VarRef::GetVar(StackHandler& sa, const VarRef *ref, void *data)
{
  if (data == 0)
  {
    return SQ_ERROR;
  }

  switch (ref->m_type)
  {
    case TypeInfo<bool>::TypeID:
    {
      const bool *value = static_cast<const bool*>(data);
      return sa.Return(*value);
    }
    case TypeInfo<char>::TypeID:
    {
      const char *value = static_cast<const char*>(data);
      return sa.Return(static_cast<SQInteger>(*value));
    }
    case TypeInfo<unsigned char>::TypeID:
    {
      const unsigned char *value = static_cast<const unsigned char*>(data);
      return sa.Return(static_cast<SQInteger>(*value));
    }
    case TypeInfo<short>::TypeID:
    {
      const short *value = static_cast<const short*>(data);
      return sa.Return(static_cast<SQInteger>(*value));
    }
    case TypeInfo<unsigned short>::TypeID:
    {
      const unsigned short *value = static_cast<const unsigned short*>(data);
      return sa.Return(static_cast<SQInteger>(*value));
    }
    case TypeInfo<int32_t>::TypeID:
    {
      const int32_t *value = static_cast<const int32_t*>(data);
      return sa.Return(static_cast<SQInteger>(*value));
    }
    case TypeInfo<uint32_t>::TypeID:
    {
      const uint32_t *value = static_cast<const uint32_t*>(data);
      return sa.Return(static_cast<SQInteger>(*value));
    }
    case TypeInfo<int64_t>::TypeID:
    {
      const int64_t *value = static_cast<const int64_t*>(data);
      return sa.Return(static_cast<SQInteger>(*value));
    }
    case TypeInfo<uint64_t>::TypeID:
    {
      const uint64_t *value = static_cast<const uint64_t*>(data);
      return sa.Return(static_cast<SQInteger>(*value));
    }
    case TypeInfo<float>::TypeID:
    {
      const float *value = static_cast<const float*>(data);
      return sa.Return(static_cast<SQFloat>(*value));
    }
    case TypeInfo<double>::TypeID:
    {
      const double *value = static_cast<const double*>(data);
      return sa.Return(static_cast<SQFloat>(*value));
    }
    case TypeInfo<const SQChar*>::TypeID:
    {
      const SQChar *const *value = static_cast<const SQChar *const*>(data);
      return sa.Return(static_cast<const SQChar*>(*value));
    }
    case TypeInfo<SQUserPointer>::TypeID:
      return sa.Return(data);
    case VAR_TYPE_INSTANCE:
      // no release hook is used as the variable is a member of the instance
      if (!CreateNativeClassInstance(
            sa.GetVMPtr(),
            ref->m_varType->m_namespace,
            ref->m_varType->GetTypeName(),
            data,
            0))
      {
        return sa.ThrowError(_SC("Could not create instance '%s'"), ref->m_varType->GetTypeName());
      }
      return 1;
    default:
      break;
  }
  return SQ_ERROR;
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger VarRef::SetVarFunc(HSQUIRRELVM vm)
{
  StackHandler sa(vm);
  if (sa.GetType(1) == OT_TABLE)
  {
    VarRef *ref;
    SQInteger res = GetVarInfo(sa, ref);
    if (res != SQ_OK)
    {
      return res;
    }
    return SetVar(sa, ref, ref->m_offsetOrAddress);
  }
  return SQ_ERROR;
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger VarRef::GetVarFunc(HSQUIRRELVM vm)
{
  StackHandler sa(vm);
  if (sa.GetType(1) == OT_TABLE)
  {
    VarRef *ref;
    SQInteger res = GetVarInfo(sa, ref);
    if (res != SQ_OK)
    {
      return res;
    }
    return GetVar(sa, ref, ref->m_offsetOrAddress);
  }
  return SQ_ERROR;
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger VarRef::SetInstanceVarFunc(HSQUIRRELVM vm)
{
  StackHandler sa(vm);
  if (sa.GetType(1) == OT_INSTANCE)
  {
    VarRef *ref;
    void *data;
    SQInteger res = GetInstanceVarInfo(sa, ref, data);
    if (res != SQ_OK)
    {
      return res;
    }
    return SetVar(sa, ref, data);
  }
  return SQ_ERROR;
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger VarRef::GetInstanceVarFunc(HSQUIRRELVM vm)
{
  StackHandler sa(vm);
  if (sa.GetType(1) == OT_INSTANCE)
  {
    VarRef *ref;
    void *data;
    SQInteger res = GetInstanceVarInfo(sa, ref, data);
    if (res != SQ_OK)
    {
      return res;
    }
    return GetVar(sa, ref, data);
  }
  return SQ_ERROR;
}

}
