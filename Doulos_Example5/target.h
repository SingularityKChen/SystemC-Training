#ifndef TARGET_H
#define TARGET_H

// *****************************************************************************************
// Target memory implements b_transport, DMI and debug
// *****************************************************************************************

#include <systemc.h>
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

struct Memory: sc_module
{
    tlm_utils::simple_target_socket<Memory> socket;

    enum { SIZE = 64 };
    const sc_time LATENCY;

    SC_CTOR(Memory)
            : socket("socket"), LATENCY(10, SC_NS)
    {
        socket.register_b_transport(       this, &Memory::b_transport);
        socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);
        socket.register_transport_dbg(     this, &Memory::transport_dbg);

        // Initialize memory with random data
        for (int i = 0; i < SIZE; i++)
            mem[i] = 0xAA000000 | (mem_nr << 20) | (rand() % 256);

        // Each instance is given identifiable contents to help debug
        ++mem_nr;

        SC_THREAD(invalidation_process);
    }

    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
    {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    adr = trans.get_address() / 4;
        unsigned char*   ptr = trans.get_data_ptr();
        unsigned int     len = trans.get_data_length();
        unsigned char*   byt = trans.get_byte_enable_ptr();
        unsigned int     wid = trans.get_streaming_width();

        if (adr > sc_dt::uint64(SIZE)) {
            trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
            return;
        }
        if (byt != 0) {
            trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE );
            return;
        }
        if (len > 4 || wid < len) {
            trans.set_response_status( tlm::TLM_BURST_ERROR_RESPONSE );
            return;
        }

        if (trans.get_command() == tlm::TLM_READ_COMMAND)
            memcpy(ptr, &mem[adr], len);
        else if (cmd == tlm::TLM_WRITE_COMMAND)
            memcpy(&mem[adr], ptr, len);

        // Use temporal decoupling: add memory latency to delay argument
        delay += LATENCY;

        trans.set_dmi_allowed(true);
        trans.set_response_status( tlm::TLM_OK_RESPONSE );
    }


    // TLM-2 DMI method
    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                                    tlm::tlm_dmi& dmi_data)
    {
        // Permit read and write access
        dmi_data.allow_read_write();

        // Set other details of DMI region
        dmi_data.set_dmi_ptr( reinterpret_cast<unsigned char*>( &mem[0] ) );
        dmi_data.set_start_address( 0 );
        dmi_data.set_end_address( SIZE*4-1 );
        dmi_data.set_read_latency( LATENCY );
        dmi_data.set_write_latency( LATENCY );

        return true;
    }


    // TLM-2 debug transaction method
    virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans)
    {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    adr = trans.get_address() / 4;
        unsigned char*   ptr = trans.get_data_ptr();
        unsigned int     len = trans.get_data_length();

        // Calculate the number of bytes to be actually copied
        unsigned int num_bytes = (len < (SIZE - adr) * 4) ? len : (SIZE - adr) * 4;

        if ( cmd == tlm::TLM_READ_COMMAND )
            memcpy(ptr, &mem[adr], num_bytes);
        else if ( cmd == tlm::TLM_WRITE_COMMAND )
            memcpy(&mem[adr], ptr, num_bytes);

        return num_bytes;
    }

    void invalidation_process()
    {
        // Invalidate DMI pointers once just as an example of routing a call back to initiators
        wait(3, SC_US);
        socket->invalidate_direct_mem_ptr(0, SIZE-1);
    }

    int mem[SIZE];
    static unsigned int mem_nr;  // Unique memory number to help debug
};

unsigned int Memory::mem_nr = 0;

#endif
