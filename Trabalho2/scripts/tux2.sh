#!/bin/bash
ifconfig eth0 up
ifconfig eth0 172.16.31.1/24
route add -net 172.16.30.0/24 gw 172.16.31.253
route add default gw 172.16.31.254
ifconfig
route -n
