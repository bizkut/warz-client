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

#include "RendererConfig.h"

#if defined(RENDERER_ENABLE_LIBGXM)

#include "GXMRendererMaterial.h"
#include "GXMRendererTexture2D.h"
#include "GXMRendererVertexBuffer.h"
#include "GXMRendererInstanceBuffer.h"

#include <RendererMaterialDesc.h>

#include <SamplePlatform.h>

// for PsString.h
namespace physx
{
	namespace string
	{}
}
#include <PsString.h>
#include <PsFile.h>

using namespace SampleRenderer;


const SceGxmProgram* loadFragmentFile(const char* file, const char* pass, const char* assetDir)
{	
	char file2[512];
	char file3[512];

	strcpy(file3, file);
	
	char* endStr = strstr(file3,".cg");
	if(pass)
	{
		strcpy(endStr, "_");
		strcat(endStr, pass);
		strcat(endStr, ".gxp");
	}
	else
	{
		
		strcpy(endStr, ".gxp");
	}

    sprintf(file2, "%scompiledshaders/psp2/%s", assetDir, file3);
	FILE *f = fopen(file2, "rb");

	RENDERER_ASSERT(f, "Failed to load shader program.");	

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buffer = new char[size+1];
	fread(buffer, 1, size, f);
	fclose(f);
	buffer[size] = '\0';

	return (const SceGxmProgram*) buffer;
}

