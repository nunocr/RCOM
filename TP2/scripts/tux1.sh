#!/bin/bash
ifconfig eth0 up
ifconfig eht0 172.16.30.1
route add -net 172.16.30.0/24 gw 172.16.30.254
echo Tux1 configurations:
ifconfig
route -n
