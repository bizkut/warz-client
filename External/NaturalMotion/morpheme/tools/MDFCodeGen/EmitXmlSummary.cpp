// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------

#include "Precompiled.h"
#include "Utils.h"
#include "NMTinyXML.h"

//----------------------------------------------------------------------------------------------------------------------
/**
 * Function strips off every occurrence of a specified symbol from the beginning of a string.
 */
static void stripEverySymbolFromFront(std::string* string, const std::string& symbol)
{
  NMP_ASSERT(string);
  std::string::size_type brkLocation = std::string::npos;

  do
  {
    brkLocation = string->find(symbol);
    if (brkLocation == 0)
    {
      string->replace(brkLocation, symbol.size(), "");
    }
  }
  while (brkLocation == 0);
}

//----------------------------------------------------------------------------------------------------------------------
/**
 * Function strips off firstly found occurrence of a specified symbol from a string.
 */
static void stripSymbol(std::string* string, const std::string& symbol)
{
  NMP_ASSERT(string);
  std::string::size_type brkLocation = std::string::npos;

  brkLocation = string->rfind(symbol);
  if (brkLocation != std::string::npos)
  {
    string->replace(brkLocation, symbol.size(), "");
  }
}

//----------------------------------------------------------------------------------------------------------------------
/**
 * Function strips off any comment tags, trims extra whitespaces from
 * the beginning of a comment, transforms a multi-line comment into a single liner.
 */
static void GetCommentForTiXml(const std::string& comment, std::string* xmlComment)
{
  NMP_ASSERT(xmlComment);
  xmlComment->clear();

  const std::string endOfLine("\n");
  const std::string lessThan("<");
  const std::string slash("/");
  const std::string whitespace(" ");

  std::string commentLocalCpy(comment);
  const std::string::size_type commentSize = commentLocalCpy.size();
  std::string::size_type currentSize = 0;
  bool singleLineComment = true;

  std::string::size_type breaklocation = std::string::npos;

  do
  {
    // Erase < symbol from the comment.
    stripSymbol(&commentLocalCpy, lessThan);

    breaklocation = commentLocalCpy.find(endOfLine);
    if (breaklocation != std::string::npos)
    {
      singleLineComment = false;

      std::string commentLine(commentLocalCpy.substr(0, breaklocation + endOfLine.size()));
      currentSize += commentLine.size();

      // Strip all slashes from the beginning of the comment line.
      stripEverySymbolFromFront(&commentLine, slash);

      // Strip all whitespaces from the beginning of the comment line.
      stripEverySymbolFromFront(&commentLine, whitespace);

      // Strip line ending.
      stripSymbol(&commentLine, endOfLine);

      if (currentSize == commentSize) // Is it the last comment line?
      {
        breaklocation = std::string::npos; // Exit the loop just after appending last comment line.
      }
      else
      {
        commentLine.append(whitespace);
        commentLocalCpy.erase(0, breaklocation + endOfLine.size());
      }

      xmlComment->append(commentLine);
    }
    else if (singleLineComment)
    {
      // Strip all slashes from the beginning of the comment line.
      stripEverySymbolFromFront(&commentLocalCpy, slash);

      // Strip all whitespaces from the beginning of the comment line.
      stripEverySymbolFromFront(&commentLocalCpy, whitespace);

      // Strip line ending.
      stripSymbol(&commentLocalCpy, endOfLine);

      xmlComment->append(commentLocalCpy);
      break;
    }
  }
  while (breaklocation != std::string::npos);
}

