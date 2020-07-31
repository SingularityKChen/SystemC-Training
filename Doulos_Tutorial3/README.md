# Tutorial 3 - Routing Methods through Interconnect Components 

This is a TLM demo developed by Doulos, 
you can find the original descriptions 
[here](https://www.doulos.com/knowhow/systemc/tlm2/tutorial__3/).

In this project, it:
+ Shows a router modeled as an interconnect component 
between the initiator and the target
+ The router decodes the address to select a target, 
and masks the address in the transaction
+ Shows the router passing transport, 
DMI and debug transactions along forward and backward paths
 and doing address translation in both directions
 
The hierarchy is shown bellow:
 
![the module hierarchy](https://www.doulos.com/knowhow/systemc/tlm2/tutorial__3/ex_3.gif)