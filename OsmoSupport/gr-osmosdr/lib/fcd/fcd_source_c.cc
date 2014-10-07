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

#include <fstream>
#include <string>
#include <sstream>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>

#include <gnuradio/io_signature.h>

#include "fcd_source_c.h"

#include "arg_helpers.h"

using namespace boost::assign;

fcd_source_c_sptr make_fcd_source_c(const std::string &args)
{
  return gnuradio::get_initial_sptr(new fcd_source_c(args));
}

/*
 2 [V10            ]: USB-Audio - FUNcube Dongle V1.0
                      Hanlincrest Ltd. FUNcube Dongle V1.0 at usb-0000:00:1d.0-2, full speed
 */
/*
 2 [V20            ]: USB-Audio - FUNcube Dongle V2.0
                      Hanlincrest Ltd. FUNcube Dongle V2.0 at usb-0000:00:1d.0-2, full speed
 */

typedef std::pair< fcd_source_c::dongle_type, std::string > device_t;
typedef std::vector< device_t > devices_t;

static devices_t _get_devices() /* FIXME: non-portable way to discover dongles */
{
  devices_t devices;

  std::string line;
  std::ifstream cards( "/proc/asound/cards" );
  if ( cards.is_open() )
  {
    while ( cards.good() )
    {
      getline (cards, line);

      fcd_source_c::dongle_type type = fcd_source_c::FUNCUBE_UNKNOWN;

      if ( line.find( "USB-Audio - FUNcube Dongle V1.0" ) != std::string::npos )
        type = fcd_source_c::FUNCUBE_V1;

      if ( line.find( "USB-Audio - FUNcube Dongle V2.0" ) != std::string::npos )
        type = fcd_source_c::FUNCUBE_V2;

      if ( type != fcd_source_c::FUNCUBE_UNKNOWN )
      {
        int id;
        std::istringstream( line ) >> id;

        std::ostringstream hw_id;
        hw_id << "hw:" << id; /* build alsa identifier */

        devices += device_t( type, hw_id.str() );
      }
    }

    cards.close();
  }

  return devices;
}

fcd_source_c::fcd_source_c(const std::string &args) :
  gr::hier_block2("fcd_source_c",
                 gr::io_signature::make(0, 0, 0),
                 gr::io_signature::make(1, 1, sizeof (gr_complex))),
  _type( FUNCUBE_UNKNOWN )
{
  std::string dev_name;
  unsigned int dev_index = 0;

  dict_t dict = params_to_dict(args);

  if (dict.count("fcd"))
  {
    std::string value = dict["fcd"];
    if ( value.length() )
    {
      try {
        dev_index = boost::lexical_cast< unsigned int >( value );
      } catch ( std::exception &ex ) {
        throw std::runtime_error(
              "Failed to use '" + value + "' as index: " + ex.what());
      }
    }
  }

  if (dict.count("device"))
  {
    dev_name = dict["device"];
    _type = FUNCUBE_V1;
  }

  if (dict.count("type"))
  {
    _type = (dongle_type) boost::lexical_cast< int >( dict["type"] );

    if ( FUNCUBE_V1 != _type && FUNCUBE_V2 != _type )
      throw std::runtime_error("FUNcube Dongle type must be 1 or 2.");
  }

  devices_t devices = _get_devices();

  if ( devices.size() )
  {
    if ( FUNCUBE_UNKNOWN == _type )
      _type = devices[dev_index].first;

    if ( dev_name.length() == 0 )
      dev_name = devices[dev_index].second;
  }
  else if ( dev_name.length() == 0 )
    throw std::runtime_error("No FUNcube Dongle found.");

  std::cerr << "Using " << name() << " (" << dev_name << ")" << std::endl;

#ifdef HAVE_FCD
  if ( FUNCUBE_V1 == _type )
  {
    _src_v1 = gr::fcd::source_c::make( dev_name );
    connect( _src_v1, 0, self(), 0 );

    set_gain( 20, "LNA" );
    set_gain( 12, "MIX" );
  }
#endif

#ifdef HAVE_FCDPP
  if ( FUNCUBE_V2 == _type )
  {
    _src_v2 = gr::fcdproplus::fcdproplus::make( dev_name );
    connect( _src_v2, 0, self(), 0 );

    set_gain( 1, "LNA" );
    set_gain( 1, "MIX" );
    set_gain( 15, "BB" );
  }
#endif
}

fcd_source_c::~fcd_source_c()
{
}

std::vector< std::string > fcd_source_c::get_devices()
{
  int id = 0;
  std::vector< std::string > devices;

  BOOST_FOREACH( device_t dev, _get_devices() )
  {
    std::string args = "fcd=" + boost::lexical_cast< std::string >( id++ );

    if ( dev.first == fcd_source_c::FUNCUBE_V1 )
      args += ",label='FUNcube Dongle V1.0'";
    else if ( dev.first == fcd_source_c::FUNCUBE_V2 )
      args += ",label='FUNcube Dongle V2.0'";

    devices.push_back( args );
  }

  return devices;
}

std::string fcd_source_c::name()
{
  if ( FUNCUBE_V1 == _type )
    return "FUNcube Dongle V1.0";
  else if ( FUNCUBE_V2 == _type )
    return "FUNcube Dongle V2.0";

  return "";
}

size_t fcd_source_c::get_num_channels( void )
{
  return 1;
}

osmosdr::meta_range_t fcd_source_c::get_sample_rates( void )
{
  osmosdr::meta_range_t range;

  range += osmosdr::range_t( get_sample_rate() );

  return range;
}

double fcd_source_c::set_sample_rate( double rate )
{
  return get_sample_rate();
}

