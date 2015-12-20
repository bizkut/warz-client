#include "r3dPCH.h"
#include "r3d.h"

#include "menu/m_gui.h"
#include "GameCommon.h"
#include "multiplayer\P2PMessages.h"
#include "ObjectsCode\AI\obj_AI_Base.h"
#include "ObjectsCode\World\Building.h"
#include "multihud.h"
#include "Game.h"
#include "NewLogic\SpellManager.h"

#pragma warning(disable:4244)

//
//
//
//
#include "multihud.h"

extern	void Bar_Graph(float x, float y, float wid, float height, float fill, const r3dColor& colorback, const r3dColor& colorfill, float Wd=0);

extern	gobjid_t	UI_TargetObjID;
extern	gobjid_t	UI_SelectedObjID;

extern	int		UI_bDisplayHUDText;

#include "NewLogic\SpellScripts.h"
//#include "NewLogic\SpellManager.h" 

#include "menu/m_ItemShop.h"
#include "menu/m_CharInfo.h"

#define USE_PARTICLE_EDITOR
#ifdef USE_PARTICLE_EDITOR
  #include "menu/m_Launcher.h"
  Menu_Launcher* menu_ToolsLauncher = NULL;
#endif 


#include "console.hpp"
pwConsole Console;

#include "version.h"

const static int SpellsUpLevels[4][6] = {
	{1,3,5,7,9,99},
	{1,3,5,7,9,99},
	{1,3,5,7,9,99},
	{6,11,16,99,99,99}
};
const static int SpellMaxLevels[4] = {
  5, 5, 5, 3,
};

CGUIMap		guiMap;

extern r3dPoint3D Level_Center;
extern r3dPoint3D Level_Size;
extern r3dPoint3D Level_Org;

static int FowAlwaysVisible = 1;

bool lolGetFOW (const r3dPoint3D &Position)
{
	if (FowAlwaysVisible) return 1;

	if ( guiMap.fowArray.Get(Position) < 0.8f ) return 0;

	return 1;
}

void RenderFakeFog(r3dCamera &Cam)
{
	extern 	void 		r3dDrawNormQuad(r3dCamera &Cam, const r3dPoint3D& Pos, const r3dPoint3D& Norm, float Size, const r3dColor24& Col);
	
	if (Keyboard->WasPressed(kbsJ)) FowAlwaysVisible = 1 - FowAlwaysVisible;
	if (FowAlwaysVisible) return;


	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
	r3dSetFiltering(1);
	r3dRenderer->SetTexture(guiMap.fowTex);

	r3dDrawNormQuad(Cam, Level_Center, r3dPoint3D(0,1,0), Level_Size.X, r3dColor(255,255,255));
	r3dRenderer->Flush();
    r3dRenderer->SetMaterial(NULL);
}


