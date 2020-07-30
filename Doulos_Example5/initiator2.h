#ifndef INITIATOR2_H
#define INITIATOR2_H

#include "utilities.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/tlm_quantumkeeper.h"

// *****************************************************************************************
// Initiator2 reads from all 4 memories, but does not use DMI or debug transport
// Uses an explicit memory manager and transaction pool
// *****************************************************************************************

struct Initiator2: sc_module
{
    tlm_utils::simple_initiator_socket<Initiator2> socket;

    SC_CTOR(Initiator2) : socket("socket")
    {
        // No callback methods registered with socket

        SC_THREAD(thread_process);
    }

    void thread_process()
    {
        tlm::tlm_generic_payload* trans;
        sc_time delay;

        // Reset the local quantum keeper
        m_qk.reset();
        wait(1, SC_US);

        for (int i = 0; i < RUN_LENGTH; i += 4)
        {
            // Grab a new transaction from the memory manager
            trans = m_mm.allocate();
            trans->acquire();

            data = i;

            trans->set_command( tlm::TLM_READ_COMMAND );
            trans->set_address( i );
            trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
            trans->set_data_length( 4 );
            trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
            trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
            trans->set_dmi_allowed( false ); // Mandatory initial value
            trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

            delay = m_qk.get_local_time();

            socket->b_transport( *trans, delay );

            // Initiator obliged to check response status
            if (trans->is_response_error())
                SC_REPORT_ERROR("TLM-2", trans->get_response_string().c_str());
            if (data != i)
                SC_REPORT_ERROR("TLM-2", "Mismatch in initiator when reading back data");

            cout << "READ     addr = " << hex << i << ", data = " << data
                 << " at " << sc_time_stamp() << " delay = " << delay << "\n";
            trans->release();

            // Accumulate local time and synchronize when quantum is reached
            m_qk.set( delay );
            m_qk.inc( sc_time(100, SC_NS) );// Model time used for additional processing
            if (m_qk.need_sync()) m_qk.sync();
        }
    }

    int data; // Internal data buffer used by initiator with generic payload

    mm   m_mm;                         // Memory manager
    tlm_utils::tlm_quantumkeeper m_qk; // Quantum keeper for temporal decoupling
};

#endif

