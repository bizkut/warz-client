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

#if defined(RENDERER_ENABLE_DIRECT3D9)

#include "D3D9RendererMaterial.h"
#include <RendererMaterialDesc.h>

#include "D3D9RendererTexture2D.h"

#include <stdio.h>

// for PsString.h
namespace physx
{
namespace string
	{}
}
#include <PsString.h>
#include <PsFile.h>

using namespace SampleRenderer;

void D3D9RendererMaterial::setModelMatrix(const float* matrix)
{
	if (m_instancedVertexConstants.table && m_instancedVertexConstants.modelMatrix)
	{
		m_instancedVertexConstants.table->SetMatrix(m_renderer.getD3DDevice(), m_instancedVertexConstants.modelMatrix, (const D3DXMATRIX*)matrix);
	}
}

D3D9RendererMaterial::ShaderConstants::ShaderConstants(void)
{
	memset(this, 0, sizeof(*this));
}

D3D9RendererMaterial::ShaderConstants::~ShaderConstants(void)
{
	if (table)
	{
		table->Release();
	}
}

static D3DXHANDLE getShaderConstantByName(ID3DXConstantTable& table, const char* name)
{
	D3DXHANDLE found = 0;
	D3DXCONSTANTTABLE_DESC desc;
	table.GetDesc(&desc);
	for (UINT i = 0; i < desc.Constants; i++)
	{
		D3DXHANDLE constant = table.GetConstant(0, i);
		RENDERER_ASSERT(constant, "Unable to find constant");
		if (constant)
		{
			D3DXCONSTANT_DESC cdesc;
			UINT              count = 1;
			table.GetConstantDesc(constant, &cdesc, &count);
			RENDERER_ASSERT(count == 1, "Unable to obtain Constant Descriptor.");
			if (count == 1 && !strcmp(cdesc.Name, name))
			{
				found = constant;
				break;
			}
		}
	}
	return found;
}

static D3DBLEND getD3DBlendFunc(RendererMaterial::BlendFunc func)
{
	D3DBLEND d3dfunc = D3DBLEND_FORCE_DWORD;
	switch (func)
	{
	case RendererMaterial::BLEND_ZERO:
		d3dfunc = D3DBLEND_ZERO;
		break;
	case RendererMaterial::BLEND_ONE:
		d3dfunc = D3DBLEND_ONE;
		break;
	case RendererMaterial::BLEND_SRC_COLOR:
		d3dfunc = D3DBLEND_SRCCOLOR;
		break;
	case RendererMaterial::BLEND_ONE_MINUS_SRC_COLOR:
		d3dfunc = D3DBLEND_INVSRCCOLOR;
		break;
	case RendererMaterial::BLEND_SRC_ALPHA:
		d3dfunc = D3DBLEND_SRCALPHA;
		break;
	case RendererMaterial::BLEND_ONE_MINUS_SRC_ALPHA:
		d3dfunc = D3DBLEND_INVSRCALPHA;
		break;
	case RendererMaterial::BLEND_DST_ALPHA:
		d3dfunc = D3DBLEND_DESTALPHA;
		break;
	case RendererMaterial::BLEND_ONE_MINUS_DST_ALPHA:
		d3dfunc = D3DBLEND_INVDESTALPHA;
		break;
	case RendererMaterial::BLEND_DST_COLOR:
		d3dfunc = D3DBLEND_DESTCOLOR;
		break;
	case RendererMaterial::BLEND_ONE_MINUS_DST_COLOR:
		d3dfunc = D3DBLEND_INVDESTCOLOR;
		break;
	case RendererMaterial::BLEND_SRC_ALPHA_SATURATE:
		d3dfunc = D3DBLEND_SRCALPHASAT;
		break;
	}
	RENDERER_ASSERT(d3dfunc != D3DBLEND_FORCE_DWORD, "Failed to look up D3D Blend Func.");
	return d3dfunc;
}

