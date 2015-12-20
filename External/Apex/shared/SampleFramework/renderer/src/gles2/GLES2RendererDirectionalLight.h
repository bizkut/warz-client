
#ifndef GLES2_RENDERER_DIRECTIONAL_LIGHT_H
#define GLES2_RENDERER_DIRECTIONAL_LIGHT_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_GLES2)

#include <RendererDirectionalLight.h>

#include "GLES2Renderer.h"

namespace SampleRenderer
{
class RendererDirectionalLightDesc;

class GLES2RendererDirectionalLight : public RendererDirectionalLight
{
	public:
		GLES2RendererDirectionalLight(const RendererDirectionalLightDesc &desc,	GLES2Renderer &renderer);
		virtual ~GLES2RendererDirectionalLight(void);
		
		virtual void bind(void) const;
	private:
		GLfloat m_lightColor[3];
		GLES2Renderer& m_gles2renderer;
};
}

#endif // #if defined(RENDERER_ENABLE_GLES2)
#endif
