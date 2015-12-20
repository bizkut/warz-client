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

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_LIBGXM)

#include "GXMRenderer.h"

#include <RendererDesc.h>

#include <RendererVertexBufferDesc.h>
#include "GXMRendererVertexBuffer.h"

#include <RendererIndexBufferDesc.h>
#include "GXMRendererIndexBuffer.h"

#include <RendererInstanceBufferDesc.h>
#include "GXMRendererInstanceBuffer.h"

#include <RendererMeshDesc.h>
#include <RendererMeshContext.h>
#include "GXMRendererMesh.h"

#include <RendererMaterialDesc.h>
#include <RendererMaterialInstance.h>
#include "GXMRendererMaterial.h"

#include <RendererLightDesc.h>
#include <RendererDirectionalLightDesc.h>
#include "GXMRendererDirectionalLight.h"
#include <RendererSpotLightDesc.h>
#include "GXMRendererSpotLight.h"

#include <RendererTexture2DDesc.h>
#include "GXMRendererTexture2D.h"
#include "GXMRendererUtils.h"

#include <RendererProjection.h>

#include <SamplePlatform.h>

// gxm includes
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libdbg.h>
#include <libdbgfont.h>
#include <kernel.h>
#include <display.h>
#include <gxm.h>
#include <ctrl.h>
#include <vectormath.h>

#ifdef ENABLE_RAZOR_CAPTURE
	#include <libsysmodule.h>
	#include <razor_capture.h>
#endif

/********************************************************************************************/
/*                                definitions                                               */
/********************************************************************************************/

/*	Define the width and height to render at the native resolution on ES2
	hardware.
*/
#define DISPLAY_WIDTH				960
#define DISPLAY_HEIGHT				544
#define DISPLAY_STRIDE_IN_PIXELS	1024

/*	Define the libgxm color format to render to.  This should be kept in sync
	with the display format to use with the SceDisplay library.
*/
#define DISPLAY_COLOR_FORMAT		SCE_GXM_COLOR_FORMAT_A8B8G8R8
#define DISPLAY_PIXEL_FORMAT		SCE_DISPLAY_PIXELFORMAT_A8B8G8R8

/* 	Define the number of back buffers to use with this sample.  Most applications
	should use a value of 2 (double buffering) or 3 (triple buffering).
*/
#define DISPLAY_BUFFER_COUNT		3

/*	Define the maximum number of queued swaps that the display queue will allow.
	This limits the number of frames that the CPU can get ahead of the GPU,
	and is independent of the actual number of back buffers.  The display
	queue will block during sceGxmDisplayQueueAddEntry if this number of swaps
	have already been queued.
*/
#define DISPLAY_MAX_PENDING_SWAPS	2

/*	Define the MSAA mode.  This can be changed to 4X on ES1 hardware to use 4X
	multi-sample anti-aliasing, and can be changed to 4X or 2X on ES2 hardware.
*/
#define MSAA_MODE					SCE_GXM_MULTISAMPLE_NONE

/* Color definition */
                /*    AABBGGRR  */
#define WHITE       0xffc0c0c0
#define HILIGHT     0xffffffff
#define RED         0xff000080
#define GREEN       0xff008000
#define HIGREEN     0xff00ff00
#define ORANGE      0xff00A5ff
#define YELLOW      0xff00ffff
#define BLUE        0xff800000
#define PURPLE      0xfff020A0

/* Invalid touch ID	*/
#define INVALID_ID	0xff

// Align value
#define GXM_ALIGN(x, a)					(((x) + ((a) - 1)) & ~((a) - 1))

// Mark variable as used
#define	UNUSED(a)					(void)(a)

// for PsString.h
namespace physx
{
	namespace string
	{}
}
#include <PsString.h>


using namespace SampleRenderer;

/* Callback function for displaying a buffer */
static void displayCallback(const void *callbackData);


void displayCallback(const void *callbackData)
{
	int32_t returnCode = SCE_OK;
	// cast the parameters back
	const DisplayData *displayData = (const DisplayData *)callbackData;
	SceDisplayFrameBuf framebuf;		

	// swap to the new buffer on the next VSYNC
	memset(&framebuf, 0x00, sizeof(SceDisplayFrameBuf));
	framebuf.size        = sizeof(SceDisplayFrameBuf);
	framebuf.base        = displayData->m_address;
	framebuf.pitch       = GRAPHICS_UTIL_DEFAULT_DISPLAY_STRIDE_IN_PIXELS;
	framebuf.pixelformat = GRAPHICS_UTIL_DEFAULT_DISPLAY_PIXEL_FORMAT;
	framebuf.width       = GRAPHICS_UTIL_DEFAULT_DISPLAY_WIDTH;
	framebuf.height      = GRAPHICS_UTIL_DEFAULT_DISPLAY_HEIGHT;
	returnCode = sceDisplaySetFrameBuf( &framebuf, SCE_DISPLAY_UPDATETIMING_NEXTVSYNC );
	RENDERER_ASSERT((returnCode == SCE_OK), "Error during swap to the new buffer on the next VSYNC \n");

	// Block this callback until the swap has occurred and the old buffer
	// is no longer displayed 
	returnCode = sceDisplayWaitVblankStart();
	RENDERER_ASSERT((returnCode == SCE_OK), "Error during sceDisplayWaitVblankStart \n");

}

