#ifndef INITIATOR1_H
#define INITIATOR1_H

#include "utilities.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"


// *****************************************************************************************
// Initiator1 writes to all 4 memories, and demonstrates DMI and debug transport
// Does not use an explicit memory manager
// *****************************************************************************************

const int RUN_LENGTH = 256;

struct Initiator1: sc_module
{
    tlm_utils::simple_initiator_socket<Initiator1> socket;

    SC_CTOR(Initiator1) : socket("socket"), dmi_ptr_valid(false)
    {
        socket.register_invalidate_direct_mem_ptr(this, &Initiator1::invalidate_direct_mem_ptr);

        SC_THREAD(thread_process);

        // *************************************************************************
        // All initiators use a quantum of 1us, that is, they synchronize themselves
        // to simulation time every 1us using the quantum keeper
        // *************************************************************************

        m_qk.set_global_quantum( sc_time(1, SC_US) );
        m_qk.reset();
    }

    void thread_process() {
        // Use debug transaction interface to dump entire memory contents
        dump();

        tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
        sc_time delay;

        for (int i = 0; i < RUN_LENGTH; i += 4)
        {
            data = i;
            delay = m_qk.get_local_time();

            if (dmi_ptr_valid && sc_dt::uint64(i) >= dmi_data.get_start_address()
                && sc_dt::uint64(i) <= dmi_data.get_end_address())
            {
                // Bypass transport interface and use direct memory interface
                assert( dmi_data.is_write_allowed() );
                memcpy(dmi_data.get_dmi_ptr() + i - dmi_data.get_start_address(), &data, 4);

                // Accumulate memory latency into local time
                delay += dmi_data.get_write_latency();

                cout << "WRITE/DMI addr = " << hex << i << ", data = " << data
                     << " at " << sc_time_stamp() << " delay = " << delay << "\n";
            }
            else
            {
                // No DMI, so use blocking transport interface
                trans->set_command( tlm::TLM_WRITE_COMMAND );
                trans->set_address( i );
                trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
                trans->set_data_length( 4 );
                trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
                trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
                trans->set_dmi_allowed( false ); // Mandatory initial value
                trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

                socket->b_transport( *trans, delay );

                cout << "WRITE     addr = " << hex << i << ", data = " << data
                     << " at " << sc_time_stamp() << " delay = " << delay << "\n";

                // Initiator obliged to check response status
                if (trans->is_response_error())
                    SC_REPORT_ERROR("TLM-2", trans->get_response_string().c_str());

                if ( trans->is_dmi_allowed() )
                {
                    dmi_data.init();
                    dmi_ptr_valid = socket->get_direct_mem_ptr( *trans, dmi_data );
                }
            }

            // Accumulate local time and synchronize when quantum is reached
            m_qk.set( delay );
            m_qk.inc( sc_time(100, SC_NS) ); // Model time used for additional processing
            if (m_qk.need_sync()) m_qk.sync();
        }

        // Use debug transaction interface to dump entire memory contents
        dump();
    }


    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
    {
        cout << "INVALIDATE DMI (" << start_range << ".." << end_range
             << ") for Initiator1 at " << sc_time_stamp() << "\n";

        // Ignore range and invalidate all DMI pointers regardless
        dmi_ptr_valid = false;
    }

    void dump()
    {
        unsigned char buffer[64];
        // Use debug transaction interface to dump memory contents

        cout << "\nDump memories at time " << sc_time_stamp() << "\n";

        for (unsigned int k = 0; k < 4; k++)
        {
            tlm::tlm_generic_payload dbg;
            sc_dt::uint64 A = 64 * k;
            dbg.set_address(A);
            dbg.set_read();
            dbg.set_data_length(64);
            dbg.set_data_ptr(buffer);

            unsigned int n_bytes = socket->transport_dbg( dbg );

            for (unsigned int i = 0; i < n_bytes; i += 4)
            {
                cout << "mem[" << hex << (A + i) << "] = "
                     << *(reinterpret_cast<unsigned int*>( &buffer[i] )) << endl;
            }
        }
        cout << "\n";
    }

    int data; // Internal data buffer used by initiator with generic payload
    tlm_utils::tlm_quantumkeeper m_qk; // Quantum keeper for temporal decoupling
    bool dmi_ptr_valid;
    tlm::tlm_dmi dmi_data;
};

#endif
