/**************************************************************************

Filename    :   D3D1x_HALSetup.cpp
Content     :   
Created     :   Mar 2011
Authors     :   Bart Muzzin

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "D3D1x_HAL.h"
#include "Kernel/SF_Debug.h"
#include "Kernel/SF_HeapNew.h"
#include "Render/Render_BufferGeneric.h"

#if SF_CC_MSVC >= 1600 // MSVC 2010
#include <crtdefs.h>
#endif // SF_CC_MSVC >= 1600 // MSVC 2010

namespace Scaleform { namespace Render { namespace D3D1x {

// *** RenderHAL_D3D1x Implementation
    
bool HAL::InitHAL(const D3D1x::HALInitParams& params)
{
    if ( !Render::HAL::initHAL(params))
        return false;

    if (!params.pD3DDevice)
        return 0;

    pDevice = params.pD3DDevice;

    D3D11(
        ID3D1x(DeviceContext)* pd3dDeviceContext = params.pD3DContext;
        if (!pd3dDeviceContext)
            pDevice->GetImmediateContext(&pd3dDeviceContext);
        );

    D3D10_11( pDeviceContext = pDevice, pDeviceContext  = pd3dDeviceContext);

    pDevice->AddRef();
    pDeviceContext->AddRef();

    if ( createBlendStates() &&
         createDepthStencilStates() &&
         createRasterStates() &&
         createConstantBuffers() && 
         SManager.Initialize(this) &&
         Cache.Initialize(pDevice, pDeviceContext))
    {
        // Create Texture manager if needed.
        if (params.pTextureManager)
            pTextureManager = params.GetTextureManager();
        else
        {
            pTextureManager = 
                *SF_HEAP_AUTO_NEW(this) TextureManager(pDevice, pDeviceContext,
                                                       params.RenderThreadId, pRTCommandQueue);
            if (!pTextureManager)
            {
                Cache.Reset();
                SManager.Reset();
            }
        }

        // Allocate our matrix state
        Matrices = *SF_HEAP_AUTO_NEW(this) MatrixState(this);


        // Create RenderBufferManager if needed.
        if (params.pRenderBufferManager)
            pRenderBufferManager = params.pRenderBufferManager;
        else
        {
            // Create the default render target, and manager.
            pRenderBufferManager = *SF_HEAP_AUTO_NEW(this) RenderBufferManagerGeneric(true);
            if ( !pRenderBufferManager || !createDefaultRenderBuffer())
            {
                ShutdownHAL();
                return false;
            }
        }

        for (unsigned i = 0; i < VertexShaderDesc::VSI_Count; i++)
        {
            if (VertexShaderDesc::Descs[i] && VertexShaderDesc::Descs[i]->pBinary)
            {
                if (!StaticVShaders[i].Init(pDevice, VertexShaderDesc::Descs[i]))
                    return false;
            }
        }
    
        for (unsigned i = 0; i < FragShaderDesc::FSI_Count; i++)
        {
            if (FragShaderDesc::Descs[i] && FragShaderDesc::Descs[i]->pBinary)
            {
                if ( !StaticFShaders[i].Init(pDevice, FragShaderDesc::Descs[i]) )
                    return false;
            }
        }

        // Success.
        if ( pTextureManager && pRenderBufferManager )
        {
            HALState|= HS_ModeSet;
            notifyHandlers(HAL_Initialize);

            // Map the vertex format that we use internally for DrawableImage/Filters/Clears
            MapVertexFormat(PrimFill_Texture, &VertexXY16iAlpha::Format, 
                (const VertexFormat**)&MappedXY16fAlphaTexture[PrimitiveBatch::DP_Single],
                (const VertexFormat**)&MappedXY16fAlphaTexture[PrimitiveBatch::DP_Batch], 
                (const VertexFormat**)&MappedXY16fAlphaTexture[PrimitiveBatch::DP_Instanced]);   
            MapVertexFormat(PrimFill_SolidColor, &VertexXY16iAlpha::Format, 
                (const VertexFormat**)&MappedXY16fAlphaSolid[PrimitiveBatch::DP_Single],
                (const VertexFormat**)&MappedXY16fAlphaSolid[PrimitiveBatch::DP_Batch], 
                (const VertexFormat**)&MappedXY16fAlphaSolid[PrimitiveBatch::DP_Instanced]);   

            return true;
        }
    }

    // Failure.
    pDevice->Release();
    pDeviceContext->Release();
    pDevice = 0;
    pDeviceContext = 0;
    return false;
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

    // Destroy device states.
    destroyBlendStates();
    destroyDepthStencilStates();
    destroyRasterStates();
    destroyConstantBuffers();

    // Release the views, if they exist.
    pRenderTargetView = 0;
    pDepthStencilView = 0;

    for (unsigned i = 0; i < PrimitiveBatch::DP_DrawableCount; ++i )
    {
        MappedXY16fAlphaTexture[i]->pSysFormat = 0;
        MappedXY16fAlphaTexture[i] = 0;
        MappedXY16fAlphaSolid[i]->pSysFormat = 0;
        MappedXY16fAlphaSolid[i] = 0;
    }

    destroyRenderBuffers();
    pRenderBufferManager.Clear();


    // Do TextureManager::Reset to ensure shutdown on the current thread.
    pTextureManager->Reset();
    pTextureManager.Clear();
    SManager.Reset();
    Cache.Reset();

    pDeviceContext->Release();
    pDeviceContext  = 0;

    pDevice->Release();
    pDevice         = 0;
    return true;
}

void HAL::PrepareForReset()
{
    SF_ASSERT(HALState & HS_ModeSet);
    if (HALState & HS_ReadyForReset)
        return;

    // Release the default rendertarget, and depth stencil references.
    if ( RenderTargetStack.GetSize() > 0 )
    {
        RenderTargetEntry& entry = RenderTargetStack[0];
        if ( entry.pRenderTarget )
        {
            SF_ASSERT( entry.pRenderTarget->GetType() == RBuffer_Default);
            D3D1x::RenderTargetData* phd = (D3D1x::RenderTargetData*)entry.pRenderTarget->GetRenderTargetData();
            if ( phd && phd->pRenderSurface )
            {
                phd->pRenderSurface->Release();
                phd->pRenderSurface = 0;
            }
            if ( phd && phd->pDSSurface )
            {
                phd->pDSSurface->Release();
                phd->pDSSurface = 0;
            }
        }
    }
    
    notifyHandlers(HAL_PrepareForReset);
    HALState |= HS_ReadyForReset;
}

bool HAL::RestoreAfterReset()
{
    if (!IsInitialized())
        return false;
    if (!(HALState & HS_ReadyForReset))
        return true;

    // Reobtain the default rendertarget and depthstencil references.
    if ( RenderTargetStack.GetSize() > 0 )
    {
        RenderTargetEntry& entry = RenderTargetStack[0];

        ID3D1x(RenderTargetView)* rtView;
        ID3D1x(DepthStencilView)* dsView;
        pDeviceContext->OMGetRenderTargets(1, &rtView, &dsView);

        RenderTargetData* phd = 0;
        if ( entry.pRenderTarget && rtView)
        {
            SF_ASSERT( entry.pRenderTarget->GetType() == RBuffer_Default);
            phd = (D3D1x::RenderTargetData*)entry.pRenderTarget->GetRenderTargetData();
            SF_ASSERT( phd->pRenderSurface == 0 );
            phd->pRenderSurface = rtView;
            if ( dsView )
                phd->pDSSurface = dsView;
        }

        // Note: No Release called on resource views, it acts as an AddRef on them,
        // which the RenderTargetData class expects.
    }

    notifyHandlers(HAL_RestoreAfterReset);
    HALState &= ~HS_ReadyForReset;
    return true;
}

// ***** Rendering

bool HAL::BeginScene()
{
    if ( !Render::HAL::BeginScene() )
        return false;

    // Render event for anything that is done immediately within BeginScene (but not the entire scene).
    ScopedRenderEvent GPUEvent(GetEvent(Event_BeginScene), __FUNCTION__ "-SetState");

    // Get the rendertarget and depth surfaces we will render the scene to.
    pDeviceContext->OMGetRenderTargets(1, &pRenderTargetView.GetRawRef(), &pDepthStencilView.GetRawRef());

    pDeviceContext->IASetPrimitiveTopology(D3D1x(PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    pDeviceContext->OMSetDepthStencilState(DepthStencilStates[StencilMode_Disabled], 0);
    CurrentConstantBuffer = 0;

    // Start the scene
    SManager.BeginScene();
    ShaderData.BeginScene();
    return true;
}

bool HAL::EndScene()
{
    ScopedRenderEvent GPUEvent(GetEvent(Event_Scene), 0, false);

    if ( !Render::HAL::EndScene())
        return false;
    SManager.EndScene();

    // Release the views.
    pRenderTargetView = 0;
    pDepthStencilView = 0;

    return true;
}

void HAL::beginDisplay(BeginDisplayData* data)
{
    GetEvent(Event_Display).Begin(__FUNCTION__);
    pDeviceContext->RSSetState(RasterStates[RasterMode]);
    Render::HAL::beginDisplay(data);
}

// Updates D3D HW Viewport and ViewportMatrix based on provided viewport
// and view rectangle.
void HAL::updateViewport()
{
    D3D1x(VIEWPORT) vp;
    Rect<int>      vpRect;

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

        if ( !(HALState & HS_InRenderTarget))
            vpRect = ViewRect;
        else
            vpRect.SetRect(VP.Left, VP.Top, VP.Left + VP.Width, VP.Top + VP.Height);
    }

    vp.TopLeftX = D3D10_11((UINT),(FLOAT))vpRect.x1;
    vp.TopLeftY = D3D10_11((UINT),(FLOAT))vpRect.y1;
    vp.Width    = D3D10_11((UINT),(FLOAT))vpRect.Width();
    vp.Height   = D3D10_11((UINT),(FLOAT))vpRect.Height();

    // DX can't handle a vp with zero area.
    vp.Width  = Alg::Max<D3D10_11(UINT,FLOAT)>(vp.Width, 1);
    vp.Height = Alg::Max<D3D10_11(UINT,FLOAT)>(vp.Height, 1);

    vp.MinDepth = 0.0f;
    vp.MaxDepth = 0.0f;
    pDeviceContext->RSSetViewports(1, &vp);
}


void HAL::CalcHWViewMatrix(Matrix* pmatrix,
                           const Rect<int>& viewRect, int dx, int dy)
{
    float       vpWidth = (float)viewRect.Width();
    float       vpHeight= (float)viewRect.Height();

    pmatrix->SetIdentity();
    pmatrix->Sx() = 2.0f  / vpWidth;
    pmatrix->Sy() = -2.0f / vpHeight;
    pmatrix->Tx() = -1.0f - pmatrix->Sx() * ((float)dx); 
    pmatrix->Ty() = 1.0f  - pmatrix->Sy() * ((float)dy);
}


}}} // Scaleform::Render::D3D1x
