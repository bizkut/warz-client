/*	see copyright notice in squirrel.h */
#ifndef _SQSTD_AUXLIB_H_
#define _SQSTD_AUXLIB_H_

#include <sqstdlib/interface.h>

#ifdef __cplusplus
extern "C" {
#endif

  SQSTDLIB_API void sqstd_seterrorhandlers(HSQUIRRELVM v);
  SQSTDLIB_API void sqstd_printcallstack(HSQUIRRELVM v);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* _SQSTD_AUXLIB_H_ */
