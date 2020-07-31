#ifndef TOP_H
#define TOP_H

#include "initiator1.h"
#include "initiator2.h"
#include "bus.h"
#include "target.h"

// *****************************************************************************************
// Top-level module instantiates 2 initiators, a bus, and 4 memories
// *****************************************************************************************

SC_MODULE(Top)
{
    Initiator1* init1;
    Initiator2* init2;
    Bus<2,4>*   bus;
    Memory*     memory[4];

    SC_CTOR(Top)
    {
        init1 = new Initiator1("init1");
        init2 = new Initiator2("init2");
        bus   = new Bus<2,4>  ("bus");

        init1->socket.bind( *(bus->targ_socket[0]) );
        init2->socket.bind( *(bus->targ_socket[1]) );

        for (int i = 0; i < 4; i++)
        {
            char txt[20];
            sprintf(txt, "memory_%d", i);
            memory[i] = new Memory(txt);

            ( *(bus->init_socket[i]) ).bind( memory[i]->socket );
        }
    }
};

#endif
