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
using System.Web;
using System.Xml;
using System.Xml.Serialization;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter
{
    /// <summary>
    /// Models the morpheme network file.
    /// </summary>
    public class MCN : IEquatable<MCN>
    {
        #region IEquatable Members
        public bool Equals(MCN other)
        {
            if (MCNFileName == null || 
                MCPFileName == null ||
                other == null)
            {
                return false;
            }

            if (MCNFileName.Equals(other.MCNFileName, StringComparison.CurrentCultureIgnoreCase) &&
                MCPFileName.Equals(other.MCPFileName, StringComparison.CurrentCultureIgnoreCase) &&
                AssetCompiler == other.AssetCompiler)
            {
                return true;
            }

            return false;
        }

        public override int GetHashCode()
        {
            return (MCNFileName + MCPFileName + AssetCompiler).GetHashCode();
        }
        #endregion


        public const string MCNExtension = ".mcn";
        public const string MCPExtension = ".mcp";
        public const string MCNIntermediateExtension = ".xml";

        /// <summary>
        /// Contains all the possible asset compiler types.
        /// The application then chooses the proper ones depending on the SKU.
        /// </summary>
        public enum AssetCompilerType
        {
            NoPhysics,
            PhysX2,
            PhysX3,
            Euphoria,
            Havok
        }

        public static List<AssetCompilerType> SKUSupportedAssetCompilers = new List<AssetCompilerType>();

        public MCN()
        {
            Use = true;
            MCNFileName = string.Empty;
            AssetCompiler = AssetCompilerType.NoPhysics;
            MCPFileName = string.Empty;
            Processed = false;
            Exported = false;
        }


        [XmlIgnore]
        public bool Use { get; set; }
        
        string mCNFileName = null;
        public string MCNFileName
        {
            get
            {
                return MacrosSystem.GetInstance().ConvertToMacro(mCNFileName);
            }
            set
            {
                if (value != this.mCNFileName)
                {
                    this.mCNFileName = MakePathRelativeToMCNList(value);
                }
            }
        }

       
        public AssetCompilerType AssetCompiler { get; set; }

        string mCPFileName = null;
        public string MCPFileName
        {
            get
            {
                return MacrosSystem.GetInstance().ConvertToMacro(mCPFileName);
            }
            set
            {
                try
                {
                    if (value != this.mCPFileName)
                    {
                        this.mCPFileName = MakePathRelativeToMCNList(value);
                    }
                }
                catch (System.Exception ex)
                {
                    OutputLog.Log(LogSeverity.Error, ex.ToString());
                }
               
            }
        }

        [XmlIgnore]
        public bool Processed { get; set; }

        [XmlIgnore]
        public bool Exported { get; set; }

        [XmlIgnore]
        public Logger OutputLog { private get; set; }

        private string MakePathRelativeToMCNList(string path)
        {
            if (string.IsNullOrEmpty(path))
            {
                return path;
            }

            string file = MacrosSystem.GetInstance().ExpandMacro(path);
            if (File.Exists(file))
            {
                System.Uri uri1 = new Uri(Path.GetFullPath(file));
                System.Uri uri2 = new Uri(MCNCollection.GetInstance().FileDirectory);

                if (uri2.IsBaseOf(uri1) == false)
                {
                    // only use relative paths if the file is in a subdirectory of the mcn liit file
                    return path;
                }

                Uri relativeUri = uri2.MakeRelativeUri(uri1);

                // replace %20 with spaces, etc
                string decoded = HttpUtility.UrlDecode(relativeUri.OriginalString);

                return decoded;
            }

            return path;
        }

        // convert the mcn path/filename to its absolute path and expand any macros within it
        public string GetMCNPathAbsoluteExpanded()
        {
            return GetAbsolutePathExpanded(mCNFileName);
        }

        // convert the mcp path/filename to its absolute path and expand any macros within it
        public string GetMCPPathAbsoluteExpanded()
        {
            return GetAbsolutePathExpanded(mCPFileName);
        }

        private string GetAbsolutePathExpanded(string path)
        {
            path = MacrosSystem.GetInstance().ExpandMacro(path);
            
            bool isAbsolute = Path.IsPathRooted(path);

            if (!isAbsolute)
            {
                string newPath = Path.Combine(MCNCollection.GetInstance().FileDirectory, path);

                newPath = Path.GetFullPath(newPath);

                return newPath;
            }

            return path;
        }
    }

    /// <summary>
    /// Collection of networks that also supports optional binding to a DataGridView.
    /// </summary>
    public class MCNCollection : BindingList<MCN>
    {
        public static MCNCollection GetInstance()
        {
            if (Instance == null)
            {
                Instance = new MCNCollection();
            }

            return Instance;
        }

        [XmlIgnore]
        public Logger OutputLog { private get; set; }

        public bool Add(string mcnFilename, int majorVersion)
        {
            MCN mcn = new MCN();
            mcn.MCNFileName = mcnFilename;

            if (!string.IsNullOrEmpty(mcnFilename))
            {
                // find the mcp
                string path = Path.GetDirectoryName(mcnFilename);
                DirectoryInfo dirInfo = new DirectoryInfo(path);

                while (dirInfo != null)
                {
                    FileInfo[] mcps = dirInfo.GetFiles("*" + MCN.MCPExtension);

                    if (mcps.Length > 0)
                    {
                        // assume it is the first found
                        mcn.MCPFileName = mcps[0].FullName;

                        break;
                    }

                    dirInfo = dirInfo.Parent;
                }

                // 'best guess' the asset compiler
                {
                    XmlTextReader reader = new XmlTextReader(mcnFilename);

                    while (reader.Read())
                    {
                        if (reader.Name.IndexOf("euphoria", StringComparison.CurrentCultureIgnoreCase) != -1 ||
                            reader.Value.IndexOf("euphoria", StringComparison.CurrentCultureIgnoreCase) != -1)
                        {
                            mcn.AssetCompiler = MCN.AssetCompilerType.Euphoria;
                        }

                        if (mcn.AssetCompiler == MCN.AssetCompilerType.NoPhysics)
                        {
                            if (reader.Name.IndexOf("physics", StringComparison.CurrentCultureIgnoreCase) != -1 ||
                                reader.Value.IndexOf("physics", StringComparison.CurrentCultureIgnoreCase) != -1 ||
                                reader.Name.IndexOf("physX", StringComparison.CurrentCultureIgnoreCase) != -1 ||
                                reader.Value.IndexOf("physX", StringComparison.CurrentCultureIgnoreCase) != -1)
                            {
                                mcn.AssetCompiler = majorVersion >= 5 ? MCN.AssetCompilerType.PhysX3 : MCN.AssetCompilerType.PhysX2;
                            }
                        }
                    }
                }
            }

            // add to the list
            Add(mcn);

            return true;
        }

        public int NumSelectedNetworks()
        {
            int numSelectedNetworks = 0;
            foreach (MCN mcn in MCNCollection.GetInstance())
            {
                if (mcn.Use)
                {
                    ++numSelectedNetworks;
                }
            }

            return numSelectedNetworks;
        }

        public void Serialise(FileStream writer)
        {
            System.Type bindingListType = typeof(BindingList<MCN>);
            XmlSerializer ser = new XmlSerializer(bindingListType);
            ser.Serialize(writer, this);
        }

        public void Deserialise(FileStream reader)
        {
            FileFullName = reader.Name;

            Clear();

            FileInfo fileInfo = new FileInfo(FileFullName);
            if (fileInfo != null &&
                fileInfo.Length == 0)
            {
                // empty file
                OutputLog.Log(LogSeverity.Info, "opened empty file " + FileFullName);
                return;
            }

            XmlSerializer ser = new XmlSerializer(typeof(BindingList<MCN>));
            BindingList<MCN> list = ser.Deserialize(reader) as BindingList<MCN>;

            foreach (MCN mcn in list)
            {
                Add(mcn);
            }
        }

        public string FileFullName {get; set;}

        public string FileName
        {
            get
            {
                if (string.IsNullOrEmpty(FileFullName))
                {
                    return string.Empty;
                }

                FileInfo fileInfo = new FileInfo(FileFullName);

                return fileInfo.Name;
            }
        }

        public string FileDirectory
        {
            get
            {
                if (string.IsNullOrEmpty(FileFullName))
                {
                    return string.Empty;
                }

                FileInfo fileInfo = new FileInfo(FileFullName);

                return fileInfo.DirectoryName + "\\";
            }
        }

        private MCNCollection()
        {
            FileFullName = string.Empty;
        }

        private static MCNCollection Instance = null;
    }
}
