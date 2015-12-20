//=========================================================================
//	Module: r3dProfileDataRecorder.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"

#if !DISABLE_PROFILER

#include "r3d.h"

#include "r3dProfiler.h"
#include "../../EclipseStudio/Sources/GameLevel.h"

#include <Shlwapi.h>

//////////////////////////////////////////////////////////////////////////

r3dString gProfileDataXMLOutFile;
r3dString gProfileDataTXTOutFile;
r3dString gProfileDataPath ;
int gProfileRecorderStartFrame = -1;
float gScheduledProfileTime = 0.0f;
bool gProfileD3DFromCommandLine = false;

//////////////////////////////////////////////////////////////////////////

namespace
{
	struct ProfilerItem
	{
		r3dString name;
		float averageTime;
		int numCalls;

		ProfilerItem()
		: averageTime(0)
		, numCalls(0)
		{}
	};

	bool operator < (const ProfilerItem &l, const ProfilerItem &r)
	{
		return l.averageTime > r.averageTime;
	}

	/**	Output sample node into xml tree. */
	pugi::xml_node AppendSampleXML(pugi::xml_node &parentNode, const char *name, float time, int numCalls)
	{
		pugi::xml_node node = parentNode.append_child();
		node.set_name("sample");
		node.append_attribute("name") = name;
		node.append_attribute("time") = time;
		node.append_attribute("num_calls") = numCalls;
		return node;
	}

