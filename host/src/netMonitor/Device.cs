/*
 * Project: Bluetooth Net Monitor (USB Host Program)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/
 */

using LibUsbDotNet;
using LibUsbDotNet.Main;
using System;

namespace netMonitor
{
    class Device
    {
// https://github.com/kehribar/Little-Wire/blob/master/computer_interface/C%23/library/littleWire.cs
        private UsbDevice device;
        private int ven_id;
        private int dev_id;
        private string ven_name;
        private string dev_name;

        public UsbDevice usb
        {
            get { return device; }
            set { device = value; }
        }

        public Device(int ven_id, string ven_name, int dev_id, string dev_name)
        {
            this.ven_id = ven_id;
            this.ven_name = ven_name;
            this.dev_id = dev_id;
            this.dev_name = dev_name;

            find();
        }

        public bool connected()
        {
            return device.IsOpen;
        }

        public void sendPkt(UsbSetupPacket pkt)
        {
            int c;
            if (!device.ControlTransfer(ref pkt, null, 0, out c))
            {
                find();
                device.ControlTransfer(ref pkt, null, 0, out c);
            }
        }

        private void find()
        {
            UsbRegDeviceList allDevices = UsbDevice.AllDevices;
            foreach (UsbRegistry usbRegistry in allDevices)
            {
                UsbDevice tempDevice;
                if (usbRegistry.Open(out tempDevice))
                {
                    if (tempDevice.Info.ProductString.Equals(dev_name) && tempDevice.Info.ManufacturerString.Equals(ven_name))
                    {
                        device = tempDevice;
                        break;
                    }

                    tempDevice.Close();
                }
            }
        }
    }
}
