#ifndef R3DASSERT_H_INCLUDED
#define R3DASSERT_H_INCLUDED

// copy pasted content of <assert.h>
#undef  assert

extern void r3d_actual_assert(const char *expr, const char *filename, unsigned lineno, bool docrash);

// pt: asserts are working in release mode now
#ifdef FINAL_BUILD
  extern bool r3dOutToLog(const char* Str, ...);
  #define assert(_Expression) (void)( (!!(_Expression)) || (r3dOutToLog("ASSERT at file %s, line %d: "#_Expression"\n", __FILE__, __LINE__)) )
  #define r3d_assert(_Expression) (void)( (!!(_Expression)) || (r3dOutToLog("ASSERT at file %s, line %d: "#_Expression"\n", __FILE__, __LINE__)) )
  //#define assert(_Expression) ((void)0)
#else //FINAL_BUILD
#if defined(WO_SERVER) // server version of asserts - show log and CRASH
  #define assert(_Expression) (void)( (!!(_Expression)) || (r3d_actual_assert(#_Expression, __FILE__, __LINE__, true), 0) )
  #define r3d_assert(_Expression) (void)( (!!(_Expression)) || (r3d_actual_assert(#_Expression, __FILE__, __LINE__, true), 0) )
#else
  #define assert(_Expression) (void)( (!!(_Expression)) || (r3d_actual_assert(#_Expression, __FILE__, __LINE__, false), 0) )
  #define r3d_assert(_Expression) (void)( (!!(_Expression)) || (r3d_actual_assert(#_Expression, __FILE__, __LINE__, false), 0) )
#endif
#endif //FINAL_BUILD

#endif