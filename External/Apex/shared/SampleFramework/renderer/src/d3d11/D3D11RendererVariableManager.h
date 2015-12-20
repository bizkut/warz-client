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
#ifndef D3D11_RENDERER_VARIABLE_MANAGER_H
#define D3D11_RENDERER_VARIABLE_MANAGER_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D11)

#include <RendererMaterial.h>

#include "D3D11RendererMaterial.h"
#include "D3D11RendererTraits.h"
#include "D3D11RendererUtils.h"
#include "D3Dcompiler.h"

#include <set>
#include <string>
#include <limits>

// Enable to check that binding a shared variable by name actually
//   finds the specified shared variable 
#define RENDERER_ASSERT_SHARED_VARIABLE_EXISTS 0

namespace SampleRenderer
{

static const PxU32 NUM_SHADER_TYPES = D3DTypes::NUM_SHADER_TYPES + RendererMaterial::NUM_PASSES;

class D3D11RendererVariableManager
{
public:
	enum SharedVariableSize
	{
		USE_DEFAULT = 0,
	};

	enum BindMode
	{
		BIND_MAP = 0,
		BIND_SUBRESOURCE
	};

public:
	typedef std::set<std::string> StringSet;

	D3D11RendererVariableManager(D3D11Renderer& renderer, StringSet& cbNames, BindMode bindMode = BIND_SUBRESOURCE);
	virtual ~D3D11RendererVariableManager(void);

public:
	void bind(const void* pResource, D3DType shaderType, RendererMaterial::Pass pass = RendererMaterial::NUM_PASSES) const;
	void setSharedVariable(const char* sharedBufferName, const char* variableName, const void* data, UINT size = USE_DEFAULT, UINT offset = 0);
	void loadVariables(D3D11RendererMaterial* pMaterial, ID3DBlob* pShader, D3DType shaderType, RendererMaterial::Pass pass = RendererMaterial::NUM_PASSES);
	void loadSharedVariables(const void* pResource, ID3DBlob* pShader, D3DType shaderType, RendererMaterial::Pass pass = RendererMaterial::NUM_PASSES);
	void unloadVariables(const void* pResource);

	class D3D11ConstantBuffer;
	class D3D11DataVariable;
	class D3D11TextureVariable;
	class D3D11SharedVariable;

	typedef std::vector<D3D11ConstantBuffer*> ConstantBuffers;
	typedef std::vector<ID3D11Buffer*>        D3DBuffers;
	typedef std::vector<D3D11SharedVariable*> Variables;
	typedef std::vector<D3D11RendererMaterial::Variable*> MaterialVariables;

	typedef D3D11StringKey                          StringKey;
	typedef const void*                             ResourceKey;
	typedef PxU32                                   ShaderTypeKey;
	typedef std::pair<StringKey,     StringKey>     VariableKey;

	typedef std::map<StringKey,   D3D11ConstantBuffer*>  NameBuffersMap;
	typedef std::map<VariableKey, D3D11SharedVariable*>  NameVariablesMap;
	typedef std::map<ResourceKey, ConstantBuffers>       ResourceBuffersMap;

	typedef ConstantBuffers::const_iterator CBIterator;

private:
	D3D11RendererVariableManager& operator=(const D3D11RendererVariableManager&)
	{
		return *this;
	}

	D3D11ConstantBuffer* loadBuffer(MaterialVariables& variables,
	                                PxU32& variableBufferSize,
	                                ShaderTypeKey typeKey,
	                                ID3D11ShaderReflectionConstantBuffer* pReflectionBuffer,
	                                const D3D11_SHADER_BUFFER_DESC& sbDesc,
	                                const D3D11_BUFFER_DESC& cbDesc);
	D3D11ConstantBuffer* loadSharedBuffer(ShaderTypeKey typeKey,
	                                      ID3D11ShaderReflectionConstantBuffer* pReflectionBuffer,
	                                      const D3D11_SHADER_BUFFER_DESC& sbDesc,
	                                      const D3D11_BUFFER_DESC& cbDesc);

	void loadConstantVariables(const void* pResource,
	                           ID3DBlob* pShader,
	                           ShaderTypeKey typeKey,
	                           ID3D11ShaderReflection* pReflection,
	                           MaterialVariables* pVariables = NULL,
	                           PxU32* pVariableBufferSize    = NULL);
	void loadTextureVariables(D3D11RendererMaterial* pMaterial,
	                          ID3DBlob* pShader,
	                          ShaderTypeKey typeKey,
	                          ID3D11ShaderReflection* pReflection);

	void internalSetVariable(D3D11ConstantBuffer* pBuffer, PxU32 offset, const void* data, PxU32 size);
	void updateVariables(const ConstantBuffers*) const;
	void bindVariables(const ConstantBuffers*, bool bFragment) const;

private:

	D3D11Renderer&         mRenderer;
	StringSet              mSharedBufferNames;

	BindMode               mBindMode;

	Variables              mVariables;

	NameBuffersMap         mNameToSharedBuffer;
	NameVariablesMap       mNameToSharedVariables;

	ResourceBuffersMap     mResourceToBuffers[NUM_SHADER_TYPES];
};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_DIRECT3D11)
#endif