void centerPrintf(const MenuLayer::ZoneInfo& z, const r3dColor& clr, const char* fmt, ...)
{
  char buf[4096];
  va_list ap;
  va_start(ap, fmt);
  StringCbVPrintfA(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  
  SIZE sz;
  Font_Text->GetTextExtent(buf, &sz);
  
  Font_Text->PrintF(z.pos.X + z.pos.Width/2 - sz.cx/2, z.pos.Y + z.pos.Height/2 - sz.cy/2, clr, buf);
  return;
}



	float		_fow_Decay = 5.0f;		// default decay in seconds
	int		_fow_Cells = 64;		// dimension for fow array

CFieldOfView::CFieldOfView()
{
  fowData   = NULL;

  fowDecay  = _fow_Decay;
  worldSize = 10000;
}

CFieldOfView::~CFieldOfView()
{
}

void CFieldOfView::Init(int w, int h)
{
  assert(!fowData);
  
  cellsX_ = w;
  cellsY_ = h;
  fowData = new float[w * h + 1];
  
  for(int i=0; i<cellsX_ * cellsY_; i++) {
    fowData[i] = 0.0f;
  }
  
  return;
}

void CFieldOfView::Update(float timePassed)
{
 worldSize = Level_Size.X;

	const float minVisitedVisibility = 0.2f;
  for(int i=0; i<cellsX_ * cellsY_; i++) {
    if(fowData[i] < 0.0001f) continue;
    
    fowData[i] -= timePassed / fowDecay;
    if(fowData[i] < minVisitedVisibility) fowData[i] = minVisitedVisibility;
  }
}

void CFieldOfView::MarkCell(const r3dPoint3D& pos, float radius)
{
worldSize = Level_Size.X;

  r3dPoint3D pos1 = pos - Level_Org;

  int x1 = ((pos1.x - radius) / worldSize) * cellsX_;
  int x2 = ((pos1.x + radius) / worldSize) * cellsX_;
  int y1 = ((pos1.z - radius) / worldSize) * cellsY_;
  int y2 = ((pos1.z + radius) / worldSize) * cellsY_;
  x1 = R3D_CLAMP(x1, 0, cellsX_ - 1);
  x2 = R3D_CLAMP(x2, 0, cellsX_ - 1);
  y1 = R3D_CLAMP(y1, 0, cellsY_ - 1);
  y2 = R3D_CLAMP(y2, 0, cellsY_ - 1);
  
#if 0
  // simple square fill
  for(int y = y1; y <= y2; y++) {
    float* map = &fowData[y * cellsX_];
    for(int x = x1; x <= x2; x++) {
      map[x] = 1.0f;
    } 
  }
#else
  // circle fill
  int rsq = int(radius * radius);
  int cw  = int(worldSize) / cellsX_;
  int ch  = int(worldSize) / cellsY_;
  int yc  = (y2+y1)/2;		// center of y;
  int xc  = (x2+x1)/2;		// center of x;
  
  for(int y = y1; y <= y2; y++) {
    float* map = &fowData[y * cellsX_];
    int ysq = (y-yc)*(y-yc)*ch*ch;
    for(int x = x1; x <= x2; x++) {
      int xsq = (x-xc)*(x-xc)*cw*cw;
      if(xsq+ysq < rsq) {
        map[x] = 1.0f;
      }
    } 
  }
#endif  
  
  return;
}

float CFieldOfView::Get(const r3dPoint3D& pos)
{
  worldSize = Level_Size.X;

  r3dPoint3D pos1 = pos - Level_Org;

  int x1 = (pos1.x / worldSize) * cellsX_;
  int y1 = (pos1.z / worldSize) * cellsY_;
  if(x1 < 0 || x1 >= cellsX_) return 0;
  if(y1 < 0 || y1 >= cellsY_) return 0;

  return Get(x1, y1);
}

void CFieldOfView::Destroy()
{
  SAFE_DELETE_ARRAY(fowData);
}


CGUIMap::CGUIMap()
{
  fowTex = NULL;
}

CGUIMap::~CGUIMap()
{
}

void CGUIMap::Init(float x, float y, float w, float h, float ws, r3dTexture* tex)
{
  map_x     = x;
  map_y     = y;
  map_w     = w;
  map_h     = h;
  
  worldSize = ws;
  
  pingTex   = tex;
  pingStop  = 0;

  // note: texture & array must have same width & height
  
  fowTex    = r3dRenderer->AllocateTexture();
  fowTex->Create(_fow_Cells, _fow_Cells, D3DFMT_A8R8G8B8, 1);
  fowArray.Init(_fow_Cells, _fow_Cells);

  nextFowUpdate = r3dGetTime();
}

void CGUIMap::Destroy()
{
  if(fowTex) {
    r3dRenderer->DeleteTexture(fowTex, 1);
    fowTex = NULL;
  }
  
  fowArray.Destroy();
}

void CGUIMap::UpdateFow()
{
  assert(fowTex);
  
  // 1st: update field of view data - every tick
  {
    fowArray.Update(r3dGetFrameTime());
  
    for(const GameObject* obj = GameWorld.GetFirstObject(); obj; obj = GameWorld.GetNextObject(obj)) {
      if(!obj->Active || obj->bDead) continue;
      if(obj->TeamID != Player->TeamID) continue;
      
      float radius = 800;			// default   - 400
      if(objIsHeroAI(obj))       radius = 800;	// heroes    - 700
      else if(!objIsBaseAI(obj)) radius = 1100;	// buildings - 1000
    
      fowArray.MarkCell(obj->Position, radius);
    }
  }

  // 2nd: update texture  
  if(r3dGetTime() >= nextFowUpdate) {
    nextFowUpdate = r3dGetTime() + 0.5f;	// update 2 times per sec
    
    assert(fowTex->Height == fowArray.cellsX_);
    assert(fowTex->Width == fowArray.cellsY_);

    DWORD* clr32 = (DWORD*)fowTex->Lock(true);
    assert(clr32);
    int pitch = fowTex->Pitch / 4;
  
    // texture 0,0 is in upper-left part
    for(int y=fowTex->Height-1; y>=0; y--) {
      for(int x=0; x<fowTex->Width; x++) {
        float a1 = 255.0f - (fowArray.Get(x, y) * 255);
        int   a2 = R3D_CLAMP(int(a1), 0, 255);
        clr32[x] = 0x00000000 | (a2 << 24);
      }
      clr32 += pitch;
    }

    fowTex->Unlock();
  }
  
  return;
}

void CGUIMap::Ping(const r3dPoint3D& pos)
{
  pingStop = r3dGetTime() + 3.0f;
  pingLoc  = pos;
  
  return;
}

void CGUIMap::Update()
{
  UpdateFow();

  return;
}

void CGUIMap::Draw()
{
  r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
  r3dDrawBox2D(map_x, map_y, map_w, map_h, r3dColor(255, 255, 255, 255), Hud.MiniMap);

  for(const GameObject* obj = GameWorld.GetFirstObject(); obj; obj = GameWorld.GetNextObject(obj))
  {
    if(!obj->Active) continue;
    if(obj->bDead) continue;
    if(obj->TeamID == GameObject::TEAM_UNKNOWN) continue;
    
    float x, y;
    ToMap(obj->Position, &x, &y);
    
    r3dColor clr(255, 255, 255);
    
    if(objIsHeroAI(obj)) {
      switch(obj->TeamID) {
        case GameObject::TEAM_ORDER:   clr = r3dColor(127,127,255); break;
        case GameObject::TEAM_CHAOS:   clr = r3dColor(255,127,127); break;
      }

      r3dDrawBox2D(x-1, y,  4,  2, clr);
      r3dDrawBox2D(x,   y-1, 2, 4, clr);
      continue;
    }
    
    if(objIsBaseAI(obj)) {
      switch(obj->TeamID) {
        case GameObject::TEAM_ORDER:   clr = r3dColor(0,0,255); break;
        case GameObject::TEAM_CHAOS:   clr = r3dColor(255,0,0); break;
        case GameObject::TEAM_NEUTRAL: clr = r3dColor(0,255,255); break;
      }
      
      r3dDrawBox2D(x-1, y-1, 2, 2, clr);
      continue;
    }
    
    if(obj->Class->Name == "obj_Turret") {
      switch(obj->TeamID) {
        case GameObject::TEAM_ORDER:   clr = r3dColor(40,180,210); break;
        case GameObject::TEAM_CHAOS:   clr = r3dColor(220,40,200); break;
      }

      r3dDrawBox2D(x-1, y-1, 3, 3, clr);
      continue;
    }
      
    if(obj->Class->Name == "obj_Barracks") {
      switch(obj->TeamID) {
        case GameObject::TEAM_ORDER:   clr = r3dColor(40,180,210); break;
        case GameObject::TEAM_CHAOS:   clr = r3dColor(220,40,200); break;
      }

      r3dDrawBox2D(x-2, y-2, 4, 4, clr);
      continue;
    }

    if(obj->Class->Name == "obj_HQ") {
      switch(obj->TeamID) {
        case GameObject::TEAM_ORDER:   clr = r3dColor(40,180,210); break;
        case GameObject::TEAM_CHAOS:   clr = r3dColor(220,40,200); break;
      }

      r3dDrawBox2D(x-3, y-3, 6, 6, clr);
      continue;
    }
  }

  r3dSetFiltering(1);
  r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
  r3dDrawBox2D(map_x, map_y, map_w, map_h, r3dColor(255, 255, 255, 100), fowTex);
  r3dRenderer->SetTexture(NULL);
  r3dSetFiltering(0);

  // camera location
  {
    r3dColor clr = r3dColor(255, 255, 0);
    
    #if 1
      r3dPoint3D sv[4];
      r3dScreenTo3D(0, 0, &sv[0]);
      r3dScreenTo3D(r3dRenderer->ScreenW, 0, &sv[1]);
      r3dScreenTo3D(r3dRenderer->ScreenW, r3dRenderer->ScreenH, &sv[2]);
      r3dScreenTo3D(0, r3dRenderer->ScreenH, &sv[3]);
    
      D3DXPLANE groundplane(0, terra_GetH(gCam), 0, 0);

      r3dPoint3D mv[4];
      for(int i=0; i<4; i++) {
        r3dPoint3D v1 = gCam + sv[i] * 10000;
        r3dPoint3D iv;
        D3DXPlaneIntersectLine(iv.d3dx(), &groundplane, gCam.d3dx(), v1.d3dx());

        ToMap(iv, &mv[i].x, &mv[i].y);
        mv[i].x = R3D_CLAMP(mv[i].x, map_x, map_x + map_w);
        mv[i].y = R3D_CLAMP(mv[i].y, map_y, map_y + map_h);
      }
    
      extern void r3dDrawLine2D(float x1, float y1, float x2, float y2, float w, const r3dColor& clr);
      r3dDrawLine2D(mv[0].x, mv[0].y, mv[1].x, mv[1].y, 1, clr);
      r3dDrawLine2D(mv[1].x, mv[1].y, mv[2].x, mv[2].y, 1, clr);
      r3dDrawLine2D(mv[2].x, mv[2].y, mv[3].x, mv[3].y, 1, clr);
      r3dDrawLine2D(mv[3].x, mv[3].y, mv[0].x, mv[0].y, 1, clr);
      r3dRenderer->Flush();
    
    #else
      float x, y;
      ToMap(Hud.CameraWorldPosition, &x, &y);
      
      r3dDrawBox2D(x-8, y-6, 16, 12, clr);
    #endif
  }

  if(r3dGetTime() < pingStop) {
    float s = sinf(r3dGetTime() * 20);
    s = (s + 1.0f) * 0.5f;	// bring to [0..1]
    
    float x, y;
    ToMap(pingLoc, &x, &y);
    
    r3dColor clr(255, 255, 255);
    clr.A = BYTE(128 + 127 * s);

    float tw = pingTex->Width/2 * 4;
    float th = pingTex->Height/2 * 4;

    r3dDrawBox2D(x-tw/2, y-th, tw, th, clr, pingTex);
  }
}

static	float	_SafeBorder = 10;


//
// futile attempt to simulate UI elements
//
static const char* hudSheetCoords1[] = {
  "User Lifebar Frame;256x128;0,0;1x1",
  "Friendly Target Lifebar Frame;256x128;0,1;1x1",
  "Enemy Target Lifebar Frame;256x128;0,2;1x1",
  "Day Night Frame;256x128;0,3;1x1",
  "Level 1;32x32;4,0;1x1",
  "Level 2;32x32;4,1;1x1",
  "Level 3;32x32;4,2;1x1",
  "Level 4;32x32;4,3;1x1",
  "Level 5;32x32;4,4;1x1",
  "Level 6;32x32;4,5;1x1",
  "Level 7;32x32;4,6;1x1",
  "Level 8;32x32;4,7;1x1",
  "Level 9;32x32;4,8;1x1",
  "Level 10;32x32;4,9;1x1",
  "Level 11;32x32;4,10;1x1",
  "Level 12;32x32;4,11;1x1",
  "Level 13;32x32;4,12;1x1",
  "Level 14;32x32;4,13;1x1",
  "Level 15;32x32;4,14;1x1",
  "Level 16;32x32;4,15;1x1",
  "Level 17;32x32;4,16;1x1",
  "Level 18;32x32;4,17;1x1",
  "Audio Option Speaker Big;32x32;4,18;1x1",
  "Check mark;32x32;4,19;1x1",
  "X Close button;32x32;4,20;1x1",
  "Up Arrow Slider;32x32;4,21;1x1",
  "Down Arrow Slider;32x32;4,22;1x1",
  "Slider bar;32x32;4,23;1x1",
  "Display Minion Button;16x16;10,0;1x1",
  "Ping Map Button;16x16;10,1;1x1",
  "Plus Symbol;16x16;10,2;1x1",
  "Equals Symbol;16x16;10,3;1x1",
  "Left Arrow;16x16;10,4;1x1",
  "Right Arrow;16x16;10,5;1x1",
  "Slider;16x16;10,6;1x1",
  "Left slider bg;16x16;10,7;1x1",
  "Middle Slider Bg;16x16;10,8;1x1",
  "Right  Slider Bg;16x16;10,9;1x1",
  "Checkbox;16x16;10,10;1x1",
  "Small Speaker;16x16;10,11;1x1",
  "Abilites Frame Grey;64x64;3,0;1x1",
  "Abilites Frame Blue;64x64;3,1;1x1",
  "Abilites Frame Red;64x64;3,2;1x1",
  "Abilites Frame Green;64x64;3,3;1x1",
  "Ally Hero Lifebar Frame;128x64;3,3;1x1",
  "Character Menu Button;64x64;4,0;1x1",
  "Immortal Menu Button;64x64;4,1;1x1",
  "Options Button;64x64;4,2;1x1",
  "Statistics Button;64x64;4,3;1x1",
  "Upgrade Ability Button;64x64;4,4;1x1",
  "Cancel Upgrade Button;64x64;4,5;1x1",
  "HL_Character Menu Button;64x64;5,0;1x1",
  "HL_Immortal Menu Button;64x64;5,1;1x1",
  "HL_Options Button;64x64;5,2;1x1",
  "HL_Statistics Button;64x64;5,3;1x1",
  "HL_Upgrade Ability Button;64x64;5,4;1x1",
  "HL_Cancel Upgrade Button;64x64;5,5;1x1",
  "Highlighted Store Item;256x64;4,2;1x1",
  "Immortal Bar;64x64;5,6;7x2",
  "Abilities Button;64x32;12,0;1x1",
  "Stats Button;64x32;12,1;1x1",
  "Lore Button;64x32;12,2;1x1",
  "Tips Button;64x32;12,3;1x1",
  "Close Button;64x32;12,4;1x1",
  "HL_Abilities Button;64x32;13,0;1x1",
  "HL_Stats Button;64x32;13,1;1x1",
  "HL_Lore Button;64x32;13,2;1x1",
  "HL_Tips Button;64x32;13,3;1x1",
  "HL_Close Button;64x32;13,4;1x1",
  "Defense Button;128x32;14,0;1x1",
  "Offense Button;128x32;14,1;1x1",
  "Basic Button;128x32;14,2;1x1",
  "Consumables Button;128x32;14,3;1x1",
  "Regenerative Button;128x32;14,4;1x1",
  "Recommended Button;128x32;14,5;1x1",
  "Legendary Button;128x32;14,6;1x1",
  "Default Button;128x32;14,7;1x1",
  "HL_Defense Button;128x32;15,0;1x1",
  "HL_Offense Button;128x32;15,1;1x1",
  "HL_Basic Button;128x32;15,2;1x1",
  "HL_Consumables Button;128x32;15,3;1x1",
  "HL_Regenerative Button;128x32;15,4;1x1",
  "HL_Recommended Button;128x32;15,5;1x1",
  "HL_Legendary Button;128x32;15,6;1x1",
  "HL_Default Button;128x32;15,7;1x1",
  "Interface Options Button;128x32;16,0;1x1",
  "Audio Options Button;128x32;16,1;1x1",
  "Video Options Button;128x32;16,2;1x1",
  "Resume Game Button;128x32;16,3;1x1",
  "Controls Button;128x32;16,4;1x1",
  "Help Button;128x32;16,5;1x1",
  "Back Button;128x32;16,6;1x1",
  "End Game Button;128x32;16,7;1x1",
  "Return Main Menu Button;256x32;17,0;1x1",
  "Mouse Control Button;256x32;17,1;1x1",
  "Key Bindings Button;256x32;17,2;1x1",
  "HL_Return Main Menu Button;256x32;18,0;1x1",
  "HL_Mouse Control Button;256x32;18,1;1x1",
  "HL_Key Bindings Button;256x32;18,2;1x1",
  "Mini Map Frame;256x256;3,3;1x1",
  "Game Menu Buttons Frame;64x64;9,12;3x3",
  "Store Main Frame UL;32x32;20,0;1x1",
  "Store Main Frame Top;32x32;20,1;1x1",
  "Store Main Frame UR;32x32;20,2;1x1",
  "Store Main Frame Left;32x32;21,0;1x1",
  "Store Main Frame Center;32x32;21,1;1x1",
  "Store Main Frame Right;32x32;21,2;1x1",
  "Store Main Frame BL;32x32;22,0;1x1",
  "Store Main Frame Bottom;32x32;22,1;1x1",
  "Store Main Frame BR;32x32;22,2;1x1",
  "Store Main Frame Middle UL;32x32;20,3;1x1",
  "Store Main Frame Middle Top;32x32;20,4;1x1",
  "Store Main Frame Middle UR;32x32;20,5;1x1",
  "Store Main Frame Middle Left;32x32;21,3;1x1",
  "Store Main Frame Middle Center;32x32;21,4;1x1",
  "Store Main Frame Middle Right;32x32;21,5;1x1",
  "Store Main Frame Middle BL;32x32;22,3;1x1",
  "Store Main Frame Middle Bottom;32x32;22,4;1x1",
  "Store Main Frame Middle BR;32x32;22,5;1x1",
  "Store Item Frame UL;16x16;40,18;1x1",
  "Store Item Frame Top;16x16;40,19;1x1",
  "Store Item Frame UR;16x16;40,20;1x1",
  "Store Item Frame Left;16x16;41,18;1x1",
  "Store Item Frame Center;16x16;41,19;1x1",
  "Store Item Frame Right;16x16;41,20;1x1",
  "Store Item Frame BL;16x16;42,18;1x1",
  "Store Item Frame Bottom;16x16;42,19;1x1",
  "Store Item Frame BR;16x16;42,20;1x1",
  "Large Tool Tip UL;32x32;20,6;1x1",
  "Large Tool Tip Top;32x32;20,7;1x1",
  "Large Tool Tip UR;32x32;20,8;1x1",
  "Large Tool Tip Left;32x32;21,6;1x1",
  "Large Tool Tip Center;32x32;21,7;1x1",
  "Large Tool Tip Right;32x32;21,8;1x1",
  "Large Tool Tip BL;32x32;22,6;1x1",
  "Large Tool Tip Bottom;32x32;22,7;1x1",
  "Large Tool Tip BR;32x32;22,8;1x1",
  "Small Tool Tip UL;16x16;40,21;1x1",
  "Small Tool Tip Top;16x16;40,22;1x1",
  "Small Tool Tip UR;16x16;40,23;1x1",
  "Small Tool Tip Left;16x16;41,21;1x1",
  "Small Tool Tip Center;16x16;41,22;1x1",
  "Small Tool Tip Right;16x16;41,23;1x1",
  "Small Tool Tip BL;16x16;42,21;1x1",
  "Small Tool Tip Bottom;16x16;42,22;1x1",
  "Small Tool Tip BR;16x16;42,23;1x1",
  "Character Menu UL;64x64;12,0;1x1",
  "Character Menu Top;64x64;12,1;1x1",
  "Character Menu UR;64x64;12,2;1x1",
  "Character Menu Left;64x64;13,0;1x1",
  "Character Menu Center;64x64;13,1;1x1",
  "Character Menu Right;64x64;13,2;1x1",
  "Character Menu BL;64x64;14,0;1x1",
  "Character Menu Bottom;64x64;14,1;1x1",
  "Character Menu BR;64x64;14,2;1x1",
  "Pause Menu Frame UL;64x64;12,3;1x1",
  "Pause Menu Frame Top;64x64;12,4;1x1",
  "Pause Menu Frame UR;64x64;12,5;1x1",
  "Pause Menu Frame Left;64x64;13,3;1x1",
  "Pause Menu Frame Center;64x64;13,4;1x1",
  "Pause Menu Frame Right;64x64;13,5;1x1",
  "Pause Menu Frame BL;64x64;14,3;1x1",
  "Pause Menu Frame Bottom;64x64;14,4;1x1",
  "Pause Menu Frame BR;64x64;14,5;1x1",
};

static const char* hudSheetCoords2[] = {
	"<elm name=mainpane id=0 x=0 y=834 w=1024 h=191 />",
        "<elm name=mpaneL id=0 x=518 y=328 w=122 h=184 />",
        "<elm name=mpaneR id=0 x=640 y=328 w=122 h=184 />",
        "<elm name=map id=0 x=260 y=260 w=209 h=221 />",
        "<elm name=btnping_1 id=0 x=0 y=64 w=27 h=27 />",
        "<elm name=HL_btnping_1 id=0 x=0 y=96 w=27 h=27 />",
        "<elm name=btnsumm_1 id=0 x=32 y=64 w=27 h=27 />",
        "<elm name=HL_btnsumm_1 id=0 x=32 y=96 w=27 h=27 />",
        "<elm name=btnsupg_1 id=0 x=64 y=64 w=19 h=19 />",
        "<elm name=HL_btnsupg_1 id=0 x=64 y=96 w=19 h=19 />",
        "<elm name=invpane id=0 x=1 y=385 w=147 h=43 />",
        "<elm name=petpane id=0 x=0 y=326 w=191 h=56 />",
        "<elm name=bbar id=0 x=1 y=257 w=217 h=18 />",
        "<elm name=rbar id=0 x=1 y=289 w=223 h=18 />",
        "<elm name=hpbar id=0 x=1 y=128 w=92 h=10 />",
        "<elm name=mpbar id=0 x=1 y=161 w=92 h=8 />",
        "<elm name=sumbar1 id=0 x=1 y=225 w=226 h=7 />",
        "<elm name=sumbar2 id=0 x=0 y=193 w=243 h=7 />",
};


void Menu_GUI::ActivateShop()
{
  gMenuLayersMgr.BringToFront(menuShop);
}

void Menu_GUI::ActivateCharacterInfo(int TabID)
{
  gMenuLayersMgr.BringToFront(menuCharInfo);
  menuCharInfo->CurCharMode = TabID;
}

Menu_GUI::Menu_GUI()
{

}

void Menu_GUI::Init()
{
	imgHUD1       = r3dRenderer->LoadTexture("Data\\Images\\UI\\HUD_Main1.dds");
	imgHUD2       = r3dRenderer->LoadTexture("Data\\Images\\UI\\NewHUD.dds");

	imgDayNight   = r3dRenderer->LoadTexture("Data\\Images\\UI\\DayNightSpinner.tga");
	imgPingMarker = r3dRenderer->LoadTexture("Data\\Images\\UI\\PingMarker.dds");
	
	ui_elm = new UIElements(R3D_ARRAYSIZE(hudSheetCoords1) + R3D_ARRAYSIZE(hudSheetCoords2));
	ui_elm->AddData1(imgHUD1, hudSheetCoords1, R3D_ARRAYSIZE(hudSheetCoords1));
	ui_elm->AddData2(imgHUD2, hudSheetCoords2, R3D_ARRAYSIZE(hudSheetCoords2));
	
	mReset();
	
	mAddZone(bPingMiniMap, 5+89, 5+185, 27, 27);

	mAddZone(bMiniMap, 20,20, 200,200);
	guiMap.Init(20, 20, 200, 200, 1000.0f, imgPingMarker);
	
	int reloc_zone_start = this->n_zones;

	const static int splx[4] = {258, 307, 356, 405};
	for(int i=0; i<4; i++) {
	  mAddZone(bSpellCast1+i, splx[i], 683, 45, 45);
	  mAddZone(bSpellUpgrade1+i, splx[i]-3, 683+47, 19, 19);
	}
	
	const static int itemsx[6] = {498, 542, 586, 631, 676, 721};
	for(int i=0; i<6; i++) {
	  mAddZone(bInvSlot1+i, itemsx[i], 684, 41, 43);
	}
	
	mAddZone(bSummonCast, 491, 735, 27, 27);
	mAddZone(bSummonBar1, 247, 756, 226, 7);
	mAddZone(bSummonBar2, 531, 756, 243, 7);

	mAddZone(bChar1Pict, 10, 641, 87, 84);
	mAddZone(bChar1HPBar, 6, 738, 92, 10);
	mAddZone(bChar1MPBar, 6, 752, 92, 8);
	mAddZone(bChar1EXPBar, 0, 611, 217, 18);
	mAddZone(bChar1Stats, 115, 699, 95, 58);
	mAddZone(bChar1Buffs, 115, 645, 95, 37);

	mAddZone(bChar2Pict, 927, 641, 87, 84);
	mAddZone(bChar2HPBar, 924, 738, 92, 10);
	mAddZone(bChar2MPBar, 924, 752, 92, 8);
	mAddZone(bChar2EXPBar, 801, 611, 217, 18);
	mAddZone(bChar2Stats, 813, 699, 95, 58);
	mAddZone(bChar2Buffs, 813, 645, 95, 37);
	
	for(int i=0; i<6; i++) {
	  // note, coordinates should be the same as in target inventory draw
	  mAddZone(bTargetInvSlot1 + i, 876+10+(i*22), 547+17, 20, 20);
	}
	mAddZone(bTargetInvPane, 876, 547, 147, 43);

	mAddZone(bPetPane, 580, 591, 191, 56);

	mAddZone(bGoldArea, 462, 660, 78, 14);

	// add temporary zone which cover bottom HUD. it should be last (after all buttons)
	mAddZone(bMainHUDArea, 0,  r3dRenderer->ScreenH-180, r3dRenderer->ScreenW, 180);
	
	// relocate added zones, based on 1024x768 appearance (recenter on bottom center)
	int reloc_zone_end = this->n_zones;
	float offx = (r3dRenderer->ScreenW - 1024) / 2;
	float offy = r3dRenderer->ScreenH - 768;
	for(int i=reloc_zone_start; i<reloc_zone_end; i++) {
	  zones[i].pos.X += offx;
	  zones[i].pos.Y += offy;
	}
	
	mAddZone(bStats_Close, _SafeBorder, _SafeBorder, 64, 64);

	mAddZone(bOptions_Options, r3dRenderer->ScreenW/2-52, r3dRenderer->ScreenH/2-40*2, 104, 32);
	mAddZone(bOptions_Resume,  r3dRenderer->ScreenW/2-52, r3dRenderer->ScreenH/2-40*1, 104, 32);
	mAddZone(bOptions_Quit,    r3dRenderer->ScreenW/2-52, r3dRenderer->ScreenH/2+40*2, 104, 32);

	/*  Zone for localized tooltips KB 10.7.07  */
	//mAddZone(ltt_TimeOfDay, r3dRenderer->ScreenW/2, 0, 128, 128);

	CurMode = GUIMODE_MAIN;
	
  // create floating menus and add them to layer manager
	menuShop     = new Menu_ItemShop2;
	gMenuLayersMgr.Insert(menuShop, ui_elm);

	menuCharInfo = new Menu_CharInfo;
	gMenuLayersMgr.Insert(menuCharInfo, ui_elm);

#ifdef USE_PARTICLE_EDITOR
	menu_ToolsLauncher = new Menu_Launcher;
	gMenuLayersMgr.Insert(menu_ToolsLauncher, ui_elm);
#endif	
	
	Console.Init();
	Console.bActive = FALSE;

	mPinging = false;
	mMapMoveBlocked = false;
	invDragSlot = -1;
	
	invPane2Visible = 0.0f;
	petPaneVisible = 0.0f;
}


void Menu_GUI::Destroy()
{
	r3dRenderer->DeleteTexture(imgHUD1);
	r3dRenderer->DeleteTexture(imgHUD2);
	r3dRenderer->DeleteTexture(imgDayNight);
	r3dRenderer->DeleteTexture(imgPingMarker);

	// unregister floating menus
	gMenuLayersMgr.Remove(menuShop);	SAFE_DELETE(menuShop);
	gMenuLayersMgr.Remove(menuCharInfo);	SAFE_DELETE(menuCharInfo);

#ifdef USE_PARTICLE_EDITOR
	gMenuLayersMgr.Remove(menu_ToolsLauncher);		SAFE_DELETE(menu_ToolsLauncher);
#endif

	guiMap.Destroy();

	SAFE_DELETE(ui_elm);
}


void Menu_GUI::Activate()
{
	CurMode = GUIMODE_MAIN;
}


void Menu_GUI::Deactivate()
{
}

int Menu_GUI::Process()
{
  mUpdate();

  // we should do that in Init() but on the time, levelsizer object isn't inited
  extern float gTerrainSizeX;
  //guiMap.SetWorldSize(Terrain ? Terrain->WorldSize : gTerrainSizeX);
  guiMap.SetWorldSize(gTerrainSizeX);
  guiMap.Update();

  gMenuLayersMgr.Update(this);
  gMenuLayersMgr.disableLayersUpdate = false;	// hack: reenable layers update. currently it might be disabled in PAUSE menu processing
        
  if(CurMode == GUIMODE_STATS && Keyboard->WasReleased(kbsTab)) {
    CurMode = GUIMODE_MAIN;
  }

  // target inventory pane: update
  {
    GameObject* target = GameWorld.GetObject(UI_SelectedObjID);
    if(target && objIsHeroAI(target)) 
      invPane2Visible += r3dGetFrameTime() * 4.0f; 
    else                              
      invPane2Visible -= r3dGetFrameTime() * 4.0f; 
    
    invPane2Visible = R3D_CLAMP(invPane2Visible, 0.0f, 1.0f);
  }
  
  // pet pane: update
  {
    GameObject* pet = GameWorld.GetObject(Player->AI_PetIDs[0]);
    if(pet && !pet->bDead) 
      petPaneVisible += r3dGetFrameTime() * 4.0f; 
    else                              
      petPaneVisible -= r3dGetFrameTime() * 4.0f; 
    
    petPaneVisible = R3D_CLAMP(petPaneVisible, 0.0f, 1.0f);
  }

	DisableZone(bOptions_Resume);
	DisableZone(bOptions_Options);
	DisableZone(bOptions_Quit);
  

	switch(CurMode)
	{
		case GUIMODE_MAIN:
		{
			// show upgrades if we have points availbiles
			for(int i=0; i<4; i++) 
			{
				SpellDataInst& sdi = Player->Spells[i];
				if(sdi.SData && Player->mLevel >= SpellsUpLevels[i][sdi.Level] && Player->GetAPoints() && sdi.Level < SpellMaxLevels[i])
					EnableZone(bSpellUpgrade1+i);
				else
					DisableZone(bSpellUpgrade1+i);
			}
	
			ProcessMain();

#ifdef USE_PARTICLE_EDITOR
			if(Keyboard->WasPressed(kbsTab))
			{
			  gMenuLayersMgr.BringToFront(menu_ToolsLauncher);
			  break;
			}
#endif			

			if(Keyboard->WasPressed(kbsEsc)) 
			{
				invDragSlot = -1;

				if(gMenuLayersMgr.CloseTopWindow()) 
					break;

				if(Player->AI_SelectedSpell != -1) {
					Player->AI_SelectedSpell = -1;
					break;
				}
			
				if(mPinging) {
					mPinging = false;
					mMapMoveBlocked = false;
					break;
				}

				CurMode = GUIMODE_PAUSE;
			}

			break;
		}

		case GUIMODE_STATS:
			EnableZone(bStats_Close);
			ProcessGameStats();
			break;

		case GUIMODE_PAUSE:
			gMenuLayersMgr.disableLayersUpdate = true;

			EnableZone(bOptions_Resume);
			EnableZone(bOptions_Options);
			EnableZone(bOptions_Quit);
			ProcessPause();
			break;

		case GUIMODE_CONSOLE:
			ProcessConsole();
			break;
	}
	
	return 0;
}

void Menu_GUI::ProcessMain()
{
	//
	// First process minimap click directly
	//
	if(mMapMoveBlocked && !lbp) mMapMoveBlocked = false;
	
	if(rbr && (curz_id == bMiniMap))
	{
		r3dPoint3D pos;
		guiMap.FromMap(mx, my, &pos);
		if(Keyboard->IsPressed(kbsLeftShift) || Keyboard->IsPressed(kbsRightShift))
		   Player->IssueOrder(obj_AI_Base::AI_ATTACKMOVE, pos);			
			//ADD ATTACK MOVE FOR PETS
		else if((Keyboard->IsPressed(kbsLeftAlt) || Keyboard->IsPressed(kbsRightAlt)) && Player->AI_PetIDs[0] != invalidGameObjectID) {
		    GameObject* pet_obj = GameWorld.GetObject(Player->AI_PetIDs[0]);
			if (pet_obj)
				if (!pet_obj->bDead) {
					obj_AI_Base* pet = objToBaseAI(pet_obj);
					pet->IssueOrder(obj_AI_Base::AI_PETMOVE, pos);
				}
		}
		else {
			Player->IssueOrder(obj_AI_Base::AI_MOVETO, pos);

		//if you have a pet, potentially issue it an order as well
			if(Player->AI_PetIDs[0] != invalidGameObjectID) {
				GameObject* petObj = GameWorld.GetObjectA(Player->AI_PetIDs[0]);
				if (petObj)
					if(!petObj->bDead && DistanceBetweenObjectAndPoint(Player, pos) >= 1000.0f)
						(objToBaseAI(petObj))->IssueOrder(obj_AI_Base::AI_HOLD, pos);
			}
		}
	}
	
	// left click mini map
	if(lbp && (curz_id == bMiniMap))
	{
		if(!mPinging && !mMapMoveBlocked) {
			r3dPoint3D pos;
			guiMap.FromMap(mx, my, &pos);
			
			Hud.CameraWorldPosition.X = pos.x;
			Hud.CameraWorldPosition.Z = pos.z;
			Hud.RestrictCameraMovement();		
		}
		else {
			r3dPoint3D pos;
			guiMap.FromMap(mx, my, &pos);
			
			PKT_MapPing_s n;
			n.EventID	= PKT_MapPing;
			n.FromID	= 0;
			n.pos		= pos;

			p2pSendBroadcast(sizeof(n), &n, true);
			mMapMoveBlocked = true;
		}
	}

	// quick use inventory slots
	if(Keyboard->WasPressed(kbs1)) released_id = bInvSlot1;
	if(Keyboard->WasPressed(kbs2)) released_id = bInvSlot2;
	if(Keyboard->WasPressed(kbs3)) released_id = bInvSlot3;
	if(Keyboard->WasPressed(kbs4)) released_id = bInvSlot4;
	if(Keyboard->WasPressed(kbs5)) released_id = bInvSlot5;
	if(Keyboard->WasPressed(kbs6)) released_id = bInvSlot6;

    if(!(Keyboard->IsPressed(kbsLeftAlt)) && !(Keyboard->IsPressed(kbsRightAlt)))
    {
        if(Keyboard->IsPressed(kbsQ)) released_id = bSpellCast1;
	    if(Keyboard->IsPressed(kbsW)) released_id = bSpellCast2;
	    if(Keyboard->IsPressed(kbsE)) released_id = bSpellCast3;
	    if(Keyboard->IsPressed(kbsR)) released_id = bSpellCast4;
    }

	//Menu type screens
	if(Keyboard->WasPressed(kbsZ)) released_id = bUI_Options;
	
	if(Keyboard->WasPressed(kbsV)) { 
		if(menuCharInfo->layerVisible) {
			gMenuLayersMgr.CloseTopWindow();
		}
		else {
			ActivateCharacterInfo(1);
		}
	}


	if(Keyboard->WasPressed(kbsC)) { released_id = bChar1Pict; lbr_dbl = 1; }
	if(Keyboard->WasPressed(kbsX)) { released_id = bUI_Immortal; }
	
	if((Keyboard->IsPressed(kbsLeftShift) || Keyboard->IsPressed(kbsRightShift)) && Keyboard->WasPressed(kbsG)) {
		//PingMiniMap(Player->Position);
	}

	//Ping minimap.  Need to add ping functionality
	if(Keyboard->WasPressed(kbsG)) { 
		mPinging = true;
	}

	//Minimap info toggle.  Need to add flag, modify minimap display code
	if(Keyboard->WasPressed(kbsT)) { }

	//Toggle hud message bubbles
	if(Keyboard->WasPressed(kbsH)) { UI_bDisplayHUDText = 1 - UI_bDisplayHUDText; }

	//For chat
//	#ifndef USE_PARTICLE_EDITOR
	if(Keyboard->WasPressed(kbsEnter) && CurMode != GUIMODE_CONSOLE) {
		CurMode = GUIMODE_CONSOLE;
		Console.Show();
	}
//	#endif

	//Scoreboard
	if(Keyboard->IsPressed(kbsTab)) {
		released_id = bUI_GameStatus;
	}
	
	// item-dragging: start on right click
	if(invDragSlot == -1 && rbr && curz_id >= bInvSlot1 && curz_id <= bInvSlot6) {
	  int slot = curz_id - bInvSlot1;
	  
	  if(Player->Inventory[slot].IData && Player->Inventory[slot].IData->Icon) {
	    invDragSlot = slot;

	    Player->AI_SelectedSpell = -1;
	    return;
	  }
	}
	
	// item-dragging: update
	if(invDragSlot != -1) {
	  if(rbr) {
	    invDragSlot = -1;
	    return;
	  }
	  
	  if(Player->Inventory[invDragSlot].IData == NULL) {
	    invDragSlot = -1;
	    return;
	  }
	  
	  // in drag/drop mode we do not process anything else
	  if(released_id >= bInvSlot1 && released_id <= bInvSlot6) {
            int slot = released_id - bInvSlot1;
	    assert(slot >= 0 && slot < NUM_INVENTORY_SLOTS);

	    Player->SwapItem(invDragSlot, slot);
	    invDragSlot = -1;
	  }
	  return;
	}
	
	if(rbr && curz_id >= bSpellCast1 && curz_id <= bSpellCast4) {
	  int slot = curz_id - bSpellCast1;
	  SpellDataInst& sdi = Player->Spells[slot];
	  
	  if(sdi.SData && (sdi.SData->Flags & SpellData::SPELL_Autocast)) {
	    if(Player->AI_AutocastSpell == slot) 
	      Player->SetAutocast(-1);
	    else 
	      Player->SetAutocast(slot);
	  }
	}
	
	if(released_id != -1) switch(released_id) 
	{
		case bPingMiniMap:
			mPinging = !mPinging;
			break;
			
		case bInvSlot1:
		case bInvSlot2:
		case bInvSlot3:
		case bInvSlot4:
		case bInvSlot5:
		case bInvSlot6:
		{
			int slot = released_id - bInvSlot1;
			assert(slot >= 0 && slot < NUM_INVENTORY_SLOTS);
			if(Player->CanUseItem(slot) == obj_AI_Base::USE_Ok) {
				Player->UseItem(slot);
			}
			break;
		}
		case bSpellUpgrade1:
		case bSpellUpgrade2:
		case bSpellUpgrade3:
		case bSpellUpgrade4:
		{
			const ZoneInfo& z = mGetZone(released_id);
			int slot = released_id - bSpellUpgrade1;
			Player->UpgradeSpell(slot);
			break;
		}

		case bSpellCast1:
		case bSpellCast2:
		case bSpellCast3:
		case bSpellCast4:
		{
			int slot = released_id - bSpellCast1;
			switch(Player->CanUseSpell(slot)) {
				case obj_AI_Base::USE_NoMana:
				{
					const int no_mana_sound_id = 126;
					static char msg[128] = { "Not enough mana." }; 
					snd_PlaySound_2D(no_mana_sound_id, SOUND_GROUP_A, 128);
					MCommand.SetMsg(1, 5.0f, msg);
					break;
				}
				case obj_AI_Base::USE_Ok:
				{
					Player->StartCastSpell(slot);
					break;
				}
			}
			break;
		}

		case bChar1Pict:
			if(lbr_dbl) {
				//gMenuLayersMgr.BringToFront(menuCharInfo);
				if(menuCharInfo->layerVisible) {
					gMenuLayersMgr.CloseTopWindow();
				}
				else {
					ActivateCharacterInfo(0);
				}
			} else {
				Hud.CameraWorldPosition.X = Player->Position.x;
				Hud.CameraWorldPosition.Z = Player->Position.z;
				Hud.RestrictCameraMovement();		
			}
			break;

		case bUI_GameStatus:
			CurMode = GUIMODE_STATS;
			break;

		case bUI_Options:
			if(CurMode == GUIMODE_PAUSE) {
				CurMode = GUIMODE_MAIN;
			}
			else {
				CurMode = GUIMODE_PAUSE;
			}
			break;


		case bMiniMap:
			{
			}
			break;

		}
}


void Menu_GUI::ProcessGameStats()
{
		if(released_id != -1)
		switch (released_id )
		{ 
			case bStats_Close:
				CurMode = GUIMODE_MAIN;
			break;
			case bUI_GameStatus:
				CurMode = GUIMODE_MAIN;
			break;
		}

}


void Menu_GUI::ProcessPause()
{
if(Keyboard->WasPressed(kbsEsc)) CurMode = GUIMODE_MAIN;

		if(released_id != -1)
		switch (released_id )
		{ 
			case bOptions_Resume:
				CurMode = GUIMODE_MAIN;
			break;
			case bOptions_Quit:
				CurMode = GUIMODE_MAIN;
				
				CurrentGameMode = GAMESTATE_ENDGAME;
				GameExitCode = EXITCODE_ABANDON;
			break;

		}

}


void Menu_GUI::ProcessConsole()
{
	if(Keyboard->WasPressed(kbsEsc) && CurMode == GUIMODE_CONSOLE) { 
		Console.bActive = false;
		Keyboard->ClearPressed();
		CurMode = GUIMODE_MAIN;
		return;
	}

//	#ifndef USE_PARTICLE_EDITOR
	if(Keyboard->WasPressed(kbsEnter) && CurMode == GUIMODE_CONSOLE) {
		Console.ProcessCommand();
		Keyboard->ClearPressed();
		Console.Draw();
		Console.bActive = false;
		CurMode = GUIMODE_MAIN;
	}
//	#endif

}


void DrawUIButton(float X, float Y, float W, float H, int bHover, int bPressed, r3dTexture *Tex, float CellX, float CellY, float SX, float SY, float SSX, float SSY)
{
 r3dColor24 Col = r3dColor24(200,200,200);

 if (bHover) 
 {
   Col = r3dColor24(255,255,255);
   SY += 1;
 }

 int PressedState = (bPressed & 0x1) ? 1 : 0;

 pw_Draw2DTC(X, Y, W, H, Col, CellX, CellY, SX, SY, SSX, SSY, Tex);
}




void _Draw3DTextBox(r3dPoint3D &v1, int SX, int SY, r3dColor24 &clr, char *Str, ...)
{
	D3DVIEWPORT9 vp;
	r3dRenderer->pd3ddev->GetViewport(&vp);
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);

	D3DXVECTOR3 p1(v1.x, v1.y, v1.z);
	D3DXVECTOR3 p2;
	D3DXVec3Project(&p2, &p1, &vp, &r3dRenderer->mProj, &r3dRenderer->mCamera, &mWorld);

	if(p2.z>0&&p2.z<1 && p2.x>0&&p2.x<r3dRenderer->ScreenW && p2.y>0&&p2.y<r3dRenderer->ScreenH)
	{
		char buf[2048];

		va_list ap;
		va_start(ap, Str);
		vsprintf(buf, Str, ap);
		va_end(ap);

		SIZE pt;
		Font_Label->GetTextExtent(buf, &pt );

		r3dDrawBox2D(p2.x - SX/2, p2.y - SY/2, SX, SY, r3dColor(0,0,0,200));
		Font_Text->DrawScaledText(p2.x, p2.y, 1, 1, 1.0f, clr, buf, D3DFONT_CENTERED);
	}
}





