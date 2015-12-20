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

namespace NaturalMotion.AssetExporter.Internal
{
    /// <summary>
    /// A binding list that can be updated from any thread.
    /// </summary>
    /// <typeparam name="T">The type of elements in the list.</typeparam>
    public class SyncList<T> : System.ComponentModel.BindingList<T>
    {
        private System.ComponentModel.ISynchronizeInvoke SyncObject { get; set; }

        public SyncList()
            : this(null)
        {
        }

        public SyncList(System.ComponentModel.ISynchronizeInvoke syncObject)
        {
            SyncObject = syncObject;
        }

        protected override void OnListChanged(System.ComponentModel.ListChangedEventArgs args)
        {
            if ((SyncObject != null) && SyncObject.InvokeRequired)
            {
                System.Action<System.ComponentModel.ListChangedEventArgs> method = FireEvent;
                SyncObject.Invoke(method, new object[] { args });
            }
            else
            {
                FireEvent(args);
            }
        }

        private void FireEvent(System.ComponentModel.ListChangedEventArgs args)
        {
            base.OnListChanged(args);
        }
    }
}
