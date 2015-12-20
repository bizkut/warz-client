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

namespace NaturalMotion.AssetExporter.CommandLine.Utils
{
    /// <summary>
    /// A utility class that holds a name and a list of parameters.
    /// </summary>
    public class NamedParameterList
    {
        public string Name { get; set; }
        public string[] Parameters { get; set; }

        public NamedParameterList(string name)
        {
            Name = name;
        }
    }
}
