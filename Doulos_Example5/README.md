# Example 5 - Temporal Decoupling, Multiple Initiators and Targets

This is a TLM demo developed by Doulos, 
you can find the original descriptions 
[here](https://www.doulos.com/knowhow/systemc/tlm2/example_5/index.php).

In this project, it:
+ Shows two loosely-timed initiators both with temporal decoupling and quantum keeper
+ Shows a bus with multiple initiators and multiple targets (four memories)
+ Routes transactions to target and back using address decoding built into the bus
+ Uses tagged interfaces and sockets to implement multiple fw/bw interfaces in a single module
+ Propagates DMI calls on both forward and backward paths, 
with 'invalidate' being broadcast to every initiator
+ Shows transaction pooling using a memory manager