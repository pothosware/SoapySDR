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
#ifndef FCD_SOURCE_C_H
#define FCD_SOURCE_C_H

#include <gnuradio/hier_block2.h>

#ifdef HAVE_FCD
#include <gnuradio/fcd/source_c.h>
#endif

#ifdef HAVE_FCDPP
#include <fcdproplus/fcdproplus.h>
#endif

#include "source_iface.h"

class fcd_source_c;

typedef boost::shared_ptr< fcd_source_c > fcd_source_c_sptr;

fcd_source_c_sptr make_fcd_source_c( const std::string & args = "" );

class fcd_source_c :
    public gr::hier_block2,
    public source_iface
{
private:
  friend fcd_source_c_sptr make_fcd_source_c(const std::string &args);

  fcd_source_c(const std::string &args);

public:
  ~fcd_source_c();

  enum dongle_type {
    FUNCUBE_UNKNOWN,
    FUNCUBE_V1,
    FUNCUBE_V2
  };

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

private:
  dongle_type _type;
#ifdef HAVE_FCD
  gr::fcd::source_c::sptr _src_v1;
#endif
#ifdef HAVE_FCDPP
  gr::fcdproplus::fcdproplus::sptr _src_v2;
#endif
  double _lna_gain, _mix_gain, _bb_gain, _freq;
  int _correct;
};

#endif // FCD_SOURCE_C_H
