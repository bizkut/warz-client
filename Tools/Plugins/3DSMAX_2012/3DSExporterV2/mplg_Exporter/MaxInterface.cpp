//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "Exporter.h"

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


	Interface	*_ip = NULL;
	ExpInterface	*_ei = NULL;
	CMaxInterface	gMaxHelper;

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//
CMaxInterface::CMaxInterface()
{
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//
CMaxInterface::~CMaxInterface()
{
}

//----------------------------------------------------------------------------//
// Get if user canceled the export                                            //
//----------------------------------------------------------------------------//
BOOL CMaxInterface::GetCancel()
{
  return _ip->GetCancel() ? TRUE : FALSE;
}

//----------------------------------------------------------------------------//
// Get the current frame of the animation                                     //
//----------------------------------------------------------------------------//
TimeValue CMaxInterface::GetMaxTime()
{
  return _ip->GetTime();
}

//----------------------------------------------------------------------------//
// Get the main window of the host application                                //
//----------------------------------------------------------------------------//
CWnd *CMaxInterface::GetMainWnd()
{
  return CWnd::FromHandle(_ip->GetMAXHWnd());
}


// Return a pointer to a TriObject given an INode or return NULL
// if the node cannot be converted to a TriObject
static TriObject* GetTriObjectFromNode(INode *pNode, TimeValue t, int &deleteIt)
{
  Object *obj = pNode->EvalWorldState(t).obj;

  deleteIt = FALSE;

  if(obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) {
    TriObject *tri = (TriObject *)obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));
    // Note that the TriObject should only be deleted
    // if the pointer to it is not equal to the object
    // pointer that called ConvertToType()
    if(obj != tri) 
      deleteIt = TRUE;
    return tri;
  } else {
    return NULL;
  }
}

//----------------------------------------------------------------------------//
// Get the mesh of a given node                                               //
//----------------------------------------------------------------------------//
CMaxMesh *CMaxInterface::GetMesh(INode *pNode, TimeValue time)
{
  // check for invalid nodes
  if(pNode == 0)
    ThrowError("Invalid handle.");

  // create 3dsmax TriMesh object
  BOOL bDeleteMesh;
  TriObject *pTriObject  = GetTriObjectFromNode(pNode, time, bDeleteMesh);

  if(!pTriObject)
    return NULL;

  // allocate a new max mesh instance
  CMaxMesh *pMaxMesh;
  pMaxMesh = new CMaxMesh();
  if(pMaxMesh == 0)
    ThrowError("Memory allocation failed.");

  // create the max mesh
  pMaxMesh->Create(pNode, pTriObject, bDeleteMesh ? TRUE : FALSE, time);

  return pMaxMesh;
}

//----------------------------------------------------------------------------//
// Get the transformation matrix of a given max node                          //
//----------------------------------------------------------------------------//
Matrix3 CMaxInterface::GetNodeTM(INode *pNode, TimeValue time)
{
  // initialize matrix with the identity
  Matrix3 tm(TRUE);

  // only do this for valid nodes
  if(pNode != 0) {
    // get the node transformation
    tm = pNode->GetNodeTM(time);

    extern Matrix3 GetRigidTransform(Matrix3& tm);
    tm = GetRigidTransform(tm);

    // make the transformation uniform
    //tm.NoScale();	// it does not work with ISkin matrices by some reason!
  }

  return U_FixCoordSys(tm);
}

Matrix3 CMaxInterface::GetObjectTM(INode *pNode, TimeValue time)
{
  // initialize matrix with the identity
  Matrix3 tm(TRUE);

  // only do this for valid nodes
  if(pNode != 0) {
    // get the node transformation
    tm = pNode->GetObjectTM(time);

    extern Matrix3 GetRigidTransform(Matrix3& tm);
    tm = GetRigidTransform(tm);

    // make the transformation uniform
    //tm.NoScale();	// it does not work with ISkin matrices by some reason!
  }

  return U_FixCoordSys(tm);
}


