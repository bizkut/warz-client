#ifndef R3D_JOBCHIEF_H
#define R3D_JOBCHIEF_H

class JobChief
{
public:
	typedef void (*ExecFunc)( void* Data, size_t ItemStart, size_t ItemCount, size_t ThreadIndex );

	typedef r3dTL::TArray< HANDLE > ThreadHandles;

public:
	JobChief();
	~JobChief();

public:
	void Init();
	void Close();

	void Exec( ExecFunc func, void* data, size_t itemCount );

	void BeginQueueMode();
	void EndQueueMode();

	uint32_t GetThreadCount() const;

private:
	void FireThreads() ;

private:
	ThreadHandles	mThreadHandles;
	bool			mInQueueMode;

};
extern JobChief* g_pJobChief;

#endif