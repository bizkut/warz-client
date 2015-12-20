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
#ifndef RENDERER_MATERIAL_H
#define RENDERER_MATERIAL_H

#include <RendererConfig.h>
#include <vector>

namespace SampleRenderer
{

	class RendererMaterialDesc;

	class RendererTexture;
	class RendererTextureDesc;
	
	//ps3 need this declare
	class RendererMaterialInstance;

	//ps3 need this declare
	class Renderer;

	class RendererMaterial
	{
		friend class Renderer;
		friend class RendererMaterialInstance;
	public:
		typedef enum Type
		{
			TYPE_UNLIT = 0,
			TYPE_LIT,		
			NUM_TYPES,
		} Type;

		typedef enum AlphaTestFunc
		{
			ALPHA_TEST_ALWAYS = 0, // disabled alpha testing...
			ALPHA_TEST_EQUAL,
			ALPHA_TEST_NOT_EQUAL,
			ALPHA_TEST_LESS,
			ALPHA_TEST_LESS_EQUAL,
			ALPHA_TEST_GREATER,
			ALPHA_TEST_GREATER_EQUAL,

			NUM_ALPHA_TEST_FUNCS,
		} AlphaTestFunc;

		typedef enum BlendFunc
		{
			BLEND_ZERO = 0,
			BLEND_ONE,
			BLEND_SRC_COLOR,
			BLEND_ONE_MINUS_SRC_COLOR,
			BLEND_SRC_ALPHA,
			BLEND_ONE_MINUS_SRC_ALPHA,
			BLEND_DST_ALPHA,
			BLEND_ONE_MINUS_DST_ALPHA,
			BLEND_DST_COLOR,
			BLEND_ONE_MINUS_DST_COLOR,
			BLEND_SRC_ALPHA_SATURATE,
		} BlendFunc;

		typedef enum Pass
		{
			PASS_UNLIT = 0,

			PASS_AMBIENT_LIGHT,
			PASS_POINT_LIGHT,
			PASS_DIRECTIONAL_LIGHT,
			PASS_SPOT_LIGHT_NO_SHADOW,
			PASS_SPOT_LIGHT,

			PASS_NORMALS,
			PASS_DEPTH,

			// LRR: The deferred pass causes compiles with the ARB_draw_buffers profile option, creating 
			// multiple color draw buffers.  This doesn't work in OGL on ancient Intel parts.
			//PASS_DEFERRED,

			NUM_PASSES,
		} Pass;

		typedef enum VariableType
		{
			VARIABLE_FLOAT = 0,
			VARIABLE_FLOAT2,
			VARIABLE_FLOAT3,
			VARIABLE_FLOAT4,
			VARIABLE_FLOAT4x4,
			VARIABLE_INT,

			VARIABLE_SAMPLER2D,
			VARIABLE_SAMPLER3D,

			NUM_VARIABLE_TYPES
		} VariableType;

		class Variable
		{
			friend class RendererMaterial;
		protected:
			Variable(const char *name, VariableType type, PxU32 offset);
			virtual ~Variable(void);

		public:
			const char  *getName(void) const;
			VariableType getType(void) const;
			PxU32        getDataOffset(void) const;
			PxU32        getDataSize(void) const;

			void		 setSize(PxU32);

		private:
			char        *m_name;
			VariableType m_type;
			PxU32        m_offset;
			PxI32		 m_size;
		};

	public:
		static const char *getPassName(Pass pass);
		virtual void setModelMatrix(const PxF32 *matrix) = 0;

	protected:
		RendererMaterial(const RendererMaterialDesc &desc, bool enableMaterialCaching);
		virtual ~RendererMaterial(void);

	public:
		void incRefCount() { m_refCount++; }
		void release(void);
		const Variable *findVariable(const char *name, VariableType varType);

		PX_FORCE_INLINE	Type			getType(void)						const { return m_type; }
		PX_FORCE_INLINE	AlphaTestFunc	getAlphaTestFunc(void)				const { return m_alphaTestFunc; }
		PX_FORCE_INLINE	float			getAlphaTestRef(void)				const { return m_alphaTestRef; }
		PX_FORCE_INLINE	bool			getBlending(void)					const { return m_blending && !rendererBlendingOverrideEnabled(); }
		PX_FORCE_INLINE	BlendFunc		getSrcBlendFunc(void)				const { return m_srcBlendFunc; }
		PX_FORCE_INLINE	BlendFunc		getDstBlendFunc(void)				const { return m_dstBlendFunc; }
		PX_FORCE_INLINE	PxU32	getMaterialInstanceDataSize(void)	const { return m_variableBufferSize; }

	protected:
		virtual const Renderer& getRenderer() const = 0;
		virtual void bind(RendererMaterial::Pass pass, RendererMaterialInstance *materialInstance, bool instanced) const;
		virtual void bindMeshState(bool instanced) const = 0;
		virtual void unbind(void) const = 0;
		virtual void bindVariable(Pass pass, const Variable &variable, const void *data) const = 0;

		RendererMaterial &operator=(const RendererMaterial&) { return *this; }

		bool rendererBlendingOverrideEnabled() const;

	protected:
		const Type          m_type;

		const AlphaTestFunc m_alphaTestFunc;
		float               m_alphaTestRef;

		bool                m_blending;
		const BlendFunc     m_srcBlendFunc;
		const BlendFunc     m_dstBlendFunc;

		std::vector<Variable*> m_variables;
		PxU32               m_variableBufferSize;

		PxI32               m_refCount;
		bool				mEnableMaterialCaching;
	};
} // namespace SampleRenderer

#endif