GXMRendererMaterial::GXMRendererMaterial(GXMRenderer &renderer, const RendererMaterialDesc &desc) :
	RendererMaterial(desc),
	mRenderer(renderer), mGmxVertexProgram(0), mGmxVertexProgramId(0), mFinalVertexProgram(0)
{
	int err = SCE_OK;

	mFragmentPrograms.reserve(8);

	strcpy(mFragmentProgramName, desc.fragmentShaderPath);
	strcpy(mVertexProgramName, desc.vertexShaderPath);
	const SceGxmProgram* vertexProgramData = NULL;
	if(desc.instanced)
	{
		vertexProgramData = loadFragmentFile(desc.vertexShaderPath, "INSTANCED", mRenderer.getAssetDir());
	}
	else
	{
		vertexProgramData = loadFragmentFile(desc.vertexShaderPath, NULL, mRenderer.getAssetDir());
	}
	const SceGxmProgram* fragmentProgramData = loadFragmentFile(desc.fragmentShaderPath,"UNLIT", mRenderer.getAssetDir());
	const SceGxmProgram* fragmentProgramDataDL = loadFragmentFile(desc.fragmentShaderPath,"DIRECTIONAL_LIGHT", mRenderer.getAssetDir());

	mParticleBlendInfo = NULL;

	mNumVertexAttributes = 0;

	if(vertexProgramData)
	{
		err = sceGxmProgramCheck(vertexProgramData);
		PX_ASSERT(err == SCE_OK);

		// register programs with the patcher
		err = sceGxmShaderPatcherRegisterProgram(mRenderer.getShaderPatcher(), vertexProgramData, &mGmxVertexProgramId);
		PX_ASSERT(err == SCE_OK);

		mGmxVertexProgram = sceGxmShaderPatcherGetProgramFromId(mGmxVertexProgramId);

		loadCustomConstants(mGmxVertexProgram);
	}

	mParticleBlendInfo = NULL;

	if(desc.blending)
	{
		mParticleBlendInfo = new SceGxmBlendInfo();

		mParticleBlendInfo->colorMask = SCE_GXM_COLOR_MASK_ALL;
		mParticleBlendInfo->colorFunc = SCE_GXM_BLEND_FUNC_ADD;

		switch (desc.srcBlendFunc)
		{
		case BLEND_ONE:
			mParticleBlendInfo->colorSrc = SCE_GXM_BLEND_FACTOR_ONE;
			break;

		case BLEND_SRC_ALPHA:
			mParticleBlendInfo->colorSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
			break;

		case BLEND_SRC_COLOR:
			mParticleBlendInfo->colorSrc = SCE_GXM_BLEND_FACTOR_SRC_COLOR;
			break;

		default:
			PX_ALWAYS_ASSERT();
		}

		switch (desc.dstBlendFunc)
		{
		case BLEND_ONE:
			mParticleBlendInfo->colorDst = SCE_GXM_BLEND_FACTOR_ONE;
			break;

		case BLEND_ONE_MINUS_SRC_ALPHA:
			mParticleBlendInfo->colorDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			break;

		case BLEND_ONE_MINUS_SRC_COLOR:
			mParticleBlendInfo->colorDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			break;

		default:
			PX_ALWAYS_ASSERT();
		}

		mParticleBlendInfo->alphaFunc = SCE_GXM_BLEND_FUNC_NONE;
		mParticleBlendInfo->alphaSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA; //SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		mParticleBlendInfo->alphaDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	}

	parseFragmentShader(fragmentProgramData, RendererMaterial::PASS_UNLIT);
	parseFragmentShader(fragmentProgramDataDL, RendererMaterial::PASS_DIRECTIONAL_LIGHT);
}

GXMRendererMaterial::~GXMRendererMaterial(void)
{
	for (size_t i = mFragmentPrograms.size(); i--;)
	{
		if(mFragmentPrograms[i].mFinalFragmentProgram)
		{
			sceGxmShaderPatcherReleaseFragmentProgram(mRenderer.getShaderPatcher(),mFragmentPrograms[i].mFinalFragmentProgram);
		}

		sceGxmShaderPatcherUnregisterProgram(mRenderer.getShaderPatcher(),mFragmentPrograms[i].mGmxFragmentProgramId);
	}

	mFragmentPrograms.clear();

	if(mFinalVertexProgram)
	{
		sceGxmShaderPatcherReleaseVertexProgram(mRenderer.getShaderPatcher(), mFinalVertexProgram);
	}

	sceGxmShaderPatcherUnregisterProgram(mRenderer.getShaderPatcher(),mGmxVertexProgramId);	

	delete mParticleBlendInfo;
}

SceGxmFragmentProgram* GXMRendererMaterial::getFinalFramentProgram(RendererMaterial::Pass pass) const
{
	for (size_t i = 0; i < mFragmentPrograms.size(); i++)
	{
		if(mFragmentPrograms[i].mPass == pass)
		{
			return mFragmentPrograms[i].mFinalFragmentProgram;
		}
	}

	return NULL;
}

const GXMRendererMaterial::FragmentProgram* GXMRendererMaterial::getFramentProgram(RendererMaterial::Pass pass) const
{
	for (size_t i = 0; i < mFragmentPrograms.size(); i++)
	{
		if(mFragmentPrograms[i].mPass == pass)
		{
			return &mFragmentPrograms[i];
		}
	}

	return NULL;
}


void GXMRendererMaterial::parseFragmentShader(const SceGxmProgram* program, RendererMaterial::Pass pass)
{
	int err = SCE_OK;

	if(program)
	{
		FragmentProgram fragmentProgram;
		fragmentProgram.mPass = pass;

		memset(&fragmentProgram.mShaderConstant,0,sizeof(ShaderConstant));

		err = sceGxmProgramCheck(program);
		PX_ASSERT(err == SCE_OK);

		err = sceGxmShaderPatcherRegisterProgram(mRenderer.getShaderPatcher(), program, &fragmentProgram.mGmxFragmentProgramId);
		PX_ASSERT(err == SCE_OK);

		fragmentProgram.mGmxFragmentProgram = sceGxmShaderPatcherGetProgramFromId(fragmentProgram.mGmxFragmentProgramId);

		err = sceGxmShaderPatcherCreateFragmentProgram(
			mRenderer.getShaderPatcher(),
			fragmentProgram.mGmxFragmentProgramId,
			SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
			SCE_GXM_MULTISAMPLE_NONE,
			mParticleBlendInfo,
			mGmxVertexProgram,
			&fragmentProgram.mFinalFragmentProgram);
		RENDERER_ASSERT(err == SCE_OK,"Failed to create fragment program");
		
		if(pass == PASS_UNLIT)
			loadCustomConstants(fragmentProgram.mGmxFragmentProgram, true);

		loadShaderConstants(mGmxVertexProgram, &fragmentProgram.mShaderConstant);
		loadShaderConstants(fragmentProgram.mGmxFragmentProgram, &fragmentProgram.mShaderConstant);

		mFragmentPrograms.push_back(fragmentProgram);
	}
}

RendererMaterial::VariableType GXMRendererMaterial::gxmTypetoVariableType(SceGxmParameterType type, SceGxmParameterCategory category, uint32_t componentCount, uint32_t arraysize)
{
	switch (type)
	{	
	case SCE_GXM_PARAMETER_TYPE_F32:
		{
			if(category == SCE_GXM_PARAMETER_CATEGORY_SAMPLER)
			{
				return VARIABLE_SAMPLER2D;
			}
			
			if(arraysize == 1)
			{
				switch(componentCount)
				{
				case 1:
					return VARIABLE_FLOAT;
				case 2:
					return VARIABLE_FLOAT2;
				case 3:
					return VARIABLE_FLOAT3;
				case 4:
					return VARIABLE_FLOAT4;
				}
			}

			if(arraysize == 4)
			{
				return VARIABLE_FLOAT4x4;
			}
			
		}
	default: return NUM_VARIABLE_TYPES;
	}
}

void GXMRendererMaterial::loadShaderConstants(const SceGxmProgram* program, ShaderConstant* sc)
{
	PX_ASSERT(program);

	if(!sc->mModelMatrixUniform)
		sc->mModelMatrixUniform = sceGxmProgramFindParameterByName(program, "g_modelMatrix");
	if(!sc->mViewMatrixUniform)
		sc->mViewMatrixUniform = sceGxmProgramFindParameterByName(program, "g_viewMatrix");
	if(!sc->mProjMatrixUniform)
		sc->mProjMatrixUniform = sceGxmProgramFindParameterByName(program, "g_projMatrix");
	if(!sc->mModelViewMatrixUniform)
		sc->mModelViewMatrixUniform = sceGxmProgramFindParameterByName(program, "g_modelViewMatrix");
	if(!sc->mBoneMatricesUniform)
		sc->mBoneMatricesUniform = sceGxmProgramFindParameterByName(program, "g_boneMatrices");	
	if(!sc->mAmbientColor)
		sc->mAmbientColor  = sceGxmProgramFindParameterByName(program, "g_ambientColor"); 

	if(!sc->mModelViewProjMatrixUniform)
		sc->mModelViewProjMatrixUniform = sceGxmProgramFindParameterByName(program, "g_modelViewProjMatrix");
	if(!sc->mFogColorAndDistance)
		sc->mFogColorAndDistance  = sceGxmProgramFindParameterByName(program, "g_fogColorAndDistance");
	if(!sc->mEyePosition)
		sc->mEyePosition = sceGxmProgramFindParameterByName(program, "g_eyePosition");
	if(!sc->mEyeDirection)
		sc->mEyeDirection = sceGxmProgramFindParameterByName(program, "g_eyeDirection");	

	if(!sc->mLightColor)
		sc->mLightColor = sceGxmProgramFindParameterByName(program, "g_lightColor");
	if(!sc->mLightIntensity)
		sc->mLightIntensity = sceGxmProgramFindParameterByName(program, "g_lightIntensity");
	if(!sc->mLightDirection)
		sc->mLightDirection = sceGxmProgramFindParameterByName(program, "g_lightDirection");
	if(!sc->mLightPosition)
		sc->mLightPosition = sceGxmProgramFindParameterByName(program, "g_lightPosition");
	if(!sc->mLightInnerRadius)
		sc->mLightInnerRadius = sceGxmProgramFindParameterByName(program, "g_lightInnerRadius");
	if(!sc->mLightOuterRadius)
		sc->mLightOuterRadius = sceGxmProgramFindParameterByName(program, "g_lightOuterRadius");
	if(!sc->mLightInnerCone)
		sc->mLightInnerCone = sceGxmProgramFindParameterByName(program, "g_lightInnerCone");
	if(!sc->mLightOuterCone)
		sc->mLightOuterCone = sceGxmProgramFindParameterByName(program, "g_lightOuterCone");

	if(!sc->mLightShadowMap)
		sc->mLightShadowMap  = sceGxmProgramFindParameterByName(program, "g_lightShadowMap");
	if(!sc->mLightShadowMatrix)
		sc->mLightShadowMatrix = sceGxmProgramFindParameterByName(program, "g_lightShadowMatrix");

	if(!sc->mShadeMode)
		sc->mShadeMode = sceGxmProgramFindParameterByName(program,"shadeMode");

	if(!sc->mVfaceScale)
		sc->mVfaceScale = sceGxmProgramFindParameterByName(program, "g_vfaceScale");
}

void GXMRendererMaterial::loadCustomConstants(const SceGxmProgram* program, bool fragment)
{
	PX_ASSERT(program);

	uint32_t count = sceGxmProgramGetParameterCount(program);	
	for (int i = 0; i < count; i++)
	{		
		const SceGxmProgramParameter* parameter =  sceGxmProgramGetParameter(program , i);
		SceGxmParameterCategory cat = sceGxmProgramParameterGetCategory(parameter);

		if(!fragment)
		{
			if(cat != SCE_GXM_PARAMETER_CATEGORY_UNIFORM)
			{
				mNumVertexAttributes++;		
			}		
		}

		const char* name = sceGxmProgramParameterGetName(parameter);
		SceGxmParameterType type = sceGxmProgramParameterGetType(parameter);
		uint32_t arraySize = sceGxmProgramParameterGetArraySize(parameter);
		uint32_t componentCount = sceGxmProgramParameterGetComponentCount(parameter);
		RendererMaterial::VariableType vtype = gxmTypetoVariableType(type, cat, componentCount, arraySize);
		
		if (strncmp(name, "g_", 2) && (vtype != NUM_VARIABLE_TYPES))
		{
			GXMVariable *var = new GXMVariable(name, vtype, m_variableBufferSize);
			var->m_resourceIndex = sceGxmProgramParameterGetResourceIndex(parameter);
			m_variables.push_back(var);
			m_variableBufferSize += var->getDataSize();
		}
	}
}

uint32_t GXMRendererMaterial::getResourceIndex(const char* name) const 
{
	for (int i = m_variables.size();i--; )
	{
		GXMVariable* var = (GXMVariable*) m_variables[i];
		if(strstr(var->getName(), name))
		{
			return var->m_resourceIndex;
		}

	}
	return 0;
}

void GXMRendererMaterial::fillShaderAttribute(const GXMRenderer::ShaderEnvironment& env, GXMVariable* var, SceGxmVertexAttribute& attribute) const
{
	const char* name = var->getName();
	
	const GXMRendererVertexBuffer* vb = (const GXMRendererVertexBuffer*) env.mVertexBuffer;
	const GXMRendererInstanceBuffer* instb = (const GXMRendererInstanceBuffer*) env.mInstanceBuffer;

	if(strstr(name,"localSpacePosition"))
	{
		PxI32 offset = vb->GetSemanticOffset(RendererVertexBuffer::SEMANTIC_POSITION);

		if(offset >= 0)
		{
			attribute.streamIndex = 0;
			attribute.offset = offset;
			attribute.format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attribute.componentCount = 3;

			attribute.regIndex = getResourceIndex("localSpacePosition");

			return;
		}
	}

	if(strstr(name, "vertexColor"))
	{
		PxI32 offset = vb->GetSemanticOffset(RendererVertexBuffer::SEMANTIC_COLOR);

		if(offset >= 0)
		{
			attribute.streamIndex = 0;
			attribute.offset = offset;
			attribute.format = SCE_GXM_ATTRIBUTE_FORMAT_U8;
			attribute.componentCount = 4;

			attribute.regIndex = getResourceIndex("vertexColor");

			return;
		}
	}

	if(strstr(name, "localSpaceNormal"))
	{
		PxI32 offset = vb->GetSemanticOffset(RendererVertexBuffer::SEMANTIC_NORMAL);

		if(offset >= 0)
		{
			attribute.streamIndex = 0;
			attribute.offset = offset;
			attribute.format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attribute.componentCount = 3;

			attribute.regIndex = getResourceIndex("localSpaceNormal");

			return;
		}
	}

	if(strstr(name, "Texcoord0"))
	{
		PxI32 offset = vb->GetSemanticOffset(RendererVertexBuffer::SEMANTIC_TEXCOORD0);

		if(offset >= 0)
		{
			attribute.streamIndex = 0;
			attribute.offset = offset;
			attribute.format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			attribute.componentCount = 2;

			attribute.regIndex = getResourceIndex("Texcoord0");

			return;
		}
	}

	if(strstr(name, "instanceOffset"))
	{
		PX_ASSERT(instb);

		PxU32 offset = instb->getOffsetForSemantic(RendererInstanceBuffer::SEMANTIC_POSITION);

		attribute.streamIndex = 1;
		attribute.offset = offset;
		attribute.format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attribute.componentCount = 3;

		attribute.regIndex = getResourceIndex("instanceOffset");

		return;
	}

	if(strstr(name, "instanceNormalX"))
	{
		PX_ASSERT(instb);

		PxU32 offset = instb->getOffsetForSemantic(RendererInstanceBuffer::SEMANTIC_NORMALX);

		attribute.streamIndex = 1;
		attribute.offset = offset;
		attribute.format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attribute.componentCount = 3;

		attribute.regIndex = getResourceIndex("instanceNormalX");

		return;
	}

	if(strstr(name, "instanceNormalY"))
	{
		PX_ASSERT(instb);

		PxU32 offset = instb->getOffsetForSemantic(RendererInstanceBuffer::SEMANTIC_NORMALY);

		attribute.streamIndex = 1;
		attribute.offset = offset;
		attribute.format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attribute.componentCount = 3;

		attribute.regIndex = getResourceIndex("instanceNormalY");

		return;
	}

	if(strstr(name, "instanceNormalZ"))
	{
		PX_ASSERT(instb);

		PxU32 offset = instb->getOffsetForSemantic(RendererInstanceBuffer::SEMANTIC_NORMALZ);

		attribute.streamIndex = 1;
		attribute.offset = offset;
		attribute.format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attribute.componentCount = 3;

		attribute.regIndex = getResourceIndex("instanceNormalZ");

		return;
	}

	attribute.streamIndex = 0;
	attribute.offset = 0;
	attribute.format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	attribute.componentCount = 1;

	attribute.regIndex = getResourceIndex(name);

}

void GXMRendererMaterial::bindEnvironment(const GXMRenderer::ShaderEnvironment& env, bool instanced) const
{
	SceGxmErrorCode err = -1;

	// TODO find out where this instanced part were originally planed to be implemented
	if(instanced){
			SceGxmVertexAttribute basicVertexAttributes[20];		
			SceGxmVertexStream basicVertexStreams[2];

			for (int i = 0; i < mNumVertexAttributes; i++)
			{
				GXMVariable* var = (GXMVariable*) m_variables[i];

				fillShaderAttribute(env, var, basicVertexAttributes[i]);		
			}

			basicVertexStreams[0].stride = env.mVertexBuffer->GetStride();
			basicVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;
			basicVertexStreams[1].stride = env.mInstanceBuffer->getStride(); // stride == sizeof(matrix)			
			basicVertexStreams[1].indexSource = SCE_GXM_INDEX_SOURCE_INSTANCE_16BIT;

			// create cube vertex program
			err = sceGxmShaderPatcherCreateVertexProgram(
				mRenderer.getShaderPatcher(),
				mGmxVertexProgramId,
				basicVertexAttributes,
				mNumVertexAttributes,
				basicVertexStreams,
				2,
				&mFinalVertexProgram);
	}
	else{
		SceGxmVertexAttribute basicVertexAttributes[20];
		SceGxmVertexStream basicVertexStreams[1];

		for (int i = 0; i < mNumVertexAttributes; i++)
		{
			GXMVariable* var = (GXMVariable*) m_variables[i];

			fillShaderAttribute(env, var, basicVertexAttributes[i]);		
		}

		basicVertexStreams[0].stride = env.mVertexBuffer->GetStride();
		basicVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

		// create cube vertex program
		err = sceGxmShaderPatcherCreateVertexProgram(
			mRenderer.getShaderPatcher(),
			mGmxVertexProgramId,
			basicVertexAttributes,
			mNumVertexAttributes,
			basicVertexStreams,
			1,
			&mFinalVertexProgram);
	}

	if(err != SCE_OK)
	{
		RENDERER_ASSERT(err == SCE_OK , "Failed to create vertex program.");
	}
}

void printMatrix(const float* mat)
{
	printf("Matrix %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f \n", mat[0], mat[1],
		mat[2], mat[3] ,mat[4] ,mat[5] ,mat[6] ,mat[7] ,mat[8] ,mat[9]);
}

void GXMRendererMaterial::bind(RendererMaterial::Pass pass, RendererMaterialInstance *materialInstance, bool instanced) const
{

	const GXMRenderer::ShaderEnvironment &shaderEnv = mRenderer.getShaderEnvironment();

	if(shaderEnv.mValid)
	{
		if(mFinalVertexProgram)
		{
			sceGxmSetVertexProgram(mRenderer.getContext(), mFinalVertexProgram);
		}
		else
		{
			bindEnvironment(shaderEnv, instanced);

			sceGxmSetVertexProgram(mRenderer.getContext(), mFinalVertexProgram);
		}

		const FragmentProgram* fp = getFramentProgram(pass);
		if(fp && fp->mFinalFragmentProgram)
		{			
			sceGxmSetFragmentProgram(mRenderer.getContext(), fp->mFinalFragmentProgram);
		}
		else
		{
			PX_ALWAYS_ASSERT();
		}


		// set the vertex program constants
		
		sceGxmReserveFragmentDefaultUniformBuffer(mRenderer.getContext(), &mFragmentUniformDefaultBuffer);
		sceGxmReserveVertexDefaultUniformBuffer(  mRenderer.getContext(), &mVertexUniformDefaultBuffer );
		if(fp->mShaderConstant.mModelMatrixUniform)
		{
			sceGxmSetUniformDataF( mVertexUniformDefaultBuffer, fp->mShaderConstant.mModelMatrixUniform, 0, 16, (const float*) &shaderEnv.mModelMatrix);
		}

		if(fp->mShaderConstant.mViewMatrixUniform)
		{
			sceGxmSetUniformDataF( mVertexUniformDefaultBuffer, fp->mShaderConstant.mViewMatrixUniform, 0, 16, (const float*) &shaderEnv.mViewMatrix);			
		}

		if(fp->mShaderConstant.mProjMatrixUniform)
		{
			sceGxmSetUniformDataF( mVertexUniformDefaultBuffer, fp->mShaderConstant.mProjMatrixUniform, 0, 16, (const float*) &shaderEnv.mProjMatrix);			
		}

		if(fp->mShaderConstant.mModelViewMatrixUniform)
		{
			sceGxmSetUniformDataF( mVertexUniformDefaultBuffer, fp->mShaderConstant.mModelViewMatrixUniform, 0, 16, (const float*) &shaderEnv.mModelViewMatrix);			
		}

		if(fp->mShaderConstant.mEyePosition)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mEyePosition, 0, 3, (const float*) &shaderEnv.mEyePosition);
		}

		if(fp->mShaderConstant.mEyeDirection)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mEyeDirection, 0, 3, (const float*) &shaderEnv.mEyeDirection);
		}

		if(fp->mShaderConstant.mAmbientColor)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mAmbientColor, 0, 3, (const float*) &shaderEnv.mAmbientColor);
		}

		float shadeMode = 0.0f;
		if(fp->mShaderConstant.mShadeMode)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mShadeMode, 0, 1, (const float*) &shadeMode);
		}

		if(fp->mShaderConstant.mLightColor)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mLightColor, 0, 3, (const float*) &shaderEnv.mLightColor);
		}

		if(fp->mShaderConstant.mLightIntensity)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mLightIntensity, 0, 1, (const float*) &shaderEnv.mLightIntensity);			
		}
		
		if(fp->mShaderConstant.mLightDirection)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mLightDirection, 0, 3, (const float*) &shaderEnv.mLightDirection[0]);
		}

		if(fp->mShaderConstant.mLightPosition)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mLightPosition, 0, 4, (const float*) &shaderEnv.mLightPosition);			
		}

		if(fp->mShaderConstant.mLightInnerRadius)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mLightInnerRadius, 0, 4, (const float*) &shaderEnv.mLightInnerRadius);			
		}

		if(fp->mShaderConstant.mLightOuterRadius)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mLightOuterRadius, 0, 4, (const float*) &shaderEnv.mLightOuterRadius);			
		}

		if(fp->mShaderConstant.mLightInnerCone)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mLightInnerCone, 0, 4, (const float*) &shaderEnv.mLightInnerCone);			
		}

		if(fp->mShaderConstant.mLightOuterCone)
		{
			sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, fp->mShaderConstant.mLightOuterCone, 0, 4, (const float*) &shaderEnv.mLightOuterCone);			
		}

		const SceGxmProgramParameter * par = sceGxmProgramFindParameterByName(mGmxVertexProgram, "particleSize");
		if(par)
		{
			static float particleSize = 1.0f;
			sceGxmSetUniformDataF( mVertexUniformDefaultBuffer, par, 0, 1, &particleSize);
		}

		par = sceGxmProgramFindParameterByName(mGmxVertexProgram, "windowWidth");
		if(par)
		{
			static float windowWidth = 960.0f;
			sceGxmSetUniformDataF( mVertexUniformDefaultBuffer, par, 0, 1, &windowWidth);
		}

	}

	RendererMaterial::bind(pass, materialInstance, instanced);
}

