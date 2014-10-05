/* -*- c++ -*- */
/*
 * Copyright 2012 Dimitri Stolnikov <horiz0n@gmx.net>
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
#ifndef UHD_SOURCE_C_H
#define UHD_SOURCE_C_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/uhd/usrp_source.h>

#include "source_iface.h"

class uhd_source_c;

typedef boost::shared_ptr< uhd_source_c > uhd_source_c_sptr;

uhd_source_c_sptr make_uhd_source_c(const std::string &args = "");

class uhd_source_c :
    public gr::hier_block2,
    public source_iface
{
private:
  friend uhd_source_c_sptr make_uhd_source_c(const std::string &args);

  uhd_source_c(const std::string &args);

public:
  ~uhd_source_c();

  static std::vector< std::string > get_devices();

  std::string name();

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
  double set_gain( double gain, size_t chan = 0 );
  double set_gain( double gain, const std::string & name, size_t chan = 0 );
  double get_gain( size_t chan = 0 );
  double get_gain( const std::string & name, size_t chan = 0 );

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
  double _center_freq;
  double _freq_corr;
  double _lo_offset;
  gr::uhd::usrp_source::sptr _src;
};

#endif // UHD_SOURCE_C_H
