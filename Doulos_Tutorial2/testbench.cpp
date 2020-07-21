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

// Version 2  18-June-2008 - updated for TLM-2.0
// Version 3   3-July-2008 - bug fix: call dmi_data.init()
// Version 4  12-Jan-2009  - fix bug in transport_dbg
// Version 5  26-Sep-2009  - fix bug with set_end_address

// Getting Started with TLM-2.0, Tutorial Example 2

// For a full description, see http://www.doulos.com/knowhow/systemc/tlm2

// Shows the direct memory interfaces and the DMI hint.
// Shows the debug transaction interface
// Shows the proper use of response status

// Define the following macro to invoke an error response from the target
// #define INJECT_ERROR

#include "top.h"

int sc_main(int argc, char* argv[])
{
    Top top("top");
    sc_start();
    return 0;
}