void Menu_GUI::PingMiniMap(const r3dPoint3D& PingLoc)
{
  guiMap.Ping(PingLoc);
  
  mPinging = false;
}

bool temp_buffIsPositive(int type)
{
  if(type == BUFF_CombatDehancer) return false;
  if(type == BUFF_Stun) return false;
  if(type == BUFF_Taunt) return false;
  if(type == BUFF_Polymorph) return false;
  if(type == BUFF_Damage) return false;
  if(type == BUFF_Fear) return false;
  if(type == BUFF_Slow) return false;
  if(type == BUFF_Snare) return false;
  if(type == BUFF_Sleep) return false;
  if(type == BUFF_Silence) return false;
  
  return true;
}

void Menu_GUI::DrawUnitInfo(const GameObject* obj, bool bTarget)
{
  if(!obj)
    return;
    
  UIElm* hpbar  = ui_elm->Find("hpbar");
  UIElm* mpbar  = ui_elm->Find("mpbar");
  UIElm* expbar = ui_elm->Find("bbar");
  UIElm* rbar   = ui_elm->Find("rbar");

  const ZoneInfo* z = NULL;
  const int zoff = bTarget ? 6 : 0;	// offset to skip from bChar1Pict to bChar1Pict2

 // hp
  z = &mGetZone(bChar1HPBar + zoff);
  float hp_perc = obj->mHP / obj->mMaxHP;
  hpbar->DrawWH(z->pos.X, z->pos.Y, z->pos.Width*hp_perc, z->pos.Height, hp_perc, 1.0f);
  centerPrintf(*z, r3dColor(255,255,255), "%d/%d", int(obj->mHP), int(obj->mMaxHP));

 // mp
  if (obj->mMaxMP > 0.01f) {
  z = &mGetZone(bChar1MPBar + zoff);
  float mp_perc = obj->mMP / obj->mMaxMP;
  mpbar->DrawWH(z->pos.X, z->pos.Y, z->pos.Width*mp_perc, z->pos.Height, mp_perc, 1.0f);
  centerPrintf(*z, r3dColor(255,255,255), "%d/%d", int(obj->mMP), int(obj->mMaxMP));
  }
  
  if(!objIsBaseAI(obj)) {
    return;
  }
    
  obj_AI_Base* ai = objToBaseAI(obj);

 // picture
  z = &mGetZone(bChar1Pict + zoff);
  if(ai->bDead) {
    // 1.	If your hero dies then their portrait is grey scaled, and a large timer appears over the portrait 
    // counting down how long until they respawn
    r3dDrawBox2D(z->pos.X, z->pos.Y, z->pos.Width, z->pos.Height, r3dColor(80, 80, 80), ai->CharacterData->Square);
    
    if(objIsHeroAI(ai)) {
      int ttr = (3000 * ai->mLevel) - ai->DeathTimer.Time();
      if(ttr < 0) ttr = 0;

      centerPrintf(*z, r3dColor(255, 255, 255), "%.1f sec", (float)ttr / 1000.0f);
    }
  } else {
    r3dDrawBox2D(z->pos.X, z->pos.Y, z->pos.Width, z->pos.Height, r3dColor(255,255,255), ai->CharacterData->Square);
  }
  Font_Text->PrintF(z->pos.X+56, z->pos.Y+65, r3dColor(0,0,0), "Lv%d", ai->mLevel);
  Font_Text->PrintF(z->pos.X+56, z->pos.Y+66, r3dColor(255,255,255), "Lv%d", ai->mLevel);

 // exp
  z = &mGetZone(bChar1EXPBar + zoff);
  
  if(!bTarget) {
    float curlevel = ai->ExpToCurrentLevel();
    float nextlevel = ai->ExpToNextLevel();
    float left = ai->mEXP - curlevel;
    float exp_perc = R3D_CLAMP(left/(nextlevel-curlevel), 0.0f, 1.0f);
    expbar->DrawWH(z->pos.X, z->pos.Y, z->pos.Width*exp_perc, z->pos.Height, exp_perc, 1.0f);
  } else {
    rbar->DrawWH(z->pos.X, z->pos.Y, z->pos.Width, z->pos.Height, 1.0f, 1.0f);
  }
  centerPrintf(*z, r3dColor(255,255,255), "%s", ai->Name.c_str());
  
 // stats
  {
    z = &mGetZone(bChar1Stats + zoff);
    float x = z->pos.X;
    float y = z->pos.Y;
    r3dColor tclr = r3dColor(255, 255, 255);
             
             Font_Text->PrintF(x, y, tclr, "Armor: %.0f\n", ai->CharInter.mArmor);
    y += 15; Font_Text->PrintF(x, y, tclr, "Speed: %.0f\n", ai->CharInter.mMoveSpeed);
	y += 15; Font_Text->PrintF(x, y, tclr, "Damage: %.0f\n", (ai->CharInter.mAttackDamage+ai->CharInter.mFlatPhysicalDamageMod)*(1.0f+ai->CharInter.mPercentagePhysicalDamageMod));
  }

 // buffs
  {
    z = &mGetZone(bChar1Buffs + zoff);

    float x = z->pos.X;
    float y = z->pos.Y;
    r3dColor tclr = r3dColor(255, 255, 255);
    
    const obj_AI_Base::buff_s* buffs[obj_AI_Base::MAX_SPELL_BUFFS];
    int n_buffs = 0;

    // positive first (meaning, negatives last)
    for(int i=0; i<obj_AI_Base::MAX_SPELL_BUFFS; i++) {
      if(ai->SpellBuffs[i].Script && temp_buffIsPositive(ai->SpellBuffs[i].type)) {
        buffs[n_buffs++] = &ai->SpellBuffs[i];
      }
    }
    // negatives last
    for(int i=0; i<obj_AI_Base::MAX_SPELL_BUFFS; i++) {
      if(ai->SpellBuffs[i].Script && !temp_buffIsPositive(ai->SpellBuffs[i].type)) {
        buffs[n_buffs++] = &ai->SpellBuffs[i];
      }
    }
    assert(n_buffs < obj_AI_Base::MAX_SPELL_BUFFS);
      
    int slot_x = 0;
    int slot_y = 0;

    for(int i=0; i<n_buffs; i++) {
	  if(buffs[i]->type == BUFF_Internal)
		  continue;
      float bx = x + slot_x * 20;
      float by = y + slot_y * 20;
      float bw = 18;			// width of buff icon

      if(buffs[i]->type == BUFF_CombatDehancer) {
        r3dDrawBox2D(bx-1, by-1, bw+2, bw+2, r3dColor(255, 0, 0));
      }
      if(buffs[i]->type == BUFF_Aura) {
        r3dDrawBox2D(bx-1, by-1, bw+2, bw+2, r3dColor(0, 255, 255));
      }

      r3dTexture* buffTex = buffs[i]->Script->GetBuffIcon();
      r3dDrawBox2D(bx, by, bw, bw, r3dColor(255, 255, 255), buffTex ? buffTex : ai->CharacterData->Square);
      if(++slot_x > 4) {
        slot_x = 0;
        if(++slot_y > 1) break;
      }
    }
  }
  
  return;
}



