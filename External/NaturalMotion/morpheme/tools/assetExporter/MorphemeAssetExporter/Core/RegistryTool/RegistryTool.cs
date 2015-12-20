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
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.Win32;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter.RegistryTool
{
    /// <summary>
    /// Reads installed morpheme versions from the Windows registry.
    /// </summary>
    public class RegistryTool
    {
        public static Logger OutputLog { private get; set; }

        public const string Wow64SubKeyName = @"SOFTWARE\WoW6432Node\NaturalMotion";
        public const string DefaultSubKeyName = @"SOFTWARE\NaturalMotion";
        
        /// <summary>
        /// Opens the NaturalMotion registry key under the local machine hive.
        /// </summary>
        /// <returns> The NaturalMotion registry key, or null if it does not exist.</returns>
        public static RegistryKey GetLocalMachineRootKey()
        {
            RegistryKey rootKey = Registry.LocalMachine.OpenSubKey(Wow64SubKeyName);
            if (rootKey == null)
            {
                rootKey = Registry.LocalMachine.OpenSubKey(DefaultSubKeyName);
            }

            return rootKey;
        }

        /// <summary>
        /// Opens the NaturalMotion registry key under the current user hive.
        /// </summary>
        /// <returns> The NaturalMotion registry key. </returns>
        public static RegistryKey GetCurrentUserRootKey()
        {
            RegistryKey rootKey = Registry.CurrentUser.OpenSubKey(DefaultSubKeyName);
            if (rootKey == null)
            {
                throw new Exception("Failed to identify root key!");
            }

            return rootKey;
        }
        
        /// <summary>
        /// Enumerates morpheme version information stored in the registry.
        /// </summary>
        /// <returns> Version information for each morpheme instance found in the registry. </returns>
        static public Version[] GetVersionNames()
        {
            List<Version> versions = new List<Version>();

            // Find shared documents folder (try PUBLIC first then fall back to ALLUSERSPROFILE).
            string sharedDocumentsPath = Environment.GetEnvironmentVariable("PUBLIC");
            if ((sharedDocumentsPath == null) || (sharedDocumentsPath.Length == 0))
            {
                sharedDocumentsPath = Environment.GetEnvironmentVariable("ALLUSERSPROFILE");
            }

            if ((sharedDocumentsPath == null) || (sharedDocumentsPath.Length == 0))
            {
                sharedDocumentsPath = string.Empty;
                OutputLog.Log(LogSeverity.Error, "Failed to identify shared documents path!");
            }

            using (RegistryKey rootKey = GetLocalMachineRootKey())
            {
                if (rootKey != null)
                {
                    foreach (string versionName in rootKey.GetSubKeyNames())
                    {
                        try
                        {
                            Version version = new Version();
                            bool ignoreVersion = false;

                            // Identify product.
                            string versionNameLowerCase = versionName.ToLower();
                            
                            if (versionNameLowerCase.StartsWith("morpheme"))
                            {
                                version.Product = "Morpheme";
                            }
                            else if (versionNameLowerCase.StartsWith("euphoria"))
                            {
                                version.Product = "Euphoria";
                            }
                            else
                            {
                                ignoreVersion = true;
                            }

                            // Skip other products.
                            if (ignoreVersion)
                            {
                                OutputLog.Log(LogSeverity.Info, string.Format("Skipped product with registry name {0}.", versionName));
                                continue;
                            }
                            
                            // Extract version numbers. Major/minor version are always expected to be present, release
                            // version is either the version number or '_' or '-' (to allow identifying versions without
                            // release part later).
                            Match versionMatch = Regex.Match(versionName, @"[_-](?'major'\d+)\.(?'minor'\d+)\.?(?'release'\d*)(?:[_-]win32)?", RegexOptions.IgnoreCase);

                            if (versionMatch.Success)
                            {
                              version.Major = int.Parse(versionMatch.Groups["major"].Value);
                              version.Minor = int.Parse(versionMatch.Groups["minor"].Value);
                              version.Release = versionMatch.Groups["release"].Value != String.Empty ? int.Parse(versionMatch.Groups["release"].Value) : 0;
                              version.Revision = versionMatch.Groups["revision"].Value;
                              version.DevVersion = false;
                            }
                            else
                            {
                              // take dev version into account
                              version.DevVersion = true;
                              Match devVersionMatch = Regex.Match(versionName, @"[_](?'dev'\w+)[_]", RegexOptions.IgnoreCase);
                              version.DevName = version.Product + "_" + devVersionMatch.Groups["dev"].Value; // Remember the full name for dev versions.
                            }

                            // Evaluate paths.
                            version.InstallPath = rootKey.OpenSubKey(versionName).GetValue("") as string;
                            if (!Directory.Exists(version.InstallPath))
                            {
                                throw new Exception("Install path not found!");
                            }

                            if (versionMatch.Success && version.Major < 3)
                            {
                                version.SamplesPath = Path.Combine(version.InstallPath, "samples");
                                version.FullInstallPath = Path.Combine(version.InstallPath, version.Product.ToLower() + "Connect.exe");
                                version.LocalAppPath = version.InstallPath;
                            }
                            else
                            {
                                version.SamplesPath = Path.Combine(Path.Combine(Path.Combine(Path.Combine(sharedDocumentsPath, "Documents"), "NaturalMotion"), versionName), "samples");
                                version.FullInstallPath = Path.Combine(Path.Combine(version.InstallPath, "bin"), version.Product.ToLower() + "Connect.exe");
                                string localAppPath = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
                                version.LocalAppPath = Path.Combine(Path.Combine(localAppPath, "NaturalMotion"), versionName);
                            }

                            if (!Directory.Exists(version.SamplesPath))
                            {
                                version.SamplesPath = string.Empty;
                                OutputLog.Log(LogSeverity.Info, string.Format("Samples not installed for version {0}", versionName));
                            }

                            if (!File.Exists(version.FullInstallPath))
                            {
                                throw new Exception("FullInstallPath not found!");
                            }

                            if (!Directory.Exists(version.LocalAppPath))
                            {
                                throw new Exception(string.Format("LocalAppPath '{0}' not found (this may happen if the particular product version is installed but was never run)!", version.LocalAppPath));
                            }

                            versions.Add(version);
                        }
                        catch (Exception e)
                        {
                            OutputLog.Log(LogSeverity.Error, e.Message);
                        }
                    }
                }
            }

            Version[] versions_ = new Version[versions.Count];
            {
                versions.Sort();
                versions.CopyTo(versions_);
            }

            return versions_;
        }
    }
}
