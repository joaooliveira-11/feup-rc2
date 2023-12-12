#!/bin/bash

systemctl restart networking

ifconfig eth0 up

ifconfig eth0 172.16.10.1/24

ifconfig
route -n