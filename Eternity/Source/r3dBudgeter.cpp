#include "r3dPCH.h"

#if !DISABLE_BUDGETER

#include "r3d.h"
#include "r3dBudgeter.h"

const float GAUGE_HEIGHT = 12.0f;
const float GAUGE_COEF = 1.3f ;

static void Text_Print(float x, float y, const r3dColor& color, const char *message, ...);

void ValueToText(int category, int value, char* text)
{
	int K = (	category == TexMem				||
				category == PlayerTexMem		||
				category == BufferMem			||
				category == PlayerBufferMem		||
				category == TerrainBufferMem	||
				category == GrassBufferMem		||
				category == OtherBufferMem		||
				category == RenderTargetMem		||
				category == PlayerTotalMem		||
				category == VMemTotal
				) ? 1024 : 1000;

	int m = value / (K * K);
	int k = value / (K);

	if(m > 0)
	{
		sprintf(text, (K == 1024) ? "%.2f Mb" : "%.2f M", ((float)value) / (K*K) );
	}
	else if( k > 0)
	{
		sprintf(text, (K == 1024) ? "%.2f Kb" : "%.2f K", ((float)value) / K);
	}
	else
	{
		sprintf(text, "%d", value);
	}
}

static const char* captions[BUDGET_CATEGORY_COUNT] = 
{
	"TexMem",
	"    PlayerTexMem",
	"    UITexMem",
	"BufferMem",
	"    PlayerBufferMem",
	"    TerrainBufferMem", // terrain, grass and other are already included in BufferMem, so show in budgeter as it is already included
	"    GrassBufferMem",
	"    OtherBufferMem",
	"    UIBufferMem",
	"RenderTargetMem",
	"PlayerTotalMem",
	"Video Memory",
	"NumMaterialChanges",
	"NumModesChanges",
	"NumTextureChanges",
	"NumTrianglesRendered",
	"AverageStripLength",
	"NumDraws",
	"NumTerrainDraws",
	"NumTerrainTris",
	"NumOcclusionQueries",
	"NumParticlesRendered",
	"NumLocks",
	"NumNooverwriteLock",
	"NumDiscardLocks",
	"NumBytesLocked",
	"NumVisGridOcclusions",
	"FrameTime",
};

#ifdef USE_R3D_MEMORY_ALLOCATOR
const char * const memoryTypes[ALLOC_TYPE_TOTAL] = 
{
	"Gfx: ",
	"Game: ",
	"Physx: ",
	"Scaleform: ",
	"Nav: "
};
#endif //USE_R3D_MEMORY_ALLOCATOR

void r3dBudgeter::Data::Defaults()
{
	values[NumDraws]				= 4000; 
	values[TexMem]					= (512)*1024*1024;
	values[PlayerTexMem]			= (192)*1024*1024;
	values[UITexMem]				= (128)*1024*1024;
	values[RenderTargetMem]			= (128)*1024*1024;
	values[BufferMem]				= 256*1024*1024;
	values[PlayerBufferMem]			= 32*1024*1024;
	values[GrassBufferMem]			= 32*1024*1024;
	values[TerrainBufferMem]		= 128*1024*1024;
	values[OtherBufferMem]			= 32*1024*1024;
	values[UIBufferMem]				= 32*1024*1024;
	values[PlayerTotalMem]			= 128*1024*1024;
	values[VMemTotal]				= 512*1024*1024;
	values[NumMaterialChanges]		= 500;
	values[NumModesChanges]			= 1000;
	values[NumTextureChanges]		= 2000;
	values[NumTrianglesRendered]	= 5*1024*1024;
	values[AverageStripLength]		= 1000;
	values[NumTerrainDraws]			= 500;
	values[NumTerrainTris]			= 512*1024;
	values[NumOcclusionQueries]		= 1000;
	values[NumParticlesRendered]	= 10000;
	values[NumLocks]				= 768;
	values[NumNooverwriteLock]		= 768;
	values[NumDiscardLocks]			= 32;
	values[NumBytesLocked]			= 4*1024*1024;
	values[NumVisGridOcclusions]	= 100 ;
	values[FrameTime]				= 33;
}

