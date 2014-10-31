using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace netMonitor
{
    public partial class Draw : Form
    {
        public Color drawCol;
        Form1 mainForm;

        public Draw(Form1 mainForm)
        {
            InitializeComponent();

            var assembly = System.Reflection.Assembly.GetExecutingAssembly();
            Version version = assembly.GetName().Version;

            Icon = Icon.ExtractAssociatedIcon(assembly.Location);
            Text = String.Format("Net Monitor Control - Draw", version.Major, version.Minor);

            this.mainForm = mainForm;

            MouseMove += new System.Windows.Forms.MouseEventHandler(mainForm.Draw_MouseMove);
        }

        private void Draw_Load(object sender, EventArgs e)
        {
            TopMost = true;
            FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            WindowState = FormWindowState.Maximized;
        }

        private void btnExit_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void btnClear_Click(object sender, EventArgs e)
        {

        }

        private void panelColour_MouseClick(object sender, MouseEventArgs e)
        {
            drawCol = ((Panel)sender).BackColor;

            foreach (Control control in Controls)
            {
                if(control.GetType() == typeof(Panel))
                    ((Panel)control).BorderStyle = BorderStyle.None;
            }

            ((Panel)sender).BorderStyle = BorderStyle.Fixed3D;
        }
    }
}
