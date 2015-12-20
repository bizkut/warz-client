/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
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
#ifndef RENDERER_TEXTURE_H
#define RENDERER_TEXTURE_H

#include <RendererConfig.h>

namespace SampleRenderer
{

	class RendererTextureDesc;

	class RendererTexture
	{
	public:
		enum Format
		{
			FORMAT_B8G8R8A8 = 0,
			FORMAT_R8G8B8A8,
			FORMAT_A8,
			FORMAT_R32F,

			FORMAT_DXT1,
			FORMAT_DXT3,
			FORMAT_DXT5,

			FORMAT_D16,
			FORMAT_D24S8,

			FORMAT_GXT,

			FORMAT_PVR_2BPP,
			FORMAT_PVR_4BPP,

			NUM_FORMATS
		}_Format;

		enum Filter
		{
			FILTER_NEAREST = 0,
			FILTER_LINEAR,
			FILTER_ANISOTROPIC,

			NUM_FILTERS
		}_Filter;

		enum Addressing
		{
			ADDRESSING_WRAP = 0,
			ADDRESSING_CLAMP,
			ADDRESSING_MIRROR,

			NUM_ADDRESSING
		}_Addressing;

	public:
		static PxU32 computeImageByteSize(PxU32 width, PxU32 height, PxU32 depth, Format format);
		static PxU32 getLevelDimension(PxU32 dimension, PxU32 level);
		static bool  isCompressedFormat(Format format);
		static bool  isDepthStencilFormat(Format format);
		static PxU32 getFormatNumBlocks(PxU32 dimension, Format format);
		static PxU32 getFormatBlockSize(Format format);

	protected:
		RendererTexture(const RendererTextureDesc &desc);
		virtual ~RendererTexture(void);

	public:
		void release(void) { delete this; }

		Format     getFormat(void)      const { return m_format; }
		Filter     getFilter(void)      const { return m_filter; }
		Addressing getAddressingU(void) const { return m_addressingU; }
		Addressing getAddressingV(void) const { return m_addressingV; }
		PxU32      getWidth(void)       const { return m_width; }
		PxU32      getHeight(void)      const { return m_height; }
		PxU32      getDepth(void)       const { return m_depth; }
		PxU32      getNumLevels(void)   const { return m_numLevels; }

		PxU32      getWidthInBlocks(void)  const;
		PxU32      getHeightInBlocks(void) const;
		PxU32      getBlockSize(void)      const;

	public:
		//! pitch is the number of bytes between the start of each row.
		virtual void *lockLevel(PxU32 level, PxU32 &pitch) = 0;
		virtual void  unlockLevel(PxU32 level)             = 0;
		virtual	void  select(PxU32 stageIndex)	           = 0;

	private:
		RendererTexture &operator=(const RendererTexture&) { return *this; }

	private:
		Format     m_format;
		Filter     m_filter;
		Addressing m_addressingU;
		Addressing m_addressingV;
		PxU32      m_width;
		PxU32      m_height;
		PxU32      m_depth;
		PxU32      m_numLevels;
	};

} // namespace SampleRenderer

#endif
