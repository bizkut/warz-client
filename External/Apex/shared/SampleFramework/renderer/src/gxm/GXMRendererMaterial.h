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
#ifndef GXM_RENDERER_MATERIAL_H
#define GXM_RENDERER_MATERIAL_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_LIBGXM)

#include <RendererMaterial.h>

#include "GXMRenderer.h"


namespace SampleRenderer
{


	class GXMVariable : public RendererMaterial::Variable
	{
	public:
		GXMVariable(const char *name, RendererMaterial::VariableType type, PxU32 offset)
			: Variable(name, type, offset)
		{
		}

		uint32_t m_resourceIndex;
	};

	class GXMRendererMaterial : public RendererMaterial
	{
		friend class OGLRendererMesh;

		struct ShaderConstant
		{
			const SceGxmProgramParameter* mModelMatrixUniform;
			const SceGxmProgramParameter* mViewMatrixUniform;
			const SceGxmProgramParameter* mProjMatrixUniform;
			const SceGxmProgramParameter* mModelViewMatrixUniform;
			const SceGxmProgramParameter* mBoneMatricesUniform;
			const SceGxmProgramParameter* mAmbientColor;

			const SceGxmProgramParameter* mModelViewProjMatrixUniform;
			
			const SceGxmProgramParameter* mFogColorAndDistance;

			const SceGxmProgramParameter* mEyePosition;
			const SceGxmProgramParameter* mEyeDirection;			

			const SceGxmProgramParameter* mLightColor;
			const SceGxmProgramParameter* mLightIntensity;
			const SceGxmProgramParameter* mLightDirection;
			const SceGxmProgramParameter* mLightPosition;
			const SceGxmProgramParameter* mLightInnerRadius;
			const SceGxmProgramParameter* mLightOuterRadius;
			const SceGxmProgramParameter* mLightInnerCone;
			const SceGxmProgramParameter* mLightOuterCone;
			const SceGxmProgramParameter* mLightShadowMap;
			const SceGxmProgramParameter* mLightShadowMatrix;

			const SceGxmProgramParameter* mShadeMode;

			const SceGxmProgramParameter* mVfaceScale;
		};

		struct FragmentProgram
		{
			RendererMaterial::Pass	mPass;
			const SceGxmProgram *	mGmxFragmentProgram;
			SceGxmShaderPatcherId	mGmxFragmentProgramId;
			SceGxmFragmentProgram*	mFinalFragmentProgram;
			ShaderConstant			mShaderConstant;
		};

	public:
		GXMRendererMaterial(GXMRenderer &renderer, const RendererMaterialDesc &desc);
		virtual ~GXMRendererMaterial(void);
		virtual void setModelMatrix(const float *matrix) 
		{
			PX_FORCE_PARAMETER_REFERENCE(matrix);
			PX_ALWAYS_ASSERT();
		}

		const GXMRendererMaterial::FragmentProgram* getFramentProgram(RendererMaterial::Pass) const;
		SceGxmFragmentProgram* getFinalFramentProgram(RendererMaterial::Pass) const;
		SceGxmVertexProgram* getFinalVertexProgram() const { return mFinalVertexProgram; }		

		uint32_t getResourceIndex(const char* name) const;

	private:
		virtual Renderer& getRenderer() { return mRenderer; }
		virtual void bind(RendererMaterial::Pass pass, RendererMaterialInstance *materialInstance, bool instanced) const;
		virtual void bindMeshState(bool instanced) const;
		virtual void unbind(void) const;
		virtual void bindVariable(Pass pass, const Variable &variable, const void *data) const;

		void loadCustomConstants(const SceGxmProgram* program, bool fragment = false);
		void loadShaderConstants(const SceGxmProgram* program, ShaderConstant* sc);
		RendererMaterial::VariableType gxmTypetoVariableType(SceGxmParameterType type, SceGxmParameterCategory category, uint32_t componentCount, uint32_t arraysize);
		void bindEnvironment(const GXMRenderer::ShaderEnvironment& env, bool instanced) const;

		void fillShaderAttribute(const GXMRenderer::ShaderEnvironment& env, GXMVariable* var, SceGxmVertexAttribute& attribute) const;

		void parseFragmentShader(const SceGxmProgram*, RendererMaterial::Pass );

	private:
		GXMRendererMaterial &operator=(const GXMRendererMaterial&) { return *this; }

	private:
		friend class GXMRenderer;
		GXMRenderer &			mRenderer;

		const SceGxmProgram *	mGmxVertexProgram;
		SceGxmShaderPatcherId	mGmxVertexProgramId;

		std::vector<FragmentProgram> mFragmentPrograms;

		PxU16					mNumVertexAttributes;
		SceGxmBlendInfo*		mParticleBlendInfo;
		
		mutable SceGxmVertexProgram*	mFinalVertexProgram;		

		mutable void *mVertexUniformDefaultBuffer;
		mutable void *mFragmentUniformDefaultBuffer;

		char	mFragmentProgramName[512];
		char	mVertexProgramName[512];
	};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_LIBGXM)
#endif
