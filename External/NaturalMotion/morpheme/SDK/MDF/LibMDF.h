#pragma once
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

class ParserMemory;
struct MDFNetworkState;
struct SyntaxHighlighterState;
struct NetworkDatabase;

// ---------------------------------------------------------------------------------------------------------------------
// interface class for handling logging / error reporting during parsing and MDF processing; client
// applications will inherit from and implement pure virtuals to respond to events. This class is passed
// into the major top-level LibMDF manager classes.
//
class MDFFeedbackInterface
{
public:

  virtual ~MDFFeedbackInterface();

  enum Stage
  {
    Parser,                           // tokenizing
    Execution,                        // executing rules, building MDF DB
    Validation,                       // validating MDF DB
    Connectivity,                     // wiring up modules, checking connections

    StageCount
  };

  // called when a given block of processing is about to begin
  virtual void onStageBegin(
    Stage stage) = 0;

  // called when something has gone awry
  virtual void onProcessingFailure(
    Stage duringStage,                // which processing stage we were in when something bad happened
    const char* inFilename,           // full path to the filename where the error is located, or empty if we are running syntax highlighting on memory streams
    unsigned int atLine,              // 0-base line where the error lies
    const char *message,              // description of the problem
    const char *reftext,              // [may be null] otherwise name/ref of specific object causing the problem (eg variable name, attribute text)
    void *clientUserData) = 0;        // [may be null] passed back if set by client code during network load

  // called once a given block is finished successfully
  virtual void onStageComplete(
    Stage stage) = 0;

  enum MessageType
  {
    Info,
    Warning,
    Error,
    APIError,

    MessageTypeCount
  };

  // printf() style logging that then calls onLogMessage()
  void logMessage(MDFFeedbackInterface::MessageType mt, const char* msg, ...);

protected:

  // called to report log messages
  virtual void onLogMessage(
    MDFFeedbackInterface::MessageType mt,
    const char* msg) = 0;
};



// ---------------------------------------------------------------------------------------------------------------------
// NetworkBuilder is the main utility class that clients of LibMDF will use. It offers a simple API
// to load types and MDF files, assemble them into a network and work with the results.
//
// init()
// {
//   addNetworkConstant()s if required
//
//   { 
//     addTypes() for each .types file
//   }
//   postProcessTypes();
//
//   { 
//     addMDF() for each .module file
//   }
//   postProcessMDFs();
//
//   { 
//     addBehaviourDef() for each .behaviour file
//   }
//   postProcessBehaviourDefs();
// }
//
class MDFNetworkBuilder
{
public:

  MDFNetworkBuilder();
  ~MDFNetworkBuilder();

  // -------------------------------------------------------------------------------------------------------------------

  // call to prepare to build the network - the MDFNetworkBuilder can be re-used, just call term() and
  // then init() again to build a new network. This re-uses memory managers and can be more efficient than
  // building a MDFNetworkBuilder over and over
  void init(
    MDFFeedbackInterface* fbInterface,      // valid instance of feedback interface to use when reporting errors, logging
    unsigned int networkUID);               // UID for this network, should be unique per group of networks loaded at runtime


  // -------------------------------------------------------------------------------------------------------------------

  // add globally available key-value pair; can be used to size arrays, etc. in module code
  bool addNetworkConstant(const char* cName, int cValue);

  // -------------------------------------------------------------------------------------------------------------------

  // load a type definition file
  bool addTypes(const char* typedefFileName, void* clientUserData = 0);

  // call once all types have been loaded, validates and registers all loaded types
  bool postProcessTypes();

  // -------------------------------------------------------------------------------------------------------------------

  // call on each file to add to the internal network representation. returns false if anything fails.
  bool addMDF(const char* fname, void* clientUserData = 0);

  // call once all MDFs have been loaded successfully to perform verification & connectivity phases
  bool postProcessMDFs();

  // -------------------------------------------------------------------------------------------------------------------

  // parse a behaviour definition and add it into the database
  bool addBehaviourDef(const char* fname, void* clientUserData = 0);

  // finalize the behaviour definitions
  bool postProcessBehaviourDefs();

  // -------------------------------------------------------------------------------------------------------------------

  // optional stage, preparation for code generation; creates 'artifcats' to represent the output
  // files to be generated, does hierarhical checking to see what needs to be built, etc
  bool preprocessForCodeGeneration(const char* outputDirectory, bool forceRegeneration, bool verboseMode);

  // -------------------------------------------------------------------------------------------------------------------

  // is it valid to call getNDB()
  bool getNDBIsValid() const;
  // get access to the loaded network
  const NetworkDatabase& getNDB() const;


  // -------------------------------------------------------------------------------------------------------------------

  // call before re-init()'ing to flush out the contents of the loaded network
  void term();


private:
  MDFNetworkState* state;
  ParserMemory* mmgr;
  MDFNetworkBuilder(const MDFNetworkBuilder&);
};


// ---------------------------------------------------------------------------------------------------------------------
// interface class used to handle syntax highlighting events, passed to the syntax highlighting helper
// class.
//
class MDFSyntaxHighlightingInterface
{
public:

  virtual ~MDFSyntaxHighlightingInterface();

  enum Styling
  {
    sStyleBase = 4,

    sKeyword1,              // generic keyword, eg. const, static
    sKeyword2,              // mdf mark-up keyword, eg. module_def, in:, out:
    sTypeName,
    sFuncName,
    sEnumEntry,
    sIdentifier,            // catch-all for variable names, etc
    sString,
    sModifierAttribute,     // __these_things__ 
    sComment,
    sNumeric,
  };

  /**
   * mark the given chunk of text as the given style; line & column are 0-base
   */
  virtual void apply(int line, int column, int length, Styling st) = 0;

  /**
   * if required, enable a folding block from start-end lines given;
   * line is 0-base, endLine is exclusive
   */
  virtual void markBlock(int startLine, int endLine) = 0;
};


// ---------------------------------------------------------------------------------------------------------------------
// helper class for running syntax highlighting passes on individual MDF code files.
// designed to be instantiated once and then re-used, called many times a second if 
// necessary.
//
class MDFSyntaxHighlighter
{
public:

  MDFSyntaxHighlighter(MDFFeedbackInterface* fbInterface);
  ~MDFSyntaxHighlighter();

  enum Grammar
  {
    Types,
    Modules,
    Behaviours
  };

  bool run(const char* filename, Grammar gr, MDFSyntaxHighlightingInterface* shi);
  bool run(const char* stream, size_t streamLength, Grammar gr, MDFSyntaxHighlightingInterface* shi);
  bool run(const wchar_t* stream, size_t streamLength, Grammar gr, MDFSyntaxHighlightingInterface* shi);

private:
  SyntaxHighlighterState* state;
  MDFSyntaxHighlighter(const MDFSyntaxHighlighter&);
};
