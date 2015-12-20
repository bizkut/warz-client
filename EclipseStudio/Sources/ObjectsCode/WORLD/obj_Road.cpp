#include "r3dPCH.h"
#include "r3d.h"
#include "r3dSpline.h"

#include "GameCommon.h"
#include "GameLevel.h"
#include "obj_Road.h"


extern	void		r3dDrawCircle3DT(const r3dPoint3D& P1, const float Radius, const r3dCamera &Cam, float Width, const r3dColor24& clr, r3dTexture *Tex=NULL);
extern	void		r3dDrawUniformLine3DT(const r3dPoint3D& p1, const r3dPoint3D& p2, r3dCamera &Cam, const r3dColor24& clr);

/*static*/ r3dTL::TArray< obj_Road* > obj_Road::AllRoads;

static	float		_road_normalPointDist = 10.0f;
static	float		_road_PointSelDist    = 10.0f;
int		_road_VSegPerRoad     = 3;		// number of road vertical segments
int		_road_UseWireframe    = 0;
int		_road_ShowZFail		  = 0;
int		_road_FixZInShader	  = 1;
extern int VS_CLEAR_FLOAT_ID;
extern int PS_CLEAR_FLOAT_ID;

const char*	obj_Road::BaseMaterialDir = "data/objectsdepot/_roads/materials";

//
//
// 	class for Building Object, as you may guess..
//
//

IMPLEMENT_CLASS(obj_Road, "obj_Road", "Object");
AUTOREGISTER_CLASS(obj_Road);

//////////////////////////////////////////////////////////////////////////

obj_Road::obj_Road()
: autoRegenFlag_(0)
, drawPriority( 5 )
, EDITOR_lastSyncedPosition( 0, 0, 0 )
{
	AllRoads.PushBack( this );

	struct RoadCompare
	{
		bool operator() ( obj_Road* r0, obj_Road* r1 )
		{
			return r0->drawPriority < r1->drawPriority;
		}
	} compare;

	std::sort( &AllRoads[ 0 ], &AllRoads[ 0 ] + AllRoads.Count(), compare );

	ObjTypeFlags |= OBJTYPE_Road;

	setSkipOcclusionCheck(true);
	ObjFlags	|=	OBJFLAG_DisableShadows | OBJFLAG_ForceSleep;

	r3dBoundBox bboxLocal ;

	bboxLocal.Org  = r3dPoint3D(0, 999999, 0);
	bboxLocal.Size = r3dPoint3D(0, 0, 0);

	SetBBoxLocal( bboxLocal ) ;

	width_      = 10.0f;
	texTileLen_ = 100.0f;

	heightAdd_  = 0.0f;

	edSelectedPnt_    = 0;
	edSelectedNormal_ = 0;

	mesh_       = NULL;

	r3dscpy( matName, "-DEFAULT-" );

	UpdateTransform();

	return;
}

obj_Road::~obj_Road()
{
	for( int i = 0, e = (int)AllRoads.Count(); i < e; i ++ )
	{
		if( AllRoads[ i ] == this )
		{
			AllRoads.Erase( i );
			break;
		}
	}

	if( Terrain2 )
		Terrain2->OnRoadDelete( this );

	SAFE_DELETE(mesh_);

	return;
}


#ifndef FINAL_BUILD

static float sDrawPriority = 0.f ;

float obj_Road::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry ;

	sDrawPriority = (float)drawPriority ;

	scry += imgui_Value_Slider( scrx, scry, "Draw priority", &sDrawPriority, 0.f, 16.f, "%.0f" ) ;

	drawPriority = (int)sDrawPriority ;

	return scry - starty ;
}
#endif

BOOL obj_Road::OnCreate()
{
	parent::OnCreate();
	DrawOrder	= OBJ_DRAWORDER_FIRST+5;

	DrawDistanceSq = FLT_MAX;

	EDITOR_lastSyncedPosition = GetPosition();

	return 1;
}



BOOL obj_Road::Update()
{
	if ( mesh_ )
	{
		SetBBoxLocalAndWorld( mesh_->localBBox, mesh_->localBBox ) ;
	}
	return TRUE;
}

void obj_Road::getPointOnSpline(int seg, float mu, r3dPoint3D& pos, r3dPoint3D& vF, r3dPoint3D& vR)
{
	const pnt_s& cp1 = orgPoints_[seg];
	const pnt_s& cp2 = orgPoints_[seg+1];

	r3d_assert(fabs(cp1.n.y) < 0.1f);
	r3d_assert(fabs(cp1.p.y) < 0.1f);

	r3dPoint3D p1 = r3dSpline3D::Bezier4(cp1.p, cp1.p+cp1.n, cp2.p-cp2.n, cp2.p, mu);
	pos = p1;

	const float step = 0.01f;
	if(mu + step > 1.0f) {
		vF = cp2.n.NormalizeTo();
	} else {
		r3dPoint3D p2 = r3dSpline3D::Bezier4(cp1.p, cp1.p+cp1.n, cp2.p-cp2.n, cp2.p, mu + step);
		vF  = (p2 - p1).NormalizeTo();
	}

	vR  = r3dPoint3D(0, 1, 0).Cross(vF);

	return;
}

