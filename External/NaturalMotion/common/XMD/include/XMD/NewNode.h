//----------------------------------------------------------------------------------------------------------------------
/// \file   NewNode.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a set of proxy classes to handle various aspects of
///         adding in your own custom types. To add your own custom types,
///         inherit from one of the following base classes :
///          
///              XMD :: PxAnimNode
///              XMD :: PxConstraintNode
///              XMD :: PxDeformerNode
///              XMD :: PxEmitterNode
///              XMD :: PxFieldNode
///              XMD :: PxLightNode
///              XMD :: PxMaterialNode
///              XMD :: PxNode
///              XMD :: PxShapeNode
///              XMD :: PxTransformNode
/// \note  (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include <map>
#include "XMD/Base.h"
#include "XMD/Material.h"
#include "XMD/Geometry.h"
#include "XMD/Deformer.h"
#include "XMD/Bone.h"
#include "XMD/Light.h"
#include "XMD/Constraint.h"
#include "XMD/Emitter.h"
#include "XMD/Field.h"
#include "XMD/HwShaderNode.h"
#include "XMD/ExtraAttributes.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// proxy types that provide the base types for all additional nodes
/// added into the supported types.
//----------------------------------------------------------------------------------------------------------------------
enum PxType 
{
  /// generic dependency node type
  PX_NONE,

  /// a generic material type
  PX_MATERIAL,

  /// an object that can be parented
  PX_OBJECT,

  /// a light
  PX_LIGHT,

  /// a transform node
  PX_TRANSFORM,

  /// a particle emitter
  PX_EMITTER,

  /// a dynamics field
  PX_FIELD,

  /// a constraint node
  PX_CONSTRAINT,

  /// a deformer type
  PX_DEFORMER,

  /// an animation node
  PX_ANIM,

  /// a deformer type
  PX_HW_SHADER,

  /// a custom geometry type
  PX_GEOMETRY,

  PX_LAST
};

// forward declaration
// 
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT PxAnimNode;
class XMD_EXPORT PxConstraintNode;
class XMD_EXPORT PxDeformerNode;
class XMD_EXPORT PxEmitterNode;
class XMD_EXPORT PxFieldNode;
class XMD_EXPORT PxLightNode;
class XMD_EXPORT PxMaterialNode;
class XMD_EXPORT PxHwShaderNode;
class XMD_EXPORT PxNode;
class XMD_EXPORT PxShapeNode;
class XMD_EXPORT PxTransformNode;
class XMD_EXPORT PxNewNodeDescription;
#endif


/// a function type that is used to register a new type with the FTK
/// 
typedef XBase* (*PxCreator)(XModel*,PxNewNodeDescription*);

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNewNodeDescription
/// \brief This class holds a brief bit of info on a custom plugin
///        type added into the FTK. It allows the lib to be able 
///        to correctly interpret the various un-recognised chunk
///        headers that may be present in the file.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxNewNodeDescription
{
public:

  /// ctor
  PxNewNodeDescription();

  /// \brief  ctor
  /// \param  header  - the file header, eg "MESH"
  /// \param  base    - the base derived type
  /// \param  _create - a creator function
  /// \param  type_id - the type id for this node type
  /// \param  type    - the maya type name, eg "mesh"
  PxNewNodeDescription(const XChar* header, PxType base,PxCreator _create, XU32 type_id, const XChar* type = 0);

  /// dtor
  ~PxNewNodeDescription();

  /// \brief  read the description of the new node
  /// \param  ifs  -  the input file stream
  /// \return true if OK
  bool Read(std::istream& ifs);

  /// \brief  write the description of the new node
  /// \param  ofs  -  the output file stream
  /// \return true if OK
  bool Write(std::ostream& ofs);

  /// \brief  This function returns the Chunk Header string for
  ///         this type
  /// \return The chunk header
  const XChar* GetChunkName() const;

  /// \brief  This function returns the Type name of the new node
  const XChar* GetTypeName() const;

  /// \brief  This returns a constant to refer to the base proxy
  ///         type from which this node type was derived
  /// \return The type constant for the base
  const PxType GetBaseType() const;

  /// \brief  This function creates a node of this type. If no
  ///         creator function was specified when the node was
  ///         registered, then a default one will be used. This
  ///         means that any new nodes that have not yet been
  ///         properly specced in the XMD api, will still be loaded
  ///         but the new data will be stored as a text string.
  /// \param  pMod  -  the model the node is part of
  /// \return A pointer to the new node
  ///
  XBase* Create(XModel* pMod);

  /// \brief  This function writes the data for the chunk. The header,
  ///         name, extra attributes and surrounding braces will have
  ///         been written already.
  /// \param  io  -  the file stream
  /// \return true if OK
  bool DoData(XFileIO& io);

  /// \brief  This function reads/writes the header for the inherited node info.
  /// \param  io  -  the binary file object
  /// \return true if OK
  bool DoHeader(XFileIO& io);

  /// \brief  This function returns the size in bytes needed for this chunk
  /// \return the total size in bytes of this chunk
  XU32 GetDataSize() const;

private:

  /// the base type for the node
  PxType m_BaseType;

  /// the base type for the node
  XU32 m_TypeID;

  /// the name of this node type
  XString m_TypeName;

  /// the header identifier for this type
  XString m_ChunkHeader;

  /// a creator function for that node type
  PxCreator m_Creator;
};

