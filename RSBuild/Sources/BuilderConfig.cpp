#include "r3dPCH.h"
#include "r3d.h"

#include "..\..\External\pugiXML\src\pugixml.hpp"

#include "BuilderConfig.h"

bool BuilderConfig::ParseConfig(const char* xml_fname)
{
  r3dOutToLog("Loading config file %s\n", xml_fname);
  CLOG_INDENT;
  
  FILE* f = fopen(xml_fname, "rb");
  if(!f) {
    r3dError("can't open %s", xml_fname);
    return false;
  }
  
  fseek(f, 0, SEEK_END);
  long xml_size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* xml_data = new char[xml_size + 1];
  fread(xml_data, 1, xml_size, f);
  xml_data[xml_size] = 0;
  fclose(f);
  
  pugi::xml_document xmlDoc;
  pugi::xml_parse_result parseResult = xmlDoc.load_buffer_inplace(xml_data, xml_size);
  if(!parseResult) {
    r3dError("Failed to parse XML, error: %s", parseResult.description());
    return false;
  }
  pugi::xml_node node = xmlDoc.child("build").first_child();
  while(!node.empty()) {
    ParseNode(node);
    node = node.next_sibling();
  }
  
  delete[] xml_data;
  return true;
}

bool BuilderConfig::ParseNode(const pugi::xml_node& node)
{
  const char* name = node.name();

  if(strcmp(name, "output") == 0) 
  {
    const char* dir = node.attribute("dir").value();
    if(*dir == 0) r3dError("%s: no dir\n", dir);
    strcpy(bld_.outputDir_, dir);
    const char* base = node.attribute("base").value();
    if(*base) r3dscpy(bld_.outputBaseName_, base);
  } 
  else if(strcmp(name, "cdn") == 0) 
  {
    const char* url = node.attribute("url").value();
    if(*url == 0) r3dError("%s: no url\n", url);
    strcpy(bld_.webCDNDir_, url);
  } 
  else if(strcmp(name, "exclude") == 0) 
  {
    const char* mask = node.attribute("mask").value();
    if(*mask == 0) r3dError("%s: no mask\n", name);
    bld_.AddExclude(mask);
  } 
  else if(strcmp(name, "include") == 0) 
  {
    const char* mask = node.attribute("mask").value();
    if(*mask == 0) r3dError("%s: no mask\n", name);
    bld_.AddInclude(mask);
  } 
  else if(strcmp(name, "base") == 0) 
  {
    const char* file = node.attribute("file").value();
    if(*file == 0) r3dError("%s: no file\n", name);
    bld_.OpenBaseArchive(file);
  } 
  else
  {
    r3dOutToLog("invalid xml config node '%s'\n", name);
    return false;
  }

  return true;
}
