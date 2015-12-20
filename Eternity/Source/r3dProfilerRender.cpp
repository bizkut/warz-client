#include "r3dPCH.h"

#if !DISABLE_PROFILER

#include "r3d.h"

#include "r3dStats.h"

#include "r3dBackgroundTaskDispatcher.h"

#include "r3dProfilerRender.h"
#include "r3dHeap.h"
#include "UI/UIimEdit.h"

void RenderScaleformProfiler();
void RenderGrassStats();
void PrintBackGroundTasks();

r3dProfileRender* r3dProfileRender::sInstance = NULL;

const float PROFILE_LEGEND_Y = 172.0f;
const float PROFILE_TOP_Y = PROFILE_LEGEND_Y-40.0f;
const float PROFILE_DATA_START_Y = PROFILE_LEGEND_Y+25.0f;
const float PROFILE_HEIGHT = 500.0f;
const float PROFILE_LEFT_BORDER = 10.0f;
const float PROFILE_VIEW_LINE_JUMP = 15.0f;
const float PROFILE_RIGHT_BORDER = 400.0f;
const float PROFILE_WIDTH = 1280-PROFILE_RIGHT_BORDER-PROFILE_LEFT_BORDER;

static float PROFILEVIEW_TABS[3] =
{
	260.0f, 310.0f, 350.0f
};

const int NUM_HIERARCHY_COLORS = 12;
static r3dColor HIERARCHY_COLOR[NUM_HIERARCHY_COLORS] = 
{
	r3dColor(120, 120, 255, 128),
	r3dColor(255, 120, 120, 128),
	r3dColor(120, 255, 120, 128),
	r3dColor(255, 255, 120, 128),
	r3dColor(120, 255, 255, 128),
	r3dColor(255, 120, 255, 128),
	r3dColor(0,   0,   255, 128),
	r3dColor(255, 0,   0,   128),
	r3dColor(0,   255, 0,   128),
	r3dColor(255, 255, 0,   128),
	r3dColor(0,   255, 255, 128),
	r3dColor(255, 0,   255, 128),
};

static int good_avarage = 0 ;

enum
{
	PROFILER_MAX_DISPLAYED_ITEMS = 20
};

static void Text_Print(float x, float y,r3dColor color,char *message, ...)
{
	va_list	va;
	char		buffer[1000];
	va_start(va, message);
	vsprintf(buffer, message, va);
	va_end(va);
	_r3dSystemFont->PrintF(x/1280.0f*r3dRenderer->ScreenW,y/720.0f*r3dRenderer->ScreenH, color,buffer);
}

bool r3dProfileRender::Create()
{
	if(sInstance)
		return false;

	sInstance = gfx_new r3dProfileRender();
	return true;
}

bool r3dProfileRender::Destroy()
{
	if(sInstance == NULL)
		return false;

	delete sInstance;
	sInstance = NULL;
	return true;
}


r3dProfileRender::r3dProfileRender()
{
	m_CursorSample = NULL;
	m_CursorFrame = 0;
	m_YOffset = 0.0f;
	m_LeftBorder = PROFILE_LEFT_BORDER;

	m_PrintSampleStart = 0 ;
	m_PrintSamplePos = 0 ;

}

r3dProfileRender::~r3dProfileRender()
{
}


void r3dProfileRender::Render()
{
	if( r_profiler_scaleform->GetBool() )
	{
		RenderScaleformProfiler();
	}

	if( r_stats_grass->GetInt() )
	{
		RenderGrassStats();
	}

	if( r_print_background_tasks->GetInt() )
	{
		PrintBackGroundTasks();
	}

	if( !r_show_profiler->GetBool() && !r_show_d3dmarks->GetBool() )
		return;

	if( r3dProfiler::Instance() == NULL )
		return;

	if( r_show_profiler->GetBool() )
		RenderProfiler() ;
	else
	{
		if( r_show_d3dmarks->GetBool() )
		{
			RenderD3DMarks() ;
		}
	}
}

//------------------------------------------------------------------------

