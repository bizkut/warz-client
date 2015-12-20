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
using System.Drawing;
using System.IO;
using NaturalMotion.AssetExporter.Internal;

namespace NaturalMotion.AssetExporter.Logging
{
    /// <summary>
    /// Top-level interface to the logger.
    /// </summary>
    /// <todo>
    /// This should defer logging tasks to log appenders rather than maintain a sync-list of items.
    /// </todo>
    public class Logger
    {
        public static readonly Color Color_GoodNews = Color.YellowGreen;
        public static readonly Color Color_Info = Color.White;
        public static readonly Color Color_Warning = Color.Orange;
        public static readonly Color Color_Error = Color.OrangeRed;

        public static System.ComponentModel.ISynchronizeInvoke SynchonizeInvoke { get; set; }

        public SyncList<LogItem> LogList { get; set; }
        public SyncList<LogItem> VisibleList { get; set; }

        bool _ShowGoodNews = true;
        bool _ShowInfo = true;
        bool _ShowWarnings = true;
        bool _ShowErrors = true;

        public bool ShowGoodNews { get { return _ShowGoodNews; } set { lock (this) { _ShowGoodNews = value; VisibleSetDirty = true; } } }
        public bool ShowInfo { get { return _ShowInfo; } set { lock (this) { _ShowInfo = value; VisibleSetDirty = true; } } }
        public bool ShowWarnings { get { return _ShowWarnings; } set { lock (this) { _ShowWarnings = value; VisibleSetDirty = true; } } }
        public bool ShowErrors { get { return _ShowErrors; } set { lock (this) { _ShowErrors = value; VisibleSetDirty = true; } } }

        private bool VisibleSetDirty { get; set; }

        /// <summary>
        /// Returns the color associated with the given severity.
        /// </summary>
        /// <param name="severity">Log severity as per LogSeverity.</param>
        /// <returns>The color associated with the given severity.</returns>
        /// <todo>
        /// This should not be defined in the logger itself.
        /// </todo>
        public static Color FindColorForSeverity(string severity)
        {
            if (severity.Equals("GoodNews"))
            {
                return Color_GoodNews;
            }
            else if (severity.Equals("Info"))
            {
                return Color_Info;
            }
            else if (severity.Equals("Warning"))
            {
                return Color_Warning;
            }
            else
            {
                System.Diagnostics.Debug.Assert(severity.Equals("Error"));
                return Color_Error;
            }
        }

        public Logger()
        {
            LogList = new SyncList<LogItem>(SynchonizeInvoke);
            VisibleList = new SyncList<LogItem>(SynchonizeInvoke);
        }

        /// <summary>
        /// Adds a new log item to the log.
        /// </summary>
        /// <param name="severity">The severity of the log message.</param>
        /// <param name="message">The description of this log item.</param>
        public void Log(LogSeverity severity, string message)
        {
            LogItem logItem = new LogItem(severity, message);

            lock (this)
            {
                LogList.Add(logItem);

                if (ShouldShow(severity))
                {
                    VisibleList.Add(logItem);
                }
            }

            // Write warnings (or worse) to the console.
            if (severity >= LogSeverity.Warning)
            {
                Console.WriteLine("");
                Console.WriteLine(logItem.ToString());
            }
        }

        public void SetAndUpdateVisibleSet(bool showGoodNews, bool showInfo, bool showWarnings, bool showErrors)
        {
            lock (this)
            {
                _ShowGoodNews = showGoodNews;
                _ShowInfo = showInfo;
                _ShowWarnings = showWarnings;
                _ShowErrors = showErrors;

                VisibleSetDirty = true;

                UpdateVisible();
            }
        }
        
        /// <summary>
        /// Updates the currently displayed log items if there are changes to the visible list.
        /// </summary>
        public void UpdateVisible()
        {
            if (VisibleSetDirty)
            {
                lock (this)
                {
                    if (VisibleSetDirty)
                    {
                        //% Using Linq Select may be quicker here?
                        VisibleList.Clear();

                        foreach (LogItem logItem in LogList)
                        {
                            if (ShouldShow(logItem.Severity))
                            {
                                VisibleList.Add(logItem);
                            }
                        }

                        VisibleSetDirty = false;
                    }
                }
            }
        }

        bool ShouldShow(LogSeverity severity)
        {
            return ((severity == LogSeverity.Info) && ShowInfo) ||
                    ((severity == LogSeverity.GoodNews) && ShowGoodNews) ||
                    ((severity == LogSeverity.Warning) && ShowWarnings) ||
                    ((severity == LogSeverity.Error) && ShowErrors);
        }

        public void WriteToStream(Stream stream)
        {
            StreamWriter writer = new StreamWriter(stream);

            lock (this)
            {
                foreach (LogItem logItem in LogList)
                {
                    writer.WriteLine(logItem.ToString());
                }
            }

            writer.Close();
        }
    }
}
