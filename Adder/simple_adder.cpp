/** This file is copied from the slides.
 * */
#include <systemc.h>
#include <iostream>
template <class T> SC_MODULE(DF_Adder){
    sc_fifo_in<T> input1, input2;
    sc_fifo_out<T> output;
    void process(){
        while(1)
            output.write(input1.read()+input2.read());
    }

    SC_CTOR(DF_Adder)
    {
        SC_THREAD(process);
    }
};

template <class T> SC_MODULE(DF_Const) {
    sc_fifo_out<T> output;
    void process() {
        while (1) output.write(constant_);
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
        } }
    SC_CTOR(DF_Fork) {
        SC_THREAD(process)
    }
};

template <class T> SC_MODULE(DF_Printer){
    sc_fifo_in<T> input;
    SC_HAS_PROCESS(DF_Printer);
    DF_Printer(const sc_module_name& NAME, unsigned N_ITER):
    sc_module(NAME), n_iterations_(N_ITER), done_(false)
    {SC_THREAD(print_process)}
    void print_process(){
        for(unsigned i=0;i<n_iterations_;i++){
            T value = input.read();
            cout<<name()<<""<<value
            <<endl;
        }
        done_=true;
   }
    ~DF_Printer() override{
        if(!done_) cout<<name()<<"not done yet"
        <<endl;
    }
    unsigned n_iterations_;
    bool done_;
};

int sc_main(int, char**) {
    DF_Const<int> constant("constant",3);
    DF_Adder<int> adder("adder");
    DF_Fork<int> fork("fork");
    DF_Printer<int> printer("printer",10);
    //fifo
    sc_fifo<int> const_out("const_out",1);
    sc_fifo<int> adder_out("adder_out",1);
    sc_fifo<int> feedback("feedback",1);
    sc_fifo<int> to_printer("tc_printer",1);
    feedback.write(42);
    constant.output(const_out);
    adder.input1(feedback);
    adder.input2(const_out);
    adder.output(adder_out);
    fork.input(adder_out);
    fork.output1(feedback);
    fork.output2(to_printer);
    printer.input(to_printer);
    //sc_start(-1);
    sc_start();
    return 0;
}