void r3dProfileRender::RenderD3DMarks()
{
	float xstart, ystart ;

	xstart = 121.f ;
	ystart = 289.f ;

	for( int i = 0, e = r3dProfiler::GetDedicatedD3DStampsCount() ; i < e; i ++ )
	{
		float stats[ r3dProfiler::NAMED_D3D_MARKS_HISTORY_DEPTH ] ;

		r3dProfiler::GetDedicatedD3DStampStats( i, stats ) ;

		float minv = 0.f, maxv = 0.f, std = 0.f ;

		float avg = r3dStats::CalcGoodAverage< float >( stats, r3dProfiler::NAMED_D3D_MARKS_HISTORY_DEPTH, r3dStats::C95, &std, &minv, &maxv ) ;

		const float scale = 1000.f ;

#if 0
		Text_Print( xstart, ystart, r3dColor::white, "%s = %.2f (%.2f, [%.2f, %.2f, %2.f])", 
						r3dProfiler::GetDedicatedD3DStampName( i ).c_str(), 
								r3dProfiler::GetDedicatedD3DStampTime( i ) * scale, 
									r3dProfiler::GetDedicatedD3DStampAvgTime( i ) * scale,
										avg * scale, minv * scale, maxv * scale );
#else
		Text_Print( xstart, ystart, r3dColor::white, "%-11s = %7.2f (%7.2f)", 
						r3dProfiler::GetDedicatedD3DStampName( i ).c_str(), 
								r3dProfiler::GetDedicatedD3DStampTime( i ) * scale, 
									r3dProfiler::GetDedicatedD3DStampAvgTime( i ) * scale );

#endif

		ystart += 22.f ;
	}
}

//------------------------------------------------------------------------

