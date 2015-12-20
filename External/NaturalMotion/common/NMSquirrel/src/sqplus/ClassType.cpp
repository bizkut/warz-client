#include "sqplus/ClassType.h"
#include "sqplus/VarRef.h"

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
ClassTypeBase::ClassTypeBase()
: m_pbase(0),
  m_name(0),
  m_offset(0),
  m_mayHaveOffset(-1),
  m_releaseHook(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool ClassTypeBase::MayHaveOffset()
{
  if (m_mayHaveOffset < 0)
  {
    if (m_offset)
    {
      m_mayHaveOffset = 1;
    }
    else
    {
      m_mayHaveOffset = m_pbase ? m_pbase->MayHaveOffset() : 0;
    }
  }
  return (m_mayHaveOffset != 0);
}

//----------------------------------------------------------------------------------------------------------------------
size_t ClassTypeBase::GetOffsetTo(ClassTypeBase *pbase)
{
  if (m_pbase == 0)
  {
    return 0;
  }
  return (m_pbase == pbase) ? m_offset : m_offset + m_pbase->GetOffsetTo(pbase);
}

}
