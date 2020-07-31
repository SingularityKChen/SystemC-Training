#ifndef INITIATOR_H
#define INITIATOR_H

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"


// Initiator module generating generic payload transactions

struct Initiator: sc_module
{
    // TLM-2 socket, defaults to 32-bits wide, base protocol
    tlm_utils::simple_initiator_socket<Initiator> socket;

    SC_CTOR(Initiator)
            : socket("socket"),  // Construct and name socket
              dmi_ptr_valid(false)
    {
        // Register callbacks for incoming interface method calls
        socket.register_invalidate_direct_mem_ptr(this, &Initiator::invalidate_direct_mem_ptr);

        SC_THREAD(thread_process);
    }

    void thread_process()
    {
        // TLM-2 generic payload transaction, reused across calls to b_transport, DMI and debug
        tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
        sc_time delay = sc_time(10, SC_NS);

        // Generate a random sequence of reads and writes
        for (int i = 256-64; i < 256+64; i += 4)
        {
            int data;
            tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2);
            if (cmd == tlm::TLM_WRITE_COMMAND) data = 0xFF000000 | i;

            // Use DMI if it is available
            if (dmi_ptr_valid && sc_dt::uint64(i) >= dmi_data.get_start_address()
                && sc_dt::uint64(i) <= dmi_data.get_end_address())
            {
                // Bypass transport interface and use direct memory interface
                // Implement target latency
                if ( cmd == tlm::TLM_READ_COMMAND )
                {
                    assert( dmi_data.is_read_allowed() );
                    memcpy(&data, dmi_data.get_dmi_ptr() + i - dmi_data.get_start_address(), 4);
                    wait( dmi_data.get_read_latency() );
                }
                else if ( cmd == tlm::TLM_WRITE_COMMAND )
                {
                    assert( dmi_data.is_write_allowed() );
                    memcpy(dmi_data.get_dmi_ptr() + i - dmi_data.get_start_address(), &data, 4);
                    wait( dmi_data.get_write_latency() );
                }

                cout << "DMI   = { " << (cmd ? 'W' : 'R') << ", " << hex << i
                     << " } , data = " << hex << data << " at time " << sc_time_stamp() << endl;
            }
            else
            {
                trans->set_command( cmd );
                trans->set_address( i );
                trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
                trans->set_data_length( 4 );
                trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
                trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
                trans->set_dmi_allowed( false ); // Mandatory initial value
                trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value


#ifdef INJECT_ERROR
                if (i > 90) trans->set_streaming_width(2);
#endif

                // Other fields default: byte enable = 0, streaming width = 0, DMI_hint = false, no extensions

                socket->b_transport( *trans, delay );  // Blocking transport call

                // Initiator obliged to check response status
                if ( trans->is_response_error() )
                {
                    // Print response string
                    char txt[100];
                    sprintf(txt, "Error from b_transport, response status = %s",
                            trans->get_response_string().c_str());
                    SC_REPORT_ERROR("TLM-2", txt);

                }

                // Check DMI hint
                if ( trans->is_dmi_allowed() )
                {
                    // *********************************************
                    // Re-use transaction object for DMI. Reset the address because it could
                    // have been modified by the interconnect on the previous transport call
                    // *********************************************

                    trans->set_address( i );
                    dmi_ptr_valid = socket->get_direct_mem_ptr( *trans, dmi_data );
                }

                cout << "trans = { " << (cmd ? 'W' : 'R') << ", " << hex << i
                     << " } , data = " << hex << data << " at time " << sc_time_stamp() << endl;
            }
        }

        // Use debug transaction interface to dump memory contents, reusing same transaction object
        sc_dt::uint64 A = 128;
        trans->set_address(A);
        trans->set_read();
        trans->set_data_length(256);

        unsigned char* data = new unsigned char[256];
        trans->set_data_ptr(data);

        unsigned int n_bytes = socket->transport_dbg( *trans );

        for (unsigned int i = 0; i < n_bytes; i += 4)
        {
            cout << "mem[" << (A + i) << "] = "
                 << *(reinterpret_cast<unsigned int*>( &data[i] )) << endl;
        }

        A = 256;
        trans->set_address(A);
        trans->set_data_length(128);

        n_bytes = socket->transport_dbg( *trans );

        for (unsigned int i = 0; i < n_bytes; i += 4)
        {
            cout << "mem[" << (A + i) << "] = "
                 << *(reinterpret_cast<unsigned int*>( &data[i] )) << endl;
        }
    }

    // TLM-2 backward DMI method
    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                           sc_dt::uint64 end_range)
    {
        // Ignore range and invalidate all DMI pointers regardless
        dmi_ptr_valid = false;
    }

    bool dmi_ptr_valid;
    tlm::tlm_dmi dmi_data;
};

#endif
