/**************************************************************************

Filename    :   GL_HAL.cpp
Content     :   GL Renderer HAL Prototype implementation.
Created     :   
Authors     :   

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#if !defined(SF_USE_GLES)   // Do not compile under GLES 1.1

#include "Kernel/SF_Debug.h"
#include "Kernel/SF_Random.h"
#include "Kernel/SF_HeapNew.h"
#include "Kernel/SF_Debug.h"

#include "Render/GL/GL_HAL.h"
#include "Render/GL/GL_Events.h"

#ifdef SF_GL_BINARY_SHADERS
#include <unistd.h>
#include "Kernel/SF_SysFile.h"
#endif


namespace Scaleform { namespace Render { namespace GL {


// ***** RenderHAL_GL

HAL::HAL(ThreadCommandQueue* commandQueue)
:   Render::HAL(commandQueue),
    MultiBitStencil(1),
    EnabledVertexArrays(-1),
    Cache(Memory::GetGlobalHeap(), MeshCacheParams::PC_Defaults),
    SManager(&Profiler),
    ShaderData(GetHAL()),
    PrevBatchType(PrimitiveBatch::DP_None)
{
    Caps = 0;
    memset(StaticShaders, 0, sizeof(StaticShaders));
    memset(MappedXY16iUVTexture, 0, sizeof(MappedXY16iUVTexture));
    memset(MappedXY16iAlphaTexture, 0, sizeof(MappedXY16iAlphaTexture));
    memset(MappedXY16iAlphaSolid, 0, sizeof(MappedXY16iAlphaSolid));
}

HAL::~HAL()
{
    ShutdownHAL();
}

void   HAL::MapVertexFormat(PrimitiveFillType fill, const VertexFormat* sourceFormat,
                            const VertexFormat** single,
                            const VertexFormat** batch, const VertexFormat** instanced, unsigned)
{
    // TODO: enable instancing
    unsigned instancingFlag = 0 ; // (Caps&Cap_Instancing ? MVF_HasInstancing : 0);
    SManager.MapVertexFormat(fill, sourceFormat, single, batch, instanced, (Caps&MVF_Align)| instancingFlag);
#if defined(GL_ES_VERSION_2_0)
    if (Caps & Cap_NoBatching)
        *batch = 0;
#endif
}

PrimitiveFill*  HAL::CreatePrimitiveFill(const PrimitiveFillData &data)
{
    return SF_HEAP_NEW(pHeap) PrimitiveFill(data);
}

// Draws a range of pre-cached and preprocessed primitives
void        HAL::DrawProcessedPrimitive(Primitive* pprimitive,
                                        PrimitiveBatch* pstart, PrimitiveBatch *pend)
{
    SF_AMP_SCOPE_RENDER_TIMER("HAL::DrawProcessedPrimitive", Amp_Profile_Level_High);
    if (!checkState(HS_InDisplay, __FUNCTION__) ||
        !pprimitive->GetMeshCount() )
        return;

    // If in overdraw profile mode, and this primitive is part of a mask, draw it in color mode.
    static bool drawingMask = false;
    if ( !Profiler.ShouldDrawMask() && !drawingMask && (HALState & HS_DrawingMask) )
    {
        drawingMask = true;
        glColorMask(1,1,1,1);
        glDisable(GL_STENCIL_TEST);
        DrawProcessedPrimitive(pprimitive, pstart, pend );
        glColorMask(0,0,0,0);
        glEnable(GL_STENCIL_TEST);
        drawingMask = false;
    }

    SF_ASSERT(pend != 0);
    
    PrimitiveBatch* pbatch = pstart ? pstart : pprimitive->Batches.GetFirst();

    unsigned bidx = 0;
    while (pbatch != pend)
    {        
        // pBatchMesh can be null in case of error, such as VB/IB lock failure.
        MeshCacheItem* pmesh = (MeshCacheItem*)pbatch->GetCacheItem();
        unsigned       meshIndex = pbatch->GetMeshIndex();
        unsigned       batchMeshCount = pbatch->GetMeshCount();

        if (pmesh)
        {
            Profiler.SetBatch((UPInt)pprimitive, bidx);

            unsigned fillFlags = FillFlags;
            if ( batchMeshCount > 0 )
                fillFlags |= pprimitive->Meshes[0].M.Has3D() ? FF_3DProjection : 0;

            const ShaderManager::Shader& pShader =
                SManager.SetPrimitiveFill(pprimitive->pFill, fillFlags, pbatch->Type, pbatch->pFormat, batchMeshCount, Matrices,
                                          &pprimitive->Meshes[meshIndex], &ShaderData);

            if ((HALState & HS_ViewValid) && pShader &&
                SetVertexArray(pbatch->pFormat, pmesh->pVertexBuffer->GetBuffer(),
                               pmesh->pVertexBuffer->GetBufferBase() + pmesh->VBAllocOffset))
            {
                SF_ASSERT((pbatch->Type != PrimitiveBatch::DP_Failed) &&
                          (pbatch->Type != PrimitiveBatch::DP_Virtual) &&
                          (pbatch->Type != PrimitiveBatch::DP_Instanced));


                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pmesh->pIndexBuffer->GetBuffer());

                // Make sure the blending state is correct.
                int blend = (fillFlags & FF_Blending) ? 1 : 0;
                if (blend != BlendEnable)
                {
                    if (blend)
                        glEnable(GL_BLEND);
                    else
                        glDisable(GL_BLEND);
                    BlendEnable = blend;
                }
                Profiler.SetFillFlags(fillFlags);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pmesh->pIndexBuffer->GetBuffer());
                // Draw the object with cached mesh.
                if (pbatch->Type != PrimitiveBatch::DP_Instanced)
                	drawIndexedPrimitive(pmesh->IndexCount, pmesh->MeshCount, pmesh->pIndexBuffer->GetBufferBase() + pmesh->IBAllocOffset);
                else
                	drawIndexedInstanced(pmesh->IndexCount, batchMeshCount, pmesh->pIndexBuffer->GetBufferBase() + pmesh->IBAllocOffset);
            }

            pmesh->MoveToCacheListFront(MCL_ThisFrame);
        }

        pbatch = pbatch->GetNext();
        bidx++;
    }
}


void HAL::DrawProcessedComplexMeshes(ComplexMesh* complexMesh,
                                           const StrideArray<HMatrix>& matrices)
{    
    typedef ComplexMesh::FillRecord   FillRecord;
    typedef PrimitiveBatch::BatchType BatchType;
    
    MeshCacheItem* pmesh = (MeshCacheItem*)complexMesh->GetCacheItem();
    if (!checkState(HS_InDisplay, __FUNCTION__) || !pmesh)
        return;
    
    // If in overdraw profile mode, and this primitive is part of a mask, draw it in color mode.
    static bool drawingMask = false;
    if ( !Profiler.ShouldDrawMask() && !drawingMask && (HALState & HS_DrawingMask) )
    {
        drawingMask = true;
        glColorMask(1,1,1,1);
        glDisable(GL_STENCIL_TEST);
        DrawProcessedComplexMeshes(complexMesh, matrices);
        glColorMask(0,0,0,0);
        glEnable(GL_STENCIL_TEST);
        drawingMask = false;
    }

    const FillRecord* fillRecords = complexMesh->GetFillRecords();
    unsigned    fillCount     = complexMesh->GetFillRecordCount();
    unsigned    instanceCount = (unsigned)matrices.GetSize();
    BatchType   batchType = PrimitiveBatch::DP_Single;
    unsigned    formatIndex;
    unsigned    maxDrawCount = 1;

    if (instanceCount > 1 && SManager.HasInstancingSupport())
    {
        maxDrawCount = Alg::Min(instanceCount, Cache.GetParams().MaxBatchInstances);
        batchType = PrimitiveBatch::DP_Instanced;
        formatIndex = 1;
    }
    else
    {
        batchType = PrimitiveBatch::DP_Single;
        formatIndex = 0;
    }

    const Matrix2F* textureMatrices = complexMesh->GetFillMatrixCache();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pmesh->pIndexBuffer->GetBuffer());
    ShaderData.BeginPrimitive();

    for (unsigned fillIndex = 0; fillIndex < fillCount; fillIndex++)
    {
        const FillRecord& fr = fillRecords[fillIndex];

        Profiler.SetBatch((UPInt)complexMesh, fillIndex);

        unsigned fillFlags = FillFlags;
        unsigned startIndex = 0;
        if ( instanceCount > 0 )
        {
            const HMatrix& hm = matrices[0];
            fillFlags |= hm.Has3D() ? FF_3DProjection : 0;

            for (unsigned i = 0; i < instanceCount; i++)
            {
                const HMatrix& hm = matrices[startIndex + i];
                Cxform finalCx = Profiler.GetCxform(hm.GetCxform());
                if (!(finalCx == Cxform::Identity))
                    fillFlags |= FF_Cxform;
                if (finalCx.RequiresBlend())
                    fillFlags |= FF_Blending;
            }
        }

        // Apply fill.
        PrimitiveFillType fillType = Profiler.GetFillType(fr.pFill->GetType());
        const ShaderManager::Shader& pso = SManager.SetFill(fillType, fillFlags, batchType, fr.pFormats[formatIndex], &ShaderData);
        SetVertexArray(fr.pFormats[formatIndex], pmesh->pVertexBuffer->GetBuffer(), pmesh->pVertexBuffer->GetBufferBase() + pmesh->VBAllocOffset + fr.VertexByteOffset);

        UByte textureCount = fr.pFill->GetTextureCount();
        bool solid = (fillType == PrimFill_None || fillType == PrimFill_Mask || fillType == PrimFill_SolidColor);

        int blend = ((fillFlags & FF_Blending) || fr.pFill->RequiresBlend()) ? 1 : 0;
        if (blend != BlendEnable)
        {
            if (blend)
                glEnable(GL_BLEND);
            else
                glDisable(GL_BLEND);
            BlendEnable = blend;
        }

        Profiler.SetFillFlags(fillFlags);

        for (unsigned i = 0; i < instanceCount; i++)
        {            
            const HMatrix& hm = matrices[startIndex + i];

            ShaderData.SetMatrix(pso, Uniform::SU_mvp, complexMesh->GetVertexMatrix(), hm, Matrices, 0, i%maxDrawCount);
            if (solid)
                ShaderData.SetColor(pso, Uniform::SU_cxmul, Profiler.GetColor(fr.pFill->GetSolidColor()), 0, i%maxDrawCount);
            else if (fillFlags & FF_Cxform)
                ShaderData.SetCxform(pso, Profiler.GetCxform(hm.GetCxform()), 0, i%maxDrawCount);

            for (unsigned tm = 0, stage = 0; tm < textureCount; tm++)
            {
                ShaderData.SetMatrix(pso, Uniform::SU_texgen, textureMatrices[fr.FillMatrixIndex[tm]], tm, i%maxDrawCount);
                Texture* ptex = (Texture*)fr.pFill->GetTexture(tm);
                ShaderData.SetTexture(pso, Uniform::SU_tex, ptex, fr.pFill->GetFillMode(tm), stage);
                stage += ptex->GetPlaneCount();
            }

            ShaderData.Finish(0);

            bool lastPrimitive = (i == instanceCount-1);
            if ( batchType != PrimitiveBatch::DP_Instanced )
            {
                ShaderData.Finish(1);
            	drawIndexedPrimitive(fr.IndexCount, 1, pmesh->pIndexBuffer->GetBufferBase() + pmesh->IBAllocOffset + sizeof(IndexType) * fr.IndexOffset);
                AccumulatedStats.Primitives++;
                if ( !lastPrimitive )
                    ShaderData.BeginPrimitive();
            }
            else if (( (i+1) % maxDrawCount == 0 && i != 0) || lastPrimitive )
            {
                unsigned drawCount = maxDrawCount;
                if ( lastPrimitive && (i+1) % maxDrawCount != 0)
                    drawCount = (i+1) % maxDrawCount;
                ShaderData.Finish(drawCount);
            	drawIndexedInstanced(fr.IndexCount, drawCount, pmesh->pIndexBuffer->GetBufferBase() + pmesh->IBAllocOffset + sizeof(IndexType) * fr.IndexOffset);
                AccumulatedStats.Primitives++;
                if ( !lastPrimitive )
                    ShaderData.BeginPrimitive();
            }
        }
    } // for (fill record)
  
    pmesh->MoveToCacheListFront(MCL_ThisFrame);
}


//--------------------------------------------------------------------
// Background clear helper, expects viewport coordinates.
void HAL::clearSolidRectangle(const Rect<int>& r, Color color)
{
    color = Profiler.GetClearColor(color);

    if (color.GetAlpha() == 0xFF && !(VP.Flags & Viewport::View_Stereo_AnySplit))
    {
        glEnable(GL_SCISSOR_TEST);

        PointF tl((float)(VP.Left + r.x1), (float)(VP.Top + r.y1));
        PointF br((float)(VP.Left + r.x2), (float)(VP.Top + r.y2));
        tl = Matrices->Orient2D * tl;
        br = Matrices->Orient2D * br;
        Rect<int> scissor((int)Alg::Min(tl.x, br.x), (int)Alg::Min(tl.y,br.y), (int)Alg::Max(tl.x,br.x), (int)Alg::Max(tl.y,br.y));
        glScissor(scissor.x1, scissor.y1, scissor.Width(), scissor.Height());
        glClearColor(color.GetRed() * 1.f/255.f, color.GetGreen() * 1.f/255.f, color.GetBlue() * 1.f/255.f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

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
    else
    {
        float colorf[4];
        color.GetRGBAFloat(colorf, colorf+1, colorf+2, colorf+3);
        Matrix2F m((float)r.Width(), 0.0f, (float)r.x1,
                   0.0f, (float)r.Height(), (float)r.y1);

        Matrix2F  mvp(m, Matrices->UserView);

        unsigned fillflags = 0;
        const ShaderManager::Shader& pso = SManager.SetFill(PrimFill_SolidColor, fillflags, PrimitiveBatch::DP_Single, &VertexXY16iInstance::Format, &ShaderData);
        ShaderData.SetMatrix(pso, Uniform::SU_mvp, mvp);
        ShaderData.SetUniform(pso, Uniform::SU_cxmul, colorf, 4);
        ShaderData.Finish(1);

        SetVertexArray(&VertexXY16iInstance::Format, Cache.MaskEraseBatchVertexBuffer, 0);
        drawPrimitive(6,1);
    }
}

//--------------------------------------------------------------------
// *** Mask / Stencil support
//--------------------------------------------------------------------

// Mask support is implemented as a stack, enabling for a number of optimizations:
//
// 1. Large "Clipped" marks are clipped to a custom viewport, allowing to save on
//    fill-rate when rendering both the mask and its content. The mask area threshold
//    that triggers this behavior is determined externally.
//      - Clipped masks can be nested, but not batched. When erased, clipped masks
//        clear the clipped intersection area.
// 2. Small masks can be Batched, having multiple mask areas with multiple mask
//    content items inside.
//      - Small masks can contain clipped masks either regular or clipped masks.
// 3. Mask area dimensions are provided as HMatrix, which maps a unit rectangle {0,0,1,1}
//    to a mask bounding rectangle. This rectangle can be rotated (non-axis aligned),
//    allowing for more efficient fill.
// 4. PopMask stack optimization is implemented that does not erase nested masks; 
//    Stencil Reference value is changed instead. Erase of a mask only becomes
//    necessary if another PushMask_BeginSubmit is called, in which case previous
//    mask bounding rectangles are erased. This setup avoids often unnecessary erase 
//    operations when drawing content following a nested mask.
//      - To implement this MaskStack keeps a previous "stale" MaskPrimitive
//        located above the MaskStackTop.


void HAL::PushMask_BeginSubmit(MaskPrimitive* prim)
{
    if (!checkState(HS_InDisplay, __FUNCTION__))
        return;

    Profiler.SetDrawMode(1);

    glColorMask(0,0,0,0);                       // disable framebuffer writes
    glEnable(GL_STENCIL_TEST);

    bool viewportValid = (HALState & HS_ViewValid) != 0;

    // Erase previous mask if it existed above our current stack top.
    if (MaskStackTop && (MaskStack.GetSize() > MaskStackTop) && viewportValid && MultiBitStencil)
    {
        glStencilFunc(GL_LEQUAL, MaskStackTop, 0xff);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        MaskPrimitive* erasePrim = MaskStack[MaskStackTop].pPrimitive;
        drawMaskClearRectangles(erasePrim->GetMaskAreaMatrices(), erasePrim->GetMaskCount());
    }

    MaskStack.Resize(MaskStackTop+1);
    MaskStackEntry &e = MaskStack[MaskStackTop];
    e.pPrimitive       = prim;
    e.OldViewportValid = viewportValid;
    e.OldViewRect      = ViewRect; // TBD: Must assign
    MaskStackTop++;

    HALState |= HS_DrawingMask;

    if (prim->IsClipped() && viewportValid)
    {
        Rect<int> boundClip;

        // Apply new viewport clipping.
        if (!Matrices->OrientationSet)
        {
            const Matrix2F& m = prim->GetMaskAreaMatrix(0).GetMatrix2D();

            // Clipped matrices are always in View coordinate space, to allow
            // matrix to be use for erase operation above. This means that we don't
            // have to do an EncloseTransform.
            SF_ASSERT((m.Shx() == 0.0f) && (m.Shy() == 0.0f));
            boundClip = Rect<int>(VP.Left + (int)m.Tx(), VP.Top + (int)m.Ty(),
                                  VP.Left + (int)(m.Tx() + m.Sx()), VP.Top + (int)(m.Ty() + m.Sy()));
        }
        else
        {
            Matrix2F m = prim->GetMaskAreaMatrix(0).GetMatrix2D();
            m.Append(Matrices->Orient2D);

            RectF rect = m.EncloseTransform(RectF(0,0,1,1));
            boundClip = Rect<int>(VP.Left + (int)rect.x1, VP.Top + (int)rect.y1,
                                  VP.Left + (int)rect.x2, VP.Top + (int)rect.y2);
        }

        if (!ViewRect.IntersectRect(&ViewRect, boundClip))
        {
            ViewRect.Clear();
            HALState &= ~HS_ViewValid;
            viewportValid = false;
        }
        updateViewport();
        
        // Clear full viewport area, which has been resized to our smaller bounds.
        if ((MaskStackTop == 1) && viewportValid)
        {
            glClearStencil(0);
            glClear(GL_STENCIL_BUFFER_BIT);
        }
    }
    else
        if ((MaskStackTop == 1) && viewportValid)
    {
        glStencilFunc(GL_ALWAYS, 0, 0xff);
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

        drawMaskClearRectangles(prim->GetMaskAreaMatrices(), prim->GetMaskCount());

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    }

    if (MultiBitStencil)
    {
        glStencilFunc(GL_EQUAL, MaskStackTop-1, 0xff);
        glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    }
    else if (MaskStackTop == 1)
    {
        glStencilFunc(GL_ALWAYS, 1, 0xff);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    }
    ++AccumulatedStats.Masks;
}


void HAL::EndMaskSubmit()
{
    Profiler.SetDrawMode(0);

    if (!checkState(HS_InDisplay|HS_DrawingMask, __FUNCTION__))
        return;

    HALState &= ~HS_DrawingMask;    
    SF_ASSERT(MaskStackTop);

    glColorMask(1,1,1,1);
    glStencilFunc(GL_LEQUAL, MaskStackTop, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}


void HAL::PopMask()
{
    if (!checkState(HS_InDisplay, __FUNCTION__))
        return;

    SF_ASSERT(MaskStackTop);
    MaskStackTop--;

    if (MaskStack[MaskStackTop].pPrimitive->IsClipped())
    {
        // Restore viewport
        ViewRect      = MaskStack[MaskStackTop].OldViewRect;

        if (MaskStack[MaskStackTop].OldViewportValid)
            HALState |= HS_ViewValid;
        else
            HALState &= ~HS_ViewValid;
        updateViewport();
    }

    if (MaskStackTop == 0)
        glDisable(GL_STENCIL_TEST);
    else
        glStencilFunc(GL_LEQUAL, MaskStackTop, 0xff);
}


void HAL::drawMaskClearRectangles(const HMatrix* matrices, UPInt count)
{
    // This operation is used to clear bounds for masks.
    // Potential issue: Since our bounds are exact, right/bottom pixels may not
    // be drawn due to HW fill rules.
    //  - This shouldn't matter if a mask is tessellated within bounds of those
    //    coordinates, since same rules are applied to those render shapes.
    //  - EdgeAA must be turned off for masks, as that would extrude the bounds.

    unsigned fillflags = 0;
    const ShaderManager::Shader& pso = SManager.SetFill(PrimFill_SolidColor, fillflags, PrimitiveBatch::DP_Batch, &VertexXY16iInstance::Format, &ShaderData);

    unsigned drawRangeCount = 0;
    for (UPInt i = 0; i < count; i+= (UPInt)drawRangeCount)
    {
        drawRangeCount = Alg::Min<unsigned>((unsigned)count, SF_RENDER_MAX_BATCHES);

        for (unsigned j = 0; j < drawRangeCount; j++)
        {
            ShaderData.SetMatrix(pso, Uniform::SU_mvp, Matrix2F::Identity, matrices[i+j], Matrices, 0, j);
        }
        ShaderData.Finish(drawRangeCount);

        SetVertexArray(&VertexXY16iInstance::Format, Cache.MaskEraseBatchVertexBuffer, 0);
        drawPrimitive(drawRangeCount * 6, drawRangeCount);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



//--------------------------------------------------------------------
// *** BlendMode Stack support
//--------------------------------------------------------------------

void HAL::applyBlendModeImpl(BlendMode mode, bool sourceAc, bool forceAc)
{    
    static const UInt32 BlendOps[BlendOp_Count] = 
    {
        GL_FUNC_ADD,                // BlendOp_ADD
        GL_MAX,                     // BlendOp_MAX
        GL_MIN,                     // BlendOp_MIN
        GL_FUNC_REVERSE_SUBTRACT,   // BlendOp_REVSUBTRACT
    };

    static const UInt32 BlendFactors[BlendFactor_Count] = 
    {
        GL_ZERO,                // BlendFactor_ZERO
        GL_ONE,                 // BlendFactor_ONE
        GL_SRC_ALPHA,           // BlendFactor_SRCALPHA
        GL_ONE_MINUS_SRC_ALPHA, // BlendFactor_INVSRCALPHA
        GL_DST_COLOR,           // BlendFactor_DESTCOLOR
    };

    GLenum sourceColor = BlendFactors[BlendModeTable[mode].SourceColor];
    if ( sourceAc && sourceColor == GL_SRC_ALPHA )
        sourceColor = GL_ONE;

    if (VP.Flags & Viewport::View_AlphaComposite || forceAc)
    {
        glBlendFuncSeparate(sourceColor, BlendFactors[BlendModeTable[mode].DestColor], 
            BlendFactors[BlendModeTable[mode].SourceAlpha], BlendFactors[BlendModeTable[mode].DestAlpha]);
    }
    else
    {
        glBlendFunc(sourceColor, BlendFactors[BlendModeTable[mode].DestColor]);
    }
    glBlendEquation(BlendOps[BlendModeTable[mode].Operator]);
}

RenderTarget* HAL::CreateRenderTarget(GLuint fbo)
{
    GLint currentFbo;
    RenderTarget* prt = pRenderBufferManager->CreateRenderTarget(getFboInfo(fbo, currentFbo, false), RBuffer_User, Image_R8G8B8A8, 0);
    if ( !prt )
        return 0;
    if ( prt->GetRenderTargetData() != 0 )
        return prt;

    RenderTargetData::UpdateData(prt, this, fbo, 0);
    return prt;
}

RenderTarget* HAL::CreateRenderTarget(Render::Texture* texture, bool needsStencil)
{
    GL::Texture* pt = (GL::Texture*)texture;

    if ( !pt || pt->TextureCount != 1 )
        return 0;

    GLuint fboID = 0, dsbID = 0;
    RenderTarget* prt = pRenderBufferManager->CreateRenderTarget(
        texture->GetSize(), RBuffer_Texture, texture->GetFormat(), texture);
    if ( !prt )
        return 0;
    Ptr<DepthStencilBuffer> pdsb;

    // Cannot render to textures which have multiple HW representations.
    SF_ASSERT(pt->TextureCount == 1); 
    GLuint colorID = pt->pTextures[0].TexId;

    glGenFramebuffersEXT(1, &fboID);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
    ++AccumulatedStats.RTChanges;

#if defined(GL_ES_VERSION_2_0)
    // If on GLES2, and it has NPOT limitations, then we need to ensure that the texture
    // uses clamping mode without mipmapping, otherwise the glCheckFramebufferStatus will 
    // return that the target is unsupported.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
#endif
    
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorID, 0);
    if ( needsStencil )
    {
        pdsb = *pRenderBufferManager->CreateDepthStencilBuffer(texture->GetSize());
        if ( pdsb )
        {
            DepthStencilSurface* surf = (GL::DepthStencilSurface*)pdsb->GetSurface();
            if ( surf )
            {
                dsbID = surf->RenderBufferID;
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, dsbID);
            }
        }
    }

    RenderTargetData::UpdateData(prt, this, fboID, pdsb);
    return prt;
}

RenderTarget* HAL::CreateTempRenderTarget(const ImageSize& size, bool needsStencil)
{
    RenderTarget* prt = pRenderBufferManager->CreateTempRenderTarget(size);
    if ( !prt )
        return 0;
    Texture* pt = (Texture*)prt->GetTexture();
    if ( !pt )
        return 0;

    RenderTargetData* phd = (RenderTargetData*)prt->GetRenderTargetData();
    if ( phd && (!needsStencil || phd->pDepthStencilBuffer != 0 ))
        return prt;

    Ptr<DepthStencilBuffer> pdsb = 0;
    GLuint dsbID = 0;
    if ( needsStencil )
    {
        pdsb = *pRenderBufferManager->CreateDepthStencilBuffer(size);
        DepthStencilSurface* pdss = (DepthStencilSurface*)pdsb->GetSurface();
        dsbID = pdss->RenderBufferID;
    }

    // If only a new depth stencil is required.
    GLuint fboID;
    GLuint colorID = pt->pTextures[0].TexId;

    if ( phd )
        fboID = phd->FBOID;
    else
        glGenFramebuffersEXT(1, &fboID);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
    ++AccumulatedStats.RTChanges;

#if defined(GL_ES_VERSION_2_0)
    // If on GLES2, and it has NPOT limitations, then we need to ensure that the texture
    // uses clamping mode without mipmapping, otherwise the glCheckFramebufferStatus will 
    // return that the target is unsupported.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
#endif
    
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorID, 0);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, dsbID);

    // Some devices require that the depth buffer be attached, even if we don't use it.
    if (GL::DepthStencilSurface::CurrentFormatHasDepth())
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, dsbID);

    // If this check fails, it means that the stencil format and color format are incompatible.
    // In this case, we will need to try another depth stencil format combination.
    GLenum framebufferStatusError;
    while ((framebufferStatusError = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)) != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        SF_DEBUG_WARNING1(1,"glCheckFramebufferStatusEXT failed (code=0x%x)", framebufferStatusError);
        pdsb = *pRenderBufferManager->CreateDepthStencilBuffer(size);
        DepthStencilSurface* pdss = (DepthStencilSurface*)pdsb->GetSurface();
        dsbID = pdss->RenderBufferID;        
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, dsbID);

        // Some devices require that the depth buffer be attached, even if we don't use it. If it was previously attached,
        // and now our format does not have depth, we must remove it.
        if (GL::DepthStencilSurface::CurrentFormatHasDepth())
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, dsbID);
        else
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

        if (!GL::DepthStencilSurface::SetNextGLFormatIndex())
        {
            SF_DEBUG_WARNING(1, "No compatible depth stencil formats available. Masking in filter will be disabled");
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
            pdsb = 0;
            break;
        }
    }

    RenderTargetData::UpdateData(prt, this, fboID, pdsb);
    return prt;
}

bool HAL::SetRenderTarget(RenderTarget* ptarget, bool setState)
{
    // Cannot set the bottom level render target if already in display.
    if ( HALState & HS_InDisplay )
        return false;

    // When changing the render target while in a scene, we must flush all drawing.
    if ( HALState & HS_InScene)
        Flush();

    RenderTargetEntry entry;
    if ( setState )
    {
        RenderTargetData* phd = (RenderTargetData*)ptarget->GetRenderTargetData();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, phd->FBOID);
    }

    entry.pRenderTarget = ptarget;

    // Replace the stack entry at the bottom, or if the stack is empty, add one.
    if ( RenderTargetStack.GetSize() > 0 )
        RenderTargetStack[0] = entry;
    else
        RenderTargetStack.PushBack(entry);
    return true;   
}

void HAL::PushRenderTarget(const RectF& frameRect, RenderTarget* prt, unsigned flags)
{
    // Setup the render target/depth stencil on the device.
    HALState |= HS_InRenderTarget;
    RenderTargetEntry entry;
    entry.pRenderTarget = prt;
    entry.OldViewport = VP;
    entry.OldViewRect = ViewRect;
    entry.OldMatrixState.CopyFrom(Matrices);
    Matrices->Orient2D.SetIdentity();
    Matrices->Orient3D.SetIdentity();
    Matrices->SetUserMatrix(Matrix2F::Identity);
    // VP.Flags &= ~Viewport::View_Orientation_Mask;

    // Setup the render target/depth stencil.
    if ( !prt )
    {
        SF_DEBUG_WARNING(1, "HAL::PushRenderTarget - invalid render target.");
        RenderTargetStack.PushBack(entry);
        return;
    }
    RenderTargetData* phd = (GL::RenderTargetData*)prt->GetRenderTargetData();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, phd->FBOID);
    ++AccumulatedStats.RTChanges;

    glDisable(GL_SCISSOR_TEST);

    // Clear, if not specifically excluded
    if ( (flags & PRT_NoClear) == 0 )
    {
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // Setup viewport.
    Rect<int> viewRect = prt->GetRect(); // On the render texture, might not be the entire surface.
    const ImageSize& bs = prt->GetBufferSize();
    VP = Viewport(bs.Width, bs.Height, viewRect.x1, viewRect.y1, viewRect.Width(), viewRect.Height());    
    VP.Flags |= Viewport::View_IsRenderTexture;

    ViewRect.x1 = (int)frameRect.x1;
    ViewRect.y1 = (int)frameRect.y1;
    ViewRect.x2 = (int)frameRect.x2;
    ViewRect.y2 = (int)frameRect.y2;

    // Must offset the 'original' viewrect, otherwise the 3D compensation matrix will be offset.
    Matrices->ViewRectOriginal.Offset(-entry.OldViewport.Left, -entry.OldViewport.Top);
    Matrices->UVPOChanged = true;

    HALState |= HS_ViewValid;
    updateViewport();

    RenderTargetStack.PushBack(entry);
}

void HAL::PopRenderTarget(unsigned)
{
    RenderTargetEntry& entry = RenderTargetStack.Back();
    RenderTarget* prt = entry.pRenderTarget;
    prt->SetInUse(false);
    if ( prt->GetType() == RBuffer_Temporary )
    {
        // Strip off the depth stencil surface/buffer from temporary targets.
        GL::RenderTargetData* plasthd = (GL::RenderTargetData*)prt->GetRenderTargetData();
        if ( plasthd->pDepthStencilBuffer )
        {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, plasthd->FBOID);
            ++AccumulatedStats.RTChanges;
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
        }
        plasthd->pDepthStencilBuffer = 0;
    }
    Matrices->CopyFrom(&entry.OldMatrixState);
    ViewRect = entry.OldViewRect;
    VP = entry.OldViewport;

    // Must reverse the offset of the 'original' viewrect.
    Matrices->ViewRectOriginal.Offset(entry.OldViewport.Left, entry.OldViewport.Top);
    Matrices->UVPOChanged = true;

    RenderTargetStack.PopBack();
    RenderTargetData* phd = 0;
    GLuint fboID = 0;
    if ( RenderTargetStack.GetSize() > 0 )
    {
        RenderTargetEntry& back = RenderTargetStack.Back();
        phd = (GL::RenderTargetData*)back.pRenderTarget->GetRenderTargetData();
        fboID = phd->FBOID;
    }

    if ( RenderTargetStack.GetSize() == 1 )
        HALState &= ~HS_InRenderTarget;

    // Restore the old render target.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboID);
    ++AccumulatedStats.RTChanges;

    // Reset the viewport to the last render target on the stack.
    HALState |= HS_ViewValid;
    updateViewport();
}

ImageSize HAL::getFboInfo(GLint fbo, GLint& currentFBO, bool useCurrent)
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &currentFBO);
    if (!useCurrent)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
        ++AccumulatedStats.RTChanges;
    }

    bool validFBO = glIsFramebufferEXT( fbo ) ? true : false;
    GLint width = 0, height = 0;
    GLint type, id;

    if ( validFBO )
    {
        glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT, &type );
        glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT, &id );
        switch(type)
        {
        case GL_TEXTURE:
            {
#ifdef GL_TEXTURE_WIDTH
                glBindTexture(GL_TEXTURE_2D, id );
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );
#endif
                break;
            }
        case GL_RENDERBUFFER_EXT:
            if ( !glIsRenderbufferEXT( id ) )
                break;
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, id);
            glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &width );
            glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_HEIGHT_EXT, &height );
            break;
        }
    }

    if (width == 0 || height == 0)
    {
        // Get the dimensions of the framerect from glViewport.
        GLfloat viewport[4];
        glGetFloatv(GL_VIEWPORT, viewport);
        width = (GLint)viewport[2];
        height = (GLint)viewport[3];
    }

    if (!useCurrent)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, currentFBO);
        ++AccumulatedStats.RTChanges;
    }

    return ImageSize(width, height);
}

bool HAL::createDefaultRenderBuffer()
{
    ImageSize rtSize;

    if ( GetDefaultRenderTarget() )
    {
        RenderTarget* prt = GetDefaultRenderTarget();
        rtSize = prt->GetSize();
    }
    else
    {
        RenderTargetEntry entry;

        GLint currentFBO;
        rtSize = getFboInfo(0, currentFBO, true);

        Ptr<RenderTarget> ptarget = *SF_HEAP_AUTO_NEW(this) RenderTarget(0, RBuffer_Default, rtSize );
        Ptr<DepthStencilBuffer> pdsb = *SF_HEAP_AUTO_NEW(this) DepthStencilBuffer(0, rtSize);
        RenderTargetData::UpdateData(ptarget, this, currentFBO, pdsb);

        if (!SetRenderTarget(ptarget))
            return false;
    }

    return pRenderBufferManager->Initialize(pTextureManager, Image_R8G8B8A8, rtSize );
}

void HAL::PushFilters(FilterPrimitive* prim)
{
    if (!checkState(HS_InDisplay, __FUNCTION__))
        return;

    FilterStackEntry e = {prim, 0};

    // Do not render filters if the profile does not support it (unfiltered content will be rendered).
    if (!shouldRenderFilters(prim))
    {
        FilterStack.PushBack(e);
        return;
    }

    // Queue the profiler off of whether masks should be draw or not.
    if ( !Profiler.ShouldDrawMask() )
    {
        Profiler.SetDrawMode(2);

        unsigned fillflags = 0;
        float colorf[4];
        Profiler.GetColor(0xFFFFFFFF).GetRGBAFloat(colorf);
        const ShaderManager::Shader& pso = SManager.SetFill(PrimFill_SolidColor, fillflags, 
            PrimitiveBatch::DP_Single, &VertexXY16iInstance::Format, &ShaderData);
        Matrix2F mvp(prim->GetFilterAreaMatrix().GetMatrix2D(), Matrices->UserView);
        ShaderData.SetMatrix(pso, Uniform::SU_mvp, mvp);
        ShaderData.SetUniform(pso, Uniform::SU_cxmul, colorf, 4);
        ShaderData.Finish(1);

        SetVertexArray(&VertexXY16iInstance::Format, Cache.MaskEraseBatchVertexBuffer, 0);
        drawPrimitive(6,1);
        FilterStack.PushBack(e);
        return;
    }

    if ( (HALState & HS_CachedFilter) )
    {
        FilterStack.PushBack(e);
        return;
    }

    // Disable masking from previous target, if this filter primitive doesn't have any masking.
    if ( MaskStackTop != 0 && !prim->GetMaskPresent() && prim->GetCacheState() != FilterPrimitive::Cache_Target)
    {
        glDisable(GL_STENCIL_TEST);
    }

    HALState |= HS_DrawingFilter;

    if ( prim->GetCacheState() ==  FilterPrimitive::Cache_Uncached )
    {
        // Draw the filter from scratch.
        const Matrix2F& m = e.pPrimitive->GetFilterAreaMatrix().GetMatrix2D();
            e.pRenderTarget = *CreateTempRenderTarget(ImageSize((UInt32)m.Sx(), (UInt32)m.Sy()), prim->GetMaskPresent());
        RectF frameRect(m.Tx(), m.Ty(), m.Tx() + m.Sx(), m.Ty() + m.Sy());
        PushRenderTarget(frameRect, e.pRenderTarget);
        applyBlendMode(BlendModeStack.GetSize()>=1 ? BlendModeStack.Back() : Blend_Normal, false, true);

        // If this primitive has masking, then clear the entire area to the current mask level, because 
        // the depth stencil target may be different, and thus does not contain the previously written values.
        if ( prim->GetMaskPresent())
        {
            glClearStencil(MaskStackTop);
            glClear(GL_STENCIL_BUFFER_BIT);
        }
    }
    else
    {
        // Drawing a cached filter, ignore all draw calls until the corresponding PopFilters.
        // Keep track of the level at which we need to draw the cached filter, by adding entries to the stack.
        HALState |= HS_CachedFilter;
        CachedFilterIndex = (int)FilterStack.GetSize();
        GetRQProcessor().SetQueueEmitFilter(RenderQueueProcessor::QPF_Filters);
    }
    FilterStack.PushBack(e);
}

void HAL::drawUncachedFilter(const FilterStackEntry& e)
{
    const FilterSet* filters = e.pPrimitive->GetFilters();
    unsigned filterCount = filters->GetFilterCount();
    const Filter* filter = 0;
    unsigned pass = 0, passes = 0;

    // Invalid primitive or rendertarget.
    if ( !e.pPrimitive || !e.pRenderTarget )
        return;

    // Bind the render target.
    SF_ASSERT(RenderTargetStack.Back().pRenderTarget == e.pRenderTarget);
    const unsigned MaxTemporaryTextures = 3;
    Ptr<RenderTarget> temporaryTextures[MaxTemporaryTextures];
    memset(temporaryTextures, 0, sizeof temporaryTextures);

    // Fill our the source target.
    ImageSize size = e.pRenderTarget->GetSize();
    temporaryTextures[0] = e.pRenderTarget;

    glColorMask(1,1,1,1);
    FilterVertexBufferSet = 0;

    glDisable(GL_SCISSOR_TEST);

    // Overlay mode isn't actually supported, it contains the blend mode for filter sub-targets.
    applyBlendMode(Blend_Overlay, true, false);

    // Render filter(s).
    unsigned shaders[ShaderManager::MaximumFilterPasses];
    BlurFilterState LEBlurState (8);

    for ( unsigned i = 0; i < filterCount; ++i )
    {
        filter = filters->GetFilter(i);
        passes = SManager.SetupFilter(filter, FillFlags, shaders, LEBlurState);

        // All shadows (except those hiding the object) need the original texture.
        bool requireSource = false;
        if ( filter->GetFilterType() >= Filter_Shadow &&
             filter->GetFilterType() <= Filter_Blur_End &&
             !(((BlurFilterImpl*)filter)->GetParams().Mode & BlurFilterParams::Mode_HideObject) )
        {
            temporaryTextures[Target_Original] = temporaryTextures[Target_Source];
            requireSource = true;
        }

        // Now actually render the filter.
        for (pass = 0; pass < passes; ++pass)
        {
            // Render the final pass directly to the target surface.
            if (pass == passes-1 && i == filterCount-1)
                break;

            // Create a destination texture if required.
            if ( !temporaryTextures[1] )
            {
                temporaryTextures[1] = *CreateTempRenderTarget(size, false);
            }

            GLuint fboid = ((GL::RenderTargetData*)temporaryTextures[1]->GetRenderTargetData())->FBOID;
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboid );
            ++AccumulatedStats.RTChanges;
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT);
            
            // Scale to the size of the destination.
            RenderTarget* prt = temporaryTextures[1];
            const Rect<int>& viewRect = prt->GetRect(); // On the render texture, might not be the entire surface.
            const ImageSize& bs = prt->GetBufferSize();
            VP = Viewport(bs.Width, bs.Height, viewRect.x1, viewRect.y1, viewRect.Width(), viewRect.Height());    
            ViewRect = Rect<int>(viewRect.x1, viewRect.y1, viewRect.x2, viewRect.y2);
            HALState |= HS_ViewValid;
            updateViewport();

            Matrix2F mvp = Matrix2F::Scaling(2,2) * Matrix2F::Translation(-0.5f, -0.5f);                          
            DrawFilter(mvp, Cxform::Identity, filter, temporaryTextures, shaders, pass, passes, 
                MappedXY16iUVTexture[PrimitiveBatch::DP_Single], LEBlurState, false);

            // If we require the original source, create a new target for the source.
            if ( requireSource && pass == 0)
                temporaryTextures[0] = *CreateTempRenderTarget(size, false);

            // Setup for the next pass.
            Alg::Swap(temporaryTextures[0], temporaryTextures[1]);
        }
    }

	if (Profiler.IsFilterCachingEnabled() && temporaryTextures[Target_Source])
	{
	    // Cache the 2nd last step so it might be available as a cached filter next time.
		RenderTarget* cacheResults[2] = { temporaryTextures[0], temporaryTextures[2] };
		e.pPrimitive->SetCacheResults(FilterPrimitive::Cache_PreTarget, cacheResults, 2);
		((GL::RenderTargetData*)cacheResults[0]->GetRenderTargetData())->CacheID = reinterpret_cast<UPInt>(e.pPrimitive.GetPtr());
		if ( cacheResults[1] )
			((GL::RenderTargetData*)cacheResults[1]->GetRenderTargetData())->CacheID = reinterpret_cast<UPInt>(e.pPrimitive.GetPtr());
	}

    // Pop the temporary target, begin rendering to the previous surface.
    PopRenderTarget();

    // Re-[en/dis]able masking from previous target, if available.
    if ( MaskStackTop != 0 )
        glEnable(GL_STENCIL_TEST);

    // Now actually draw the filtered sub-scene to the target below.
    const Matrix2F& mvp = Matrices->UserView * e.pPrimitive->GetFilterAreaMatrix().GetMatrix2D();
    const Cxform&   cx  = e.pPrimitive->GetFilterAreaMatrix().GetCxform();
    applyBlendMode(BlendModeStack.GetSize()>=1 ? BlendModeStack.Back() : Blend_Normal, true, true);
    DrawFilter(mvp, cx, filter, temporaryTextures, shaders, pass, passes, MappedXY16iUVTexture[PrimitiveBatch::DP_Single], LEBlurState, true);
    applyBlendMode(BlendModeStack.GetSize()>=1 ? BlendModeStack.Back() : Blend_Normal, false, (HALState&HS_InRenderTarget) != 0);

    if ( HALState & HS_DrawingMask )
        glColorMask(0,0,0,0);

    // Cleanup.
    for ( unsigned i = 0; i < MaxTemporaryTextures; ++i )
    {
        if (temporaryTextures[i])
            temporaryTextures[i]->SetInUse(false);
    }
    AccumulatedStats.Filters += filters->GetFilterCount();

}

void HAL::drawCachedFilter(FilterPrimitive* primitive)
{
    FilterVertexBufferSet = 0;
    BlurFilterState LEBlurState (8);

    const unsigned MaxTemporaryTextures = 3;
    switch(primitive->GetCacheState())
    {
        // We have one-step from final target. Render it to a final target now.
        case FilterPrimitive::Cache_PreTarget:
        {
            const FilterSet* filters = primitive->GetFilters();
            UPInt filterIndex = filters->GetFilterCount()-1;
            const Filter* filter = filters->GetFilter(filterIndex);
            unsigned shaders[ShaderManager::MaximumFilterPasses];

            unsigned passes = SManager.SetupFilter(filter, FillFlags, shaders, LEBlurState);
            
            // Fill out the temporary textures from the cached results.
            Ptr<RenderTarget> temporaryTextures[MaxTemporaryTextures];
            memset(temporaryTextures, 0, sizeof temporaryTextures);
            RenderTarget* results[2];
            primitive->GetCacheResults(results, 2);
            temporaryTextures[0] = results[0];
            ImageSize size = temporaryTextures[0]->GetSize();
            temporaryTextures[1] = *CreateTempRenderTarget(size, false);
            temporaryTextures[2] = results[1];
            PushRenderTarget(RectF((float)size.Width,(float)size.Height), temporaryTextures[1]);

            // Render to the target.
            Matrix2F mvp = Matrix2F::Scaling(2,2) * Matrix2F::Translation(-0.5f, -0.5f);
            const Cxform & cx = primitive->GetFilterAreaMatrix().GetCxform();

            applyBlendMode(BlendModeStack.GetSize()>=1 ? BlendModeStack.Back() : Blend_Normal, true, true);
            DrawFilter(mvp, cx, filter, temporaryTextures, shaders, passes-1, passes, 
                MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single], LEBlurState, true);

            PopRenderTarget();
            
            // Re-[en/dis]able masking from previous target, if available.
            if ( MaskStackTop != 0 )
                glEnable(GL_STENCIL_TEST);

            // Re-[en/dis]able masking from previous target, if available.
            if ( HALState & HS_DrawingMask )
                glColorMask(0,0,0,0);

            // Set this as the final cache result, and then render it.
            RenderTarget* prt = temporaryTextures[1];
            primitive->SetCacheResults(FilterPrimitive::Cache_Target, &prt, 1);
            ((GL::RenderTargetData*)prt->GetRenderTargetData())->CacheID = reinterpret_cast<UPInt>(primitive);
            drawCachedFilter(primitive);

            // Cleanup.
            for ( unsigned i = 0; i < MaxTemporaryTextures; ++i )
            {
                if (temporaryTextures[i])
                    temporaryTextures[i]->SetInUse(false);
            }
            break;
        }

        // We have a final filtered texture. Just apply it to a screen quad.
        case FilterPrimitive::Cache_Target:
        {
            unsigned fillFlags = (FillFlags & ~FF_Cxform);
            const ShaderManager::Shader& pso = SManager.SetFill(PrimFill_Texture, fillFlags, PrimitiveBatch::DP_Single, 
                MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single], &ShaderData);

            RenderTarget* results;
            primitive->GetCacheResults(&results, 1);
            Texture* ptexture = (GL::Texture*)results->GetTexture();
            const Matrix2F& mvp = Matrices->UserView * primitive->GetFilterAreaMatrix().GetMatrix2D();
            const Rect<int>& srect = results->GetRect();
            Matrix2F texgen;
            texgen.AppendTranslation((float)srect.x1, (float)srect.y1);
            texgen.AppendScaling((float)srect.Width() / ptexture->GetSize().Width, (float)srect.Height() / ptexture->GetSize().Height);

            ShaderData.SetUniform(pso, Uniform::SU_mvp, &mvp.M[0][0], 8 );
            ShaderData.SetUniform(pso, Uniform::SU_texgen, &texgen.M[0][0], 8 );
            ShaderData.SetTexture(pso, Uniform::SU_tex, ptexture, ImageFillMode(Wrap_Clamp, Sample_Linear));
            ShaderData.Finish();

            if (!FilterVertexBufferSet)
            {
                SetVertexArray(&VertexXY16iAlpha::Format, Cache.MaskEraseBatchVertexBuffer, 0);
                FilterVertexBufferSet = 1;
            }

            applyBlendMode(BlendModeStack.GetSize()>=1 ? BlendModeStack.Back() : Blend_Normal, true, true);
            drawPrimitive(6,1);
            applyBlendMode(BlendModeStack.GetSize()>=1 ? BlendModeStack.Back() : Blend_Normal, false, (HALState&HS_InRenderTarget)!=0);

            // Cleanup.
            results->SetInUse(false);
            if ( !Profiler.IsFilterCachingEnabled() )
                primitive->SetCacheResults(FilterPrimitive::Cache_Uncached, 0, 0);
            break;
        }

        // Should have been one of the other two caching types.
        default: SF_ASSERT(0); break;
    }
}

void HAL::DrawableCxform( Render::Texture** tex, const Matrix2F* texgen, const Cxform* cx)
{
    ScopedRenderEvent GPUEvent(GetEvent(Event_DICxform), __FUNCTION__);
    SManager.SetDrawableCxform(tex, texgen, RenderTargetStack.Back().pRenderTarget->GetSize(), cx, 
        MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single], &ShaderData, ShaderManager::CPF_InvertedViewport );
    drawScreenQuad();
}

void HAL::DrawableCompare( Render::Texture** tex, const Matrix2F* texgen )
{
    ScopedRenderEvent GPUEvent(GetEvent(Event_DICompare), __FUNCTION__);
    SManager.SetDrawableCompare(tex, texgen, RenderTargetStack.Back().pRenderTarget->GetSize(), 
        MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single], &ShaderData, ShaderManager::CPF_InvertedViewport );
    drawScreenQuad();
}

void HAL::DrawableCopyChannel( Render::Texture** tex, const Matrix2F* texgen, const Matrix4F* cxmul ) 
{
    ScopedRenderEvent GPUEvent(GetEvent(Event_DIMerge), __FUNCTION__);
    SManager.SetDrawableMergeFill(tex, texgen, RenderTargetStack.Back().pRenderTarget->GetSize(), 
        cxmul, MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single], &ShaderData, ShaderManager::CPF_InvertedViewport);

    drawScreenQuad();
}

void HAL::DrawableMerge( Render::Texture** tex, const Matrix2F* texgen, const Matrix4F* cxmul )
{
    ScopedRenderEvent GPUEvent(GetEvent(Event_DIMerge), __FUNCTION__);
    SManager.SetDrawableMergeFill(tex, texgen, RenderTargetStack.Back().pRenderTarget->GetSize(), 
        cxmul, MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single], &ShaderData, ShaderManager::CPF_InvertedViewport );

    drawScreenQuad();
}

void HAL::DrawableCopyPixels( Render::Texture** tex, const Matrix2F* texgen, const Matrix2F& mvp, bool mergeAlpha, bool destAlpha )
{
    ScopedRenderEvent GPUEvent(GetEvent(Event_DICopyPixels), __FUNCTION__);
    SManager.SetDrawableCopyPixelsFill(tex, texgen, RenderTargetStack.Back().pRenderTarget->GetSize(), mvp,
        mergeAlpha, destAlpha, MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single], &ShaderData, ShaderManager::CPF_InvertedViewport );

    drawScreenQuad();
}

void HAL::DrawablePaletteMap( Render::Texture** tex, const Matrix2F* texgen, const Matrix2F& mvp, unsigned channelMask, const UInt32* values)
{
    ScopedRenderEvent GPUEvent(GetEvent(Event_DIPaletteMap), __FUNCTION__);

    // Create a temporary texture with the palette map. There may be a more efficient way to do this; however, using
    // uniforms seems unworkable, due to shader constant slot constraints.
    ImageData data;
    Ptr<Render::Texture> ptex = *pTextureManager->CreateTexture(Image_B8G8R8A8, 1, ImageSize(256, 4), ImageUse_Map_Mask, 0);
    if ( !ptex->Map(&data, 0, 1) )
        return;
    for ( int channel = 0; channel < 4; ++channel )
    {
        UInt32* dataPtr = (UInt32*)data.GetScanline(channel);
        if ( channelMask & (1<<channel))
        {
            memcpy(dataPtr, values + channel*256, 256*sizeof(UInt32));
        }
        else
        {
            // Channel was not provided, just do a straight mapping.
            for ( unsigned i = 0; i < 256; ++i )
                *dataPtr++ = (i << (channel*8));
        }
    }
    if (!ptex->Unmap())
        return;

    // First pass overwrites everything.
    applyBlendMode(Blend_OverwriteAll, true, true);
    SManager.SetDrawablePaletteMap(tex, texgen, RenderTargetStack.Back().pRenderTarget->GetSize(), mvp,
        ptex, MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single], &ShaderData, ShaderManager::CPF_InvertedViewport );

    drawScreenQuad();
}

void HAL::DrawableThreshold(Render::Texture** tex, const Matrix2F* texgen, const Matrix2F& mvp, DrawableImage::OperationType op, 
                            UInt32 threshold, UInt32 color, UInt32 mask, bool copySource)
{
    ScopedRenderEvent GPUEvent(GetEvent(Event_DIThreshold), __FUNCTION__);

    SManager.SetDrawableThreshold(tex, texgen, RenderTargetStack.Back().pRenderTarget->GetSize(), mvp,
        op, threshold, color, mask, copySource, MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single], 
        &ShaderData, ShaderManager::CPF_InvertedViewport );

    drawScreenQuad();
}

void HAL::DrawableCopyback( Render::Texture* source, const Matrix2F& mvp, const Matrix2F& texgen )
{
    ScopedRenderEvent GPUEvent(GetEvent(Event_DICopyback), __FUNCTION__);

    // Set shader constants.
    unsigned fillFlags = 0;
    const ShaderManager::Shader& pso = SManager.SetFill(PrimFill_Texture, fillFlags, PrimitiveBatch::DP_Single, 
        MappedXY16iAlphaTexture[PrimitiveBatch::DP_Single], &ShaderData);    

    ShaderData.SetMatrix(pso,  Uniform::SU_mvp,    mvp);
    ShaderData.SetMatrix(pso,  Uniform::SU_texgen, texgen);
    ShaderData.SetTexture(pso, Uniform::SU_tex,    source, ImageFillMode());
    ShaderData.Finish(1);

    drawScreenQuad();
}

//--------------------------------------------------------------------
#if defined(GL_ES_VERSION_2_0)
#include "Render/GL/GLES_ExtensionMacros.h"
#else
#include "Render/GL/GL_ExtensionMacros.h"
#endif

RenderTargetData::~RenderTargetData()
{
    if (pBuffer->GetType() != RBuffer_Default && pBuffer->GetType() != RBuffer_User)
    {
        GL::TextureManager* pmgr = (GL::TextureManager*)pHAL->GetTextureManager();

        // If the texture manager isn't present, just try deleting it immediately.
        if (!pmgr)
            glDeleteFramebuffersEXT(1, &FBOID);
        else
            pmgr->DestroyFBO(FBOID);
    }

}

class VertexBuilder
{
public:
    UByte*    vertexOffset;
    unsigned  Size;
    HAL*      pHal;

    VertexBuilder(HAL* phal, UByte* v, unsigned s) : vertexOffset(v), Size(s), pHal(phal) {}
    HAL* GetHAL() { return pHal; }

    void Add(int vi, int attr, int ac, int offset)
    {
        GLenum vet; bool norm;

        switch (attr & VET_CompType_Mask)
        {
        case VET_U8:  vet = GL_UNSIGNED_BYTE; norm = false; break;
        case VET_U8N: vet = GL_UNSIGNED_BYTE; norm = true; break;
        case VET_U16: vet = GL_UNSIGNED_SHORT; norm = false; break;
        case VET_S16: vet = GL_SHORT; norm = false; break;
        case VET_U32: vet = GL_UNSIGNED_INT; norm = false; break;
        case VET_F32: vet = GL_FLOAT; norm = false;  break;

        // Instance indices are not used in the vertex arrays, so just ignore them.
        case VET_I8:
        case VET_I16:
            return;

        default: SF_ASSERT(0); vet = GL_FLOAT; norm = false; break;
        }

        if ( pHal->EnabledVertexArrays < vi )
        {
            glEnableVertexAttribArray(vi);
            pHal->EnabledVertexArrays++;
        }
        glVertexAttribPointer(vi, ac, vet, norm, Size, vertexOffset + offset);
    }

    void Finish(int vi)
    {
        int newEnabledCount = vi-1;
        for (int i = vi; i < pHal->EnabledVertexArrays; i++)
        {
            glDisableVertexAttribArray(i);
        }
        pHal->EnabledVertexArrays = newEnabledCount;
    }
};

bool HAL::SetVertexArray(const VertexFormat* pFormat, GLuint buffer, UByte* vertexOffset)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    VertexBuilder vb (this, vertexOffset, pFormat->Size);
    BuildVertexArray(pFormat, vb);

    return true;
}

const ShaderObject* HAL::GetStaticShader( FragShaderDesc::ShaderType shaderType )
{
    unsigned comboIndex = FragShaderDesc::GetShaderComboIndex(shaderType);
    SF_DEBUG_ASSERT(VertexShaderDesc::GetShaderComboIndex((VertexShaderDesc::ShaderType)shaderType) == comboIndex,
        "Expected ComboIndex for both vertex and fragment shaders to be equivalent.");
    if ( comboIndex >= UniqueShaderCombinations )
        return 0;

    ShaderObject* shader = &StaticShaders[comboIndex];

    // Initialize the shader if it hasn't already been initialized.
    if ( (VMCFlags & HALConfig_DynamicShaderCompile) && shader->Prog == 0 )
    {
        if ( !shader->Init(this, (VertexShaderType)shaderType, shaderType))
            return 0;
    }
    return shader;
}

bool HAL::initializeShaders(bool force)
{
#ifdef SF_GL_BINARY_SHADERS
    if ((Caps & Cap_BinaryShaders) && BinaryShaderPath.GetLength())
    {
        String shpath = BinaryShaderPath + "GFxShaders.cache";
        Ptr<File>  pfile = *SF_NEW SysFile(shpath);

        //SF_DEBUG_MESSAGE1(1, "Shader binary file is %d bytes", pfile->GetLength());
        if (pfile)
        {
            char header[10];
            if (pfile->Read((UByte*)header, 10) < 10 || strncmp(header, "GFxShaders", 10))
                goto noBinary;

            SInt64 version = pfile->ReadSInt64();
            SF_DEBUG_WARNING2(version != SF_GFXSHADERMAKER_TIMESTAMP, "Binary shaders timestamps do not match executable. "
                "Using source shaders (bin=%lld, exe=%lld)", version, SF_GFXSHADERMAKER_TIMESTAMP);
            if ( version != SF_GFXSHADERMAKER_TIMESTAMP )
                goto noBinary;

            GLsizei bufferSize = 0;
            void* buffer = 0;

            SInt32 count = pfile->ReadSInt32();
            for (int i = 0; i < count; i++)
            {
                SInt32 comboIndex = pfile->ReadSInt32();
                //SF_DEBUG_MESSAGE1(1, "Loading binary shader (comboIndex=%d)", comboIndex);

                // Find the shader type that goes with this combination.
                if (!StaticShaders[comboIndex].InitBinary(comboIndex, pfile, buffer, bufferSize))
                {
                    SF_FREE(buffer);
                    SF_DEBUG_WARNING(1, "Error loading some binary shaders. Using source.");

                    // Kill all previously loaded binary shaders, they are likely bogus.
                    for ( unsigned ci = 0; ci < UniqueShaderCombinations; ++ci)
                        StaticShaders[ci].Shutdown();

                    goto noBinary;
                }                    
            }
            SF_FREE(buffer);
            return true;
        }
    }
    noBinary:
#endif


    if ( (VMCFlags & HALConfig_DynamicShaderCompile) == 0 || force)
    {
        for (unsigned i = 0; i < UniqueShaderCombinations; i++)
        {
            if ( StaticShaders[i].Prog )
                continue;

            // Find the shader type that goes with this combination.
            FragShaderDesc::ShaderIndex fsIndex = FragShaderDesc::GetShaderIndexForComboIndex(i);
            VertexShaderDesc::ShaderIndex vsIndex = VertexShaderDesc::GetShaderIndexForComboIndex(i);

            if (!FragShaderDesc::Descs[fsIndex] || !VertexShaderDesc::Descs[vsIndex] )
                continue;

            // If the platform does not support dynamic loops, do not initialize shaders that use them.
            if ((FragShaderDesc::Descs[fsIndex]->Flags & Shader_DynamicLoop) && (Caps & Cap_NoDynamicLoops))
                continue;

            // If the platform doesn't support instancing, do not initialize shaders that use it.
            if ( (FragShaderDesc::Descs[fsIndex]->Flags & Shader_Instanced) && !SManager.HasInstancingSupport() )
                continue;

            if ( !StaticShaders[i].Init(this, VertexShaderDesc::Descs[vsIndex]->Type, FragShaderDesc::Descs[fsIndex]->Type))
                return false;
        }
    }

#ifdef SF_GL_BINARY_SHADERS
    if ((Caps & Cap_BinaryShaders) && BinaryShaderPath.GetLength())
    {
        String shpath = BinaryShaderPath + "GFxShaders.cache";
        Ptr<File>  pfile = *SF_NEW SysFile(shpath, File::Open_Write|File::Open_Create|File::Open_Truncate);

        if (pfile->IsValid())
        {
            GLsizei totalSize = 0;
            GLsizei bufferSize = 0;
            void* buffer = 0;

            pfile->Write((const UByte*) "GFxShaders", 10);
            pfile->WriteSInt64(SF_GFXSHADERMAKER_TIMESTAMP);
            totalSize += 10 + 8;

            SInt32 count = 0;
            for (unsigned i = 0; i < UniqueShaderCombinations; i++)
                if (StaticShaders[i].Prog)
                    count++;

            pfile->WriteSInt32(count);
            totalSize += 4;

            for (unsigned i = 0; i < UniqueShaderCombinations; i++)
            {
                if (StaticShaders[i].Prog)
                {
                    pfile->WriteSInt32(i);
                    totalSize += 4;
                    //SF_DEBUG_MESSAGE1(1, "Saving binary shader (comboIndex=%d)", i);
                    if (!StaticShaders[i].SaveBinary(pfile, buffer, bufferSize, totalSize))
                    {
                        SF_DEBUG_WARNING1(1, "Error saving shader (comboIndex=%d)", i);
                        pfile->Close();
                        unlink(shpath.ToCStr());
                        break;
                    }
                }
            }

            //SF_DEBUG_MESSAGE1(1, "Total bytes written to shader cache file: %d", totalSize);
            pfile->Close();
        }
        else
            SF_DEBUG_WARNING2(1, "Error creating binary shader cache %s: %d", shpath.ToCStr(), pfile->GetErrorCode());
    }
#endif

    return true;
}

bool HAL::shouldRenderFilters(const FilterPrimitive*) const
{
    return true;
}

// Simply sets a quad vertex buffer and draws.
void HAL::drawScreenQuad()
{
    // Set the vertices, and Draw
    SetVertexArray(&VertexXY16iInstance::Format, Cache.MaskEraseBatchVertexBuffer, 0);
    drawPrimitive(6,1);
}

void    HAL::DrawFilter(const Matrix2F& mvp, const Cxform & cx, const Filter* filter, Ptr<RenderTarget> * targets, 
                        unsigned* shaders, unsigned pass, unsigned passCount, const VertexFormat* pvf, 
                        BlurFilterState& leBlur, bool isLastPass)
{
    if (leBlur.Passes > 0)
    {
        leBlur.SetPass(pass);

        const BlurFilterShader* pShader = ShaderData.GetBlurShader(leBlur);
        if (!pShader)
            return;

        Rect<int> srcrect = targets[Target_Source]->GetRect();
        Rect<int> destrect = Rect<int>(0,0,1,1);

        glUseProgram(pShader->Shader);

        if (!isLastPass)
        {
            BlendEnable = 1;
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glBlendEquation(GL_FUNC_ADD);
        }
        // else do nothing, dest blend mode was set before calling DrawFilter

        glUniform4fv(pShader->mvp, 2, &mvp.M[0][0]);

        if (pShader->cxadd >= 0)
        {
            float cxform[2][4];
            cx.GetAsFloat2x4(cxform);
            glUniform4fv(pShader->cxmul, 1, cxform[0]);
            glUniform4fv(pShader->cxadd, 1, cxform[1]);
        }

        for (int i = 0; i < 2; i++)
            if (pShader->scolor[i] >= 0)
            {
                float color[4];
                leBlur.CurPass->Colors[i].GetRGBAFloat(color);
                glUniform4fv(pShader->scolor[i], 1, color);
            }

        if (pShader->samples >= 0)
        {
            glUniform1f(pShader->samples, 1.0f/leBlur.Samples);
        }

        if (pShader->tex[1] >= 0)
        {
            GL::Texture *ptexture = (GL::Texture*) targets[Target_Original]->GetTexture();
            GL::TextureManager* pmanager = (GL::TextureManager*)ptexture->GetTextureManager();
            pmanager->ApplyTexture(1, ptexture->pTextures[0].TexId);
            glUniform1i(pShader->tex[1], 1);
            glUniform2f(pShader->texscale[1], 1.0f/ptexture->GetSize().Width, 1.0f/ptexture->GetSize().Height);
        }

        GL::Texture *ptexture = (GL::Texture*) targets[Target_Source]->GetTexture();
        GL::TextureManager* pmanager = (GL::TextureManager*)ptexture->GetTextureManager();
        pmanager->ApplyTexture(0, ptexture->pTextures[0].TexId);
        glUniform1i(pShader->tex[0], 0);
        glUniform2f(pShader->texscale[0], 1.0f/ptexture->GetSize().Width, 1.0f/ptexture->GetSize().Height);

        float* pvertices = (float*) alloca(sizeof(float) * leBlur.GetVertexBufferSize());
        VertexFunc_Buffer vout (pvertices);
        leBlur.GetVertices(srcrect, destrect, vout);
        int vbstride = leBlur.VertexAttrs * 2 * sizeof(float);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        if (EnabledVertexArrays < 0)
            glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, 0, vbstride, pvertices);
        for (int i = 0; i < leBlur.TotalTCs; i++)
        {
            if (EnabledVertexArrays < i + 1)
                glEnableVertexAttribArray(i + 1);
            glVertexAttribPointer(i + 1, 2, GL_FLOAT, 0, vbstride, pvertices+(2+i*2));
        }
        for (int i = leBlur.TotalTCs+2; i < EnabledVertexArrays; i++)
           glDisableVertexAttribArray(i);
        EnabledVertexArrays = leBlur.TotalTCs;

        drawPrimitive(6 * leBlur.Quads, leBlur.Quads);

        FilterVertexBufferSet = 0;
    }
    else
    {
        if (!FilterVertexBufferSet)
        {
            SetVertexArray(&VertexXY16iAlpha::Format, Cache.MaskEraseBatchVertexBuffer, 0);
            FilterVertexBufferSet = 1;
        }

        SManager.SetFilterFill(mvp, cx, filter, targets, shaders, pass, passCount, pvf, &ShaderData);
        drawPrimitive(6,1);
    }
}

void HAL::drawPrimitive(unsigned indexCount, unsigned meshCount)
{
    glDrawArrays(GL_TRIANGLES, 0, indexCount);

    SF_UNUSED(meshCount);
#if !defined(SF_BUILD_SHIPPING)
    AccumulatedStats.Meshes += meshCount;
    AccumulatedStats.Triangles += indexCount / 3;
    AccumulatedStats.Primitives++;
#endif
}

void HAL::drawIndexedPrimitive( unsigned indexCount, unsigned meshCount, UByte* indexPtr)
{
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, indexPtr);

    SF_UNUSED(meshCount);
#if !defined(SF_BUILD_SHIPPING)
    AccumulatedStats.Meshes += meshCount;
    AccumulatedStats.Triangles += indexCount / 3;
    AccumulatedStats.Primitives++;
#endif
}

void HAL::drawIndexedInstanced( unsigned indexCount, unsigned meshCount, UByte* indexPtr)
{
#if !defined (SF_USE_GLES_ANY) && !defined(SF_OS_MAC)
    glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, indexPtr, meshCount);
#else
    SF_DEBUG_ASSERT(0, "Instancing not supported on GLES platforms.");
#endif

#if !defined(SF_BUILD_SHIPPING)
    AccumulatedStats.Meshes += meshCount;
    AccumulatedStats.Triangles += indexCount / 3;
    AccumulatedStats.Primitives++;
#endif
}

Render::RenderEvent& HAL::GetEvent(EventType eventType)
{
#if !defined(SF_BUILD_SHIPPING) && !defined(SF_USE_GLES_ANY)
    static GL::RenderEvent GLEvents[Event_Count];
    static bool EventsInitialized = false;
    if (!EventsInitialized)
    {
        for ( unsigned event = 0; event < Event_Count; ++event)
            GLEvents[event].pHAL = this;
    }
    return GLEvents[eventType];
#else
    // Shipping builds just return a default event, which does nothing.
    return Render::HAL::GetEvent(eventType);
#endif
}

void MatrixState::recalculateUVPOC() const
{
    if (UVPOChanged)
    {
        // Recalculated the view compensation matrix.
        if ( ViewRect != ViewRectOriginal && !ViewRectOriginal.IsNull())
        {
            Point<int> dc = ViewRect.Center() - ViewRectOriginal.Center();
            float      dx = ((float)ViewRectOriginal.Width()) / ViewRect.Width();
            float      dy = ((float)ViewRectOriginal.Height()) / ViewRect.Height();
            float      ox = 2.0f * dc.x / ViewRect.Width();
            float      oy = 2.0f * dc.y / ViewRect.Height();
            ViewRectCompensated3D.MultiplyMatrix(Matrix4F::Translation(-ox, oy, 0), Matrix4F::Scaling(dx, dy, 1));
        }
        else
        {
            ViewRectCompensated3D = Matrix4F::Identity;
        }

        const Matrix4F& Projection = updateStereoProjection();

        Matrix4F flipmat;
        flipmat.SetIdentity();
        if(pHAL && pHAL->RenderTargetStack.GetSize() > 1)
        {
            flipmat.Append(Matrix4F::Scaling(1.0f, -1.0f, 1.0f));
        }
        
        Matrix4F FV(flipmat, ViewRectCompensated3D);
        Matrix4F UO(User3D, FV);
        Matrix4F VRP(Orient3D, Projection);
        UVPO = Matrix4F(Matrix4F(UO, VRP), View3D);
        UVPOChanged = 0;
    }
}

}}} // Scaleform::Render::GL

#endif // !defined(SF_USE_GLES)
