configure terminal
vlan 30
end

configure terminal
vlan 31
end

configure terminal
interface fastethernet 0/1
switchport mode access
switchport access vlan 30
end

configure terminal
interface fastethernet 0/2
switchport mode access
switchport access vlan 30
end

configure terminal
interface fastethernet 0/13
switchport mode access
switchport access vlan 31
end

configure terminal
interface fastethernet 0/14
switchport mode access
switchport access vlan 31
end