#include "r3dPCH.h"
#ifndef FINAL_BUILD

#include "r3d.h"
#include "r3dLight.h"
#include "d3dfont.h"

#include "GameCommon.h"
#include "RoadEditor.h"
#include "ObjectsCode/world/obj_Road.h"

#include "ObjectManipulator3d.h"
#include "r3dBackgroundTaskDispatcher.h"

extern float GetRoadCtrlScreenSize ( r3dPoint3D vPos, r3dVector vVec, r3dVector vOffset );

//-------------------------------------------------------------------------
//	Undo "Allow spline change" flag with corresponding road-to-ground snapping
//-------------------------------------------------------------------------

class AllowSplineEditChange: public IUndoItem
{
	static const UndoAction_e ms_eActionID = UA_ALLOW_ROAD_SPLINE_EDIT;
	obj_Road::Vertices vertices_;
	obj_Road::Indices indices_;
	obj_Road *owner_;

public:

	void				Release			()							{ PURE_DELETE(this); };
	UndoAction_e		GetActionID		()							{ return ms_eActionID; };

	void				Undo			();
	void				Redo			();
	void				SetData			(obj_Road *r);

	AllowSplineEditChange();
	~AllowSplineEditChange();

	static IUndoItem * CreateUndoItem()
	{
		return game_new AllowSplineEditChange;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
		g_pUndoHistory->RegisterUndoAction(action);
	}
};

//////////////////////////////////////////////////////////////////////////

void gDoRoadEditor(float x, float y)
{
  static Editor_Road ed;
  ed.ProcessTab(x, y);
}

void r3dDrawLine3DT(const r3dPoint3D& p1, const r3dPoint3D& p2, r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex)
{
  const r3dPoint3D dir  = (p2 - p1).NormalizeTo();
  const float      len  = (p2 - p1).Length();
  const float      step = 3.0f;

  r3dPoint3D v1 = p1;
  r3dPoint3D v2 = p1;
  v1.Y = terra_GetH(v1) + 0.5f;

  for(float dist = step; dist < len; dist += step) {
    v2 = p1 + dir * dist;
    v2.Y = terra_GetH(v2) + 0.5f;
    r3dDrawLine3D(v1, v2, Cam, Width, clr, Tex);
    v1 = v2;
  }
  
  // last step
  v2 = p1 + dir * len;
  v2.Y = terra_GetH(v2) + 0.5f;
  r3dDrawLine3D(v1, v2, Cam, Width, clr, Tex);

  return;
}
void r3dDrawUniformLine3DT(const r3dPoint3D& p1, const r3dPoint3D& p2, r3dCamera &Cam, const r3dColor24& clr)
{
	const r3dPoint3D dir  = (p2 - p1).NormalizeTo();
	const float      len  = (p2 - p1).Length();
	const float      step = 3.0f;

	r3dPoint3D v1 = p1;
	r3dPoint3D v2 = p1;
	v1.Y = terra_GetH(v1) + 0.5f;

	for(float dist = step; dist < len; dist += step)
	{
		v2 = p1 + dir * dist;
		v2.Y = terra_GetH(v2) + 0.5f;
		r3dDrawUniformLine3D(v1, v2, Cam, clr);
		v1 = v2;
	}

	// last step
	v2 = p1 + dir * len;
	v2.Y = terra_GetH(v2) + 0.5f;
	r3dDrawUniformLine3D(v1, v2, Cam, clr);
}

//////////////////////////////////////////////////////////////////////////

Editor_Road::Editor_Road()
{
  curTab_     = TAB_SELECT;
  roadAdding_ = 0;
  pointMode_  = 0;
  addDir_     = 1;
  
  isDragging_ = false;
  dragPoint_  = -1;
  
  drawRoadNormal = 0;
  drawRoadConfig = 1;
  
  lastRegenTime_ = 0;
  regenDelay_    = 0.5f;

#ifndef WO_SERVER
  AllowSplineEditChange::Register();
#endif // WO_SERVER
}

