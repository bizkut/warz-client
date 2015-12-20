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

namespace NaturalMotion.AssetExporter.CommandLine
{
    /// <summary>
    /// A template describing a command.
    /// </summary>
    public class CommandTemplate
    {
        public string Description { get; set; }
        public string ValidOptions { get; set; }
        public CommandAction Action { get; set; }

        public CommandTemplate(string description, string validOptions, CommandAction action)
        {
            Description = description;
            ValidOptions = validOptions;
            Action = action;
        }
    }
}
