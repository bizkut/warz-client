//=========================================================================
//	Module: r3dBackgroundTaskDispatcher.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "r3dDeviceQueue.h"

#include "r3dBackgroundTaskDispatcher.h"
#include "r3dDebug.h"

extern CRITICAL_SECTION g_ResourceCritSection;

//////////////////////////////////////////////////////////////////////////

// g_pBackgroundTaskDispatcher will be NULL in other project that do not create it!
r3dBackgroundTaskDispatcher *g_pBackgroundTaskDispatcher = 0;

//////////////////////////////////////////////////////////////////////////

namespace
{

/**	Data passed to worker thread. */
struct ThreadData
{
	/**	Array with tasks. */
	r3dTL::TArray<r3dBackgroundTaskDispatcher::TaskDescriptor> *tasks;
	
	volatile LONG* taskCount ;

	/**	Start work event. */
	HANDLE startEvent;

	/**	Task array guard critical section. */
	CRITICAL_SECTION *taskGuard;

	volatile LONG needTerminate;

	ThreadData(): tasks(0), taskCount(0), startEvent(0), taskGuard(0), needTerminate( 0 ) {}
} static * g_pThreadData;

//////////////////////////////////////////////////////////////////////////

void CompleteTask( r3dBackgroundTaskDispatcher::TaskDescriptor* td )
{
	//	Indicate task completion.
	if (td->CompletionFlag)
		InterlockedExchange(td->CompletionFlag, 1L);

	if(td->Params)
		InterlockedExchange(&td->Params->Taken,0L);
}

/**	Worker function */
unsigned int WINAPI BackgroundTaskDispatcherThreadFunc(void* Par)
{
	r3dOutToLog( "Started background thread: %d\n", GetCurrentThreadId() );

	r3dThreadAutoInstallCrashHelper crashHelper;
	ThreadData *data = reinterpret_cast<ThreadData*>(Par);

	while (true)
	{
		WaitForSingleObject(data->startEvent, INFINITE);
		EnterCriticalSection(data->taskGuard);
		//	No tasks, this is signal to terminate
		if (data->tasks->Count() == 0)
		{			
			LeaveCriticalSection( data->taskGuard );

			if( data->needTerminate )
				break;
			else
			{
#ifndef FINAL_BUILD
				r3dOutToLog( "BackgroundTaskDispatcherThreadFunc: SetEvent/WaitForSingleObject mismtach.\n" );
#endif
				continue;
			}
		}

#ifndef FINAL_BUILD
		void CheckLoadTextureStack( r3dTL::TArray<r3dBackgroundTaskDispatcher::TaskDescriptor> *);
		CheckLoadTextureStack( data->tasks );
#endif

		//	Otherwise pick one task and proceed.
		r3dBackgroundTaskDispatcher::TaskDescriptor td( (*data->tasks)[ 0 ] );

		data->tasks->Erase( 0 );

		int foundCollapsable = 0;

		int numCompleted = 1;

		r3dBackgroundTaskDispatcher::TaskDescriptor collapsibleTD;

		if( td.CompareFn )
		{
			for( int i = 0, e = (int)data->tasks->Count(); i < e; i ++ )
			{
				r3dBackgroundTaskDispatcher::TaskDescriptor& td0 = (*data->tasks)[ i ];
				if( td.TaskClass == td0.TaskClass && td.CompareFn( &td, &td0 ) )
				{
					collapsibleTD = td0;
					numCompleted ++;

					data->tasks->Erase( i );
					foundCollapsable = 1;
					break;
				}
			}
		}

		LeaveCriticalSection(data->taskGuard);

		int collapsableBrace = 0;

		if( foundCollapsable )
		{
			if( collapsibleTD.Fn && collapsibleTD.Params )
			{
				collapsibleTD.Params->Cancel = 1;
				collapsableBrace ++;

				r3dCSHolder block( g_ResourceCritSection );
				collapsibleTD.Fn( collapsibleTD.Params );
			}

			CompleteTask( &collapsibleTD );
		}

		//	Exec work function
		if( td.Fn )
		{
			if( foundCollapsable )
			{
				// only execute on foundCollapsable if we have valid params ( can pass 'cancel' through it)

				if( td.Params )
				{
					td.Params->Cancel = 1;
					collapsableBrace --;

					r3dCSHolder block( g_ResourceCritSection );
					td.Fn(td.Params);
				}
			}
			else
			{
				td.Params->Cancel = 0;

				r3dCSHolder block( g_ResourceCritSection );
				td.Fn(td.Params);
			}
		}

		// check if both sides of collapsable operations got canceled
		r3d_assert( !collapsableBrace );

		CompleteTask( &td );

		//	If there are more pending tasks in array, set event to signaled state to continue operation
		EnterCriticalSection(data->taskGuard);
		if (data->tasks->Count() > 0)
			SetEvent(data->startEvent);
		*data->taskCount -= numCompleted;
		LeaveCriticalSection(data->taskGuard);
	}

	r3dOutToLog( "Exiting background thread: %d\n", GetCurrentThreadId() );

	return 0;
}
} // unnamed namespace

