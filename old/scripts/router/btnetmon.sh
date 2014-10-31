#!/bin/bash
#
# Project: Bluetooth Net Monitor
# Author: Zak Kemble, me@zakkemble.co.uk
# Copyright: (C) 2013 by Zak Kemble
# License: GNU GPL v3 (see License.txt)
# Web: http://blog.zakkemble.co.uk/bluetooth-net-monitor/
#
# $|syncdown|syncup|ratedown|rateup|ping|loss|ip|emails|date|time|*

IF=ppp1 # Interface to monitor
SERIAL=/dev/ttyUSB0 # Serial device to send data to
BAUD=115200 # Serial baud rate
ADSLPAGE=http://192.168.0.1/cgi-bin/monitor/adsl_btnetmon # Modem page with ADSL sync rates
ADSLCACHETIME=60 # Sync rate cache time (don't want to do web page requests all the time)
LOSSTIME=60 # Percentage packet loss time
EMAILPAGE=https://mail.google.com/mail/feed/atom # Mail feed address
EMAILCACHETIME=60 # How often to check for new mail

EMAILUSER= # Email username
EMAILPASS= # Email password

# Temp file locations
PID=/tmp/btnetmon.pid
ADSLDOWNFILE=/tmp/btnetmon_adsldown
ADSLUPFILE=/tmp/btnetmon_adslup
LASTRXFILE=/tmp/btnetmon_lastrx
LASTTXFILE=/tmp/btnetmon_lasttx
PINGFILE=/tmp/btnetmon_ping
LOSSFILE=/tmp/btnetmon_loss
EMAILFILE=/tmp/btnetmon_email

makeFile()
{
	echo "0" > $1
	if [ ! -f $1 ] ; then
		echo "Unable to create file $1"
		exit 1
	fi
}

updateADSL()
{
	touch $ADSLDOWNFILE
	ADSLDATA=$(curl --retry 3 --max-time 2 $ADSLPAGE 2>/dev/null)

# This is were we parse the ADSL status

	if [ "$ADSLDATA" != "" ] ; then
		echo $ADSLDATA | cut -f 1 -d ' ' > $ADSLDOWNFILE
		echo $ADSLDATA | cut -f 2 -d ' ' > $ADSLUPFILE
	else
		echo 0 > $ADSLDOWNFILE
		echo 0 > $ADSLUPFILE
	fi
}

updateEmail()
{
# This is were we get and parse the email feed

	touch $EMAILFILE

	if [[ "$EMAILUSER" != "" && "$EMAILPASS" != "" && "$EMAILPAGE" != "" ]] ; then
		EMAILDATA=$(curl --retry 3 --max-time 2 -u $EMAILUSER:$EMAILPASS $EMAILPAGE 2>/dev/null)
		if [ "$EMAILDATA" != "" ] ; then
			echo "$EMAILDATA" | grep fullcount | tr -d '\t' | sed 's/^<.*>\([^<].*\)<.*>$/\1/' 1>$EMAILFILE
		else
			echo 0 > $EMAILFILE
		fi
	fi
}

send()
{
	BYTECOUNTS=$(ifconfig $IF 2>/dev/null | grep "RX bytes" | sed 's/^ *//g')
	CURRENTRX=$(echo $BYTECOUNTS | cut -f 2 -d ' ' | cut -f 2 -d ':')
	CURRENTTX=$(echo $BYTECOUNTS | cut -f 6 -d ' ' | cut -f 2 -d ':')
	LASTRX=$(cat $LASTRXFILE 2>/dev/null)
	LASTTX=$(cat $LASTTXFILE 2>/dev/null)
	NETIP=$(ifconfig $IF 2>/dev/null | grep "inet addr" | sed 's/^ *//g' | cut -f 2 -d ' ' | cut -f 2 -d ':')
	PING=$(cat $PINGFILE 2>/dev/null)
	SYNCDOWN=$(cat $ADSLDOWNFILE 2>/dev/null)
	SYNCUP=$(cat $ADSLUPFILE 2>/dev/null)
	LOSS=$(cat $LOSSFILE 2>/dev/null)
	EMAILS=$(cat $EMAILFILE 2>/dev/null)
	DATE=$(date +%a\ %d\ %b\ %y)
	TIME=$(date +%H:%M:%S)

	[ "$CURRENTRX" = "" ] && CURRENTRX="0"
	[ "$CURRENTTX" = "" ] && CURRENTTX="0"
	[ "$LASTRX" = "" ] && LASTRX="0"
	[ "$LASTTX" = "" ] && LASTTX="0"
	
	RATEDOWN=$(($(($CURRENTRX - $LASTRX)) / 1024))
	RATEUP=$(($(($CURRENTTX - $LASTTX)) / 1024))

	echo $CURRENTRX > $LASTRXFILE
	echo $CURRENTTX > $LASTTXFILE

	NOW=$(date +%s)
	CACHEMODTIME=$(stat --format=%Y $ADSLDOWNFILE 2>/dev/null)
	[ $(($NOW - $CACHEMODTIME)) -ge $ADSLCACHETIME ] && updateADSL &

	CACHEMODTIME=$(stat --format=%Y $EMAILFILE 2>/dev/null)
	[ $(($NOW - $CACHEMODTIME)) -ge $EMAILCACHETIME ] && updateEmail &

	[ "$SYNCDOWN" = "" ] && SYNCDOWN="0"
	[ "$SYNCUP" = "" ] && SYNCUP="0"
	[ "$RATEDOWN" = "" ] && RATEDOWN="0"
	[ "$RATEUP" = "" ] && RATEUP="0"
	[ "$PING" = "" ] && PING="0"
	[ "$LOSS" = "" ] && LOSS="0"
	[ "$NETIP" = "" ] && NETIP="0.0.0.0"
	[ "$EMAILS" = "" ] && EMAILS="0"
	[ "$DATE" = "" ] && DATE="--- 00 --- 00"
	[ "$TIME" = "" ] && TIME="00:00:00"

	echo "$|$SYNCDOWN|$SYNCUP|$RATEDOWN|$RATEUP|$PING|$LOSS|$NETIP|$EMAILS|$DATE|$TIME|*" > $SERIAL
#	tail $SERIAL >/dev/null
}

