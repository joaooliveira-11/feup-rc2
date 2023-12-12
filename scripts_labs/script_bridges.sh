/system reset-configuration

/interface bridge port remove [find interface =ether4]  // 2
/interface bridge port remove [find interface =ether14]  // 4
/interface bridge port remove [find interface =ether18] // 10

/interface bridge add name=bridge40
/interface bridge add name=bridge41

/interface bridge port add bridge=bridge40 interface=ether14 // 2
/interface bridge port add bridge=bridge40 interface=ether18 // 4
/interface bridge port add bridge=bridge41 interface=ether4 // 10