void GXMRendererMaterial::bindMeshState(bool instanced) const
{

}

void GXMRendererMaterial::unbind(void) const
{

}


void GXMRendererMaterial::bindVariable(Pass pass, const Variable &variable, const void *data) const
{
	const FragmentProgram* fp = getFramentProgram(pass);

	if(!fp)
		return;	

	if(strstr(variable.getName(),"diffuseColor"))
	{	
		const SceGxmProgramParameter * par = sceGxmProgramFindParameterByName(fp->mGmxFragmentProgram, "diffuseColor");
		PX_ASSERT(par);				
		sceGxmSetUniformDataF( mFragmentUniformDefaultBuffer, par, 0, 4, (const float*) data);
	}

	if(strstr(variable.getName(), "diffuseTexture0"))
	{
		const SceGxmProgramParameter * par = sceGxmProgramFindParameterByName(fp->mGmxFragmentProgram, "diffuseTexture0");
		if(!par)
			return;

		data = *(void**)data;
		RENDERER_ASSERT(data, "NULL Sampler.");

		if(data)
		{
			GXMRendererTexture2D *tex = const_cast<GXMRendererTexture2D*>(static_cast<const GXMRendererTexture2D*>(data));

			tex->bind(0);
		}		

		return;
	}

	if(strstr(variable.getName(), "diffuseTexture1"))
	{
		const SceGxmProgramParameter * par = sceGxmProgramFindParameterByName(fp->mGmxFragmentProgram, "diffuseTexture1");
		if(!par)
			return;

		data = *(void**)data;
		RENDERER_ASSERT(data, "NULL Sampler.");
		if(data)
		{
			GXMRendererTexture2D *tex = const_cast<GXMRendererTexture2D*>(static_cast<const GXMRendererTexture2D*>(data));

			tex->bind(1);
		}		

		return;
	}


	if(strstr(variable.getName(), "diffuseTexture"))
	{
		const SceGxmProgramParameter * par = sceGxmProgramFindParameterByName(fp->mGmxFragmentProgram, "diffuseTexture");
		if(!par)
			return;

		data = *(void**)data;
		RENDERER_ASSERT(data, "NULL Sampler.");
		if(data)
		{
			GXMRendererTexture2D *tex = const_cast<GXMRendererTexture2D*>(static_cast<const GXMRendererTexture2D*>(data));

			tex->bind(0);
		}		
	}

}



#endif // #if defined(RENDERER_ENABLE_LIBGXM)
