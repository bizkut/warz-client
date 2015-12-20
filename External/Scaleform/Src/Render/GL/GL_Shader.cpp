/**************************************************************************

Filename    :   GL_Shader.cpp
Content     :   
Created     :   
Authors     :   

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#if !defined(SF_USE_GLES)   // Do not compile under GLES 1.1

#include "Render/GL/GL_Shader.h"
#include "Render/GL/GL_HAL.h"
#include "Kernel/SF_Debug.h"

#if defined(GL_ES_VERSION_2_0)
#include "Render/GL/GLES_ExtensionMacros.h"
#include "Render/GL/GLES_ShaderDescs.h"
#else
#include "Render/GL/GL_ExtensionMacros.h"
#include "Render/GL/GL_ShaderDescs.h"
#endif

#include "Render/Render_FiltersLE.h"
#include "Kernel/SF_MsgFormat.h"

namespace Scaleform { namespace Render { namespace GL {

extern const char* ShaderUniformNames[Uniform::SU_Count];
unsigned ShaderInterface::MaxRowsPerInstance = 0;
static const unsigned MaxShaderCodeSize = 4096; // Assume all shaders have a buffer smaller than this.

// Replaces the array size of a shader variable with the new count. This method assumes that 
// psrcPtr is a buffer with at least MaxShaderCodeSize bytes, and arrayString is the name of
// an shader variable which is an array.
void overwriteArrayCount(char* psrcPtr, const char* arrayString, unsigned newCount)
{
    char tempBuffer[MaxShaderCodeSize];
    if ( !psrcPtr )
        return;

    char * matFind = strstr(psrcPtr, arrayString);
    if (!matFind)
        return;
    
    SF_DEBUG_ASSERT(newCount > 0, "Can't have an array of size zero.");
    char tempNumber[16];
    SFsprintf(tempNumber, 16, "[%d]", newCount);
    UPInt size = (matFind - psrcPtr) + SFstrlen(arrayString);
    SFstrncpy(tempBuffer, MaxShaderCodeSize, psrcPtr, size);
    tempBuffer[size] = 0;
    SFstrcat(tempBuffer, MaxShaderCodeSize, tempNumber);

    char* endPtr = SFstrchr(matFind, ']');
    SF_DEBUG_ASSERT1(endPtr != 0, "Expected shader variable to be an array %s, but closing bracket not found.", arrayString );
    if (!endPtr)
        return;
    SFstrcat(tempBuffer, MaxShaderCodeSize, endPtr+1);

    // Overwrite the original buffer with the modified code.
    SFstrcpy(psrcPtr, MaxShaderCodeSize, tempBuffer);
}

bool ShaderObject::Init(HAL* phal, VertexShaderType vsType, FragShaderType fsType)
{
    pHal = phal;
    if (Prog)
    {
        glDeleteProgram(Prog);
        Prog = 0;
    }

    int vs = VertexShaderDesc::GetShaderIndex(vsType);
    int fs = FragShaderDesc::GetShaderIndex(fsType);
    if ( vs >= VertexShaderDesc::VSI_Count || fs >= FragShaderDesc::FSI_Count )
        return false;
    
    pVDesc = VertexShaderDesc::Descs[vs];
    pFDesc = FragShaderDesc::Descs[fs];

    if ( !pVDesc || !pFDesc )
        return false;

    GLint result;
    GLuint vp = 0;

    // Note: although we may actually modify the vertex shader source, we use the hash on the original
    // string pointer, so that we will find duplicate shaders. This assumes that all vertex shaders will
    // be modified in a consistent manner.
    if (!pHal->CompiledShaderHash.Get((void*)pVDesc->pSource, &vp))
    {
        // By default, the batch shaders are compiled with a batch count of 30. However, depending
        // on the maximum number of uniforms supported, this may not be possible, and the shader source
        // will not compile. Thus, we need to modify the incoming source, so it can compile.
        const char * vdescpSource = (const char*)pVDesc->pSource;
        char modifiedShaderSource[MaxShaderCodeSize];
        if ( pVDesc->Flags & Shader_Batch )
        {
            unsigned maxUniforms = (phal->Caps & Cap_MaxUniforms) >> Cap_MaxUniforms_Shift;
            unsigned maxInstances = Alg::Min<unsigned>(SF_RENDER_MAX_BATCHES, 
                                                       maxUniforms / ShaderInterface::GetMaximumRowsPerInstance());

            if ( maxInstances < SF_RENDER_MAX_BATCHES)
            {
			    SF_DEBUG_WARNONCE4(1, "For the default batch count of %d, %d uniforms are required."
						      "System only supports %d uniforms, batch count will be reduced to %d\n",
						      SF_RENDER_MAX_BATCHES, SF_RENDER_MAX_BATCHES *
						      ShaderInterface::GetMaximumRowsPerInstance(), maxUniforms,
						      maxInstances);
    							  
                vdescpSource = modifiedShaderSource;
                UPInt originalLength = SFstrlen(pVDesc->pSource);
                SF_ASSERT(originalLength < MaxShaderCodeSize);
                SFstrncpy(modifiedShaderSource, MaxShaderCodeSize, pVDesc->pSource, originalLength);
                modifiedShaderSource[originalLength] = 0;
                overwriteArrayCount(modifiedShaderSource, "vfmuniforms", 
                    ShaderInterface::GetCountPerInstance(pVDesc, Uniform::SU_vfmuniforms) * maxInstances);
                overwriteArrayCount(modifiedShaderSource, "vfuniforms", 
                    ShaderInterface::GetCountPerInstance(pVDesc, Uniform::SU_vfuniforms) * maxInstances);
            }
        }

        vp = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vp, 1, const_cast<const char**>(&vdescpSource), 0);
        glCompileShader(vp);
        glGetShaderiv(vp, GL_COMPILE_STATUS, &result);
        if (!result)
        {
            GLchar msg[512];
            glGetShaderInfoLog(vp, sizeof(msg), 0, msg);
            SF_DEBUG_ERROR2(1, "%s: %s\n", vdescpSource, msg);
            glDeleteShader(vp);
            return 0;
        }
        pHal->CompiledShaderHash.Set((void*)pVDesc->pSource, vp);
    }

    GLuint fp = 0;
    if (!pHal->CompiledShaderHash.Get((void*)pFDesc->pSource, &fp))
    {
        fp = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fp, 1, const_cast<const char**>(&pFDesc->pSource), 0);
        glCompileShader(fp);
        glGetShaderiv(fp, GL_COMPILE_STATUS, &result);
        if (!result)
        {
            GLchar msg[1512];
            glGetShaderInfoLog(fp, sizeof(msg), 0, msg);
            SF_DEBUG_ERROR2(1, "%s: %s\n", pFDesc->pSource, msg);
            glDeleteShader(fp);
            return 0;
        }
        pHal->CompiledShaderHash.Set((void*)pFDesc->pSource, fp);
    }

    Prog = glCreateProgram();
    glAttachShader(Prog, vp);
    glAttachShader(Prog, fp);
    
    for (int i = 0; i < pVDesc->NumAttribs; i++)
        glBindAttribLocation(Prog, i, pVDesc->Attributes[i].Name);

    glLinkProgram(Prog);

    // The shaders will not actually be deleted until the program is destroyed.
    // We check the status of deletion, because some platforms (iOS) generate 
    // errors for deleting shaders multiple times.
    GLint fstatus, vstatus;
    glGetShaderiv(fp, GL_DELETE_STATUS, &fstatus);
    glGetShaderiv(vp, GL_DELETE_STATUS, &vstatus);
    if (fstatus == GL_FALSE)
        glDeleteShader(fp);
    if (vstatus == GL_FALSE)
        glDeleteShader(vp);
    
    if (!InitUniforms())
    {
        glDeleteProgram(Prog);
        Prog = 0;
        return false;
    }
    return true;
}

bool ShaderObject::InitUniforms()
{
    GLint result;
    glGetProgramiv(Prog, GL_LINK_STATUS, &result);
    if (!result)
    {
        GLchar msg[512];
        glGetProgramInfoLog(Prog, sizeof(msg), 0, msg);
        SF_DEBUG_ERROR1(1, "link: %s\n", msg);
        return false;
    }

    for (unsigned i = 0; i < Uniform::SU_Count; i++)
    {
        if (pFDesc->Uniforms[i].Location >= 0 || pVDesc->Uniforms[i].Location >= 0)
        {
            Uniforms[i] = glGetUniformLocation(Prog, ShaderUniformNames[i]);

            // It seems that the binary shaders could store uniform names with '[0]' appended on them
            // So, if we fail to find the uniform with the original name, search for that.
            if (Uniforms[i] < 0)
            {
                char arrayname[128];
                SFstrcpy(arrayname, 128, ShaderUniformNames[i]);
                SFstrcat(arrayname, 128, "[0]");
                Uniforms[i] = glGetUniformLocation(Prog, arrayname);
            }

            // Couldn't find the uniform at all.
            if (Uniforms[i] < 0 )
            {
                SF_DEBUG_ERROR1(1, "Failed to find uniform %s (program uniforms):", ShaderUniformNames[i]);
                dumpUniforms();
                return false;
            }

            if (pFDesc->Uniforms[i].Location >= 0)
            {
                SF_ASSERT(pVDesc->Uniforms[i].Location < 0);
                AllUniforms[i] = pFDesc->Uniforms[i];
            }
            else
                AllUniforms[i] = pVDesc->Uniforms[i];
        }
        else
            Uniforms[i] = -1;
    }
    return result != 0;
}

void ShaderObject::Shutdown( )
{
    if (Prog)
        glDeleteProgram(Prog);

    Prog   = 0;
    pVDesc = 0;
    pFDesc = 0;
    pHal   = 0;
}

ShaderObject::~ShaderObject()
{
    Shutdown();
}

#ifdef SF_GL_BINARY_SHADERS
bool ShaderObject::InitBinary(unsigned comboIndex, File* pfile, void*& buffer, GLsizei& bufferSize)
{
    if (Prog)
    {
        glDeleteProgram(Prog);
        Prog = 0;
    }

    VertexShaderDesc::ShaderIndex vidx = VertexShaderDesc::GetShaderIndexForComboIndex(comboIndex);
    FragShaderDesc::ShaderIndex fidx = FragShaderDesc::GetShaderIndexForComboIndex(comboIndex);
    pVDesc = VertexShaderDesc::Descs[vidx];
    pFDesc = FragShaderDesc::Descs[fidx];

    if (!pVDesc || !pFDesc)
        return false;


    //SF_DEBUG_MESSAGE4(1, "Shader indices (v=%d - type=%d, f=%d - type = %d)", vidx, pVDesc->Type, fidx, pFDesc->Type);
    Prog = glCreateProgram();

    GLenum format = pfile->ReadSInt32();
    GLsizei size = pfile->ReadSInt32();

    //SF_DEBUG_MESSAGE2(1, "Loading binary shader (size=%d, format=%d)", size, format);
    if (bufferSize < size)
    {
        bufferSize = (size + 1023) & ~1023;
        //SF_DEBUG_MESSAGE1(1, "Allocating shader buffer, size %d", bufferSize );
        buffer = buffer == 0 ? SF_ALLOC(bufferSize, Stat_Default_Mem) : SF_REALLOC(buffer, bufferSize, Stat_Default_Mem);
    }
    if (pfile->Read((UByte*)buffer, size) < size)
    {
        SF_DEBUG_WARNING(1, "Error reading from binary shader file (insufficient space remaining).");
        glDeleteProgram(Prog);
        Prog = 0;
        return false;
    }

    glProgramBinaryOES(Prog, format, buffer, size);

    if (!InitUniforms())
    {
        glDeleteProgram(Prog);
        Prog = 0;
        return false;
    }

    //dumpUniforms();
    return true;
}

bool ShaderObject::SaveBinary(File* pfile, void*& buffer, GLsizei& bufferSize, GLsizei& totalSize)
{
    GLsizei size;
    glGetProgramiv(Prog, GL_PROGRAM_BINARY_LENGTH_OES, &size);

    if (bufferSize < size)
    {
        bufferSize = (size + 1023) & ~1023;
        buffer = buffer == 0 ? SF_ALLOC(bufferSize, Stat_Default_Mem) : SF_REALLOC(buffer, bufferSize, Stat_Default_Mem);
    }

    GLenum format;
    memset(buffer, 0, bufferSize);
    glGetProgramBinaryOES(Prog, bufferSize, 0, &format, buffer);
    if (size > 0)
    {
        pfile->WriteSInt32(format);
        pfile->WriteSInt32(size);
        //SF_DEBUG_MESSAGE2(1, "Saving binary shader (size=%d, format=%d) - uniforms:", size, format);
        //dumpUniforms();

        if (pfile->Write((UByte*)buffer, size) < size)
            return false;
        totalSize += size;

        return true;
    }
    return false;
}
#endif

void ShaderObject::dumpUniforms()
{
#if defined(SF_USE_GLES2)
    GLint uniformCount;
    glGetProgramiv(Prog, GL_ACTIVE_UNIFORMS, &uniformCount);
    SF_DEBUG_MESSAGE1(1, "Shader has %d uniforms:", uniformCount);
    for ( int uniform = 0; uniform < uniformCount; ++uniform)
    {
        char uniformName[128];
        GLsizei length;
        GLint size;
        GLenum type;
        glGetActiveUniform(Prog, uniform, 128, &length, &size, &type, uniformName);
        SF_DEBUG_MESSAGE3(1,"\t%16s (size=%d, type=%d)", uniformName, size, type);
    }
#endif
}


bool ShaderInterface::SetStaticShader(VertexShaderDesc::ShaderType, FragShaderDesc::ShaderType shader, const VertexFormat*)
{
    CurShader.pShaderObj = pHal->GetStaticShader(shader);
    if ( !CurShader.pShaderObj || !CurShader.pShaderObj->Prog )
    {
        CurShader.pVDesc = 0;
        CurShader.pFDesc = 0;
        glUseProgram(0);
        SF_ASSERT(0);
        return false;
    }
    CurShader.pVDesc = CurShader.pShaderObj->pVDesc;
    CurShader.pFDesc = CurShader.pShaderObj->pFDesc;
    glUseProgram(CurShader.pShaderObj->Prog);

    return true;
}

void ShaderInterface::SetTexture(Shader sd, unsigned var, Render::Texture* ptex, ImageFillMode fm, unsigned index)
{
    GL::Texture* ptexture = (GL::Texture*)ptex;
    GL::TextureManager* pmanager = (GL::TextureManager*)ptexture->GetManager();

    GLint minfilter = (fm.GetSampleMode() == Sample_Point) ? GL_NEAREST : (ptexture->MipLevels>1 ? GL_LINEAR_MIPMAP_LINEAR  : GL_LINEAR);
    GLint magfilter = (fm.GetSampleMode() == Sample_Point) ? GL_NEAREST : GL_LINEAR;
    GLint address = (fm.GetWrapMode() == Wrap_Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

    SF_ASSERT(CurShader.pShaderObj->Uniforms[var] >= 0 ); // Expected texture uniform does not exist in this shader.
    int *textureStages = 0;
    int *stageCount = 0;
    int baseLocation = sd->pFDesc->Uniforms[var].Location;

    // Find our texture uniform index.
    int tu;
    for ( tu = 0; tu < FragShaderDesc::MaxTextureSamplers; ++tu)
    {
        if ( TextureUniforms[tu].UniformVar < 0 || TextureUniforms[tu].UniformVar == (int)var )
        {
            TextureUniforms[tu].UniformVar = var;
            textureStages = TextureUniforms[tu].SamplerStages;
            stageCount = &TextureUniforms[tu].StagesUsed;
            break;
        }
    }
    SF_DEBUG_ASSERT(tu < FragShaderDesc::MaxTextureSamplers, "Unexpected number of texture uniforms used.");

    for (unsigned plane = 0; plane < ptexture->GetTextureStageCount() ; plane++)
    {
        int stageIndex = baseLocation + index + plane;
        textureStages[plane + index] = stageIndex;
        *stageCount = Alg::Max<int>(*stageCount, index+plane+1);

        pmanager->ApplyTexture(stageIndex, ptexture->pTextures[plane].TexId);
        if (ptexture->LastMinFilter != minfilter || ptexture->LastAddress != address)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);              
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, address);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, address);

            ptexture->LastMinFilter = minfilter;
            ptexture->LastAddress = address;
        }
    }
}

void ShaderInterface::Finish(unsigned batchCount)
{
    SF_ASSERT(CurShader.pShaderObj->Prog);

    const ShaderObject* pCurShader = CurShader.pShaderObj;
    for (int var = 0; var < Uniform::SU_Count; var++)
    {
        if (UniformSet[var])
        {
            unsigned size;
            if (pCurShader->AllUniforms[var].BatchSize > 0)
                size = batchCount * pCurShader->AllUniforms[var].BatchSize;
            else if (pCurShader->AllUniforms[var].ElementSize)
                size = pCurShader->AllUniforms[var].Size / pCurShader->AllUniforms[var].ElementSize;
            else
                continue;

            switch (pCurShader->AllUniforms[var].ElementCount)
            {
                case 16:
                    glUniformMatrix4fv(pCurShader->Uniforms[var], size, false /* transpose */,
                        UniformData + pCurShader->AllUniforms[var].ShadowOffset);
                    break;
                case 4:
                    glUniform4fv(pCurShader->Uniforms[var], size,
                        UniformData + pCurShader->AllUniforms[var].ShadowOffset);
                    break;
                case 3:
                    glUniform3fv(pCurShader->Uniforms[var], size,
                        UniformData + pCurShader->AllUniforms[var].ShadowOffset);
                    break;
                case 2:
                    glUniform2fv(pCurShader->Uniforms[var], size,
                        UniformData + pCurShader->AllUniforms[var].ShadowOffset);
                    break;
                case 1:
                    glUniform1fv(pCurShader->Uniforms[var], size,
                        UniformData + pCurShader->AllUniforms[var].ShadowOffset);
                    break;

                default:
                    SF_ASSERT(0);
            }
        }
    }

    // Set sampler stage uniforms.
    for (int tu = 0; tu < FragShaderDesc::MaxTextureSamplers; ++tu)
    {
        if ( TextureUniforms[tu].UniformVar < 0 )
            break;

        glUniform1iv( pCurShader->Uniforms[TextureUniforms[tu].UniformVar], 
            TextureUniforms[tu].StagesUsed, TextureUniforms[tu].SamplerStages );
    }

    memset(UniformSet, 0, Uniform::SU_Count);
    memset(TextureUniforms, -1, sizeof(TextureUniforms));
}

