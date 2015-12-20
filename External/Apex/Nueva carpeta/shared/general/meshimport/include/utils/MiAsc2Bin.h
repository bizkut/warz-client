// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
// distribution of this software and related documentation without an express 
// license agreement from NVIDIA Corporation is strictly prohibited.
// 
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.

#ifndef ASC2BIN_H
#define ASC2BIN_H

// types:
//
//          f   : 4 byte MiF32
//          d   : 4 byte integer
//          c   : 1 byte character
//          b   : 1 byte integer
//          h   : 2 byte integer
//          p   : 4 byte const char *
//          x1  : 1 byte hex
//          x2  : 2 byte hex
//          x4  : 4 byte hex (etc)
// example usage:
//
//    Asc2Bin("1 2 3 4 5 6",1,"fffff",0);

#include "MiPlatformConfig.h"

namespace mimp
{

void * Asc2Bin(const char *source,const MiI32 count,const char *ctype,void *dest=0);

};

#endif // ASC2BIN_H
