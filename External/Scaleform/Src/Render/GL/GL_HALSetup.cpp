/**************************************************************************

Filename    :   GL_HALSetup.cpp
Content     :   Renderer HAL Prototype header.
Created     :   
Authors     :   

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#if !defined(SF_USE_GLES)  // Do not compile under GLES 1.1

#include "GL_HAL.h"
#include "Kernel/SF_Debug.h"
#include "Kernel/SF_HeapNew.h"
#include "Render/Render_BufferGeneric.h"

namespace Scaleform { namespace Render { namespace GL {


// *** RenderHAL_GL Implementation
    
extern bool CheckExtension (const char *exts, const char *name);

bool HAL::InitHAL(const GL::HALInitParams& params)
{
    if ( !Render::HAL::initHAL(params))
        return false;

    // Clear the error stack.
    glGetError();

#ifdef SF_GL_RUNTIME_LINK
    Extensions::Init();
#endif

#if defined(SF_USE_GLES_ANY)
    const char *glexts = (const char *) glGetString(GL_EXTENSIONS);
    const char *ren = (const char*) glGetString(GL_RENDERER);

    SF_DEBUG_MESSAGE1(1, "GL_VENDOR     = %s", (const char*)glGetString(GL_VENDOR));
    SF_DEBUG_MESSAGE1(1, "GL_VERSION    = %s", (const char*)glGetString(GL_VERSION));
    SF_DEBUG_MESSAGE1(1, "GL_RENDERER   = %s", (const char*)glGetString(GL_RENDERER));
    SF_DEBUG_MESSAGE1(1, "GL_EXTENSIONS = %s", (const char*)glGetString(GL_EXTENSIONS));
    
    if (CheckExtension(glexts, "GL_OES_mapbuffer"))
        Caps |= Cap_MapBuffer;
    if (CheckExtension(glexts, "GL_OES_get_program_binary") && strncmp(ren, "PowerVR", 7))
        Caps |= Cap_BinaryShaders;
#endif
    
#if defined(SF_USE_GLES2)
    // Adreno GPU/driver, needs aligned vertices
    if (!strncmp(ren, "Adreno", 6))
        Caps |= MVF_AlignVertexStride|Cap_NoBatching;

    // Check if platform supports dynamic looping. If it doesn't, can't support blur-type filter shaders.
    if ( !ShaderData.GetDynamicLoopSupport() )
        Caps |= Cap_NoDynamicLoops;
    
#elif defined(SF_USE_GLES)
    // Never dynamic loops or batching on GLES1.1
    Caps |= Cap_NoDynamicLoops | Cap_NoBatching;
#else
    // OpenGL
    Caps = Caps_Standard;
#endif

#if !defined(SF_USE_GLES_ANY) && !defined(SF_OS_MAC)
    // Check for instancing (glDrawElementsInstanced is available).
    if ( Has_glDrawElementsInstanced() )
        Caps |= Cap_Instancing;
#endif

    GLint maxUniforms = 128;
#if !defined(GL_MAX_VERTEX_UNIFORM_VECTORS)
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxUniforms);
#else
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxUniforms);
#endif

#if defined(SF_OS_ANDROID)
    // Reports 128 uniforms but some drivers crash when more than 64 are used. These drivers
    // only seem to be present on Android.
    if (!strncmp(ren, "PowerVR SGX 5", 12))
        maxUniforms = 64;
#endif

    Caps |= maxUniforms << Cap_MaxUniforms_Shift;

#ifdef SF_GL_BINARY_SHADERS
    BinaryShaderPath = params.BinaryShaderPath;
#endif

    SManager.SetCaps(Caps);
    SF_DEBUG_MESSAGE1(1, "GL_CAPS       = 0x%x", Caps);

    pTextureManager = params.GetTextureManager();
    if (!pTextureManager)
    {
        pTextureManager = 
            *SF_HEAP_AUTO_NEW(this) TextureManager(params.RenderThreadId, pRTCommandQueue);
    }
    pTextureManager->Initialize(this);

    // Allocate our matrix state
    Matrices = *SF_HEAP_AUTO_NEW(this) MatrixState(this);


    pRenderBufferManager = params.pRenderBufferManager;
    if (!pRenderBufferManager)
    {
        pRenderBufferManager = *SF_HEAP_AUTO_NEW(this) RenderBufferManagerGeneric();
        if ( !pRenderBufferManager || !createDefaultRenderBuffer())
        {
            ShutdownHAL();
            return false;
        }
    }


    if (!initializeShaders() ||
        !Cache.Initialize(this))
        return false;

    // Map the vertex format that we use internally for DrawableImage/Filters/Clears
    MapVertexFormat(PrimFill_SolidColor, &VertexXY16iUV::Format, 
        (const VertexFormat**)&MappedXY16iUVTexture[PrimitiveBatch::DP_Single],
        (const VertexFormat**)&MappedXY16iUVTexture[PrimitiveBatch::DP_Batch], 
        (const VertexFormat**)&MappedXY16iUVTexture[PrimitiveBatch::DP_Instanced]);
    MapVertexFormat(PrimFill_Texture, &VertexXY16iAlpha::Format, 
        (const VertexFormat**)&MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single],
        (const VertexFormat**)&MappedXY16iAlphaTexture[PrimitiveBatch::DP_Batch], 
        (const VertexFormat**)&MappedXY16iAlphaTexture[PrimitiveBatch::DP_Instanced]);   
    MapVertexFormat(PrimFill_SolidColor, &VertexXY16iAlpha::Format, 
        (const VertexFormat**)&MappedXY16iAlphaSolid[PrimitiveBatch::DP_Single],
        (const VertexFormat**)&MappedXY16iAlphaSolid[PrimitiveBatch::DP_Batch], 
        (const VertexFormat**)&MappedXY16iAlphaSolid[PrimitiveBatch::DP_Instanced]);   

    HALState|= HS_ModeSet;    
    notifyHandlers(HAL_Initialize);
    return true;
}

// Returns back to original mode (cleanup)
bool HAL::ShutdownHAL()
{
    if (!(HALState & HS_ModeSet))
        return true;

    Render::HAL::shutdownHAL();

    destroyRenderBuffers();
    pRenderBufferManager.Clear();
    pTextureManager->ProcessQueues();
    pTextureManager.Clear();
    Cache.Reset();

    // Destroy the shader programs as well.
    CompiledShaderHash.Clear();
    for (unsigned i = 0; i < UniqueShaderCombinations; i++)
        StaticShaders[i].Shutdown();

    return true;
}

bool HAL::ResetContext()
{
    notifyHandlers(HAL_PrepareForReset);
    pTextureManager->NotifyLostContext();
    Cache.Reset(true);

    CompiledShaderHash.Clear();
    for (unsigned i = 0; i < UniqueShaderCombinations; i++)
        StaticShaders[i].Shutdown();

    ShaderData.ResetContext();
    pTextureManager->Initialize(this);
    pTextureManager->RestoreAfterLoss();

#ifdef SF_GL_RUNTIME_LINK
    Extensions::Init();
#endif
    
    if (!initializeShaders() ||
        !Cache.Initialize(this))
        return false;

    if (pRenderBufferManager)
        pRenderBufferManager->Reset();

    notifyHandlers(HAL_RestoreAfterReset);
    return true;
}

  
// ***** Rendering

bool HAL::BeginFrame()
{
    // Clear the error state.
    glGetError();
    return Render::HAL::BeginFrame();
}

// Set states not changed in our rendering, or that are reset after changes
bool HAL::BeginScene()
{
    if ( !Render::HAL::BeginScene())
        return false;

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

#ifndef GL_ES_VERSION_2_0
    glDisable(GL_ALPHA_TEST);
#endif
    glStencilMask(0xffffffff);

    BlendEnable = -1;

    // Reset vertex array usage (in case it changed between frames).
    EnabledVertexArrays = -1;
    GLint va;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &va);
    for (int i = 0; i < va; i++)
        glDisableVertexAttribArray(i);

    return true;
}

bool HAL::EndScene()
{
    if ( !Render::HAL::EndScene())
        return false;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

    return true;
}


void HAL::beginDisplay(BeginDisplayData* data)
{
    glDisable(GL_STENCIL_TEST);

    Render::HAL::beginDisplay(data);
}

// Updates HW Viewport and ViewportMatrix based on provided viewport
// and view rectangle.
void HAL::updateViewport()
{
    Viewport vp;

    if (HALState & HS_ViewValid)
    {
        int dx = ViewRect.x1 - VP.Left,
            dy = ViewRect.y1 - VP.Top;
        
        // Modify HW matrix and viewport to clip.
        CalcHWViewMatrix(VP.Flags, &Matrices->View2D, ViewRect, dx, dy);
        Matrices->SetUserMatrix(Matrices->User);
        Matrices->ViewRect    = ViewRect;
        Matrices->UVPOChanged = 1;

        if ( HALState & HS_InRenderTarget )
        {
            glViewport(VP.Left, VP.Top, VP.Width, VP.Height);
            glDisable(GL_SCISSOR_TEST);
        }
        else
        {
			vp = VP;
            vp.Left     = ViewRect.x1;
            vp.Top      = ViewRect.y1;
            vp.Width    = ViewRect.Width();
            vp.Height   = ViewRect.Height();
            vp.SetStereoViewport(Matrices->S3DDisplay);
            glViewport(vp.Left, VP.BufferHeight-vp.Top-vp.Height, vp.Width, vp.Height);
            if (VP.Flags & Viewport::View_UseScissorRect)
            {
                glEnable(GL_SCISSOR_TEST);
                glScissor(VP.ScissorLeft, VP.BufferHeight-VP.ScissorTop-VP.ScissorHeight, VP.ScissorWidth, VP.ScissorHeight);
            }
            else
            {
                glDisable(GL_SCISSOR_TEST);
            }
        }
    }
    else
    {
        glViewport(0,0,0,0);
    }
}

void HAL::CalcHWViewMatrix(unsigned VPFlags, Matrix* pmatrix, const Rect<int>& viewRect, int dx, int dy)
{
    float       vpWidth = (float)viewRect.Width();
    float       vpHeight= (float)viewRect.Height();

    pmatrix->SetIdentity();
    if (VPFlags & Viewport::View_IsRenderTexture)
    {
        pmatrix->Sx() = 2.0f  / vpWidth;
        pmatrix->Sy() = 2.0f /  vpHeight;
        pmatrix->Tx() = -1.0f - pmatrix->Sx() * ((float)dx); 
        pmatrix->Ty() = -1.0f - pmatrix->Sy() * ((float)dy);
    }
    else
    {
        pmatrix->Sx() = 2.0f  / vpWidth;
        pmatrix->Sy() = -2.0f / vpHeight;
        pmatrix->Tx() = -1.0f - pmatrix->Sx() * ((float)dx); 
        pmatrix->Ty() = 1.0f  - pmatrix->Sy() * ((float)dy);
    }
}

}}} // Scaleform::Render::GL

#endif // !defined(SF_USE_GLES)
