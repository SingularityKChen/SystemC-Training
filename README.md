# SystemC Training

This project is used for me to learn SystemC TLM.

## Adder

This is a simpe project from National Chiao-Tung University.

## [SimpleBus](./SimpleBus)

This is a simple SystemC tutorial by Synopsys.

In this project, it:
+ Shows the usage of ports, channels;
+ Block and non block require/response;

## [Doulos_Sockets: Sockets, Generic Payload, Blocking Transport](./Doulos_Sockets)

This is the [first TLM tutorial](https://www.doulos.com/knowhow/systemc/tlm2/tutorial__1/) by Doulos.

In this project, it:
+ Shows the generic payload, sockets, and blocking transport interface.
+ Shows the responsibilities of initiator and target with respect to the generic payload.
+ Has only dummy implementations of the direct memory and debug transaction interfaces.
+ Does not show the non-blocking transport interface.

## [Doulos_Tutorial2: Response Status, DMI, and Debug Transport](./Doulos_Tutorial2)

This is the [second TLM tutorial](https://www.doulos.com/knowhow/systemc/tlm2/tutorial__2/) by Doulos.

In this project, it:
+ Shows the direct memory interfaces and the DMI hint.
+ Shows the debug transaction interface
+ Shows the proper use of response status

## [Doulos_Tutorial3: Routing Methods through Interconnect Components ](./Doulos_Tutorial3)

This is the [third TLM tutorial](https://www.doulos.com/knowhow/systemc/tlm2/tutorial__3/) by Doulos.

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

## [Doulos_Example5: Temporal Decoupling, Multiple Initiators and Targets](./Doulos_Example5)

This is the [fifth TLM example](https://www.doulos.com/knowhow/systemc/tlm2/example_5/index.php) by Doulos.

In this project, it:
+ Shows two loosely-timed initiators both with temporal decoupling and quantum keeper
+ Shows a bus with multiple initiators and multiple targets (four memories)
+ Routes transactions to target and back using address decoding built into the bus
+ Uses tagged interfaces and sockets to implement multiple fw/bw interfaces in a single module
+ Propagates DMI calls on both forward and backward paths, 
with 'invalidate' being broadcast to every initiator
+ Shows transaction pooling using a memory manager