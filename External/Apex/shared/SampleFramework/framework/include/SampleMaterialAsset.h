
#ifndef SAMPLE_MATERIAL_ASSET_H
#define SAMPLE_MATERIAL_ASSET_H
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
#include <SampleAsset.h>
#include <vector>

namespace FAST_XML
{
	class xml_node;
}

namespace SampleRenderer
{
	class RendererMaterial;
	class RendererMaterialInstance;
}

namespace SampleFramework
{
	class SampleAssetManager;

	class SampleMaterialAsset : public SampleAsset
	{
		friend class SampleAssetManager;
	protected:
		SampleMaterialAsset(SampleAssetManager &assetManager, FAST_XML::xml_node &xmlroot, const char *path);
		SampleMaterialAsset(SampleAssetManager &assetManager, Type type, const char *path);
		virtual ~SampleMaterialAsset(void);

	public:
		size_t                                    getNumVertexShaders() const;
		SampleRenderer::RendererMaterial         *getMaterial(size_t vertexShaderIndex = 0);
		SampleRenderer::RendererMaterialInstance *getMaterialInstance(size_t vertexShaderIndex = 0);
		unsigned int                              getMaxBones(size_t vertexShaderIndex) const;

	public:
		virtual bool isOk(void) const;

	protected:
		SampleAssetManager       &m_assetManager;
		struct MaterialStruct
		{
			SampleRenderer::RendererMaterial         *m_material;
			SampleRenderer::RendererMaterialInstance *m_materialInstance;
			unsigned int                              m_maxBones;
		};
		std::vector<MaterialStruct> m_vertexShaders;
		std::vector<SampleAsset*> m_assets;
	};

} // namespace SampleFramework

#endif
