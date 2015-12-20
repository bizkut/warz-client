// DlgSceneExport.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSceneExport.h"
#include "Resource.h"
#include "../Common/Config.h"

#include "Exporter.h"	// need for configurable vars

// DlgSceneExport dialog

IMPLEMENT_DYNAMIC(DlgSceneExport, CDialog)

DlgSceneExport::DlgSceneExport(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSceneExport::IDD, pParent)
{
}

DlgSceneExport::~DlgSceneExport()
{
}

void DlgSceneExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_TEXCOPY, m_CheckTexCopy);
	DDX_Control(pDX, IDC_CHECK_CONVDDS, m_CheckConvertDDS);
	DDX_Control(pDX, IDC_EXPORTPATH, m_EdExportPath);
	DDX_Control(pDX, IDC_NRM_EPSILON, m_NrmEpsilon);
	DDX_Control(pDX, IDC_NRM_TEXT, m_NrmText);
	DDX_Control(pDX, IDC_CHECK_SAVEMATS, m_CheckSaveMats);
	DDX_Control(pDX, IDC_CHECK_VTXCOLOR, m_CheckVtxColors);
	DDX_Control(pDX, IDC_EDIT1, m_EditNvdxtPath);
	DDX_Control(pDX, IDC_EDIT2, m_EditNvdxtParams);
}


BEGIN_MESSAGE_MAP(DlgSceneExport, CDialog)
	ON_BN_CLICKED(IDC_SCENE_EXPORT, &DlgSceneExport::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK_TEXCOPY, &DlgSceneExport::OnBnClickedCheckTexcopy)
	ON_BN_CLICKED(IDC_CHECK_CONVDDS, &DlgSceneExport::OnBnClickedCheckConvdds)
	ON_BN_CLICKED(IDCLOSE, &DlgSceneExport::OnBnClickedClose)
	ON_BN_CLICKED(IDC_BROWSE, &DlgSceneExport::OnBnClickedBrowse)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// DlgSceneExport message handlers

void DlgSceneExport::ReadConfig()
{
  extern Interface* _ip;
  m_exportPath   = g_Config.GetStr("scene_path", _ip->GetDir(APP_EXPORT_DIR));
  
  m_NrmEpsilon.SetRange(0, 20, TRUE);
  m_NrmEpsilon.SetPos(theExporter.m_NormalTolerance);
  m_NrmEpsilon.SetTicFreq(1);
  char buf[256];
  sprintf(buf, "%d degrees", theExporter.m_NormalTolerance);
  m_NrmText.SetWindowText(buf);
}

void DlgSceneExport::WriteConfig()
{
  CString m_nvdxtPath;
  m_EditNvdxtPath.GetWindowText(m_nvdxtPath);
  CString m_nvdxtParams;
  m_EditNvdxtParams.GetWindowText(m_nvdxtParams);
  
  theExporter.m_ExpSaveMats = m_CheckSaveMats.GetCheck();
  theExporter.m_ExpVtxColors = m_CheckVtxColors.GetCheck();
  strcpy(theExporter.m_nvdxtPath, m_nvdxtPath.GetBuffer());
  strcpy(theExporter.m_nvdxtParams, m_nvdxtParams.GetBuffer());

  g_Config.SetStr("scene_path",      m_exportPath.c_str());
  g_Config.SetInt("scene_copytex",   theExporter.m_ExpCopyTex);
  g_Config.SetInt("scene_ddsconv",   theExporter.m_ExpConvertToDDS);
  g_Config.SetStr("nvdxt_path",      theExporter.m_nvdxtPath);
  g_Config.SetStr("nvdxt_params",    theExporter.m_nvdxtParams);
  g_Config.SetInt("scene_nrmeps",    theExporter.m_NormalTolerance);

  g_Config.SaveConfig();
  return;
}

BOOL DlgSceneExport::OnInitDialog()
{
  __super::OnInitDialog();
  
  ReadConfig();
  
  m_EdExportPath.SetWindowText(m_exportPath.c_str());
  m_CheckTexCopy.SetCheck(theExporter.m_ExpCopyTex);
  m_CheckConvertDDS.SetCheck(theExporter.m_ExpConvertToDDS);
  m_EditNvdxtPath.SetWindowText(theExporter.m_nvdxtPath);
  m_EditNvdxtParams.SetWindowText(theExporter.m_nvdxtParams);
  m_CheckSaveMats.SetCheck(theExporter.m_ExpSaveMats);
  m_CheckVtxColors.SetCheck(0);

  return TRUE;
}

void DlgSceneExport::OnBnClickedButton1()
{
  WriteConfig();
  EndDialog(IDC_SCENE_EXPORT);
}

void DlgSceneExport::OnBnClickedCheckTexcopy()
{
  theExporter.m_ExpCopyTex = m_CheckTexCopy.GetCheck();
}

void DlgSceneExport::OnBnClickedCheckConvdds()
{
  theExporter.m_ExpConvertToDDS = m_CheckConvertDDS.GetCheck();
}

void DlgSceneExport::OnBnClickedClose()
{
  EndDialog(IDCANCEL);
}

void DlgSceneExport::OnBnClickedBrowse()
{
  char dir[MAX_PATH];
  if(!DirectoryPicker(GetSafeHwnd(), "Select Export Directory", m_exportPath.c_str(), dir, NULL))
    return;
    
  m_exportPath = dir;
  m_EdExportPath.SetWindowText(dir);
}

void DlgSceneExport::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if(pScrollBar == (CScrollBar*)&m_NrmEpsilon) 
  {
    theExporter.m_NormalTolerance = m_NrmEpsilon.GetPos();
    char buf[256];
    sprintf(buf, "%d degrees", theExporter.m_NormalTolerance);
    m_NrmText.SetWindowText(buf);
  }

  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

