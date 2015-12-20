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
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Windows.Forms;
using NaturalMotion.AssetExporter.Logging;

namespace NaturalMotion.AssetExporter
{
    public partial class ApplicationWindow : Form
    {
        // Column names for the dataGridViews

        class DataGridViewPlatformNames
        {
            public const string Use = "Use";
#if DEV_ENV_SUPPORT
            public const string Name = "name";
#endif
            public const string ProcessPath = "Process sub path";
            public const string AssetCompilerCustomEXEName = "Asset compiler executable";

            public const string KinectOption = "Kinect";
            public const string MoveOption = "Move";

#if DEV_ENV_SUPPORT
            public static readonly string AssetCompilerEXEPlatform = "asset compiler exe platform";

            public static readonly string AssetCompilerSLNFile = "asset compiler sln platform";
            public static readonly string Config = "build config";

            public static readonly int BuildButtonStartColumnIndex = 7; // the first index (after the columns above) for building the ACs
#endif
        }

        public ApplicationWindow()
        {
            try
            {
                InitializeComponent();

                // allow drag and drop
                this.AllowDrop = true;
                this.DragEnter += new DragEventHandler(Form1_DragEnter);
                this.DragDrop += new DragEventHandler(Form1_DragDrop);

                Logger.SynchonizeInvoke = this;

                // set up default directories for the file dialogs
                {
                    string currentDirectory = Application.StartupPath;
                    string currentDirectoryParent = Directory.GetParent(currentDirectory).FullName;
                    string currentDirectoryParentParent = Directory.GetParent(currentDirectoryParent).FullName;
                    string currentDirectoryParentParentParent = Directory.GetParent(currentDirectoryParentParent).FullName;

                    openFileDialogMCNList.InitialDirectory = Path.Combine(currentDirectoryParentParentParent, "Samples");
                    saveFileCreateMCNList.InitialDirectory = Path.Combine(currentDirectoryParentParentParent, "Samples");

                    saveFileDialogCofig.InitialDirectory = Path.Combine(currentDirectoryParentParentParent, "Samples");
                    openFileDialogConfig.InitialDirectory = saveFileDialogCofig.InitialDirectory;

                    saveFileDialogLog.InitialDirectory = currentDirectoryParentParentParent;

                    //TODO use Environment.GetFolderPath(Environment.SpecialFolder.CommonDocuments);
                    openFileDialogMCNs.InitialDirectory = @"C:\Users\Public\Documents\NaturalMotion\";

                    string programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
                    openFileDialogBrowseConnect.InitialDirectory = programFiles;

                    openFileDialogAssetCompiler.InitialDirectory = Settings.GetInstance().RuntimeSDKRoot;
                }

                InitCustomComponents();

                UpdatLogView();

                ValidateMCNListButtons();
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }
          
        void Form1_DragEnter(object sender, DragEventArgs e)
        {
            try
            {
                if (e.Data.GetDataPresent(DataFormats.FileDrop)) e.Effect = DragDropEffects.Copy;
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }

        void AddFiles(string[] files, int majorVersion)
        {
            foreach (string file in files)
            {
                if (string.IsNullOrEmpty(file))
                {
                    continue;
                }

                // get the file attributes for file or directory
                FileAttributes attr = File.GetAttributes(file);

                if ((attr & FileAttributes.Directory) == FileAttributes.Directory)
                {
                    // only 1 level of recursion needed with SearchOption.AllDirectories
                    string[] subFiles = Directory.GetFiles(file, "*" + MCN.MCNExtension, SearchOption.AllDirectories);
                    AddFiles(subFiles, majorVersion);
                }
                else
                {
                    if (File.Exists(file) == false)
                    {
                        continue;
                    }

                    FileInfo fileInfo = new FileInfo(file);

                    if (false == fileInfo.Extension.Equals(MCN.MCNExtension, StringComparison.CurrentCultureIgnoreCase))
                    {
                        // not an MCN
                        Program.Logger.Log(LogSeverity.Warning, "skipping " + file + " - not an mcn");
                        continue;
                    }

                    MCNCollection.GetInstance().Add(file, majorVersion);
                }
            }
        }

        void Form1_DragDrop(object sender, DragEventArgs e)
        {
            try
            {
                string[] files = e.Data.GetData(DataFormats.FileDrop) as string[];

                if (dataGridViewMCNList.Enabled)
                {
                    AddFiles(files, MorphemeVersionSelector.GetInstance().SelectedVersion.MajorVersion);
                }
                else
                {
                    Program.Logger.Log(LogSeverity.Error, "Cannot add files. Please create a new list");
                }
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }

        void InitCustomComponents()
        {
            // create columns in the DataGridViews
            // data bind, etc

            // MCN list
            {
                // bind to the mcns
                dataGridViewMCNList.DataSource = MCNCollection.GetInstance();

                // Set up the Combo box column data source
                DataGridViewComboBoxColumn assetCompilerCol = (DataGridViewComboBoxColumn)dataGridViewMCNList.Columns[2];
                assetCompilerCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
                assetCompilerCol.DataSource = MCN.SKUSupportedAssetCompilers;
            }

            // settings
            {
                // dataGridViewPlatforms
                {
                    dataGridViewPlatforms.AutoGenerateColumns = false;

                    dataGridViewPlatforms.DataSource = Settings.GetInstance().Platforms;

                    dataGridViewPlatforms.RowHeadersVisible = false;

                    DataGridViewCheckBoxColumn checkBox = new DataGridViewCheckBoxColumn();
                    checkBox.DataPropertyName = "UseDuringExport";
                    checkBox.Name = DataGridViewPlatformNames.Use;

                    dataGridViewPlatforms.Columns.Add(checkBox);

#if DEV_ENV_SUPPORT
                    DataGridViewTextBoxColumn nameBox = new DataGridViewTextBoxColumn();
                    nameBox.DataPropertyName = "Name";
                    nameBox.Name = DataGridViewPlatformNames.Name;
                    dataGridViewPlatforms.Columns.Add(nameBox);
#endif
                    DataGridViewTextBoxColumn processPathBox = new DataGridViewTextBoxColumn();
                    processPathBox.DataPropertyName = "ProcessPathSubDirectory";
                    processPathBox.Name = DataGridViewPlatformNames.ProcessPath;
                    processPathBox.ToolTipText = "Sub-folder of the export root where to store the processed networks";
                    dataGridViewPlatforms.Columns.Add(processPathBox);

#if DEV_ENV_SUPPORT

                    DataGridViewTextBoxColumn slnFileBox = new DataGridViewTextBoxColumn();
                    slnFileBox.DataPropertyName = "SlnPlatform";
                    slnFileBox.Name = DataGridViewPlatformNames.AssetCompilerSLNFile;
                    dataGridViewPlatforms.Columns.Add(slnFileBox);

                    DataGridViewTextBoxColumn configFileBox = new DataGridViewTextBoxColumn();
                    configFileBox.DataPropertyName = "ConfigFileBox";
                    configFileBox.Name = DataGridViewPlatformNames.Config;
                    dataGridViewPlatforms.Columns.Add(configFileBox);

                    DataGridViewColumn col = dataGridViewPlatforms.Columns["build config"];

                    Array values = Enum.GetValues(typeof(MCN.AssetCompilerType));

                    foreach (MCN.AssetCompilerType val in values)
                    {
                        DataGridViewButtonColumn buttonBox = new DataGridViewButtonColumn();
                        buttonBox.Name = Enum.GetName(typeof(MCN.AssetCompilerType), val);

                        dataGridViewPlatforms.Columns.Add(buttonBox);
                    }
#endif

                    DataGridViewTextBoxColumn exeName = new DataGridViewTextBoxColumn();
                    exeName.DataPropertyName = "AssetCompilerCustomEXE";
                    exeName.Name = DataGridViewPlatformNames.AssetCompilerCustomEXEName;
                    dataGridViewPlatforms.Columns.Add(exeName);

                    DataGridViewCheckBoxColumn kinectCheckBox = new DataGridViewCheckBoxColumn();
                    kinectCheckBox.DataPropertyName = "UseKinect";
                    kinectCheckBox.Name = DataGridViewPlatformNames.KinectOption;

                    dataGridViewPlatforms.Columns.Add(kinectCheckBox);


                    DataGridViewCheckBoxColumn moveCheckBox = new DataGridViewCheckBoxColumn();
                    moveCheckBox.DataPropertyName = "UseMove";
                    moveCheckBox.Name = DataGridViewPlatformNames.MoveOption;

                    dataGridViewPlatforms.Columns.Add(moveCheckBox);

                    foreach (DataGridViewColumn col in dataGridViewPlatforms.Columns)
                    {
                        col.AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                    }
                }

                // bind the text boxes
                textBoxMorphemeConnect.DataBindings.Add("Text", Settings.GetInstance(), "MorphemeConnectPath");
                textBoxMorphemeSDKRoot.DataBindings.Add("Text", Settings.GetInstance(), "RuntimeSDKRoot");
                textBoxExportPath.DataBindings.Add("Text", Settings.GetInstance(), "ExportRootPath");
                textBoxAssetCompilerCommandLine.DataBindings.Add("Text", Settings.GetInstance(), "AssetCompilerCommandLine");
            }

            // bind the log output
            logItemBindingSource.DataSource = Program.Logger.VisibleList;

            {
                MCN.SKUSupportedAssetCompilers.Clear();
                foreach (MCN.AssetCompilerType acType in Enum.GetValues(typeof(MCN.AssetCompilerType)))
                {
                    MCN.SKUSupportedAssetCompilers.Add(acType);
                }
            }

            // bind the product combo box
            {
                comboBoxMorphemeVersion.DisplayMember = "Key";
                comboBoxMorphemeVersion.ValueMember = "Key";

                BindingSource binding = new BindingSource();
                binding.DataSource = MorphemeVersionSelector.GetInstance().VersionDictionary;
                comboBoxMorphemeVersion.DataSource = binding;

                comboBoxMorphemeVersion.DataBindings.Add("SelectedValue", MorphemeVersionSelector.GetInstance(), "SelectedVersionKey", true,
                DataSourceUpdateMode.OnPropertyChanged);
            }
            {
                BindingSource binding = new BindingSource();
                binding.DataSource = Settings.GetInstance().UseDynamicAssetCompilerPlugins;
                
                checkBoxUseDynamicPlugins.DataBindings.Add("Checked", Settings.GetInstance(), "UseDynamicAssetCompilerPlugins");
            }
        }

        private void buttonExportAll_Click(object sender, EventArgs e)
        {
            // disbale UI
            Enabled = false;

            StartTimer = DateTime.Now;

            // reset progress bar (with a small amount of progress)
            progressBar1.Value = 2;

            // Export only.
            MorphemePipeLine.GetInstance().Process(false, new ProgressChangedEventHandler(worker_ProgressChanged), new RunWorkerCompletedEventHandler(worker_RunWorkerCompleted));
        }

        private void buttonExportAndProcessAll_Click(object sender, EventArgs e)
        {
            // disbale UI
            Enabled = false;

            StartTimer = DateTime.Now;

            // reset progress bar (with a small amount of progress)
            progressBar1.Value = 2;

            // Export and process.
            MorphemePipeLine.GetInstance().Process(true, new ProgressChangedEventHandler(worker_ProgressChanged), new RunWorkerCompletedEventHandler(worker_RunWorkerCompleted));
        }

        void worker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progressBar1.Value = e.ProgressPercentage;
        }

        void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            // done! exported [and processed] all networks

            // re-enable the form
            Enabled = true;

            // complete the progress bar
            progressBar1.Value = 100;

            dataGridViewMCNList.Refresh();

            UpdatLogView();
        }

        private void dataGridView2_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            // check that the mcn and mcp exist
            try
            {
                if (e.RowIndex < 0)
                {
                    return;
                }

                DataGridViewColumn column = dataGridViewMCNList.Columns[e.ColumnIndex];

                if (column is DataGridViewTextBoxColumn)
                {
                    DataGridViewCell cell = dataGridViewMCNList[e.ColumnIndex, e.RowIndex];

                    if (cell == null)
                    {
                        return;
                    }

                    if (cell.Value == null)
                    {
                        // reset the background colour
                        cell.Style.BackColor = Logger.Color_Info;
                        return;
                    }

                    string file = cell.Value.ToString();

                    if (string.IsNullOrEmpty(file))
                    {
                        // reset the background colour
                        cell.Style.BackColor = Logger.Color_Info;
                        return;
                    }

                    // expand any macros in the file
                    file = MacrosSystem.GetInstance().ExpandMacro(file);

                    bool isAbsolute = Path.IsPathRooted(file);

                    if (!isAbsolute)
                    {
                        // it's a relative path. prepend the mcn list path to make it absolute
                        string newPath = Path.Combine(MCNCollection.GetInstance().FileDirectory, file);

                        file = Path.GetFullPath(newPath);
                    }

                    cell.Style.BackColor = Logger.Color_Info;

                    if (!File.Exists(file))
                    {
                        // cannot find file - show warning 
                        cell.Style.BackColor = Logger.Color_Warning;
                        return;
                    }

                    // check the mcn is valid
                    {
                        string MCNFileName = "MCNFileName";
                        MemberInfo[] info = typeof(MCN).GetMember(MCNFileName);
                        if (info.Length > 0)
                        {
                            if (column.DataPropertyName == MCNFileName)
                            {
                                FileInfo fileInfo = new FileInfo(file);
                                if (false == fileInfo.Extension.Equals(MCN.MCNExtension, StringComparison.CurrentCultureIgnoreCase))
                                {
                                    cell.Style.BackColor = Logger.Color_Warning;
                                    return;
                                }
                            }
                        }
                        else
                        {
                            Program.Logger.Log(LogSeverity.Error, "Cannot find member variable " + MCNFileName);
                        }
                    }

                    // check the mcp is valid
                    {
                        string MCPFileName = "MCPFileName";
                        MemberInfo[] info = typeof(MCN).GetMember(MCPFileName);
                        if (info.Length > 0)
                        {
                            if (column.DataPropertyName == MCPFileName)
                            {
                                FileInfo fileInfo = new FileInfo(file);
                                if (false == fileInfo.Extension.Equals(MCN.MCPExtension, StringComparison.CurrentCultureIgnoreCase))
                                {
                                    cell.Style.BackColor = Logger.Color_Warning;
                                    return;
                                }
                            }
                        }
                        else
                        {
                            Program.Logger.Log(LogSeverity.Error, "Cannot find member variable " + MCPFileName);
                        }
                    }
                }  
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, "Exception caught: " + ex.ToString());
            }     
        }

