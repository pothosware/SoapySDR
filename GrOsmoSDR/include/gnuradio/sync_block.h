#pragma once
#include <osmosdr/api.h>
#include <gnuradio/basic_block.h>
#include <gnuradio/io_signature.h>

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
        
    }
    

    void consume_each(int nitems)
    {
        
    }

    void consume(int which_input, int nitems)
    {
        
    }

    int general_work(int noutput_items,
                     gr_vector_int &ninput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items)
    {
        
    }
};
}
