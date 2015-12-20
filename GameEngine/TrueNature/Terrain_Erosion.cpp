#include "r3dPCH.h"

#pragma warning( disable : 4244 )
#pragma warning( disable : 4018 )
#pragma warning( disable : 4800 )

#include "r3d.h"
#include "r3dBuffer.h"

#include "GameCommon.h"
#include "rendering/Deffered/DeferredHelpers.h"
#include "rendering/Deffered/CommonPostFX.h"

#include "../SF/Console/Config.h"
#include "../SF/RenderBuffer.h"

#include "r3dBitMaskArray.h"

#include "../TrueNature2/Terrain3.h"
#include "../TrueNature2/Terrain2.h"


#define CellGridSize 64
namespace
{
	typedef r3dTL::TArray< unsigned char > Bytes;
	typedef r3dTL::TArray< float > Floats;
}


// number of cells x,y same as max_erosion_Size * 2
#define EROSION_N 128
// size is x*y and add 2 to each for border
#define	EROSION_GRID_SIZE	((EROSION_N+2) * (EROSION_N+2))
// fast index access to allow x,y coords (or i,j faggots)
#define IX(i,j) ((i)+(EROSION_N+2)*(j))
#define SWAP(x0,x) {float *tmp=x0;x0=x;x=tmp;}


static float u[EROSION_GRID_SIZE];
static float v[EROSION_GRID_SIZE];
static float u_prev[EROSION_GRID_SIZE];
static float v_prev[EROSION_GRID_SIZE];
static float dens2[EROSION_GRID_SIZE];
static float dens2_prev[EROSION_GRID_SIZE];

struct CErosionWater{
	float mHeight;
	float mVelocityX; // water
	float mVelocityZ;
	float mOutFlux[4]; // for flux values
	float mVel; // calced once
};

struct CErosionSed{
	float mAmount; // amount in sed
};

#define MAX_EROSION_SIZE	128

CErosionWater mErosionWaterMap[MAX_EROSION_SIZE*2][MAX_EROSION_SIZE*2];
float mErosionHeightMap[MAX_EROSION_SIZE*2][MAX_EROSION_SIZE*2];
float mOldErosionHeightMap[MAX_EROSION_SIZE*2][MAX_EROSION_SIZE*2];
float mErosionRainMap[MAX_EROSION_SIZE*2][MAX_EROSION_SIZE*2];
float mErosionDepositMap[MAX_EROSION_SIZE*2][MAX_EROSION_SIZE*2];
float mErosionDropMap[MAX_EROSION_SIZE*2][MAX_EROSION_SIZE*2];
CErosionSed mErosionSedMap[MAX_EROSION_SIZE*2][MAX_EROSION_SIZE*2];

// for render


#define MAX_EROSION_VERT		(MAX_EROSION_SIZE*MAX_EROSION_SIZE * 4)
#define MAX_EROSION_IDX		(MAX_EROSION_SIZE * MAX_EROSION_SIZE * 3 * 2)

R3D_DEBUG_VERTEX mErosionMesh[MAX_EROSION_SIZE*MAX_EROSION_SIZE * 4 ];// x*y verts
uint16_t mErosionIndicies[MAX_EROSION_SIZE * MAX_EROSION_SIZE * 3 * 2]; // tri verts. 2 per grid, 3 per tri


r3dPoint3D lastpt;
int mErosionMeshNum = 0;
int mErosionNumPrim = 0;


void set_bnd ( int N, int b, float *x );
void project ( int N, float *u, float *v, float *p, float *div );
void vel_step ( int N, float *u, float *v, float *u0, float *v0, float visc, float dt );
void dens_step ( int N, float *x, float *x0, float *u, float *v, float diff,float dt );
void advect ( int N, int b, float *d, float *d0, float *u, float *v, float dt );
void diffuse ( int N, int b, float *x, float *x0, float diff, float dt );
void add_source ( int N, float *x, float *s, float dt );
void Clear_New_Erosion();
void New_Erosion_Move_Water(int nc,float CellSize);
void Add_Water_And_Height2(int nc,float CellSize, float *dens,float *edens, float *u_prev, float *v_prev );


///////////////////////////////////////////////////////////////
// simulation variables
///////////////////////////////////////////////////////////////

int gErosionShowWater = true;
int gErosionShowSed = false;
int gErosionShowVel = true;
int gErosionShowArrow = false;
int gErosionShowDep = false;

float gErosionStrength = 81.1f;
float mEvaporateValue = 0.0001f;
float gErosionVelocity = 6.9f;
float gErosionNumLoop = 2;
float gErosionViscosity = 0.16f;
float gErosionDiffusion = 0.1f;
float gErosionTimeStep = 0.1f;
float gErosionSedSat = 0.47f; // percent of water that is sed
float gErosionSedMax = 0.4f;// max amoount of sed
float gErosionShowHeight = 0.0f;// to render water
float gErosionMaxDrops = 10000.0f;
float gErosionDropSize = 0.01f;
float gErosionWaterMin = 0.0f; // lowest amount of water that allows sedement to be picked up