void r3dProfileRender::RenderProfiler()
{

	m_PrintSamplePos = 0 ;

	R3DPROFILE_FUNCTION("r3dProfileRender::Render");
	
	struct D3DDebug
	{
		D3DDebug()
		{
			D3DPERF_BeginEvent( 0, L"r3dProfileRender::RenderProfiler" ) ;
		}

		~D3DDebug()
		{
			D3DPERF_EndEvent() ;
		}

	} d3ddebug; (void) d3ddebug ;

	{
		if(m_CursorSample == NULL)
		{
			m_CursorSample = r3dProfiler::Instance()->GetRoot();
			m_CursorSample->Open();
		}

		if(Keyboard->WasPressed(kbsUp))
		{
			r3dProfilerSample* prev = m_CursorSample->GetPrevious();
			//Search down to see if its open
			while(prev)
			{
				r3dProfilerSample *parent = prev->GetParent();
				if(!parent)
					break;
				if(prev->GetAverageTime() == 0.0f)
					prev = prev->GetPrevious();
				else
				{
					r3dProfilerSample *p = prev->GetParent();
					while(p)
					{
						if(p->IsOpen())
							p = p->GetParent();
						else
							break;
					}
					if(!p)
						break;
					else
						prev = prev->GetPrevious();
				}
			}
			if(prev)
				m_CursorSample = prev;

			int offset( 0 ) ;
			GetSampleOffset( r3dProfiler::Instance()->GetRoot(), m_CursorSample, &offset );

			if( offset < m_PrintSampleStart )
			{
				m_PrintSampleStart = offset ;
			}
		}
		if(Keyboard->WasPressed(kbsDown))
		{
			r3dProfilerSample* next = m_CursorSample->GetNext();
			while(next)
			{
				r3dProfilerSample *parent = next->GetParent();
				if(!parent)
					break;

				if(next->GetAverageTime() == 0.0f)
					next = next->GetNext();
				else
				{
					r3dProfilerSample *p = next->GetParent();
					while(p)
					{
						if(p->IsOpen())
							p = p->GetParent();
						else
							break;
					}
					if(!p)
						break;
					else
						next = next->GetNext();
				}
			}

			if(next)
				m_CursorSample = next;

			int offset( 0 ) ;
			GetSampleOffset( r3dProfiler::Instance()->GetRoot(), m_CursorSample, &offset );

			if( offset >= m_PrintSampleStart + PROFILER_MAX_DISPLAYED_ITEMS )
			{
				m_PrintSampleStart = offset - PROFILER_MAX_DISPLAYED_ITEMS + 1 ;
			}
		}
		if(Keyboard->WasPressed(kbsRight))
			m_CursorFrame++;
		if(Keyboard->WasPressed(kbsLeft))
			m_CursorFrame--;

		if(Keyboard->WasPressed(kbsSpace))
		{
			if(m_CursorSample->IsOpen())
			{
				m_CursorSample->Close();
			}
			else
				m_CursorSample->Open();
		}

	}

	R3DPROFILE_START("RenderProfiler:RenderBlock0");

	if(r3dProfiler::Instance()->IsPaused())
		m_TotalTime = r3dProfiler::Instance()->GetFirstItem()->GetTotalTime(m_CursorFrame+r3dProfiler::Instance()->GetCurrentFrame());
	else
	{
		r3dProfilerItem * item = r3dProfiler::Instance()->GetFirstItem() ;

		if( good_avarage )
			m_TotalTime = item->GetGoodAverageTime();
		else
			m_TotalTime = item->GetAverageTime();
	}

	// background
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	{
		r3dSetFwdColorShaders( r3dColor(30,30,30,127) ) ;
		r3dSetIdentityTransform( 0 ) ;
		r3dDrawBox2DNoTex(PROFILE_LEFT_BORDER/1280.0f*r3dRenderer->ScreenW, PROFILE_TOP_Y/720.0f*r3dRenderer->ScreenH, PROFILE_WIDTH/1280.0f*r3dRenderer->ScreenW, PROFILE_HEIGHT/720.0f*r3dRenderer->ScreenH, r3dColor(30,30,30,127));
	}

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	if(r3dProfiler::Instance()->IsPaused())
	{
		static uint32_t blinking_counter = 0;
		++blinking_counter;
		if(blinking_counter%6!=0)
			Text_Print(PROFILE_LEFT_BORDER, (PROFILE_TOP_Y+PROFILE_VIEW_LINE_JUMP), r3dColor(255,0,0,128), "Profiling Paused");
	}
	Text_Print(PROFILE_LEFT_BORDER, PROFILE_TOP_Y, r3dColor::white, "FrameRate:%i", (int)(1.0f/m_TotalTime));

	static float sColumn2 = 500.0f;
	static float sColumn3 = 680.0f;
	//Text_Print(sColumn2, PROFILE_TOP_Y, r3dColor::white, "(O) Hierarchy Render");
	//Text_Print(sColumn2, (PROFILE_TOP_Y+PROFILE_VIEW_LINE_JUMP), r3dColor::white, "(P) Pause");
	//Text_Print(sColumn3, PROFILE_TOP_Y, r3dColor::white, "(L) Offset Hierarchy Right");
	//Text_Print(sColumn3, (PROFILE_TOP_Y+PROFILE_VIEW_LINE_JUMP), r3dColor::white, "(K) Offset Hierarchy Left");

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	if( r3dRenderer->SupportsStampQueries && r_allow_gpu_timestamps->GetInt() )
	{
		int profileRenderSetting = (int)r_profiler_d3d->GetBool();
		if ( imgui_Button( sColumn2, PROFILE_TOP_Y, 70, 22, "Profile D3D", profileRenderSetting, false ) ) 
			r_profiler_d3d->SetBool(!profileRenderSetting);
	}

	{
		int renderHierarchy = (int)r_profiler_hierarchy->GetBool();
		if ( imgui_Button( sColumn2+80, PROFILE_TOP_Y, 110, 22, "Hierarchy Render", renderHierarchy, false ) ) 
			r_profiler_hierarchy->SetBool(!renderHierarchy);
	}

	bool pause = !r3dProfiler::Instance()->IsPaused();
	if ( imgui_Button( sColumn2+200, PROFILE_TOP_Y, 70, 22, "Start/Stop", pause, false ) ) 
		r_profiler_paused->SetBool(pause);
	r3dProfiler::Instance()->SetPaused(r_profiler_paused->GetBool());

	if ( imgui_Button( sColumn2+280, PROFILE_TOP_Y, 70, 22, "Dump" ) ) 
	{
		CreateProfileDumpFolders( m_LevelFolder.c_str() ) ;
		DoDumpProfilerData() ;
		DumpProfileScreenShot();
	}

	if ( imgui_Button( sColumn2+355, PROFILE_TOP_Y, 16, 22, "G", good_avarage ) ) 
	{
		good_avarage = !good_avarage ;
	}


	imgui_Value_Slider(PROFILE_LEFT_BORDER, PROFILE_TOP_Y, "Scroll",	&m_LeftBorder, -200, +200,"%.0f", 1, false);

#if 0
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);
#endif

	PrintLegend();

	R3DPROFILE_END("RenderProfiler:RenderBlock0");

	if(r_profiler_hierarchy->GetBool())
		RenderHierarchy();
	else
		RenderSummary();

	RenderCursor();

	//Text_Print(1000, 600, r3dColor::white, "DIPs: %d", r3dRenderer->Stats.NumDraws);

	r3dRenderer->Flush();
}