void Menu_GUI::DrawHeroInfo(const GameObject* obj)
{
  if(!obj)
    return;
    
  UIElm* hpbar  = ui_elm->Find("hpbar");
  UIElm* mpbar  = ui_elm->Find("mpbar");
  UIElm* expbar = ui_elm->Find("bbar");
  UIElm* rbar   = ui_elm->Find("rbar");

  const ZoneInfo* z = NULL;
  

 // hp
  z = &mGetZone(bChar1HPBar);
  float hp_perc = obj->mHP / obj->mMaxHP;
  hpbar->DrawWH(z->pos.X, z->pos.Y, z->pos.Width*hp_perc, z->pos.Height, hp_perc, 1.0f);
  centerPrintf(*z, r3dColor(255,255,255), "%d/%d", int(obj->mHP), int(obj->mMaxHP));

 // mp
  if (obj->mMaxMP > 0.01f) {
  z = &mGetZone(bChar1MPBar);
  float mp_perc = obj->mMP / obj->mMaxMP;
  mpbar->DrawWH(z->pos.X, z->pos.Y, z->pos.Width*mp_perc, z->pos.Height, mp_perc, 1.0f);
  centerPrintf(*z, r3dColor(255,255,255), "%d/%d", int(obj->mMP), int(obj->mMaxMP));
  }
  
  if(!objIsBaseAI(obj)) {
    return;
  }
    
  obj_AI_Base* ai = objToBaseAI(obj);

 // picture
  
  z = &mGetZone(bChar1Pict);
  /*
  if(ai->bDead) {
    // 1.	If your hero dies then their portrait is grey scaled, and a large timer appears over the portrait 
    // counting down how long until they respawn
    r3dDrawBox2D(z->pos.X, z->pos.Y, z->pos.Width, z->pos.Height, r3dColor(80, 80, 80), ai->CharacterData->Square);
    
    if(objIsHeroAI(ai)) {
      int ttr = (3000 * ai->mLevel) - ai->DeathTimer.Time();
      if(ttr < 0) ttr = 0;

      centerPrintf(*z, r3dColor(255, 255, 255), "%.1f sec", (float)ttr / 1000.0f);
    }
  } else {
    r3dDrawBox2D(z->pos.X, z->pos.Y, z->pos.Width, z->pos.Height, r3dColor(255,255,255), ai->CharacterData->Square);
  }
  */
  Font_Text->PrintF(z->pos.X+56, z->pos.Y+65, r3dColor(0,0,0), "Lv%d", ai->mLevel);
  Font_Text->PrintF(z->pos.X+56, z->pos.Y+66, r3dColor(255,255,255), "Lv%d", ai->mLevel);

 // exp
  z = &mGetZone(bChar1EXPBar);
  
    float curlevel = ai->ExpToCurrentLevel();
    float nextlevel = ai->ExpToNextLevel();
    float left = ai->mEXP - curlevel;
    float exp_perc = R3D_CLAMP(left/(nextlevel-curlevel), 0.0f, 1.0f);
    expbar->DrawWH(z->pos.X, z->pos.Y, z->pos.Width*exp_perc, z->pos.Height, exp_perc, 1.0f);

	centerPrintf(*z, r3dColor(255,255,255), "%s", ai->Name.c_str());
  
 // stats
  {
    z = &mGetZone(bChar1Stats);
    float x = z->pos.X;
    float y = z->pos.Y;
    r3dColor tclr = r3dColor(255, 255, 255);
             
             Font_Text->PrintF(x, y, tclr, "Armor: %.0f\n", ai->CharInter.mArmor);
    y += 15; Font_Text->PrintF(x, y, tclr, "Speed: %.0f\n", ai->CharInter.mMoveSpeed);
	y += 15; Font_Text->PrintF(x, y, tclr, "Damage: %.0f\n", (ai->CharInter.mAttackDamage+ai->CharInter.mFlatPhysicalDamageMod)*(1.0f+ai->CharInter.mPercentagePhysicalDamageMod));
  }

 // buffs
  {
	  float x = r3dRenderer->ScreenW - 70;
	  float y = r3dRenderer->ScreenH2 - 8*32;
    r3dColor tclr = r3dColor(255, 255, 255);
    
    const obj_AI_Base::buff_s* buffs[obj_AI_Base::MAX_SPELL_BUFFS];
    int n_buffs = 0;

    // positive first (meaning, negatives last)
    for(int i=0; i<obj_AI_Base::MAX_SPELL_BUFFS; i++) {
      if(ai->SpellBuffs[i].Script && temp_buffIsPositive(ai->SpellBuffs[i].type)) {
        buffs[n_buffs++] = &ai->SpellBuffs[i];
      }
    }
    // negatives last
    for(int i=0; i<obj_AI_Base::MAX_SPELL_BUFFS; i++) {
      if(ai->SpellBuffs[i].Script && !temp_buffIsPositive(ai->SpellBuffs[i].type)) {
        buffs[n_buffs++] = &ai->SpellBuffs[i];
      }
    }
    assert(n_buffs < obj_AI_Base::MAX_SPELL_BUFFS);
      
    for(int i=0; i<n_buffs; i++) 
	{
	  if(buffs[i]->type == BUFF_Internal)
		  continue;
      
      float by = y + i*70;
      float bw = 64;			// width of buff icon

      if(buffs[i]->type == BUFF_CombatDehancer) {
        r3dDrawBox2D(x-1, by-1, bw+2, bw+2, r3dColor(255, 0, 0));
      }

      if(buffs[i]->type == BUFF_Aura) {
        r3dDrawBox2D(x-1, by-1, bw+2, bw+2, r3dColor(0, 255, 255));
      }

      r3dTexture* buffTex = buffs[i]->Script->GetBuffIcon();
      r3dDrawBox2D(x, by, bw, bw, r3dColor(255, 255, 255), buffTex ? buffTex : ai->CharacterData->Square);
    }
  }
  
  return;
}



