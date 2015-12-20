/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#ifndef RENDERER_CONFIG_H
#define RENDERER_CONFIG_H

#include <RendererFoundation.h>
#include <assert.h>

namespace SampleRenderer
{
	// 2D and 3D Textures have identical external interfaces
	//    Using a typedef provides compatibility with legacy code that used only 2D textures
	class RendererTexture;
	class RendererTextureDesc;
	typedef RendererTexture     RendererTexture2D;
	typedef RendererTextureDesc RendererTexture2DDesc;
	typedef RendererTexture     RendererTexture3D;
	typedef RendererTextureDesc RendererTexture3DDesc;
}

#define RENDERER_TEXT(_foo)   #_foo
#define RENDERER_TEXT2(_foo)  RENDERER_TEXT(_foo)

// number of lights required before it switches from forward rendering to deferred rendering.
#define RENDERER_DEFERRED_THRESHOLD  0x7FFFFFFF // set to a big number just to disable it for now...

// Enables/Disables support for dresscode in the renderer...
#define RENDERER_ENABLE_DRESSCODE 0

// If turned on, asserts get compiled in as print statements in release mode.
#define RENDERER_ENABLE_CHECKED_RELEASE 0

// If enabled, all lights will be bound in a single pass. Requires appropriate shader support.
#define RENDERER_ENABLE_SINGLE_PASS_LIGHTING 0

// maximum number of bones per-drawcall allowed.
#if defined(__CELLOS_LV2__)
#define RENDERER_MAX_BONES 50
#else
#define RENDERER_MAX_BONES 60
#endif

#define RENDERER_TANGENT_CHANNEL            5
#define RENDERER_BONEINDEX_CHANNEL          6
#define RENDERER_BONEWEIGHT_CHANNEL         7
#define RENDERER_INSTANCE_POSITION_CHANNEL  8
#define RENDERER_INSTANCE_NORMALX_CHANNEL   9
#define RENDERER_INSTANCE_NORMALY_CHANNEL  10
#define RENDERER_INSTANCE_NORMALZ_CHANNEL  11
#define RENDERER_INSTANCE_VEL_LIFE_CHANNEL 12
#define RENDERER_INSTANCE_DENSITY_CHANNEL  13

#define RENDERER_INSTANCE_UV_CHANNEL       12
#define RENDERER_INSTANCE_LOCAL_CHANNEL    13

#define RENDERER_DISPLACEMENT_CHANNEL      14
#define RENDERER_X_DISPLACEMENT_CHANNEL    13
#define RENDERER_Y_DISPLACEMENT_CHANNEL    14
#define RENDERER_Z_DISPLACEMENT_CHANNEL    15
#define RENDERER_DISPLACEMENT_FLAGS_CHANNEL 15

// Compiler specific configuration...
#if defined(_MSC_VER)
#define RENDERER_VISUALSTUDIO
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4100) // unreferenced formal parameter

#elif defined(__GNUC__)
#define RENDERER_GCC
#elif defined (SN_TARGET_PSP2)
	#define RENDERER_GXM
#else
#error Unknown Compiler!

#endif

// Platform specific configuration...
#if defined(WIN32) || defined(WIN64)
#define RENDERER_WINDOWS
#define RENDERER_ENABLE_DIRECT3D9
#define RENDERER_ENABLE_DIRECT3D11
#define RENDERER_ENABLE_NVPERFHUD
#define RENDERER_ENABLE_TGA_SUPPORT
#if defined(WIN64)
#define RENDERER_64BIT
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#elif defined(__APPLE__) && !defined(__arm__)
#define RENDERER_MACOSX
#define RENDERER_ENABLE_OPENGL
#define RENDERER_ENABLE_CG
#if !defined(PX_PPC)
#define RENDERER_ENABLE_TGA_SUPPORT
#endif

#elif defined(_XBOX) && _XBOX_VER == 200
#define RENDERER_XBOX360
#define RENDERER_ENABLE_DIRECT3D9
#define NOMINMAX
#define RENDERER_ENABLE_TGA_SUPPORT
#define RENDERER_BIG_ENDIAN

#elif defined(__CELLOS_LV2__)
#define RENDERER_PS3
#define RENDERER_ENABLE_OPENGL
#define RENDERER_ENABLE_LIBGCM
#define RENDERER_ENABLE_CG
#define RENDERER_ENABLE_TGA_SUPPORT
#define RENDERER_BIG_ENDIAN

#elif defined(__CYGWIN__)
#define RENDERER_LINUX
#define RENDERER_ENABLE_OPENGL
#define RENDERER_ENABLE_CG
#define RENDERER_ENABLE_TGA_SUPPORT

#elif defined(ANDROID)
	#define RENDERER_DEBUG
	#define RENDERER_ANDROID
	#define RENDERER_TABLET
	#define RENDERER_ENABLE_GLES2
	#define RENDERER_ENABLE_TGA_SUPPORT

