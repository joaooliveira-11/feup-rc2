#!/bin/bash

systemctl restart networking

ifconfig eth0 up

ifconfig eth0 172.16.40.254/24

ifconfig
route -n