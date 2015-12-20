#pragma once

#include "UI\UIElements.h"

#define MAX_STRING_SIZE  64

extern	CD3DFont*	MenuFont_Editor;

extern void pw_Draw2DTC(float X, float Y, float W, float H, const r3dColor& C, float CellX, float CellY, float SX, float SY, float SSX, float SSY, r3dTexture *Tex);

extern	int r3dMenuInit();
extern	int r3dMenuClose();


extern int input_getch();
extern int input_kbhit();
extern int input_Flush();

#define MENU_CODE(code)		code

#define CALL_MENU(xxx)		\
  {                             \
    xxx	*__menu = game_new xxx;      \
    m_ret = __menu->DoModal();  \
    delete __menu;              \
  }


#define START_MENU_ENUMS		\
  enum {                                \
    bFirstEnum = MENU_CODE(0x100),

int Menu_Init();
int Menu_Destroy();


class menuFloatLayer;

//
//
//
//
//MenuLayer
class MenuLayer
{
  public:
	int		CurFrame;
	int		Menu_ID;

	int		mx, my;
	int		mzMotion;		// wheel motion

	#define MAX_ZONES	200
	struct ZoneInfo
	{
	  r3dPolygon2D	pos;
	  int		id;
	  int 		bEnabled;
	};
	ZoneInfo	zones[MAX_ZONES];
	int		n_zones;
	int		curz, curz_id;

	int		pressed_id;
	int		released_id;

	float		mStartFrameTime;
	float		mLastFrameTime;


  public:
			MenuLayer();
virtual			~MenuLayer();
virtual	int		DoModal() = NULL;

	int		mReset();
	int		mUpdate();

	int		mAddZone(int zone_id, int x, int y, int w, int h);
	int		mFindZone(int zone_id);
	ZoneInfo&	MenuLayer::mGetZone(int zone_id);

	void		DisableZone(int ZoneID = -1);
	void		EnableZone(int ZoneID = -1);
	
	void		mDrawStart();
	void		mDrawCursor();
	void		mDrawEnd();
};

class MRect
{
  public:
	float		x, y, w, h;
  public:
	MRect() {}
	MRect(float _x, float _y, float _w, float _h) {
	  x = _x;
	  y = _y;
	  w = _w;
	  h = _h;
	}
	int		Contains(float mx, float my) 
	{ 
	  return(mx >= x && mx <= x+w && my >= y && my <= y + h);
	}
};

class MListBox
{
  public:
	struct data_s {
	  char		*str;
	  int		id;
	  int		enabled;
	};
	r3dgameVector(data_s) data;
	
	MenuLayer	*ml;
	MRect		zlb;
	MRect		zup;
	MRect		zdn;
	MRect		zsc;
	
	float		yposf;
	int		ypos, ystart, ylen;
  
  public:
	MListBox();
	~MListBox();
	void		Init(MenuLayer *_ml, int _zone_id);
	void		Reset();
	void		AddString(char *str, int id);
	
	void		Draw();
	int		GetCurID(int *enabled = NULL);
	void		SetCurID(int id);
	void		SetEnabled(int id, int enabled);
};

class MenuLayer2 : public MenuLayer
{
  friend class MenuLayersMgr;
  protected:
	enum {
	  zTitle = MENU_CODE(0xFF),
	};
	
	float		layerX, layerY;
	float		layerW, layerH;
	void		RelocateZones(float dx, float dy);
	bool		bDragging;
	void		ProcessDragging();	// will be called from MenuLayersMgr
	
	UIElements*	ui_elm;			// *shared* ui_element - do not delete

  public:
	bool		layerVisible;
	bool		layerDraggable;
	bool		layerExclusive;		// true if this layer is exclusive (Modal) - focus can't switch out from this layer

  protected:
virtual void		Init()       = NULL;
virtual void		Destroy()    = NULL;
virtual void		Activate()   = NULL;
virtual void		Deactivate() = NULL;
virtual	int		Update()     = NULL;
virtual void		Draw()       = NULL;

  public:
	MenuLayer2();
virtual	~MenuLayer2();

	bool		IsMouseOver(float x, float y);
	// element drawing primitives
	void		DrawButton(int zone, const char* btnname, bool selected);
	void		DrawTextButtons(int start_id, int num, const char* names[], int selected_id = -1);
};

class MenuLayersMgr
{
  private:
	typedef r3dgameList(MenuLayer2*) layerList_t;
	layerList_t	layers;
	MenuLayer2*	activeLayer;
	layerList_t::iterator FindLayer(MenuLayer2* layer, bool bAbortOnError = true);
	MenuLayer2*	mouseOver;
	
  public:
	bool		disableLayersUpdate;		// hack: if this is 'true', all layer updating is disabled
  
  public:
	MenuLayersMgr();
	~MenuLayersMgr();
	
	void		Insert(MenuLayer2* layer, UIElements* base_ui_elm);
	void		Remove(MenuLayer2* layer);
	
	void		BringToFront(MenuLayer2* layer);
	bool		CloseTopWindow();
	
	bool		Update(MenuLayer* baseMenu = NULL);
	void		Draw();
	
	bool		IsMouseOverWindow() { 
	  return mouseOver != NULL;
	}
};
extern	MenuLayersMgr	gMenuLayersMgr;