float mErosionPipeVisc = 0.001f;
float mErosionWaterMax = 10.0f;

float gErosionShowMinVel = 2.5;
float gErosionWaterMinVel = 0.31f;

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


void add_source ( int N, float *x, float *s, float dt )
{
	int i, size=(N+2)*(N+2);
	for ( i=0 ; i<size ; i++ ) {
		x[i] += dt*s[i];
	}
}



void diffuse ( int N, int b, float *x, float *x0, float diff, float dt )
{
	int i, j, k;
	float a=dt*diff*N*N;
	for ( k=0 ; k<20 ; k++ ) {
		for ( i=1 ; i<=N ; i++ ) {
			for ( j=1 ; j<=N ; j++ ) {
				x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/(1+4*a);
			}
		}
		set_bnd ( N, b, x );
	}
}

void advect ( int N, int b, float *d, float *d0, float *u, float *v, float dt )
{
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;
	dt0 = dt*N;
	for ( i=1 ; i<=N ; i++ ) {
		for ( j=1 ; j<=N ; j++ ) {
			x = i-dt0*u[IX(i,j)];
			y = j-dt0*v[IX(i,j)];
			if (x<0.5){
				x=0.5;
			}
			if (x>N+0.5){
				x=N+ 0.5;
			}
			i0=(int)x;
			i1=i0+1;
			if (y<0.5){
				y=0.5;
			}
			if (y>N+0.5){
				y=N+ 0.5;
			}
			j0=(int)y;
			j1=j0+1;
			s1 = x-i0;
			s0 = 1-s1;
			t1 = y-j0;
			t0 = 1-t1;
			d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
		}
	}
	set_bnd ( N, b, d );
}
#if 0
void dens_step ( int N, float *x, float *x0, float *u, float *v, float diff,float dt )
{
	add_source ( N, x, x0, dt );
	SWAP ( x0, x );
	diffuse ( N, 0, x, x0, diff, dt );
	SWAP ( x0, x );
	advect ( N, 0, x, x0, u, v, dt );
}
#endif

void dens_step ( int N, float *x, float *x0, float *u, float *v, float diff,float dt )
{
	advect ( N, 0, x, x0, u, v, dt );
}

void vel_step ( int N, float *u, float *v, float *u0, float *v0, float visc, float dt )
{

	add_source ( N, u, u0, dt );
	add_source ( N, v, v0, dt );
	SWAP ( u0, u );
	diffuse ( N, 1, u, u0, visc, dt );
	SWAP ( v0, v );
	diffuse ( N, 2, v, v0, visc, dt );
	project ( N, u, v, u0, v0 );
	SWAP ( u0, u );
	SWAP ( v0, v );
	advect ( N, 1, u, u0, u0, v0, dt );
	advect ( N, 2, v, v0, u0, v0, dt );
	project ( N, u, v, u0, v0 );
}



void project ( int N, float *u, float *v, float *p, float *div )
{
	int i, j, k;
	float h;
	h = 1.0/N;
	for ( i=1 ; i<=N ; i++ ) {
		for ( j=1 ; j<=N ; j++ ) {
			div[IX(i,j)] = -0.5*h*(u[IX(i+1,j)]-u[IX(i-1,j)]+v[IX(i,j+1)]-v[IX(i,j-1)]);
			p[IX(i,j)] = 0;
		}
	}
	set_bnd ( N, 0, div );
	set_bnd ( N, 0, p );
	for ( k=0 ; k<20 ; k++ ) {
		for ( i=1 ; i<=N ; i++ ) {
			for ( j=1 ; j<=N ; j++ ) {
				p[IX(i,j)] = (div[IX(i,j)]+p[IX(i-1,j)]+p[IX(i+1,j)]+p[IX(i,j-1)]+p[IX(i,j+1)])/4;
			}
		}
		set_bnd ( N, 0, p );
	}
	for ( i=1 ; i<=N ; i++ ) {
		for ( j=1 ; j<=N ; j++ ) {
			u[IX(i,j)] -= 0.5*(p[IX(i+1,j)]-p[IX(i-1,j)])/h;
			v[IX(i,j)] -= 0.5*(p[IX(i,j+1)]-p[IX(i,j-1)])/h;
		}
	}
	set_bnd ( N, 1, u );
	set_bnd ( N, 2, v );
}


void set_bnd ( int N, int b, float *x )
{
	int i;
	for ( i=1 ; i<=N ; i++ ) {
		x[IX(0 ,i)]		= b==1 ? -x[IX(1,i)] : x[IX(1,i)];
		x[IX(N+1,i)]	= b==1 ? -x[IX(N,i)] : x[IX(N,i)];
		x[IX(i,0 )]		= b==2 ? -x[IX(i,1)] : x[IX(i,1)];
		x[IX(i,N+1)]	= b==2 ? -x[IX(i,N)] : x[IX(i,N)];
	}
	x[IX(0 ,0 )]	= 0.5*(x[IX(1,0 )]+x[IX(0 ,1)]);
	x[IX(0 ,N+1)]	= 0.5*(x[IX(1,N+1)]+x[IX(0 ,N )]);
	x[IX(N+1,0 )]	= 0.5*(x[IX(N,0 )]+x[IX(N+1,1)]);
	x[IX(N+1,N+1)]	= 0.5*(x[IX(N,N+1)]+x[IX(N+1,N )]);
}





