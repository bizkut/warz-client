//----------------------------------------------------------------------------------------------------------------------
/// \file   NodeTypeRegister.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the core static type registration classes for XMD. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
#include "XMD/Logger.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
#ifndef DOXYGEN_PROCESSING
// forward declaration
class XMD_EXPORT XBase;
class XMD_EXPORT XModel;
class XMD_EXPORT XFileTranslator;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::IXCreateBase 
/// \brief This is an interface used to register the various chunk types
///        with XMD. There is no need to use this class directly, the 
///        DECLARE_API_TYPE and DECLARE_CHUNK_TYPE macro's handle this for you. 
//---------------------------------------------------------------------------------------------------------------------- 
struct XMD_EXPORT IXCreateBase 
{
  /// \brief  override to return the type of the node you are registering
  /// \return an XMD type enum
  virtual XFn::Type GetType() const=0;
  /// \brief  override to return the type string of the node, i.e. "mesh"
  /// \return the data type as a string, i.e. "mesh"
  virtual XString GetTypeString() const=0;
  /// \brief  override to return the file identifier of the node, i.e. "MESH"
  /// \return the file identifier as a string, i.e. "MESH"
  virtual XString GetChunkName() const=0;
  /// \brief  override to return a new instance of the node
  /// \param  model - the model into which the node will be bound
  /// \return a new node instance of the type you are registering
  virtual XBase* CreateNode(XModel* model) const
  {
    (void)model;
    return 0; 
  }
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XFileTranslatorRegister 
/// \brief holds information about a file translator registered with XMD. 
//---------------------------------------------------------------------------------------------------------------------- 
struct XMD_EXPORT XFileTranslatorRegister 
{
  /// the file translator object
  XFileTranslator* m_Translator;

  /// the file extension
  XString m_Extension;

  /// \brief  ctor
  ~XFileTranslatorRegister();

  /// \brief  dtor
  XFileTranslatorRegister();
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XGlobal 
/// \brief This is a global class that contains all of the static registration
///        info about the Types after they have been registered with XMD. 
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XGlobal 
{
public:

  /// \brief  Gets a pointer to the currently used logger 
  /// \return the current logger
  static XBasicLogger* GetLogger();

  /// \brief  Sets the logger to a new type
  /// \param  pLogger - the new logger
  static void SetLogger(XBasicLogger* pLogger);

  /// \brief  sets the version identifier for the output file. This is a 
  ///         4 character string that allows you to identify where the file 
  ///         came from, ie "MAYA".
  /// \param  version - a 4 character product identifier.
  static void SetVersionIdentifier(const XChar* version);

  /// \brief  This is an 8 character identifier that allows you to insert
  ///         a version into the file. For example, you may have the Maya 
  ///         export plugin version "000001.1". This is mainly used for 
  ///         customer support to track which plug-in version was used 
  ///         to output a given file. 
  /// \param  version - an 8 character version number or identifier.
  static void SetVersion(const XChar* version);

  /// \brief  This is used to register a specific node type with XMD. This
  ///         is currently all done within XMDInit(), so there should be no
  ///         real need to call this. 
  /// \param  type  - the type info used to generate the new node type
  /// \return true if OK
  static bool RegisterTypeInfo(IXCreateBase* type);

  /// \brief  This function allows you to register your own file translator 
  ///         class to convert a file format into XMD and vice versa 
  ///         depending on the registered extension.
  /// \param  translator  - the new file translator class
  /// \param  ext - the extension this translator supports
  /// \return true if OK
  static bool RegisterTranslator(XFileTranslator* translator, const XChar* ext);

  /// \brief  Returns the node type of the specified chunk name
  /// \param  chunk_name - the name of the chunk, eg "MESH"
  /// \return the XMD node Type
  static XFn::Type NodeTypeFromChunkName(const XChar* chunk_name);

  /// \brief  Returns the chunk name of the specified node type
  /// \param  type - the typeID of the node
  /// \return the XMD chunk name, eg "MESH"
  static XString ChunkNameFromNodeType(XFn::Type type);

  /// \brief  Returns the node type of the specified api type name
  /// \param  api_name - the name of the chunk, eg "mesh"
  /// \return the XMD node Type
  static XFn::Type NodeTypeFromApiTypeStr(const XChar* api_name);

  /// \brief  Returns the chunk name of the specified node type
  /// \param  type - the typeID of the node
  /// \return the XMD api name, eg "mesh"
  static XString ApiTypeStrFromNodeType(XFn::Type type);

private:

  friend class XMD_EXPORT XModel;
  friend class XMD_EXPORT XBase;
  friend class XMD_EXPORT XFactory;
  friend bool XMD_EXPORT XMDInit(XBasicLogger*);
  friend bool XMD_EXPORT XMDCleanup();
  friend XFn::Type XMD_EXPORT HeaderToType(const XString&);
  friend XString XMD_EXPORT TypeToHeader(XFn::Type);
  friend XBase XMD_EXPORT * XCreateNode(XModel*, XFn::Type);

  /// \brief  this class contains the internal data for XMD node type info. 
  ///         A single instance is created after calling XMDInit(), and it is
  ///         again freed after XMDCleanup() is called. 
  class InternalData 
  {
  private:
    /// internal array of all registered types
    XM2::pod_vector<IXCreateBase*> m_RegisteredTypes;

    /// internal array of registered file translators
    XM2::pod_vector<XFileTranslatorRegister*> m_RegisteredTranslators;
  public:
    InternalData();
    ~InternalData();
    IXCreateBase* FindType(XFn::Type);
    IXCreateBase* FindTypeFromApiStr(XString);
    IXCreateBase* FindTypeFromChunkName(XString);
    XFileTranslator* GetFileTranslatorForExt(const XChar*);
    void RegisterType(IXCreateBase*);
    void RegisterTranslator(XFileTranslatorRegister*);
    XString version;
    XString ident;
  };

  static XBasicLogger* gLogger;
  static XStdOutLogger gDefaultLogger;

  /// the singleton of the registered XMD data types
  static InternalData* gInternal;

  /// \brief  generates the XMD file header that appears at the start
  ///         of the XMD file
  static XString GetXmdHeader();

  static void Init(XBasicLogger* logger);
  static void Cleanup();

  static bool Import(const XChar*, XModel&);
  static bool ImportAnim(const XChar*, XModel&);
  static bool Export(const XChar*, XModel&);

  static XBase* CreateFromId(XFn::Type, XModel*);
  static XBase* CreateFromChunkName(const XChar*, XModel*);
  static XBase* CreateFromApiStr(const XChar*, XModel*); 

};
}