Editor_Road::~Editor_Road()
{
}

static r3dPoint3D GetUITargetPos()
{
	extern	r3dPoint3D	UI_TerraTargetPos;		
	extern	r3dPoint3D	UI_TargetPos; // snapped to object position (object center)

	return Terrain ? UI_TerraTargetPos : UI_TargetPos;
}

void Editor_Road::ProcessSelect()
{
	r3dPoint3D targetPos = GetUITargetPos();

	if ( g_Manipulator3d.IsLocked() )
		g_Manipulator3d.Unlock ();
  // adding
  if(imgui_Button(SliderX, SliderY, 360, 25, "Add New Road", roadAdding_)) {
    if(roadAdding_) roadAdding_ = 0;
    else            roadAdding_ = 1;
    
    imgui_lbr = 0;
    imgui_lbp = 0;
  }
  SliderY += 30;
  SliderY += 30;

  StartDrawHelpers();

  switch(roadAdding_)
  {
    default: r3d_assert(0);
    case 0:
      break;

    case 1:
    {
		float fScaleCoeff = GetRoadCtrlScreenSize( targetPos, r3dVector ( 0.0f, 0.0f, 0.0f ), r3dVector ( 1.0f, 0.0f, 0.0f ) );

      r3dDrawUniformCircle3D(targetPos, fScaleCoeff * fCircleRadius, gCam, r3dColor(255, 0, 0));
      if(imgui_lbr) {
        roadPnt_[0] = targetPos;
        roadAdding_++;
      }
      break;
    }

    case 2:
    {
      roadPnt_[1] = targetPos;
	  
		float fScaleCoeff = GetRoadCtrlScreenSize( roadPnt_[0], r3dVector ( 0.0f, 0.0f, 0.0f ), r3dVector ( 1.0f, 0.0f, 0.0f ) );
      r3dDrawUniformSphere(roadPnt_[0], fScaleCoeff * fCircleRadius, gCam, r3dColor(255, 0, 255));
		fScaleCoeff = GetRoadCtrlScreenSize( roadPnt_[1], r3dVector ( 0.0f, 0.0f, 0.0f ), r3dVector ( 1.0f, 0.0f, 0.0f ) );
      r3dDrawUniformCircle3D(roadPnt_[1], fScaleCoeff * fCircleRadius, gCam, r3dColor(255, 0, 0));
      r3dDrawUniformLine3DT(roadPnt_[0], roadPnt_[1], gCam, r3dColor(0, 255, 0));
      
      if(imgui_lbr) {
        char name[256];
        sprintf(name, "road_%dx%d", int(roadPnt_[0].x), int(roadPnt_[0].z));
        obj_Road* road = (obj_Road*)srv_CreateGameObject("obj_Road", name, roadPnt_[0]);

        road->edCreate(r3dPoint3D(0,0,0), roadPnt_[1] - roadPnt_[0]);
		g_Manipulator3d.PickedObjectSet ( road );

        roadAdding_ = 0;

        // switch to adding mode
        curTab_     = TAB_EDIT;
        pointMode_  = POINT_ADD;
        addDir_     = 1;
        roadId_     = road->ID;
		road->autoRegenFlag_ = 1;
      }
      break;
    }
  }

  StopDrawHelpers();

  if(roadAdding_)
    return;

  if ( g_Manipulator3d.PickedObject() )
  {
	  g_Manipulator3d.Lock ();
	  roadId_    = g_Manipulator3d.PickedObject()->ID;
	  curTab_    = TAB_EDIT;
	  pointMode_ = POINT_MODIFY;
	  curPoint_  = 0;
	  return;
  }

  for(GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj)) 
  {
    if(!(obj->Class->Name == "obj_Road")) {
      continue;
    }
    
    bool selected = false;

    if(imgui_Button(SliderX, SliderY, 360, 25, obj->Name.c_str(), 0)) {
      selected = true;
    }
    SliderY += 30;
    
    /*
    obj_Road* road = (obj_Road*)obj;
    int idx = road->edGetClosestPoint(UI_TargetPos, NULL);
    if(idx != -1 && imgui_lbp) {
      selected = true;
    }
    */
    
    if(selected) {
	  g_Manipulator3d.PickedObjectSet ( obj );
	  g_Manipulator3d.Lock ();
      roadId_    = obj->ID;
      curTab_    = TAB_EDIT;
      pointMode_ = POINT_MODIFY;
      curPoint_  = 0;
      break;
    }
  }

  return;
}

