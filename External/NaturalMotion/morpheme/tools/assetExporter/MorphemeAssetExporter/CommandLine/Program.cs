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

using System;
using System.ComponentModel;
using System.IO;
using NaturalMotion.AssetExporter.CommandLine;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter.CLI
{
  class Program
  {
    private static Logger Logger { get; set; }

    private static readonly CommandLineTemplate CommandLine = new CommandLineTemplate("")
    .AddCommand("help", "Show usage information", "", OnHelp)
    .AddCommand("export", "Export and process a list of networks", "m|multithreaded", OnExportAndProcess)
    .AddCommand("exportOnly", "Export a list of networks", "m|multithreaded", OnExport);

    private const int ErrorReturnValue = 1;
    private const int SuccessReturnValue = 0;

    public static int Main(string[] args)
    {
      Logger = new Logger();
      MacrosSystem.OutputLog = Logger;
      MorphemeVersionSelector.OutputLog = Logger;
      RegistryTool.RegistryTool.OutputLog = Logger;
      MorphemePipeLine.OutputLog = Logger;

      // parse the command line
      Command command = CommandLineParser.Parse(args);

      if (command == null)
      {
        OnHelp(null);
        return ErrorReturnValue;
      }

      int returnValue = SuccessReturnValue;

      try
      {
        CommandLine.Validate(command);
        command.Invoke(true);
      }
      catch (InvalidCommandException)
      {
        Console.WriteLine("Error: Invalid command '{0}'", command.Name);
        OnHelp(null);
        returnValue = ErrorReturnValue;
      }
      catch (Exception e)
      {
        Console.WriteLine("Error: {0}", e.Message);
        returnValue = ErrorReturnValue;
      }

      Console.Write("Press any key to continue...\n");
      Console.ReadKey();

      return returnValue;
    }

    private static void OnHelp(Command command)
    {
      Console.WriteLine("Usage:\n Commands:");
      foreach (string commandName in CommandLine.CommandTemplates.Keys)
      {
        Console.WriteLine("  {0,-20}{1}", commandName, CommandLine.CommandTemplates[commandName].Description);
      }

      if (CommandLine.OptionTemplates.Keys.Count != 0)
      {
        Console.WriteLine("\n Options:");
        foreach (string optionName in CommandLine.OptionTemplates.Keys)
        {
          Console.WriteLine("  {0,-20}{1}", optionName, CommandLine.OptionTemplates[optionName].Description);
        }
      }
    }

    private static bool PrepareNetworksAndConfig(Command command)
    {
      if (command.Parameters.Length != 2)
      {
        Console.WriteLine("Usage: {0} <configFile> <networkListFile>", command.Name);
        Console.WriteLine("A sample config file and network list are located in morpheme/tools/assetExporter/Samples.");
        return false;
      }

      string configFilePath = command.Parameters[0];
      string networkListFilePath = command.Parameters[1];

      if (File.Exists(configFilePath) == false)
      {
        Program.Logger.Log(LogSeverity.Error, "cannot find config file: " + configFilePath);
        return false;
      }

      if (File.Exists(networkListFilePath) == false)
      {
        Program.Logger.Log(LogSeverity.Error, "cannot find list file: " + networkListFilePath);
        return false;
      }

      using (FileStream fs = new FileStream(configFilePath, FileMode.Open))
      {
        Config config = new Config();
        config.Deserialise(fs);
        MacrosSystem.GetInstance().BuildDictionary();
      }

      using (FileStream fs = new FileStream(networkListFilePath, FileMode.Open))
      {
        MCNCollection.GetInstance().Deserialise(fs);
      }

      return true;
    }

    static void OnExport(Command command)
    {
      if (PrepareNetworksAndConfig(command))
      {
        MorphemePipeLine.GetInstance().Process(false, new ProgressChangedEventHandler(worker_ProgressChanged), null);
        MorphemePipeLine.GetInstance().autoResetEvent.WaitOne();
      }
    }

    static void OnExportAndProcess(Command command)
    {
      if (PrepareNetworksAndConfig(command))
      {
        MorphemePipeLine.GetInstance().Process(true, new ProgressChangedEventHandler(worker_ProgressChanged), null);
        MorphemePipeLine.GetInstance().autoResetEvent.WaitOne();
      }
    }

    static void worker_ProgressChanged(object sender, ProgressChangedEventArgs e)
    {
      // show progress by writing another * to the window
      Console.Write('*');
    }
  }
}