void obj_Road::getTriPointOnSpline(int seg, float mu, r3dPoint3D v[3])
{
	float w2 = width_ / 2;

	r3dPoint3D pos, vF, vR;
	// fill start values
	getPointOnSpline(seg, mu, pos, vF, vR);
	v[0] = pos;
	v[1] = pos - vR * w2;
	v[2] = pos + vR * w2;

	for(int i=0; i<3; i++) {
		v[i].Y = terra_GetH(v[i]) + heightAdd_;
	}

	// if calculated distance to road edges is greater that expected (because of terrain slope) - scale distance and recalc edges
	if(1) {
		float l1 = (v[1] - v[0]).Length(); 
		float c1 = l1 / w2;
		if(c1 > 1.0f) {
			v[1] = pos - vR * (w2 / c1);
			v[1].Y = terra_GetH(v[1]) + heightAdd_;
		}

		float l2 = (v[2] - v[0]).Length(); 
		float c2 = l2 / w2;
		if(c2 > 1.0f) {
			v[2] = pos + vR * (w2 / c2);
			v[2].Y = terra_GetH(v[2]) + heightAdd_;
		}
	}


	return;
}

void obj_Road::edDrawRoadNormals()
{
	r3d_assert(mesh_);

	const float w = 0.02f;
	const float h = 0.5f;

#if 0
	// from the internal buffer
	for(unsigned int i=0; i<vtx_.size(); i++) {
		const r3dPoint3D& v = vtx_[i].v;
		const r3dPoint3D& n = vtx_[i].n;
		r3dDrawLine3D(v, v+n*h, gCam, w, r3dColor(0, 255, 0));
	}
#else
	// from the mesh
	for(unsigned int i=0; i<vtx_.size(); i++) {
		const r3dPoint3D& v = mesh_->VertexPositions[i];
		const r3dPoint3D& n = mesh_->VertexNormals[i];
		const r3dPoint3D& t = mesh_->VertexTangents[i];
		const r3dPoint3D& b = n.Cross( t ) * ( mesh_->VertexTangentWs[ i ] > 0 ? 1.f : -1.f );

		r3dDrawLine3D(v, v+n*h, gCam, w, r3dColor(0, 255, 0));
		r3dDrawLine3D(v, v+t*h, gCam, w, r3dColor(255, 0, 0));
		r3dDrawLine3D(v, v+b*h, gCam, w, r3dColor(0, 0, 255));
	}

	r3dRenderer->Flush();
#endif  

	return;
}

float GetRoadCtrlScreenSize ( r3dPoint3D vPos, r3dVector vVec, r3dVector vOffset )
{
	D3DXMATRIX mView = r3dRenderer->ViewMatrix;
	D3DXMATRIX mProj = r3dRenderer->ProjMatrix;

	D3DXMATRIX  mTrans;
	D3DXMatrixTranslation ( &mTrans, vPos.x, vPos.y, vPos.z );

	D3DXMATRIX mFinal = mTrans * mView;
	mFinal._41 = 0.0f;
	mFinal._42 = 0.0f;

	mFinal = mFinal * mProj;

	D3DXMATRIX mFinalInv;
	D3DXMatrixInverse ( &mFinalInv, NULL, &mFinal );

	if( r3dRenderer->ZDir == r3dRenderLayer::ZDIR_INVERSED )
	{
		vOffset.z = 1.0f - vOffset.z;
	}

	D3DXVECTOR3 vRes1, vRes2, vOffsetObject;
	D3DXVec3TransformCoord ( &vRes1, vVec.d3dx(), &mFinal );
	D3DXVec3TransformCoord ( &vOffsetObject, vOffset.d3dx(), &mFinalInv );
	vOffsetObject = *vVec.d3dx() - vOffsetObject;
	D3DXVec3TransformCoord ( &vRes2, &vOffsetObject, &mFinal );

	r3dPoint2D v1 ( vRes1.x, vRes1.y );
	r3dPoint2D v2 ( vRes2.x, vRes2.y );

	return r3dRenderer->NearClip / fabs ( v2.x - v1.x );
}

