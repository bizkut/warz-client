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
#include <SampleTextureAsset.h>

#include <Renderer.h>
#include <RendererTexture2D.h>
#include <RendererTexture2DDesc.h>
#include <RendererMemoryMacros.h>

#include "nv_dds.h"

#ifdef RENDERER_ENABLE_TGA_SUPPORT
# include "targa.h"
#endif

#ifdef RENDERER_ENABLE_GXT_SUPPORT
#include <gxt.h>
#endif

#ifdef RENDERER_ENABLE_PVR_SUPPORT
#include "pvrt.h"
#endif

using namespace SampleFramework;

SampleTextureAsset::SampleTextureAsset(SampleRenderer::Renderer &renderer, FILE &file, const char *path, Type texType) :
	SampleAsset(ASSET_TEXTURE, path)
{
	m_texture = 0;

	switch(texType)
	{
	case DDS: loadDDS(renderer, file); break;
	case TGA: loadTGA(renderer, file); break;
	case GXT: loadGXT(renderer, file); break;
	case PVR: loadPVR(renderer, file); break;
	default: PX_ASSERT(0 && "Invalid texture type requested"); break;
	}
}

SampleTextureAsset::SampleTextureAsset(SampleRenderer::Renderer &renderer, 
									   const void* pixels, int width, int height, int bitsPerPixel, 
									   bool invertWidth, bool invertHeight, bool swapRB,
									   const char* path) :
	SampleAsset(ASSET_TEXTURE, path)
{
	m_texture = 0;
	
	loadBitmap(renderer, pixels, width, height, bitsPerPixel, invertWidth, invertHeight, swapRB);
}

SampleTextureAsset::~SampleTextureAsset(void)
{
	SAFE_RELEASE(m_texture);
}

void SampleTextureAsset::loadDDS(SampleRenderer::Renderer &renderer, FILE &file) 
{
#ifdef RENDERER_PSP2
	RENDERER_ASSERT(0, "DDS format currently unsupported on PSP2 convert dds to gxt please.");
#endif

	nv_dds::CDDSImage ddsimage;
	bool ok = ddsimage.load(&file, false);
	PX_ASSERT(ok);
	if(ok)
	{
		SampleRenderer::RendererTexture2DDesc tdesc;
		nv_dds::TextureFormat ddsformat = ddsimage.get_format();
		switch(ddsformat)
		{
		case nv_dds::TextureBGRA:      tdesc.format = SampleRenderer::RendererTexture2D::FORMAT_B8G8R8A8; break;
		case nv_dds::TextureDXT1:      tdesc.format = SampleRenderer::RendererTexture2D::FORMAT_DXT1;     break;
		case nv_dds::TextureDXT3:      tdesc.format = SampleRenderer::RendererTexture2D::FORMAT_DXT3;     break;
		case nv_dds::TextureDXT5:      tdesc.format = SampleRenderer::RendererTexture2D::FORMAT_DXT5;     break;
		default:					   PX_ALWAYS_ASSERT(); break;
		}
		tdesc.width     = ddsimage.get_width();
		tdesc.height    = ddsimage.get_height();
		tdesc.depth     = ddsimage.get_depth();
		// if there is 1 mipmap, nv_dds reports 0
		tdesc.numLevels = ddsimage.get_num_mipmaps()+1;
		PX_ASSERT(tdesc.isValid());
		m_texture = renderer.createTexture(tdesc);
		PX_ASSERT(m_texture);
		if(m_texture)
		{
			PxU32 pitch  = 0;
			void *buffer = m_texture->lockLevel(0, pitch);
			PX_ASSERT(buffer);
			if(buffer)
			{
				//PxU32 size = ddsimage.get_size();
				for (PxU32 d = 0; d < ddsimage.get_depth(); ++d)
				{
					const PxU32 levelWidth  = m_texture->getWidthInBlocks();
					const PxU32 levelHeight = m_texture->getHeightInBlocks();
					const PxU32 rowSrcSize  = levelWidth * m_texture->getBlockSize();

					PxU8       *levelDst    = (PxU8*)buffer                   + d * pitch      * levelHeight;
					const PxU8 *levelSrc    = (PxU8*)(unsigned char*)ddsimage + d * rowSrcSize * levelHeight;
					PX_ASSERT(rowSrcSize <= pitch); // the pitch can't be less than the source row size.
					for(PxU32 row=0; row<levelHeight; row++)
					{
						memcpy(levelDst, levelSrc, rowSrcSize);
						levelDst += pitch;
						levelSrc += rowSrcSize;
					}
				}
				m_texture->unlockLevel(0);

				for(PxU32 i=1; i<tdesc.numLevels; i++)
				{
					void *buffer = m_texture->lockLevel(i, pitch);
					PX_ASSERT(buffer);
					if(buffer && pitch)
					{
						for (PxU32 d = 0; d < ddsimage.get_depth(); ++d)
						{
							const nv_dds::CSurface &surface = ddsimage.get_mipmap(i-1);
							//PxU32 size = surface.get_size();

							const PxU32 levelWidth  = SampleRenderer::RendererTexture2D::getFormatNumBlocks(surface.get_width(),  m_texture->getFormat());
							const PxU32 levelHeight = SampleRenderer::RendererTexture2D::getFormatNumBlocks(surface.get_height(), m_texture->getFormat());
							const PxU32 rowSrcSize  = levelWidth * m_texture->getBlockSize();

							PxU8       *levelDst    = (PxU8*)buffer                  + d * pitch      * levelHeight;
							const PxU8 *levelSrc    = (PxU8*)(unsigned char*)surface + d * rowSrcSize * levelHeight;
							PX_ASSERT(rowSrcSize <= pitch); // the pitch can't be less than the source row size.
							for(PxU32 row=0; row<levelHeight; row++)
							{
								memcpy(levelDst, levelSrc, rowSrcSize);
								levelDst += pitch;
								levelSrc += rowSrcSize;
							}
						}
						m_texture->unlockLevel(i);
					}
				}
			}
		}
	}
}

