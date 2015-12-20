#ifndef SAMPLE_PLATFORM_H
#define SAMPLE_PLATFORM_H
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
#include <RendererDesc.h>
#include <RendererWindow.h>
#include <RendererTexture2D.h>

namespace SampleFramework
{
	class SampleApplication;
	class SampleUserInput;

	/* This class declares and partly implements platform-abstraction level.
		Do not use platform-specific types and functions here.
	*/
	class SamplePlatform 
	{
	protected:
		SampleRenderer::RendererWindow*		m_app;
		SampleApplication*					m_sf_app;
		static SamplePlatform*				m_platform;

	public:
		// access
		static SamplePlatform*				platform();
		static void							setPlatform(SamplePlatform*);
		SampleApplication*					application();

		// creation
		explicit							SamplePlatform(SampleRenderer::RendererWindow* _app);
		virtual								~SamplePlatform() = 0;
		// System
		virtual void						showCursor(bool);

		virtual size_t						getCWD(char* path, size_t len);
		virtual void						setCWDToEXE(void);
		virtual void						popPathSpec(char *path);
		virtual bool						preOpenWindow(void * ptr);
		virtual bool						openWindow(physx::PxU32& width, 
														physx::PxU32& height,
														const char* title,
														bool fullscreen);
		virtual bool						useWindow(physx::PxU64 hwnd);
		virtual void						update();
		virtual bool						closeWindow();
		virtual bool						updateWindow();
		virtual bool						hasFocus() const;
		virtual void						setFocus(bool b);
		virtual bool						isOpen();
		virtual physx::PxU64				getWindowHandle();
		virtual void						setWindowSize(physx::PxU32 width, 
															physx::PxU32 height);
		virtual void						getWindowSize(physx::PxU32& width, physx::PxU32& height);
		virtual void						getTitle(char *title, physx::PxU32 maxLength) const;
		virtual void						setTitle(const char *title);

		virtual void						setMouseCursorRecentering(bool val) {}
		virtual bool						getMouseCursorRecentering() const { return false; }

		virtual void						showMessage(const char* title, const char* message);
		virtual bool						saveBitmap(const char* fileName, 
														physx::PxU32 width, 
														physx::PxU32 height, 
														physx::PxU32 sizeInBytes, 
														const void* data);
		virtual void*						compileProgram(void * context, 
															const char* assetDir, 
															const char *programPath, 
															physx::PxU64 profile, 
															const char* passString, 
															const char *entry, 
															const char **args);
		virtual void*						initializeD3D9();
		virtual void*						initializeD3D11();
		virtual bool						isD3D9ok();
		virtual const char*					getPathSeparator();
		virtual bool						isD3D11ok();
		// Rendering
		virtual void						initializeCGRuntimeCompiler();
		virtual void						initializeOGLDisplay(const SampleRenderer::RendererDesc& desc,
																 physx::PxU32& width, 
																 physx::PxU32& height);
		virtual physx::PxU32				initializeD3D9Display(void * presentParameters, 
																char* m_deviceName, 
																  physx::PxU32& width, 
																  physx::PxU32& height,
																void * m_d3dDevice_out);
		virtual physx::PxU32				initializeD3D11Display(void *dxgiSwapChainDesc, 
																char *m_deviceName, 
																   physx::PxU32& width, 
																   physx::PxU32& height,
																void *m_d3dDevice_out,
																void *m_d3dDeviceContext_out,
																void *m_dxgiSwap_out);
		virtual physx::PxU32				D3D9Present();
		virtual void						D3D9BlockUntilNotBusy(void * resource);
		virtual void						D3D9DeviceBlockUntilIdle();
		virtual physx::PxU64				getD3D9TextureFormat(SampleRenderer::RendererTexture2D::Format format);
		virtual physx::PxU32				D3D11Present(bool vsync);
		virtual physx::PxU64				getD3D11TextureFormat(SampleRenderer::RendererTexture2D::Format format);
		virtual void						postInitializeOGLDisplay();
		virtual void						setOGLVsync(bool on);
		virtual bool						makeContextCurrent();
		virtual bool						isContextValid();
		virtual void						freeDisplay();
		virtual void						swapBuffers();
		virtual void						postRendererRelease();
		virtual void						preRendererSetup();
		virtual void						postRendererSetup(SampleRenderer::Renderer* renderer);
		virtual void						setupRendererDescription(SampleRenderer::RendererDesc& renDesc);
		// Input
		virtual void						doInput();

		virtual void						showSoftInput();

		virtual const SampleUserInput*		getSampleUserInput() const = 0;
		virtual SampleUserInput*			getSampleUserInput() = 0;

		virtual const char*					getPlatformName() const { return NULL; }
		// File System
		virtual bool						makeSureDirectoryPathExists(const char* dirPath);
	};

	SamplePlatform*		createPlatform(SampleRenderer::RendererWindow* _app);
}

#endif
