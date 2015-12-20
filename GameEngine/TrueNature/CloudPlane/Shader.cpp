#include "r3dPCH.h"
#include "r3d.h"
#include "Shader.h"


void Shader::Create(const char* vsFile, const char* psFile)
{
	ps = r3dRenderer->GetPixelShaderIdx( psFile );
	vs = r3dRenderer->GetVertexShaderIdx( vsFile );

	r3d_assert( vs != -1 && ps != -1 ) ;

}

void Shader::SetShaders()
{
 	r3dRenderer->SetVertexShader(vs);
 	r3dRenderer->SetPixelShader(ps);
}


