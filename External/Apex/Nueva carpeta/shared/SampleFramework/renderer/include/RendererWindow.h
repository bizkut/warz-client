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
#ifndef RENDERER_WINDOW_H
#define RENDERER_WINDOW_H

#include <RendererConfig.h>

namespace SampleFramework {
	class SamplePlatform;
}

namespace SampleRenderer
{

	class RendererWindow
	{
	public:
		RendererWindow(void);
		virtual ~RendererWindow(void);

		bool open(PxU32 width, PxU32 height, const char *title, bool fullscreen=false);
		void close(void);

		bool isOpen(void) const;

		// update the window's state... handle messages, etc.
		void update(void);

		// resize the window...
		void setSize(PxU32 width, PxU32 height);

		// get/set the window's title...
		void getTitle(char *title, PxU32 maxLength) const;
		void setTitle(const char *title);

		bool	hasFocus() const;
		void	setFocus(bool b);

		SampleFramework::SamplePlatform* getPlatform() { return m_platform; }
	public:
		// called just AFTER the window opens.
		virtual void onOpen(void) {}

		// called just BEFORE the window closes. return 'true' to confirm the window closure.
		virtual bool onClose(void) { return true; }

		// called just AFTER the window is resized.
		virtual void onResize(PxU32 /*width*/, PxU32 /*height*/) {}

		// called when the window's contents needs to be redrawn.
		virtual void onDraw(void) = 0;

	protected:
		SampleFramework::SamplePlatform*				m_platform;

	private:
		bool											m_isOpen;
	};

} // namespace SampleRenderer

#endif