void obj_Road::edDrawRoadConfig()
{
#ifndef FINAL_BUILD
	for(unsigned int seg=0; seg<orgPoints_.size()-1; seg++) 
	{
//		const pnt_s& cp1 = orgPoints_[seg];
//		const pnt_s& cp2 = orgPoints_[seg+1];

		r3dPoint3D v1[3];
		r3dPoint3D v2[3];
		getTriPointOnSpline(seg, 0.0f, v1);

		int iter = 0;

		// draw 3 control lines    
		const float step = 1.0f / 25;
		for(float mu = step; mu < 999.0f; mu += step) 
		{
			if(mu > 1.0f) { // fill last spot to 1.0f
				mu = 1.0f;
			}

			getTriPointOnSpline(seg, mu, v2);

			r3dColor clr = r3dColor(255, 255, 255);
			if((++iter)%2 == 1) clr = r3dColor(0, 0, 0);

			r3dDrawUniformLine3D(v1[0], v2[0], gCam, clr);
			r3dDrawUniformLine3D(v1[1], v2[1], gCam, clr);
			r3dDrawUniformLine3D(v1[2], v2[2], gCam, clr);

			v1[0] = v2[0];
			v1[1] = v2[1];
			v1[2] = v2[2];

			if(mu >= 1.0f) {
				break;
			}
		}
	}

	// control points
	for(unsigned int i=0; i<orgPoints_.size(); i++) 
	{
		const pnt_s& cp = orgPoints_[i];
		r3dPoint3D p1 = cp.p; 
		p1.y = terra_GetH(p1);

		float fScaleCoeff = GetRoadCtrlScreenSize( p1, r3dVector ( 0.0f, 0.0f, 0.0f ), r3dVector ( 1.0f, 0.0f, 0.0f ) );

		r3dDrawUniformSphere(p1, fCircleRadius * fScaleCoeff, gCam, r3dColor(255, 0, 255));

		if(i == edSelectedPnt_ && !edSelectedNormal_) {
			r3dDrawUniformLine3D(p1, p1 + r3dPoint3D(0, fLineHeight * fScaleCoeff, 0), gCam, r3dColor(255, 0, 0));
		} else {
			r3dDrawUniformLine3D(p1, p1 + r3dPoint3D(0, fLineHeight * fScaleCoeff, 0), gCam, r3dColor(0, 255, 0));
		}

		r3dPoint3D cn1 = cp.p + cp.n;
		cn1.y = terra_GetH(cn1);
		r3dDrawUniformLine3DT(p1, cn1, gCam, r3dColor(0, 255, 0));
		r3dDrawUniformSphere(cn1, fCircle2Radius * fScaleCoeff, gCam, r3dColor(200, 0, 200));
		if(i == edSelectedPnt_ && edSelectedNormal_) {
			r3dDrawUniformLine3D(cn1, cn1 + r3dPoint3D(0, fLine2Height * fScaleCoeff, 0), gCam, r3dColor(255, 0, 0));
		} else {
			r3dDrawUniformLine3D(cn1, cn1 + r3dPoint3D(0, fLine2Height * fScaleCoeff, 0), gCam, r3dColor(0, 200, 0));
		}

		r3dPoint3D cn2 = cp.p - cp.n;
		cn2.y = terra_GetH(cn2);
		r3dDrawUniformLine3DT(p1, cn2, gCam, r3dColor(0, 255, 0));
	}
#endif
	return;
}

void obj_Road::addSingleVLine(const r3dPoint3D* v1, float tex_u_off, float add_h)
{
	const float voff = 1.0f   / (float)_road_VSegPerRoad;  // U offset per vertical segment
	const float yoff = width_ / (float)_road_VSegPerRoad;  // zdistance per vertical segment

	r3dPoint3D vR1 = (v1[2] - v1[1]).NormalizeTo();

	for(int vseg = 0; vseg <= _road_VSegPerRoad; vseg++) 
	{
		vtx_s vtx;
		vtx.v   = v1[2] - vR1 * (vseg * yoff);
		vtx.v.Y = terra_GetH(vtx.v) + heightAdd_ + add_h;
		vtx.tv  = 1.0f - (vseg * voff);
		vtx.tu  = tex_u_off;
		vtx_.push_back(vtx);
	}

	return;
}