void Clear_New_Erosion()
{
	memset(u,0,sizeof(u));
	memset(v,0,sizeof(v));
	memset(dens2,0,sizeof(dens2));
	memset(dens2_prev,0,sizeof(dens2));
}

// now jus sed
void Add_Sed_And_Velocity(int nc,float CellSize, float *edens, float *u_prev, float *v_prev )
{

	// here we add any rain
	// then we update velociy to = water pressure.
	// water pressure is differentation from height (ground + water)


	// so calculate velocity based on height
	// then add to current velocity
	// then just copy water and veclocut over to the new structure.
	for(int x = 1; x < (nc*2)-1; x++){
		for(int y = 1; y < (nc*2)-1; y++){
			if(((x-nc) * (x-nc)) + ((y-nc) * (y-nc)) >= (nc-1) * (nc-1)){
				continue;
			}
			edens[IX(x,y)]	= mErosionSedMap[x][y].mAmount;
			u_prev[IX(x,y)]	=  mErosionWaterMap[x][y].mVelocityX;
			v_prev[IX(x,y)]	= mErosionWaterMap[x][y].mVelocityZ;
		}
	}
}

void Copy_Sed_And_Velocity_Back(int nc,float CellSize,float *edens)
{

	// clear out old values
	memset(mErosionSedMap,0,sizeof(mErosionSedMap));
	// ok, this is easy, just copy back the values
	for(int x = 1; x < (nc*2)-1; x++){
		for(int y = 1; y < (nc*2)-1; y++){
			if(((x-nc) * (x-nc)) + ((y-nc) * (y-nc)) >= (nc-1) * (nc-1)){
				continue;
			}
			mErosionSedMap[x][y].mAmount = edens[IX(x,y)];
		}
	}
}


#if 0
void New_Erosion_Move_Sed(int nc,float CellSize)
{
	float dt = gErosionTimeStep; // ??? this is time differential.. t
	float visc = gErosionViscosity; // viscosity? viscous diffusion rate
	float diff = gErosionDiffusion; //??? diffusion amount: how much density diffues accross the plane..


	// copy in current water.
	// add velocity based on gravity( which is realyl watter differental and height diff)

	memset(u_prev,0,sizeof(u_prev));
	memset(v_prev,0,sizeof(v_prev));
	memset(dens2,0,sizeof(dens2));
	memset(dens2_prev,0,sizeof(dens2));


	// copy current density over as well as add any velocity from water movement
	Add_Water_And_Height2 (nc,CellSize,dens2, u_prev, v_prev );

	// move the velocity field
	vel_step ( EROSION_N, u, v, u_prev, v_prev, visc, dt );
	// move the density using the velocity field
	dens_step ( EROSION_N, dens2, dens2_prev, u, v, diff, dt ); // sedement
	// now copy density and velocity back to starting structures.
	Copy_Water_Velocity_Back(nc,CellSize,dens2,u,v);
}
#endif

void Erosion_Move_Sed(int nc,float CellSize)
{
	float dt = gErosionTimeStep; // ??? this is time differential.. t
	float visc = gErosionViscosity; // viscosity? viscous diffusion rate
	float diff = gErosionDiffusion; //??? diffusion amount: how much density diffues accross the plane..


	// copy in current water.
	// add velocity based on gravity( which is realyl watter differental and height diff)

	memset(u,0,sizeof(u_prev));
	memset(v,0,sizeof(v_prev));
	memset(dens2_prev,0,sizeof(dens2));
	memset(dens2,0,sizeof(dens2));


	// copy current density over as well as add any velocity from water movement
	Add_Sed_And_Velocity(nc,CellSize,dens2, u, v );
	// move the density using the velocity field
	dens_step ( EROSION_N, dens2_prev,dens2, u, v, diff, dt ); // sedement
	// now copy density and velocity back to starting structures.
	Copy_Sed_And_Velocity_Back(nc,CellSize,dens2_prev);
}

/*
		ok, basic strategy:
			1) need height map for water: height of water and velocity vector(speed and dir)
			2) height map for sediment: same as above.

		Start with rain.

		Drop rain: fill height map with volume and zero velocity.
		move water: based on height differential (both geometry and water) velocity will increase
			use simple grid pipe to move water volumes: combine velocityies at end of loop.
		move sediment: pick up and drop based on speed, volume of water & sed, and height differentail of terrain.
		Calc evaportaion: for now, uniform water reduction.

		loop as needed.

		when done, drop the sediment.

		need to calc interactive terrain height as if the sedimate is dropped each frame iteration.
		but will do several iterations before drawing frame.

		for now, one to one mapping of grid cells to height map, but may change....
*/






void Clear_Erosion_Prims()
{
	mErosionMeshNum = 0;
	mErosionNumPrim = 0;
}

