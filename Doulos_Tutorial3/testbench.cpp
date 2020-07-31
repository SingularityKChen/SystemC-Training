#//----------------------------------------------------------------------
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

// Version 2 - fix warnings that only showed up using g++
// Version 3  18-June-2008 - updated for TLM-2.0
// Version 4  12-Jan-2009  - fix bug in transport_dbg
// Version 5  26-Sep-2009  - fix bug with set_end_address


// Getting Started with TLM-2.0, Tutorial Example 3

// For a full description, see http://www.doulos.com/knowhow/systemc/tlm2

// Shows a router modeled as an interconnect component between the initiator and the target
// The router decodes the address to select a target, and masks the address in the transaction
// Shows the router passing transport, DMI and debug transactions along forward and backward paths
// and doing address translation in both directions


// Define the following macro to invoke an error response from the target
// #define INJECT_ERROR

#include "top.h"

int sc_main(int argc, char* argv[])
{
    Top top("top");
    sc_start();
    return 0;
}