void obj_Road::createInternalBuffers()
{
	// step 1 - create vertices.
	vtx_.clear();
	vtx_.reserve(30000);

	float cur_texv_off = 0.0f;
	int   cur_hseg     = 0;

	r3dPoint3D v1[3];	// 3 coords for previous segment vertical line. [0] - center, [1] - left, [2] - right
	r3dPoint3D v2[3];	// 3 coords for current  segment vertical line. [0] - center, [1] - left, [2] - right
	getTriPointOnSpline(0, 0.0f, v1);
	addSingleVLine(v1, cur_texv_off, orgPoints_[0].height);
	cur_hseg++;

	for(unsigned int seg=0; seg<orgPoints_.size()-1; seg++) 
	{
		const pnt_s& cp1 = orgPoints_[seg];
		const pnt_s& cp2 = orgPoints_[seg+1];

		const float step = 1.0f / 50;
		for(float mu = step; mu < 999.0f; mu += step) 
		{
			if(mu > 0.98f) { // fill last spot to 1.0f
				mu = 1.0f;
			}

			getTriPointOnSpline(seg, mu, v2);

			// calc distance 
			float dist = (v1[0] - v2[0]).Length();

			// update texture offset
			//float uv1  = cur_texv_off;
			float uv2  = cur_texv_off + (dist / texTileLen_);
			cur_texv_off = uv2;

			// added height from control points
			float add_h = cp1.height + (cp2.height - cp1.height) * (mu);

			addSingleVLine(v2, cur_texv_off, add_h);
			cur_hseg++;

			v1[0] = v2[0];
			v1[1] = v2[1];
			v1[2] = v2[2];

			if(mu >= 1.0f) {
				break;
			}
		} // mu loop
	} // per each segment

	// step 2 - create indices
	idx_.clear();
	idx_.reserve(30000);

	for(int hseg = 0; hseg < cur_hseg - 1; hseg++) 
	{
		int voff = _road_VSegPerRoad+1;
		for(int vseg = 0; vseg < _road_VSegPerRoad; vseg++) 
		{
			idx_.push_back((hseg+0)*voff + vseg);
			idx_.push_back((hseg+0)*voff + vseg + 1);
			idx_.push_back((hseg+1)*voff + vseg + 1);

			idx_.push_back((hseg+1)*voff + vseg + 1);
			idx_.push_back((hseg+1)*voff + vseg);
			idx_.push_back((hseg+0)*voff + vseg);
		}
	}

	return;
}

r3dVector CalcFaceNormal(const r3dPoint3D& vert0, const r3dPoint3D& vert1, const r3dPoint3D& vert2)
//-----------------------------------------------------------------------
{
	r3dVector a, b;

	a      = vert1 - vert0;
	b      = vert2 - vert0;
	r3dVector Normal = a.Cross(b);
	
	if (Normal.LengthSq() > 0)
		Normal.Normalize();

	if(R3D_ABS(Normal.X) < 0.0005) Normal.X = 0.;
	if(R3D_ABS(Normal.Y) < 0.0005) Normal.Y = 0.;
	if(R3D_ABS(Normal.Z) < 0.0005) Normal.Z = 0.;

	return Normal;
}


void obj_Road::recalcNormals()
{
	const int numFaces = idx_.size() / 3;
	const int numVtx   = vtx_.size();

	// reset internal shared vertices normals
	for(int i=0; i<numVtx; i++) {
		vtx_[i].n    = r3dPoint3D(0, 0, 0);
		vtx_[i].cntn = 0;
	}

	// fill normals
	for(int face=0; face<numFaces; face++) 
	{
		int baseIndex = face * 3;
		int i0 = mesh_->Indices[baseIndex + 0];
		int i1 = mesh_->Indices[baseIndex + 1];
		int i2 = mesh_->Indices[baseIndex + 2];

		const r3dVector & v0 = mesh_->VertexPositions[i0];
		const r3dVector & v1 = mesh_->VertexPositions[i1];
		const r3dVector & v2 = mesh_->VertexPositions[i2];

		r3dVector normal = CalcFaceNormal(v0, v1, v2); 
		r3d_assert(r3d_vector_isFinite(normal));

		for(int i=0; i<3; i++)
		{
			const int idx = idx_[face*3+i];
			vtx_[idx].n += normal;
			vtx_[idx].cntn++;
		}
	}

	// smooth
	for(unsigned int i=0; i<vtx_.size(); i++) 
	{
		vtx_s& vtx = vtx_[i];

		r3d_assert(vtx.cntn > 0);
		vtx.n /= (float)vtx.cntn;
		if(vtx.n.LengthSq() > 0)
			vtx.n.Normalize();
//		r3d_assert(vtx.n.Length() > 0.01f);
	}

	// copy normals to r3dMesh
	for(int idx=0; idx<numVtx; idx++) 
	{
		mesh_->VertexNormals[idx] = vtx_[idx].n;
	}
}

