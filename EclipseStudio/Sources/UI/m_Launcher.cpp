#include "r3dPCH.h"
#include "r3d.h"

#include "menu/m_Launcher.h"
#include "UI/UIElements.h"
#include "menu/m_gui.h"

#include "Gamecommon.h"

  #include "menu/m_LightEditor.h"
  Menu_LightEditor*    menu_LE = NULL;

  #include "menu/m_ObjectEditor.h"
  Menu_ObjectEditor*   menu_OE = NULL;

  #include "menu/m_SkyEditor.h"
  Menu_SkyEditor*   menu_SE = NULL;

#include "menu/m_BakeTool.h"
  Menu_BakeTool* menu_BT = NULL;


#pragma warning(push)
#pragma warning(disable:4244)

static const int menu_layer_w     = 200;	// width & height of the this menu
static const int menu_layer_h     = 400;

static const int menu_btns_x      = 10;		// buttons start x/y
static const int menu_btns_y      = 25;
static const int menu_sliders_x   = 10;		// emitters slots x/y
static const int menu_sliders_y   = 65;

extern	bool	imgui_MouseWasDown;
extern	bool	imgui_disabled;

extern Menu_GUI *GUIMenu;


Menu_Launcher::Menu_Launcher()
{
}

Menu_Launcher::~Menu_Launcher()
{
}

void Menu_Launcher::Init()
{
  mReset();
  mAddZone(bClose,        menu_layer_w-32-2, -2, 24, 24);	//NOTE: add 'x' button
  mAddZone(zTitle,        0,   0, menu_layer_w, 20);		//NOTE: add title zone for dragging

  for(int i=0; i<bBtnGroup1_End-bBtnGroup1_Start-1; i++) {
    mAddZone(bBtnGroup1_Start+i+1, menu_btns_x, menu_btns_y+i*30, 180, 25);
  }

  layerX = 0;
  layerY = 0;
  layerW = menu_layer_w;
  layerH = menu_layer_h;
  RelocateZones(10, r3dRenderer->ScreenH - layerH-20);

  menu_LE = new Menu_LightEditor;
  gMenuLayersMgr.Insert(menu_LE, GUIMenu->ui_elm);

  menu_OE = new Menu_ObjectEditor;
  gMenuLayersMgr.Insert(menu_OE, GUIMenu->ui_elm);

  menu_SE = new Menu_SkyEditor;
  gMenuLayersMgr.Insert(menu_SE, GUIMenu->ui_elm);

  menu_BT = new Menu_BakeTool;
  gMenuLayersMgr.Insert(menu_BT, GUIMenu->ui_elm);
 
  return;
}

void Menu_Launcher::Destroy()
{
  //NOTE: here you should destroy all things created in Init()
	gMenuLayersMgr.Remove(menu_LE);		SAFE_DELETE(menu_LE);
	gMenuLayersMgr.Remove(menu_OE);		SAFE_DELETE(menu_OE);
	gMenuLayersMgr.Remove(menu_SE);		SAFE_DELETE(menu_SE);
    gMenuLayersMgr.Remove(menu_BT);     SAFE_DELETE(menu_BT);
}


void Menu_Launcher::Activate()
{
  return;
}

void Menu_Launcher::Deactivate()
{
  //Called when menu is deactivated (closed)
  //NOTE: here you should destroy all things created in Activate()
  
  return;
}


int Menu_Launcher::Update()
{
  // NOTE: process all logic (button clicks & keyboard) here
  //  if Update() return 0 - menu will become inactive in floating menu manager

  switch(released_id) 
  {
    case bClose:
      Deactivate();
      layerVisible = false;
      return 0;
      
    case bButton1: 
		{
//		  gMenuLayersMgr.BringToFront(menu_PE);
		}
		break;

    case bButton2: 
		{
			
extern int		bLightEditorActive;
		  bLightEditorActive = 1;
		  gMenuLayersMgr.BringToFront(menu_LE);

		}
		break;

    case bButton3: 
		{
			gMenuLayersMgr.BringToFront(menu_OE);

		}
		break;

	    case bButton4: 
		{
			gMenuLayersMgr.BringToFront(menu_SE);

		}
		break;

	    case bLightBaking:
        	{
        		gMenuLayersMgr.BringToFront(menu_BT);
	        }
		break;


		
  }


  return 1;
}

void Menu_Launcher::Draw()
{
  r3dDrawBox2D(int(layerX) + 0, int(layerY) + 0, layerW, layerH, r3dColor24(0, 0, 0));
  uiDrawFrame(ui_elm, int(layerX) + 0, int(layerY) + 0, layerW, layerH, "Store Main Frame");
  
  // header
  if(1) {
    SIZE sz;
    Font_Editor->GetTextExtent("Editors", &sz);
    Font_Editor->PrintF(layerX + float(layerW/2-sz.cx/2), layerY, r3dColor(0, 0, 0), "Editors");
  }

  // nice highlighted button
  DrawButton(bClose, "X Close button",      false);

  // draw & update immediate mode controls
  ProcessIMControls();
  
  return;
}

void Menu_Launcher::ProcessIMControls()
{
 //NOTE: here you should place all immediate mode sliders and stuff

  const char* N1[] = {"Particle Editor", "Light Editor", "Objects Tweaker", "Sky Editor", "Minion Editor", "Light Baking", "Turret Editor"};
  DrawTextButtons(bBtnGroup1_Start+1, 7, N1);
  
  return;
}

#pragma warning(pop)
