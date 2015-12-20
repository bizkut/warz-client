#include "r3dPCH.h"
#include "r3d.h"

#include "menu/m_LightEditor.h"
#include "menu/UIElements.h"

#include "GameCommon.h"

#include "ObjectsCode\World\Lamp.h"
#include "GameStart.h"


#pragma warning(push)
#pragma warning(disable:4244)

static const int menu_layer_w     = 500;	// width & height of the this menu
static const int menu_layer_h     = 300;

static const int menu_btns_x      = 10;		// buttons start x/y
static const int menu_btns_y      = 25;
static const int menu_sliders_x   = 10;		// emitters slots x/y
static const int menu_sliders_y   = 65;

extern	bool	imgui_MouseWasDown;
extern	bool	imgui_disabled;

extern int		bLightEditorActive;
extern GameObject*	LightObject;
GameObject*	PrevLightObject = NULL;




Menu_LightEditor::Menu_LightEditor()
{
}

Menu_LightEditor::~Menu_LightEditor()
{
}

void Menu_LightEditor::Init()
{
  mReset();
  mAddZone(bClose,        menu_layer_w-32-2, -2, 24, 24);	//NOTE: add 'x' button
  mAddZone(zTitle,        0,   0, menu_layer_w, 20);		//NOTE: add title zone for dragging

  for(int i=0; i<bBtnGroup1_End-bBtnGroup1_Start-1; i++) {
    mAddZone(bBtnGroup1_Start+i+1, menu_btns_x+i*100, menu_btns_y, 110, 25);
  }

  layerX = 0;
  layerY = 0;
  layerW = menu_layer_w;
  layerH = menu_layer_h;
  RelocateZones(10, r3dRenderer->ScreenH - layerH-20);
  
  return;
}

void Menu_LightEditor::Destroy()
{
  //NOTE: here you should destroy all things created in Init()
}

void Menu_LightEditor::Activate()
{
  // called when menu is activated
  Light_R = 255;
  Light_G = 255;
  Light_B = 255;
  Light_Radius = 300;

  if (LightObject)
  {
   obj_LampBulb *L = (obj_LampBulb *)LightObject;

   Light_R = (float)L->Color.R;
   Light_G = (float)L->Color.G;
   Light_B = (float)L->Color.B;
   Light_Radius = L->LT.GetOuterRadius();
  }
 
  return;
}

void Menu_LightEditor::Deactivate()
{
  //Called when menu is deactivated (closed)
  //NOTE: here you should destroy all things created in Activate()
  
  return;
}

int Menu_LightEditor::Update()
{
  if (LightObject && (PrevLightObject != LightObject))
  {
   PrevLightObject = LightObject;
   obj_LampBulb *L = (obj_LampBulb *)LightObject;

   Light_R = (float)L->Color.R;
   Light_G = (float)L->Color.G;
   Light_B = (float)L->Color.B;
   Light_Radius = L->Radius;
  }


  // NOTE: process all logic (button clicks & keyboard) here
  //  if Update() return 0 - menu will become inactive in floating menu manager

  if (!bLightEditorActive)
  {
      Deactivate();
      layerVisible = false;
	  return 0;
  }

  switch(released_id) 
  {
    case bClose:
      Deactivate();
      layerVisible = false;
	  bLightEditorActive = 0;
      return 0;
      
    case bButton1: 
		{
			FILE *f;
				
				char FName[256];
   
				sprintf(FName,"Levels\\%s\\Light.dat", MissionInfo.MissionDir);

				f = fopen(FName, "wt");

				for(const GameObject* obj = GameWorld.GetFirstObject(); obj; obj = GameWorld.GetNextObject(obj)) 
				{
					if(obj->Class->Name == "obj_LampBulb")
					{
						obj_LampBulb *L = (obj_LampBulb *)obj;
						fprintf(f,"%d %d %d %d %d %d %d\n", int(L->Position.X), int(L->Position.Y), int(L->Position.Z), 
							L->Color.R, L->Color.G, L->Color.B, int(L->Radius));
					}
				}
				fclose (f);
		}
		break;
		
  }

  return 1;
}

