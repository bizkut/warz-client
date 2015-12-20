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
#include <RendererMaterialInstance.h>
#include <RendererMaterial.h>

using namespace SampleRenderer;

RendererMaterialInstance::RendererMaterialInstance(RendererMaterial &material) :
m_material(material)
{
	m_data = 0;
	PxU32 dataSize = m_material.getMaterialInstanceDataSize();
	if(dataSize > 0)
	{
		m_data = new PxU8[dataSize];
		memset(m_data, 0, dataSize);
	}
}

RendererMaterialInstance::RendererMaterialInstance(const RendererMaterialInstance& other) :
m_material(other.m_material)
{
	PxU32 dataSize = m_material.getMaterialInstanceDataSize();
	if (dataSize > 0)
	{
		m_data = new PxU8[dataSize];
		memcpy(m_data, other.m_data, dataSize);
	}
}

RendererMaterialInstance::~RendererMaterialInstance(void)
{
	if(m_data) delete [] m_data;
}

void RendererMaterialInstance::writeData(const RendererMaterial::Variable &var, const void *data)
{
	if(m_data && data)
	{
		memcpy(m_data+var.getDataOffset(), data, var.getDataSize());
	}
}

RendererMaterialInstance &RendererMaterialInstance::operator=(const RendererMaterialInstance &b)
{
	PX_ASSERT(&m_material == &b.m_material);
	if(&m_material == &b.m_material)
	{
		memcpy(m_data, b.m_data, m_material.getMaterialInstanceDataSize());
	}
	return *this;
}
