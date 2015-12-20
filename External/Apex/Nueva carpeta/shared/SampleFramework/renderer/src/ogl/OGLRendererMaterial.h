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
#ifndef OGL_RENDERER_MATERIAL_H
#define OGL_RENDERER_MATERIAL_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_OPENGL)

#include <RendererMaterial.h>

#include "OGLRenderer.h"

namespace SampleRenderer
{

	class OGLRendererMaterial : public RendererMaterial
	{
		friend class OGLRendererMesh;
	public:
		OGLRendererMaterial(OGLRenderer &renderer, const RendererMaterialDesc &desc);
		virtual ~OGLRendererMaterial(void);
		virtual void setModelMatrix(const float *matrix) 
		{
			PX_FORCE_PARAMETER_REFERENCE(matrix);
			PX_ALWAYS_ASSERT();
		}

	private:
		virtual const Renderer& getRenderer() const { return m_renderer; }
		virtual void bind(RendererMaterial::Pass pass, RendererMaterialInstance *materialInstance, bool instanced) const;
		virtual void bindMeshState(bool instanced) const;
		virtual void unbind(void) const;
		virtual void bindVariable(Pass pass, const Variable &variable, const void *data) const;

#if defined(RENDERER_ENABLE_CG)
		void loadCustomConstants(CGprogram program, Pass pass);
#endif

	private:
#if defined(RENDERER_ENABLE_CG)
		class CGVariable : public Variable
		{
			friend class OGLRendererMaterial;
		public:
			CGVariable(const char *name, VariableType type, PxU32 offset);
			virtual ~CGVariable(void);

			void addVertexHandle(CGparameter handle);
			void addFragmentHandle(CGparameter handle, Pass pass);

		private:
			CGparameter m_vertexHandle;
			CGparameter m_fragmentHandles[NUM_PASSES];
		};
#endif

	private:
		OGLRendererMaterial &operator=(const OGLRendererMaterial&) { return *this; }

	private:
		OGLRenderer &m_renderer;

		GLenum       m_glAlphaTestFunc;

#if defined(RENDERER_ENABLE_CG)
		CGprofile    m_vertexProfile;
		CGprogram    m_vertexProgram;

		CGprofile    m_fragmentProfile;
		CGprogram    m_fragmentPrograms[NUM_PASSES];
#if defined(RENDERER_PS3)
	public:
		CGprogram GetVertexProgramPS3(){ return m_vertexProgram;}
#endif
#endif
	};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_OPENGL)
#endif
