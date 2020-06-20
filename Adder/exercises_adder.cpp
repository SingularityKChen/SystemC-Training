/** This file is based on the `simple_adder.cpp`,
 * and modified for the exercises.
 * (1) Dump the execution order of the different processes
 * (2) Set the sizes of all the FIFO channels to 5,
 *      and compare the execution order of the processes with that in (1)
 * (3) Continue the experiment in (1) and modify the DF_fork module such that
 *      it produces two values on each output for every data sample consumed.
 *      Identify the step in which the simulation halts and state the reasons
 * (4) Continue the experiment in (3), break the feedback loop, remove the adder,
 *      and state whether the system will halt or not?
 *      Give the reasons behind your observations.
 * (5) Continue the experiment in (1), set the delay z^(-1) to z^(-3)
 * */

#include <systemc.h>
#include <iostream>
template <class T> SC_MODULE(DF_Adder){
    sc_fifo_in<T> input1, input2;
    sc_fifo_out<T> output;
    void process(){
        while(1) {
            T data = input1.read()+input2.read();
            wait(200, SC_NS);
            output.write(data);
            std::string logInfo = "[" + sc_time_stamp().to_string() + "]"
                    + name() + " now process";
            cout << logInfo << endl;
        }

    }

    SC_CTOR(DF_Adder)
    {
        SC_THREAD(process);
    }
};

template <class T> SC_MODULE(DF_Const) {
    sc_fifo_out<T> output;
    void process() {
        while (1) {
            output.write(constant_);
            std::string logInfo = "[" + sc_time_stamp().to_string() + "]"
                                  + name() + " now process with a constant ";
            cout << logInfo << constant_ << endl;
        };
    }
    SC_HAS_PROCESS(DF_Const);
    DF_Const(sc_module_name N, const T&C):
            sc_module(N), constant_(C)
    {SC_THREAD(process);}
    T constant_;
};

template <class T> SC_MODULE(DF_Fork){
    sc_fifo_in<T> input;
    sc_fifo_out<T> output1,output2;
    void process(){
        while(1) {
            T value=input.read();
            output1.write(value);
            output2.write(value);
            std::string logInfo = "[" + sc_time_stamp().to_string() + "]"
                                  + name() + " now process with a value ";
            cout << logInfo << value << endl;
        } }
    SC_CTOR(DF_Fork) {
        SC_THREAD(process)
    }
};

template <class T, unsigned n_iterations>
SC_MODULE(DF_Printer){
    sc_fifo_in<T> input;
    sc_out<bool> done;
    SC_CTOR(DF_Printer) {
        SC_THREAD(print_process);
        done.initialize(false);
    }
    void print_process(){
        for(unsigned i=0;i<n_iterations;i++)
        {
            T readData;
            readData = input.read();
            std::string logInfo = "[" + sc_time_stamp().to_string() + "]"
                                  + name() + " ";
            cout << logInfo << readData << endl;
        }
        done=true;
        while(1) input.read();
    }
};

SC_MODULE(Terminator) {
    sc_port<sc_signal_in_if<bool>, 0> inputs;
    SC_CTOR(Terminator) {
        SC_METHOD(arnold);
        sensitive << inputs;
    }

    void arnold() {
        for (unsigned i = 0; i < inputs.size(); i++)
            if (inputs[i]->read() == 0)
                return;
        std::string logInfo = "[" + sc_time_stamp().to_string() + "]"
                              + name() + " now stop";
        cout << logInfo << endl;
        sc_stop();
    }
};

int sc_main(int, char**) {
    int fifo_size = 5; // exercise 2
    DF_Const<int> constant("constant",3);
    DF_Adder<int> adder("adder");
    DF_Fork<int> fork("fork");
    DF_Printer<int, 10> printer("printer");
    Terminator terminator("terminator");
    //fifo
    sc_fifo<int> const_out("const_out",fifo_size);
    sc_fifo<int> adder_out("adder_out",fifo_size);
    sc_fifo<int> feedback("feedback",fifo_size);
    sc_fifo<int> to_printer("to_printer",fifo_size);
    sc_signal<bool> to_terminator;
    feedback.write(42);
    constant.output(const_out);
    adder.input1(feedback);
    adder.input2(const_out);
    adder.output(adder_out);
    fork.input(adder_out);
    fork.output1(feedback);
    fork.output2(to_printer);
    printer.input(to_printer);
    printer.done(to_terminator); //output
    terminator.inputs(to_terminator);
    //sc_start(-1);
    sc_start();
    //sc_start(200, SC_MS);
    return 0;
}

/**
 * The original output:
 * printer45
 * printer48
 * printer51
 * printer54
 * printer57
 * printer60
 * printer63
 * printer66
 * printer69
 * printer72
 *
 * The output of exercise (1):
 * [0 s]constant now process with a constant 3
 * [0 s]constant now process with a constant 3
 * [200 ns]adder now process
 * [200 ns]fork now process with a value 45
 * [200 ns]printer 45
 * [200 ns]constant now process with a constant 3
 * [400 ns]adder now process
 * [400 ns]fork now process with a value 48
 * [400 ns]printer 48
 * [400 ns]constant now process with a constant 3
 * [600 ns]adder now process
 * [600 ns]fork now process with a value 51
 * [600 ns]printer 51
 * [600 ns]constant now process with a constant 3
 * [800 ns]adder now process
 * [800 ns]fork now process with a value 54
 * [800 ns]printer 54
 * [800 ns]constant now process with a constant 3
 * [1 us]adder now process
 * [1 us]fork now process with a value 57
 * [1 us]printer 57
 * [1 us]constant now process with a constant 3
 * [1200 ns]adder now process
 * [1200 ns]fork now process with a value 60
 * [1200 ns]printer 60
 * [1200 ns]constant now process with a constant 3
 * [1400 ns]adder now process
 * [1400 ns]fork now process with a value 63
 * [1400 ns]printer 63
 * [1400 ns]constant now process with a constant 3
 * [1600 ns]adder now process
 * [1600 ns]fork now process with a value 66
 * [1600 ns]printer 66
 * [1600 ns]constant now process with a constant 3
 * [1800 ns]adder now process
 * [1800 ns]fork now process with a value 69
 * [1800 ns]printer 69
 * [1800 ns]constant now process with a constant 3
 * [2 us]adder now process
 * [2 us]fork now process with a value 72
 * [2 us]printer 72
 * [2 us]terminator now stop
 * [2 us]constant now process with a constant 3
 *
 * The output of exercise (2):
 *
 * The output of exercise (3):
 * */