        private void AddNetworksDialog()
        {
            // open a dialog and ask the user to add mcn networks

            DialogResult result = openFileDialogMCNs.ShowDialog();

            if (result == DialogResult.OK)
            {
                AddFiles(openFileDialogMCNs.FileNames, MorphemeVersionSelector.GetInstance().SelectedVersion.MajorVersion);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            AddNetworksDialog();
        }

        private void buttonSaveMCNList_Click(object sender, EventArgs e)
        {
            // save the mcn list
            try
            {
                using (FileStream fs = new FileStream(MCNCollection.GetInstance().FileFullName, FileMode.Create))
                {
                    MCNCollection.GetInstance().Serialise(fs);
                    Program.Logger.Log(LogSeverity.GoodNews, "Saved mcn list " + MCNCollection.GetInstance().FileFullName);
                }
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, "Saving may have failed. Exception caught: " + ex.ToString());
            }
        }


        private void buttonOpenMCNList_Click(object sender, EventArgs e)
        {
            DialogResult result = openFileDialogMCNList.ShowDialog(); // Show the dialog.
        }

        private void openFileDialogMCN_FileOk(object sender, CancelEventArgs e)
        {
            try
            {
                // enable the list here for the binding to work correctly
                dataGridViewMCNList.Enabled = true;

                // populate the list
                using (FileStream fs = new FileStream(openFileDialogMCNList.FileName, FileMode.Open))
                {
                    MCNCollection.GetInstance().Deserialise(fs);
                    ValidateMCNListButtons();
                }

                // show it is no-longer disabled
                dataGridViewMCNList.RowsDefaultCellStyle.BackColor = Color.White;
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());

                // something went wrong - disable again
                dataGridViewMCNList.Enabled = false;
            }
        }

        private void ValidateMCNListButtons()
        {
            // set the status of the buttons depending on having a valid mcn list

            if (MCNCollection.GetInstance().FileFullName == string.Empty)
            {
                buttonExport.Enabled = false;
                buttonExportAndProcess.Enabled = false;            
                buttonAddMCNs.Enabled = false;
                buttonSaveMCNList.Enabled = false;
                buttonClearMCNList.Enabled = false;
            }
            else
            {
                buttonAddMCNs.Enabled = true;
                buttonSaveMCNList.Enabled = true;
                buttonClearMCNList.Enabled = true;

                if (dataGridViewMCNList.RowCount > 1)
                {
                    buttonExport.Enabled = true;
                    buttonExportAndProcess.Enabled = true;
                }
                else
                {
                    buttonExport.Enabled = false;
                    buttonExportAndProcess.Enabled = true;
                }
            }
        }

        private void dataGridView2_RowsAdded(object sender, DataGridViewRowsAddedEventArgs e)
        {
            ValidateMCNListButtons();
        }

        private void dataGridView2_RowsRemoved(object sender, DataGridViewRowsRemovedEventArgs e)
        {
            ValidateMCNListButtons();
        }

        private void buttonClear_Click(object sender, EventArgs e)
        {
            MCNCollection.GetInstance().Clear();
        }

        private void buttonClearLog_Click(object sender, EventArgs e)
        {
            Program.Logger.LogList.Clear();
            UpdatLogView();
        }

        private void buttonSaveConfig_Click(object sender, EventArgs e)
        {
            saveFileDialogCofig.ShowDialog(); // Show the dialog.
        }

        private void saveFileDialog2_FileOk(object sender, CancelEventArgs e)
        {
            // save the config file
            try
            {
                using (FileStream fs = new FileStream(saveFileDialogCofig.FileName, FileMode.Create))
                {
                    Config config = new Config();
                    config.Serialise(fs);
                }
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }

        private void buttonOpenConfig_Click(object sender, EventArgs e)
        {
            DialogResult result = openFileDialogConfig.ShowDialog();
        }

        private void openFileDialog3_FileOk(object sender, CancelEventArgs e)
        {
            // desrialise the config and build the macro table
            try
            {
                using (FileStream fs = new FileStream(openFileDialogConfig.FileName, FileMode.Open))
                {
                    Config config = new Config();
                    config.Deserialise(fs);
                    MacrosSystem.GetInstance().BuildDictionary();

                    dataGridViewPlatforms.Refresh();
                    textBoxMorphemeConnect.Refresh();
                    textBoxExportPath.Refresh();
                }
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }

        private void UpdatLogView()
        {
            // refresh the logger with the correct visibilities
            Program.Logger.SetAndUpdateVisibleSet(showGoodNewsCheckBox.Checked, showInfoCheckBox.Checked, showWarningsCheckBox.Checked, showErrorsCheckBox.Checked);
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            UpdatLogView();
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            UpdatLogView();
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            UpdatLogView();
        }

        private void checkBox4_CheckedChanged(object sender, EventArgs e)
        {
            UpdatLogView();
        }

        private void dataGridViewLogger_RowsAdded(object sender, DataGridViewRowsAddedEventArgs e)
        {
            // when a new item is added to the logger dataGridView, set the back colour depending on the severity
            for (int rowIndex = e.RowIndex; rowIndex < (e.RowIndex + e.RowCount); ++rowIndex)
            {
                if ((dataGridViewLogger[0, rowIndex] != null) && (dataGridViewLogger[0, rowIndex].Value != null))
                {
                    Color color = Logger.FindColorForSeverity(dataGridViewLogger[0, rowIndex].Value.ToString());

                    dataGridViewLogger[0, rowIndex].Style.BackColor = color;
                    dataGridViewLogger[0, rowIndex].Style.SelectionBackColor = color;
                }
            }
        }
        
        private void DataGridViewPlatformsValidate(int rowIndex)
        {
#if DEV_ENV_SUPPORT
            try
            {
                if (dataGridViewPlatforms.ColumnCount <= 1)
                {
                    return;
                }

                if (dataGridViewPlatforms[DataGridViewPlatformNames.AssetCompilerEXEPath, rowIndex].Value == null)
                {
                    return;
                }

                if (dataGridViewPlatforms[DataGridViewPlatformNames.AssetCompilerEXEPlatform, rowIndex].Value == null)
                {
                    return;
                }

                Array values = Enum.GetValues(typeof(MCN.AssetCompilerType));

                int columnIndex = DataGridViewPlatformNames.BuildButtonStartColumnIndex;
                foreach (MCN.AssetCompilerType val in values)
                {
                    Enum.GetName(typeof(MCN.AssetCompilerType), val);

                    string assetCompilerPath = MacrosSystem.GetInstance().ExpandMacro(Settings.GetInstance().RuntimeSDKRoot);
                    string assetCompilerSubPath = dataGridViewPlatforms[DataGridViewPlatformNames.AssetCompilerEXEPath, rowIndex].Value.ToString();
                    string assetCompilerType = Enum.GetName(typeof(MCN.AssetCompilerType), val);
                    string assetCompilerPlatform = dataGridViewPlatforms[DataGridViewPlatformNames.AssetCompilerEXEPlatform, rowIndex].Value.ToString();

                    string assetCompilerExe = MorphemeAssetProcessor.ConstructAssetCompilerFile(assetCompilerPath,
                                                                                            assetCompilerSubPath,
                                                                                            assetCompilerType,
                                                                                            assetCompilerPlatform);

                    DataGridViewButtonCell button = dataGridViewPlatforms[columnIndex, rowIndex] as DataGridViewButtonCell;
                    button.Value = "build";

                    if (!File.Exists(assetCompilerExe))
                    {
                        Program.Logger.Log(LogSeverity.Info, "cannot find asset compiler: " + assetCompilerExe);

                        button.FlatStyle = FlatStyle.Popup;
                        button.Style.BackColor = Logger.Color_Error;
                    }
                    else
                    {
                        button.FlatStyle = FlatStyle.Popup;
                        button.Style.BackColor = Logger.Color_GoodNews;
                    }

                    columnIndex++;
                }
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
#endif // #if DEV_ENV_SUPPORT
        }

        private void dataGridViewPlatforms_RowsAdded(object sender, DataGridViewRowsAddedEventArgs e)
        {
            DataGridViewPlatformsValidate(e.RowIndex);
        }

        private void dataGridViewPlatforms_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
#if DEV_ENV_SUPPORT
            try
            {
                if ((e.RowIndex >= 0)
                    && (dataGridViewPlatforms[e.ColumnIndex, e.RowIndex] is DataGridViewButtonCell)
                    && (dataGridViewPlatforms[DataGridViewPlatformNames.AssetCompilerSLNFile, e.RowIndex].Value != null))
                {
                    string testFileName = "build.log";

                    // create the file, close the file stream
                    using (File.Create(testFileName))
                    {
                    }

                    string slnPlatformName = dataGridViewPlatforms[DataGridViewPlatformNames.AssetCompilerSLNFile, e.RowIndex].Value.ToString();
                    string type = dataGridViewPlatforms.Columns[e.ColumnIndex].Name;

                    // build the correct asset compiler
                    string assetCompiler = string.Format("morphemeAssetCompiler_{0}_{1}.sln", type, slnPlatformName);

                    string path = Path.Combine(MacrosSystem.GetInstance().ExpandMacro(Settings.GetInstance().RuntimeSDKRoot), @"morpheme\tools\assetCompiler");
                    string sln = Path.Combine(path, assetCompiler);

                    if (!File.Exists(sln))
                    {
                        Program.Logger.Log(LogSeverity.Error, "cannot find solution " + sln);
                        return;
                    }

                    // build up the config string
                    string config = dataGridViewPlatforms[DataGridViewPlatformNames.Config, e.RowIndex].Value.ToString();

                    string commandLine = "\"" + sln + "\"" + " " + "/Build \"" + config + "\" /Out " + testFileName;

                    // build up the devenv exe
                    string exe = string.Empty;

                    string platformName = dataGridViewPlatforms[DataGridViewPlatformNames.Name, e.RowIndex].Value.ToString();

                    exe = Path.Combine(Environment.GetEnvironmentVariable("VS90COMNTOOLS"), @"..\IDE\devenv.exe");

                    if (!File.Exists(exe))
                    {
                        Program.Logger.Log(LogSeverity.Error, "cannot find executable " + exe);
                        return;
                    }

                    Program.Logger.Log(LogSeverity.Info, "building ... " + commandLine);

                    using (Process proc = System.Diagnostics.Process.Start(exe, commandLine))
                    {
                        // wait for the process to end
                        proc.WaitForExit();
                    }

                    using (StreamReader sr = new StreamReader(testFileName))
                    {
                        String line;

                        // Read and display lines from the file until the end of
                        // the file is reached.
                        while ((line = sr.ReadLine()) != null)
                        {
                            Program.Logger.Log(LogSeverity.Info, "devenv.exe > " + line);
                        }
                    }

                    DataGridViewPlatformsValidate(e.RowIndex);
                }
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
#endif // #if DEV_ENV_SUPPORT
        }

        private void buttonSaveLog_Click(object sender, EventArgs e)
        {
            saveFileDialogLog.ShowDialog(); // Show the dialog.
        }

        private void saveFileDialogLog_FileOk(object sender, CancelEventArgs e)
        {
            // save out the log file
            try
            {
                using (FileStream file = new FileStream(saveFileDialogLog.FileName, FileMode.Create))
                {
                    Program.Logger.WriteToStream(file);
                }
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }

        public Color ValidateTextBoxIsFile(TextBox textBox)
        {
            // helper function to check for file existence
            string text = MacrosSystem.GetInstance().ExpandMacro(textBox.Text);
            if (File.Exists(text) || string.IsNullOrEmpty(text))
            {
                return Logger.Color_Info;
            }
            else
            {
                return Logger.Color_Warning;
            }
        }

        public Color ValidateTextBoxIsDirectory(TextBox textBox)
        {
            // helper function to check for directory existence
            string text = MacrosSystem.GetInstance().ExpandMacro(textBox.Text);
            if (Directory.Exists(text) || string.IsNullOrEmpty(text))
            {
                return Logger.Color_Info;
            }
            else
            {
                return Logger.Color_Warning;
            }
        }

        private void textBoxMorphemeConnect_TextChanged(object sender, EventArgs e)
        {
            TextBox textBox = sender as TextBox;
            textBox.BackColor = ValidateTextBoxIsFile(textBox);

            // special case for custom selection
            if (MorphemeVersionSelector.GetInstance().SelectedVersionKey.Equals(MorphemeVersionSelector.CustomStr))
            {
                // set the macro for the custom install path
                MorphemeVersionSelector.GetInstance().SetCustomValue(MacrosSystem.ReservedVersionMacroKeys.APP_INSTALL_FULL_PATH, textBox.Text);
            }
        }

        private void textBoxMorphemeSDKRoot_TextChanged(object sender, EventArgs e)
        {
            TextBox textBox = sender as TextBox;
            textBox.BackColor = ValidateTextBoxIsDirectory(textBox);
        }

        private void textBoxExportPath_TextChanged(object sender, EventArgs e)
        {
            TextBox textBox = sender as TextBox;
            textBox.BackColor = ValidateTextBoxIsDirectory(textBox);
        }

        private void buttonEditMacros_Click(object sender, EventArgs e)
        {
            // open up the edit macros form
            var editMacros = new EditMacros();
            editMacros.Show();
        }

        public static float Clamp(float val, float min, float max)
        {
            if (val.CompareTo(min) < 0) return min;
            else if (val.CompareTo(max) > 0) return max;
            else return val;
        }

        private void buttonCreateMCNList_Click(object sender, EventArgs e)
        {
            saveFileCreateMCNList.FileName = "newList";
            saveFileCreateMCNList.ShowDialog();
        }

        private void saveFileCreateMCNList_FileOk(object sender, CancelEventArgs e)
        {
            try
            {
                // enable now to ensure binding works correctly
                dataGridViewMCNList.Enabled = true;

                // create a new .lst file and validate the buttons 
                using (FileStream fs = new FileStream(saveFileCreateMCNList.FileName, FileMode.Create))
                {
                    MCNCollection.GetInstance().FileFullName = saveFileCreateMCNList.FileName;
                    ValidateMCNListButtons();
                }

                dataGridViewMCNList.RowsDefaultCellStyle.BackColor = Color.White;

                // created new list. popup the add networks dialog
                AddNetworksDialog();
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());

                // something went wrong - disable again
                dataGridViewMCNList.Enabled = false;
            }
        }

        private void comboBoxMorphemeVersion_SelectedIndexChanged(object sender, EventArgs e)
        {
            // the Product combo box has changed

            try
            {         
                ComboBox cb = sender as ComboBox;

                string newValue = cb.Text;

                // Leave the euphoria asset compiler in place
                if (!newValue.Equals(MorphemeVersionSelector.CustomStr))
                {
                  if (newValue.Contains("Euphoria") && !MCN.SKUSupportedAssetCompilers.Contains(MCN.AssetCompilerType.Euphoria))
                  {
                      MCN.SKUSupportedAssetCompilers.Add(MCN.AssetCompilerType.Euphoria);
                  }

                  if (!newValue.Contains("Euphoria") && MCN.SKUSupportedAssetCompilers.Contains(MCN.AssetCompilerType.Euphoria))
                  {
                    MCN.SKUSupportedAssetCompilers.Remove(MCN.AssetCompilerType.Euphoria);
                  }
                }
                
                // if the new value is the custom selection then allow browsing for connect
                if (newValue == MorphemeVersionSelector.CustomStr)
                {
                    textBoxMorphemeConnect.Enabled = true;
                    buttonBrowseConnect.Enabled = true;
                    checkBoxUseDynamicPlugins.Enabled = true;
                }
                else
                {
                    textBoxMorphemeConnect.Enabled = false;
                    buttonBrowseConnect.Enabled = false;
                    checkBoxUseDynamicPlugins.Enabled = false;

                    // version 3.6 introduced dynamic plugins
                    MorphemeVersionSelector.MorphemeVersion version = MorphemeVersionSelector.GetInstance().SelectedVersion;
                    checkBoxUseDynamicPlugins.Checked = !(version.MajorVersion < 3 || (version.MajorVersion == 3 && version.MinorVersion < 6));
                }

                int numModified = 0;
                foreach (MCN mcn in MCNCollection.GetInstance())
                {
                    if (mcn.AssetCompiler == MCN.AssetCompilerType.Euphoria &&
                        !MCN.SKUSupportedAssetCompilers.Contains(MCN.AssetCompilerType.Euphoria))
                    {
                        ++numModified;
                       mcn.AssetCompiler = MCN.AssetCompilerType.NoPhysics;
                    }
                }
                
                if (numModified > 0)
                {
                    dataGridViewMCNList.Refresh();
                }
            }
            catch (System.Exception ex)
            {
                Program.Logger.Log(LogSeverity.Error, ex.ToString());
            }
        }

        private void buttonBrowseExport_Click(object sender, EventArgs e)
        {
            // open up a folder browser for the Export Root path
            DialogResult result = folderBrowserDialog1.ShowDialog();

            if (result == DialogResult.OK)
            {
                Settings.GetInstance().ExportRootPath = folderBrowserDialog1.SelectedPath;
            }
        }

        private void buttonBrowseSDK_Click(object sender, EventArgs e)
        {
            // open up a folder browser for the Runtime SDK path
            DialogResult result = folderBrowserDialog2.ShowDialog();

            if (result == DialogResult.OK)
            {
                Settings.GetInstance().RuntimeSDKRoot = folderBrowserDialog2.SelectedPath;
            }
        }

        private void buttonBrowseConnect_Click(object sender, EventArgs e)
        {
            // open up a file dialog to browse to connect
            DialogResult result = openFileDialogBrowseConnect.ShowDialog();
        }

        private void openFileDialogBrowseConnect_FileOk(object sender, CancelEventArgs e)
        {
            // connect file is ok - set the textbox
            Settings.GetInstance().MorphemeConnectPath = openFileDialogBrowseConnect.FileName;
        }

        // used for showing export and process time
        DateTime StartTimer = new DateTime();

        private void dataGridViewPlatforms_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {             
                // Find the cell under the mouse pointer.
                Point p = dataGridViewPlatforms.PointToClient(new Point(Cursor.Position.X, Cursor.Position.Y));
                DataGridView.HitTestInfo hti = dataGridViewPlatforms.HitTest(p.X, p.Y);
                if (hti.Type == DataGridViewHitTestType.Cell)
                {
                    // Do not allow deleting the last row.  
                    if (hti.RowIndex < dataGridViewPlatforms.Rows.Count - 1)
                    {
                        // Attach the row to the menu items.
                        ToolStripItemCollection items = contextMenuPlatforms.Items;
                        foreach (ToolStripItem item in items)
                        {
                            item.Tag = dataGridViewPlatforms.Rows[hti.RowIndex];
                        }

                        contextMenuPlatforms.Show(Cursor.Position);
                    }
                }
            }
        }

        private void deleteToolStripMenuItemNetworks_Click(object sender, EventArgs e)
        {
            ToolStripItem item = sender as ToolStripItem;

            if (item.Tag != null)
            {
                // Delete the row.
                DataGridViewRow row = item.Tag as DataGridViewRow;
                dataGridViewMCNList.Rows.Remove(row);
            } 
        }

        private void deleteToolStripMenuItemPlatforms_Click(object sender, EventArgs e)
        {
            ToolStripItem item = sender as ToolStripItem;

            if (item.Tag != null)
            {
                // Delete the row.
                DataGridViewRow row = item.Tag as DataGridViewRow;
                dataGridViewPlatforms.Rows.Remove(row);
            } 
        }

        private void dataGridViewMCNList_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                // Find the cell under the mouse pointer.
                Point p = dataGridViewMCNList.PointToClient(new Point(Cursor.Position.X, Cursor.Position.Y));
                DataGridView.HitTestInfo hti = dataGridViewMCNList.HitTest(p.X, p.Y);
                if (hti.Type == DataGridViewHitTestType.Cell)
                {
                    // Do not allow deleting the last row.  
                    if (hti.RowIndex < dataGridViewMCNList.Rows.Count - 1)
                    {
                        // Attach the row to the menu items.
                        ToolStripItemCollection items = contextMenuNetworks.Items;
                        foreach (ToolStripItem item in items)
                        {
                            item.Tag = dataGridViewMCNList.Rows[hti.RowIndex];
                        }

                        contextMenuNetworks.Show(Cursor.Position);
                    }
                }
            }
        }

        private void dataGridViewPlatforms_CellContentDoubleClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void dataGridViewPlatforms_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (dataGridViewPlatforms.Columns[e.ColumnIndex].Name == DataGridViewPlatformNames.AssetCompilerCustomEXEName)
                {
                    // Open a file dialog to choose the asset compiler executable.
                    openFileDialogAssetCompiler.InitialDirectory = Settings.GetInstance().RuntimeSDKRoot;
                    DialogResult dialogResult = openFileDialogAssetCompiler.ShowDialog();
                    if (dialogResult == DialogResult.OK)
                    {
                        string acExe = openFileDialogAssetCompiler.FileNames[0];
                        string localAcExe = MacrosSystem.GetInstance().ConvertToMacro(acExe);
                        Settings.GetInstance().Platforms[e.RowIndex].AssetCompilerCustomEXE = localAcExe;

                        dataGridViewPlatforms.Refresh();
                    }
                }
            }
        }

        // TODO remove if not needed... currently just testing.
        private void dataGridViewPlatforms_CurrentCellDirtyStateChanged(object sender, EventArgs e)
        {
            if (dataGridViewPlatforms.IsCurrentCellDirty)
            {
              dataGridViewPlatforms.CommitEdit(DataGridViewDataErrorContexts.Commit);
            }
        }

        // TODO remove if not needed... currently just testing.
        private void dataGridViewPlatforms_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (dataGridViewPlatforms.Columns[e.ColumnIndex].Name == DataGridViewPlatformNames.AssetCompilerCustomEXEName)
            {
                DataGridViewTextBoxCell textCell = (DataGridViewTextBoxCell)dataGridViewPlatforms.Rows[e.RowIndex].Cells[DataGridViewPlatformNames.AssetCompilerCustomEXEName];
                textCell.Value = Settings.GetInstance().Platforms[e.RowIndex].AssetCompilerCustomEXE;
                dataGridViewPlatforms.Invalidate();             
            }
        }

        private void checkBoxUseDynamicPlugins_CheckStateChanged(object sender, EventArgs e)
        {
            // Show/Hide the asset compiler column depending if the dynamic plugins are in use.
            dataGridViewMCNList.Columns["assetCompilerDataGridViewTextBoxColumn"].Visible = checkBoxUseDynamicPlugins.Checked;

            // Show/Hide Kinect/Move columns depending if the dynamic plugins are in use.
            dataGridViewPlatforms.Columns["Move"].Visible = checkBoxUseDynamicPlugins.Checked;
            dataGridViewPlatforms.Columns["Kinect"].Visible = checkBoxUseDynamicPlugins.Checked;
        }
    }
}
