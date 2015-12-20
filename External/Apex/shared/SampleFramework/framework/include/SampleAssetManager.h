
#ifndef SAMPLE_ASSET_MANAGER_H
#define SAMPLE_ASSET_MANAGER_H
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
#include <SampleTextureAsset.h>
#include <stdio.h>
#include <vector>

#include <RendererConfig.h>



namespace SampleRenderer
{
	class Renderer;
}

namespace SampleFramework
{
	class SampleAsset;

	class SampleAssetManager
	{
	public:

		class SampleAssetCreator {
		public:
			virtual SampleAsset* create(const char*, SampleAsset::Type) = 0;
		};

	public:
									SampleAssetManager(SampleRenderer::Renderer &renderer,
													   SampleAssetCreator* fallbackAssetCreator = NULL);
									~SampleAssetManager();

		SampleRenderer::Renderer&	getRenderer() { return m_renderer; }

		SampleAsset*				getAsset(const char* path, SampleAsset::Type type);
		void						returnAsset(SampleAsset& asset);

	protected:
		SampleAsset*				findAsset(const char* path);
		SampleAsset*				loadAsset(const char* path, SampleAsset::Type type);
		void						releaseAsset(SampleAsset& asset);

		void						addAssetUser(SampleAsset& asset);
		void						addAsset(SampleAsset* asset);
		void						deleteAsset(SampleAsset* asset);

		SampleAsset*				loadXMLAsset(FILE& file, const char* path);
		SampleAsset*				loadTextureAsset(FILE& file, const char* path, SampleTextureAsset::Type texType);
		SampleAsset*				loadODSAsset(FILE& file, const char* path);

	private:
		SampleAssetManager&	operator=(const SampleAssetManager&) { return *this; }

	protected:
		SampleRenderer::Renderer&	m_renderer;
		SampleAssetCreator*			m_fallbackAssetCreator;
		std::vector<SampleAsset*>	m_assets;
	};

	void		addSearchPath(const char* path);
	void		clearSearchPaths();
	FILE*		findFile(const char* path, bool binary = true);
	const char*	findPath(const char* path);

	/**
	Search for the speficied path in the current directory. If not found, move up in the folder hierarchy
	until the path can be found or until the specified maximum number of steps is reached.

	\note On consoles no recursive search will be done

	\param	[in] path The path to look for
	\param	[out] buffer Buffer to store the (potentially) adjusted path
	\param	[in] bufferSize Size of buffer
	\param	[in] maxRecursion Maximum number steps to move up in the folder hierarchy
	return	true if path was found
	*/
	bool		searchForPath(const char* path, char* buffer, int bufferSize, bool isReadOnly, int maxRecursion);

} // namespace SampleFramework

#endif
