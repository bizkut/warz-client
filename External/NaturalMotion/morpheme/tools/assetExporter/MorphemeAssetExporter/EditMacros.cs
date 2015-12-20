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
using System.Windows.Forms;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter
{
    public partial class EditMacros : Form
    {
        public EditMacros()
        {
            try
            {
                InitializeComponent();

                MacrosSystem.GetInstance().BackUpCustomList();

                macroPairBindingSource.DataSource = MacrosSystem.GetInstance().MacroStringCustomList;
                
                macroPairBindingSource2.DataSource = MacrosSystem.GetInstance().MacroStringSystemList;

                // reserved macros
                {
                    int counter = 0;
                    foreach (MacrosSystem.ReservedNetworkMacroKeys macro in Enum.GetValues(typeof(MacrosSystem.ReservedNetworkMacroKeys)))
                    {
                        DataGridViewRow row = new DataGridViewRow();

                        DataGridViewTextBoxCell cell1 = new DataGridViewTextBoxCell();
                        cell1.Value = Enum.GetName(typeof(MacrosSystem.ReservedNetworkMacroKeys), macro);

                        DataGridViewTextBoxCell cell2 = new DataGridViewTextBoxCell();
                        cell2.Value = MacrosSystem.ReservedNetworkMacroDescriptions[counter];

                        row.Cells.Add(cell1);
                        row.Cells.Add(cell2);

                        row.ReadOnly = true;

                        dataGridView3.Rows.Add(row);

                        counter++;
                    }
                }
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }

        private void buttonMacrosOK_Click(object sender, EventArgs e)
        {
            try
            {
                MacrosSystem.GetInstance().BuildDictionary();
                this.Close();
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }

        private void buttonMacrosCancel_Click(object sender, EventArgs e)
        {
            try
            {
                MacrosSystem.GetInstance().RevertCustomList();
                this.Close();
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }

        private void dataGridView1_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                if (e.ColumnIndex == 0)
                {
                    DataGridViewTextBoxCell cell = dataGridView1[e.ColumnIndex, e.RowIndex] as DataGridViewTextBoxCell;

                    string macro = cell.Value as string;
                   
                    if (string.IsNullOrEmpty(macro))
                    {
                        return;
                    }

                    // The following function call outputs to the log if the macro is not valid.
                    MacrosSystem.GetInstance().IsCustomMacroValid(macro);
                }
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }
    }
}
