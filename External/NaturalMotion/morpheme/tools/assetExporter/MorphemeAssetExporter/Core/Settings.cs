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
using System.Collections.Generic;
using System.ComponentModel;
using System.Xml.Serialization;

namespace NaturalMotion.AssetExporter
{
    public class Settings : INotifyPropertyChanged
    {
        public static Settings GetInstance()
        {
            if (Instance == null)
            {
                Instance = new Settings();
            }

            return Instance;
        }

        public void CopyFrom(Settings settings)
        {
            // cannot overwrite the object (due to binding) so copy over the data
            MorphemeConnectPath = settings.MorphemeConnectPath;
            RuntimeSDKRoot = settings.RuntimeSDKRoot;
            ExportRootPath = settings.ExportRootPath;
            AssetCompilerCommandLine = settings.AssetCompilerCommandLine;
            PreProcessCommand = settings.PreProcessCommand;
            PostProcessCommand = settings.PostProcessCommand;
            UseDynamicAssetCompilerPlugins = settings.UseDynamicAssetCompilerPlugins;

            Platforms.Clear();

            foreach (PlatformConfiguration platform in settings.Platforms)
            {
                platform.UseDuringExport = true; // the flag to use this platform is not part of the config, default to true

                Platforms.Add(platform);
            }
        }

        readonly List<MacrosSystem.ReservedVersionMacroKeys> MacrosToIgnoreInConnectPath = new List<MacrosSystem.ReservedVersionMacroKeys>()
        {
            MacrosSystem.ReservedVersionMacroKeys.APP_INSTALL_FULL_PATH,
            MacrosSystem.ReservedVersionMacroKeys.APP_INSTALL_PATH,
        };

        string morphemeConnectPath = string.Empty;
        public string MorphemeConnectPath
        {
            get
            {
                // we want to show the actual path so don't convert to the macros that make up most of this path
                return MacrosSystem.GetInstance().ConvertToMacroIgnoringReservedKey(morphemeConnectPath, MacrosToIgnoreInConnectPath);
            }
            set
            {
                if (value != this.morphemeConnectPath)
                {
                    this.morphemeConnectPath = value;
                    NotifyPropertyChanged("MorphemeConnectPath");
                }
            }
        }

        string runtimeSDKRoot = string.Empty;
        public string RuntimeSDKRoot
        {
            get
            {
                return MacrosSystem.GetInstance().ConvertToMacro(runtimeSDKRoot);
            }
            set
            {
                if (value != this.runtimeSDKRoot)
                {
                    this.runtimeSDKRoot = value;
                    NotifyPropertyChanged("RuntimeSDKRoot");
                }
            }
        }

        string exportRootPath = string.Empty;
        public string ExportRootPath
        {
            get
            {
                return MacrosSystem.GetInstance().ConvertToMacro(exportRootPath);
            }
            set
            {
                if (value != this.exportRootPath)
                {
                    this.exportRootPath = value;
                    NotifyPropertyChanged("ExportRootPath");
                }
            }
        }

        bool useDynamicAssetCompilerPlugins = true;
        public bool UseDynamicAssetCompilerPlugins
        {
            get
            {
                return useDynamicAssetCompilerPlugins;
            }
            set
            {
                if (value != this.useDynamicAssetCompilerPlugins)
                {
                    this.useDynamicAssetCompilerPlugins = value;
                    NotifyPropertyChanged("UseDynamicAssetCompilerPlugins");
                }
            }
        }

        string assetCompilerCommandLine = ""; // the default command line
        public string AssetCompilerCommandLine
        {
            get
            {
                return assetCompilerCommandLine;
            }
            set
            {
                if (value != this.assetCompilerCommandLine)
                {
                    this.assetCompilerCommandLine = value;
                    NotifyPropertyChanged("AssetCompilerCommandLine");
                }
            }
        }


        string preProcessCommand = string.Empty;
        public string PreProcessCommand // not currently supported
        {
            get
            {
                return preProcessCommand;
            }
            set
            {
                if (value != this.preProcessCommand)
                {
                    this.preProcessCommand = value;
                    NotifyPropertyChanged("PreProcessCommand");
                }
            }
        }

        string postProcessCommand = string.Empty;
        public string PostProcessCommand // not currently supported
        {
            get
            {
                return postProcessCommand;
            }
            set
            {
                if (value != this.postProcessCommand)
                {
                    this.postProcessCommand = value;
                    NotifyPropertyChanged("PostProcessCommand");
                }
            }
        }

        [XmlArray("platforms")]
        [XmlArrayItem("platform")]
        public BindingList<PlatformConfiguration> Platforms { get; set; }

        public int NumSelectedPlatforms()
        {
            int numSelectedPlatforms = 0;
            foreach (PlatformConfiguration platform in Platforms)
            {
                if (platform.UseDuringExport)
                {
                    ++numSelectedPlatforms;
                }
            }

            return numSelectedPlatforms;
        }

        // event handler
        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(String info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }

        private Settings()
        {
            Platforms = new BindingList<PlatformConfiguration>();
        }

        private static Settings Instance = null;
    }
}
