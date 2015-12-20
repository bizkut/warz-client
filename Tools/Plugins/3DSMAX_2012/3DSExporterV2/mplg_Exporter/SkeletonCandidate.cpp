//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "Exporter.h"
#include "SkeletonCandidate.h"
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


	CSkeleton	theSkeleton;

CSkeleton::CSkeleton()
{
  pBipMaster = NULL;
  pBipExport = NULL;
}

CSkeleton::~CSkeleton()
{
  Clear();
}

BOOL CSkeleton::AddNode(INode *pNode, int parentId)
{
  // check if the node is valid
  if(pNode == 0)
    ThrowError("Invalid handle!");

  bool isExcluded = false;
  // exclude Nub bones!
  const char* boneName = pNode->GetName();
  const size_t boneLen = strlen(boneName);
  if(boneLen > 3 && strcmp(boneName + boneLen - 3, "Nub") == 0) {
    U_Log("!dbone %s excluded\n", boneName);
    isExcluded = true;
  }

  // check if the node is a bone
  if(!isExcluded && (gMaxHelper.IsBone(pNode) || gMaxHelper.IsDummy(pNode)))
  {
    CBone *pBone;
    pBone = CreateBone(pNode, parentId);

    // adjust skeleton id..
    for(int i=0; i<strlen(pNode->GetName()); i++)
      dwSkeletonID += (pNode->GetName()[i]) * int(m_vBones.size());
    
    // if this is a root bone, lets try to get biped controllers
    if(parentId == -1) {
      Control* pCtrl = pNode->GetTMController(); 
      if(pCtrl) {
        pBipMaster = (IBipMaster*)(pCtrl->GetInterface(I_BIPMASTER));
        pBipExport = (IBipedExport *)pCtrl->GetInterface(I_BIPINTERFACE);
      }
    }

    // set parent id for the childrens
    parentId = pBone->GetId();
  }

  // handle all children of the node
  for(int childId = 0; childId < pNode->NumberOfChildren(); childId++) 
  {
    AddNode(pNode->GetChildNode(childId), parentId);
  }

  return TRUE;
}

//----------------------------------------------------------------------------//
// Clear this skeleton                                               //
//----------------------------------------------------------------------------//
void CSkeleton::Clear()
{
  // destroy all bones stored in this skeleton 
  int boneId;
  for(boneId = 0; boneId < m_vBones.size(); boneId++)
  {
    delete m_vBones[boneId];
  }
  m_vBones.clear();

  // clear root bone  id list
  m_vRootBonesID.clear();
}

///////////////////////////////////////
// Create skeleton bone 
///////////////////////////////////////
CBone * CSkeleton::CreateBone(INode* pNode, int parentId) 
{
  // allocate a new bone 
  CBone *pBone = new CBone;
  // create the bone 
  pBone->Create(int(m_vBones.size()), parentId, pNode);

  // insert node element into hierarchy
  m_vBones.push_back(pBone);

  // insert node element id into parent node element
  if(parentId == -1)
  {
    // no parent -> this is a root node
    m_vRootBonesID.push_back(pBone->GetId());
    //U_Log("bone: %s root\n", pNode->GetName());
  }
  else
  {
    // valid parent -> this is a child node
    m_vBones[parentId]->AddChildId(pBone->GetId());
    //U_Log("bone: %s, child of %s\n", pNode->GetName(), m_vBones[parentId]->GetName());
  }
    
  return pBone;
}

//----------------------------------------------------------------------------//
// Create a skeleton  instance from the exporter interface           //
//----------------------------------------------------------------------------//

BOOL CSkeleton::CreateFromInterface()
{
  // clear current content
  Clear();
  
  dwSkeletonID = 0;

  // add root node always
  if(!AddNode(_ip->GetRootNode(), -1)) 
    return FALSE;

  // high byte of skeleton ID will be number of bones    
  dwSkeletonID = (dwSkeletonID & 0x00FFFFFF) | (DWORD(m_vBones.size()) << 24);

  AdjustAnimationRange();

/*
  for(int i=0; i<m_vBones.size(); i++) {
    CBone *bc = m_vBones[i];

    Control *tmControl = bc->GetNode()->GetTMController();

    if(tmControl->ClassID() == LINKCTRL_CLASSID) {
      ILinkCtrl* ilk = (ILinkCtrl*)tmControl->GetInterface(LINK_CONSTRAINT_INTERFACE);  
      if(ilk && ilk->GetNumTargets()) {
        INode *pTarget = ilk->GetNode(0);

	U_Log("found linkcontroller node! targetting to %p\n", pTarget);

	for(int j=0; j<m_vBones.size(); j++) {
	  if(pTarget == m_vBones[j]->GetNode()) {
	    U_Log("found a link to bone %d\n", j);
	    // remove current parent, and set parent to this bone!
            if(bc->m_parentId != -1) {
	      m_vBones[bc->m_parentId]->m_listChildId.remove(bc->GetId());
	    } else {
	      m_vRootBonesID.remove(bc->GetId());
	    }
	    bc->m_parentId = j;
            m_vBones[j]->AddChildId(bc->GetId());
	  }
	}
      }
    }
  }
*/

  return TRUE;
}

void CSkeleton::AdjustAnimationRange()
{
  if(!pBipMaster) 
    return;

  Interval iv = pBipMaster->GetCurrentRange();
  theExporter.m_iStartFrame  = iv.Start() / GetTicksPerFrame();
  theExporter.m_iEndFrame    = iv.End() / GetTicksPerFrame();
}

//----------------------------------------------------------------------------//
// Get the bone id for a given bone name                                      //
//----------------------------------------------------------------------------//
int CSkeleton::FindBoneByNode(INode *pNode)
{
  // loop through all bones
  int boneId;
  for(boneId = 0; boneId < m_vBones.size(); boneId++)
  {
    // check if the bone  is the one we are looking for
    if(m_vBones[boneId]->GetNode() == pNode) 
      return boneId;
  }

  ThrowError("FindBoneByNode(%s): bone not in skeleton", pNode->GetName());
  return -1;
}

//----------------------------------------------------------------------------//
// Get the Bone for a given bone name                                      //
//----------------------------------------------------------------------------//

CBone *CSkeleton::GetBone(int boneId)
{
  return m_vBones[boneId];
}

//----------------------------------------------------------------------------//
// Get the number of selected bones                                 //
//----------------------------------------------------------------------------//
int CSkeleton::GetSelectedCount()
{
	// loop through all bones
	int boneId;
	int selectedCount;
	for(boneId = 0, selectedCount = 0; boneId < m_vBones.size(); boneId++)
	{
		// check if the bone  is selected
		if(m_vBones[boneId]->IsSelected()) selectedCount++;
	}

	return selectedCount;
}

//----------------------------------------------------------------------------//
// Get the bone's reletive transformation matrix at a given time.             //
//----------------------------------------------------------------------------//
Matrix3 CSkeleton::GetBoneRTM(int boneId, TimeValue time)
{
	// check if the bone  id is valid
	if((boneId < 0) || (boneId >= m_vBones.size())) return NULL;

	// get the bone 
	CBone *pBone = m_vBones[boneId];
	// get the node of the bone 
	INode *pNode = pBone->GetNode();

	// get the parent id
	int parentId;
	parentId = pBone->GetParentId();

	// get the node of the parent bone 
	INode *pParentNode;
	if(parentId == -1)
	{
		pParentNode = 0;
	}
	else
	{
		pParentNode = m_vBones[parentId]->GetNode();
	}

	return gMaxHelper.GetNodeRTM(pNode, pParentNode, time);
}