double fcd_source_c::get_sample_rate( void )
{
  if ( FUNCUBE_V1 == _type )
    return 96e3;
  else if ( FUNCUBE_V2 == _type )
    return 192e3;

  return 0;
}

osmosdr::freq_range_t fcd_source_c::get_freq_range( size_t chan )
{
  if ( FUNCUBE_V1 == _type )
    return osmosdr::freq_range_t( 52e6, 2.2e9 );
  else if ( FUNCUBE_V2 == _type )
    return osmosdr::freq_range_t( 150e3, 2.05e9 );

  return osmosdr::freq_range_t();
}

double fcd_source_c::set_center_freq( double freq, size_t chan )
{
#ifdef HAVE_FCD
  if ( FUNCUBE_V1 == _type )
    _src_v1->set_freq( float(freq) );
#endif

#ifdef HAVE_FCDPP
  if ( FUNCUBE_V2 == _type )
    _src_v2->set_freq( float(freq) );
#endif

  _freq = freq;

  return get_center_freq(chan);
}

double fcd_source_c::get_center_freq( size_t chan )
{
  return _freq;
}

double fcd_source_c::set_freq_corr( double ppm, size_t chan )
{
#ifdef HAVE_FCD
  if ( FUNCUBE_V1 == _type )
    _src_v1->set_freq_corr( ppm );
#endif

#ifdef HAVE_FCDPP
  if ( FUNCUBE_V2 == _type )
    _src_v2->set_freq_corr( ppm );
#endif

  _correct = ppm;

  return get_freq_corr( chan );
}

double fcd_source_c::get_freq_corr( size_t chan )
{
  return _correct;
}

std::vector<std::string> fcd_source_c::get_gain_names( size_t chan )
{
  std::vector< std::string > names;

  names += "LNA";
  names += "MIX";

  if ( FUNCUBE_V2 == _type )
    names += "BB";

  return names;
}

osmosdr::gain_range_t fcd_source_c::get_gain_range( size_t chan )
{
  std::string name = "";

  if ( FUNCUBE_V1 == _type )
    name = "LNA"; /* use LNA gain for V1 dongle */
  else if ( FUNCUBE_V2 == _type )
    name = "BB"; /* use BB gain for V2 dongle */

  return get_gain_range( name, chan );
}

osmosdr::gain_range_t fcd_source_c::get_gain_range( const std::string & name, size_t chan )
{
  if ( FUNCUBE_V1 == _type )
  {
    if ( "LNA" == name )
      return osmosdr::gain_range_t(-5, 30, 2.5);
    else if ( "MIX" == name )
      return osmosdr::gain_range_t(4, 12, 8);
  }
  else if ( FUNCUBE_V2 == _type )
  {
    if ( "LNA" == name )
      return osmosdr::gain_range_t(0, 1, 1);
    else if ( "MIX" == name )
      return osmosdr::gain_range_t(0, 1, 1);
    else if ( "BB" == name )
      return osmosdr::gain_range_t(0, 59, 1);
  }

  return osmosdr::gain_range_t();
}

double fcd_source_c::set_gain( double gain, size_t chan )
{
  if ( FUNCUBE_V1 == _type )
    _lna_gain = set_gain( gain, "LNA" );

  if ( FUNCUBE_V2 == _type )
    _bb_gain = set_gain( gain, "BB" );

  return get_gain(chan);
}

double fcd_source_c::set_gain( double gain, const std::string & name, size_t chan )
{
#ifdef HAVE_FCD
  if ( FUNCUBE_V1 == _type )
  {
    if ( "LNA" == name )
    {
      _lna_gain = gain;
      _src_v1->set_lna_gain(_lna_gain);
    }
    else if ( "MIX" == name )
    {
      _mix_gain = gain > 4 ? 12 : 4;
      _src_v1->set_mixer_gain(_mix_gain);
    }
  }
#endif

#ifdef HAVE_FCDPP
  if ( FUNCUBE_V2 == _type )
  {
    if ( "LNA" == name )
    {
      _lna_gain = gain > 0 ? 1 : 0;
      _src_v2->set_lna(_lna_gain);
    }
    else if ( "MIX" == name )
    {
      _mix_gain = gain > 0 ? 1 : 0;
      _src_v2->set_mixer_gain(_mix_gain);
    }
    else if ( "BB" == name )
    {
      _bb_gain = gain;
      _src_v2->set_if_gain(_bb_gain);
    }
  }
#endif

  return get_gain( name, chan );
}

double fcd_source_c::get_gain( size_t chan )
{
  if ( FUNCUBE_V1 == _type )
    return get_gain( "LNA", chan );
  else if ( FUNCUBE_V2 == _type )
    return get_gain( "BB", chan );

  return 0;
}

double fcd_source_c::get_gain( const std::string & name, size_t chan )
{
  if ( FUNCUBE_V1 == _type )
  {
    if ( "LNA" == name )
      return _lna_gain;
    else if ( "MIX" == name )
      return _mix_gain;
  }
  else if ( FUNCUBE_V2 == _type )
  {
    if ( "LNA" == name )
      return _lna_gain;
    else if ( "MIX" == name )
      return _mix_gain;
    else if ( "BB" == name )
      return _bb_gain;
  }

  return 0;
}

std::vector< std::string > fcd_source_c::get_antennas( size_t chan )
{
  std::vector< std::string > antennas;

  antennas += get_antenna(chan);

  return antennas;
}

std::string fcd_source_c::set_antenna( const std::string & antenna, size_t chan )
{
  return get_antenna(chan);
}

std::string fcd_source_c::get_antenna( size_t chan )
{
  return "RX";
}
