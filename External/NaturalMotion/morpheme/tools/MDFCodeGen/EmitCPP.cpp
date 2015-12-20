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
#include "MappedIO.h"
#include "EmitCPP.h"
#include "EmitCPP_Module.h"
#include "EmitCPP_Types.h"
#include "EmitCPP_Behaviour.h"
#include "EmitCPP_Desc.h"
#include "FileUtils.h"

#include "CodeWriter.h"
#include <fstream>


//----------------------------------------------------------------------------------------------------------------------
bool prepareSubDirectory(const char* outputDirectory, const char* subName, bool &forceGeneration)
{
  StringBuilder sb(64);

  // create a path for types to live in
  sb.appendCharBuf(outputDirectory);
  sb.appendCharBuf(subName);
  sb.appendChar('/');

  // ensure the output directory exists
  if ((_mkdir(sb.getBufferPtr()) == ENOENT) || !PathIsDirectory(sb.c_str()))
  {
    printf("ERROR: could not create/validate %s output directory:\n\t'%s'\n", subName, sb.c_str());
    return false;
  }

  // nuke the contents of the output dir if we are in 'force' mode
  if (forceGeneration)
  {
    deleteFiles(sb.getBufferPtr(), "*.cpp");
    deleteFiles(sb.getBufferPtr(), "*.h");
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool emitNetworkDescriptor(const char* outputDirectory, const NetworkDatabase &ndb, bool forceGeneration)
{
  // simple little jobs-to-do list that calls a given writer fn to generate a named output file
  struct DescEmitJob
  {
    enum { numDescEmitJobs = 6 };
    const char* filename;
    bool (*descWriter)(CodeWriter&, const NetworkDatabase &);
  } jobs[DescEmitJob::numDescEmitJobs] = 
  {
    { "NetworkDescriptor.h" ,     &DescGen::writeDescriptorHeader },
    { "NetworkDescriptor.cpp" ,   &DescGen::writeDescriptorCode },
    { "NetworkDataMemory.h" ,     &DescGen::writeDataMemoryHeader },
    { "NetworkForceInclude.h" ,   &DescGen::writeForceInclude },
    { "NetworkConstants.h",       &DescGen::writeNetworkConstants },
    { "NetworkExecution.cpp",     &DescGen::writeTaskExec },
  };

  StringBuilder sb(64);
  sb.appendCharBuf(outputDirectory);
  sb.save();

  CodeWriter codeFile;

  for (int i=0; i<DescEmitJob::numDescEmitJobs; i++)
  {
    sb.restore();
    sb.appendCharBuf(jobs[i].filename);

    // generate the contents
    codeFile.begin();
    if (jobs[i].descWriter(codeFile, ndb))
    {
      // unless we are forced to regenerate, check to see if the on-disk version is different
      bool filesMatched = false;
      if (!forceGeneration)
        filesMatched = fileContentsCompare(sb.c_str(), codeFile.getContents());

      if (!filesMatched)
      {
        printf("    %s\n", jobs[i].filename);

        FILE *myFile = NULL;
        fopen_s(&myFile, sb.c_str(), "wt");

        if (!myFile)
        {
          printf("    Could not open for writing (Error %i)\n", sb.c_str(), GetLastError());
          return false;
        }

        codeFile.writeContentsAndClose(myFile);
        fclose(myFile);
      }
    }
    else
    {
      printf(" Generation Failed - %s\n", sb.c_str());
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool writeInterfaceHeader(CodeWriter &code, const NetworkDatabase &ndb)
{
  HeaderGuard hg("BehaviourInterfaces");
  code.writeHeaderTop(hg);

  code.writeHorizontalSeparator();

  code.write("#include \"NetworkDescriptor.h\"");
  iterate_begin_const_ptr(ModuleDef, ndb.m_moduleDefs, def)
  {
    if (hasModuleModifier(def->m_modifiers, mmGenerateInterface))
      code.write("#include \"%s.h\"", def->m_name);
  } iterate_end;
  code.newLine();

  code.write("#ifndef NM_BEHAVIOUR_LIB_NAMESPACE");
  code.write("  #error behaviour library namespace undefined");
  code.write("#endif");
  code.newLine();

  code.write("namespace NM_BEHAVIOUR_LIB_NAMESPACE\n{\n");

  code.writeHorizontalSeparator();
  code.write("template <typename BIType>");
  code.write("class BI");
  code.openBrace();
  code.write("public:");
  code.write("BI(const ER::Character *character, NetworkManifest::Modules moduleType) : m_module(0)");
  code.openBrace();
  code.write("if (!character || !character->m_euphoriaRootModule->isEnabled(moduleType))");
  code.write("  return;");
  code.write("ER::Module *m = character->m_euphoriaRootModule->getModule(moduleType);");
  code.write("m_module = (BIType *) m;");
  code.closeBrace();
  code.write("bool isAvailable() const {return m_module != 0;}");
  code.write("protected:");
  code.write("const BIType *m_module;");
  code.closeBraceWithSemicolon();
  code.newLine();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool writeInterfaceFooter(CodeWriter &code)
{
  code.writeHorizontalSeparator();

  code.write("} // namespace NM_BEHAVIOUR_LIB_NAMESPACE");
  code.newLine();

  HeaderGuard hg("BehaviourInterfaces.h");
  code.writeHeaderBottom(hg);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool emitCPPCode(const char* outputDirectory, const char* projectRootDir, const NetworkDatabase &ndb, bool forceGeneration)
{
  printf("\n---------------------------------------------------------\nCode Generation (Types):\n");

  if (!prepareSubDirectory(outputDirectory, "Types", forceGeneration))
    return false;

  CodeWriter codeWriter;
  TypeGen typeGen(ndb.m_keywords, &codeWriter, projectRootDir);
  ModuleGen moduleGen(ndb.m_keywords, &codeWriter, projectRootDir);

  // generate struct type headers
  StructMap::const_value_walker structs = ndb.m_structMap.constWalker();
  while (structs.next())
  {
    Struct* s = structs();

    iterate_begin_const(CodeGenArtefact, s->m_artefacts, atf)
    {
      switch (atf.m_type)
      {
        case Struct::StructHeader:
          {
            const char* filePath = atf.m_fullPathCache.c_str();

            codeWriter.begin();
            bool genSucceeded = typeGen.writeType(s->m_type);

            if (genSucceeded)
            {
              // unless we are forced to regenerate, check to see if the on-disk version is different
              bool filesMatched = false;
              if (!forceGeneration)
                filesMatched = fileContentsCompare(filePath, codeWriter.getContents());

              if (!filesMatched)
              {
                printf("    %s\n", atf.m_filepath.c_str());

                FILE *myFile = NULL;
                fopen_s(&myFile, filePath, "wt");

                if (!myFile)
                {
                  printf("Could not open '%s' for writing (Error %i)\n", filePath, GetLastError());
                  return false;
                }

                codeWriter.writeContentsAndClose(myFile);
                fclose(myFile);
              }
            }
            else
            {
              printf("Error generating code - '%s'\n", filePath);
              DeleteFile(filePath);
            }
          }
          break;

        default:
          printf("Unknown artefact ID encountered in '%s' (ID %i)\n", s->m_name.c_str(), atf.m_type);
          return false;
          break;
      }
    }
    iterate_end
  }

  // -------------------------------------------------------------------------------------------------------------------

  // nuke the contents of the output dir if we are in 'force' mode
  if (forceGeneration)
  {
    deleteFiles(outputDirectory, "*.h");
    deleteFiles(outputDirectory, "*.cpp");
  }

  printf("\n---------------------------------------------------------\nCode Generation (Modules):\n");
  iterate_begin_const_ptr(ModuleDef, ndb.m_moduleDefs, def)
  {
    iterate_begin_const(CodeGenArtefact, def->m_artefacts, atf)
    {
      const char* filePath = atf.m_fullPathCache.c_str();

      bool genSucceeded = false;
      
      codeWriter.begin();
      switch (atf.m_type)
      {
        case ModuleDef::ModuleCode:
          genSucceeded = moduleGen.writeCode(*def);
          break;

        case ModuleDef::ModuleHeader:
          genSucceeded = moduleGen.writeHeader(*def);
          break;

        case ModuleDef::ModulePackaging:
          genSucceeded = moduleGen.writePackagingHeader(*def);
          break;

        case ModuleDef::ModulePackagingCode:
          genSucceeded = moduleGen.writePackagingCode(*def);
          break;

        case ModuleDef::ModuleData:
          genSucceeded = moduleGen.writeDataHeader(*def);
          break;

        default:
          printf("Unknown artefact ID encountered in '%s' (ID %i)\n", def->m_name.c_str(), atf.m_type);
          return false;
          break;
      }

      if (genSucceeded)
      {
        // unless we are forced to regenerate, check to see if the on-disk version is different
        bool filesMatched = false;
        if (!forceGeneration)
          filesMatched = fileContentsCompare(filePath, codeWriter.getContents());

        if (!filesMatched)
        {
          printf("    %s\n", atf.m_filepath.c_str());

          FILE *myFile = NULL;
          fopen_s(&myFile, filePath, "wt");
          if (!myFile)
          {
            printf("Could not open '%s' for writing (Error %i)\n", filePath, GetLastError());
            return false;
          }

          codeWriter.writeContentsAndClose(myFile);
          fclose(myFile);
        }
      }
      else
      {
        printf("Error generating code - '%s'\n", filePath);
        DeleteFile(filePath);
      }
    }
    iterate_end
  }
  iterate_end

  printf("\n---------------------------------------------------------\nCode Generation (Interfaces):\n");
  {
    StringBuilder sb(64);
    sb.appendCharBuf(outputDirectory);
    sb.appendCharBuf("BehaviourInterfaces.h");
    const char* filePath = sb.c_str();

    codeWriter.begin();
    writeInterfaceHeader(codeWriter, ndb);
    iterate_begin_const_ptr(ModuleDef, ndb.m_moduleDefs, def)
    {
      if (!moduleGen.writeInterface(*def, codeWriter))
      {
        printf("Error generating interface code for '%s'\n", def->m_name);
        return false;
      }
    } iterate_end;
    writeInterfaceFooter(codeWriter);

    bool filesMatched = filesMatched = fileContentsCompare(filePath, codeWriter.getContents());

    if (!filesMatched)
    {
      printf("    %s\n", filePath);

      FILE *myFile = NULL;
      fopen_s(&myFile, filePath, "wt");
      if (!myFile)
      {
        printf("Could not open '%s' for writing (Error %i)\n", filePath, GetLastError());
        return false;
      }

      codeWriter.writeContentsAndClose(myFile);
      fclose(myFile);
    }
  }

  // -------------------------------------------------------------------------------------------------------------------

  if (!prepareSubDirectory(outputDirectory, "Behaviours", forceGeneration))
    return false;

  printf("\n---------------------------------------------------------\nCode Generation (Behaviours):\n");
  iterate_begin_const_ptr(BehaviourDef, ndb.m_behaviourDefs, bv)
  {
    iterate_begin_const(CodeGenArtefact, bv->m_artefacts, atf)
    {
      const char* filePath = atf.m_fullPathCache.c_str();

      bool genSucceeded = false;

      codeWriter.begin();
      switch (atf.m_type)
      {
        case BehaviourDef::BehaviourCode:
          genSucceeded = BehaviourGen::writeBehaviourClass(*bv, codeWriter, projectRootDir);
          break;

        case BehaviourDef::BehaviourHeader:
          genSucceeded = BehaviourGen::writeBehaviourHeader(*bv, codeWriter, ndb.m_limbInstances, projectRootDir);
          break;

        default:
          printf("Unknown artefact ID encountered in '%s' (ID %i)\n", bv->m_name.c_str(), atf.m_type);
          return false;
          break;
      }

      if (genSucceeded)
      {
        // unless we are forced to regenerate, check to see if the on-disk version is different
        bool filesMatched = false;
        if (!forceGeneration)
          filesMatched = fileContentsCompare(filePath, codeWriter.getContents());

        if (!filesMatched)
        {
          printf("    %s\n", atf.m_filepath.c_str());

          FILE *myFile = NULL;
          fopen_s(&myFile, filePath, "wt");
          if (!myFile)
          {
            printf("Could not open '%s' for writing (Error %i)\n", filePath, GetLastError());
            return false;
          }

          codeWriter.writeContentsAndClose(myFile);
          fclose(myFile);
        }
      }
      else
      {
        printf("Error generating code - '%s'\n", filePath);
        DeleteFile(filePath);
      }

    }
    iterate_end
  }
  iterate_end

  printf("\n---------------------------------------------------------\nWriting Network Bindings:\n");
  if (!emitNetworkDescriptor(outputDirectory, ndb, forceGeneration))
    return false;

  return true;
}