	/**	Output sample text entry into file. */
	void AppendSampleTXT(FILE *f, const char *name, float time, int numCalls, int depth)
	{
		char expandedName[ 512 ] ;

		{
			int i, e ;
			for( i = 0, e = depth * 2 ; i < e ; i ++ )
			{
				expandedName[ i ] = ' ' ;
			}

			expandedName[ i ] = 0 ;

			strcat ( expandedName, name ) ;
		}

		fprintf( f, "%-52s - %6.2f - %5d\n", expandedName, time, numCalls ) ;
	}

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////

void DumpProfilerHierarchyRecursively(r3dProfilerSample *s, pugi::xml_node &parentNode, FILE* txtFile, int frame, int depth, float totalTime)
{
	int numFrames = d_profile_dump_num_frames->GetInt();
	float siblingsTime = 0;
	bool printUnprofiled = false;

	while (s)
	{
		int numCalls = s->GetAverageNumCalls(numFrames) ;
		float time = s->GetAverageTime(numFrames) * 1000 ;
		siblingsTime += time;
		if (time > 0.009999999f)
		{
			const char *name = s->GetItem()->GetName();
			//	Xml tag
			pugi::xml_node node = AppendSampleXML(parentNode, name, time, numCalls);
			//	Text entry
			AppendSampleTXT(txtFile, name, time, numCalls, depth);

			r3dProfilerSample *child = s->GetChild();

			if (child)
			{
				DumpProfilerHierarchyRecursively(child, node, txtFile, frame, depth + 1, time);
				printUnprofiled = true;
			}
		}
		s = s->GetSibling();
	}

	if (printUnprofiled)
	{
		//	Output unprofiled time
		float timeDiff = R3D_MAX(totalTime - siblingsTime, 0.f);
		if (timeDiff > 0.01)
		{
			const char *name = "UNPROFILED";
			AppendSampleXML(parentNode, name, timeDiff, 0);
			AppendSampleTXT(txtFile, name, timeDiff, 0, depth);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool DoDumpProfilerData()
{
	int numFrames = d_profile_dump_num_frames->GetInt();
	r3dProfiler *p = r3dProfiler::Instance();

	int curFrame = p->GetCurrentFrame();
	struct ProfileTxt
	{
		ProfileTxt()
		{
			file = fopen( gProfileDataTXTOutFile.c_str(), "wb" ) ;
		}

		~ProfileTxt()
		{
			fclose( file ) ;
		}


		FILE* file ;
	} txt;

	if (gProfileRecorderStartFrame + numFrames > curFrame)
	{
		//	Wait for data gather
		return false;
	}
	gProfileRecorderStartFrame = -1;

	typedef r3dgfxVector(ProfilerItem) ProfilerItems;
	ProfilerItems dumpItems;
	dumpItems.reserve(500);

	r3dProfilerItem * item = p->GetFirstItem();

	while (item)
	{
		float time = item->GetAverageTime(numFrames) * 1000;
		if (time > 0.009999999f)
		{
			dumpItems.resize(dumpItems.size() + 1);
			dumpItems.back().averageTime = time;
			dumpItems.back().numCalls = item->GetAverageNumCalls(numFrames);
			dumpItems.back().name = item->GetName();
		}

		item = item->GetNext();
	}
	std::sort(dumpItems.begin(), dumpItems.end());

	//	Now write xml with collected data
	pugi::xml_document xmlProfileData;
	pugi::xml_node root = xmlProfileData.append_child() ;
	root.set_name( "root" ) ;
	pugi::xml_node flatProfileInfo = root.append_child() ;
	flatProfileInfo.set_name("flat_profile_info");

	fprintf( txt.file, "Flat Data\n\n" ) ;
	fprintf( txt.file, "%-32s - %6s - %5s\n", "Name", "Time", "Calls" ) ;

	for (size_t i = 0; i < dumpItems.size(); ++i)
	{
		ProfilerItem &pi = dumpItems[i];
		pugi::xml_node xmlEntry = flatProfileInfo.append_child();
		xmlEntry.set_name("entry");
		xmlEntry.append_attribute("name") = pi.name.c_str();
		xmlEntry.append_attribute("time") = pi.averageTime;
		xmlEntry.append_attribute("num_calls") = pi.numCalls;

		fprintf( txt.file, "%-32s - %6.2f - %5d\n", pi.name.c_str(), pi.averageTime, pi.numCalls );
	}

	//	Write hierarchy info
	r3dProfilerSample *rootSample = r3dProfiler::Instance()->GetRoot();
	pugi::xml_node hierarchyInfo = root.append_child();
	hierarchyInfo.set_name("hierarchy_profile_info");

	fprintf( txt.file, "\n\n\nHierarchical Data\n\n" ) ;
	fprintf( txt.file, "%-52s - %6s - %5s\n", "Name", "Time", "Calls" ) ;

	DumpProfilerHierarchyRecursively(rootSample, hierarchyInfo, txt.file, numFrames, 0, 0);

	//	Add occlusion queries
	r3dProfilerSample *qs = r3dProfiler::Instance()->GetQuerySample();

	pugi::xml_node queryData = xmlProfileData.append_child();
	queryData.set_name("occlusion_query_info");

	queryData.append_attribute("time") = qs->GetAverageTime(numFrames) * 1000;
	queryData.append_attribute("num_queries") = qs->GetAverageNumCalls(numFrames);

	//	Dump hierarchy info
	xmlProfileData.save_file(gProfileDataXMLOutFile.c_str());
	return true;
}

//------------------------------------------------------------------------

void DumpProfileData()
{
	if (gProfileRecorderStartFrame == -1)
		return;

	if (r_profiler_paused->GetBool())
	{
		r3dProfiler::Instance()->SetPaused(false);
		r_profiler_paused->SetBool(false);

		if (gProfileD3DFromCommandLine)
		{
			r_profiler_d3d->SetBool(true);
		}
	}

	if (DoDumpProfilerData())
		MessageBox( 0, "Profiling finished!", "Message", MB_OK );
}

//////////////////////////////////////////////////////////////////////////

void ScheduleProfileDataDump()
{
	gProfileRecorderStartFrame = r3dProfiler::Instance()->GetCurrentFrame();
}

//////////////////////////////////////////////////////////////////////////

bool CreateProfileDumpFolders( const char* LevelDir )
{
	r3dSTLString s = LevelDir ;
	//	Remove 'Level\' substring sting
	s.erase(0, 7);

	//	Construct profile data folder
	r3dString path = r3dString("profiling\\") + s.c_str() + "_";

	const char *timeFormat = "HH'_'mm'_'ss";
	const char *dateFormat = "ddd',' MMMdd yyyy";

	int bufSizeTime = GetTimeFormat
	(
		LOCALE_INVARIANT,
		TIME_FORCE24HOURFORMAT,
		0,
		timeFormat,
		0,
		0
	);

	int bufSizeDate = GetDateFormat
	(
		LOCALE_INVARIANT,
		0,
		0,
		dateFormat,
		0,
		0
	);

	int bufSize = R3D_MAX(bufSizeDate, bufSizeTime);
	char *buf = game_new char[bufSize];

	bufSizeDate = GetDateFormat
	(
		LOCALE_INVARIANT,
		0,
		0,
		dateFormat,
		buf,
		bufSize
	);
	r3d_assert(bufSizeDate > 0);
	path += buf + r3dString("_");

	bufSizeTime = GetTimeFormat
	(
		LOCALE_INVARIANT,
		TIME_FORCE24HOURFORMAT,
		0,
		timeFormat,
		buf,
		bufSize
	);
	r3d_assert(bufSizeTime > 0);
	path += buf;

	delete [] buf;

	char fullPath[MAX_PATH] = {0};
	PathSearchAndQualify(path.c_str(), fullPath, _countof(fullPath));

	int rv = SHCreateDirectoryEx
	(
		0,
		fullPath,
		0
	);

	if (rv != ERROR_SUCCESS && rv != ERROR_FILE_EXISTS && rv != ERROR_ALREADY_EXISTS)
		return false ;

	gProfileDataXMLOutFile = fullPath;
	gProfileDataTXTOutFile = fullPath;
	gProfileDataPath = fullPath ;
	gProfileDataXMLOutFile += "\\profile_data.xml";
	gProfileDataTXTOutFile += "\\profile_data.txt";

	return true ;
}

//------------------------------------------------------------------------

void DumpProfileScreenShot()
{
	//	Save screenshot
	r_do_screenshot->SetBool(true);
	r3dString  fullPath = gProfileDataPath ;
	fullPath += "\\screenshot.tga";
	r_screenshot_name->SetString(fullPath.c_str());
}

//------------------------------------------------------------------------

void DumpAutomaticProfileInfo( const char* LevelFolder )
{
	DumpProfileData();

	if (gScheduledProfileTime <= 0 || gScheduledProfileTime >= r3dGetTime())
		return;

	gScheduledProfileTime = -1;

	CreateProfileDumpFolders( LevelFolder );

	ScheduleProfileDataDump();

	DumpProfileScreenShot();
}

#endif