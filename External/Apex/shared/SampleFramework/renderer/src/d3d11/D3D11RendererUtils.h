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

#ifndef D3D11_RENDERER_UTILS_H
#define D3D11_RENDERER_UTILS_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D11)

#include "D3D11Renderer.h"
#include "D3D11RendererTraits.h"

#include <d3dx11tex.h>

struct ID3DX11ThreadPump;

namespace SampleRenderer
{

PX_INLINE PxU32 fast_hash(const char *str, SIZE_T wrdlen)
{
	// "FNV" string hash
	const PxU32 PRIME = 1607;

	PxU32 hash32 = 2166136261;
	const char *p = str;

	for(; wrdlen >= sizeof(PxU32); wrdlen -= sizeof(PxU32), p += sizeof(PxU32)) {
		hash32 = (hash32 ^ *(PxU32 *)p) * PRIME;
	}
	if (wrdlen & sizeof(PxU16)) {
		hash32 = (hash32 ^ *(PxU16*)p) * PRIME;
		p += sizeof(PxU16);
	}
	if (wrdlen & 1) 
		hash32 = (hash32 ^ *p) * PRIME;

	return hash32 ^ (hash32 >> 16);
}

class D3D11StringKey
{
public:
	D3D11StringKey(const char* stringKey) : mStringHash(fast_hash(stringKey, strlen(stringKey))) { }
	D3D11StringKey(const std::string& stringKey) : mStringHash(fast_hash(stringKey.c_str(), stringKey.length())) { }
	bool operator<(const D3D11StringKey& other) const { return mStringHash < other.mStringHash; }
private:
	D3D11StringKey();
	PxU32 mStringHash;
};

class D3DX11
{
	friend class D3D11Renderer;
private:
	D3DX11(void);
	~D3DX11(void);

public:
	HRESULT compileShaderFromFile(LPCSTR pSrcFile, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
		LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2, ID3DX11ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, HRESULT* pHResult) const;
	HRESULT compileShaderFromMemory(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
		LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2, ID3DX11ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, HRESULT* pHResult) const;
	HRESULT reflect(LPCVOID pSrcData, SIZE_T SrcDataSize, REFIID pInterface, void** ppReflector);
	HRESULT saveTextureToMemory(ID3D11DeviceContext *pContext, ID3D11Resource *pSrcTexture, D3DX11_IMAGE_FILE_FORMAT DestFormat, LPD3D10BLOB *ppDestBuf, UINT Flags);
	HRESULT createBlob(SIZE_T Size, ID3DBlob **ppBlob);

public:
	static void processCompileErrors(ID3DBlob* errors);

	static PxU64 getInputHash(const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDesc);
	static PxU64 getInputHash(const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numInputElementDescs);

private:
#if defined(RENDERER_WINDOWS)
	HMODULE  m_library;
	HMODULE  m_compiler_library;

	typedef HRESULT(WINAPI* D3DX11COMPILEFROMFILE)(LPCSTR, CONST D3D10_SHADER_MACRO*, LPD3D10INCLUDE, LPCSTR, LPCSTR, UINT, UINT, ID3DX11ThreadPump*, ID3D10Blob**, ID3D10Blob**, HRESULT*);
	D3DX11COMPILEFROMFILE m_pD3DX11CompileFromFile;
	typedef HRESULT(WINAPI* D3DX11COMPILEFROMMEMORY)(LPCSTR, SIZE_T, LPCSTR, CONST D3D10_SHADER_MACRO*, LPD3D10INCLUDE, LPCSTR, LPCSTR, UINT, UINT, ID3DX11ThreadPump*, ID3D10Blob**, ID3D10Blob**, HRESULT*);
	D3DX11COMPILEFROMMEMORY m_pD3DX11CompileFromMemory;
	typedef HRESULT(WINAPI* D3DREFLECT)(LPCVOID, SIZE_T, REFIID, void**);
	D3DREFLECT m_pD3DReflect;
	typedef HRESULT(WINAPI* D3DX11SAVETEXTURETOMEMORY)(ID3D11DeviceContext *, ID3D11Resource *, D3DX11_IMAGE_FILE_FORMAT, LPD3D10BLOB *, UINT);
	D3DX11SAVETEXTURETOMEMORY m_pD3DX11SaveTextureToMemory;
	typedef HRESULT(WINAPI* D3DCREATEBLOB)(SIZE_T, ID3DBlob**);
	D3DCREATEBLOB m_pD3DCreateBlob;

#endif

};

class D3D11ShaderIncluder : public ID3DInclude
{
public:
	D3D11ShaderIncluder(const char* assetDir);

private:
	STDMETHOD(Open(
	              D3D_INCLUDE_TYPE includeType,
	              LPCSTR fileName,
	              LPCVOID parentData,
	              LPCVOID* data,
	              UINT* dataSize
	          ));