void *GXMRenderer::graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignment, uint32_t attribs, SceUID *uid)
{
	int err = SCE_OK;
	UNUSED(err);

	/*	Since we are using sceKernelAllocMemBlock directly, we cannot directly
		use the alignment parameter.  Instead, we must allocate the size to the
		minimum for this memblock type, and just assert that this will cover
		our desired alignment.

		Developers using their own heaps should be able to use the alignment
		parameter directly for more minimal padding.
	*/
	if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA) {
		// CDRAM memblocks must be 256KiB aligned
		RENDERER_ASSERT(alignment <= 256*1024,"Alignment error");
		size = GXM_ALIGN(size, 256*1024);
	} else {
		// LPDDR memblocks must be 4KiB aligned
		RENDERER_ASSERT(alignment <= 4*1024,"Alignment error");
		size = GXM_ALIGN(size, 4*1024);
	}
	UNUSED(alignment);

	// allocate some memory
	*uid = sceKernelAllocMemBlock("basic", type, size, NULL);
	RENDERER_ASSERT(*uid >= SCE_OK,"GPU alloc fail");

	// grab the base address
	void *mem = NULL;
	err = sceKernelGetMemBlockBase(*uid, &mem);
	RENDERER_ASSERT(err == SCE_OK,"Mem adress fail");

	// map for the GPU
	err = sceGxmMapMemory(mem, size, attribs);
	RENDERER_ASSERT(err == SCE_OK,"Map memory fail");

	// done
	return mem;
}

void GXMRenderer::graphicsFree(SceUID uid)
{
	int err = SCE_OK;
	UNUSED(err);

	// grab the base address
	void *mem = NULL;
	err = sceKernelGetMemBlockBase(uid, &mem);
	RENDERER_ASSERT(err == SCE_OK,"Get block fail");

	// unmap memory
	err = sceGxmUnmapMemory(mem);
	RENDERER_ASSERT(err == SCE_OK,"Unmap fail");

	// free the memory block
	err = sceKernelFreeMemBlock(uid);
	RENDERER_ASSERT(err == SCE_OK,"Free fail");
}


GXMRenderer::ShaderEnvironment::ShaderEnvironment(void)
{
	memset(this, 0, sizeof(*this));
}

GXMRenderer::GXMRenderer(const RendererDesc &desc, const char* assetDir) :
Renderer(DRIVER_LIBGXM, assetDir), m_platform(SampleFramework::SamplePlatform::platform()), m_shaderPatcher(NULL)
{
	m_useShadersForTextRendering	= false;
	m_displayWidth  = DISPLAY_WIDTH;
	m_displayHeight = DISPLAY_HEIGHT;
	m_currMaterial  = 0;
	m_viewMatrix = PxMat44::createIdentity();

	m_displayData.m_renderer = this;

	m_TextRendererVertexBuffer = NULL;
	m_TextRendererIndexBuffer = NULL;

	// initialize the render
	initGXM();

	m_shaderPatcher = m_graphicsData.pShaderPatcher;

	checkResize();
}

GXMRenderer::~GXMRenderer(void)
{
	// wait until rendering is done
	sceGxmFinish(m_graphicsData.pContext);

	sceGxmDisplayQueueFinish();

	GXMRenderer::graphicsFree(m_TextRendererIndexBufferUid);
	GXMRenderer::graphicsFree(m_TextRendererVertexBufferUid);

	graphicsUtilDestroyRenderTarget(&m_graphicsData, m_renderTarget);

	graphicsUtilShutdown(&m_graphicsData);

#ifdef ENABLE_RAZOR_CAPTURE
	// Terminate Razor capture.
	// This should be done after the call to sceGxmTerminate().
	sceSysmoduleUnloadModule( SCE_SYSMODULE_RAZOR_CAPTURE );
#endif

	sceDbgFontExit();

	m_platform->freeDisplay();
}

void GXMRenderer::finishRendering()
{
	SceGxmErrorCode sceCode = sceGxmFinish(m_graphicsData.pContext);

	PX_ASSERT(sceCode == 0);

	//sceGxmDisplayQueueFinish();
}