bool r3dProfileRender::PrintItem(const char* aName, float aX, float aY, int a_iDepth, int aNumCalls, float aSeconds, const r3dColor& aColor, bool aOpenFlag, bool aChildrenFlag)
{
	if( aY < PROFILE_DATA_START_Y )
		return false ;
	if( aY > (PROFILE_TOP_Y + PROFILE_HEIGHT))
		return false ;

	char trunc[128];
	r3dscpy(trunc, aName);

	if(aOpenFlag && aChildrenFlag)
	{
		Text_Print(aX, aY, aColor, "-%s", trunc);
	}
	else if(aChildrenFlag)
	{
		Text_Print(aX, aY, aColor, "+%s", trunc);
	}
	else
	{
		Text_Print(aX+6, aY, aColor, "%s", trunc);
	}

	Text_Print(PROFILEVIEW_TABS[0], aY, aColor, "%i", aNumCalls);
	Text_Print(PROFILEVIEW_TABS[1], aY, aColor, "%i", (int)(aSeconds * 100.0f / m_TotalTime));
	Text_Print(PROFILEVIEW_TABS[2], aY, aColor, "%.2f", aSeconds * 1000.0f);

	return true ;
}

void r3dProfileRender::PrintLegend()
{
	Text_Print(PROFILE_LEFT_BORDER, PROFILE_LEGEND_Y, r3dColor::yellow, "Function");
	Text_Print(PROFILEVIEW_TABS[0], PROFILE_LEGEND_Y, r3dColor::yellow, "Calls");
	Text_Print(PROFILEVIEW_TABS[1], PROFILE_LEGEND_Y, r3dColor::yellow, "%%");
}

void r3dProfileRender::RenderSummary()
{
	int currFrame = r3dProfiler::Instance()->GetCurrentFrame();
	float y = PROFILE_DATA_START_Y - m_YOffset;

	r3dProfilerItem* item = r3dProfiler::Instance()->GetFirstItem();

	const int MAX_ITEMS = 200 ;

	char heapMem[HEAPBUFFERSIZE(MAX_ITEMS,r3dProfilerItem*,float)];
	r3dHeap<r3dProfilerItem*, float> itemHeap(MAX_ITEMS, 999999999.9f, heapMem);

	while(item && !itemHeap.IsFull())
	{
		float time = item->GetTotalTime(m_CursorFrame+currFrame);
		if(!r3dProfiler::Instance()->IsPaused())
		{
			if( good_avarage )
				time = item->GetGoodAverageTime();
			else
				time = item->GetAverageTime();
		}
		itemHeap.Push(item, time);
		item = item->GetNext();
	}

	typedef r3dTL::TFixedArray< r3dProfilerItem*, MAX_ITEMS > TempProfItems ;

	float				lineYs[ MAX_ITEMS ] ;
	TempProfItems		lineItems ;
	int					lineItemCount = 0 ;

	while(!itemHeap.IsEmpty())
	{
		item = itemHeap.Get();
		float time = itemHeap.Value();
		itemHeap.Pop();

		int calls = item->GetNumCalls(m_CursorFrame+currFrame);
		if(!r3dProfiler::Instance()->IsPaused())
			calls = item->GetAverageNumCalls();

		if( PrintItem(item->GetName(), PROFILE_LEFT_BORDER, y, 0, calls, time) )
		{
			lineItems[ lineItemCount ] = item ;
			lineYs[ lineItemCount ] = y ;

			lineItemCount ++ ;
		}
		y += PROFILE_VIEW_LINE_JUMP;
		item = item->GetNext();
	}

	StartLineRender() ;
	for( int i = 0, e = lineItemCount ; i < e; i ++ )
	{
		DrawHistoryLine( lineItems[ i ], lineYs[ i ] ) ;
	}
	StopLineRender() ;
}

