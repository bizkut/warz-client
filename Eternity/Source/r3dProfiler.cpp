#include "r3dPCH.h"

#if !DISABLE_PROFILER

#include "r3d.h"
#include "r3dStats.h"

struct r3dSampleQueryLink
{
	r3dSampleQueryLink();

	void Init();

	IDirect3DQuery9*	Start ;
	IDirect3DQuery9*	End ;
	int					Frame ;
	r3dSampleQueryLink*	Next ;
};


#include "r3dProfiler.h"

#include "r3dDeviceQueue.h"

double r3dProfiler::sSecondPerTick = 1.0f;
int r3dProfiler::m_NumInstances = 0;
r3dProfiler* r3dProfiler::sInstance = NULL;
static INT64	GPUFreq;
static float	InvGPUFreq = 1.f;
static int		gFrame = 0;
DWORD MainThreadID ;

static int g_ShowD3DMarks = 0 ;

class QueryPool : public r3dIResource
{
public:
	static const UINT MAX_QUERY_COUNT = 12288;
	typedef r3dTL::TFixedArray< IDirect3DQuery9*, MAX_QUERY_COUNT > Queries ;
	typedef r3dTL::TFixedArray< r3dSampleQueryLink, MAX_QUERY_COUNT * 2 > QueryLinks ;
	typedef r3dTL::TFixedArray< IDirect3DQuery9*, NUM_D3D_PROFILE_FRAMES > FreqQueries ;

	struct NamedQueries
	{
		r3dString				name ;
		IDirect3DQuery9*	start_query ;
		IDirect3DQuery9*	end_query ;
		LONGLONG			last_value ;
		int					open_guard ;

		LONGLONG			history[ r3dProfiler::NAMED_D3D_MARKS_HISTORY_DEPTH ] ;
		int					historyPtr ;

		bool				flushed ;
		int					nesting_control ;

		NamedQueries() ;
	};

	typedef r3dTL::TArray< NamedQueries > NamedQueryArr ;

	QueryPool( const r3dIntegrityGuardian& ig = r3dIntegrityGuardian() );
	virtual ~QueryPool();

public:
	r3dSampleQueryLink*	AquireLink() ;
	IDirect3DQuery9*	GetLastQuery() const ;
	IDirect3DQuery9*	GetNextQuery() ;

	IDirect3DQuery9*	GetFreqQuery( uint32_t frame ) const;

	void Reset();

	int					CreateNamedQuery( const char* name ) ;
	void				StartNamedQuery( int idx ) ;
	void				EndNamedQuery( int idx ) ;

	const r3dString&		GetNamedQueryName( int idx ) const ;
	LONGLONG			GetNamedQueryDuration( int idx ) ;
	LONGLONG			GetNamedQueryAvgDuration( int idx ) ;

	LONGLONG*			GetNamedQueryStats( int idx ) ;

	int					GetNamedQueryCount() const ;

	void				EndFrame() ;

private:
	virtual	void D3DCreateResource()  OVERRIDE;
	virtual	void D3DReleaseResource() OVERRIDE;

private:
	Queries		mQueries[ NUM_D3D_PROFILE_FRAMES ] ;
	UINT		mLastQuery ;
	QueryLinks	mQueryLinks[ NUM_D3D_PROFILE_FRAMES ] ;
	UINT		mLastLink ;

	NamedQueryArr	mNamedQueries ;

	FreqQueries		mFreqQueries ;

} * g_pQueryPool;

//------------------------------------------------------------------------

QueryPool::NamedQueries::NamedQueries()
: start_query( NULL )
, end_query( NULL )
, last_value( 0 )
, flushed( true )
, nesting_control( 0 )
, historyPtr( 0 )
, open_guard( 0 )
{
	memset( history, 0, sizeof history ) ;
}

//------------------------------------------------------------------------

QueryPool::QueryPool( const r3dIntegrityGuardian& ig /*= r3dIntegrityGuardian()*/ )
: r3dIResource( ig )
{
	CreateQueuedResource( this ) ;
}

//------------------------------------------------------------------------