void drawSpellSlot(const MenuLayer::ZoneInfo& z, const SpellDataInst& sdi, obj_AI_Base::usestatus_e ss, r3dTexture* icon, bool selected, bool focused, const char* msg1, const char* msg2)
{
  const r3dColor use_nomana   = r3dColor(20, 20, 155);
  const r3dColor use_disabled = r3dColor(110, 110, 110);
  const r3dColor use_fillcd   = r3dColor24(0, 0, 0, 120);
  
  r3dColor iclr(255, 255, 255);	// icon color
  r3dColor tclr(255, 255, 255);	// text color
  
  do 
  {
    if(ss == obj_AI_Base::USE_NotLearned || ss == obj_AI_Base::USE_NoSpell) {
      r3dDrawBox2D(z.pos.X, z.pos.Y, z.pos.Width, z.pos.Height, r3dColor(255, 255, 255), icon);
      break;
    }

    if(ss == obj_AI_Base::USE_Ok && selected) {
      r3dDrawBox2D(z.pos.X-4, z.pos.Y-4, z.pos.Width+8, z.pos.Height+8, r3dColor(255, 255, 255), icon);
      break;
    }

    if(ss == obj_AI_Base::USE_Ok && focused) {
      // offset icon, so it'll look like a button
      r3dDrawBox2D(z.pos.X+1, z.pos.Y+1, z.pos.Width, z.pos.Height, r3dColor(255, 255, 255), icon);
      break;
    }

    if(ss == obj_AI_Base::USE_Disabled) {
      iclr = use_disabled;
    }
      
    // not enough mana 
    if(sdi.SData)
    {
        if(sdi.SData->Mana[sdi.Level-1] >= Player->mMP) {
          iclr = use_nomana;
        }
    }

    r3dDrawBox2D(z.pos.X, z.pos.Y, z.pos.Width, z.pos.Height, iclr, icon);
    
  } while(0);

  // bottom-right text
  if(msg1 && *msg1) {
    Font_Text->PrintF(z.pos.X + 33, z.pos.Y+27, r3dColor(0,0,0), msg1);
    Font_Text->PrintF(z.pos.X + 34, z.pos.Y+28, tclr, msg1);
  }

  // upper-left text
  if(msg2 && *msg2) {
    Font_Text->PrintF(z.pos.X + 3, z.pos.Y+3, r3dColor(0,0,0), msg2);
    Font_Text->PrintF(z.pos.X + 4, z.pos.Y+4, tclr, msg2);
  }
      
  if(ss == obj_AI_Base::USE_Cooldown) {
    float koef = (sdi.CooldownExpires - r3dGetTime()) / sdi.SData->CooldownTime;
    koef = R3D_CLAMP(koef, 0.0f, 1.0f);

    r3dDrawBox2D(z.pos.X, z.pos.Y, z.pos.Width, z.pos.Height*koef, use_fillcd);
  }
  
  return;
}