void r3dProfileRender::StartLineRender()
{
	r3dSetFwdColorShaders( r3dColor( 255, 255, 0, 128 ) ) ;

	D3DXMATRIX transform(
		2.0f / r3dRenderer->ScreenW,	0.f,							0.f, 0.f,
		0.f,							-2.0f / r3dRenderer->ScreenH,	0.f, 0.f,
		0.f,							0.f,							1.f, 0.f,
		-1.0f,							1.0f,							0.f, 1.f
		) ;

	D3DXMatrixTranspose( &transform, &transform ) ;

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float*)&transform, 4 ) ) ;
}

void r3dProfileRender::StopLineRender()
{
	r3dRenderer->SetPixelShader() ;
	r3dRenderer->SetVertexShader() ;
}

void r3dProfileRender::DrawHistoryLine(r3dProfilerHistory* aHistory, float aY)
{
	if( aY < PROFILE_DATA_START_Y )
		return;

	R3DPROFILE_FUNCTION( "r3dProfileRender::DrawHistoryLine" ) ;

	const float sHistoryLineScale = 2.0f * 1000.f ;

	int frame = r3dProfiler::Instance()->GetCurrentFrame();

	float x = PROFILEVIEW_TABS[2] + 45.0f;
	aY += PROFILE_VIEW_LINE_JUMP * 0.5f;

	float average = good_avarage ? aHistory->GetGoodAverageTime() : aHistory->GetAverageTime();

	float lineStep = ((1280.0f-PROFILE_RIGHT_BORDER) - x) / NUM_PROFILE_FRAME_HISTORY;

	r3dStartLineStrip2D( NUM_PROFILE_FRAME_HISTORY - 1 ) ;

	for( int t=0 ; t < NUM_PROFILE_FRAME_HISTORY; t++ )
	{
		float px = x + t * lineStep;
		float py = aY + (average - aHistory->GetTotalTime(t + frame)) * sHistoryLineScale;

		r3dLineStrip2D( int( px / 1280.0f * r3dRenderer->ScreenW ), (int) ( py / 720.0f*r3dRenderer->ScreenH ) ) ;
	}

	r3dEndLineStrip2D() ;
}

int r3dProfileRender::GetSampleOffset( r3dProfilerSample* aCurSample, r3dProfilerSample* aSample, int *ioOffset )
{
	if( !aCurSample )
		return 0 ;

	if( aCurSample == aSample )
		return 1 ;
	else
	{
		if( aCurSample->GetAverageTime() != 0.f )
		{
			*ioOffset += 1 ;

			if( aCurSample->IsOpen() )
			{


				if( r3dProfilerSample* child = aCurSample->GetChild() )
				{
					if( GetSampleOffset( child, aSample, ioOffset ) )
						return 1 ;
				}
			}
		}

		return GetSampleOffset( aCurSample->GetSibling(), aSample, ioOffset );
	}
}

void r3dProfileRender::SetLevelFolder( const char* LevelFolder ) 
{
	m_LevelFolder = LevelFolder ;
}

void r3dProfileRender::DrawHierarchyLine(r3dProfilerSample* aSample, int aDepth )
{
	float time = good_avarage ? aSample->GetGoodAverageTime() : aSample->GetAverageTime();
	if(time > 0.0f)
	{
		if( m_PrintSamplePos >= m_PrintSampleStart && m_PrintSamplePos < m_PrintSampleStart + PROFILER_MAX_DISPLAYED_ITEMS )
		{
			DrawHistoryLine(aSample, m_HierarchyPoses[ m_PrintSamplePos - m_PrintSampleStart ] );
		}

		m_PrintSamplePos ++ ;

		if(aSample->IsOpen() && aSample->GetChild())
		{
			DrawHierarchyLine(aSample->GetChild(), aDepth+1); 
		}
	}

	if(aSample->GetSibling())
	{
		if( m_PrintSamplePos < m_PrintSampleStart + PROFILER_MAX_DISPLAYED_ITEMS )
		{
			DrawHierarchyLine(aSample->GetSibling(), aDepth); 
		}
	}
}

