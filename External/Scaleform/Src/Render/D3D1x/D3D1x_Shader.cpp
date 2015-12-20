/**************************************************************************

Filename    :   D3D1x_Shader.cpp
Content     :   
Created     :   
Authors     :   

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "Render/D3D1x/D3D1x_Shader.h"
#include "Render/D3D1x/D3D1x_HAL.h"
#include "Kernel/SF_Debug.h"

namespace Scaleform { namespace Render { namespace D3D1x {

extern const char* ShaderUniformNames[Uniform::SU_Count];

bool VertexShader::Init(ID3D1x(Device)* pdevice, const VertexShaderDesc* pd)
{
    pDesc = pd;
    if ( FAILED( D3D1xCreateVertexShader(pdevice, pDesc->pBinary, pDesc->BinarySize, &pProg.GetRawRef())))
        return false;

#if 0 && !defined(SF_OS_WINMETRO)
    // Need to parse the constant table to find out where the constant offsets are. They might not be as
    // reported in the Desc, because the shader compiler can align things in a way it doesn't predict.
    ID3D1x(ShaderReflection)* preflector;
#if SF_D3D_VERSION == 11
    if ( FAILED( D3DReflect(pDesc->pBinary, pDesc->BinarySize, IID_ID3D1x(ShaderReflection), (void**)&preflector )))
        return false;
#elif SF_D3D_VERSION == 10
    // D3DReflect doesn't seem to work for DX10. Use the deprecated function.
    if ( FAILED( D3D10ReflectShader(pDesc->pBinary, pDesc->BinarySize, &preflector )))
        return false;
#endif
    ID3D1x(ShaderReflectionConstantBuffer)* pcb = preflector->GetConstantBufferByIndex(0);
    for ( unsigned uniform = 0; uniform < Uniform::SU_Count; ++uniform)
    {
        ID3D1x(ShaderReflectionVariable)* pvar = pcb->GetVariableByName(ShaderUniformNames[uniform]);
        D3D1x(SHADER_VARIABLE_DESC) varDesc;
        if ( !pvar || FAILED(pvar->GetDesc(&varDesc)))
        {
            SF_ASSERT(pd->Uniforms[uniform].Location == -1);
            UniformOffset[uniform] = -1;
            continue;
        }
        UniformOffset[uniform] = varDesc.StartOffset;
        SF_DEBUG_ASSERT2((int)varDesc.StartOffset == pd->Uniforms[uniform].Location * 16, "shader %d uniform %d location differs", pDesc->Type, uniform);
    }
#else
    for ( unsigned uniform = 0; uniform < Uniform::SU_Count; ++uniform)
    {
        UniformOffset[uniform] = pd->Uniforms[uniform].Location * 16;
    }
#endif
    return true;
}

void VertexShader::Shutdown( )
{
    pProg = 0;
}

bool FragShader::Init(ID3D1x(Device)* pdevice, const FragShaderDesc* pd)
{
    pDesc = pd;
    if ( !SUCCEEDED( D3D1xCreatePixelShader(pdevice, pDesc->pBinary, pDesc->BinarySize, &pProg.GetRawRef())) )
        return false;

#if 0 && !defined(SF_OS_WINMETRO)
	// Need to parse the constant table to find out where the constant offsets are. They might not be as
    // reported in the Desc, because the shader compiler can align things in a way it doesn't predict.
    ID3D1x(ShaderReflection)* preflector;
#if SF_D3D_VERSION == 11
    if ( FAILED( D3DReflect(pDesc->pBinary, pDesc->BinarySize, IID_ID3D1x(ShaderReflection), (void**)&preflector )))
        return false;
#elif SF_D3D_VERSION == 10
    // D3DReflect doesn't seem to work for DX10. Use the deprecated function.
    if ( FAILED( D3D10ReflectShader(pDesc->pBinary, pDesc->BinarySize, &preflector )))
        return false;
#endif
    ID3D1x(ShaderReflectionConstantBuffer)* pcb = preflector->GetConstantBufferByIndex(0);
    for ( unsigned uniform = 0; uniform < Uniform::SU_Count; ++uniform)
    {
        ID3D1x(ShaderReflectionVariable)* pvar = pcb->GetVariableByName(ShaderUniformNames[uniform]);
        D3D1x(SHADER_VARIABLE_DESC) varDesc;
        if ( !pvar || FAILED(pvar->GetDesc(&varDesc)))
        {
            // Can't actually check this because of texture uniforms.
            //SF_ASSERT(pd->Uniforms[uniform].Location == -1);
            UniformOffset[uniform] = -1;
            continue;
        }
        UniformOffset[uniform] = varDesc.StartOffset;
        SF_DEBUG_ASSERT2((int)varDesc.StartOffset == pd->Uniforms[uniform].Location * 16, "shader %d uniform %d location differs", pDesc->Type, uniform);
    }
#else
    for ( unsigned uniform = 0; uniform < Uniform::SU_Count; ++uniform)
    {
        UniformOffset[uniform] = pd->Uniforms[uniform].Location * 16;
    }
#endif
    return true;
}

void FragShader::Shutdown( )
{
    pProg = 0;
}

struct D3D1xVertexDeclBuilder
{
    D3D1x(INPUT_ELEMENT_DESC)    Elements[8];
    WORD                        Count;

    D3D1xVertexDeclBuilder() : Count(0) { }

    void    Add(LPCSTR name, UINT index, DXGI_FORMAT format)
    {
        D3D1x(INPUT_ELEMENT_DESC) &e = Elements[Count++];
        e.SemanticName = name;
        e.SemanticIndex = index;
        e.Format = format;
        e.InputSlot = 0;
        e.AlignedByteOffset = D3D1x(APPEND_ALIGNED_ELEMENT);
        e.InputSlotClass = D3D1x(INPUT_PER_VERTEX_DATA);
        e.InstanceDataStepRate = 0;
    }
};

SysVertexFormat::SysVertexFormat(ID3D1x(Device)* pdevice, const VertexFormat* vf, const VertexShaderDesc* pvdesc) : pVDecl(0)
{
    // The vertex format is ignored, except if it specifies a different format for the position; it is assumed that it will 
    // match the VertexShaderDesc's attribute definitions. They may not coincide exactly, for instance, the VSD's attributes 
    // for factors and batch indices may have already been combined.

    const VertexElement* ve = vf->GetElement(VET_Pos, VET_Usage_Mask);
    SF_ASSERT(ve); SF_UNUSED(ve); // must have position.
    
    D3D1xVertexDeclBuilder builder;
    const VertexShaderDesc& vdesc = *pvdesc;
    for (int attr = 0; attr < vdesc.NumAttribs; attr++)
    {
        builder.Add(vdesc.Attributes[attr].SemanticName, vdesc.Attributes[attr].SemanticIndex, 
                    (DXGI_FORMAT)vdesc.Attributes[attr].Format );
    }

    memcpy(&VertexElements, &builder.Elements, builder.Count * sizeof(D3D1x(INPUT_ELEMENT_DESC)));
    VertexElementCount = builder.Count;

    if ( FAILED( pdevice->CreateInputLayout(builder.Elements, builder.Count, pvdesc->pBinary, 
                                            pvdesc->BinarySize, &pVDecl.GetRawRef())))
    {
        SF_ASSERT(0);
        pVDecl = 0;
    }
    memcpy(VertexElements, builder.Elements, sizeof VertexElements);
}

bool ShaderInterface::SetStaticShader(VertexShaderDesc::ShaderType vshader, FragShaderDesc::ShaderType shader, const VertexFormat* pformat)
{
    CurShaders.pVFormat = pformat;
    CurShaders.pVS      = &pHal->StaticVShaders[VertexShaderDesc::GetShaderIndex(vshader)];
    CurShaders.pVDesc   = CurShaders.pVS->pDesc;
    CurShaders.pFS      = &pHal->StaticFShaders[FragShaderDesc::GetShaderIndex(shader)];
    CurShaders.pFDesc   = CurShaders.pFS->pDesc;

    if ( pformat && !pformat->pSysFormat )
        (const_cast<VertexFormat*>(pformat))->pSysFormat = *SF_NEW SysVertexFormat(pHal->GetDevice(), pformat, CurShaders.pVS->pDesc);

    return (bool)CurShaders;
}

void ShaderInterface::SetTexture(Shader, unsigned var, Render::Texture* ptexture, ImageFillMode fm, unsigned index)
{
    D3D1x::Texture *pd3dTexture = (D3D1x::Texture*)ptexture;
    SF_ASSERT(CurShaders.pFDesc->Uniforms[var].Location >= 0 );
    SF_ASSERT(CurShaders.pFDesc->Uniforms[var].Location + CurShaders.pFDesc->Uniforms[var].Size >= 
        (short)(index + ptexture->TextureCount));
    pd3dTexture->ApplyTexture(CurShaders.pFDesc->Uniforms[var].Location + index, fm);
}

// Record the min/max shader constant registers, and just set the entire buffer in one call,
// instead of doing them individually. This could result in some uninitialized data to be sent,
// but it should be unreferenced by the shaders, and should be more efficient.
struct ShaderConstantRange
{
    ShaderConstantRange(HAL* phal, float* data) : UniformData(data), pConstantBuffer(0), pHal(phal) { };
    void Update( int offset, int size, int shadowLocation)
    {
        if ( offset < 0 )
            return;

        if (pConstantBuffer == 0 )
        {
            pConstantBuffer = pHal->getNextConstantBuffer();
            D3D1xMapBuffer(pHal->pDeviceContext, pConstantBuffer, 0, D3D1x(MAP_WRITE_DISCARD), 0, &MappedBuffer);
        }
        memcpy((float*)MappedBuffer.pData + (offset/sizeof(float)), UniformData + shadowLocation, size*sizeof(float) );
    }
    void Finish(bool bfragment)
    {
        if (pConstantBuffer)
        {
            D3D1xUnmapBuffer(pHal->pDeviceContext, pConstantBuffer, 0);
            if ( bfragment )
                pHal->pDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
            else
                pHal->pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
        }
    }

    float* UniformData;
    D3D1x(MAPPED_BUFFER) MappedBuffer;
    ID3D1x(Buffer)*      pConstantBuffer;
    HAL*                 pHal;
};

void ShaderInterface::Finish(unsigned meshCount)
{
    // Mesh count is unneeded - the constant registers that have been updated are tracked,
    // which includes any registers set for batching.
    SF_UNUSED(meshCount);

    ShaderConstantRange shaderConstantRangeFS(pHal, UniformData);
    ShaderConstantRange shaderConstantRangeVS(pHal, UniformData);
    for (int i = 0; i < Uniform::SU_Count; i++)
    {
        if (UniformSet[i])
        {
            if (CurShaders.pFS->UniformOffset[i] >= 0)
            {
                shaderConstantRangeFS.Update( CurShaders.pFS->UniformOffset[i],
                                               CurShaders.pFDesc->Uniforms[i].Size,
                                               CurShaders.pFDesc->Uniforms[i].ShadowOffset);
            }
            else if (CurShaders.pVS->UniformOffset[i] >= 0 )
            {
                shaderConstantRangeVS.Update( CurShaders.pVS->UniformOffset[i],
                                               CurShaders.pVDesc->Uniforms[i].Size,
                                               CurShaders.pVDesc->Uniforms[i].ShadowOffset);
            }
        }
    }


    shaderConstantRangeVS.Finish(false);
    shaderConstantRangeFS.Finish(true);
    memset(UniformSet, 0, sizeof(UniformSet));

    ID3D1x(DeviceContext)* pdevice = pHal->pDeviceContext;
    if (pLastVS != CurShaders.pVS->pProg)
    {
        D3D1xVSSetShader( pdevice, CurShaders.pVS->pProg);
        pLastVS = CurShaders.pVS->pProg;
    }

    ID3D1x(InputLayout)* pdecl = ((SysVertexFormat*)(CurShaders.pVFormat->pSysFormat.GetPtr()))->pVDecl;
    if (pLastDecl != pdecl)
    {
        pdevice->IASetInputLayout(pdecl);
        pLastDecl = pdecl;
    }

    if (pLastFS != CurShaders.pFS->pProg)
    {
        D3D1xPSSetShader(pdevice, CurShaders.pFS->pProg);
        pLastFS = CurShaders.pFS->pProg;
    }
}

// *** ShaderManager
bool ShaderManager::HasInstancingSupport() const
{
    return true;
}

void ShaderManager::MapVertexFormat(PrimitiveFillType fill, const VertexFormat* sourceFormat,
                                    const VertexFormat** single, const VertexFormat** batch, 
                                    const VertexFormat** instanced)
{
    // D3D1x's shader input types need to match their input layouts. So, just convert all position data
    // to float (instead of using short). Extra shaders could be generated to support both, however, that
    // would require double the number of vertex shaders.
    VertexElement floatPositionElements[8];
    VertexFormat floatPositionFormat;
    floatPositionFormat.pElements = floatPositionElements;
    floatPositionFormat.pSysFormat = 0;
    unsigned offset = 0;
    unsigned i;
    for ( i = 0; sourceFormat->pElements[i].Attribute != VET_None; ++i )
    {
        floatPositionElements[i].Attribute = sourceFormat->pElements[i].Attribute;
        if ( (floatPositionElements[i].Attribute&VET_Usage_Mask) == VET_Pos)
        {
            floatPositionElements[i].Attribute &= ~VET_CompType_Mask;
            floatPositionElements[i].Attribute |= VET_F32;
            floatPositionElements[i].Offset = sourceFormat->pElements[i].Offset + offset;
            if ( (floatPositionElements[i].Attribute&VET_CompType_Mask) == VET_S16)
                offset += 4;
        }
        else
        {
            floatPositionElements[i].Offset = sourceFormat->pElements[i].Offset + offset;
        }
    }
    floatPositionElements[i].Attribute = VET_None;
    floatPositionElements[i].Offset    = 0;

    // Now let the base class actually do the mapping.
    StaticShaderManager::MapVertexFormat(fill, &floatPositionFormat, single, batch, instanced, MVF_HasInstancing | MVF_Align);

    // TEMP
    //*instanced = 0;
}

bool ShaderManager::Initialize(HAL* phal)
{
    pDevice = phal->GetDevice();
    return true;
}

void ShaderManager::BeginScene()
{
    
}

void ShaderManager::EndScene()
{

}

void ShaderManager::Reset()
{
    pDevice = 0;
    VFormats.Clear();
}

}}}
