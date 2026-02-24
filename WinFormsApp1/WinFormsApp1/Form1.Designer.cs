namespace WinFormsApp1
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            listBoxDevices = new ListBox();
            button1 = new Button();
            label1 = new Label();
            labelStatus = new Label();
            picColorWheel = new PictureBox();
            panelCurrentColor = new Panel();
            trackBarValue = new TrackBar();
            label2 = new Label();
            ((System.ComponentModel.ISupportInitialize)picColorWheel).BeginInit();
            ((System.ComponentModel.ISupportInitialize)trackBarValue).BeginInit();
            SuspendLayout();
            // 
            // listBoxDevices
            // 
            listBoxDevices.FormattingEnabled = true;
            listBoxDevices.Location = new Point(88, 100);
            listBoxDevices.Name = "listBoxDevices";
            listBoxDevices.Size = new Size(120, 94);
            listBoxDevices.TabIndex = 0;
            listBoxDevices.SelectedIndexChanged += listBoxDevices_SelectedIndexChanged;
            // 
            // button1
            // 
            button1.Location = new Point(88, 209);
            button1.Name = "button1";
            button1.Size = new Size(117, 23);
            button1.TabIndex = 1;
            button1.Text = "Цвет / Яркость";
            button1.TextAlign = ContentAlignment.BottomCenter;
            button1.UseVisualStyleBackColor = true;
            button1.Click += button1_Click;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(88, 82);
            label1.Name = "label1";
            label1.Size = new Size(101, 15);
            label1.TabIndex = 2;
            label1.Text = "Ожидание плат...";
            // 
            // labelStatus
            // 
            labelStatus.AutoSize = true;
            labelStatus.Location = new Point(449, 19);
            labelStatus.Name = "labelStatus";
            labelStatus.Size = new Size(38, 15);
            labelStatus.TabIndex = 4;
            labelStatus.Text = "label2";
            // 
            // picColorWheel
            // 
            picColorWheel.Image = (Image)resources.GetObject("picColorWheel.Image");
            picColorWheel.Location = new Point(523, 82);
            picColorWheel.Name = "picColorWheel";
            picColorWheel.Size = new Size(250, 217);
            picColorWheel.SizeMode = PictureBoxSizeMode.Zoom;
            picColorWheel.TabIndex = 5;
            picColorWheel.TabStop = false;
            picColorWheel.Paint += PicColorWheel_Paint;
            picColorWheel.MouseDown += ColorWheel_Interaction;
            picColorWheel.MouseMove += ColorWheel_Interaction;
            // 
            // panelCurrentColor
            // 
            panelCurrentColor.Location = new Point(298, 204);
            panelCurrentColor.Name = "panelCurrentColor";
            panelCurrentColor.Size = new Size(174, 95);
            panelCurrentColor.TabIndex = 6;
            // 
            // trackBarValue
            // 
            trackBarValue.Location = new Point(321, 82);
            trackBarValue.Maximum = 255;
            trackBarValue.Name = "trackBarValue";
            trackBarValue.Size = new Size(104, 45);
            trackBarValue.TabIndex = 3;
            trackBarValue.TickFrequency = 10;
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(345, 186);
            label2.Name = "label2";
            label2.Size = new Size(80, 15);
            label2.TabIndex = 7;
            label2.Text = "ЦВЕТ ДИОДА";
            label2.Click += label2_Click;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(800, 450);
            Controls.Add(label2);
            Controls.Add(panelCurrentColor);
            Controls.Add(picColorWheel);
            Controls.Add(labelStatus);
            Controls.Add(trackBarValue);
            Controls.Add(label1);
            Controls.Add(button1);
            Controls.Add(listBoxDevices);
            Name = "Form1";
            Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)picColorWheel).EndInit();
            ((System.ComponentModel.ISupportInitialize)trackBarValue).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private ListBox listBoxDevices;
        private Button button1;
        private Label label1;
        private Label labelStatus;
        private PictureBox picColorWheel;
        private Panel panelCurrentColor;
        private TrackBar trackBarValue;
        private Label label2;
    }
}
