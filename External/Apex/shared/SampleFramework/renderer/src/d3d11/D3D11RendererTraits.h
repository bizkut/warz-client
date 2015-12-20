/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
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
#ifndef D3D11_RENDERER_TRAITS_H
#define D3D11_RENDERER_TRAITS_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D11)

#include <tuple>

namespace SampleRenderer
{

namespace D3DTypes
{
	enum D3DType
	{
		SHADER_PIXEL = 0,
		SHADER_VERTEX,
		SHADER_GEOMETRY,
		SHADER_HULL,
		SHADER_DOMAIN,
		LAYOUT_INPUT,
		STATE_RASTERIZER,
		STATE_BLEND,
		STATE_DEPTHSTENCIL,
		NUM_TYPES,
		NUM_SHADER_TYPES = SHADER_DOMAIN + 1,
		NUM_NON_PIXEL_SHADER_TYPES = NUM_SHADER_TYPES - 1,
		INVALID = NUM_TYPES
	};
}
typedef D3DTypes::D3DType D3DType;

typedef std::string D3DStringKey;
 
template<typename d3d_type>
class D3DTraits
{
public:
	typedef D3DStringKey key_type;
	typedef IUnknown*    value_type;
	static  D3DType getType() { return D3DTypes::INVALID; }
};

template <>
class D3DTraits<ID3D11VertexShader>
{
public:
	// InputLayoutHash + Shader Name
	typedef std::pair<PxU64, D3DStringKey>            key_type;
	typedef std::pair<ID3D11VertexShader*, ID3DBlob*> value_type;
	static const char* getEntry() { return "vmain"; }
	static const char* getProfile(D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0) 
	{ 
		switch(featureLevel) 
		{
		case D3D_FEATURE_LEVEL_11_0: return "vs_5_0"; 
		case D3D_FEATURE_LEVEL_10_1: return "vs_4_1";
		case D3D_FEATURE_LEVEL_10_0: return "vs_4_0";
		default: RENDERER_ASSERT(0, "Invalid feature level"); return "vs_invalid";
		}
	};
	static D3DType getType() { return D3DTypes::SHADER_VERTEX; }
	static HRESULT create( ID3D11Device* pDevice, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11VertexShader **ppShader)
	{
		return pDevice->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppShader);
	}
	static void setConstants( ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
	{
		pContext->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
	}
};

template <>
class D3DTraits<ID3D11PixelShader>
{
public:
	// Pass + Shader Name
	typedef std::pair<PxU32, D3DStringKey>           key_type;
	typedef std::pair<ID3D11PixelShader*, ID3DBlob*> value_type;
	static const char* getEntry() { return "fmain"; }
	static const char* getProfile(D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0) { 
		switch(featureLevel) 
		{
		case D3D_FEATURE_LEVEL_11_0: return "ps_5_0"; 
		case D3D_FEATURE_LEVEL_10_1: return "ps_4_1";
		case D3D_FEATURE_LEVEL_10_0: return "ps_4_0";
		default: RENDERER_ASSERT(0, "Invalid feature level"); return "ps_invalid";
		};
	}
	static D3DType getType() { return D3DTypes::SHADER_PIXEL; }
	static HRESULT create( ID3D11Device* pDevice, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11PixelShader **ppShader)
	{
		return pDevice->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppShader);
	}
	static void setConstants( ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
	{
		pContext->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
	}
};

template <>
class D3DTraits<ID3D11GeometryShader>
{
public:
	// Shader Name
	typedef D3DStringKey                               key_type;
	typedef std::pair<ID3D11GeometryShader*,ID3DBlob*> value_type;
	static const char* getEntry() { return "gmain"; }()
	static const char* getProfile(D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0) 
	{ 
		switch(featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_0: return "gs_5_0";
		case D3D_FEATURE_LEVEL_10_1: return "gs_4_1";
		case D3D_FEATURE_LEVEL_10_0: return "gs_4_0";
		default: RENDERER_ASSERT(0, "Invalid geometry shader feature level") return "gs_invalid";
		};
	}