void SampleTextureAsset::loadGXT(SampleRenderer::Renderer &renderer, FILE &file)
{
#ifdef RENDERER_ENABLE_GXT_SUPPORT
	char file2[512];

	strcpy(file2, getPath());

	char* endStr = strstr(file2,".dds");	
	strcpy(endStr, ".gxt");

	FILE *f = NULL;

	if(strstr(file2, "savedata0"))
	{
		f =  fopen(file2, "rb");
	}
	else
	{
		// add path its in samples folder
		char file3[512];
		sprintf(file3, "app0:%s%s", ASSET_DIR, file2);


		f = fopen(file3, "rb");
	}

	if(!f)
	{
		RENDERER_ASSERT(f, "Failed to load gxt.");	
	}

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buffer = new char[size+1];
	fread(buffer, 1, size, f);
	fclose(f);
	buffer[size] = '\0';

	SceGxtErrorCode err = sceGxtCheckData(buffer);

	RENDERER_ASSERT(err == SCE_OK, "GXT texture data not correct.");

	const SceGxtHeader *hdr = (const SceGxtHeader*) buffer;

	PxU32 textureIndex = 0;

	const SceGxtTextureInfo* info = (const SceGxtTextureInfo*)(hdr + 1) + textureIndex;

	SampleRenderer::RendererTexture2DDesc tdesc;
	tdesc.format = SampleRenderer::RendererTexture2D::FORMAT_GXT;
	tdesc.width     = info->width;
	tdesc.height    = info->height;	
	// temp just 1 
	tdesc.numLevels = 1 ;//info->mipCount;
	tdesc.data = buffer;

	PX_ASSERT(tdesc.isValid());
	m_texture = renderer.createTexture2D(tdesc);
	PX_ASSERT(m_texture);
	
	delete buffer;
#endif
}

