#include "r3dPCH.h"
#include "r3d.h"
#include "d3dfont.h"
#include "d3dfont.h"

#include "UI\Hud_PhysicsEditor.h"

#include "GameCommon.h"

// on-cursor selection
r3dPoint3D	UI_Pos;

static int enablemouselook = 1;
static int firstrun = 1;


void PhysicsHUD::InitPure()
{
	UI_Pos= r3dPoint3D(0, 0, 0);
	FPS_Position.Assign(0,20,-20);

	FPS_Acceleration.Assign(0,0,0);
	FPS_vViewOrig.Assign(0,-25,0);
	FPS_ViewAngle.Assign(0,0,0);
	FPS_vVision.Assign(0, 0, 1);

	PhysicsObject = 0;
}

void PhysicsHUD::DestroyPure()
{
}

void PhysicsHUD::SetCameraDir(r3dPoint3D vPos)
{
	FPS_vVision = vPos;
	FPS_vVision.Normalize();
}

r3dPoint3D PhysicsHUD::GetCameraDir () const
{
	return FPS_vVision;
}

void PhysicsHUD::SetCameraPure(r3dCamera &Cam)
{
	r3dPoint3D CamPos = FPS_Position;
	r3dPoint3D ViewPos = CamPos + FPS_vVision*10.0f;

	Cam.FOV = 60;
	Cam.SetPosition( CamPos );
	Cam.PointTo(ViewPos);
}  

void PhysicsHUD::Draw()
{
	assert(bInited);
	if ( !bInited ) return;

	r3dSetFiltering( R3D_POINT );

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	FALSE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF,        	1 );

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	if(!enablemouselook)
	{
		ProcessPhysicsEditor();
	}

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	FALSE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF,        	1 );
}

float impulse_power = 1.0f;
bool mouse_over_object = false;
extern bool gPhysics_DisableCacheForEditor;
void PhysicsHUD::Process()
{
	gPhysics_DisableCacheForEditor = true;

	FPS_Acceleration.Assign(0, 0, 0);

	float	glb_MouseSens    = 0.5f;	// in range (0.1 - 1.0)
	float  glb_MouseSensAdj = 1.0f;	// in range (0.1 - 1.0)

	enablemouselook = (Mouse->IsPressed(r3dMouse::mRightButton)) && !g_imgui_LockRbr;

	if(Mouse->IsPressed(r3dMouse::mLeftButton))
	{
		if(PhysicsObject && PhysicsObject->PhysicsObject && mouse_over_object)
		{
			r3dPoint3D impulse = (UI_Pos-gCam).NormalizeTo()*impulse_power;
			PhysicsObject->PhysicsObject->AddImpulseAtPos(impulse, UI_Pos);
		}
	}

	int mMX=Mouse->m_MouseMoveX, mMY=Mouse->m_MouseMoveY;

	if (!enablemouselook)
		ProcessPick();

	if(!enablemouselook && !firstrun) 
		return;
	
	firstrun = 0;

	FPS_vViewOrig.x += float(-mMX) * glb_MouseSensAdj;
	FPS_vViewOrig.y += float(-mMY) * glb_MouseSensAdj;

	// Player can't look too high!
	if(FPS_vViewOrig.y > 85)  FPS_vViewOrig.y = 85;
	if(FPS_vViewOrig.y < -85) FPS_vViewOrig.y = -85;

	FPS_ViewAngle = FPS_vViewOrig; // + FPS_vViewDistortion;

	if(FPS_ViewAngle.y > 360 ) FPS_ViewAngle.y = FPS_ViewAngle.y - 360;
	if(FPS_ViewAngle.y < 0 )   FPS_ViewAngle.y = FPS_ViewAngle.y + 360;

	D3DXMATRIX mr;

	D3DXMatrixIdentity(&mr);
	D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(-FPS_ViewAngle.x), R3D_DEG2RAD(-FPS_ViewAngle.y), 0);

	FPS_vVision  = r3dVector(mr._31, mr._32, mr._33);

	D3DXMatrixIdentity(&mr);
	D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(-FPS_ViewAngle.x), 0, 0);
	FPS_vRight = r3dVector(mr._11, mr._12, mr._13);
	FPS_vUp    = r3dVector(0, 1, 0);
	FPS_vForw  = r3dVector(mr._31, mr._32, mr._33);

	FPS_vForw.Normalize();
	FPS_vRight.Normalize();
	FPS_vVision.Normalize();

	// walk
	float fSpeed = 5.0f; //_ai_fWalkSpeed;

	if(Keyboard->IsPressed(kbsW)) FPS_Acceleration.Z = fSpeed;
	if(Keyboard->IsPressed(kbsS)) FPS_Acceleration.Z = -fSpeed * 0.7f;
	if(Keyboard->IsPressed(kbsA)) FPS_Acceleration.X = -fSpeed * 0.7f;
	if(Keyboard->IsPressed(kbsD)) FPS_Acceleration.X = fSpeed * 0.7f;
	if(Keyboard->IsPressed(kbsQ)) FPS_Position.Y    += SRV_WORLD_SCALE(1.0f)* r3dGetFrameTime();
	if(Keyboard->IsPressed(kbsE)) FPS_Position.Y    -= SRV_WORLD_SCALE(1.0f)* r3dGetFrameTime();

	float mult = 1;
	if(Keyboard->IsPressed(kbsLeftShift)) mult = 64.0f;

	FPS_Position += FPS_vVision * FPS_Acceleration.Z * r3dGetFrameTime() * mult;
	FPS_Position += FPS_vRight * FPS_Acceleration.X * r3dGetFrameTime() *mult;

	return;
}

