#ifndef __R3D_PROFILER_H__
#define __R3D_PROFILER_H__

#if !DISABLE_PROFILER

#include "r3dTypedefs.h"
#include "r3dHash.h"
#include "r3dTreeNode.h"
//#include "r3dRender.h"
#include "r3dProfileDataRecorder.h"

#define R3DPROFILE_ENABLED

const int MAX_NUM_PROFILE_NAME_CHARS = 32;
const int NUM_PROFILE_FRAME_HISTORY = 280;

static const int NUM_D3D_PROFILE_FRAMES = 2;

class r3dProfilerHistory
{
public:
	r3dProfilerHistory();

	int GetNumCalls(int aFrame);
	int GetAverageNumCalls(int numFrames = NUM_PROFILE_FRAME_HISTORY);
	float GetAverageTime(int numFrames = NUM_PROFILE_FRAME_HISTORY);
	float GetGoodAverageTime(int numFrames = NUM_PROFILE_FRAME_HISTORY);
	float GetTotalTime(int aFrame);
	void AddTime(int aFrame, float aTime);
	void AddTime(int aFrame, float aTime, int aCallCount );

protected:
	void ResetFrame(int aFrame);

	float m_TotalTime[NUM_PROFILE_FRAME_HISTORY];
	int   m_NumCalls[NUM_PROFILE_FRAME_HISTORY];
};

// profilerItem is created only once per each profiler_start
// so don't be afraid of new's in code, they will be called only once for that piece of code
class r3dProfilerItem : public r3dProfilerHistory
{
public:
	r3dProfilerItem(const char* pName, uint32_t hashName);
	~r3dProfilerItem()
	{
		if(m_Next)
			delete m_Next;
	}

	void Reset(int aFrame);

	uint32_t GetNameHash() {return m_HashedName;}
	const char* GetName() {return m_Name;}

	void AddLast(r3dProfilerItem* aItem)
	{
		r3dProfilerItem* pItem = this;
		while(pItem->m_Next)
			pItem = pItem->m_Next;
		pItem->m_Next = aItem;
	}

	r3dProfilerItem* GetNext() {return m_Next;}
	void SetNext(r3dProfilerItem* pItem) {m_Next = pItem;}

private:
	char m_Name[MAX_NUM_PROFILE_NAME_CHARS];
	uint32_t m_HashedName;
	r3dProfilerItem* m_Next;
};

struct r3dSampleQueryLink;
class r3dProfilerSample :	public r3dTreeNode<r3dProfilerSample>, 
							public r3dProfilerHistory
{
public:
	r3dProfilerSample(r3dProfilerItem* pItem);
	~r3dProfilerSample();

	void			Start( );
	void			Stop( int aFrame );

	r3dProfilerItem*	GetItem( )	{ return m_Item; }
	float			GetTime();
	float			GetGPUTime();

	void			Finalize( int diametricFrame, int nextFrame );
	void			ResetQueries( int queryFrame );

	void			Reset( int aFrame );
	bool			IsOpen( )	{ return m_OpenFlag; }
	void			Open( )		{ m_OpenFlag = true; }
	void			Close( )
	{
		m_OpenFlag = false;
		if ( GetChild() )
			GetChild()->CloseAll();
	}
private:

	void			DoFinalize( int diametricFrame );

	void			CloseAll()
	{
		Close();
		if ( GetSibling() )
			GetSibling()->CloseAll();
	}

	bool				m_OpenFlag;
	r3dProfilerItem*	m_Item;
	LONGLONG			m_EndTime;
	LONGLONG			m_StartTime;
	r3dSampleQueryLink*	m_Queries[ NUM_D3D_PROFILE_FRAMES ];

};


class r3dProfiler
{
public:

	enum
	{
		NAMED_D3D_MARKS_HISTORY_DEPTH = 64
	};

	static bool			Create( int aNumThreads );
	static bool			Destroy( );
	static r3dProfiler*	Instance();

	static void			StartSample( const char *aName, uint32_t aHashName );
	static void			EndSample( const char *aName, uint32_t aHashName);

	void				StartFrame( );
	void				EndFrame( );

	r3dProfilerSample *	GetRoot( )						{ return m_RootSample; }
	r3dProfilerItem *	GetFirstItem( )					{ return m_FirstProfileItem; }
	r3dProfilerSample *	GetCurrent( )					{ return m_CurrentSample; }

	r3dProfilerSample * GetQuerySample(	)				{ return m_QuerySample; }

	int					GetCurrentFrame( )				{ return m_CurrentFrame; }
	bool				IsPaused( )						{ return m_PausedFlag && m_PausedFrame; }
	void				SetPaused( bool aPause = true )	{ m_PausedFlag = aPause; }

	static double		sSecondPerTick;

	void				Start( const char* aName, uint32_t aHashName );
	void				End( const char* aName, uint32_t aHashName );

