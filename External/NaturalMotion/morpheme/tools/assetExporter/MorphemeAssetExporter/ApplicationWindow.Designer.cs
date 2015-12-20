namespace NaturalMotion.AssetExporter
{
    partial class ApplicationWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
      this.components = new System.ComponentModel.Container();
      System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
      System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
      System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
      System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ApplicationWindow));
      this.textBoxExportPath = new System.Windows.Forms.TextBox();
      this.label1 = new System.Windows.Forms.Label();
      this.buttonExportAndProcess = new System.Windows.Forms.Button();
      this.buttonSaveMCNList = new System.Windows.Forms.Button();
      this.buttonOpenMCNList = new System.Windows.Forms.Button();
      this.buttonSaveConfig = new System.Windows.Forms.Button();
      this.buttonOpenConfig = new System.Windows.Forms.Button();
      this.textBoxMorphemeConnect = new System.Windows.Forms.TextBox();
      this.label3 = new System.Windows.Forms.Label();
      this.dataGridViewMCNList = new System.Windows.Forms.DataGridView();
      this.use = new System.Windows.Forms.DataGridViewCheckBoxColumn();
      this.mCNFileNameDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.assetCompilerDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewComboBoxColumn();
      this.mCPFileNameDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.processedDataGridViewCheckBoxColumn = new System.Windows.Forms.DataGridViewCheckBoxColumn();
      this.mCNManagerBindingSource = new System.Windows.Forms.BindingSource(this.components);
      this.buttonAddMCNs = new System.Windows.Forms.Button();
      this.openFileDialogMCNs = new System.Windows.Forms.OpenFileDialog();
      this.openFileDialogMCNList = new System.Windows.Forms.OpenFileDialog();
      this.showInfoCheckBox = new System.Windows.Forms.CheckBox();
      this.showGoodNewsCheckBox = new System.Windows.Forms.CheckBox();
      this.showWarningsCheckBox = new System.Windows.Forms.CheckBox();
      this.showErrorsCheckBox = new System.Windows.Forms.CheckBox();
      this.buttonClearMCNList = new System.Windows.Forms.Button();
      this.saveFileDialogCofig = new System.Windows.Forms.SaveFileDialog();
      this.dataGridViewPlatforms = new System.Windows.Forms.DataGridView();
      this.openFileDialogConfig = new System.Windows.Forms.OpenFileDialog();
      this.dataGridViewLogger = new System.Windows.Forms.DataGridView();
      this.severityDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.messageDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.logItemBindingSource = new System.Windows.Forms.BindingSource(this.components);
      this.buttonClearLog = new System.Windows.Forms.Button();
      this.buttonSaveLog = new System.Windows.Forms.Button();
      this.textBoxMorphemeSDKRoot = new System.Windows.Forms.TextBox();
      this.label2 = new System.Windows.Forms.Label();
      this.saveFileDialogLog = new System.Windows.Forms.SaveFileDialog();
      this.progressBar1 = new System.Windows.Forms.ProgressBar();
      this.buttonEditMacros = new System.Windows.Forms.Button();
      this.buttonCreateMCNList = new System.Windows.Forms.Button();
      this.saveFileCreateMCNList = new System.Windows.Forms.SaveFileDialog();
      this.label4 = new System.Windows.Forms.Label();
      this.comboBoxMorphemeVersion = new System.Windows.Forms.ComboBox();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.checkBoxUseDynamicPlugins = new System.Windows.Forms.CheckBox();
      this.buttonBrowseConnect = new System.Windows.Forms.Button();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.textBoxAssetCompilerCommandLine = new System.Windows.Forms.TextBox();
      this.label9 = new System.Windows.Forms.Label();
      this.buttonBrowseSDK = new System.Windows.Forms.Button();
      this.buttonBrowseExport = new System.Windows.Forms.Button();
      this.showGroup = new System.Windows.Forms.GroupBox();
      this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
      this.folderBrowserDialog2 = new System.Windows.Forms.FolderBrowserDialog();
      this.openFileDialogBrowseConnect = new System.Windows.Forms.OpenFileDialog();
      this.label5 = new System.Windows.Forms.Label();
      this.label6 = new System.Windows.Forms.Label();
      this.label7 = new System.Windows.Forms.Label();
      this.label8 = new System.Windows.Forms.Label();
      this.contextMenuPlatforms = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.deleteToolStripMenuItemPlatforms = new System.Windows.Forms.ToolStripMenuItem();
      this.contextMenuNetworks = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.deleteToolStripMenuItemNetworks = new System.Windows.Forms.ToolStripMenuItem();
      this.buttonExport = new System.Windows.Forms.Button();
      this.openFileDialogAssetCompiler = new System.Windows.Forms.OpenFileDialog();
      ((System.ComponentModel.ISupportInitialize)(this.dataGridViewMCNList)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.mCNManagerBindingSource)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.dataGridViewPlatforms)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.dataGridViewLogger)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.logItemBindingSource)).BeginInit();
      this.groupBox1.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.showGroup.SuspendLayout();
      this.contextMenuPlatforms.SuspendLayout();
      this.contextMenuNetworks.SuspendLayout();
      this.SuspendLayout();
      // 
      // textBoxExportPath
      // 
      this.textBoxExportPath.BackColor = System.Drawing.Color.White;
      this.textBoxExportPath.Location = new System.Drawing.Point(9, 68);
      this.textBoxExportPath.Name = "textBoxExportPath";
      this.textBoxExportPath.Size = new System.Drawing.Size(435, 20);
      this.textBoxExportPath.TabIndex = 2;
      this.textBoxExportPath.TabStop = false;
      this.textBoxExportPath.TextChanged += new System.EventHandler(this.textBoxExportPath_TextChanged);
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(6, 52);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(58, 13);
      this.label1.TabIndex = 11;
      this.label1.Text = "Export root";
      // 
      // buttonExportAndProcess
      // 
      this.buttonExportAndProcess.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonExportAndProcess.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.buttonExportAndProcess.Location = new System.Drawing.Point(1168, 494);
      this.buttonExportAndProcess.Name = "buttonExportAndProcess";
      this.buttonExportAndProcess.Size = new System.Drawing.Size(82, 39);
      this.buttonExportAndProcess.TabIndex = 23;
      this.buttonExportAndProcess.TabStop = false;
      this.buttonExportAndProcess.Text = "Export and Process";
      this.buttonExportAndProcess.UseVisualStyleBackColor = true;
      this.buttonExportAndProcess.Click += new System.EventHandler(this.buttonExportAndProcessAll_Click);
      // 
      // buttonSaveMCNList
      // 
      this.buttonSaveMCNList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonSaveMCNList.Location = new System.Drawing.Point(1168, 345);
      this.buttonSaveMCNList.Name = "buttonSaveMCNList";
      this.buttonSaveMCNList.Size = new System.Drawing.Size(82, 36);
      this.buttonSaveMCNList.TabIndex = 26;
      this.buttonSaveMCNList.TabStop = false;
      this.buttonSaveMCNList.Text = "Save List";
      this.buttonSaveMCNList.UseVisualStyleBackColor = true;
      this.buttonSaveMCNList.Click += new System.EventHandler(this.buttonSaveMCNList_Click);
      // 
      // buttonOpenMCNList
      // 
      this.buttonOpenMCNList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonOpenMCNList.Location = new System.Drawing.Point(1168, 308);
      this.buttonOpenMCNList.Name = "buttonOpenMCNList";
      this.buttonOpenMCNList.Size = new System.Drawing.Size(82, 36);
      this.buttonOpenMCNList.TabIndex = 27;
      this.buttonOpenMCNList.TabStop = false;
      this.buttonOpenMCNList.Text = "Open List";
      this.buttonOpenMCNList.UseVisualStyleBackColor = true;
      this.buttonOpenMCNList.Click += new System.EventHandler(this.buttonOpenMCNList_Click);
      // 
      // buttonSaveConfig
      // 
      this.buttonSaveConfig.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonSaveConfig.Location = new System.Drawing.Point(1168, 65);
      this.buttonSaveConfig.Name = "buttonSaveConfig";
      this.buttonSaveConfig.Size = new System.Drawing.Size(82, 36);
      this.buttonSaveConfig.TabIndex = 28;
      this.buttonSaveConfig.TabStop = false;
      this.buttonSaveConfig.Text = "Save Config";
      this.buttonSaveConfig.UseVisualStyleBackColor = true;
      this.buttonSaveConfig.Click += new System.EventHandler(this.buttonSaveConfig_Click);
      // 
      // buttonOpenConfig
      // 
      this.buttonOpenConfig.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonOpenConfig.Location = new System.Drawing.Point(1168, 23);
      this.buttonOpenConfig.Name = "buttonOpenConfig";
      this.buttonOpenConfig.Size = new System.Drawing.Size(82, 36);
      this.buttonOpenConfig.TabIndex = 29;
      this.buttonOpenConfig.TabStop = false;
      this.buttonOpenConfig.Text = "Open Config";
      this.buttonOpenConfig.UseVisualStyleBackColor = true;
      this.buttonOpenConfig.Click += new System.EventHandler(this.buttonOpenConfig_Click);
      // 
      // textBoxMorphemeConnect
      // 
      this.textBoxMorphemeConnect.Location = new System.Drawing.Point(7, 88);
      this.textBoxMorphemeConnect.Name = "textBoxMorphemeConnect";
      this.textBoxMorphemeConnect.Size = new System.Drawing.Size(435, 20);
      this.textBoxMorphemeConnect.TabIndex = 41;
      this.textBoxMorphemeConnect.TabStop = false;
      this.textBoxMorphemeConnect.TextChanged += new System.EventHandler(this.textBoxMorphemeConnect_TextChanged);
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(4, 72);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(100, 13);
      this.label3.TabIndex = 42;
      this.label3.Text = "Connect install path";
      // 
      // dataGridViewMCNList
      // 
      this.dataGridViewMCNList.AllowUserToResizeRows = false;
      this.dataGridViewMCNList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.dataGridViewMCNList.AutoGenerateColumns = false;
      this.dataGridViewMCNList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.dataGridViewMCNList.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.use,
            this.mCNFileNameDataGridViewTextBoxColumn,
            this.assetCompilerDataGridViewTextBoxColumn,
            this.mCPFileNameDataGridViewTextBoxColumn,
            this.processedDataGridViewCheckBoxColumn});
      this.dataGridViewMCNList.DataSource = this.mCNManagerBindingSource;
      this.dataGridViewMCNList.EditMode = System.Windows.Forms.DataGridViewEditMode.EditOnEnter;
      this.dataGridViewMCNList.Enabled = false;
      this.dataGridViewMCNList.Location = new System.Drawing.Point(10, 272);
      this.dataGridViewMCNList.Name = "dataGridViewMCNList";
      this.dataGridViewMCNList.RowHeadersVisible = false;
      dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
      this.dataGridViewMCNList.RowsDefaultCellStyle = dataGridViewCellStyle1;
      this.dataGridViewMCNList.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
      this.dataGridViewMCNList.Size = new System.Drawing.Size(1152, 204);
      this.dataGridViewMCNList.TabIndex = 43;
      this.dataGridViewMCNList.TabStop = false;
      this.dataGridViewMCNList.CellMouseDown += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.dataGridViewMCNList_CellMouseDown);
      this.dataGridViewMCNList.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView2_CellValueChanged);
      this.dataGridViewMCNList.RowsAdded += new System.Windows.Forms.DataGridViewRowsAddedEventHandler(this.dataGridView2_RowsAdded);
      this.dataGridViewMCNList.RowsRemoved += new System.Windows.Forms.DataGridViewRowsRemovedEventHandler(this.dataGridView2_RowsRemoved);
      // 
      // use
      // 
      this.use.DataPropertyName = "Use";
      this.use.HeaderText = "Use";
      this.use.MinimumWidth = 25;
      this.use.Name = "use";
      this.use.Width = 30;
      // 
      // mCNFileNameDataGridViewTextBoxColumn
      // 
      this.mCNFileNameDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
      this.mCNFileNameDataGridViewTextBoxColumn.DataPropertyName = "MCNFileName";
      this.mCNFileNameDataGridViewTextBoxColumn.HeaderText = "Network file";
      this.mCNFileNameDataGridViewTextBoxColumn.Name = "mCNFileNameDataGridViewTextBoxColumn";
      // 
      // assetCompilerDataGridViewTextBoxColumn
      // 
      this.assetCompilerDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
      this.assetCompilerDataGridViewTextBoxColumn.DataPropertyName = "AssetCompiler";
      this.assetCompilerDataGridViewTextBoxColumn.HeaderText = "Asset Compiler";
      this.assetCompilerDataGridViewTextBoxColumn.Name = "assetCompilerDataGridViewTextBoxColumn";
      this.assetCompilerDataGridViewTextBoxColumn.Resizable = System.Windows.Forms.DataGridViewTriState.True;
      this.assetCompilerDataGridViewTextBoxColumn.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
      this.assetCompilerDataGridViewTextBoxColumn.Width = 101;
      // 
      // mCPFileNameDataGridViewTextBoxColumn
      // 
      this.mCPFileNameDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
      this.mCPFileNameDataGridViewTextBoxColumn.DataPropertyName = "MCPFileName";
      this.mCPFileNameDataGridViewTextBoxColumn.HeaderText = "Project File";
      this.mCPFileNameDataGridViewTextBoxColumn.Name = "mCPFileNameDataGridViewTextBoxColumn";
      // 
      // processedDataGridViewCheckBoxColumn
      // 
      this.processedDataGridViewCheckBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.ColumnHeader;
      this.processedDataGridViewCheckBoxColumn.DataPropertyName = "Processed";
      this.processedDataGridViewCheckBoxColumn.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
      this.processedDataGridViewCheckBoxColumn.HeaderText = "Processed";
      this.processedDataGridViewCheckBoxColumn.Name = "processedDataGridViewCheckBoxColumn";
      this.processedDataGridViewCheckBoxColumn.ReadOnly = true;
      this.processedDataGridViewCheckBoxColumn.Width = 63;
      // 
      // mCNManagerBindingSource
      // 
      this.mCNManagerBindingSource.DataSource = typeof(NaturalMotion.AssetExporter.MCNCollection);
      // 
      // buttonAddMCNs
      // 
      this.buttonAddMCNs.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonAddMCNs.FlatAppearance.BorderColor = System.Drawing.Color.White;
      this.buttonAddMCNs.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.buttonAddMCNs.Location = new System.Drawing.Point(1168, 382);
      this.buttonAddMCNs.Name = "buttonAddMCNs";
      this.buttonAddMCNs.Size = new System.Drawing.Size(82, 36);
      this.buttonAddMCNs.TabIndex = 44;
      this.buttonAddMCNs.TabStop = false;
      this.buttonAddMCNs.Text = "Add Networks";
      this.buttonAddMCNs.UseVisualStyleBackColor = true;
      this.buttonAddMCNs.Click += new System.EventHandler(this.button2_Click);
      // 
      // openFileDialogMCNs
      // 
      this.openFileDialogMCNs.Filter = "morpheme networks|*.mcn";
      this.openFileDialogMCNs.Multiselect = true;
      this.openFileDialogMCNs.Title = "Add Networks";
      // 
      // openFileDialogMCNList
      // 
      this.openFileDialogMCNList.Filter = "morpheme network lists|*.lst";
      this.openFileDialogMCNList.RestoreDirectory = true;
      this.openFileDialogMCNList.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialogMCN_FileOk);
      // 
      // showInfoCheckBox
      // 
      this.showInfoCheckBox.AutoSize = true;
      this.showInfoCheckBox.Checked = true;
      this.showInfoCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
      this.showInfoCheckBox.Location = new System.Drawing.Point(6, 42);
      this.showInfoCheckBox.Name = "showInfoCheckBox";
      this.showInfoCheckBox.Size = new System.Drawing.Size(43, 17);
      this.showInfoCheckBox.TabIndex = 45;
      this.showInfoCheckBox.TabStop = false;
      this.showInfoCheckBox.Text = "info";
      this.showInfoCheckBox.UseVisualStyleBackColor = true;
      this.showInfoCheckBox.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
      // 
      // showGoodNewsCheckBox
      // 
      this.showGoodNewsCheckBox.AutoSize = true;
      this.showGoodNewsCheckBox.Checked = true;
      this.showGoodNewsCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
      this.showGoodNewsCheckBox.Location = new System.Drawing.Point(6, 19);
      this.showGoodNewsCheckBox.Name = "showGoodNewsCheckBox";
      this.showGoodNewsCheckBox.Size = new System.Drawing.Size(78, 17);
      this.showGoodNewsCheckBox.TabIndex = 46;
      this.showGoodNewsCheckBox.TabStop = false;
      this.showGoodNewsCheckBox.Text = "good news";
      this.showGoodNewsCheckBox.UseVisualStyleBackColor = true;
      this.showGoodNewsCheckBox.CheckedChanged += new System.EventHandler(this.checkBox2_CheckedChanged);
      // 
      // showWarningsCheckBox
      // 
      this.showWarningsCheckBox.AutoSize = true;
      this.showWarningsCheckBox.Checked = true;
      this.showWarningsCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
      this.showWarningsCheckBox.Location = new System.Drawing.Point(6, 65);
      this.showWarningsCheckBox.Name = "showWarningsCheckBox";
      this.showWarningsCheckBox.Size = new System.Drawing.Size(63, 17);
      this.showWarningsCheckBox.TabIndex = 47;
      this.showWarningsCheckBox.TabStop = false;
      this.showWarningsCheckBox.Text = "warning";
      this.showWarningsCheckBox.UseVisualStyleBackColor = true;
      this.showWarningsCheckBox.CheckedChanged += new System.EventHandler(this.checkBox3_CheckedChanged);
      // 
      // showErrorsCheckBox
      // 
      this.showErrorsCheckBox.AutoSize = true;
      this.showErrorsCheckBox.Checked = true;
      this.showErrorsCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
      this.showErrorsCheckBox.Location = new System.Drawing.Point(6, 88);
      this.showErrorsCheckBox.Name = "showErrorsCheckBox";
      this.showErrorsCheckBox.Size = new System.Drawing.Size(47, 17);
      this.showErrorsCheckBox.TabIndex = 48;
      this.showErrorsCheckBox.TabStop = false;
      this.showErrorsCheckBox.Text = "error";
      this.showErrorsCheckBox.UseVisualStyleBackColor = true;
      this.showErrorsCheckBox.CheckedChanged += new System.EventHandler(this.checkBox4_CheckedChanged);
      // 
      // buttonClearMCNList
      // 
      this.buttonClearMCNList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonClearMCNList.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.buttonClearMCNList.Location = new System.Drawing.Point(1168, 418);
      this.buttonClearMCNList.Name = "buttonClearMCNList";
      this.buttonClearMCNList.Size = new System.Drawing.Size(82, 36);
      this.buttonClearMCNList.TabIndex = 50;
      this.buttonClearMCNList.TabStop = false;
      this.buttonClearMCNList.Text = "Clear List";
      this.buttonClearMCNList.UseVisualStyleBackColor = true;
      this.buttonClearMCNList.Click += new System.EventHandler(this.buttonClear_Click);
      // 
      // saveFileDialogCofig
      // 
      this.saveFileDialogCofig.DefaultExt = "xml";
      this.saveFileDialogCofig.Filter = "configs|*.cfg";
      this.saveFileDialogCofig.FileOk += new System.ComponentModel.CancelEventHandler(this.saveFileDialog2_FileOk);
      // 
      // dataGridViewPlatforms
      // 
      this.dataGridViewPlatforms.AllowUserToResizeRows = false;
      this.dataGridViewPlatforms.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
      dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Control;
      dataGridViewCellStyle2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.WindowText;
      dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Control;
      dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.WindowText;
      dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
      this.dataGridViewPlatforms.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle2;
      this.dataGridViewPlatforms.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
      dataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Window;
      dataGridViewCellStyle3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      dataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.ControlText;
      dataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Window;
      dataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.ControlText;
      dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
      this.dataGridViewPlatforms.DefaultCellStyle = dataGridViewCellStyle3;
      this.dataGridViewPlatforms.EditMode = System.Windows.Forms.DataGridViewEditMode.EditOnEnter;
      this.dataGridViewPlatforms.Location = new System.Drawing.Point(505, 25);
      this.dataGridViewPlatforms.Name = "dataGridViewPlatforms";
      this.dataGridViewPlatforms.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
      this.dataGridViewPlatforms.Size = new System.Drawing.Size(657, 228);
      this.dataGridViewPlatforms.TabIndex = 51;
      this.dataGridViewPlatforms.TabStop = false;
      this.dataGridViewPlatforms.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridViewPlatforms_CellContentClick);
      this.dataGridViewPlatforms.CellContentDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridViewPlatforms_CellContentDoubleClick);
      this.dataGridViewPlatforms.CellMouseDoubleClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.dataGridViewPlatforms_CellMouseDoubleClick);
      this.dataGridViewPlatforms.CellMouseDown += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.dataGridViewPlatforms_CellMouseDown);
      this.dataGridViewPlatforms.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridViewPlatforms_CellValueChanged);
      this.dataGridViewPlatforms.CurrentCellDirtyStateChanged += new System.EventHandler(this.dataGridViewPlatforms_CurrentCellDirtyStateChanged);
      this.dataGridViewPlatforms.RowsAdded += new System.Windows.Forms.DataGridViewRowsAddedEventHandler(this.dataGridViewPlatforms_RowsAdded);
      // 
      // openFileDialogConfig
      // 
      this.openFileDialogConfig.Filter = "configs|*.cfg";
      this.openFileDialogConfig.RestoreDirectory = true;
      this.openFileDialogConfig.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialog3_FileOk);
      // 
      // dataGridViewLogger
      // 
      this.dataGridViewLogger.AllowUserToAddRows = false;
      this.dataGridViewLogger.AllowUserToDeleteRows = false;
      this.dataGridViewLogger.AllowUserToResizeRows = false;
      this.dataGridViewLogger.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.dataGridViewLogger.AutoGenerateColumns = false;
      this.dataGridViewLogger.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.dataGridViewLogger.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.severityDataGridViewTextBoxColumn,
            this.messageDataGridViewTextBoxColumn});
      this.dataGridViewLogger.DataSource = this.logItemBindingSource;
      dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
      dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.Window;
      dataGridViewCellStyle4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      dataGridViewCellStyle4.ForeColor = System.Drawing.SystemColors.ControlText;
      dataGridViewCellStyle4.SelectionBackColor = System.Drawing.SystemColors.Window;
      dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.Desktop;
      dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
      this.dataGridViewLogger.DefaultCellStyle = dataGridViewCellStyle4;
      this.dataGridViewLogger.EditMode = System.Windows.Forms.DataGridViewEditMode.EditOnEnter;
      this.dataGridViewLogger.Location = new System.Drawing.Point(10, 579);
      this.dataGridViewLogger.Name = "dataGridViewLogger";
      this.dataGridViewLogger.RowHeadersVisible = false;
      this.dataGridViewLogger.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
      this.dataGridViewLogger.ShowCellErrors = false;
      this.dataGridViewLogger.Size = new System.Drawing.Size(1152, 162);
      this.dataGridViewLogger.TabIndex = 52;
      this.dataGridViewLogger.TabStop = false;
      this.dataGridViewLogger.RowsAdded += new System.Windows.Forms.DataGridViewRowsAddedEventHandler(this.dataGridViewLogger_RowsAdded);
      // 
      // severityDataGridViewTextBoxColumn
      // 
      this.severityDataGridViewTextBoxColumn.DataPropertyName = "Severity";
      this.severityDataGridViewTextBoxColumn.HeaderText = "Severity";
      this.severityDataGridViewTextBoxColumn.Name = "severityDataGridViewTextBoxColumn";
      this.severityDataGridViewTextBoxColumn.ReadOnly = true;
      this.severityDataGridViewTextBoxColumn.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
      this.severityDataGridViewTextBoxColumn.Width = 75;
      // 
      // messageDataGridViewTextBoxColumn
      // 
      this.messageDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
      this.messageDataGridViewTextBoxColumn.DataPropertyName = "Message";
      this.messageDataGridViewTextBoxColumn.HeaderText = "Message";
      this.messageDataGridViewTextBoxColumn.Name = "messageDataGridViewTextBoxColumn";
      this.messageDataGridViewTextBoxColumn.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
      // 
      // logItemBindingSource
      // 
      this.logItemBindingSource.DataSource = typeof(NaturalMotion.AssetExporter.Logging.LogItem);
      // 
      // buttonClearLog
      // 
      this.buttonClearLog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonClearLog.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.buttonClearLog.Location = new System.Drawing.Point(1173, 696);
      this.buttonClearLog.Name = "buttonClearLog";
      this.buttonClearLog.Size = new System.Drawing.Size(82, 25);
      this.buttonClearLog.TabIndex = 53;
      this.buttonClearLog.TabStop = false;
      this.buttonClearLog.Text = "Clear Log";
      this.buttonClearLog.UseVisualStyleBackColor = true;
      this.buttonClearLog.Click += new System.EventHandler(this.buttonClearLog_Click);
      // 
      // buttonSaveLog
      // 
      this.buttonSaveLog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonSaveLog.Location = new System.Drawing.Point(1173, 727);
      this.buttonSaveLog.Name = "buttonSaveLog";
      this.buttonSaveLog.Size = new System.Drawing.Size(82, 25);
      this.buttonSaveLog.TabIndex = 54;
      this.buttonSaveLog.TabStop = false;
      this.buttonSaveLog.Text = "Save Log";
      this.buttonSaveLog.UseVisualStyleBackColor = true;
      this.buttonSaveLog.Click += new System.EventHandler(this.buttonSaveLog_Click);
      // 
      // textBoxMorphemeSDKRoot
      // 
      this.textBoxMorphemeSDKRoot.Location = new System.Drawing.Point(9, 29);
      this.textBoxMorphemeSDKRoot.Name = "textBoxMorphemeSDKRoot";
      this.textBoxMorphemeSDKRoot.Size = new System.Drawing.Size(435, 20);
      this.textBoxMorphemeSDKRoot.TabIndex = 55;
      this.textBoxMorphemeSDKRoot.TabStop = false;
      this.textBoxMorphemeSDKRoot.TextChanged += new System.EventHandler(this.textBoxMorphemeSDKRoot_TextChanged);
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(6, 13);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(92, 13);
      this.label2.TabIndex = 56;
      this.label2.Text = "Runtime SDK root";
      // 
      // saveFileDialogLog
      // 
      this.saveFileDialogLog.DefaultExt = "log";
      this.saveFileDialogLog.Filter = "log|*.log";
      this.saveFileDialogLog.FileOk += new System.ComponentModel.CancelEventHandler(this.saveFileDialogLog_FileOk);
      // 
      // progressBar1
      // 
      this.progressBar1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.progressBar1.Location = new System.Drawing.Point(10, 521);
      this.progressBar1.Name = "progressBar1";
      this.progressBar1.Size = new System.Drawing.Size(1152, 21);
      this.progressBar1.Step = 6;
      this.progressBar1.TabIndex = 57;
      // 
      // buttonEditMacros
      // 
      this.buttonEditMacros.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonEditMacros.Location = new System.Drawing.Point(1168, 217);
      this.buttonEditMacros.Name = "buttonEditMacros";
      this.buttonEditMacros.Size = new System.Drawing.Size(82, 36);
      this.buttonEditMacros.TabIndex = 61;
      this.buttonEditMacros.TabStop = false;
      this.buttonEditMacros.Text = "Edit Config Macros...";
      this.buttonEditMacros.UseVisualStyleBackColor = true;
      this.buttonEditMacros.Click += new System.EventHandler(this.buttonEditMacros_Click);
      // 
      // buttonCreateMCNList
      // 
      this.buttonCreateMCNList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonCreateMCNList.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.buttonCreateMCNList.Location = new System.Drawing.Point(1168, 271);
      this.buttonCreateMCNList.Name = "buttonCreateMCNList";
      this.buttonCreateMCNList.Size = new System.Drawing.Size(82, 36);
      this.buttonCreateMCNList.TabIndex = 62;
      this.buttonCreateMCNList.TabStop = false;
      this.buttonCreateMCNList.Text = "Create List";
      this.buttonCreateMCNList.UseVisualStyleBackColor = true;
      this.buttonCreateMCNList.Click += new System.EventHandler(this.buttonCreateMCNList_Click);
      // 
      // saveFileCreateMCNList
      // 
      this.saveFileCreateMCNList.DefaultExt = "xml";
      this.saveFileCreateMCNList.Filter = "morpheme network lists|*.lst";
      this.saveFileCreateMCNList.RestoreDirectory = true;
      this.saveFileCreateMCNList.Title = "Create New MCN List";
      this.saveFileCreateMCNList.FileOk += new System.ComponentModel.CancelEventHandler(this.saveFileCreateMCNList_FileOk);
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point(4, 22);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(44, 13);
      this.label4.TabIndex = 63;
      this.label4.Text = "Product";
      // 
      // comboBoxMorphemeVersion
      // 
      this.comboBoxMorphemeVersion.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.comboBoxMorphemeVersion.FormattingEnabled = true;
      this.comboBoxMorphemeVersion.Location = new System.Drawing.Point(6, 38);
      this.comboBoxMorphemeVersion.Name = "comboBoxMorphemeVersion";
      this.comboBoxMorphemeVersion.Size = new System.Drawing.Size(148, 21);
      this.comboBoxMorphemeVersion.TabIndex = 64;
      this.comboBoxMorphemeVersion.TabStop = false;
      this.comboBoxMorphemeVersion.SelectedIndexChanged += new System.EventHandler(this.comboBoxMorphemeVersion_SelectedIndexChanged);
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.checkBoxUseDynamicPlugins);
      this.groupBox1.Controls.Add(this.buttonBrowseConnect);
      this.groupBox1.Controls.Add(this.label4);
      this.groupBox1.Controls.Add(this.comboBoxMorphemeVersion);
      this.groupBox1.Controls.Add(this.label3);
      this.groupBox1.Controls.Add(this.textBoxMorphemeConnect);
      this.groupBox1.Location = new System.Drawing.Point(12, 0);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(487, 118);
      this.groupBox1.TabIndex = 65;
      this.groupBox1.TabStop = false;
      // 
      // checkBoxUseDynamicPlugins
      // 
      this.checkBoxUseDynamicPlugins.AutoSize = true;
      this.checkBoxUseDynamicPlugins.Location = new System.Drawing.Point(168, 42);
      this.checkBoxUseDynamicPlugins.Name = "checkBoxUseDynamicPlugins";
      this.checkBoxUseDynamicPlugins.Size = new System.Drawing.Size(126, 17);
      this.checkBoxUseDynamicPlugins.TabIndex = 65;
      this.checkBoxUseDynamicPlugins.Text = "Use Dynamic Plugins";
      this.checkBoxUseDynamicPlugins.UseVisualStyleBackColor = true;
      this.checkBoxUseDynamicPlugins.CheckStateChanged += new System.EventHandler(this.checkBoxUseDynamicPlugins_CheckStateChanged);
      // 
      // buttonBrowseConnect
      // 
      this.buttonBrowseConnect.Enabled = false;
      this.buttonBrowseConnect.Location = new System.Drawing.Point(448, 88);
      this.buttonBrowseConnect.Name = "buttonBrowseConnect";
      this.buttonBrowseConnect.Size = new System.Drawing.Size(25, 20);
      this.buttonBrowseConnect.TabIndex = 59;
      this.buttonBrowseConnect.TabStop = false;
      this.buttonBrowseConnect.Text = "...";
      this.buttonBrowseConnect.UseVisualStyleBackColor = true;
      this.buttonBrowseConnect.Click += new System.EventHandler(this.buttonBrowseConnect_Click);
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.textBoxAssetCompilerCommandLine);
      this.groupBox2.Controls.Add(this.label9);
      this.groupBox2.Controls.Add(this.buttonBrowseSDK);
      this.groupBox2.Controls.Add(this.buttonBrowseExport);
      this.groupBox2.Controls.Add(this.label2);
      this.groupBox2.Controls.Add(this.label1);
      this.groupBox2.Controls.Add(this.textBoxExportPath);
      this.groupBox2.Controls.Add(this.textBoxMorphemeSDKRoot);
      this.groupBox2.Location = new System.Drawing.Point(10, 115);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(487, 138);
      this.groupBox2.TabIndex = 66;
      this.groupBox2.TabStop = false;
      // 
      // textBoxAssetCompilerCommandLine
      // 
      this.textBoxAssetCompilerCommandLine.BackColor = System.Drawing.Color.White;
      this.textBoxAssetCompilerCommandLine.Location = new System.Drawing.Point(9, 111);
      this.textBoxAssetCompilerCommandLine.Name = "textBoxAssetCompilerCommandLine";
      this.textBoxAssetCompilerCommandLine.Size = new System.Drawing.Size(467, 20);
      this.textBoxAssetCompilerCommandLine.TabIndex = 60;
      this.textBoxAssetCompilerCommandLine.TabStop = false;
      // 
      // label9
      // 
      this.label9.AutoSize = true;
      this.label9.Location = new System.Drawing.Point(6, 95);
      this.label9.Name = "label9";
      this.label9.Size = new System.Drawing.Size(143, 13);
      this.label9.TabIndex = 59;
      this.label9.Text = "Asset compiler command line";
      // 
      // buttonBrowseSDK
      // 
      this.buttonBrowseSDK.Location = new System.Drawing.Point(450, 28);
      this.buttonBrowseSDK.Name = "buttonBrowseSDK";
      this.buttonBrowseSDK.Size = new System.Drawing.Size(25, 20);
      this.buttonBrowseSDK.TabIndex = 58;
      this.buttonBrowseSDK.TabStop = false;
      this.buttonBrowseSDK.Text = "...";
      this.buttonBrowseSDK.UseVisualStyleBackColor = true;
      this.buttonBrowseSDK.Click += new System.EventHandler(this.buttonBrowseSDK_Click);
      // 
      // buttonBrowseExport
      // 
      this.buttonBrowseExport.Location = new System.Drawing.Point(450, 68);
      this.buttonBrowseExport.Name = "buttonBrowseExport";
      this.buttonBrowseExport.Size = new System.Drawing.Size(25, 20);
      this.buttonBrowseExport.TabIndex = 57;
      this.buttonBrowseExport.TabStop = false;
      this.buttonBrowseExport.Text = "...";
      this.buttonBrowseExport.UseVisualStyleBackColor = true;
      this.buttonBrowseExport.Click += new System.EventHandler(this.buttonBrowseExport_Click);
      // 
      // showGroup
      // 
      this.showGroup.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.showGroup.Controls.Add(this.showGoodNewsCheckBox);
      this.showGroup.Controls.Add(this.showInfoCheckBox);
      this.showGroup.Controls.Add(this.showWarningsCheckBox);
      this.showGroup.Controls.Add(this.showErrorsCheckBox);
      this.showGroup.Location = new System.Drawing.Point(1173, 579);
      this.showGroup.Name = "showGroup";
      this.showGroup.Size = new System.Drawing.Size(86, 111);
      this.showGroup.TabIndex = 67;
      this.showGroup.TabStop = false;
      this.showGroup.Text = "Show";
      // 
      // folderBrowserDialog1
      // 
      this.folderBrowserDialog1.RootFolder = System.Environment.SpecialFolder.MyComputer;
      // 
      // folderBrowserDialog2
      // 
      this.folderBrowserDialog2.RootFolder = System.Environment.SpecialFolder.MyComputer;
      this.folderBrowserDialog2.ShowNewFolderButton = false;
      // 
      // openFileDialogBrowseConnect
      // 
      this.openFileDialogBrowseConnect.FileName = "morphemeConnect.exe";
      this.openFileDialogBrowseConnect.Filter = "|*.exe";
      this.openFileDialogBrowseConnect.Title = "Locate Connect";
      this.openFileDialogBrowseConnect.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialogBrowseConnect_FileOk);
      // 
      // label5
      // 
      this.label5.AutoSize = true;
      this.label5.Location = new System.Drawing.Point(502, 9);
      this.label5.Name = "label5";
      this.label5.Size = new System.Drawing.Size(50, 13);
      this.label5.TabIndex = 65;
      this.label5.Text = "Platforms";
      // 
      // label6
      // 
      this.label6.AutoSize = true;
      this.label6.Location = new System.Drawing.Point(7, 256);
      this.label6.Name = "label6";
      this.label6.Size = new System.Drawing.Size(50, 13);
      this.label6.TabIndex = 68;
      this.label6.Text = "MCN List";
      // 
      // label7
      // 
      this.label7.AutoSize = true;
      this.label7.Location = new System.Drawing.Point(7, 560);
      this.label7.Name = "label7";
      this.label7.Size = new System.Drawing.Size(25, 13);
      this.label7.TabIndex = 69;
      this.label7.Text = "Log";
      // 
      // label8
      // 
      this.label8.AutoSize = true;
      this.label8.Location = new System.Drawing.Point(7, 505);
      this.label8.Name = "label8";
      this.label8.Size = new System.Drawing.Size(48, 13);
      this.label8.TabIndex = 70;
      this.label8.Text = "Progress";
      // 
      // contextMenuPlatforms
      // 
      this.contextMenuPlatforms.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteToolStripMenuItemPlatforms});
      this.contextMenuPlatforms.Name = "contextMenuPlatforms";
      this.contextMenuPlatforms.Size = new System.Drawing.Size(108, 26);
      // 
      // deleteToolStripMenuItemPlatforms
      // 
      this.deleteToolStripMenuItemPlatforms.Name = "deleteToolStripMenuItemPlatforms";
      this.deleteToolStripMenuItemPlatforms.Size = new System.Drawing.Size(107, 22);
      this.deleteToolStripMenuItemPlatforms.Text = "Delete";
      this.deleteToolStripMenuItemPlatforms.Click += new System.EventHandler(this.deleteToolStripMenuItemPlatforms_Click);
      // 
      // contextMenuNetworks
      // 
      this.contextMenuNetworks.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteToolStripMenuItemNetworks});
      this.contextMenuNetworks.Name = "contextMenuNetworks";
      this.contextMenuNetworks.Size = new System.Drawing.Size(108, 26);
      // 
      // deleteToolStripMenuItemNetworks
      // 
      this.deleteToolStripMenuItemNetworks.Name = "deleteToolStripMenuItemNetworks";
      this.deleteToolStripMenuItemNetworks.Size = new System.Drawing.Size(107, 22);
      this.deleteToolStripMenuItemNetworks.Text = "Delete";
      this.deleteToolStripMenuItemNetworks.Click += new System.EventHandler(this.deleteToolStripMenuItemNetworks_Click);
      // 
      // buttonExport
      // 
      this.buttonExport.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonExport.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.buttonExport.Location = new System.Drawing.Point(1168, 455);
      this.buttonExport.Name = "buttonExport";
      this.buttonExport.Size = new System.Drawing.Size(82, 39);
      this.buttonExport.TabIndex = 71;
      this.buttonExport.TabStop = false;
      this.buttonExport.Text = "Export";
      this.buttonExport.UseVisualStyleBackColor = true;
      this.buttonExport.Click += new System.EventHandler(this.buttonExportAll_Click);
      // 
      // openFileDialogAssetCompiler
      // 
      this.openFileDialogAssetCompiler.Filter = "|*.exe";
      this.openFileDialogAssetCompiler.Title = "Locate Asset Compiler";
      // 
      // ApplicationWindow
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(1264, 753);
      this.Controls.Add(this.buttonExport);
      this.Controls.Add(this.label8);
      this.Controls.Add(this.label7);
      this.Controls.Add(this.label6);
      this.Controls.Add(this.label5);
      this.Controls.Add(this.showGroup);
      this.Controls.Add(this.groupBox2);
      this.Controls.Add(this.groupBox1);
      this.Controls.Add(this.buttonCreateMCNList);
      this.Controls.Add(this.buttonEditMacros);
      this.Controls.Add(this.progressBar1);
      this.Controls.Add(this.buttonSaveLog);
      this.Controls.Add(this.buttonClearLog);
      this.Controls.Add(this.dataGridViewLogger);
      this.Controls.Add(this.dataGridViewPlatforms);
      this.Controls.Add(this.buttonClearMCNList);
      this.Controls.Add(this.buttonAddMCNs);
      this.Controls.Add(this.dataGridViewMCNList);
      this.Controls.Add(this.buttonOpenConfig);
      this.Controls.Add(this.buttonSaveConfig);
      this.Controls.Add(this.buttonOpenMCNList);
      this.Controls.Add(this.buttonSaveMCNList);
      this.Controls.Add(this.buttonExportAndProcess);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "ApplicationWindow";
      this.Text = "Morpheme Asset Exporter";
      ((System.ComponentModel.ISupportInitialize)(this.dataGridViewMCNList)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.mCNManagerBindingSource)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.dataGridViewPlatforms)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.dataGridViewLogger)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.logItemBindingSource)).EndInit();
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      this.groupBox2.ResumeLayout(false);
      this.groupBox2.PerformLayout();
      this.showGroup.ResumeLayout(false);
      this.showGroup.PerformLayout();
      this.contextMenuPlatforms.ResumeLayout(false);
      this.contextMenuNetworks.ResumeLayout(false);
      this.ResumeLayout(false);
      this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBoxExportPath;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button buttonExportAndProcess;
        private System.Windows.Forms.Button buttonSaveMCNList;
        private System.Windows.Forms.Button buttonOpenMCNList;
        private System.Windows.Forms.Button buttonSaveConfig;
        private System.Windows.Forms.Button buttonOpenConfig;
        private System.Windows.Forms.TextBox textBoxMorphemeConnect;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.BindingSource mCNManagerBindingSource;
        private System.Windows.Forms.Button buttonAddMCNs;
        private System.Windows.Forms.OpenFileDialog openFileDialogMCNs;
        private System.Windows.Forms.OpenFileDialog openFileDialogMCNList;
        private System.Windows.Forms.CheckBox showErrorsCheckBox;
        private System.Windows.Forms.CheckBox showWarningsCheckBox;
        private System.Windows.Forms.CheckBox showGoodNewsCheckBox;
        private System.Windows.Forms.CheckBox showInfoCheckBox;
        private System.Windows.Forms.Button buttonClearMCNList;
        private System.Windows.Forms.SaveFileDialog saveFileDialogCofig;
        private System.Windows.Forms.DataGridView dataGridViewPlatforms;
        private System.Windows.Forms.OpenFileDialog openFileDialogConfig;
        private System.Windows.Forms.DataGridView dataGridViewLogger;
        private System.Windows.Forms.Button buttonClearLog;
        private System.Windows.Forms.Button buttonSaveLog;
        private System.Windows.Forms.TextBox textBoxMorphemeSDKRoot;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.SaveFileDialog saveFileDialogLog;
        private System.Windows.Forms.DataGridView dataGridViewMCNList;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Button buttonEditMacros;
        private System.Windows.Forms.Button buttonCreateMCNList;
        private System.Windows.Forms.SaveFileDialog saveFileCreateMCNList;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox comboBoxMorphemeVersion;
        private System.Windows.Forms.BindingSource logItemBindingSource;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox showGroup;
        private System.Windows.Forms.Button buttonBrowseExport;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.Button buttonBrowseSDK;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog2;
        private System.Windows.Forms.Button buttonBrowseConnect;
        private System.Windows.Forms.OpenFileDialog openFileDialogBrowseConnect;
        private System.Windows.Forms.DataGridViewTextBoxColumn severityDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn messageDataGridViewTextBoxColumn;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.DataGridViewCheckBoxColumn use;
        private System.Windows.Forms.DataGridViewTextBoxColumn mCNFileNameDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewComboBoxColumn assetCompilerDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn mCPFileNameDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewCheckBoxColumn processedDataGridViewCheckBoxColumn;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox textBoxAssetCompilerCommandLine;
        private System.Windows.Forms.ContextMenuStrip contextMenuPlatforms;
        private System.Windows.Forms.ToolStripMenuItem deleteToolStripMenuItemPlatforms;
        private System.Windows.Forms.ContextMenuStrip contextMenuNetworks;
        private System.Windows.Forms.ToolStripMenuItem deleteToolStripMenuItemNetworks;
        private System.Windows.Forms.Button buttonExport;
        private System.Windows.Forms.OpenFileDialog openFileDialogAssetCompiler;
        private System.Windows.Forms.CheckBox checkBoxUseDynamicPlugins;
    }
}

