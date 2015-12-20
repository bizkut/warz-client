/*	see copyright notice in squirrel.h */
#ifndef _SQSTDBLOB_H_
#define _SQSTDBLOB_H_

#include <sqstdlib/interface.h>

#ifdef __cplusplus
extern "C" {
#endif

  SQSTDLIB_API SQUserPointer sqstd_createblob(HSQUIRRELVM v, SQInteger size);
  SQSTDLIB_API SQRESULT sqstd_getblob(HSQUIRRELVM v,SQInteger idx,SQUserPointer *ptr);
  SQSTDLIB_API SQInteger sqstd_getblobsize(HSQUIRRELVM v,SQInteger idx);

  SQSTDLIB_API SQRESULT sqstd_register_bloblib(HSQUIRRELVM v);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*_SQSTDBLOB_H_*/

