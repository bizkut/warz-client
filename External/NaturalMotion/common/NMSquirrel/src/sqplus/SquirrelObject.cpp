#include "sqplus/SquirrelObject.h"

#include "sqplus/ClassType.h"
#include "sqplus/BaseHeader.h"
#include "sqplus/SquirrelVM.h"

/*lint -e534 ignoring return value of function*/
/*lint -e1554 direct pointer copy of member within constructor*/
/*lint -e1555 direct pointer copy of member*/

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
SquirrelObject::SquirrelObject()
: m_vm(0)
{
  sq_resetobject(&m_obj);
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject::SquirrelObject(HSQUIRRELVM vm)
: m_vm(vm)
{
  sq_resetobject(&m_obj);
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject::SquirrelObject(const SquirrelObject& rhs)
: m_vm(rhs.m_vm),
  m_obj(rhs.m_obj)
{
  if (m_vm)
  {
    sq_addref(m_vm, &m_obj);
  }
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject::~SquirrelObject()
{
  if (m_vm)
  {
    sq_release(m_vm, &m_obj);
    m_vm = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
const SquirrelObject& SquirrelObject::operator = (const SquirrelObject& rhs)
{
  if (&rhs == this)
  {
    return *this;
  }

  if (!m_vm)
  {
    m_vm = rhs.m_vm;
  }

  // have to use a temp as rhs.m_obj is const
  HSQOBJECT temp = rhs.m_obj;
  sq_addref(m_vm, &temp);
  sq_release(m_vm, &m_obj);

  m_obj = temp;

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void SquirrelObject::AttachToStackObject(SQInteger idx, SQRESULT *pResult)
{
  SQRESULT res = SQ_ERROR;

  if (m_vm)
  {
    HSQOBJECT obj;
    res = sq_getstackobj(m_vm, idx, &obj);
    sq_addref(m_vm, &obj);

    sq_release(m_vm, &m_obj);
    m_obj = obj;
  }
  if (pResult)
  {
    *pResult = res;
  }
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelObject::Clone() const
{
  SquirrelObject result(m_vm);
  if (sq_isarray(m_obj) || sq_isinstance(m_obj) || sq_istable(m_obj))
  {
    sq_pushobject(m_vm, m_obj);
    sq_clone(m_vm, -1);

    result.AttachToStackObject(-1);

    sq_pop(m_vm, 2);
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger SquirrelObject::GetSize() const
{
  SQInteger result = 0;
  if(sq_isarray(m_obj) || sq_istable(m_obj) || sq_isstring(m_obj) || sq_isuserdata(m_obj)) {
    sq_pushobject(m_vm, m_obj);
    result = sq_getsize(m_vm, -1);
    sq_pop(m_vm, 1);
  }
  return result ;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetInstanceUP(SQUserPointer instance)
{
  if (!sq_isinstance(m_obj))
  {
    return false; 
  }

  sq_pushobject(m_vm, m_obj);
  sq_setinstanceup(m_vm, -1, instance);
  sq_pop(m_vm, 1);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
SQUserPointer SquirrelObject::GetInstanceUP(SQUserPointer typetag)
{
  SQUserPointer instance = 0;

  sq_pushobject(m_vm, m_obj);
  sq_getinstanceup(m_vm, -1, reinterpret_cast<SQUserPointer*>(&instance), typetag);
  sq_pop(m_vm, 1);

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::NewUserData(const SQChar *key, SQUnsignedInteger size, SQUserPointer *typetag)
{
  bool ret = false;
  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushstring(m_vm, key, -1);

  sq_newuserdata(m_vm, size);
  if (typetag)
  {
    sq_settypetag(m_vm, -1, typetag);
  }

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::GetUserData(const SQChar *key, SQUserPointer *data, SQUserPointer *typetag)
{
  bool ret = false;
  if (GetSlot(key))
  {
    sq_getuserdata(m_vm, -1, data, typetag);
    sq_pop(m_vm, 1);
    ret = true;
  }
  sq_pop(m_vm, 1);
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::RawGetUserData(const SQChar *key, SQUserPointer *data, SQUserPointer *typetag)
{
  bool ret = false;
  if (RawGetSlot(key))
  {
    sq_getuserdata(m_vm, -1, data, typetag);
    sq_pop(m_vm, 1);
    ret = true;
  }
  sq_pop(m_vm, 1);
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
SQRESULT SquirrelObject::GetTypeTag(SQUserPointer *typeTag) const
{
  return sq_getobjtypetag(const_cast<HSQOBJECT*>(&m_obj), typeTag);
}

//----------------------------------------------------------------------------------------------------------------------
const SQChar *SquirrelObject::GetTypeName(const SQChar *key) const
{
  SquirrelObject so = GetValue(key);
  if (so.IsNull())
  {
    return 0;
  }
  return so.GetTypeName();
}

//----------------------------------------------------------------------------------------------------------------------
const SQChar *SquirrelObject::GetTypeName(SQInteger key) const
{
  SquirrelObject so = GetValue(key);
  if (so.IsNull())
  {
    return 0;
  }
  return so.GetTypeName();
}

//----------------------------------------------------------------------------------------------------------------------
const SQChar *SquirrelObject::GetTypeName() const
{
  // handle instance, class or userdata first
  if (m_obj._type == OT_INSTANCE || m_obj._type == OT_CLASS || m_obj._type == OT_USERDATA)
  {
    // try and get the typetag.
    SQUserPointer typetag = 0;
    if (SQ_SUCCEEDED(sq_getobjtypetag(const_cast<HSQOBJECT*>(&m_obj), &typetag)))
    {
      // if there was a type tag assume it was bound with sqplus and use the name of the ClassTypeBase.
      if (typetag)
      {
        ClassTypeBase *ctb = static_cast<ClassTypeBase*>(typetag);
        return ctb->GetTypeName();
      }
    }
  }

  // if the type isn't an instance, class or userdata or there was no typetag return the raw string type.
  return RawTypeToTypeName(m_obj._type);
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::ArrayAppend(bool value)
{
  if (sq_isarray(m_obj))
  {
    sq_pushobject(m_vm, m_obj);
    sq_pushbool(m_vm, (value ? SQTrue : SQFalse));
    SQRESULT result = sq_arrayappend(m_vm, -2);
    sq_pop(m_vm, 1);

    return SQ_SUCCEEDED(result);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::ArrayAppend(SQInteger value)
{
  if (sq_isarray(m_obj))
  {
    sq_pushobject(m_vm, m_obj);
    sq_pushinteger(m_vm, value);
    SQRESULT result = sq_arrayappend(m_vm, -2);
    sq_pop(m_vm, 1);

    return SQ_SUCCEEDED(result);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::ArrayAppend(SQFloat value)
{
  if (sq_isarray(m_obj))
  {
    sq_pushobject(m_vm, m_obj);
    sq_pushfloat(m_vm, value);
    SQRESULT result = sq_arrayappend(m_vm, -2);
    sq_pop(m_vm, 1);

    return SQ_SUCCEEDED(result);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::ArrayAppend(const SQChar *value)
{
  if (sq_isarray(m_obj))
  {
    sq_pushobject(m_vm, m_obj);
    sq_pushstring(m_vm, value, -1);
    SQRESULT result = sq_arrayappend(m_vm, -2);
    sq_pop(m_vm, 1);

    return SQ_SUCCEEDED(result);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::ArrayAppend(const SquirrelObject& value)
{
  if (sq_isarray(m_obj))
  {
    sq_pushobject(m_vm, m_obj);
    sq_pushobject(m_vm, value.m_obj);
    SQRESULT result = sq_arrayappend(m_vm, -2);
    sq_pop(m_vm, 1);

    return SQ_SUCCEEDED(result);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelObject::ArrayPop(bool retval)
{
  SquirrelObject result(m_vm);
  SQInteger top = sq_gettop(m_vm);

  sq_pushobject(m_vm, m_obj);
  if (SQ_SUCCEEDED(sq_arraypop(m_vm, -1, (retval ? SQTrue : SQFalse))))
  {
    if (retval)
    {
      result.AttachToStackObject(-1);
    }
  }

  sq_settop(m_vm, top);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::BeginIteration()
{
  if (!sq_istable(m_obj) && !sq_isarray(m_obj) && !sq_isclass(m_obj))
  {
    return false;
  }

  sq_pushobject(m_vm, m_obj);
  sq_pushnull(m_vm);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::Next(SquirrelObject &key, SquirrelObject &value)
{
  if (SQ_SUCCEEDED(sq_next(m_vm, -2)))
  {
    key.AttachToStackObject(-2);
    value.AttachToStackObject(-1);
    sq_pop(m_vm, 2);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void SquirrelObject::EndIteration()
{
  sq_pop(m_vm, 2);
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetDelegate(SquirrelObject &obj)
{
  if (sq_istable(obj.m_obj) || sq_isnull(obj.m_obj))
  {
    sq_pushobject(m_vm, m_obj);
    sq_pushobject(m_vm, obj.m_obj);
    if (SQ_SUCCEEDED(sq_setdelegate(m_vm, -2)))
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelObject::GetDelegate()
{
  SquirrelObject result;
  if (sq_istable(m_obj) || sq_isuserdata(m_obj))
  {
    sq_pushobject(m_vm, m_obj);
    sq_getdelegate(m_vm, -1);
    result.AttachToStackObject(-1);
    sq_pop(m_vm, 2);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::Exists(SQInteger key) const
{
  if (GetSlot(key))
  {
    sq_pop(m_vm, 2);
    return true;
  }
  else
  {
    sq_pop(m_vm, 1);
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::Exists(const SQChar *key) const
{
  if (GetSlot(key))
  {
    sq_pop(m_vm, 2);
    return true;
  }
  else
  {
    sq_pop(m_vm, 1);
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::Exists(const SquirrelObject& key) const
{
  if (GetSlot(key))
  {
    sq_pop(m_vm, 2);
    return true;
  }
  else
  {
    sq_pop(m_vm, 1);
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelObject::CreateTable(SQInteger key)
{
  SquirrelObject table(GetVMPtr());

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  
  sq_pushinteger(m_vm, key);
  
  sq_newtable(m_vm);
  SQRESULT result;
  table.AttachToStackObject(-1, &result);
  if (SQ_SUCCEEDED(result))
  {
    if (SQ_FAILED(sq_rawset(m_vm, -3)))
    {
      table = SquirrelObject(m_vm);
    }
  }

  sq_settop(m_vm, top);
  
  return table;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelObject::CreateTable(const SQChar *key)
{
  SquirrelObject table(GetVMPtr());

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);

  sq_pushstring(m_vm, key, -1);

  sq_newtable(m_vm);
  SQRESULT result;
  table.AttachToStackObject(-1, &result);
  if (SQ_SUCCEEDED(result))
  {
    if (SQ_FAILED(sq_rawset(m_vm, -3)))
    {
      table = SquirrelObject(m_vm);
    }
  }

  sq_settop(m_vm, top);

  return table;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelObject::CreateTable(const SquirrelObject& key)
{
  SquirrelObject table(GetVMPtr());

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);

  sq_pushobject(m_vm, key.m_obj);

  sq_newtable(m_vm);
  SQRESULT result;
  table.AttachToStackObject(-1, &result);
  if (SQ_SUCCEEDED(result))
  {
    if (SQ_FAILED(sq_rawset(m_vm, -3)))
    {
      table = SquirrelObject(m_vm);
    }
  }

  sq_settop(m_vm, top);

  return table;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(SQInteger key, bool value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushinteger(m_vm, key);

  sq_pushbool(m_vm, (value ? SQTrue : SQFalse));

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(SQInteger key, SQInteger value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushinteger(m_vm, key);

  sq_pushinteger(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(SQInteger key, SQFloat value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushinteger(m_vm, key);

  sq_pushfloat(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(SQInteger key, const SQChar *value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushinteger(m_vm, key);

  sq_pushstring(m_vm, value, -1);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(SQInteger key, const SquirrelObject& value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushinteger(m_vm, key);

  sq_pushobject(m_vm, value.m_obj);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(const SQChar *key, bool value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushstring(m_vm, key, -1);

  sq_pushbool(m_vm, (value ? SQTrue : SQFalse));

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(const SQChar *key, SQInteger value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushstring(m_vm, key, -1);

  sq_pushinteger(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(const SQChar *key, SQFloat value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushstring(m_vm, key, -1);

  sq_pushfloat(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(const SQChar *key, const SQChar *value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushstring(m_vm, key, -1);

  sq_pushstring(m_vm, value, -1);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(const SQChar *key, const SquirrelObject& value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushstring(m_vm, key, -1);

  sq_pushobject(m_vm, value.m_obj);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(const SquirrelObject& key, bool value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushobject(m_vm, key.m_obj);

  sq_pushbool(m_vm, (value ? SQTrue : SQFalse));

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(const SquirrelObject& key, SQInteger value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushobject(m_vm, key.m_obj);

  sq_pushinteger(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(const SquirrelObject& key, SQFloat value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushobject(m_vm, key.m_obj);

  sq_pushfloat(m_vm, value);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(const SquirrelObject& key, const SQChar *value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushobject(m_vm, key.m_obj);

  sq_pushstring(m_vm, value, -1);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::SetValue(const SquirrelObject& key, const SquirrelObject& value)
{
  bool ret = false;

  SQInteger top = sq_gettop(m_vm);
  sq_pushobject(m_vm, m_obj);
  sq_pushobject(m_vm, key.m_obj);

  sq_pushobject(m_vm, value.m_obj);

  if (SQ_SUCCEEDED(sq_rawset(m_vm, -3)))
  {
    ret = true;
  }
  sq_settop(m_vm, top);

  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::GetBool(SQInteger key) const
{
  SQBool result = SQFalse;
  if (GetSlot(key))
  {
    sq_getbool(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result != SQFalse;
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger SquirrelObject::GetInteger(SQInteger key) const
{
  SQInteger result = 0;
  if (GetSlot(key))
  {
    sq_getinteger(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SQFloat SquirrelObject::GetFloat(SQInteger key) const
{
  float result = 0.0f;
  if (GetSlot(key))
  {
    sq_getfloat(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const SQChar *SquirrelObject::GetString(SQInteger key) const
{
  const SQChar *result = 0;
  if (GetSlot(key))
  {
    sq_getstring(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelObject::GetValue(SQInteger key) const
{
  SquirrelObject result(m_vm);
  if (GetSlot(key))
  {
    result.AttachToStackObject(-1);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::GetBool(const SQChar *key) const
{
  SQBool result = SQFalse;
  if (GetSlot(key))
  {
    sq_getbool(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result != SQFalse;
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger SquirrelObject::GetInteger(const SQChar *key) const
{
  SQInteger result = 0;
  if (GetSlot(key))
  {
    sq_getinteger(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SQFloat SquirrelObject::GetFloat(const SQChar *key) const
{
  float result = 0.0f;
  if (GetSlot(key))
  {
    sq_getfloat(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const SQChar *SquirrelObject::GetString(const SQChar *key) const
{
  const SQChar *result = 0;
  if (GetSlot(key))
  {
    sq_getstring(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelObject::GetValue(const SQChar *key) const
{
  SquirrelObject result(m_vm);
  if (GetSlot(key))
  {
    result.AttachToStackObject(-1);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::GetBool(const SquirrelObject& key) const
{
  SQBool result = SQFalse;
  if (GetSlot(key))
  {
    sq_getbool(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result != SQFalse;
}

//----------------------------------------------------------------------------------------------------------------------
SQInteger SquirrelObject::GetInteger(const SquirrelObject& key) const
{
  SQInteger result = 0;
  if (GetSlot(key))
  {
    sq_getinteger(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SQFloat SquirrelObject::GetFloat(const SquirrelObject& key) const
{
  float result = 0.0f;
  if (GetSlot(key))
  {
    sq_getfloat(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const SQChar *SquirrelObject::GetString(const SquirrelObject& key) const
{
  const SQChar *result = 0;
  if (GetSlot(key))
  {
    sq_getstring(m_vm, -1, &result);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelObject::GetValue(const SquirrelObject& key) const
{
  SquirrelObject result(m_vm);
  if (GetSlot(key))
  {
    result.AttachToStackObject(-1);
    sq_pop(m_vm, 1);
  }
  sq_pop(m_vm, 1);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::GetSlot(const SQInteger key) const
{
  sq_pushobject(m_vm, m_obj);
  sq_pushinteger(m_vm, key);
  if (SQ_SUCCEEDED(sq_get(m_vm, -2)))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::GetSlot(const SQChar *key) const
{
  sq_pushobject(m_vm, m_obj);
  sq_pushstring(m_vm, key, -1);
  if (SQ_SUCCEEDED(sq_get(m_vm, -2)))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::GetSlot(const SquirrelObject& key) const
{
  sq_pushobject(m_vm, m_obj);
  sq_pushobject(m_vm, key.m_obj);
  if (SQ_SUCCEEDED(sq_get(m_vm, -2)))
  {
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------------------------------
bool SquirrelObject::RawGetSlot(const SQInteger key) const
{
  sq_pushobject(m_vm, m_obj);
  sq_pushinteger(m_vm, key);
  if (SQ_SUCCEEDED(sq_rawget(m_vm, -2)))
  {
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------------------------------
bool SquirrelObject::RawGetSlot(const SQChar *key) const
{
  sq_pushobject(m_vm, m_obj);
  sq_pushstring(m_vm, key, -1);
  if (SQ_SUCCEEDED(sq_rawget(m_vm, -2)))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelObject::RawGetSlot(const SquirrelObject& key) const
{
  sq_pushobject(m_vm, m_obj);
  sq_pushobject(m_vm, key.m_obj);
  if (SQ_SUCCEEDED(sq_rawget(m_vm, -2)))
  {
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------------------------------
}
