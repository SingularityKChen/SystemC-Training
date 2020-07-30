#ifndef BUS_H
#define BUS_H

#include "utilities.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

// ************************************************************************************
// Bus model supports multiple initiators and multiple targets
// Supports b_ and nb_ transport interfaces, although only b_transport is actually used
// It does no arbitration, but routes all transactions from initiators without blocking
// It uses a simple built-in routing algorithm
// ************************************************************************************

template<unsigned int N_INITIATORS, unsigned int N_TARGETS>
struct Bus: sc_module
{
    // Tagged sockets allow incoming transactions to be identified
    tlm_utils::simple_target_socket_tagged<Bus>*    targ_socket[N_INITIATORS];
    tlm_utils::simple_initiator_socket_tagged<Bus>* init_socket[N_TARGETS];

    SC_CTOR(Bus)
    {
        for (unsigned int i = 0; i < N_INITIATORS; i++)
        {
            char txt[20];
            sprintf(txt, "targ_socket_%d", i);
            targ_socket[i] = new tlm_utils::simple_target_socket_tagged<Bus>(txt);

            targ_socket[i]->register_nb_transport_fw(   this, &Bus::nb_transport_fw, i);
            targ_socket[i]->register_b_transport(       this, &Bus::b_transport, i);
            targ_socket[i]->register_get_direct_mem_ptr(this, &Bus::get_direct_mem_ptr, i);
            targ_socket[i]->register_transport_dbg(     this, &Bus::transport_dbg, i);
        }
        for (unsigned int i = 0; i < N_TARGETS; i++)
        {
            char txt[20];
            sprintf(txt, "init_socket_%d", i);
            init_socket[i] = new tlm_utils::simple_initiator_socket_tagged<Bus>(txt);

            init_socket[i]->register_nb_transport_bw(          this, &Bus::nb_transport_bw, i);
            init_socket[i]->register_invalidate_direct_mem_ptr(this, &Bus::invalidate_direct_mem_ptr, i);
        }
    }


    // Tagged non-blocking transport forward method
    virtual tlm::tlm_sync_enum nb_transport_fw(int id,
                                               tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
    {
        if (id < N_INITIATORS)
        {
            // Forward path
            m_id_map[ &trans ] = id;

            sc_dt::uint64 address = trans.get_address();
            sc_dt::uint64 masked_address;
            unsigned int target_nr = decode_address( address, masked_address);

            if (target_nr < N_TARGETS)
            {
                // Modify address within transaction
                trans.set_address( masked_address );

                // Forward transaction to appropriate target
                tlm::tlm_sync_enum status = (*init_socket[target_nr])->nb_transport_fw(trans, phase, delay);

                if (status == tlm::TLM_COMPLETED)
                    // Put back original address
                    trans.set_address( address );
                return status;
            }
            else
                return tlm::TLM_COMPLETED;
        }
        else
        {
            SC_REPORT_FATAL("TLM-2", "Invalid tagged socket id in bus");
            return tlm::TLM_COMPLETED;
        }
    }

    // Tagged non-blocking transport backward method
    virtual tlm::tlm_sync_enum nb_transport_bw(int id,
                                               tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
    {
        if (id < N_TARGETS)
        {
            // Backward path

            // Replace original address
            sc_dt::uint64 address = trans.get_address();
            trans.set_address( compose_address( id, address ) );

            return ( *(targ_socket[ m_id_map[ &trans ] ]) )->nb_transport_bw(trans, phase, delay);
        }
        else
        {
            SC_REPORT_FATAL("TLM-2", "Invalid tagged socket id in bus");
            return tlm::TLM_COMPLETED;
        }
    }

    // Tagged TLM-2 blocking transport method
    virtual void b_transport( int id, tlm::tlm_generic_payload& trans, sc_time& delay )
    {
        if (id < N_INITIATORS)
        {
            // Forward path
            sc_dt::uint64 address = trans.get_address();
            sc_dt::uint64 masked_address;
            unsigned int target_nr = decode_address( address, masked_address);

            if (target_nr < N_TARGETS)
            {
                // Modify address within transaction
                trans.set_address( masked_address );

                // Forward transaction to appropriate target
                (*init_socket[target_nr])->b_transport(trans, delay);

                // Replace original address
                trans.set_address( address );
            }
        }
        else
            SC_REPORT_FATAL("TLM-2", "Invalid tagged socket id in bus");
    }

    // Tagged TLM-2 forward DMI method
    virtual bool get_direct_mem_ptr(int id,
                                    tlm::tlm_generic_payload& trans,
                                    tlm::tlm_dmi&  dmi_data)
    {
        sc_dt::uint64 masked_address;
        unsigned int target_nr = decode_address( trans.get_address(), masked_address );
        if (target_nr >= N_TARGETS)
            return false;

        trans.set_address( masked_address );

        bool status = ( *init_socket[target_nr] )->get_direct_mem_ptr( trans, dmi_data );

        // Calculate DMI address of target in system address space
        dmi_data.set_start_address( compose_address( target_nr, dmi_data.get_start_address() ));
        dmi_data.set_end_address  ( compose_address( target_nr, dmi_data.get_end_address() ));

        return status;
    }


    // Tagged debug transaction method
    virtual unsigned int transport_dbg(int id, tlm::tlm_generic_payload& trans)
    {
        sc_dt::uint64 masked_address;
        unsigned int target_nr = decode_address( trans.get_address(), masked_address );
        if (target_nr >= N_TARGETS)
            return 0;
        trans.set_address( masked_address );

        // Forward debug transaction to appropriate target
        return ( *init_socket[target_nr] )->transport_dbg( trans );
    }


    // Tagged backward DMI method
    virtual void invalidate_direct_mem_ptr(int id,
                                           sc_dt::uint64 start_range,
                                           sc_dt::uint64 end_range)
    {
        // Reconstruct address range in system memory map
        sc_dt::uint64 bw_start_range = compose_address( id, start_range );
        sc_dt::uint64 bw_end_range   = compose_address( id, end_range );

        // Propagate call backward to all initiators
        for (unsigned int i = 0; i < N_INITIATORS; i++)
            (*targ_socket[i])->invalidate_direct_mem_ptr(bw_start_range, bw_end_range);
    }

    // Simple fixed address decoding
    inline unsigned int decode_address( sc_dt::uint64 address, sc_dt::uint64& masked_address )
    {
        unsigned int target_nr = static_cast<unsigned int>( (address >> 6) & 0x3 );
        masked_address = address & 0x3F;
        return target_nr;
    }

    inline sc_dt::uint64 compose_address( unsigned int target_nr, sc_dt::uint64 address)
    {
        return (target_nr << 6) | (address & 0x3F);
    }

    std::map <tlm::tlm_generic_payload*, unsigned int> m_id_map;
};

#endif