void D3D9RendererMaterial::ShaderConstants::loadConstants(void)
{
	if (table)
	{
		modelMatrix           = table->GetConstantByName(0, "g_" "modelMatrix");
		viewMatrix            = table->GetConstantByName(0, "g_" "viewMatrix");
		projMatrix            = table->GetConstantByName(0, "g_" "projMatrix");
		modelViewMatrix       = table->GetConstantByName(0, "g_" "modelViewMatrix");
		modelViewProjMatrix   = table->GetConstantByName(0, "g_" "modelViewProjMatrix");

		boneMatrices          = getShaderConstantByName(*table, "g_" "boneMatrices");

		fogColorAndDistance   = table->GetConstantByName(0, "g_" "fogColorAndDistance");

		eyePosition           = table->GetConstantByName(0, "g_" "eyePosition");
		eyeDirection          = table->GetConstantByName(0, "g_" "eyeDirection");

		ambientColor          = table->GetConstantByName(0, "g_" "ambientColor");

		lightColor            = table->GetConstantByName(0, "g_" "lightColor");
		lightIntensity        = table->GetConstantByName(0, "g_" "lightIntensity");
		lightDirection        = table->GetConstantByName(0, "g_" "lightDirection");
		lightPosition         = table->GetConstantByName(0, "g_" "lightPosition");
		lightInnerRadius      = table->GetConstantByName(0, "g_" "lightInnerRadius");
		lightOuterRadius      = table->GetConstantByName(0, "g_" "lightOuterRadius");
		lightInnerCone        = table->GetConstantByName(0, "g_" "lightInnerCone");
		lightOuterCone        = table->GetConstantByName(0, "g_" "lightOuterCone");

		lightShadowMap        = table->GetConstantByName(0, "g_" "lightShadowMap");
		lightShadowMatrix     = table->GetConstantByName(0, "g_" "lightShadowMatrix");

		vfaceScale            = table->GetConstantByName(0, "g_" "vfaceScale");
	}
}

