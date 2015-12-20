#include "r3dPCH.h"
#include "r3d.h"

#include "UI\r3dMenu.h"


#pragma warning(disable:4244)

r3dTexture	*menu_Cursor;
CD3DFont 	*MenuFont_Editor;





int r3dMenuInit()
{
	Mouse->SetRange(win::hWnd);
	//Mouse->MoveTo(r3dRenderer->ScreenW/2, r3dRenderer->ScreenH/2);

	menu_Cursor = r3dRenderer->LoadTexture("data\\Menu\\cursor.dds");

	r3dIntegrityGuardian ig ;

	MenuFont_Editor = game_new CD3DFont( ig, "Verdana", 8, 0);//D3DFONT_FILTERED);
	MenuFont_Editor->CreateSystemFont();

	return 1;
}


int r3dMenuClose()
{
	r3dRenderer->DeleteTexture(menu_Cursor);

	delete MenuFont_Editor;
	return 1;
}







MenuLayer::MenuLayer()
{
	n_zones = 0;
	CurFrame  = 0;

	mx = Mouse->m_MouseMoveX;
	my = Mouse->m_MouseMoveY;
	Mouse->GetXY(mx, my);

	mzMotion  = 0;

	mStartFrameTime = 0;
	mLastFrameTime  = 0.3f;
}


MenuLayer::~MenuLayer()
{
	mReset();
}

int MenuLayer::mReset()
{
	n_zones = 0;

	return 1;
}

int MenuLayer::mAddZone(int id, int x, int y, int w, int h)
{
	// just to clear any current button press
	//BP
	curz    = -1;
	curz_id = -1;

	zones[n_zones].pos.Init((float)x, (float)y, (float)w, (float)h);
	// zones[n_zones].pos.X = x;
	// zones[n_zones].pos.Y = y;
	// zones[n_zones].pos.Width = w;
	// zones[n_zones].pos.Height = h; 

	zones[n_zones].id    = id;
	zones[n_zones].bEnabled = 1; 

	n_zones++;
	if(n_zones >= MAX_ZONES){ 
		assert(!(n_zones >= MAX_ZONES));
		return -1; //pwAbortApp("mAddZone::too much zones");
	}

	return n_zones-1;
}

int MenuLayer::mFindZone(int zone_id)
{
	int	i;

	for(i=0; i<n_zones; i++)
	{
		if(zones[i].id == zone_id)
			return i;
	}
	assert(0);
	return 0;
}

MenuLayer::ZoneInfo& MenuLayer::mGetZone(int zone_id)
{
	int	i;

	for(i=0; i<n_zones; i++) {
		if(zones[i].id == zone_id)
			return zones[i];
	}

	r3dError("mGetZone %x", zone_id);
	return zones[0];
}


void MenuLayer::EnableZone(int ZoneID)
{
	int	i;

	if (ZoneID == -1)
	{
		for(i=0; i<n_zones; i++)
			zones[i].bEnabled = 1;
		return;
	}

	zones[mFindZone(ZoneID)].bEnabled = 1;
	return;
}


void MenuLayer::DisableZone(int ZoneID)
{
	int	i;

	if (ZoneID == -1)
	{
		for(i=0; i<n_zones; i++)
			zones[i].bEnabled = 0;
		return;
	}

	zones[mFindZone(ZoneID)].bEnabled = 0;
	return;
}



extern void InputUpdate();
int MenuLayer::mUpdate()
{
	int	i;

	InputUpdate();

	Mouse->GetXY(mx, my);
	mzMotion = 0;
	//	Mouse->QueryWheelMotion(&mzMotion);


	// check zones
	curz    = -1;
	curz_id = -1;
	for(i = 0; i < n_zones; i++)
	{
		if(zones[i].pos.Contains(mx, my) && zones[i].bEnabled) 
		{
			curz   = i;
			curz_id = zones[i].id;
			break;
		}
	}
	released_id = -1;
	pressed_id  = -1;
	if(curz != -1 && Mouse->WasReleased(r3dMouse::mLeftButton))
		released_id = zones[curz].id;

	if(curz != -1 && Mouse->WasPressed(r3dMouse::mLeftButton))
		pressed_id = zones[curz].id;

	return 1;
}