void SampleTextureAsset::loadPVR(SampleRenderer::Renderer& renderer, FILE& file)
{
#ifdef RENDERER_ENABLE_PVR_SUPPORT	
	fseek(&file, 0, SEEK_END);
	size_t size = ftell(&file);
	fseek(&file, 0, SEEK_SET);
	char* fileBuffer = new char[size+1];
	fread(fileBuffer, 1, size, &file);
	fclose(&file);
	fileBuffer[size] = '\0';
	
	const PVRTextureInfo info(fileBuffer);
	PX_ASSERT(info.data);
	
	SampleRenderer::RendererTexture2DDesc tdesc;
	if (info.glFormat == GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG)
	{
		tdesc.format = SampleRenderer::RendererTexture2D::FORMAT_PVR_4BPP;
	}
	else 
	{
		tdesc.format = SampleRenderer::RendererTexture2D::FORMAT_PVR_2BPP;
	}

	tdesc.width     = info.width;
	tdesc.height    = info.height;	
	tdesc.numLevels = info.mipCount + 1;
	tdesc.data = NULL;
	
	PX_ASSERT(tdesc.isValid());
	m_texture = renderer.createTexture2D(tdesc);
	PX_ASSERT(m_texture);
	if (!info.data)
	{
		delete[] fileBuffer;
		return;
	}
	
	PxU32 pitch  = 0;
	PxU8* levelDst = (PxU8*)m_texture->lockLevel(0, pitch);
	const PxU8* levelSrc = (PxU8*)info.data;

	PX_ASSERT(levelDst && levelSrc);
	{
		PxU32 levelWidth = tdesc.width;
		PxU32 levelHeight = tdesc.height;
		const PxU32 levelSize = m_texture->computeImageByteSize(levelWidth, levelHeight, tdesc.format);
		memcpy(levelDst, levelSrc, levelSize);
		levelSrc += levelSize;
	}
	m_texture->unlockLevel(0);
	
	for(PxU32 i=1; i<tdesc.numLevels; i++)
	{
		PxU8* levelDst = (PxU8*)m_texture->lockLevel(i, pitch);
		PX_ASSERT(levelDst);
		{
			const PxU32 levelWidth  = m_texture->getLevelDimension(tdesc.width, i);
			const PxU32 levelHeight = m_texture->getLevelDimension(tdesc.height, i);
			const PxU32 levelSize  = m_texture->computeImageByteSize(levelWidth, levelHeight, tdesc.format);
			memcpy(levelDst, levelSrc, levelSize);
			levelSrc += levelSize;
		}
		m_texture->unlockLevel(i);
	}
	
	delete[] fileBuffer;
	
#endif
}

void SampleTextureAsset::loadTGA(SampleRenderer::Renderer &renderer, FILE &file)
{
#ifdef RENDERER_ENABLE_TGA_SUPPORT

	tga_image* image = new tga_image();
	bool ok = (TGA_NOERR == tga_read_from_FILE( image, &file ));

	// flip it to make it look correct in the SampleFramework's renderer
	tga_flip_vert( image );

	PX_ASSERT(ok);
	if( ok )
	{
		SampleRenderer::RendererTexture2DDesc tdesc;
		int componentCount = image->pixel_depth/8;
		if( componentCount == 3 || componentCount == 4 )
		{
			tdesc.format = SampleRenderer::RendererTexture2D::FORMAT_B8G8R8A8;

			tdesc.width     = image->width;
			tdesc.height    = image->height;

			tdesc.numLevels = 1;
			PX_ASSERT(tdesc.isValid());
			m_texture = renderer.createTexture2D(tdesc);
			PX_ASSERT(m_texture);

			if(m_texture)
			{
				PxU32 pitch  = 0;
				void *buffer = m_texture->lockLevel(0, pitch);
				PX_ASSERT(buffer);
				if(buffer)
				{
					PxU8       *levelDst    = (PxU8*)buffer;
					const PxU8 *levelSrc    = (PxU8*)image->image_data;
					const PxU32 levelWidth  = m_texture->getWidthInBlocks();
					const PxU32 levelHeight = m_texture->getHeightInBlocks();
					const PxU32 rowSrcSize  = levelWidth * m_texture->getBlockSize();
					PX_ASSERT(rowSrcSize <= pitch); // the pitch can't be less than the source row size.
					for(PxU32 row=0; row<levelHeight; row++)
					{ 
						if( componentCount == 3 )
						{
							// copy per pixel to handle RBG case, based on component count
							for(PxU32 col=0; col<levelWidth; col++)
							{
								*levelDst++ = levelSrc[0];
								*levelDst++ = levelSrc[1];
								*levelDst++ = levelSrc[2];
								*levelDst++ = 0xFF; //alpha
								levelSrc += componentCount;
							}
						}
						else
						{
							memcpy(levelDst, levelSrc, rowSrcSize);
							levelDst += pitch;
							levelSrc += rowSrcSize;
						}
					}
				}
				m_texture->unlockLevel(0);
			}
		}
	}
	free(image->image_data);
	delete image;

#endif /* RENDERER_ENABLE_TGA_SUPPORT */
}

