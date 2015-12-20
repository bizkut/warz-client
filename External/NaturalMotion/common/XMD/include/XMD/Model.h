//----------------------------------------------------------------------------------------------------------------------
/// \file   Model.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  The main loader of all things in a model file
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Texture.h"
#include "XMD/Image.h"
#include "XMD/Base.h"
#include "XMD/Deformer.h"
#include "XMD/Info.h"
#include "XMD/Instance.h"
#include "XMD/Logger.h"
#include "XMD/AnimCycle.h"
#include "XMD/NewNode.h"
#include "XMD/InheritedNode.h"
#include "XMD/ClonePolicy.h"
#include "XMD/CreatePolicy.h"
#include "XMD/DeletePolicy.h"
#include "XMD/ExportPolicy.h"
#include "XMD/ImportPolicy.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
// forward declaration
class XMD_EXPORT XBasicLogger;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief  initialise the XMD library. This must be called before using
///         any other class or method within XMD. 
/// \return true if OK
//----------------------------------------------------------------------------------------------------------------------
bool XMD_EXPORT XMDInit(XBasicLogger* logger = 0);

//----------------------------------------------------------------------------------------------------------------------
/// \brief  cleanup the XMD library. This should be called to delete
///         any internal static data shared between XModel classes.
/// \return true if OK
//----------------------------------------------------------------------------------------------------------------------
bool XMD_EXPORT XMDCleanup();