#elif defined(__linux__)
	#define RENDERER_LINUX
	#define RENDERER_ENABLE_OPENGL
	#define RENDERER_ENABLE_CG
	#define RENDERER_ENABLE_TGA_SUPPORT

#elif defined(__APPLE__) && defined(__arm__)
	#define RENDERER_DEBUG
	#define RENDERER_IOS
	#define RENDERER_TABLET
	#define RENDERER_ENABLE_GLES2
	#define RENDERER_ENABLE_TGA_SUPPORT
	#define RENDERER_ENABLE_PVR_SUPPORT

#elif defined(SN_TARGET_PSP2)
	#define RENDERER_PSP2
	#define RENDERER_ENABLE_LIBGXM
	#define RENDERER_ENABLE_TGA_SUPPORT
	#define RENDERER_ENABLE_GXT_SUPPORT 

#elif defined(DINGO)
#define RENDERER_WINDOWS
#define RENDERER_ENABLE_DIRECT3D11
#define RENDERER_ENABLE_TGA_SUPPORT
#define RENDERER_64BIT
#include <windows.h>

#elif defined(PS4)
	#define RENDERER_PS4
#define RENDERER_ENABLE_OPENGL
#define RENDERER_ENABLE_LIBGCM
#define RENDERER_ENABLE_CG
#define RENDERER_ENABLE_TGA_SUPPORT

#else
#error "Unknown Platform!"

#endif

// To enable creating a Razor capture file, add ENABLE_RAZOR_CAPTURE to the Preprocessor Definitions in SampleRenderer-MT  
#if defined (ENABLE_RAZOR_CAPTURE)
	#define RAZOR_CAPTURE_NAME "app0:razor_capture.sgx"
	#include <razor_capture.h>
#endif

#if defined(_DEBUG)
#define RENDERER_DEBUG
#endif


#if defined(RENDERER_DEBUG)
	#if defined(RENDERER_WINDOWS)
		#define RENDERER_ASSERT(_exp, _msg)                     \
		    if(!(_exp))                                         \
		    {                                                   \
		        MessageBoxA(0, _msg, "Renderer Assert", MB_OK); \
		        __debugbreak();                                 \
		    }
	#elif defined(RENDERER_ANDROID)
	#include <android/log.h>
	#include "SamplePlatform.h"
	#define RENDERER_ASSERT(_exp, _msg)														\
		if(!(_exp)) {																		\
			__android_log_print(ANDROID_LOG_INFO, _msg, "RENDERER_ASSERT: %s\n", #_exp);    \
			if(SampleFramework::SamplePlatform::platform()) 								\
				SampleFramework::SamplePlatform::platform()->showMessage("Renderer assert", _msg);   			\
		}																					\
		assert(_exp && (_msg));
	#else
		#define RENDERER_ASSERT(_exp, _msg) assert(_exp && (_msg));
	#endif
#elif RENDERER_ENABLE_CHECKED_RELEASE
	#if defined(RENDERER_VISUALSTUDIO)
		#define RENDERER_ASSERT(_exp, _msg)                                          \
			if(!(_exp))                                                              \
			{                                                                        \
				OutputDebugStringA("*** (" __FILE__":"RENDERER_TEXT2(__LINE__)") "); \
				OutputDebugStringA(_msg);                                            \
				OutputDebugStringA(" ***\n");                                        \
			}
	#else
		#define RENDERER_ASSERT(_exp, _msg) if(!(_exp)) printf("*** (" __FILE__ ":" RENDERER_TEXT2(__LINE__)") %s ***\n", _msg);
	#endif
#else
	#define RENDERER_ASSERT(_exp, _msg)
#endif

#define RENDERER_OUTPUT_MESSAGE(_rendererPtr, _msg) \
	if((_rendererPtr) && (_rendererPtr)->getErrorCallback()) \
	{ \
		(_rendererPtr)->getErrorCallback()->reportError(PxErrorCode::eDEBUG_INFO, (_msg), __FILE__, __LINE__); \
	}


#if defined(RENDERER_ANDROID)
	#include <android/log.h>
	#define LOG_INFO(title, ...) ((void)__android_log_print(ANDROID_LOG_INFO, title, __VA_ARGS__))
#else

#if 0
	#include <stdio.h>
	#include <stdarg.h>

	static void printInfo(const char* title, const char* message, ...)
	{
		char buff[4096];
		
		printf("%s ", title);
		va_list va;
		va_start(va, message);
		vsprintf(buff, message, va);
		va_end(va);
		if (strlen(buff)>=4096)
			assert(!"buffer overflow!!");
		
		printf("%s\n", buff);
	}

	#define LOG_INFO(title, ...) (printInfo(title, __VA_ARGS__))
#else
	#define LOG_INFO(title, ...)
#endif

#endif

#ifdef RENDERER_XBOX360
#define RENDERER_INSTANCING 0
#else
#define RENDERER_INSTANCING 1
#endif

#endif