void GXMRenderer::initGXM()
{
	SceDbgFontConfig config;
	memset((void*)&config, 0, sizeof(config));

	config.fontSize = SCE_DBGFONT_FONTSIZE_DEFAULT;

	sceDbgFontInit(&config);

#ifdef ENABLE_RAZOR_CAPTURE
	// Initialize the Razor capture system.
	// This should be done before the call to sceGxmInitialize().
	SceInt32 err = sceSysmoduleLoadModule( SCE_SYSMODULE_RAZOR_CAPTURE );
	if(err != SCE_OK)
	{
		printf("Error loading SCE_SYSMODULE_RAZOR_CAPTURE \n");
		return;
	}
	sceRazorCaptureSetTrigger(100, "host0:captureFromSetTrigger.sgx");
#endif

	// Initialize graphics Utility
	SceInt32 returnCode = graphicsUtilInit(&m_graphicsData, &displayCallback, sizeof(DisplayData), NULL);
	if(returnCode != SCE_OK)
	{
		printf("Error during graphics utility initialization\n");
		return;
	}

	m_renderTarget = graphicsUtilCreateRenderTarget(&m_graphicsData, GRAPHICS_UTIL_DEFAULT_DISPLAY_WIDTH, GRAPHICS_UTIL_DEFAULT_DISPLAY_HEIGHT, SCE_GXM_MULTISAMPLE_NONE);
	RENDERER_ASSERT((m_renderTarget), "Error during render target creation \n");

	sceGxmSetCullMode(m_graphicsData.pContext, SCE_GXM_CULL_CCW);
}

bool GXMRenderer::begin(void)
{
	bool ok = false;
	ok = m_platform->makeContextCurrent();
	return ok;
}

void GXMRenderer::end(void)
{

}

bool GXMRenderer::captureScreen(const char* filename)
{
	return false;
}

void GXMRenderer::checkResize(void)
{

}

// clears the offscreen buffers.
void GXMRenderer::clearBuffers(void)
{
	if(begin())
	{
		// start rendering to the main render target
		sceGxmBeginScene(	m_graphicsData.pContext, 
			0,
			m_renderTarget,
			NULL,
			NULL,
			m_graphicsData.pDisplayBufferSync[m_graphicsData.displayBackBufferIndex],
			&m_graphicsData.displaySurface[m_graphicsData.displayBackBufferIndex],  
			&m_graphicsData.displayDepthSurface);
	}
	end();
}

// presents the current color buffer to the screen.
// returns true on device reset and if buffers need to be rewritten.
bool GXMRenderer::swapBuffers(void)
{
	if(begin())
	{
		//SceDisplayFrameBuf framebuf;
		//memset(&framebuf, 0x00, sizeof(SceDisplayFrameBuf));
		//framebuf.size        = sizeof(SceDisplayFrameBuf);
		//framebuf.base        = (SceUChar8 *) m_graphicsData.pDisplayBufferData[m_graphicsData.displayFrontBufferIndex];
		//framebuf.pitch       = GRAPHICS_UTIL_DEFAULT_DISPLAY_STRIDE_IN_PIXELS;
		//framebuf.pixelformat = GRAPHICS_UTIL_DEFAULT_DISPLAY_PIXEL_FORMAT;
		//framebuf.width       = GRAPHICS_UTIL_DEFAULT_DISPLAY_WIDTH;
		//framebuf.height      = GRAPHICS_UTIL_DEFAULT_DISPLAY_HEIGHT;
		//SceInt32 returnCode = sceDisplaySetFrameBuf( &framebuf, SCE_DISPLAY_UPDATETIMING_NEXTVSYNC );
		//RENDERER_ASSERT((returnCode == SCE_OK), "Error during swap to the new buffer on the next VSYNC \n");

		sceGxmEndScene( m_graphicsData.pContext, NULL, NULL );

		m_displayData.m_address = m_graphicsData.pDisplayBufferData[m_graphicsData.displayBackBufferIndex];		

		// Update the display ring buffer
		graphicsUtilUpdateDisplayQueue(&m_graphicsData, &m_displayData);
	}
	end();
	return false;
}

void GXMRenderer::getWindowSize(PxU32 &width, PxU32 &height) const
{
	RENDERER_ASSERT(m_displayHeight * m_displayWidth > 0, "variables not initialized properly");
	width = m_displayWidth;
	height = m_displayHeight;
}

