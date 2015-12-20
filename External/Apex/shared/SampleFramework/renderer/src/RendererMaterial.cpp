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
#include <RendererMaterial.h>

#include <Renderer.h>
#include <RendererMaterialDesc.h>
#include <RendererMaterialInstance.h>

// for PsString.h
namespace physx
{
	namespace string
	{}
}
#include <PsString.h>
#undef PxI32

using namespace SampleRenderer;

static PxU32 getVariableTypeSize(RendererMaterial::VariableType type)
{
	PxU32 size = 0;
	switch(type)
	{
	case RendererMaterial::VARIABLE_FLOAT:     size = sizeof(float)*1;            break;
	case RendererMaterial::VARIABLE_FLOAT2:    size = sizeof(float)*2;            break;
	case RendererMaterial::VARIABLE_FLOAT3:    size = sizeof(float)*3;            break;
	case RendererMaterial::VARIABLE_FLOAT4:    size = sizeof(float)*4;            break;
	case RendererMaterial::VARIABLE_FLOAT4x4:  size = sizeof(float)*4*4;          break;
	case RendererMaterial::VARIABLE_INT:       size = sizeof(int)*1;              break;
	case RendererMaterial::VARIABLE_SAMPLER2D: size = sizeof(RendererTexture2D*); break;
	case RendererMaterial::VARIABLE_SAMPLER3D: size = sizeof(RendererTexture3D*); break;
	default: break;
	}
	RENDERER_ASSERT(size>0, "Unable to compute Variable Type size.");
	return size;
}

RendererMaterial::Variable::Variable(const char *name, VariableType type, unsigned int offset)
{
	size_t len = strlen(name)+1;
	m_name = new char[len];
	physx::string::strcpy_s(m_name, len, name);
	m_type   = type;
	m_offset = offset;
	m_size = -1;
}

void RendererMaterial::Variable::setSize(PxU32 size) 
{
	m_size = size;
}

RendererMaterial::Variable::~Variable(void)
{
	if(m_name) delete [] m_name;
}

const char *RendererMaterial::Variable::getName(void) const
{
	return m_name;
}

RendererMaterial::VariableType RendererMaterial::Variable::getType(void) const
{
	return m_type;
}

PxU32 RendererMaterial::Variable::getDataOffset(void) const
{
	return m_offset;
}

PxU32 RendererMaterial::Variable::getDataSize(void) const
{
	return m_size != -1 ? m_size : getVariableTypeSize(m_type);
}

const char *RendererMaterial::getPassName(Pass pass)
{
	const char *passName = 0;
	switch(pass)
	{
	case PASS_UNLIT:                passName="PASS_UNLIT";                break;

	case PASS_AMBIENT_LIGHT:        passName="PASS_AMBIENT_LIGHT";        break;
	case PASS_POINT_LIGHT:          passName="PASS_POINT_LIGHT";          break;
	case PASS_DIRECTIONAL_LIGHT:    passName="PASS_DIRECTIONAL_LIGHT";    break;
	case PASS_SPOT_LIGHT_NO_SHADOW: passName="PASS_SPOT_LIGHT_NO_SHADOW"; break;
	case PASS_SPOT_LIGHT:           passName="PASS_SPOT_LIGHT";           break;

	case PASS_NORMALS:              passName="PASS_NORMALS";              break;
	case PASS_DEPTH:                passName="PASS_DEPTH";                break;
	default: break;

		// LRR: The deferred pass causes compiles with the ARB_draw_buffers profile option, creating 
		// multiple color draw buffers.  This doesn't work in OGL on ancient Intel parts.
		//case PASS_DEFERRED:          passName="PASS_DEFERRED";          break;
	}
	RENDERER_ASSERT(passName, "Unable to obtain name for the given Material Pass.");
	return passName;
}

RendererMaterial::RendererMaterial(const RendererMaterialDesc &desc, bool enableMaterialCaching) :
	m_type(desc.type),
	m_alphaTestFunc(desc.alphaTestFunc),
	m_srcBlendFunc(desc.srcBlendFunc),
	m_dstBlendFunc(desc.dstBlendFunc),
	m_refCount(1),
	mEnableMaterialCaching(enableMaterialCaching)
{
	m_alphaTestRef       = desc.alphaTestRef;
	m_blending           = desc.blending;
	m_variableBufferSize = 0;
}

RendererMaterial::~RendererMaterial(void)
{
	RENDERER_ASSERT(m_refCount == 0, "RendererMaterial was not released as often as it was created");

	PxU32 numVariables = (PxU32)m_variables.size();
	for(PxU32 i=0; i<numVariables; i++)
	{
		delete m_variables[i];
	}
}

void RendererMaterial::release()
{
	m_refCount--;

	if (!mEnableMaterialCaching)
	{
		PX_ASSERT(m_refCount == 0);
		delete this;
	}
}

void RendererMaterial::bind(RendererMaterial::Pass pass, RendererMaterialInstance *materialInstance, bool instanced) const
{
	if(materialInstance)
	{
		PxU32 numVariables = (PxU32)m_variables.size();
		for(PxU32 i = 0; i < numVariables; i++)
		{
			const Variable &variable = *m_variables[i];
			bindVariable(pass, variable, materialInstance->m_data+variable.getDataOffset());
		}
	}
}

bool RendererMaterial::rendererBlendingOverrideEnabled() const
{
	return getRenderer().blendingOverrideEnabled(); 
}

const RendererMaterial::Variable *RendererMaterial::findVariable(const char *name, RendererMaterial::VariableType varType)
{
	RendererMaterial::Variable *var = 0;
	PxU32 numVariables = (PxU32)m_variables.size();
	for(PxU32 i=0; i<numVariables; i++)
	{
		RendererMaterial::Variable &v = *m_variables[i];
		if(!strcmp(v.getName(), name))
		{
			var = &v;
			break;
		}
	}
	if(var && var->getType() != varType)
	{
		var = 0;
	}
	return var;
}