void Editor_Road::ProcessEditMaterials()
{
	g_pDesktopManager->End();

	obj_Road* road = (obj_Road*)GameWorld().GetObject(roadId_);
	r3d_assert(road);

	if(r3dGetTime() > lastRegenTime_ + regenDelay_) {
		lastRegenTime_ = r3dGetTime();
		if (road->autoRegenFlag_)
			road->createInternalBuffers();
			road->regenerateMesh();
	}

	float fX = 5;
	float fY = 55;

	static char szNewMatName[ R3D_MAX_MATERIAL_NAME ] = { 0 };

	fY += imgui_Static(fX, 55, "Materials", 300, true, 25, true );
	char MatFName[MAX_PATH];
	char mask[MAX_PATH];
	sprintf(mask, "%s\\*.mat", obj_Road::BaseMaterialDir);
	sprintf( MatFName, "Current Material: %s", road->matName );
	SliderY += imgui_Static( SliderX, SliderY, MatFName );
	sprintf( MatFName, "%s.mat", road->matName );

	static float offset = 0.f ;

	if ( imgui_FileList(fX, fY, 300,380, mask, MatFName, &offset ) )
	{
		char* p;
		if((p = strrchr(MatFName, '.')) != 0) *p = 0;
		if ( strcmp( MatFName, road->matName )!=0 )
		{
			road->SetMaterial( MatFName );
			r3dscpy( szNewMatName, MatFName );
		}
	}

	fY += 380;

	imgui2_StringValueEx( fX, fY, 300, "New Mat Name:", szNewMatName );

	fY += 25;

	if ( imgui_Button(fX, fY, 300, 20, "Add New Material") )
	{
		if ( *szNewMatName != '\0' )
		{
			char matname[MAX_PATH];
			sprintf( matname, "%s\\%s.mat", obj_Road::BaseMaterialDir, szNewMatName );
			if ( !r3dFileExists( matname ) )
			{
				FILE* f = fopen( matname, "wt" );
				if (f != NULL)
				{
					r3dMaterial mat( 0 );
					r3dscpy( mat.Name, szNewMatName );
					r3dscpy( mat.DepotName, "_Roads" );
					mat.SaveAscii(f);
					fclose(f);
					extern void imgui_FileListReset();
					imgui_FileListReset();
				}
				else
					r3dOutToLog("RoadEditor: can't open %s for writing\n", matname);
			}
		}
	}

	g_pDesktopManager->Begin( "ed_roads" );

	return;
}

