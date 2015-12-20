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

#include <RendererConfig.h>
#include <RendererTexture.h>
#include <RendererTextureDesc.h>
#include "foundation/PxMath.h"

using namespace SampleRenderer;

static PxU32 computeCompressedDimension(PxU32 dimension)
{
	if(dimension < 4)
	{
		dimension = 4;
	}
	else
	{
		PxU32 mod = dimension % 4;
		if(mod) dimension += 4 - mod;
	}
	return dimension;
}

PxU32 RendererTexture::computeImageByteSize(PxU32 width, PxU32 height, PxU32 depth, RendererTexture::Format format)
{
	PxU32 size = 0;
	PxU32 numPixels = width * height * depth;
	switch(format)
	{
	case RendererTexture::FORMAT_B8G8R8A8:
	case RendererTexture::FORMAT_R8G8B8A8:
		size = numPixels * sizeof(PxU8) * 4;
		break;
	case RendererTexture::FORMAT_A8:
		size = numPixels * sizeof(PxU8) * 1;
		break;
	case RendererTexture::FORMAT_R32F:
		size = numPixels * sizeof(float);
		break;
	case RendererTexture::FORMAT_DXT1:
		width  = computeCompressedDimension(width);
		height = computeCompressedDimension(height);
		size   = computeImageByteSize(width, height, depth, RendererTexture::FORMAT_B8G8R8A8) / 8;
		break;
	case RendererTexture::FORMAT_DXT3:
	case RendererTexture::FORMAT_DXT5:
		width  = computeCompressedDimension(width);
		height = computeCompressedDimension(height);
		size   = computeImageByteSize(width, height, depth, RendererTexture::FORMAT_B8G8R8A8) / 4;
		break;
	case RendererTexture::FORMAT_PVR_2BPP:
		{
			PxU32 bytesPerBlock = 8 * 4 * 2; //8 by 4 pixels times bytes per pixel
			width = PxMax(PxI32(width / 8), 2);
			height = PxMax(PxI32(height / 4), 2);
			size = width*height*bytesPerBlock / 8;
		}
		break;
	case RendererTexture::FORMAT_PVR_4BPP:
		{
			PxU32 bytesPerBlock = 4 * 4 * 4; //4 by 4 pixels times bytes per pixel
			width = PxMax(PxI32(width / 4), 2);
			height = PxMax(PxI32(height / 4), 2);
			size = width*height*bytesPerBlock / 8;
		}
	default: break;
	}
	RENDERER_ASSERT(size, "Unable to compute Image Size.");
	return size;
}

PxU32 RendererTexture::getLevelDimension(PxU32 dimension, PxU32 level)
{
	dimension >>=level;
	if(!dimension) dimension=1;
	return dimension;
}

bool RendererTexture::isCompressedFormat(Format format)
{
	return (format >= FORMAT_DXT1 && format <= FORMAT_DXT5);
}

bool RendererTexture::isDepthStencilFormat(Format format)
{
	return (format >= FORMAT_D16 && format <= FORMAT_D24S8);
}

PxU32 RendererTexture::getFormatNumBlocks(PxU32 dimension, Format format)
{
	PxU32 blockDimension = 0;
	if(isCompressedFormat(format))
	{
		blockDimension = dimension / 4;
		if(dimension % 4) blockDimension++;
	}
	else
	{
		blockDimension = dimension;
	}
	return blockDimension;
}

PxU32 RendererTexture::getFormatBlockSize(Format format)
{
	return computeImageByteSize(1, 1, 1, format);
}

RendererTexture::RendererTexture(const RendererTextureDesc &desc)
{
	m_format      = desc.format;
	m_filter      = desc.filter;
	m_addressingU = desc.addressingU;
	m_addressingV = desc.addressingV;
	m_width       = desc.width;
	m_height      = desc.height;
	m_depth       = desc.depth;
	m_numLevels   = desc.numLevels;
}

RendererTexture::~RendererTexture(void)
{

}

PxU32 RendererTexture::getWidthInBlocks(void) const
{
	return getFormatNumBlocks(getWidth(), getFormat());
}

PxU32 RendererTexture::getHeightInBlocks(void) const
{
	return getFormatNumBlocks(getHeight(), getFormat());
}

PxU32 RendererTexture::getBlockSize(void) const
{
	return getFormatBlockSize(getFormat());
}