//----------------------------------------------------------------------------//
// Get the relative transformation matrix of a given node at a given time     //
//----------------------------------------------------------------------------//
Matrix3 CMaxInterface::GetNodeRTM(INode *pNode, INode *pParentNode, TimeValue time)
{
  if(pParentNode == 0)
    return GetNodeTM(pNode, time);

#if 0 

  /* 
  Notice that for the node we call GetObjectTM() and for the parent we call
  GetNodeTM(). The reason for this that in MAX a matrix can, besides the
  regular translation, also contain a translation that is *not* inherited to
  its children in the tree but which is used when drawing geometry at the
  given level.
  So for the parent we want the transform *without* this offset and the the child
  we want the transform *with* this offset.
  */

  //NOTE: some models do not work with thic technique. Some works with GetObjectTM replaced with GetNodeTM() with GetRigidTransform intact
  

  // calculate transform from root to node.
  Matrix3 tm = U_FixCoordSys(GetRigidTransform(pNode->GetObjectTM(time)));

  // calculate transform from root to parent.
  Matrix3 parent_tm = U_FixCoordSys(GetRigidTransform(pParentNode->GetNodeTM(time)));
  tm = tm * Inverse(parent_tm);
  
  return tm;
#else  

  // calculate the relative transformation standart way
  return GetNodeTM(pNode, time) * Inverse(GetNodeTM(pParentNode, time));

#endif  
}

//----------------------------------------------------------------------------//
// Check if the given node is a biped bone                                    //
//----------------------------------------------------------------------------//
BOOL CMaxInterface::IsBipedBone(INode *pNode)
{
	// check for invalid nodes
	if(pNode == 0) return FALSE;

	// check for root node
	if(pNode->IsRootNode()) return FALSE;

	// check for bone node
	ObjectState os;
	os = pNode->EvalWorldState(GetMaxTime());
	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0)) return FALSE;

	// check for biped node
	Control *pControl;
	pControl = pNode->GetTMController();
	if((pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || (pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)) return TRUE;

	return FALSE;
}

//----------------------------------------------------------------------------//
// Check if the given node is a bone                                          //
//----------------------------------------------------------------------------//
BOOL CMaxInterface::IsBone(INode *pNode)
{
	// check for invalid nodes
	if(pNode == 0) return FALSE;

	// check for root node
	if(pNode->IsRootNode()) return FALSE;

	// check for bone node
	ObjectState os;
	os = pNode->EvalWorldState(GetMaxTime());
	if(os.obj->ClassID() == Class_ID(BONE_CLASS_ID, 0)) return TRUE;
	if(os.obj->ClassID() == BONE_OBJ_CLASSID) return TRUE;
	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0)) return FALSE;
	
	// CAT guids
	if(os.obj->ClassID() == Class_ID(0x56ae72e5, 0x389b6659)) return TRUE; //CatRig CATParentFPInterface
	if(os.obj->ClassID() == Class_ID(0x2e6a0c09, 0x43d5c9c0)) return TRUE; //Limb - CATObjectFPInterface
	if(os.obj->ClassID() == Class_ID(0x73dc4833, 0x65c93caa)) return TRUE; //HUB - CATObjectFPInterface
	if(os.obj->ClassID() == Class_ID(0x67a52f20, 0x2eac1b81)) return TRUE; //?

	// check for biped node
	Control *pControl;
	pControl = pNode->GetTMController();
	if((pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || (pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)) return TRUE;

	return FALSE;
}

//----------------------------------------------------------------------------//
// Check if the given node is a dummy                                         //
//----------------------------------------------------------------------------//
BOOL CMaxInterface::IsDummy(INode *pNode)
{
	// check for invalid nodes
	if(pNode == 0) return FALSE;

	// check for root node
	if(pNode->IsRootNode()) return FALSE;

	// check for dummy node
	ObjectState os;
	os = pNode->EvalWorldState(GetMaxTime());
	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0)) return TRUE;

	return FALSE;
}

//----------------------------------------------------------------------------//
// Check if the given node is a mesh                                          //
//----------------------------------------------------------------------------//
BOOL CMaxInterface::IsMesh(INode *pNode)
{
	// check for invalid nodes
	if(pNode == 0) return FALSE;

	// check for root node
	if(pNode->IsRootNode()) return FALSE;
	
	// we should skip dummy objects
	if(IsDummy(pNode)) return FALSE;

	// check for mesh node
	ObjectState os;
	os = pNode->EvalWorldState(GetMaxTime());
	Class_ID a = os.obj->ClassID();
	if((os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID) && 
	  ((os.obj->ClassID() == Class_ID(EDITTRIOBJ_CLASS_ID, 0)) ||
	   (os.obj->ClassID() == Class_ID(POLYOBJ_CLASS_ID, 0)) || 
	   (os.obj->ClassID() == EPOLYOBJ_CLASS_ID)
	  )) 
	  return TRUE;

	return FALSE;
}