	static D3DType getType() { return D3DTypes::SHADER_GEOMETRY; }
	static HRESULT create( ID3D11Device* pDevice, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppShader)
	{
		return pDevice->CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppShader);
	}
	static void setConstants( ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
	{
		pContext->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
	}
};

template <>
class D3DTraits<ID3D11HullShader>
{
public:
	// Shader Name
	typedef std::pair<PxU32, D3DStringKey>         key_type;
	typedef std::pair<ID3D11HullShader*,ID3DBlob*> value_type;
	static const char* getEntry() { return "hmain"; }
	static const char* getProfile(D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0)
	{ 
		switch(featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_0: return "hs_5_0";
		default: RENDERER_ASSERT(0, "Invalid hull shader feature level") return "hs_invalid";
		};
	}
	static D3DType getType() { return D3DTypes::SHADER_HULL; }
	static HRESULT create( ID3D11Device* pDevice, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11HullShader **ppShader)
	{
		return pDevice->CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppShader);
	}
	static void setConstants( ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
	{
		pContext->HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
	}
};

template <>
class D3DTraits<ID3D11DomainShader>
{
public:
	// Shader Name
	typedef std::pair<PxU32, D3DStringKey>            key_type;
	typedef std::pair<ID3D11DomainShader*, ID3DBlob*> value_type;
	static const char* getEntry() { return "dmain"; }
	static const char* getProfile(D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0)
	{ 
		switch(featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_0: return "ds_5_0";
		default: RENDERER_ASSERT(0, "Invalid domain shader feature level") return "ds_invalid";
		};
	}		
	static D3DType getType() { return D3DTypes::SHADER_DOMAIN; }
	static HRESULT create( ID3D11Device* pDevice, const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11DomainShader **ppShader)
	{
		return pDevice->CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppShader);
	}
	static void setConstants( ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
	{
		pContext->DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
	}
};

template <>
class D3DTraits<ID3D11InputLayout>
{
public:
	// InputLayoutHash + Shader Name
	typedef std::pair<PxU64, ID3DBlob*> key_type;
	typedef ID3D11InputLayout*          value_type;
	static D3DType getType() { return D3DTypes::LAYOUT_INPUT; }
};

template <>
class D3DTraits<ID3D11RasterizerState>
{
public:
	typedef std::tr1::tuple<D3D11_FILL_MODE, D3D11_CULL_MODE, int> key_type;
	typedef ID3D11RasterizerState*                      value_type;
	static D3DType getType() { return D3DTypes::STATE_RASTERIZER; }
};


template <>
class D3DTraits<ID3D11DepthStencilState>
{
public:
	// Depth enable + stencil enable
	typedef std::pair<bool, bool>    key_type;
	typedef ID3D11DepthStencilState* value_type;
	static D3DType getType() { return D3DTypes::STATE_DEPTHSTENCIL; }
};


template <>
class D3DTraits<ID3D11BlendState>
{
public:
	typedef D3D11_RENDER_TARGET_BLEND_DESC key_type;
	typedef ID3D11BlendState*              value_type;
	static D3DType getType() { return D3DTypes::STATE_BLEND; }
};

// This lets us lookup a traits class based on the D3DType key, which is rather handy
template<PxU32 N>
class D3DTraitsLookup { };
template<>
class D3DTraitsLookup<D3DTypes::SHADER_PIXEL>       { typedef ID3D11PixelShader       d3d_type; };
template<>
class D3DTraitsLookup<D3DTypes::SHADER_VERTEX>      { typedef ID3D11VertexShader      d3d_type; };
template<>
class D3DTraitsLookup<D3DTypes::SHADER_GEOMETRY>    { typedef ID3D11GeometryShader    d3d_type; };
template<>
class D3DTraitsLookup<D3DTypes::SHADER_HULL>        { typedef ID3D11HullShader        d3d_type; };
template<>
class D3DTraitsLookup<D3DTypes::SHADER_DOMAIN>      { typedef ID3D11DomainShader      d3d_type; };
template<>
class D3DTraitsLookup<D3DTypes::LAYOUT_INPUT>       { typedef ID3D11InputLayout       d3d_type; };
template<>
class D3DTraitsLookup<D3DTypes::STATE_BLEND>        { typedef ID3D11BlendState        d3d_type; };
template<>
class D3DTraitsLookup<D3DTypes::STATE_DEPTHSTENCIL> { typedef ID3D11DepthStencilState d3d_type; };
template<>
class D3DTraitsLookup<D3DTypes::STATE_RASTERIZER>   { typedef ID3D11RasterizerState   d3d_type; };

template <typename T>
struct NullTraits
{
	static T get() { return 0; }
};

template <typename T>
struct NullTraits<T*> 
{
	static T* get() { return NULL; }
};

template<typename T1,typename T2>
struct NullTraits< std::pair<T1*,T2*> > 
{
	static std::pair<T1*,T2*> get() { return std::pair<T1*,T2*>((T1*)NULL,(T2*)NULL); }
};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_DIRECT3D11)

#endif
