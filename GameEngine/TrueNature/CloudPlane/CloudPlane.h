#pragma once

#include "Cloud.h"
#include "CloudGrid.h"
#include "SceneParameter.h"
#include "Shader.h"

class CloudPlane
{
public:
	CloudPlane() ;

	int initialized ;

	Cloud cloud;
	SSceneParamter sceneParam;
	SSceneParamter storedSceneParam;

	bool IsActive() const ;

	void Init();
	void Clear();
	void Render(const D3DXVECTOR3& cameraPos, const D3DXVECTOR3& lightDir, float dt, const D3DXMATRIX& viewProj, float farPlane, r3dTexture* cloudTex, r3dTexture* cloudCoverTex);
	void Save(const char* folder);
	void Load(const char* folder);
	void SetDefaults();
	bool NeedSave() const;
};