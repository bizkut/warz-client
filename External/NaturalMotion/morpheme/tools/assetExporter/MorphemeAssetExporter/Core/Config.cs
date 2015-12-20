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

using System.IO;
using System.Xml.Serialization;

namespace NaturalMotion.AssetExporter
{
    /// <summary>
    /// Stores asset exporter configuration information.
    /// </summary>
    public class Config
    {
        public Config()
        {
            settings = Settings.GetInstance();
            macro = MacrosSystem.GetInstance();
            morphemeVersionSelector = MorphemeVersionSelector.GetInstance();
        }

        public void Serialise(Stream stream)
        {
            // special case code to serialise out just the custom config
            morphemeVersionSelector.BuildCustomVersionForSerialisation();

            XmlSerializer serializer = new XmlSerializer(typeof(Config));
            serializer.Serialize(stream, this);
        }

        public void Deserialise(Stream stream)
        {
            XmlSerializer serializer = new XmlSerializer(typeof(Config));
            Config config = serializer.Deserialize(stream) as Config;
            
            macro.CopyFrom(config.macro); // import macros first

            morphemeVersionSelector.CopyFrom(config.morphemeVersionSelector);

            settings.CopyFrom(config.settings);
        }

        public Settings settings { get; set; }
        public MacrosSystem macro { get; set; }
        public MorphemeVersionSelector morphemeVersionSelector { get; set; }
    }
}