unsigned ShaderInterface::GetMaximumRowsPerInstance()
{
    // Check for cached value. This should not change between runs. TBD: precalculate.
    if ( MaxRowsPerInstance == 0 )
    {
        // Note: this assumes that batch variables are stored in shader descs.
        MaxRowsPerInstance = 0;
        for ( unsigned desc = 0; desc < VertexShaderDesc::VSI_Count; ++desc )
        {
            const VertexShaderDesc* pvdesc = VertexShaderDesc::Descs[desc];
            MaxRowsPerInstance = Alg::Max(MaxRowsPerInstance, GetRowsPerInstance(pvdesc));
        }
    }
    return MaxRowsPerInstance;
}

bool ShaderInterface::GetDynamicLoopSupport()
{
    // Check cached value. -1 indicates not calculated yet.
    if ( DynamicLoops < 0 )
    {
        // Just try to compile a shader we know has dynamic loops, and see if it fails.
        for ( int i = 0; i < FragShaderDesc::FSI_Count; ++i )
        {
            if ( FragShaderDesc::Descs[i] && FragShaderDesc::Descs[i]->Flags & Shader_DynamicLoop )
            {
                DynamicLoops = pHal->StaticShaders[i].Init(pHal, (VertexShaderType)FragShaderDesc::Descs[i]->Type, 
                                                           (FragShaderType)FragShaderDesc::Descs[i]->Type ) ? 1 : 0;            
                break;
            }
        }
    }
    return DynamicLoops ? true : false;
}