void Add_Erosion_Mesh(r3dPoint3D *p,int numpts,r3dColor24 color)
{
	uint16_t index[4] = {0,0,0,0};
	for(int t = 0; t < numpts;t++){
		int same = false;
		for(int l = 0; l < mErosionMeshNum;l++){
			if(p[t].AlmostEqual(mErosionMesh[l].Pos)){
				same = true;
				index[t] = l;
			}
			if(same){
				break;
			}
		}
		// if new vert then add and assign new index
		if(!same){
			index[t] = mErosionMeshNum;
			mErosionMesh[mErosionMeshNum].Pos = p[t];
			mErosionMesh[mErosionMeshNum].color = color;
			mErosionMesh[mErosionMeshNum].Normal = r3dPoint3D(0.0f, 1.0f, 0.0f);
			mErosionMeshNum++;
		}
	}
	if(numpts == 4){
		// add two triangles
		mErosionIndicies[(mErosionNumPrim * 3) + 0] = index[0];
		mErosionIndicies[(mErosionNumPrim * 3) + 1] = index[3];
		mErosionIndicies[(mErosionNumPrim * 3) + 2] = index[2];
		mErosionNumPrim++;
	}
	mErosionIndicies[(mErosionNumPrim * 3) + 0] = index[0];
	mErosionIndicies[(mErosionNumPrim * 3) + 1] = index[2];
	mErosionIndicies[(mErosionNumPrim * 3) + 2] = index[1];
	mErosionNumPrim++;
}


void Add_Erosion_Mesh2(r3dPoint3D *p,int numpts,r3dColor24 color)
{
	uint16_t index[4] = {0,0,0,0};
	for(int t = 0; t < numpts;t++){
		int same = false;
		for(int l = 0; l < mErosionMeshNum;l++){
			r3dPoint3D pp;
			pp.x = color.R;
			pp.y = color.G;
			pp.z = color.B;
			r3dPoint3D pp2;
			pp2.x = mErosionMesh[l].color.R;
			pp2.y = mErosionMesh[l].color.G;
			pp2.z = mErosionMesh[l].color.B;
			if(p[t].AlmostEqual(mErosionMesh[l].Pos) && pp.AlmostEqual(pp2)){
				same = true;
				index[t] = l;
			}
			if(same){
				break;
			}
		}
		// if new vert then add and assign new index
		if(!same){
			index[t] = mErosionMeshNum;
			mErosionMesh[mErosionMeshNum].Pos = p[t];
			mErosionMesh[mErosionMeshNum].color = color;
			mErosionMeshNum++;
			assert(mErosionMeshNum < MAX_EROSION_VERT);
		}
	}
	mErosionIndicies[(mErosionNumPrim * 3) + 0] = index[0];
	mErosionIndicies[(mErosionNumPrim * 3) + 1] = index[2];
	mErosionIndicies[(mErosionNumPrim * 3) + 2] = index[1];
	mErosionNumPrim++;
	assert(mErosionNumPrim * 3 < MAX_EROSION_IDX);
}


void Erosion_Do_Rain(int nc,int numdrops, float Hardness)
{
	// hardness is used as percent of nc to actually use.

	// so
	//memset(mErosionRainMap,0,sizeof(mErosionRainMap));
	for(int t = 0; t < numdrops; t++){
		// pick a random cell
		int xx,yy;
		while(1){
			// ok, so this is search space
			// should be circle that fits in grid, so radius must be wid/high
			// do -1 on all sides

			int num = nc * 2 * Hardness;

			xx = random(num) + (((nc * 2) - num )/ 2);
			yy = random(num) + (((nc * 2) - num )/ 2);

			if(((xx-nc) * (xx-nc)) + ((yy-nc) * (yy-nc)) < ((nc-1)*Hardness) * ((nc-1) * Hardness)){
				break;
			}
		}


		// this makes erosion grid easy: value - start:)
		//mErosionRainMap[xx][yy]+= mDropHeight;
		int size = (int)((gErosionDropSize * 0.9f) * 100.0f);
		mErosionWaterMap[xx][yy].mHeight+= (float)(random(size) * 0.001f) + (gErosionDropSize * 0.1);
	}
}

