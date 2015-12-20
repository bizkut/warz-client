#ifdef _MSC_VER
#pragma once
#endif
#ifndef _SQPLUS_CLASSTYPE_INL_
#define _SQPLUS_CLASSTYPE_INL_

namespace sqp
{
//----------------------------------------------------------------------------------------------------------------------
/// ClassTypeBase
//----------------------------------------------------------------------------------------------------------------------
inline const SQChar *ClassTypeBase::GetTypeName() const
{
  return m_name;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline void ClassTypeCopyImpl<T, true>::copy(T *dst, T *src)
{
  *dst = *src;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline void ClassTypeCopyImpl<T, false>::copy(T *dst, T *src)
{
  memcpy(dst, src, sizeof(T));
}

//----------------------------------------------------------------------------------------------------------------------
inline void ClassTypeCopyImpl<void, true>::copy(void *SQUIRREL_UNUSED(dst), void *SQUIRREL_UNUSED(src))
{
}

//----------------------------------------------------------------------------------------------------------------------
inline void ClassTypeCopyImpl<void, false>::copy(void *SQUIRREL_UNUSED(dst), void *SQUIRREL_UNUSED(src))
{
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline ClassType<T>::ClassType()
{
  m_name = TypeInfo<T>().m_typeName;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline CopyVarFunc ClassType<T>::GetCopyFunc()
{
  return reinterpret_cast<CopyVarFunc>(&ClassTypeBase::copy);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
template<class BC>
void ClassType<T>::SetBase(TypeWrapper<BC>)
{
  m_pbase = ClassType<BC>::Get();
  // get the this pointer as the type T (could be anything instead of this)
  const T *pt = reinterpret_cast<T *>(this);
  // subtract the type pointer from the same pointer cast to the base type
  // the cast to base type offsets the pointer if there is any offset between types BC and T
  // subtracting T* from BC* for the same instance will result in the offset.
  m_offset = reinterpret_cast<const char *>(pt) - reinterpret_cast<const char *>(static_cast<const BC *>(pt));
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline void ClassType<T>::SetReleaseHook(SQRELEASEHOOK releaseHook)
{
  m_releaseHook = releaseHook;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline SQRELEASEHOOK ClassType<T>::GetReleaseHook() const
{
  return m_releaseHook;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
inline HSQOBJECT ClassType<T>::GetNamespace() const
{
  return m_namespace;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
ClassType<T> *ClassType<T>::Get()
{
  static ClassType<T> st_ct;
  return &st_ct;
}

}

#endif
