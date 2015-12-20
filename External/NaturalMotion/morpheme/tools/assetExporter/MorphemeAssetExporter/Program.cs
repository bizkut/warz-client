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
using System.Windows.Forms;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter
{
    static class Program
    {
        public static Logger Logger { get; private set; }
        public static bool UseMultithreadedExport { get; set; }
        public static bool ShowUsage { get; set; }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
          Logger = new Logger();
          MacrosSystem.OutputLog = Logger;
          MorphemeVersionSelector.OutputLog = Logger;
          RegistryTool.RegistryTool.OutputLog = Logger;
          MorphemePipeLine.OutputLog = Logger;

          try
          {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new ApplicationWindow());
          }
          catch (System.Exception ex)
          {
            Logger.Log(LogSeverity.Error, ex.ToString());
          }
        }
    }
}
