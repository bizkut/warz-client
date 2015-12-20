#ifndef WINDOWS_SAMPLE_PLATFORM_H
#define WINDOWS_SAMPLE_PLATFORM_H
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
#include <SamplePlatform.h>
#include <windows/WindowsSampleUserInput.h>

struct IDirect3D9;
struct IDirect3DDevice9;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGIFactory1;
struct IDXGISwapChain;

namespace SampleFramework
{
	class WindowsPlatform : public SamplePlatform 
	{
	public:
		explicit							WindowsPlatform(SampleRenderer::RendererWindow* _app);
		virtual								~WindowsPlatform();
		// System
		virtual void						showCursor(bool);
		virtual void						postRendererSetup(SampleRenderer::Renderer* renderer);
		virtual size_t						getCWD(char* path, size_t len);
		virtual void						setCWDToEXE(void);
		virtual bool						openWindow(physx::PxU32& width, 
														physx::PxU32& height,
														const char* title,
														bool fullscreen);
		virtual bool						useWindow(physx::PxU64 hwnd);
		virtual void						update();
		virtual bool						isOpen();
		virtual bool						closeWindow();
		virtual bool						hasFocus() const;
		virtual void						getTitle(char *title, physx::PxU32 maxLength) const;
		virtual void						setTitle(const char *title);
		virtual void						setFocus(bool b);
		virtual physx::PxU64				getWindowHandle();
		virtual void						setWindowSize(physx::PxU32 width, 
															physx::PxU32 height);
		virtual void						getWindowSize(physx::PxU32& width, physx::PxU32& height);
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
		virtual bool						isD3D9ok();
		virtual void*						initializeD3D11();
		virtual bool						isD3D11ok();
		// Rendering
		virtual void						initializeOGLDisplay(const SampleRenderer::RendererDesc& desc, 
																physx::PxU32& width, 
																physx::PxU32& height);
		virtual void						postInitializeOGLDisplay();
		virtual void						setOGLVsync(bool on);
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
		virtual physx::PxU64				getD3D9TextureFormat(SampleRenderer::RendererTexture2D::Format format);
		virtual physx::PxU32				D3D11Present(bool vsync);
		virtual physx::PxU64				getD3D11TextureFormat(SampleRenderer::RendererTexture2D::Format format);
		virtual bool						makeContextCurrent();
		virtual void						freeDisplay();
		virtual bool						isContextValid();
		virtual void						swapBuffers();
		virtual void						setupRendererDescription(SampleRenderer::RendererDesc& renDesc);
		// Input
		virtual void						doInput();
		// File System
		virtual bool						makeSureDirectoryPathExists(const char* dirPath);

		virtual const SampleUserInput*		getSampleUserInput() const { return &m_windowsSampleUserInput; }
		virtual SampleUserInput*			getSampleUserInput() { return &m_windowsSampleUserInput; }

		WindowsSampleUserInput&				getWindowsSampleUserInput() { return m_windowsSampleUserInput; }
		const WindowsSampleUserInput&		getWindowsSampleUserInput() const { return m_windowsSampleUserInput; }

		virtual const char*					getPlatformName() const { return m_platformName; }

		virtual void						setMouseCursorRecentering(bool val);
		virtual bool						getMouseCursorRecentering() const;
		

		physx::PxVec2						getMouseCursorPos() const { return m_mouseCursorPos; }
		void								setMouseCursorPos(const physx::PxVec2& pos) { m_mouseCursorPos = pos; }
		void								recenterMouseCursor(bool generateEvent);
	
	protected:	
		IDirect3D9*							m_d3d;
		IDirect3DDevice9*					m_d3dDevice;
		IDXGIFactory1*						m_dxgiFactory;
		IDXGISwapChain*						m_dxgiSwap;
		ID3D11Device*						m_d3d11Device;
		ID3D11DeviceContext*				m_d3d11DeviceContext;
		HWND								m_hwnd;
		HDC									m_hdc;
		HGLRC								m_hrc;
		HMODULE								m_library;
		HMODULE								m_dxgiLibrary;
		HMODULE								m_d3d11Library;
		bool								m_ownsWindow;
		bool								m_isHandlingMessages;
		bool								m_destroyWindow;
		bool								m_hasFocus;
		bool								m_showCursor;
		char								m_platformName[256];
		WindowsSampleUserInput				m_windowsSampleUserInput;
		physx::PxVec2						m_mouseCursorPos;
		bool								m_recenterMouseCursor;
		bool								m_vsync;
	};
}

#endif
