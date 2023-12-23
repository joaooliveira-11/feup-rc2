#GTKTERM

/system reset-configuration

/interface bridge port remove [find interface =ether2]
/interface bridge port remove [find interface =ether4]
/interface bridge port remove [find interface =ether10]

/interface bridge add name=bridge40
/interface bridge add name=bridge41

/interface bridge port add bridge=bridge40 interface=ether2
/interface bridge port add bridge=bridge40 interface=ether4
/interface bridge port add bridge=bridge41 interface=ether10