QueryPool::~QueryPool()
{
	ReleaseQueuedResource( this );
}

//------------------------------------------------------------------------

r3dSampleQueryLink*
QueryPool::AquireLink() 
{
	r3dSampleQueryLink* link = &mQueryLinks[ gFrame ][ mLastLink ];

	link->Init();

	mLastLink ++ ;

	return link;
}

//------------------------------------------------------------------------

IDirect3DQuery9*
QueryPool::GetLastQuery() const
{
	r3d_assert( mLastQuery );

	return mQueries[ gFrame ][ mLastQuery - 1 ];
}

//------------------------------------------------------------------------

IDirect3DQuery9*
QueryPool::GetNextQuery() 
{
	return mQueries[ gFrame ][ mLastQuery ++ ];
}

//------------------------------------------------------------------------

IDirect3DQuery9*
QueryPool::GetFreqQuery( uint32_t frame ) const
{
	return mFreqQueries[ frame ];
}

//------------------------------------------------------------------------

void
QueryPool::Reset()
{
	mLastLink		= 0;
	mLastQuery		= 0;
}

//------------------------------------------------------------------------

int QueryPool::CreateNamedQuery( const char* name )
{
	NamedQueries q ;

	q.name = name ;

	if( r3dRenderer->SupportsStampQueries && r_allow_gpu_timestamps)
	{
		D3D_V( r3dRenderer->pd3ddev->CreateQuery( D3DQUERYTYPE_TIMESTAMP, &q.start_query ) ) ;
		D3D_V( r3dRenderer->pd3ddev->CreateQuery( D3DQUERYTYPE_TIMESTAMP, &q.end_query ) ) ;

		q.flushed = true ;
	}
	else
	{
		q.start_query = NULL ;
		q.end_query = NULL ;
	}

	mNamedQueries.PushBack( q ) ;

	return mNamedQueries.Count() - 1 ;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
void QueryPool::StartNamedQuery( int idx )
{
	NamedQueries& q = mNamedQueries[ idx ] ;

	q.open_guard ++ ;

	if( q.start_query && q.flushed )
	{
		D3D_V( q.start_query->Issue( D3DISSUE_END ) ) ;
	}
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
void QueryPool::EndNamedQuery( int idx )
{
	NamedQueries& q = mNamedQueries[ idx ] ;

	q.open_guard -- ;

	if( q.end_query && q.flushed )
	{
		D3D_V( q.end_query->Issue( D3DISSUE_END ) ) ;

		q.flushed = false ;
	}

	q.nesting_control ++ ;

	// shouldn't nest it & invoke same query several times per frame
	r3d_assert( q.nesting_control <= 1 ) ;
}

//------------------------------------------------------------------------

const r3dString& QueryPool::GetNamedQueryName( int idx ) const
{
	return mNamedQueries[ idx ].name ;
}

//------------------------------------------------------------------------

LONGLONG QueryPool::GetNamedQueryDuration( int idx )
{
	NamedQueries& q = mNamedQueries[ idx ] ;

	if( !q.start_query || !g_ShowD3DMarks )
	{
		return 0 ;
	}

	LONGLONG start, end ;

	if( q.start_query->GetData( &start, sizeof start, 0 ) == S_OK )
	{
		if( q.end_query->GetData( &end, sizeof end, 0 ) == S_OK )
		{
			q.flushed = true ;
			q.history[ q.historyPtr ++ ] = q.last_value ;

			if( q.historyPtr >= R3D_ARRAYSIZE( q.history ) )
			{
				q.historyPtr = 0 ;
			}

			q.last_value = end - start ;
		}
	}

	return q.last_value ;
}

//------------------------------------------------------------------------

LONGLONG QueryPool::GetNamedQueryAvgDuration( int idx )
{
	NamedQueries& q = mNamedQueries[ idx ] ;

	LONGLONG avg = 0 ;

	for( int i = 0, e = R3D_ARRAYSIZE( q.history ) ; i < e; i ++ )
	{
		avg += q.history[ i ] ;
	}

	return avg / R3D_ARRAYSIZE( q.history ) ;
}

//------------------------------------------------------------------------

LONGLONG* QueryPool::GetNamedQueryStats( int idx )
{
	NamedQueries& q = mNamedQueries[ idx ] ;

	return q.history ;
}

//------------------------------------------------------------------------

int
QueryPool::GetNamedQueryCount() const
{
	return mNamedQueries.Count() ;
}

//------------------------------------------------------------------------

void QueryPool::EndFrame()
{
#if 0
	r3dOutToLog( "QL: %d\n", mLastLink );
#endif

	for( int i = 0, e = mNamedQueries.Count() ; i < e ; i ++ )
	{
		r3d_assert( mNamedQueries[ i ].nesting_control <= 1 ) ;
		r3d_assert( !mNamedQueries[ i ].open_guard ) ;
		mNamedQueries[ i ].nesting_control = 0 ;
	}
}

//------------------------------------------------------------------------
/*virtual*/
void
QueryPool::D3DCreateResource()  /*OVERRIDE*/
{
	if( r_allow_gpu_timestamps->GetBool() && r3dRenderer->SupportsStampQueries )
	{
		for( UINT j = 0, e = NUM_D3D_PROFILE_FRAMES; j < e; j ++ )
		{
			for( UINT i = 0, e = Queries::COUNT; i < e; i ++ )
			{
				r3d_assert( !mQueries[ j ][ i ] );
				D3D_V( r3dRenderer->pd3ddev->CreateQuery( D3DQUERYTYPE_TIMESTAMP, &mQueries[ j ][ i ] ) ) ;
			}
		}

		for( UINT i = 0, e = FreqQueries::COUNT; i < e; i ++ )
		{
			D3D_V( r3dRenderer->pd3ddev->CreateQuery( D3DQUERYTYPE_TIMESTAMPFREQ, &mFreqQueries[ i ] ) );
		}

		for( UINT i = 0, e = mNamedQueries.Count() ; i < e ; i ++ )
		{
			NamedQueries& q = mNamedQueries[ i ] ;

			D3D_V( r3dRenderer->pd3ddev->CreateQuery( D3DQUERYTYPE_TIMESTAMPFREQ, &q.start_query ) );
			D3D_V( r3dRenderer->pd3ddev->CreateQuery( D3DQUERYTYPE_TIMESTAMPFREQ, &q.end_query ) );

			q.flushed = true ;
		}
	}

	Reset();
}

//------------------------------------------------------------------------
/*virtual*/
void
QueryPool::D3DReleaseResource() /*OVERRIDE*/
{
	if( r_allow_gpu_timestamps->GetBool() && r3dRenderer->SupportsStampQueries )
	{
		for( UINT j = 0, e = NUM_D3D_PROFILE_FRAMES; j < e; j ++ )
		{
			for( UINT i = 0, e = Queries::COUNT; i < e; i ++ )
			{
				SAFE_RELEASE( mQueries[ j ][ i ] );
			}
		}

		for( UINT i = 0, e = FreqQueries::COUNT; i < e; i ++ )
		{
			SAFE_RELEASE( mFreqQueries[ i ] );
		}

		for( UINT i = 0, e = mNamedQueries.Count() ; i < e ; i ++ )
		{
			NamedQueries& q = mNamedQueries[ i ] ;

			SAFE_RELEASE( q.start_query ) ;
			SAFE_RELEASE( q.end_query ) ;
		}
	}
}

LONGLONG GetCounter()
{
	LARGE_INTEGER time;
	::QueryPerformanceCounter(&time);

	return time.QuadPart;
}

r3dSampleQueryLink::r3dSampleQueryLink()
{
	Init() ;
}

void
r3dSampleQueryLink::Init()
{
	MainThreadID = GetCurrentThreadId();

	Start	= NULL ;
	End		= NULL ;
	Frame	= 0 ;
	Next	= NULL ;
}

r3dProfilerHistory::r3dProfilerHistory()
{
	for(int t=0; t < NUM_PROFILE_FRAME_HISTORY; t++)
		ResetFrame(t);
}

int r3dProfilerHistory::GetNumCalls(int aFrame) 
{
	return m_NumCalls[aFrame % NUM_PROFILE_FRAME_HISTORY];
}

float r3dProfilerHistory::GetTotalTime(int aFrame)
{
	return m_TotalTime[aFrame % NUM_PROFILE_FRAME_HISTORY];
}

int r3dProfilerHistory::GetAverageNumCalls(int numFrames)
{
	r3d_assert(numFrames <= NUM_PROFILE_FRAME_HISTORY);

	int curFrame = r3dProfiler::Instance()->GetCurrentFrame();

	int temp = 0;
	for(int t=0; t < numFrames; t++)
	{
		int idx = curFrame - t;
		if (idx < 0)
			idx = NUM_PROFILE_FRAME_HISTORY + idx;
		temp += m_NumCalls[idx];
	}
	return (int)ceilf( float(temp) / numFrames );
}

float r3dProfilerHistory::GetAverageTime(int numFrames /* = NUM_PROFILE_FRAME_HISTORY */)
{
	r3d_assert(numFrames <= NUM_PROFILE_FRAME_HISTORY);

	int curFrame = r3dProfiler::Instance()->GetCurrentFrame();

	float temp = 0;
	for(int t=0; t < numFrames; t++)
	{
		int idx = curFrame - t;
		if (idx < 0)
			idx = NUM_PROFILE_FRAME_HISTORY + idx;
		temp += m_TotalTime[idx];
	}
	return temp / numFrames;
}

float r3dProfilerHistory::GetGoodAverageTime(int numFrames /*= NUM_PROFILE_FRAME_HISTORY*/)
{
	r3d_assert(numFrames <= NUM_PROFILE_FRAME_HISTORY);

	float std, minv, maxv ;

	return r3dStats::CalcGoodAverage( m_TotalTime, numFrames, r3dStats::C95, &std, &minv, &maxv ) ;	
}

void r3dProfilerHistory::ResetFrame(int aFrame)
{
	m_TotalTime[aFrame] = 0;
	m_NumCalls[aFrame] = 0;
}

void r3dProfilerHistory::AddTime(int aFrame, float aTime)
{
	m_TotalTime[aFrame] += aTime;
	m_NumCalls[aFrame]++;
}

void r3dProfilerHistory::AddTime(int aFrame, float aTime, int aCallCount )
{
	m_TotalTime[aFrame] += aTime;
	m_NumCalls[aFrame] += aCallCount;
}

r3dProfilerItem::r3dProfilerItem(const char* aName, uint32_t aHashName)
{
	m_HashedName = aHashName;
	memcpy(m_Name, aName, MAX_NUM_PROFILE_NAME_CHARS);
	m_Name[MAX_NUM_PROFILE_NAME_CHARS - 1] = '\0';
	m_Next = NULL;
}

void r3dProfilerItem::Reset(int aFrame)
{
	ResetFrame(aFrame);
	if(m_Next)
		m_Next->Reset(aFrame);
}

r3dProfilerSample::r3dProfilerSample(r3dProfilerItem* aItem)
{
	for( uint32_t i = 0, e = NUM_D3D_PROFILE_FRAMES; i < e; i ++ )
	{
		m_Queries[ i ] = NULL ;
	}

	m_Item = aItem;
	m_OpenFlag = false;

}

r3dProfilerSample::~r3dProfilerSample()
{

}

static bool profileRenderOn = false;
void r3dProfilerSample::Start() 
{
	// Empty the command buffer from the unprofiled and wait until the GPU is idle.

	if( profileRenderOn && GPUFreq && r3dRenderer->SupportsStampQueries && r_allow_gpu_timestamps->GetBool() )
	{
		r3dSampleQueryLink* link = g_pQueryPool->AquireLink();

		r3dSampleQueryLink*& currLink = m_Queries[ gFrame ];

		if( currLink )
		{
			link->Next = currLink ;
		}

		currLink = link ;

		// Something happened since last stamp. Have to set a new one
		if( r3dRenderer->ActiveCommandCount != r3dRenderer->LastIssueActiveCommandsCount )
		{
			currLink->Start = g_pQueryPool->GetNextQuery();
			r3dRenderer->SetPendingQuery( currLink->Start );
		}
		else
		{
			currLink->Start = g_pQueryPool->GetLastQuery();
		}
	}
	else
	{
		m_StartTime = GetCounter();
	}
}

void r3dProfilerSample::Stop(int aFrame)
{
	// Empty the command buffer and wait until the GPU is idle.
	if( profileRenderOn && GPUFreq && r3dRenderer->SupportsStampQueries && r_allow_gpu_timestamps->GetBool() )
	{
		r3dSampleQueryLink* currLink = m_Queries[ gFrame ];

		if( r3dRenderer->LastIssueActiveCommandsCount != r3dRenderer->ActiveCommandCount )
		{
			currLink->End = g_pQueryPool->GetNextQuery();
				
			r3dRenderer->SetPendingQuery( currLink->End );
		}
		else
		{
			currLink->End = g_pQueryPool->GetLastQuery();
		}

		currLink->Frame = aFrame ;
	}
	else
	{
		m_EndTime = GetCounter() ;

		float time = GetTime() ;
		AddTime(aFrame, time) ;
		GetItem()->AddTime(aFrame, time) ;
	}

}

float r3dProfilerSample::GetTime() 
{
	return (float)( (m_EndTime - m_StartTime) * r3dProfiler::sSecondPerTick );
}

float r3dProfilerSample::GetGPUTime()
{
	return ( m_EndTime - m_StartTime ) * InvGPUFreq ;
}

void r3dProfilerSample::Reset(int aFrame) 
{
	m_Queries[ gFrame ] = NULL ;

	ResetFrame(aFrame);

	if(GetChild())
		GetChild()->Reset(aFrame);
	if(GetSibling())
		GetSibling()->Reset(aFrame);
}


void r3dProfilerSample::DoFinalize( int diametricFrame )
{
	r3dSampleQueryLink* q = m_Queries[ diametricFrame ];

	for( ; q ; q = q->Next )
	{
		for( ; S_OK != q->Start->GetData( &m_StartTime, sizeof m_StartTime, 0 ) ; ) 
			q->Start->GetData( 0, 0, D3DGETDATA_FLUSH ) ;

		for( ; S_OK != q->End->GetData( &m_EndTime, sizeof m_EndTime, 0 ) ; ) 
			q->End->GetData( 0, 0, D3DGETDATA_FLUSH ) ;

		float time = GetGPUTime() ;
		AddTime( q->Frame, time ) ;
		GetItem()->AddTime( q->Frame, time ) ;
	}

	if( r3dProfilerSample * ch = GetChild() )
		ch->DoFinalize( diametricFrame );
	if( r3dProfilerSample * ch = GetSibling() )
		ch->DoFinalize( diametricFrame );

}

void r3dProfilerSample::Finalize( int diametricFrame, int nextFrame )
{
	float start = r3dGetTime();

	DoFinalize( diametricFrame );

	ResetQueries( nextFrame );
}

void r3dProfilerSample::ResetQueries( int queryFrame )
{
	m_Queries[ queryFrame ] = NULL ;

	if( r3dProfilerSample * ch = GetChild() )
		ch->ResetQueries( queryFrame );
	if( r3dProfilerSample * ch = GetSibling() )
		ch->ResetQueries( queryFrame );
}

bool r3dProfiler::Create(int aNumThreads)
{
	if(sInstance)
		return false;

	m_NumInstances = aNumThreads;
	sInstance = game_new r3dProfiler[aNumThreads];

	g_pQueryPool = gfx_new QueryPool;

	return true;
}

bool r3dProfiler::Destroy()
{
	if(sInstance == NULL)
		return false;

	delete[] sInstance;
	sInstance = NULL;

	delete g_pQueryPool ;

	return true;
}

void r3dProfiler::Start(const char* aName, uint32_t aHashName)
{
	if ( !m_CurrentSample)
		return;

	//Find the child
	r3dProfilerSample* sample = m_CurrentSample->GetChild();
	while(sample && sample->GetItem() && sample->GetItem()->GetNameHash() != aHashName)
	{
		sample = sample->GetSibling();
	}

	//Create new sample
	if(sample==NULL)
	{
		//Find item
		r3dProfilerItem* item = m_FirstProfileItem;
		while(item && item->GetNameHash() != aHashName)
			item = item->GetNext();

		if(item == NULL)
		{
			item = game_new r3dProfilerItem(aName, aHashName);
			m_FirstProfileItem->AddLast(item);
		}
		sample = game_new r3dProfilerSample(item);
		//Add it to the tree
		m_CurrentSample->AddChild(sample);
	}

	m_CurrentSample = sample;
	m_CurrentSample->Start();
}

void r3dProfiler::End(const char* aName, uint32_t aHashName)
{
	if ( !m_CurrentSample )
		return;

	if (m_CurrentSample->GetItem()->GetNameHash() != aHashName)
	{
		if ( _stricmp(m_CurrentSample->GetItem()->GetName(), aName)!=0 )
		{
			r3dError("profiler block '%s' not closed properly, we are closing '%s'\n", m_CurrentSample->GetItem()->GetName(), aName );
		}
	}

	m_CurrentSample->Stop(m_CurrentFrame);
	m_CurrentSample = m_CurrentSample->GetParent();
}

//------------------------------------------------------------------------
/*static*/
int
r3dProfiler::CreateDedicatedD3DStamp( const char* Name )
{
	return g_pQueryPool->CreateNamedQuery( Name ) ;
}

//------------------------------------------------------------------------
/*static*/
void
r3dProfiler::StartDedicatedD3DStamp( int idx )
{
	if( g_ShowD3DMarks )
	{
		g_pQueryPool->StartNamedQuery( idx ) ;
	}
}

//------------------------------------------------------------------------
/*static*/
void
r3dProfiler::EndDedicatedD3DStamp( int idx )
{
	if( g_ShowD3DMarks )
	{
		g_pQueryPool->EndNamedQuery( idx ) ;
	}
}

//------------------------------------------------------------------------
/*static*/
const r3dString&
r3dProfiler::GetDedicatedD3DStampName( int idx )
{
	return g_pQueryPool->GetNamedQueryName( idx ) ;
}

//------------------------------------------------------------------------
/*static*/
float
r3dProfiler::GetDedicatedD3DStampTime( int idx )
{
	LONGLONG res = g_pQueryPool->GetNamedQueryDuration( idx ) ;

	return res * InvGPUFreq ;
}

//------------------------------------------------------------------------
/*static*/

float
r3dProfiler::GetDedicatedD3DStampAvgTime( int idx )
{
	return g_pQueryPool->GetNamedQueryAvgDuration( idx ) * InvGPUFreq ;
}

//------------------------------------------------------------------------
/*static*/

void
r3dProfiler::GetDedicatedD3DStampStats( int idx, float (&stats)[ NAMED_D3D_MARKS_HISTORY_DEPTH ] )
{
	LONGLONG* longStats = g_pQueryPool->GetNamedQueryStats( idx ) ;

	for( int i = 0 ; i < NAMED_D3D_MARKS_HISTORY_DEPTH ; i ++ )
	{
		stats[ i ] = longStats[ i ] * InvGPUFreq ;
	}
}

//------------------------------------------------------------------------

/*static*/
int
r3dProfiler::GetDedicatedD3DStampsCount()
{
	return g_pQueryPool->GetNamedQueryCount() ;
}

//------------------------------------------------------------------------

r3dProfiler::r3dProfiler()
{
	Init();
}

r3dProfiler::~r3dProfiler()
{
	delete (m_RootSample); m_RootSample=0;
	delete (m_FirstProfileItem); m_RootSample=0;

	delete (m_QuerySample); m_QuerySample=0;
	delete (m_QueryItem); m_QueryItem=0;
}

void r3dProfiler::Init()
{
	m_CurrentFrame = 0;
	m_FirstProfileItem = game_new r3dProfilerItem("Start", r3dHash::MakeHash("Start"));
	m_RootSample = game_new r3dProfilerSample(m_FirstProfileItem);

	m_QueryItem = game_new r3dProfilerItem( "Occ. Queries", r3dHash::MakeHash( "Occ. Queries" ) );
	m_QuerySample = game_new r3dProfilerSample( m_QueryItem );

	m_CurrentSample = NULL;

	m_PausedFlag = true;
	m_PausedFrame = m_PausedFlag;
}

void r3dProfiler::StartFrame()
{
	g_pQueryPool->Reset();

	if( ( profileRenderOn || g_ShowD3DMarks ) && r3dRenderer->SupportsStampQueries )
	{
		D3D_V( g_pQueryPool->GetFreqQuery( gFrame )->Issue( D3DISSUE_END ) );
	}

	LARGE_INTEGER proc_freq;
	::QueryPerformanceFrequency(&proc_freq);
	sSecondPerTick = 1.0 / proc_freq.QuadPart;

	m_PausedFrame = m_PausedFlag;
	if(IsPaused())
		return;
	
	m_CurrentSample = m_RootSample;

	m_CurrentFrame++;
	if(m_CurrentFrame == NUM_PROFILE_FRAME_HISTORY)
		m_CurrentFrame = 0;

	//Reset all samples
	if(m_CurrentSample)
		m_CurrentSample->Reset(m_CurrentFrame);

	//Set all times to 0
	r3dProfilerItem* item = m_FirstProfileItem;
	if(item)
		item->Reset(m_CurrentFrame);

	if ( m_CurrentSample)
		m_CurrentSample->Start();

	if( m_QueryItem )
		m_QueryItem->Reset( m_CurrentFrame );

	if( m_QuerySample )
		m_QuerySample->Reset( m_CurrentFrame );
}


void r3dProfiler::EndFrame()
{
	INT64 NewGPUFreq = 0;

	g_pQueryPool->EndFrame() ;

	r3dRenderer->IssuePendingQuery();

	int diametricFrame = gFrame ? gFrame - 1 : NUM_D3D_PROFILE_FRAMES - 1;

	if( ( profileRenderOn || g_ShowD3DMarks ) && r3dRenderer->SupportsStampQueries  )
	{
		IDirect3DQuery9* q = g_pQueryPool->GetFreqQuery( diametricFrame );
		for( ; S_OK != q->GetData( &NewGPUFreq, sizeof NewGPUFreq, 0 ) ; ) 
			q->GetData( 0, 0, D3DGETDATA_FLUSH ) ;
		InvGPUFreq = 1.f / NewGPUFreq;
	}

	if (m_CurrentSample)
	{
		m_CurrentSample->Stop( m_CurrentFrame );
	}

	gFrame = ( gFrame + 1 ) % NUM_D3D_PROFILE_FRAMES ;

	if( profileRenderOn && GPUFreq )
	{
		m_RootSample->Finalize( diametricFrame, gFrame );
	}
	else
	{
		for( int i = 0, e = NUM_D3D_PROFILE_FRAMES; i < e; i ++ )
		{
			m_RootSample->ResetQueries( i );
		}
	}

	
	float ourTime = r_use_oq->GetBool() ? r3dGetFrameTime() * 0.5f : 0.f ;

	if( !r_profiler_d3d->GetBool() && !profileRenderOn && !IsPaused() )
	{
		m_QuerySample->AddTime( m_CurrentFrame, ourTime, r3dRenderer->Stats.GetNumOcclusionQueries() );
		m_QueryItem->AddTime( m_CurrentFrame, ourTime, r3dRenderer->Stats.GetNumOcclusionQueries() );
	}

	// prevent cutting it in inside the frame
	profileRenderOn = r_profiler_d3d->GetBool();
	GPUFreq = NewGPUFreq ;

	m_CurrentSample = NULL;
}

void r3dSetShowD3DMarks( int show )
{
	g_ShowD3DMarks = show ;
}

ScaleFormSamples g_ScaleFormSamples;

#endif // !DISABLE_PROFILER