void D3D9RendererMaterial::ShaderConstants::bindEnvironment(IDirect3DDevice9& d3dDevice, const D3D9Renderer::ShaderEnvironment& shaderEnv) const
{
	if (table)
	{
#define SET_MATRIX(_name)                         \
	if(_name)                                     \
	{                                             \
		const D3DXMATRIX xm(shaderEnv._name);     \
		table->SetMatrix(&d3dDevice, _name, &xm); \
	}

#define SET_FLOAT3(_name)                            \
	if(_name)                                        \
	{                                                \
		const D3DXVECTOR4 xv(shaderEnv._name[0],     \
		                     shaderEnv._name[1],     \
		                     shaderEnv._name[2], 1); \
		table->SetVector(&d3dDevice, _name, &xv);    \
	}

#define SET_FLOAT4(_name)                            \
	if(_name)                                        \
	{                                                \
		const D3DXVECTOR4 xv(shaderEnv._name[0],     \
		                     shaderEnv._name[1],     \
		                     shaderEnv._name[2],     \
		                     shaderEnv._name[3]);    \
		table->SetVector(&d3dDevice, _name, &xv);    \
	}

#define SET_COLOR(_name)                                \
	if(_name)                                           \
	{                                                   \
		const D3DXCOLOR xc(shaderEnv._name);            \
		const D3DXVECTOR4 xv(xc.r, xc.g, xc.b, xc.a);   \
		table->SetVector(&d3dDevice, _name, &xv);       \
	}

#define SET_FLOAT(_name) \
	if(_name) table->SetFloat(&d3dDevice, _name, shaderEnv._name);

		SET_MATRIX(modelMatrix)
		SET_MATRIX(viewMatrix)
		SET_MATRIX(projMatrix)
		SET_MATRIX(modelViewMatrix)
		SET_MATRIX(modelViewProjMatrix)

		if (boneMatrices && shaderEnv.numBones > 0)
		{
			table->SetMatrixArray(&d3dDevice, boneMatrices, shaderEnv.boneMatrices, shaderEnv.numBones);
		}

		SET_FLOAT4(fogColorAndDistance)


		SET_FLOAT3(eyePosition)
		SET_FLOAT3(eyeDirection)

		SET_COLOR(ambientColor)

		SET_COLOR(lightColor)
		SET_FLOAT(lightIntensity)
		SET_FLOAT3(lightDirection)
		SET_FLOAT3(lightPosition)
		SET_FLOAT(lightInnerRadius)
		SET_FLOAT(lightOuterRadius)
		SET_FLOAT(lightInnerCone)
		SET_FLOAT(lightOuterCone)
		if (lightShadowMap)
		{
			UINT index = table->GetSamplerIndex(lightShadowMap);
			d3dDevice.SetSamplerState((DWORD)index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			d3dDevice.SetSamplerState((DWORD)index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			d3dDevice.SetSamplerState((DWORD)index, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			d3dDevice.SetSamplerState((DWORD)index, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
			d3dDevice.SetSamplerState((DWORD)index, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
			d3dDevice.SetTexture(index, shaderEnv.lightShadowMap);
		}
		SET_MATRIX(lightShadowMatrix)

		if (vfaceScale)
		{
			float vfs = 1.0f;
			DWORD cullmode = 0;
			d3dDevice.GetRenderState(D3DRS_CULLMODE, &cullmode);
			if (cullmode == D3DCULL_CW)
			{
				vfs = -1.0f;
			}
#if defined(RENDERER_XBOX360)
			else if (cullmode == D3DCULL_NONE)
			{
				vfs = -1.0f;
			}
#endif
			table->SetFloat(&d3dDevice, vfaceScale, vfs);
		}

#undef SET_MATRIX
#undef SET_FLOAT3
#undef SET_FLOAT4
#undef SET_COLOR
#undef SET_FLOAT
	}
}

/**************************************
*  D3D9RendererMaterial::D3D9Variable *
**************************************/

D3D9RendererMaterial::D3D9Variable::D3D9Variable(const char* name, VariableType type, PxU32 offset) :
	Variable(name, type, offset)
{
	m_vertexHandle = 0;
	memset(m_fragmentHandles, 0, sizeof(m_fragmentHandles));
}

D3D9RendererMaterial::D3D9Variable::~D3D9Variable(void)
{

}

void D3D9RendererMaterial::D3D9Variable::addVertexHandle(ID3DXConstantTable& table, D3DXHANDLE handle)
{
	m_vertexHandle = handle;
	D3DXCONSTANT_DESC cdesc;
	UINT              count = 1;
	table.GetConstantDesc(handle, &cdesc, &count);
	m_vertexRegister = cdesc.RegisterIndex;
}

void D3D9RendererMaterial::D3D9Variable::addFragmentHandle(ID3DXConstantTable& table, D3DXHANDLE handle, Pass pass)
{
	m_fragmentHandles[pass] = handle;
	D3DXCONSTANT_DESC cdesc;
	UINT              count = 1;
	table.GetConstantDesc(handle, &cdesc, &count);
	m_fragmentRegisters[pass] = cdesc.RegisterIndex;
}

/*********************************
*  D3D Shader Compiler Callbacks *
*********************************/

static void processCompileErrors(LPD3DXBUFFER errors)
{
#if defined(RENDERER_WINDOWS)
	if (errors)
	{
		const char* errorStr = (const char*)errors->GetBufferPointer();
		if (errorStr)
		{
			static bool ignoreErrors = false;
			if (!ignoreErrors)
			{
				int ret = MessageBoxA(0, errorStr, "D3DXCompileShaderFromFile Error", MB_ABORTRETRYIGNORE);
				if (ret == IDABORT)
				{
					exit(0);
				}
				else if (ret == IDIGNORE)
				{
					ignoreErrors = true;
				}
			}
		}
	}
#elif defined(RENDERER_XBOX360)
	// this allows to watch errors in the debugger
	if(errors)
	{
		const char *errorStr = (const char*)errors->GetBufferPointer();
		PX_UNUSED(errorStr);
	}
#endif
}

class D3D9ShaderIncluder : public ID3DXInclude
{
public:
	D3D9ShaderIncluder(const char* assetDir) : m_assetDir(assetDir) {}

private:
#if defined(RENDERER_XBOX360)
	STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE includeType, LPCSTR fileName, LPCVOID parentData, LPCVOID* data, UINT* dataSize, LPSTR pFullPath, DWORD cbFullPath)
#else
	STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE includeType, LPCSTR fileName, LPCVOID parentData, LPCVOID* data, UINT* dataSize)
#endif
	{
		HRESULT result = D3DERR_NOTFOUND;

		char fullpath[1024];
		physx::string::strcpy_s(fullpath, 1024, m_assetDir);
		physx::string::strcat_s(fullpath, 1024, "shaders/");
		if (includeType == D3DXINC_SYSTEM)
		{
			physx::string::strcat_s(fullpath, 1024, "include/");
		}
		physx::string::strcat_s(fullpath, 1024, fileName);

		FILE* file = 0;
		Fnd::fopen_s(&file, fullpath, "r");
		if (file)
		{
			fseek(file, 0, SEEK_END);
			size_t fileLen = ftell(file);
			if (fileLen > 1)
			{
				fseek(file, 0, SEEK_SET);
				char* fileData = new char[fileLen + 1];
				fileLen = fread(fileData, 1, fileLen, file);
				fileData[fileLen] = 0;
				*data     = fileData;
				*dataSize = (UINT)fileLen;
			}
			fclose(file);
			result = D3D_OK;
		}
		RENDERER_ASSERT(result == D3D_OK, "Failed to include shader header.");
		return result;
	}

	STDMETHOD(Close)(THIS_ LPCVOID data)
	{
		delete []((char*)data);
		return D3D_OK;
	}

	const char* m_assetDir;
};

D3D9RendererMaterial::D3D9RendererMaterial(D3D9Renderer& renderer, const RendererMaterialDesc& desc) :
	RendererMaterial(desc, renderer.getEnableMaterialCaching()),
	m_renderer(renderer)
{
	m_d3dAlphaTestFunc      = D3DCMP_ALWAYS;
	m_vertexShader          = 0;
	m_instancedVertexShader = 0;
	memset(m_fragmentPrograms, 0, sizeof(m_fragmentPrograms));

	AlphaTestFunc alphaTestFunc = getAlphaTestFunc();
	switch (alphaTestFunc)
	{
	case ALPHA_TEST_ALWAYS:
		m_d3dAlphaTestFunc = D3DCMP_ALWAYS;
		break;
	case ALPHA_TEST_EQUAL:
		m_d3dAlphaTestFunc = D3DCMP_EQUAL;
		break;
	case ALPHA_TEST_NOT_EQUAL:
		m_d3dAlphaTestFunc = D3DCMP_NOTEQUAL;
		break;
	case ALPHA_TEST_LESS:
		m_d3dAlphaTestFunc = D3DCMP_LESS;
		break;
	case ALPHA_TEST_LESS_EQUAL:
		m_d3dAlphaTestFunc = D3DCMP_LESSEQUAL;
		break;
	case ALPHA_TEST_GREATER:
		m_d3dAlphaTestFunc = D3DCMP_GREATER;
		break;
	case ALPHA_TEST_GREATER_EQUAL:
		m_d3dAlphaTestFunc = D3DCMP_GREATEREQUAL;
		break;
	default:
		RENDERER_ASSERT(0, "Unknown Alpha Test Func.");
	}

	m_d3dSrcBlendFunc = getD3DBlendFunc(getSrcBlendFunc());
	m_d3dDstBlendFunc = getD3DBlendFunc(getDstBlendFunc());

	D3D9Renderer::D3DXInterface& d3dx      = m_renderer.getD3DX();
	IDirect3DDevice9*            d3dDevice = m_renderer.getD3DDevice();
	if (d3dDevice)
	{
		D3D9ShaderIncluder shaderIncluder(m_renderer.getAssetDir());

		const char* vertexEntry      = "vmain";
		const char* vertexProfile    = d3dx.GetVertexShaderProfile(d3dDevice);
		const char* vertexShaderPath = desc.vertexShaderPath;
		const DWORD vertexFlags      = D3DXSHADER_PACKMATRIX_COLUMNMAJOR;
		const D3DXMACRO vertexDefines[] =
		{
			{"RENDERER_VERTEX", "1"},
			{"RENDERER_D3D", "1"},
			{"SEMANTIC_TANGENT", "TANGENT"}, // This will prevent mapping tangent to texcoord5 and instead to the proper TANGENT semantic
			{0, 0}
		};
		LPD3DXBUFFER        vshader = 0;
		LPD3DXBUFFER        verrors = 0;
		HRESULT result = d3dx.CompileShaderFromFileA(vertexShaderPath, vertexDefines, &shaderIncluder, vertexEntry, vertexProfile, vertexFlags, &vshader, &verrors, &m_vertexConstants.table);
		processCompileErrors(verrors);
		RENDERER_ASSERT(result == D3D_OK && vshader, "Failed to compile Vertex Shader.");
		if (result == D3D_OK && vshader)
		{
			result = d3dDevice->CreateVertexShader((const DWORD*)vshader->GetBufferPointer(), &m_vertexShader);
			RENDERER_ASSERT(result == D3D_OK && m_vertexShader, "Failed to load Vertex Shader.");
			if (result == D3D_OK && m_vertexShader)
			{
				m_vertexConstants.loadConstants();
				if (m_vertexConstants.table)
				{
					loadCustomConstants(*m_vertexConstants.table, NUM_PASSES);
				}
			}
		}
		if (vshader)
		{
			vshader->Release();
		}
		if (verrors)
		{
			verrors->Release();
		}
#if RENDERER_INSTANCING
		const D3DXMACRO vertexDefinesInstanced[] =
		{
			{"RENDERER_VERTEX",    "1"},
			{"RENDERER_D3D", "1"},
			{"RENDERER_INSTANCED", "1"},
			{"SEMANTIC_TANGENT", "TANGENT"},
#if defined(PX_WINDOWS)
			{"PX_WINDOWS",         "1"},
#elif defined(PX_X360)
			{"PX_X360",            "1"},
#endif
			{0, 0}
		};
#else
		const D3DXMACRO vertexDefinesInstanced[] =
		{
			{"RENDERER_VERTEX",    "1"},
			{"RENDERER_INSTANCED", "0"},
			{"SEMANTIC_TANGENT", "TANGENT"},
#if defined(PX_WINDOWS)
			{"PX_WINDOWS",         "1"},
#elif defined(PX_X360)
			{"PX_X360",            "1"},
#endif
			{0, 0}
		};
#endif
		vshader = 0;
		verrors = 0;
		result = d3dx.CompileShaderFromFileA(vertexShaderPath, vertexDefinesInstanced, &shaderIncluder, vertexEntry, vertexProfile, vertexFlags, &vshader, &verrors, &m_instancedVertexConstants.table);
		processCompileErrors(verrors);
		RENDERER_ASSERT(result == D3D_OK && vshader, "Failed to compile Vertex Shader.");
		if (result == D3D_OK && vshader)
		{
			result = d3dDevice->CreateVertexShader((const DWORD*)vshader->GetBufferPointer(), &m_instancedVertexShader);
			RENDERER_ASSERT(result == D3D_OK && m_vertexShader, "Failed to load Vertex Shader.");
			if (result == D3D_OK && m_vertexShader)
			{
				m_instancedVertexConstants.loadConstants();
				if (m_instancedVertexConstants.table)
				{
					loadCustomConstants(*m_instancedVertexConstants.table, NUM_PASSES);
				}
			}
		}
		if (vshader)
		{
			vshader->Release();
		}
		if (verrors)
		{
			verrors->Release();
		}

		const char* fragmentEntry      = "fmain";
		const char* fragmentProfile    = d3dx.GetPixelShaderProfile(d3dDevice);
		const char* fragmentShaderPath = desc.fragmentShaderPath;
		const DWORD fragmentFlags      = D3DXSHADER_PACKMATRIX_COLUMNMAJOR;
		
		// profile string in the format "ps_x_x"
		int majorVersion = fragmentProfile[3]-'0';

		// vface is sm3.0 and up
		bool vFaceSupported = majorVersion > 2;
		// shadow shader pushes fragment prog registers > 32 which limits support to sm 3.0 as well
		bool shadowsSupported = majorVersion > 2;
		PX_UNUSED(shadowsSupported);

		for (PxU32 i = 0; i < NUM_PASSES; i++)
		{
			const D3DXMACRO fragmentDefines[] =
			{
				{"RENDERER_FRAGMENT",  "1"},
				{getPassName((Pass)i), "1"},
				{"ENABLE_VFACE",       vFaceSupported?"1":"0"},
				{"ENABLE_VFACE_SCALE", vFaceSupported?"1":"0"},
#if defined(PX_WINDOWS)
				{"PX_WINDOWS",         "1"},
				{"ENABLE_SHADOWS",     shadowsSupported?"1":"0"},
#elif defined(PX_X360)
				{"PX_X360",            "1"},
#endif
				{0, 0}
			};
			LPD3DXBUFFER        fshader = 0;
			LPD3DXBUFFER        ferrors = 0;
			result = d3dx.CompileShaderFromFileA(fragmentShaderPath, fragmentDefines, &shaderIncluder, fragmentEntry, fragmentProfile, fragmentFlags, &fshader, &ferrors, &m_fragmentConstants[i].table);
			processCompileErrors(ferrors);
			RENDERER_ASSERT(result == D3D_OK && fshader, "Failed to compile Fragment Shader.");
			if (result == D3D_OK && vshader)
			{
				result = d3dDevice->CreatePixelShader((const DWORD*)fshader->GetBufferPointer(), &m_fragmentPrograms[i]);
				RENDERER_ASSERT(result == D3D_OK && m_fragmentPrograms[i], "Failed to load Fragment Shader.");
				if (result == D3D_OK && m_fragmentPrograms[i])
				{
					m_fragmentConstants[i].loadConstants();
					if (m_fragmentConstants[i].table)
					{
						loadCustomConstants(*m_fragmentConstants[i].table, (Pass)i);
					}
				}
			}
			if (fshader)
			{
				fshader->Release();
			}
			if (ferrors)
			{
				ferrors->Release();
			}
		}
	}
}

D3D9RendererMaterial::~D3D9RendererMaterial(void)
{
	if (m_vertexShader)
	{
		m_vertexShader->Release();
	}
	if (m_instancedVertexShader)
	{
		m_instancedVertexShader->Release();
	}
	for (PxU32 i = 0; i < NUM_PASSES; i++)
	{
		IDirect3DPixelShader9* fp = m_fragmentPrograms[i];
		if (fp)
		{
			fp->Release();
		}
	}
}

void D3D9RendererMaterial::bind(RendererMaterial::Pass pass, RendererMaterialInstance* materialInstance, bool instanced) const
{
	IDirect3DDevice9* d3dDevice = m_renderer.getD3DDevice();
	RENDERER_ASSERT(pass < NUM_PASSES, "Invalid Material Pass.");
	if (d3dDevice && pass < NUM_PASSES)
	{
		const D3D9Renderer::ShaderEnvironment& shaderEnv = m_renderer.getShaderEnvironment();

		if (m_d3dAlphaTestFunc == D3DCMP_ALWAYS)
		{
			d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, 0);
		}
		else
		{
			PxU8 alphaTestRef = (PxU8)(PxClamp(getAlphaTestRef(), 0.0f, 1.0f) * 255.0f);
			d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, 1);
			d3dDevice->SetRenderState(D3DRS_ALPHAFUNC, (DWORD)m_d3dAlphaTestFunc);
			d3dDevice->SetRenderState(D3DRS_ALPHAREF , (DWORD)alphaTestRef);
		}

		if (getBlending())
		{
			d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
			d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,     0);
			d3dDevice->SetRenderState(D3DRS_SRCBLEND, (DWORD)m_d3dSrcBlendFunc);
			d3dDevice->SetRenderState(D3DRS_DESTBLEND, (DWORD)m_d3dDstBlendFunc);
		}

		if (instanced)
		{
			d3dDevice->SetVertexShader(m_instancedVertexShader);
		}
		else
		{
			d3dDevice->SetVertexShader(m_vertexShader);
		}

		m_fragmentConstants[pass].bindEnvironment(*d3dDevice, shaderEnv);
		d3dDevice->SetPixelShader(m_fragmentPrograms[pass]);

		RendererMaterial::bind(pass, materialInstance, instanced);
	}
}

