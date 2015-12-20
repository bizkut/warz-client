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

namespace NaturalMotion.AssetExporter.RegistryTool
{
    /// <summary>
    /// Contains morpheme version information 
    /// </summary>
    public class Version : IComparable, IComparable<Version>, IEquatable<Version>
    {
        public string Product { get; set; }
        public int Major { get; set; }
        public int Minor { get; set; }
        public int Release { get; set; }
        public string Revision { get; set; }

        public string Name {
            get {
              if (!DevVersion)
              {
                if (string.IsNullOrEmpty(Revision))
                {
                  return String.Format("{0}_{1}.{2}.{3}", Product, Major, Minor, Release);
                }
                else
                {
                  return String.Format("{0}_{1}.{2}.{3}_{4}", Product, Major, Minor, Release, Revision);
                }
              }
              else
              {
                return DevName;
              }
            }
        }

        public string DevName { get; set; }
        public bool DevVersion { get; set; }

        public string InstallPath { get; set; }
        public string SamplesPath { get; set; }
        public string FullInstallPath { get; set; }
        public string LocalAppPath { get; set; }

        public int CompareTo(object obj)
        {
            Version other = obj as Version;
            return CompareTo(other);
        }

        /// <summary>
        /// Implements the generic IComparable interface.
        /// </summary>
        /// <param name="other"> The second version number to check. </param>
        /// <returns> 1 if other is sorted after this, -1 if other is sorted before this, 0 if the two objects are equivalent. </returns>
        public int CompareTo(Version other)
        {
            if (DevVersion)
            {
                if (other.DevVersion)
                {
                    return other.DevName.CompareTo(DevName);
                }
                else
                {
                    return -1;
                }
            }
            else if (other.DevVersion)
            {
                return 1;
            }

            int result = other.Major.CompareTo(Major);
            if (result == 0)
            {
                result = other.Minor.CompareTo(Minor);
                if (result == 0)
                {
                    result = other.Release.CompareTo(Release);
                    if (result == 0)
                    {
                        result = other.Revision.CompareTo(Revision);
                        if (result == 0)
                        {
                            result = other.Product.CompareTo(Product);
                        }
                    }
                }
            }

            return result;
        }

        public bool Equals(Version other)
        {
            if (DevVersion)
            {
                if (other.DevVersion)
                {
                    return DevName.Equals(other.DevName);
                }

                return false;
            }

            return (Major == other.Major) && (Minor == other.Minor) && (Release == other.Release) && (Product == other.Product);
        }
    }
}