void Erosion_Add_Water_Render(int x,int y,int x1,int y1,float CellSize,float ratio = 1.0f)
{
	// add an erosion prim before evaporation
	float mWaterScale = 3.0;
	float mCellScale = 0.25;
	r3dPoint3D p[4];
	r3dPoint3D vec;
	r3dPoint3D center;
	r3dColor24 color;
	vec.y = 0;
	vec.x = mErosionWaterMap[x][y].mVelocityX;
	vec.z = mErosionWaterMap[x][y].mVelocityZ;
	color.R = 0;
	color.G = 20;
	color.B = 255;
	int dovel = false;
	float a = (mErosionWaterMap[x][y].mHeight - mErosionPipeVisc) * 255;
	if(gErosionShowVel){
		a = vec.LengthSq() * 0.1 * 255; // could use vel!!! BP
	}
	if(gErosionShowArrow){
		dovel = true;
	}
	if(gErosionShowSed){
		if(mErosionSedMap[x][y].mAmount > 0.0){
			color.R = 0;
			color.G = 255;
			color.B = 20;
			a = mErosionSedMap[x][y].mAmount * 255.0;
		}
	}
	if(gErosionShowDep){
		if(mErosionDepositMap[x][y] > 0.0){
			color.R = 200;
			color.B = 10;
			color.G = 10;
			a = mErosionDepositMap[x][y] * 255.0 * 50.0;
		}
	}
	if(dovel){
		a = 170;
	}
	if(a > 200){
		a = 200;
	}else if(a < 3){
		a = 3;
	}
	color.A = a;
	center.x = (x1 + x) * CellSize;
	center.y = mErosionWaterMap[x][y].mHeight + mErosionHeightMap[x][y] + 0.1;
	center.z = (y1 + y) * CellSize;
	if(!dovel || (fabs(mErosionWaterMap[x][y].mVelocityX) < 0.0001 && fabs(mErosionWaterMap[x][y].mVelocityZ) < 0.0001)){
		mCellScale = 1.0;

		p[0].x = center.x - (CellSize * mCellScale * 0.5);
		p[0].y = mErosionWaterMap[x][y].mHeight + mErosionHeightMap[x][y] + 0.1;
		p[0].z = center.z - (CellSize * mCellScale * 0.5);

		// right top
		p[1].x = center.x + (CellSize * mCellScale * 0.5);
		p[1].y = mErosionWaterMap[x+1][y].mHeight + mErosionHeightMap[x+1][y] + 0.1;
		p[1].z = center.z - (CellSize * mCellScale * 0.5);

		// right bottom
		p[2].x = center.x + (CellSize * mCellScale * 0.5);
		p[2].y = mErosionWaterMap[x+1][y+1].mHeight + mErosionHeightMap[x+1][y+1] + 0.1;
		p[2].z = center.z + (CellSize * mCellScale * 0.5);

		// left bottom
		p[3].x = center.x - (CellSize * mCellScale * 0.5);
		p[3].y = mErosionWaterMap[x][y+1].mHeight + mErosionHeightMap[x][y+1] + 0.1;
		p[3].z = center.z + (CellSize * mCellScale * 0.5);

		Add_Erosion_Mesh(p,4,color);
		return;
	}
	// dont do these unless doing arrows... slow it down :)
	mCellScale = 0.5;
	vec.Normalize();
	r3dPoint3D vec2 = vec;
	r3dPoint3D vec3 = vec;
	vec2.RotateAroundY(90);
	vec3.RotateAroundY(270);
	// arrow tip
	p[0].x = center.x + (vec.x * 3.0  * CellSize * mCellScale);
	p[0].y = center.y;
	p[0].z = center.z + (vec.z * 3.0  * CellSize * mCellScale);

	p[2].x = center.x + (vec2.x * 0.5 * CellSize * mCellScale);
	p[2].y = center.y;
	p[2].z = center.z + (vec2.z * 0.5 * CellSize * mCellScale);

	p[1].x = center.x + (vec3.x * 0.5 * CellSize * mCellScale);
	p[1].y = center.y;
	p[1].z = center.z + (vec3.z * 0.5 * CellSize * mCellScale);

	Add_Erosion_Mesh(p,3,color);

}
float gErosionHardness = 1.0f;

