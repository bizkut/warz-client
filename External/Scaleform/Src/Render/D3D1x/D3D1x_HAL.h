/**************************************************************************

Filename    :   D3D1x_HAL.h
Content     :   
Created     :   Mar 2011
Authors     :   Bart Muzzin

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef INC_SF_D3D1X_HAL_H
#define INC_SF_D3D1X_HAL_H
#pragma once

#include "Render/D3D1x/D3D1x_Config.h"
#include "Render/Render_HAL.h"
#include "Render/D3D1x/D3D1x_MeshCache.h"
#include "Render/D3D1x/D3D1x_Shader.h"
#include "Render/D3D1x/D3D1x_Texture.h"

namespace Scaleform { namespace Render { namespace D3D1x {

#define SF_RENDERER_VSHADER_PROFILE "vs_4_0"

//------------------------------------------------------------------------
enum HALConfigFlags
{
};

// D3D1x::HALInitParems provides D3D10/D3D11 specific rendering initialization
// parameters for HAL::InitHAL.

struct HALInitParams : public Render::HALInitParams
{
    ID3D1x(Device)*               pD3DDevice;
    D3D11(ID3D1x(DeviceContext)* pD3DContext);

    HALInitParams(ID3D1x(Device)* device,
                  D3D11(ID3D1x(DeviceContext)* pD3DContext,)
                  UInt32 halConfigFlags = 0,
                  ThreadId renderThreadId = ThreadId())
    : pD3DDevice(device),
      D3D11(pD3DContext(pD3DContext),)
      Render::HALInitParams(0, halConfigFlags, renderThreadId)
    { }

    // D3D1x::TextureManager accessors for correct type.
    void SetTextureManager(TextureManager* manager) { pTextureManager = manager; }
    TextureManager* GetTextureManager() const       { return (TextureManager*) pTextureManager.GetPtr(); }
};


//------------------------------------------------------------------------

class HAL : public Render::HAL
{
public:

    // Direct3D Device we are using.
    ID3D1x(Device)*               pDevice;
    ID3D1x(DeviceContext)*        pDeviceContext;
    Ptr<ID3D1x(RenderTargetView)> pRenderTargetView;
    Ptr<ID3D1x(DepthStencilView)> pDepthStencilView;

    MeshCache            Cache;

    ShaderManager            SManager;
    Ptr<TextureManager>      pTextureManager;
    FragShader               StaticFShaders[FragShaderDesc::FSI_Count];
    VertexShader             StaticVShaders[VertexShaderDesc::VSI_Count];
    ShaderInterface          ShaderData;
    
    // Previous batching mode
    PrimitiveBatch::BatchType PrevBatchType;

    // Self-accessor used to avoid constructor warning.
    HAL*      getThis() { return this; }

public:    
    
    HAL(ThreadCommandQueue* commandQueue = 0);
    virtual ~HAL();
    
    // *** HAL Initialization / Shutdown Logic

    // Initializes HAL for rendering.
    virtual bool        InitHAL(const D3D1x::HALInitParams& params);
    // ShutdownHAL shuts down rendering, releasing resources allocated in InitHAL.
    virtual bool        ShutdownHAL();

    // - PrepareForReset should be called before the device's SwapChain is resized.
    // The HAL holds a reference to the default back and depthstencil buffers, which need
    // to be released and reobtained.
    void                PrepareForReset();
    // - RestoreAfterReset called after reset to restore needed variables.
    bool                RestoreAfterReset();


    // *** Rendering
    virtual bool        BeginScene();
    virtual bool        EndScene();

    // Bracket the displaying of a frame from a movie.
    // Fill the background color, and set up default transforms, etc.
    virtual void        beginDisplay(BeginDisplayData* data);

    void                CalcHWViewMatrix(Matrix* pmatrix, const Rect<int>& viewRect,
                                         int dx, int dy);

    // Updates D3D HW Viewport and ViewportMatrix based on the current
    // values of VP, ViewRect and ViewportValid.
    virtual void        updateViewport();


    // Creates / Destroys mesh and DP data 

    virtual PrimitiveFill*  CreatePrimitiveFill(const PrimitiveFillData& data);    

    virtual void        DrawProcessedPrimitive(Primitive* pprimitive,
                                               PrimitiveBatch* pstart, PrimitiveBatch *pend);

    virtual void        DrawProcessedComplexMeshes(ComplexMesh* complexMesh,
                                                   const StrideArray<HMatrix>& matrices);

    // *** Mask Support
    // This flag indicates whether we've checked for stencil after BeginDisplay or not.
    bool            StencilChecked;
    // This flag is stencil is available, after check.
    bool            StencilAvailable;
    bool            DepthBufferAvailable;    

    virtual void    PushMask_BeginSubmit(MaskPrimitive* primitive);
    virtual void    EndMaskSubmit();
    virtual void    PopMask();

    virtual void    beginMaskDisplay()
    {
        SF_ASSERT(MaskStackTop == 0);
        StencilChecked  = 0;
        StencilAvailable= 0;
        DepthBufferAvailable = 0;
        HALState &= ~HS_DrawingMask;
    }

    bool    checkMaskBufferCaps();
    void    drawMaskClearRectangles(const HMatrix* matrices, UPInt count);

    // Background clear helper, expects viewport coordinates.
    virtual void    clearSolidRectangle(const Rect<int>& r, Color color);

    // *** BlendMode
    void            applyBlendModeImpl(BlendMode mode, bool sourceAc = false, bool forceAc = false);


    // *** Device States
    enum ColorWriteMode
    {
        Write_All,
        Write_None,
        Write_Count
    };

    static const unsigned BlendTypeCount = Blend_Count*2 + 1;
    static unsigned GetBlendType( BlendMode blendMode, ColorWriteMode writeMode, bool sourceAc = false) 
    { 
        unsigned base = 0;
        if ( writeMode == Write_None )
        {
            base = Blend_Count*2;
            blendMode = (BlendMode)0;
        }
        else if ( sourceAc )
            base += Blend_Count;
        return base + (unsigned)blendMode;
    }
    ID3D1x(BlendState)* BlendStates[BlendTypeCount];
    bool createBlendStates();
    void destroyBlendStates();

    enum StencilModes
    {
        StencilMode_Disabled,
        StencilMode_Available_ClearMasks,
        StencilMode_Available_ClearMasksAbove,
        StencilMode_Available_WriteMask,
        StencilMode_DepthOnly_WriteMask,
        StencilMode_Available_TestMask,
        StencilMode_DepthOnly_TestMask,
        StencilMode_Unavailable,
        StencilMode_Count
    };
    ID3D1x(DepthStencilState)* DepthStencilStates[StencilMode_Count];
    bool createDepthStencilStates();
    void destroyDepthStencilStates();

    enum RasterModes
    {
        RasterMode_Default,
        RasterMode_Wireframe,
        RasterMode_Count
    };
    RasterModes RasterMode;
    void SetRasterMode(RasterModes mode) { RasterMode = mode; }
    ID3D1x(RasterizerState)* RasterStates[RasterMode_Count];
    bool createRasterStates();
    void destroyRasterStates();


    ID3D1x(Device)*   GetDevice() const { return pDevice; }

    static const unsigned   ConstantBufferCount = 8;
    ID3D1x(Buffer)*         ConstantBuffers[ConstantBufferCount];
    unsigned                CurrentConstantBuffer;

    bool                  createConstantBuffers();
    void                  destroyConstantBuffers();
    ID3D1x(Buffer)*       getNextConstantBuffer();

    virtual Render::TextureManager* GetTextureManager() const
    {
        return pTextureManager.GetPtr();
    }

    virtual RenderTarget*   CreateRenderTarget(ID3D1x(View)* pRenderTarget, ID3D1x(View)* pDepthStencil);
    virtual RenderTarget*   CreateRenderTarget(Render::Texture* texture, bool needsStencil);
    virtual RenderTarget*   CreateTempRenderTarget(const ImageSize& size, bool needsStencil);
    virtual bool            SetRenderTarget(RenderTarget* target, bool setState = 1);
    virtual void            PushRenderTarget(const RectF& frameRect, RenderTarget* prt, unsigned flags =0);
    virtual void            PopRenderTarget(unsigned flags = 0);

    virtual bool            createDefaultRenderBuffer();

    // *** Filters
    virtual void          PushFilters(FilterPrimitive* primitive);
    virtual void          drawUncachedFilter(const FilterStackEntry& e);
    virtual void          drawCachedFilter(FilterPrimitive* primitive);

    // *** DrawableImage
    virtual void          DrawableCxform( Render::Texture** tex, const Matrix2F* texgen, const Cxform* cx);
    virtual void          DrawableCompare( Render::Texture** tex, const Matrix2F* texgen );
    virtual void          DrawableCopyChannel( Render::Texture** tex, const Matrix2F* texgen, const Matrix4F* cxmul ) 
                          { DrawableMerge(tex, texgen, cxmul); }
    virtual void          DrawableMerge( Render::Texture** tex, const Matrix2F* texgen, const Matrix4F* cxmul );
    virtual void          DrawableCopyPixels( Render::Texture** tex, const Matrix2F* texgen, const Matrix2F& mvp, 
                                              bool mergeAlpha, bool destAlpha );
    virtual void          DrawablePaletteMap( Render::Texture** tex, const Matrix2F* texgen, const Matrix2F& mvp, 
                                              unsigned channelMask, const UInt32* values);
    virtual void          DrawableThreshold(  Render::Texture** tex, const Matrix2F* texgen, const Matrix2F& mvp, 
                                              DrawableImage::OperationType op, UInt32 threshold, UInt32 color, 
                                              UInt32 mask, bool copySource);
    virtual void          DrawableCopyback( Render::Texture* tex, const Matrix2F& mvp, const Matrix2F& texgen );


    virtual class MeshCache&       GetMeshCache()        { return Cache; }
        
    virtual void    MapVertexFormat(PrimitiveFillType fill, const VertexFormat* sourceFormat,
                                    const VertexFormat** single,
                                    const VertexFormat** batch, const VertexFormat** instanced, 
                                    unsigned flags = 0)
    {
        SF_UNUSED(flags);
        SManager.MapVertexFormat(fill, sourceFormat,
                                        single, batch, instanced);
    }

protected:

    void        drawPrimitive(unsigned indexCount, unsigned meshCount);
    void        drawIndexedPrimitive( unsigned indexCount, unsigned meshCount, UPInt indexOffset, unsigned vertexBaseIndex);
    void        drawIndexedInstanced( unsigned indexCount, unsigned meshCount, UPInt indexOffset, unsigned vertexBaseIndex);

    // *** Events
    virtual Render::RenderEvent& GetEvent(EventType eventType);

    void drawScreenQuad();

    // Cached mappings of VertexXY16fAlpha vertex format.
    VertexFormat* MappedXY16fAlphaTexture[PrimitiveBatch::DP_DrawableCount];
    VertexFormat* MappedXY16fAlphaSolid[PrimitiveBatch::DP_DrawableCount];
};

//--------------------------------------------------------------------
// RenderTargetData, used for both RenderTargets and DepthStencilSurface implementations.
class RenderTargetData : public RenderBuffer::RenderTargetData
{
public:
    friend class HAL;
    ID3D1x(View)*           pRenderSurface;         // View of the render target.
    ID3D1x(View)*           pDSSurface;             // View of the depth stencil surface (0 if not required).

    static void UpdateData( RenderBuffer* buffer, ID3D1x(View)* prt, 
                            DepthStencilBuffer* pdsb, ID3D1x(View)* pdss )
    {
        if ( !buffer )
            return;

        RenderTargetData* poldHD = (D3D1x::RenderTargetData*)buffer->GetRenderTargetData();
        if ( !poldHD )
        {
            poldHD = SF_NEW RenderTargetData(buffer, prt, pdsb, pdss);
            buffer->SetRenderTargetData(poldHD);
            return;
        }

        if ( prt )
            prt->AddRef();
        if ( pdss )
            pdss->AddRef();
        if ( poldHD->pRenderSurface ) 
            poldHD->pRenderSurface->Release();
        if ( poldHD->pDSSurface ) 
            poldHD->pDSSurface->Release();

        poldHD->pDepthStencilBuffer = pdsb;
        poldHD->pDSSurface          = pdss;
        poldHD->pRenderSurface      = prt;
    }

    virtual ~RenderTargetData()
    {
        if ( pRenderSurface )
            pRenderSurface->Release();
        if ( pDSSurface )
            pDSSurface->Release();
    }

private:
    RenderTargetData( RenderBuffer* buffer, ID3D1x(View)* prt, DepthStencilBuffer* pdsb, ID3D1x(View)* pdss) : 
      RenderBuffer::RenderTargetData(buffer, pdsb), pRenderSurface(prt), pDSSurface(pdss)
    {
        if ( pRenderSurface )
            pRenderSurface->AddRef();
        if ( pDSSurface )
            pDSSurface->AddRef();
    }
};

}}} // Scaleform::Render::D3D1x

#endif // INC_SF_D3D1X_HAL_H