void GXMRenderer::renderDebugText( const DisplayData* displayData )
{
	return;
	// Draws the debug text
	SceChar8 string4[32];
	snprintf( (char*)string4, sizeof(string4), "Tutorial Sample Utility" );
	sceDbgFontPrint( 15, 5, 0x8000c0c0, string4 );

	SceDbgFontFrameBufInfo info;
	memset( &info, 0, sizeof(SceDbgFontFrameBufInfo) );
	info.frameBufAddr = (SceUChar8 *)displayData->m_address;
	info.frameBufPitch = GRAPHICS_UTIL_DEFAULT_DISPLAY_STRIDE_IN_PIXELS;
	info.frameBufWidth = GRAPHICS_UTIL_DEFAULT_DISPLAY_WIDTH;
	info.frameBufHeight = GRAPHICS_UTIL_DEFAULT_DISPLAY_HEIGHT;
	info.frameBufPixelformat = GRAPHICS_UTIL_DEFAULT_DISPLAY_DBGFONT_FORMAT;

	// flush debug text
	sceDbgFontFlush( &info );

}


RendererVertexBuffer *GXMRenderer::createVertexBuffer(const RendererVertexBufferDesc &desc)
{
	GXMRendererVertexBuffer *vb = 0;
	RENDERER_ASSERT(desc.isValid(), "Invalid Vertex Buffer Descriptor.");
	if(desc.isValid())
	{
		vb = new GXMRendererVertexBuffer(desc, *this);
	}
	return vb;}

RendererIndexBuffer *GXMRenderer::createIndexBuffer(const RendererIndexBufferDesc &desc)
{	
	GXMRendererIndexBuffer *ib = 0;
	RENDERER_ASSERT(desc.isValid(), "Invalid Index Buffer Descriptor.");
	if(desc.isValid())
	{
		ib = new GXMRendererIndexBuffer(desc);
	}
	return ib;
}

void* GXMRenderer::getDevice()
{
	return NULL;
}

RendererInstanceBuffer *GXMRenderer::createInstanceBuffer(const RendererInstanceBufferDesc &desc)
{
	GXMRendererInstanceBuffer *ib = 0;
	RENDERER_ASSERT(desc.isValid(), "Invalid Instance Buffer Descriptor.");
	if(desc.isValid())
	{
		ib = new GXMRendererInstanceBuffer(desc, *this);
	}
	return ib;
}

RendererTexture2D *GXMRenderer::createTexture2D(const RendererTexture2DDesc &desc)
{
	RendererTexture2D *texture = 0;
	RENDERER_ASSERT(desc.isValid(), "Invalid Texture 2D Descriptor.");
	if(desc.isValid())
	{
		texture = new GXMRendererTexture2D(desc, *this);
	}
	return texture;
}

RendererTarget *GXMRenderer::createTarget(const RendererTargetDesc &desc)
{
	RENDERER_ASSERT(0, "Not Implemented.");
	return 0;
}

RendererMaterial *GXMRenderer::createMaterial(const RendererMaterialDesc &desc)
{
	GXMRendererMaterial *mat = 0;
	RENDERER_ASSERT(desc.isValid(), "Invalid Material Descriptor.");
	if(desc.isValid())
	{
		mat = new GXMRendererMaterial(*this, desc);
	}	
	return mat;
}

RendererMesh *GXMRenderer::createMesh(const RendererMeshDesc &desc)
{
	GXMRendererMesh *mesh = 0;
	RENDERER_ASSERT(desc.isValid(), "Invalid Mesh Descriptor.");
	if(desc.isValid())
	{
		mesh = new GXMRendererMesh(*this, desc);
	}
	return mesh;
}

RendererLight *GXMRenderer::createLight(const RendererLightDesc &desc)
{
	RendererLight *light = 0;
	RENDERER_ASSERT(desc.isValid(), "Invalid Light Descriptor.");
	if(desc.isValid())
	{
		switch(desc.type)
		{
		case RendererLight::TYPE_DIRECTIONAL:
			light = new GXMRendererDirectionalLight(*static_cast<const RendererDirectionalLightDesc*>(&desc), *this);
			break;
		case RendererLight::TYPE_SPOT:
			{
				light = new GXMRendererSpotLight(*static_cast<const RendererSpotLightDesc*>(&desc));
			}
			break;
		}
	}
	return light;
}

void GXMRenderer::bindViewProj(const physx::PxMat44 &eye, const RendererProjection &proj)
{
	m_viewMatrix = eye.inverseRT();
	convertToSce(m_environment.mViewMatrix, m_viewMatrix);
	convertToSce(m_environment.mProjMatrix, proj);

	const PxVec3 &eyePosition  =  eye.getPosition();
	const PxVec3  eyeDirection = -eye.getBasis(2);
	memcpy(m_environment.mEyePosition,  &eyePosition.x,  sizeof(float)*3);
	memcpy(m_environment.mEyeDirection, &eyeDirection.x, sizeof(float)*3);
}

void GXMRenderer::bindFogState(const RendererColor &fogColor, float fogDistance)
{
}

void GXMRenderer::bindAmbientState(const RendererColor &ambientColor)
{
	convertToSce(m_environment.mAmbientColor, ambientColor);
}

