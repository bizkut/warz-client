#include "r3dPCH.h"
#include "r3d.h"

#pragma warning(disable: 4244)

float FVDL = -0.5f;

void r3dDrawBlurQuad(float x, float y, float w, float h, float Tap[8], float Power)
{
	R3D_SCREEN_VERTEX 	V[6];
	float  	Z = r3dRenderer->NearClip;

	for (int i = 0; i < _countof(V); i++)
	{
		V[i].color  = 0xffffffff; //clr.GetPacked();
		V[i].z  = Z;
		V[i].rwh = Z;//1.0f / Z;
	}

	//  V[0].color  = 0xffffffff; //clr.GetPacked();
	//  V[1].color  = 0xffffffff; //clr.GetPacked();
	//  V[2].color  = 0xffffffff; //clr.GetPacked();
	//  V[3].color  = 0xffffffff; //clr.GetPacked();

	float DS =  FVDL; //-0.25f;

	V[0].x   = x+DS;  	V[0].y   = y + h+DS;
	V[1].x   = x+DS;  	V[1].y   = y+DS;
	V[2].x   = x + w+DS;  	V[2].y   = y + h+DS;
	V[3].x   = x + w+DS;  	V[3].y   = y+DS;

	V[0].tu = 0+Power*Tap[0]; 		V[0].tv = 1+Power*Tap[1];
	V[1].tu = 0+Power*Tap[0]; 		V[1].tv = 0+Power*Tap[1];
	V[2].tu = 1+Power*Tap[0]; 		V[2].tv = 1+Power*Tap[1];
	V[3].tu = 1+Power*Tap[0]; 		V[3].tv = 0+Power*Tap[1];

	V[0].tu2 = 0+Power*Tap[2]; 		V[0].tv2 = 1+Power*Tap[3];
	V[1].tu2 = 0+Power*Tap[2]; 		V[1].tv2 = 0+Power*Tap[3];
	V[2].tu2 = 1+Power*Tap[2]; 		V[2].tv2 = 1+Power*Tap[3];
	V[3].tu2 = 1+Power*Tap[2]; 		V[3].tv2 = 0+Power*Tap[3];

	V[0].tu3 = 0+Power*Tap[4]; 		V[0].tv3 = 1+Power*Tap[5];
	V[1].tu3 = 0+Power*Tap[4]; 		V[1].tv3 = 0+Power*Tap[5];
	V[2].tu3 = 1+Power*Tap[4]; 		V[2].tv3 = 1+Power*Tap[5];
	V[3].tu3 = 1+Power*Tap[4]; 		V[3].tv3 = 0+Power*Tap[5];

	V[0].tu4 = 0+Power*Tap[6]; 		V[0].tv4 = 1+Power*Tap[7];
	V[1].tu4 = 0+Power*Tap[6]; 		V[1].tv4 = 0+Power*Tap[7];
	V[2].tu4 = 1+Power*Tap[6]; 		V[2].tv4 = 1+Power*Tap[7];
	V[3].tu4 = 1+Power*Tap[6]; 		V[3].tv4 = 0+Power*Tap[7];

	V[4] = V[3];
	V[3] = V[1];
	V[5] = V[2];

	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	r3dRenderer->pd3ddev->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	r3dRenderer->pd3ddev->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	r3dRenderer->pd3ddev->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	r3dRenderer->pd3ddev->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	r3dRenderer->pd3ddev->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	r3dRenderer->pd3ddev->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	D3D_V( d3dc._SetVertexShader(0) );
	r3dDrawTriangleList(V, _countof(V));

	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

void r3dBlurBuffer(r3dScreenBuffer *SourceTex, r3dScreenBuffer *TempTex, int BlurPower)
{
	float TapFilterH[] = {0,0, -1,0, 1,0, 0,0};
	float TapFilterV[] = {0,0,  0,-1, 0,1, 0,0};

	int BlurIdx = r3dRenderer->GetShaderIdx("PS_BLUR");

	if (BlurIdx < 0 ) return;

	float *T[] = {TapFilterH,TapFilterV};

	r3dScreenBuffer *Buffer[2];
	Buffer[0] = SourceTex;
	Buffer[1] = TempTex;

	int CurBB = 1;

	r3dRenderer->StartRenderSimple(0);
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);
	r3dRenderer->SetPixelShader(BlurIdx);
	r3dSetFiltering( R3D_BILINEAR );

	for (int i=0;i<BlurPower;i++)
	{
		float PixSize = (1.0f / SourceTex->Tex->GetWidth());
		float DV = PixSize /2.0f + (PixSize * float(1));

		Buffer[CurBB]->Activate();

		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,1);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,2);
		//r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,3);

		r3dDrawBlurQuad(0, 0, Buffer[CurBB]->Tex->GetWidth(), Buffer[CurBB]->Tex->GetHeight(), T[0], DV); //, BlurVShader->m_dwShader);
		Buffer[CurBB]->Deactivate();
		CurBB = 1 - CurBB;   
	}



	for(int i=0;i<BlurPower;i++)
	{
		float PixSize = (1.0f / SourceTex->Tex->GetWidth());
		float DV = PixSize /2.0f + (PixSize * float(1));

		Buffer[CurBB]->Activate();
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,1);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,2);
		//r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,3);

		r3dDrawBlurQuad(0,0, Buffer[CurBB]->Tex->GetWidth(), Buffer[CurBB]->Tex->GetHeight(), T[1], DV); //, BlurVShader->m_dwShader);

		Buffer[CurBB]->Deactivate();
		CurBB = 1 - CurBB;   
	}

	r3dRenderer->SetPixelShader();
	r3dRenderer->EndRenderSimple();
}




