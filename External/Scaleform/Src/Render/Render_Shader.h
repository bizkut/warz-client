/**************************************************************************

PublicHeader:   Render
Filename    :   Render_Shader.h
Content     :   
Created     :   
Authors     :   

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef INC_SF_Render_Shader_H
#define INC_SF_Render_Shader_H

#include "Kernel/SF_RefCount.h"
#include "Kernel/SF_Array.h"
#include "Kernel/SF_Hash.h"
#include "Kernel/SF_Random.h"

#include "Render/Render_Matrix2x4.h"
#include "Render/Render_Matrix4x4.h"
#include "Render/Render_Color.h"
#include "Render/Render_CxForm.h"
#include "Render/Render_HAL.h"
#include "Render/Render_MatrixPool.h"
#include "Render/Render_Stats.h"
#include "Render/Render_Containers.h"
#include "Render/Render_Primitive.h"
#include "Render/Render_Gradients.h"
#include "Render/Render_Vertex.h"
#include "Render/Render_Types2D.h"
#include "Render/Render_Math2D.h"
#include "Render/Render_Twips.h"

namespace Scaleform { namespace Render {

enum StandardShaderFlags
{
    SS_Batch      = 1,
    SS_Position3d = 2,
    SS_Mul        = 4,
    SS_Cxform     = 8,
};

enum VertexFormatFlags
{
    MVF_AlignVertexStride     = 0x01,
    MVF_AlignVertexAttrs      = 0x02,
    MVF_Align                 = MVF_AlignVertexStride | MVF_AlignVertexAttrs,
    MVF_ReverseColor          = 0x04,
    MVF_HasInstancing         = 0x08,
    MVF_EndianSwapFactors     = 0x10,
};

template<typename Uniforms, typename Shader>
class ShaderInterfaceBase
{
protected:
    float    UniformData[Uniforms::SU_TotalSize];
    bool     UniformSet[Uniforms::SU_Count];
    Texture* Textures[4];

public:

    ShaderInterfaceBase()
    {
        memset(Textures, 0, sizeof(Textures));
        memset(UniformSet, 0, sizeof(UniformSet));
    }

    unsigned GetParameterStage(const Shader& sd, unsigned var, int index = 0) const
    {
        if (sd->pVDesc->BatchUniforms[var].Offset >= 0 && index < sd->pVDesc->BatchUniforms[var].Size)
            return 0x10000 | sd->pVDesc->BatchUniforms[var].Size;
        else if (sd->pVDesc->Uniforms[var].Location >= 0 && index < sd->pVDesc->Uniforms[var].Size)
            return 0x10000 | sd->pVDesc->Uniforms[var].Size / sd->pVDesc->Uniforms[var].ElementSize;
        else if (sd->pFDesc->BatchUniforms[var].Offset >= 0 && index < sd->pFDesc->BatchUniforms[var].Size)
            return 0x20000 | sd->pFDesc->BatchUniforms[var].Size;
        else if (sd->pFDesc->Uniforms[var].Location >= 0 && index < sd->pFDesc->Uniforms[var].Size)
            return 0x20000 | sd->pFDesc->Uniforms[var].Size / sd->pVDesc->Uniforms[var].ElementSize;
        else
            return 0;
    }

    void BeginPrimitive()
    {
#ifdef SF_BUILD_DEBUG
        // If we are beginning a primitive, no uniforms should be currently set.
        for (int i = 0; i < Uniforms::SU_Count; i++)
        {
            SF_DEBUG_ASSERT1(!UniformSet[i], "Unexpected uniform set during BeginPrimitive (%d).", i);
        }
        memset(UniformData, 0, sizeof(UniformData));
#endif // SF_BUILD_DEBUG
    	memset(UniformSet, 0, sizeof(UniformSet));
        memset(Textures, 0, sizeof(Textures));
    }

    void SetUniform0(const Shader& sd, unsigned var, const float* v, unsigned n, unsigned index = 0, unsigned batch = 0)
    {
	    SF_ASSERT(batch == 0); SF_UNUSED(batch);
	    SF_DEBUG_ASSERT(sd->pFDesc->Uniforms[var].Size > 0 || sd->pVDesc->Uniforms[var].Size > 0, "Setting uniform not used by current shader.");

        if (sd->pVDesc->Uniforms[var].Size)
        {
            SF_ASSERT(sd->pVDesc->Uniforms[var].ShadowOffset + sd->pVDesc->Uniforms[var].ElementSize * 
                      index + n <= Uniforms::SU_TotalSize);
    	    memcpy(UniformData + sd->pVDesc->Uniforms[var].ShadowOffset + 
                   sd->pVDesc->Uniforms[var].ElementSize * index, v, n * sizeof(float));
        }
        if (sd->pFDesc->Uniforms[var].Size)
        {
            SF_ASSERT(sd->pFDesc->Uniforms[var].ShadowOffset + sd->pFDesc->Uniforms[var].ElementSize * 
                      index + n <= Uniforms::SU_TotalSize);
            memcpy(UniformData + sd->pFDesc->Uniforms[var].ShadowOffset + 
                   sd->pFDesc->Uniforms[var].ElementSize * index, v, n * sizeof(float));
        }
	    UniformSet[var] = 1;
    }

    void SetUniform(const Shader& sd, unsigned var, const float* v, unsigned n, unsigned index = 0, unsigned batch = 0)
    {
        if (sd->pVDesc->BatchUniforms[var].Offset >= 0)
	    {
	        unsigned bvar = sd->pVDesc->BatchUniforms[var].Array;
	        SetUniform0(sd, sd->pVDesc->BatchUniforms[var].Array, v, n,
			    batch * sd->pVDesc->Uniforms[bvar].BatchSize + sd->pVDesc->BatchUniforms[var].Offset + index);
	    }
        else if (sd->pFDesc->BatchUniforms[var].Offset >= 0)
        {
            unsigned bvar = sd->pFDesc->BatchUniforms[var].Array;
            SetUniform0(sd, sd->pFDesc->BatchUniforms[var].Array, v, n,
                batch * sd->pFDesc->Uniforms[bvar].BatchSize + sd->pFDesc->BatchUniforms[var].Offset + index);
        }
	    else
	        SetUniform0(sd, var, v, n, index, batch);
    }

    void SetColor(const Shader& sd, unsigned var, Color c, unsigned index = 0, unsigned batch = 0)
    {
	    const float mult  = 1.0f / 255.0f;
	    float rgba[4] = 
	    {
	        c.GetRed() * mult,  c.GetGreen() * mult,
	        c.GetBlue() * mult, c.GetAlpha() * mult
	    };
	    SetUniform(sd, var, rgba, 4, index, batch);
    }

    void SetMatrix(const Shader& sd, unsigned var, const Matrix2F &m, unsigned index = 0, unsigned batch = 0)
    {
        const float *rows = &m.M[0][0];
	    SetUniform(sd, var, rows, 8, index*2, batch);
    }

    void SetMatrix(const Shader& sd, unsigned var, const Matrix2F &m1, const HMatrix &m2, const MatrixState* Matrices, unsigned index = 0, unsigned batch = 0)
    {
        if (m2.Has3D())
        {
            Matrix4F m(Matrices->GetUVP(), Matrix3F(m2.GetMatrix3D(), m1));
            const float *rows = m.Data();
            SetUniform(sd, var, rows, 16, index, batch);
        }
        else
        {
            Matrix2F  m(m1, m2.GetMatrix2D(), Matrices->UserView);
            const float *rows = &m.M[0][0];

            SetUniform(sd, var, rows, 8, index*2, batch);
        }
    }

    void SetCxform(const Shader& sd, const Cxform & cx, unsigned index = 0, unsigned batch = 0)
    {
        const float (*rows)[4] = &cx.M[0];

        SetUniform(sd, Uniforms::SU_cxmul, rows[0], 4, index, batch);
        SetUniform(sd, Uniforms::SU_cxadd, rows[1], 4, index, batch);
    }

    // Returns 0 if uniform not present in shader
    unsigned GetUniformSize(const Shader& sd, unsigned i)
    {
        unsigned size = 0;
        if (sd->pVDesc->BatchUniforms[i].Array <= Uniforms::SU_Count-1)
            size = sd->pVDesc->BatchUniforms[i].Size * sd->pVDesc->Uniforms[(int)sd->pVDesc->BatchUniforms[i].Array].ElementSize;
        else if (sd->pVDesc->Uniforms[i].Location >= 0)
            size = sd->pVDesc->Uniforms[i].Size;
        else if (sd->pFDesc->BatchUniforms[i].Array <= Uniforms::SU_Count-1)
            size = sd->pFDesc->BatchUniforms[i].Size * sd->pFDesc->Uniforms[(int)sd->pFDesc->BatchUniforms[i].Array].ElementSize;
        else if (sd->pFDesc->Uniforms[i].Location >= 0)
            size = sd->pFDesc->Uniforms[i].Size;
        return size;
    }

    static bool IsBuiltinUniform(unsigned var)
    {
        return Uniforms::UniformFlags[var] & Uniforms::Uniform_Builtin;
    }

    void SetUserUniforms(const Shader& sd, const HMatrix &m, int batch = 0)
    {
        float* pdata = (float*)m.GetUserData();
        for (int i = 0; i < Uniforms::SU_Count; i++)
            if (!IsBuiltinUniform(i))
            {
                unsigned size = GetUniformSize(sd, i);

                if (size)
                {
                    if ((Uniforms::UniformFlags[i] & Uniforms::Uniform_TexScale) && size <= 4 && Textures[0])
                    {
                        ImageSize tsize = Textures[0]->GetSize();
                        float uscale = 1.0f/tsize.Width;
                        float vscale = 1.0f/tsize.Height;
                        float scaled[4];
                        scaled[0] = pdata[0] * uscale;
                        scaled[1] = pdata[1] * vscale;
                        scaled[2] = pdata[2];
                        scaled[3] = pdata[3];
                        SetUniform(sd, i, scaled, size, 0, batch);
                    }
                    else
                        SetUniform(sd, i, pdata, size, 0, batch);
                    pdata += size;
                }
            }
    }
};

template<typename FShaderDesc, typename VShaderDesc,
         class Uniforms, class ShaderInterface, class NativeTexture>
class StaticShaderManager
{
public:
    typedef typename VShaderDesc::VertexAttrDesc VertexAttrDesc;
    typedef typename VShaderDesc::ShaderType VShaderType;
    typedef typename FShaderDesc::ShaderType FShaderType;
    typedef typename ShaderInterface::Shader Shader;

protected:
    MultiKeyCollection<VertexElement, VertexFormat, 32>        VFormats;
    ProfileViews*                                              Profiler;

public:
    StaticShaderManager(ProfileViews* prof) : Profiler(prof) {}

    FShaderType StaticShaderForFill (PrimitiveFill* pfill, unsigned& fillflags, unsigned batchType)
    {
        switch(pfill->GetType())
        {
        // Override these specifically for video textures. It needs to set the frag shader as a video compatible one.
        case PrimFill_Texture:
        case PrimFill_Texture_EAlpha:
            if ( pfill->GetTextureCount() == 1 && ImageData::GetFormatPlaneCount(pfill->GetTexture(0)->GetFormat()) >= 3)
            {
                unsigned shader = pfill->GetType() == PrimFill_Texture_EAlpha ? 
                    FShaderDesc::FS_FYUVEAlpha : FShaderDesc::FS_FYUV;

                if (ImageData::GetFormatPlaneCount(pfill->GetTexture(0)->GetFormat()) == 4)
                    shader += FShaderDesc::FS_base_YUVA;

                if ((fillflags & FF_Cxform) && shader != FShaderDesc::FS_FText)
                    shader += FShaderDesc::FS_base_Cxform;

                switch(batchType)
                {
                case PrimitiveBatch::DP_Batch:
                    shader += FShaderDesc::FS_base_Batch;
                    break;

                case PrimitiveBatch::DP_Instanced:
                    shader += FShaderDesc::FS_base_Instanced;
                    break;
                default: break;
        }

                if (fillflags & FF_Multiply)
                    shader += FShaderDesc::FS_base_Mul;

                if (fillflags & FF_3DProjection)
                    shader += FShaderDesc::FS_base_Position3d;


                return (FShaderType)shader;
            }
            break;

        default: break;
        }
        return StaticShaderForFill(pfill->GetType(), fillflags, batchType);
    }

    FShaderType StaticShaderForFill (PrimitiveFillType fill, unsigned& fillflags, unsigned batchType)
    {
        unsigned shader = FShaderDesc::FS_None;

        switch (fill)
        {
        default:
        case PrimFill_None:
        case PrimFill_Mask:
        case PrimFill_SolidColor:
            shader = FShaderDesc::FS_FSolid;
            fillflags &= ~FF_Cxform;
            break;

        case PrimFill_VColor:
            shader = FShaderDesc::FS_FVertex;
            break;

        case PrimFill_VColor_EAlpha:
            shader = FShaderDesc::FS_FVertexEAlpha;
            break;

        case PrimFill_Texture:
            shader = FShaderDesc::FS_FTexTG;
            break;

        case PrimFill_Texture_EAlpha:
            shader = FShaderDesc::FS_FTexTGEAlpha;
            break;
    		
        case PrimFill_Texture_VColor:
            shader = FShaderDesc::FS_FTexTGVertex;
            break;

        case PrimFill_Texture_VColor_EAlpha:
            shader = FShaderDesc::FS_FTexTGVertexEAlpha;
            break;

        case PrimFill_2Texture:
            shader = FShaderDesc::FS_FTexTGTexTG;
            break;

        case PrimFill_2Texture_EAlpha:
            shader = FShaderDesc::FS_FTexTGTexTGEAlpha;
            break;

        case PrimFill_UVTexture:
            shader = FShaderDesc::FS_FTextColor;
            break;

        case PrimFill_UVTextureAlpha_VColor:
            shader = FShaderDesc::FS_FText;
            fillflags |= FF_Cxform;
            break;

        case PrimFill_UVTextureDFAlpha_VColor:
            fillflags |= FF_Cxform;
            return FShaderDesc::FindStaticShader("TextDFA", (batchType == PrimitiveBatch::DP_Batch ? SS_Batch : 0) | (fillflags & FF_Multiply ? SS_Mul : 0));
            break;
        }

        if ((fillflags & FF_Cxform) && shader != FShaderDesc::FS_FText)
            shader += FShaderDesc::FS_base_Cxform;


        switch(batchType)
        {
        case PrimitiveBatch::DP_Batch:
            shader += FShaderDesc::FS_base_Batch;
            break;

        case PrimitiveBatch::DP_Instanced:
            shader += FShaderDesc::FS_base_Instanced;
            break;

        default: break;
        }

        if (fillflags & FF_Multiply)
            shader += FShaderDesc::FS_base_Mul;

        if (fillflags & FF_3DProjection)
            shader += FShaderDesc::FS_base_Position3d;

        return (FShaderType)shader;
    }

    const VertexFormat* GetVertexFormat(VertexElement* pelements, unsigned count, unsigned size, unsigned alignment = 1)
    {
        VertexFormat  *pformat   = VFormats.Find(pelements, count);
        if (pformat)
            return pformat;

        pformat = VFormats.Add(&pelements, pelements, count);
        if (!pformat)
            return 0;

        // Align vertex size to if requested
        size = (size+(alignment-1)) & ~(alignment-1);

        pformat->Size      = size;
        pformat->pElements = pelements;
        return pformat;
    }

    struct SourceFormatHash
    {
        SourceFormatHash(PrimitiveFillType  fill,
                         const VertexFormat* sourceFormat,
                         unsigned           flags) :
            FillType(fill),
            SourceFormat(sourceFormat),
            FormatFlags(flags) { }

        bool operator==(const SourceFormatHash& other) const
        {
            return other.FillType == FillType &&
                   other.SourceFormat == SourceFormat &&
                   other.FormatFlags == FormatFlags;
        }
        PrimitiveFillType       FillType;
        const VertexFormat*     SourceFormat;
        unsigned                FormatFlags;
    };
    struct ResultFormat
    {
        const VertexFormat*     single;
        const VertexFormat*     batch;
        const VertexFormat*     instanced;
    };
    HashLH<SourceFormatHash, ResultFormat>  VertexFormatComputedHash;

    void    MapVertexFormat(PrimitiveFillType fill, const VertexFormat* sourceFormat,
			    const VertexFormat** single,
			    const VertexFormat** batch, const VertexFormat** instanced,
                unsigned flags = 0)
    {
        // Hashed lookup. If we have done this source format/fill and flags combination before, simply return it.
        ResultFormat result;
        SourceFormatHash sourceKey(fill, sourceFormat, flags);
        if (VertexFormatComputedHash.Get(sourceKey, &result))
        {
            *single = result.single;
            *batch = result.batch;
            *instanced = result.instanced;
            return;
        }
        
        unsigned             fillflags = 0;
        FShaderType          shader = this->StaticShaderForFill(fill, fillflags, PrimitiveBatch::DP_Single);
        const VShaderDesc*   pshader = VShaderDesc::GetDesc(FShaderDesc::VShaderForFShader(shader));

        const VertexAttrDesc* psvf = pshader->Attributes;
        const unsigned       maxVertexElements = 8;
        VertexElement        outf[maxVertexElements];
        unsigned             size = 0, maxalign = 4;
        int                  batchOffset = -1, batchElement = -1;
        int                  j = 0;

        for (int i = 0; i < pshader->NumAttribs; i++)
        {
            if ((psvf[i].Attr & (VET_Usage_Mask | VET_Index_Mask | VET_Components_Mask)) == (VET_Color | (1 << VET_Index_Shift) | 4))
            {
                // XXX - change shaders to use .rg instead of .ra for these
                if ( (flags & MVF_EndianSwapFactors) == 0 )
                {
                outf[j].Offset = size;
                outf[j].Attribute = VET_T0Weight8;
                j++;
                outf[j].Offset = size+3;
                outf[j].Attribute = VET_FactorAlpha8;
                batchOffset = size+2;
                batchElement = j;
                }
                else
                {
                    outf[j].Offset = size;
                    outf[j].Attribute = VET_FactorAlpha8;
                j++;
                    outf[j].Offset = size+3;
                    outf[j].Attribute = VET_T0Weight8;
                    batchElement = j;
                    batchOffset = size+1;
                }
                j++;
                size += 4;
                continue;
            }

            const VertexElement* pv = sourceFormat->GetElement(psvf[i].Attr & (VET_Usage_Mask|VET_Index_Mask), VET_Usage_Mask|VET_Index_Mask);
            if (!pv)
            {
                *batch = *single = *instanced = NULL;
                return;
            }
            outf[j] = *pv;
            outf[j].Offset = size;

            if (flags & MVF_ReverseColor)
            {
                if ((pv->Attribute & (VET_Usage_Mask|VET_CompType_Mask|VET_Components_Mask)) == VET_ColorRGBA8)
                    outf[j].Attribute = VET_ColorARGB8 | (pv->Attribute & ~(VET_Usage_Mask|VET_CompType_Mask|VET_Components_Mask));
            }
            else
            {
                if ((pv->Attribute & (VET_Usage_Mask|VET_CompType_Mask|VET_Components_Mask)) == VET_ColorARGB8)
                    outf[j].Attribute = VET_ColorRGBA8 | (pv->Attribute & ~(VET_Usage_Mask|VET_CompType_Mask|VET_Components_Mask));
            }

            SF_ASSERT((outf[j].Attribute & VET_Components_Mask) > 0 && (outf[j].Attribute & VET_Components_Mask) <= 4);
            maxalign = Alg::Max(maxalign, pv->CompSize());
            size += outf[j].Size();
            if (flags & MVF_AlignVertexAttrs)
                size = (size + 3) & ~3;

            j++;
        }
        outf[j].Attribute = VET_None;
        outf[j].Offset = 0;
        *single = GetVertexFormat(outf, j+1, size, flags & MVF_AlignVertexStride ? maxalign : 1);

        // If we have instancing support, add an I8 vertex element to indicate this.
        if ( flags & MVF_HasInstancing )
        {
            outf[j].Attribute = VET_Instance | VET_I8  | 1 | VET_Argument_Flag;
            outf[j].Offset = 0;
            outf[j+1].Attribute = VET_None;
            outf[j+1].Offset = 0;
            *instanced = GetVertexFormat(outf, j+2, size, flags & MVF_AlignVertexStride ? maxalign : 1);
        }
        else
        *instanced = 0;

        if (batchOffset >= 0)
        {
            for (int i = j-1; i >= batchElement; i--)
                outf[i+1] = outf[i];

            outf[batchElement].Attribute = VET_Instance8;
            outf[batchElement].Offset = batchOffset;
        }
        else
        {
            outf[j].Attribute = VET_Instance8;
            outf[j].Offset = (flags & MVF_EndianSwapFactors) == 0 ? size : size+3;
            size += outf[j].Size();
        }
        outf[j+1].Attribute = VET_None;
        outf[j+1].Offset = 0;
        *batch = GetVertexFormat(outf, j+2, size, flags & MVF_AlignVertexStride ? maxalign : 1);

        // Set the results in the hash.
        result.single    = *single;
        result.batch     = *batch;
        result.instanced = *instanced;
        VertexFormatComputedHash.Set(sourceKey, result);
    }

    const Shader& SetPrimitiveFill(PrimitiveFill* pfill, unsigned& fillFlags, unsigned batchType, const VertexFormat* pvf, unsigned meshCount,
                                   const MatrixState* Matrices, const Primitive::MeshEntry* pmeshes, ShaderInterface* psi)
    {
        PrimitiveFillType fillType = pfill->GetType();

        if ((fillFlags & FF_Blending) == 0 && pfill->RequiresBlend())
            fillFlags |= FF_Blending;

            for (unsigned i = 0; i < meshCount; i++)
            {
                Cxform finalCx = Profiler->GetCxform(pmeshes[i].M.GetCxform());
                if (!(finalCx == Cxform::Identity))
                {
                    fillFlags |= FF_Cxform;
                    if (finalCx.RequiresBlend())
                        fillFlags |= FF_Blending;
                    break;
                }
            }

        FShaderType shader = StaticShaderForFill(pfill, fillFlags, batchType);
        VShaderType vshader = FShaderDesc::VShaderForFShader(shader);

        psi->SetStaticShader(vshader, shader, pvf);
        psi->BeginPrimitive();
        const Shader& pso = psi->GetCurrentShaders();
        bool solid = (fillType == PrimFill_None || fillType == PrimFill_Mask || fillType == PrimFill_SolidColor);

        if (solid)
            psi->SetColor(pso, Uniforms::SU_cxmul, Profiler->GetColor(pfill->GetSolidColor()));
        else if (fillType >= PrimFill_Texture)
        {     
            NativeTexture*  pt0 = (NativeTexture*)pfill->GetTexture(0);
            SF_ASSERT(pt0);

            ImageFillMode fm0 = pfill->GetFillMode(0);


            psi->SetTexture(pso, Uniforms::SU_tex, pt0, fm0);
            if ((fillType == PrimFill_2Texture) || (fillType == PrimFill_2Texture_EAlpha))
            {
                NativeTexture* pt1 = (NativeTexture*)pfill->GetTexture(1);
                ImageFillMode  fm1 = pfill->GetFillMode(1);

                psi->SetTexture(pso, Uniforms::SU_tex, pt1, fm1, pt0->GetPlaneCount());
            }
        }

        if (fillType == PrimFill_UVTextureDFAlpha_VColor)
        {
            for (unsigned i = 0; i < meshCount; i++)
            {
                psi->SetUserUniforms(pso, pmeshes[i].M, i);
            }
        }

        unsigned tmCount = (psi->GetParameterStage(pso, Uniforms::SU_texgen) & 0xffff) >> 1;

        for (unsigned i = 0; i < meshCount; i++)
        {
            psi->SetMatrix(pso, Uniforms::SU_mvp, pmeshes[i].pMesh->VertexMatrix, pmeshes[i].M, Matrices, 0, i);

            if (fillType == PrimFill_Mask)
                psi->SetColor(pso, Uniforms::SU_cxmul, Color(128,0,0,128));
            else
            if (fillFlags & FF_Cxform)
                psi->SetCxform(pso, Profiler->GetCxform(pmeshes[i].M.GetCxform()), 0, i);

            for (unsigned tm = 0; tm < tmCount; tm++)
            {
                Matrix2F m(pmeshes[i].pMesh->VertexMatrix);
                m.Append(pmeshes[i].M.GetTextureMatrix(tm));
                psi->SetMatrix(pso, Uniforms::SU_texgen, m, tm, i);
            }
        }

        if (!(fillFlags & FF_LeaveOpen))
        psi->Finish(meshCount);

        return pso;
    }

    const Shader& SetFill(PrimitiveFillType fillType, unsigned& fillFlags, unsigned batchType, const VertexFormat* pvf, ShaderInterface* psi)
    {
        FShaderType shader = this->StaticShaderForFill(fillType, fillFlags, batchType);
        VShaderType vshader = FShaderDesc::VShaderForFShader(shader);

        psi->SetStaticShader(vshader, shader, pvf);
        psi->BeginPrimitive();
        return psi->GetCurrentShaders();
    }

    // Applies the given fill pass for the given filter. The first target in the list is the texture to be filtered.
    bool SetFilterFill(const Matrix2F& mvp, const Cxform & cx, const Filter* filter, Ptr<RenderTarget> * targets, 
                       unsigned* shaders, unsigned pass, unsigned passCount, const VertexFormat* pvf, 
                       ShaderInterface* psi)
    { 
        VShaderType vshader = FShaderDesc::VShaderForFShader((FShaderType)shaders[pass]);

        if ( !psi->SetStaticShader( vshader, (FShaderType)shaders[pass], pvf ) )
            return false;
        psi->BeginPrimitive();
        const Shader& pso = psi->GetCurrentShaders();

        // Apply the mvp
        psi->SetMatrix(pso, Uniforms::SU_mvp, mvp );

        // Apply the source texture.
        bool shadowPass = shaders[pass] >= FShaderDesc::FS_start_shadows && shaders[pass] <= FShaderDesc::FS_end_shadows;
        NativeTexture* ptexture = (NativeTexture*)targets[Target_Source]->GetTexture();
        psi->SetTexture(pso, Uniforms::SU_tex, ptexture, ImageFillMode(Wrap_Clamp, Sample_Linear));

        // Apply the texture matrix.
        Matrix2F texgen;
        const Rect<int>& srect = targets[Target_Source]->GetRect();
        texgen.AppendTranslation((float)srect.x1, (float)srect.y1);
        texgen.AppendScaling((float)srect.Width() / ptexture->GetSize().Width, (float)srect.Height() / ptexture->GetSize().Height);
        psi->SetUniform(pso, Uniforms::SU_texgen, &(texgen.M[0][0]), 8);

        // Apply the remainder of the uniforms.
        if ( filter->GetFilterType() <= Filter_Blur_End )
        {
            BlurFilter* blurFilter = (BlurFilter*)filter;
            const BlurFilterParams& params = blurFilter->GetParams();

            // Common parameters.
            psi->SetCxform(pso, cx);

            float texscale[2];
            texscale[0] = 1.0f / ptexture->GetSize().Width;
            texscale[1] = 1.0f / ptexture->GetSize().Height;

            float fsize[4];
            float blurx = Alg::Max( 1.0f, floorf(TwipsToPixels(params.BlurX)));
            float blury = Alg::Max( 1.0f, floorf(TwipsToPixels(params.BlurY)));
            if ( shaders[pass] == FShaderDesc::FS_FBox1Blur || shaders[pass] == FShaderDesc::FS_FBox1BlurMul)
            {
                // On even passes, do the X blur, on odd passes, do the Y blur.
                if ( (pass & 1) == 0 )
                {
                    fsize[0] = (blurx-1)*0.5f;
                    fsize[1] = 0;
                    fsize[3] = 1.0f / (blurx);
                    texscale[1] = 0;
                }
                else
                {
                    fsize[0] = (blury-1)*0.5f;
                    fsize[1] = 0;
                    fsize[3] = 1.0f / (blury);
                    texscale[0] = 0;
                }
            }
            else if ( pass == passCount-1 && pass != 0)
            {
                fsize[0] = 0.0f;
                fsize[1] = (blury-1)*0.5f;
                fsize[3] = 1.0f / (blury);
            }
            else
            {
                fsize[0] = (blurx-1)*0.5f;
                fsize[1] = (blury-1)*0.5f;
                fsize[3] = 1.0f / (blurx * blury);
            }

            // fsize.z = strength, only used on the last pass.
            fsize[2] = (pass == passCount-1) ? params.Strength : 1.0f;

            psi->SetUniform(pso, Uniforms::SU_fsize, fsize, 4 );
            psi->SetUniform(pso, Uniforms::SU_texscale, texscale, 2 );

            if ( shadowPass )
            {
                float scolors[2][4];
                float srctexscale[2];
                float offset[2];
                params.Colors[0].GetRGBAFloat(scolors[0]);
                params.Colors[1].GetRGBAFloat(scolors[1]);
                offset[0] = -TwipsToPixels(params.Offset.x);
                offset[1] = -TwipsToPixels(params.Offset.y);

                if ( !(params.Mode & BlurFilterParams::Mode_HideObject &&
                     !(params.Mode & BlurFilterParams::Mode_Knockout)) && 
                     targets[Target_Original] )
                {
                    NativeTexture* psrctex = (NativeTexture*)targets[Target_Original]->GetTexture();
                    srctexscale[0] = 1.0f / (psrctex->GetSize().Width * texscale[0] );
                    srctexscale[1] = 1.0f / (psrctex->GetSize().Height * texscale[1] );
                    psi->SetUniform(pso, Uniforms::SU_srctexscale, srctexscale, 2 );
                    psi->SetTexture(pso, Uniforms::SU_srctex, psrctex, ImageFillMode(Wrap_Clamp, Sample_Linear));
                }
        
                psi->SetUniform(pso, Uniforms::SU_scolor, scolors[0], 4 );
                psi->SetUniform(pso, Uniforms::SU_offset, offset, 2 );

                if ( params.Mode & BlurFilterParams::Mode_Highlight)
                    psi->SetUniform(pso, Uniforms::SU_scolor2, scolors[1], 4 );
            }
        }
        else
        {
            ColorMatrixFilter& matrixFilter = *(ColorMatrixFilter*)filter;

            // If there is a color transform, in addition to the filter, embed that directly into the matrix.
            float matrixData[ColorMatrixFilter::ColorMatrixEntries];
            memcpy(matrixData, &matrixFilter[0], sizeof(matrixData));
            SF_COMPILER_ASSERT(ColorMatrixFilter::ColorMatrixEntries == 20);
            for (unsigned i = 0; i < 5; i++)
            {
                matrixData[i*4+0] = matrixData[i*4+0] * cx.M[0][0] * cx.M[0][3];
                matrixData[i*4+1] = matrixData[i*4+1] * cx.M[0][1] * cx.M[0][3];
                matrixData[i*4+2] = matrixData[i*4+2] * cx.M[0][2] * cx.M[0][3];
                matrixData[i*4+3] = matrixData[i*4+3] * cx.M[0][3];
            }
            matrixData[16] = (matrixData[16] + cx.M[1][0] * 1.f/255.f) * cx.M[0][3];
            matrixData[17] = (matrixData[17] + cx.M[1][1] * 1.f/255.f) * cx.M[0][3];
            matrixData[18] = (matrixData[18] + cx.M[1][2] * 1.f/255.f) * cx.M[0][3];
            matrixData[19] = (matrixData[19] + cx.M[1][3] * 1.f/255.f) * cx.M[0][3];

            // Now apply the data.
            psi->SetUniform(pso, Uniforms::SU_cxadd, &(matrixData[16]), 4 );
            psi->SetUniform(pso, Uniforms::SU_cxmul, &(matrixData[0]), 16 );
        }

        psi->Finish(0);

        return true;
    }

    static const int MaximumBlurKernel = 32 * 20 * 20; // in twips^2
    static const int MaximumFilterPasses = 8;

    // Returns the number of passes required to render the given filter from scratch. Fills out the passes
    // array (which must contain at least MaximumPasses elements), with the shaders used per-pass.
    unsigned GetFilterPasses(const Filter* filter, unsigned fillFlags, unsigned* passes ) const
    {
        unsigned passCount;
        if ( filter->GetFilterType() <= Filter_Blur_End )
        {
            BlurFilter* bfilter = (BlurFilter*)filter;
            const BlurFilterParams& params = bfilter->GetParams();
            bool box1 = false;

            // If the blur is too large, do two 1D blurs, instead of one 2D blur.
            if ( (params.BlurX * params.BlurY) >= (float)MaximumBlurKernel )
            {
                passCount = 2 * params.Passes;
                box1 = true;
            }
            else
                passCount = params.Passes;

            // Every pass except the last is simply a blur
            unsigned pass;
            for ( pass = 0; pass < passCount-1; ++pass )
                passes[pass] = box1 ? FShaderDesc::FS_FBox1Blur : FShaderDesc::FS_FBox2Blur;

            switch( params.Mode & BlurFilterParams::Mode_FilterMask )
            {
                default:
                case Filter_Blur:
                    passes[pass] = FShaderDesc::FS_start_blurs;

                    // Extra flags.
                    passes[pass] += box1 ? 0 : FShaderDesc::FS_blurs_Box2;
                    if ( fillFlags & FF_Multiply )
                        passes[pass] += (unsigned)FShaderDesc::FS_blurs_Mul;
                    break;
                case Filter_Glow:
                case Filter_Bevel:
                case Filter_Shadow:

                    // Base shader.
                    if ( params.Mode & BlurFilterParams::Mode_Inner )
					{
						if ( !(params.Mode & BlurFilterParams::Mode_Highlight) )
							passes[pass] = FShaderDesc::FS_FBox2InnerShadow;
						else
							passes[pass] = FShaderDesc::FS_FBox2InnerShadowHighlight;
					}
                    else if (params.Mode & BlurFilterParams::Mode_HideObject &&
                        !(params.Mode & BlurFilterParams::Mode_Knockout))
					{
						if ( !(params.Mode & BlurFilterParams::Mode_Highlight) )
							passes[pass] = FShaderDesc::FS_FBox2Shadowonly;
						else
							passes[pass] = FShaderDesc::FS_FBox2ShadowonlyHighlight;
					}
                    else
					{
						if ( !(params.Mode & BlurFilterParams::Mode_Highlight) )
							passes[pass] = FShaderDesc::FS_FBox2Shadow;
						else
							passes[pass] = FShaderDesc::FS_FBox2ShadowHighlight;
					}

                    // Extra flags.
                    if ( fillFlags & FF_Multiply )
                        passes[pass] += (unsigned)FShaderDesc::FS_shadows_Mul;
                    if ( params.Mode & BlurFilterParams::Mode_Knockout )
                        passes[pass] += (unsigned)FShaderDesc::FS_shadows_Knockout;
                    break;

                // Unsupported.
                //case Filter_GradientGlow:
                //case Filter_GradientBevel:
            }
        }
        else
        {
            passCount = 1;
            ColorMatrixFilter* matrixFilter = (ColorMatrixFilter*)filter;
            float* params = &((float&)matrixFilter[0]);
            SF_UNUSED(params);
            passes[0] = FShaderDesc::FS_start_cmatrix;
            if ( fillFlags & FF_Multiply )
                passes[0] += (unsigned)FShaderDesc::FS_cmatrix_Mul;
        }
        return passCount;
    }

    // Set of flags that may be passed to various DrawableImage functions.
    enum DrawableImageFlags
    {
        CPF_HalfPixelOffset     = 0x1,      // Some platforms require a half-pixel offset to have pixel-centres match exactly (D3D9).
        CPF_InvertedViewport    = 0x2,      // Some platforms require their render target output to be flipped (GL).
    };

    // Sets a fill based on a CopyPixels BitmapData command.
    bool SetDrawableCopyPixelsFill( Render::Texture** tex, const Matrix2F* texgen, const Size<int> texsize, 
                                    const Matrix2F& mvp, bool mergeAlpha, bool destAlpha, const VertexFormat* pvf,
									ShaderInterface* psi, unsigned flags = 0 )
    {
        // Determine which shader to use, based on parameters.
		unsigned type = VShaderDesc::VS_start_DrawableCopyPixels;
        if ( tex[2] )
		{
            type += VShaderDesc::VS_DrawableCopyPixels_DrawableCopyPixelsAlpha;
		}
		if ( mergeAlpha )
			type += VShaderDesc::VS_DrawableCopyPixels_MergeAlpha;
		if ( !destAlpha )
			type += VShaderDesc::VS_DrawableCopyPixels_NoDestAlpha;
		type += VShaderDesc::VS_DrawableCopyPixels_CopyLerp;

        if (!psi->SetStaticShader((VShaderType)type, (FShaderType)type, pvf ))
            return false;

        psi->BeginPrimitive();
        return DrawableFinish(2 + (tex[2] ? 1 : 0), tex, texgen, texsize, mvp, psi, flags);
    }

    // Sets a fill based on a CopyChannel or Merge BitmapData command.
    bool SetDrawableMergeFill(  Render::Texture** tex, const Matrix2F* texgen, const Size<int> texsize, 
                                const Matrix4F* cxmul,
                                const VertexFormat* pvf, ShaderInterface* psi, unsigned flags = 0 )
    {
        if (!psi->SetStaticShader(VShaderDesc::VS_VDrawableMerge, FShaderDesc::FS_FDrawableMerge, pvf ))
            return false;
        psi->BeginPrimitive();
        const Shader& pso = psi->GetCurrentShaders();

        // Set the color matrices which will perform the operation.
        psi->SetUniform(pso, Uniforms::SU_cxmul, cxmul[0], 16);
        psi->SetUniform(pso, Uniforms::SU_cxmul1, cxmul[1], 16);

        // Make the mvp cover the entire viewport.
        Matrix2F mvp = Matrix2F::Scaling(2,-2) * Matrix2F::Translation(-0.5f, -0.5f);
        return DrawableFinish(2, tex, texgen, texsize, mvp, psi, flags);
    }

    bool SetDrawableCxform( Render::Texture** tex, const Matrix2F* texgen, const Size<int> texsize, 
                            const Cxform* cx,
                            const VertexFormat* pvf, ShaderInterface* psi, unsigned flags = 0 )
    {
        if (!psi->SetStaticShader(VShaderDesc::VS_VTexTGCxform, FShaderDesc::FS_FTexTGCxform, pvf ))
            return false;
        psi->BeginPrimitive();
        const Shader& pso = psi->GetCurrentShaders();

        psi->SetCxform(pso, *cx);

        // Make the mvp cover the entire viewport.
        Matrix2F mvp = Matrix2F::Scaling(2,-2) * Matrix2F::Translation(-0.5f, -0.5f);
        return DrawableFinish(1, tex, texgen, texsize, mvp, psi, flags);
    }

    bool SetDrawableCompare( Render::Texture** tex, const Matrix2F* texgen, const Size<int> texsize, 
                             const VertexFormat* pvf, ShaderInterface* psi, unsigned flags = 0 )
    {
        if (!psi->SetStaticShader(VShaderDesc::VS_VDrawableCompare, FShaderDesc::FS_FDrawableCompare, pvf ))
            return false;
        psi->BeginPrimitive();

        // Make the mvp cover the entire viewport.
        Matrix2F mvp = Matrix2F::Scaling(2,-2) * Matrix2F::Translation(-0.5f, -0.5f);
        return DrawableFinish(2, tex, texgen, texsize, mvp, psi, flags);
    }

    bool SetDrawablePaletteMap( Render::Texture** tex, const Matrix2F* texgen, const Size<int> texsize, 
                                const Matrix2F& mvp, Render::Texture* paletteMap,
                                const VertexFormat* pvf, ShaderInterface* psi, unsigned flags = 0 )
    {
        if (!psi->SetStaticShader(VShaderDesc::VS_VDrawablePaletteMap, FShaderDesc::FS_FDrawablePaletteMap, pvf ))
            return false;
        psi->BeginPrimitive();
        const Shader& pso = psi->GetCurrentShaders();

        // Set the palette map texture.
        psi->SetTexture(pso, Uniforms::SU_srctex, paletteMap, ImageFillMode(Wrap_Clamp, Sample_Point));

        return DrawableFinish(1, tex, texgen, texsize, mvp, psi, flags);
    }

#if 1
    bool SetDrawableThreshold(  Render::Texture**, const Matrix2F*, const Size<int>, 
        const Matrix2F&, DrawableImage::OperationType, UInt32, UInt32, UInt32, bool,
        const VertexFormat*, ShaderInterface*, unsigned = 0 )
    {
        return false;
    }
#else
    bool SetDrawableThreshold(  Render::Texture** tex, const Matrix2F* texgen, const Size<int> texsize, 
                                const Matrix2F& mvp, DrawableImage::OperationType op, UInt32 threshold, UInt32 color, UInt32 mask, bool copySource,
                                const VertexFormat* pvf, ShaderInterface* psi, unsigned flags = 0 )
    {
        // Determine which shader to use, based on parameters.
        unsigned type = VShaderDesc::VS_start_DrawableThreshold;
        if ( copySource )
        {
            type += VShaderDesc::VS_DrawableThreshold_DrawableThresholdSource;
            type += VShaderDesc::VS_DrawableThreshold_ThresholdFinishSource;
        }
        else
            type += VShaderDesc::VS_DrawableThreshold_ThresholdFinish;

        switch(op)
        {
        case DrawableImage::Operator_LT: type += VShaderDesc::VS_DrawableThreshold_LT; break;
        case DrawableImage::Operator_LE: type += VShaderDesc::VS_DrawableThreshold_LE; break;
        case DrawableImage::Operator_GT: type += VShaderDesc::VS_DrawableThreshold_GT; break;
        case DrawableImage::Operator_GE: type += VShaderDesc::VS_DrawableThreshold_GE; break;
        case DrawableImage::Operator_EQ: type += VShaderDesc::VS_DrawableThreshold_EQ; break;
        case DrawableImage::Operator_NE: type += VShaderDesc::VS_DrawableThreshold_NE; break;
        }

        if (!psi->SetStaticShader((VShaderType)type, (FShaderType)type, pvf ))
            return false;
        psi->BeginPrimitive();
        const Shader& pso = psi->GetCurrentShaders();

        // Threshold is expected to already be masked in the shader.
        Color maskedThreshold = threshold & mask;
        float colorConv[4];
        maskedThreshold.GetRGBAFloat(colorConv);
        psi->SetUniform(pso, Uniforms::SU_threshold, colorConv, 4 );
        Color maskC = mask;
        maskC.GetRGBAFloat(colorConv);
        psi->SetUniform(pso, Uniforms::SU_mask, colorConv, 4);
        Color col = color;
        col.GetRGBAFloat(colorConv);
        psi->SetUniform(pso, Uniforms::SU_color, colorConv, 4);
   
        return DrawableFinish(copySource ? 2 : 1, tex, texgen, texsize, mvp, psi, flags);
    }
#endif

    // This function is used by many BitmapData commands. It assumes that one of the SetDrawable* functions
    // has already been called.
    bool DrawableFinish    (  unsigned srcCount, 
                              Render::Texture** tex, const Matrix2F* texgen, const Size<int> texsize, 
                              const Matrix2F& mvpOriginal,
                              ShaderInterface* psi, unsigned flags = 0 )
    {
        const Shader& pso = psi->GetCurrentShaders();

        // Caculate the MVP, should be the whole viewport.
        Matrix2F mvp = mvpOriginal;

        if ( flags & CPF_InvertedViewport )
        {
            mvp.PrependTranslation(0.0f, 1.0f);
            mvp.PrependScaling(1.0f, -1.0f);
        }
        if ( flags & CPF_HalfPixelOffset )
        {
            mvp.Tx() -= 1.0f/texsize.Width;   // D3D9 1/2 pixel center offset
            mvp.Ty() += 1.0f/texsize.Height;
        }

        psi->SetMatrix (pso, Uniforms::SU_mvp,      mvp );

        for ( unsigned i =0; i < srcCount; ++i )
        {
            psi->SetTexture(pso, Uniforms::SU_tex, tex[i], ImageFillMode(Wrap_Clamp, Sample_Point), i);
            psi->SetMatrix(pso, Uniforms::SU_texgen, texgen[i], i);
        }
        psi->Finish(1);
        return true;
    }
};

template<class Builder>
void BuildVertexArray(const VertexFormat* pfmt, Builder& output)
{
    const VertexElement* pve = pfmt->pElements;
    int vi = 0;
    for (; pve->Attribute != VET_None; pve++, vi++)
    {
        unsigned offset = pve->Offset;
        int ac = (pve->Attribute & VET_Components_Mask);

        // Packed factors
        if (((pve[0].Attribute | pve[1].Attribute) & (VET_Usage_Mask|VET_Index_Mask)) == (VET_Color | (3 << VET_Index_Shift)))
        {
            ac = 4;
            SF_ASSERT(pve[1].Offset == offset+3);
            pve++;
        }
        // Factors with Batch index
        else if (((pve[1].Attribute & VET_Usage_Mask) == VET_Instance) &&
            ((pve[0].Attribute | pve[2].Attribute) & (VET_Usage_Mask|VET_Index_Mask)) == (VET_Color | (3 << VET_Index_Shift)))
        {
            ac = 4;
#ifndef SF_OS_XBOX360
            SF_ASSERT(pve[1].Offset == offset+2);
            SF_ASSERT(pve[2].Offset == offset+3);
#endif
            pve+=2;
        }

        output.Add(vi, pve->Attribute, ac, offset);
    }

    output.Finish(vi);
}

}}

#endif