void Erosion_Move_Water_Pipe(int nc,int x1,int y1,int numloops,float CellSize)
{

	// MUST BE CLOCKWIZE for index+2%4 to work to get opposite side i.e my up is your down my left is your right
	int adjmap[4][2] = {
		{0,1}, // up
		{1,0}, // right
		{0,-1}, // down
		{-1,0} // left
	};
	int numpass = 100;
	// clear out velocity
	for(int x = 1; x < (nc*2)-1; x++){
		for(int y = 1; y < (nc*2)-1; y++){
			// clear out velocitys while im here
			mErosionWaterMap[x][y].mVelocityX = 0.0f;
			mErosionWaterMap[x][y].mVelocityZ = 0.0f;
		}
	}

	// get area of circle
	float area = nc * CellSize * 2.0 * R3D_PI;
	int numdrops = (int)( area * gErosionStrength * 1.0f);

	int maxdrops = gErosionMaxDrops * 10.0f;
	if(numdrops > maxdrops){
		numdrops = maxdrops;
	}
	for(int t=  0; t < numpass; t++){
		// drop some rain
		Erosion_Do_Rain(nc,numdrops/10.0f,gErosionHardness);
		// first pass, figure out "out flux": how my water want to move towards my neighbors
		// second pass, move all the water
		for(int x = 1; x < (nc*2)-1; x++){
			for(int y = 1; y < (nc*2)-1; y++){
				if(mErosionWaterMap[x][y].mHeight <= 0.0){
					continue;
				}
				if(((x-nc) * (x-nc)) + ((y-nc) * (y-nc)) >= (nc-1) * (nc-1)){
					continue;
				}
				// clamp high values
				if(mErosionWaterMap[x][y].mHeight > mErosionWaterMax){
					mErosionWaterMap[x][y].mHeight = mErosionWaterMax;
				}
				// out flux is simple: how much lower/higher are you only caculate out flow, so only if lower
				// to simplify things later, store total in/out. if this is greater than my total, then I need to scale it
				float totalflux = 0.0;
				for(int fl = 0; fl < 4; fl++){

					// flux is simple to calc: height diff/2. that way: diff of 2, means I give you 1 and we are even 4 2 = 4-1 2+1 = 3 3
					float diff = (mErosionWaterMap[x][y].mHeight + mErosionHeightMap[x][y]) - (mErosionWaterMap[x + adjmap[fl][0]][y + adjmap[fl][1]].mHeight + mErosionHeightMap[x + adjmap[fl][0]][y + adjmap[fl][1]]);

					// when moveing to a place with no water then dont do so unless I am at least above viscosity value
					if(mErosionWaterMap[x + adjmap[fl][0]][y + adjmap[fl][1]].mHeight <= 0.0 && diff < mErosionPipeVisc){
						diff = 0.0;
					}
					if(diff < 0.0){
						diff = 0.0;
					}
					// add +- 10% random to the water flow.. doesnt help a lot :(
					diff *= 0.9 + (((float)random(20)) * 0.1);
					mErosionWaterMap[x][y].mOutFlux[fl] = diff/2.0f;
					totalflux += mErosionWaterMap[x][y].mOutFlux[fl];
				}
				if(totalflux <= 0.0f){
					continue;
				}
				float mMaxWaterMove = mErosionWaterMap[x][y].mHeight * 0.3f * 0.5f;
				float ratio = mMaxWaterMove / totalflux;
				// scale all down by this amount
				for(int fl = 0; fl < 4; fl++){
					mErosionWaterMap[x][y].mOutFlux[fl] *=ratio;
				}
			}
		}
		// now apply all the flux values: guranteed to not lose mass, or have negative cells :)
		for(int x = 1; x < (nc*2)-1; x++){
			for(int y = 1; y < (nc*2)-1; y++){
				if(mErosionWaterMap[x][y].mHeight <= 0.0){
					continue;
				}
				if(((x-nc) * (x-nc)) + ((y-nc) * (y-nc)) >= (nc-1) * (nc-1)){
					// clear out borders so water will drain out
					mErosionWaterMap[x][y].mHeight = 0.0;
					mErosionSedMap[x][y].mAmount = 0.0;
					continue;
				}
				for(int fl = 0; fl < 4; fl++){
					float value = mErosionWaterMap[x][y].mOutFlux[fl];
					if(!value){
						continue;
					}
					mErosionWaterMap[x][y].mHeight -= value;
					mErosionWaterMap[x + adjmap[fl][0]][y + adjmap[fl][1]].mHeight += value;

					// give each half the velocity
					mErosionWaterMap[x][y].mVelocityX += value * adjmap[fl][0] * gErosionVelocity;
					mErosionWaterMap[x][y].mVelocityZ += value * adjmap[fl][1] * gErosionVelocity;
					//mErosionWaterMap[x + adjmap[fl][0]][y + adjmap[fl][1]].mVelocityX += value * adjmap[fl][0] * 0.5f * gErosionVelocity;
					//mErosionWaterMap[x + adjmap[fl][0]][y + adjmap[fl][1]].mVelocityZ += value * adjmap[fl][1] * 0.5f * gErosionVelocity;

				}
			}
		}
	}
	for(int x = 1; x < (nc*2)-1; x++){
		for(int y = 1; y < (nc*2)-1; y++){
			if(mErosionWaterMap[x][y].mHeight <= 0.0){
				continue;
			}
			r3dPoint3D vec(mErosionWaterMap[x][y].mVelocityX,mErosionWaterMap[x][y].mVelocityZ,0);
			float vel = vec.Length();
			mErosionWaterMap[x][y].mVel = vel;
		}
	}
}



void Erosion_Render(int nc,int x1,int y1,float CellSize)
{
	for(int x = 1; x < (nc*2)-1; x++){
		for(int y = 1; y < (nc*2)-1; y++){

			if(mErosionWaterMap[x][y].mVel <=gErosionShowMinVel){ // hide low water...
				continue;
			}
			if(((x-nc) * (x-nc)) + ((y-nc) * (y-nc)) >= (nc-1) * (nc-1)){
				continue;
			}
			Erosion_Add_Water_Render(x,y,x1,y1,CellSize);
		}
	}
}


void Copy_Height_Field(Floats *heightfielddata,int Width,int nc,int x1,int y1);


// gets midpoint height of x,y + x+1,y + x+1,y+1 + x+1,y
float Erosion_Get_Midpoint_Height(int x,int y)
{
	float h1 = mErosionHeightMap[x][y] + mErosionHeightMap[x+1][y] + mErosionHeightMap[x+1][y+1] + mErosionHeightMap[x][y+1];
	return h1 * 0.25f;
}

