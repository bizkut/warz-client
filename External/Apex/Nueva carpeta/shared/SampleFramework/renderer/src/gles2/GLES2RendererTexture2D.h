
#ifndef GLES2_RENDERER_TEXTURE_2D_H
#define GLES2_RENDERER_TEXTURE_2D_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_GLES2)

#include <RendererTexture2D.h>
#include <RendererTexture2DDesc.h>
#include "GLES2Renderer.h"

namespace SampleRenderer
{
class RendererTextureDesc;

class GLES2RendererTexture2D : public RendererTexture2D
{
	public:
		GLES2RendererTexture2D(const RendererTextureDesc &desc);
		virtual ~GLES2RendererTexture2D(void);
	
	public:
		virtual void *lockLevel(PxU32 level, PxU32 &pitch);
		virtual void  unlockLevel(PxU32 level);
		
		void bind(PxU32 textureUnit);
		void select(PxU32 stageIndex);
		
	private:
		GLuint m_textureid;
		GLuint m_glformat;
		GLuint m_glinternalformat;
		GLuint m_gltype;
		
		PxU32  m_width;
		PxU32  m_height;
		
		PxU32  m_numLevels;
		
		PxU8 **m_data;
};
}
#endif // #if defined(RENDERER_ENABLE_GLES2)
#endif