void PhysicsHUD::ProcessPick()
{
	int mx, my;
	Mouse->GetXY(mx,my);

	r3dPoint3D dir;
	r3dScreenTo3D((float)mx, (float)my, &dir);

	CollisionInfo CL;
	const GameObject* target = GameWorld().CastRay(gCam, dir, 2000.0f, &CL); //bp, do bbox check so that clicks are better

	mouse_over_object = (target == PhysicsObject);
	if(target)
	{
		UI_Pos   = CL.NewPosition;
	} 
	else 
	{
		extern BOOL terra_FindIntersection(r3dPoint3D &vFrom, r3dPoint3D &vTo, r3dPoint3D &colpos, int iterations);
		r3dPoint3D v3;
		if(terra_FindIntersection(gCam, gCam + dir*2000, v3, 2000)) 
		{
			UI_Pos = v3;
		}
	}

	return;
}


int				PhysObjCatInit = 0;
int				PhysNumCategories = 0;

struct  PhysCategoryStruct
{
	int			Type;
	float		Offset;
	int			NumObjects;
	stringlist_t	ObjectsClasses;
	clrstringlist_t	ObjectsNames;
};

typedef r3dgameVector(PhysCategoryStruct)  catlist;
catlist  PhysObjectCategories;
stringlist_t 	PhysCatNames;
float			PhysCatOffset = 0.0f;

void PhysFillCatFromDir( PhysCategoryStruct& cat, const char* clazz, const char* path )
{
	char TempS[ 256 ];
	char physicsFile[256];
	sprintf ( TempS,"Data\\ObjectsDepot\\%s\\*.sco", path ); // ptumik: parse only text meshes
	WIN32_FIND_DATA ffblk;
	HANDLE h;
	cat.NumObjects = 0;

	h = FindFirstFile(TempS, &ffblk);
	if(h != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if (ffblk.cFileName[0] != '.')
			{
				if(strstr(ffblk.cFileName, "_collision.sco") || strstr(ffblk.cFileName, "_Collision.sco"))
					continue;

				cat.NumObjects ++;
				ColorStringList elem;
				elem.str = ffblk.cFileName;
				
				memset(physicsFile, 0, sizeof(physicsFile));
				sprintf(physicsFile, "Data\\ObjectsDepot\\%s\\%s", path, ffblk.cFileName);
				int len = strlen(physicsFile);
				r3dscpy(&physicsFile[len-3], "phx");
				if(r3d_access(physicsFile, 0) == 0) // exist
					elem.color = r3dColor(0,255,0);
				else
					elem.color = r3dColor(255,0,0);

				cat.ObjectsNames.push_back(elem);
				cat.ObjectsClasses.push_back( clazz );
			}
		} while(FindNextFile(h, &ffblk) != 0);
		FindClose(h);
	}
}

void PhysIntroduceCat( PhysCategoryStruct& cat, const char* name )
{
	PhysCatNames.push_back( name );
	PhysCatOffset = 0;
	cat.ObjectsNames.clear();
	cat.ObjectsClasses.clear();
	cat.Offset = 0;
	cat.Type = 0;
}

