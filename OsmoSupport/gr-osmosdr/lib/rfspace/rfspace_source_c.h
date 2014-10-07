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
#ifndef INCLUDED_RFSPACE_SOURCE_C_H
#define INCLUDED_RFSPACE_SOURCE_C_H

//#define USE_ASIO

#ifdef USE_ASIO
#include <boost/asio.hpp>
#endif
#include <gnuradio/thread/thread.h>
#include <gnuradio/block.h>
#include <gnuradio/sync_block.h>

#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "osmosdr/ranges.h"
#include "source_iface.h"
#ifdef USE_ASIO
using boost::asio::ip::tcp;
using boost::asio::ip::udp;
#endif
class rfspace_source_c;

#ifndef SOCKET
#define SOCKET int
#endif

/*
 * We use boost::shared_ptr's instead of raw pointers for all access
 * to gr_blocks (and many other data structures).  The shared_ptr gets
 * us transparent reference counting, which greatly simplifies storage
 * management issues.  This is especially helpful in our hybrid
 * C++ / Python system.
 *
 * See http://www.boost.org/libs/smart_ptr/smart_ptr.htm
 *
 * As a convention, the _sptr suffix indicates a boost::shared_ptr
 */
typedef boost::shared_ptr<rfspace_source_c> rfspace_source_c_sptr;

/*!
 * \brief Return a shared_ptr to a new instance of rfspace_source_c.
 *
 * To avoid accidental use of raw pointers, rfspace_source_c's
 * constructor is private.  rfspace_make_source_c is the public
 * interface for creating new instances.
 */
rfspace_source_c_sptr make_rfspace_source_c (const std::string & args = "");

class rfspace_source_c :
    public gr::sync_block,
    public source_iface
{
private:
  // The friend declaration allows rfspace_make_source_c to
  // access the private constructor.
  friend rfspace_source_c_sptr make_rfspace_source_c (const std::string & args);

  rfspace_source_c (const std::string & args);  	// private constructor

public:
  ~rfspace_source_c ();	// public destructor

  bool start();
  bool stop();

  int work(int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items );

  static std::vector< std::string > get_devices( bool fake = false );

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

  std::vector< std::string > get_antennas( size_t chan = 0 );
  std::string set_antenna( const std::string & antenna, size_t chan = 0 );
  std::string get_antenna( size_t chan = 0 );

  double set_bandwidth( double bandwidth, size_t chan = 0 );
  double get_bandwidth( size_t chan = 0 );
  osmosdr::freq_range_t get_bandwidth_range( size_t chan = 0 );

private: /* functions */
  void apply_channel( unsigned char *cmd, size_t chan = 0 );

  bool transaction( const unsigned char *cmd, size_t size );

  bool transaction( const unsigned char *cmd, size_t size,
                    std::vector< unsigned char > &response );

  void usb_read_task();

private: /* members */
  enum radio_type
  {
    RADIO_UNKNOWN = 0,
    RFSPACE_SDR_IQ,
    RFSPACE_SDR_IP,
    RFSPACE_NETSDR
  };

  radio_type _radio;

#ifdef USE_ASIO
  boost::asio::io_service _io_service;
  tcp::resolver _resolver;
  tcp::socket _t;
  udp::socket _u;
#else
  SOCKET _tcp;
  SOCKET _udp;
#endif
  int _usb;
  bool _running;
  bool _keep_running;
  uint16_t _sequence;

  size_t _nchan;
  double _sample_rate;
  double _bandwidth;

  gr::thread::thread _thread;
  bool _run_usb_read_task;

  boost::circular_buffer<gr_complex> *_fifo;
  boost::mutex _fifo_lock;
  boost::condition_variable _samp_avail;

  std::vector< unsigned char > _resp;
  boost::mutex _resp_lock;
  boost::condition_variable _resp_avail;
};

#endif /* INCLUDED_RFSPACE_SOURCE_C_H */