void obj_Road::regenerateMesh()
{
	SAFE_DELETE(mesh_);
	mesh_ = gfx_new r3dMesh( 1 );

	const int numIndices = idx_.size();
	const int numVtx   = vtx_.size();

	mesh_->InitIndexList(numIndices);
	mesh_->InitVertsList(numVtx);

	// copy vertices
	for(int i=0; i<numVtx; i++) 
	{
		mesh_->VertexPositions[i] = vtx_[i].v;
		mesh_->VertexUVs[i] = r3dPoint2D(vtx_[i].tu, vtx_[i].tv);
	}
	for(int i=0; i<numIndices; ++i)
		mesh_->Indices[i] = idx_[i];

	// some code from finalize object loading  
	mesh_->SetLoaded();
	mesh_->SetDrawable();

	mesh_->RecalcBoundBox();

	// recalc normals  
	recalcNormals();
	mesh_->RecalcBasisVectors();

	extern bool g_bEditMode;

	if( g_bEditMode )
		mesh_->FillBuffers_SaveVertexData();
	else
		mesh_->FillBuffers();
	
	SetBBoxLocal( mesh_->localBBox );
	UpdateTransform();

	SetMaterial( matName );

	return;
}

void obj_Road::DrawRoad()
{
	if(mesh_ == NULL)
		return;

	D3DPERF_BeginEvent( 0, L"obj_Road::DrawRoad" );

	if(_road_UseWireframe) r3dSetWireframe(1);

	D3DXMATRIX world; D3DXMatrixIdentity(&world);

	// hack for minimap rendering
#ifndef FINAL_BUILD
	extern bool g_bIsMinimapRendering;
	if(g_bIsMinimapRendering)
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_PUSH );
	else
#endif
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH );

	mesh_->SetShaderConsts(world);

	float vConst[4] = { -r_roads_zdisplace->GetFloat(), r3dRenderer->ProjMatrix._43, r3dRenderer->ProjMatrix._33, 0 };
	//	Disable z-displacement when error show mode is on
	if (_road_ShowZFail > 0 || _road_FixZInShader == 0) { vConst[0] = 0; }
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( MC_DDEPTH, vConst, 1 );

	//r3dRenderer->SetMipMapBias(-2.0f);

	//disable writing to MRT2 - depth
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE1, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE2, 0);
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE3, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE );

	DWORD curStencilEnable = 0;
	DWORD curStencilRef = 0;
	DWORD curStencilFunc = 0;
	DWORD curStencilPass = 0;
	DWORD curStencilZFail = 0;
	DWORD curStencilWriteMask = 0;
	if (_road_ShowZFail > 0)
	{
		//	Save all current states
		D3D_V(r3dRenderer->pd3ddev->GetRenderState(D3DRS_STENCILENABLE, &curStencilEnable));
		D3D_V(r3dRenderer->pd3ddev->GetRenderState(D3DRS_STENCILWRITEMASK, &curStencilWriteMask));
		D3D_V(r3dRenderer->pd3ddev->GetRenderState(D3DRS_STENCILREF, &curStencilRef));
		D3D_V(r3dRenderer->pd3ddev->GetRenderState(D3DRS_STENCILFUNC, &curStencilFunc));
		D3D_V(r3dRenderer->pd3ddev->GetRenderState(D3DRS_STENCILPASS, &curStencilPass));
		D3D_V(r3dRenderer->pd3ddev->GetRenderState(D3DRS_STENCILZFAIL, &curStencilZFail));
		//	Mark z-fail regions with stencil bit 3
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, TRUE));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILWRITEMASK, 4));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILREF, 4));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_REPLACE));
	}

	mesh_->DrawMeshDeferred( r3dColor::white, R3D_MATF_ROAD );

	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );


	// enable writing to MRT

	r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE1, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE2, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE3, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED);
	//r3dRenderer->SetMipMapBias(__WorldRenderBias);

#if 0
	// debug version of drawing - straight from calculated vertices
	for(unsigned int idx=0; idx<idx_.size(); idx += 3) {
		r3dWorldVertex V[3];
		for(int i=0; i<3; i++) {
			V[i].color  = r3dColor(255, 255, 255);
			V[i].Normal = r3dPoint3D(0, 1, 0);
			V[i].Pos    = vtx_[idx_[idx+i]].v;
			V[i].tu     = vtx_[idx_[idx+i]].tu;
			V[i].tv     = vtx_[idx_[idx+i]].tv;
		}
		r3dRenderer->Render3DPolygon(3, V);
	}
	r3dRenderer->Flush();