void r3dBlurBufferG(r3dScreenBuffer *SourceTex1, r3dScreenBuffer *SourceTex, r3dScreenBuffer *TempTex, int BlurPower)
{
	int BlurIdx = r3dRenderer->GetShaderIdx("PS_BLURG");

	if (BlurIdx < 0 ) return;

	r3dScreenBuffer *Buffer[2];
	Buffer[0] = SourceTex;
	Buffer[1] = TempTex;

	int CurBB = 1;

	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	r3dRenderer->StartRenderSimple(0);
	// r3dRenderer->SetMaterial(NULL);
	r3dSetFiltering( R3D_BILINEAR );


	SourceTex->Activate();
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);
	r3dDrawBox2D(0,0,SourceTex->Tex->GetWidth(),SourceTex->Tex->GetHeight(), r3dColor(255,255,255), SourceTex1->Tex);
	r3dRenderer->SetRenderingMode(R3D_BLEND_ADD);
	r3dDrawBox2D(0,0,SourceTex->Tex->GetWidth(),SourceTex->Tex->GetHeight(), r3dColor(155,155,155), SourceTex1->Tex);
	SourceTex->Deactivate();


	for (int i=0;i<BlurPower;i++)
	{
		float PixSizeX = 1.0f/float(Buffer[CurBB]->Tex->GetWidth());
		float PixSizeY = 1.0f/float(Buffer[CurBB]->Tex->GetHeight());

		float XS = PixSizeX* float(i+1) + 0.75f*PixSizeX;
		float YS = PixSizeY* float(i+1) + 0.75f*PixSizeY;

		float samples4[] = {
			-1.0f,	-1.0f,	1.0f,	-1.0f,
			1.0f,  1.0f,	- 1.0f,	1.0f};

			for (int k=0;k<8;k+=2)
			{
				samples4[k] *= XS;
				samples4[k+1] *= YS;
			}

			r3dRenderer->pd3ddev->SetPixelShaderConstantF(  1, (float *)samples4,  2 );


			//  D3DXVECTOR4 BlurMul(XS, YS, 0,0);
			//  r3dRenderer->pd3ddev->SetPixelShaderConstantF(  0, (float *)&BlurMul,  1 );

			Buffer[CurBB]->Activate();
			//  r3dRenderer->StartRenderSimple(0);
			//  r3dRenderer->SetMaterial(NULL);

			if (!i)
			{
				r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);
				r3dDrawBox2D(0,0, Buffer[CurBB]->Tex->GetWidth(), Buffer[CurBB]->Tex->GetHeight(), r3dColor(0,0,0));
			}

			r3dSetFiltering( R3D_BILINEAR );
			r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);//NOALPHA);

			r3dRenderer->SetPixelShader(BlurIdx);

			int Vis = 255; //255/2;
			r3dDrawBox2D(0,0, Buffer[CurBB]->Tex->GetWidth(), Buffer[CurBB]->Tex->GetHeight(), r3dColor(Vis,Vis,Vis), Buffer[1-CurBB]->Tex);

			r3dRenderer->SetPixelShader();

			//  r3dRenderer->EndRenderSimple();
			Buffer[CurBB]->Deactivate(0);
			CurBB = 1 - CurBB;   
	}

	Buffer[1 - CurBB]->Deactivate();

	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	r3dRenderer->EndRenderSimple();
}



