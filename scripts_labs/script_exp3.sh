# configuration script for Tux64

ifconfig eth1 up
ifconfig eth1 172.16.41.253/24
# Enable IP forwarding
sysctl net.ipv4.ip_forward=1
# Disable ICMP echo ignore broadcast
sysctl net.ipv4.icmp_echo_ignore_broadcasts=0


/interface bridge port remove [find interface =ether6] // 12
/interface bridge port add bridge=bridge41 interface=ether6 // 12



route add -net  172.16.40.0/24 gw 172.16.41.253 # no Tux42
route add -net  172.16.41.0/24 gw 172.16.40.254 # no Tux43

arp -d 172.16.41.253 #Tux42
arp -d 172.16.40.254 #Tux43
arp -d 172.16.40.1 #Tux44
arp -d 172.16.41.1 #Tux44   
