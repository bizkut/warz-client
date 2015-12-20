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
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter
{
    /// <summary>
    /// Exports a collection of MCNs from morpheme:connect.
    /// </summary>
    public class MorphemeConnectExporter
    {
        private const string morphemeConnectLog = "morphemeConnect_log.txt";

        public MorphemeConnectExporter()
        {
            Log = new List<LogItem>();
        }

        public List<LogItem> Log { get; private set; }

        public const string LuaLogSubDirectory = "\\LuaLogs";
        public const string AppDataSubFolder = "\\Naturalmotion\\AssetExporter";
      
        public class Result
        {
            public Result(MCN mcn)
            {
                Network = mcn;
                ErrorString = string.Empty;
                Success = true;
            }

            public MCN Network { get; set; }
            public string ErrorString { get; set; }
            public bool Success { get; set; }
        }

        public void ExportAll(object sender)
        {
            if (!File.Exists(MacrosSystem.GetInstance().ExpandMacro(Settings.GetInstance().MorphemeConnectPath)))
            {
                Log.Add(new LogItem(LogSeverity.Error, "cannot find morpheme connect exe " + MacrosSystem.GetInstance().ExpandMacro(Settings.GetInstance().MorphemeConnectPath)));
                return;
            }

            string exportRootPath = MacrosSystem.GetInstance().ExpandMacro(Settings.GetInstance().ExportRootPath);
            if (!Directory.Exists(exportRootPath))
            {
                try
                {
                    Directory.CreateDirectory(exportRootPath);
                }
                catch (System.Exception ex)
                {
                    Log.Add(new LogItem(LogSeverity.Error, String.Format("cannot find Export root: {0} ({1})", exportRootPath, ex.Message)));
                    return;
                }
            }

            int index = 0;
            int count = MCNCollection.GetInstance().Count;

            // only required for multithreaded export
            ManualResetEvent[] doneEvents = new ManualResetEvent[count];
            Result[] results = new Result[count];

            List<MCN> uniqueList = MCNCollection.GetInstance().Distinct().ToList();

            if (uniqueList.Count != MCNCollection.GetInstance().Count)
            {
                Log.Add(new LogItem(LogSeverity.Warning, "connect exporter skipping duplicate triplet(s) <mcn, asset compiler, mcp>"));
            }

            Log.Add(new LogItem(LogSeverity.Info, "morpheme connect started exporting " + uniqueList.Count + " networks..."));

            MorphemeConnectBatchExportTask batchExporter = new MorphemeConnectBatchExportTask();

            // export each network
            foreach (MCN network in uniqueList)
            {
                // currently connect writes to a hard coded log file so we need to disable logging or fix the connect log file issue
                try
                {
                    if (!network.Use)
                    {
                        Log.Add(new LogItem(LogSeverity.Info, "connect exporter skipping network at user request: " + network.MCNFileName));
                        continue;
                    }

                    if (!File.Exists(network.GetMCNPathAbsoluteExpanded()))
                    {
                        Log.Add(new LogItem(LogSeverity.Warning, "connect exporter skipping missing mcn " + network.MCNFileName));
                        continue;
                    }

                    if (!File.Exists(network.GetMCPPathAbsoluteExpanded()))
                    {
                        Log.Add(new LogItem(LogSeverity.Warning, "connect exporter skipping missing mcp " + network.MCPFileName));
                        continue;
                    }

                    // create a new Result to monitor when it is done
                    results[index] = new Result(network);

                    string exportPath = MacrosSystem.GetInstance().ExpandMacro(Settings.GetInstance().ExportRootPath);
                    string rootPathAdjustedForNetwork = MacrosSystem.GetInstance().ExpandMacroUsingNetwork(exportPath, network);

                    // create the export directory, if required
                    if (!Directory.Exists(rootPathAdjustedForNetwork))
                    {
                        Directory.CreateDirectory(rootPathAdjustedForNetwork);
                        Log.Add(new LogItem(LogSeverity.Info, "Created directory: " + rootPathAdjustedForNetwork));
                    }

                    // add the task
                    batchExporter.AddTask(network, rootPathAdjustedForNetwork);
                }
                catch (System.IO.IOException ex)
                {
                    Log.Add(new LogItem(LogSeverity.Error, ex.ToString()));
                    doneEvents[index].Set();
                    results[index].Success = false;
                }
                catch (System.Exception ex)
                {
                    Log.Add(new LogItem(LogSeverity.Error, ex.ToString()));
                    doneEvents[index].Set();
                    results[index].Success = false;
                }
            }

            Result result = new Result(new MCN());
            batchExporter.BatchExport(ref result);

            ParseConnectlogFile();

            if (result.Success)
            {
                Log.Add(new LogItem(LogSeverity.GoodNews, "exported all networks" ));
            }
            else
            {
                Log.Add(new LogItem(LogSeverity.Error, result.ErrorString));
            }
        }

        private void ParseConnectlogFile()
        {
            string logFile = MorphemeVersionSelector.GetInstance().GetCurrentValue(MacrosSystem.ReservedVersionMacroKeys.APP_LOCAL_SETTINGS_PATH);

            logFile = Path.Combine(logFile, morphemeConnectLog);

            if (!File.Exists(logFile))
            {
                Log.Add(new LogItem(LogSeverity.Warning, "cannot find log file " + logFile));
            }
            else
            {
                // write the connect log file to the local log
                StreamReader sr = null;
                try
                {
                    sr = new StreamReader(logFile);
                    String line;
                    // Read and display lines from the file until the end of
                    // the file is reached.
                    while ((line = sr.ReadLine()) != null)
                    {
                        string[] parts = line.Split('|');

                        if (parts.Length > 0)
                        {
                            LogSeverity logType = LogSeverity.Info;

                            switch (parts[0].Trim().ToLower())
                            {
                                case "info":
                                    logType = LogSeverity.Info;
                                    break;
                                case "warning":
                                    logType = LogSeverity.Warning;
                                    break;
                                case "error":
                                    logType = LogSeverity.Error;
                                    break;
                                default:
                                    break;
                            }

                            // connect 2.3.3 will report "RUN error" as info
                            if (parts[0].Contains("RUN error"))
                            {
                                logType = LogSeverity.Error;
                            }

                            Log.Add(new LogItem(logType, "morphemeConnect.exe > " + line));
                        }
                    }
                }
                catch (Exception ex)
                {
                  Log.Add(new LogItem(LogSeverity.Warning, ex.Message));
                }
                finally
                {
                  if (sr != null)
                  {
                    sr.Dispose();
                  }
                }
            }
        }
    }

    public class MorphemeConnectBatchExportTask
    {
        public MorphemeConnectBatchExportTask()
        {
            morphemeConnectDataList = new List<MorphemeConnectData>();
        }

        public void AddTask(MCN network, string exportPath)
        {
            morphemeConnectDataList.Add(new MorphemeConnectData(network, exportPath, null));
        }

        public void BatchExport(ref MorphemeConnectExporter.Result result)
        {
            try
            {
                string morphemeConnectExe = MacrosSystem.GetInstance().ExpandMacro(Settings.GetInstance().MorphemeConnectPath);

                string appDataFolder = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
                appDataFolder += MorphemeConnectExporter.AppDataSubFolder;
                string luaLogDirectory = appDataFolder += MorphemeConnectExporter.LuaLogSubDirectory;

                if (false == Directory.Exists(luaLogDirectory))
                {
                  Directory.CreateDirectory(luaLogDirectory);
                }

                string lua = "";

                foreach (MorphemeConnectData morphemeConnectData in morphemeConnectDataList)
                {
                    string mcpFile;
                    string mcnFile;
                    string intermediateFile;

                    MorphemeHelper.GetFilePathsFromNetwork(morphemeConnectData.Network, morphemeConnectData.ExportPath, out mcpFile, out mcnFile, out intermediateFile);

                    // TODO optimization. pre-sort on mcp and only open when project changes

                    // generate the lua to open the project, open the mcn and export the mcn
                    lua += string.Format(" project.open({0})\n mcn.open({1})\n mcn.export({2})", '\"' + mcpFile + '\"', '\"' + mcnFile + '\"', '\"' + intermediateFile + "\"\n");
                }

                string luaFileName = "BatchExport_" + DateTime.Now.Ticks.ToString() + ".lua";

                MorphemeHelper.CreateLuaAndLaunchConnect(luaFileName, lua, morphemeConnectExe, ref result);
            }
            catch (System.Exception ex)
            {
                result.Success = false;
                result.ErrorString = ex.ToString();
            }
        }

        private List<MorphemeConnectData> morphemeConnectDataList { get; set; }
    }

    public class MorphemeConnectExportTask
    {
        public MorphemeConnectExportTask(MCN network, string exepath, ManualResetEvent doneEvent)
        {
            morphemeConnectData = new MorphemeConnectData(network, exepath, doneEvent);
        }

        // Wrapper method for use with thread pool.
        public void ThreadPoolCallback(ref MorphemeConnectExporter.Result result)
        {
            Export(ref result);
            morphemeConnectData.DoneEvent.Set();
        }

        public void Export(ref MorphemeConnectExporter.Result result)
        {
            try
            {
                string morphemeConnectExe = MacrosSystem.GetInstance().ExpandMacro(Settings.GetInstance().MorphemeConnectPath);

                string appDataFolder = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
                appDataFolder += MorphemeConnectExporter.AppDataSubFolder;
                string luaLogDirectory = appDataFolder += MorphemeConnectExporter.LuaLogSubDirectory;

                if (false == Directory.Exists(luaLogDirectory))
                {
                    Directory.CreateDirectory(luaLogDirectory);
                }

                string mcpFile;
                string mcnFile;
                string intermediateFile;

                MorphemeHelper.GetFilePathsFromNetwork(morphemeConnectData.Network, morphemeConnectData.ExportPath, out mcpFile, out mcnFile, out intermediateFile);

                // generate the lua to open the project, open the mcn and export the mcn
                string lua = string.Format(" project.open({0})\n mcn.open({1})\n mcn.export({2})", '\"' + mcpFile + '\"', '\"' + mcnFile + '\"', '\"' + intermediateFile + '\"');

                // generate the lua fileName
                FileInfo mcnfileInfo = new FileInfo(mcnFile);
                string luaFileName = Path.GetFileNameWithoutExtension(mcnfileInfo.Name);
                luaFileName += "_" + DateTime.Now.Ticks.ToString() + ".lua";

                MorphemeHelper.CreateLuaAndLaunchConnect(luaFileName, lua, morphemeConnectExe, ref result);
            }
            catch (System.Exception ex)
            {
                result.Success = false;
                result.ErrorString = ex.ToString();
            }
        }

        MorphemeConnectData morphemeConnectData { get; set; }
    }

    class MorphemeConnectData
    {
        public MorphemeConnectData(MCN network,string exportPath, ManualResetEvent doneEvent)
        {
            Network = network;
            ExportPath = exportPath;
            DoneEvent = doneEvent;
        }

        private MorphemeConnectData() { }

        public MCN Network { get; set; }
        public string ExportPath { get; set; }
        public ManualResetEvent DoneEvent { get; set; }
    }

    public class MorphemeHelper
    {
        public static void GetFilePathsFromNetwork(MCN network, string exportRootPath, out string mcpFile, out string mcnFile, out string intermediateFile)
        {
            // build the xml file
            FileInfo mcnfileInfo = new FileInfo(network.GetMCNPathAbsoluteExpanded());
            string exportFileName = mcnfileInfo.Name.Replace(MCN.MCNExtension, MCN.MCNIntermediateExtension);
            string exportFileFullName = Path.Combine(exportRootPath, exportFileName);

            // format the paths to work with connect
            mcnFile = network.GetMCNPathAbsoluteExpanded().Replace("\\", "/");
            mcpFile = network.GetMCPPathAbsoluteExpanded().Replace("\\", "/");
            intermediateFile = exportFileFullName.Replace("\\", "/");
        }

        public static void CreateLuaAndLaunchConnect( string luaFileName, string lua, string morphemeConnectExe, ref MorphemeConnectExporter.Result result )
        {
            // write out the lua to a file
            string appDataFolder = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
            appDataFolder += MorphemeConnectExporter.AppDataSubFolder;
            string luaLogDirectory = appDataFolder += MorphemeConnectExporter.LuaLogSubDirectory;

            string luaPath = Path.Combine(luaLogDirectory, luaFileName);

            StreamWriter writer = new StreamWriter(luaPath);
            writer.Write(lua);
            writer.Close();

            // build up the command line
            string commandLine = "-nogui -script \"" + luaPath + "\"";

            // fire up connect and run the just generated lua
            Process.LeaveDebugMode();
            using (Process proc = System.Diagnostics.Process.Start(morphemeConnectExe, commandLine))
            {
                // wait for the process to end
                proc.WaitForExit();

                if (proc.ExitCode != 0)
                {
                    result.Success = false;
                    result.ErrorString = "batch export failed " + " " + commandLine + " exited with ExitCode " + proc.ExitCode.ToString();
                }
            }
            Process.EnterDebugMode();
        }
    }

}