static int bDrawHUD = 1;

void Menu_GUI::Draw()
{
  if (Keyboard->WasPressed(kbsF5)) bDrawHUD = 1 - bDrawHUD;
  if (!bDrawHUD) return;

	
  r3dSetFiltering(0);
  r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

  if(CurMode ==	GUIMODE_PAUSE) {
    DrawPause();
    Hud.DrawCursor();
    return;
  }
  
  // target inventory pane: draw before main pane
  if(invPane2Visible > 0.0f)
  {
    const ZoneInfo* z = &mGetZone(bTargetInvPane);
    UIElm* invpane = ui_elm->Find("invpane");
    
    float x = z->pos.X;
    float y = z->pos.Y + (1.0f - invPane2Visible) * z->pos.Height;
    invpane->Draw(x, y);
    
    GameObject* target = GameWorld.GetObject(UI_SelectedObjID);
    if(target && objIsBaseAI(target)) {
      // draw target inventory
      x += 10;
      y += 17;
      obj_AI_Base* ai = objToBaseAI(target);
      for(int i=0; i<NUM_INVENTORY_SLOTS; i++) {
        //r3dDrawBox2D(x, y, 20, 20, r3dColor(255, 255, 255));
        if(ai->Inventory[i].IData) {
          r3dDrawBox2D(x, y, 20, 20, r3dColor(255, 255, 255), ai->Inventory[i].IData->Icon);
        }
        
        x += 22;
      }
    }
  }

  if (0)
  {
    // main pane (relocated from 1024x768 area)
    float offx  = (r3dRenderer->ScreenW - 1024) / 2;
    float offy  = r3dRenderer->ScreenH - 768;

    UIElm* pm = ui_elm->Find("mainpane");
    pm->Draw(offx, r3dRenderer->ScreenH - pm->sh);

    UIElm* pl = ui_elm->Find("mpaneL");
    pl->DrawWH(0,                           r3dRenderer->ScreenH - pl->sh, offx, pl->sh);
    UIElm* pr = ui_elm->Find("mpaneR");
    pr->DrawWH(r3dRenderer->ScreenW - offx, r3dRenderer->ScreenH - pr->sh, offx, pr->sh);
  }

  // Draw charInfo
  //[ST] !!! Console.Draw();

  //Draw Build Number info on hotkey press.
  if(Keyboard->IsPressed(kbsB))
  {
    //r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
    r3dDrawBox2D(r3dRenderer->ScreenW/2 - 250.0f, 200.0f, 500.0f, 40.0f, r3dColor(0,0,0));
    Font_Text->PrintF( r3dRenderer->ScreenW/2 - 245.0f, 205, r3dColor(255,255,255), "Onslaught Build: %s", versionGetString());
  }


  static int ShowStuff = 0;
  if (Keyboard->WasPressed(kbsM)) ShowStuff = 1 - ShowStuff;

  if (ShowStuff)
  { 
	  int but = Mouse->GetButtons();
   extern float BillAngle;

   extern float BillDistX;
   extern float BillDistY;
   extern float __ShadowBoxSize;
   extern float __ShadowDistance;
   extern float GameFOV;

   int SliderY = _SafeBorder+35;
   Edit_Value_Slider(r3dRenderer->ScreenW/2+130, SliderY, 30,80,"%-02.0f","Angle",&BillAngle,mx,my,but,1); SliderY += 30;
   Edit_Value_Slider(r3dRenderer->ScreenW/2+130, SliderY, 10,120,"%-02.0f","FOV",&GameFOV,mx,my,but,1); SliderY += 30;
   Edit_Value_Slider(r3dRenderer->ScreenW/2+130, SliderY, 10,7000,"%-02.0f","SB Size",&__ShadowBoxSize,mx,my,but,1); SliderY += 30;
   Edit_Value_Slider(r3dRenderer->ScreenW/2+130, SliderY, 50,6000,"%-02.0f","SB Dist",&__ShadowDistance,mx,my,but,1); SliderY += 30;
  }


  // Draw Day/night 
  if(0)
  {
    //float CAngle = (__CurTime/24.0f)*360.0f;

    //r3dDrawBox2DRotate(r3dRenderer->ScreenW/2, 0, 128, 128, CAngle, r3dColor24(255,255,255), imgDayNight);
    //int zNum = mFindZone(ltt_TimeOfDay);
    //r3dDrawBox2DRotate(zones[zNum].pos.X, zones[zNum].pos.Y, zones[zNum].pos.Width, zones[zNum].pos.Height, CAngle, r3dColor24(255,255,255), imgDayNight);

    //pw_Draw2DTC(r3dRenderer->ScreenW/2-128, 0, 256,128, r3dColor(255,255,255), 16, 16, 12, 0, 4,2, imgHUD1);
  }

  // Minimap
  {
    float x = 5;
    float y = 5;
  
    UIElm* map = ui_elm->Find("map");
    //map->Draw(x, y, r3dColor(255, 255, 255, 200));
  
    guiMap.Draw();
  
    const ZoneInfo& z = mGetZone(bPingMiniMap);
    //uiDrawButton(ui_elm, z.pos.X, z.pos.Y, "btnping_1", curz_id == bPingMiniMap, mPinging);
  }
  
  
  // spell upgrades
  for(int i=0;i<4;i++) {
    const ZoneInfo& z = mGetZone(bSpellUpgrade1+i);
    if(!z.bEnabled) continue;
      
    uiDrawButton(ui_elm, z.pos.X, z.pos.Y, "btnsupg_1", curz_id == z.id, false);
  }
  
  // spells
  for(int i=0;i<MAX_SPELLS_PER_CHAR;i++) 
  {
    const ZoneInfo& z = mGetZone(bSpellCast1+i);
    SpellDataInst& sdi = Player->Spells[i];
    if(!sdi.SData) continue;
      
    r3dTexture* icon = sdi.SData->imgIconTex;
      
    obj_AI_Base::usestatus_e ss = Player->CanUseSpell(i);
      
    if(ss == obj_AI_Base::USE_NotLearned) {
      r3dDrawBox2D(z.pos.X, z.pos.Y, z.pos.Width, z.pos.Height, r3dColor(80, 80, 80), icon);
      continue;
    }
    
    if(Player->AI_AutocastSpell == i) {
      float alpha = 128 + (1.0f + sinf(r3dGetTime() * 10.0f)) * 0.5f * 128;
      r3dDrawBox2D(z.pos.X-1, z.pos.Y-1, z.pos.Width+2, z.pos.Height+2, r3dColor(255, 0, 0, (int)alpha));
    }
    
    char msg1[32]; 
    sprintf(msg1, "%d", sdi.Level);
    drawSpellSlot(z, sdi, ss, icon, Player->AI_SelectedSpell == i, curz_id == z.id, msg1, NULL);
  }
  
  // inventory (there is major copy-paste code from spells, but with few changes)
  for(int i=0;i<NUM_INVENTORY_SLOTS;i++) {
    const ZoneInfo& z = mGetZone(bInvSlot1+i);
      
    const InventorySlot& inv = Player->Inventory[i];
    if(!inv.IData) continue;

    r3dTexture* icon = inv.IData->Icon;
      
    const SpellDataInst& sdi = Player->Spells[i + obj_AI_Base::SPELLS_ITEMS_SLOT];

    obj_AI_Base::usestatus_e ss = Player->CanUseItem(i);

    char msg1[32] = "";
    char msg2[32] = "";
    if(inv.IData->SpellCharges) sprintf(msg1, "%d", inv.SpellCharges);
    if(inv.IData->MaxStack>1)   sprintf(msg2, "%d", Player->Inventory[i].NumInSlot);

    drawSpellSlot(z, sdi, ss, icon, Player->AI_SelectedSpell == i, curz_id == z.id, msg1, msg2);
  }
  
  // character info
  DrawHeroInfo(Player);
//  DrawUnitInfo(GameWorld.GetObject(UI_SelectedObjID), true);
  
  // gold
  centerPrintf(mGetZone(bGoldArea), r3dColor(255,255,255), "%d", (int)Player->mGold);

  gMenuLayersMgr.Draw();

  Console.Draw();

 Hud.DrawCursor();

 // item-dragging: draw
 if(invDragSlot != -1) {
   assert(Player->Inventory[invDragSlot].IData);
   
   r3dTexture* icon = Player->Inventory[invDragSlot].IData->Icon;
   assert(icon);
   
   r3dDrawBox2D(mx, my, icon->Width, icon->Height, r3dColor(255, 255, 255), icon);
 }

  r3dSetFiltering(1);
}

