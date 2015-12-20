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

namespace NaturalMotion.AssetExporter.Logging
{
    /// <summary>
    /// An individual log item consisting of a message and associated severity.
    /// </summary>
    public class LogItem
    {
        public LogSeverity Severity { get; set; }
        public string Message { get; set; }

        public LogItem(LogSeverity severity, string message)
        {
            Severity = severity;
            Message = message;
        }

        public new string ToString()
        {
            return Severity.ToString() + ": " + Message;
        }
    }

}