void GXMRenderer::bindDeferredState(void)
{
	RENDERER_ASSERT(0, "Not implemented!");
}

void GXMRenderer::bindMeshContext(const RendererMeshContext &context)
{
	physx::PxMat44 model;
	physx::PxMat44 modelView;
	if(context.transform) model = *context.transform;
	else                  model = PxMat44::createIdentity();

	modelView = m_viewMatrix * model;

	convertToSce(m_environment.mModelMatrix,     model);
	convertToSce(m_environment.mModelViewMatrix, modelView);

	// it appears that D3D winding is backwards, so reverse them...
	SceGxmCullMode cullMode = SCE_GXM_CULL_CCW;
	switch(context.cullMode)
	{
	case RendererMeshContext::CLOCKWISE: 
		cullMode = SCE_GXM_CULL_CCW;
		break;
	case RendererMeshContext::COUNTER_CLOCKWISE: 
		cullMode = SCE_GXM_CULL_CW;
		break;
	case RendererMeshContext::NONE: 
		cullMode = SCE_GXM_CULL_NONE;
		break;
	default:
		RENDERER_ASSERT(0, "Invalid Cull Mode");
	}

	sceGxmSetCullMode(m_graphicsData.pContext, cullMode);

	switch(context.fillMode)
	{
	case RendererMeshContext::SOLID:
		m_environment.mFillMode = SCE_GXM_PRIMITIVE_TRIANGLES;
		break;
	case RendererMeshContext::LINE:
		m_environment.mFillMode = SCE_GXM_PRIMITIVE_LINES;
		break;
	case RendererMeshContext::POINT:
		m_environment.mFillMode = SCE_GXM_PRIMITIVE_POINTS;
		break; 
	}

	m_environment.mIndexBufffer = (const GXMRendererIndexBuffer*) context.mesh->getIndexBuffer();
	m_environment.mNumIndices = context.mesh->getNumIndices();
	if(context.mesh->getNumVertexBuffers())
	{
		m_environment.mVertexBuffer = (const GXMRendererVertexBuffer*) context.mesh->getVertexBuffers()[0];
		m_environment.mNumVertices = context.mesh->getNumVertices();
	}
	else
	{
		m_environment.mVertexBuffer = NULL;
	}

	if(context.mesh->getInstanceBuffer())
	{
		m_environment.mInstanceBuffer = (const GXMRendererInstanceBuffer*) context.mesh->getInstanceBuffer();
		m_environment.mNumInstances = context.mesh->getNumInstances(); 
	}

	m_environment.mValid = true;
}

void GXMRenderer::beginMultiPass(void)
{
}

void GXMRenderer::endMultiPass(void)
{
}

void GXMRenderer::renderDeferredLight(const RendererLight &/*light*/)
{
	RENDERER_ASSERT(0, "Not implemented!");
}

PxU32 GXMRenderer::convertColor(const RendererColor& color) const
{
	return color.a << 24 | color.b << 16 | color.g << 8 | color.r;
}


bool GXMRenderer::isOk(void) const
{
	bool ok = true;
	ok = m_platform->isContextValid();
	return ok;
}

///////////////////////////////////////////////////////////////////////////////

void GXMRenderer::setupTextRenderStates()
{
	sceGxmSetCullMode(m_graphicsData.pContext, SCE_GXM_CULL_NONE);	

	sceGxmSetFrontDepthWriteEnable(m_graphicsData.pContext, SCE_GXM_DEPTH_WRITE_DISABLED);
	sceGxmSetBackDepthWriteEnable(m_graphicsData.pContext, SCE_GXM_DEPTH_WRITE_DISABLED);
}

void GXMRenderer::resetTextRenderStates()
{
	sceGxmSetFrontDepthWriteEnable(m_graphicsData.pContext, SCE_GXM_DEPTH_WRITE_ENABLED);
	sceGxmSetBackDepthWriteEnable(m_graphicsData.pContext, SCE_GXM_DEPTH_WRITE_ENABLED);

	sceGxmSetCullMode(m_graphicsData.pContext, SCE_GXM_CULL_CCW);
}

