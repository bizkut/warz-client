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
#ifndef RENDERER_LIGHT_H
#define RENDERER_LIGHT_H

#include <RendererConfig.h>
#include <RendererColor.h>
#include <RendererMaterial.h>
#include <RendererProjection.h>

namespace SampleRenderer
{

	class RendererLightDesc;
	class Renderer;
	class RendererLight
	{
		friend class Renderer;
	public:
		enum Type
		{
			TYPE_POINT = 0,
			TYPE_DIRECTIONAL,
			TYPE_SPOT,

			NUM_TYPES
		}_Type;

	protected:
		RendererLight(const RendererLightDesc &desc);
		virtual ~RendererLight(void);

	public:
		void                      release(void);

		Type                      getType(void) const;

		RendererMaterial::Pass    getPass(void) const;

		const RendererColor      &getColor(void) const;
		void                      setColor(const RendererColor &color);

		float                     getIntensity(void) const;
		void                      setIntensity(float intensity);

		bool                      isLocked(void) const;

		RendererTexture2D        *getShadowMap(void) const;
		void                      setShadowMap(RendererTexture2D *shadowMap);

		const physx::PxTransform &getShadowTransform(void) const	{ return m_shadowTransform; }
		void                      setShadowTransform(const physx::PxTransform &shadowTransform) { m_shadowTransform = shadowTransform; }

		const RendererProjection &getShadowProjection(void) const;
		void                      setShadowProjection(const RendererProjection &shadowProjection);

	private:
		RendererLight &operator=(const RendererLight &) { return *this; }

		virtual void bind(void) const = 0;

	protected:
		const Type         m_type;

		RendererColor      m_color;
		float              m_intensity;

		RendererTexture2D *m_shadowMap;
		physx::PxTransform m_shadowTransform;
		RendererProjection m_shadowProjection;

	private:
		Renderer          *m_renderer;
	};

} // namespace SampleRenderer

#endif