void r3dProfileRender::PrintHierarchySample(r3dProfilerSample* aSample, float aX, float& aY, int aDepth)
{
	float time = good_avarage ? aSample->GetGoodAverageTime() : aSample->GetAverageTime();
	if(time > 0.0f)
	{
		int currFrame = r3dProfiler::Instance()->GetCurrentFrame();
		int numCalls = aSample->GetNumCalls(m_CursorFrame+currFrame);
		if(r3dProfiler::Instance()->IsPaused())
			time = aSample->GetTotalTime(m_CursorFrame+currFrame);
		else
			numCalls = aSample->GetAverageNumCalls();

		if( m_PrintSamplePos >= m_PrintSampleStart && m_PrintSamplePos < m_PrintSampleStart + PROFILER_MAX_DISPLAYED_ITEMS )
		{
			PrintItem(aSample->GetItem()->GetName(), aX, aY, aDepth, numCalls, time, m_CursorSample == aSample ? r3dColor::white: HIERARCHY_COLOR[aDepth%NUM_HIERARCHY_COLORS], aSample->IsOpen(), aSample->GetChild() != NULL);
			m_HierarchyPoses.PushBack( aY ) ;
			aY += PROFILE_VIEW_LINE_JUMP;
		}

		m_PrintSamplePos ++ ;

		if(aSample->IsOpen() && aSample->GetChild())
		{
			if( m_PrintSamplePos >= m_PrintSampleStart && m_PrintSamplePos < m_PrintSampleStart + PROFILER_MAX_DISPLAYED_ITEMS )
			{
				PrintHeirarchySampleSum(aSample->GetChild(), aX + 10.0f, aY, aDepth+1, time);
			}

			PrintHierarchySample(aSample->GetChild(), aX + 10.0f, aY, aDepth+1); 
		}
	}

	if(aSample->GetSibling())
	{
		if( m_PrintSamplePos < m_PrintSampleStart + PROFILER_MAX_DISPLAYED_ITEMS )
		{
			PrintHierarchySample(aSample->GetSibling(), aX, aY, aDepth); 
		}
	}
}

void r3dProfileRender::PrintHeirarchySampleSum(r3dProfilerSample* aSample, float aX, float& aY, int aDepth, float aSeconds)
{
	float sum = 0.f;

	if (aSeconds > 0.f)
	{
		while (aSample)
		{
			float time = aSample->GetAverageTime();
			int currFrame = r3dProfiler::Instance()->GetCurrentFrame();
			int numCalls = aSample->GetNumCalls(m_CursorFrame+currFrame);
			if(r3dProfiler::Instance()->IsPaused())
				time = aSample->GetTotalTime(m_CursorFrame+currFrame);
			else
				numCalls = aSample->GetAverageNumCalls();
			sum += time;
			aSample = aSample->GetSibling();
		}
	}

	static bool sMax = true;
	float loss = sMax?R3D_MAX(aSeconds-sum,0.f):aSeconds-sum;

	static bool sSpacer1 = false;
	static bool sSpacer2 = true;
	if (sSpacer1)
		aY += PROFILE_VIEW_LINE_JUMP;
	PrintItem("UNPROFILED", aX, aY, aDepth, 0, loss, HIERARCHY_COLOR[aDepth%NUM_HIERARCHY_COLORS], false, false);
	if (sSpacer2)
		aY += PROFILE_VIEW_LINE_JUMP;
}


void r3dProfileRender::RenderHierarchy()
{
	float y = PROFILE_DATA_START_Y - m_YOffset;

	r3dProfilerSample* sample = r3dProfiler::Instance()->GetRoot();

	m_PrintSamplePos = 0 ;

	if(sample)
	{

		m_HierarchyPoses.Clear() ;
		PrintHierarchySample(sample, m_LeftBorder, y, 0); 

		m_PrintSamplePos = 0 ;

		StartLineRender() ;
		DrawHierarchyLine( sample, 0 ) ;
		StopLineRender() ;
	}

	if( r3dProfilerSample* qs = r3dProfiler::Instance()->GetQuerySample() )
	{
		PrintHierarchySample( qs, m_LeftBorder, y, 0); 
	}


}