bool GXMRenderer::initTexter()
{
	Renderer::initTexter();

	PxU32 width, height;
	getWindowSize(width, height);
	const PxU32 MIN_CHARACTER_WIDTH = 8;
	const PxU32 TEXT_MAX_VERTICES = 160 * (width / MIN_CHARACTER_WIDTH);
	const PxU32 TEXT_MAX_INDICES = TEXT_MAX_VERTICES * 5;

	m_TextMaxVerts = TEXT_MAX_VERTICES;
	m_TextMaxIndices = TEXT_MAX_INDICES;

	//// initialize vertex, index buffer		
	m_TextRendererVertexBuffer = GXMRenderer::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		(16 + 16 + 16) * TEXT_MAX_VERTICES,
		8,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&m_TextRendererVertexBufferUid);

	PX_ASSERT(m_TextRendererVertexBuffer);
	
	m_TextRendererIndexBuffer = GXMRenderer::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		sizeof(PxU16) * TEXT_MAX_INDICES,
		8,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&m_TextRendererIndexBufferUid);

	PX_ASSERT(m_TextRendererIndexBuffer);

	m_CurrentTextRendererVertexBufferSize = 0;
	m_CurrentTextRendererIndexBufferSize = 0;

	return true;
}

void GXMRenderer::renderTextBuffer(const void* verts, PxU32 nbVerts, const PxU16* indices, PxU32 nbIndices, RendererMaterial* material)
{		
	GXMRendererMaterial* gxmMat = (GXMRendererMaterial*) material;
	if(!gxmMat->getFinalVertexProgram())
	{
		// create the vertex prg

		SceGxmVertexAttribute basicVertexAttributes[4];
		SceGxmVertexStream basicVertexStreams[1];
		basicVertexAttributes[0].streamIndex = 0;
		basicVertexAttributes[0].offset = 0;
		basicVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		basicVertexAttributes[0].componentCount = 4;
		
		basicVertexAttributes[0].regIndex = gxmMat->getResourceIndex("localSpacePosition");

		basicVertexAttributes[1].streamIndex = 0;
		basicVertexAttributes[1].offset = 16;
		basicVertexAttributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		basicVertexAttributes[1].componentCount = 4;

		basicVertexAttributes[1].regIndex = gxmMat->getResourceIndex("vertexColor");

		basicVertexAttributes[2].streamIndex = 0;
		basicVertexAttributes[2].offset = 32;
		basicVertexAttributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		basicVertexAttributes[2].componentCount = 2;

		basicVertexAttributes[2].regIndex = gxmMat->getResourceIndex("Texcoord0");

		basicVertexStreams[0].stride = (16 + 16 +16);
		basicVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

		// create cube vertex program
		SceGxmErrorCode err = sceGxmShaderPatcherCreateVertexProgram(
			m_graphicsData.pShaderPatcher,
			gxmMat->mGmxVertexProgramId,
			basicVertexAttributes,
			3,
			basicVertexStreams,
			1,
			&gxmMat->mFinalVertexProgram);
		RENDERER_ASSERT(err == SCE_OK, "Failed to create vertex program");
	}

	// cycle the buffer
	void* currentTextRendererVertexBuffer = NULL;
	void* currentTextRendererIndexBuffer = NULL;

	if((m_CurrentTextRendererVertexBufferSize + 48*nbVerts) > m_TextMaxVerts*48)
	{
		m_CurrentTextRendererVertexBufferSize = 0;
	}	

	currentTextRendererVertexBuffer = static_cast<PxU8*>(m_TextRendererVertexBuffer) + m_CurrentTextRendererVertexBufferSize;
	m_CurrentTextRendererVertexBufferSize += 48*nbVerts;

	if((m_CurrentTextRendererIndexBufferSize + sizeof(PxU16)*nbIndices) > m_TextMaxIndices*sizeof(PxU16))
	{
		m_CurrentTextRendererIndexBufferSize = 0;
	}	

	currentTextRendererIndexBuffer = static_cast<PxU8*>(m_TextRendererIndexBuffer) + m_CurrentTextRendererIndexBufferSize;
	m_CurrentTextRendererIndexBufferSize += sizeof(PxU16)*nbIndices;


	PxU8* locked_positions = static_cast<PxU8*>(currentTextRendererVertexBuffer);
	PxU8* locked_texcoords = static_cast<PxU8*>(currentTextRendererVertexBuffer) + 16 + 16;
	PxU8* locked_colors = static_cast<PxU8*>(currentTextRendererVertexBuffer) + 16;

	PxU16 stride = 16 + 16 + 16;

	PxU32 windowWidth, windowHeight;
	getWindowSize(windowWidth, windowHeight);
	PxReal windowWidthHalf = windowWidth / 2.0f, 
		windowHeightHalf = windowHeight / 2.0f;

	TextVertex* tv = (TextVertex*)verts;
	for(PxU32 i = 0; i < nbVerts; ++i, 
		locked_positions += stride, 
		locked_colors += stride,
		locked_texcoords += stride,
		tv += 1) 
	{
		PxVec4 pos = PxVec4(tv->p.x / windowWidthHalf - 1.0f, 1.0f - tv->p.y / windowHeightHalf, 0.0f, 1.0f);
		memcpy(locked_positions, &(pos), sizeof(PxVec4));
		PxVec4 color;
		color.x = (tv->color & 0xFF)/255.0f;
		color.y = ((tv->color & 0xFF00) >> 8)/255.0f;
		color.z = ((tv->color & 0xFF0000) >> 16)/255.0f;
		color.w = ((tv->color & 0xFF000000) >> 24)/255.0f;
		memcpy(locked_colors, &(color), sizeof(PxVec4));
		memcpy(locked_texcoords, &(tv->u), sizeof(PxReal));
		memcpy(locked_texcoords + sizeof(PxReal), &(tv->v), sizeof(PxReal));
	}

	memcpy(currentTextRendererIndexBuffer, indices, sizeof(PxU16) * nbIndices);

	/*sceGxmSetFrontDepthFunc( m_graphicsData.pContext, SCE_GXM_DEPTH_FUNC_ALWAYS );
	sceGxmSetBackDepthFunc( m_graphicsData.pContext, SCE_GXM_DEPTH_FUNC_ALWAYS );*/	

	sceGxmSetVertexProgram(m_graphicsData.pContext, gxmMat->getFinalVertexProgram());
	sceGxmSetFragmentProgram(m_graphicsData.pContext, gxmMat->getFinalFramentProgram(RendererMaterial::PASS_UNLIT));
	SceGxmErrorCode errCode = sceGxmSetVertexStream(m_graphicsData.pContext, 0, currentTextRendererVertexBuffer);

	PX_ASSERT(errCode == SCE_OK);

	errCode = sceGxmDraw(m_graphicsData.pContext, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, currentTextRendererIndexBuffer, nbIndices);

	PX_ASSERT(errCode == SCE_OK);
}

