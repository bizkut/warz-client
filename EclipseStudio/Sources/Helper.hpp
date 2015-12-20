
extern float terra_GetH(const r3dPoint3D &vPos);
extern float GetFloor(const r3dPoint3D& pos, float maxRayLength);

float GetWorldHeight(r3dPoint3D &Pos1)
{
	float TH = terra_GetH(Pos1);
	return TH;
	/*
	CollisionInfo CL;

	r3dPoint3D Pos = Pos1 + r3dPoint3D(0, 15.0f, 0);
	GameObject* Obj = GameWorld().CastMeshRay(Pos, r3dVector(0, -1, 0), 1000, &CL);

	if(CL.Type == CLT_Face) {
	float H = Pos.Y - CL.Distance;
	if(H > TH) return H;
	}
	return TH;
	*/  
}

//------------------------------------------------------------------------

float GetFloor(const r3dPoint3D& pos, float maxRayLength)
{
	CollisionInfo cinfo;

	float h = -FLT_MAX ;

	if( GameObject* gobj = GameWorld().CastRay( pos, r3dPoint3D( 0, -1, 0 ), maxRayLength, &cinfo ) )
	{
		h = R3D_MAX( h, cinfo.NewPosition.y );
	}

	if( Terrain && Terrain->IsLoaded() )
	{
		h = R3D_MAX( h, Terrain->GetHeight( pos ) );
	}

	return h;
}



int IsObstacle(r3dPoint3D Pos, r3dVector V)
{
	CollisionInfo CL;
	//GameObject* Obj = GameWorld().CastRay(Pos, V, 10000, &CL);

	//if ( CL.Type == CLT_Face) return 1;

	return 0;
}

void CastWorldRay()
{
#if 0
	return;

	static int __CASTINC = 0;

	// __CASTINC ++;
	// if (__CASTINC < 30 ) return;
	// __CASTINC = 0;

	r3dPoint3D Pos;
	Pos          = Player->Position;
	Pos.Y		+= Player->Height;
	Pos += Player->vForw *10.0f;

	CollisionInfo CL;
	GameObject* Obj = GameWorld().CastMeshRay(Pos, Player->vForw, 13000, &CL);

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	// if (Obj)  Font_Label->PrintF (0, 40, r3dColor(255,0,0), "%s", Obj->Name);
	// else
	//  Font_Label->PrintF (0, 40, r3dColor(255,0,0), "NO OBJECT");

	if ( CL.Type == CLT_Face)
	{
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

		r3dDrawBox2D(0,0,220,170,r3dColor(0,0,0,100));

		Font_Label->PrintF (0,0, r3dColor(0,0,0), 
			"%s\n"  
			"Dist      %.2f\n" 
			"Verts     %d\n"
			"Faces     %d\n"
			"NumMates  %d",
			CL.pMeshObj->Name, CL.Distance/srv_GetWorldScale(),
			CL.pMeshObj->NumVertices,CL.pMeshObj->NumIndices/3, CL.pMeshObj->NumMatChunks );

		Font_Label->PrintF (0,0, r3dColor(255,100,0), 
			"%s\n"  
			"Dist      %.2f\n" 
			"Verts     %d\n"
			"Faces     %d\n"
			"NumMates  %d\n",
			CL.pMeshObj->Name, CL.Distance/srv_GetWorldScale(),
			CL.pMeshObj->NumVertices,CL.pMeshObj->NumIndices/3, CL.pMeshObj->NumMatChunks);

		r3dRenderer->Flush();


		char FName[256];
		char Str[256];

		sprintf(FName,"%s\\Scene\\CFG\\ObjectCFG.cfg", r3dGameLevel::GetHomeDir());


		// if(Keyboard->IsPressed(kbsF6)) pl->SetCurrentWeapon(5);
		// if(Keyboard->IsPressed(kbsF7)) pl->SetCurrentWeapon(6);


		float Shift = 0.5f;

		r3dVector V = r3dReadCFG_V(FName, CL.pMeshObj->Name, "Move", r3dVector(0,0,0));
		int bCh = 0;

		if (Keyboard->IsPressed(kbsLeftShift)) Shift = 2.0f;

		if (Keyboard->WasPressed(kbsIns))
		{
			CL.pMeshObj->Move (r3dPoint3D(0,Shift,0));
			CL.pMeshObj->ResetXForm();
			V.Y += Shift;
			bCh++;
		}

		if( Keyboard->WasPressed(kbsDel) )
		{
			CL.pMeshObj->Move (r3dPoint3D(0,-Shift,0));
			CL.pMeshObj->ResetXForm();
			V.Y -= Shift;
			bCh++;
		}


		if (Keyboard->WasPressed(kbsHome))
		{
			CL.pMeshObj->Move (r3dPoint3D(Shift,0,0));
			CL.pMeshObj->ResetXForm();
			V.X += Shift;
			bCh++;
		}

		if (Keyboard->WasPressed(kbsEnd))
		{
			CL.pMeshObj->Move (r3dPoint3D(-Shift,0,0));
			CL.pMeshObj->ResetXForm();
			V.X -= Shift;
			bCh++;
		}

		if (Keyboard->WasPressed(kbsPgUp))
		{
			CL.pMeshObj->Move (r3dPoint3D(0,0,Shift));
			CL.pMeshObj->ResetXForm();
			V.Z += Shift;
			bCh++;
		}

		if (Keyboard->WasPressed(kbsPgDn))
		{
			CL.pMeshObj->Move (r3dPoint3D(0,0,-Shift));
			CL.pMeshObj->ResetXForm();
			V.Z -= Shift;
			bCh++;
		}

		sprintf (Str,"%f %f %f", V.X, V.Y, V.Z);

		if (bCh) r3dWriteCFG_S(FName, CL.pMeshObj->Name, "Move", Str);

	}
#endif
}


