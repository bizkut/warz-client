/**************************************************************************

Filename    :   D3D9_HALSetup.cpp
Content     :   Renderer HAL Prototype header.
Created     :   May 2009
Authors     :   Michael Antonov

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

// This removes the need to link with dxguid.lib.
#define DIRECT3D_VERSION 0x0900
#include <InitGuid.h>

#include "D3D9_HAL.h"
#include "Kernel/SF_Debug.h"
#include "Kernel/SF_HeapNew.h"
#include "Render/Render_BufferGeneric.h"
#include <d3dx9.h>

#if SF_CC_MSVC >= 1600 // MSVC 2010
#include <crtdefs.h>
#endif // SF_CC_MSVC >= 1600 // MSVC 2010

namespace Scaleform { namespace Render { namespace D3D9 {

// *** RenderHAL_D3D9 Implementation
bool HAL::InitHAL(const D3D9::HALInitParams& params)
{
    ScopedRenderEvent GPUEvent(GetEvent(Event_InitHAL), __FUNCTION__);

    if ( !Render::HAL::initHAL(params))
        return false;

    if (!params.pD3DDevice)
        return 0; 

    pDevice = params.pD3DDevice;
    pDevice->AddRef();

    // Detect shader level features.
    D3DCAPSx caps;
    D3DDEVICE_CREATION_PARAMETERS cparams;
    pDevice->GetDeviceCaps(&caps);
    pDevice->GetCreationParameters(&cparams);

    if (!Cache.Initialize(pDevice, !(params.ConfigFlags&HALConfig_StaticBuffers)) ||
        !SManager.Initialize(this))
    {
        ShutdownHAL();
        return false;
    }

    // Create Texture manager if needed.
    if (params.pTextureManager)
        pTextureManager = params.GetTextureManager();
    else
    {
        D3DCapFlags ourCaps;
        ourCaps.InitFromHWCaps(caps, cparams);

        // Determine D3D9Ex usage.
        IDirect3DDevice9Ex* d3d9exPtr;
        if ( SUCCEEDED(pDevice->QueryInterface(IID_IDirect3DDevice9Ex, (void**)&d3d9exPtr)) && d3d9exPtr)
        {
            d3d9exPtr->Release();
            ourCaps.Flags |= D3DCapFlags::Cap_D3D9Ex;
        }

        pTextureManager = 
            *SF_HEAP_AUTO_NEW(this) TextureManager(pDevice, ourCaps,
                                                   params.RenderThreadId, pRTCommandQueue);
        if (!pTextureManager)
        {
            ShutdownHAL();
            return false;
        }
    }

    Matrices = *SF_HEAP_AUTO_NEW(this) MatrixState(this);

    // Create RenderBufferManager if needed.
    if (params.pRenderBufferManager)
        pRenderBufferManager = params.pRenderBufferManager;
    else
    {
        // Create the default render target, and manager.
        pRenderBufferManager = *SF_HEAP_AUTO_NEW(this) RenderBufferManagerGeneric();
        if ( !pRenderBufferManager || !createDefaultRenderBuffer())
        {
            ShutdownHAL();
            return false;
        }
    }

    if ( !initializeShaders() )
    {
        ShutdownHAL();
        return false;
    }

    // Detect stencil op.
    if (caps.StencilCaps & D3DSTENCILCAPS_INCR)
    {
        StencilOpInc = D3DSTENCILOP_INCR;
    }
    else if (caps.StencilCaps & D3DSTENCILCAPS_INCRSAT)
    {
        StencilOpInc = D3DSTENCILOP_INCRSAT;
    }
    else
    {   // Stencil ops not available.
        StencilOpInc = D3DSTENCILOP_REPLACE;
    }

    // Map the vertex format that we use internally for DrawableImage/Filters/Clears
    MapVertexFormat(PrimFill_Texture, &VertexXY16iAlpha::Format, 
        (const VertexFormat**)&MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single],
        (const VertexFormat**)&MappedXY16iAlphaTexture[PrimitiveBatch::DP_Batch], 
        (const VertexFormat**)&MappedXY16iAlphaTexture[PrimitiveBatch::DP_Instanced]);   
    MapVertexFormat(PrimFill_SolidColor, &VertexXY16iAlpha::Format, 
        (const VertexFormat**)&MappedXY16iAlphaSolid[PrimitiveBatch::DP_Single],
        (const VertexFormat**)&MappedXY16iAlphaSolid[PrimitiveBatch::DP_Batch], 
        (const VertexFormat**)&MappedXY16iAlphaSolid[PrimitiveBatch::DP_Instanced]);   

    memcpy(&PresentParams, &params.PresentParams, sizeof(D3DPRESENT_PARAMETERS));
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

    for (unsigned i = 0; i < VertexShaderDesc::VSI_Count; i++)
        if (VertexShaderDesc::Descs[i] && VertexShaderDesc::Descs[i]->pBinary)
            StaticVShaders[i].Shutdown();

    for (unsigned i = 0; i < FragShaderDesc::FSI_Count; i++)
        if (FragShaderDesc::Descs[i] && FragShaderDesc::Descs[i]->pBinary)
            StaticFShaders[i].Shutdown();

    destroyRenderBuffers();
    pRenderBufferManager.Clear();

    for (unsigned i = 0; i < PrimitiveBatch::DP_DrawableCount; ++i )
    {
        MappedXY16iAlphaTexture[i]->pSysFormat = 0;
        MappedXY16iAlphaTexture[i] = 0;
        MappedXY16iAlphaSolid[i]->pSysFormat = 0;
        MappedXY16iAlphaSolid[i] = 0;
    }

    // Do TextureManager::Reset to ensure shutdown on the current thread.
    if ( pTextureManager )
        pTextureManager->Reset();
    pTextureManager.Clear();
    SManager.Reset();
    Cache.Reset();
     
    if (pDevice)
        pDevice->Release();
    pDevice = 0;

    return true;
}

void HAL::PrepareForReset()
{
    SF_ASSERT(HALState & HS_ModeSet);
    if (HALState & HS_ReadyForReset)
        return;

    notifyHandlers(HAL_PrepareForReset);

    destroyRenderBuffers();

    pRenderBufferManager->Reset();
    pTextureManager->PrepareForReset();
    Cache.Reset();    

    HALState |= HS_ReadyForReset;
}

// - RestoreAfterReset called after reset to restore needed variables.
bool HAL::RestoreAfterReset()
{
    if (!IsInitialized())
        return false;
    if (!(HALState & HS_ReadyForReset))
        return true;

    if (!Cache.Initialize(pDevice, Cache.UsesDynamicMeshes()))
        return false;

    pTextureManager->RestoreAfterReset();

    if (!createDefaultRenderBuffer())
        return false;

     notifyHandlers(HAL_RestoreAfterReset);

     HALState &= ~HS_ReadyForReset;
     return true;
}

    
// ***** Rendering

bool HAL::BeginScene()
{
    if ( !Render::HAL::BeginScene())
        return false;

    ScopedRenderEvent GPUEvent(GetEvent(Event_BeginScene), __FUNCTION__ "-SetState");

    // Blending render states.
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);    

    // Not necessary of not alpha testing:
    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);  // Important!

    union 
    {
        float fbias;
        DWORD d;
    } bias;
    bias.fbias = -0.75f;

    pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);

    pDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, bias.d );
    pDevice->SetSamplerState(0, D3DSAMP_ELEMENTINDEX, 0);
    pDevice->SetSamplerState(1, D3DSAMP_ELEMENTINDEX, 0);

    // Set texture coordinate indices to match their stages, as we will be using the programmable pipeline.
    for ( unsigned i = 0; i < 8; i++ )
        pDevice->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);

    // Textures off by default.
    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);

    // No ZWRITE by default
    pDevice->SetRenderState(D3DRS_ZWRITEENABLE, 0);
    pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
        D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

    // Turn off back-face culling.
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

    // Disable fog.
    pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE); 

    // Start the scene
    if (!(VMCFlags&HALConfig_NoSceneCalls))
        pDevice->BeginScene();

    SManager.BeginScene();
    ShaderData.BeginScene();

    return true;
}

bool HAL::EndScene()
{
    if ( !Render::HAL::EndScene())
        return false;

    SManager.EndScene();

    // 'undo' instancing setup, if it was set.
    pDevice->SetStreamSourceFreq(0, 1);
    pDevice->SetStreamSourceFreq(1, 1);
    PrevBatchType = PrimitiveBatch::DP_None;

    if (!(VMCFlags&HALConfig_NoSceneCalls))
        pDevice->EndScene();

    return true;
}

// Updates D3D HW Viewport and ViewportMatrix based on provided viewport
// and view rectangle.
void HAL::updateViewport()
{
    D3DVIEWPORTx vp;
    Rect<int>    vpRect;

    if (HALState & HS_ViewValid)
    {
        int dx = ViewRect.x1 - VP.Left,
            dy = ViewRect.y1 - VP.Top;
        
        // Modify HW matrix and viewport to clip.
        CalcHWViewMatrix(&Matrices->View2D, ViewRect, dx, dy);
        Matrices->SetUserMatrix(Matrices->User);
        Matrices->ViewRect    = ViewRect;
        Matrices->UVPOChanged = 1;

        /*
        // TBD: Prepend UserMatrix here is incorrect for nested viewport-based
        // mask clipping; what's needed is a complex combination of viewport and
        // coordinate adjustment. Until this is done, mask viewport clipping will be
        // in the wrong place for UserMatrix.
        if (UserMatrix != Matrix2F::Identity)
        {
            Rect<int> viewportRect;
            Rect<int> userViewRect(
                ViewRect.x1 + (int)UserMatrix.Tx(),
                ViewRect.y1 + (int)UserMatrix.Ty(),
                Size<int>((int)(UserMatrix.Sx() * (float)ViewRect.Width()),
                          (int)(UserMatrix.Sy() * (float)ViewRect.Height())));

            VP.GetClippedRect(&viewportRect);
            viewportRect.IntersectRect(&vpRect, userViewRect);
        }
        */

        if ( !(HALState & HS_InRenderTarget) )
		{
			Viewport vp = VP;
			vp.Left     = ViewRect.x1;
			vp.Top      = ViewRect.y1;
			vp.Width    = ViewRect.Width();
			vp.Height   = ViewRect.Height();
            vp.SetStereoViewport(Matrices->S3DDisplay);
			vpRect.SetRect(vp.Left, vp.Top, vp.Left + vp.Width, vp.Top + vp.Height);
		}
        else
            vpRect.SetRect(VP.Left, VP.Top, VP.Left + VP.Width, VP.Top + VP.Height);
    }

    vp.X        = vpRect.x1;
    vp.Y        = vpRect.y1;

    // DX9 can't handle a vp with zero area.
    vp.Width    = (DWORD)Alg::Max<int>(vpRect.Width(), 1);
    vp.Height   = (DWORD)Alg::Max<int>(vpRect.Height(), 1);

    vp.MinZ     = 0.0f;
    vp.MaxZ     = 0.0f;
    pDevice->SetViewport(&vp);
}


void HAL::CalcHWViewMatrix(Matrix* pmatrix,
                           const Rect<int>& viewRect, int dx, int dy)
{
    float       vpWidth = (float)viewRect.Width();
    float       vpHeight= (float)viewRect.Height();
    // Adjust by -0.5 pixel to match DirectX pixel coverage rules.
    float       xhalfPixelAdjust = (viewRect.Width() > 0) ? (1.0f / vpWidth) : 0.0f;
    float       yhalfPixelAdjust = (viewRect.Height()> 0) ? (1.0f / vpHeight) : 0.0f;

    pmatrix->SetIdentity();
    pmatrix->Sx() = 2.0f  / vpWidth;
    pmatrix->Sy() = -2.0f / vpHeight;
    pmatrix->Tx() = -1.0f - pmatrix->Sx() * ((float)dx) - xhalfPixelAdjust; 
    pmatrix->Ty() = 1.0f  - pmatrix->Sy() * ((float)dy) + yhalfPixelAdjust;
}


}}} // Scaleform::Render::D3D9