void D3D9RendererMaterial::bindMeshState(bool instanced) const
{
	IDirect3DDevice9* d3dDevice = m_renderer.getD3DDevice();
	if (d3dDevice)
	{
		const D3D9Renderer::ShaderEnvironment& shaderEnv = m_renderer.getShaderEnvironment();

		if (instanced)
		{
			m_instancedVertexConstants.bindEnvironment(*d3dDevice, shaderEnv);
		}
		else
		{
			m_vertexConstants.bindEnvironment(*d3dDevice, shaderEnv);
		}
	}
}

void D3D9RendererMaterial::unbind(void) const
{
	IDirect3DDevice9* d3dDevice = m_renderer.getD3DDevice();
	if (d3dDevice)
	{
		if (getBlending())
		{
			d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
			d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,     1);
		}

		d3dDevice->SetVertexShader(0);
		d3dDevice->SetPixelShader(0);
	}
}

template<class TextureType, class D3DTextureType>
static void bindSamplerVariable(IDirect3DDevice9& d3dDevice, ID3DXConstantTable& table, D3DXHANDLE handle, TextureType& texture)
{
	if (handle)
	{
		UINT samplerIndex = table.GetSamplerIndex(handle);
		static_cast<D3DTextureType*>(&texture)->bind(samplerIndex);
	}
}