#endif

	if(_road_UseWireframe) r3dSetWireframe(0);

	if (_road_ShowZFail > 0)
	{
		DWORD curStencilMask = 0;
		D3D_V(r3dRenderer->pd3ddev->GetRenderState(D3DRS_STENCILMASK, &curStencilMask));
		
		//	Now Render full screen quad and mark all road occlusions
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILMASK, 4));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP));

		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH);
 		r3dRenderer->SetPixelShader( VS_CLEAR_FLOAT_ID );
 		r3dRenderer->SetPixelShader( PS_CLEAR_FLOAT_ID );

		float cl[4] = { 1.0f, 0, 0, 0 };
		r3dRenderer->pd3ddev->SetPixelShaderConstantF (0, &cl[0], 1);

		r3dDrawBoxFS(r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::red);
		r3dRenderer->SetRenderingMode(R3D_BLEND_POP);

		//	Restore stencil state
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, curStencilEnable));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILREF, curStencilRef));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILFUNC, curStencilFunc));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILPASS, curStencilPass));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILZFAIL, curStencilZFail));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILWRITEMASK, curStencilWriteMask));
		D3D_V(r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILMASK, curStencilMask));
	}


	D3DPERF_EndEvent();

	return;
}

//------------------------------------------------------------------------

void
obj_Road::DrawRoadToTerraAtlas()
{
	if(mesh_ == NULL)
		return;

	D3DPERF_BeginEvent( 0, L"obj_Road::DrawRoadToTerraAtlas" );

	D3DXMATRIX world; D3DXMatrixIdentity(&world);

	mesh_->SetShaderConsts(world);

	mesh_->DrawMeshDeferred( r3dColor::white, R3D_MATF_ROAD | R3D_MATF_NO_SHADERS );

	D3DPERF_EndEvent();

	return;
}

//------------------------------------------------------------------------

struct RoadRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		R3DPROFILE_FUNCTION("RoadRenderable");
		RoadRenderable *This = static_cast<RoadRenderable*>( RThis );

		This->Parent->DrawRoad();
	}

	obj_Road*	Parent;	
};

/*virtual*/
void
obj_Road::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
	RoadRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.SortValue	= ( 8 + drawPriority ) * RENDERABLE_USER_SORT_VALUE ;

	render_arrays[ rsFillGBuffer ].PushBack( rend );
}

void obj_Road::ReadSerializedData(pugi::xml_node& node)
{
	char dir[MAX_PATH], file[MAX_PATH];
	sprintf(dir, "%s\\roads", r3dGameLevel::GetHomeDir());
	sprintf(file, "%s\\%s.dat", dir, Name.c_str());

	pugi::xml_node road_child = node.child( "road" ) ;

	if( !road_child.empty() )
	{
		pugi::xml_attribute road_priority = road_child.attribute( "draw_priority" ) ;
		if( !road_priority.empty() )
		{
			drawPriority = road_priority.as_int() ;
		}
		else
		{
			drawPriority = 5 ;
		}
	}

	r3dFile* f = r3d_open(file, "rb");
	if(!f) {
		r3dArtBug("!!!WARNING!!! obj_Road %s have no data file %s\n", Name.c_str(), file);

		setActiveFlag(0);
		return;
	}

	WORD version;
	fread(&version, sizeof(version), 1, f);
	switch (version)
	{
	case 3:
		ReadVer3SerializedData(f);
		break;
	case 4:
		ReadVer4SerializedData(f);
		break;
	default:
		r3dError("invalid obj_Road version %d file %s", version, file);
		fclose(f);
		setActiveFlag(0);
		return;
	}
	fclose(f);

	if (autoRegenFlag_ != 0)
		createInternalBuffers();
	regenerateMesh();
}

//////////////////////////////////////////////////////////////////////////

/**	Read specific header from save. Delete [] returned value after use. */
template <typename HeaderType>
char * ReadBinHeader(r3dFile *f, obj_Road &o)
{
	char * data = game_new char[sizeof(HeaderType) + R3D_MAX_MATERIAL_NAME];
	HeaderType *pHdr = reinterpret_cast<HeaderType*>(data);
	HeaderType& hdr = *pHdr;
	fread(&hdr.length, sizeof(hdr.length), 1, f);
	fread( ( ( char* ) pHdr ) + sizeof( hdr.length ), hdr.length - sizeof( hdr.length ), 1, f );

	o.width_      = hdr.width;
	o.texTileLen_ = hdr.texTileLen;
	if (hdr.MaterialName[0] != 0)
	{
		r3dscpy(o.matName, hdr.MaterialName);
	}

	r3d_assert(hdr.numPoints >= 2);
	for(unsigned int i=0; i<hdr.numPoints; i++)
	{
		obj_Road::pnt_s cp;
		fread(&cp, sizeof(cp), 1, f);
		o.orgPoints_.push_back(cp);
	}
	return data;
}

//////////////////////////////////////////////////////////////////////////

//	binhdr003_s Version
void obj_Road::ReadVer3SerializedData(r3dFile *f)
{
	char * data = ReadBinHeader<binhdr003_s>(f, *this);
	delete [] data;
}

