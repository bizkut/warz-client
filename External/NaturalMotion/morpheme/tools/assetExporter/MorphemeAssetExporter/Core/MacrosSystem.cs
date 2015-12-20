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
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Xml.Serialization;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter
{
    /// <summary>
    /// Stores reserved, pre-defined, and user customized macros used in string substitution throughout the asset exporter.
    /// </summary>
    public class MacrosSystem
    {
        // reserved macro keys

        // ReservedNetworkMacroKeys 
        // in the asset compiler section, each networks mcp path can be referenced simply by MCP_PATH
        public enum ReservedNetworkMacroKeys
        {
            MCP_PATH,
            MCN_NAME,
            MCN_PATH,
            MCN_DIR,
        };

        public static readonly string [] ReservedNetworkMacroDescriptions = new string[]
        {
            "The full directory path of the mcn.",
            "The name of the mcn without the .mcn extension.",
            "The full directory path of the parent directory of the mcn.",
            "The base name of the parent directory of the mcn.",
        };

        public enum ReservedVersionMacroKeys
        {
            APP_INSTALL_PATH,
            APP_INSTALL_FULL_PATH, // e.g. C:\Program Files (x86)\NaturalMotion\morphemeConnect 2.3.3\morphemeConnect.exe
            APP_SAMPLES_PATH,
            APP_LOCAL_SETTINGS_PATH,
            APP_ROAMING_SETTINGS_PATH,
        };

        static readonly string Prefix = "$(";
        static readonly string Suffix = ")";

        public class Macro
        {
            public Macro()
            {
                Key = string.Empty;
                Value = string.Empty;
            }

            public Macro(string key, string value)
            {
                Key = key;
                Value = value;
            }

            public string Key { get; set; }
            public string Value { get; set; }
        }

        [XmlArray("macros")]
        [XmlArrayItem("macro")]
        public BindingList<Macro> MacroStringCustomList { get; set; }

        [XmlIgnore]
        List<Macro> MacroStringCustomListBackup { get; set; }

        [XmlIgnore]
        public BindingList<Macro> MacroStringSystemList { get; set; }

        [XmlIgnore]
        Dictionary<string, string> MacroStringDictionary { get; set; }

        [XmlIgnore]
        public static Logger OutputLog { set; private get; }

        static MacrosSystem Instance = null;

        public static MacrosSystem GetInstance()
        {
            if (Instance == null)
            {
                Instance = new MacrosSystem();
            }

            return Instance;
        }

        public void BuildDictionary()
        {
            MacroStringDictionary.Clear();
            MacroStringSystemList.Clear();

            foreach (ReservedVersionMacroKeys versionKey in Enum.GetValues(typeof(ReservedVersionMacroKeys)))
            {
                string key = Enum.GetName(typeof(ReservedVersionMacroKeys), versionKey);
                string value = MorphemeVersionSelector.GetInstance().GetCurrentValue(versionKey);

                if (string.IsNullOrEmpty(key) ||
                    string.IsNullOrEmpty(value))
                {
                    continue;
                }

                if (MacroStringDictionary.ContainsKey(Prefix + key + Suffix))
                {
                    OutputLog.Log(LogSeverity.Warning, "skipping duplicate macro key: " + key);
                    continue;
                }

                MacroStringDictionary.Add(Prefix + key + Suffix, value);
                MacroStringSystemList.Add(new Macro(key, value));
            }

            foreach (Environment.SpecialFolder specialFolder in Enum.GetValues(typeof(Environment.SpecialFolder)))
            {
                // only add certain SpecialFolders
                if (specialFolder != Environment.SpecialFolder.ApplicationData &&
                    specialFolder != Environment.SpecialFolder.CommonApplicationData &&
                    specialFolder != Environment.SpecialFolder.CommonProgramFiles &&
                    specialFolder != Environment.SpecialFolder.Desktop &&
                    specialFolder != Environment.SpecialFolder.LocalApplicationData &&
                    specialFolder != Environment.SpecialFolder.MyComputer &&
                    specialFolder != Environment.SpecialFolder.MyDocuments &&
                    specialFolder != Environment.SpecialFolder.ProgramFiles &&
                    specialFolder != Environment.SpecialFolder.ProgramFilesX86 &&
                    specialFolder != Environment.SpecialFolder.Programs &&
                    specialFolder != Environment.SpecialFolder.System)
                {
                    continue;
                }

                string key = Enum.GetName(typeof(Environment.SpecialFolder), specialFolder);

                var duplicates = from pair in MacroStringSystemList.ToList() // (binding list needs to be converted to list)
                                 where (pair.Key != null && pair.Key.Equals(key, StringComparison.CurrentCultureIgnoreCase))
                                 select pair;

                if (duplicates.Count() > 0)
                {
                    // only supports one value per key (a 1-1 mapping)
                    continue;
                }

                string folderPath = Environment.GetFolderPath(specialFolder);

                if (string.IsNullOrEmpty(folderPath))
                {
                    continue;
                }

                MacroStringDictionary.Add(Prefix + key + Suffix, folderPath);
                MacroStringSystemList.Add(new Macro(key, folderPath));
            }

            // lastly add the custom macros and do not allow duplicates of system macros added above

            foreach (Macro pair in MacroStringCustomList)
            {
                if (string.IsNullOrEmpty(pair.Key) ||
                    string.IsNullOrEmpty(pair.Value))
                {
                    continue;
                }

                if (pair.Key.Equals(pair.Value, StringComparison.CurrentCultureIgnoreCase))
                {
                    // avoid infinite recursion
                    OutputLog.Log(LogSeverity.Warning, "skipping macro with equal key and value");
                    continue;
                }

                if (MacroStringDictionary.ContainsKey(Prefix + pair.Key + Suffix))
                {
                    OutputLog.Log(LogSeverity.Warning, "skipping duplicate macro key: " + pair.Key);
                    continue;
                }

                MacroStringDictionary.Add(Prefix + pair.Key + Suffix, pair.Value);
            }

            SortDictionary();
        }

        public void BackUpCustomList()
        {
            MacroStringCustomListBackup = new List<Macro>(MacroStringCustomList);
        }

        public void RevertCustomList()
        {
            MacroStringCustomList = new BindingList<Macro>(MacroStringCustomListBackup);
        }


        MacrosSystem()
        {
            MacroStringDictionary = new Dictionary<string, string>();
            MacroStringCustomList = new BindingList<Macro>();
            MacroStringSystemList = new BindingList<Macro>();
            MacroStringCustomListBackup = new List<Macro>(MacroStringCustomList);
        }


        void SortDictionary()
        {
            // we want to replace the longest substrings first so sort the dictionary by the value length
            var sortedDict = (from entry in MacroStringDictionary orderby entry.Value.Length descending select entry);

            MacroStringDictionary = sortedDict.ToDictionary(pair => pair.Key, pair => pair.Value);
        }

        public void AddReservedString(ReservedNetworkMacroKeys reservedKey, string value)
        {
            string keyOriginal = Prefix + Enum.GetName(typeof(ReservedNetworkMacroKeys), reservedKey);
            int counter = 1;

            string key = keyOriginal + "_" + counter.ToString() + Suffix;

            while (true)
            {
                 if (MacroStringDictionary.ContainsKey(key) &&
                     MacroStringDictionary[key].Equals(value, StringComparison.CurrentCultureIgnoreCase))
                {
                    // already have this key with this value
                    // nothing more to do
                    return;
                }

                if (MacroStringDictionary.ContainsKey(key) == false)
                {
                    // don't have this key so add it
                    MacroStringDictionary.Add(key, value);

                    // ensure longest strings are replaced first
                    SortDictionary();

                    return;
                }

                // generate the next key and try again
                key = keyOriginal + "_" + counter.ToString() + Suffix;

                counter++;
            } 
        }


        public string ConvertToMacro(string str)
        {
            return ConvertToMacroIgnoringReservedKey(str, null);
        }

        public string ConvertToMacroIgnoringReservedKey(string str, List<ReservedVersionMacroKeys> ignoreList)
        {
            // replace strings with their macros
            // e.g. "3.0.4" -> $(MORPHEME_VERSION)

            if (string.IsNullOrEmpty(str))
            {
                return str;
            }

            str = str.Replace("/", @"\");

            foreach (KeyValuePair<string, string> dictValue in MacroStringDictionary)
            {
                // skip over the reserved macro to be ignored
                if (ignoreList != null)
                {
                    bool ok = true;
                    foreach (ReservedVersionMacroKeys ignore in ignoreList)
                    {
                        string reservedMacroAsKey = Prefix + Enum.GetName(typeof(ReservedVersionMacroKeys), ignore) + Suffix;
                        if (reservedMacroAsKey.Equals(dictValue.Key, StringComparison.CurrentCultureIgnoreCase))
                        {
                            ok = false;
                            break;
                        }
                    }

                    if (!ok)
                    {
                        continue;
                    }
                }

                var regex = new Regex(Regex.Escape(dictValue.Value), RegexOptions.IgnoreCase);
                str = regex.Replace(str, dictValue.Key);
            }

            return str;
        }

        public string ExpandMacro(string str)
        {
            // replace macros with their strings
            // e.g. $(MORPHEME_VERSION) -> "3.0.4"

            if (string.IsNullOrEmpty(str))
            {
                return string.Empty;
            }

            // keep track of the original string. 
            string originalString = str;

            // replace all macros with their values
            foreach (KeyValuePair<string, string> dictValue in MacroStringDictionary)
            {
                var regex = new Regex(Regex.Escape(dictValue.Key), RegexOptions.IgnoreCase);
                str = regex.Replace(str, dictValue.Value);
            }

            // the replacement string is now rescanned for further macro expansion
            if (str.Contains(Prefix) &&
                false == str.Equals(originalString, StringComparison.CurrentCultureIgnoreCase))
            {
                str = ExpandMacro(str);
            }

            return str;
        }


        public string ExpandMacroUsingNetwork(string str, MCN network)
        {
            // replace strings with their macros, using the network context if required.

            if (string.IsNullOrEmpty(str))
            {
                return string.Empty;
            }

            str = ExpandMacro(str);

            foreach (ReservedNetworkMacroKeys macro in Enum.GetValues(typeof(ReservedNetworkMacroKeys)))
            {
                string macroString = Prefix + Enum.GetName(typeof(ReservedNetworkMacroKeys), macro) + Suffix;

                var regex = new Regex(Regex.Escape(macroString), RegexOptions.IgnoreCase);

                if (regex.IsMatch(str))
                {
                    switch (macro)
                    {
                        case ReservedNetworkMacroKeys.MCP_PATH:
                            {
                                FileInfo fileInfo = new FileInfo(network.GetMCPPathAbsoluteExpanded());
                                str = regex.Replace(str, fileInfo.DirectoryName);
                                break;
                            }
                        case ReservedNetworkMacroKeys.MCN_NAME:
                            {
                                FileInfo fileInfo = new FileInfo(network.GetMCNPathAbsoluteExpanded());
                                str = regex.Replace(str, Path.GetFileNameWithoutExtension(fileInfo.Name));
                                break;
                            }
                        case ReservedNetworkMacroKeys.MCN_PATH:
                            {
                              FileInfo fileInfo = new FileInfo(network.GetMCNPathAbsoluteExpanded());
                              str = fileInfo.Directory.FullName;
                              break;
                            }
                        case ReservedNetworkMacroKeys.MCN_DIR:
                            {
                                FileInfo fileInfo = new FileInfo(network.GetMCNPathAbsoluteExpanded());
                                str = regex.Replace(str, fileInfo.Directory.Name);
                                break;
                            }
                    }
                }
            }

            return str;
        }

        public void CopyFrom(MacrosSystem macroSystem)
        {
            MacroStringDictionary.Clear();
            MacroStringCustomList = new BindingList<Macro>(macroSystem.MacroStringCustomList);
            MacroStringSystemList.Clear();
            MacroStringCustomListBackup.Clear();

            BuildDictionary();
        }

        public bool IsCustomMacroValid(string macro)
        {
            {
                var duplicates = from pair in MacroStringCustomList.ToList() // (binding list needs to be converted to list)
                                 where (pair.Key != null && pair.Key.Equals(macro, StringComparison.CurrentCultureIgnoreCase))
                                 select pair;

                if (duplicates.Count() > 1)
                {
                    /// already have this macro
                    OutputLog.Log(LogSeverity.Warning, "duplicate macro will be ignored: " + macro);
                    return false;
                }
            }

            {
                var duplicates = from pair in MacroStringSystemList.ToList() // (binding list needs to be converted to list)
                                 where (pair.Key != null && pair.Key.Equals(macro, StringComparison.CurrentCultureIgnoreCase))
                                 select pair;

                if (duplicates.Count() > 0)
                {
                    /// cannot use a system macro
                    OutputLog.Log(LogSeverity.Warning, "system macro will be ignored: " + macro);
                    return false;
                }
            }


            foreach (ReservedNetworkMacroKeys key in Enum.GetValues(typeof(ReservedNetworkMacroKeys)))
            {
                string keyStr = Enum.GetName(typeof(ReservedNetworkMacroKeys), key);

                if (keyStr.Equals(macro, StringComparison.CurrentCultureIgnoreCase))
                {
                    // cannot use a context reserved macro
                    OutputLog.Log(LogSeverity.Warning, "context macro will be ignored: " + macro);
                    return false;
                }
            }

            return true;
        }
    }
}
