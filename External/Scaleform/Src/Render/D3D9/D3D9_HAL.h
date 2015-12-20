/**************************************************************************

Filename    :   D3D9_HAL.h
Content     :   Renderer HAL Prototype header.
Created     :   May 2009
Authors     :   Michael Antonov

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef INC_SF_Render_D3D9_HAL_H
#define INC_SF_Render_D3D9_HAL_H

#include "Render/Render_HAL.h"
#include "Render/D3D9/D3D9_MeshCache.h"
#include "Render/D3D9/D3D9_Texture.h"
#include "Render/D3D9/D3D9_Shader.h" // ShaderManager

#include <d3d9.h>

namespace Scaleform { namespace Render { namespace D3D9 {


// HALConfigFlags enumeration defines system-specific HAL configuration
// flags passed into InitHAL though HALInitParams.

enum HALConfigFlags
{
    // Prevents BeginScene/EndScene from being called inside BeginDisplay/EndDisplay
    // - assumes that Direct3D is already in scene by the time BeginDisplay/EndDisplay are called
    // - allows user to manage their own begin/end scene calls 
    HALConfig_NoSceneCalls          = 0x00000001,

    // Use D3DUSAGE_DYNAMIC updaeable textures for compatibility with some broken drivers
    HALConfig_UseDynamicTex         = 0x00000004,

    // Use static buffers in the mesh cache. This has no visible impact, however, it may
    // have performance implications. Rendering from static buffers generally has faster 
    // throughput, but can cause significant latency issues with dynamic content. This also 
    // may be used silently, if the underlying hardware does not support D3D9 queries.
    HALConfig_StaticBuffers         = 0x00000010,
};


// D3D9::HALInitParems provides D3D9-specific rendering initialization
// parameters for HAL::InitHAL.

struct HALInitParams : public Render::HALInitParams
{
    IDirect3DDeviceX*       pD3DDevice;
    D3DPRESENT_PARAMETERS   PresentParams;

    HALInitParams(IDirect3DDeviceX* device,
                  const D3DPRESENT_PARAMETERS& presentParams,
                  UInt32 halConfigFlags = 0,
                  ThreadId renderThreadId = ThreadId())
    : pD3DDevice(device), PresentParams(presentParams),
      Render::HALInitParams(0, halConfigFlags, renderThreadId)
    {
    }

    // D3D9::TextureManager accessors for correct type.
    void SetTextureManager(TextureManager* manager) { pTextureManager = manager; }
    TextureManager* GetTextureManager() const       { return (TextureManager*) pTextureManager.GetPtr(); }
};



class HAL : public Render::HAL
{
public:

    // Direct3D Device we are using.
    IDirect3DDeviceX*        pDevice;

    // Presentation parameters specified to configure the mode.
    D3DPRESENT_PARAMETERS    PresentParams;

    MeshCache                Cache;

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

    // Initialize rendering
    virtual bool        InitHAL(const D3D9::HALInitParams& params);
    // Shutdown rendering (cleanup).
    virtual bool        ShutdownHAL();

    // D3D9 device Reset and lost device support.
    // - PrepareForReset should be called before IDirect3DDevice9::Reset to release
    // caches and other system-specific references.
    void                PrepareForReset();
    // - RestoreAfterReset called after reset to restore needed variables.
    bool                RestoreAfterReset();


    // *** Rendering
    virtual bool        BeginScene();
    virtual bool        EndScene();

    void                CalcHWViewMatrix(Matrix* pmatrix, const Rect<int>& viewRect,
                                         int dx, int dy);

    // Updates D3D HW Viewport and ViewportMatrix based on the current
    // values of VP, ViewRect and ViewportValid.
    virtual void        updateViewport();


    // Creates / Destroys mesh and DP data 

    virtual PrimitiveFill*  CreatePrimitiveFill(const PrimitiveFillData& data);    

    virtual void        DrawProcessedPrimitive(Primitive* pprimitive,
                                               PrimitiveBatch* pstart, PrimitiveBatch *pend);

    virtual void        DrawProcessedComplexMeshes(ComplexMesh* p,
                                                   const StrideArray<HMatrix>& matrices);
    
    // Stream Source modification

    inline void         setLinearStreamSource(PrimitiveBatch::BatchType type)
    {
        if (PrevBatchType >= PrimitiveBatch::DP_Instanced)
        {
            pDevice->SetStreamSource(1, NULL, 0, 0);
            pDevice->SetStreamSourceFreq(0, 1);
            pDevice->SetStreamSourceFreq(1, 1);
        }
        PrevBatchType = type;
    }

    inline void         setInstancedStreamSource(unsigned instanceCount)
    {
        SF_ASSERT(SManager.HasInstancingSupport());
        if (PrevBatchType != PrimitiveBatch::DP_Instanced)
        {
            pDevice->SetStreamSource(1, Cache.pInstancingVertexBuffer.GetPtr(),
                                     0, sizeof(Render_InstanceData));
            pDevice->SetStreamSourceFreq(1, (UINT) D3DSTREAMSOURCE_INSTANCEDATA | 1);
            PrevBatchType = PrimitiveBatch::DP_Instanced;
        }                
        pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | instanceCount);
    }


    // *** Mask Support
    // This flag indicates whether we've checked for stencil after BeginDisplay or not.
    bool            StencilChecked;
    // This flag is stencil is available, after check.
    bool            StencilAvailable;
    bool            MultiBitStencil;
    bool            DepthBufferAvailable;    
    // Increment op we need for stencil.
    D3DSTENCILOP    StencilOpInc;    

    virtual void    PushMask_BeginSubmit(MaskPrimitive* primitive);
    virtual void    EndMaskSubmit();
    virtual void    PopMask();

    virtual void    beginMaskDisplay()
    {
        SF_ASSERT(MaskStackTop == 0);
        StencilChecked  = 0;
        StencilAvailable= 0;
        MultiBitStencil = 0;
        DepthBufferAvailable = 0;
        HALState &= ~HS_DrawingMask;
    }

    bool    checkMaskBufferCaps();
    void    drawMaskClearRectangles(const HMatrix* matrices, UPInt count);

    // Background clear helper, expects viewport coordinates.
    virtual void        clearSolidRectangle(const Rect<int>& r, Color color);


    // *** BlendMode
    virtual void        applyBlendModeImpl(BlendMode mode, bool sourceAc = false, bool forceAc = false);



    IDirect3DDeviceX*   GetDevice() const { return pDevice; }


    virtual Render::TextureManager* GetTextureManager() const
    {
        return pTextureManager.GetPtr();
    }

    virtual RenderTarget*   CreateRenderTarget(IDirect3DSurface9* pcolor, IDirect3DSurface9* pdepth);
    virtual RenderTarget*   CreateRenderTarget(Render::Texture* texture, bool needsStencil);
    virtual RenderTarget*   CreateTempRenderTarget(const ImageSize& size, bool needsStencil);
    virtual bool            SetRenderTarget(RenderTarget* target, bool setState = 1);
    virtual void            PushRenderTarget(const RectF& frameRect, RenderTarget* prt, unsigned flags=0);
    virtual void            PopRenderTarget(unsigned flags = 0);
    
    virtual bool            createDefaultRenderBuffer();

    // *** Filters
    virtual void            PushFilters(FilterPrimitive* primitive);
    virtual void            drawUncachedFilter(const FilterStackEntry& e);
    virtual void            drawCachedFilter(FilterPrimitive* primitive);

    // *** DrawableImage
    virtual void            DrawableCxform( Render::Texture** tex, const Matrix2F* texgen, const Cxform* cx);
    virtual void            DrawableCompare( Render::Texture** tex, const Matrix2F* texgen );
    virtual void            DrawableCopyChannel( Render::Texture** tex, const Matrix2F* texgen, const Matrix4F* cxmul ) 
                            { DrawableMerge(tex, texgen, cxmul); }
    virtual void            DrawableMerge( Render::Texture** tex, const Matrix2F* texgen, const Matrix4F* cxmul );
    virtual void            DrawableCopyPixels( Render::Texture** tex, const Matrix2F* texgen, const Matrix2F& mvp, 
                                                bool mergeAlpha, bool destAlpha );
    virtual void            DrawablePaletteMap( Render::Texture** tex, const Matrix2F* texgen, const Matrix2F& mvp, 
                                                unsigned channelMask, const UInt32* values);
    virtual void            DrawableThreshold(  Render::Texture** tex, const Matrix2F* texgen, const Matrix2F& mvp, 
                                                DrawableImage::OperationType op, UInt32 threshold, UInt32 color, 
                                                UInt32 mask, bool copySource);
    virtual void            DrawableCopyback( Render::Texture* tex, const Matrix2F& mvp, const Matrix2F& texgen );

    virtual class MeshCache&       GetMeshCache()        { return Cache; }
        
    virtual void    MapVertexFormat(PrimitiveFillType fill, const VertexFormat* sourceFormat,
                                    const VertexFormat** single,
                                    const VertexFormat** batch, const VertexFormat** instanced, unsigned flags =0)
    {
        SF_UNUSED(flags);
        SManager.MapVertexFormat(fill, sourceFormat, single, batch, instanced, 
            MVF_Align | MVF_ReverseColor | (SManager.HasInstancingSupport() ? MVF_HasInstancing : 0));
    }

protected:

    void        drawPrimitive(unsigned indexCount, unsigned meshCount);
    void        drawIndexedPrimitive( unsigned indexCount, unsigned vertexCount, unsigned meshCount, UPInt indexOffset, unsigned vertexBaseIndex);
    void        drawIndexedInstanced( unsigned indexCount, unsigned vertexCount, unsigned meshCount, UPInt indexOffset, unsigned vertexBaseIndex);

    // Initializes all shaders. If compilation of any shader fails, returns false, in which case the Prog member is set to zero.
    bool                initializeShaders(bool force = false);

    // Returns whether the profile can render any of the filters contained in the FilterPrimitive.
    // If a profile does not support dynamic looping (Cap_NoDynamicLoops), no blur/shadow type filters
    // can be rendered, in which case this may return false, however, ColorMatrix filters can still be rendered.
    bool                shouldRenderFilters(const FilterPrimitive* prim) const;

    // Simply sets a quad vertex buffer and draws.
    virtual void        drawScreenQuad();

    // *** Events
    virtual Render::RenderEvent& GetEvent(EventType eventType);

    // Cached mappings of VertexXY16iAlpha vertex format.
    VertexFormat* MappedXY16iAlphaTexture[PrimitiveBatch::DP_DrawableCount];
    VertexFormat* MappedXY16iAlphaSolid[PrimitiveBatch::DP_DrawableCount];
};

//--------------------------------------------------------------------
// RenderTargetData, used for both RenderTargets and DepthStencilSurface implementations.
class RenderTargetData : public RenderBuffer::RenderTargetData
{
public:
    friend class HAL;
    IDirect3DSurface9*       pRenderSurface;        // Render target surface.
    IDirect3DSurface9*       pDSSurface;            // Depth stencil surface (0 if not required)

    static void UpdateData( RenderBuffer* buffer, IDirect3DSurface9* prt, 
                            DepthStencilBuffer* pdsb, IDirect3DSurface9* pdss )
    {
        if ( !buffer )
            return;

        RenderTargetData* poldHD = (D3D9::RenderTargetData*)buffer->GetRenderTargetData();
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
        poldHD->CacheID             = 0;
    }

    virtual ~RenderTargetData()
    {
        if ( pRenderSurface )
            pRenderSurface->Release();
        if ( pDSSurface )
            pDSSurface->Release();        
        pDepthStencilBuffer = 0;
    }

private:
    RenderTargetData( RenderBuffer* buffer, IDirect3DSurface9* prt, DepthStencilBuffer* pdsb, IDirect3DSurface9* pdss) : 
      RenderBuffer::RenderTargetData(buffer, pdsb), pRenderSurface(prt), pDSSurface(pdss)
    {
        if ( pRenderSurface )
            pRenderSurface->AddRef();
        if ( pDSSurface )
            pDSSurface->AddRef();        
    }
};


}}} // Scaleform::Render::D3D9

#endif
