#pragma once

#include "AtlasComposer/RectPlacement.h"

struct AtlasDesc 
{

#pragma pack(push,1)
	struct Rect
	{
		float minX;
		float minY;
		float maxX;
		float maxY;
	};
#pragma pack(pop)


	AtlasDesc();
	AtlasDesc(int cnt);

	~AtlasDesc();

	void init(int count);
	void clear();
	void save(const char* fileName);
	void load(const char* fileName);

	const Rect& rect(int idx) const;

	int count;
	Rect* rects;	
};

struct TextureRectangle
{
	r3dSTLString fileName;
	LPDIRECT3DTEXTURE9 texture;
	CRectPlacement::TRect rect;			

	TextureRectangle();
	~TextureRectangle();
};

struct TextureAtlas
{
	typedef std::list<TextureRectangle> TextureList;
	TextureList rectangles;
	TextureRectangle result;
};

int ComposeAtlas( const char* basePath, const char* atlasName );