void Menu_LightEditor::Draw()
{

	  extern int bUseDynamicLight;
  if (Keyboard->WasPressed(kbsF11)) bUseDynamicLight = 1 - bUseDynamicLight;

 
  Font_Editor->PrintF(5,5,r3dColor(255,255,255), bUseDynamicLight ? "F11: BAKED LIGHTING" : "F11: DYNAMIC LIGHTING");



  if (LightObject && (PrevLightObject != LightObject))
  {
   
   PrevLightObject = LightObject;
   obj_LampBulb *L = (obj_LampBulb *)LightObject;

   Light_R = (float)L->Color.R;
   Light_G = (float)L->Color.G;
   Light_B = (float)L->Color.B;
   Light_Radius = L->Radius;
  }
    
  r3dDrawBox2D(int(layerX) + 0, int(layerY) + 0, layerW, layerH, r3dColor24(0, 0, 0));
  uiDrawFrame(ui_elm, int(layerX) + 0, int(layerY) + 0, layerW, layerH, "Store Main Frame");
  
  // header
  if(1) {
    SIZE sz;
    Font_Editor->GetTextExtent("LIGHT SOURCE PROPERTIES", &sz);
    Font_Editor->PrintF(layerX + float(layerW/2-sz.cx/2), layerY, r3dColor(0, 0, 0), "LIGHT SOURCE PROPERTIES");
  }

  // nice highlighted button
  DrawButton(bClose, "X Close button",      false);

  // draw & update immediate mode controls
  ProcessIMControls();
  
  return;
}

void Menu_LightEditor::ProcessIMControls()
{
 //NOTE: here you should place all immediate mode sliders and stuff

  const char* N1[] = {"Save", "Green", "Radius", "B4"};
  DrawTextButtons(bBtnGroup1_Start+1, 1, N1);
  
  float x = menu_sliders_x + (int)layerX;
  float y = menu_sliders_y + (int)layerY;
  int but = Mouse->GetButtons();

  if (Keyboard->WasPressed(kbsIns))
  {
	r3dPoint3D dir;
	r3dScreenTo3D(mx, my, &dir);

	GameObject* target = NULL;

	CollisionInfo CL;
	target = GameWorld.CastRay(gCam, dir, 5000.0f, &CL);
	if(target)
	{
	  obj_LampBulb *proj = (obj_LampBulb *)srv_CreateGameObject("obj_LampBulb", "Light", CL.NewPosition - dir*100.0f);
		assert(proj);

		proj->Color	= r3dColor(255,255,255);
		proj->Radius = 200;
		
		LightObject = proj;
	}

  }



//  Font_Editor->PrintF(x, y, r3dColor(255,255,255), "Vars: %.1f %.1f %.1f", editVar1, editVar2, editVar3);
//  y += 50;
  
  y += 25; Edit_Value_Slider(x, y, 0.0f, 255.0f, "%02.0f", "RED",    &Light_R, mx, my, but, true);
  y += 25; Edit_Value_Slider(x, y, 0.0f, 255.0f, "%02.0f", "GREEN",  &Light_G, mx, my, but, true);
  y += 25; Edit_Value_Slider(x, y, 0.0f, 255.0f, "%02.0f", "BLUE",   &Light_B, mx, my, but, true);
  y += 25; Edit_Value_Slider(x, y, 10.0f, 4000.0f, "%02.0f", "RADIUS", &Light_Radius, mx, my, but, true);
  
  if( Keyboard->IsPressed( kbsLeftShift ) )
  {
      Tool.setSensitivity(.6f);
  }
  else
  {
      Tool.setSensitivity(2.1f);
  }

  if (LightObject)
  {
   obj_LampBulb *L = (obj_LampBulb *)LightObject;

   L->Color.R = Light_R;
   L->Color.G = Light_G;
   L->Color.B = Light_B;   
   L->Radius = Light_Radius;

	L->LightBBox.Org = L->Position;
	L->LightBBox.Org -= r3dPoint3D(Light_Radius, Light_Radius, Light_Radius);
	L->LightBBox.Size = r3dPoint3D(Light_Radius*2, Light_Radius*2, Light_Radius*2);

	Tool.setPosition( L->Position);
	Tool.Picked(mx, my, but & Mouse->mbLeftButton);

	L->SetPosition(Tool.getPosition());
    Tool.Draw();
//	L->LT.X = L->Position.X;
//	L->LT.Y = L->Position.Y;
//	L->LT.Z = L->Position.Z;

	if (Keyboard->WasPressed(kbsDel)) { L->bKilled = 1; LightObject = NULL; }

  }
  
  return;
}

#pragma warning(pop)