//----------------------------------------------------------------------------------------------------------------------
static void emitXMLBehaviourAttributeSummary(
  NM::TiXmlElement* sectionXml,
  const char* name,
  const char* title,
  const char* comment)
{
  NM::TiXmlElement* variableXml = new NM::TiXmlElement("Attribute");
  variableXml->AddAttribute("name", name);
  if (title && (strlen(title) != 0))
  {
    variableXml->AddAttribute("title", title);
  }
  sectionXml->LinkEndChild(variableXml);

  if (comment && *comment)
  {
    NM::TiXmlElement* commentXml = new NM::TiXmlElement("Comment");
    variableXml->LinkEndChild(commentXml);

    std::string textXml;
    GetCommentForTiXml(comment, &textXml);
    NM::TiXmlText* commentTextXml = new NM::TiXmlText(textXml.c_str());
    commentXml->LinkEndChild(commentTextXml);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <class PtrType>
static void emitXMLBehaviourSectionSummary(
  NM::TiXmlElement* behaviourXml,
  const char* name,
  const nmtl::vector<PtrType>& variableList)
{
  if (!variableList.empty())
  {
    NM::TiXmlElement* sectionXml = new NM::TiXmlElement("Section");
    sectionXml->AddAttribute("name", name);
    behaviourXml->LinkEndChild(sectionXml);

    for (nmtl::vector<PtrType>::const_iterator it = variableList.begin(), itend = variableList.end(); it != itend; ++it)
    {
      const PtrType& av = *it;
      emitXMLBehaviourAttributeSummary(sectionXml, av.m_name.c_str(), 0, av.m_comment.c_str());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void emitXMLBehavioursSummary(NM::TiXmlElement* rootXml, const NetworkDatabase &ndb)
{
  NM::TiXmlElement* behavioursXml = new NM::TiXmlElement("Behaviours");
  rootXml->LinkEndChild(behavioursXml);

  iterate_begin_const_ptr(BehaviourDef, ndb.m_behaviourDefs, bv)
  {
    NM::TiXmlElement* behaviourXml = new NM::TiXmlElement("Behaviour");
    behaviourXml->SetAttribute("name", bv->m_name.c_str());
    behaviourXml->SetAttribute("development", bv->m_development ? "true" : "false");
    behavioursXml->LinkEndChild(behaviourXml);

    NM::TiXmlElement* shortDescXml = new NM::TiXmlElement("ShortDesc");
    behaviourXml->LinkEndChild(shortDescXml);

    if (!bv->m_shortDesc.empty())
    {
      std::string textXml;
      GetCommentForTiXml((bv->m_shortDesc).c_str(), &textXml);
      NM::TiXmlText* commentTextXml = new NM::TiXmlText(textXml.c_str());
      shortDescXml->LinkEndChild(commentTextXml);
    }

    // Write common base type sections.
    if (!bv->m_controlParamGroups.empty())
    {
      NM::TiXmlElement* emSectionXml = new NM::TiXmlElement("ControlParameters");
      emSectionXml->AddAttribute("name", "EmittedMessages");
      behaviourXml->LinkEndChild(emSectionXml);

      iterate_begin_const(BehaviourDef::ControlParamGroup, bv->m_controlParamGroups, cg)
      {
        emitXMLBehaviourSectionSummary(emSectionXml, cg.m_name.c_str(), cg.m_params);
      }
      iterate_end
    }
    emitXMLBehaviourSectionSummary(behaviourXml, "OutputControlParams", bv->m_outputControlParams);
    emitXMLBehaviourSectionSummary(behaviourXml, "AnimationInputs", bv->m_animationInputs);

    // Write emitted messages.
    if (!bv->m_emittedMessages.empty())
    {
      NM::TiXmlElement* emSectionXml = new NM::TiXmlElement("Section");
      emSectionXml->AddAttribute("name", "EmittedMessages");
      behaviourXml->LinkEndChild(emSectionXml);

      iterate_begin_const(BehaviourDef::EmittedMessage, bv->m_emittedMessages, em)
      {
        emitXMLBehaviourAttributeSummary(emSectionXml, em.m_name.c_str(), 0, 0);
      }
      iterate_end
    }

    // Write any other attribute groups.
    StringBuilder attrTitleSb(100);
    iterate_begin_const(BehaviourDef::AttributeGroup, bv->m_attributeGroups, ag)
    {
      NM::TiXmlElement* agSectionXml = new NM::TiXmlElement("Section");
      agSectionXml->AddAttribute("name", ag.m_name.c_str());
      behaviourXml->LinkEndChild(agSectionXml);

      iterate_begin_const(BehaviourDef::AttributeVariable, ag.m_vars, av)
      {
        attrTitleSb.reset();
        getAttributeVariableTitle(av, attrTitleSb);

        emitXMLBehaviourAttributeSummary(agSectionXml, av.m_name.c_str(), attrTitleSb.c_str(), av.m_comment.c_str());
      }
      iterate_end
    }
    iterate_end
  }
  iterate_end
}

//----------------------------------------------------------------------------------------------------------------------
bool emitXMLSummary(const char* outputDirectory, const NetworkDatabase& ndb)
{
  printf("\n---------------------------------------------------------\nXML Generation:\n");

  // Create base xml doc with root.
  NM::TiXmlDocument doc;
  NM::TiXmlDeclaration* decl = new NM::TiXmlDeclaration("1.0", "", "");
  doc.LinkEndChild(decl);
  NM::TiXmlElement* rootXml = new NM::TiXmlElement("BehaviourNetworkDefs");
  doc.LinkEndChild(rootXml);

  emitXMLBehavioursSummary(rootXml, ndb);

  char xmlOutput[MAX_PATH];
  NMP_STRNCPY_S(xmlOutput, MAX_PATH, outputDirectory);
  PathAddBackslash(xmlOutput);
  NMP_STRNCAT_S(xmlOutput, MAX_PATH, "BehaviourNetworkDefs.xml");
  doc.SaveFile(xmlOutput);

  return true;
}
