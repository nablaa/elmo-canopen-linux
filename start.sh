#!/bin/bash
ifconfig -a
sudo ifconfig can0 up
lsmod | grep can
cat /proc/pcan

