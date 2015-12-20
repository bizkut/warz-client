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
#ifndef D3D9_RENDERER_MATERIAL_H
#define D3D9_RENDERER_MATERIAL_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D9)

#include <RendererMaterial.h>

#include "D3D9Renderer.h"

namespace SampleRenderer
{

	class D3D9Renderer;

	class D3D9RendererMaterial : public RendererMaterial
	{
	public:
		D3D9RendererMaterial(D3D9Renderer &renderer, const RendererMaterialDesc &desc);
		virtual ~D3D9RendererMaterial(void);
		virtual void setModelMatrix(const float *matrix);

	private:
		virtual const Renderer& getRenderer() const { return m_renderer; }
		virtual void bind(RendererMaterial::Pass pass, RendererMaterialInstance *materialInstance, bool instanced) const;
		virtual void bindMeshState(bool instanced) const;
		virtual void unbind(void) const;
		virtual void bindVariable(Pass pass, const Variable &variable, const void *data) const;

		void loadCustomConstants(ID3DXConstantTable &table, Pass pass);

	private:
		class ShaderConstants
		{
		public:
			LPD3DXCONSTANTTABLE table;

			D3DXHANDLE          modelMatrix;
			D3DXHANDLE          viewMatrix;
			D3DXHANDLE          projMatrix;
			D3DXHANDLE          modelViewMatrix;
			D3DXHANDLE          modelViewProjMatrix;

			D3DXHANDLE          boneMatrices;

			D3DXHANDLE          fogColorAndDistance;

			D3DXHANDLE          eyePosition;
			D3DXHANDLE          eyeDirection;

			D3DXHANDLE          ambientColor;

			D3DXHANDLE          lightColor;
			D3DXHANDLE          lightIntensity;
			D3DXHANDLE          lightDirection;
			D3DXHANDLE          lightPosition;
			D3DXHANDLE          lightInnerRadius;
			D3DXHANDLE          lightOuterRadius;
			D3DXHANDLE          lightInnerCone;
			D3DXHANDLE          lightOuterCone;
			D3DXHANDLE          lightShadowMap;
			D3DXHANDLE          lightShadowMatrix;

			D3DXHANDLE          vfaceScale;

		public:
			ShaderConstants(void);
			~ShaderConstants(void);

			void loadConstants(void);

			void bindEnvironment(IDirect3DDevice9 &d3dDevice, const D3D9Renderer::ShaderEnvironment &shaderEnv) const;
		};

		class D3D9Variable : public Variable
		{
			friend class D3D9RendererMaterial;
		public:
			D3D9Variable(const char *name, VariableType type, PxU32 offset);
			virtual ~D3D9Variable(void);

			void addVertexHandle(ID3DXConstantTable &table, D3DXHANDLE handle);
			void addFragmentHandle(ID3DXConstantTable &table, D3DXHANDLE handle, Pass pass);

		private:
			D3D9Variable &operator=(const D3D9Variable&) { return *this; }

		private:
			D3DXHANDLE          m_vertexHandle;
			UINT                m_vertexRegister;
			D3DXHANDLE          m_fragmentHandles[NUM_PASSES];
			UINT                m_fragmentRegisters[NUM_PASSES];
		};

	private:
		D3D9RendererMaterial &operator=(const D3D9RendererMaterial&) { return *this; }

	private:
		D3D9Renderer           &m_renderer;

		D3DCMPFUNC              m_d3dAlphaTestFunc;
		D3DBLEND                m_d3dSrcBlendFunc;
		D3DBLEND                m_d3dDstBlendFunc;

		IDirect3DVertexShader9 *m_vertexShader;
		IDirect3DVertexShader9 *m_instancedVertexShader;
		IDirect3DPixelShader9  *m_fragmentPrograms[NUM_PASSES];

		ShaderConstants         m_vertexConstants;
		ShaderConstants         m_instancedVertexConstants;
		ShaderConstants         m_fragmentConstants[NUM_PASSES];
	};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_DIRECT3D9)
#endif
