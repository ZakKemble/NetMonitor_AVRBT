/*
 * Project: Bluetooth Net Monitor (USB Host Program)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/
 */

using LibUsbDotNet.Main;
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Threading;

namespace netMonitor
{
    class NetMonitor
    {
        private const byte USB_REQ_IMAGE = 1;
        private const byte USB_REQ_DRAW = 2;
        private const byte USB_REQ_BRIGHTNESS = 3;
        private const byte USB_REQ_BLANK = 4;
        private const byte USB_REQ_RESET = 5;

        private const byte USB_REQUEST_TYPE_IN = (byte)UsbRequestType.TypeVendor | (byte)UsbRequestRecipient.RecipDevice | (byte)UsbEndpointDirection.EndpointIn;
        private const byte USB_REQUEST_TYPE_OUT = (byte)UsbRequestType.TypeVendor | (byte)UsbRequestRecipient.RecipDevice | (byte)UsbEndpointDirection.EndpointOut;

        private Device usb;

        private bool busy;

        public NetMonitor()
        {
            usb = new Device(MCU.VEN_ID, MCU.VEN_NAME, MCU.DEV_ID, MCU.DEV_NAME);
            busy = false;
        }

        public void brightness(byte brightness)
        {
            UsbSetupPacket brightnessPkt = new UsbSetupPacket(
                USB_REQUEST_TYPE_IN,
                USB_REQ_BRIGHTNESS,
                brightness,
                0,
                0
                );
            usb.sendPkt(brightnessPkt);
        }

        // Needs to be threaded or somthing
        // http://stackoverflow.com/questions/3115076/adjust-the-contrast-of-an-image-in-c-sharp-efficiently
        public void uploadImage(Bitmap uploadImage2)
        {
            Bitmap originalImage = (Bitmap)uploadImage2.Clone();
            Bitmap uploadImage = new Bitmap(uploadImage2.Width, uploadImage2.Height);
            float brightness = 1.0f; // no change in brightness
            float contrast = 1.0f; // twice the contrast
            float gamma = 1.6f; // no change in gamma

            float adjustedBrightness = brightness - 1.0f;
            // create matrix that will brighten and contrast the image
            float[][] ptsArray ={
                new float[] {contrast, 0, 0, 0, 0}, // scale red
                new float[] {0, contrast, 0, 0, 0}, // scale green
                new float[] {0, 0, contrast, 0, 0}, // scale blue
                new float[] {0, 0, 0, 1.0f, 0}, // don't scale alpha
                new float[] {adjustedBrightness, adjustedBrightness, adjustedBrightness, 0, 1}
            };

            var imageAttributes = new ImageAttributes();
            imageAttributes.ClearColorMatrix();
            imageAttributes.SetColorMatrix(new ColorMatrix(ptsArray), ColorMatrixFlag.Default, ColorAdjustType.Bitmap);
            imageAttributes.SetGamma(gamma, ColorAdjustType.Bitmap);
            Graphics g = Graphics.FromImage(uploadImage);
            g.DrawImage(originalImage, new Rectangle(0, 0, uploadImage.Width, uploadImage.Height)
                , 0, 0, originalImage.Width, originalImage.Height, GraphicsUnit.Pixel, imageAttributes);

            clearScreen();

            int totalSize = uploadImage.Width * uploadImage.Height * 2;

            // Give time to clear screen
            Thread.Sleep(500);

            // Convert image to byte array
            Rectangle r = new Rectangle(0, 0, uploadImage.Width, uploadImage.Height);
            BitmapData bd = uploadImage.LockBits(r, ImageLockMode.ReadOnly, PixelFormat.Format16bppRgb565);

            byte[] data = new byte[totalSize];
            Marshal.Copy(bd.Scan0, data, 0, data.Length);
            uploadImage.UnlockBits(bd);

            // Swap byte order
            for (int i = 0; i < data.Length; i+=2)
            {
                byte byte1 = data[i];
                data[i] = data[i + 1];
                data[i + 1] = byte1;
            }

            int totalSent = 0;

            for (int i = 0; totalSent < totalSize; i++, totalSent += 254)
            {
                int remaining = totalSize - totalSent;
                byte chunkSize = (remaining > 254) ? (byte)254 : (byte)remaining;

                byte[] buffer = new byte[chunkSize];
                Array.Copy(data, 254 * i, buffer, 0, chunkSize);

                UsbSetupPacket imageDataPkt = new UsbSetupPacket(
                    USB_REQUEST_TYPE_OUT,
                    USB_REQ_IMAGE,
                    0,
                    0,
                    chunkSize
                    );

                int c;
                usb.usb.ControlTransfer(ref imageDataPkt, buffer, chunkSize, out c);

                //prbUpload.Value = (int)(((double)totalSent / (double)totalSize) * 100);
            }
        }

        public void clearScreen()
        {
            UsbSetupPacket pkt = new UsbSetupPacket(
                USB_REQUEST_TYPE_IN,
                USB_REQ_BLANK,
                0,
                0,
                0
                );
            usb.sendPkt(pkt);
        }

        public void pixel(byte x, byte y, short col)
        {
            UsbSetupPacket pkt = new UsbSetupPacket(
                USB_REQUEST_TYPE_IN,
                USB_REQ_DRAW,
                (short)((y << 8) | (x)),
                col,
                0
                );
            usb.sendPkt(pkt);
        }

        public void reset()
        {
            UsbSetupPacket pkt = new UsbSetupPacket(
                USB_REQUEST_TYPE_IN,
                USB_REQ_RESET,
                0,
                0,
                0
                );
            usb.sendPkt(pkt);
        }
    }
}
