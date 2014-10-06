#pragma once
#include <osmosdr/api.h>
#include <gnuradio/basic_block.h>

#define WORK_DONE -1

namespace gr
{
struct OSMOSDR_API sync_block
{
    sync_block(void){}
    sync_block(const std::string &name,
           gr::io_signature::sptr input_signature,
           gr::io_signature::sptr output_signature)
    {
        return;
    }

    virtual ~sync_block(void);

    //! may only be called during constructor
    void set_input_signature(gr::io_signature::sptr) {
    }

    //! may only be called during constructor
    void set_output_signature(gr::io_signature::sptr) {
    }

    void consume_each(int nitems)
    {
        //only care about one channel -- homogenous assumption
        _consumed_total += nitems;
    }

    void consume(int which_input, int nitems)
    {
        //only care about one channel -- homogenous assumption
        if (which_input == 0) _consumed_total += nitems;
    }

    virtual int work( int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items ) = 0;

    virtual bool start(void){return true;}
    virtual bool stop(void){return true;}

    int _consumed_total;
};
}