Modifier *CMaxInterface::FindModifier(INode *pNode, Class_ID &ClassId)
{
  // get the object reference of the node
  Object *pObject;
  pObject = pNode->GetObjectRef();
  if(pObject == 0) return NULL;

  // loop through all derived objects
  while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
  {
    IDerivedObject *pDerivedObject = static_cast<IDerivedObject *>(pObject);;

    // loop through all modifiers
    for(int stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
    {
      // get the modifier
      Modifier *pMod;
      pMod = pDerivedObject->GetModifier(stackId);
      // check if we found the needed modifier
      if(pMod->ClassID() == ClassId) return pMod;
    }

    // continue with next derived object
    pObject = pDerivedObject->GetObjRef();
  }

  return NULL;
}

CMaxInterface::NodeType CMaxInterface::GetNodeType(INode *pNode)
{
  // find the corresponding type of the node
  if(IsBone(pNode)) 
    return TYPE_BONE;
  if(IsDummy(pNode)) 
    return TYPE_DUMMY;
	
  return TYPE_OTHER;
}


//----------------------------------------------------------------------------//
// Progress callback function                                                 //
//----------------------------------------------------------------------------//

static DWORD WINAPI _ProgressFunction(LPVOID arg)
{
  return 0;
}

//----------------------------------------------------------------------------//
// Set the progress information to a given text and percentage                //
//----------------------------------------------------------------------------//

void CMaxInterface::SetProgressInfo(int percentage)
{
  // minimize redrawing - redraw only percent changed
  static int lastPerc = 0;
  if(abs(lastPerc-percentage)>=2 || percentage>=100) {
    char buf[256];
    sprintf(buf, "%d%% %s", percentage, m_progressText.c_str());
    // by some absolutely fucked up reason we need to call update twice. otherwise progress bar is not updated!!!!
    _ip->ProgressUpdate(percentage + 1, FALSE, buf);
    _ip->ProgressUpdate(percentage, FALSE, buf);
    lastPerc = percentage;
  }
}

//----------------------------------------------------------------------------//
// Start the progress information process                                     //
//----------------------------------------------------------------------------//

void CMaxInterface::StartProgressInfo(const char *strText)
{
  //_ip->ProgressStart(const_cast<char *>(strText.c_str()), TRUE, _ProgressFunction, 0);

  static int bStarted = 0;
  if(!bStarted) {
    _ip->ProgressStart("Exporting Process:", TRUE, _ProgressFunction, 0);
    bStarted = 1;
  }
  m_progressText = strText;
  SetProgressInfo(0);
}

//----------------------------------------------------------------------------//
// Stop the progress information process                                      //
//----------------------------------------------------------------------------//

void CMaxInterface::StopProgressInfo()
{
  //_ip->ProgressEnd();
}



BOOL _MMRES_Present(INode *pNode)
{
  return gMaxHelper.FindModifier(pNode, MULTIRES_CLASS_ID) ? TRUE : FALSE;
}

BOOL _MMRES_Attach(INode *pNode)
{
  Modifier    *pMod;
  pMod = gMaxHelper.FindModifier(pNode, MULTIRES_CLASS_ID);
  if(pMod) 
    return TRUE;

  int havePhys = 0;
  /*
  //note: by some reason if we apply multires modifier before physique, physique will get screwed up
  // so, disable it for now, and apply at top of the stack.

  if(pMod = gMaxHelper.FindModifier(pNode, Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B)))
    havePhys = 1;
  */

  // create multires modifier and add it
  Object *pObject;
  pObject = pNode->GetObjectRef();

  //U_Log("!d-Adding MultiRes %s ", havePhys ? "to stack bottom (have Physique)" : "to stack top");

  IDerivedObject *pDerivedObject;
  if(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID) {
    pDerivedObject = static_cast<IDerivedObject *>(pObject);
  } else {
    pDerivedObject = CreateDerivedObject(pObject);
    pNode->SetObjectRef(pDerivedObject);
  }

  // add to depth of stack
  pMod = (Modifier *)CreateInstance(0x810, Class_ID(0x6a9e4c6b, 0x494744dd));
  pDerivedObject->AddModifier(pMod, NULL, havePhys ? pDerivedObject->NumModifiers() : 0);
  // evaluate it
  pNode->EvalWorldState(gMaxHelper.GetMaxTime());

  // press "Generate" button
  IParamBlock2 *pbl = pMod->GetParamBlockByID(0);
  if(!pbl) { 
    U_Log("!B!rNo IParamBlock2\n"); 
    return FALSE; 
  }
  pbl->SetValue(MULTIRES_PBID_GENERATE, gMaxHelper.GetMaxTime(), 0, NULL);
  pNode->EvalWorldState(gMaxHelper.GetMaxTime());

  // this is needed, don't know why. we need to set vertexcount to not 100%, 
  // otherwise mesh will be totally messed up with physique
  pbl->SetValue(MULTIRES_PBID_VERTEXPERCENT, gMaxHelper.GetMaxTime(), 100.0f);
  pNode->EvalWorldState(gMaxHelper.GetMaxTime());
  int      vtxcnt;
  Interval ival;
  pbl->GetValue(MULTIRES_PBID_VERTEXCOUNT,   gMaxHelper.GetMaxTime(), vtxcnt, ival);
  pbl->SetValue(MULTIRES_PBID_VERTEXCOUNT,   gMaxHelper.GetMaxTime(), vtxcnt - 2);
  pNode->EvalWorldState(gMaxHelper.GetMaxTime());

  // restore to full value.
  pbl->SetValue(MULTIRES_PBID_VERTEXPERCENT, gMaxHelper.GetMaxTime(), 100.0f);
  pNode->EvalWorldState(gMaxHelper.GetMaxTime());


  //U_Log(": ok\n");

  return TRUE;
}


BOOL _MMRES_Remove(INode *pNode)
{
  // same code as in CMaxInterface::FindModifier()
  Object *pObject;
  pObject = pNode->GetObjectRef();
  if(pObject == 0) return FALSE;

  while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID) {
    IDerivedObject *pDerivedObject = static_cast<IDerivedObject *>(pObject);;

    for(int stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++) {
      Modifier *pMod = pDerivedObject->GetModifier(stackId);;
      if(pMod->ClassID() == MULTIRES_CLASS_ID) {

        // U_Log("!d-Removing MultiRes\n");

	// ok, we found MultiRes - remove it..
        IParamBlock2 *pbl = pMod->GetParamBlockByID(0);
        if(!pbl) return FALSE;

        // this is needed, don't know why. we need to set vertexcount to not 100%, otherwise physique data will be totally messed after MultiRes delete
        pbl->SetValue(MULTIRES_PBID_VERTEXPERCENT, gMaxHelper.GetMaxTime(), 100.0f);
        pNode->EvalWorldState(gMaxHelper.GetMaxTime());
	int      vtxcnt;
	Interval ival;
	pbl->GetValue(MULTIRES_PBID_VERTEXCOUNT,   gMaxHelper.GetMaxTime(), vtxcnt, ival);
	pbl->SetValue(MULTIRES_PBID_VERTEXCOUNT,   gMaxHelper.GetMaxTime(), vtxcnt - 2);
        pNode->EvalWorldState(gMaxHelper.GetMaxTime());

        pDerivedObject->DeleteModifier(stackId);
        pNode->EvalWorldState(gMaxHelper.GetMaxTime());
	return TRUE;
      }
    }

    // continue with next derived object
    pObject = pDerivedObject->GetObjRef();
  }

  return FALSE;
}