unsigned ShaderInterface::GetRowsPerInstance( const VertexShaderDesc* pvdesc )
{
    // Desc doesn't exist, or isn't batched, don't consider it.
    if ( !pvdesc || (pvdesc->Flags & Shader_Batch) == 0 )
        return 0;

    unsigned currentUniforms = 0;
    for ( unsigned uniform = 0; uniform < Uniform::SU_Count; ++uniform )
    {
        if ( pvdesc->BatchUniforms[uniform].Size > 0 )
        {
            currentUniforms += pvdesc->BatchUniforms[uniform].Size *
                (pvdesc->BatchUniforms[uniform].Array == Uniform::SU_vfmuniforms ? 4 : 1);
        }
    }
    return currentUniforms;
}

// Returns the number of entries per instance of the given uniform type.
unsigned ShaderInterface::GetCountPerInstance(const VertexShaderDesc* pvdesc, Uniform::UniformType arrayType)
{
    // Desc doesn't exist, or isn't batched, don't consider it.
    if ( !pvdesc || (pvdesc->Flags & Shader_Batch) == 0 )
        return 0;

    unsigned currentUniforms = 0;
    for ( unsigned uniform = 0; uniform < Uniform::SU_Count; ++uniform )
    {
        if ( pvdesc->BatchUniforms[uniform].Size > 0 )
        {
            if ( pvdesc->BatchUniforms[uniform].Array == arrayType )
                currentUniforms += pvdesc->BatchUniforms[uniform].Size;
        }
    }
    return currentUniforms;
}


