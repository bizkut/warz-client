#include "sqplus/StackHandler.h"

#include <stdio.h>
#include <stdarg.h>

#include "sqplus/ClassType.h"

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
const SQChar *StackHandler::GetTypeName(SQInteger idx) const
{
  if (idx > 0 && idx <= m_top)
  {
    SQObjectType type = sq_gettype(m_vm, idx);
    // handle instance, class or userdata first
    if (type == OT_INSTANCE || type == OT_CLASS || type == OT_USERDATA)
    {
      // try and get the typetag.
      SQUserPointer typetag = 0;
      if (SQ_SUCCEEDED(sq_gettypetag(m_vm, idx, &typetag)))
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
    return RawTypeToTypeName(type);
  }

  return _SC("");
}

//----------------------------------------------------------------------------------------------------------------------
SQRESULT StackHandler::ThrowError(const SQChar *format, ...)
{
  if (format != 0)
  {
    const SQInteger increment = 256;
    SQInteger size = increment;
    SQChar *buffer = sq_getscratchpad(m_vm, size);
    if (buffer)
    {
      va_list vargs;
      va_start(vargs, format);
      int length = scvsprintf(buffer, size, format, vargs);
      va_end(vargs);
      while (length == -1)
      {
        size += increment;
        buffer = sq_getscratchpad(m_vm, size);
        if (buffer == 0)
        {
          break;
        }
        va_start(vargs, format);
        length = scvsprintf(buffer, size, format, vargs);
        va_end(vargs);
      }
      return sq_throwerror(m_vm, buffer);
    }
    return sq_throwerror(m_vm, _SC("StackHandler::ThrowError(): unable to format message, sq_getscratchpad returned null"));
  }
  return sq_throwerror(m_vm, _SC("Null format string passed to StackHandler::ThrowError()"));
}

}
