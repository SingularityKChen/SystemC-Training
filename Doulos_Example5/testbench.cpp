//----------------------------------------------------------------------
//  Copyright (c) 2007-2008 by Doulos Ltd.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//----------------------------------------------------------------------

// Version 1, 26-June-2008
// Version 2,  3-July-2008 - fix bug: call dmi_data.init()
// Version 3  12-Jan-2009  - fix bug in transport_dbg
// Version 4  26-Sep-2009  - fix bug with set_end_address


// Getting Started with TLM-2.0, Example 5

// Shows two loosely-timed initiators both with temporal decoupling and quantum keeper

// Shows a bus with multiple initiators and multiple targets (four memories)
// Routes transactions to target and back using address decoding built into the bus
// Uses tagged interfaces and sockets to implement multiple fw/bw interfaces in a single module
// Propagates DMI calls on both forward and backward paths,
// with 'invalidate' being broadcast to every initiator

// Shows transaction pooling using a memory manager

#include "top.h"

int sc_main(int argc, char* argv[])
{
    Top top("top");
    sc_start();
    return 0;
}
