#!/bin/bash
ifconfig eth0 up
ifconfig eth0 172.16.30.254/24
ifconfig eth1 up
ifconfig eth1 172.16.31.253/24
ifconfig
route -n
