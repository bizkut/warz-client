#pragma once

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

class Shader 
{
public:
	void Create(const char* vsFile, const char* psFile);
	void SetShaders();

protected:
	int vs, ps;
};


