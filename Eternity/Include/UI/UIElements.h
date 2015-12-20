#pragma once

//
// temporary place for temporary placeholders for drawing 'named' UI elements
//


class UIElm 
{
  public:
	r3dSTLString	name;

	r3dTexture*	pTex;
	float		sx, sy;
	float		sw, sh;
	
  public:
	UIElm() {}
	void		DrawWH(float x, float y, float w, float h, float px = 1.0f, float py = 1.0f, const r3dColor& col = r3dColor(255, 255, 255)) const;
	void		Draw(float x, float y, const r3dColor& col = r3dColor(255, 255, 255)) const {
	  DrawWH(x, y, sw, sh, 1.0f, 1.0f, col);
	}
};

class UIElements
{
  public:
	UIElm*		elmData;
	int		elmCurSize, elmMaxSize;
	
  public:	
	UIElements(int size);
	~UIElements();
	void		AddData1(r3dTexture* baseTex, const char** data, int size);
	void		AddData2(r3dTexture* baseTex, const char** data, int size);
	
	UIElm*		Find(const char* name);
	void		Draw(const char* name, float x, float y, const r3dColor& col = r3dColor(255, 255, 255));
};

extern	void		uiDrawFrame(UIElements* ui_elm, float in_x, float in_y, float in_w, float in_h, const char* base);
extern	void		uiDrawButton(UIElements* ui_elm, float x, float y, const char* btnname, bool hovering, bool selected);