void MenuLayer::mDrawStart()
{
	CurFrame++;
	mStartFrameTime = r3dGetTime();

	//////!!!
	r3dRenderer->StartFrame();
	r3dRenderer->StartRender(1);

	//  if(curz != -1)
	//    Draw_OnZone(curz);
}

// draw cursor
void MenuLayer::mDrawCursor()
{
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
	r3dDrawBox2D(mx, my, 32, 32, r3dColor(255,255,255), menu_Cursor);

	// Font.PrintF ( 0,200,r3dColor24(255, 0, 0), "%d %d", mx, my);
}

void MenuLayer::mDrawEnd()
{
	mDrawCursor();

	r3dRenderer->Flush();  

	r3dRenderer->EndFrame();

	if( r3dRenderer->DeviceAvailable )
	{
		r3dUpdateScreenShot();
		if(Keyboard->WasPressed(kbsPrtScr))
			r3dToggleScreenShot();
	}

	r3dRenderer->EndRender( true );
	Sleep(5);

	r3dProcessWindowMessages();

	mLastFrameTime = r3dGetTime() - mStartFrameTime;

	return;
}




MenuLayer2::MenuLayer2()
{
	ui_elm = NULL;

	layerX = 0;
	layerY = 0;
	bDragging = false;

	layerVisible = false;
	layerDraggable = true;
	layerExclusive = false;

	return;
}

MenuLayer2::~MenuLayer2()
{
}

void MenuLayer2::ProcessDragging()
{
	extern const void* imgui_val;

	static float drag_x;
	static float drag_y;
	if(Mouse->WasPressed(r3dMouse::mLeftButton) && !bDragging && curz_id == zTitle) {
		bDragging = true;
		drag_x = mx - mGetZone(zTitle).pos.X;
		drag_y = my - mGetZone(zTitle).pos.Y;

		//HACK: simulate that we're dragging some weird slider (from Slider.hpp) - 
		imgui_val = &drag_x;
	}

	if(bDragging) {
		float dx, dy;

		dx = (mx - drag_x) - mGetZone(zTitle).pos.X;
		dy = (my - drag_y) - mGetZone(zTitle).pos.Y;
		if(dx*dx + dy*dy > 0.1f) {
			RelocateZones(dx, dy);
		}

		if(Mouse->WasReleased(r3dMouse::mLeftButton)) {
			bDragging = false;
			imgui_val = NULL;
		}
	}

	return;
}

bool MenuLayer2::IsMouseOver(float x, float y)
{
	if(!layerVisible)
		return false;

	if(x >= layerX && x <= layerX + layerW && y >= layerY && y <= layerY + layerH)
		return true;

	return false;
}

void MenuLayer2::RelocateZones(float dx, float dy)
{
	for(int i=0; i<n_zones; i++) {
		zones[i].pos.X += dx;
		zones[i].pos.Y += dy;
	}

	layerX += dx;
	layerY += dy;

	return;
}

void MenuLayer2::DrawButton(int zone, const char* btnname, bool selected)
{
	int z = mFindZone(zone);
	int x = int(zones[z].pos.X);
	int y = int(zones[z].pos.Y);

	uiDrawButton(ui_elm, x, y, btnname, z == curz, selected);
	return;
}

void MenuLayer2::DrawTextButtons(int start_id, int num, const char* names[], int selected_id)
{
	int startz = mFindZone(start_id);
	assert(startz >= 0);

	for(int i=0;i<num;i++)
	{
		const MenuLayer::ZoneInfo& z = zones[i + startz];

		r3dColor24 TCol = r3dColor24(255,255,255);
		r3dColor24 BCol = r3dColor24(80,80,80);

		if(z.id == selected_id) {
			TCol = r3dColor24(170,170,255);
			BCol = r3dColor24(100,100,100);
		}

		if(curz == (i + startz)) {
			TCol = r3dColor24(255,168,0);
			BCol = r3dColor24(120,120,120);
		}

		r3dDrawBox2D(z.pos.X-1, z.pos.Y-1, z.pos.Width+2, z.pos.Height+2, r3dColor24(0,0,0));
		r3dDrawBox2D(z.pos.X, z.pos.Y, z.pos.Width, z.pos.Height, BCol);

		MenuFont_Editor->DrawScaledText(z.pos.X+z.pos.Width/2, z.pos.Y+z.pos.Height/2-2, 
			1,1,1, TCol, names[i], D3DFONT_CENTERED);
	}

	r3dRenderer->Flush();
	return;
}


