/*
 * Project: Bluetooth Net Monitor (USB Host Program)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/
 */

using LibUsbDotNet;
using LibUsbDotNet.Descriptors;
using LibUsbDotNet.Info;
using LibUsbDotNet.Main;
using System;
using System.Collections.ObjectModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace netMonitor
{
    public partial class Form1 : Form
    {
        private const byte LCD_WIDTH = 160;
        private const byte LCD_HEIGHT = 128;

        private NetMonitor netMonitor;
        private Bitmap uploadImage = new Bitmap(LCD_WIDTH, LCD_HEIGHT, PixelFormat.Format16bppRgb565);

        public Form1()
        {
            InitializeComponent();

            var assembly = System.Reflection.Assembly.GetExecutingAssembly();
            Version version = assembly.GetName().Version;

            Icon = Icon.ExtractAssociatedIcon(assembly.Location);
            Text = String.Format("Net Monitor Control v{0}.{1}", version.Major, version.Minor);

            // Tray icon
            notifyIcon1.Icon = Icon;
            notifyIcon1.Text = "Net Monitor Control";
            notifyIcon1.BalloonTipText = "";
            notifyIcon1.BalloonTipTitle = "";
            notifyIcon1.BalloonTipIcon = ToolTipIcon.Info;
            notifyIcon1.Visible = true;
            //notifyIcon1.ShowBalloonTip(1000);

            // Drag and drop
            gbImageUpload.AllowDrop = true;
            gbImageUpload.DragEnter += new DragEventHandler(event_DragEnter);
            gbImageUpload.DragDrop += new DragEventHandler(event_DragDrop);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            netMonitor = new NetMonitor();
        }

        // Drag and drop
        private void event_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
                e.Effect = DragDropEffects.Copy;
        }

        // Drag and drop
        private void event_DragDrop(object sender, DragEventArgs e)
        {
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
            loadImage(files[0]);
        }

        // Hide on minimize
        private void Form1_Resize(object sender, EventArgs e)
        {
            if (FormWindowState.Minimized == this.WindowState)
                Hide();
        }

        // 
        private void notifyIcon1_DoubleClick(object sender, EventArgs e)
        {
            Show();
            WindowState = FormWindowState.Normal;
        }

        // 
        private void tbBrightness_Scroll(object sender, EventArgs e)
        {
            netMonitor.brightness((byte)tbBrightness.Value);
        }
        
        // Load image
        private void loadImage(string file)
        {
            // Load image
            Bitmap img = new Bitmap(file);

            // Resize image to fit LCD while keeping aspect ratio
            int width = img.Width;
            int height = img.Height;
            double ratio = (double)width / (double)height;

            int targetWidth = pbPreview.Width;
            int targetHeight = pbPreview.Height;
            double targetRatio = (double)targetWidth / (double)targetHeight;

            if (ratio > targetRatio) // Wide
            {
                height = (targetWidth * height) / width;
                width = targetWidth;
            }
            else if (ratio < targetRatio) // Tall
            {
                width = (targetHeight * width) / height;
                height = targetHeight;
            }
            else // Same
            {
                width = targetWidth;
                height = targetHeight;
            }

            // Center image
            int x = (width / 2) * -1;
            int y = (height / 2) * -1;

            // Render image
            using (Graphics g = Graphics.FromImage(uploadImage))
            {
                g.Clear(Color.Black);
                g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
                g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
                g.TranslateTransform((targetWidth / 2), (targetHeight / 2));
                g.DrawImage(img, x, y, width, height);
            }

            // Free resources
            img.Dispose();

            // Set preview image
            pbPreview.Image = uploadImage;
        }

        private void btnUpload_Click(object sender, EventArgs e)
        {
            // 
            Stopwatch stopWatch = new Stopwatch();
            stopWatch.Start();

            netMonitor.uploadImage(uploadImage);

            stopWatch.Stop();
            Console.WriteLine("Upload time: " + stopWatch.Elapsed.TotalMilliseconds.ToString());
        }

        private void btnDraw_Click(object sender, EventArgs e)
        {
            netMonitor.clearScreen();

            Form drawForm = new Draw(this);
            drawForm.Show();
        }

        public void Draw_MouseMove(object sender, MouseEventArgs e)
        {
            if ((MouseButtons.Left & e.Button) == 0)
                return;

            //Console.WriteLine(e.X + " " + e.Y);

            byte x = (byte)(MousePosition.X * LCD_WIDTH / Screen.PrimaryScreen.Bounds.Width);
            byte y = (byte)(MousePosition.Y * LCD_HEIGHT / Screen.PrimaryScreen.Bounds.Height);

            int col;
            /*
            if ((MouseButtons.Right & e.Button) == 0)
            {
                Random random = new Random();
                col = random.Next(0, 0xFFFF);
            }
            else
                col = 0x001F;
            */
            col = ((Draw)sender).drawCol.ToArgb();

            short col2 = (short)(((col & 0x00F80000) >> 8) | ((col & 0x0000FC00) >> 5) | ((col & 0x000000FF) >> 3));

            netMonitor.pixel(x, y, col2);
        }

        private void btnClear_Click(object sender, EventArgs e)
        {
            pbPreview.Image = new Bitmap(LCD_WIDTH, LCD_HEIGHT);

            netMonitor.clearScreen();
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            netMonitor.reset();
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            MessageBox.Show("drag and drop an image file");
        }
    }
}