void Editor_Road::ProcessEdit()
{
  r3dPoint3D targetPos = GetUITargetPos();

  obj_Road* road = (obj_Road*)GameWorld().GetObject(roadId_);
  if(!road) {
    roadId_  = invalidGameObjectID;
    curTab_  = TAB_SELECT;
    return;
  }

  if ( g_Manipulator3d.PickedObject() != road )
  {
	  g_Manipulator3d.PickerResetPicked ();
	  g_Manipulator3d.PickerAddToPicked ( road );
  }  

  if ( !g_Manipulator3d.IsLocked() )
	  g_Manipulator3d.Lock();

  if ( Keyboard->WasPressed(kbsDel) )
  {
	  g_Manipulator3d.DeletePickedObjects();
	  return;
  }

  //r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC);

  StartDrawHelpers();

  R3DPROFILE_START("edDrawRoadConfig");
  if(drawRoadConfig) road->edDrawRoadConfig();
  R3DPROFILE_END("edDrawRoadConfig");
  R3DPROFILE_START("edDrawRoadNormals");
  if(drawRoadNormal) road->edDrawRoadNormals();
  R3DPROFILE_END("edDrawRoadNormals");

  StopDrawHelpers();

  //r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);


  imgui2_StartArea("area222", SliderX, SliderY, 360, 22);
  char road_name[128];
  r3dscpy(road_name, road->Name.c_str());
  imgui2_StringValue("Name:", road_name);
  road->Name = road_name;
  SliderY += 22;
  imgui2_EndArea();
  SliderY += 30;

#if 0
  static float roadOffsetX = 0.f;
  static float roadOffsetZ = 0.f;

  SliderY += imgui_Value_Slider( SliderX, SliderY, "Offset Road X", &roadOffsetX, -8192.f, 8192.f, "%.2f" );
  SliderY += imgui_Value_Slider( SliderX, SliderY, "Offset Road Z", &roadOffsetZ, -8192.f, 8192.f, "%.2f" );

  if( imgui_Button( SliderX, SliderY, 360.f, 22.f, "Offset!" ) )
  {
	if( MessageBoxA( r3dRenderer->HLibWin, "This operation can not be undone! Proceed?", "Question", MB_YESNO ) == IDYES )
	{
		for( int i = 0, e = (int)road->orgPoints_.size(); i < e; i ++ )
		{
			road->orgPoints_[ i ].p.x -= roadOffsetX;
			road->orgPoints_[ i ].p.z -= roadOffsetZ;
		}
		r3dPoint3D roadPos = road->GetPosition();

		road->SetPosition( r3dPoint3D( roadPos.x + roadOffsetX, roadPos.y, roadPos.z + roadOffsetZ ) );

		road->createInternalBuffers();
		road->regenerateMesh();
	}
  }

  SliderY += 33.f;
#endif

  extern int _road_VSegPerRoad;
  extern int _road_UseWireframe;
  extern int _road_ShowZFail;
  extern int _road_FixZInShader;
  DWORD oldAutoRegenFlag_ = road->autoRegenFlag_;
  if (road->autoRegenFlag_)
  {
	  SliderY += imgui_Value_SliderI(SliderX, SliderY, "SegPerV(global)", &_road_VSegPerRoad, 2, 10,  "%d", 1);
	  SliderY += imgui_Value_Slider(SliderX, SliderY, "autoRegenTime", &regenDelay_, 0.01f, 2,  "%02.2f", 1);
  }
  SliderY += imgui_Checkbox(SliderX, SliderY, "Allow Spline Editing", reinterpret_cast<int*>(&road->autoRegenFlag_), 0x1);
  drawRoadConfig = road->autoRegenFlag_;
  SliderY += imgui_Checkbox(SliderX, SliderY, "Wireframe", &_road_UseWireframe, 0x1);
  SliderY += imgui_Checkbox(SliderX, SliderY, "RoadNormal", &drawRoadNormal, 0x1);
  SliderY += imgui_Checkbox(SliderX, SliderY, "Show Z Fail", &_road_ShowZFail, 0x1);
  SliderY += imgui_Checkbox(SliderX, SliderY, "Fix Z in Shader", &_road_FixZInShader, 0x1);
  SliderY += 30;

  if (road->autoRegenFlag_)
  {
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Width",  &road->width_,      1, 100, "%02.0f", 1);
	SliderY += imgui_Value_Slider(SliderX, SliderY, "Tiling", &road->texTileLen_, 1, 200, "%02.0f", 1);
	SliderY += 30;
  }

  GameObjects selected ;

  selected.PushBack( road ) ;

  SliderY += road->DrawPropertyEditor( SliderX, SliderY, 360, 330, &obj_Road::ClassData, selected ) ;

  //	Create undo item if needed
  if (oldAutoRegenFlag_ == 0 && road->autoRegenFlag_ == 1)
  {
	  AllowSplineEditChange *ua = reinterpret_cast<AllowSplineEditChange*>(g_pUndoHistory->CreateUndoItem(UA_ALLOW_ROAD_SPLINE_EDIT));
	  ua->SetData(road);
  }
  
  ProcessEditMaterials();
  SliderY += 30;

  if( road->autoRegenFlag_ )
  {
	  const static char* list[] = { "MODIFY", "ADD"};
	  SliderY += imgui_Toolbar(SliderX, SliderY, 360, 35, &pointMode_, 0, list, 2);

	  int maxPoint = (int)road->orgPoints_.size();
	  
	  if(curPoint_ < 0)         curPoint_ = 0;
	  if(curPoint_ >= maxPoint) curPoint_ = maxPoint - 1;
	  
	  switch(pointMode_) 
	  {
		case POINT_MODIFY:
		{
		  char buf[256];
		  sprintf(buf, "Point: %d/%d", curPoint_ + 1, maxPoint);
		  SliderY += imgui_Static(SliderX, SliderY, buf);
		  SliderY += imgui_Value_Slider(SliderX, SliderY, "Height Add", &road->orgPoints_[curPoint_].height, 0, 2,  "%02.2f", 1);
		  SliderY += imgui_Static(SliderX, SliderY, "select control point with left mouse button");
		  SliderY += imgui_Static(SliderX, SliderY, "drag it with CONTROL pressed");

		  // drag & drop
		  if(isDragging_) {
			SliderY += imgui_Static(SliderX, SliderY, "dragging");

			if(!Keyboard->IsPressed(kbsLeftControl)) {
			  isDragging_ = false;
			  dragPoint_  = -1;
			  break;
			}
	        
			if(isNormal_) {
			  road->orgPoints_[curPoint_].n = targetPos - road->orgPoints_[curPoint_].p;
			  road->orgPoints_[curPoint_].n.y = 0;
			} else {
			  road->orgPoints_[curPoint_].p = targetPos;
			  road->orgPoints_[curPoint_].p.y = 0;
			}
			break;
		  }

		  // select new point
		  int selNormal;
		  int selPoint = road->edGetClosestPoint(targetPos, &selNormal);

		  // draw selected point, and select it on left mouse button
		  if(selPoint != -1) {
			const obj_Road::pnt_s& cp = road->orgPoints_[selPoint];
			r3dPoint3D selp = selNormal ? cp.p+cp.n : cp.p;
			selp.Y = terra_GetH(selp);
			float fScaleCoeff = GetRoadCtrlScreenSize( selp, r3dVector ( 0.0f, 0.0f, 0.0f ), r3dVector ( 1.0f, 0.0f, 0.0f ) );

			StartDrawHelpers();
			r3dDrawUniformSphere(selp, fScaleCoeff * fCircleRadius, gCam, r3dColor(0, 255, 255));
			StopDrawHelpers();

			if(imgui_lbr) {
			  curPoint_ = selPoint;
			  isNormal_ = selNormal;
	          
			  road->edSelectedNormal_ = isNormal_;
			  road->edSelectedPnt_    = curPoint_;
			}
	        
			if(Keyboard->IsPressed(kbsLeftControl) && curPoint_ == selPoint) {
			  isDragging_ = true;
			  dragPoint_  = selPoint;
			}
		  }
	      
		  if(imgui_Button(SliderX+50, SliderY, 100, 25, "del", 0)) {
			if(maxPoint > 2) {
			  road->orgPoints_.erase(road->orgPoints_.begin() + curPoint_);
			}
		  }
		  SliderY += 30;

		  if(imgui_Button(SliderX+50, SliderY, 100, 25, "split prev", 0)) {
			if(curPoint_ == 0) {
			  pointMode_ = POINT_ADD;
			  addDir_    = 0;
			} else {
			  road->edSplit(curPoint_, 0);
			}
		  }
		  SliderY += 30;

		  if(imgui_Button(SliderX+50, SliderY, 100, 25, "split next", 0)) {
			if(curPoint_ == maxPoint - 1) {
			  pointMode_ = POINT_ADD;
			  addDir_   = 1;
			} else {
			  road->edSplit(curPoint_, 1);
			}
		  }
		  SliderY += 30;
	      
		  break;
		}

		case POINT_ADD:
		{
		  r3dPoint3D p1 = road->orgPoints_[addDir_ ? maxPoint - 1 : 0].p;
		  r3dPoint3D p2 = targetPos;
		  float fScaleCoeff = GetRoadCtrlScreenSize( targetPos, r3dVector ( 0.0f, 0.0f, 0.0f ), r3dVector ( 1.0f, 0.0f, 0.0f ) );

		  StartDrawHelpers();
		  r3dDrawUniformCircle3D(p2, fScaleCoeff * fCircleRadius, gCam, r3dColor(255, 0, 0));
		  StopDrawHelpers();
	      
		  SliderY += imgui_Static(SliderX, SliderY, "hold CONTROL to add road");
	      
		  if(Keyboard->IsPressed(kbsLeftControl)) {
			p1.y = terra_GetH(p1);

			StartDrawHelpers();
			r3dDrawUniformLine3DT(p1, p2, gCam, r3dColor(0, 255, 0));
			StopDrawHelpers();
	      
			float dist = (p1 - p2).Length();
	      
			if(imgui_lbr && dist > 10.0f) {
			  road->edAddPoint(p2, addDir_);
			}
		  }
	      
		  if(Keyboard->WasPressed(kbsEsc)) {
			pointMode_ = POINT_MODIFY;
		  }
		  break;
		}
	  }
  }

  return;
}