BOOL _MMRES_Rebuild(INode *pNode, TimeValue time)
{
  Modifier    *pMod;
  pMod = gMaxHelper.FindModifier(pNode, MULTIRES_CLASS_ID);
  if(!pMod)
    return FALSE;

  IParamBlock2 *pbl = pMod->GetParamBlockByID(0);
  if(!pbl) { 
    U_Log("!B!rNo IParamBlock2\n"); 
    return FALSE; 
  }
  pbl->SetValue(MULTIRES_PBID_GENERATE, time, 0, NULL);
  pNode->EvalWorldState(time);

  return TRUE;
}

BOOL _MMRES_SetLOD(INode *pNode, float perc)
{
  Modifier    *pMod;
  pMod = gMaxHelper.FindModifier(pNode, MULTIRES_CLASS_ID);
  if(!pMod)
    return FALSE;

  IParamBlock2 *pbl = pMod->GetParamBlockByID(0);
  if(!pbl) { 
    U_Log("!B!rNo IParamBlock2\n"); 
    return FALSE; 
  }
  pbl->SetValue(MULTIRES_PBID_VERTEXPERCENT, gMaxHelper.GetMaxTime(), perc, NULL);
  pNode->EvalWorldState(gMaxHelper.GetMaxTime());

  return TRUE;
}
