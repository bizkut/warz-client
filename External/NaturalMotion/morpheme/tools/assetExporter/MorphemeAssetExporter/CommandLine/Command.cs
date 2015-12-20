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
    /// A single command and it's parameters.
    /// </summary>
    public class Command : Utils.NamedParameterList
    {
        public OptionCollection Options { get; set; }
        public CommandTemplate Template { get; set; }

        public Command(string name) : base(name)
        {
            Options = new OptionCollection();
        }

        public void Invoke(bool invokeOptions)
        {
            if (invokeOptions)
            {
                foreach (Option option in Options)
                {
                    option.Invoke();
                }
            }

            Template.Action(this);
        }
    }
}