/// \brief  a lookup to map between the CHUNK headers expected in 
///         the ASCII file, and the description of what that chunk 
///         data contains. 
typedef std::map<XString,PxNewNodeDescription*> XNewNodeInfoList;

//----------------------------------------------------------------------------------------------------------------------
///  \brief  This class forms the base class of all proxy classes 
///      added into the FTK. This basically means that all custom
///      plugin nodes for the File ToolKit need to have this 
///      template as the base class. 
//----------------------------------------------------------------------------------------------------------------------
template
<
  typename TBase
>
class PxNewNode
  : public TBase
{
public:

  /// dtor
  ~PxNewNode() {}

  /// \brief  returns the type info for the new node type
  /// \return the new node type info
  const PxNewNodeDescription* GetTypeInfo() const 
  {
    return m_pType;
  }

  /// \brief  return a pointer to this level in the inheritance hierarchy
  virtual XBase* GetFn(XFn::Type type)
  {
    return TBase::GetFn(type);
  }

  /// \brief  return a pointer to this level in the inheritance hierarchy
  virtual const XBase* GetFn(XFn::Type type) const
  {
    return TBase::GetFn(type);
  }

protected:

  /// ctor
  PxNewNode(XModel* pmod,PxNewNodeDescription* desc) : TBase(pmod)
  {
    m_pType = desc;
  }

  /// \brief  This function is used to notify an object about the recent
  ///         death of another node. This allows it to check for any
  ///         broken dependencies and set the offending dependency to NULL.
  /// \param  id  -  the id of the transform
  /// \return true if OK
  virtual bool NodeDeath(XId id)
  {
    return TBase::NodeDeath(id);
  }

  /// \brief  called before node gets deleted, allows it to check the
  ///         models delete policy to see if anything needs to be deleted.
  /// \param  extra_nodes - insert a node into this list to safely delete it.
  virtual void PreDelete(XIdSet& extra_nodes)
  {
    return TBase::PreDelete(extra_nodes);
  }

  /// \brief  Overload this function in your plugin classes to write 
  ///         any custom data to the file for this type.
  /// \param  ofs  -  the output file stream
  /// \return true if OK
  virtual bool WriteCustom(std::ostream& ofs)
  {
    if(m_Data.size()) 
    {
      ofs << m_Data.c_str();
      if (m_Data[m_Data.size()-1] != '\n') {
        ofs << "\n";
      }
    }
    return ofs.good();
  }

  /// \brief  Overload this function in your plugin classes to read
  ///         any custom data that was written into the file for
  ///         this type.
  /// \param  ifs  -  the input file stream
  /// \return true if OK
  virtual bool ReadCustom(std::istream& ifs)
  {
    return ifs.good();
  }

private:
#ifndef DOXYGEN_PROCESSING

  /// \brief  This function writes the data for the chunk. The header,
  ///         name, extra attributes and surrounding braces will have
  ///         been written already.
  /// \param  ofs  -  the output file stream
  /// \return true if OK
  virtual bool WriteChunk(std::ostream& ofs)
  {
    if(!TBase::WriteChunk(ofs)) {
      return false;
    }

    if (!WriteCustom(ofs)) {
      return false;
    }

    return ofs.good();
  }

  /// \brief  This function reads the data for the chunk. The header,
  ///         name, extra attributes and surrounding braces get read for you.
  /// \param  ifs  -  the input file stream
  /// \return true if OK
  virtual bool ReadChunk(std::istream& ifs)
  {
    if(!TBase::ReadChunk(ifs)) {
      return false;
    }

    if (!ReadCustom(ifs)) 
    {
      XString buffer;
      XString data;
      XS32 bracket_count=1;
      bool done=false;
      while (!done)
      {
        XChar c = (XChar)ifs.get();
        
        switch(c) {

        case '}':
          --bracket_count;
          if(bracket_count) {
            data += c;
          }
          else
            done=true;
          break;

        case '{':
          ++bracket_count;
          data += c;
          break;

        default:
          data += c;
          break;
        }

        // value used for test to see if we've read in the EXTRA_ATTRIBUTES
        // flag by mistake. 
        XS32 dd = static_cast<XU32>(data.size()-strlen("EXTRA_ATTRIBUTES"));

        if (dd>0) 
        {
          const XChar* test_str = data.c_str() + dd;
          if (strcmp(test_str,"EXTRA_ATTRIBUTES")==0) {
            
            // remove "EXTRA_ATTRIBUTES" from the end of the data
            data.resize( dd );

            // done reading stuff
            done = true;

            // read the extra attributes
            TBase::m_pAttributes = new XExtraAttributes;
            ifs >> *TBase::m_pAttributes;
          }
        }
      }
      return ifs.good();

    }

    return ifs.good();
  }

  /// \brief  returns the name of the object
  /// \return the object name
  virtual const XChar* GetChunkName() const
  {
    if (m_pType) {
      return m_pType->GetChunkName();
    }
    return 0;
  }
    
  /// a pointer to the type description of this custom node
  PxNewNodeDescription* m_pType;

  /// the data read from the file (if no WriteCustom or ReadCustom/
  /// methods exist for this type)
  XString m_Data;
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief This class should form the base class of any new random
///        nodes you add into maya. Simply inherit from
///        this node, and provide implementations of WriteCustom()
///        and ReadCustom(). Use this only when no other PxClass
///        seems best suited to the task. This is generally quite
///        a minimal node...
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxNode 
  : public PxNewNode<XBase> {
  friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  PxNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief This class should form the base class of any new material
///        nodes you add into maya. Simply inherit from
///        this node, and provide implementations of WriteCustom()
///        and ReadCustom()
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxMaterialNode 
  : public PxNewNode<XMaterial> {
  friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxMaterialNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief This class should form the base class of any new material
///        nodes you add into maya. Simply inherit from
///        this node, and provide implementations of WriteCustom()
///        and ReadCustom()
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxHwShaderNode
  : public PxNewNode<XHwShaderNode> {
  friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxHwShaderNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief This class should form the base class of any new shape
///        nodes you add into maya. Simply inherit from
///        this node, and provide implementations of WriteCustom()
///        and ReadCustom()
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxShapeNode 
  : public PxNewNode<XShape> {
  friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxShapeNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief This class should form the base class of any new shape
///        nodes you add into maya. Simply inherit from
///        this node, and provide implementations of WriteCustom()
///        and ReadCustom()
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxGeometryNode 
  : public PxNewNode<XGeometry> {
  friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxGeometryNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief This class should form the base class of any new light
///        nodes you add into maya. Simply inherit from
///        this node, and provide implementations of WriteCustom()
///        and ReadCustom()
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxLightNode 
  : public PxNewNode<XLight> {
  friend class XMD_EXPORT PxNewNodeDescription;
public:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxLightNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief This class should form the base class of any new transform
///        nodes you add into maya. Simply inherit from
///        this node, and provide implementations of WriteCustom()
///        and ReadCustom()
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxTransformNode
  :  public PxNewNode<XBone> {
    friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxTransformNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief This class should form the base class of any new emitter
///        node you add into maya. Simply inherit from
///        this node, and provide implementations of WriteCustom()
///        and ReadCustom()
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxEmitterNode
  : public PxNewNode<XEmitter> {
    friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxEmitterNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief This class should form the base class of any Field nodes
///        you add into the model file format. Simply inherit from
///        this node, and provide implementations of WriteCustom()
///        and ReadCustom()
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxFieldNode
  : public PxNewNode<XField> {
    friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxFieldNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
///  \brief
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxConstraintNode 
  : public PxNewNode<XConstraint> {
    friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxConstraintNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief This class should form the base class of any custom
///        deformers you wish to add into the file format.Simply
///        inherit from this node, and provide implementations
///        of WriteCustom() and ReadCustom()
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxDeformerNode 
  : public PxNewNode<XDeformer> {
    friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxDeformerNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::PxNode
/// \brief Allows you to create your own custom Animation Nodes
///        that you define via script within the Maya Export
///        options.Simply inherit from this node, and provide
///        implementations of WriteCustom() and ReadCustom()
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT PxAnimNode
  : public PxNewNode<XBase> {
    friend class XMD_EXPORT PxNewNodeDescription;
protected:
#ifndef DOXYGEN_PROCESSING
  XFn::Type GetApiType() const;
  PxAnimNode(XModel* pmod,PxNewNodeDescription* desc);
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;
  virtual void DoCopy(const XBase* rhs){XBase::DoCopy(rhs);}
#endif
};

#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::PxNode, PxNode, "pxNode");
DECLARE_API_TYPE(XFn::PxMaterial, PxMaterialNode, "pxMaterial");
DECLARE_API_TYPE(XFn::PxHwShader, PxHwShaderNode, "pxHwShader");
DECLARE_API_TYPE(XFn::PxShape, PxShapeNode, "pxShape");
DECLARE_API_TYPE(XFn::PxGeometry, PxGeometryNode, "pxGeometry");
DECLARE_API_TYPE(XFn::PxLight, PxLightNode, "pxLight");
DECLARE_API_TYPE(XFn::PxTransform,PxTransformNode, "pxTransform");
DECLARE_API_TYPE(XFn::PxEmitter, PxEmitterNode, "pxEmitter");
DECLARE_API_TYPE(XFn::PxField, PxFieldNode, "pxField");
DECLARE_API_TYPE(XFn::PxConstraint, PxConstraintNode, "pxConstraint");
DECLARE_API_TYPE(XFn::PxDeformer, PxDeformerNode, "pxDeformer");
DECLARE_API_TYPE(XFn::PxAnim, PxAnimNode, "pxAnim");
#endif
}
