#include "sqplus/ClassHelpers.h"

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
bool CreateNativeClassInstance(HSQUIRRELVM vm, HSQOBJECT sqnamespace, const SQChar *classname, SQUserPointer ud, SQRELEASEHOOK hook)
{
  SQInteger oldtop = sq_gettop(vm);

  sq_pushobject(vm, sqnamespace);
  sq_pushstring(vm, classname, -1);
  if (SQ_FAILED(sq_rawget(vm, -2)))
  {
    sq_settop(vm, oldtop);
    return false;
  }

  if (SQ_FAILED(sq_createinstance(vm, -1)))
  {
    sq_settop(vm, oldtop);
    return false;
  }

  // remove root table
  sq_remove(vm, -3);
  // remove class
  sq_remove(vm, -2);

  if (SQ_FAILED(sq_setinstanceup(vm, -1, ud)))
  {
    sq_settop(vm, oldtop);
    return false;
  }

  if (hook)
  {
    sq_setreleasehook(vm, -1, hook);
  }
  
  return true;
}

}