void r3dBudgeter::Data::Save(const char* fileName) const
{
	for (int i = 0; i < BUDGET_CATEGORY_COUNT; ++i)
	{
		r3dWriteCFG_I(fileName, "budget", captions[i], values[i]);
	}	
}

void r3dBudgeter::Data::Load(const char* fileName)
{
	Defaults();

	for (int i = 0; i < BUDGET_CATEGORY_COUNT; ++i)
	{
		if( r3dCFGHas( fileName, "budget", captions[i] ) )
		{
			values[i] = r3dReadCFG_I(fileName, "budget", captions[i], values[i]);
		}
	}
}

void r3dBudgeter::Init(const char* fileName)
{
	if(r3d_access(fileName, 0) == 0)
	{
		limitData.Load(fileName);
	}
	else
	{
		limitData.Defaults();
		limitData.Save(fileName);
	}
}

void r3dBudgeter::Draw()
{
	UpdateCurrent();
	if(r_show_budgeter->GetBool())
	{
		const float PROFILE_TOP_Y = 140.0f;
		const float PROFILE_LEFT_BORDER = 10.0f;
		const float PROFILE_RIGHT_BORDER = 400.0f;
		const float PROFILE_WIDTH = r3dRenderer->ScreenW-PROFILE_RIGHT_BORDER-PROFILE_LEFT_BORDER ;
		const float PROFILE_HEIGHT = GAUGE_HEIGHT * GAUGE_COEF * ( BUDGET_CATEGORY_COUNT + 2 ) ;


		r3dRenderer->SetVertexShader();
		r3dRenderer->SetPixelShader();
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
		r3dDrawBox2DNoTex(PROFILE_LEFT_BORDER, PROFILE_TOP_Y, PROFILE_WIDTH, PROFILE_HEIGHT, r3dColor(30,30,30,170));
		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

		float yPos = 0.0f;
		for (int i = 0, index = 0; i < BUDGET_CATEGORY_COUNT; ++i)
		{
			if(!IsSkipped(i))
			{
				yPos = DrawGauge(i, index);
			}
			
			if(!IsSkipped(i+1))
				++index;
		}

		//	Draw memory statistics
#ifdef USE_R3D_MEMORY_ALLOCATOR
		float xStart = PROFILE_LEFT_BORDER + 10;
		Text_Print(xStart, yPos + 22.0f, r3dColor::white, "Memory");
		xStart += 70.0f;
		for (int i = 0; i < ALLOC_TYPE_TOTAL; ++i)
		{
			const r3dAllocatorMemoryStats &ms = r3dGetAllocatorStats(static_cast<r3dAllocationTypes>(i));
			float kb = ms.totalAllocatedMemory / 1024.0f;
			const char *suffix = "KB";
			if (kb > 1024.0f)
			{
				suffix = "MB";
				kb /= 1024.0f;
			}
			Text_Print(xStart, yPos + 22.0f, r3dColor::white, "%s%.2f%s", memoryTypes[i], kb, suffix);
			xStart += 170.0f;
		}
#endif
	}	
}

