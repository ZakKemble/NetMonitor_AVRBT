THIS IS OLD STUFF FOR BLUETOOTH NET MONITOR V1


Bluetooth Net Monitor
http://blog.zakkemble.co.uk/bluetooth-net-monitor/

The .hex file in ./firmware/hex/ is compiled for ATmega328P
Fuse settings are in ./firmware/src/config.h

./scripts/modem/adsl_btnetmon
This is the CGI script which runs on the modem to get ADSL stats

./scripts/router/btnetmon.sh
This is the main shell script which runs on the router

./utils/lcdBitmapConverter.exe
Utility to convert red channel of images (in ./imgs/) to pixel intensity byte array

The ./driver/ directory contain parts of libusb-win32 available at http://sourceforge.net/apps/trac/libusb-win32/wiki
The ./firmware/src/ directory and subdirectories contains the V-USB library available at http://www.obdev.at/products/vusb/index.html
Third party contents are copyrighted by their respective authors.
My productions are published under GNU GPL v3 (see License.txt).

--------

Zak Kemble
me@zakkemble.co.uk