//////////////////////////////////////////////////////////////////////////

r3dBackgroundTaskDispatcher::r3dBackgroundTaskDispatcher()
: m_ThreadHandle(0)
, m_StartEvent(0)
, m_TaskCount(0)
{
}

//////////////////////////////////////////////////////////////////////////

r3dBackgroundTaskDispatcher::~r3dBackgroundTaskDispatcher()
{

}

//------------------------------------------------------------------------

void
r3dBackgroundTaskDispatcher::Init()
{
	InitializeCriticalSection(&m_TaskCS);
	m_StartEvent = CreateEvent(0, FALSE, FALSE, 0);

	ThreadData *td = game_new ThreadData;
	td->taskGuard = &m_TaskCS;
	td->startEvent = m_StartEvent;
	td->taskCount = &m_TaskCount;
	td->tasks = &m_Tasks;

	g_pThreadData = td;

	m_TaskPtrParams.Init( 512 );
	m_ThreadHandle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, BackgroundTaskDispatcherThreadFunc, td, 0, NULL));
	if(m_ThreadHandle == NULL)
		r3dError("Failed to begin thread");
}

//------------------------------------------------------------------------

void
r3dBackgroundTaskDispatcher::Close()
{
	r3dOutToLog( "r3dBackgroundTaskDispatcher::Close\n" );

	//	Send termination signal to background thread.
	InterlockedExchange( &g_pThreadData->needTerminate, 1L );

	EnterCriticalSection(&m_TaskCS);
	m_Tasks.Clear();
	LeaveCriticalSection(&m_TaskCS);

	SetEvent(m_StartEvent);
	//	Wait for thread termination
	WaitForSingleObject(m_ThreadHandle, INFINITE);

	//	Close handles
	CloseHandle(m_ThreadHandle);
	CloseHandle(m_StartEvent);
	DeleteCriticalSection(&m_TaskCS);

	SAFE_DELETE( g_pThreadData );
}

//------------------------------------------------------------------------

void r3dBackgroundTaskDispatcher::AddTask(const r3dBackgroundTaskDispatcher::TaskDescriptor &td)
{
	EnterCriticalSection(&m_TaskCS);
	if( !td.Priority )
	{
		m_Tasks.PushBack(td);
	}
	else
	{
		int i = 0;
		for( int e = m_Tasks.Count(); i < e; i ++ )
		{
			if( td.Priority > m_Tasks[ i ].Priority )
				break;
		}

		m_Tasks.Insert( i, td );
	}
	m_TaskCount++ ;
	SetEvent(m_StartEvent);
	LeaveCriticalSection(&m_TaskCS);
}

//------------------------------------------------------------------------

r3dTaskPtrParams*
r3dBackgroundTaskDispatcher::AllocPtrTaskParam()
{
	return m_TaskPtrParams.Alloc() ;
}

//------------------------------------------------------------------------

int
r3dBackgroundTaskDispatcher::GetTaskCount() const
{
	return m_TaskCount ;
}

//------------------------------------------------------------------------