mainWorker()
{
	LASTRX=$(ifconfig $IF 2>/dev/null | grep "RX bytes" | sed 's/^ *//g' | cut -f 2 -d ' ' | cut -f 2 -d ':')
	LASTTX=$(ifconfig $IF 2>/dev/null | grep "TX bytes" | sed 's/^ *//g' | cut -f 7 -d ' ' | cut -f 2 -d ':')

	echo $LASTRX > $LASTRXFILE
	echo $LASTTX > $LASTTXFILE

	while true
	do
		sleep 1
		[ ! -f $PID ] && exit 0
		send &
	done
}

pingWorker()
{
	LOSSTIME=$(($LOSSTIME - 1))
	IDX=0
#	PACKETLOST=0
	declare -a LOSTPACKETS_A
	for i in $(seq 0 $LOSSTIME)
	do
		LOSTPACKETS_A[$i]=0
	done

#	LOSTPACKETS=0

	sleep 0.5

	while true
	do
		sleep 1
		[ ! -f $PID ] && exit 0

		GW=$(ifconfig $IF 2>/dev/null | grep "P-t-P" | sed 's/^ *//g' | cut -f 4 -d ' ' | cut -f 2 -d ':')
#		GW=$(cat /ip 2>/dev/null)
		[ "$GW" = "" ] && PING="" || PING=$(ping -c 1 -W 1 -I $IF $GW 2>/dev/null | grep "bytes from" | cut -f 7 -d ' ' | cut -f 2 -d '=' | cut -f 1 -d '.')

		if [ "$PING" != "" ] ; then
			echo $PING > $PINGFILE
#			LOSTPACKETS=$(($LOSTPACKETS - 1))
#			PACKETLOST=0
			LOSTPACKETS_A[$IDX]=0
		else
			echo 999 > $PINGFILE
#			LOSTPACKETS=$(($LOSTPACKETS + 1))
#			PACKETLOST=1
			LOSTPACKETS_A[$IDX]=1
		fi

		# Make sure LOSTPACKETS stays in range
#		if [ $LOSTPACKETS -gt $LOSSTIME ] ; then
#			LOSTPACKETS=$LOSTIME
#		elif [ $LOSTPACKETS -lt 0 ] ; then
#			LOSTPACKETS=0
#		fi
		
		LOSTCOUNT=0
		for i in $(seq 0 $LOSSTIME)
		do
#			if [[ PACKETLOST -eq 0 && ${LOSTPACKETS_A[$i]} -eq 1 ]] ; then
#				LOSTPACKETS_A[$i]=0
#				PACKETLOST=2
#			elif [[ PACKETLOST -eq 1 && ${LOSTPACKETS_A[$i]} -eq 0 ]] ; then
#				LOSTPACKETS_A[$i]=1
#				PACKETLOST=2
#			fi

			[ ${LOSTPACKETS_A[$i]} -eq 1 ] && LOSTCOUNT=$(($LOSTCOUNT + 1))
		done
		
		[ $LOSTCOUNT -eq 0 ] && PACKETLOSS=0 || PACKETLOSS=$(echo "scale=2; ($LOSTCOUNT / $LOSSTIME) * 99" | bc | cut -f 1 -d '.')
		echo $PACKETLOSS > $LOSSFILE

		# Workout lost packet percentage
#		[ $LOSTPACKETS -eq 0 ] && PACKETLOSS=0 || PACKETLOSS=$(echo "scale=2; ($LOSTPACKETS / $LOSSTIME) * 99" | bc | cut -f 1 -d '.')
#		echo $PACKETLOSS > $LOSSFILE
		
		IDX=$(($IDX + 1))
		[ $IDX -gt $LOSSTIME ] && IDX=0
	done
}

if [[ "$1" = "start" || "$1" = "restart" ]] ; then
	if [ -f $PID ] ; then
		echo "Bluetooth net monitor restarting..."
		rm -f $PID
		sleep 1
	fi
	
	if [ ! -e $SERIAL ] ; then
		echo "Serial device $SERIAL not found"
		exit 1
	fi
	stty -F $SERIAL $BAUD cs8

# Something must be reading the serial port otherwise data being sent won't be sent properly
# BUG 1: stopping or restarting this script doesn't stop this cat process, they will keep piling up!
# BUG 2: the cat process will end if the serial port is removed, then if the port is added back a new cat process is not created
# The sooner I get this script coded in C the better >.<
	cat $SERIAL >/dev/null &

	makeFile $ADSLDOWNFILE
	makeFile $ADSLUPFILE
	makeFile $LASTRXFILE
	makeFile $LASTTXFILE
	makeFile $PINGFILE
	makeFile $LOSSFILE

	touch -t 0001010000 $ADSLDOWNFILE
	touch -t 0001010000 $EMAILFILE
	
	makeFile $PID
	mainWorker &
	pingWorker &
	echo "Bluetooth net monitor started"
elif [ "$1" = "stop" ] ; then
	rm -f $PID
	echo "Bluetooth net monitor stopped"
else
	echo "Bluetooth net monitor v1"
	echo "zakkemble.co.uk"
	echo "Available args: start, restart and stop"
fi
