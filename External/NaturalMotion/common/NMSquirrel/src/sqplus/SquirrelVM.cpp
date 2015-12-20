#include "sqplus/SquirrelVM.h"

#include <new>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <sqstdlib/sqstdio.h>
#include <sqstdlib/sqstdmath.h>
#include <sqstdlib/sqstdstring.h>
#include <sqstdlib/sqstdblob.h>

/*lint -e952 Parameter could be declared const*/
/*lint -e953 Variable could be declared as const*/
/*lint -e1540 Pointer member neither freed nor zeroed by destructor*/
/*lint -e1554 direct pointer copy of member within constructor*/

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
SquirrelVM::SquirrelVM()
: m_vm(0),
  m_rootTable(0),
  m_constTable(0)
{

}

//----------------------------------------------------------------------------------------------------------------------
SquirrelVM::SquirrelVM(HSQUIRRELVM vm)
: m_vm(vm),
  m_rootTable(0),
  m_constTable(0)
{

}

//----------------------------------------------------------------------------------------------------------------------
SquirrelVM::SquirrelVM(const SquirrelVM& rhs)
: m_vm(rhs.m_vm),
  m_rootTable(0),
  m_constTable(0)
{

}

//----------------------------------------------------------------------------------------------------------------------
SquirrelVM::~SquirrelVM()
{
  // set the root table to null
  if (m_rootTable != 0)
  {
    m_rootTable->~SquirrelObject();
    sq_free(m_rootTable, sizeof(SquirrelObject));
    m_rootTable = 0;
  }

  if (m_constTable != 0)
  {
    m_constTable->~SquirrelObject();
    sq_free(m_constTable, sizeof(SquirrelObject));
    m_constTable = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SquirrelVM::Shutdown()
{
  if (m_vm)
  {
    if (m_rootTable != 0)
    {
      m_rootTable->~SquirrelObject();
      sq_free(m_rootTable, sizeof(SquirrelObject));
      m_rootTable = 0;
    }

    if (m_constTable != 0)
    {
      m_constTable->~SquirrelObject();
      sq_free(m_constTable, sizeof(SquirrelObject));
      m_constTable = 0;
    }

    sq_close(m_vm);
    m_vm = 0;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
SQRESULT SquirrelVM::ClearRootTable()
{
  // set the root table to null
  if (m_rootTable)
  {
    m_rootTable->~SquirrelObject();
    sq_free(m_rootTable, sizeof(SquirrelObject));
    m_rootTable = 0;
  }
  sq_pushnull(m_vm);
  return sq_setroottable(m_vm);
}

//----------------------------------------------------------------------------------------------------------------------
void SquirrelVM::PushRootTable()
{
  sq_pushroottable(m_vm);
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject& SquirrelVM::GetRootTable() const
{
  if (m_rootTable == 0)
  {
    void *memory = sq_malloc(sizeof(SquirrelObject));
    m_rootTable = new (memory) SquirrelObject(m_vm);
  }

  sq_pushroottable(m_vm);    
  m_rootTable->AttachToStackObject(-1);
  sq_pop(m_vm, 1);

  return *m_rootTable;
}

//----------------------------------------------------------------------------------------------------------------------
void SquirrelVM::PushConstTable()
{
  sq_pushconsttable(m_vm);
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject& SquirrelVM::GetConstTable() const
{
  if (m_constTable == 0)
  {
    void *memory = sq_malloc(sizeof(SquirrelObject));
    m_constTable = new (memory) SquirrelObject(m_vm);
  }

  sq_pushconsttable(m_vm);    
  m_constTable->AttachToStackObject(-1);
  sq_pop(m_vm, 1);

  return *m_constTable;
}

//----------------------------------------------------------------------------------------------------------------------
SQRESULT SquirrelVM::SetConstTable(const SquirrelObject& table)
{
  // gets rid of const
  sq_pushobject(m_vm, table.m_obj);
  if (SQ_SUCCEEDED(sq_setconsttable(m_vm)))
  {
    if (m_constTable != 0)
    {
      *m_constTable = table;
    }
    return SQ_OK;
  }
  return SQ_ERROR;
}

//----------------------------------------------------------------------------------------------------------------------
SQRESULT SquirrelVM::RegisterStdLibs()
{
  if (SQRESULT result = sqstd_register_iolib(m_vm) != SQ_OK)
  {
    return result;
  }
  if (SQRESULT result = sqstd_register_mathlib(m_vm) != SQ_OK)
  {
    return result;
  }
  if (SQRESULT result = sqstd_register_stringlib(m_vm) != SQ_OK)
  {
    return result;
  }
  return sqstd_register_bloblib(m_vm);
}

//----------------------------------------------------------------------------------------------------------------------
void SquirrelVM::SetPrintFunc(SQPRINTFUNCTION func)
{
  sq_setprintfunc(m_vm, func, func);
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::CompileScript(const SQChar *s, bool printerror, SQRESULT *pResult)
{
  SquirrelObject result(m_vm);
  if (SQ_SUCCEEDED(sqstd_loadfile(m_vm, s, (printerror ? SQTrue : SQFalse))))
  {
    result.AttachToStackObject(-1);
    sq_pop(m_vm, 1);

    if (pResult)
    {
      *pResult = SQ_OK;
    }
  }
  else
  {
    if (pResult)
    {
      *pResult = SQ_ERROR;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::CompileBuffer(const SQChar *s, bool printerror, const SQChar *debugInfo, SQRESULT *pResult)
{
  SquirrelObject result(m_vm);
  if (SQ_SUCCEEDED(sq_compilebuffer(
    m_vm,
    s,
    static_cast<SQInteger>(scstrlen(s)  *sizeof(SQChar)),
    debugInfo,
    (printerror ? SQTrue : SQFalse))))
  {
    result.AttachToStackObject(-1);
    sq_pop(m_vm,1);

    if (pResult)
    {
      *pResult = SQ_OK;
    }
  }
  else
  {
    if (pResult)
    {
      *pResult = SQ_ERROR;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::RunScript(
  const SquirrelObject& script,
  const SquirrelObject *_this,
  bool raiseError,
  SQRESULT *pResult)
{
  SquirrelObject result(m_vm);
  sq_pushobject(m_vm, script.m_obj);

  if (_this)
  {
    sq_pushobject(m_vm, _this->m_obj);
  }
  else
  {
    sq_pushroottable(m_vm);
  }

  if (SQ_SUCCEEDED(sq_call(m_vm, 1, SQTrue, (raiseError ? SQTrue : SQFalse))))
  {
    result.AttachToStackObject(-1);
    sq_pop(m_vm, 2);

    if (pResult)
    {
      *pResult = SQ_OK;
    }
  }
  else
  {
    sq_pop(m_vm, 1);

    if (pResult)
    {
      *pResult = SQ_ERROR;
    }
  }
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::CreateString(const SQChar *str)
{
  SquirrelObject result(m_vm);

  sq_pushstring(m_vm, str, -1);
  result.AttachToStackObject(-1);
  sq_pop(m_vm, 1);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::CreateTable()
{
  SquirrelObject result(m_vm);

  sq_newtable(m_vm);
  result.AttachToStackObject(-1);
  sq_pop(m_vm, 1);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::CreateArray(SQInteger size)
{
  SquirrelObject result;

  sq_newarray(m_vm, size);
  result.AttachToStackObject(-1);
  sq_pop(m_vm, 1);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::CreateUserData(SQUnsignedInteger size, SQUserPointer *newUserData)
{
  SquirrelObject result(m_vm);

  SQUserPointer data = sq_newuserdata(m_vm, size);
  if (newUserData)
  {
    *newUserData = data;
  }
  result.AttachToStackObject(-1);
  sq_pop(m_vm, 1);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::CreateClass(
  const SQChar *name,
  const SQChar *basename,
  SQUserPointer typetag,
  SQRESULT *result)
{
  SquirrelObject newClass(m_vm);

  SQInteger oldtop = GetTop();

  // if there is no table on top add the class to the root table.
  SquirrelObject namespaceTable;
  namespaceTable.AttachToStackObject(m_vm, -1);
  SQObjectType type = namespaceTable.GetType();
  if (type != OT_TABLE && type != OT_CLASS)
  {
    PushRootTable();
  }

  Push(name);

  if (basename)
  {
    Push(basename);
    SQRESULT res = Get(-3);
    if (SQ_FAILED(res))
    {
      SetTop(oldtop);
      if (result)
      {
        *result = res;
      }
      return newClass;
    }
  }

  SQRESULT res = sq_newclass(m_vm, (basename ? SQTrue : SQFalse));
  if (SQ_FAILED(res))
  {
    SetTop(oldtop);
    if (result)
    {
      *result = res;
    }
    return newClass;
  }
  newClass.AttachToStackObject(-1);

  res = sq_settypetag(m_vm, -1, typetag);
  if (SQ_FAILED(res))
  {
    SetTop(oldtop);
    if (result)
    {
      *result = res;
    }
    return newClass;
  }
  NewSlot(-3, false);
  SetTop(oldtop);

  if (result)
  {
    *result = SQ_OK;
  }

  return newClass;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::CreateInstance(const SquirrelObject &classdef)
{
  SquirrelObject result(m_vm);

  // store in case of error
  SQInteger oldtop = sq_gettop(m_vm);

  // gets rid of const
  HSQOBJECT obj = classdef.m_obj;
  sq_pushobject(m_vm, obj);
  if (SQ_FAILED(sq_createinstance(m_vm, -1)))
  {
    sq_settop(m_vm, oldtop);
  }
  else
  {
    result.AttachToStackObject(-1);
    sq_pop(m_vm, 2);
  }
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::CreateFunction(SQFUNCTION func)
{
  SquirrelObject result(m_vm);

  sq_newclosure(m_vm, func, 0);
  result.AttachToStackObject(-1);
  sq_pop(m_vm, 1);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SquirrelObject SquirrelVM::CreateFunction(
  SQFUNCTION func,
  const SQChar *funcName,
  SQInteger nParams,
  const SQChar *typeMask,
  bool isStatic)
{
  SquirrelObject sqfunc(m_vm);

  sq_pushstring(m_vm, funcName, -1);
  sq_newclosure(m_vm, func, 0);
  
  sqfunc.AttachToStackObject(-1);

  if (typeMask != 0)
  {
    SQRESULT result = sq_setparamscheck(m_vm, nParams, typeMask);
    if (SQ_FAILED(result))
    {
      // todo: handle error
    }
  }

  SQRESULT result = sq_newslot(m_vm, -3, (isStatic ? SQTrue : SQFalse));
  if (SQ_FAILED(result))
  {
    // todo: handle error
  }

  return sqfunc;
}

//----------------------------------------------------------------------------------------------------------------------
void SquirrelVM::PrintFunc(HSQUIRRELVM SQUIRREL_UNUSED(v), const SQChar *s, ...)
{
  // should maybe use vprintf?
  if (s != 0)
  {
    va_list vl;
    va_start(vl, s);
#if defined(SQUNICODE)
    vwprintf(s, vl);
#else
    vprintf(s, vl);
#endif
    va_end(vl);
  }
}
  
//----------------------------------------------------------------------------------------------------------------------
}
