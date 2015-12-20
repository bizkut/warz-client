#ifndef	EXPORTER_H_113123423
#define EXPORTER_H_113123423

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//
#include <string>
#include <vector>
#include <list>
#include "MaxMaterial.h"
#include "SkeletonCandidate.h"
#include "MeshCandidate.h"
#include "..\..\..\..\..\eternity\include\r3dBinFmt.h"

#define MAX_BONE_COUNT	 4
#define WEIGHT_THRESHOLD 0.001f

extern Interface	*_ip;			// 3dsmax interface
extern ExpInterface	*_ei;
extern CMaxInterface	gMaxHelper;		// helper class for 3dsmax interface

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//
enum ExportType
{ 
  EXPORT_ALL, 
  EXPORT_SKELETON, 
  EXPORT_ANIMATION, 
  EXPORT_MESH,
  EXPORT_BONEANIM,
};

class CExporter
{
public:
	BOOL		m_bExportSelected;

	int		m_iStartFrame;
	int		m_iEndFrame;
	int		m_iFPS;

	enum ExportType m_eExportType;

	float		m_fGlobalScale;
	char		m_curDir[MAX_PATH];

	// flags
	int		m_bBatchExport;
	
	// configurable variables	
	int		m_ExpCopyTex;
	int		m_ExpConvertToDDS;
	char		m_nvdxtPath[MAX_PATH];
	char		m_nvdxtParams[512];
	int		m_ExpSaveMats;
	int		m_ExpVtxColors;
	int		m_NormalTolerance;
	//

	std::vector<INode*>	m_allMeshes;

	// TRUE if we have mesh with attached bones to it.
	int		m_bBoneAnimationPresent;
	std::vector<std::string>	m_smcBoundBoxes;

// constructors/destructor
public:
	CExporter();
	virtual ~CExporter();

// member functions
public:
	void		Create();
	void		  WalkAllNodes(INode *pNode);

	void		SetDefaultConfig();

	void		ExportAll(const std::string& strFilename);

	BOOL		WalkAllNotMeshNodes(INode* pNode, const std::string& strBase); 

	void		ExportSkeleton(const std::string& strFilename);
	void		 WriteSkeleton(FILE *f, TimeValue time);

	void		ExportBoneAnimation(const std::string& strFilename);
	void		 WriteBoneAnimation(FILE *f);

	void		ExportMesh(const std::string& strFilename);
	void		 WriteSingleMesh(INode *pNode, char *fname, TimeValue time);
	void		  WriteMeshData(FILE *f, const CMesh& mesh);
	void		  WriteMeshWeights(FILE *f, const CMesh& mesh);
	
	void		ShowVertexCount();
};

extern	CExporter	theExporter;
extern	UD3D9		d3d9;

extern	const char*	U_FixName(const char* name);
extern	Point3		U_FixCoordSys(const Point3 &vector, BOOL bScale);
extern	Matrix3		U_FixCoordSys(Matrix3 &m);
extern	void		U_WriteMatrix(FILE *f, Matrix3 m);
extern	void		ThrowError(const char* strText, ...);


//
// macros for 3dsmax export interfaces (from file->export)  
//

#define EXPORTER_MSG_HEADER	"TSG Exporter"


#define	PREPARE_TO_EXPORT(ip, ei)			\
  AFX_MANAGE_STATE(AfxGetStaticModuleState());		\
  /* store max interface pointer */			\
  _ip = ip;						\
  _ei = ei;						\
  /* create an exporter instance */			\
  theExporter.Create();					\
  theExporter.m_bExportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;	\
  /* build a skeleton  */				\
  theSkeleton.CreateFromInterface();			\
  /* init D3D */					\
  d3d9.Init();		

#define EXPORT_BEGIN1\
  int iExpResult = 1;\
  try 
  
#define EXPORT_END1	\
  catch(char *msg) {\
    MessageBox(NULL, msg, EXPORTER_MSG_HEADER, MB_OK | MB_ICONEXCLAMATION);\
    iExpResult = 0;\
  }\
  catch(...) {\
    MessageBox(NULL, "plugin module crashed", EXPORTER_MSG_HEADER, MB_OK | MB_ICONSTOP);\
    iExpResult = 0;\
  }\
  ip->ProgressEnd();\
  return iExpResult;


#endif	//EXPORTER_H_113123423