float Erosion_Midpoint_Height(int x,int y,int corner)
{
	switch(corner){
		case 0:
			return Erosion_Get_Midpoint_Height(x - 1,y - 1);
		case 1:
			return Erosion_Get_Midpoint_Height(x ,y - 1);
		case 2:
			return Erosion_Get_Midpoint_Height(x ,y);
		case 3:
			return Erosion_Get_Midpoint_Height(x - 1 ,y);
	}
	return 0.0f;
}

void Copy_Height_Field_Smooth(Floats *heightfielddata,int Width,int nc,int x1,int y1);

void Erosion_Update_Sed(int nc,int x1,int y1,float CellSize,Floats *HeightFieldData,int Width)
{
#define max(x,y)		(x) > (y) ? (x) : (y)
	// add sed

	// this is to track deposits and take aways to be applied laytr
	memset(mErosionDropMap,0,sizeof(mErosionDepositMap));

	for(int x = 1; x < (nc*2)-1; x++){
		for(int y = 1; y < (nc*2)-1; y++){
			// need at least this much water to cause movement
			//if(mErosionWaterMap[x][y].mHeight < gErosionWaterMin){
			//	continue;
			//}
			if(((x-nc) * (x-nc)) + ((y-nc) * (y-nc)) >= (nc-1) * (nc-1)){
				continue;
			}
			// amount of sat based on velocity
#if 0		// faster!!
			float sedsat = gErosionSedSat * (fabs(mErosionWaterMap[x][y].mVelocityX) + fabs(mErosionWaterMap[x][y].mVelocityZ));
			//float sedsat = gErosionSedSat * max(fabs(mErosionWaterMap[x][y].mVelocityX) ,fabs(mErosionWaterMap[x][y].mVelocityZ));
#else
			float vel = mErosionWaterMap[x][y].mVel;
			// min velocity in order to move sed
			if(vel < gErosionWaterMinVel){
				continue;
			}
			float randval = 0.9f + (((float)random(20)) * 0.1f);
			float sedsat = gErosionSedSat * vel * randval; // WAY SLOW!!
#endif
			int adjmap[4][2] = {
				{-1,0},
				{0,-1},
				{0,1},
				{1,0},
			};
			int adjmap9[8][2] = {
				{-1,-1},
				{-1,0},
				{-1,1},

				{0,-1},
				{0,1},

				{1,-1},
				{1,0},
				{1,1},
			};
			// ok, based on angle, so take largest diff and use that as value
			float maxdiff = 0; // for taking
			// never take if I am lower than any neigbor!
			int isneg = true;
			for(int ll = 0; ll < 8; ll++){
				// get diff from me and midpoints around me
				//float ndiff =  mErosionHeightMap[x][y] - Erosion_Midpoint_Height(x,y,ll);
				float ndiff =  mErosionHeightMap[x][y] - mErosionHeightMap[x+adjmap9[ll][0]][y+adjmap9[ll][1]];
				if(ndiff < 0.0f){
					if(-ndiff > maxdiff){
						maxdiff = -ndiff;
						isneg = true;
					}
				}else if(ndiff > maxdiff){
					maxdiff = ndiff;
					isneg = false;
				}
			}
			if(isneg){
				maxdiff *=-1.0f;
			}
			// get angle of the cell
			float angle = atan2(maxdiff,1.0f);
			//float angle = atan2(1.0f,maxdiff);
			assert(R3D_DEG2RAD(angle) >= -90 && R3D_DEG2RAD(angle) <= 90);
			assert(R3D_DEG2RAD(angle) <= 180 && R3D_DEG2RAD(angle) >= -180);
			float sa = fabs(sin(angle));
			assert(sa >= 0.0f && sa <= 1.0f);
			// multiply times the sed saturation value

			//sa = 1.0f; // BP DONT USE ANGLE!!!!!!!!
			sedsat *=sa;
			if(sedsat < 0.0f){
				continue;
			}

			// make sure it's less than max value
			if(sedsat > gErosionSedMax){
				sedsat = gErosionSedMax;
			}
			// get differeance from current saturated sed
			float amount = sedsat - mErosionSedMap[x][y].mAmount;

			// positive amount, means that current sed saturation is low, so we will take some from terrain
			// negative amount, means that current sed saturation is too high, so we will add some to terrain


			// need some simple constraints...
			// after a certian angle, will not take or add
			// this keeps more natural formations

#if 1
			// if angle too high, dont add
			if(angle > R3D_DEG2RAD(10) && amount < 0.0){
				continue;
			}
			// if angle too low, dont subtract
			if(angle < R3D_DEG2RAD(-10) && amount > 0.0){
				continue;
			}
#endif
			if(amount < 0.0){
				mErosionDepositMap[x][y]-=amount; // update deposit map
			}
			// save for later..
			mErosionDropMap[x][y] += amount;
			mErosionSedMap[x][y].mAmount += amount;
		}
	}
	// clean terrain before copy?

	Copy_Height_Field_Smooth(HeightFieldData,Width,nc,x1,y1);
}



		// evaporate
