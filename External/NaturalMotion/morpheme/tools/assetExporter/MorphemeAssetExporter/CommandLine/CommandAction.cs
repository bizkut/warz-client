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
    /// The action to take if a specific command is present.
    /// </summary>
    /// <param name="option">The command the action was invoked for.</param>
    public delegate void CommandAction(Command command);

}
