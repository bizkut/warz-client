
#include "GLES2RendererDirectionalLight.h"
#include "GLES2Renderer.h"
#include "RendererDirectionalLightDesc.h"

#if defined(RENDERER_ENABLE_GLES2)
namespace SampleRenderer
{

GLES2RendererDirectionalLight::GLES2RendererDirectionalLight(const RendererDirectionalLightDesc &desc, GLES2Renderer &renderer) :
	RendererDirectionalLight(desc), m_gles2renderer(renderer)
{
	m_lightColor[0] = desc.color.r / 255.0f;
	m_lightColor[1] = desc.color.g / 255.0f;
	m_lightColor[2] = desc.color.b / 255.0f;
}

GLES2RendererDirectionalLight::~GLES2RendererDirectionalLight(void)
{
}

void GLES2RendererDirectionalLight::bind(void) const
{
	m_gles2renderer.shaderContext().m_lightColor = PxVec3(m_lightColor[0], m_lightColor[1], m_lightColor[2]);
	m_gles2renderer.shaderContext().m_lightIntensity = m_intensity;
	m_gles2renderer.shaderContext().m_lightDirection = m_direction;
}

}

#endif // #if defined(RENDERER_ENABLE_GLES2)
