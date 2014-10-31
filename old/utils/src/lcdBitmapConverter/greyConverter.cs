/*
 * Project: Bluetooth Net Monitor
 * Author: Zak Kemble, me@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
 */

using System;
using System.Drawing;
using System.Windows.Forms;
using System.Text;

namespace lcdBitmapConverter
{
    class greyConverter
    {
        private Bitmap img; // Image
        private string lastErrorStr = "";
        private StringBuilder code = new StringBuilder();

        // Get last error
        public string lastError()
        {
            return lastErrorStr;
        }

        // Load image
        public bool load(string file)
        {
            // Try to load image
            try
            {
                img = new Bitmap(file);

                // Make sure its in the right format
                img = img.Clone(new Rectangle(0, 0, img.Width, img.Height), System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            }
            catch (Exception ex)
            {
                lastErrorStr = ex.Message;
                return false;
            }

            redToGreyHex();

            return true;
        }

        // Get red values from image
        private void redToGreyHex()
        {
            //code.Clear();
            code.Length = 0;
            code.Capacity = 0;

            code.AppendLine("static const byte imageData[] PROGMEM ={");

            for (int y = 0; y < img.Height; y++)
            {
                for (int x = 0; x < img.Width; x++)
                {
                    byte red = img.GetPixel(x, y).R;
                    Color pixel = Color.FromArgb(255, red, red, red);
                    img.SetPixel(x, y, pixel);
                    code.AppendFormat("0x{0:X2},", red);
                }
                code.Append(Environment.NewLine);
            }

            code.AppendLine("};");
        }

        public string getCode()
        {
            return code.ToString();
        }

        public Bitmap getImage()
        {
            return img;
        }
    }
}
