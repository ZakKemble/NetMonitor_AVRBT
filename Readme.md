Bluetooth Net Monitor
=====================

http://blog.zakkemble.co.uk/bluetooth-net-monitor-v2/

The firmware .hex files (bootloader & monitor) are compiled for ATmega328P

Fuses for ATmega328/328P  
External full swing crystal 16K CK/14CK + 65ms startup  
Serial program (SPI) enabled  
Brown-out at 2.7V  
Bootloader enabled, 2048 words  
High:		0xD8 (or 0xDF for no bootloader)  
Low:		0xF7  
Extended:	0xFD

--------

`./modem/adsl_btnetmon`  
This is the CGI script which runs on the modem to get ADSL stats (Only tested on Netgear DG834G v3 running OpenWRT)

`./router/btnetmon`  
This is the main program which runs on the router, the binary was compiled on Debain 7.7 x64 so it may or may not work without having to recompile (differently named libs, etc). More info in `./router/Readme.md`

`./utils/lcdBitmapConverter.exe`  
Utility to convert red channel of images (in `./imgs/`) to pixel intensity byte array

`./driver/` contains parts of libusb-win32 available at http://sourceforge.net/apps/trac/libusb-win32/wiki  
`./firmware/` contains the V-USB library available at http://www.obdev.at/products/vusb/index.html  
`./firmware/USBaspLoader/` contains the USBaspLoader bootloader available at https://github.com/baerwolf/USBaspLoader  
Font used is MicroExtendFLF (bold italics) available at http://www.fontspace.com/casady-and-greene/microextendflf  
Third party contents are copyrighted by their respective authors.  
My productions are published under GNU GPL v3 (see License.txt).

--------

Zak Kemble

contact@zakkemble.co.uk
