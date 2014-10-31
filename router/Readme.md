Quick setup guide for the router
================================

Plug a USB bluetooth adapter into the router  
Plug the net monitor into anywhere

Compiling:
```
apt-get install gcc liboping-dev libcurl4-gnutls-dev libbluetooth-dev bluez
chmod +x make.sh
./make.sh
```

Search for the net monitor:
```
hcitool scan
```

One of the lines should be the net monitor, copy the hex address string for it.

Set up config:  
Open ./btnetmon.conf  
Set btaddr to the hex address for the net monitor  
`btaddr=AA:BB:CC.....`  
Make sure all the other settings are OK  

Paring:  
`bluetooth-agent 1234 &`  
1234 is the PIN for the net monitor.  

Run main program:  
`./btnetmon -d`

If it successfully connects then CTRL + C.

`pkill bluetooth-agent`  
You don't need to do the bluetooth-agent things again, it's only needed for the first connection.

`./btnetmon`  
Program should now be running in the background.


If for some crazy reason it stops being able to connect to the net monitor run:  
`rm -r /var/lib/bluetooth/(btaddr)/`  
That will clear all saved settings for the bluetooth device with that address, then do the bluetooth-agent stuff again.
