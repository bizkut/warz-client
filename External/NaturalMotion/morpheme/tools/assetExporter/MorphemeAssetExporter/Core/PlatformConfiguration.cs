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

using System.Xml.Serialization;

namespace NaturalMotion.AssetExporter
{
    /// <summary>
    /// Provides additional configuration information about the platform to export for.
    /// </summary>
    public class PlatformConfiguration
    {
        public enum Targets
        {
            pc,
            x360,
            ps3,
            wii,
            vita,
            ios,
            android,
            wiiu
        }

        public PlatformConfiguration()
        {
            UseDuringExport = true;
        }

        /// <summary>
        /// Is this platform configuration used during the next export?
        /// </summary>
        [XmlIgnore]
        public bool UseDuringExport { get; set; }

#if DEV_ENV_SUPPORT
        /// <summary>
        /// The name of this platform configuration.
        /// </summary>
        [XmlElement("platform-name")]
        public string Name { get; set; }
#endif

        /// <summary>
        /// The export location relative to the shared export root to use for this platform configuration.
        /// </summary>
        [XmlElement("process-path")]
        public string ProcessPathSubDirectory { get; set; }

        /// <summary>
        /// If specified, this is the asset compiler executable name, which is used instead of trying to extrapolate
        /// the name from the platform settings.
        /// </summary>
        [XmlElement("ac-customexe")]
        public string AssetCompilerCustomEXE { get; set; }

        /// <summary>
        /// Is this platform using kinect? For versions older than 3.6 this property is not relevant as
        /// the asset compiler is fully defined by it's executable.
        /// </summary>
        [XmlElement("ac-kinect")]
        public bool UseKinect { get; set; }

        /// <summary>
        /// Is this platform using move? For versions older than 3.6 this property is not relevant as
        /// the asset compiler is fully defined by it's executable.
        /// </summary>
        [XmlElement("ac-move")]
        public bool UseMove { get; set; }

#if DEV_ENV_SUPPORT

        /// <summary>
        /// The solution required to build the Asset Compiler for this platform configuration.
        /// </summary>
        [XmlElement("ac-build-solution")]
        public string SlnPlatform { get; set; }

        /// <summary>
        /// The configuration to use out of the selected Asset Compiler solution for this platform configuration.
        /// </summary>
        [XmlElement("ac-build-configuration")]
        public string ConfigFileBox { get; set; }
#endif
    }
}