void r3dProfileRender::RenderCursor()
{
	float x = PROFILEVIEW_TABS[2] + 45.0f;
	float lineStep = (1280.0f - x - PROFILE_RIGHT_BORDER) / NUM_PROFILE_FRAME_HISTORY;

	x += m_CursorFrame * lineStep;

	r3dDrawLine2D(x/1280.0f*r3dRenderer->ScreenW, PROFILE_DATA_START_Y/720.0f*r3dRenderer->ScreenH, x/1280.0f*r3dRenderer->ScreenW, (PROFILE_DATA_START_Y+PROFILE_HEIGHT-55.0f)/720.0f*r3dRenderer->ScreenH, 1, r3dColor::red);
}

void RenderScaleformProfiler()
{
#ifndef FINAL_BUILD
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	extern float g_ScaleFormCompositeInvoke ;
	extern int g_ScaleFormInvokeCount ;

	extern float g_ScaleFormUpdateAndDraw ;
	extern int g_ScaleFormUpdateAndDrawCount ;

	Text_Print( 110.f, r3dRenderer->ScreenH2 +  0, r3dColor::white, "%-7d invokes of %7.2f duration", g_ScaleFormInvokeCount, g_ScaleFormCompositeInvoke * 1000.f ) ;
	Text_Print( 110.f, r3dRenderer->ScreenH2 + 22, r3dColor::white, "%-7d updates of %7.2f duration", g_ScaleFormUpdateAndDrawCount , g_ScaleFormUpdateAndDraw * 1000.f ) ;

	int c = 0;

	for( ScaleFormSamples::iterator i = g_ScaleFormSamples.begin(), 
									e = g_ScaleFormSamples.end();
									i != e;
									++ i
									)
	{
		ScaleFormSample &sfs = i->second;

		if( sfs.count > 10 )
		{
			sfs.avg = sfs.time / sfs.count;
			sfs.count = 0;
			sfs.time = 0.f;
		}

		Text_Print( 110.f, r3dRenderer->ScreenH2 + 44 + 15 * c ++, r3dColor::white, "%s - %.2f", i->first.c_str(), sfs.avg * 1000.f );
	}
	
	g_ScaleFormInvokeCount = 0 ;
	g_ScaleFormCompositeInvoke = 0 ;

	g_ScaleFormUpdateAndDraw = 0 ;
	g_ScaleFormUpdateAndDrawCount = 0 ;
#endif
}

void PrintBackGroundTasks()
{
#ifndef FINAL_BUILD
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	if( !g_pBackgroundTaskDispatcher )
		return;

	r3dBackgroundTaskDispatcher::TaskClasses taskClasses;

	g_pBackgroundTaskDispatcher->GetPendingTaskClasses( &taskClasses );

	const float PRINT_Y_START = 121.f;

	float printX = 110.f;
	float printY = PRINT_Y_START;

	for( int i = 0, e = (int)taskClasses.Count(); i < e; i ++ )
	{
		const char* typeName = "UNKNOWN";

		switch( taskClasses[ i ] )
		{
		case r3dBackgroundTaskDispatcher::TASK_CLASS_GENERIC:
			typeName = "Generic";
			break;
		case r3dBackgroundTaskDispatcher::TASK_CLASS_TEXTURE:
			typeName = "Texture";
			break;
		case r3dBackgroundTaskDispatcher::TASK_CLASS_MEGATEXTURE:
			typeName = "MegaTexture";
			break;
		case r3dBackgroundTaskDispatcher::TASK_CLASS_GRASS:
			typeName = "Grass";
			break;

		default:
			r3d_assert( "PrintBackGroundTasks: implement me!" );
		}

		Text_Print( printX, printY, r3dColor::green, "%d. %-7s", i, typeName );

		printY += 22.f;

		if( printY > r3dRenderer->ScreenH * 0.75f )
		{
			printY = PRINT_Y_START;
			printX += 160.f;
		}
	}
#endif
}

#endif // !DISABLE_PROFILER