void D3D9RendererMaterial::bindVariable(Pass pass, const Variable& variable, const void* data) const
{
	D3D9Variable& var = *(D3D9Variable*)&variable;
	IDirect3DDevice9* d3dDevice = m_renderer.getD3DDevice();
	if (d3dDevice)
	{
		switch (var.getType())
		{
		case VARIABLE_FLOAT:
		{
			const float fdata[4] = {*(const float*)data, 0, 0, 0};
			if (var.m_vertexHandle)
			{
				d3dDevice->SetVertexShaderConstantF(var.m_vertexRegister,          fdata, 1);
			}
			else if (var.m_fragmentHandles[pass])
			{
				d3dDevice->SetPixelShaderConstantF(var.m_fragmentRegisters[pass], fdata, 1);
			}
			break;
		}
		case VARIABLE_FLOAT2:
		{
			const float fdata[4] = {((const float*)data)[0], ((const float*)data)[1], 0, 0};
			if (var.m_vertexHandle)
			{
				d3dDevice->SetVertexShaderConstantF(var.m_vertexRegister,          fdata, 1);
			}
			else if (var.m_fragmentHandles[pass])
			{
				d3dDevice->SetPixelShaderConstantF(var.m_fragmentRegisters[pass], fdata, 1);
			}
			break;
		}
		case VARIABLE_FLOAT3:
		{
			const float fdata[4] = {((const float*)data)[0], ((const float*)data)[1], ((const float*)data)[2], 0};
			if (var.m_vertexHandle)
			{
				d3dDevice->SetVertexShaderConstantF(var.m_vertexRegister,          fdata, 1);
			}
			else if (var.m_fragmentHandles[pass])
			{
				d3dDevice->SetPixelShaderConstantF(var.m_fragmentRegisters[pass], fdata, 1);
			}
			break;
		}
		case VARIABLE_FLOAT4:
		{
			const float fdata[4] = {((const float*)data)[0], ((const float*)data)[1], ((const float*)data)[2], ((const float*)data)[3]};
			if (var.m_vertexHandle)
			{
				d3dDevice->SetVertexShaderConstantF(var.m_vertexRegister,          fdata, 1);
			}
			else if (var.m_fragmentHandles[pass])
			{
				d3dDevice->SetPixelShaderConstantF(var.m_fragmentRegisters[pass], fdata, 1);
			}
			break;
		}
		case VARIABLE_SAMPLER2D:
			data = *(void**)data;
			RENDERER_ASSERT(data, "NULL Sampler.");
			if (data)
			{
				bindSamplerVariable<RendererTexture2D, D3D9RendererTexture2D>(*m_renderer.getD3DDevice(), *m_vertexConstants.table,         var.m_vertexHandle,          *(RendererTexture2D*)data);
				bindSamplerVariable<RendererTexture2D, D3D9RendererTexture2D>(*m_renderer.getD3DDevice(), *m_fragmentConstants[pass].table, var.m_fragmentHandles[pass], *(RendererTexture2D*)data);
			}
			break;
		case VARIABLE_SAMPLER3D:
			RENDERER_ASSERT(0, "3D D3D9 Textures Not Implemented.");
			/*
			data = *(void**)data;
			RENDERER_ASSERT(data, "NULL Sampler.");
			if (data)
			{
				bindSamplerVariable<RendererTexture3D, D3D9RendererTexture3D>(*m_renderer.getD3DDevice(), *m_vertexConstants.table,         var.m_vertexHandle,          *(RendererTexture2D*)data);
				bindSamplerVariable<RendererTexture3D, D3D9RendererTexture3D>(*m_renderer.getD3DDevice(), *m_fragmentConstants[pass].table, var.m_fragmentHandles[pass], *(RendererTexture2D*)data);
			}
			*/
			break;
		}
	}
}

