
#ifndef GLES2_RENGERER_INDEXBUFFER_H
#define GLES2_RENDERER_INDEXBUFFER_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_GLES2)

#include <RendererIndexBuffer.h>
#include "GLES2Renderer.h"

namespace SampleRenderer
{
class GLES2RendererIndexBuffer : public RendererIndexBuffer
{
	public:
		GLES2RendererIndexBuffer(const RendererIndexBufferDesc &desc);
		virtual ~GLES2RendererIndexBuffer(void);
		
	public:
		virtual void *lock(void);
		virtual void  unlock(void);
		
	private:
		virtual void bind(void) const;
		virtual void unbind(void) const;
	
	private:
		GLuint m_ibo;
		PxU32  m_indexSize;
};
}
#endif // #if defined(RENDERER_ENABLE_GLES2)
#endif