unsigned ShaderManager::SetupFilter(const Filter* filter, unsigned fillFlags, unsigned* passes, BlurFilterState& leBlur) const
{
    // If we don't support dynamic loops only allow color matrix filters (which don't have dynamic loops).
    if ( (Caps & Cap_NoDynamicLoops) == 0 || filter->GetFilterType() == Filter_ColorMatrix )
    {
        leBlur.Passes = 0;
        return StaticShaderManagerType::GetFilterPasses(filter, fillFlags, passes);
    }

    if (leBlur.Setup(filter))
        return leBlur.Passes;

    return 0;
}

bool ShaderManager::HasInstancingSupport() const
{
    // TODO: enable instancing.
    // Caps generated on InitHAL.
    //return (Caps & Cap_Instancing) != 0;
    return false;
}

void ShaderInterface::ResetContext()
{
    for (BlurShadersHash::Iterator It = BlurShaders.Begin(); It != BlurShaders.End(); ++It)
        delete It->Second;

    BlurShaders.Clear();
}

const BlurFilterShader* ShaderInterface::GetBlurShader(const BlurFilterShaderKey& params)
{
    const BlurFilterShader*const* psh = BlurShaders.Get(params);
    if (psh)
        return *psh;

    StringBuffer vsrc, fsrc;

    fsrc.AppendString(
#if defined(GL_ES_VERSION_2_0)
        "precision mediump float;\n"
#endif
        "uniform sampler2D tex;\n"
        "uniform vec2 texscale;\n"
        "uniform vec4 cxmul;\n"
        "uniform vec4 cxadd;\n"
        "uniform float samples;\n");

    vsrc.AppendString("uniform vec4 mvp[2];\nuniform vec2 texscale;\n");
    for (int i = 0; i < params.TotalTCs; i++)
    {
        SPrintF(vsrc, "attribute vec2 intc%d;\n", i);
        SPrintF(vsrc, "varying   vec2 tc%d;\n", i);
        SPrintF(fsrc, "varying   vec2 tc%d;\n", i);
    }

    if (params.Mode & Filter_Shadow)
    {
        vsrc.AppendString("uniform vec2 srctexscale;\n");

        fsrc.AppendString("uniform vec4 scolor;\n"
                          "uniform sampler2D srctex;\n");

        if (params.Mode & BlurFilterParams::Mode_Highlight)
        {
            fsrc.AppendString("uniform vec4 scolor2;\n");
        }
    }

    vsrc.AppendString("attribute vec4 pos;\n"
                      "void main(void)\n{\n"
                      "  vec4 opos = pos;\n"
                      "  opos.x = dot(pos, mvp[0]);\n"
                      "  opos.y = dot(pos, mvp[1]);\n"
                      "  gl_Position = opos;\n");
    for (int i = 0; i < params.TotalTCs-params.BaseTCs; i++)
        SPrintF(vsrc, "  tc%d = intc%d * texscale;\n", i, i);
    for (int i = params.TotalTCs-params.BaseTCs; i < params.TotalTCs; i++)
        SPrintF(vsrc, "  tc%d = intc%d * srctexscale;\n", i, i);

    vsrc.AppendString("\n}");

    fsrc.AppendString("void main(void)\n{\n"
        "  vec4 color = vec4(0);\n");

    if (params.Mode & Filter_Shadow)
    {
        const char *color;
        if (params.Mode & BlurFilterParams::Mode_Highlight)
        {
            for (int i = 0; i < params.BoxTCs; i++)
            {
                SPrintF(fsrc, "  color.a += texture2D(tex, tc%d).a;\n", i*2);
                SPrintF(fsrc, "  color.r += texture2D(tex, tc%d).a;\n", i*2+1);
            }
            color = "(scolor * color.a + scolor2 * color.r)";
        }
        else
        {
            for (int i = 0; i < params.BoxTCs; i++)
                SPrintF(fsrc, "  color += texture2D(tex, tc%d);\n", i);
            color = "(scolor * color.a)";
        }

        SPrintF(fsrc,   "  color *= samples;\n");

        if (params.Mode & BlurFilterParams::Mode_HideObject)
            SPrintF(fsrc, "  gl_FragColor = %s;}\n", color);
        else
        {
            SPrintF(fsrc,   "  vec4 base = texture2D(srctex, tc%d);\n", params.TotalTCs-1);

            if (params.Mode & BlurFilterParams::Mode_Inner)
            {
                if (params.Mode & BlurFilterParams::Mode_Highlight)
                {
                    fsrc.AppendString("  color.ar = clamp((1.0 - color.ar) - (1.0 - color.ra) * 0.5, 0.0,1.0);\n");
                    fsrc.AppendString("  color = (scolor * (color.a) + scolor2 * (color.r)\n"
                        "           + base * (1.0 - color.a - color.r)) * base.a;\n");
                }
                else if (params.Mode & BlurFilterParams::Mode_Knockout)
                {
                    fsrc.AppendString("  color = scolor * (1-color.a) * base.a;\n");
                }
                else
                    fsrc.AppendString("  color = mix(scolor, base, color.a) * base.a;\n");

                fsrc.AppendString("  gl_FragColor = color * cxmul + cxadd * color.a;\n}");
            }
            else
            {
                SPrintF  (fsrc, "  color = %s * (1.0-base.a) + base;\n", color);

                if (params.Mode & BlurFilterParams::Mode_Knockout)
                {
                    fsrc.AppendString("  color *= (1.0 - base.a);\n"
                        "  gl_FragColor = color * cxmul + cxadd * color.a;\n}");
                }
                else
                    fsrc.AppendString("  gl_FragColor = color * cxmul + cxadd * color.a;\n}");
            }
        }
    }
    else
    {
        for (int i = 0; i < params.BoxTCs; i++)
            SPrintF(fsrc, "  color += texture2D(tex, tc%d);\n", i);
        SPrintF(fsrc,       "  color = color * samples;\n");
        fsrc.AppendString(    "  gl_FragColor = color * cxmul + cxadd * color.a;\n}\n");
    }

    const GLchar* pvsource = (const GLchar*)(vsrc.ToCStr());
    const GLchar* pfsource = (const GLchar*)(fsrc.ToCStr());

    //SF_DEBUG_MESSAGE2(1, "Blur shader\n%s\n\n%s\n\n", pvsource, pfsource);

    GLuint vp = glCreateShader(GL_VERTEX_SHADER);
    GLint result;
    glShaderSource(vp, 1, &pvsource, 0);
    glCompileShader(vp);
    glGetShaderiv(vp, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        GLchar msg[512];
        glGetShaderInfoLog(vp, sizeof(msg), 0, msg);
        SF_DEBUG_ERROR2(1, "%s: %s\n", pvsource, msg);
        glDeleteShader(vp);
        return 0;
    }
    GLint prog = glCreateProgram();
    glAttachShader(prog, vp);
    GLuint fp = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fp, 1, &pfsource, 0);
    glCompileShader(fp);
    glGetShaderiv(fp, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        GLchar msg[1512];
        glGetShaderInfoLog(fp, sizeof(msg), 0, msg);
        SF_DEBUG_ERROR2(1, "%s: %s\n", pfsource, msg);
        glDeleteShader(fp);
        glDeleteProgram(prog);
        prog = 0;
        return 0;
    }
    glAttachShader(prog, fp);

    glBindAttribLocation(prog, 0, "pos");
    for (int i = 0; i < params.TotalTCs; i++)
    {
        char tc[16];
        SPrintF(tc, "intc%d", i);
        glBindAttribLocation(prog, 1+i, tc);
    }

    glLinkProgram(prog);
    GLint fstatus, vstatus;
    glGetShaderiv(fp, GL_DELETE_STATUS, &fstatus);
    glGetShaderiv(vp, GL_DELETE_STATUS, &vstatus);
    if (fstatus == GL_FALSE)
        glDeleteShader(fp);
    if (vstatus == GL_FALSE)
        glDeleteShader(vp);
    glGetProgramiv(prog, GL_LINK_STATUS, &result);
    if (!result)
    {
        GLchar msg[512];
        glGetProgramInfoLog(prog, sizeof(msg), 0, msg);
        SF_DEBUG_ERROR3(1, "link:\n%s\n%s %s\n", pvsource, pfsource, msg);
        glDeleteProgram(prog);
        return 0;
    }

    BlurFilterShader* pfs = new BlurFilterShader;
    pfs->Shader = prog;

    pfs->mvp = glGetUniformLocation(prog, "mvp");
    pfs->cxmul = glGetUniformLocation(prog, "cxmul");
    pfs->cxadd = glGetUniformLocation(prog, "cxadd");
    pfs->tex[0] = glGetUniformLocation(prog, "tex");
    pfs->tex[1] = glGetUniformLocation(prog, "srctex");
    pfs->texscale[0] = glGetUniformLocation(prog, "texscale");
    pfs->texscale[1] = glGetUniformLocation(prog, "srctexscale");
    pfs->samples = glGetUniformLocation(prog, "samples");

    pfs->scolor[0] = glGetUniformLocation(prog, "scolor");
    pfs->scolor[1] = glGetUniformLocation(prog, "scolor2");

    BlurShaders.Add(params, pfs);
    return pfs;
}

}}}

#endif // !defined(SF_USE_GLES)