static RendererMaterial::VariableType getVariableType(const D3DXCONSTANT_DESC& desc)
{
	RendererMaterial::VariableType vt = RendererMaterial::NUM_VARIABLE_TYPES;
	switch (desc.Type)
	{
	case D3DXPT_FLOAT:
		if (desc.Rows == 4 && desc.Columns == 4)
		{
			vt = RendererMaterial::VARIABLE_FLOAT4x4;
		}
		else if (desc.Rows == 1 && desc.Columns == 1)
		{
			vt = RendererMaterial::VARIABLE_FLOAT;
		}
		else if (desc.Rows == 1 && desc.Columns == 2)
		{
			vt = RendererMaterial::VARIABLE_FLOAT2;
		}
		else if (desc.Rows == 1 && desc.Columns == 3)
		{
			vt = RendererMaterial::VARIABLE_FLOAT3;
		}
		else if (desc.Rows == 1 && desc.Columns == 4)
		{
			vt = RendererMaterial::VARIABLE_FLOAT4;
		}
		break;
	case D3DXPT_SAMPLER2D:
		vt = RendererMaterial::VARIABLE_SAMPLER2D;
		break;
	}
	RENDERER_ASSERT(vt < RendererMaterial::NUM_VARIABLE_TYPES, "Unable to convert shader variable type.");
	return vt;
}

