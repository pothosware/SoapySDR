/* -*- c++ -*- */
/*
 * Copyright 2013 Dimitri Stolnikov <horiz0n@gmx.net>
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_HACKRF_SINK_C_H
#define INCLUDED_HACKRF_SINK_C_H

#include <gnuradio/thread/thread.h>
#include <gnuradio/sync_block.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <libhackrf/hackrf.h>

#include "sink_iface.h"

class hackrf_sink_c;

typedef struct circular_buffer
{
  void *buffer;     // data buffer
  void *buffer_end; // end of data buffer
  size_t capacity;  // maximum number of items in the buffer
  size_t count;     // number of items in the buffer
  size_t sz;        // size of each item in the buffer
  void *head;       // pointer to head
  void *tail;       // pointer to tail
} circular_buffer_t;

/*
 * We use boost::shared_ptr's instead of raw pointers for all access
 * to gr::blocks (and many other data structures).  The shared_ptr gets
 * us transparent reference counting, which greatly simplifies storage
 * management issues.  This is especially helpful in our hybrid
 * C++ / Python system.
 *
 * See http://www.boost.org/libs/smart_ptr/smart_ptr.htm
 *
 * As a convention, the _sptr suffix indicates a boost::shared_ptr
 */
typedef boost::shared_ptr<hackrf_sink_c> hackrf_sink_c_sptr;

/*!
 * \brief Return a shared_ptr to a new instance of hackrf_sink_c.
 *
 * To avoid accidental use of raw pointers, hackrf_sink_c's
 * constructor is private.  make_hackrf_sink_c is the public
 * interface for creating new instances.
 */
hackrf_sink_c_sptr make_hackrf_sink_c (const std::string & args = "");

class hackrf_sink_c :
    public gr::sync_block,
    public sink_iface
{
private:
  // The friend declaration allows hackrf_make_sink_c to
  // access the private constructor.
  friend hackrf_sink_c_sptr make_hackrf_sink_c (const std::string & args);

  hackrf_sink_c (const std::string & args);  	// private constructor

public:
  ~hackrf_sink_c (); 	// public destructor

  bool start();
  bool stop();

  int work( int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items );

  static std::vector< std::string > get_devices();

  size_t get_num_channels( void );

  osmosdr::meta_range_t get_sample_rates( void );
  double set_sample_rate( double rate );
  double get_sample_rate( void );

  osmosdr::freq_range_t get_freq_range( size_t chan = 0 );
  double set_center_freq( double freq, size_t chan = 0 );
  double get_center_freq( size_t chan = 0 );
  double set_freq_corr( double ppm, size_t chan = 0 );
  double get_freq_corr( size_t chan = 0 );

  std::vector<std::string> get_gain_names( size_t chan = 0 );
  osmosdr::gain_range_t get_gain_range( size_t chan = 0 );
  osmosdr::gain_range_t get_gain_range( const std::string & name, size_t chan = 0 );
  bool set_gain_mode( bool automatic, size_t chan = 0 );
  bool get_gain_mode( size_t chan = 0 );
  double set_gain( double gain, size_t chan = 0 );
  double set_gain( double gain, const std::string & name, size_t chan = 0 );
  double get_gain( size_t chan = 0 );
  double get_gain( const std::string & name, size_t chan = 0 );

  double set_if_gain( double gain, size_t chan = 0 );
  double set_bb_gain( double gain, size_t chan = 0 );

  std::vector< std::string > get_antennas( size_t chan = 0 );
  std::string set_antenna( const std::string & antenna, size_t chan = 0 );
  std::string get_antenna( size_t chan = 0 );

  double set_bandwidth( double bandwidth, size_t chan = 0 );
  double get_bandwidth( size_t chan = 0 );
  osmosdr::freq_range_t get_bandwidth_range( size_t chan = 0 );

private:
  static int _hackrf_tx_callback(hackrf_transfer* transfer);
  int hackrf_tx_callback(unsigned char *buffer, uint32_t length);
  static void _hackrf_wait(hackrf_sink_c *obj);
  void hackrf_wait();

  static int _usage;
  static boost::mutex _usage_mutex;

  hackrf_device *_dev;
//  gr::thread::thread _thread;

  circular_buffer_t _cbuf;
  char *_buf;
  unsigned int _buf_num;
  unsigned int _buf_used;
  boost::mutex _buf_mutex;
  boost::condition_variable _buf_cond;

  double _sample_rate;
  double _center_freq;
  double _freq_corr;
  bool _auto_gain;
  double _amp_gain;
  double _vga_gain;
  double _bandwidth;
};

#endif /* INCLUDED_HACKRF_SINK_C_H */
