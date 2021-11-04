#!/bin/sh -vex

drivername=serco
devicename=serco
devpath=/dev

rmmod $drivername || true
insmod $drivername.ko
rm -f $devpath/$devicename

major=$(cat /proc/devices | grep $devicename | awk '{print $1}')
mknod $devpath/$devicename c $major 0 
chmod a+rw $devpath/$devicename
