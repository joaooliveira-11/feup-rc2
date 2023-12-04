/system reset-configuration

/interface bridge port remove [find interface =ether4]
/interface bridge port remove [find interface =ether14]
/interface bridge port remove [find interface =ether18]

/interface bridge add name=bridge40
/interface bridge add name=bridge41

/interface bridge port add bridge=bridge40 interface=ether14
/interface bridge port add bridge=bridge40 interface=ether18
/interface bridge port add bridge=bridge41 interface=ether4