void Menu_GUI::DrawGameStats()
{
	int but    = Mouse->GetButtons();

	int ID = mFindZone(bStats_Close);
	int bHover = (curz == ID); 
 	
	int ChaosTurretsDestroyed = 0, OrderTurretsDestroyed = 0;
	int ChaosKills = 0, OrderKills = 0;
	int ChaosDeaths = 0, OrderDeaths = 0;

	int x_size = 470, y_size = 350;
	int y_offset = 10;
	int row_height = 36;
	int col_width = 120;
	int name_width = 150, char_width = 150, kill_width = 80, death_width = 80;
	int death_timer_width = 40;
	int mLevelTime = 0;

	r3dColor TextCol = r3dColor(255,255,255);

	GameObject *obj = GameWorld.GetFirstObject();

 	DrawUIButton(zones[ID].pos.X, zones[ID].pos.Y,
	     zones[ID].pos.Width, zones[ID].pos.Height,
	     bHover, but, imgHUD1, 16, 16, 11,7,1,1);

	//r3dDrawBox2D(_SafeBorder, _SafeBorder+64, 1000, 470, r3dColor24(15,15,25,220));
	//r3dDrawBox2D(r3dRenderer->ScreenW - x_size - 10, 10, x_size, y_size, r3dColor24(15,15,25,128));
	uiDrawFrame(ui_elm, r3dRenderer->ScreenW - x_size - 20, 5, x_size, y_size, "Store Main Frame");

	Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10, 10 + y_offset, name_width, row_height, TextCol, "Player");
	Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10 + name_width, 10 + y_offset, char_width, row_height, TextCol, "Hero");
	Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10 + name_width + char_width, 10 + y_offset, kill_width, row_height, TextCol, "Kills");
	Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10 + name_width + char_width + kill_width, 10 + y_offset, death_width, row_height, TextCol, "Deaths");

	for(const GameObject* obj = GameWorld.GetFirstObject(); obj; obj = GameWorld.GetNextObject(obj)) 
	{
		if(!objIsHeroAI(obj))
			continue;
		
		const obj_AI_Hero* ai = (const obj_AI_Hero*)obj;
			
		if(ai->TeamID == GameObject::TEAM_ORDER) {
			TextCol = r3dColor(0,0,255);

			Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10, row_height + y_offset, col_width, row_height, TextCol, "%s", ai->Name.c_str());
			if(ai->bDead) {
				mLevelTime = (ai->mLevel * 3) - (int)(ai->DeathTimer.Seconds());
				Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10, row_height + y_offset, name_width, row_height, TextCol, "%s", ai->Name.c_str());
				Font_Label->PrintF(r3dRenderer->ScreenW - x_size + name_width - death_timer_width - 4, row_height+y_offset, death_timer_width, row_height, TextCol, "(%d)", mLevelTime);
			}
			else {
				Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10, row_height + y_offset, name_width, row_height, TextCol, "%s", ai->Name.c_str());
			}
			Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10 + name_width, row_height + y_offset, char_width, row_height, TextCol, "%s", ai->CharacterData->Name);
			Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10 + name_width + char_width, row_height + y_offset, kill_width, row_height, TextCol, "%d", ai->Stats.mNumHerosKilled);
			Font_Label->PrintF(r3dRenderer->ScreenW-x_size-10+name_width+char_width+kill_width, row_height+y_offset, death_width, row_height, TextCol, "%d", ai->Stats.mNumDeaths);
		}

		if(ai->TeamID == GameObject::TEAM_CHAOS) {
			TextCol = r3dColor(255,0,0);

			Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10, row_height + y_offset, col_width, row_height, TextCol, "%s", ai->Name.c_str());
			if(ai->bDead) {
				mLevelTime = (ai->mLevel * 3) - (int)(ai->DeathTimer.Seconds());
				Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10, row_height*6 + y_offset, name_width, row_height, TextCol, "%s", ai->Name.c_str());
				Font_Label->PrintF(r3dRenderer->ScreenW - x_size + name_width - death_timer_width - 4, row_height*6+y_offset, death_timer_width, row_height, TextCol, "(%d)", mLevelTime);
			}
			else {
				Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10, row_height*6 + y_offset, name_width, row_height, TextCol, "%s", ai->Name.c_str());
			}
			Font_Label->PrintF(r3dRenderer->ScreenW - x_size-10+name_width, row_height*6 + y_offset, char_width, row_height, TextCol, "%s", ai->CharacterData->Name);
			Font_Label->PrintF(r3dRenderer->ScreenW - x_size-10+name_width + char_width, row_height*6 + y_offset, kill_width, row_height, TextCol, "%d", ai->Stats.mNumHerosKilled);
			Font_Label->PrintF(r3dRenderer->ScreenW-x_size-10+name_width+char_width+kill_width, row_height*6+y_offset, death_width, row_height, TextCol, "%d", ai->Stats.mNumDeaths);
		}

		y_offset += row_height;
	}

	TextCol = r3dColor(0,0,255);
	Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10, y_offset + 30, 250, row_height, TextCol, "Order Towers Lost: %d\n", OrderTurretsDestroyed);
	TextCol = r3dColor(255,0,0);
	Font_Label->PrintF(r3dRenderer->ScreenW - x_size - 10, y_offset + 50, 250, row_height, TextCol, "Chaos Towers Lost: %d\n", ChaosTurretsDestroyed);
}



void Menu_GUI::DrawPause()
{
  extern r3dScreenBuffer* BlurBuffer;
  //r3dRenderer->SetPixelShader("PS_FILTER_BW");
  //r3dDrawBox2D(0,0,r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(180, 190, 230), BlurBuffer.Tex);
  r3dDrawBox2D(0,0,r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(0,0,0,220));
  //r3dRenderer->SetPixelShader();

  uiDrawFrame(ui_elm, r3dRenderer->ScreenW/2-100, r3dRenderer->ScreenH/2-150, 200, 300, "Pause Menu Frame");

  const char* CMenu[] = {"Interface Options Button", "Back Button", "End Game Button"};
  for(int i=0;i<3;i++) {
    int z = mFindZone(bOptions_Options+i);
    uiDrawButton(ui_elm, zones[z].pos.X, zones[z].pos.Y, CMenu[i], z == curz, false);
  }
  
  return;
}
