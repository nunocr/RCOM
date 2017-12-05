#!/bin/bash
ifconfig eth0 up
ifconfig eth0 172.16.30.254
ifconfig eth1 up
ifconfig eth1 172.16.31.253
echo Tux4 configurations:
ifconfig
route -n
