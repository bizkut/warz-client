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
using System.Threading;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter
{
    /// <summary>
    /// Exports assets from morpheme:connect and optionally runs asset compilers to process the exported networks.
    /// </summary>
    public class MorphemePipeLine
    {
        public static MorphemePipeLine GetInstance()
        {
            if (Instance == null)
            {
                Instance = new MorphemePipeLine();
            }

            return Instance;
        }

        public AutoResetEvent autoResetEvent { get; private set; }

        public static Logger OutputLog { private get; set; }

        public void Process(bool bExportAndProcess, ProgressChangedEventHandler progressChangedEventHandler, RunWorkerCompletedEventHandler workerCompletedEventHandler)
        {
            autoResetEvent = new AutoResetEvent(false);

            StartTimer = DateTime.Now;

            foreach (MCN network in MCNCollection.GetInstance())
            {
                network.Processed = false;
                network.Exported = false;
            }

            // fire up a background worker thread to keep the UI responsive
            BackgroundWorker worker = new BackgroundWorker();

            if (bExportAndProcess)
            {
                worker.DoWork += new DoWorkEventHandler(worker_DoWorkExportAndProcess);
            }
            else
            {
                worker.DoWork += new DoWorkEventHandler(worker_DoWorkExport);
            }

            // add the default event handlers and the caller defined versions, if valid
            {
                worker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(worker_RunWorkerCompleted);

                if (workerCompletedEventHandler != null)
                {
                    worker.RunWorkerCompleted += workerCompletedEventHandler;
                }

                worker.ProgressChanged += new ProgressChangedEventHandler(worker_ProgressChanged);

                if (progressChangedEventHandler != null)
                {
                    worker.ProgressChanged += progressChangedEventHandler;
                }
            }

            worker.WorkerReportsProgress = true;

            worker.RunWorkerAsync();
        }

        void worker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            // nothing to do currently
        }

        void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            // pull out the log from the completed thread
            List<LogItem> log = new List<LogItem>(e.Result as List<LogItem>);

            int Errors = 0;
            int Warnings = 0;

            // count up the errors/warnigns to decide on the colour of the final message
            foreach (LogItem item in log)
            {
                OutputLog.Log(item.Severity, item.Message);

                if (item.Severity == LogSeverity.Error)
                {
                    Errors++;
                }
                else if (item.Severity == LogSeverity.Warning)
                {
                    Warnings++;
                }
            }

            LogSeverity completedMessageType = LogSeverity.GoodNews;

            if (Errors > 0)
            {
                completedMessageType = LogSeverity.Error;
            }
            else if (Warnings > 0)
            {
                completedMessageType = LogSeverity.Warning;
            }

            OutputLog.Log(completedMessageType, "Batch completed with: " + Errors + " errors and " + Warnings + " warnings.");

            // show how long it took
            TimeSpan timeSpan = DateTime.Now - StartTimer;
            OutputLog.Log(LogSeverity.Info, "Total time: " + timeSpan.TotalSeconds.ToString("N2") + " seconds.");

            autoResetEvent.Set();
        }

        void worker_DoWorkExportAndProcess(object sender, DoWorkEventArgs e)
        {
            // export...
            MorphemeConnectExporter Exporter = new MorphemeConnectExporter();
            Exporter.ExportAll(sender);

            /// ...and process!
            MorphemeAssetProcessor Processor = new MorphemeAssetProcessor();
            Processor.ProcessAll(sender);

            // get the local logs from both and pass it as the Result
            List<LogItem> Log = new List<LogItem>(Exporter.Log);
            Log.AddRange(Processor.Log);

            e.Result = Log;
        }

        void worker_DoWorkExport(object sender, DoWorkEventArgs e)
        {
            // export...
            MorphemeConnectExporter Exporter = new MorphemeConnectExporter();
            Exporter.ExportAll(sender);

            // get the local from the exporter and pass it as the Result
            List<LogItem> Log = new List<LogItem>(Exporter.Log);
            e.Result = Log;
        }

        private static MorphemePipeLine Instance = null;

        private DateTime StartTimer { get; set; }

        private MorphemePipeLine()
        {
        }
    }
}