	static int				CreateDedicatedD3DStamp( const char* Name ) ;
	static int				GetDedicatedD3DStampsCount();
	static void				StartDedicatedD3DStamp( int idx ) ;
	static void				EndDedicatedD3DStamp( int idx ) ;
	static const r3dString&	GetDedicatedD3DStampName( int idx ) ;
	static float			GetDedicatedD3DStampTime( int idx ) ;
	static float			GetDedicatedD3DStampAvgTime( int idx ) ;

	static void				GetDedicatedD3DStampStats( int idx, float (&stats)[ NAMED_D3D_MARKS_HISTORY_DEPTH ] ) ;

private:

	r3dProfiler();
	~r3dProfiler();

	void				Init( );

	int					m_CurrentFrame;
	bool				m_PausedFlag;
	bool				m_PausedFrame;

	r3dProfilerSample *	m_CurrentSample;
	r3dProfilerSample *	m_RootSample;
	r3dProfilerSample *	m_QuerySample;
	r3dProfilerItem *	m_QueryItem;
	r3dProfilerItem *	m_FirstProfileItem;	

	static int				m_NumInstances;
	static r3dProfiler*		sInstance;
};


class r3dProfilerObject
{
public:
	r3dProfilerObject(const char* aName, uint32_t aHashName)
	{
		m_Name = aName;
		m_HashedName = aHashName;

		r3dProfiler::StartSample(m_Name, m_HashedName);
	}

	~r3dProfilerObject()
	{
		r3dProfiler::EndSample(m_Name, m_HashedName);
	}

	const char* m_Name;
	uint32_t m_HashedName;
};

//==============================================================================
inline r3dProfiler *r3dProfiler::Instance()
{
	if(sInstance == NULL)
		return NULL;

// in future when we will have more than one thread
//	if(index == 0)
//		index = (int)TlsGetValue(THREAD_ID_INDEX);

	int index = 1;
	if(index > 0 && index <= m_NumInstances)
		return &(sInstance[index-1]);
	return NULL;
}

inline void r3dProfiler::StartSample( const char* aName, uint32_t aHashName)
{
	extern DWORD MainThreadID ;
	if( GetCurrentThreadId() == MainThreadID )
	{
		r3dProfiler *inst = Instance();
		if ( inst && !inst->IsPaused() )
			inst->Start(aName, aHashName);
	}
}

inline void r3dProfiler::EndSample( const char* aName, uint32_t aHashName )
{
	extern DWORD MainThreadID ;
	if( GetCurrentThreadId() == MainThreadID )
	{
		r3dProfiler *inst = Instance();
		if ( inst && !inst->IsPaused() )
			inst->End(aName, aHashName);
	}
}

#endif // !DISABLE_PROFILER

#ifdef R3DPROFILE_ENABLED
#define R3DPROFILE_START(n) \
	{ \
		static uint32_t sHash=0; \
		while(!sHash) sHash=r3dHash::MakeHash(n); \
		r3dProfiler::StartSample(n, sHash); \
	}

#define R3DPROFILE_END(n) \
	{ \
		static uint32_t sHash=0; \
		while(!sHash) sHash=r3dHash::MakeHash(n); \
		r3dProfiler::EndSample(n, sHash); \
	}

#define R3DPROFILE_FUNCTION(n)  \
	static uint32_t sFuncHash=0; \
	while(!sFuncHash) sFuncHash=r3dHash::MakeHash(n); \
	r3dProfilerObject profileFunction(n,sFuncHash);

#define R3DPROFILE_D3DSTART(n)	r3dProfiler::StartDedicatedD3DStamp(n)
#define R3DPROFILE_D3DEND(n)	r3dProfiler::EndDedicatedD3DStamp(n)

#define R3D_LOG_TIMESPAN_START(Name) float timeStart_##Name = r3dGetTime()
#define R3D_LOG_TIMESPAN_END(Name) r3dOutToLog( #Name "=%f\n", r3dGetTime() - timeStart_##Name )

#else //R3DPROFILE_ENABLED
#define R3DPROFILE_START(n)
#define R3DPROFILE_END(n)
#define R3DPROFILE_FUNCTION(n)
#define R3DPROFILE_D3DSTART(n)
#define R3DPROFILE_D3DEND(n)

#define R3D_LOG_TIMESPAN_START(Name)
#define R3D_LOG_TIMESPAN_END(Name)

#endif //R3DPROFILE_ENABLED

void r3dSetShowD3DMarks( int show ) ;

struct ScaleFormSample
{
	float time;
	float avg;
	int count;

	ScaleFormSample()
	{
		avg = 0;
		time = 0;
		count = 0;
	}
};

typedef r3dgameMap( r3dSTLString, ScaleFormSample ) ScaleFormSamples;
extern ScaleFormSamples g_ScaleFormSamples;

#endif //__R3D_PROFILER_H__