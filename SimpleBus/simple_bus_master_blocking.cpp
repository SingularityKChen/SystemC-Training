/** This is a simple bus master block
 * */

#include <iostream>
#include <systemc.h>

enum class simple_bus_status {
    SIMPLE_BUS_REQUEST,
    SIMPLE_BUS_WAIT,
    SIMPLE_BUS_OK,
    SIMPLE_BUS_ERROR
};

class simple_bus_blocking_if : public virtual sc_interface
{
public: // blocking BUS interface
    virtual simple_bus_status burst_read(unsigned int unique_priority
            , int *data
            , unsigned int start_address
            , unsigned int length = 1
            , bool lock = false) = 0;

    virtual simple_bus_status burst_write(unsigned int unique_priority
            , int *data
            , unsigned int start_address
            , unsigned int length = 1
            , bool lock = false) = 0;
}; // end class simple_bus_blocking_if


SC_MODULE(simple_bus_master_blocking) {
    // ports
    sc_in_clk clock;
    sc_port<simple_bus_blocking_if> bus_port;

    SC_HAS_PROCESS(simple_bus_master_blocking);

    // constructor
    simple_bus_master_blocking(sc_module_name name_
    , unsigned int unique_priority
    , unsigned int address
    , bool lock
    , int timeout)
    : sc_module(name_)
    , m_unique_priority(unique_priority)
    , m_address(address)
    , m_lock(lock)
    , m_timeout(timeout)
    {
        // process declaration
        SC_THREAD(main_action);
        sensitive_pos << clock;
    }

    // process
    void main_action();

private:
    unsigned int m_unique_priority;
    unsigned int m_address;
    bool m_lock;
    int m_timeout;
}; // end class simple_bus_master_blocking

void simple_bus_master_blocking::main_action() {
    // storage capacity/burst length in words
    const unsigned int myLength = 0x10;
    int myData[myLength];
    unsigned int i;
    simple_bus_status status;
    while (true) {
        wait(); // for the next rising clock edge
         status = bus_port->burst_read(m_unique_priority, myData, m_address, myLength, m_lock);
        /** wait for [[myLength]] cycles*/
        for (i = 0; i < myLength; ++i) {
            myData[i] += i;
            wait();
        }
        status = bus_port->burst_write(m_unique_priority, myData, m_address, myLength, m_lock);
        wait(m_timeout, SC_NS);
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
