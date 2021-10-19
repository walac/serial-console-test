#!/bin/sh -vex

drivername=serco
devicename=serco

rmmod $drivername || true
insmod $drivername.ko
rm -f /tmp/$devicename

major=$(cat /proc/devices | grep $devicename | awk '{print $1}')
mknod /tmp/$devicename c $major 0 
chmod a+rw /tmp/$devicename