//////////////////////////////////////////////////////////////////////////

//	binhdr004_s Version
void obj_Road::ReadVer4SerializedData(r3dFile *f)
{
	char * data = ReadBinHeader<binhdr004_s>(f, *this);
	binhdr004_s * h = reinterpret_cast<binhdr004_s*>(data);
	//	Read vertices
	vtx_.resize(h->numVertices);
	fread(&vtx_[0], sizeof(Vertices::value_type), vtx_.size(), f);
	//	Read indices
	idx_.resize(h->numIndices);
	fread(&idx_[0], sizeof(Indices::value_type), idx_.size(), f);
	delete [] data;
}

//////////////////////////////////////////////////////////////////////////

void obj_Road::WriteSerializedData(pugi::xml_node& node)
{
	pugi::xml_node road_child ;
	road_child = node.append_child() ;
	road_child.set_name( "road" ) ;
	road_child.append_attribute( "draw_priority" ) = drawPriority ;

	char dir[MAX_PATH], file[MAX_PATH];
	sprintf(dir, "%s\\roads", r3dGameLevel::GetSaveDir() );
	sprintf(file, "%s\\%s.dat", dir, Name.c_str());

	CreateDirectory(dir, NULL);
	FILE* f = fopen_for_write(file, "wb");
	r3d_assert(f);

	WORD version = 4;
	fwrite(&version, sizeof(version), 1, f);

	typedef binhdr004_s HeaderType;

	HeaderType* pHdr = ( HeaderType* ) game_new char[ sizeof( HeaderType ) + R3D_MAX_MATERIAL_NAME ];
	HeaderType& hdr = *pHdr;
	DWORD matNameLen = strlen( matName );
	hdr.length = sizeof( HeaderType ) + matNameLen;
	hdr.numPoints  = orgPoints_.size();
	hdr.width      = width_;
	hdr.texTileLen = texTileLen_;
	hdr.numVertices = static_cast<DWORD>(vtx_.size());
	hdr.numIndices = static_cast<DWORD>(idx_.size());
	hdr.autoRegenFlag = autoRegenFlag_;
	memcpy( hdr.MaterialName, matName, matNameLen + 1 );
	fwrite(&hdr, sizeof( HeaderType ) + matNameLen, 1, f);

	for(unsigned int i=0; i<hdr.numPoints; i++) {
		const pnt_s& cp = orgPoints_[i];
		fwrite(&cp, sizeof(cp), 1, f);
	}

	delete[] pHdr;

	//	Write vertices
	if (!vtx_.empty())
		fwrite(&vtx_[0], sizeof(Vertices::value_type), vtx_.size(), f);

	//	Write indices
	if (!idx_.empty())
		fwrite(&idx_[0], sizeof(Indices::value_type), idx_.size(), f);

	fclose(f);
}

//------------------------------------------------------------------------

/*virtual*/ BOOL obj_Road::OnPositionChanged()
{
#ifndef FINAL_BUILD
	BOOL res = GameObject::OnPositionChanged();

	r3dPoint3D curPos = GetPosition();

	r3dPoint3D offset = curPos - EDITOR_lastSyncedPosition;

	for( int i = 0, e = (int)orgPoints_.size(); i < e; i ++ )
	{
		orgPoints_[ i ].p.x += offset.x;
		orgPoints_[ i ].p.z += offset.z;
	}

	createInternalBuffers();
	regenerateMesh();

	EDITOR_lastSyncedPosition = curPos;

	return res;
#else
	return GameObject::OnPositionChanged();
#endif
}

//////////////////////////////////////////////////////////////////////////

void obj_Road::edCreate(const r3dPoint3D& in_p1, const r3dPoint3D& in_p2)
{
	r3d_assert(orgPoints_.size() == 0);

	// zero Y coord
	r3dPoint3D p1 = in_p1; p1.y = 0;
	r3dPoint3D p2 = in_p2; p2.y = 0;

	pnt_s cp1;
	cp1.p = p1;
	cp1.n = (p2-p1).NormalizeTo() * _road_normalPointDist;

	pnt_s cp2;
	cp2.p = p2;
	cp2.n = (p2-p1).NormalizeTo() * _road_normalPointDist;

	orgPoints_.push_back(cp1);
	orgPoints_.push_back(cp2);

	return;
}

void obj_Road::edAddPoint(const r3dPoint3D& in_pnt, int toTail)
{
	r3dPoint3D pnt = in_pnt; pnt.y = 0;

	pnt_s cp;
	cp.p = pnt;

	if(toTail) {
		const r3dPoint3D& p1 = orgPoints_[orgPoints_.size() - 1].p;
		const r3dPoint3D& p2 = pnt;
		cp.n = (p2-p1).NormalizeTo() * _road_normalPointDist;
		orgPoints_.push_back(cp);
	} else {
		const r3dPoint3D& p1 = pnt;
		const r3dPoint3D& p2 = orgPoints_[0].p;
		cp.n = (p2-p1).NormalizeTo() * _road_normalPointDist;
		orgPoints_.insert(orgPoints_.begin(), cp);
	}

	return;
}