//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class defines an enum that determines the status of
///         the fail codes for the loading or saving of a file.
//----------------------------------------------------------------------------------------------------------------------
class XFileError 
{
public:
  /// \brief  The error enum for FILE io
  enum eType {
    /// the FILEIO was successful
    Success,
    /// the file was of an old (un-supported) file type
    OldFormat,
    /// the file is of a new format
    NewFormat,
    /// the file could not be opened
    FailedToOpen,
    /// the file data is corrupt
    FileCorrupt,
    /// the file type is unknown
    InvalidFile,
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  The XModel class can be used to create, modify, load and save
///         XMD files. You must call XMD:XMDInit() before instantiating
///         this class, otherwise no type information of any nodes will 
///         be available to the XModel, which will usually result in the 
///         Loading of an XMD or creating a node to fail. 
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XModel
{
  // so that the geom can access the internal instance list
  friend class XGeometry;
  friend class XShape;
  friend class XInstance;
  friend class XBone;
  friend XBase XMD_EXPORT * XCreateNode(XModel* pmod,const XFn::Type type);
public:

  /// \brief  ctor
  XModel();

  /// \brief  dtor
  virtual ~XModel(void);

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   XModel Policy Manipulation
  /// \brief  functions to set and query the various policies that control
  ///         the way in which the XModel loads, saves, deletes and 
  ///         creates nodes
  //@{

  /// \brief  returns a reference to the internal delete policy used when 
  ///         duplicating nodes
  /// \return the clone policy reference
  XClonePolicy& GetClonePolicy();

  /// \brief  returns a reference to the internal clone policy used when 
  ///         duplicating nodes
  /// \return the clone policy reference
  const XClonePolicy& GetClonePolicy() const;

  /// \brief  returns a reference to the internal delete policy used when 
  ///         deleting nodes
  /// \return the delete policy reference
  XExportPolicy& GetExportPolicy();

  /// \brief  returns a reference to the internal delete policy used when 
  ///         deleting nodes
  /// \return the delete policy reference
  const XExportPolicy& GetExportPolicy() const;

  /// \brief  returns a reference to the internal delete policy used when 
  ///         deleting nodes
  /// \return the delete policy reference
  XImportPolicy& GetImportPolicy();

  /// \brief  returns a reference to the internal delete policy used when 
  ///         deleting nodes
  /// \return the delete policy reference
  const XImportPolicy& GetImportPolicy() const;

  /// \brief  returns a reference to the internal delete policy used when 
  ///         deleting nodes
  /// \return the delete policy reference
  XCreatePolicy& GetCreatePolicy();

  /// \brief  returns a reference to the internal delete policy used when 
  ///         deleting nodes
  /// \return the delete policy reference
  const XCreatePolicy& GetCreatePolicy() const;

  /// \brief  returns a reference to the internal delete policy used when 
  ///         deleting nodes
  /// \return the delete policy reference
  XDeletePolicy& GetDeletePolicy();

  /// \brief  returns a reference to the internal delete policy used when 
  ///         deleting nodes
  /// \return the delete policy reference
  const XDeletePolicy& GetDeletePolicy() const;
  //@}


  //--------------------------------------------------------------------------------------------------------------------
  /// \name   XModel Load/Save Functions
  /// \brief  functions to load and save data files
  //@{

  /// \brief  Loads the specified model file
  /// \param  filename    -  the name of the file to load
  /// \param  include_anim  -  if true, loads the animation data, 
  ///                otherwise false  just loads the model
  /// \return XFileError::Success if OK
  XFileError::eType Load(const XString& filename, bool include_anim = true);

  /// \brief  Loads the data from a stream.
  /// \param  ifs - the input stream
  /// \param  filename    -  the name of the file to load
  /// \param  include_anim  -  if true, loads the animation data, 
  ///                otherwise false  just loads the model
  /// \return XFileError::Success if OK
  XFileError::eType LoadFromAsciiStream(std::istream& ifs, const XString& filename = "", bool include_anim = true);

  /// \brief  Loads the data from a buffer.
  /// \param  buffer - the data block to read from
  /// \param  filename    -  the name of the file to load
  /// \param  include_anim  -  if true, loads the animation data, 
  ///                otherwise false  just loads the model
  /// \return XFileError::Success if OK
  XFileError::eType LoadFromAsciiBuffer(const XChar* buffer, const XString& filename = "", bool include_anim = true);

  /// \brief  Loads the animation data from the specified model file
  /// \param  filename    -  the name of the file to load
  /// \return XFileError::Success if OK
  XFileError::eType LoadAnim(const XString& filename);

  /// \brief  Loads the animation data from the specified model file
  /// \param  filename    -  the name of the file to load
  /// \return XFileError::Success if OK
  ///
  XFileError::eType LoadTextures(const XString& filename);

  /// \brief  Loads the animation data from the specified model file
  /// \param  file -  the file pointer to load data from
  /// \return XFileError::Success if OK
  XFileError::eType LoadTextures(FILE* file);

  /// \brief  Saves the specified model file
  /// \param  filename -  the name of the output XMD file
  /// \param  ascii - save in ascii (true) or binary (false)
  /// \param  include_anim -  if true, saves the animation data, 
  ///         otherwise false just saves the model
  /// \deprecated please use the other implementation of Save, the boolean
  ///         paremeters can now be set via XMD::XModel::GetExportPolicy().
  /// \return XFileError::Success if OK
  #ifndef DOXYGEN_PROCESSING
  #if _MSC_VER  >= 1400
  __declspec(deprecated("This Save function is deprecated. Please Use XFileError::eType Save(const XString& filename); instead")) 
  #endif
  #endif
  XFileError::eType Save(const XString& filename, bool ascii, bool include_anim = true);

  /// \brief  Loads the specified model file
  /// \param  filename -  the name of the output XMD file
  /// \return XFileError::Success if OK
  XFileError::eType Save(const XString& filename);

  /// \brief  Loads the specified model file
  /// \param  fp -  the binary stream to save to
  /// \return XFileError::Success if OK
  XFileError::eType SaveToBinaryStream(FILE* fp);

  /// \brief  Writes the XMD file to an ascii stream
  /// \param  os -  the stream to write to
  /// \return XFileError::Success if OK
  XFileError::eType SaveToAsciiStream(std::ostream& os);

  /// \brief  Loads the animation data from the specified model file
  /// \param  filename    -  the returned texture list
  /// \param  cycle_ids   - the ID's of the nima cycles to export
  /// \return XFileError::Success if OK
  XFileError::eType SaveAnims(const XString& filename, const XIndexList& cycle_ids);

  /// \brief  Saves the image data to the specified file
  /// \param  filename    -  the output texture file
  /// \return XFileError::Success if OK
  XFileError::eType SaveTextures(const XString& filename);

  /// \brief  Saves the image data to the specified file
  /// \param  file     -  the output texture file
  /// \return XFileError::Success if OK
  XFileError::eType SaveTextures(FILE* file);

  /// \brief  saves the file using a plugin filetranslator as registered
  ///         in the XGlobal class. 
  /// \param  filename    -  the name of the file to save
  /// \return true if OK
  XFileError::eType Export(const XString& filename);

  /// \brief  frees all currently loaded data
  virtual void Release();

  //@}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name   Node Query Functions
  /// \brief  Allows you to query for the existence of a particular
  ///         node, or to list a set of specific Node types. 
  //@{

  /// \brief  allows you to retrieve a specific node from the model
  /// \param  name  -  the name of the node
  /// \return a pointer to the base node
  XBase* FindNode(const XString& name);

  /// \brief  allows you to retrieve a specific node from the model
  /// \param  id_  -  the id of the node
  /// \return a pointer to the base node
  XBase* FindNode(XId id_);

  /// \brief  allows you to retrieve a specific node from the model
  /// \param  name  -  the name of the node
  /// \return a pointer to the base node
  const XBase* FindNode(const XString& name) const;

  /// \brief  allows you to retrieve a specific node from the model
  /// \param  id_  -  the id of the node
  /// \return a pointer to the base node
  const XBase* FindNode(XId id_) const;

  /// \brief  allows you to retrieve a specific node from the model
  /// \param  name  -  the name of the node
  /// \return a pointer to the base node
  XList FindNodes(const XString& name) const;

  /// \brief  used specifically when importing animations onto the rig. 
  ///         This allow you to attempt to find a bone via an approximate 
  ///         name rather than the full name. 
  /// \param  name  -  the name of the node
  /// \return a pointer to the returned bone
  XBone* GuessBone(const XString& name) const;

  /// \brief  returns a list of all bones in the model
  /// \param  _list  -  the returned list of bones
  /// \return true if any returned
  bool GetBones(XBoneList& _list);

  /// \brief  returns a list of all bones in the model
  /// \param  _list  -  the returned list of bones
  /// \return true if any returned
  bool GetBones(XBoneList& _list) const;

  /// \brief  returns a list of all deformers in the model
  /// \param  _list  -  the returned list of deformers
  /// \return true if any returned
  bool GetDeformers(XDeformerList& _list) const;

  /// \brief  returns a list of all deformers in the model
  /// \param  _list  -  the returned list of deformers
  /// \param  type  -  specifies a type to retrieve.
  /// \return true if any returned
  bool GetDeformers(XDeformerList& _list, XFn::Type type) const;

  /// \brief  returns the info structure from the model (if there is one)
  /// \return the information structure.
  const XInfo* GetInfo() const;

  /// \brief  returns the info structure from the model (if there is one)
  /// \return the information structure.
  XInfo* GetInfo();

  /// \brief  creates a new file info structure on the model
  /// \return the new information structure.
  XInfo* CreateInfo();

  /// \brief  returns the requested instance from it's ID
  /// \param  instance - the ID of the instance.Note: this is a different set of
  ///                    ID's from the node ID's!
  /// \return the instance pointer
  XInstance* GetInstance(XId instance);

  /// \brief  returns the requested instance from it's ID
  /// \param  instance - the ID of the instance.Note: this is a different set of
  ///                     ID's from the node ID's!
  /// \return the instance pointer
  const XInstance* GetInstance(XId instance) const;

  /// \brief  returns a list of all object instances
  /// \param  _list - the returned list of instances
  /// \return true if any found
  bool GetObjectInstances(XInstanceList& _list);

  /// \brief  returns a list of all object instances
  /// \param  _list  -  the returned list of object instances
  /// \param  type  -  specifies a type to retrieve.
  /// \return true if any found
  bool GetObjectInstances(XInstanceList& _list, XFn::Type type);

  /// \brief  returns a list of all root bones in the model
  /// \param  _list  -  the returned list of bones
  /// \return true if any returned
  bool GetRootBones(XBoneList& _list);

  /// \brief  returns a list of all root bones in the model
  /// \param  _list  -  the returned list of bones
  /// \return true if any returned
  bool GetRootBones(XBoneList& _list) const;

  /// \brief  returns a list of the used textures
  /// \param  _list  -  the returned texture list
  /// \return true if any found
  bool GetUsedTextures(XTextureList& _list);

  /// \brief  returns a list of the materials used by the model
  /// \param  _list  -  th returned list of materials
  /// \return true if any found
  bool GetUsedMaterials(XMaterialList& _list);

  /// \brief  Gets a list of all nodes compatible with the specified
  ///         function set type.
  /// \param  items -  a list of all the items returned
  /// \param  type -  the object type to list
  /// \return true if objects found
  bool List(XList& items, XFn::Type type, bool exactType = false);

  /// \brief  Gets a list of all nodes compatible with the specified
  ///         function set type.
  /// \param  items -  a list of all the items returned
  /// \param  must_have_attrs  -  a list of attribute names that the node must have
  /// \return true if objects found
  bool List(XList& items, const XStringList& must_have_attrs);

  /// \brief  Gets a list of all nodes compatible with the specified
  ///         function set type. Also ensures that the nodes returned
  ///         all have the requested attributes
  /// \param  items -  a list of all the items returned
  /// \param  type -  the object type to list
  /// \param  must_have_attrs  -  a list of attribute names that the node must have
  /// \return true if objects found
  bool List(XList& items, XFn::Type type, const XStringList& must_have_attrs, bool exactType = false);

  /// \brief  Gets a list of all nodes compatible with the specified
  ///         function set type.
  /// \param  items  -  a list of all the items returned
  /// \param  type  -  the object type to list
  /// \return true if objects found
  bool List(XList& items, XFn::Type type, bool exactType=false) const;

  /// \brief  Gets a list of all nodes compatible with the specified
  ///         function set type.
  /// \param  items      -  a list of all the items returned
  /// \param  must_have_attrs  -  a list of attribute names that the node must have
  /// \return true if objects found
  bool List(XList& items, const XStringList& must_have_attrs) const;

  /// \brief  Gets a list of all nodes compatible with the specified
  ///         function set type. Also ensures that the nodes returned
  ///         all have the requested attributes
  /// \param  items      -  a list of all the items returned
  /// \param  type      -  the object type to list
  /// \param  must_have_attrs  -  a list of attribute names that the node must have
  /// \return true if objects found
  bool List(XList& items, XFn::Type type, const XStringList& must_have_attrs, bool exactType = false) const;

  //@}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name   Transform/Bind Pose Manipulation
  //@{

  /// \brief  removes all joint and rotation orients from the skeletal 
  ///         hierarchy, and does the same for all animation data 
  ///         provided. This is a lossy process - whilst the animation
  ///         should remain the same, you will not be able to undo this
  ///         action - the data will be lost
  bool FreezeAllTransforms();

  /// \brief  This function updates the current world and local space 
  ///         matrices within the model. This is useful if you have 
  ///         changed some transform values and now want to get that info
  ///         in matrix form. 
  void BuildMatrices();

  /// \brief  This places the model in it's bind pose
  void SetToBindPose();

  //@}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name   Node Validity Checks
  /// \brief  Tests for the validity of a given name, node pointer 
  ///         or ID.
  //@{

  /// \brief  A quick query function to determine if the specified
  ///         node name is actually valid
  /// \param  name  -  the name of a node
  /// \return true if a valid node
  bool IsValidNode(const XString& name) const;

  /// \brief  A quick query function to determine if the specified
  ///         node ID is actually valid
  /// \param  node  -  the id of a node
  /// \return true if a valid node
  bool IsValidNode(XId node) const;

  /// \brief  A quick query function to determine if the specified
  ///         node ID is actually valid
  /// \param  node  -  the id of a node
  /// \return true if a valid node
  bool IsValidNode(const XBase* node) const;

  /// \brief  checks to see if the node name specified is valid
  /// \param  name  - the name of the node to check
  /// \return true if the node exists
  bool IsValidObj(const XString& name) const 
  {
    return IsValidNode(name);
  }

  /// \brief  checks to see if the node name specified is valid
  /// \param  id_num  - the unique id of the node to check
  /// \return true if the node exists
  ///
  bool IsValidObj(XId id_num) const 
  {
    return IsValidNode(id_num);
  }

  /// \brief  checks to see if the node name specified is valid
  /// \param  node  - a pointer to the node to check
  /// \return true if the node exists
  bool IsValidObj(const XBase* node) const 
  {
    return IsValidNode(node);
  }

  //@}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name   Animation functions
  /// \brief  Functions to evaluate and query animation cycle data
  //@{

  /// \brief  returns the number of animation cycles
  /// \return the num anim cycles
  XU32 NumAnimCycles() const;

  /// \brief  returns a pointer to the requested animation cycle
  /// \param  idx  -  the animation cycle index
  /// \return a pointer to the cycle
  XAnimCycle* GetAnimCycle(XU32 idx);

  /// \brief  returns a pointer to the requested animation cycle
  /// \param  idx  -  the animation cycle index
  /// \return a pointer to the cycle
  const XAnimCycle* GetAnimCycle(XU32 idx) const;

  /// \brief  evaluates the hierarchy at the specified time along the
  ///         specified animation cycle.
  /// \param  anim  -  the animation cycle to use
  /// \param  sec    -  the animation time
  void EvaluateAt(XBase* anim, XReal sec);

  //@}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name   Node naming functions
  /// \brief  functions to rename nodes within an XModel
  //@{

  /// \brief  A function to rename the specified node. If this name is
  ///         a type or has already been used, then the function will
  ///         try a variety of alternatives to see if it can find a valid
  ///         name. The name returned should therefore be checked.
  /// \param  oldname  -  the old name of a node
  /// \param  newname  -  the new name of a node
  /// \return the actual name assigned.
  XString RenameNode(const XString oldname, const XString newname);

  /// \brief  A function to rename the specified node. If this name is
  ///         a type or has already been used, then the function will
  ///         try a variety of alternatives to see if it can find a valid
  ///         name. The name returned should therefore be checked.
  /// \param  node  -  the node ptr
  /// \param  newname  -  the new name of a node
  /// \return the actual name assigned.
  XString RenameNode(XBase* node, const XString newname);

  /// \brief  A function to rename the specified node. If this name is
  ///         a type or has already been used, then the function will
  ///         try a variety of alternatives to see if it can find a valid
  ///         name. The name returned should therefore be checked.
  /// \param  id    -  the node id
  /// \param  newname  -  the new name of a node
  /// \return the actual name assigned.
  XString RenameNode(XId id, const XString newname);

  /// \brief  Removes quotations, slashes and replaces spaces with underscores,
  ///         this makes the name safe to use in file names.
  static void FormatName(XString& name);

  //@}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name   Node Creation/Deletion functions
  /// \brief  functions to Create and Delete various node types
  //@{

  /// \brief  allows you to create a new instance of the specified
  ///         geometry under the specified bone
  /// \param  bone  -  the bone to use as transform
  /// \param  object  - the object to parent underneath it
  /// \return a pointer to the base node
  XInstance* CreateGeometryInstance(XBone* bone, XShape* object);

  /// \brief  Deletes the specified node safely from the model
  /// \param  obj  -  the node to delete
  /// \return true if OK, false if cannot be deleted
  bool DeleteNode(XBase* obj);

  /// \brief  Deletes the specified node safely from the model
  /// \param  obj  -  the node id to delete
  /// \return true if OK, false if cannot be deleted
  bool DeleteNode(XId obj);

  /// \brief  Deletes the specified instance safely from the model
  /// \param  instance  -  the id of the instance to delete
  /// \return true if OK, false if cannot be deleted
  bool DeleteInstance(XId instance);

  /// \brief  Deletes all of the node of a specified type from the model
  /// \param  type  -  the node type to delete
  /// \return true if OK
  bool DeleteAllOfType(XFn::Type type);

  /// \brief  This function lets you create a new node of the specific 
  ///         type. I'm testing this for file import to XMD at the moment.
  /// \param  type  -  the type of node to create
  /// \return pointer to the new node
  XBase* CreateNode(XFn::Type type);

  //@}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name   Image Data functions
  /// \brief  Functions to query any images loaded with this model
  //@{

  /// \brief  returns the requested image data for the specified index
  /// \param  index  -  the image index to query
  /// \return pointer to the image data
  XImage* GetImage(XId index);

  /// \brief  returns the number of images currently loaded into the Model.
  /// \return the number of images loaded
  XU32 GetNumImages();

  //@}

#ifndef DOXYGEN_PROCESSING
  /// \brief  this function sets up any extra internal references required
  ///         before the data gets exported to disk.
  void PostLoad();
  XU32 GetInputVersion() const;
protected:
  /// \brief  This function re-maps all indices
  /// \return true if OK
  bool ReassignIDs();
  void BuildInternalArrays();
  /// used to determine which version of the file format we are currently 
  /// reading
  XU32 m_InputVersion;
  /// pointer to the file info
  XInfo* m_pFileInfo;
  /// all of the images loaded in the scene
  XImageList m_Images;
  /// all of the instances
  XInstanceList m_Instances;
  /// the animation cycles stored for this model.
  XAnimList m_AnimCycles;
  typedef std::multimap<XString,XBase*> XMap;
  typedef std::map<XId,XBase*> XMap2;
  /// an internal map of all nodes within the model
  XMap m_Nodes;
  XMap2 m_NodeLookup;
  /// used to generate ID number for the object instances
  XU32 m_InstanceIDGen;
  /// a list of entirely new nodes within the scene.
  XInheritedNodeList m_Inherited;
  /// a list of specs for new nodes. If no gubbins have been registered
  /// a default will be attempted. It is very important that you register
  XNewNodeInfoList m_NewNodes;
  XU32 m_IdGen;
  /// the internal creation policy
  XClonePolicy  m_ClonePolicy;
  /// the internal creation policy
  XCreatePolicy m_CreatePolicy;
  /// the internal delete policy
  XDeletePolicy m_DeletePolicy;
  /// the internal export policy
  XExportPolicy m_ExportPolicy;
  /// the internal import policy
  XImportPolicy m_ImportPolicy;

private:
  void DoFinalDeleteNode(XBase* obj); 
  XFileError::eType LoadAscii(const XString& filename, bool include_anim = true);
  XFileError::eType LoadBin(const XString& filename, bool include_anim = true);
  XFileError::eType LoadBinAnim(const XString& filename);
  bool DoDeleteNode(XBase* ptr, XIdSet& set_);
#endif
};
}
