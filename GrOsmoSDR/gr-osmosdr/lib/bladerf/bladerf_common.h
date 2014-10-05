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
#ifndef INCLUDED_BLADERF_COMMON_H
#define INCLUDED_BLADERF_COMMON_H

#include <vector>
#include <string>

#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <gnuradio/thread/thread.h>
#include <gnuradio/gr_complex.h>

#include <libbladeRF.h>

#include "osmosdr/ranges.h"
#include "arg_helpers.h"

typedef boost::shared_ptr<struct bladerf> bladerf_sptr;

class bladerf_common
{
public:
  bladerf_common();
  virtual ~bladerf_common();

protected:
  /* Handle initialized and parameters common to both source & sink */
  void init(dict_t &dict, bladerf_module module);

  bool start(bladerf_module module);
  bool stop(bladerf_module module);

  double set_sample_rate(bladerf_module module, double rate);
  double get_sample_rate(bladerf_module module);

  int set_dc_offset(bladerf_module module, const std::complex<double> &offset, size_t chan);
  int set_iq_balance(bladerf_module module, const std::complex<double> &balance, size_t chan);

  osmosdr::freq_range_t freq_range();
  osmosdr::meta_range_t sample_rates();
  osmosdr::freq_range_t filter_bandwidths();

  static std::vector< std::string > devices();

  bladerf_sptr _dev;

  size_t _num_buffers;
  size_t _samples_per_buffer;
  size_t _num_transfers;
  unsigned int _stream_timeout_ms;

  int16_t *_conv_buf;
  int _conv_buf_size; /* In units of samples */

  osmosdr::gain_range_t _vga1_range;
  osmosdr::gain_range_t _vga2_range;

  std::string _pfx;

  bool _xb_200_attached;

  /* BladeRF IQ correction parameters */
  static const int16_t DCOFF_SCALE  = 2048;
  static const int16_t GAIN_SCALE   = 4096;
  static const int16_t PHASE_SCALE  = 4096;

private:
  bladerf_sptr open(const std::string &device_name);
  static void close(void *dev); /* called by shared_ptr */
  static bladerf_sptr get_cached_device(struct bladerf_devinfo devinfo);

  void set_verbosity(const std::string &verbosity);
  void set_loopback_mode(const std::string &loopback);

  static boost::mutex _devs_mutex;
  static std::list<boost::weak_ptr<struct bladerf> > _devs;
};

#endif