MenuLayersMgr	gMenuLayersMgr;

MenuLayersMgr::MenuLayersMgr()
{
	activeLayer = NULL;
	mouseOver   = NULL;
	disableLayersUpdate = false;
}

MenuLayersMgr::~MenuLayersMgr()
{
}

MenuLayersMgr::layerList_t::iterator MenuLayersMgr::FindLayer(MenuLayer2* layer, bool bAbortOnError)
{
	assert(layer);

	for(layerList_t::iterator it = layers.begin(); it != layers.end(); ++it) {
		if(*it == layer)
			return it;
	}

	if(bAbortOnError) 
		r3dError("layer %p isn't in layer list", layer);

	return layers.end();
}


void MenuLayersMgr::Insert(MenuLayer2* layer, UIElements* base_ui_elm)
{
	assert(layer);
	assert(FindLayer(layer, false) == layers.end());

	layer->ui_elm = base_ui_elm;
	layer->Init();

	layers.push_front(layer);
	return;
}

void MenuLayersMgr::Remove(MenuLayer2* layer)
{
	assert(layer);

	layerList_t::iterator it = FindLayer(layer);
	layers.erase(it);

	if(layer == activeLayer) {
		activeLayer = NULL;
	}

	layer->Destroy();

	return;
}

bool MenuLayersMgr::Update(MenuLayer* baseMenu)
{
	if(disableLayersUpdate)
		return false;

	int mx, my;
	Mouse->GetXY(mx, my);

	// check if we over any visible layers
	mouseOver = NULL;
	for(layerList_t::iterator it = layers.begin(); it != layers.end(); ++it) {
		MenuLayer2* l = *it;

		if(l->layerVisible && l->IsMouseOver(mx, my)) {
			mouseOver = l;
			break;
		}
	}

	if(mouseOver && baseMenu) {
		// clear base menu pressed/released vars
		baseMenu->pressed_id  = -1;
		baseMenu->released_id = -1;
		baseMenu->curz        = -1;
		baseMenu->curz_id     = -1;
	}

	// process current active layer
	if(activeLayer) {

		activeLayer->mUpdate();

		if(activeLayer->layerDraggable) {
			activeLayer->ProcessDragging();
		}

		if(activeLayer->Update() == 0) {
			activeLayer = NULL;
		}
	}

	// see if we need to switch to new layer. if so - bring it to front.
	if((Mouse->IsPressed(r3dMouse::mLeftButton)) && 
		mouseOver && 
		activeLayer != mouseOver && 
		(activeLayer == NULL || activeLayer->layerExclusive == false)) 
	{
		activeLayer = mouseOver;

		layerList_t::iterator it = FindLayer(mouseOver);
		layers.erase(it);
		layers.push_front(activeLayer);
	}

	return mouseOver != NULL;
}

void MenuLayersMgr::Draw()
{
	extern bool imgui_disabled;

	// draw back-to-front
	for(layerList_t::reverse_iterator it = layers.rbegin(); it != layers.rend(); ++it) {
		MenuLayer2* l = *it;

		//HACK: allow sliders to work only when layer is active
		if(l == activeLayer && !disableLayersUpdate) 
			imgui_disabled = false;
		else                 
			imgui_disabled = true;

		if(l->layerVisible)
			l->Draw();
	}

	imgui_disabled = false;

	return;
}

bool MenuLayersMgr::CloseTopWindow()
{
	if(activeLayer) {
		activeLayer->Deactivate();
		activeLayer->layerVisible = false;
		activeLayer = NULL;
		return true;
	}

	return false;
}

void MenuLayersMgr::BringToFront(MenuLayer2* layer)
{
	assert(layer);

	layerList_t::iterator it = FindLayer(layer);
	layers.erase(it);
	layers.push_front(layer);

	if(!layer->layerVisible) {
		layer->Activate();
		layer->layerVisible = true;
	}

	activeLayer = layer;

	return;
}

