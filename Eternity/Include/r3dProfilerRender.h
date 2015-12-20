#ifndef __R3D_PROFILER_RENDER_H__
#define __R3D_PROFILER_RENDER_H__

#if !DISABLE_PROFILER

#include "r3dProfiler.h"

// renders nice graphics and usage stuff
class r3dProfileRender
{
public:
	static bool Create();
	static bool Destroy();
	static r3dProfileRender* Instance() {return sInstance;}

	void SetLevelFolder( const char* LevelFolder ) ;

	void Render();
private:
	static r3dProfileRender* sInstance;

	void RenderD3DMarks() ;
	void RenderProfiler() ;

	void RenderSummary();
	void RenderHierarchy();
	void RenderCursor();

	void PrintLegend();
	void DrawHierarchyLine(r3dProfilerSample* aSample, int aDepth = 0);
	void PrintHierarchySample(r3dProfilerSample* aSample, float aX, float& aY, int aDepth = 0);
	void PrintHeirarchySampleSum(r3dProfilerSample* aSample, float aX, float& aY, int aDepth, float aSeconds);

	bool PrintItem(const char* aName, float aX, float aY, int a_iDepth, int aNumCalls, float aSeconds, const r3dColor& aColor = r3dColor(200, 200, 255, 255), bool aOpenFlag = false, bool aChildrenFlag = false);

	void StartLineRender();
	void StopLineRender();
	void DrawHistoryLine(r3dProfilerHistory* aHistory, float aY);

	int GetSampleOffset( r3dProfilerSample* aCurSample, r3dProfilerSample* aSample, int *ioOffset );

	r3dProfileRender();
	~r3dProfileRender();

	r3dProfilerSample* m_CursorSample;

	float m_TotalTime;

	int m_CursorFrame;
	float m_LeftBorder;

	float m_YOffset;

	int m_PrintSampleStart ;
	int m_PrintSamplePos ;

	r3dString m_LevelFolder ;

	r3dTL::TArray< float > m_HierarchyPoses ;

};

#endif //!DISABLE_PROFILER

#endif //__R3D_PROFILER_RENDER_H__