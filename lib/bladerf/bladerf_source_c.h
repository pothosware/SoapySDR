/* -*- c++ -*- */
/*
 * Copyright 2013 Nuand LLC
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
#ifndef INCLUDED_BLADERF_SOURCE_C_H
#define INCLUDED_BLADERF_SOURCE_C_H

#include <gnuradio/thread/thread.h>
#include <gnuradio/block.h>
#include <gnuradio/sync_block.h>

#include "osmosdr/ranges.h"
#include "source_iface.h"
#include "bladerf_common.h"

class bladerf_source_c;

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
typedef boost::shared_ptr<bladerf_source_c> bladerf_source_c_sptr;

/*!
 * \brief Return a shared_ptr to a new instance of bladerf_source_c.
 *
 * To avoid accidental use of raw pointers, bladerf_source_c's
 * constructor is private.  bladerf_make_source_c is the public
 * interface for creating new instances.
 */
bladerf_source_c_sptr make_bladerf_source_c (const std::string & args = "");

class bladerf_source_c :
    public gr::sync_block,
    public source_iface,
    protected bladerf_common
{
private:
  // The friend declaration allows bladerf_make_source_c to
  // access the private constructor.
  friend bladerf_source_c_sptr make_bladerf_source_c (const std::string & args);

  bladerf_source_c (const std::string & args);  	// private constructor

public:
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

  double set_bb_gain( double gain, size_t chan = 0 );

  std::vector< std::string > get_antennas( size_t chan = 0 );
  std::string set_antenna( const std::string & antenna, size_t chan = 0 );
  std::string get_antenna( size_t chan = 0 );

  void set_dc_offset_mode( int mode, size_t chan = 0 );
  void set_dc_offset( const std::complex<double> &offset, size_t chan = 0 );

  void set_iq_balance_mode( int mode, size_t chan = 0 );
  void set_iq_balance( const std::complex<double> &balance, size_t chan = 0 );

  double set_bandwidth( double bandwidth, size_t chan = 0 );
  double get_bandwidth( size_t chan = 0 );
  osmosdr::freq_range_t get_bandwidth_range( size_t chan = 0 );

private:
  osmosdr::gain_range_t _lna_range;
};

#endif /* INCLUDED_BLADERF_SOURCE_C_H */
