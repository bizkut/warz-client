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
    /// A single command option and it's parameters.
    /// </summary>
    public class Option : Utils.NamedParameterList
    {
        public OptionTemplate Template { get; set; }

        public Option(string name) : base(name)
        {
        }

        public void Invoke()
        {
            Template.Action(this);
        }
    }
}