void SampleTextureAsset::loadBitmap(SampleRenderer::Renderer &renderer, 
									const void* pixels, int width, int height, int bitsPerPixel,
									bool invertWidth, bool invertHeight, bool swapRB)
{
	SampleRenderer::RendererTexture2DDesc tdesc;
	tdesc.format	= SampleRenderer::RendererTexture2D::FORMAT_B8G8R8A8;
	tdesc.width		= width;
	tdesc.height	= height;
	tdesc.numLevels	= 1;

	PX_ASSERT(tdesc.isValid());
	m_texture		= renderer.createTexture2D(tdesc);
	PX_ASSERT(m_texture);

	const PxU32 componentCount = bitsPerPixel / 8 ;

	if(m_texture)
	{
		PxU32 pitch = 0;
		void* buffer = m_texture->lockLevel(0, pitch);
		PX_ASSERT(buffer);
		if(buffer)
		{
			PxU8* levelDst			= (PxU8*)buffer;
			const PxU8* levelSrc	= (PxU8*)pixels;
			const PxU32 levelWidth	= m_texture->getWidthInBlocks();
			const PxU32 levelHeight	= m_texture->getHeightInBlocks();
			PX_ASSERT(levelWidth * m_texture->getBlockSize() <= pitch); // the pitch can't be less than the source row size.
			static const int srcOrders[2][3] = {{0, 1, 2}, {2, 1, 0}};
			const int* srcOrder = swapRB ? srcOrders[1] : srcOrders[0];
			for(PxU32 row=0; row<levelHeight; row++)
			{ 
				const PxU8* rowSrc = NULL;
				if (invertHeight == false)
				{
					rowSrc = levelSrc + (levelHeight - row - 1) * componentCount * width;
				}
				else
				{
					rowSrc = levelSrc + row * componentCount * width;
				}
				if (invertWidth)
				{
					rowSrc += (width - 1) * componentCount;
				}

				// copy per pixel to handle RBG case, based on component count
				for(PxU32 col=0; col<levelWidth; col++)
				{
					*levelDst++ = rowSrc[srcOrder[0]];
					*levelDst++ = rowSrc[srcOrder[1]];
					*levelDst++ = rowSrc[srcOrder[2]];
					*levelDst++ = 0xFF; //alpha

					if (invertWidth)
					{
						rowSrc -= componentCount;
					}
					else
					{
						rowSrc += componentCount;
					}
				}
			}
		}
		m_texture->unlockLevel(0);
	}
}

SampleRenderer::RendererTexture *SampleTextureAsset::getTexture(void)
{
	return m_texture;
}

const SampleRenderer::RendererTexture *SampleTextureAsset::getTexture(void) const
{
	return m_texture;
}

bool SampleTextureAsset::isOk(void) const
{
	return m_texture ? true : false;
}