void Erosion_Evaporate(int nc,int x1,int y1,float CellSize,Floats *HeightFieldData,int Width)
{
	for(int x = 0; x < nc*2; x++){
		for(int y = 0; y < nc*2; y++){
			if(((x-nc) * (x-nc)) + ((y-nc) * (y-nc)) > (nc-1) * (nc-1)){
				continue;
			}
			mErosionWaterMap[x][y].mHeight -= mEvaporateValue * 0.1; // down by 10th...
			// if killed all water here
			if(mErosionWaterMap[x][y].mHeight <= 0.0){
				mErosionWaterMap[x][y].mHeight = 0.0;
				// drop of the sed that was here if any
				(*HeightFieldData)[ (y + y1) * int(Width) + (x+x1) ] += mErosionSedMap[x][y].mAmount;
				mErosionHeightMap[x][y]+=mErosionSedMap[x][y].mAmount;
				mErosionDepositMap[x][y]+=mErosionSedMap[x][y].mAmount; // add to deposit map
				mErosionSedMap[x][y].mAmount = 0.0f;
			}
		}
	}
}

void Clear_Erosion()
{
	memset(mErosionDepositMap,0,sizeof(mErosionDepositMap));
	memset(mErosionWaterMap,0,sizeof(mErosionWaterMap));
	memset(mErosionSedMap,0,sizeof(mErosionSedMap));
	Clear_New_Erosion();
}

void Copy_Height_Field(Floats *heightfielddata,int Width,int nc,int x1,int y1)
{
	// save off the current height map
	for(int x = 0; x < nc*2; x++){
		for(int y = 0; y < nc*2; y++){
			mErosionHeightMap[x][y] = (*heightfielddata)[ (y + y1) * int(Width) + (x+x1) ];
		}
	}
}

void Copy_Height_Field_Smooth(Floats *heightfielddata,int Width,int nc,int x1,int y1)
{

	// apply the height changes first.
	for(int x = 0; x < nc*2; x++){
		for(int y = 0; y < nc*2; y++){
			float amount = mErosionDropMap[x][y];
			(*heightfielddata)[ (y + y1) * int(Width) + (x+x1) ] -= amount;
		}
	}

	for(int x = 1; x < nc*2; x++){
		for(int y = 1; y < nc*2; y++){
			if(((x-nc) * (x-nc)) + ((y-nc) * (y-nc)) > (nc-1) * (nc-1)){
				continue;
			}
			if(mErosionDropMap[x][y] == 0.0){
				continue;
			}
			int adjmap9[9][2]={
				{-1,-1},
				{-1,0},
				{-1,1},

				{0,-1},
				{0,0},
				{0,1},

				{1,-1},
				{1,0},
				{1,1},

			};
			float h[9];
			for(int ll = 0; ll < 9; ll++){
				h[ll] = (*heightfielddata)[ (y + y1 + adjmap9[ll][1]) * int(Width) + (x+x1 + adjmap9[ll][0]) ];
			}
			// look for large disparity.
			// on cross lines... i.e, any line in the grid should be smooth not up/down/up or down/up/down

			int smoothlines[4][3]={
				{0,4,8},
				{1,4,7},
				{2,4,6},
				{3,4,5},
			};
			float maxdiff = 0.01f;
			for(int ll = 0; ll < 4; ll++){
				int l1 = h[smoothlines[ll][0]];
				int l2 = h[smoothlines[ll][2]];
				float diff1 = l1 - h[4]; // minus center
				float diff2 = l2 - h[4]; // minus center
				if(diff1 > maxdiff && diff2 > maxdiff){
					// both higher
					(*heightfielddata)[ (y + y1) * int(Width) + (x+x1) ] += maxdiff;
				}else if(diff1 < -maxdiff && diff2 < -maxdiff){
					// both lower
					(*heightfielddata)[ (y + y1) * int(Width) + (x+x1) ] += -maxdiff;
				}
			}
		}
	}
	Copy_Height_Field(heightfielddata,Width,nc,x1,y1);
}

void Old_Copy_Height_Field_Smooth(Floats *heightfielddata,int Width,int nc,int x1,int y1)
{
	// save off the current height map
	for(int x = 0; x < nc*2; x++){
		for(int y = 0; y < nc*2; y++){
			int adjmap9[9][2]={
				{-1,-1},
				{-1,0},
				{-1,1},

				{0,-1},
				{0,0},
				{0,1},

				{1,-1},
				{1,0},
				{1,1},

			};
#if 1
			float appmap9[9]={
				0.10f,	0.10f,	0.10f,
				0.10f,	0.10f,	0.10f,
				0.10f,	0.10f,	0.10f
			};
#else
			float appmap9[9]={
				0.05f,	0.12f,	0.05f,
				0.12f,	0.20f,	0.12f,
				0.05f,	0.12f,	0.05f
			};
#endif
			float oamount = mErosionDropMap[x][y];
			for(int ll = 0; ll < 9; ll++){
				float amount = oamount * appmap9[ll];
				(*heightfielddata)[ (y + y1+adjmap9[ll][1]) * int(Width) + (x+x1+adjmap9[ll][0]) ] -= amount;
			}
		}
	}
	Copy_Height_Field(heightfielddata,Width,nc,x1,y1);
}
int mErosionPreview = false;