void PhysInitCategories()
{
	if(PhysObjCatInit) return;

	PhysObjCatInit = 1;

	PhysObjectCategories.clear();
	PhysCatNames.clear();


	Script_c script;

	script.OpenFile( "Data\\ObjectsDepot\\DepotConfig.cfg" );


	char buffer[ 512 ];
	//fgets( buffer, sizeof buffer, f );

	int dummy( 0 );

	// handle old format (num fields goes first)
/*	if( !sscanf( buffer, "%d", &dummy ) || strlen( buffer ) - strspn( buffer, "0123456789" ) > 2 )
	{
		// go back in case that's a new format without leading count
		fseek( f, 0, SEEK_SET );
	}*/

	PhysNumCategories = 0;

	typedef r3dgameSet( r3dSTLString ) CatSet;

	CatSet catSet;

	while( ! script.EndOfFile() )
	{
		char S1[64], S2[64];
		PhysCategoryStruct cat;

		buffer[ 0 ] = 0;

		script.GetToken( S1 );
		if ( ! *S1 )
			continue;
		script.GetToken( S2 );

		if (!strcmp(S2,"OBJLIST"))
		{
		}
		else
		{
			PhysNumCategories++;
			char loweredCat[64];
			r3dscpy( loweredCat, S1 );
			strlwr( loweredCat );

			r3d_assert( catSet.find( loweredCat ) == catSet.end() );

			catSet.insert( loweredCat );

			PhysIntroduceCat( cat, S1 );
			PhysFillCatFromDir( cat, S2, S1 );
			PhysObjectCategories.push_back(cat);
		}
	}

	// parse remaining dirs
	{
		CatSet unspecifiedCatSet;

		WIN32_FIND_DATA ffblk;
		HANDLE h = FindFirstFile("Data\\ObjectsDepot\\*.*", &ffblk);
		if(h != INVALID_HANDLE_VALUE)
		{
			do 
			{
				char lowered[256];

				r3dscpy( lowered, ffblk.cFileName );
				strlwr( lowered );

				if( ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && 
					strcmp( ffblk.cFileName, "." )!=0 &&
					strcmp( ffblk.cFileName, ".." )!=0 && 
					!( ffblk.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) && 
					catSet.find( lowered ) == catSet.end() )
				{
					unspecifiedCatSet.insert( ffblk.cFileName );
				}

			} while(FindNextFile(h, &ffblk) != 0);
			FindClose(h);
		}

		for( CatSet::const_iterator i = unspecifiedCatSet.begin(), e = unspecifiedCatSet.end(); i != e; ++i )
		{
			PhysCategoryStruct cat;
			const char* name = (*i).c_str();
			PhysIntroduceCat( cat, name );
			PhysFillCatFromDir( cat, "obj_Building", name );
			PhysObjectCategories.push_back(cat);
		}
	}
}