	STDMETHOD(Close(LPCVOID data));

	const char* m_assetDir;
};


class D3D11ShaderCacher
{
public:
	D3D11ShaderCacher(D3D11RendererResourceManager* pResourceManager);

	template<typename d3d_type>
	bool check(const typename D3DTraits<d3d_type>::key_type& key,
	           d3d_type*& d3dResource,
	           ID3DBlob*& d3dResourceBlob) const
	{
		typename D3DTraits<d3d_type>::value_type value = mResourceManager->hasResource<d3d_type>(key);
		d3dResource = value.first;
		d3dResourceBlob = value.second;
		return !((NULL == d3dResource) || (NULL == d3dResourceBlob));
	}

	template<typename d3d_type>
	void cache(const typename D3DTraits<d3d_type>::key_type& key,
	           const typename D3DTraits<d3d_type>::value_type& value)
	{
		mResourceManager->registerResource<d3d_type>(key, value);
	}

private:
	D3D11ShaderCacher();

	D3D11RendererResourceManager* mResourceManager;
};


class D3D11ShaderLoader
{
public:
	D3D11ShaderLoader(D3D11Renderer& renderer);

	template<typename d3d_type>
	HRESULT load(typename D3DTraits<d3d_type>::key_type shaderKey,
	             const char *pShaderPath,
	             const D3D10_SHADER_MACRO* pDefines, 
	             d3d_type **ppShader               = NULL,
	             ID3DBlob **ppShaderBlob           = NULL,
	             bool bCheckCacheBeforeLoading     = true,
	             bool *pbLoadedFromCache           = NULL)
	{
		HRESULT result        = S_OK;
		d3d_type* pShader     = NULL;
		ID3DBlob* pShaderBlob = NULL;

		if (!bCheckCacheBeforeLoading || !mCacher.check<d3d_type>(shaderKey, pShader, pShaderBlob))
		{
			result = internalLoad(pShaderPath, pDefines, &pShader, &pShaderBlob);
			mCacher.cache<d3d_type>(shaderKey, std::make_pair(pShader, pShaderBlob));
			if (pbLoadedFromCache) *pbLoadedFromCache = false;
		}
		else if (pbLoadedFromCache)
		{
			*pbLoadedFromCache = true;
		}

		if (SUCCEEDED(result))
		{
			if (ppShader)         *ppShader = pShader;
			if (ppShaderBlob) *ppShaderBlob = pShaderBlob;
		}
		else
		{
			RENDERER_ASSERT(0, "Error loading D3D11 shader");
		}

		return result;
	}

protected:
	template<typename d3d_type>
	HRESULT internalLoad(const char* pShaderName, const D3D10_SHADER_MACRO* pDefines, d3d_type** ppShader, ID3DBlob** ppBlob)
	{
		HRESULT result          = S_OK;
		const DWORD shaderFlags = D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
		ID3DBlob* pErrorsBlob   = NULL;

		result = mCompiler->compileShaderFromFile(
			pShaderName,
			pDefines,
			&mIncluder,
			D3DTraits<d3d_type>::getEntry(),
			D3DTraits<d3d_type>::getProfile(mFeatureLevel),
			shaderFlags,
			0,
			NULL,
			ppBlob,
			&pErrorsBlob,
			NULL
			);
		D3DX11::processCompileErrors(pErrorsBlob);

		RENDERER_ASSERT(SUCCEEDED(result) && *ppBlob, "Failed to compile shader.");
		if (SUCCEEDED(result) && *ppBlob)
		{
			result = D3DTraits<d3d_type>::create(mDevice, (*ppBlob)->GetBufferPointer(), (*ppBlob)->GetBufferSize(), NULL, ppShader);
			RENDERER_ASSERT(SUCCEEDED(result) && *ppShader, "Failed to load Fragment Shader.");
		}
		if (pErrorsBlob)
		{
			pErrorsBlob->Release();
		}
		return result;
	}
	
private:
	D3D11ShaderLoader();
	D3D11ShaderLoader& operator=(const D3D11ShaderLoader&);

	D3D_FEATURE_LEVEL       mFeatureLevel;
	D3D11ShaderCacher       mCacher;
	D3D11ShaderIncluder     mIncluder;
	D3DX11*                 mCompiler;
	ID3D11Device*           mDevice;
};


}

#endif // #if defined(RENDERER_ENABLE_DIRECT3D11)
#endif