void r3dBlur2Buffer(r3dScreenBuffer *SourceTex, r3dScreenBuffer *TargetTex, r3dScreenBuffer *TempTex, int BlurPower)
{
	float TapFilterH[] = {0,0, -1,0, 1,0, 0,0};
	float TapFilterV[] = {0,0,  0,-1, 0,1, 0,0};

	float *T[] = {TapFilterH,TapFilterV};

	int BlurIdx = r3dRenderer->GetShaderIdx("PS_BLUR");

	float TC[16];

	float XS = float(r3dRenderer->ScreenW) / float(SourceTex->Width);
	float YS = float(r3dRenderer->ScreenH) / float(SourceTex->Height);

	TC[0]	= 0;	TC[1]	= 0;
	TC[2]	= XS;	TC[3]	= 0;
	TC[4]	= XS;	TC[5]	= YS;
	TC[6]	= 0;	TC[7]	= YS;


	r3dRenderer->StartRenderSimple(0);
	r3dSetFiltering( R3D_BILINEAR );
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

	TargetTex->Activate();
	// r3dRenderer->SetMaterial(NULL);
	r3dDrawBox2D(0,0,TargetTex->Tex->GetWidth(),TargetTex->Tex->GetHeight(), r3dColor(255,255,255), SourceTex->Tex, TC);
	// r3dRenderer->EndRenderSimple();
	TargetTex->Deactivate();


	r3dScreenBuffer *Buffer[2];
	Buffer[0] = TargetTex;
	Buffer[1] = TempTex;

	int CurBB = 1;

	// r3dSetFiltering( R3D_BILINEAR );

	r3dRenderer->SetPixelShader(BlurIdx);
	// r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

	for (int i=0;i<BlurPower;i++)
	{
		float PixSize = (1.0f / TargetTex->Tex->GetWidth());
		float DV = (PixSize * float(i)) + PixSize /2.0f;

		Buffer[CurBB]->Activate();
		//  r3dRenderer->StartRenderSimple(SClear[i]);
		//  r3dRenderer->SetMaterial(NULL);
		//  r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,1);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,2);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,3);

		r3dDrawBlurQuad(0,0, TargetTex->Tex->GetWidth(), TargetTex->Tex->GetHeight(), T[0], DV); //, BlurVShader->m_dwShader);

		//  r3dRenderer->EndRenderSimple();

		Buffer[CurBB]->Deactivate(0);
		CurBB = 1 - CurBB;   
	}


	for(int i=0;i<BlurPower;i++)
	{
		float PixSize = (1.0f / TargetTex->Tex->GetWidth());
		float DV = (PixSize * float(i)) + PixSize /2.0f;

		Buffer[CurBB]->Activate();
		//  r3dRenderer->StartRenderSimple(0);
		//  r3dRenderer->SetMaterial(NULL);
		//  r3dSetFiltering( R3D_BILINEAR );
		//  r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,1);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,2);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,3);

		//  r3dRenderer->SetPixelShader(BlurIdx);
		r3dDrawBlurQuad(0,0, TargetTex->Tex->GetWidth(), TargetTex->Tex->GetHeight(), T[1], DV); //, BlurVShader->m_dwShader);
		//  r3dRenderer->SetPixelShader();

		//  r3dRenderer->EndRenderSimple();
		Buffer[CurBB]->Deactivate(0);
		CurBB = 1 - CurBB;   
	}


	Buffer[1-CurBB]->Deactivate();

	r3dRenderer->SetPixelShader();
	r3dRenderer->EndRenderSimple();

}