void editor_regenerateAllRoads()
{
  for(GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj)) {
    if(!(obj->Class->Name == "obj_Road")) {
      continue;
    }
    
    obj_Road* road = (obj_Road*)obj;
	road->createInternalBuffers();
    road->regenerateMesh();
  }
  
  return;
}

void Editor_Road::ProcessRegenerate()
{
  if(imgui_Button(SliderX, SliderY, 360, 25, "RecreateAll")) {
    editor_regenerateAllRoads();
    imgui_lbr = 0;
    imgui_lbp = 0;
  }
  SliderY += 30;

  return;
}

static int g_helpers_opened = 0;

void Editor_Road::StartDrawHelpers()
{
	r3d_assert( !g_helpers_opened );
	g_helpers_opened = 1;

	r3dRenderer->Flush();

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );
}

void Editor_Road::StopDrawHelpers()
{
	r3d_assert( g_helpers_opened );
	g_helpers_opened = 0;

	r3dRenderer->Flush();

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ) );
}

void Editor_Road::ProcessTab(float in_SliderX, float in_SliderY)
{
  struct StartFinishTab
  {
    StartFinishTab()
    {
      g_pDesktopManager->Begin( "ed_roads" );
    }
    ~StartFinishTab()
    {
      g_pDesktopManager->End();
    }
  } g_StartAndFinishTab;

  if( Terrain3 )
  {
	  int terraQL = r_terrain_quality->GetInt();

	  r3dTerrain3::QualitySettings sts = Terrain3->GetCurrentQualitySettings();

	  if( sts.BakeRoads )
	  {
		  sts.BakeRoads = 0;
		  Terrain3->SetQualitySettings( sts, true, terraQL - 1 );
		  Terrain3->RefreshAtlasTiles();

		  r3dFinishBackGroundTasks();

		  extern int g_RoadEditingSwtichedOffTerrain3RoadBaking;
		  g_RoadEditingSwtichedOffTerrain3RoadBaking = 1;
	  }
  }

  if( Terrain2 )
  {
	  int terraQL = r_terrain_quality->GetInt() ;

	  r3dTerrain2::QualitySettings sts = Terrain2->GetCurrentQualitySettings() ;

	  if( sts.BakeRoads )
	  {
		  sts.BakeRoads = 0 ;
		  Terrain2->SetQualitySettings( sts, true, terraQL - 1 ) ;
		  Terrain2->RefreshAtlasTiles() ;

		  extern int g_RoadEditingSwtichedOffTerrain2RoadBaking ;
		  g_RoadEditingSwtichedOffTerrain2RoadBaking = 1 ;
	  }
  }

  SliderX = in_SliderX;
  SliderY = in_SliderY;

  r3dRenderer->Flush();
  r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
  
  int iLastTab = curTab_;
  const static char* list[] = { "SELECT", "EDIT" };
  SliderY += imgui_Toolbar(SliderX, SliderY, 360, 35, &curTab_, 0, list, 2);

  ProcessRegenerate();

  if ( iLastTab == TAB_EDIT && curTab_ == TAB_SELECT )
	  g_Manipulator3d.PickerResetPicked();
  
  switch(curTab_)
  {
    default: r3d_assert(0);

    case TAB_SELECT:
      ProcessSelect();
      return;
      
    case TAB_EDIT:
      roadAdding_ = 0;
      ProcessEdit();
      break;
  }

  return;
}