void obj_Road::edSplit(int idx, int toRight)
{
	int i1 = toRight ? idx      : idx - 1;
	int i2 = toRight ? idx + 1  : idx;

	r3d_assert(i1 >= 0 && i1 < (int)orgPoints_.size());
	r3d_assert(i2 >= 0 && i2 < (int)orgPoints_.size());

	const r3dPoint3D& p1 = orgPoints_[i1].p;
	const r3dPoint3D& p2 = orgPoints_[i2].p;

	pnt_s cp;
	cp.p = (p1 + (p2-p1) / 2);
	cp.n = (p2-p1).NormalizeTo() * _road_normalPointDist;

	orgPoints_.insert(orgPoints_.begin() + i2, cp);

	return;
}

int obj_Road::edGetClosestPoint(const r3dPoint3D& pnt, int* isNormal)
{
	R3DPROFILE_FUNCTION("obj_Road::edGetClosestPoint");
	r3dPoint3D p = r3dRenderer->CameraPosition;
	r3dPoint3D d = pnt - p;
	r3dRay worldRay(p, d.Normalize());
	r3dVector zero(0, 0, 0);
	r3dVector left(1, 0, 0);

	for (unsigned int i = 0; i < orgPoints_.size(); i++)
	{
		const pnt_s& cp = orgPoints_[i];

		r3dVector realP(cp.p);
		realP.y = terra_GetH(realP);

		float fScaleCoeff = GetRoadCtrlScreenSize(realP, zero, left);

		r3dSphere s1(realP, fScaleCoeff * fCircleRadius);
		BOOL intersectP = worldRay.IsIntersect(s1);
		if (intersectP)
		{
			if (isNormal) *isNormal = 0;
			return i;
		}

		realP.Assign(cp.n + cp.p);
		realP.y = terra_GetH(realP);
		
		fScaleCoeff = GetRoadCtrlScreenSize(realP, zero, left);

		r3dSphere s2(realP, fScaleCoeff * fCircle2Radius);
		intersectP = worldRay.IsIntersect(s2);
		if (intersectP)
		{
			if (isNormal) *isNormal = 1;
			return i;
		}
	}

	return -1;
}

void obj_Road::SetMaterial( const char* szName )
{
	if ( mesh_ )
	{
		r3dTriBatch& batch = mesh_->MatChunks[ 0 ];
		r3dMaterial* pMat = r3dMaterialLibrary::RequestMaterialByMesh( szName, BaseMaterialDir, false, false );
		if ( pMat )
		{
			batch.Mat = pMat;
			r3dscpy( matName, szName );
			batch.StartIndex = 0;
			batch.EndIndex = mesh_->NumIndices;
			mesh_->NumMatChunks = 1;
		}
	}
}

//-------------------------------------------------------------------------
//	Standalone helper functions
//-------------------------------------------------------------------------

#ifndef FINAL_BUILD
void CleanOrphanedRoadFiles()
{
	//	Collect all file names in level_name/roads directory
	r3dTL::TArray<r3dString> roadFiles;
	WIN32_FIND_DATA fd;
	char path[MAX_PATH] = {0};
	sprintf(path, "%s\\roads\\*.dat", r3dGameLevel::GetHomeDir());

	HANDLE h = FindFirstFile(path, &fd);
	if (h == INVALID_HANDLE_VALUE)
		return;
	
	do 
	{
		fd.cFileName[strlen(fd.cFileName) - 4] = 0;
		roadFiles.PushBack(fd.cFileName);
	}
	while (FindNextFile(h, &fd) != 0);
	FindClose(h);

	//	Iterate over all objects and remove valid roads from orphan list
	for (const GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
	{
		if (!obj->isObjType(OBJTYPE_Road))
			continue;

		const obj_Road *r = static_cast<const obj_Road*>(obj);
		int idx = roadFiles.FindItemIndex(r->FileName);
		if (idx != -1)
		{
			//	Not orphaned, remove from list
			roadFiles.Erase(idx);
		}
	}

	//	Now delete all orphan road files
	for (uint32_t i = 0; i < roadFiles.Count(); ++i)
	{
		const r3dString &name = roadFiles[i];
		sprintf(path, "%s\\roads\\%s.dat", r3dGameLevel::GetHomeDir(), name);
		DeleteFile(path);
	}
}
#endif