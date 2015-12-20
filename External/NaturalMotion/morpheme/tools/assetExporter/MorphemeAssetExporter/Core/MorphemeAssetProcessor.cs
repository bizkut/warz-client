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
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter
{
    /// <summary>
    /// Runs asset compilers as tasks in a thread pool to process all exported morpheme networks.
    /// </summary>
    class MorphemeAssetProcessor
    {
        public class Result
        {
            public Result(MCN network)
            {
                Network = network;
                Platform = string.Empty;
                ErrorString = string.Empty;
                Success = true;
            }

            public MCN Network { get; set; }
            public string Platform { get; set; }
            public string ErrorString { get; set; }
            public bool Success { get; set; }
        }

        public MorphemeAssetProcessor()
        {
            Log = new List<LogItem>();
        }

        public List<LogItem> Log { get; private set; }

        public void ProcessAll(object sender)
        {
            // using a thread pool, fire up all the reuqired asset compilers

            int index = 0;
            int numSelectedPlatforms = Settings.GetInstance().NumSelectedPlatforms();
            int numSelectedNetworks = MCNCollection.GetInstance().NumSelectedNetworks();
            int count = numSelectedPlatforms * numSelectedNetworks;
            

            // set up the events to monitor when each task is done
            ManualResetEvent[] doneEvents = new ManualResetEvent[count];
            Result[] results = new Result[count];

            // check for duplicates
            List<MCN> uniqueList = MCNCollection.GetInstance().Distinct().ToList();

            if (uniqueList.Count != MCNCollection.GetInstance().Count)
            {
                Log.Add(new LogItem(LogSeverity.Warning, "asset processor skipping duplicate triplet(s) <mcn, asset compiler, mcp>"));
            }


            int actualNumNetworks = 0;
            foreach (MCN mcn in uniqueList)
            {
                if (mcn.Use)
                {
                    ++actualNumNetworks;
                }
            }

            Log.Add(new LogItem(LogSeverity.Info, "asset compiler started processing " + actualNumNetworks + " networks on " + numSelectedPlatforms + " platforms..."));

            // for each platform export all the mcns
            foreach (PlatformConfiguration platform in Settings.GetInstance().Platforms)
            {
                if (platform.UseDuringExport == false)
                {
                    index++;
                    Log.Add(new LogItem(LogSeverity.Info, "asset processor skipping platform at user request: " + platform.AssetCompilerCustomEXE));
                    continue;
                }

                foreach (MCN network in uniqueList)
                {
                    try
                    {
                        if (network.Use == false)
                        {
                            index++;
                            Log.Add(new LogItem(LogSeverity.Info, "asset processor skipping network at user request: " + network.MCNFileName));
                            continue;
                        }

                        // skip over missing mcns, missing mcps
                        if (!File.Exists(network.GetMCNPathAbsoluteExpanded()) ||
                            !File.Exists(network.GetMCPPathAbsoluteExpanded())
                            )
                        {
                            index++;
                            Log.Add(new LogItem(LogSeverity.Warning, "asset processor skipping missing mcn/mcp " + network.MCNFileName + " " + network.MCPFileName));
                            continue;
                        }

                        // build up all the paths to generate the asset compiler exe
                        string assetCompilerPath = MacrosSystem.GetInstance().ExpandMacro(Settings.GetInstance().RuntimeSDKRoot);
                        string assetCompilerType = Enum.GetName(typeof(MCN.AssetCompilerType), network.AssetCompiler);
                                             
                        string assetCompilerExe = MacrosSystem.GetInstance().ExpandMacro(platform.AssetCompilerCustomEXE);

                        string commandLineOptions = ConstructAssetCompilerOptions(assetCompilerExe, Settings.GetInstance().AssetCompilerCommandLine, network.AssetCompiler, platform);
                        
                        if (!File.Exists(assetCompilerExe))
                        {
                            Log.Add(new LogItem(LogSeverity.Error, "cannot find asset compiler: " + assetCompilerExe));
                            continue;
                        }

                        results[index] = new Result(network);

                        // set task to 'not done'
                        doneEvents[index] = new ManualResetEvent(false);

                        // build more paths
                        string rootPathAdjustedForNetwork = MacrosSystem.GetInstance().ExpandMacroUsingNetwork(Settings.GetInstance().ExportRootPath, network);

                        string processSubPath = Path.Combine(rootPathAdjustedForNetwork, MacrosSystem.GetInstance().ExpandMacroUsingNetwork(platform.ProcessPathSubDirectory, network));

                        if (false == Directory.Exists(processSubPath))
                        {
                            if (File.Exists(processSubPath))
                            {
                                Log.Add(new LogItem(LogSeverity.Error, "cannot create directory - file with this name already exists: " + processSubPath));
                                index++;
                                continue;
                            }

                            Directory.CreateDirectory(processSubPath);

                            // wait for creation
                            while (false == Directory.Exists(processSubPath))
                            {
                                Thread.Sleep(100);
                            }

                            Log.Add(new LogItem(LogSeverity.Info, "Created directory: " + processSubPath));
                        }

                        // create the task and send it to the thread pool
                        MorphemeAssetProcessorTask processor = new MorphemeAssetProcessorTask(network, rootPathAdjustedForNetwork, processSubPath, assetCompilerExe, commandLineOptions, doneEvents[index]);

                        // need to keep a local copy as it doesn't appear that the lambda expression below is evaluated on that line
                        Result res = results[index];
                        ThreadPool.QueueUserWorkItem(o => processor.ThreadPoolCallback(ref res));

                        // default to true and set to false on errors (see below)
                        network.Processed = true;

                        // finally, increment index (ensure this is the final statement as it is used in the catch below)
                        index++;
                    }
                    catch (System.Exception ex)
                    {
                        Log.Add(new LogItem(LogSeverity.Error, ex.ToString()));
                        doneEvents[index].Set();
                        results[index].Success = false;
                    }
                }
            }

            try
            {
                index = 0;

                // wait for all the threads to be done
                foreach (WaitHandle handle in doneEvents)
                {
                    if (handle != null)
                    {
                        handle.WaitOne();
                    }

                    if (results[index] != null)
                    {
                        if (results[index].Success)
                        {
                            Log.Add(new LogItem(LogSeverity.GoodNews, "processed " + results[index].Platform + " " + results[index].Network.GetMCNPathAbsoluteExpanded()));
                        }
                        else
                        {
                            Log.Add(new LogItem(LogSeverity.Error, "FAILED " + results[index].Platform + " " + results[index].Network.GetMCNPathAbsoluteExpanded() + " " + results[index].ErrorString));
                            results[index].Network.Processed = false;
                        }
                    }
                  
                    index++;

                    // update the progress bar
                    if (sender != null)
                    {
                        (sender as BackgroundWorker).ReportProgress(50 + ((50 * index) / count));
                    }
                }
            }
            catch (System.Exception ex)
            {
                Log.Add(new LogItem(LogSeverity.Error, ex.ToString()));
            }
        }

        /// <summary>
        /// Automatically determine additional asset compiler options.
        /// - Check if a plugin cfg file or a plugin list are passed in on the command line. In these cases, the list of plugins is predetermined.
        /// - If no list, nor cfg are used, check if 
        /// - In any other cases, the asset compiler if for an older version (3.6), where the executable describes the asset compiler completely.
        /// </summary>
        /// <param name="assetCompilerExe"></param>
        /// <param name="assetCompilerCommandLine"></param>
        /// <returns></returns>
        public static string ConstructAssetCompilerOptions(string assetCompilerExe, string assetCompilerCommandLine, MCN.AssetCompilerType assetCompilerType, PlatformConfiguration platform)
        {
            // If the dynamic plugins are not used (<3.6) there is no need for special options.
            // Same thing if the plugins are specified in a config file or via command line already.
            bool useDynamicPlugins = Settings.GetInstance().UseDynamicAssetCompilerPlugins;
            if (!useDynamicPlugins || assetCompilerCommandLine.Contains("-plugin") || assetCompilerCommandLine.Contains("-optionsFile"))
            {
                // In this case, it's taken care on the command line.
                // TODO Add compatibility warnings. For example: -plugin acPlugin_Physics
                return "";
            }

            // Euphoria is a special case.
            if (assetCompilerType == MCN.AssetCompilerType.Euphoria)
            {
                return "-plugin acPluginPhysX3 -plugin acPluginEuphoria";
            }

            // Another simple case is when this is a no physics asset compiler without additional modules (kinect, move).
            if (assetCompilerType == MCN.AssetCompilerType.NoPhysics && !platform.UseKinect && !platform.UseMove)
            {
                return "";
            } 

            // Ok, at this point, some sort of plugin will be needed.
            string assetCompilerOptions = "";

            string pluginPrefix = "acPlugin";

            bool usePhysicsPlugin = assetCompilerType != MCN.AssetCompilerType.NoPhysics;        
            if (usePhysicsPlugin)
            {
              string physicsPlugin = pluginPrefix + assetCompilerType.ToString();

              // Add the plugin to the list.
              assetCompilerOptions += "-plugin " + physicsPlugin + " ";
            }

            bool useAdditionalPlugin = platform.UseMove || platform.UseKinect;
            if (useAdditionalPlugin)
            {
                string additionalPlugin = pluginPrefix;

                if (platform.UseMove)
                {
                    additionalPlugin += "Move";
                }
                else
                {
                    additionalPlugin += "Kinect";
                }

                assetCompilerOptions += "-plugin " + additionalPlugin + " ";
            }     

            return assetCompilerOptions;
        }

#if FALSE
        public static string ConstructAssetCompilerFile(string assetCompilerPath, PlatformConfiguration.BuildConfigs buildConfig, string assetCompilerType, string assetCompilerPlatform)
        {
            string assetCompilerSubPath = PlatformConfiguration.BuildConfigPaths[(int)buildConfig];

            string assetCompiler = string.Format("morphemeAssetCompiler_{0}_target_{1}.exe", assetCompilerType, assetCompilerPlatform);

            string fullPath = Path.Combine(assetCompilerPath, assetCompilerSubPath);

            string assetCompilerFull = Path.Combine(fullPath, assetCompiler);

            if (false == File.Exists(assetCompilerFull))
            {
                // 3.5 has a new format for the asset compiler path/name. Try this format.
                assetCompilerSubPath = PlatformConfiguration.BuildConfigPaths3_5[(int)buildConfig];

                string exeSuffix = PlatformConfiguration.BuildConfigPathsEXESuffix3_5[(int)buildConfig];

                assetCompiler = string.Format("morphemeAssetCompiler_{0}_target_{1}{2}.exe", assetCompilerType, assetCompilerPlatform, exeSuffix);

                fullPath = Path.Combine(assetCompilerPath, assetCompilerSubPath);

                assetCompilerFull = Path.Combine(fullPath, assetCompiler);
            }

            return assetCompilerFull;
        }
#endif

    }


    class MorphemeAssetProcessorTask
    {
        public MorphemeAssetProcessorTask(MCN network, string exportRootPath, string exportSubPath, string assetCompilerExe, string commandLineOptions, ManualResetEvent doneEvent)
        {
            Network = network;
            ExportRootPath = exportRootPath;
            ProcessSubPath = exportSubPath;
            AssetCompilerExe = assetCompilerExe;
            CommandLineOptions = commandLineOptions;
            DoneEvent = doneEvent;
        }
  
        // Wrapper method for use with thread pool.
        public void ThreadPoolCallback(ref MorphemeAssetProcessor.Result result)
        {
            Process(ref result);         
            DoneEvent.Set();
        }

        public void Process(ref MorphemeAssetProcessor.Result result)
        {
            try
            {
                // get the paths from the Network 
                string mcpFile;
                string mcnFile;
                string intermediateFile;

                MorphemeHelper.GetFilePathsFromNetwork(Network, ExportRootPath, out mcpFile, out mcnFile, out intermediateFile);

                result.Platform = AssetCompilerExe;

                if (File.Exists(intermediateFile) == false)
                {
                    result.Success = false;
                    result.ErrorString = "Cannot find intermediate file: " + intermediateFile;
                    return;
                }

                // extract the root path
                FileInfo mcpfileInfo = new FileInfo(mcpFile);
                string rootPath = mcpfileInfo.DirectoryName;

                string commandLine = "-baseDir \"" + rootPath + "\" -outputDir \"" + ExportRootPath + ProcessSubPath + "\" " + Settings.GetInstance().AssetCompilerCommandLine + " -asset \"" + intermediateFile + "\"";
                if (!CommandLineOptions.Equals(""))
                {
                  commandLine += " ";
                  commandLine += CommandLineOptions;
                }

                // get ready to start the asset compiler with the above command line in a hidden window
                ProcessStartInfo procStartInfo = new ProcessStartInfo(AssetCompilerExe, commandLine);
                procStartInfo.WindowStyle = ProcessWindowStyle.Hidden;

                // run the asset compiler
                using (Process proc = System.Diagnostics.Process.Start(procStartInfo))
                {
                    // wait for the process to end
                    proc.WaitForExit();

                    if (proc.ExitCode != 0)
                    {
                        result.Success = false;
                        result.ErrorString = AssetCompilerExe + " " + commandLine + " exited with code " + proc.ExitCode.ToString();
                    }
                }
            }
            catch (System.Exception ex)
            {
                result.Success = false;
                result.ErrorString = ex.ToString();
            }
        }

        private MCN Network { get; set; }
        private string AssetCompilerExe { get; set; }
        private string CommandLineOptions { get; set; }
        private string ExportRootPath { get; set; }
        private string ProcessSubPath { get; set; }
        private ManualResetEvent DoneEvent { get; set; }
    }
}