void r3dBlurDOFBuffer(r3dScreenBuffer *DepthBuffer, r3dScreenBuffer *TempTex1, r3dScreenBuffer *TempTex2, int BlurPower)
{
	float TapFilterH[] = {0,0, -1,0, 1,0, 0,0};
	float TapFilterV[] = {0,0,  0,-1, 0,1, 0,0};

	float *T[] = {TapFilterH,TapFilterV};

	int BlurIdx = r3dRenderer->GetShaderIdx("PS_BLUR");

	float TC[16];

	float XS = float(r3dRenderer->ScreenW) / float(DepthBuffer->Width);
	float YS = float(r3dRenderer->ScreenH) / float(DepthBuffer->Height);

	TC[0]	= 0;	TC[1]	= 0;
	TC[2]	= XS;	TC[3]	= 0;
	TC[4]	= XS;	TC[5]	= YS;
	TC[6]	= 0;	TC[7]	= YS;


	r3dRenderer->StartRenderSimple(0);
	r3dSetFiltering( R3D_BILINEAR );
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

	r3dRenderer->SetPixelShader("PS_BLURNEAR1");

	TempTex1->Activate();
	r3dDrawBox2D(0,0,TempTex1->Tex->GetWidth(),TempTex1->Tex->GetHeight(), r3dColor(255,255,255), DepthBuffer->Tex, TC);
	TempTex1->Deactivate();
	// TempTex1->Tex->SaveBMP ("Blur1.tga");


	r3dScreenBuffer *Buffer[2];
	Buffer[0] = TempTex1;
	Buffer[1] = TempTex2;

	int CurBB = 1;

	// r3dSetFiltering( R3D_BILINEAR );

	r3dRenderer->SetPixelShader(BlurIdx);
	// r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);


	for (int i=0;i<BlurPower;i++)
	{
		float PixSize = (1.0f / TempTex2->Tex->GetWidth());
		float DV = (PixSize * float(i)) + PixSize /2.0f;

		Buffer[CurBB]->Activate();
		//  r3dRenderer->StartRenderSimple(SClear[i]);
		//  r3dRenderer->SetMaterial(NULL);
		//  r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,1);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,2);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,3);

		r3dDrawBlurQuad(0,0, TempTex2->Tex->GetWidth(), TempTex2->Tex->GetHeight(), T[0], DV); //, BlurVShader->m_dwShader);

		//  r3dRenderer->EndRenderSimple();

		Buffer[CurBB]->Deactivate(0);
		CurBB = 1 - CurBB;   
	}


	for(int i=0;i<BlurPower;i++)
	{
		float PixSize = (1.0f / TempTex2->Tex->GetWidth());
		float DV = (PixSize * float(i)) + PixSize /2.0f;

		Buffer[CurBB]->Activate();
		//  r3dRenderer->StartRenderSimple(0);
		//  r3dRenderer->SetMaterial(NULL);
		//  r3dSetFiltering( R3D_BILINEAR );
		//  r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,1);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,2);
		r3dRenderer->SetTex(Buffer[1-CurBB]->Tex,3);

		//  r3dRenderer->SetPixelShader(BlurIdx);
		r3dDrawBlurQuad(0,0, TempTex2->Tex->GetWidth(), TempTex2->Tex->GetHeight(), T[1], DV); //, BlurVShader->m_dwShader);
		//  r3dRenderer->SetPixelShader();

		//  r3dRenderer->EndRenderSimple();
		Buffer[CurBB]->Deactivate(0);
		CurBB = 1 - CurBB;   
	}


	Buffer[1-CurBB]->Deactivate();

	// TempTex2->Tex->SaveBMP ("Blur2.tga");

	r3dRenderer->SetPixelShader("PS_BLURNEAR2");
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_GREEN);

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	DepthBuffer->Activate();
	r3dDrawBox2D(0,0,DepthBuffer->Tex->GetWidth(),DepthBuffer->Tex->GetHeight(), r3dColor(255,255,255), Buffer[1-CurBB]->Tex);
	DepthBuffer->Deactivate();

	r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);


	r3dRenderer->SetPixelShader();
	r3dRenderer->EndRenderSimple();

}