/*
void drawTerrainGrid()
{
  int X = gCam.X / Terrain->GetDesc().CellSize;
  int Y = gCam.Z / Terrain->GetDesc().CellSize;
  int w = Terrain1->Width;
  
  int step = 0;
  int off  = 15;
  for(int y=Y-off; y<Y+off; y++) {
    for(int x=X-off; x<X+off; x++) {
      float h1 = Terrain1->HeightMapData[(y+0)*w+(x+0)] + 0.01f;
      float h2 = Terrain1->HeightMapData[(y+0)*w+(x+1)] + 0.01f;
      float h3 = Terrain1->HeightMapData[(y+1)*w+(x+0)] + 0.01f;
      float h4 = Terrain1->HeightMapData[(y+0)*w+(x+1)] + 0.01f;
      
      r3dPoint3D v1 = r3dPoint3D((x+0)*Terrain1->CellSize, h1, (y+0)*Terrain1->CellSize);
      r3dPoint3D v2 = r3dPoint3D((x+1)*Terrain1->CellSize, h2, (y+0)*Terrain1->CellSize);
      r3dPoint3D v3 = r3dPoint3D((x+0)*Terrain1->CellSize, h3, (y+1)*Terrain1->CellSize);
      r3dPoint3D v4 = r3dPoint3D((x+1)*Terrain1->CellSize, h4, (y+1)*Terrain1->CellSize);
      
      r3dColor clr(255, 255, 255);
      if((++step)%2 == 0) clr = r3dColor(255, 0, 0);
      r3dDrawLine3D(v1, v2, gCam, 0.03f, clr);
      r3dDrawLine3D(v1, v3, gCam, 0.03f, clr);
    }
  }

  return;
}
*/

//-------------------------------------------------------------------------
//	AllowSplineEditChange
//-------------------------------------------------------------------------

void AllowSplineEditChange::Undo()
{
	if (owner_)
	{
		owner_->vtx_ = vertices_;
		owner_->idx_ = indices_;
		owner_->autoRegenFlag_ = 0;
	}
}

//////////////////////////////////////////////////////////////////////////

void AllowSplineEditChange::Redo()
{
	if (owner_)
		owner_->autoRegenFlag_ = 1;
}

//////////////////////////////////////////////////////////////////////////

AllowSplineEditChange::AllowSplineEditChange()
: owner_(0)
{
	m_sTitle = "Allow spline edit";
}

//////////////////////////////////////////////////////////////////////////

AllowSplineEditChange::~AllowSplineEditChange()
{
}

//////////////////////////////////////////////////////////////////////////

void AllowSplineEditChange::SetData(obj_Road *r)
{
	if (!r) return;
	owner_ = r;
	vertices_ = r->vtx_;
	indices_ = r->idx_;
}

#endif // FINAL_BUILD