int r3dBackgroundTaskDispatcher::GetTaskCount( TaskClassEnum taskClass )
{
	int count = 0;

	EnterCriticalSection(&m_TaskCS);
	
	for( int i = 0, e = m_Tasks.Count(); i < e; i ++ )
	{
		TaskDescriptor& td = m_Tasks[ i ];

		if( td.TaskClass == taskClass )
		{
			count ++;
		}
	}

	LeaveCriticalSection(&m_TaskCS);

	return count;
}


//------------------------------------------------------------------------

void r3dBackgroundTaskDispatcher::GetPendingTaskClasses( TaskClasses* oTaskClasses )
{
	oTaskClasses->Clear();

	EnterCriticalSection(&m_TaskCS);

	for( int i = 0, e = (int)m_Tasks.Count(); i < e; i ++ )
	{
		oTaskClasses->PushBack( TaskClassEnum( m_Tasks[ i ].TaskClass ) );
	}

	LeaveCriticalSection(&m_TaskCS);
}

//------------------------------------------------------------------------

void r3dBackgroundTaskDispatcher::GetPendingTasks( Tasks* oArr )
{
	oArr->Clear();
	oArr->Reserve( m_Tasks.Count() );

	EnterCriticalSection(&m_TaskCS);

	for( int i = 0, e = (int)m_Tasks.Count(); i < e; i ++ )
	{
		oArr->PushBack( m_Tasks[ i ] );
	}

	LeaveCriticalSection(&m_TaskCS);
}

//------------------------------------------------------------------------

void r3dFinishBackGroundTasksWithProgressReport()
{
	if(g_pBackgroundTaskDispatcher == NULL)
		return;

	if( !g_pBackgroundTaskDispatcher->GetTaskCount() )
		return;

	float REPORT_INTERVAL = 0.125f;
	float lastTime = r3dGetTime();

	int frameEnded = 0;

	r3dRenderer->EndFrame();
	r3dRenderer->EndRender( true );

	// wait for all background tasks to finish
	for( ; g_pBackgroundTaskDispatcher->GetTaskCount(); )
	{
		// loading tasks may issude d3d commands into main thread
		ProcessDeviceQueue( r3dGetTime(), REPORT_INTERVAL );

		if( r3dGetTime() - lastTime > REPORT_INTERVAL && g_pBackgroundTaskDispatcher->GetTaskCount() )
		{
			lastTime = r3dGetTime();

			r3dRenderer->StartFrame();

			d3dc.Reset();

			r3dRenderer->StartRender( 0 );

			r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), r3dRenderer->GetClearZValue(), 0 );

			r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );

			extern	CD3DFont*	MenuFont_Editor;

			r3dRenderer->SetVertexShader();
			r3dRenderer->SetPixelShader();

			char msg[ 512 ];
			sprintf( msg, "Waiting for background tasks (%d)", g_pBackgroundTaskDispatcher->GetTaskCount() );

			MenuFont_Editor->DrawScaledText( 128.f, 64.f, 2, 2, 1, r3dColor::white, msg, 0 );

			r3dProcessWindowMessages();

			r3dRenderer->EndFrame();
			r3dRenderer->EndRender( true );
		}
	}

	r3dRenderer->StartFrame();
	d3dc.Reset();
	r3dRenderer->StartRender( 0 );
}

//------------------------------------------------------------------------

void r3dFinishBackGroundTasks()
{
	if(g_pBackgroundTaskDispatcher == NULL)
		return;
		
	// wait for all background tasks to finish
	for( ; g_pBackgroundTaskDispatcher->GetTaskCount() ; )
	{
		// loading tasks may issude d3d commands into main thread
		ProcessDeviceQueue( r3dGetTime(), 1.0f ) ;
	}
}

//------------------------------------------------------------------------

void r3dTaskParamTimeSlice()
{
	ProcessDeviceQueue( r3dGetTime(), 0.1f ) ;
	Sleep( 1 ) ;
}

void r3dSetAsyncLoading( int onOff )
{
	if( onOff != g_async_loading->GetInt() )
	{
		r3dFinishBackGroundTasks() ;
		g_async_loading->SetInt( onOff ) ;
	}
}