void D3D9RendererMaterial::loadCustomConstants(ID3DXConstantTable& table, Pass pass)
{
	D3DXCONSTANTTABLE_DESC desc;
	table.GetDesc(&desc);
	for (UINT i = 0; i < desc.Constants; i++)
	{
		D3DXHANDLE constant = table.GetConstant(0, i);
		RENDERER_ASSERT(constant, "Unable to find constant");
		if (constant)
		{
			D3DXCONSTANT_DESC cdesc;
			UINT              count = 1;
			table.GetConstantDesc(constant, &cdesc, &count);
			PX_ASSERT(count == 1);
			if (count == 1 && strncmp(cdesc.Name, "g_", 2))
			{
				VariableType type = getVariableType(cdesc);
				if (type < NUM_VARIABLE_TYPES)
				{
					D3D9Variable* var = 0;
					// search to see if the variable already exists...
					PxU32 numVariables = (PxU32)m_variables.size();
					for (PxU32 j = 0; j < numVariables; j++)
					{
						if (!strcmp(m_variables[j]->getName(), cdesc.Name))
						{
							var = static_cast<D3D9Variable*>(m_variables[j]);
							break;
						}
					}
					// check to see if the variable is of the same type.
					if (var)
					{
						RENDERER_ASSERT(var->getType() == type, "Variable changes type!");
					}
					// if we couldn't find the variable... create a new variable...
					if (!var)
					{
						var = new D3D9Variable(cdesc.Name, type, m_variableBufferSize);
						m_variables.push_back(var);
						m_variableBufferSize += var->getDataSize();
					}
					// add the handle to the variable...
					if (pass < NUM_PASSES)
					{
						var->addFragmentHandle(table, constant, pass);
					}
					else
					{
						var->addVertexHandle(table, constant);
					}
				}
			}
		}
	}
}

#endif
