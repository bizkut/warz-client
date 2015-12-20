/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#define KY_MAJOR_VERSION 2014
#define KY_MINOR_VERSION 5
#define KY_PATCH_VERSION 4
//#define KY_SPECIFIC_VERSION expectedQualifierForASpecificVersion



// ///////////////////////////////////////////////////////////
// The following macros should not be used directly outside of Navigation SDK, please use Kaim::Version's functions instead.
//
// KY_QUOTED_SPECIFIC_VERSION gives KY_SPECIFIC_VERSION as a char*
// KY_VERSION_NAME gives the name of the version including the specific version
// __NOTE__: If the format of the two string in the aforementionned macros is changed be sure to update Kaim::VersionReader and Kaim::VersionBlob accordingly.
//
// KY_CHECK_MATCHING_VERSION_FUNCTION_NAME creates a name for a function used to find out if a desynchro occurred between header files and libs,
// KY_EXPANDMACRO_VERSION_FUNCTION_NAME and KY_TOKENPASTE_VERSION_FUNCTION_NAME are used
// to be able to expand the macros given in parameter before pasting them to form the function name.
//

#define KY_VERSION \
	KY_QUOTE(KY_MAJOR_VERSION) "." KY_QUOTE(KY_MINOR_VERSION) "." KY_QUOTE(KY_PATCH_VERSION)

#if !defined(KY_SPECIFIC_VERSION)
# define KY_QUOTED_SPECIFIC_VERSION                                           ""
# define KY_VERSION_NAME                                                      KY_VERSION
# define KY_TOKENPASTE_VERSION_FUNCTION_NAME(major, minor, patch)             CheckVersion_##major##_##minor##_##patch
# define KY_EXPANDMACRO_VERSION_FUNCTION_NAME(major, minor, patch)            KY_TOKENPASTE_VERSION_FUNCTION_NAME(major, minor, patch)
# define KY_CHECK_MATCHING_VERSION_FUNCTION_NAME                              \
	KY_EXPANDMACRO_VERSION_FUNCTION_NAME(KY_MAJOR_VERSION, KY_MINOR_VERSION, KY_PATCH_VERSION)
#else
# define KY_QUOTED_SPECIFIC_VERSION                                           KY_QUOTE(KY_SPECIFIC_VERSION)
# define KY_VERSION_NAME                                                      KY_VERSION "-" KY_QUOTED_SPECIFIC_VERSION
# define KY_TOKENPASTE_VERSION_FUNCTION_NAME(major, minor, patch, specific)   CheckVersion_##major##_##minor##_##patch##_##specific
# define KY_EXPANDMACRO_VERSION_FUNCTION_NAME(major, minor, patch, specific)  KY_TOKENPASTE_VERSION_FUNCTION_NAME(major, minor, patch, specific)
# define KY_CHECK_MATCHING_VERSION_FUNCTION_NAME                              \
	KY_EXPANDMACRO_VERSION_FUNCTION_NAME(KY_MAJOR_VERSION, KY_MINOR_VERSION, KY_PATCH_VERSION,KY_SPECIFIC_VERSION)
#endif


