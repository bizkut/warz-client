#include "StdAfx.h"
#include "Exporter.h"
#include "VertexCandidate.h"
#include "BoneCandidate.h"

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBone::CBone()
{
  m_pNode = 0;
}

CBone::~CBone()
{
}

//----------------------------------------------------------------------------//
// Add a child id to the child id list                                        //
//----------------------------------------------------------------------------//
void CBone::AddChildId(int childId)
{
  m_listChildId.push_back(childId);
  return;
}

//----------------------------------------------------------------------------//
// Create a bone                                                     //
//----------------------------------------------------------------------------//
void CBone::Create(int id, int parentId, INode *pNode)
{
  m_id        = id;
  m_parentId  = parentId;
  m_pNode     = pNode;
  m_bSelected = ((theExporter.m_eExportType == EXPORT_ANIMATION) ? !gMaxHelper.IsDummy(pNode) : 1); 

  // reference in CExporter::WriteBoneAnimation, binary animation format restriction
  size_t boneNameLen = strlen(GetName());
  if(boneNameLen > 31)
	ThrowError("by animation binary format we can't have bones with names larger that 31 chars\n\nplease rename '%s' (%d chars)", GetName(), boneNameLen);
}

//----------------------------------------------------------------------------//
// Get the id of the bone                                            //
//----------------------------------------------------------------------------//
int CBone::GetId()
{
  return m_id;
}

//----------------------------------------------------------------------------//
// Get the child id list                                                      //
//----------------------------------------------------------------------------//
std::list<int> CBone::GetListChildId()
{
  return m_listChildId;
}

//----------------------------------------------------------------------------//
// Get the name of the bone                                          //
//----------------------------------------------------------------------------//
const char* CBone::GetName()
{
  // check if the node is valid
  if(m_pNode == 0) 
    return "_null_";

  return U_FixName(m_pNode->GetName());
}

//----------------------------------------------------------------------------//
// Get the node of the bone                                          //
//----------------------------------------------------------------------------//
INode *CBone::GetNode()
{
  return m_pNode;
}

//----------------------------------------------------------------------------//
// Get the id of the parent bone                                     //
//----------------------------------------------------------------------------//
int CBone::GetParentId()
{
  return m_parentId;
}

int CBone::GetChildId()
{
  return m_listChildId.front();
}

//----------------------------------------------------------------------------//
// Get the select state of the bone                                  //
//----------------------------------------------------------------------------//
BOOL CBone::IsSelected()
{
  return m_bSelected;
}

BOOL CBone::HasChild()
{ 
  return m_listChildId.size() > 0;
}

//----------------------------------------------------------------------------//
// Set the select state of the bone                                  //
//----------------------------------------------------------------------------//

void CBone::SetSelected(BOOL bSelected)
{
  m_bSelected = bSelected;
}
