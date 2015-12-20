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
using System.Linq;
using System.Xml.Serialization;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter
{
    /// <summary>
    /// Ensures that the currently selected morpheme version stays in sync with the macro system, UI, and other configuration.
    /// </summary>
    public class MorphemeVersionSelector : INotifyPropertyChanged
    {
        public class MorphemeVersion
        {
            public MorphemeVersion()
            {
                AppInstall = string.Empty;
                AppExe = string.Empty;
                AppSamples = string.Empty;
                AppLocalSettings = string.Empty;
                AppRoamingSettings = string.Empty;
                MajorVersion = 0;
                MinorVersion = 0;
            }

            public string AppInstall { get; set; }
            public string AppExe { get; set; }
            public string AppSamples { get; set; }
            public string AppLocalSettings { get; set; }
            public string AppRoamingSettings { get; set; }
            public int MajorVersion { get; set; }
            public int MinorVersion { get; set; }
        }

        public static MorphemeVersionSelector GetInstance()
        {
            if (Instance == null)
            {
                Instance = new MorphemeVersionSelector();
                if (Instance.VersionDictionary.Keys.Count != 0)
                {
                    Instance.SelectedVersionKey = Instance.VersionDictionary.Keys.First();
                }
            }

            return Instance;
        }

        [XmlIgnore]
        public static Logger OutputLog { private get; set; }
        
        public const string CustomStr = "Custom";
        
        private static MorphemeVersionSelector Instance = null;

        private MorphemeVersionSelector()
        {
            VersionDictionary = new Dictionary<string, MorphemeVersion>();
            SelectedVersionKey = string.Empty;

            PopulateVersionDictionary();
        }

        private void PopulateVersionDictionary()
        {
            try
            {
                // use the registry tool to pull out the installed versions of connect
                RegistryTool.Version[] versions = RegistryTool.RegistryTool.GetVersionNames();

                foreach (RegistryTool.Version version in versions)
                {
                    MorphemeVersion morphemeVersion = new MorphemeVersion();
                    morphemeVersion.AppInstall = version.InstallPath;
                    morphemeVersion.AppExe = version.FullInstallPath;
                    morphemeVersion.AppSamples = version.SamplesPath;
                    morphemeVersion.AppLocalSettings = version.LocalAppPath;
                    morphemeVersion.AppRoamingSettings = string.Empty; // TODO

                    if (!version.DevVersion)
                    {
                        morphemeVersion.MajorVersion = version.Major;
                        morphemeVersion.MinorVersion = version.Minor;
                    }

                    VersionDictionary.Add(version.Name, morphemeVersion);
                }

                VersionDictionary.Add(CustomStr, new MorphemeVersion());
            }
            catch (System.Exception ex)
            {
                OutputLog.Log(LogSeverity.Error, ex.ToString());
            }
        }

        public string GetCurrentValue(MacrosSystem.ReservedVersionMacroKeys key)
        {

            if (string.IsNullOrEmpty(SelectedVersionKey))
            {
                return string.Empty;
            }
            // based on the key, get the value for the currently selected connect version
            switch (key)
            {
                case MacrosSystem.ReservedVersionMacroKeys.APP_INSTALL_PATH:
                    return VersionDictionary[SelectedVersionKey].AppInstall;

                case MacrosSystem.ReservedVersionMacroKeys.APP_INSTALL_FULL_PATH:
                    return VersionDictionary[SelectedVersionKey].AppExe;

                case MacrosSystem.ReservedVersionMacroKeys.APP_SAMPLES_PATH:
                    return VersionDictionary[SelectedVersionKey].AppSamples;

                case MacrosSystem.ReservedVersionMacroKeys.APP_LOCAL_SETTINGS_PATH:
                    return VersionDictionary[SelectedVersionKey].AppLocalSettings;

                case MacrosSystem.ReservedVersionMacroKeys.APP_ROAMING_SETTINGS_PATH:
                    return VersionDictionary[SelectedVersionKey].AppRoamingSettings;

                default:
                    OutputLog.Log(LogSeverity.Error, "cannot find case for switch value " + key.ToString());
                    return string.Empty;
            }
        }

        public void SetCustomValue(MacrosSystem.ReservedVersionMacroKeys key, string value)
        {
            // based on the key, set the value for the currently selected connect version
            switch (key)
            {
                case MacrosSystem.ReservedVersionMacroKeys.APP_INSTALL_PATH:
                    VersionDictionary[CustomStr].AppInstall = value;
                    break;

                case MacrosSystem.ReservedVersionMacroKeys.APP_INSTALL_FULL_PATH:
                    VersionDictionary[CustomStr].AppExe = value;
                    break;

                case MacrosSystem.ReservedVersionMacroKeys.APP_SAMPLES_PATH:
                    VersionDictionary[CustomStr].AppSamples = value;
                    break;

                case MacrosSystem.ReservedVersionMacroKeys.APP_LOCAL_SETTINGS_PATH:
                    VersionDictionary[CustomStr].AppLocalSettings = value;
                    break;

                case MacrosSystem.ReservedVersionMacroKeys.APP_ROAMING_SETTINGS_PATH:
                    VersionDictionary[CustomStr].AppRoamingSettings = value;
                    break;

                default:
                    OutputLog.Log(LogSeverity.Error, "cannot find case for switch value " + key.ToString());
                    break;
            }

            MacrosSystem.GetInstance().BuildDictionary();
        }

        public MorphemeVersion SelectedVersion
        {
            get { return VersionDictionary[SelectedVersionKey]; }
        }

        string selectedVersionKey = string.Empty;
        public string SelectedVersionKey
        {
            get 
            {
                return selectedVersionKey;
            }
            set
            {
                if (value != selectedVersionKey)
                {
                    selectedVersionKey = value;

                    // rebuild the macros so that, for example, $(APP_INSTALL_FULL_PATH) points to the correct connect version
                    MacrosSystem.GetInstance().BuildDictionary();

                    // update the settings for the connect path
                    string exeName = MorphemeVersionSelector.GetInstance().GetCurrentValue(MacrosSystem.ReservedVersionMacroKeys.APP_INSTALL_FULL_PATH);

                    Settings.GetInstance().MorphemeConnectPath = exeName;

                    NotifyPropertyChanged("SelectedVersionKey");
                }
            }
        }

        public void CopyFrom(MorphemeVersionSelector morphemeVersionSelector)
        {
            string versionSelected = morphemeVersionSelector.SelectedVersionKey;
            if (versionSelected == null || versionSelected.Length == 0)
            {
                // Choose the latest available (not counting 'custom').
                // There should be the custom version + at least another one. Take the one with the 'greatest' name.
                // This is not the most robust solution but it allows to test dev installers.
                versionSelected = "";
                if (morphemeVersionSelector.VersionDictionary.Count >= 2)
                {   
                    foreach (var pair in morphemeVersionSelector.VersionDictionary)
                    {
                        if (pair.Key.CompareTo(versionSelected) > 0)
                        {
                            versionSelected = pair.Key;
                        }
                    }
                }
                else
                {
                    // Fall back on the custom one.
                    versionSelected = CustomStr;
                }              
            }

            SelectedVersionKey = versionSelected;

            VersionDictionary[CustomStr] = morphemeVersionSelector.CustomVersionReference;    
        }

        [XmlIgnore]
        public Dictionary<string, MorphemeVersion> VersionDictionary  { get; private set; }

        // custom KeyValuePair for serialisation
        public struct KeyValuePair<K, V>
        {
            public K Key { get; set; }
            public V Value { get; set; }
        }

        public void BuildCustomVersionForSerialisation()
        {
            // called just before saving out, so that the correct custom setting is saved
            CustomVersionReference = VersionDictionary[CustomStr];
        }

        // a reference to the custom version require for xml serialisation
        [XmlElement("custom-config")]
        public MorphemeVersion CustomVersionReference { get; set; }

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(String info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }
    }
}