void r3dBudgeter::UpdateCurrent()
{
	const r3dGPUStats& s = r3dRenderer->Stats;

	currentData.values[NumDraws]				= s.GetNumDraws() - s.GetNumTerrainDraws();
	currentData.values[TexMem]					= s.GetTexMem();
	currentData.values[PlayerTexMem]			= s.GetPlayerTexMem();
	currentData.values[UITexMem]				= s.GetUITexMem();
	currentData.values[BufferMem]				= s.GetBufferMem();
	currentData.values[PlayerBufferMem]			= s.GetPlayerBufferMem();
	currentData.values[TerrainBufferMem]		= s.GetTerrainBufferDXMem();
	currentData.values[GrassBufferMem]			= s.GetGrassBufferDXMem();
	currentData.values[OtherBufferMem]			= s.GetOtherBufferDXMem();
	currentData.values[UIBufferMem]				= s.GetUIBufferMem();
	currentData.values[RenderTargetMem]			= s.GetRenderTargetMem();
	currentData.values[PlayerTotalMem]			= s.GetPlayerTexMem() + s.GetPlayerBufferMem();
	currentData.values[VMemTotal]				= s.GetVMemTotal();
	currentData.values[NumMaterialChanges]		= s.GetNumMaterialChanges();
	currentData.values[NumModesChanges]			= s.GetNumModesChanges();
	currentData.values[NumTextureChanges]		= s.GetNumTextureChanges();
	currentData.values[NumTrianglesRendered]	= s.GetNumTrianglesRendered();
	currentData.values[AverageStripLength]		= s.GetAverageStripLength();
	currentData.values[NumTerrainDraws]			= s.GetNumTerrainDraws();
	currentData.values[NumTerrainTris]			= s.GetNumTerrainTris();
	currentData.values[NumOcclusionQueries]		= s.GetNumOcclusionQueries();
	currentData.values[NumParticlesRendered]	= s.GetNumParticlesRendered();
	currentData.values[NumLocks]				= s.GetNumLocks();
	currentData.values[NumNooverwriteLock]		= s.GetNumLocksNooverwrite();
	currentData.values[NumDiscardLocks]			= s.GetNumLocksDiscard();
	currentData.values[NumBytesLocked]			= s.GetBytesLocked();
	currentData.values[NumVisGridOcclusions]	= s.GetNumVisGridOcclusions();

	currentData.values[FrameTime]				= (int)(r3dGetFrameTime()*1000.0f);
}

static void Text_Print(float x, float y, const r3dColor& color, const char *message, ...)
{
	va_list	va;
	char		buffer[1000];
	va_start(va, message);
	vsprintf(buffer, message, va);
	va_end(va);
	_r3dSystemFont->PrintF(x,y, color,buffer);
}

float r3dBudgeter::DrawGauge(int category, int index)
{
	const float maxScreenWidth = 1280.0f;
	float screenWidth = std::min(maxScreenWidth, r3dRenderer->ScreenW);

	const float PROFILE_TOP_Y = 140.0f;
	const float PROFILE_LEFT_BORDER = 10.0f;
	const float PROFILE_RIGHT_BORDER = 400.0f;
	const float PROFILE_WIDTH = screenWidth-PROFILE_RIGHT_BORDER-PROFILE_LEFT_BORDER;

	const float GAUGE_WIDTH = screenWidth-PROFILE_RIGHT_BORDER-PROFILE_LEFT_BORDER;
	const float GAUGE_LEFT = 200;
	const float GAUGE_RIGHT = 100;
	const float GAUGE_TOP = 20;

	float yPos = (PROFILE_TOP_Y + GAUGE_TOP + index*GAUGE_HEIGHT * GAUGE_COEF);
	float xPos = (PROFILE_LEFT_BORDER + GAUGE_LEFT);
	
	float ySize = GAUGE_HEIGHT;
	float xSize = (GAUGE_WIDTH - GAUGE_LEFT - GAUGE_RIGHT);

	r3dColor c = r3dColor(30, 170, 30,255);
	float value = GetValue(category, c);
	float xSizeValue = xSize * value;


	r3dDrawBox2DNoTex(xPos, yPos, xSizeValue, ySize, c );
	r3dDrawBox2DWireframe(xPos, yPos, xSize, ySize, r3dColor(255,255,255) );

	float xPosText = (PROFILE_LEFT_BORDER);
	Text_Print(xPosText, yPos, r3dColor::white, captions[category]);

	char valueText[64];
	ValueToText(category, currentData.values[category], valueText);
	Text_Print(xPos + xSize + 5, yPos, r3dColor::white, valueText);
	
	return yPos;
}

float r3dBudgeter::GetValue(int category, r3dColor& color) const
{
	color = r3dColor(30, 170, 30,255);
	float v = (float)currentData.values[category] / limitData.values[category];
	if(v > 0.8f) color = r3dColor(170, 170, 30,255);
	
	bool extended = v > 1.0f;

	if(extended) color = r3dColor(170, 40, 30,255);

	return extended ? 1.0f : v;
}

bool r3dBudgeter::IsSkipped(int category)
{
	return category == AverageStripLength;
}

r3dBudgeter gBudgeter;

#endif