void PhysicsHUD::ProcessPhysicsEditor()
{
#ifndef FINAL_BUILD
	d_physx_debug->SetInt(1);
#endif

	imgui_Update();
	extern void imgui2_Update();
	imgui2_Update();

	static char CategoryName[64] = "";
	static char ClassName[64] = "";
	static char FileName[64] = "";

	PhysInitCategories();

	static int idx = -1;
	static int idx1 = 0;
	static int current_obj_idx = 0;
	static int current_obj_idx1 = 0;
	bool changed_object = false;

	
	// CODE FOR AUTO UPDATING ALL PHYSICS MESH FILES :)
	/*static int delay = 0;
	delay++;
	if(delay>5 && idx != -1)
	{
		delay = 0;
		idx1++;

		if(idx1>=(int)PhysObjectCategories.at(idx).ObjectsNames.size())
		{
			idx1 = 0;
		}

		sprintf (ClassName,"%s", PhysObjectCategories.at(idx).ObjectsClasses.at(idx1).c_str());
		sprintf (FileName,"%s", PhysObjectCategories.at(idx).ObjectsNames.at(idx1).str.c_str());
		if(ClassName[0] != 0 && FileName[0] != 0)
		{
			current_obj_idx = idx;
			current_obj_idx1 = idx1;
			changed_object = true;
		}
	}*/

	int SliderX = 10;
	int SliderY = 100;
	if (imgui_DrawList((float)SliderX, (float)SliderY, 360, 200, PhysCatNames, &PhysCatOffset, &idx, true, false, true))
	{
		idx1 = 0;
		sprintf(CategoryName, "%s", PhysCatNames.at(idx).c_str());

		sprintf(ClassName,"");
		sprintf(FileName, "");
		if(PhysObjectCategories.at(idx).ObjectsClasses.size() > 0) 
		{
			sprintf(ClassName, "%s", PhysObjectCategories.at(idx).ObjectsClasses.at(0).c_str());
			sprintf(FileName, "%s", PhysObjectCategories.at(idx).ObjectsNames.at(0).str.c_str());
		}
	}

	SliderY += 210;

	SliderY += imgui_Static((float)SliderX, (float)SliderY, FileName);

	char Str[256];

	if (idx != -1)
	{
		if (imgui_DrawListColored((float)SliderX, (float)SliderY, 360, r3dRenderer->ScreenH-40-SliderY, PhysObjectCategories.at(idx).ObjectsNames , &PhysObjectCategories.at(idx).Offset, &idx1, true, true))
		{
			sprintf (ClassName,"%s", PhysObjectCategories.at(idx).ObjectsClasses.at(idx1).c_str());
			sprintf (FileName,"%s", PhysObjectCategories.at(idx).ObjectsNames.at(idx1).str.c_str());
			if(ClassName[0] != 0 && FileName[0] != 0)
			{
				current_obj_idx = idx;
				current_obj_idx1 = idx1;
				changed_object = true;
			}
		}
		if (PhysObjectCategories.at(idx).Type)
			sprintf (Str,"%s", FileName);
		else
			sprintf (Str,"Data\\ObjectsDepot\\%s\\%s", CategoryName, FileName);
	}

	

	if (changed_object)
	{
		if (PhysicsObject )  
			GameWorld().DeleteObject(PhysicsObject);
		PhysicsObject=srv_CreateGameObject(ClassName, Str, r3dPoint3D(0,0,0));
	}

	imgui_Value_Slider(400, 650, "Impulse power", &impulse_power, 1.0f, 10000.0f, "%.1f", true, false);
	if(imgui_Button(800, 650, 100, 20, "Reset object", false, false))
	{
		if(PhysicsObject)
		{
			PhysicsObject->SetPosition(r3dPoint3D(0,0,0));
			PhysicsObject->SetRotationVector(r3dVector(0,0,0));
			PhysicsObject->SetVelocity(r3dPoint3D(0,0,0));
		}

	}

	// object editing
	if(PhysicsObject && PhysicsObject->GetObjectMesh())
	{
		r3dMesh* mesh = PhysicsObject->GetObjectMesh();
		float scrx = 900.0f;
		float starty = 100.0f;

		int tempy;
		stringlist_t TypeNames;
		TypeNames.push_back( "Unknown" );
		TypeNames.push_back( "Box" );
		TypeNames.push_back( "Sphere" );
		TypeNames.push_back( "Mesh" );
		TypeNames.push_back( "Convex" );
		//TypeNames.push_back( "Controller" );

		bool hasCookedMesh = g_pPhysicsWorld->HasCookedMesh(mesh);
		bool hasConvexMesh = g_pPhysicsWorld->HasConvexMesh(mesh);
		if(imgui_Button(scrx, starty, 160, 30, hasCookedMesh?"Re-cook mesh":"Cook mesh"))
		{
			g_pPhysicsWorld->CookMesh(mesh);
			PhysicsObject->RecreatePhysicsObject = true;
		}
		if(imgui_Button(scrx+160.0f, starty, 160, 30, hasConvexMesh?"Re-cook convex":"Cook convex"))
		{
			g_pPhysicsWorld->CookConvexMesh(mesh);
			PhysicsObject->RecreatePhysicsObject = true;
		}
		starty += 30.0f;

		tempy = imgui_Static(scrx, starty, "Type:", 100);
		static float typeOffset;
		if ( imgui_DrawList( scrx + 100.0f, starty, 200, 120, TypeNames, &typeOffset, (int*)&PhysicsObject->PhysicsConfig.type, true, false, true ) )
			PhysicsObject->RecreatePhysicsObject = true;
		starty += 125.0f;
		
		int prevIsDynamic = PhysicsObject->PhysicsConfig.isDynamic;
		int isDynamic = PhysicsObject->PhysicsConfig.isDynamic;
		starty += imgui_Checkbox(scrx, starty, "Dynamic", &isDynamic, 1);
		PhysicsObject->PhysicsConfig.isDynamic = isDynamic!=0?true:false;
		if(prevIsDynamic != isDynamic)
			PhysicsObject->RecreatePhysicsObject = true;

		/*if(PhysicsObject->PhysicsConfig.isDynamic)
			g_pPhysicsWorld->CookMesh(mesh);*/


		int prevexplMesh = PhysicsObject->PhysicsConfig.needExplicitCollisionMesh;
		int explMesh = PhysicsObject->PhysicsConfig.needExplicitCollisionMesh;
		starty += imgui_Checkbox(scrx, starty, "Explicit collision mesh", &explMesh, 1);
		PhysicsObject->PhysicsConfig.needExplicitCollisionMesh = explMesh!=0?true:false;
		if(prevexplMesh != explMesh)
			PhysicsObject->RecreatePhysicsObject = true;

		int prevIsFastMoving = PhysicsObject->PhysicsConfig.isFastMoving;
		int isFastMoving = PhysicsObject->PhysicsConfig.isFastMoving;
		starty += imgui_Checkbox(scrx, starty, "Is fast moving or very small", &isFastMoving, 1);
		PhysicsObject->PhysicsConfig.isFastMoving = isFastMoving!=0?true:false;
		if(prevIsFastMoving != isFastMoving)
			PhysicsObject->RecreatePhysicsObject = true;


		float prev_mass = PhysicsObject->PhysicsConfig.mass;
		starty += imgui_Value_Slider(scrx, starty, "Mass", &PhysicsObject->PhysicsConfig.mass, 0.1f, 10000.0f, "%.2f");
		if(prev_mass != PhysicsObject->PhysicsConfig.mass)
			PhysicsObject->RecreatePhysicsObject = true;
		
		if(PhysicsObject->RecreatePhysicsObject && PhysicsObject->PhysicsConfig.type != PHYSICS_TYPE_UNKNOWN)
		{
			// correct params
			if(PhysicsObject->PhysicsConfig.type == PHYSICS_TYPE_MESH && PhysicsObject->PhysicsConfig.isDynamic)
				PhysicsObject->PhysicsConfig.isDynamic = 0; // mesh cannot be dynamic
			if(imgui_Button(scrx+80.0f, starty, 160, 30, "Create Physics Object"))
			{
				if(PhysicsObject->PhysicsObject) 
					delete PhysicsObject->PhysicsObject; 
				PhysicsObject->PhysicsObject = 0;
				
                if(PhysicsObject->PhysicsConfig.meshFilename)
                    free(PhysicsObject->PhysicsConfig.meshFilename);

				if(PhysicsObject->PhysicsConfig.destroyedMeshFilename)
				{
					free(PhysicsObject->PhysicsConfig.destroyedMeshFilename);
					PhysicsObject->PhysicsConfig.destroyedMeshFilename = NULL;
				}

                PhysicsObject->PhysicsConfig.meshFilename = strdup(PhysicsObject->FileName.c_str());


				if(PhysicsObject->PhysicsConfig.isDynamic || PhysicsObject->PhysicsConfig.isKinematic)
					PhysicsObject->PhysicsObject = BasePhysicsObject::CreateDynamicObject(PhysicsObject->PhysicsConfig, PhysicsObject);
				else
					PhysicsObject->PhysicsObject = BasePhysicsObject::CreateStaticObject(PhysicsObject->PhysicsConfig, PhysicsObject);
				
				PhysicsObject->RecreatePhysicsObject = false;
			}
			starty += 35.0f;
		}
		if(imgui_Button(scrx+80.0f, starty, 160, 30, "Save physics data"))
		{
			PhysicsObject->SavePhysicsData();
			PhysObjectCategories.at(current_obj_idx).ObjectsNames.at(current_obj_idx1).color = r3dColor(0,255,0);
		}
		starty += 30.0f;
		starty += 10.0f;

		/*if(PhysicsObject && PhysicsObject->PhysicsObject)
		{
			PxActor* actor = PhysicsObject->PhysicsObject->getPhysicsActor();
			if(actor)
			{
				PxBounds3 pxBbox = actor->getWorldBounds();
				PxVec3 pxCenter = pxBbox.getCenter();

				r3dDrawUniformSphere(r3dVector(pxCenter.x, pxCenter.y, pxCenter.z), 0.025f, gCam, r3dColor::red);
				r3dDrawUniformSphere(r3dVector(pxCenter.x, pxBbox.minimum.y, pxCenter.z), 0.0125f, gCam, r3dColor::red);

				r3dDrawUniformSphere(PhysicsObject->GetPosition(), 0.025f, gCam, r3dColor::green);

				//r3dDrawSphereSolid(r3dVector(pxCenter.x, pxCenter.y, pxCenter.z), 0.05f, gCam, r3dColor::red);
			}
		}*/

	}
}

