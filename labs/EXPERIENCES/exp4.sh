#GTKTERM

/interface bridge port remove [find interface=ether14]
/interface bridge port add bridge=bridge41 interface=ether14

# CHANGE FROM SWITCH TO ROUTER MT

/system reset-configuration

/ip address add address=172.16.2.49/24 interface=ether1  # If working in the main classroom use 172.16.(1).49/24
/ip address add address=172.16.41.254/24 interface=ether2


route add default gw 172.16.41.254 # Gnu/Tux42 
route add default gw 172.16.40.254 # Gnu/Tux43
route add default gw 172.16.41.254 # Gnu/Tux44

/ip route add dst-address=172.16.40.0/24 gateway=172.16.41.253 
/ip route add dst-address=0.0.0.0/0 gateway=172.16.2.254  # If working in the main classroom use 172.16.1.254


# Gnu43 (Testing connectivity)
ping 172.16.40.254
ping 172.16.41.1
ping 172.16.41.254


# Gnu42

sysctl net.ipv4.conf.eth0.accept_redirects=0
sysctl net.ipv4.conf.all.accept_redirects=0 

route del -net 172.16.40.0 gw 172.16.41.253 netmask 255.255.255.0

ping 172.16.40.1 # The connection is established using Rc as a router instead of Gnu44.

traceroute -n 172.16.40.1  # Trace route from Gnu42 to Gnu43

route add -net 172.16.40.0/24 gw 172.16.41.253 # Adding the deleted route again

traceroute -n 172.16.40.1  # Trace route from Gnu42 to Gnu43

sysctl net.ipv4.conf.eth0.accept_redirects=1
sysctl net.ipv4.conf.all.accept_redirects=1

# Gnu43

ping 172.16.2.254

/ip firewall nat disable 0

ping 172.16.2.254 # Doesnt work

/ip firewall nat enable 0