void GXMRenderer::renderLines2D(const void* vertices, PxU32 nbPassedVerts, RendererMaterial* material)
{	
	PX_ASSERT(nbPassedVerts > 1);
	//Create vertex program
	GXMRendererMaterial* gxmMat = (GXMRendererMaterial*) material;
	if(!gxmMat->getFinalVertexProgram())
	{
		SceGxmVertexAttribute basicVertexAttributes[4];
		SceGxmVertexStream basicVertexStreams[1];
		basicVertexAttributes[0].streamIndex = 0;
		basicVertexAttributes[0].offset = 0;
		basicVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		basicVertexAttributes[0].componentCount = 4;
		
		basicVertexAttributes[0].regIndex = gxmMat->getResourceIndex("localSpacePosition");

		basicVertexAttributes[1].streamIndex = 0;
		basicVertexAttributes[1].offset = 16;
		basicVertexAttributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		basicVertexAttributes[1].componentCount = 4;

		basicVertexAttributes[1].regIndex = gxmMat->getResourceIndex("vertexColor");

		basicVertexAttributes[2].streamIndex = 0;
		basicVertexAttributes[2].offset = 32;
		basicVertexAttributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		basicVertexAttributes[2].componentCount = 2;

		basicVertexAttributes[2].regIndex = gxmMat->getResourceIndex("Texcoord0");

		basicVertexStreams[0].stride = (16 + 16 +16);
		basicVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

		// create cube vertex program
		SceGxmErrorCode err = sceGxmShaderPatcherCreateVertexProgram(
			m_graphicsData.pShaderPatcher,
			gxmMat->mGmxVertexProgramId,
			basicVertexAttributes,
			3,
			basicVertexStreams,
			1,
			&gxmMat->mFinalVertexProgram);
		RENDERER_ASSERT(err == SCE_OK, "Failed to create vertex program");
	}

	//Fill vertex buffer
	PxU32 nbVerts = nbPassedVerts;  

	void* currentTextRendererVertexBuffer = NULL;

	if((m_CurrentTextRendererVertexBufferSize + 48*nbVerts) > m_TextMaxVerts*48)
	{
		m_CurrentTextRendererVertexBufferSize = 0;
	}	

	currentTextRendererVertexBuffer = static_cast<PxU8*>(m_TextRendererVertexBuffer) + m_CurrentTextRendererVertexBufferSize;
	m_CurrentTextRendererVertexBufferSize += 48*nbVerts;

	PxU8* locked_positions = static_cast<PxU8*>(currentTextRendererVertexBuffer);
	PxU8* locked_texcoords = static_cast<PxU8*>(currentTextRendererVertexBuffer) + 16 + 16;
	PxU8* locked_colors = static_cast<PxU8*>(currentTextRendererVertexBuffer) + 16;

	PxU16 stride = 16 + 16 + 16;

	PxU32 windowWidth, windowHeight;
	getWindowSize(windowWidth, windowHeight);
	PxReal windowWidthHalf = windowWidth / 2.0f, 
	windowHeightHalf = windowHeight / 2.0f;

	TextVertex* tv = (TextVertex*)vertices;

	//Copy the passed vertices starting at position [1]
	for(PxU32 i = 0; i < nbVerts; ++i, 
		locked_positions += stride, 
		locked_colors += stride,
		locked_texcoords += stride,
		tv += 1) 
	{
		PxVec4 pos = PxVec4(tv->p.x / windowWidthHalf - 1.0f, 1.0f - tv->p.y / windowHeightHalf, 0.0f, 1.0f);
		memcpy(locked_positions, &(pos), sizeof(PxVec4));
		PxVec4 color;
		color.x = (tv->color & 0xFF)/255.0f;
		color.y = ((tv->color & 0xFF00) >> 8)/255.0f;
		color.z = ((tv->color & 0xFF0000) >> 16)/255.0f;
		color.w = ((tv->color & 0xFF000000) >> 24)/255.0f;
		memcpy(locked_colors, &(color), sizeof(PxVec4));
		memcpy(locked_texcoords, &(tv->u), sizeof(PxReal));
		memcpy(locked_texcoords + sizeof(PxReal), &(tv->v), sizeof(PxReal));
	}

	//Fill index buffer (Triangle List)
	int nbIndices = nbVerts*2; 

	void* currentTextRendererIndexBuffer = NULL;

	if((m_CurrentTextRendererIndexBufferSize + sizeof(PxU16)*nbIndices) > m_TextMaxIndices*sizeof(PxU16))
	{
		m_CurrentTextRendererIndexBufferSize = 0;
	}	

	currentTextRendererIndexBuffer = static_cast<PxU8*>(m_TextRendererIndexBuffer) + m_CurrentTextRendererIndexBufferSize;
	m_CurrentTextRendererIndexBufferSize += sizeof(PxU16)*nbIndices;

	PxU16* currentLines2DRendererIndexBuffer = static_cast<PxU16*>(currentTextRendererIndexBuffer);
	
	nbIndices = 0;
	for(PxU32 i = 1; i < nbVerts; ++i)
	{		
		currentLines2DRendererIndexBuffer[nbIndices]	= i - 1;
		nbIndices++;
		currentLines2DRendererIndexBuffer[nbIndices]	= i;
		nbIndices++;
	}
	
	//Setting  
	sceGxmSetVertexProgram(m_graphicsData.pContext, gxmMat->getFinalVertexProgram());
	sceGxmSetFragmentProgram(m_graphicsData.pContext, gxmMat->getFinalFramentProgram(RendererMaterial::PASS_UNLIT));

	SceGxmErrorCode errCode = sceGxmSetVertexStream(m_graphicsData.pContext, 0, currentTextRendererVertexBuffer);
	PX_ASSERT(errCode == SCE_OK);

	//Draw 
	sceGxmSetFrontDepthWriteEnable(m_graphicsData.pContext,SCE_GXM_DEPTH_WRITE_DISABLED);
	sceGxmSetFrontPolygonMode(m_graphicsData.pContext,SCE_GXM_POLYGON_MODE_LINE);
	sceGxmSetBackPolygonMode( m_graphicsData.pContext, SCE_GXM_POLYGON_MODE_LINE );

	sceGxmSetCullMode(m_graphicsData.pContext, SCE_GXM_CULL_NONE); 

	errCode = sceGxmDraw(m_graphicsData.pContext, SCE_GXM_PRIMITIVE_LINES, SCE_GXM_INDEX_FORMAT_U16, currentLines2DRendererIndexBuffer, nbIndices);
	PX_ASSERT(errCode == SCE_OK);

	sceGxmSetFrontDepthWriteEnable(m_graphicsData.pContext, SCE_GXM_DEPTH_WRITE_ENABLED);
	sceGxmSetFrontPolygonMode(m_graphicsData.pContext, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL); 
	sceGxmSetBackPolygonMode( m_graphicsData.pContext, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL );
}

void GXMRenderer::setupScreenquadRenderStates()
{
	sceGxmSetCullMode(m_graphicsData.pContext, SCE_GXM_CULL_NONE); 

	sceGxmSetFrontDepthWriteEnable(m_graphicsData.pContext, SCE_GXM_DEPTH_WRITE_DISABLED);
	sceGxmSetBackDepthWriteEnable(m_graphicsData.pContext, SCE_GXM_DEPTH_WRITE_DISABLED);	
}

void GXMRenderer::resetScreenquadRenderStates()
{
	sceGxmSetFrontDepthWriteEnable(m_graphicsData.pContext, SCE_GXM_DEPTH_WRITE_ENABLED);
	sceGxmSetBackDepthWriteEnable(m_graphicsData.pContext, SCE_GXM_DEPTH_WRITE_ENABLED);

}

#endif // #if defined(RENDERER_ENABLE_OPENGL)
