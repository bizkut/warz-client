
#ifndef SAMPLE_TEXTURE_ASSET_H
#define SAMPLE_TEXTURE_ASSET_H
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
#include <SampleAsset.h>

#include <stdio.h>

namespace SampleRenderer
{
	class Renderer;
	class RendererTexture;
}

namespace SampleFramework
{

	class SampleTextureAsset : public SampleAsset
	{
		friend class SampleAssetManager;

	public:
		enum Type
		{
			DDS,
			TGA,
			BMP,
			GXT,
			PVR,
			TEXTURE_FILE_TYPE_COUNT,
		};
	protected:
		SampleTextureAsset(SampleRenderer::Renderer &renderer, FILE &file, const char *path, Type texType);
		SampleTextureAsset(SampleRenderer::Renderer &renderer, 
						   const void* pixels, int width, int height, int bitsPerPixel, 
						   bool invertWidth, bool invertHeight, bool swapRB,
						   const char* path);
		virtual ~SampleTextureAsset(void);

	public:
		SampleRenderer::RendererTexture *getTexture(void);
		const SampleRenderer::RendererTexture *getTexture(void) const;

	public:
		virtual bool isOk(void) const;

	private:
		void loadDDS(SampleRenderer::Renderer &renderer, FILE &file);
		void loadTGA(SampleRenderer::Renderer &renderer, FILE &file);
		void loadBitmap(SampleRenderer::Renderer &renderer, 
		                const void* pixels, int width, int bitsPerPixel, int height, 
		                bool invertWidth = false, bool invertHeight = false, bool swapRB = false);
		void loadGXT(SampleRenderer::Renderer &renderer, FILE &file);
		void loadPVR(SampleRenderer::Renderer &renderer, FILE &file);

		SampleRenderer::RendererTexture *m_texture;
	};

} // namespace SampleFramework

#endif
